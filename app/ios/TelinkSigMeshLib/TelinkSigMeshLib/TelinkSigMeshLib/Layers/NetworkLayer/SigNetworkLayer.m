/********************************************************************************************************
 * @file     SigNetworkLayer.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/9/9
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/

#import "SigNetworkLayer.h"
#import "SigNetworkManager.h"
#import "SigMeshLib.h"
#import "SigLowerTransportLayer.h"
#import "SigLowerTransportPdu.h"
#import "SigControlMessage.h"
#import "SigSegmentAcknowledgmentMessage.h"

/*
 The network layer defines how transport messages are addressed towards one or more elements.
 It defines the network message format that allows Transport PDUs to be transported by the
 bearer layer. The network layer decides whether to relay/forward messages, accept them for
 further processing, or reject them. It also defines how a network message is encrypted and
 authenticated.
 */
@interface SigNetworkLayer ()
@property (nonatomic,assign) NSInteger networkTransmitCount;
//@property (nonatomic,strong) SigNetkeyModel *proxyNetworkKey;
@property (nonatomic,strong) NSMutableArray <BackgroundTimer *>*networkTransmitTimers;
@end

@implementation SigNetworkLayer

- (NSMutableArray<BackgroundTimer *> *)networkTransmitTimers {
    if (!_networkTransmitTimers) {
        _networkTransmitTimers = [NSMutableArray array];
    }
    return _networkTransmitTimers;
}

- (instancetype)initWithNetworkManager:(SigNetworkManager *)networkManager {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _networkManager = networkManager;
        _meshNetwork = networkManager.manager.dataSource;
//        _defaults = [NSUserDefaults standardUserDefaults];
//        _networkMessageCache = [[NSCache alloc] init];
    }
    return self;
}

/// This method handles the received PDU of given type and
/// passes it to Upper Transport Layer.
///
/// - parameter pdu:  The data received.
/// - parameter type: The PDU type.
- (void)handleIncomingPdu:(NSData *)pdu ofType:(SigPduType)type {
    if (_networkManager.manager.dataSource == nil) {
        TelinkLogError(@"this networkManager has not data.");
        return;
    }
    if (type == SigPduType_provisioningPdu) {
        TelinkLogError(@"Provisioning is handled using ProvisioningManager.");
        return;
    }

    // Try decoding the PDU.
    switch (type) {
        case SigPduType_networkPdu:
            {
//                TelinkLogDebug(@"receive networkPdu");
                //两个不同netkey进行解包（fast provision需要）:先使用mesh的networkKey进行解密，再使用当前networkLayer特定networkKey和ivIndex进行解密。
                SigNetworkPdu *networkPdu = [SigNetworkPdu decodePdu:pdu pduType:SigPduType_networkPdu meshDataSource:_meshNetwork];
                if (!networkPdu && _networkKey && _ivIndex) {
                    networkPdu = [[SigNetworkPdu alloc] initWithDecodePduData:pdu pduType:SigPduType_networkPdu usingNetworkKey:_networkKey ivIndex:_ivIndex];
                }
                if (networkPdu == nil) {
                    TelinkLogDebug(@"decodePdu fail.");
                    return;
                }
                [SigMeshLib.share receiveNetworkPdu:networkPdu];
                [_networkManager.lowerTransportLayer handleNetworkPdu:networkPdu];
            }
            break;
        case SigPduType_meshBeacon:
            {
//                TelinkLogVerbose(@"receive meshBeacon");
                UInt8 tem = 0;
                Byte *pduByte = (Byte *)pdu.bytes;
                memcpy(&tem, pduByte, 1);
                SigBeaconType beaconType = tem;
                if (beaconType == SigBeaconType_secureNetwork) {
                    SigSecureNetworkBeacon *beaconPdu = [SigSecureNetworkBeacon decodePdu:pdu meshDataSource:_meshNetwork];
                    if (beaconPdu != nil) {
                        [self handleSecureNetworkBeacon:beaconPdu];
                        return;
                    }
                } else if (beaconType == SigBeaconType_unprovisionedDevice) {
                    SigUnprovisionedDeviceBeacon *unprovisionedBeacon = [[SigUnprovisionedDeviceBeacon alloc] initWithDecodePdu:pdu];
                    if (unprovisionedBeacon != nil) {
                        [self handleUnprovisionedDeviceBeacon:unprovisionedBeacon];
                        return;
                    }
                } else if (beaconType == SigBeaconType_meshPrivateBeacon) {
                    SigMeshPrivateBeacon *privateBeacon = [SigMeshPrivateBeacon decodePdu:pdu meshDataSource:_meshNetwork];
                    if (privateBeacon != nil) {
                        [self handleMeshPrivateBeacon:privateBeacon];
                        return;
                    }
                }
                TelinkLogError(@"Invalid or unsupported beacon type.");
            }
            break;
        case SigPduType_proxyConfiguration:
            {
//                TelinkLogVerbose(@"receive proxyConfiguration");
                SigNetworkPdu *proxyPdu = [SigNetworkPdu decodePdu:pdu pduType:type meshDataSource:_meshNetwork];
                if (proxyPdu == nil) {
                    TelinkLogInfo(@"Failed to decrypt proxy PDU");
                    return;
                }
//                TelinkLogVerbose(@"%@ received",proxyPdu);
                [self handleSigProxyConfigurationPdu:proxyPdu];
            }
            break;
        default:
            TelinkLogDebug(@"pdu not handle.");
            break;
    }
}

