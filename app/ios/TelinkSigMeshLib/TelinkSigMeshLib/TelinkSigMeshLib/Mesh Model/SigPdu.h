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
//  SigPdu.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/9.
//  Copyright © 2019 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class SigNetkeyModel,SigDataSource,SigLowerTransportPdu,SigIvIndex;

@interface SigPdu : NSObject
@property (nonatomic, strong) NSData *pduData;
@end


@interface SigProvisioningPdu : SigPdu
@property (nonatomic, assign) SigProvisioningPduType provisionType;
#pragma mark - 组包
- (instancetype)initProvisioningInvitePduWithAttentionTimer:(UInt8)timer;
- (instancetype)initProvisioningstartPduWithAlgorithm:(Algorithm)algorithm publicKeyType:(PublicKeyType)publicKeyType authenticationMethod:(AuthenticationMethod)method authenticationAction:(UInt8)authenticationAction authenticationSize:(UInt8)authenticationSize;
- (instancetype)initProvisioningPublicKeyPduWithPublicKey:(NSData *)publicKey;
- (instancetype)initProvisioningConfirmationPduWithConfirmation:(NSData *)confirmation;
- (instancetype)initProvisioningRandomPduWithRandom:(NSData *)random;
- (instancetype)initProvisioningEncryptedDataWithMicPduWithEncryptedData:(NSData *)encryptedData;
#pragma mark - 解包
+ (void)analysisProvisioningCapabilities:(struct ProvisioningCapabilities *)provisioningCapabilities withData:(NSData *)data;
@end

/// 3.4.4 Network PDU
/// - seeAlso: Mesh_v1.0.pdf (page.43)
@interface SigNetworkPdu : SigPdu
/// The Network Key used to decode/encode the PDU.
@property (nonatomic,strong) SigNetkeyModel *networkKey;

/// Least significant bit of IV Index.
@property (nonatomic,assign) UInt8 ivi;

/// Value derived from the NetKey used to identify the Encryption Key and Privacy Key used to secure this PDU.
@property (nonatomic,assign) UInt8 nid;

/// PDU type.
@property (nonatomic,assign) SigLowerTransportPduType type;

/// Time To Live.
@property (nonatomic,assign) UInt8 ttl;

/// Sequence Number.
@property (nonatomic,assign) UInt32 sequence;

/// Source Address.
@property (nonatomic,assign) UInt16 source;

/// Destination Address.
@property (nonatomic,assign) UInt16 destination;

/// Transport Protocol Data Unit. It is guaranteed to have 1 to 16 bytes.
@property (nonatomic,strong) NSData *transportPdu;

- (instancetype)initWithDecodePduData:(NSData *)pdu pduType:(SigPduType)pduType usingNetworkKey:(SigNetkeyModel *)networkKey ivIndex:(SigIvIndex *)ivIndex;

/// Creates Network PDU object from received PDU. The initiator tries to deobfuscate and decrypt the data using given Network Key and IV Index.
///
/// - parameter pdu:        The data received from mesh network.
/// - parameter pduType:    The type of the PDU: `.networkPdu` of `.proxyConfiguration`.
/// - parameter networkKey: The Network Key to decrypt the PDU.
/// - returns: The deobfuscated and decided Network PDU object, or `nil`, if the key or IV Index don't match.
- (instancetype)initWithDecodePduData:(NSData *)pdu pduType:(SigPduType)pduType usingNetworkKey:(SigNetkeyModel *)networkKey;

- (instancetype)initWithEncodeLowerTransportPdu:(SigLowerTransportPdu *)lowerTransportPdu pduType:(SigPduType)pduType withSequence:(UInt32)sequence andTtl:(UInt8)ttl ivIndex:(SigIvIndex *)ivIndex;

/// Creates the Network PDU. This method enctypts and obfuscates data that are to be send to the mesh network.
///
/// - parameter lowerTransportPdu: The data received from higher layer.
/// - parameter pduType:  The type of the PDU: `.networkPdu` of `.proxyConfiguration`.
/// - parameter sequence: The SEQ number of the PDU. Each PDU between the source and destination must have strictly increasing sequence number.
/// - parameter ttl: Time To Live.
/// - returns: The Network PDU object.
- (instancetype)initWithEncodeLowerTransportPdu:(SigLowerTransportPdu *)lowerTransportPdu pduType:(SigPduType)pduType withSequence:(UInt32)sequence andTtl:(UInt8)ttl;

+ (SigNetworkPdu *)decodePdu:(NSData *)pdu pduType:(SigPduType)pduType usingNetworkKey:(SigNetkeyModel *)networkKey ivIndex:(SigIvIndex *)ivIndex;

