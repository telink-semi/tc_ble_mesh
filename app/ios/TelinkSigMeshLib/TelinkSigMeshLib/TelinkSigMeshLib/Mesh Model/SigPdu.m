/********************************************************************************************************
 * @file     SigPdu.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/9/9
 *
 * @par     Copyright (c) [2021], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "SigPdu.h"
#import "OpenSSLHelper.h"
#import "SigLowerTransportPdu.h"
#import "OpenSSLHelper.h"

@implementation SigPdu
/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _pduData = [NSData data];
    }
    return self;
}
@end


@implementation SigProvisioningPdu

/// Get Provisioning Pdu Class
/// - Parameter provisioningPduType: The SigProvisioningPduType value.
+ (Class)getProvisioningPduClassWithProvisioningPduType:(SigProvisioningPduType)provisioningPduType {
    Class messageType = nil;
    switch (provisioningPduType) {
        case SigProvisioningPduType_invite:
            messageType = [SigProvisioningInvitePdu class];
            break;
        case SigProvisioningPduType_capabilities:
            messageType = [SigProvisioningCapabilitiesPdu class];
            break;
        case SigProvisioningPduType_start:
            messageType = [SigProvisioningStartPdu class];
            break;
        case SigProvisioningPduType_publicKey:
            messageType = [SigProvisioningPublicKeyPdu class];
            break;
        case SigProvisioningPduType_inputComplete:
            messageType = [SigProvisioningInputCompletePdu class];
            break;
        case SigProvisioningPduType_confirmation:
            messageType = [SigProvisioningConfirmationPdu class];
            break;
        case SigProvisioningPduType_random:
            messageType = [SigProvisioningRandomPdu class];
            break;
        case SigProvisioningPduType_data:
            messageType = [SigProvisioningDataPdu class];
            break;
        case SigProvisioningPduType_complete:
            messageType = [SigProvisioningCompletePdu class];
            break;
        case SigProvisioningPduType_failed:
            messageType = [SigProvisioningFailedPdu class];
            break;
        case SigProvisioningPduType_recordRequest:
            messageType = [SigProvisioningRecordRequestPdu class];
            break;
        case SigProvisioningPduType_recordResponse:
            messageType = [SigProvisioningRecordResponsePdu class];
            break;
        case SigProvisioningPduType_recordsGet:
            messageType = [SigProvisioningInvitePdu class];
            break;
        case SigProvisioningPduType_recordsList:
            messageType = [SigProvisioningRecordsListPdu class];
            break;
        default:
            break;
    }
    return messageType;
}

@end


/// A Provisioner sends a Provisioning Invite PDU to indicate to the intended Provisionee that the provisioning
/// process is starting.
/// 5.4.1.1 Provisioning Invite
/// - seeAlso: MshPRT_v1.1.pdf (page.577)
@implementation SigProvisioningInvitePdu

/**
 * @brief   Initialize SigProvisioningInvitePdu object.
 * @param   attentionDuration    Attention Timer state (See Section 4.2.9).
 * The Attention Timer state determines if the Attention Timer state is on or off. This is generally intended to allow an
 * element to attract human attention and, among others, is used during provisioning (see Section 5.4.1.11).
 * An element may not support the Attention Timer. If an element does not support the Attention Timer, the Attention
 * Timer state shall always be set to zero.
 * If the Attention Timer is non-zero for an element, Attention Timer state is on. If the Attention Timer is zero for an
 * element, the Attention Timer state is off.
 * When the Attention Timer state is on, the value determines how long the element shall remain attracting human’s
 * attention. The element does that by behaving in a human-recognizable way (e.g., a lamp flashes, a motor makes
 * noise, an LED blinks). The exact behavior is implementation specific and depends on the type of device. Normal
 * behavior of the element is still active, although the method of identification may override the physical state of the
 * device.
 * The Attention Timer is a momentary state, active for a time indicated by its value, in seconds. The value is
 * decremented every second by 1 until it reaches zero. The values for this state are defined in Table 4.25.
 * @return  return `nil` when initialize SigProvisioningInvitePdu object fail.
 */
- (instancetype)initWithAttentionDuration:(UInt8)attentionDuration {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.provisionType = SigProvisioningPduType_invite;
        _attentionDuration = attentionDuration;
        NSMutableData *mData = [NSMutableData data];
        UInt8 tem8 = self.provisionType;
        NSData *data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = attentionDuration;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        self.pduData = mData;
    }
    return self;
}

@end


/// The Provisionee sends a Provisioning Capabilities PDU to indicate its supported provisioning capabilities to a Provisioner.
/// 5.4.1.2 Provisioning Capabilities
/// - seeAlso: MshPRT_v1.1.pdf (page.577)
@implementation SigProvisioningCapabilitiesPdu

/**
 * @brief   Initialize SigProvisioningCapabilitiesPdu object.
 * @param   parameters    the hex data of SigProvisioningCapabilitiesPdu.
 * @return  return `nil` when initialize SigProvisioningCapabilitiesPdu object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters && parameters.length == 12) {
            self.pduData = [NSData dataWithData:parameters];
            UInt8 tem8 = 0;
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem8, dataByte, 1);
            if (tem8 == SigProvisioningPduType_capabilities) {
                self.provisionType = SigProvisioningPduType_capabilities;
                memcpy(&tem8, dataByte+1, 1);
                _numberOfElements = tem8;
                memcpy(&tem16, dataByte+2, 2);
                _algorithms.value = tem16;
                memcpy(&tem8, dataByte+4, 1);
                _publicKeyType = tem8;
                memcpy(&tem8, dataByte+5, 1);
                _staticOobType.value = tem8;
                memcpy(&tem8, dataByte+6, 1);
                _outputOobSize = tem8;
                memcpy(&tem16, dataByte+7, 2);
                _outputOobActions.value = tem16;
                memcpy(&tem8, dataByte+9, 1);
                _outputOobSize = tem8;
                memcpy(&tem16, dataByte+10, 2);
                _outputOobActions.value = tem16;
                return self;
            } else {
                return nil;
            }
        }else{
            return nil;
        }
    }
    return self;
}

/// Get Description String of SigProvisioningCapabilitiesPdu.algorithms.
- (NSString *)getAlgorithmsString {
    NSString *tem = @"";
    if (_algorithms.fipsP256EllipticCurve == 1) {
        tem = [tem stringByAppendingString:@"FIPS P-256 Elliptic Curve"];
    }
    if (_algorithms.fipsP256EllipticCurve_HMAC_SHA256 == 1) {
        if (tem.length > 0) {
            tem = [tem stringByAppendingString:@"\n"];
        }
        tem = [tem stringByAppendingString:@"FIPS P-256 Elliptic Curve - HMAC - SHA256"];
    }
    if (tem.length == 0) {
        tem = @"None";
    }
    return tem;
}

/// Get Description String of SigProvisioningCapabilitiesPdu object.
- (NSString *)getCapabilitiesString {
    NSString *string = [NSString stringWithFormat:@"\n------ Capabilities ------\nNumber of elements: %d\nAlgorithms: %@\nPublic Key Type: %@\nStatic OOB Type: %@\nOutput OOB Size: %d\nOutput OOB Actions: %d\nInput OOB Size: %d\nInput OOB Actions: %d\n--------------------------",_numberOfElements,[self getAlgorithmsString],_publicKeyType == PublicKeyType_noOobPublicKey ?@"No OOB Public Key":@"OOB Public Key",_staticOobType.staticOobInformationAvailable == 1 ?@"YES":@"None",_outputOobSize,_outputOobActions.value,_inputOobSize,_inputOobActions.value];
    return string;
}

@end


/// A Provisioner sends a Provisioning Start PDU to indicate the method it has selected
/// from the options in the Provisioning Capabilities PDU.
/// 5.4.1.3 Provisioning Start
/// - seeAlso: MshPRT_v1.1.pdf (page.580)
@implementation SigProvisioningStartPdu

/**
 * @brief   Initialize SigProvisioningStartPdu object.
 * @param   algorithm    The algorithm used for provisioning (see Table 5.29).
 * @param   publicKeyType    Public Key used (see Table 5.30).
 * @param   authenticationMethod    Authentication Method used (see Table 5.31).
 * @param   authenticationAction    Selected Output OOB Action (see Table 5.32) or Input M OOB Action (see Table 5.34) or 0x00.
 * @param   authenticationSize    Size of the Output OOB used (see Table 5.33) or size M of the Input OOB used (see Table 5.35) or 0x00.
 * @return  return `nil` when initialize SigProvisioningStartPdu object fail.
 */
