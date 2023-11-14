/********************************************************************************************************
 * @file     SigPdu.h
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

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class SigNetkeyModel,SigDataSource,SigLowerTransportPdu,SigIvIndex;

@interface SigPdu : NSObject
/// Payload data.
@property (nonatomic, strong, nullable) NSData *pduData;
@end


@interface SigProvisioningPdu : SigPdu
@property (nonatomic, assign) SigProvisioningPduType provisionType;

/// Get Provisioning Pdu Class
/// - Parameter provisioningPduType: The SigProvisioningPduType value.
+ (Class)getProvisioningPduClassWithProvisioningPduType:(SigProvisioningPduType)provisioningPduType;

@end


/// A Provisioner sends a Provisioning Invite PDU to indicate to the intended Provisionee that the provisioning
/// process is starting.
/// 5.4.1.1 Provisioning Invite
/// - seeAlso: MshPRT_v1.1.pdf (page.577)
@interface SigProvisioningInvitePdu : SigProvisioningPdu
/// Attention Timer state (See Section 4.2.9)
/// The Attention Timer state determines if the Attention Timer state is on or off. This is generally intended to allow
/// an element to attract human attention and, among others, is used during provisioning (see Section 5.4.1.11).
/// An element may not support the Attention Timer. If an element does not support the Attention Timer, the Attention
/// Timer state shall always be set to zero.
/// If the Attention Timer is non-zero for an element, Attention Timer state is on. If the Attention Timer is zero for an
/// element, the Attention Timer state is off.
/// When the Attention Timer state is on, the value determines how long the element shall remain attracting human’s
/// attention. The element does that by behaving in a human-recognizable way (e.g., a lamp flashes, a motor makes
/// noise, an LED blinks). The exact behavior is implementation specific and depends on the type of device. Normal
/// behavior of the element is still active, although the method of identification may override the physical state of the
/// device.
/// The Attention Timer is a momentary state, active for a time indicated by its value, in seconds. The value is
/// decremented every second by 1 until it reaches zero. The values for this state are defined in Table 4.25.
/// Value           Description
/// 0x00            Off
/// 0x01–0xFF  On, remaining time in seconds
/// 4.2.10 Attention Timer
/// - seeAlso: MshPRT_v1.1.pdf (page.255)
@property (nonatomic, assign) UInt8 attentionDuration;

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
- (instancetype)initWithAttentionDuration:(UInt8)attentionDuration;

@end

/// The Provisionee sends a Provisioning Capabilities PDU to indicate its supported provisioning capabilities to a Provisioner.
/// 5.4.1.2 Provisioning Capabilities
/// - seeAlso: MshPRT_v1.1.pdf (page.577)
@interface SigProvisioningCapabilitiesPdu : SigProvisioningPdu
/// Number of elements supported by the Provisionee (see Table 5.20)
/// The Number of Elements values are defined in Table 5.20.
/// Value               Description
/// 0x00                Prohibited
/// 0x01–0xFF      Number of elements supported by the Provisionee
@property (nonatomic, assign) UInt8 numberOfElements;
/// Supported algorithms and other capabilities (see Table 5.21)
/// The Algorithms values are defined in Table 5.21.
/// Bit     Name
/// 0       BTM_ECDH_P256_CMAC_AES128_AES_CCM
/// 1       BTM_ECDH_P256_HMAC_SHA256_AES_CCM
/// 2–15 Reserved for Future Use
/// The BTM_ECDH_P256_HMAC_SHA256_AES_CCM bit of the Algorithms field shall be set to 1 by a Provisionee.
/// Other sections of this specification may introduce further requirements for the value of this field.
@property (nonatomic, assign) struct Algorithms algorithms;
/// Supported public key types (see Table 5.22)
/// The Public Key Type values are defined in Table 5.22.
/// Bit         Description
/// 0           Public Key OOB information available
/// 1–7       Reserved for Future Use
/// The size of the Public Key OOB information is determined by the selected Algorithm.
@property (nonatomic, assign) PublicKeyType publicKeyType;
/// Supported OOB Types (see Table 5.23)
/// The OOB Type values are defined in Table 5.23.
/// Bit         Description
/// 0           Static OOB information available
/// 1           Only OOB authenticated provisioning supported
/// 2–7       Reserved for Future Use
/// If bit 1 of the OOB Type field is set to 1, bit 0 of the Algorithms field shall be set to 0 and at least one of the conditions
/// listed below shall be met:
/// • Bit0oftheOOBTypefieldissetto1.
/// • The Output OOB Size field is not equal to 0x00.
/// • The Input OOB Size field is not equal to 0x00.
/// The maximum size of the Static OOB information is 32 octets.
/// The data type for the Static OOB information shall be Binary, Numeric, or Alphanumeric.
@property (nonatomic, assign) struct StaticOobType staticOobType;
/// Maximum size of Output OOB supported.
/// Maximum size of Output OOB supported (see Table 5.24)
/// The Output OOB Size defines the number of digits that can be output (e.g., displayed or spoken) when the value Output
/// Numeric is set and Output Alphanumeric is not set in the Output OOB Action field (see Table 5.25). The Output OOB Size
/// defines the number of digits and uppercase letters that can be output when the value Output Numeric is not set and Output
/// Alphanumeric is set in the Output OOB Action field. The Output OOB Size defines the number of digits and uppercase letters
/// that can be output when the value Output Numeric is set and Output Alphanumeric is set in the Output OOB Action field.
/// The Output OOB Size values are defined in Table 5.24.
/// Value           Description
/// 0x00            The Provisionee does not support Output OOB
/// 0x01–0x08   Maximum size of Output OOB supported by the Provisionee
/// 0x09–0xFF   Reserved for Future Use
@property (nonatomic, assign) UInt8 outputOobSize;
/// Supported Output OOB Actions (see Table 5.25)
/// The Output OOB Action values are defined in Table 5.25.
/// Bit             Description                         Data Type
/// 0               Blink                                   Numeric
/// 1               Beep                                   Numeric
/// 2               Vibrate                                Numeric
/// 3               Output Numeric                  Numeric
/// 4               Output Alphanumeric         Alphanumeric
/// 5–15         Reserved for Future Use    n/a
@property (nonatomic, assign) struct OutputOobActions outputOobActions;
/// Maximum size in octets of Input OOB supported (see Table 5.26)
/// The Input OOB Size defines the number of digits that can be entered when the value Input Numeric is set and Input Alphanumeric
/// is not set in the Input OOB Action field (see Table 5.27). The Input OOB Size defines the number of digits and uppercase letters that
/// can be entered when the value Input Numeric is not set and Input Alphanumeric is set in the Input OOB Action field. The Input OOB
/// Size defines the number of digits and uppercase letters that can be entered when the value Input Numeric is set and Input
/// Alphanumeric is set in the Input OOB Action field.
/// The Input OOB Size values are defined in Table 5.26.
/// Value                   Description
/// 0x00                    The Provisionee does not support Input OOB
/// 0x01–0x08           Maximum size of Input OOB supported by the Provisionee
/// 0x09–0xFF           Reserved for Future Use
@property (nonatomic, assign) UInt8 inputOobSize;
/// Supported Input OOB Actions (see Table 5.27)
/// The Input OOB Actions are defined in Table 5.27.
/// Bit                 Description                         Data Type
/// 0                   Push                                    Numeric
/// 1                   Twist                                    Numeric
/// 2                   Input Numeric                      Numeric
/// 3                   Input Alphanumeric             Alphanumeric
/// 4–15             Reserved for Future Use      n/a
@property (nonatomic, assign) struct InputOobActions inputOobActions;

/**
 * @brief   Initialize SigProvisioningCapabilitiesPdu object.
 * @param   parameters    the hex data of SigProvisioningCapabilitiesPdu.
 * @return  return `nil` when initialize SigProvisioningCapabilitiesPdu object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// Get Description String of SigProvisioningCapabilitiesPdu object.
- (NSString *)getCapabilitiesString;

@end


/// A Provisioner sends a Provisioning Start PDU to indicate the method it has selected
/// from the options in the Provisioning Capabilities PDU.
/// 5.4.1.3 Provisioning Start
/// - seeAlso: MshPRT_v1.1.pdf (page.580)
@interface SigProvisioningStartPdu : SigProvisioningPdu
/// The algorithm used for provisioning (see Table 5.29)
/// The Algorithm values are defined in Table 5.29.
/// Value                   Description
/// 0x00                    BTM_ECDH_P256_CMAC_AES128_AES_CCM
/// 0x01                    BTM_ECDH_P256_HMAC_SHA256_AES_CCM
/// 0x02–0xFF          Reserved for Future Use
@property (nonatomic, assign) Algorithm algorithm;
/// Public Key used (see Table 5.30)
/// The Public Key values are defined in Table 5.30.
/// Value                   Description
/// 0x00                    No OOB Public Key is used
/// 0x01                    OOB Public Key is used
/// 0x02–0xFF           Reserved for Future Use
@property (nonatomic, assign) PublicKeyType publicKeyType;
/// Authentication Method used (see Table 5.31)
/// The Authentication Method field values are defined in Table 5.31.
/// Value                   Name                                              Description
/// 0x00                    Authentication with No OOB          No OOB authentication is used
/// 0x01                    Authentication with Static OOB     Static OOB authentication is used
/// 0x02                    Authentication with Output OOB   Output OOB authentication is used
/// 0x03                    Authentication with Input OOB      Input OOB authentication is used
/// 0x04–0xFF          Prohibited                                       Prohibited
/// When the Authentication Method field value is Authentication with No OOB, the Authentication Action field
/// shall be set to 0x00 and the Authentication Size field shall be set to 0x00.
/// When the Authentication Method field value is Authentication with Static OOB, the Authentication Size shall
/// be set to 0x00 and the Authentication Action field shall be set to 0x00. The data type shall be transferred
/// using an OOB technology.
/// When the Authentication Method field value is Authentication with Output OOB, the values defined in
/// Table 5.32 and Table 5.33 shall be used to determine the data type, the Authentication Action, and the
/// Authentication Size.
@property (nonatomic, assign) AuthenticationMethod authenticationMethod;
/// Selected Output OOB Action (see Table 5.32) or Input M OOB Action (see Table 5.34) or 0x00
/// The Output OOB Action values for the Authentication Action field are defined in Table 5.32.
/// Value              Description                         Data Type
/// 0x00               Blink                                   Numeric
/// 0x01               Beep                                   Numeric
/// 0x02               Vibrate                                Numeric
/// 0x03               Output Numeric                  Numeric
/// 0x04               Output Alphanumeric         Alphanumeric
/// 0x05–0x0F     Reserved for Future Use    n/a
/// When the Authentication Method field value is Authentication with Input OOB, the values defined in Table 5.34 and
/// Table 5.35 shall be used to determine the data type, the Authentication Action and the Authentication Size.
/// The Input OOB Action values for the Authentication Action field are defined in Table 5.34.
/// Value                  Description                         Data Type
/// 0x00                   Push                                    Numeric
/// 0x01                   Twist                                    Numeric
/// 0x02                   Input Numeric                      Numeric
/// 0x03                   Input Alphanumeric             Alphanumeric
/// 0x04–0x0F         Reserved for Future Use      n/a
@property (nonatomic, assign) UInt8 authenticationAction;
/// Size of the Output OOB used (see Table 5.33) or size M of the Input OOB used (see Table 5.35) or 0x00
/// The Output OOB Size for the Authentication Size field values are defined in Table 5.33.
/// Value               Description
/// 0x00                Prohibited
/// 0x01–0x08      The Output OOB Size to be used
/// 0x09–0xFF      Reserved for Future Use
/// When the Authentication Method field value is Authentication with Input OOB, the values defined in Table 5.34 and
/// Table 5.35 shall be used to determine the data type, the Authentication Action and the Authentication Size.
/// The Input OOB Size values for the Authentication Size field are defined in Table 5.35.
/// Value           Description
/// 0x00            Prohibited
/// 0x01–0x08   The Input OOB size to be used
/// 0x09–0xFF   Reserved for Future Use
@property (nonatomic, assign) UInt8 authenticationSize;

/**
 * @brief   Initialize SigProvisioningStartPdu object.
 * @param   algorithm    The algorithm used for provisioning (see Table 5.29).
 * @param   publicKeyType    Public Key used (see Table 5.30).
 * @param   authenticationMethod    Authentication Method used (see Table 5.31).
 * @param   authenticationAction    Selected Output OOB Action (see Table 5.32) or Input M OOB Action (see Table 5.34) or 0x00.
 * @param   authenticationSize    Size of the Output OOB used (see Table 5.33) or size M of the Input OOB used (see Table 5.35) or 0x00.
 * @return  return `nil` when initialize SigProvisioningStartPdu object fail.
 */