/// This method tries to send the Lower Transport Message of given type to the
/// given destination address. If the local Provisioner does not exist, or
/// does not have Unicast Address assigned, this method does nothing.
///
/// - parameter pdu:  The Lower Transport PDU to be sent.
/// - parameter type: The PDU type.
/// - parameter ttl:  The initial TTL (Time To Live) value of the message.
/// - parameter ivIndex:  The initial ivIndex value of the message.
/// - throws: This method may throw when the `transmitter` is not set, or has
///           failed to send the PDU.
- (void)sendLowerTransportPdu:(SigLowerTransportPdu *)pdu ofType:(SigPduType)type withTtl:(UInt8)ttl ivIndex:(SigIvIndex *)ivIndex {
    if ([pdu isMemberOfClass:[SigSegmentAcknowledgmentMessage class]]) {
        if (SigBearer.share.isSending) {
            self.lastNeedSendAckMessage = (SigSegmentAcknowledgmentMessage *)pdu;
            return;
        } else {
            self.lastNeedSendAckMessage = nil;
        }
    }
    _networkKey = pdu.networkKey;
    if (pdu.ivIndex == nil) {
        if (pdu.networkKey.ivIndex != nil) {
            _ivIndex = pdu.networkKey.ivIndex;
        } else {
            _ivIndex = SigMeshLib.share.dataSource.curNetkeyModel.ivIndex;
        }
        pdu.ivIndex = _ivIndex;
    } else {
        _ivIndex = pdu.ivIndex;
    }

    // Get the current sequence number for local Provisioner's source address.
    UInt32 sequence = (UInt32)[SigMeshLib.share.dataSource getSequenceNumberUInt32];
    // As the sequence number was just used, it has to be incremented.
    [SigMeshLib.share.dataSource updateSequenceNumberUInt32WhenSendMessage:sequence+1];

    TelinkLogInfo(@"pdu sourceAddress=0x%x,sequence=0x%x,ttl=%d", pdu.source, sequence, ttl);
    SigNetworkPdu *networkPdu = [[SigNetworkPdu alloc] initWithEncodeLowerTransportPdu:pdu pduType:type withSequence:sequence andTtl:ttl ivIndex:ivIndex];
    pdu.networkPdu = networkPdu;

    // Loopback interface.
    if ([self shouldLoopback:networkPdu]) {
        if ([self isLocalUnicastAddress:networkPdu.destination]) {
            // No need to send messages targeting local Unicast Addresses.
            TelinkLogVerbose(@"No need to send messages targeting local Unicast Addresses.");
            return;
        }
        [SigBearer.share sendBlePdu:networkPdu ofType:type];
    }else{
        [SigBearer.share sendBlePdu:networkPdu ofType:type];
    }

    // SDK need use networkTransmit in gatt provision.
    SigNetworktransmitModel *networkTransmit = _meshNetwork.curLocationNodeModel.networkTransmit;
    if (type == SigPduType_networkPdu && networkTransmit != nil && networkTransmit.networkTransmitCount > 1 && !SigBearer.share.isProvisioned) {
        self.networkTransmitCount = networkTransmit.networkTransmitCount;
        __block NSInteger count = networkTransmit.networkTransmitCount;
        __weak typeof(self) weakSelf = self;
        BackgroundTimer *timer = [BackgroundTimer scheduledTimerWithTimeInterval:networkTransmit.networkTransmitIntervalSteps repeats:YES block:^(BackgroundTimer * _Nonnull t) {
            [SigBearer.share sendBlePdu:networkPdu ofType:type];
            count -= 1;
            if (count == 0) {
                [weakSelf.networkTransmitTimers removeObject:t];
                if (t) {
                    [t invalidate];
                }
            }
        }];
        [self.networkTransmitTimers addObject:timer];
    }
}