- (instancetype)initWithAlgorithm:(Algorithm)algorithm publicKeyType:(PublicKeyType)publicKeyType authenticationMethod:(AuthenticationMethod)authenticationMethod authenticationAction:(UInt8)authenticationAction authenticationSize:(UInt8)authenticationSize {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.provisionType = SigProvisioningPduType_start;
        _algorithm = algorithm;
        _publicKeyType = publicKeyType;
        _authenticationMethod = authenticationMethod;
        _authenticationAction = authenticationAction;
        _authenticationSize = authenticationSize;
        
        NSMutableData *mData = [NSMutableData data];
        UInt8 tem8 = self.provisionType;
        NSData *data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = algorithm;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = publicKeyType;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = authenticationMethod;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = authenticationAction;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = authenticationSize;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        self.pduData = mData;
    }
    return self;
}

@end


/// The Provisioner sends a Provisioning Public Key PDU to deliver the public key to be used in the ECDH calculations.
/// 5.4.1.4 Provisioning Public Key
/// - seeAlso: MshPRT_v1.1.pdf (page.582)
@implementation SigProvisioningPublicKeyPdu

/**
 * @brief   Initialize SigProvisioningPublicKeyPdu object.
 * @param   publicKey    The public key for the FIPS P-256 algorithm.
 * @return  return `nil` when initialize SigProvisioningPublicKeyPdu object fail.
 */
- (instancetype)initWithPublicKey:(NSData *)publicKey {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.provisionType = SigProvisioningPduType_publicKey;
        if (publicKey.length == 64) {
            _publicKey = publicKey;
            _publicKeyX = [publicKey subdataWithRange:NSMakeRange(0, 32)];
            _publicKeyY = [publicKey subdataWithRange:NSMakeRange(32, 32)];
            NSMutableData *mData = [NSMutableData data];
            UInt8 tem8 = self.provisionType;
            NSData *data = [NSData dataWithBytes:&tem8 length:1];
            [mData appendData:data];
            [mData appendData:_publicKey];
            self.pduData = mData;
        } else {
            return nil;
        }
    }
    return self;
}

/**
 * @brief   Initialize SigProvisioningPublicKeyPdu object.
 * @param   publicKeyX   The X component of P-256 elliptic curve public key.
 * @param   publicKeyY    The Y component of P-256 elliptic curve public key.
 * @return  return `nil` when initialize SigProvisioningPublicKeyPdu object fail.
 */
- (instancetype)initWithPublicKeyX:(NSData *)publicKeyX publicKeyY:(NSData *)publicKeyY {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.provisionType = SigProvisioningPduType_publicKey;
        if (publicKeyX.length == 32 && publicKeyY.length == 32) {
            NSMutableData *mdata = [NSMutableData dataWithData:publicKeyX];
            [mdata appendData:publicKeyY];
            _publicKey = mdata;
            _publicKeyX = publicKeyX;
            _publicKeyY = publicKeyY;
            NSMutableData *mData = [NSMutableData data];
            UInt8 tem8 = self.provisionType;
            NSData *data = [NSData dataWithBytes:&tem8 length:1];
            [mData appendData:data];
            [mData appendData:_publicKey];
            self.pduData = mData;
        } else {
            return nil;
        }
    }
    return self;
}

/**
 * @brief   Initialize SigProvisioningPublicKeyPdu object.
 * @param   parameters    the hex data of SigProvisioningPublicKeyPdu.
 * @return  return `nil` when initialize SigProvisioningPublicKeyPdu object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters && parameters.length == 65) {
            self.pduData = [NSData dataWithData:parameters];
            UInt8 tem8 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem8, dataByte, 1);
            if (tem8 == SigProvisioningPduType_publicKey) {
                self.provisionType = SigProvisioningPduType_publicKey;
                _publicKey = [parameters subdataWithRange:NSMakeRange(1, parameters.length-1)];
                _publicKeyX = [parameters subdataWithRange:NSMakeRange(1, 32)];
                _publicKeyY = [parameters subdataWithRange:NSMakeRange(33, 32)];
                return self;
            } else {
                return nil;
            }
        }else{
            return nil;
        }
    }
    return self;
}

@end


/// The Provisionee sends a Provisioning Input Complete PDU when the user completes the input operation.
/// There are no parameters for the Provisioning Input Complete PDU.
/// 5.4.1.5 Provisioning Input Complete
/// - seeAlso: MshPRFv1.0.1.pdf (page.245)
@implementation SigProvisioningInputCompletePdu

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.provisionType = SigProvisioningPduType_inputComplete;
        NSMutableData *mData = [NSMutableData data];
        UInt8 tem8 = self.provisionType;
        NSData *data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        self.pduData = mData;
    }
    return self;
}

/**
 * @brief   Initialize SigProvisioningInputCompletePdu object.
 * @param   parameters    the hex data of SigProvisioningInputCompletePdu.
 * @return  return `nil` when initialize SigProvisioningInputCompletePdu object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters && parameters.length == 1) {
            self.pduData = [NSData dataWithData:parameters];
            UInt8 tem8 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem8, dataByte, 1);
            if (tem8 == SigProvisioningPduType_inputComplete) {
                self.provisionType = SigProvisioningPduType_inputComplete;
                return self;
            } else {
                return nil;
            }
        }else{
            return nil;
        }
    }
    return self;
}

@end


/// The Provisioner or the Provisionee sends a Provisioning Confirmation PDU to its peer to confirm the values exchanged
/// so far, including the OOB Authentication value and the random number that has yet to be exchanged.
/// 5.4.1.6 Provisioning Confirmation
/// - seeAlso: MshPRT_v1.1.pdf (page.582)
@implementation SigProvisioningConfirmationPdu

/**
 * @brief   Initialize SigProvisioningConfirmationPdu object.
 * @param   confirmation    The values exchanged so far including the OOB Authentication M value.
 * The size is 16 or 32.
 * If the PDU is sent by the Provisioner, the Confirmation field shall contain the ConfirmationProvisioner value.
 * If the PDU is sent by the Provisionee, the Confirmation field shall contain the ConfirmationDevice value.
 * The size of the Confirmation field, the ConfirmationProvisioner value, and the ConfirmationDevice value are
 * defined in Section 5.4.2.4.1.
 * @return  return `nil` when initialize SigProvisioningConfirmationPdu object fail.
 */