- (instancetype)initWithAlgorithm:(Algorithm)algorithm publicKeyType:(PublicKeyType)publicKeyType authenticationMethod:(AuthenticationMethod)authenticationMethod authenticationAction:(UInt8)authenticationAction authenticationSize:(UInt8)authenticationSize;

@end


/// The Provisioner sends a Provisioning Public Key PDU to deliver the public key to be used in the ECDH calculations.
/// 5.4.1.4 Provisioning Public Key
/// - seeAlso: MshPRT_v1.1.pdf (page.582)
@interface SigProvisioningPublicKeyPdu : SigProvisioningPdu
/// The X component of P-256 elliptic curve public key.
@property (nonatomic, strong) NSData *publicKeyX;
/// The Y component of P-256 elliptic curve public key.
@property (nonatomic, strong) NSData *publicKeyY;
/// The public key for the FIPS P-256 algorithm.
@property (nonatomic, strong) NSData *publicKey;

/**
 * @brief   Initialize SigProvisioningPublicKeyPdu object.
 * @param   publicKey    The public key for the FIPS P-256 algorithm.
 * @return  return `nil` when initialize SigProvisioningPublicKeyPdu object fail.
 */
- (instancetype)initWithPublicKey:(NSData *)publicKey;

/**
 * @brief   Initialize SigProvisioningPublicKeyPdu object.
 * @param   publicKeyX   The X component of P-256 elliptic curve public key.
 * @param   publicKeyY    The Y component of P-256 elliptic curve public key.
 * @return  return `nil` when initialize SigProvisioningPublicKeyPdu object fail.
 */
