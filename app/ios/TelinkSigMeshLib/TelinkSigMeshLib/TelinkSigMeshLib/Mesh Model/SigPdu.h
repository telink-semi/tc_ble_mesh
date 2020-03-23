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
//  SigMeshLib
//
//  Created by Liangjiazhi on 2019/9/9.
//  Copyright © 2019 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SigBearer.h"
#import "SigEnumeration.h"

NS_ASSUME_NONNULL_BEGIN

@class SigNetkeyModel,SigDataSource,SigLowerTransportPdu;

typedef enum : UInt8 {
    SigProvisioningPduType_invite = 0,
    SigProvisioningPduType_capabilities = 1,
    SigProvisioningPduType_start = 2,
    SigProvisioningPduType_publicKey = 3,
    SigProvisioningPduType_inputComplete = 4,
    SigProvisioningPduType_confirmation = 5,
    SigProvisioningPduType_random = 6,
    SigProvisioningPduType_data = 7,
    SigProvisioningPduType_complete = 8,
    SigProvisioningPduType_failed = 9,
} SigProvisioningPduType;

typedef enum : UInt8 {
    /// FIPS P-256 Elliptic Curve algorithm will be used to calculate the
    /// shared secret.
    Algorithm_fipsP256EllipticCurve = 0,
} Algorithm;

/// A set of algorighms supported by the Unprovisioned Device.
struct Algorithms {
    union{
        UInt16 value;//大端数据
        struct{
            /// FIPS P-256 Elliptic Curve algorithm is supported.
            UInt16 heightUint8      :8;//value的大端数据的高8个bit
            UInt16 fipsP256EllipticCurve      :1;//value的大端数据的低1个bit
        };
    };
};

/// The type of Device Public key to be used.
typedef enum : UInt8 {
    /// No OOB Public Key is used.
    PublicKeyType_noOobPublicKey = 0,
    /// OOB Public Key is used. The key must contain the full value of the Public Key,
    /// depending on the chosen algorithm.
    PublicKeyType_oobPublicKey = 1,
} PublicKeyType;

/// The authentication method chosen for provisioning.
typedef enum : UInt8 {
    /// No OOB authentication is used.
    AuthenticationMethod_noOob = 0,
    /// Static OOB authentication is used.
    AuthenticationMethod_staticOob = 1,
    /// Output OOB authentication is used.
    /// Size must be in range 1...8.
    AuthenticationMethod_outputOob = 2,
    /// Input OOB authentication is used.
    /// Size must be in range 1...8.
    AuthenticationMethod_inputOob = 3,
} AuthenticationMethod;

/// The output action will be displayed on the device.
/// For example, the device may use its LED to blink number of times.
/// The mumber of blinks will then have to be entered to the
/// Provisioner Manager.
typedef enum : NSUInteger {
    OutputAction_blink = 0,
    OutputAction_beep = 1,
    OutputAction_vibrate = 2,
    OutputAction_outputNumeric = 3,
    OutputAction_outputAlphanumeric = 4
} OutputAction;

/// The user will have to enter the input action on the device.
/// For example, if the device supports `.push`, user will be asked to
/// press a button on the device required number of times.
typedef enum : NSUInteger {
    InputAction_push = 0,
    InputAction_twist = 1,
    InputAction_inputNumeric = 2,
    InputAction_inputAlphanumeric = 3,
} InputAction;

/// A set of supported Static Out-of-band types.
struct StaticOobType {
    union{
        UInt8 value;
        struct{
            /// Static OOB Information is available.
            UInt8 staticOobInformationAvailable      :1;//value的低1个bit
        };
    };
};

/// A set of supported Output Out-of-band actions.
typedef struct{
    union{
        UInt16 value;
        struct{
            UInt8 blink      :1;//value的低1个bit
            UInt8 beep :1;//val的低位第2个bit
            UInt8 vibrate :1;//val的低位第3个bit
            UInt8 outputNumeric :1;//val的低位第4个bit
            UInt8 outputAlphanumeric :1;//val的低位第5个bit
            // Bits 6-16 are reserved for future use.
        };
        //        public var count: Int {
        //            return rawValue.nonzeroBitCount & 0b11111
        //        }
    };
}OutputOobActions;

/// A set of supported Input Out-of-band actions.
typedef struct{
    union{
        UInt16 value;
        struct{
            UInt8 push      :1;//value的低1个bit
            UInt8 twist :1;//val的低位第2个bit
            UInt8 inputNumeric :1;//val的低位第3个bit
            UInt8 inputAlphanumeric :1;//val的低位第4个bit
            // Bits 5-16 are reserved for future use.
        };
        //        public var count: Int {
        //            return rawValue.nonzeroBitCount & 0b1111
        //        }
    };
}InputOobActions;

struct ProvisioningCapabilities {
    union{
        UInt8 value[12];
        struct{
            ///this struct is invalid when pduType is not equal SigProvisioningPduType_capabilities
            SigProvisioningPduType pduType;//1byte
            /// Number of elements supported by the device.
            UInt8 numberOfElements;//1byte
            /// Supported algorithms and other capabilities.
            struct Algorithms algorithms;//2bytes
            /// Supported public key types.
            PublicKeyType publicKeyType;//1byte
            /// Supported statuc OOB Types.
            struct StaticOobType staticOobType;//1byte
            /// Maximum size of Output OOB supported.
            UInt8 outputOobSize;//1byte
            /// Supoprted Output OOB Actions.
            OutputOobActions outputOobActions;//2bytes
            /// Maximum size of Input OOB supported.
            UInt8 inputOobSize;//1byte
            /// Supoprted Input OOB Actions.
            InputOobActions inputOobActions;//2bytes
        };
    };
    
};