- (instancetype)initWithConfirmation:(NSData *)confirmation {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.provisionType = SigProvisioningPduType_confirmation;
        if (confirmation && (confirmation.length == 16 || confirmation.length == 32)) {
            NSMutableData *mData = [NSMutableData data];
            UInt8 tem8 = self.provisionType;
            NSData *data = [NSData dataWithBytes:&tem8 length:1];
            [mData appendData:data];
            [mData appendData:confirmation];
            self.pduData = mData;
        } else {
            return nil;
        }
    }
    return self;
}

/**
 * @brief   Initialize SigProvisioningConfirmationPdu object.
 * @param   parameters    the hex data of SigProvisioningConfirmationPdu.
 * @return  return `nil` when initialize SigProvisioningConfirmationPdu object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters && (parameters.length == 17 || parameters.length == 33)) {
            self.pduData = [NSData dataWithData:parameters];
            UInt8 tem8 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem8, dataByte, 1);
            if (tem8 == SigProvisioningPduType_confirmation) {
                self.provisionType = SigProvisioningPduType_confirmation;
                _confirmation = [parameters subdataWithRange:NSMakeRange(1, parameters.length-1)];
                return self;
            } else {
                return nil;
            }
        }else{
            return nil;
        }
    }
    return self;
}

@end


/// The Provisioner or the Provisionee sends a Provisioning Random PDU to enable its peer device to validate the confirmation.
/// 5.4.1.7 Provisioning Random
/// - seeAlso: MshPRT_v1.1.pdf (page.583)
@implementation SigProvisioningRandomPdu

/**
 * @brief   Initialize SigProvisioningRandomPdu object.
 * @param   random    The final input to the confirmation.
 * If the PDU is sent by the Provisioner, the Random field shall contain the RandomProvisioner value.
 * If the PDU is sent by the Provisionee, the Random field shall contain the RandomDevice value.
 * The size of the Random field, the RandomProvisioner value, and the RandomDevice value are defined in Section 5.4.2.4.1.
 * @return  return `nil` when initialize SigProvisioningRandomPdu object fail.
 */
- (instancetype)initWithRandom:(NSData *)random {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.provisionType = SigProvisioningPduType_random;
        if (random && (random.length == 16 || random.length == 32)) {
            NSMutableData *mData = [NSMutableData data];
            UInt8 tem8 = self.provisionType;
            NSData *data = [NSData dataWithBytes:&tem8 length:1];
            [mData appendData:data];
            [mData appendData:random];
            self.pduData = mData;
        } else {
            return nil;
        }
    }
    return self;
}

/**
 * @brief   Initialize SigProvisioningRandomPdu object.
 * @param   parameters    the hex data of SigProvisioningRandomPdu.
 * @return  return `nil` when initialize SigProvisioningRandomPdu object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters && (parameters.length == 17 || parameters.length == 33)) {
            self.pduData = [NSData dataWithData:parameters];
            UInt8 tem8 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem8, dataByte, 1);
            if (tem8 == SigProvisioningPduType_random) {
                self.provisionType = SigProvisioningPduType_random;
                _random = [parameters subdataWithRange:NSMakeRange(1, parameters.length-1)];
                return self;
            } else {
                return nil;
            }
        }else{
            return nil;
        }
    }
    return self;
}

@end


/// The Provisioner sends a Provisioning Data PDU to deliver provisioning data to a Provisionee.
/// 5.4.1.8 Provisioning Data
/// - seeAlso: MshPRFv1.0.1.pdf (page.246)
@implementation SigProvisioningDataPdu

/**
 * @brief   Initialize SigProvisioningRandomPdu object.
 * @param   encryptedProvisioningData    An encrypted and authenticated network key, NetKey Index,
 * Key Refresh Flag, IV Update Flag, current value of the IV Index, and unicast address of the primary element
 * (see Section 5.4.2.5),the size is 25.
 * @param   provisioningDataMIC    PDU Integrity Check value, size is 8.
 * @return  return `nil` when initialize SigProvisioningRandomPdu object fail.
 */
- (instancetype)initWithEncryptedProvisioningData:(NSData *)encryptedProvisioningData provisioningDataMIC:(NSData *)provisioningDataMIC {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.provisionType = SigProvisioningPduType_data;
        if (encryptedProvisioningData && encryptedProvisioningData.length == 25 && provisioningDataMIC && provisioningDataMIC.length == 8) {
            NSMutableData *mData = [NSMutableData data];
            UInt8 tem8 = self.provisionType;
            NSData *data = [NSData dataWithBytes:&tem8 length:1];
            [mData appendData:data];
            [mData appendData:encryptedProvisioningData];
            [mData appendData:provisioningDataMIC];
            self.pduData = mData;
        } else {
            return nil;
        }
    }
    return self;
}

@end


/// The Provisionee sends a Provisioning Complete PDU to indicate that it has successfully
/// received and processed the provisioning data.
/// There are no parameters for the Provisioning Complete PDU.
/// 5.4.1.9 Provisioning Complete
/// - seeAlso: MshPRT_v1.1.pdf (page.584)
@implementation SigProvisioningCompletePdu

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.provisionType = SigProvisioningPduType_complete;
        NSMutableData *mData = [NSMutableData data];
        UInt8 tem8 = self.provisionType;
        NSData *data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        self.pduData = mData;
    }
    return self;
}

