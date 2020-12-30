/********************************************************************************************************
 * @file     SigNetworkLayer.m
 *
 * @brief    for TLSR chips
 *
 * @author     telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *             The information contained herein is confidential and proprietary property of Telink
 *              Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *             of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *             Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 *              Licensees are granted free, non-transferable use of the information in this
 *             file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/
//
//  SigNetworkLayer.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/9.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigNetworkLayer.h"
#import "SigNetworkManager.h"
#import "SigMeshLib.h"
#import "SigLowerTransportLayer.h"
#import "SigLowerTransportPdu.h"
#import "SigControlMessage.h"
#import "SigSegmentAcknowledgmentMessage.h"

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
    if (self = [super init]) {
        _networkManager = networkManager;
        _meshNetwork = networkManager.manager.dataSource;
//        _defaults = [NSUserDefaults standardUserDefaults];
//        _networkMessageCache = [[NSCache alloc] init];
    }
    return self;
}

- (void)handleIncomingPdu:(NSData *)pdu ofType:(SigPduType)type {
    if (_networkManager.manager.dataSource == nil) {
        TeLogError(@"this networkManager has not data.");
        return;
    }
    if (type == SigPduType_provisioningPdu) {
        TeLogError(@"Provisioning is handled using ProvisioningManager.");
        return;
    }
    
//    // Secure Network Beacons can repeat whenever the device connects to a new Proxy.
//    if (type != SigPduType_meshBeacon) {
//        // Ensure the PDU has not been handled already.
//        if ([_networkMessageCache objectForKey:pdu] != nil) {
//            TeLogDebug(@"PDU has already been handled.");
//            return;
//        }
//        [_networkMessageCache setObject:[[NSNull alloc] init] forKey:pdu];
//    }
    
    // Try decoding the PDU.
    switch (type) {
        case SigPduType_networkPdu:
            {
                TeLogDebug(@"receive networkPdu");
                //两个不同netkey进行解包（fast provision需要）:先使用mesh的networkKey进行解密，再使用当前networkLayer特定networkKey和ivIndex进行解密。
                SigNetworkPdu *networkPdu = [SigNetworkPdu decodePdu:pdu pduType:SigPduType_networkPdu forMeshNetwork:_meshNetwork];
                if (!networkPdu && _networkKey && _ivIndex) {
                    networkPdu = [SigNetworkPdu decodePdu:pdu pduType:SigPduType_networkPdu usingNetworkKey:_networkKey ivIndex:_ivIndex];
                }
                if (networkPdu == nil) {
                    TeLogDebug(@"decodePdu fail.");
                    return;
                }
                [_networkManager.lowerTransportLayer handleNetworkPdu:networkPdu];
                [SigMeshLib.share receiveNetworkPdu:networkPdu];
            }
            break;
        case SigPduType_meshBeacon:
            {
                TeLogVerbose(@"receive meshBeacon");
                SigSecureNetworkBeacon *beaconPdu = [SigSecureNetworkBeacon decodePdu:pdu forMeshNetwork:_meshNetwork];
                if (beaconPdu != nil) {
                    [self handleSecureNetworkBeacon:beaconPdu];
                    return;
                }
                SigUnprovisionedDeviceBeacon *unprovisionedBeacon = [SigUnprovisionedDeviceBeacon decodeWithPdu:pdu forMeshNetwork:_meshNetwork];
                if (unprovisionedBeacon != nil) {
                    [self handleUnprovisionedDeviceBeacon:unprovisionedBeacon];
                    return;
                }
                TeLogError(@"Invalid or unsupported beacon type.");
            }
            break;
        case SigPduType_proxyConfiguration:
            {
                TeLogVerbose(@"receive proxyConfiguration");
                SigNetworkPdu *proxyPdu = [SigNetworkPdu decodePdu:pdu pduType:type forMeshNetwork:_meshNetwork];
                if (proxyPdu == nil) {
                    TeLogInfo(@"Failed to decrypt proxy PDU");
                    return;
                }
                TeLogVerbose(@"%@ received",proxyPdu);
                [self handleSigProxyConfigurationPdu:proxyPdu];
            }
            break;
        default:
            TeLogDebug(@"pdu not handle.");
            break;
    }
}

- (void)sendLowerTransportPdu:(SigLowerTransportPdu *)pdu ofType:(SigPduType)type withTtl:(UInt8)ttl ivIndex:(SigIvIndex *)ivIndex {
    if (!SigBearer.share.isOpen) {
        TeLogError(@"bearer is closed.");
        return;
    }
    
    _ivIndex = ivIndex;
    _networkKey = pdu.networkKey;
    
    // Get the current sequence number for local Provisioner's source address.
    UInt32 sequence = (UInt32)[SigDataSource.share getCurrentProvisionerIntSequenceNumber];
    // As the sequnce number was just used, it has to be incremented.
    [SigDataSource.share updateCurrentProvisionerIntSequenceNumber:sequence+1];

//    TeLogVerbose(@"pdu,sequence=0x%x,ttl=%d",sequence,ttl);
    SigNetworkPdu *networkPdu = [[SigNetworkPdu alloc] initWithEncodeLowerTransportPdu:pdu pduType:type withSequence:sequence andTtl:ttl ivIndex:ivIndex];
    // Loopback interface.
    if ([self shouldLoopback:networkPdu]) {
        //==========telink not need this==========//
//        [self handleIncomingPdu:networkPdu.pduData ofType:type];
        //==========telink not need this==========//
        if ([self isLocalUnicastAddress:networkPdu.destination]) {
            // No need to send messages targetting local Unicast Addresses.
            TeLogVerbose(@"No need to send messages targetting local Unicast Addresses.");
            return;
        }
        [SigBearer.share sendBlePdu:networkPdu ofType:type];
    }else{
        [SigBearer.share sendBlePdu:networkPdu ofType:type];
    }

    // SDK need use networkTransmit in gatt provision.
    SigNetworktransmitModel *networkTransmit = _meshNetwork.curLocationNodeModel.networkTransmit;
    if (type == SigPduType_networkPdu && networkTransmit != nil && networkTransmit.count > 1 && !SigBearer.share.isProvisioned) {
        self.networkTransmitCount = networkTransmit.count;
        __block NSInteger count = networkTransmit.count;
        __weak typeof(self) weakSelf = self;
        BackgroundTimer *timer = [BackgroundTimer scheduledTimerWithTimeInterval:networkTransmit.interval repeats:YES block:^(BackgroundTimer * _Nonnull t) {
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

- (void)sendLowerTransportPdu:(SigLowerTransportPdu *)pdu ofType:(SigPduType)type withTtl:(UInt8)ttl {
    if (!SigBearer.share.isOpen) {
        TeLogError(@"bearer is closed.");
        return;
    }
    if ([pdu isMemberOfClass:[SigSegmentAcknowledgmentMessage class]]) {
        if (SigBearer.share.isSending) {
            self.lastNeedSendAckMessage = (SigSegmentAcknowledgmentMessage *)pdu;
            return;
        } else {
            self.lastNeedSendAckMessage = nil;
        }
    }
    
    _ivIndex = SigMeshLib.share.dataSource.curNetkeyModel.ivIndex;
    _networkKey = pdu.networkKey;
    if (pdu.ivIndex == nil) {
        pdu.ivIndex = _ivIndex;
    }
    
    // Get the current sequence number for local Provisioner's source address.
    UInt32 sequence = (UInt32)[SigDataSource.share getCurrentProvisionerIntSequenceNumber];
    // As the sequnce number was just used, it has to be incremented.
    [SigDataSource.share updateCurrentProvisionerIntSequenceNumber:sequence+1];

//    TeLogVerbose(@"pdu,sequence=0x%x,ttl=%d",sequence,ttl);
//    SigNetworkPdu *networkPdu = [[SigNetworkPdu alloc] initWithEncodeLowerTransportPdu:pdu pduType:type withSequence:sequence andTtl:ttl];
    if (pdu.networkKey == nil || pdu.ivIndex == nil) {
        TeLogError(@"networkKey or ivIndex error!!!");
    }
    SigNetworkPdu *networkPdu = [[SigNetworkPdu alloc] initWithEncodeLowerTransportPdu:pdu pduType:type withSequence:sequence andTtl:ttl ivIndex:pdu.ivIndex];
    // Loopback interface.
    if ([self shouldLoopback:networkPdu]) {
        //==========telink not need this==========//
//        [self handleIncomingPdu:networkPdu.pduData ofType:type];
        //==========telink not need this==========//
        if ([self isLocalUnicastAddress:networkPdu.destination]) {
            // No need to send messages targetting local Unicast Addresses.
            TeLogError(@"No need to send messages targetting local Unicast Addresses.");
            return;
        }
        [SigBearer.share sendBlePdu:networkPdu ofType:type];
    }else{
        [SigBearer.share sendBlePdu:networkPdu ofType:type];
    }
    if (self.lastNeedSendAckMessage) {
        //发包过程中收到segment的结束包，优先把当前包发送完成，再发送ack包。
        TeLogDebug(@"==========灵活处理中间的ack数据包。")
        SigNodeModel *provisionerNode = SigDataSource.share.curLocationNodeModel;
        UInt8 ttl = provisionerNode.defaultTTL;
        if (ttl < 2) {
            ttl = 10;
        }
        [self sendLowerTransportPdu:self.lastNeedSendAckMessage ofType:SigPduType_networkPdu withTtl:ttl];
    }
    
    // SDK need use networkTransmit in gatt provision.
    SigNetworktransmitModel *networkTransmit = _meshNetwork.curLocationNodeModel.networkTransmit;
    if (type == SigPduType_networkPdu && networkTransmit != nil && networkTransmit.count > 1 && !SigBearer.share.isProvisioned) {
        self.networkTransmitCount = networkTransmit.count;
        __block NSInteger count = networkTransmit.count;
        __weak typeof(self) weakSelf = self;
        BackgroundTimer *timer = [BackgroundTimer scheduledTimerWithTimeInterval:networkTransmit.interval repeats:YES block:^(BackgroundTimer * _Nonnull t) {
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
/// - parameter message: The Proxy Confifuration message to be sent.
- (void)sendSigProxyConfigurationMessage:(SigProxyConfigurationMessage *)message {
//    SigNetkeyModel *networkKey = _proxyNetworkKey;
    SigNetkeyModel *networkKey = _meshNetwork.curNetkeyModel;

    // If the Provisioner does not have a Unicast Address, just use a fake one
    // to configure the Proxy Server. This allows sniffing the network without
    // an option to send messages.
    UInt16 source = _meshNetwork.curLocationNodeModel.address != 0 ? _meshNetwork.curLocationNodeModel.address : MeshAddress_maxUnicastAddress;
    TeLogInfo(@"Sending %@%@ from: 0x%x to: 0000",message,message.parameters,source);
    SigControlMessage *pdu = [[SigControlMessage alloc] initFromProxyConfigurationMessage:message sentFromSource:source usingNetworkKey:networkKey];
    pdu.ivIndex = SigMeshLib.share.dataSource.curNetkeyModel.ivIndex;
    [self sendLowerTransportPdu:pdu ofType:SigPduType_proxyConfiguration withTtl:0];
    [_networkManager notifyAboutDeliveringMessage:(SigMeshMessage *)message fromLocalElement:SigMeshLib.share.dataSource.curLocationNodeModel.elements.firstObject toDestination:pdu.destination];
}

#pragma mark - private

/// This method handles the Unprovisioned Device Beacon.
///
/// The curernt implementation does nothing, as remote provisioning is
/// currently not supported.
///
/// - parameter unprovisionedDeviceBeacon: The Unprovisioned Device Beacon received.
- (void)handleUnprovisionedDeviceBeacon:(SigUnprovisionedDeviceBeacon *)unprovisionedDeviceBeacon {
    // TODO: Handle Unprovisioned Device Beacon.
}

/// This method handles the Secure Network Beacon. It will set the proper IV Index and IV Update Active flag for the Network Key that matches Network ID and change the Key Refresh Phase based on the key refresh flag specified in the beacon.
/// @param secureNetworkBeacon The Secure Network Beacon received.
- (void)handleSecureNetworkBeacon:(SigSecureNetworkBeacon *)secureNetworkBeacon {
    SigNetkeyModel *networkKey = secureNetworkBeacon.networkKey;
    if (secureNetworkBeacon.ivIndex < networkKey.ivIndex.index) {
        TeLogError(@"Discarding beacon (ivIndex: 0x%x, expected >= 0x%x)",(unsigned int)secureNetworkBeacon.ivIndex,(unsigned int)networkKey.ivIndex.index);
        return;
    }
    SigIvIndex *ivIndex = [[SigIvIndex alloc] initWithIndex:secureNetworkBeacon.ivIndex updateActive:secureNetworkBeacon.ivUpdateActive];
    networkKey.ivIndex = ivIndex;
    //==========test=========//
    TeLogVerbose(@"==========receive secure Network Beacon, ivIndex=0x%x",ivIndex.index);
    //==========test=========//

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
    
    //===========telink==========//
    if (secureNetworkBeacon.ivUpdateActive) {
        if (SigDataSource.share.curNetkeyModel.ivIndex.index != secureNetworkBeacon.ivIndex - 1) {
            SigDataSource.share.curNetkeyModel.ivIndex.updateActive = NO;
            SigDataSource.share.curNetkeyModel.ivIndex.index = secureNetworkBeacon.ivIndex - 1;
            [SigDataSource.share setIvIndex:[NSString stringWithFormat:@"%08X",(unsigned int)secureNetworkBeacon.ivIndex - 1]];
        }
    } else {
        SigDataSource.share.curNetkeyModel.ivIndex.updateActive = secureNetworkBeacon.ivUpdateActive;
        SigDataSource.share.curNetkeyModel.ivIndex.index = secureNetworkBeacon.ivIndex;
        [SigDataSource.share setIvIndex:[NSString stringWithFormat:@"%08X",(unsigned int)secureNetworkBeacon.ivIndex]];
    }
    //===========telink==========//
    if (secureNetworkBeacon.keyRefreshFlag) {
        SigDataSource.share.curNetkeyModel.key = secureNetworkBeacon.networkKey.key;
    }
    if ([_networkManager.manager.delegate respondsToSelector:@selector(didReceiveSigSecureNetworkBeaconMessage:)]) {
        [_networkManager.manager.delegate didReceiveSigSecureNetworkBeaconMessage:secureNetworkBeacon];
    }

//    [self updateProxyFilterUsingNetworkKey:networkKey];
}

///// Updates the information about the Network Key known to the current Proxy Server.
///// The Network Key is required to send Proxy Configuration Messages that can be
///// decoded by the connected Proxy.
/////
///// If the method detects that the Proxy has just been connected, or was reconnected,
///// it will initiate the Proxy Filter with local Provisioner's Unicast Address and
///// the `Address.allNodes` group address.
/////
///// - parameter networkKey: The Network Key known to the connected Proxy.
//- (void)updateProxyFilterUsingNetworkKey:(SigNetkeyModel *)networkKey {
//    BOOL justConnected = _proxyNetworkKey == nil;
//    BOOL reconnected = networkKey == _proxyNetworkKey;
//    // Keep the primary Network Key or the most recently received one from the connected
//    // Proxy Server. This is to make sure (almost) that the Proxy Configuration messages
//    // are sent encrypted with a key known to this Node.
//    if (justConnected || networkKey.isPrimary || _proxyNetworkKey.isPrimary == NO) {
//        _proxyNetworkKey = networkKey;
//    }
//    if (justConnected || reconnected) {
//        if ([_networkManager.manager.delegate respondsToSelector:@selector(newProxyDidConnect)]) {
//            [_networkManager.manager.delegate newProxyDidConnect];
//        }
//    }
//}

/// Handles the received Proxy Configuration PDU.
///
/// This method parses the payload and instantiates a message class.
/// The message is passed to the `ProxyFilter` for processing.
///
/// - parameter proxyPdu: The received Proxy Configuration PDU.
- (void)handleSigProxyConfigurationPdu:(SigNetworkPdu *)proxyPdu {
    NSData *payload = proxyPdu.transportPdu;
    if (payload.length <= 1) {
        TeLogError(@"payload.length <= 1");
        return;
    }
    SigControlMessage *controlMessage = [[SigControlMessage alloc] initFromNetworkPdu:proxyPdu];
    if (controlMessage == nil) {
        TeLogError(@"controlMessage == nil");
        return;
    }
//    TeLogInfo(@"%@ receieved (decrypted using key: %@)",controlMessage,controlMessage.networkKey);
    SigFilterStatus *filterStatus = [[SigFilterStatus alloc] init];
    if (controlMessage.opCode == filterStatus.opCode) {
        SigFilterStatus *message = [[SigFilterStatus alloc] initWithParameters:controlMessage.upperTransportPdu];
        TeLogVerbose(@"%@ received SigFilterStatus data:%@ from: 0x%x to: 0x%x",message,controlMessage.upperTransportPdu,proxyPdu.source,proxyPdu.destination);
        if ([_networkManager.manager.delegate respondsToSelector:@selector(didReceiveSigProxyConfigurationMessage:sentFromSource:toDestination:)]) {
            [_networkManager.manager.delegate didReceiveSigProxyConfigurationMessage:message sentFromSource:proxyPdu.source toDestination:proxyPdu.destination];
        }
        if ([_networkManager.manager.delegateForDeveloper respondsToSelector:@selector(didReceiveSigProxyConfigurationMessage:sentFromSource:toDestination:)]) {
            [_networkManager.manager.delegateForDeveloper didReceiveSigProxyConfigurationMessage:message sentFromSource:proxyPdu.source toDestination:proxyPdu.destination];
        }
    }else{
        TeLogInfo(@"Unsupported proxy configuration message (opcode: 0x%x)",controlMessage.opCode);
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