- (instancetype)initWithPublicKeyX:(NSData *)publicKeyX publicKeyY:(NSData *)publicKeyY;

/**
 * @brief   Initialize SigProvisioningPublicKeyPdu object.
 * @param   parameters    the hex data of SigProvisioningPublicKeyPdu.
 * @return  return `nil` when initialize SigProvisioningPublicKeyPdu object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


/// The Provisionee sends a Provisioning Input Complete PDU when the user completes the input operation.
/// There are no parameters for the Provisioning Input Complete PDU.
/// 5.4.1.5 Provisioning Input Complete
/// - seeAlso: MshPRT_v1.1.pdf (page.582)
@interface SigProvisioningInputCompletePdu : SigProvisioningPdu

/**
 * @brief   Initialize SigProvisioningInputCompletePdu object.
 * @param   parameters    the hex data of SigProvisioningInputCompletePdu.
 * @return  return `nil` when initialize SigProvisioningInputCompletePdu object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


/// The Provisioner or the Provisionee sends a Provisioning Confirmation PDU to its peer to confirm the values exchanged
/// so far, including the OOB Authentication value and the random number that has yet to be exchanged.
/// 5.4.1.6 Provisioning Confirmation
/// - seeAlso: MshPRT_v1.1.pdf (page.582)
@interface SigProvisioningConfirmationPdu : SigProvisioningPdu
/// The values exchanged so far including the OOB Authentication M value. The size is 16 or 32.
/// If the PDU is sent by the Provisioner, the Confirmation field shall contain the ConfirmationProvisioner value.
/// If the PDU is sent by the Provisionee, the Confirmation field shall contain the ConfirmationDevice value.
/// The size of the Confirmation field, the ConfirmationProvisioner value, and the ConfirmationDevice value are
/// defined in Section 5.4.2.4.1.
@property (nonatomic, strong) NSData *confirmation;

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
- (instancetype)initWithConfirmation:(NSData *)confirmation;

/**
 * @brief   Initialize SigProvisioningConfirmationPdu object.
 * @param   parameters    the hex data of SigProvisioningConfirmationPdu.
 * @return  return `nil` when initialize SigProvisioningConfirmationPdu object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


/// The Provisioner or the Provisionee sends a Provisioning Random PDU to enable its peer device to validate the confirmation.
/// 5.4.1.7 Provisioning Random
/// - seeAlso: MshPRT_v1.1.pdf (page.583)
@interface SigProvisioningRandomPdu : SigProvisioningPdu
/// The final input to the confirmation.
/// If the PDU is sent by the Provisioner, the Random field shall contain the RandomProvisioner value.
/// If the PDU is sent by the Provisionee, the Random field shall contain the RandomDevice value.
/// The size of the Random field, the RandomProvisioner value, and the RandomDevice value are defined in Section 5.4.2.4.1.
@property (nonatomic, strong) NSData *random;

/**
 * @brief   Initialize SigProvisioningRandomPdu object.
 * @param   random    The final input to the confirmation.
 * If the PDU is sent by the Provisioner, the Random field shall contain the RandomProvisioner value.
 * If the PDU is sent by the Provisionee, the Random field shall contain the RandomDevice value.
 * The size of the Random field, the RandomProvisioner value, and the RandomDevice value are defined in Section 5.4.2.4.1.
 * @return  return `nil` when initialize SigProvisioningRandomPdu object fail.
 */