/**
 * @brief   Initialize SigProvisioningCompletePdu object.
 * @param   parameters    the hex data of SigProvisioningCompletePdu.
 * @return  return `nil` when initialize SigProvisioningCompletePdu object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters && parameters.length == 1) {
            self.pduData = [NSData dataWithData:parameters];
            UInt8 tem8 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem8, dataByte, 1);
            if (tem8 == SigProvisioningPduType_complete) {
                self.provisionType = SigProvisioningPduType_complete;
                return self;
            } else {
                return nil;
            }
        }else{
            return nil;
        }
    }
    return self;
}

@end


/// The Provisionee sends a Provisioning Failed PDU if it fails to process a received provisioning protocol PDU.
/// 5.4.1.10 Provisioning Failed
/// - seeAlso: MshPRT_v1.1.pdf (page.584)
@implementation SigProvisioningFailedPdu

/**
 * @brief   Initialize SigProvisioningFailedPdu object.
 * @param   parameters    the hex data of SigProvisioningFailedPdu.
 * @return  return `nil` when initialize SigProvisioningFailedPdu object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters && parameters.length == 2) {
            self.pduData = [NSData dataWithData:parameters];
            UInt8 tem8 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem8, dataByte, 1);
            if (tem8 == SigProvisioningPduType_failed) {
                self.provisionType = SigProvisioningPduType_failed;
                memcpy(&tem8, dataByte+1, 1);
                _errorCode = tem8;
                return self;
            } else {
                return nil;
            }
        }else{
            return nil;
        }
    }
    return self;
}

@end


/// The Provisioner sends a Provisioning Record Request PDU to request a provisioning record fragment
/// (a part of a provisioning record; see Section 5.4.2.6) from the Provisionee.
/// 5.4.1.11 Provisioning Record Request
/// - seeAlso: MshPRT_v1.1.pdf (page.585)
@implementation SigProvisioningRecordRequestPdu

/**
 * @brief   Initialize SigProvisioningRecordRequestPdu object.
 * @param   recordID    Identifies the provisioning record for which the request is made M (see Section 5.4.2.6).
 * A Provisionee may contain multiple provisioning records, and the Record ID field specifies the record that is being requested.
 * @param   fragmentOffset    The starting offset of the requested fragment in the provisioning record data.
 * The Fragment Offset field value indicates the starting offset for the provisioning record data fragment in the Provisioning
 * Record Response PDU that corresponds to the Provisioning Record Request. The Fragment Offset field values are in the
 * range 0, indicating the first octet of the provisioning record data, to the provisioning record data length (in bytes) minus 1,
 * indicating the last octet of the provisioning record data.
 * @param   fragmentMaximumSize    The maximum size of the provisioning record fragment that the Provisioner can receive.
 * The Fragment Maximum Size field specifies the maximum fragment size that the Provisioner can accept. The choice of the value
 * is affected by the provisioning bearer that is being used. A value of 0x0000 is Prohibited.
 * @return  return `nil` when initialize SigProvisioningRecordRequestPdu object fail.
 */
- (instancetype)initWithRecordID:(UInt16)recordID fragmentOffset:(UInt16)fragmentOffset fragmentMaximumSize:(UInt16)fragmentMaximumSize {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.provisionType = SigProvisioningPduType_recordRequest;
        _recordID = recordID;
        _fragmentOffset = fragmentOffset;
        _fragmentMaximumSize = fragmentMaximumSize;
        NSMutableData *mData = [NSMutableData data];
        UInt16 tem16 = 0;
        UInt8 tem8 = self.provisionType;
        NSData *data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem16 = CFSwapInt16BigToHost(recordID);
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        tem16 = CFSwapInt16BigToHost(fragmentOffset);
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        tem16 = CFSwapInt16BigToHost(fragmentMaximumSize);
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        self.pduData = mData;
    }
    return self;
}

@end


/// The Provisionee sends a Provisioning Record Response PDU in response to a Provisioning Record Request PDU.
/// 5.4.1.12 Provisioning Record Response
/// - seeAlso: MshPRT_v1.1.pdf (page.585)
@implementation SigProvisioningRecordResponsePdu

/**
 * @brief   Initialize SigProvisioningRecordResponsePdu object.
 * @param   parameters    the hex data of SigProvisioningRecordResponsePdu.
 * @return  return `nil` when initialize SigProvisioningRecordResponsePdu object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters && parameters.length >= 8) {
            UInt8 tem8 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem8, dataByte, 1);
            if (tem8 == SigProvisioningPduType_recordResponse) {
                self.provisionType = SigProvisioningPduType_recordResponse;
                self.pduData = [NSData dataWithData:parameters];
                memcpy(&tem8, dataByte+1, 1);
                _status = tem8;
                UInt16 tem16 = 0;
                memcpy(&tem16, dataByte+2, 2);
                UInt16 host16 = CFSwapInt16BigToHost(tem16);
                _recordID = host16;
                memcpy(&tem16, dataByte+4, 2);
                host16 = CFSwapInt16BigToHost(tem16);
                _fragmentOffset = host16;
                memcpy(&tem16, dataByte+6, 2);
                host16 = CFSwapInt16BigToHost(tem16);
                _totalLength = host16;
                if (parameters.length > 8) {
                    _data = [parameters subdataWithRange:NSMakeRange(8, parameters.length - 8)];
                }
            } else {
                return nil;
            }
        }else{
            return nil;
        }
    }
    return self;
}

@end


/// The Provisioner sends a Provisioning Records Get PDU to request the list of IDs of the provisioning
/// records that are stored on a Provisionee.
/// There are no parameters for the Provisioning Records Get PDU.
/// 5.4.1.13 Provisioning Records Get
/// - seeAlso: MshPRT_v1.1.pdf (page.587)
@implementation SigProvisioningRecordsGetPdu

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.provisionType = SigProvisioningPduType_recordsGet;
        NSMutableData *mData = [NSMutableData data];
        UInt8 tem8 = self.provisionType;
        NSData *data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        self.pduData = mData;
    }
    return self;
}

@end


/// The Provisionee sends a Provisioning Records List PDU in response to a Provisioning Records Get PDU.
/// 5.4.1.14 Provisioning Records List
/// - seeAlso: MshPRT_v1.1.pdf (page.587)
@implementation SigProvisioningRecordsListPdu

/**
 * @brief   Initialize SigProvisioningRecordsListPdu object.
 * @param   parameters    the hex data of SigProvisioningRecordsListPdu.
 * @return  return `nil` when initialize SigProvisioningRecordsListPdu object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters && parameters.length >= 3) {
            UInt8 tem8 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem8, dataByte, 1);
            if (tem8 == SigProvisioningPduType_recordsList) {
                self.provisionType = SigProvisioningPduType_recordsList;
                self.pduData = [NSData dataWithData:parameters];
                Byte *pduByte = (Byte *)parameters.bytes;
                UInt16 tem16 = 0;
                memcpy(&tem16, pduByte+1, 2);
                _provisioningExtensions = tem16;
                NSMutableArray *mArray = [NSMutableArray array];
                while ((mArray.count + 1) * 2 + 1 + 2 <= parameters.length) {
                    memcpy(&tem16, pduByte+1+2+2*mArray.count, 2);
                    UInt16 host16 = CFSwapInt16BigToHost(tem16);
                    [mArray addObject:@(host16)];
                }
                _recordsList = mArray;
            } else {
                return nil;
            }
        } else {
            return nil;
        }
    }
    return self;
}

@end


@implementation SigNetworkPdu

/**
 * @brief   Creates Network PDU object from received PDU. The initiator tries to deobfuscate and decrypt the data using
 * given Network Key and IV Index.
 * @param   pdu    the hex data of SigProvisioningRecordsListPdu.
 * @param   pduType    The type of the PDU: `SigPduType_proxyConfiguration` or `SigPduType_networkPdu`.
 * @param   networkKey    The Network Key to decrypt the PDU.
 * @param   ivIndex    The ivIndex to decrypt the PDU.
 * @return  return `nil` when initialize SigNetworkPdu object fail.
 */