/// This method tries to send the Proxy Configuration Message.
///
/// The Proxy Filter object will be informed about the success or a failure.
///
/// - parameter message: The Proxy Configuration message to be sent.
- (void)sendSigProxyConfigurationMessage:(SigProxyConfigurationMessage *)message {
//    SigNetkeyModel *networkKey = _proxyNetworkKey;
    SigNetkeyModel *networkKey = _meshNetwork.curNetkeyModel;

    // If the Provisioner does not have a Unicast Address, just use a fake one
    // to configure the Proxy Server. This allows sniffing the network without
    // an option to send messages.
    UInt16 source = _meshNetwork.curLocationNodeModel.address != 0 ? _meshNetwork.curLocationNodeModel.address : MeshAddress_maxUnicastAddress;
    SigControlMessage *pdu = [[SigControlMessage alloc] initFromProxyConfigurationMessage:message sentFromSource:source usingNetworkKey:networkKey];
    pdu.ivIndex = SigMeshLib.share.dataSource.curNetkeyModel.ivIndex;
    TelinkLogInfo(@"Sending %@%@ from: 0x%x to: 0000,ivIndex=0x%x",message,message.parameters,source,pdu.ivIndex.index);
    [self sendLowerTransportPdu:pdu ofType:SigPduType_proxyConfiguration withTtl:0 ivIndex:SigMeshLib.share.dataSource.curNetkeyModel.ivIndex];
    [_networkManager notifyAboutDeliveringMessage:(SigMeshMessage *)message fromLocalElement:SigMeshLib.share.dataSource.curLocationNodeModel.elements.firstObject toDestination:pdu.destination];
}

#pragma mark - private

- (void)handleUnprovisionedDeviceBeacon:(SigUnprovisionedDeviceBeacon *)unprovisionedDeviceBeacon {
    // TODO: Handle Unprovisioned Device Beacon.
}