- (instancetype)initWithRandom:(NSData *)random;

/**
 * @brief   Initialize SigProvisioningRandomPdu object.
 * @param   parameters    the hex data of SigProvisioningRandomPdu.
 * @return  return `nil` when initialize SigProvisioningRandomPdu object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


/// The Provisioner sends a Provisioning Data PDU to deliver provisioning data to a Provisionee.
/// 5.4.1.8 Provisioning Data
/// - seeAlso: MshPRT_v1.1.pdf (page.583)
@interface SigProvisioningDataPdu : SigProvisioningPdu
/// An encrypted and authenticated network key, NetKey Index, Key Refresh Flag, IV Update Flag,
/// current value of the IV Index, and unicast address of the primary element (see Section 5.4.2.5),
/// the size is 25.
@property (nonatomic, strong) NSData *encryptedProvisioningData;
/// PDU Integrity Check value, size is 8.
@property (nonatomic, strong) NSData *provisioningDataMIC;

/**
 * @brief   Initialize SigProvisioningRandomPdu object.
 * @param   encryptedProvisioningData    An encrypted and authenticated network key, NetKey Index,
 * Key Refresh Flag, IV Update Flag, current value of the IV Index, and unicast address of the primary element
 * (see Section 5.4.2.5),the size is 25.
 * @param   provisioningDataMIC    PDU Integrity Check value, size is 8.
 * @return  return `nil` when initialize SigProvisioningRandomPdu object fail.
 */
- (instancetype)initWithEncryptedProvisioningData:(NSData *)encryptedProvisioningData provisioningDataMIC:(NSData *)provisioningDataMIC;

@end


/// The Provisionee sends a Provisioning Complete PDU to indicate that it has successfully
/// received and processed the provisioning data.
/// There are no parameters for the Provisioning Complete PDU.
/// 5.4.1.9 Provisioning Complete
/// - seeAlso: MshPRT_v1.1.pdf (page.584)
@interface SigProvisioningCompletePdu : SigProvisioningPdu