- (instancetype)initWithDecodePduData:(NSData *)pdu pduType:(SigPduType)pduType usingNetworkKey:(SigNetkeyModel *)networkKey ivIndex:(SigIvIndex *)ivIndex {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (pduType != SigPduType_networkPdu && pduType != SigPduType_proxyConfiguration) {
            TelinkLogError(@"pdutype is not support.");
            return nil;
        }
        self.pduData = pdu;
        if (pdu.length < 14) {
            TelinkLogDebug(@"Valid message must have at least 14 octets.");
            return nil;
        }
        
        // The first byte is not obfuscated.
        UInt8 *byte = (UInt8 *)pdu.bytes;
        UInt8 tem = 0;
        memcpy(&tem, byte, 1);
        _ivi  = tem >> 7;
        _nid  = tem & 0x7F;
        // The NID must match.
        // If the Key Refresh procedure is in place, the received packet might have been encrypted using an old key. We have to try both.
        NSMutableArray <SigNetkeyDerivatives *>*keySets = [NSMutableArray array];
        if (_nid == networkKey.nid) {
            [keySets addObject:networkKey.keys];
            TelinkLogVerbose(@"Decode networkId=0x%@", [LibTools convertDataToHexStr:networkKey.networkId]);
        } else if (_nid == networkKey.directedSecurityNid) {
            networkKey.keys.privacyKey = networkKey.keys.directedSecurityPrivacyKey;
            networkKey.keys.encryptionKey = networkKey.keys.directedSecurityEncryptionKey;
            [keySets addObject:networkKey.keys];
            TelinkLogVerbose(@"Decode networkId=0x%@", [LibTools convertDataToHexStr:networkKey.networkId]);
        }
        if (networkKey.oldKeys != nil && networkKey.oldNid == _nid) {
            [keySets addObject:networkKey.oldKeys];
            TelinkLogVerbose(@"Decode old networkId=0x%@", [LibTools convertDataToHexStr:networkKey.oldNetworkId]);
        } else if (networkKey.oldKeys != nil && networkKey.directedSecurityOldNid == _nid) {
            networkKey.oldKeys.privacyKey = networkKey.oldKeys.directedSecurityPrivacyKey;
            networkKey.oldKeys.encryptionKey = networkKey.oldKeys.directedSecurityEncryptionKey;
            [keySets addObject:networkKey.oldKeys];
            TelinkLogVerbose(@"Decode old networkId=0x%@", [LibTools convertDataToHexStr:networkKey.oldNetworkId]);
        }
        if (keySets.count == 0) {
            return nil;
        }
        
        // IVI should match the LSB bit of current IV Index.
        // If it doesn't, and the IV Update procedure is active, the PDU will be deobfuscated and decoded with IV Index decremented by 1.
        UInt32 index = ivIndex.index;
        if (_ivi != (index & 0x01)) {
            if (index > 0) {
                index -= 1;
            }
        }
        TelinkLogVerbose(@"Decode IvIndex=0x%x", index);
        for (SigNetkeyDerivatives *keys in keySets) {
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
                TelinkLogError(@"decryptedData == nil");
                continue;
            }
            
            _networkKey = networkKey;
            _ctl = type;
            _ttl = ttl;
            _sequence = sequence;
            _source = source;
            UInt8 decryptedData0 = 0,decryptedData1 = 0;
            Byte *decryptedDataByte = (Byte *)decryptedData.bytes;
            memcpy(&decryptedData0, decryptedDataByte+0, 1);
            memcpy(&decryptedData1, decryptedDataByte+1, 1);
            _destination = (UInt16)decryptedData0 << 8 | (UInt16)decryptedData1;
            _transportPdu = [decryptedData subdataWithRange:NSMakeRange(2, decryptedData.length-2)];
            return self;
        }
    }
    return nil;
}

/**
 * @brief   Creates the Network PDU. This method enctypts and obfuscates data that are to be send to the mesh network.
 * @param   lowerTransportPdu    The data received from higher layer.
 * @param   pduType    The type of the PDU: `SigPduType_proxyConfiguration` or `SigPduType_networkPdu`.
 * @param   sequence    The SEQ number of the PDU. Each PDU between the source and destination must have strictly
 * increasing sequence number.
 * @param   ttl    Time To Live.
 * @param   ivIndex    The ivIndex to decrypt the PDU.
 * @return  return `nil` when initialize SigNetworkPdu object fail.
 */
- (instancetype)initWithEncodeLowerTransportPdu:(SigLowerTransportPdu *)lowerTransportPdu pduType:(SigPduType)pduType withSequence:(UInt32)sequence andTtl:(UInt8)ttl ivIndex:(SigIvIndex *)ivIndex {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        UInt32 index = ivIndex.index;

        _networkKey = lowerTransportPdu.networkKey;
        _ivi = (UInt8)(index&0x01);
        if (SigMeshLib.share.dataSource.sendByDirectedSecurity == NO) {
            _nid = _networkKey.nid;
            if (_networkKey.phase == distributingKeys) {
                _nid = _networkKey.oldNid;
            }
        } else {
            _nid = _networkKey.directedSecurityNid;
            if (_networkKey.phase == distributingKeys) {
                _nid = _networkKey.directedSecurityOldNid;
            }
        }
        _ctl = lowerTransportPdu.type;
        _source = lowerTransportPdu.source;
        _destination = lowerTransportPdu.destination;
        _transportPdu = lowerTransportPdu.transportPdu;
        _ttl = ttl;
        _sequence = sequence;

        UInt8 iviNid = (_ivi << 7) | (_nid & 0x7F);
        UInt8 ctlTtl = (_ctl << 7) | (_ttl & 0x7F);

        // Data to be obfuscated: CTL/TTL, Sequence Number, Source Address.
        UInt32 bigSequence = CFSwapInt32HostToBig(sequence);
        UInt16 bigSource = CFSwapInt16HostToBig(_source);
        UInt16 bigDestination = CFSwapInt16HostToBig(_destination);
        UInt32 bigIndex = CFSwapInt32HostToBig(index);

        NSData *seq = [[NSData dataWithBytes:&bigSequence length:4] subdataWithRange:NSMakeRange(1, 3)];
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
        SigNetkeyDerivatives *keys = _networkKey.transmitKeys;
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

        NSData *encryptedData = nil;
        NSData *obfuscatedData = nil;
        if (SigMeshLib.share.dataSource.sendByDirectedSecurity == NO) {
            encryptedData = [OpenSSLHelper.share calculateCCM:decryptedData withKey:keys.encryptionKey nonce:networkNonce andMICSize:[self getNetMicSizeOfLowerTransportPduType:_ctl] withAdditionalData:nil];
            obfuscatedData = [OpenSSLHelper.share obfuscate:deobfuscatedData usingPrivacyRandom:encryptedData ivIndex:index andPrivacyKey:keys.privacyKey];
        } else {
            encryptedData = [OpenSSLHelper.share calculateCCM:decryptedData withKey:keys.directedSecurityEncryptionKey nonce:networkNonce andMICSize:[self getNetMicSizeOfLowerTransportPduType:_ctl] withAdditionalData:nil];
            obfuscatedData = [OpenSSLHelper.share obfuscate:deobfuscatedData usingPrivacyRandom:encryptedData ivIndex:index andPrivacyKey:keys.directedSecurityPrivacyKey];
        }
        
        NSMutableData *pduData = [NSMutableData dataWithBytes:&iviNid length:1];
        [pduData appendData:obfuscatedData];
        [pduData appendData:encryptedData];
        self.pduData = pduData;
    }
    return self;
}