- (void)handleMeshPrivateBeacon:(SigMeshPrivateBeacon *)meshPrivateBeacon {
    if (!SigMeshLib.share.dataSource.existLocationIvIndexAndLocationSequenceNumber) {
        [SigMeshLib.share.dataSource setIvIndexUInt32:meshPrivateBeacon.ivIndex];
        [SigMeshLib.share.dataSource setSequenceNumberUInt32:0];
        [SigMeshLib.share.dataSource saveCurrentIvIndex:meshPrivateBeacon.ivIndex sequenceNumber:0];
    }
    SigNetkeyModel *networkKey = meshPrivateBeacon.networkKey;
    if (meshPrivateBeacon.ivIndex < networkKey.ivIndex.index || ABS(meshPrivateBeacon.ivIndex-networkKey.ivIndex.index) > 42) {
        TelinkLogError(@"Discarding mesh private beacon (ivIndex: 0x%x, expected >= 0x%x)",(unsigned int)meshPrivateBeacon.ivIndex,(unsigned int)networkKey.ivIndex.index);
        if (SigMeshLib.share.dataSource.getSequenceNumberUInt32 >= 0xc00000) {
            SigMeshPrivateBeacon *beacon = [[SigMeshPrivateBeacon alloc] initWithKeyRefreshFlag:NO ivUpdateActive:YES ivIndex:networkKey.ivIndex.index+1 randomData:[LibTools createRandomDataWithLength:13] usingNetworkKey:networkKey];
            SigMeshLib.share.meshPrivateBeacon = beacon;
        } else {
            SigMeshPrivateBeacon *beacon = [[SigMeshPrivateBeacon alloc] initWithKeyRefreshFlag:NO ivUpdateActive:NO ivIndex:networkKey.ivIndex.index randomData:[LibTools createRandomDataWithLength:13] usingNetworkKey:networkKey];
            SigMeshLib.share.meshPrivateBeacon = beacon;
        }
        if ([_networkManager.manager.delegateForDeveloper respondsToSelector:@selector(didReceiveSigMeshPrivateBeaconMessage:)]) {
            [_networkManager.manager.delegateForDeveloper didReceiveSigMeshPrivateBeaconMessage:meshPrivateBeacon];
        }
        return;
    }
    SigMeshLib.share.meshPrivateBeacon = meshPrivateBeacon;
    SigIvIndex *ivIndex = [[SigIvIndex alloc] initWithIndex:meshPrivateBeacon.ivIndex updateActive:meshPrivateBeacon.ivUpdateActive];
    networkKey.ivIndex = ivIndex;
    TelinkLogVerbose(@"receive mesh private Beacon, ivIndex=0x%x,updateActive=%d",ivIndex.index,ivIndex.updateActive);

    // If the Key Refresh Procedure is in progress, and the new Network Key
    // has already been set, the key erfresh flag indicates switching to phase 2.
    if (networkKey.phase == distributingKeys && meshPrivateBeacon.keyRefreshFlag) {
        networkKey.phase = finalizing;
    }
    // If the Key Refresh Procedure is in phase 2, and the key refresh flag is
    // set to false.
    if (networkKey.phase == finalizing && !meshPrivateBeacon.keyRefreshFlag) {
        networkKey.oldKey = nil;//This will set the phase to .normalOperation.
    }

    if (meshPrivateBeacon.ivIndex > SigMeshLib.share.dataSource.curNetkeyModel.ivIndex.index) {
        if (meshPrivateBeacon.ivUpdateActive) {
            if (SigMeshLib.share.dataSource.curNetkeyModel.ivIndex.index != meshPrivateBeacon.ivIndex - 1) {
                SigMeshLib.share.dataSource.curNetkeyModel.ivIndex.updateActive = NO;
                SigMeshLib.share.dataSource.curNetkeyModel.ivIndex.index = meshPrivateBeacon.ivIndex - 1;
                [SigMeshLib.share.dataSource updateIvIndexUInt32FromBeacon:meshPrivateBeacon.ivIndex - 1];
            }
        } else {
            SigMeshLib.share.dataSource.curNetkeyModel.ivIndex.updateActive = meshPrivateBeacon.ivUpdateActive;
            SigMeshLib.share.dataSource.curNetkeyModel.ivIndex.index = meshPrivateBeacon.ivIndex;
            [SigMeshLib.share.dataSource updateIvIndexUInt32FromBeacon:meshPrivateBeacon.ivIndex];
        }
    }

    if (meshPrivateBeacon.keyRefreshFlag) {
        SigMeshLib.share.dataSource.curNetkeyModel.key = meshPrivateBeacon.networkKey.key;
    }
    if ([_networkManager.manager.delegate respondsToSelector:@selector(didReceiveSigMeshPrivateBeaconMessage:)]) {
        [_networkManager.manager.delegate didReceiveSigMeshPrivateBeaconMessage:meshPrivateBeacon];
    }
    if ([_networkManager.manager.delegateForDeveloper respondsToSelector:@selector(didReceiveSigMeshPrivateBeaconMessage:)]) {
        [_networkManager.manager.delegateForDeveloper didReceiveSigMeshPrivateBeaconMessage:meshPrivateBeacon];
    }
}