/**
 * @brief   Initialize SigProvisioningCompletePdu object.
 * @param   parameters    the hex data of SigProvisioningCompletePdu.
 * @return  return `nil` when initialize SigProvisioningCompletePdu object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


/// The Provisionee sends a Provisioning Failed PDU if it fails to process a received provisioning protocol PDU.
/// 5.4.1.10 Provisioning Failed
/// - seeAlso: MshPRT_v1.1.pdf (page.584)
@interface SigProvisioningFailedPdu : SigProvisioningPdu
/// This represents a specific error in the provisioning protocol encountered by a Provisionee.
@property (nonatomic, assign) ProvisioningError errorCode;

/**
 * @brief   Initialize SigProvisioningFailedPdu object.
 * @param   parameters    the hex data of SigProvisioningFailedPdu.
 * @return  return `nil` when initialize SigProvisioningFailedPdu object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


/// The Provisioner sends a Provisioning Record Request PDU to request a provisioning record fragment
/// (a part of a provisioning record; see Section 5.4.2.6) from the Provisionee.
/// 5.4.1.11 Provisioning Record Request
/// - seeAlso: MshPRT_v1.1.pdf (page.585)
@interface SigProvisioningRecordRequestPdu : SigProvisioningPdu
/// Identifies the provisioning record for which the request is made M (see Section 5.4.2.6).
/// A Provisionee may contain multiple provisioning records, and the Record ID field specifies the record
/// that is being requested.
@property (nonatomic, assign) UInt16 recordID;
/// The starting offset of the requested fragment in the provisioning record data.
/// The Fragment Offset field value indicates the starting offset for the provisioning record data fragment in
/// the Provisioning Record Response PDU that corresponds to the Provisioning Record Request. The Fragment
/// Offset field values are in the range 0, indicating the first octet of the provisioning record data, to the
/// provisioning record data length (in bytes) minus 1, indicating the last octet of the provisioning record data.
@property (nonatomic, assign) UInt16 fragmentOffset;
/// The maximum size of the provisioning record fragment that the Provisioner can receive.
/// The Fragment Maximum Size field specifies the maximum fragment size that the Provisioner can accept.
/// The choice of the value is affected by the provisioning bearer that is being used. A value of 0x0000 is Prohibited.
@property (nonatomic, assign) UInt16 fragmentMaximumSize;

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
- (instancetype)initWithRecordID:(UInt16)recordID fragmentOffset:(UInt16)fragmentOffset fragmentMaximumSize:(UInt16)fragmentMaximumSize;

@end


/// The Provisionee sends a Provisioning Record Response PDU in response to a Provisioning Record Request PDU.
/// 5.4.1.12 Provisioning Record Response
/// - seeAlso: MshPRT_v1.1.pdf (page.585)
@interface SigProvisioningRecordResponsePdu : SigProvisioningPdu
/// Indicates whether or not the request was handled successfully(see Table 5.44).
@property (nonatomic, assign) SigProvisioningRecordResponseStatus status;
/// Identifies the provisioning record whose data fragment is sent in the response (see Section 5.4.2.6).
/// The value of the Record ID field of the Provisioning Record Response PDU shall be set to the value of the Record ID
/// field of the corresponding Provisioning Record Request PDU.
@property (nonatomic, assign) UInt16 recordID;
/// The starting offset of the data fragment in the provisioning record data.
/// The value of the Fragment Offset field of the Provisioning Record Response PDU shall be set to the value of the
/// Fragment Offset field of the corresponding Provisioning Record Request PDU.
@property (nonatomic, assign) UInt16 fragmentOffset;
/// Total length of the provisioning record data stored on the Provisionee.
/// If the value of the Status field of the Provisioning Record Response PDU is either Success or Requested Offset Is
/// Out Of Bounds, the Total Length field indicates the length of provisioning record data that the Provisionee has for
/// the provisioning record specified by the Record ID field of the Provisioning Record Response PDU.
/// If the value of the Status field of the Provisioning Record Response PDU is Requested Record Is Not Present, the
/// Total Length field shall be set to 0x0000.
@property (nonatomic, assign) UInt16 totalLength;
/// Provisioning record data fragment. (Optional).
/// The Data field contains a fragment of the provisioning record specified by the Record ID field of the Provisioning
/// Record Response PDU.
/// If the value of the Status field of the Provisioning Record Response PDU is not Success, the Data field shall be empty.
/// If the value of the Status field of the Provisioning Record Response PDU is Success, the fragment shall start at the
/// position given by the Fragment Offset field of the Provisioning Record Response PDU.
/// The length of the fragment shall not exceed the value in the Fragment Maximum Size field of the corresponding Provisioning
/// Record Request PDU. The fragment may be shorter than the value of the Fragment Maximum Size field if less data is
/// available or if the Provisionee cannot create a message of the requested size because of memory limitations or some other
/// limiting factor.
@property (nonatomic, strong) NSData *data;

/**
 * @brief   Initialize SigProvisioningRecordResponsePdu object.
 * @param   parameters    the hex data of SigProvisioningRecordResponsePdu.
 * @return  return `nil` when initialize SigProvisioningRecordResponsePdu object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


/// The Provisioner sends a Provisioning Records Get PDU to request the list of IDs of the provisioning
/// records that are stored on a Provisionee.
/// There are no parameters for the Provisioning Records Get PDU.
/// 5.4.1.13 Provisioning Records Get
/// - seeAlso: MshPRT_v1.1.pdf (page.587)
@interface SigProvisioningRecordsGetPdu : SigProvisioningPdu

@end


/// The Provisionee sends a Provisioning Records List PDU in response to a Provisioning Records Get PDU.
/// 5.4.1.14 Provisioning Records List
/// - seeAlso: MshPRT_v1.1.pdf (page.587)
@interface SigProvisioningRecordsListPdu : SigProvisioningPdu
/// Bitmask indicating the provisioning extensions supported by the Provisionee (see Table 5.46)
/// The value of the Provisioning Extensions field of the Provisioning Records List PDU indicates the provisioning
/// extensions supported by the Provisionee.
@property (nonatomic, assign) UInt16 provisioningExtensions;
/// Lists the Record IDs of the provisioning records stored on the Provisionee (see Section 5.4.2.6). (Optional).
/// The Records List field of the Provisioning Records List PDU shall list the 16-bit Record IDs of the provisioning
/// records that are stored on the Provisionee. If no provisioning records are stored on the Provisionee,
/// the field shall be empty.
@property (nonatomic, strong) NSArray <NSNumber *>*recordsList;

/**
 * @brief   Initialize SigProvisioningRecordsListPdu object.
 * @param   parameters    the hex data of SigProvisioningRecordsListPdu.
 * @return  return `nil` when initialize SigProvisioningRecordsListPdu object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


/// 3.4.4 Network PDU
/// - seeAlso: MshPRT_v1.1.pdf (page.59)
@interface SigNetworkPdu : SigPdu
/// The Network Key used to decode/encode the PDU.
@property (nonatomic,strong) SigNetkeyModel *networkKey;

/// Least significant bit of IV Index.
/// 3.4.4.1 IVI
/// The IVI field contains the least significant bit of the IV Index used in the nonce to authenticate and encrypt this
/// Network PDU (see Section 3.9.2.11).
@property (nonatomic,assign) UInt8 ivi;

/// Value derived from the NetKey used to identify the Encryption Key and Privacy Key used to secure this PDU.
/// 3.4.4.2 NID
/// The NID field contains a 7-bit network identifier that allows for an easier lookup of the EncryptionKey and
/// PrivacyKey used to authenticate and encrypt this Network PDU (see Section 3.9.6.3.1).
/// The NID value is derived from the network key in conjunction with the EncryptionKey and PrivacyKey. It is derived
/// differently for Network PDUs using managed flooding security material, Network PDUs using directed security
/// material, and Network PDUs using friendship security material (see Section 3.9.6.3.1).
@property (nonatomic,assign) UInt8 nid;

/// Network Control.
/// 3.4.4.3 CTL
/// The CTL field is a 1-bit value that is used to determine if the Network PDU is part of a Transport Control message
/// or an Access message, as illustrated in Table 3.11.
/// If the CTL field is set to 0, the NetMIC is a 32-bit field and the Lower Transport PDU contains an Access message.
/// If the CTL field is set to 1, the NetMIC is a 64-bit field and the Lower Transport PDU contains a Transport Control
/// message.
@property (nonatomic,assign) SigLowerTransportPduType ctl;

/// Time To Live.
/// 3.4.4.4 TTL
/// The TTL field is a 7-bit field. The TTL field values are defined in Table 3.12.
/// Value                    Description
/// 0                           Network PDU has not been relayed and will not be relayed.
/// 1                           Network PDU has been relayed and will not be relayed.
/// 2 - 126                 Network PDU has been relayed or Network PDU has not been relayed. Network PDU can be relayed.
/// 127                      Network PDU has not been relayed and can be relayed.
/// The initial value of this field is set by the transmitting layer (lower transport layer, upper transport layer, access,
/// foundation model, model) or an application and used by the network layer when operating as a Relay node or as a
/// Directed Relay node.
/// The use of the TTL value of zero allows a node to transmit a Network PDU that it knows will not be relayed, and therefore
/// the receiving node can determine that the sending node is a single radio link away. The use of a TTL value of one or larger
/// cannot be used for such a determination.
@property (nonatomic,assign) UInt8 ttl;

/// Sequence Number.
/// 3.4.4.5 SEQ
/// The SEQ field is a 24-bit integer. The combined SEQ field, IV Index field, and SRC field (see Section 3.4.4.6) shall be a
/// unique value for each new Network PDU that this element originates.
@property (nonatomic,assign) UInt32 sequence;

/// Source Address.
/// 3.4.4.6 SRC
/// The SRC field is a 16-bit value that identifies the element that originated this Network PDU. This address shall be a
/// unicast address.
/// The SRC field is set by the originating element and untouched by nodes operating as a Relay node or as a Directed
/// Relay node.
@property (nonatomic,assign) UInt16 source;

/// Destination Address.
/// 3.4.4.7 DST
/// The DST field is a 16-bit value that identifies the element or elements that this Network PDU is directed towards.
/// This address shall be a unicast address, a group address, or a virtual address.
/// The DST field is set by the originating node and is untouched by the network layer in nodes operating as a Relay
/// node or as a Directed Relay node.
@property (nonatomic,assign) UInt16 destination;

/// Transport Protocol Data Unit. It is guaranteed to have 1 to 16 bytes.
/// 3.4.4.8 TransportPDU
/// The TransportPDU field, from a network layer point of view, is a sequence of octets of data. When the CTL field value is 0,
/// the TransportPDU field shall be a maximum of 128 bits. When the CTL field value is 1, the TransportPDU field shall be a
/// maximum of 96 bits.
/// The TransportPDU field is set by the originating lower transport layer and shall not be changed by the network layer.
@property (nonatomic,strong) NSData *transportPdu;

/// Message Integrity Check for Network. The size is 4 or 8.
/// 3.4.4.9 NetMIC
/// The NetMIC field is a 32-bit or 64-bit field (depending on the value of the CTL field) that authenticates that the DST and
/// TransportPDU fields have not been changed.
/// When the CTL field value is 0, the size of the NetMIC field shall be 32 bits. When the CTL field value is 1, the size of the
/// NetMIC field shall be 64 bits.
/// The NetMIC field value is set by the network layer at each node that transmits or relays this Network PDU.
@property (nonatomic,strong) NSData *netMIC;

/**
 * @brief   Creates Network PDU object from received PDU. The initiator tries to deobfuscate and decrypt the data using
 * given Network Key and IV Index.
 * @param   pdu    the hex data of SigProvisioningRecordsListPdu.
 * @param   pduType    The type of the PDU: `SigPduType_proxyConfiguration` or `SigPduType_networkPdu`.
 * @param   networkKey    The Network Key to decrypt the PDU.
 * @param   ivIndex    The ivIndex to decrypt the PDU.
 * @return  return `nil` when initialize SigNetworkPdu object fail.
 */