/// This method goes over all Network Keys in the mesh network and tries to deobfuscate and decode the network PDU.
///
/// - parameter pdu:         The received PDU.
/// - parameter type:        The type of the PDU: `.networkPdu` of `.proxyConfiguration`.
/// - parameter meshNetwork: The mesh network for which the PDU should be decoded.
/// - returns: The deobfuscated and decoded Network PDU, or `nil` if the PDU was not signed with any of the Network Keys, the IV Index was not valid, or the PDU was invalid.
+ (SigNetworkPdu *)decodePdu:(NSData *)pdu pduType:(SigPduType)pduType forMeshNetwork:(SigDataSource *)meshNetwork;

/// Whether the Network PDU contains a segmented Lower Transport PDU, or not.
- (BOOL)isSegmented;

/// The 24-bit Seq Auth used to transmit the first segment of a segmented message, or the 24-bit sequence number of an unsegmented
/// message.
- (UInt32)messageSequence;

@end


@interface SigBeaconPdu : SigPdu

/// The beacon type.
@property (nonatomic,assign,readonly) SigBeaconType beaconType;

@end


/// 3.9.3 Secure Network beacon
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.120)
@interface SigSecureNetworkBeacon : SigBeaconPdu
/// The Network Key related to this Secure Network Beacon.
@property (nonatomic,strong) SigNetkeyModel *networkKey;
/// Key Refresh flag value.
///
/// When this flag is active, the Node shall set the Key Refresh Phase for this Network Key to `.finalizing`. When in this phase, the Node shall only transmit messages and Secure Network beacons using the new keys, shall receive messages using the old keys and the new keys, and shall only receive Secure Network beacons secured using the new Network Key.
@property (nonatomic,assign) BOOL keyRefreshFlag;
/// This flag is set to `true` if IV Update procedure is active.
@property (nonatomic,assign) BOOL ivUpdateActive;
/// Contains the value of the Network ID.
@property (nonatomic,strong) NSData *networkId;
/// Contains the current IV Index.
@property (nonatomic,assign) UInt32 ivIndex;

- (NSDictionary *)getDictionaryOfSecureNetworkBeacon;
- (void)setDictionaryToSecureNetworkBeacon:(NSDictionary *)dictionary;

/// Creates USecure Network beacon PDU object from received PDU.
///
/// - parameter pdu: The data received from mesh network.
/// - parameter networkKey: The Network Key to validate the beacon.
/// - returns: The beacon object, or `nil` if the data are invalid.
- (instancetype)initWithDecodePdu:(NSData *)pdu usingNetworkKey:(SigNetkeyModel *)networkKey;
+ (SigSecureNetworkBeacon *)decodePdu:(NSData *)pdu forMeshNetwork:(SigDataSource *)meshNetwork;
- (instancetype)initWithKeyRefreshFlag:(BOOL)keyRefreshFlag ivUpdateActive:(BOOL)ivUpdateActive networkId:(NSData *)networkId ivIndex:(UInt32)ivIndex usingNetworkKey:(SigNetkeyModel *)networkKey;

@end


@interface SigUnprovisionedDeviceBeacon : SigBeaconPdu

/// Device UUID uniquely identifying this device.
@property (nonatomic,strong) NSString *deviceUuid;
/// The OOB Information field is used to help drive the provisioning process by indicating the availability of OOB data, such as a public key of the device.
@property (nonatomic,assign) OobInformation oob;
/// Hash of the associated URI advertised with the URI AD Type.
@property (nonatomic,strong) NSData *uriHash;

/// Creates Unprovisioned Device beacon PDU object from received PDU.
///
/// - parameter pdu: The data received from mesh network.
/// - returns: The beacon object, or `nil` if the data are invalid.
- (instancetype)initWithDecodePdu:(NSData *)pdu;

/// This method goes over all Network Keys in the mesh network and tries to parse the beacon.
///
/// - parameter pdu:         The received PDU.
/// - parameter meshNetwork: The mesh network for which the PDU should be decoded.
/// - returns: The beacon object.
+ (SigUnprovisionedDeviceBeacon *)decodeWithPdu:(NSData *)pdu forMeshNetwork:(SigDataSource *)meshNetwork;

@end


@interface PublicKey : NSObject
@property (nonatomic, strong) NSData *PublicKeyData;
@property (nonatomic, assign) PublicKeyType publicKeyType;
- (instancetype)initWithPublicKeyType:(PublicKeyType)type;
@end

@interface SigProvisioningResponse : NSObject
@property (nonatomic, strong) NSData *responseData;
@property (nonatomic, assign) SigProvisioningPduType type;
@property (nonatomic, assign) struct ProvisioningCapabilities capabilities;
@property (nonatomic, strong) NSData *publicKey;
@property (nonatomic, strong) NSData *confirmation;
@property (nonatomic, strong) NSData *random;
@property (nonatomic, assign) RemoteProvisioningError error;
- (instancetype)initWithData:(NSData *)data;
- (BOOL)isValid;
@end

NS_ASSUME_NONNULL_END