/// This method handles the Secure Network Beacon. It will set the proper IV Index and IV Update Active flag for the Network Key that matches Network ID and change the Key Refresh Phase based on the key refresh flag specified in the beacon.
/// @param secureNetworkBeacon The Secure Network Beacon received.
- (void)handleSecureNetworkBeacon:(SigSecureNetworkBeacon *)secureNetworkBeacon {
    if (!SigMeshLib.share.dataSource.existLocationIvIndexAndLocationSequenceNumber) {
        TelinkLogInfo(@"Local no ivIndex, set secure network beacon (ivIndex: 0x%x)",(unsigned int)secureNetworkBeacon.ivIndex);
        [SigMeshLib.share.dataSource setIvIndexUInt32:secureNetworkBeacon.ivIndex];
        [SigMeshLib.share.dataSource setSequenceNumberUInt32:0];
        [SigMeshLib.share.dataSource saveCurrentIvIndex:secureNetworkBeacon.ivIndex sequenceNumber:0];
    }
    SigNetkeyModel *networkKey = secureNetworkBeacon.networkKey;
    if (secureNetworkBeacon.ivIndex < networkKey.ivIndex.index || ABS(secureNetworkBeacon.ivIndex-networkKey.ivIndex.index) > 42) {
        TelinkLogError(@"Discarding secure network beacon (ivIndex: 0x%x, expected >= 0x%x)",(unsigned int)secureNetworkBeacon.ivIndex,(unsigned int)networkKey.ivIndex.index);
        if (SigMeshLib.share.dataSource.getSequenceNumberUInt32 >= 0xc00000) {
            SigSecureNetworkBeacon *beacon = [[SigSecureNetworkBeacon alloc] initWithKeyRefreshFlag:NO ivUpdateActive:YES networkId:networkKey.networkId ivIndex:networkKey.ivIndex.index+1 usingNetworkKey:networkKey];
            SigMeshLib.share.secureNetworkBeacon = beacon;
        } else {
            SigSecureNetworkBeacon *beacon = [[SigSecureNetworkBeacon alloc] initWithKeyRefreshFlag:NO ivUpdateActive:NO networkId:networkKey.networkId ivIndex:networkKey.ivIndex.index usingNetworkKey:networkKey];
            SigMeshLib.share.secureNetworkBeacon = beacon;
        }
        if ([_networkManager.manager.delegateForDeveloper respondsToSelector:@selector(didReceiveSigSecureNetworkBeaconMessage:)]) {
            [_networkManager.manager.delegateForDeveloper didReceiveSigSecureNetworkBeaconMessage:secureNetworkBeacon];
        }
        return;
    }
    SigMeshLib.share.secureNetworkBeacon = secureNetworkBeacon;
    SigIvIndex *ivIndex = [[SigIvIndex alloc] initWithIndex:secureNetworkBeacon.ivIndex updateActive:secureNetworkBeacon.ivUpdateActive];
    networkKey.ivIndex = ivIndex;
    TelinkLogVerbose(@"receive secure Network Beacon, ivIndex=0x%x,updateActive=%d",ivIndex.index,ivIndex.updateActive);

    // If the Key Refresh Procedure is in progress, and the new Network Key
    // has already been set, the key erfresh flag indicates switching to phase 2.
    if (networkKey.phase == distributingKeys && secureNetworkBeacon.keyRefreshFlag) {
        networkKey.phase = finalizing;
    }
    // If the Key Refresh Procedure is in phase 2, and the key refresh flag is
    // set to false.
    if (networkKey.phase == finalizing && !secureNetworkBeacon.keyRefreshFlag) {
        networkKey.oldKey = nil;//This will set the phase to .normalOperation.
    }

    if (secureNetworkBeacon.ivIndex > SigMeshLib.share.dataSource.curNetkeyModel.ivIndex.index) {
        if (secureNetworkBeacon.ivUpdateActive) {
            if (SigMeshLib.share.dataSource.curNetkeyModel.ivIndex.index != secureNetworkBeacon.ivIndex - 1) {
                SigMeshLib.share.dataSource.curNetkeyModel.ivIndex.updateActive = NO;
                SigMeshLib.share.dataSource.curNetkeyModel.ivIndex.index = secureNetworkBeacon.ivIndex - 1;
                [SigMeshLib.share.dataSource updateIvIndexUInt32FromBeacon:secureNetworkBeacon.ivIndex - 1];
            }
        } else {
            SigMeshLib.share.dataSource.curNetkeyModel.ivIndex.updateActive = secureNetworkBeacon.ivUpdateActive;
            SigMeshLib.share.dataSource.curNetkeyModel.ivIndex.index = secureNetworkBeacon.ivIndex;
            [SigMeshLib.share.dataSource updateIvIndexUInt32FromBeacon:secureNetworkBeacon.ivIndex];
        }
    }
    if (secureNetworkBeacon.keyRefreshFlag) {
        SigMeshLib.share.dataSource.curNetkeyModel.key = secureNetworkBeacon.networkKey.key;
    }
    if ([_networkManager.manager.delegate respondsToSelector:@selector(didReceiveSigSecureNetworkBeaconMessage:)]) {
        [_networkManager.manager.delegate didReceiveSigSecureNetworkBeaconMessage:secureNetworkBeacon];
    }
    if ([_networkManager.manager.delegateForDeveloper respondsToSelector:@selector(didReceiveSigSecureNetworkBeaconMessage:)]) {
        [_networkManager.manager.delegateForDeveloper didReceiveSigSecureNetworkBeaconMessage:secureNetworkBeacon];
    }
}