- (instancetype)initWithDecodePduData:(NSData *)pdu pduType:(SigPduType)pduType usingNetworkKey:(SigNetkeyModel *)networkKey ivIndex:(SigIvIndex *)ivIndex;

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
- (instancetype)initWithEncodeLowerTransportPdu:(SigLowerTransportPdu *)lowerTransportPdu pduType:(SigPduType)pduType withSequence:(UInt32)sequence andTtl:(UInt8)ttl ivIndex:(SigIvIndex *)ivIndex;

/**
 * @brief   This method goes over all Network Keys in the mesh network and tries to deobfuscate and decode the network PDU.
 * @param   pdu    The received PDU.
 * @param   pduType    The type of the PDU: `SigPduType_proxyConfiguration` or `SigPduType_networkPdu`.
 * @param   meshDataSource    The mesh network for which the PDU should be decoded.
 * @return  return `nil` when initialize SigNetworkPdu object fail.
 */
+ (instancetype)decodePdu:(NSData *)pdu pduType:(SigPduType)pduType meshDataSource:(SigDataSource *)meshDataSource;

/// Whether the Network PDU contains a segmented Lower Transport PDU, or not.
- (BOOL)isSegmented;

/// The 24-bit Seq Auth used to transmit the first segment of a segmented message,
/// or the 24-bit sequence number of an unsegmented
/// message.
- (UInt32)messageSequence;