/**
 * @brief   This method goes over all Network Keys in the mesh network and tries to deobfuscate and decode the network PDU.
 * @param   pdu    The received PDU.
 * @param   pduType    The type of the PDU: `SigPduType_proxyConfiguration` or `SigPduType_networkPdu`.
 * @param   meshDataSource    The mesh network for which the PDU should be decoded.
 * @return  return `nil` when initialize SigNetworkPdu object fail.
 */
+ (instancetype)decodePdu:(NSData *)pdu pduType:(SigPduType)pduType meshDataSource:(SigDataSource *)meshDataSource {
    NSArray *netKeys = [NSArray arrayWithArray:meshDataSource.netKeys];
    for (SigNetkeyModel *networkKey in netKeys) {
        SigNetworkPdu *networkPdu = [[SigNetworkPdu alloc] initWithDecodePduData:pdu pduType:pduType usingNetworkKey:networkKey ivIndex:networkKey.ivIndex];
        if (networkPdu) {
            return networkPdu;
        }
    }
    return nil;
}

/// Whether the Network PDU contains a segmented Lower Transport PDU, or not.
- (BOOL)isSegmented {
    UInt8 tem = 0;
    Byte *byte = (Byte *)_transportPdu.bytes;
    memcpy(&tem, byte, 1);
    return (tem&0x80)>1;
}

/// The 24-bit Seq Auth used to transmit the first segment of a segmented message,
/// or the 24-bit sequence number of an unsegmented
/// message.
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

/// Get UInt32 decode ivIndex value.
- (UInt32)getDecodeIvIndex {
    UInt32 index = _networkKey.ivIndex.index;
    if (_ivi != (index & 0x01)) {
        if (index > 0) {
            index -= 1;
        }
    }
    return index;
}

- (UInt8)getNetMicSizeOfLowerTransportPduType:(SigLowerTransportPduType)pduType {
    UInt8 tem = 4;
    if (pduType == SigLowerTransportPduType_accessMessage) {
        tem = 4;// 32 bits
    }else if (pduType == SigLowerTransportPduType_transportControlMessage) {
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
            TelinkLogError(@"Unsupported PDU Type:%lu",(unsigned long)pduType);
            break;
    }
    return 0;
}

- (NSString *)description {
    int micSize = [self getNetMicSizeOfLowerTransportPduType:_ctl];
    NSInteger encryptedDataSize = self.pduData.length - micSize - 9;
    NSData *encryptedData = [self.pduData subdataWithRange:NSMakeRange(9, encryptedDataSize)];
    NSData *mic = [self.pduData subdataWithRange:NSMakeRange(9+encryptedDataSize,self.pduData.length - 9- encryptedDataSize)];
    return[NSString stringWithFormat:@"Network PDU (ivi: 0x%x, nid: 0x%x, ctl: 0x%x, ttl: 0x%x, seq: 0x%x, src: 0x%x, dst: 0x%x, transportPdu: %@, netMic: %@)",_ivi,_nid,_ctl,_ttl,(unsigned int)_sequence,_source,_destination,encryptedData,mic];
}

@end


@implementation SigBeaconPdu

- (void)setBeaconType:(SigBeaconType)beaconType {
    _beaconType = beaconType;
}

@end

/// The Secure Network beacon is used by nodes to identify the subnet and its security state.
/// 3.10.3 Secure Network beacon
/// - seeAlso: MshPRT_v1.1.pdf (page.214)
@implementation SigSecureNetworkBeacon

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
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
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        [super setBeaconType:SigBeaconType_secureNetwork];
        self.pduData = pdu;
        UInt8 tem = 0;
        Byte *pduByte = (Byte *)pdu.bytes;
        memcpy(&tem, pduByte, 1);
        if (pdu.length != 22 || tem != 1) {
            TelinkLogError(@"pdu data error, can not init decode.");
            return nil;
        }
        memcpy(&tem, pduByte+1, 1);
        _keyRefreshFlag = (tem & 0x01) != 0;
        _ivUpdateActive = (tem & 0x02) != 0;
        _networkId = [pdu subdataWithRange:NSMakeRange(2, 8)];
        UInt32 tem10 = 0;
        memcpy(&tem10, pduByte + 10, 4);
        _ivIndex = CFSwapInt32HostToBig(tem10);
        // Authenticate beacon using given Network Key.
        if ([_networkId isEqualToData:networkKey.networkId]) {
            NSData *authenticationValue = [OpenSSLHelper.share calculateCMAC:[pdu subdataWithRange:NSMakeRange(1, 13)] andKey:networkKey.keys.beaconKey];
            if (![[authenticationValue subdataWithRange:NSMakeRange(0, 8)] isEqualToData:[pdu subdataWithRange:NSMakeRange(14, 8)]]) {
                TelinkLogError(@"authenticationValue is not current networkID.");
                return nil;
            }
            _networkKey = networkKey;
        }else if (networkKey.oldNetworkId != nil && [networkKey.oldNetworkId isEqualToData:_networkId]) {
            NSData *authenticationValue = [OpenSSLHelper.share calculateCMAC:[pdu subdataWithRange:NSMakeRange(1, 13)] andKey:networkKey.oldKeys.beaconKey];
            if (![[authenticationValue subdataWithRange:NSMakeRange(0, 8)] isEqualToData:[pdu subdataWithRange:NSMakeRange(14, 8)]]) {
                TelinkLogError(@"authenticationValue is not current old networkID.");
                return nil;
            }
            _networkKey = networkKey;
        }else{
            return nil;
        }
    }
    return self;
}

/**
 * @brief   Creates USecure Network beacon PDU object from received PDU.
 * @param   pdu    The data received from mesh network.
 * @param   meshDataSource    The mesh network for which the PDU should be decoded.
 * @return  return `nil` when initialize SigSecureNetworkBeacon object fail.
 */
