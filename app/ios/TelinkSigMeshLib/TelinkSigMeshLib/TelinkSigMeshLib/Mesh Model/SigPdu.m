/********************************************************************************************************
 * @file     SigPdu.m
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
//  SigPdu.m
//  SigMeshLib
//
//  Created by Liangjiazhi on 2019/9/9.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigPdu.h"
#import "OpenSSLHelper.h"
#import "SigLowerTransportPdu.h"
#import "OpenSSLHelper.h"
//#import "SigProvisionigState.h"

struct InvitePdu {
    UInt8 type;
    UInt8 timer;
};

struct StartPdu {
    UInt8 type;
    Algorithm algorithm;
    PublicKeyType publicKeyType;
    AuthenticationMethod authenticationMethod;
    UInt8 authenticationAction;//OutputAction or InputAction
    UInt8 authenticationSize;
};

struct PublicKeyPdu {
    UInt8 type;
    UInt8 publicKey[64];
};

struct ConfirmationPdu {
    UInt8 type;
    UInt8 confirmation[16];
};

struct RandomPdu {
    UInt8 type;
    UInt8 random[16];
};

struct EncryptedDataWithMicPdu {
    UInt8 type;
    UInt8 encryptedDataWithMic[33];
};

@implementation SigPdu
- (instancetype)init {
    self = [super init];
    if (self) {
        _pduData = [NSData data];
    }
    return self;
}
@end


@implementation SigProvisioningPdu

- (instancetype)initProvisioningInvitePduWithAttentionTimer:(UInt8)timer {
    if (self = [super init]) {
        self.pduData = [self getProvisioningInvitePduWithAttentionTimer:timer];
    }
    return self;
}
- (instancetype)initProvisioningstartPduWithAlgorithm:(Algorithm)algorithm publicKeyType:(PublicKeyType)publicKeyType authenticationMethod:(AuthenticationMethod)method authenticationAction:(UInt8)authenticationAction authenticationSize:(UInt8)authenticationSize {
    if (self = [super init]) {
        self.pduData = [self getProvisioningstartPduWithAlgorithm:algorithm publicKeyType:publicKeyType authenticationMethod:method authenticationAction:authenticationAction authenticationSize:authenticationSize];
    }
    return self;
}

- (instancetype)initProvisioningPublicKeyPduWithPublicKey:(NSData *)publicKey {
    if (self = [super init]) {
        self.pduData = [self getProvisioningPublicKeyPduWithPublicKey:publicKey];
    }
    return self;
}

- (instancetype)initProvisioningConfirmationPduWithConfirmation:(NSData *)confirmation {
    if (self = [super init]) {
        self.pduData = [self getProvisioningConfirmationPduWithConfirmation:confirmation];
    }
    return self;
}

- (instancetype)initProvisioningRandomPduWithRandom:(NSData *)random {
    if (self = [super init]) {
        self.pduData = [self getProvisioningRandomPduWithRandom:random];
    }
    return self;
}

- (instancetype)initProvisioningEncryptedDataWithMicPduWithEncryptedData:(NSData *)encryptedData {
    if (self = [super init]) {
        self.pduData = [self getProvisioningEncryptedDataWithMicPduWithEncryptedData:encryptedData];
    }
    return self;
}

+ (void)analysisProvisioningCapabilities:(struct ProvisioningCapabilities *)provisioningCapabilities withData:(NSData *)data {
    if (data.length != 12) {
        TeLogWarn(@"receive pdu isn't ProvisioningCapabilitiesPDU.")
        return;
    }
    
    Byte *byte = (Byte *)data.bytes;
    memcpy(provisioningCapabilities, byte, 12);
    
    if (provisioningCapabilities->pduType == SigProvisioningPduType_capabilities) {
        TeLogDebug(@"analysis ProvisioningCapabilitiesPDU success.")
    }else{
        TeLogDebug(@"analysis ProvisioningCapabilitiesPDU fail.")
        memcpy(provisioningCapabilities, 0, 12);
    }
}

///document in Mesh_v1.0.pdf 5.4.1 Page 238.
- (NSData *)getProvisioningInvitePduWithAttentionTimer:(UInt8)timer {
    //SigProvisioningPduType_invite + timer
    struct InvitePdu pdu = {};
    pdu.type = SigProvisioningPduType_invite;
    pdu.timer = timer;
    NSData *pduData = [NSData dataWithBytes:&pdu length:sizeof(pdu)];
    return pduData;
}

///document in Mesh_v1.0.pdf 5.4.1.3 Page 241.
- (NSData *)getProvisioningstartPduWithAlgorithm:(Algorithm)algorithm publicKeyType:(PublicKeyType)publicKeyType authenticationMethod:(AuthenticationMethod)method authenticationAction:(UInt8)authenticationAction authenticationSize:(UInt8)authenticationSize{
    //SigProvisioningPduType_start + Algorithm + Public Key type + authenticationMethod + authenticationAction + authenticationSize
    struct StartPdu pdu = {};
    pdu.type = SigProvisioningPduType_start;
    pdu.algorithm = algorithm;
    pdu.publicKeyType = publicKeyType;
    pdu.authenticationMethod = method;
    pdu.authenticationSize = authenticationSize;
    NSData *pduData = [NSData dataWithBytes:&pdu length:sizeof(pdu)];
    return pduData;
}

///document in Mesh_v1.0.pdf 5.4.1.4 Page 243.
- (NSData *)getProvisioningPublicKeyPduWithPublicKey:(NSData *)publicKey {
    struct PublicKeyPdu pdu = {};
    pdu.type = SigProvisioningPduType_publicKey;
    UInt8 *byte = (UInt8 *)publicKey.bytes;
    memcpy(pdu.publicKey, byte, publicKey.length);//64bytes
    NSData *pduData = [NSData dataWithBytes:&pdu length:sizeof(pdu)];
    return pduData;
}

///document in Mesh_v1.0.pdf 5.4.1.6 Page 243.
- (NSData *)getProvisioningConfirmationPduWithConfirmation:(NSData *)confirmation {
    struct ConfirmationPdu pdu = {};
    pdu.type = SigProvisioningPduType_confirmation;
    UInt8 *byte = (UInt8 *)confirmation.bytes;
    memcpy(pdu.confirmation, byte, confirmation.length);//16bytes
    NSData *pduData = [NSData dataWithBytes:&pdu length:sizeof(pdu)];
    return pduData;
}

///document in Mesh_v1.0.pdf 5.4.1.7 Page 243.
- (NSData *)getProvisioningRandomPduWithRandom:(NSData *)random {
    struct RandomPdu pdu = {};
    pdu.type = SigProvisioningPduType_random;
    UInt8 *byte = (UInt8 *)random.bytes;
    memcpy(pdu.random, byte, random.length);//16bytes
    NSData *pduData = [NSData dataWithBytes:&pdu length:sizeof(pdu)];
    return pduData;
}

///document in Mesh_v1.0.pdf 5.4.1.8 Page 244.
- (NSData *)getProvisioningEncryptedDataWithMicPduWithEncryptedData:(NSData *)encryptedData {
    struct EncryptedDataWithMicPdu pdu = {};
    pdu.type = SigProvisioningPduType_data;
    UInt8 *byte = (UInt8 *)encryptedData.bytes;
    memcpy(pdu.encryptedDataWithMic, byte, encryptedData.length);//25bytes(EncryptedData)+8bytes(Mic)
    NSData *pduData = [NSData dataWithBytes:&pdu length:sizeof(pdu)];
    return pduData;
}

@end


@implementation SigNetworkPdu

/// Creates Network PDU object from received PDU. The initiator tries
/// to deobfuscate and decrypt the data using given Network Key and IV Index.
///
/// - parameter pdu:        The data received from mesh network.
/// - parameter networkKey: The Network Key to decrypt the PDU.
/// - returns: The deobfuscated and decided Network PDU object, or `nil`,
///            if the key or IV Index don't match.
- (instancetype)initWithDecodePduData:(NSData *)pdu pduType:(SigPduType)pduType usingNetworkKey:(SigNetkeyModel *)networkKey {
    if (self = [super init]) {
        if (pduType != SigPduType_networkPdu && pduType != SigPduType_proxyConfiguration) {
            TeLogError(@"pdutype is not support.");
            return nil;
        }
        self.pduData = pdu;
        if (pdu.length < 14) {
            TeLogDebug(@"Valid message must have at least 14 octets.");
            return nil;
        }
        
        // The first byte is not obfuscated.
        UInt8 *byte = (UInt8 *)pdu.bytes;
        UInt8 tem = 0;
        memcpy(&tem, byte, 1);
        _ivi  = tem >> 7;
        _nid  = tem & 0x7F;
        // The NID must match.
        // If the Key Refresh procedure is in place, the received packet might have been
        // encrypted using an old key. We have to try both.
        NSMutableArray <SigNetkeyDerivaties *>*keySets = [NSMutableArray array];
        if (_nid == networkKey.nid) {
            [keySets addObject:networkKey.keys];
        }
        if (networkKey.oldKeys != nil && networkKey.oldNid == _nid) {
            [keySets addObject:networkKey.keys];
        }
        if (keySets.count == 0) {
            return nil;
        }
        
        // IVI should match the LSB bit of current IV Index.
        // If it doesn't, and the IV Update procedure is active, the PDU will be
        // deobfuscated and decoded with IV Index decremented by 1.
        UInt32 index = networkKey.ivIndex.index;
        if (_ivi != (index & 0x01)) {
            if (_networkKey.ivIndex.updateActive && index > 1) {
                index -= 1;
            } else {
                return nil;
            }
        }
        for (SigNetkeyDerivaties *keys in keySets) {
            // Deobfuscate CTL, TTL, SEQ and SRC.
            NSData *deobfuscatedData = [OpenSSLHelper.share deobfuscate:pdu ivIndex:index privacyKey:keys.privacyKey];

            // First validation: Control Messages have NetMIC of size 64 bits.
            byte = (UInt8 *)deobfuscatedData.bytes;
            memcpy(&tem, byte, 1);
            UInt8 ctl = tem >> 7;
            if (ctl != 0 && pdu.length < 18) {
                continue;
            }
            SigLowerTransportPduType type = (SigLowerTransportPduType)ctl;
            UInt8 ttl = tem & 0x7F;
            UInt32 tem1=0,tem2=0,tem3=0,tem4=0,tem5=0;
            memcpy(&tem1, byte+1, 1);
            memcpy(&tem2, byte+2, 1);
            memcpy(&tem3, byte+3, 1);
            memcpy(&tem4, byte+4, 1);
            memcpy(&tem5, byte+5, 1);

            // Multiple octet values use Big Endian.
            UInt32 sequence = tem1 << 16 | tem2 << 8 | tem3;
            UInt32 source = tem4 << 8 | tem5;
            NSInteger micOffset = pdu.length - [self getNetMicSizeOfLowerTransportPduType:type];
            NSData *destAndTransportPdu = [pdu subdataWithRange:NSMakeRange(7, micOffset-7)];
            NSData *mic = [pdu subdataWithRange:NSMakeRange(micOffset, pdu.length-micOffset)];
            tem = [self getNonceIdOfSigPduType:pduType];
            NSData *data1 = [NSData dataWithBytes:&tem length:1];
            UInt16 tem16 = 0;
            NSData *data2 = [NSData dataWithBytes:&tem16 length:2];
            UInt32 bigIndex = CFSwapInt32HostToBig(index);
            NSData *data3 = [NSData dataWithBytes:&bigIndex length:4];
            NSMutableData *networkNonce = [NSMutableData dataWithData:data1];
            [networkNonce appendData:deobfuscatedData];
            [networkNonce appendData:data2];
            [networkNonce appendData:data3];
            if (pduType == SigPduType_proxyConfiguration) {
                UInt8 zero = 0;// Pad
                [networkNonce replaceBytesInRange:NSMakeRange(1, 1) withBytes:&zero length:1];
            }
            NSData *decryptedData = [OpenSSLHelper.share calculateDecryptedCCM:destAndTransportPdu withKey:keys.encryptionKey nonce:networkNonce andMIC:mic withAdditionalData:nil];
            if (decryptedData == nil || decryptedData.length == 0) {
                TeLogError(@"decryptedData == nil");
                continue;
            }
            

            
            _networkKey = networkKey;
            _type = type;
            _ttl = ttl;
            _sequence = sequence;
            TeLogVerbose(@"返回蓝牙包的sequence=0x%x",(unsigned int)sequence);
            [SigDataSource.share updateCurrentProvisionerIntSequenceNumber:sequence+1];
            _source = source;
            UInt8 decryptedData0 = 0,decryptedData1 = 0;
            Byte *decryptedDataByte = (Byte *)decryptedData.bytes;
            memcpy(&decryptedData0, decryptedDataByte+0, 1);
            memcpy(&decryptedData1, decryptedDataByte+1, 1);
            _destination = (UInt16)decryptedData0 << 8 | (UInt16)decryptedData1;
            _transportPdu = [decryptedData subdataWithRange:NSMakeRange(2, decryptedData.length-2)];
//            TeLogDebug(@"================1.1._transportPdu=%@,ttl=0x%hhx,sequence=0x%x,decryptedData=0x%@",_transportPdu,_ttl,_sequence,decryptedData);
            return self;
        }
    }
    return nil;
}

/// Creates the Network PDU. This method enctypts and obfuscates data
/// that are to be send to the mesh network.
///
/// - parameter lowerTransportPdu: The data received from higher layer.
/// - parameter sequence: The SEQ number of the PDU. Each PDU between the source
///                       and destination must have strictly increasing sequence number.
/// - parameter ttl: Time To Live.
/// - returns: The Network PDU object.
- (instancetype)initWithEncodeLowerTransportPdu:(SigLowerTransportPdu *)lowerTransportPdu pduType:(SigPduType)pduType withSequence:(UInt32)sequence andTtl:(UInt8)ttl {
    if (self = [super init]) {
        UInt32 index = lowerTransportPdu.networkKey.ivIndex.index;

        _networkKey = lowerTransportPdu.networkKey;
        _ivi = (UInt8)(index&0x01);
        _nid = _networkKey.nid;
        _type = lowerTransportPdu.type;
        _source = lowerTransportPdu.source;
        _destination = lowerTransportPdu.destination;
        _transportPdu = lowerTransportPdu.transportPdu;
        _ttl = ttl;
        _sequence = sequence;

        UInt8 iviNid = (_ivi << 7) | (_nid & 0x7F);
        UInt8 ctlTtl = (_type << 7) | (_ttl & 0x7F);

        // Data to be obfuscated: CTL/TTL, Sequence Number, Source Address.
        UInt32 bigSequece = CFSwapInt32HostToBig(sequence);
        UInt16 bigSource = CFSwapInt16HostToBig(_source);
        UInt16 bigDestination = CFSwapInt16HostToBig(_destination);
        UInt32 bigIndex = CFSwapInt32HostToBig(index);

        NSData *seq = [[NSData dataWithBytes:&bigSequece length:4] subdataWithRange:NSMakeRange(1, 3)];
        NSData *data1 = [NSData dataWithBytes:&ctlTtl length:1];
        NSData *data2 = [NSData dataWithBytes:&bigSource length:2];
        NSMutableData *deobfuscatedData = [NSMutableData dataWithData:data1];
        [deobfuscatedData appendData:seq];
        [deobfuscatedData appendData:data2];

        // Data to be encrypted: Destination Address, Transport PDU.
        NSData *data3 = [NSData dataWithBytes:&bigDestination length:2];
        NSMutableData *decryptedData = [NSMutableData dataWithData:data3];
        [decryptedData appendData:_transportPdu];
        
        // The key set used for encryption depends on the Key Refresh Phase.
        SigNetkeyDerivaties *keys = _networkKey.transmitKeys;
        UInt8 tem = [self getNonceIdOfSigPduType:pduType];
        data1 = [NSData dataWithBytes:&tem length:1];
        UInt16 tem16 = 0;
        data2 = [NSData dataWithBytes:&tem16 length:2];
        data3 = [NSData dataWithBytes:&bigIndex length:4];
        NSMutableData *networkNonce = [NSMutableData dataWithData:data1];
        [networkNonce appendData:deobfuscatedData];
        [networkNonce appendData:data2];
        [networkNonce appendData:data3];
        if (pduType == SigPduType_proxyConfiguration) {
            tem = 0x00;//Pad
            [networkNonce replaceBytesInRange:NSMakeRange(1, 1) withBytes:&tem length:1];
        }

        NSData *encryptedData = [OpenSSLHelper.share calculateCCM:decryptedData withKey:keys.encryptionKey nonce:networkNonce andMICSize:[self getNetMicSizeOfLowerTransportPduType:_type] withAdditionalData:nil];
        NSData *obfuscatedData = [OpenSSLHelper.share obfuscate:deobfuscatedData usingPrivacyRandom:encryptedData ivIndex:index andPrivacyKey:keys.privacyKey];

        NSMutableData *pduData = [NSMutableData dataWithBytes:&iviNid length:1];
        [pduData appendData:obfuscatedData];
        [pduData appendData:encryptedData];
        self.pduData = pduData;
    }
    return self;
}

/// This method goes over all Network Keys in the mesh network and tries
/// to deobfuscate and decode the network PDU.
///
/// - parameter pdu:         The received PDU.
/// - parameter meshNetwork: The mesh network for which the PDU should be decoded.
/// - returns: The deobfuscated and decoded Network PDU, or `nil` if the PDU was not
///            signed with any of the Network Keys, the IV Index was not valid, or the
///            PDU was invalid.
+ (SigNetworkPdu *)decodePdu:(NSData *)pdu pduType:(SigPduType)pduType forMeshNetwork:(SigDataSource *)meshNetwork {
    for (SigNetkeyModel *networkKey in meshNetwork.netKeys) {
        SigNetworkPdu *networkPdu = [[SigNetworkPdu alloc] initWithDecodePduData:pdu pduType:pduType usingNetworkKey:networkKey];
        if (networkPdu) {
            return networkPdu;
        }
    }
    return nil;
}

- (UInt8)getNetMicSizeOfLowerTransportPduType:(SigLowerTransportPduType)pduType {
    UInt8 tem = 4;
    if (pduType == SigLowerTransportPduType_accessMessage) {
        tem = 4;// 32 bits
    }else if (pduType == SigLowerTransportPduType_controlMessage) {
        tem = 8;// 64 bits
    }
    return tem;
}

- (UInt8)getNonceIdOfSigPduType:(SigPduType)pduType {
    switch (pduType) {
        case SigPduType_networkPdu:
            return 0x00;
            break;
        case SigPduType_proxyConfiguration:
            return 0x03;
            break;
        default:
            TeLogError(@"Unsupported PDU Type:%lu",(unsigned long)pduType);
            break;
    }
    return 0;
}

- (BOOL)isSegmented {
    UInt8 tem = 0;
    Byte *byte = (Byte *)_transportPdu.bytes;
    memcpy(&tem, byte, 1);
    return (tem&0x80)>1;
}

- (UInt32)messageSequence {
    if (self.isSegmented) {
        UInt8 tem = 0,tem2 = 0;
        Byte *byte = (Byte *)_transportPdu.bytes;
        memcpy(&tem, byte+1, 1);
        memcpy(&tem2, byte+2, 1);
        UInt32 sequenceZero = (UInt16)((tem & 0x7F) << 6) | (UInt16)(tem2 >> 2);
        return (_sequence & 0xFFE000) | (UInt32)sequenceZero;
    } else {
        return _sequence;
    }
}

- (NSString *)description {
    int micSize = [self getNetMicSizeOfLowerTransportPduType:_type];
    NSInteger encryptedDataSize = self.pduData.length - micSize - 9;
    NSData *encryptedData = [self.pduData subdataWithRange:NSMakeRange(9, encryptedDataSize)];
    NSData *mic = [self.pduData subdataWithRange:NSMakeRange(9+encryptedDataSize,self.pduData.length - 9- encryptedDataSize)];
    return[NSString stringWithFormat:@"Network PDU (ivi: 0x%x, nid: 0x%x, ctl: 0x%x, ttl: 0x%x, seq: 0x%x, src: 0x%x, dst: 0x%x, transportPdu: %@, netMic: %@)",_ivi,_nid,_type,_ttl,_sequence,_source,_destination,encryptedData,mic];
}

@end


@implementation SigBeaconPdu

- (void)setBeaconType:(SigBeaconType)beaconType {
    _beaconType = beaconType;
}

@end


@implementation SigSecureNetworkBeacon

- (instancetype)init {
    if (self = [super init]) {
        [super setBeaconType:SigBeaconType_secureNetwork];
    }
    return self;
}

/// Creates USecure Network beacon PDU object from received PDU.
///
/// - parameter pdu: The data received from mesh network.
/// - parameter networkKey: The Network Key to validate the beacon.
/// - returns: The beacon object, or `nil` if the data are invalid.
- (instancetype)initWithDecodePdu:(NSData *)pdu usingNetworkKey:(SigNetkeyModel *)networkKey {
    if (self = [super init]) {
        [super setBeaconType:SigBeaconType_secureNetwork];
        self.pduData = pdu;
        UInt8 tem = 0;
        Byte *pduByte = (Byte *)pdu.bytes;
        memcpy(&tem, pduByte, 1);
        if (pdu.length != 22 || tem != 1) {
            TeLogError(@"pdu data error, can not init decode.");
            return nil;
        }
        memcpy(&tem, pduByte+1, 1);
        _keyRefreshFlag = (tem & 0x01) != 0;
        _ivUpdateActive = (tem & 0x02) != 0;
        _networkId = [pdu subdataWithRange:NSMakeRange(2, 8)];
        UInt32 tem10 = 0;
        memcpy(&tem10, &pduByte + 10, 4);
        _ivIndex = CFSwapInt32HostToBig(tem10);
        // Authenticate beacon using given Network Key.
        if ([_networkId isEqualToData:networkKey.networkId]) {
            NSData *authenticationValue = [OpenSSLHelper.share calculateCMAC:[pdu subdataWithRange:NSMakeRange(1, 13)] andKey:networkKey.keys.beaconKey];
            if (![[authenticationValue subdataWithRange:NSMakeRange(0, 8)] isEqualToData:[pdu subdataWithRange:NSMakeRange(14, 8)]]) {
                TeLogError(@"authenticationValue is not current networkID.");
                return nil;
            }
            _networkKey = networkKey;
        }else if (networkKey.oldNetworkId != nil && [networkKey.oldNetworkId isEqualToData:_networkId]) {
            NSData *authenticationValue = [OpenSSLHelper.share calculateCMAC:[pdu subdataWithRange:NSMakeRange(1, 13)] andKey:networkKey.oldKeys.beaconKey];
            if (![[authenticationValue subdataWithRange:NSMakeRange(0, 8)] isEqualToData:[pdu subdataWithRange:NSMakeRange(14, 8)]]) {
                TeLogError(@"authenticationValue is not current old networkID.");
                return nil;
            }
            _networkKey = networkKey;
        }else{
            return nil;
        }
    }
    return self;
}

/// 3.9.3 Secure Network beacon
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.120)


- (instancetype)initWithKeyRefreshFlag:(BOOL)keyRefreshFlag ivUpdateActive:(BOOL)ivUpdateActive networkId:(NSData *)networkId ivIndex:(UInt32)ivIndex usingNetworkKey:(SigNetkeyModel *)networkKey {
    if (self = [super init]) {
        _keyRefreshFlag = keyRefreshFlag;
        _ivUpdateActive = ivUpdateActive;
        _networkId = networkId;
        _ivIndex = ivIndex;
        _networkKey = networkKey;
    }
    return self;
}

- (SigLowerTransportPdu *)getSecureNetworkbeaconPdu {
    struct Flags flags = {};
    flags.value = 0;
    if (_keyRefreshFlag) {
        flags.value |= (1 << 0);
    }
    if (_ivUpdateActive) {
        flags.value |= (1 << 1);
    }
    NSMutableData *mData = [NSMutableData data];
    [mData appendData:[NSData dataWithBytes:&flags length:1]];
    [mData appendData:_networkId];
    UInt32 ivIndex32 = _ivIndex;
    [mData appendData:[NSData dataWithBytes:&ivIndex32 length:4]];
    NSData *authenticationValue = nil;
    if ([_networkId isEqualToData:_networkKey.networkId]) {
        authenticationValue = [OpenSSLHelper.share calculateCMAC:mData andKey:_networkKey.keys.beaconKey];
    }else if (_networkKey.oldNetworkId != nil && [_networkKey.oldNetworkId isEqualToData:_networkId]) {
        authenticationValue = [OpenSSLHelper.share calculateCMAC:mData andKey:_networkKey.oldKeys.beaconKey];
    }
    if (authenticationValue) {
        [mData appendData:authenticationValue];
        SigLowerTransportPdu *pdu = [[SigLowerTransportPdu alloc] init];
        pdu.source = SigMeshLib.share.dataSource.curLocationNodeModel.address;
        pdu.destination = 0;
        pdu.networkKey = _networkKey;
        pdu.type = SigLowerTransportPduType_accessMessage;
        pdu.transportPdu = mData;
        return pdu;
    } else {
        return nil;
    }
}

/// This method goes over all Network Keys in the mesh network and tries
/// to parse the beacon.
///
/// - parameter pdu:         The received PDU.
/// - parameter meshNetwork: The mesh network for which the PDU should be decoded.
/// - returns: The beacon object.
+ (SigSecureNetworkBeacon *)decodePdu:(NSData *)pdu forMeshNetwork:(SigDataSource *)meshNetwork {
    if (pdu == nil || pdu.length <= 1) {
        TeLogError(@"decodePdu length is less than 1.");
        return nil;
    }
    UInt8 tem = 0;
    Byte *pduByte = (Byte *)pdu.bytes;
    memcpy(&tem, pduByte, 1);
    SigBeaconType beaconType = tem;
    if (beaconType == SigBeaconType_secureNetwork) {
        for (SigNetkeyModel *networkKey in meshNetwork.netKeys) {
            SigSecureNetworkBeacon *beacon = [[SigSecureNetworkBeacon alloc] initWithDecodePdu:pdu usingNetworkKey:networkKey];
            if (beacon) {
                return beacon;
            }
        }
    } else {
        return nil;
    }
    return nil;
}

- (NSString *)description {
    return[NSString stringWithFormat:@"<%p> - Secure Network Beacon, network ID:(%@), ivIndex: (%x) Key refresh Flag: (%d), IV Update active: (%d)", self, _networkId,(unsigned int)_ivIndex, _keyRefreshFlag,_ivUpdateActive];
}

@end


@implementation SigUnprovisionedDeviceBeacon

- (instancetype)init {
    if (self = [super init]) {
        [super setBeaconType:SigBeaconType_unprovisionedDevice];
    }
    return self;
}

- (instancetype)initWithDecodePdu:(NSData *)pdu {
    if (self = [super init]) {
        [super setBeaconType:SigBeaconType_unprovisionedDevice];
        self.pduData = pdu;
        UInt8 tem = 0;
        Byte *pduByte = (Byte *)pdu.bytes;
        memcpy(&tem, pduByte, 1);
        if (pdu.length < 19 || tem == 0) {
            return nil;
        }
        _deviceUuid = [LibTools convertDataToHexStr:[pdu subdataWithRange:NSMakeRange(1, 16)]];
        UInt16 temOob = 0;
        memcpy(&temOob, pduByte+17, 1);
        _oob.value = temOob;
        if (pdu.length == 23) {
            _uriHash = [pdu subdataWithRange:NSMakeRange(19, pdu.length-19)];
        } else {
            _uriHash = nil;
        }
    }
    return self;
}

+ (SigUnprovisionedDeviceBeacon *)decodeWithPdu:(NSData *)pdu forMeshNetwork:(SigDataSource *)meshNetwork {
    if (pdu == nil || pdu.length == 0) {
        TeLogError(@"decodePdu length is 0.");
        return nil;
    }
    UInt8 tem = 0;
    Byte *pduByte = (Byte *)pdu.bytes;
    memcpy(&tem, pduByte, 1);
    SigBeaconType beaconType = tem;
    if (beaconType == SigBeaconType_unprovisionedDevice) {
        SigUnprovisionedDeviceBeacon *beacon = [[SigUnprovisionedDeviceBeacon alloc] initWithDecodePdu:pdu];
        return beacon;
    } else {
        return nil;
    }
}

- (NSString *)description {
    return[NSString stringWithFormat:@"<%p> - Unprovisioned Device Beacon, uuid:(%@), OOB Info: (%x) URI hash: (%@)", self, _deviceUuid,_oob.value, _uriHash];
}

@end


@implementation PublicKey

- (instancetype)initWithPublicKeyType:(PublicKeyType)type {
    if (self = [super init]) {
        UInt8 tem = type;
        _publicKeyType = type;
        _PublicKeyData = [NSData dataWithBytes:&tem length:1];
    }
    return self;
}

@end


@implementation SigProvisioningResponse

- (instancetype)initWithData:(NSData *)data {
    if (self = [super init]) {
        if (data == nil || data.length == 0) {
            TeLogDebug(@"response data error.")
            return nil;
        }
        
        if (data.length >= 1) {
            UInt8 type = 0;
            memcpy(&type, data.bytes, 1);
            if (type == 0 || type > 9) {
                TeLogDebug(@"response data pduType error.")
                return nil;
            }
            self.type = type;
        }
        self.responseData = data;
        switch (self.type) {
            case SigProvisioningPduType_capabilities:
            {
                TeLogDebug(@"receive capabilities.");
                struct ProvisioningCapabilities tem = {};
                [SigProvisioningPdu analysisProvisioningCapabilities:&tem withData:data];
                self.capabilities = tem;
                NSData *d = nil;
                self.publicKey = d;
                self.confirmation = d;
                self.random = d;
                self.error = 0;
            }
                break;
            case SigProvisioningPduType_publicKey:
            {
                TeLogDebug(@"receive publicKey.");
                self.publicKey = [data subdataWithRange:NSMakeRange(1, data.length - 1)];
                struct ProvisioningCapabilities tem = {};
                memset(&tem, 0, 12);
                self.capabilities = tem;
                NSData *d = nil;
                self.confirmation = d;
                self.random = d;
                self.error = 0;
            }
                break;
            case SigProvisioningPduType_inputComplete:
            case SigProvisioningPduType_complete:
            {
                TeLogDebug(@"receive inputComplete or complete.");
                struct ProvisioningCapabilities tem = {};
                memset(&tem, 0, 12);
                self.capabilities = tem;
                NSData *d = nil;
                self.publicKey = d;
                self.confirmation = d;
                self.random = d;
                self.error = 0;
                
            }
                break;
            case SigProvisioningPduType_confirmation:
            {
                TeLogDebug(@"receive confirmation.");
                self.confirmation = [data subdataWithRange:NSMakeRange(1, data.length - 1)];
                struct ProvisioningCapabilities tem = {};
                memset(&tem, 0, 12);
                self.capabilities = tem;
                NSData *d = nil;
                self.publicKey = d;
                self.random = d;
                self.error = 0;
            }
                break;
            case SigProvisioningPduType_random:
            {
                TeLogDebug(@"receive random.");
                self.random = [data subdataWithRange:NSMakeRange(1, data.length - 1)];
                struct ProvisioningCapabilities tem = {};
                memset(&tem, 0, 12);
                self.capabilities = tem;
                NSData *d = nil;
                self.publicKey = d;
                self.confirmation = d;
                self.error = 0;
            }
                break;
            case SigProvisioningPduType_failed:
            {
                TeLogDebug(@"receive failed.");
                if (data.length != 2) {
                    TeLogDebug(@"response data length error.")
                    return nil;
                }
                UInt8 status = 0;
                memcpy(&status, data.bytes+1, 1);
                if (status == 0) {
                    TeLogDebug(@"provision response fail data, but analysis status error.")
                    return nil;
                }
                struct ProvisioningCapabilities tem = {};
                memset(&tem, 0, 12);
                self.capabilities = tem;
                NSData *d = nil;
                self.publicKey = d;
                self.confirmation = d;
                self.error = status;
            }
                break;
            default:
                break;
        }
    }
    return self;
}

- (BOOL)isValid {
    switch (self.type) {
        case SigProvisioningPduType_capabilities:
        {
            struct ProvisioningCapabilities tem = self.capabilities;
            struct ProvisioningCapabilities zero = {};
            memset(&zero, 0, 12);
            return memcmp(&tem, &zero, 12) != 0;
        }
            break;
        case SigProvisioningPduType_publicKey:
        {
            return self.publicKey != nil;
        }
            break;
        case SigProvisioningPduType_inputComplete:
        case SigProvisioningPduType_complete:
        {
            return YES;
        }
            break;
        case SigProvisioningPduType_confirmation:
        {
            return self.confirmation != nil && self.confirmation.length == 16;
        }
            break;
        case SigProvisioningPduType_random:
        {
            return self.random != nil && self.random.length == 16;
        }
            break;
        case SigProvisioningPduType_failed:
        {
            return self.error != 0;
        }
            break;
        default:
            break;
    }
    return NO;
}

@end