/// Handles the received Proxy Configuration PDU.
///
/// This method parses the payload and instantiates a message class.
/// The message is passed to the `ProxyFilter` for processing.
///
/// - parameter proxyPdu: The received Proxy Configuration PDU.
- (void)handleSigProxyConfigurationPdu:(SigNetworkPdu *)proxyPdu {
    NSData *payload = proxyPdu.transportPdu;
    if (payload.length <= 1) {
        TelinkLogError(@"payload.length <= 1");
        return;
    }
    SigControlMessage *controlMessage = [[SigControlMessage alloc] initFromNetworkPdu:proxyPdu];
    if (controlMessage == nil) {
        TelinkLogError(@"controlMessage == nil");
        return;
    }
//    TelinkLogInfo(@"%@ received (decrypted using key: %@)",controlMessage,controlMessage.networkKey);
    SigFilterStatus *filterStatus = [[SigFilterStatus alloc] init];
    if (controlMessage.opCode == filterStatus.opCode) {
        SigFilterStatus *message = [[SigFilterStatus alloc] initWithParameters:controlMessage.upperTransportPdu];
//        TelinkLogVerbose(@"%@ received SigFilterStatus data:%@ from: 0x%x to: 0x%x",message,controlMessage.upperTransportPdu,proxyPdu.source,proxyPdu.destination);
        if ([_networkManager.manager.delegate respondsToSelector:@selector(didReceiveSigProxyConfigurationMessage:sentFromSource:toDestination:)]) {
            [_networkManager.manager.delegate didReceiveSigProxyConfigurationMessage:message sentFromSource:proxyPdu.source toDestination:proxyPdu.destination];
        }
        if ([_networkManager.manager.delegateForDeveloper respondsToSelector:@selector(didReceiveSigProxyConfigurationMessage:sentFromSource:toDestination:)]) {
            [_networkManager.manager.delegateForDeveloper didReceiveSigProxyConfigurationMessage:message sentFromSource:proxyPdu.source toDestination:proxyPdu.destination];
        }
    }else{
        TelinkLogInfo(@"Unsupported proxy configuration message (opcode: 0x%x)",controlMessage.opCode);
    }
}

/// Returns whether the given Address is an address of a local Element.
///
/// - parameter address: The Address to check.
/// - returns: `True` if the address is a Unicast Address and belongs to
///            one of the local Node's elements; `false` otherwise.
- (BOOL)isLocalUnicastAddress:(UInt16)address {
    return [_meshNetwork.curLocationNodeModel hasAllocatedAddr:address];
}

/// Returns whether the PDU should loop back for local processing.
///
/// - parameter networkPdu: The PDU to check.
- (BOOL)shouldLoopback:(SigNetworkPdu *)networkPdu {
    UInt16 address = networkPdu.destination;
    return [SigHelper.share isGroupAddress:address] || [SigHelper.share isVirtualAddress:address] || [self isLocalUnicastAddress:address];
}

@end