+ (instancetype)decodePdu:(NSData *)pdu meshDataSource:(SigDataSource *)meshDataSource {
    if (pdu == nil || pdu.length <= 1) {
        TelinkLogError(@"decodePdu length is less than 1.");
        return nil;
    }
    UInt8 tem = 0;
    Byte *pduByte = (Byte *)pdu.bytes;
    memcpy(&tem, pduByte, 1);
    SigBeaconType beaconType = tem;
    if (beaconType == SigBeaconType_secureNetwork) {
        NSArray *netKeys = [NSArray arrayWithArray:meshDataSource.netKeys];
        for (SigNetkeyModel *networkKey in netKeys) {
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

/**
 * @brief   Initialize SigSecureNetworkBeacon object.
 * @param   keyRefreshFlag    Key Refresh flag value.
 * @param   ivUpdateActive    This flag is set to `true` if IV Update procedure is active.
 * @param   networkId    Contains the value of the Network ID.
 * @param   ivIndex    Contains the current IV Index.
 * @param   networkKey    The Network Key related to this Secure Network Beacon.
 * @return  return `nil` when initialize SigSecureNetworkBeacon object fail.
 */
- (instancetype)initWithKeyRefreshFlag:(BOOL)keyRefreshFlag ivUpdateActive:(BOOL)ivUpdateActive networkId:(NSData *)networkId ivIndex:(UInt32)ivIndex usingNetworkKey:(SigNetkeyModel *)networkKey {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        [super setBeaconType:SigBeaconType_secureNetwork];
        _keyRefreshFlag = keyRefreshFlag;
        _ivUpdateActive = ivUpdateActive;
        _networkId = networkId;
        _ivIndex = ivIndex;
        _networkKey = networkKey;
    }
    return self;
}

/// Get dictionary from SigSecureNetworkBeacon object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSecureNetworkBeacon {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (_networkKey && _networkKey.key) {
        dict[@"networkKey"] = _networkKey.key;
    }
    dict[@"keyRefreshFlag"] = [NSNumber numberWithBool:_keyRefreshFlag];
    dict[@"ivUpdateActive"] = [NSNumber numberWithBool:_ivUpdateActive];
    if (_networkId) {
        dict[@"networkId"] = _networkId;
    }
    dict[@"ivIndex"] = [NSNumber numberWithInt:_ivIndex];
    return dict;
}

/// Set dictionary to SigSecureNetworkBeacon object.
/// @param dictionary SigSecureNetworkBeacon dictionary object.
- (void)setDictionaryToSecureNetworkBeacon:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"networkKey"]) {
        for (SigNetkeyModel *model in SigMeshLib.share.dataSource.netKeys) {
            if ([model.key isEqualToString:dictionary[@"networkKey"]]) {
                _networkKey = model;
                break;
            }
        }
    }
    if ([allKeys containsObject:@"keyRefreshFlag"]) {
        _keyRefreshFlag = [dictionary[@"keyRefreshFlag"] boolValue];
    }
    if ([allKeys containsObject:@"ivUpdateActive"]) {
        _ivUpdateActive = [dictionary[@"ivUpdateActive"] boolValue];
    }
    if ([allKeys containsObject:@"networkId"]) {
        _networkId = dictionary[@"networkId"];
    }
    if ([allKeys containsObject:@"ivIndex"]) {
        _ivIndex = [dictionary[@"ivIndex"] intValue];
    }
}

- (NSData *)pduData {
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
    UInt32 ivIndex32 = CFSwapInt32HostToBig(_ivIndex);
    [mData appendData:[NSData dataWithBytes:&ivIndex32 length:4]];
    NSData *authenticationValue = nil;
    if ([_networkId isEqualToData:_networkKey.networkId]) {
        authenticationValue = [OpenSSLHelper.share calculateCMAC:mData andKey:_networkKey.keys.beaconKey];
    }else if (_networkKey.oldNetworkId != nil && [_networkKey.oldNetworkId isEqualToData:_networkId]) {
        authenticationValue = [OpenSSLHelper.share calculateCMAC:mData andKey:_networkKey.oldKeys.beaconKey];
    }
    if (authenticationValue) {
        [mData appendData:[authenticationValue subdataWithRange:NSMakeRange(0, 8)]];
        UInt8 bType = self.beaconType;
        NSMutableData *allData = [NSMutableData data];
        [allData appendData:[NSData dataWithBytes:&bType length:1]];
        [allData appendData:mData];
        return allData;
    } else {
        return nil;
    }
}

- (NSString *)description {
    return[NSString stringWithFormat:@"<%p> - Secure Network Beacon, network ID:(%@), ivIndex: (%x) Key refresh Flag: (%d), IV Update active: (%d)", self, _networkId,(unsigned int)_ivIndex, _keyRefreshFlag,_ivUpdateActive];
}

@end


/// The Unprovisioned Device beacon is used by devices that are unprovisioned
/// to allow them to be discovered by a Provisioner.
/// 3.10.2 Unprovisioned Device beacon
/// - seeAlso: MshPRT_v1.1.pdf (page.212)
@implementation SigUnprovisionedDeviceBeacon

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        [super setBeaconType:SigBeaconType_unprovisionedDevice];
    }
    return self;
}

/// Creates Unprovisioned Device beacon PDU object from received PDU.
///
/// - parameter pdu: The data received from mesh network.
/// - returns: The beacon object, or `nil` if the data are invalid.
- (instancetype)initWithDecodePdu:(NSData *)pdu {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
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

- (NSString *)description {
    return[NSString stringWithFormat:@"<%p> - Unprovisioned Device Beacon, uuid:(%@), OOB Info: (%x) URI hash: (%@)", self, _deviceUuid,_oob.value, _uriHash];
}

@end


/// The Mesh Private beacon is used by the nodes to identify the Key Refresh Flag (see Table 3.80),
/// IV Update Flag (see Table 3.80), and IV Index (see Section 3.9.4) of the subnet.
/// 3.10.4 Mesh Private beacon
/// - seeAlso: MshPRT_v1.1.pdf (page.216)
@implementation SigMeshPrivateBeacon

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        [super setBeaconType:SigBeaconType_meshPrivateBeacon];
    }
    return self;
}