/// Get UInt32 decode ivIndex value.
- (UInt32)getDecodeIvIndex;

@end


/// 3.10Mesh beacons
/// - seeAlso: MshPRT_v1.1.pdf (page.211)
@interface SigBeaconPdu : SigPdu

/// Mesh Beacon Type.
@property (nonatomic,assign,readonly) SigBeaconType beaconType;

@end


/// The Secure Network beacon is used by nodes to identify the subnet and its security state.
/// 3.10.3 Secure Network beacon
/// - seeAlso: MshPRT_v1.1.pdf (page.214)
@interface SigSecureNetworkBeacon : SigBeaconPdu
/// The Network Key related to this Secure Network Beacon.
@property (nonatomic,strong) SigNetkeyModel *networkKey;
/// Key Refresh flag value.
///
/// When this flag is active, the Node shall set the Key Refresh Phase for this Network Key to `.finalizing`.
/// When in this phase, the Node shall only transmit messages and Secure Network beacons using the new keys,
/// shall receive messages using the old keys and the new keys, and shall only receive Secure Network beacons
/// secured using the new Network Key.
@property (nonatomic,assign) BOOL keyRefreshFlag;
/// This flag is set to `true` if IV Update procedure is active.
@property (nonatomic,assign) BOOL ivUpdateActive;
/// Contains the value of the Network ID.
@property (nonatomic,strong) NSData *networkId;
/// Contains the current IV Index.
@property (nonatomic,assign) UInt32 ivIndex;

/// Creates USecure Network beacon PDU object from received PDU.
///
/// - parameter pdu: The data received from mesh network.
/// - parameter networkKey: The Network Key to validate the beacon.
/// - returns: The beacon object, or `nil` if the data are invalid.
- (instancetype)initWithDecodePdu:(NSData *)pdu usingNetworkKey:(SigNetkeyModel *)networkKey;

/**
 * @brief   Creates USecure Network beacon PDU object from received PDU.
 * @param   pdu    The data received from mesh network.
 * @param   meshDataSource    The mesh network for which the PDU should be decoded.
 * @return  return `nil` when initialize SigSecureNetworkBeacon object fail.
 */
+ (instancetype)decodePdu:(NSData *)pdu meshDataSource:(SigDataSource *)meshDataSource;