typedef enum : UInt8 {
    SigBeaconType_unprovisionedDevice = 0,
    SigBeaconType_secureNetwork = 1,
} SigBeaconType;




@interface SigPdu : NSObject
@property (nonatomic, strong) NSData *pduData;
@end


@interface SigProvisioningPdu : SigPdu
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


@interface SigNetworkPdu : SigPdu
/// The Network Key used to decode/encode the PDU.
@property (nonatomic,strong) SigNetkeyModel *networkKey;

/// Least significant bit of IV Index.
@property (nonatomic,assign) UInt8 ivi;

/// Value derived from the NetKey used to identify the Encryption Key
/// and Privacy Key used to secure this PDU.
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

/// Creates Network PDU object from received PDU. The initiator tries
/// to deobfuscate and decrypt the data using given Network Key and IV Index.
///
/// - parameter pdu:        The data received from mesh network.
/// - parameter pduType:    The type of the PDU: `.networkPdu` of `.proxyConfiguration`.
/// - parameter networkKey: The Network Key to decrypt the PDU.
/// - returns: The deobfuscated and decided Network PDU object, or `nil`,
///            if the key or IV Index don't match.
- (instancetype)initWithDecodePduData:(NSData *)pdu pduType:(SigPduType)pduType usingNetworkKey:(SigNetkeyModel *)networkKey;

/// Creates the Network PDU. This method enctypts and obfuscates data
/// that are to be send to the mesh network.
///
/// - parameter lowerTransportPdu: The data received from higher layer.
/// - parameter pduType:  The type of the PDU: `.networkPdu` of `.proxyConfiguration`.
/// - parameter sequence: The SEQ number of the PDU. Each PDU between the source
///                       and destination must have strictly increasing sequence number.
/// - parameter ttl: Time To Live.
/// - returns: The Network PDU object.
- (instancetype)initWithEncodeLowerTransportPdu:(SigLowerTransportPdu *)lowerTransportPdu pduType:(SigPduType)pduType withSequence:(UInt32)sequence andTtl:(UInt8)ttl;

/// This method goes over all Network Keys in the mesh network and tries
/// to deobfuscate and decode the network PDU.
///
/// - parameter pdu:         The received PDU.
/// - parameter type:        The type of the PDU: `.networkPdu` of `.proxyConfiguration`.
/// - parameter meshNetwork: The mesh network for which the PDU should be decoded.
/// - returns: The deobfuscated and decoded Network PDU, or `nil` if the PDU was not
///            signed with any of the Network Keys, the IV Index was not valid, or the
///            PDU was invalid.
+ (SigNetworkPdu *)decodePdu:(NSData *)pdu pduType:(SigPduType)pduType forMeshNetwork:(SigDataSource *)meshNetwork;

/// Whether the Network PDU contains a segmented Lower Transport PDU,
/// or not.
- (BOOL)isSegmented;

/// The 24-bit Seq Auth used to transmit the first segment of a
/// segmented message, or the 24-bit sequence number of an unsegmented
/// message.
- (UInt32)messageSequence;

@end


@interface SigBeaconPdu : SigPdu

/// The beacon type.
@property (nonatomic,assign,readonly) SigBeaconType beaconType;

@end


@interface SigSecureNetworkBeacon : SigBeaconPdu
/// The Network Key related to this Secure Network Beacon.
@property (nonatomic,strong) SigNetkeyModel *networkKey;
/// Key Refresh flag value.
///
/// When this flag is active, the Node shall set the Key Refresh
/// Phase for this Network Key to `.finalizing`. When in this phase,
/// the Node shall only transmit messages and Secure Network beacons
/// using the new keys, shall receive messages using the old keys
/// and the new keys, and shall only receive Secure Network beacons
/// secured using the new Network Key.
@property (nonatomic,assign) BOOL keyRefreshFlag;
/// This flag is set to `true` if IV Update procedure is active.
@property (nonatomic,assign) BOOL ivUpdateActive;
/// Contains the value of the Network ID.
@property (nonatomic,strong) NSData *networkId;
/// Contains the current IV Index.
@property (nonatomic,assign) UInt32 ivIndex;

- (instancetype)initWithDecodePdu:(NSData *)pdu usingNetworkKey:(SigNetkeyModel *)networkKey;
+ (SigSecureNetworkBeacon *)decodePdu:(NSData *)pdu forMeshNetwork:(SigDataSource *)meshNetwork;
- (instancetype)initWithKeyRefreshFlag:(BOOL)keyRefreshFlag ivUpdateActive:(BOOL)ivUpdateActive networkId:(NSData *)networkId ivIndex:(UInt32)ivIndex usingNetworkKey:(SigNetkeyModel *)networkKey;
- (SigLowerTransportPdu *)getSecureNetworkbeaconPdu;

@end


@interface SigUnprovisionedDeviceBeacon : SigBeaconPdu

/// Device UUID uniquely identifying this device.
@property (nonatomic,strong) NSString *deviceUuid;
/// The OOB Information field is used to help drive the provisioning
/// process by indicating the availability of OOB data, such as
/// a public key of the device.
@property (nonatomic,assign) OobInformation oob;
/// Hash of the associated URI advertised with the URI AD Type.
@property (nonatomic,strong) NSData *uriHash;

/// Creates Unprovisioned Device beacon PDU object from received PDU.
///
/// - parameter pdu: The data received from mesh network.
/// - returns: The beacon object, or `nil` if the data are invalid.
- (instancetype)initWithDecodePdu:(NSData *)pdu;

/// This method goes over all Network Keys in the mesh network and tries
/// to parse the beacon.
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