/// Creates Mesh Private beacon PDU object from received PDU.
///
/// - parameter pdu: The data received from mesh network.
/// - parameter networkKey: The Network Key to validate the beacon.
/// - returns: The beacon object, or `nil` if the data are invalid.
- (instancetype)initWithDecodePdu:(NSData *)pdu usingNetworkKey:(SigNetkeyModel *)networkKey {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        [super setBeaconType:SigBeaconType_meshPrivateBeacon];
        self.pduData = pdu;
        UInt8 tem = 0;
        Byte *pduByte = (Byte *)pdu.bytes;
        memcpy(&tem, pduByte, 1);
        if (pdu.length != 27 || tem != SigBeaconType_meshPrivateBeacon) {
            TelinkLogError(@"pdu data error, can not init decode.");
            return nil;
        }
        _randomData = [pdu subdataWithRange:NSMakeRange(1, 13)];
        _obfuscatedPrivateBeaconData = [pdu subdataWithRange:NSMakeRange(14, 5)];
        _authenticationTag = [pdu subdataWithRange:NSMakeRange(19, 8)];
        
        BOOL authentication = NO;
        NSMutableArray *mArray = [NSMutableArray array];
        if (networkKey.key && networkKey.key.length == 32) {
            [mArray addObject:[LibTools nsstringToHex:networkKey.key]];
        }
        if (networkKey.oldKey && networkKey.oldKey.length == 32) {
            [mArray addObject:[LibTools nsstringToHex:networkKey.oldKey]];
        }
        for (NSData *key in mArray) {
            NSData *obfuscatedPrivateBeaconDataC = [OpenSSLHelper.share calculateObfuscatedPrivateBeaconDataWithKeyRefreshFlag:networkKey.phase == distributingKeys ivUpdateActive:networkKey.ivIndex.updateActive ivIndex:networkKey.ivIndex.index randomData:_randomData usingNetworkKey:key];
            if ([obfuscatedPrivateBeaconDataC isEqualToData:_obfuscatedPrivateBeaconData]) {
                NSData *authenticationTagC = [OpenSSLHelper.share calculateAuthenticationTagWithKeyRefreshFlag:networkKey.phase == distributingKeys ivUpdateActive:networkKey.ivIndex.updateActive ivIndex:networkKey.ivIndex.index randomData:_randomData usingNetworkKey:key];
                if ([authenticationTagC isEqualToData:_authenticationTag]) {
                    authentication = YES;
                    _netKeyData = key;
                    _networkKey = networkKey;
                    NSData *privateBeaconData = [OpenSSLHelper.share calculatePrivateBeaconDataWithObfuscatedPrivateBeaconData:obfuscatedPrivateBeaconDataC randomData:_randomData usingNetworkKey:key];
                    Byte *privateBeaconByte = (Byte *)privateBeaconData.bytes;
                    memcpy(&tem, privateBeaconByte, 1);
                    _keyRefreshFlag = (tem & 0x01) != 0;
                    _ivUpdateActive = (tem & 0x02) != 0;
                    UInt32 tem32 = 0;
                    memcpy(&tem32, privateBeaconByte + 1, 4);
                    _ivIndex = CFSwapInt32HostToBig(tem32);
                    break;
                }
            }
        }
        if (authentication == NO) {
            TelinkLogError(@"Mesh Private beacon authentication fail.");
            return nil;
        }
    }
    return self;
}

/**
 * @brief   Creates Mesh Private beacon PDU object from received PDU.
 * @param   pdu    The data received from mesh network.
 * @param   meshDataSource    The mesh network for which the PDU should be decoded.
 * @return  return `nil` when initialize SigMeshPrivateBeacon object fail.
 */
+ (instancetype)decodePdu:(NSData *)pdu meshDataSource:(SigDataSource *)meshDataSource {
    if (pdu == nil || pdu.length <= 1) {
        TelinkLogError(@"decodePdu length is less than 1.");
        return nil;
    }
    UInt8 tem = 0;
    Byte *pduByte = (Byte *)pdu.bytes;
    memcpy(&tem, pduByte, 1);
    SigBeaconType beaconType = tem;
    if (beaconType == SigBeaconType_meshPrivateBeacon) {
        NSArray *netKeys = [NSArray arrayWithArray:meshDataSource.netKeys];
        for (SigNetkeyModel *networkKey in netKeys) {
            SigMeshPrivateBeacon *beacon = [[SigMeshPrivateBeacon alloc] initWithDecodePdu:pdu usingNetworkKey:networkKey];
            if (beacon) {
                return beacon;
            }
        }
    } else {
        return nil;
    }
    return nil;
}

/**
 * @brief   Initialize SigMeshPrivateBeacon object.
 * @param   keyRefreshFlag    Key Refresh flag value.
 * @param   ivUpdateActive    This flag is set to `true` if IV Update procedure is active.
 * @param   ivIndex    Contains the current IV Index.
 * @param   randomData    Random number used as an entropy for obfuscation and
 * authentication of the Mesh Private beacon. The size is 13 bytes.
 * @param   networkKey    The Network Key related to this Secure Network Beacon.
 * @return  return `nil` when initialize SigMeshPrivateBeacon object fail.
 */
- (instancetype)initWithKeyRefreshFlag:(BOOL)keyRefreshFlag ivUpdateActive:(BOOL)ivUpdateActive ivIndex:(UInt32)ivIndex randomData:(NSData *)randomData usingNetworkKey:(SigNetkeyModel *)networkKey {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        [super setBeaconType:SigBeaconType_meshPrivateBeacon];
        _keyRefreshFlag = keyRefreshFlag;
        _ivUpdateActive = ivUpdateActive;
        _randomData = [NSData dataWithData:randomData];
        _ivIndex = ivIndex;
        _networkKey = networkKey;
        _netKeyData = [LibTools nsstringToHex:networkKey.key];
    }
    return self;
}

- (NSData *)pduData {
    NSMutableData *mData = [NSMutableData data];
    UInt8 bType = self.beaconType;
    [mData appendData:[NSData dataWithBytes:&bType length:1]];
    if (_randomData == nil || _randomData.length == 0) {
        return nil;
    }
    [mData appendData:_randomData];
    NSData *obfuscatedPrivateBeaconDataC = self.obfuscatedPrivateBeaconData;
    if (obfuscatedPrivateBeaconDataC == nil || obfuscatedPrivateBeaconDataC.length == 0) {
        return nil;
    }
    [mData appendData:obfuscatedPrivateBeaconDataC];
    NSData *authenticationTagC = self.authenticationTag;
    if (authenticationTagC == nil || authenticationTagC.length == 0) {
        return nil;
    }
    [mData appendData:authenticationTagC];
    return mData;
}

- (NSData *)obfuscatedPrivateBeaconData {
    _obfuscatedPrivateBeaconData = [OpenSSLHelper.share calculateObfuscatedPrivateBeaconDataWithKeyRefreshFlag:_keyRefreshFlag ivUpdateActive:_ivUpdateActive ivIndex:_ivIndex randomData:_randomData usingNetworkKey:self.netKeyData];
    return _obfuscatedPrivateBeaconData;
}

- (NSData *)authenticationTag {
    _authenticationTag = [OpenSSLHelper.share calculateAuthenticationTagWithKeyRefreshFlag:_keyRefreshFlag ivUpdateActive:_ivUpdateActive ivIndex:_ivIndex randomData:_randomData usingNetworkKey:self.netKeyData];
    return _authenticationTag;
}

- (NSString *)description {
    return[NSString stringWithFormat:@"<%p> - Mesh Private Beacon, random:(%@), netKeyData:(%@), ivIndex: (%x) Key refresh Flag: (%d), IV Update active: (%d)", self, [LibTools convertDataToHexStr:_randomData],[LibTools convertDataToHexStr:_netKeyData],(unsigned int)_ivIndex, _keyRefreshFlag,_ivUpdateActive];
}

@end


@implementation PublicKey

- (instancetype)initWithPublicKeyType:(PublicKeyType)type {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        UInt8 tem = type;
        _publicKeyType = type;
        _PublicKeyData = [NSData dataWithBytes:&tem length:1];
    }
    return self;
}

@end