/**
 * @brief   Initialize SigSecureNetworkBeacon object.
 * @param   keyRefreshFlag    Key Refresh flag value.
 * @param   ivUpdateActive    This flag is set to `true` if IV Update procedure is active.
 * @param   networkId    Contains the value of the Network ID.
 * @param   ivIndex    Contains the current IV Index.
 * @param   networkKey    The Network Key related to this Secure Network Beacon.
 * @return  return `nil` when initialize SigSecureNetworkBeacon object fail.
 */
- (instancetype)initWithKeyRefreshFlag:(BOOL)keyRefreshFlag ivUpdateActive:(BOOL)ivUpdateActive networkId:(NSData *)networkId ivIndex:(UInt32)ivIndex usingNetworkKey:(SigNetkeyModel *)networkKey;

/// Get dictionary from SigSecureNetworkBeacon object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSecureNetworkBeacon;

/// Set dictionary to SigSecureNetworkBeacon object.
/// @param dictionary SigSecureNetworkBeacon dictionary object.
- (void)setDictionaryToSecureNetworkBeacon:(NSDictionary *)dictionary;

@end


/// The Unprovisioned Device beacon is used by devices that are unprovisioned
/// to allow them to be discovered by a Provisioner.
/// 3.10.2 Unprovisioned Device beacon
/// - seeAlso: MshPRT_v1.1.pdf (page.212)
@interface SigUnprovisionedDeviceBeacon : SigBeaconPdu
/// Device UUID uniquely identifying this device (see Section 3.11.3)
@property (nonatomic,strong) NSString *deviceUuid;
/// The OOB Information field is used to help drive the provisioning process by indicating
/// the availability of OOB data, such as a public key of the device.
@property (nonatomic,assign) struct OobInformation oob;
/// Hash of the associated URI advertised with the URI AD Type.
@property (nonatomic,strong,nullable) NSData *uriHash;

/// Creates Unprovisioned Device beacon PDU object from received PDU.
///
/// - parameter pdu: The data received from mesh network.
/// - returns: The beacon object, or `nil` if the data are invalid.
- (instancetype)initWithDecodePdu:(NSData *)pdu;

@end


/// The Mesh Private beacon is used by the nodes to identify the Key Refresh Flag (see Table 3.80),
/// IV Update Flag (see Table 3.80), and IV Index (see Section 3.9.4) of the subnet.
/// 3.10.4 Mesh Private beacon
/// - seeAlso: MshPRT_v1.1.pdf (page.216)
@interface SigMeshPrivateBeacon : SigBeaconPdu
/// The Network Key related to this Mesh Private beacon.
@property (nonatomic,strong) SigNetkeyModel *networkKey;
@property (nonatomic,strong) NSData *netKeyData;
/// Key Refresh flag value.
///
/// When this flag is active, the Node shall set the Key Refresh Phase for this Network Key to `.finalizing`.
/// When in this phase, the Node shall only transmit messages and Secure Network beacons using the new keys,
/// shall receive messages using the old keys and the new keys, and shall only receive Secure Network beacons
/// secured using the new Network Key.
@property (nonatomic,assign) BOOL keyRefreshFlag;
/// This flag is set to `true` if IV Update procedure is active.
@property (nonatomic,assign) BOOL ivUpdateActive;
/// Contains the current IV Index.
@property (nonatomic,assign) UInt32 ivIndex;
/// Random number used as an entropy for obfuscation and authentication of the Mesh Private beacon. The size is 13 bytes.
@property (nonatomic,strong) NSData *randomData;
/// Obfuscated Private Beacon Data. The size is 5 bytes.
@property (nonatomic,strong) NSData *obfuscatedPrivateBeaconData;
/// Authentication tag for the beacon. The size is 8 bytes.
@property (nonatomic,strong) NSData *authenticationTag;

/// Creates Mesh Private beacon PDU object from received PDU.
///
/// - parameter pdu: The data received from mesh network.
/// - parameter networkKey: The Network Key to validate the beacon.
/// - returns: The beacon object, or `nil` if the data are invalid.
- (instancetype)initWithDecodePdu:(NSData *)pdu usingNetworkKey:(SigNetkeyModel *)networkKey;

/**
 * @brief   Creates Mesh Private beacon PDU object from received PDU.
 * @param   pdu    The data received from mesh network.
 * @param   meshDataSource    The mesh network for which the PDU should be decoded.
 * @return  return `nil` when initialize SigMeshPrivateBeacon object fail.
 */
+ (instancetype)decodePdu:(NSData *)pdu meshDataSource:(SigDataSource *)meshDataSource;

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
- (instancetype)initWithKeyRefreshFlag:(BOOL)keyRefreshFlag ivUpdateActive:(BOOL)ivUpdateActive ivIndex:(UInt32)ivIndex randomData:(NSData *)randomData usingNetworkKey:(SigNetkeyModel *)networkKey;

@end


@interface PublicKey : NSObject
@property (nonatomic, strong) NSData *PublicKeyData;
@property (nonatomic, assign) PublicKeyType publicKeyType;
- (instancetype)initWithPublicKeyType:(PublicKeyType)type;
@end

NS_ASSUME_NONNULL_END
