/********************************************************************************************************
 * @file     SigEnumeration.h
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/9/6
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

#ifndef SigEnumeration_h
#define SigEnumeration_h

/// Table 3.65: Nonce types
/// - seeAlso: MshPRT_v1.1.pdf  (page.194)
/// The nonce is a 13-octet value that is unique for each new message encryption. There are four different
/// nonces that are used, as shown in Table 3.65. The type of the nonce is determined by the first octet of
/// the nonce, referred to as the Nonce Type.
typedef enum : UInt8 {
    /// Used with an EncryptionKey for network authentication and encryption
    SigNonceType_networkNonce    = 0,
    /// Used with an application key for upper transport authentication and encryption
    SigNonceType_applicationNonce    = 1,
    /// Used with a device key for upper transport authentication and encryption
    SigNonceType_deviceNonce = 2,
    /// Used with an EncryptionKey for proxy authentication and encryption
    SigNonceType_proxyNonce     = 3,
    /// Used with an EncryptionKey for Proxy solicitation authentication and encryption
    SigNonceType_proxySolicitationNonce     = 4,
    //0x05–0xFF, Reserved for Future Use
} SigNonceType;

/// Table 3.43: Opcode formats
/// - seeAlso: Mesh_v1.0.pdf  (page.93)
typedef enum : UInt8 {
    SigOpCodeType_sig1    = 1,//1-octet Opcodes,Opcode Format:0xxxxxxx (excluding 01111111)
    SigOpCodeType_sig2    = 2,//2-octet Opcodes,Opcode Format:10xxxxxx xxxxxxxx
    SigOpCodeType_vendor3 = 3,//3-octet Opcodes,Opcode Format:11xxxxxx zzzzzzzz
    SigOpCodeType_RFU     = 0b01111111,
} SigOpCodeType;

/// Table 5.14: Provisioning PDU types.
/// - seeAlso: Mesh_v1.0.pdf  (page.238)
/// Table 5.18: Provisioning PDU types.
/// - seeAlso: MshPRFd1.1r11_clean.pdf  (page.488)
typedef enum : UInt8 {
    /// Invites a device to join a mesh network
    SigProvisioningPduType_invite         = 0x00,
    /// Indicates the capabilities of the device
    SigProvisioningPduType_capabilities   = 0x01,
    /// Indicates the provisioning method selected by the Provisioner based on the capabilities of the device
    SigProvisioningPduType_start          = 0x02,
    /// Contains the Public Key of the device or the Provisioner
    SigProvisioningPduType_publicKey      = 0x03,
    /// Indicates that the user has completed inputting a value
    SigProvisioningPduType_inputComplete  = 0x04,
    /// Contains the provisioning confirmation value of the device or the Provisioner
    SigProvisioningPduType_confirmation   = 0x05,
    /// Contains the provisioning random value of the device or the Provisioner
    SigProvisioningPduType_random         = 0x06,
    /// Includes the assigned unicast address of the primary element, a network key, NetKey Index, Flags and the IV Index
    SigProvisioningPduType_data           = 0x07,
    /// Indicates that provisioning is complete
    SigProvisioningPduType_complete       = 0x08,
    /// Indicates that provisioning was unsuccessful
    SigProvisioningPduType_failed         = 0x09,
    /// Indicates a request to retrieve a provisioning record fragment from the device
    SigProvisioningPduType_recordRequest  = 0x0A,
    /// Contains a provisioning record fragment or an error status, sent in response to a Provisioning Record Request
    SigProvisioningPduType_recordResponse = 0x0B,
    /// Indicates a request to retrieve the list of IDs of the provisioning records that the unprovisioned device supports.
    SigProvisioningPduType_recordsGet     = 0x0C,
    /// Contains the list of IDs of the provisioning records that the unprovisioned device supports.
    SigProvisioningPduType_recordsList    = 0x0D,
    /// RFU, Reserved for Future Use, 0x0E–0xFF.
} SigProvisioningPduType;

/// 3.4.2 Addresses.
/// - seeAlso: MshPRT_v1.1.pdf  (page.54)
typedef enum : UInt16 {
    /// 3.4.2.1 Unassigned address
    /// An unassigned address is an address in which the element of a node has not been configured yet or no address
    /// has been allocated. The unassigned address shall have the value 0x0000 as shown in Figure 3.4 below. This may
    /// be used, for example, to disable message publishing of a model by setting the publish address of a model to the
    /// unassigned address.
    /// An unassigned address shall not be used in the SRC field or the DST field of a Network PDU.
    MeshAddress_unassignedAddress                                                            = 0x0000,
    /// 3.4.2.2 Unicast address
    /// A unicast address is a unique address allocated to each element. A unicast address has bit 15 set to 0. The unicast
    /// address shall not have the value 0x0000, and therefore can have any value from 0x0001 to 0x7FFF inclusive as
    /// shown in Figure 3.5 below.
    /// A unicast address is allocated to each element of a node for a term of the node on the network. The address
    /// allocation for the initial term is performed by a Provisioner during provisioning as described in Section 5.4.2.5. The
    /// address may be changed by executing the Node Address Refresh procedure (see Section 3.11.8.5) or the Node
    /// Composition Refresh procedure (see Section 3.11.8.6). The address may be unallocated by a Provisioner to allow
    /// the address to be reused using the procedure defined in Section 3.11.7.
    /// A unicast address shall be used in the SRC field of a Network PDU and may be used in the DST field of a Network
    /// PDU. A Network PDU sent to a unicast address shall be processed by at most one element.
    MeshAddress_minUnicastAddress                                                            = 0x0001,
    MeshAddress_maxUnicastAddress                                                            = 0x7FFF,
    /// 3.4.2.3 Virtual address
    /// A virtual address represents a set of destination addresses. Each virtual address logically represents a Label UUID,
    /// which is a 128-bit value that does not have to be managed centrally. One or more elements may be programmed to
    /// publish or subscribe to a Label UUID. The Label UUID is not transmitted and shall be used as the Additional Data field
    /// of the message integrity check value in the upper transport layer (see Section 3.9.7.1).
    /// The virtual address is a 16-bit value that has bit 15 set to 1, bit 14 set to 0, and bits 13 to 0 set to the value of a hash.
    /// This hash is a derivation of the Label UUID such that each hash represents many Label UUIDs.
    /// SALT                                                                                 =s1("vtad")
    /// hash                                                                                 =AES‐CMACSALT (Label UUID) mod 214
    /// When an Access message is received to a virtual address that has a matching hash, each corresponding Label UUID is
    /// used by the upper transport layer as additional data as part of the authentication of the message until a match is found.
    /// Transport Control messages cannot use virtual addresses.
    /// Label UUIDs may be generated randomly as defined in [6]. A Configuration Manager may assign and track virtual
    /// addresses; however, two devices can also create a virtual address using some out-of-band (OOB) mechanism. Unlike
    /// group addresses, these could be agreed upon by the devices involved and would not need to be registered in the
    /// centralized provisioning database, as they are unlikely to be duplicated.
    /// A disadvantage of virtual addresses is that a multi-segment message is required to transfer a Label UUID to a publishing
    /// or subscribing node during configuration.
    /// A virtual address can have any value from 0x8000 to 0xBFFF as shown in Figure 3.7 below.
    /// Note: When factoring in a 32-bit MIC and the size of the hash, there is only a 1⁄246 =1.42×10‐14 likelihood that two
    /// matching virtual addresses using the same application key but different Label UUIDs will collide.
    MeshAddress_minVirtualAddress                                                            = 0x8000,
    MeshAddress_maxVirtualAddress                                                            = 0xBFFF,
    /// A group address is an address that is programmed into zero or more elements. A group address has bit 15 set to 1 and bit 14 set to 1,
    /// as shown in Figure 3.8 below. Group addresses in the range 0xFF00 through 0xFFFF are reserved for Fixed Group addresses
    /// (see Table 3.7), and addresses in the range 0xC000 through 0xFEFF are generally available for other usage.
    /// A group address shall only be used in the DST field of a Network PDU. A Network PDU sent to a group address shall be
    /// delivered to all the instances of models that subscribe to this group address.
    MeshAddress_minGroupAddress                                                              = 0xC000,
    MeshAddress_maxGroupAddress                                                              = 0xFEFF,

    //0xFF00–0xFFF8 RFU

    /// all-ipt-border-routers(Add in mesh spec v1.1)
    MeshAddress_allIptBorderRouters                                                          = 0xFFF9,
    /// all-ipt-nodes(Add in mesh spec v1.1)
    MeshAddress_allIptNodes                                                                  = 0xFFFA,
    /// all-directed-forwarding-nodes(Add in mesh spec v1.1)
    MeshAddress_allDirectedForwardingNodes                                                   = 0xFFFB,
    /// A message sent to the all-proxies address shall be processed by the primary element of all nodes that have the proxy functionality enabled.
    MeshAddress_allProxies                                                                   = 0xFFFC,
    /// A message sent to the all-friends address shall be processed by the primary element of all nodes that have the friend functionality enabled.
    MeshAddress_allFriends                                                                   = 0xFFFD,
    /// A message sent to the all-relays address shall be processed by the primary element of all nodes that have the relay functionality enabled.
    MeshAddress_allRelays                                                                    = 0xFFFE,
    /// A message sent to the all-nodes address shall be processed by the primary element of all nodes.
    MeshAddress_allNodes                                                                     = 0xFFFF,
} MeshAddress;

typedef enum : UInt8 {
    DeviceStateOn,//on
    DeviceStateOff,//off
    DeviceStateOutOfLine,//outline
} DeviceState;//设备状态

typedef enum : NSUInteger {
    OOBSourceTypeManualInput,
    OOBSourceTypeImportFromFile,
} OOBSourceType;

/// Table 6.2: SAR field values.
/// - seeAlso: Mesh_v1.0.pdf  (page.261)
typedef enum : UInt8 {
    /// Data field contains a complete message
    SAR_completeMessage  = 0b00,
    /// Data field contains the first segment of a message
    SAR_firstSegment     = 0b01,
    /// Data field contains a continuation segment of a message
    SAR_continuation     = 0b10,
    /// Data field contains the last segment of a message
    SAR_lastSegment      = 0b11,
} SAR;

/// Table 5.12: Action field values.
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.141)
typedef enum : UInt8 {
    SchedulerTypeOff      = 0x0,
    SchedulerTypeOn       = 0x1,
    SchedulerTypeScene    = 0x2,
    SchedulerTypeNoAction = 0xF,
} SchedulerType;//闹钟类型

typedef enum : UInt8 {
    AddDeviceModelStateProvisionFail,
    AddDeviceModelStateBinding,
    AddDeviceModelStateBindSuccess,
    AddDeviceModelStateBindFail,
    AddDeviceModelStateScanned,
    AddDeviceModelStateConnecting,
    AddDeviceModelStateProvisioning,
} AddDeviceModelState;//添加的设备的状态

/// Table 5.18: Algorithms field values.
/// - seeAlso: Mesh_v1.0.pdf  (page.239)
typedef enum : UInt8 {
    /// FIPS P-256 Elliptic Curve algorithm will be used to calculate the shared secret.
    Algorithm_fipsP256EllipticCurve             = 0,
    /// BTM_ECDH_P256_HMAC_SHA256_AES_CCM
    Algorithm_fipsP256EllipticCurve_HMAC_SHA256 = 1,
    /// Reserved for Future Use: 2~15
} Algorithm;

/// Table 5.19: Public Key Type field values
/// - seeAlso: Mesh_v1.0.pdf  (page.239)
typedef enum : UInt8 {
    /// No OOB Public Key is used.
    PublicKeyType_noOobPublicKey = 0,
    /// OOB Public Key is used. The key must contain the full value of the Public Key, depending on the chosen algorithm.
    PublicKeyType_oobPublicKey   = 1,
    /// 0x02–0xFF, Prohibited.
} PublicKeyType;

/// The authentication method chosen for provisioning.
/// Table 5.28: Authentication Method field values
/// - seeAlso: Mesh_v1.0.pdf  (page.241)
typedef enum : UInt8 {
    /// No OOB authentication is used.
    AuthenticationMethod_noOob     = 0,
    /// Static OOB authentication is used.
    AuthenticationMethod_staticOob = 1,
    /// Output OOB authentication is used. Size must be in range 1...8.
    AuthenticationMethod_outputOob = 2,
    /// Input OOB authentication is used. Size must be in range 1...8.
    AuthenticationMethod_inputOob  = 3,
    /// Prohibited, 0x04–0xFF.
} AuthenticationMethod;

/// The output action will be displayed on the device. For example,
/// the device may use its LED to blink number of times. The number of blinks
/// will then have to be entered to the Provisioner Manager.
/// Table 5.22: Output OOB Action field values
/// - seeAlso: Mesh_v1.0.pdf  (page.240)
typedef enum : UInt8 {
    OutputAction_blink              = 0,
    OutputAction_beep               = 1,
    OutputAction_vibrate            = 2,
    OutputAction_outputNumeric      = 3,
    OutputAction_outputAlphanumeric = 4
    /// Reserved for Future Use, 5–15.
} OutputAction;

/// The user will have to enter the input action on the device. For example,
/// if the device supports `.push`, user will be asked to press a button on the
/// device required number of times.
/// Table 5.24: Input OOB Action field values
/// - seeAlso: Mesh_v1.0.pdf  (page.240)
typedef enum : UInt8 {
    InputAction_push              = 0,
    InputAction_twist             = 1,
    InputAction_inputNumeric      = 2,
    InputAction_inputAlphanumeric = 3,
    /// Reserved for Future Use, 4–15.
} InputAction;

/// Table 3.52: Beacon Type values
/// - seeAlso: Mesh_v1.0.pdf  (page.118)
typedef enum : UInt8 {
    SigBeaconType_unprovisionedDevice = 0,
    SigBeaconType_secureNetwork       = 1,
    /// - seeAlso: MshPRFd1.1r15_clean.pdf  (page.209)
    SigBeaconType_meshPrivateBeacon   = 2,
    /// Reserved for Future Use, 0x03–0xFF.
} SigBeaconType;

typedef enum      : UInt8 {
    /// - seeAlso : 3.4.4 Network PDU of Mesh_v1.0.pdf  (page.43)
    SigPduType_networkPdu         = 0,
    /// - seeAlso : 3.9 Mesh beacons of Mesh_v1.0.pdf  (page.117)
    SigPduType_meshBeacon         = 1,
    /// - seeAlso : 6.5 Proxy configuration messages of Mesh_v1.0.pdf  (page.262)
    SigPduType_proxyConfiguration = 2,
    /// - seeAlso : 5.4.1 Provisioning PDUs of Mesh_v1.0.pdf  (page.237)
    SigPduType_provisioningPdu    = 3,
    /// - seeAlso : 6.9.1 Solicitation PDU of MshPRT_v1.1.pdf (page.637)
    SigPduType_solicitationPDU    = 4,
} SigPduType;

/// Table 5.20: Static OOB Type field values
/// - seeAlso: Mesh_v1.0.pdf  (page.239)
typedef enum : UInt8 {
    ProvisionType_NoOOB,//普通添加模式
    ProvisionType_StaticOOB,//云端校验添加模式（阿里的天猫精灵设备、小米的小爱同学设备）
    ProvisionType_Reserved,//预留
} ProvisionType;

typedef enum : UInt8 {
    KeyBindType_Normal,//普通添加模式
    KeyBindType_Fast,//快速添加模式
    KeyBindType_Reserved,//预留
} KeyBindType;

/// The Step Resolution field enumerates the resolution of the Number of Steps field and the values are defined in Table 4.6.
/// - seeAlso: Mesh_v1.0.pdf  (page.137)
typedef enum : UInt8 {
    SigStepResolution_hundredsOfMilliseconds = 0b00,
    SigStepResolution_seconds                = 0b01,
    SigStepResolution_tensOfSeconds          = 0b10,
    SigStepResolution_tensOfMinutes          = 0b11,
} SigStepResolution;

/// The status of a Config operation.Table 4.108: Summary of status codes.
/// - seeAlso: Mesh_v1.0.pdf  (page.194)
typedef enum : UInt8 {
    SigConfigMessageStatus_success                        = 0x00,
    SigConfigMessageStatus_invalidAddress                 = 0x01,
    SigConfigMessageStatus_invalidModel                   = 0x02,
    SigConfigMessageStatus_invalidAppKeyIndex             = 0x03,
    SigConfigMessageStatus_invalidNetKeyIndex             = 0x04,
    SigConfigMessageStatus_insufficientResources          = 0x05,
    SigConfigMessageStatus_keyIndexAlreadyStored          = 0x06,
    SigConfigMessageStatus_invalidPublishParameters       = 0x07,
    SigConfigMessageStatus_notASubscribeModel             = 0x08,
    SigConfigMessageStatus_storageFailure                 = 0x09,
    SigConfigMessageStatus_featureNotSupported            = 0x0A,
    SigConfigMessageStatus_cannotUpdate                   = 0x0B,
    SigConfigMessageStatus_cannotRemove                   = 0x0C,
    SigConfigMessageStatus_cannotBind                     = 0x0D,
    SigConfigMessageStatus_temporarilyUnableToChangeState = 0x0E,
    SigConfigMessageStatus_cannotSet                      = 0x0F,
    SigConfigMessageStatus_unspecifiedError               = 0x10,
    SigConfigMessageStatus_invalidBinding                 = 0x11,
    /// - seeAlso: MshPRTd1.1r20_PRr00.pdf  (page.420)
    SigConfigMessageStatus_invalidPathEntry               = 0x12,
    SigConfigMessageStatus_cannotGet                      = 0x13,
    SigConfigMessageStatus_obsoleteInformation            = 0x14,
    SigConfigMessageStatus_invalidBearer                  = 0x15,
    /// 0x16-0xFF, RFU
} SigConfigMessageStatus;

#pragma mark - 4.2 Mesh Model Message Opcodes

/// 4.2.1 Mesh Model Message Opcodes by Value
/// - seeAlso: Assigned_Numbers.pdf (page.137),

typedef enum      : UInt32 {

    /// 4.3.4 Messages summary
    /// - seeAlso : Mesh_v1.0.pdf  (page.188)

    /// 0x00 Config AppKey Add
    SigOpCode_configAppKeyAdd                                = 0x00,
    /// 0x01 Config AppKey Update
    SigOpCode_configAppKeyUpdate                             = 0x01,
    /// 0x02 Config Composition Data Status
    SigOpCode_configCompositionDataStatus                    = 0x02,
    /// 0x03 Config Model Publication Set
    SigOpCode_configModelPublicationSet                      = 0x03,
    /// 0x04 Health Current Status
    SigOpCode_healthCurrentStatus                            = 0x04,
    /// 0x05 Health Fault Status
    SigOpCode_healthFaultStatus                              = 0x05,
    /// 0x06 Config Heartbeat Publication Status
    SigOpCode_configHeartbeatPublicationStatus               = 0x06,
    /// 0x40 Generic Location Global Status
    SigOpCode_genericLocationGlobalStatus                    = 0x40,
    /// 0x41 Generic Location Global Set
    SigOpCode_genericLocationGlobalSet                       = 0x41,
    /// 0x42 Generic Location Global Set Unacknowledged
    SigOpCode_genericLocationGlobalSetUnacknowledged         = 0x42,
    /// 0x43 Generic Manufacturer Properties Status
    SigOpCode_genericManufacturerPropertiesStatus            = 0x43,
    /// 0x44 Generic Manufacturer Property Set
    SigOpCode_genericManufacturerPropertySet                 = 0x44,
    /// 0x45 Generic Manufacturer Property Set Unacknowledged
    SigOpCode_genericManufacturerPropertySetUnacknowledged   = 0x45,
    /// 0x46 Generic Manufacturer Property Status
    SigOpCode_genericManufacturerPropertyStatus              = 0x46,
    /// 0x47 Generic Admin Properties Status
    SigOpCode_genericAdminPropertiesStatus                   = 0x47,
    /// 0x48 Generic Admin Property Set
    SigOpCode_genericAdminPropertySet                        = 0x48,
    /// 0x49 Generic Admin Property Set Unacknowledged
    SigOpCode_genericAdminPropertySetUnacknowledged          = 0x49,
    /// 0x4A Generic Admin Property Status
    SigOpCode_genericAdminPropertyStatus                     = 0x4A,
    /// 0x4B Generic User Properties Status
    SigOpCode_genericUserPropertiesStatus                    = 0x4B,
    /// 0x4C Generic User Property Set
    SigOpCode_genericUserPropertySet                         = 0x4C,
    /// 0x4D Generic User Property Set Unacknowledged
    SigOpCode_genericUserPropertySetUnacknowledged           = 0x4D,
    /// 0x4E Generic User Property Status
    SigOpCode_genericUserPropertyStatus                      = 0x4E,
    /// 0x4F Generic Client Properties Get
    SigOpCode_genericClientPropertiesGet                     = 0x4F,
    /// 0x50 Generic Client Properties Status
    SigOpCode_genericClientPropertiesStatus                  = 0x50,
    /// 0x51 Sensor Descriptor Status
    SigOpCode_sensorDescriptorStatus                         = 0x51,
    /// 0x52 Sensor Status
    SigOpCode_sensorStatus                                   = 0x52,
    /// 0x53 Sensor Column Status
    SigOpCode_sensorColumnStatus                             = 0x53,
    /// 0x54 Sensor Series Status
    SigOpCode_sensorSeriesStatus                             = 0x54,
    /// 0x55 Sensor Cadence Set
    SigOpCode_sensorCadenceSet                               = 0x55,
    /// 0x56 Sensor Cadence Set Unacknowledged
    SigOpCode_sensorCadenceSetUnacknowledged                 = 0x56,
    /// 0x57 Sensor Cadence Status
    SigOpCode_sensorCadenceStatus                            = 0x57,
    /// 0x58 Sensor Settings Status
    SigOpCode_sensorSettingsStatus                           = 0x58,
    /// 0x59 Sensor Setting Set
    SigOpCode_sensorSettingSet                               = 0x59,
    /// 0x5A Sensor Setting Set Unacknowledged
    SigOpCode_sensorSettingSetUnacknowledged                 = 0x5A,
    /// 0x5B Sensor Setting Status
    SigOpCode_sensorSettingStatus                            = 0x5B,
    /// 0x5C Time Set
    SigOpCode_timeSet                                        = 0x5C,
    /// 0x5D Time Status
    SigOpCode_timeStatus                                     = 0x5D,
    /// 0x5E Scene Status
    SigOpCode_sceneStatus                                    = 0x5E,
    /// 0x5F Scheduler Action Status
    SigOpCode_schedulerActionStatus                          = 0x5F,
    /// 0x60 Scheduler Action Set
    SigOpCode_schedulerActionSet                             = 0x60,
    /// 0x61 Scheduler Action Set Unacknowledged
    SigOpCode_schedulerActionSetUnacknowledged               = 0x61,
    /// 0x62 Light LC Property Set
    SigOpCode_LightLCPropertySet                             = 0x62,
    /// 0x63 Light LC Property Set Unacknowledged
    SigOpCode_LightLCPropertySetUnacknowledged               = 0x63,
    /// 0x64 Light LC Property Status
    SigOpCode_LightLCPropertyStatus                          = 0x64,
    /// 0x65 IEC 62386-104 Frame
    SigOpCode_IEC62386_104Frame                              = 0x65,
    /// 0x66 BLOB Chunk Transfer
    SigOpCode_BLOBChunkTransfer                              = 0x66,
    /// 0x67 BLOB Block Status
    SigOpCode_BLOBBlockStatus                                = 0x67,
    /// 0x68 BLOB Partial Block Report
    SigOpCode_BLOBPartialBlockReport                         = 0x68,
    /// 0x8000 config AppKey Delete
    SigOpCode_configAppKeyDelete                             = 0x8000,
    /// 0x8001 config AppKey Get
    SigOpCode_configAppKeyGet                                = 0x8001,
    /// 0x8002 config AppKey List
    SigOpCode_configAppKeyList                               = 0x8002,
    /// 0x8003 config AppKey Status
    SigOpCode_configAppKeyStatus                             = 0x8003,
    /// 0x80 0x04 Health Attention Get
    SigOpCode_healthAttentionGet                             = 0x8004,
    /// 0x80 0x05 Health Attention Set
    SigOpCode_healthAttentionSet                             = 0x8005,
    /// 0x80 0x06 Health Attention Set Unacknowledged
    SigOpCode_healthAttentionSetUnacknowledged               = 0x8006,
    /// 0x80 0x07 Health Attention Status
    SigOpCode_healthAttentionStatus                          = 0x8007,
    /// 0x8008 config Composition Data Get
    SigOpCode_configCompositionDataGet                       = 0x8008,
    /// 0x8009 config Beacon Get
    SigOpCode_configBeaconGet                                = 0x8009,
    /// 0x800A config Beacon Set
    SigOpCode_configBeaconSet                                = 0x800A,
    /// 0x800B config Beacon Status
    SigOpCode_configBeaconStatus                             = 0x800B,
    /// 0x800C config Default Ttl Get
    SigOpCode_configDefaultTtlGet                            = 0x800C,
    /// 0x800D config Default Ttl Set
    SigOpCode_configDefaultTtlSet                            = 0x800D,
    /// 0x800E config Default Ttl Status
    SigOpCode_configDefaultTtlStatus                         = 0x800E,
    /// 0x800F config Friend Get
    SigOpCode_configFriendGet                                = 0x800F,
    /// 0x8010 config Friend Set
    SigOpCode_configFriendSet                                = 0x8010,
    /// 0x8011 config Friend Status
    SigOpCode_configFriendStatus                             = 0x8011,
    /// 0x8012 config GATT Proxy Get
    SigOpCode_configGATTProxyGet                             = 0x8012,
    /// 0x8013 config GATT Proxy Set
    SigOpCode_configGATTProxySet                             = 0x8013,
    /// 0x8014 config GATT Proxy Status
    SigOpCode_configGATTProxyStatus                          = 0x8014,
    /// 0x8015 config Key Refresh Phase Get
    SigOpCode_configKeyRefreshPhaseGet                       = 0x8015,
    /// 0x8016 config Key Refresh Phase Set
    SigOpCode_configKeyRefreshPhaseSet                       = 0x8016,
    /// 0x8017 config Key Refresh Phase Status
    SigOpCode_configKeyRefreshPhaseStatus                    = 0x8017,
    /// 0x8018 config Model Publication Get
    SigOpCode_configModelPublicationGet                      = 0x8018,
    /// 0x8019 config Model Publication Set
    SigOpCode_configModelPublicationStatus                   = 0x8019,
    /// 0x801A config Model Publication Virtual Address Set
    SigOpCode_configModelPublicationVirtualAddressSet        = 0x801A,
    /// 0x801B config Model Subscription Add
    SigOpCode_configModelSubscriptionAdd                     = 0x801B,
    /// 0x801C config Model Subscription Delete
    SigOpCode_configModelSubscriptionDelete                  = 0x801C,
    /// 0x801D config Model Subscription Delete All
    SigOpCode_configModelSubscriptionDeleteAll               = 0x801D,
    /// 0x801E config Model Subscription Overwrite
    SigOpCode_configModelSubscriptionOverwrite               = 0x801E,
    /// 0x801F config Model Subscription Status
    SigOpCode_configModelSubscriptionStatus                  = 0x801F,
    /// 0x8020 config Model Subscription Virtual Address Add
    SigOpCode_configModelSubscriptionVirtualAddressAdd       = 0x8020,
    /// 0x8021 config Model Subscription Virtual Address Delete
    SigOpCode_configModelSubscriptionVirtualAddressDelete    = 0x8021,
    /// 0x8022 config Model Subscription Virtual Address Overwrite
    SigOpCode_configModelSubscriptionVirtualAddressOverwrite = 0x8022,
    /// 0x8023 config Network Transmit Get
    SigOpCode_configNetworkTransmitGet                       = 0x8023,
    /// 0x8024 config Network Transmit Set
    SigOpCode_configNetworkTransmitSet                       = 0x8024,
    /// 0x8025 config Network Transmit Status
    SigOpCode_configNetworkTransmitStatus                    = 0x8025,
    /// 0x8026 config Relay Get
    SigOpCode_configRelayGet                                 = 0x8026,
    /// 0x8027 config Relay Set
    SigOpCode_configRelaySet                                 = 0x8027,
    /// 0x8028 config Relay Status
    SigOpCode_configRelayStatus                              = 0x8028,
    /// 0x8029 config SIG Model Subscription Get
    SigOpCode_configSIGModelSubscriptionGet                  = 0x8029,
    /// 0x802A config SIG Model Subscription List
    SigOpCode_configSIGModelSubscriptionList                 = 0x802A,
    /// 0x802B config Vendor Model Subscription Get
    SigOpCode_configVendorModelSubscriptionGet               = 0x802B,
    /// 0x802C config Vendor Model Subscription List
    SigOpCode_configVendorModelSubscriptionList              = 0x802C,
    /// 0x802D config Low Power Node Poll Timeout Get
    SigOpCode_configLowPowerNodePollTimeoutGet               = 0x802D,
    /// 0x802E config Low Power Node Poll Timeout Status
    SigOpCode_configLowPowerNodePollTimeoutStatus            = 0x802E,
    /// 0x80 0x2F Health Fault Clear
    SigOpCode_healthFaultClear                               = 0x802F,
    /// 0x80 0x30 Health Fault Clear Unacknowledged
    SigOpCode_healthFaultClearUnacknowledged                 = 0x8030,
    /// 0x80 0x31 Health Fault Get
    SigOpCode_healthFaultGet                                 = 0x8031,
    /// 0x80 0x32 Health Fault Test
    SigOpCode_healthFaultTest                                = 0x8032,
    /// 0x80 0x33 Health Fault Test Unacknowledged
    SigOpCode_healthFaultTestUnacknowledged                  = 0x8033,
    /// 0x80 0x34 Health Period Get
    SigOpCode_healthPeriodGet                                = 0x8034,
    /// 0x80 0x35 Health Period Set
    SigOpCode_healthPeriodSet                                = 0x8035,
    /// 0x80 0x36 Health Period Set Unacknowledged
    SigOpCode_healthPeriodSetUnacknowledged                  = 0x8036,
    /// 0x80 0x37 Health Period Status
    SigOpCode_healthPeriodStatus                             = 0x8037,
    /// 0x8038 config Heartbeat Publication Get
    SigOpCode_configHeartbeatPublicationGet                  = 0x8038,
    /// 0x8039 config Heartbeat Publication Set
    SigOpCode_configHeartbeatPublicationSet                  = 0x8039,
    /// 0x803A config Heartbeat Subscription Get
    SigOpCode_configHeartbeatSubscriptionGet                 = 0x803A,
    /// 0x803B config Heartbeat Subscription Set
    SigOpCode_configHeartbeatSubscriptionSet                 = 0x803B,
    /// 0x803C config Heartbeat Subscription Status
    SigOpCode_configHeartbeatSubscriptionStatus              = 0x803C,
    /// 0x803D config Model App Bind
    SigOpCode_configModelAppBind                             = 0x803D,
    /// 0x803E config Model App Status
    SigOpCode_configModelAppStatus                           = 0x803E,
    /// 0x803F config Model App Unbind
    SigOpCode_configModelAppUnbind                           = 0x803F,
    /// 0x8040 config NetKey Add
    SigOpCode_configNetKeyAdd                                = 0x8040,
    /// 0x8041 config NetKey Delete
    SigOpCode_configNetKeyDelete                             = 0x8041,
    /// 0x8042 config NetKey Get
    SigOpCode_configNetKeyGet                                = 0x8042,
    /// 0x8043 config NetKey List
    SigOpCode_configNetKeyList                               = 0x8043,
    /// 0x8044 config NetKey Status
    SigOpCode_configNetKeyStatus                             = 0x8044,
    /// 0x8045 config NetKey Update
    SigOpCode_configNetKeyUpdate                             = 0x8045,
    /// 0x8046 config Node Identity Get
    SigOpCode_configNodeIdentityGet                          = 0x8046,
    /// 0x8047 config Node Identity Set
    SigOpCode_configNodeIdentitySet                          = 0x8047,
    /// 0x8048 config Node Identity Status
    SigOpCode_configNodeIdentityStatus                       = 0x8048,
    /// 0x8049 config Node Reset
    SigOpCode_configNodeReset                                = 0x8049,
    /// 0x804A config Node Reset Status
    SigOpCode_configNodeResetStatus                          = 0x804A,
    /// 0x804B config SIG Model App Get
    SigOpCode_configSIGModelAppGet                           = 0x804B,
    /// 0x804C config SIG Model App List
    SigOpCode_configSIGModelAppList                          = 0x804C,
    /// 0x804D config Vendor Model App Get
    SigOpCode_configVendorModelAppGet                        = 0x804D,
    /// 0x804E config Vendor Model App List
    SigOpCode_configVendorModelAppList                       = 0x804E,
    /// 0x804F remote Provisioning Scan Capabilities Get
    SigOpCode_remoteProvisioningScanCapabilitiesGet          = 0x804F,
    /// 0x8050 remote Provisioning Scan Capabilities Status
    SigOpCode_remoteProvisioningScanCapabilitiesStatus       = 0x8050,
    /// 0x8051 remote Provisioning Scan Get
    SigOpCode_remoteProvisioningScanGet                      = 0x8051,
    /// 0x8052 remote Provisioning Scan Start
    SigOpCode_remoteProvisioningScanStart                    = 0x8052,
    /// 0x8053 remote Provisioning Scan Stop
    SigOpCode_remoteProvisioningScanStop                     = 0x8053,
    /// 0x8054 remote Provisioning Scan Status
    SigOpCode_remoteProvisioningScanStatus                   = 0x8054,
    /// 0x8055 remote Provisioning Scan Report
    SigOpCode_remoteProvisioningScanReport                   = 0x8055,
    /// 0x8056 remote Provisioning Extended Scan Start
    SigOpCode_remoteProvisioningExtendedScanStart            = 0x8056,
    /// 0x8057 remote Provisioning Extended Scan Report
    SigOpCode_remoteProvisioningExtendedScanReport           = 0x8057,
    /// 0x8058 remote Provisioning Link Get
    SigOpCode_remoteProvisioningLinkGet                      = 0x8058,
    /// 0x8059 remote Provisioning Link Open
    SigOpCode_remoteProvisioningLinkOpen                     = 0x8059,
    /// 0x805A remote Provisioning Link Close
    SigOpCode_remoteProvisioningLinkClose                    = 0x805A,
    /// 0x805B remote Provisioning Link Status
    SigOpCode_remoteProvisioningLinkStatus                   = 0x805B,
    /// 0x805C remote Provisioning Link Report
    SigOpCode_remoteProvisioningLinkReport                   = 0x805C,
    /// 0x805D remote Provisioning PDU Send
    SigOpCode_remoteProvisioningPDUSend                      = 0x805D,
    /// 0x805E remote Provisioning PDU Outbound Report
    SigOpCode_remoteProvisioningPDUOutboundReport            = 0x805E,
    /// 0x805F remote Provisioning PDU Report
    SigOpCode_remoteProvisioningPDUReport                    = 0x805F,
    /// 0x8060 Private Beacon Get
    SigOpCode_PrivateBeaconGet                               = 0x8060,
    /// 0x8061 Private Beacon Set
    SigOpCode_PrivateBeaconSet                               = 0x8061,
    /// 0x8062 Private Beacon Status
    SigOpCode_PrivateBeaconStatus                            = 0x8062,
    /// 0x8063 Private Gatt Proxy Get
    SigOpCode_PrivateGattProxyGet                            = 0x8063,
    /// 0x8064 Private Gatt Proxy Set
    SigOpCode_PrivateGattProxySet                            = 0x8064,
    /// 0x8065 Private Gatt Proxy Status
    SigOpCode_PrivateGattProxyStatus                         = 0x8065,
    /// 0x8066 Private Node Identity Get
    SigOpCode_PrivateNodeIdentityGet                         = 0x8066,
    /// 0x8067 Private Node Identity Set
    SigOpCode_PrivateNodeIdentitySet                         = 0x8067,
    /// 0x8068 Private Node Identity Status
    SigOpCode_PrivateNodeIdentityStatus                      = 0x8068,
    /// 0x8069 On-Demand Private Proxy Get
    SigOpCode_OnDemandPrivateProxyGet                        = 0x8069,
    /// 0x806A On-Demand Private Proxy Set
    SigOpCode_OnDemandPrivateProxySet                        = 0x806A,
    /// 0x806B On-Demand Private Proxy Status
    SigOpCode_OnDemandPrivateProxyStatus                     = 0x806B,
    /// 0x806C SAR Transmitter Get
    SigOpCode_SARTransmitterGet                              = 0x806C,
    /// 0x806D SAR Transmitter Set
    SigOpCode_SARTransmitterSet                              = 0x806D,
    /// 0x806E SAR Transmitter Status
    SigOpCode_SARTransmitterStatus                           = 0x806E,
    /// 0x806F SAR Receiver Get
    SigOpCode_SARReceiverGet                                 = 0x806F,
    /// 0x8070 SAR Receiver Set
    SigOpCode_SARReceiverSet                                 = 0x8070,
    /// 0x8071 SAR Receiver Status
    SigOpCode_SARReceiverStatus                              = 0x8071,
    /// 0x8072 Opcodes Aggregator Sequence
    SigOpCode_OpcodesAggregatorSequence                      = 0x8072,
    /// 0x8073 Opcodes Aggregator Status
    SigOpCode_OpcodesAggregatorStatus                        = 0x8073,
    /// 0x8074 Large Composition Data Get
    SigOpCode_LargeCompositionDataGet                        = 0x8074,
    /// 0x8075 Large Composition Data Status
    SigOpCode_LargeCompositionDataStatus                     = 0x8075,
    /// 0x8076 Models Metadata Get
    SigOpCode_ModelsMetadataGet                              = 0x8076,
    /// 0x8077 Models Metadata Status
    SigOpCode_ModelsMetadataStatus                           = 0x8077,
    /// 0x8078 Solicitation Pdu RplItems Clear
    SigOpCode_SolicitationPduRplItemsClear                   = 0x8078,
    /// 0x8079 Solicitation Pdu RplItems Clear Unacknowledged
    SigOpCode_SolicitationPduRplItemsClearUnacknowledged     = 0x8079,
    /// 0x807A Solicitation Pdu RplItems Status
    SigOpCode_SolicitationPduRplItemsStatus                  = 0x807A,
    /// 0x807B Direct Control Get
    SigOpCode_DirectControlGet                               = 0x807B,
    /// 0x807C Direct Control Set
    SigOpCode_DirectControlSet                               = 0x807C,
    /// 0x807D Direct Control Status
    SigOpCode_DirectControlStatus                            = 0x807D,
    /// 0x80 0x7E PATH_METRIC_GET
    SigOpCode_pathMetricGet                                  = 0x807E,
    /// 0x80 0x7E PATH_METRIC_SET
    SigOpCode_pathMetricSet                                  = 0x807F,
    /// 0x80 0x7E PATH_METRIC_STATUS
    SigOpCode_pathMetricStatus                               = 0x8080,
    /// 0x80 0x81 DISCOVERY_TABLE_CAPABILITIES_GET
    SigOpCode_discoveryTableCapabilitiesGet                  = 0x8081,
    /// 0x80 0x82 DISCOVERY_TABLE_CAPABILITIES_SET
    SigOpCode_discoveryTableCapabilitiesSet                  = 0x8082,
    /// 0x80 0x83 DISCOVERY_TABLE_CAPABILITIES_STATUS
    SigOpCode_discoveryTableCapabilitiesStatus               = 0x8083,
    /// 0x80 0x84 ForwardingTableAdd
    SigOpCode_ForwardingTableAdd                             = 0x8084,
    /// 0x80 0x85 ForwardingTableDelete
    SigOpCode_ForwardingTableDelete                          = 0x8085,
    /// 0x80 0x86 ForwardingTableStatus
    SigOpCode_ForwardingTableStatus                          = 0x8086,
    /// 0x80B1 Subnet Bridge Get
    SigOpCode_SubnetBridgeGet                                = 0x80B1,
    /// 0x80B2 Subnet Bridge Set
    SigOpCode_SubnetBridgeSet                                = 0x80B2,
    /// 0x80B3 Subnet Bridge Status
    SigOpCode_SubnetBridgeStatus                             = 0x80B3,
    /// 0x80B4 Bridge Table Add
    SigOpCode_BridgeTableAdd                                 = 0x80B4,
    /// 0x80B5 Bridge Table Remove
    SigOpCode_BridgeTableRemove                              = 0x80B5,
    /// 0x80B6 Bridge Table Status
    SigOpCode_BridgeTableStatus                              = 0x80B6,
    /// 0x80B7 Bridge Subnets Get
    SigOpCode_BridgeSubnetsGet                               = 0x80B7,
    /// 0x80B8 Bridge Subnets List
    SigOpCode_BridgeSubnetsList                              = 0x80B8,
    /// 0x80B9 Bridge Table Get
    SigOpCode_BridgeTableGet                                 = 0x80B9,
    /// 0x80BA Bridge Table List
    SigOpCode_BridgeTableList                                = 0x80BA,
    /// 0x80BB Bridging Table Size Get
    SigOpCode_BridgingTableSizeGet                           = 0x80BB,
    /// 0x80BC Bridging Table Size Status
    SigOpCode_BridgingTableSizeStatus                        = 0x80BC,

    /// 7.1 Messages summary
    /// - seeAlso : Mesh_Model_Specification v1.0.pdf  (page.298)

    //Generic OnOff
    SigOpCode_genericOnOffGet                                = 0x8201,
    SigOpCode_genericOnOffSet                                = 0x8202,
    SigOpCode_genericOnOffSetUnacknowledged                  = 0x8203,
    SigOpCode_genericOnOffStatus                             = 0x8204,

    //Generic Level
    SigOpCode_genericLevelGet                                = 0x8205,
    SigOpCode_genericLevelSet                                = 0x8206,
    SigOpCode_genericLevelSetUnacknowledged                  = 0x8207,
    SigOpCode_genericLevelStatus                             = 0x8208,
    SigOpCode_genericDeltaSet                                = 0x8209,
    SigOpCode_genericDeltaSetUnacknowledged                  = 0x820A,
    SigOpCode_genericMoveSet                                 = 0x820B,
    SigOpCode_genericMoveSetUnacknowledged                   = 0x820C,

    //Generic Default Transition Time
    SigOpCode_genericDefaultTransitionTimeGet                = 0x820D,
    SigOpCode_genericDefaultTransitionTimeSet                = 0x820E,
    SigOpCode_genericDefaultTransitionTimeSetUnacknowledged  = 0x820F,
    SigOpCode_genericDefaultTransitionTimeStatus             = 0x8210,

    //Generic Power OnOff
    SigOpCode_genericOnPowerUpGet                            = 0x8211,
    SigOpCode_genericOnPowerUpStatus                         = 0x8212,
    //Generic Power OnOff Setup
    SigOpCode_genericOnPowerUpSet                            = 0x8213,
    SigOpCode_genericOnPowerUpSetUnacknowledged              = 0x8214,

    //Generic Power Level
    SigOpCode_genericPowerLevelGet                           = 0x8215,
    SigOpCode_genericPowerLevelSet                           = 0x8216,
    SigOpCode_genericPowerLevelSetUnacknowledged             = 0x8217,
    SigOpCode_genericPowerLevelStatus                        = 0x8218,
    SigOpCode_genericPowerLastGet                            = 0x8219,
    SigOpCode_genericPowerLastStatus                         = 0x821A,
    SigOpCode_genericPowerDefaultGet                         = 0x821B,
    SigOpCode_genericPowerDefaultStatus                      = 0x821C,
    SigOpCode_genericPowerRangeGet                           = 0x821D,
    SigOpCode_genericPowerRangeStatus                        = 0x821E,
    //Generic Power Level Setup
    SigOpCode_genericPowerDefaultSet                         = 0x821F,
    SigOpCode_genericPowerDefaultSetUnacknowledged           = 0x8220,
    SigOpCode_genericPowerRangeSet                           = 0x8221,
    SigOpCode_genericPowerRangeSetUnacknowledged             = 0x8222,

    //Generic Battery
    SigOpCode_genericBatteryGet                              = 0x8223,
    SigOpCode_genericBatteryStatus                           = 0x8224,

    //Sensor
    SigOpCode_sensorDescriptorGet                            = 0x8230,
    SigOpCode_sensorGet                                      = 0x8231,
    SigOpCode_sensorColumnGet                                = 0x8232,
    SigOpCode_sensorSeriesGet                                = 0x8233,
    //Sensor Setup
    SigOpCode_sensorCadenceGet                               = 0x8234,
    SigOpCode_sensorSettingsGet                              = 0x8235,
    SigOpCode_sensorSettingGet                               = 0x8236,

    //Time
    SigOpCode_timeGet                                        = 0x8237,
    SigOpCode_timeRoleGet                                    = 0x8238,
    SigOpCode_timeRoleSet                                    = 0x8239,
    SigOpCode_timeRoleStatus                                 = 0x823A,
    SigOpCode_timeZoneGet                                    = 0x823B,
    SigOpCode_timeZoneSet                                    = 0x823C,
    SigOpCode_timeZoneStatus                                 = 0x823D,
    SigOpCode_TAI_UTC_DeltaGet                               = 0x823E,
    SigOpCode_TAI_UTC_DeltaSet                               = 0x823F,
    SigOpCode_TAI_UTC_DeltaStatus                            = 0x8240,

    //Scene
    SigOpCode_sceneGet                                       = 0x8241,
    SigOpCode_sceneRecall                                    = 0x8242,
    SigOpCode_sceneRecallUnacknowledged                      = 0x8243,
    SigOpCode_sceneRegisterGet                               = 0x8244,
    SigOpCode_sceneRegisterStatus                            = 0x8245,
    //Scene Setup
    SigOpCode_sceneStore                                     = 0x8246,
    SigOpCode_sceneStoreUnacknowledged                       = 0x8247,
    SigOpCode_sceneDelete                                    = 0x829E,
    SigOpCode_sceneDeleteUnacknowledged                      = 0x829F,

    //Scheduler
    SigOpCode_schedulerActionGet                             = 0x8248,
    SigOpCode_schedulerGet                                   = 0x8249,
    SigOpCode_schedulerStatus                                = 0x824A,
    //Scheduler Setup

    //Light Lightness
    SigOpCode_lightLightnessGet                              = 0x824B,
    SigOpCode_lightLightnessSet                              = 0x824C,
    SigOpCode_lightLightnessSetUnacknowledged                = 0x824D,
    SigOpCode_lightLightnessStatus                           = 0x824E,
    SigOpCode_lightLightnessLinearGet                        = 0x824F,
    SigOpCode_lightLightnessLinearSet                        = 0x8250,
    SigOpCode_lightLightnessLinearSetUnacknowledged          = 0x8251,
    SigOpCode_lightLightnessLinearStatus                     = 0x8252,
    SigOpCode_lightLightnessLastGet                          = 0x8253,
    SigOpCode_lightLightnessLastStatus                       = 0x8254,
    SigOpCode_lightLightnessDefaultGet                       = 0x8255,
    SigOpCode_lightLightnessDefaultStatus                    = 0x8256,
    SigOpCode_lightLightnessRangeGet                         = 0x8257,
    SigOpCode_lightLightnessRangeStatus                      = 0x8258,
    //Light Lightness Setup
    SigOpCode_lightLightnessDefaultSet                       = 0x8259,
    SigOpCode_lightLightnessDefaultSetUnacknowledged         = 0x825A,
    SigOpCode_lightLightnessRangeSet                         = 0x825B,
    SigOpCode_lightLightnessRangeSetUnacknowledged           = 0x825C,

    //Light CTL
    SigOpCode_lightCTLGet                                    = 0x825D,
    SigOpCode_lightCTLSet                                    = 0x825E,
    SigOpCode_lightCTLSetUnacknowledged                      = 0x825F,
    SigOpCode_lightCTLStatus                                 = 0x8260,
    SigOpCode_lightCTLTemperatureGet                         = 0x8261,
    SigOpCode_lightCTLTemperatureRangeGet                    = 0x8262,
    SigOpCode_lightCTLTemperatureRangeStatus                 = 0x8263,
    SigOpCode_lightCTLTemperatureSet                         = 0x8264,
    SigOpCode_lightCTLTemperatureSetUnacknowledged           = 0x8265,
    SigOpCode_lightCTLTemperatureStatus                      = 0x8266,
    SigOpCode_lightCTLDefaultGet                             = 0x8267,
    SigOpCode_lightCTLDefaultStatus                          = 0x8268,
    //Light CTL Setup
    SigOpCode_lightCTLDefaultSet                             = 0x8269,
    SigOpCode_lightCTLDefaultSetUnacknowledged               = 0x826A,
    SigOpCode_lightCTLTemperatureRangeSet                    = 0x826B,
    SigOpCode_lightCTLTemperatureRangeSetUnacknowledged      = 0x826C,

    //Light HSL
    SigOpCode_lightHSLGet                                    = 0x826D,
    SigOpCode_lightHSLHueGet                                 = 0x826E,
    SigOpCode_lightHSLHueSet                                 = 0x826F,
    SigOpCode_lightHSLHueSetUnacknowledged                   = 0x8270,
    SigOpCode_lightHSLHueStatus                              = 0x8271,
    SigOpCode_lightHSLSaturationGet                          = 0x8272,
    SigOpCode_lightHSLSaturationSet                          = 0x8273,
    SigOpCode_lightHSLSaturationSetUnacknowledged            = 0x8274,
    SigOpCode_lightHSLSaturationStatus                       = 0x8275,
    SigOpCode_lightHSLSet                                    = 0x8276,
    SigOpCode_lightHSLSetUnacknowledged                      = 0x8277,
    SigOpCode_lightHSLStatus                                 = 0x8278,
    SigOpCode_lightHSLTargetGet                              = 0x8279,
    SigOpCode_lightHSLTargetStatus                           = 0x827A,
    SigOpCode_lightHSLDefaultGet                             = 0x827B,
    SigOpCode_lightHSLDefaultStatus                          = 0x827C,
    SigOpCode_lightHSLRangeGet                               = 0x827D,
    SigOpCode_lightHSLRangeStatus                            = 0x827E,
    //Light HSL Setup
    SigOpCode_lightHSLDefaultSet                             = 0x827F,
    SigOpCode_lightHSLDefaultSetUnacknowledged               = 0x8280,
    SigOpCode_lightHSLRangeSet                               = 0x8281,
    SigOpCode_lightHSLRangeSetUnacknowledged                 = 0x82,

    //Light xyL
    SigOpCode_lightXyLGet                                    = 0x8283,
    SigOpCode_lightXyLSet                                    = 0x8284,
    SigOpCode_lightXyLSetUnacknowledged                      = 0x8285,
    SigOpCode_lightXyLStatus                                 = 0x8286,
    SigOpCode_lightXyLTargetGet                              = 0x8287,
    SigOpCode_lightXyLTargetStatus                           = 0x8288,
    SigOpCode_lightXyLDefaultGet                             = 0x8289,
    SigOpCode_lightXyLDefaultStatus                          = 0x828A,
    SigOpCode_lightXyLRangeGet                               = 0x828B,
    SigOpCode_lightXyLRangeStatus                            = 0x828C,
    //Light xyL Setup
    SigOpCode_lightXyLDefaultSet                             = 0x828D,
    SigOpCode_lightXyLDefaultSetUnacknowledged               = 0x828E,
    SigOpCode_lightXyLRangeSet                               = 0x828F,
    SigOpCode_lightXyLRangeSetUnacknowledged                 = 0x8290,

    //Light Control
    SigOpCode_LightLCModeGet                                 = 0x8291,
    SigOpCode_LightLCModeSet                                 = 0x8292,
    SigOpCode_LightLCModeSetUnacknowledged                   = 0x8293,
    SigOpCode_LightLCModeStatus                              = 0x8294,
    SigOpCode_LightLCOMGet                                   = 0x8295,
    SigOpCode_LightLCOMSet                                   = 0x8296,
    SigOpCode_LightLCOMSetUnacknowledged                     = 0x8297,
    SigOpCode_LightLCOMStatus                                = 0x8298,
    SigOpCode_LightLCLightOnOffGet                           = 0x8299,
    SigOpCode_LightLCLightOnOffSet                           = 0x829A,
    SigOpCode_LightLCLightOnOffSetUnacknowledged             = 0x829B,
    SigOpCode_LightLCLightOnOffStatus                        = 0x829C,
    SigOpCode_LightLCPropertyGet                             = 0x829D,


    /// 3.1.3.1 BLOB Transfer messages
    /// - seeAlso : MshMDL_BLOB_CR_Vienna_IOP.pdf  (page.35)

    //BLOB Transfer Messages
    /// 0x83 0x00 BLOB Transfer Get
    SigOpCode_BLOBTransferGet                                = 0x8300,
    /// 0x83 0x01 BLOB Transfer Start
    SigOpCode_BLOBTransferStart                              = 0x8301,
    /// 0x83 0x02 BLOB Transfer Cancel
    SigOpCode_BLOBTransferCancel                             = 0x8302,
    /// 0x83 0x03 BLOB Transfer Status
    SigOpCode_BLOBTransferStatus                             = 0x8303,
    /// 0x83 0x04 BLOB Block Start
    SigOpCode_BLOBBlockStart                                 = 0x8304,
    /// 0x83 0x05 BLOB Block Get
    SigOpCode_BLOBBlockGet                                   = 0x8305,
    /// 0x83 0x06 BLOB Information Get
    SigOpCode_BLOBInformationGet                             = 0x8306,
    /// 0x83 0x07 BLOB Information Status
    SigOpCode_BLOBInformationStatus                          = 0x8307,

    /// 3.1.1 Firmware Update Model Messages
    /// - seeAlso : pre-spec OTA model opcode details.pdf  (page.2)
    /// 8.4 Firmware update messages
    /// - seeAlso : MshMDL_DFU_MBT_CR_R04_LbL25.pdf  (page.80)

    //8.4.1 Firmware Update model messages
    /// 0x83 0x08 Firmware Update Information Get
    SigOpCode_FirmwareUpdateInformationGet                   = 0x8308,
    /// 0x83 0x09 Firmware Update Information Status
    SigOpCode_FirmwareUpdateInformationStatus                = 0x8309,
    /// 0x83 0x0A Firmware Update Firmware Metadata Check
    SigOpCode_FirmwareUpdateFirmwareMetadataCheck            = 0x830A,
    /// 0x83 0x0B Firmware Update Firmware Metadata Status
    SigOpCode_FirmwareUpdateFirmwareMetadataStatus           = 0x830B,
    /// 0x83 0x0C Firmware Update Get
    SigOpCode_FirmwareUpdateGet                              = 0x830C,
    /// 0x83 0x0D Firmware Update Start
    SigOpCode_FirmwareUpdateStart                            = 0x830D,
    /// 0x83 0x0E Firmware Update Cancel
    SigOpCode_FirmwareUpdateCancel                           = 0x830E,
    /// 0x83 0x0F Firmware Update Apply
    SigOpCode_FirmwareUpdateApply                            = 0x830F,
    /// 0x83 0x10 Firmware Update Status
    SigOpCode_FirmwareUpdateStatus                           = 0x8310,
    //8.4.2 Firmware Distribution model messages
    /// 0x83 0x11 Firmware Distribution Receivers Add
    SigOpCode_FirmwareDistributionReceiversAdd               = 0x8311,
    /// 0x83 0x12 Firmware Distribution Receivers Delete All
    SigOpCode_FirmwareDistributionReceiversDeleteAll         = 0x8312,
    /// 0x83 0x13 Firmware Distribution Receivers Status
    SigOpCode_FirmwareDistributionReceiversStatus            = 0x8313,
    /// 0x83 0x14 Firmware Distribution Receivers Get
    SigOpCode_FirmwareDistributionReceiversGet               = 0x8314,
    /// 0x83 0x12 Firmware Distribution Receivers List
    SigOpCode_FirmwareDistributionReceiversList              = 0x8315,
    /// 0x83 0x16 Firmware Distribution Capabilities Get
    SigOpCode_FirmwareDistributionCapabilitiesGet            = 0x8316,
    /// 0x83 0x17 Firmware Distribution Capabilities Status
    SigOpCode_FirmwareDistributionCapabilitiesStatus         = 0x8317,
    /// 0x83 0x18 Firmware Distribution Get
    SigOpCode_FirmwareDistributionGet                        = 0x8318,
    /// 0x83 0x19 Firmware Distribution Start
    SigOpCode_FirmwareDistributionStart                      = 0x8319,
    /// 0x83 0x1A Firmware Distribution Suspend
    SigOpCode_FirmwareDistributionSuspend                    = 0x831A,
    /// 0x83 0x1B Firmware Distribution Cancel
    SigOpCode_FirmwareDistributionCancel                     = 0x831B,
    /// 0x83 0x1C Firmware Distribution Apply
    SigOpCode_FirmwareDistributionApply                      = 0x831C,
    /// 0x83 0x1D Firmware Distribution Status
    SigOpCode_FirmwareDistributionStatus                     = 0x831D,
    /// 0x83 0x1E Firmware Distribution Upload Get
    SigOpCode_FirmwareDistributionUploadGet                  = 0x831E,
    /// 0x83 0x1F Firmware Distribution Upload Start
    SigOpCode_FirmwareDistributionUploadStart                = 0x831F,
    /// 0x83 0x20 Firmware Distribution Upload OOB Start
    SigOpCode_FirmwareDistributionUploadOOBStart             = 0x8320,
    /// 0x83 0x21 Firmware Distribution Upload Cancel
    SigOpCode_FirmwareDistributionUploadCancel               = 0x8321,
    /// 0x83 0x22 Firmware Distribution Upload Status
    SigOpCode_FirmwareDistributionUploadStatus               = 0x8322,
    /// 0x83 0x23 Firmware Distribution Firmware Get
    SigOpCode_FirmwareDistributionFirmwareGet                = 0x8323,
    /// 0x83 0x24 Firmware Distribution Firmware Get By Index
    SigOpCode_FirmwareDistributionFirmwareGetByIndex         = 0x8324,
    /// 0x83 0x25 Firmware Distribution Firmware Delete
    SigOpCode_FirmwareDistributionFirmwareDelete             = 0x8325,
    /// 0x83 0x26 Firmware Distribution Firmware Delete All
    SigOpCode_FirmwareDistributionFirmwareDeleteAll          = 0x8326,
    /// 0x83 0x27 Firmware Distribution Firmware Status
    SigOpCode_FirmwareDistributionFirmwareStatus             = 0x8327,

    /// - seeAlso : fast provision流程简介.pdf  (page.1)

    /// fast provision
    SigOpCode_VendorID_MeshResetNetwork                      = 0xC5,
    SigOpCode_VendorID_MeshAddressGet                        = 0xC6,
    SigOpCode_VendorID_MeshAddressGetStatus                  = 0xC7,
    SigOpCode_VendorID_MeshAddressSet                        = 0xC8,
    SigOpCode_VendorID_MeshAddressSetStatus                  = 0xC9,
    SigOpCode_VendorID_MeshProvisionDataSet                  = 0xCA,
    SigOpCode_VendorID_MeshProvisionConfirm                  = 0xCB,
    SigOpCode_VendorID_MeshProvisionConfirmStatus            = 0xCC,
    SigOpCode_VendorID_MeshProvisionComplete                 = 0xCD,

} SigOpCode;

typedef enum : UInt8 {
    /// The segmented message has not been acknowledged before the timeout occurred.
    SigLowerTransportError_timeout = 0,
    /// The target device is busy at the moment and could not accept the message.
    SigLowerTransportError_busy    = 1,
} SigLowerTransportError;

/// Table 3.11: CTL field message types and NetMIC sizes
/// - seeAlso: MshPRT_v1.1.pdf (page.61)
typedef enum : UInt8 {
    SigLowerTransportPduType_accessMessage           = 0,
    SigLowerTransportPduType_transportControlMessage = 1,
} SigLowerTransportPduType;

typedef enum : UInt8 {
    /// Provisioning Manager is ready to start.
    ProvisioningState_ready,
    /// The manager is requesting Provisioioning Capabilities from the device.
    ProvisioningState_requestingCapabilities,
    /// Provisioning Capabilities were received.
    ProvisioningState_capabilitiesReceived,
    /// Provisioning has been started.
    ProvisioningState_provisioning,
    /// The provisioning process is complete.
    ProvisioningState_complete,
    /// The provisioning has failed because of a local error.
    ProvisioningState_fail,
    /// The manager is requesting Provisioning Records Get.
    ProvisioningState_recordsGet,
    /// Provisioning Records List were received.
    ProvisioningState_recordsList,
    /// The manager is requesting Provisioning Record.
    ProvisioningState_recordRequest,
    /// Provisioning Record Response were received.
    ProvisioningState_recordResponse,
} ProvisioningState;

/// Table 5.38: Provisioning error codes.
/// - seeAlso: Mesh_v1.0.pdf  (page.244)
typedef enum : UInt8 {
    /// Prohibited.
    ProvisioningError_prohibited            = 0,
    /// The provisioning protocol PDU is not recognized by the device.
    ProvisioningError_invalidPdu            = 1,
    /// The arguments of the protocol PDUs are outside expected values or the length of the PDU is different than expected.
    ProvisioningError_invalidFormat         = 2,
    /// The PDU received was not expected at this moment of the procedure.
    ProvisioningError_unexpectedPDU         = 3,
    /// The computed confirmation value was not successfully verified.
    ProvisioningError_confirmationFailed    = 4,
    /// The provisioning protocol cannot be continued due to insufficient resources in the device.
    ProvisioningError_outOfResources        = 5,
    /// The Data block was not successfully decrypted.
    ProvisioningError_decryptionFailed      = 6,
    /// An unexpected error occurred that may not be recoverable.
    ProvisioningError_unexpectedError       = 7,
    /// The device cannot assign consecutive unicast addresses to all elements.
    ProvisioningError_cannotAssignAddresses = 8,
    /// The Data block contains values that cannot be accepted because of general constraints.
    ProvisioningError_invalidData           = 9,//add in mesh v1.1
    /// RFU, Reserved for Future Use, 0x0A–0xFF.
} ProvisioningError;

typedef enum : NSUInteger {
    /// The provisioning protocol PDU is not recognized by the device.
    RemoteProvisioningError_invalidPdu             = 1,
    /// The arguments of the protocol PDUs are outside expected values
    /// or the length of the PDU is different than expected.
    RemoteProvisioningError_invalidFormat          = 2,
    /// The PDU received was not expected at this moment of the procedure.
    RemoteProvisioningError_unexpectedPdu          = 3,
    /// The computed confirmation value was not successfully verified.
    RemoteProvisioningError_confirmationFailed     = 4,
    /// The provisioning protocol cannot be continued due to insufficient
    /// resources in the device.
    RemoteProvisioningError_outOfResources         = 5,
    /// The Data block was not successfully decrypted.
    RemoteProvisioningError_decryptionFailed       = 6,
    /// An unexpected error occurred that may not be recoverable.
    RemoteProvisioningError_unexpectedError        = 7,
    /// The device cannot assign consecutive unicast addresses to all elements.
    RemoteProvisioningError_cannotAssignAddresses = 8,
} RemoteProvisioningError;

typedef enum : NSUInteger {
    /// Message will be sent with 32-bit Transport MIC.
    SigMeshMessageSecurityLow,
    /// Message will be sent with 64-bit Transport MIC.
    /// Unsegmented messages cannot be sent with this option.
    SigMeshMessageSecurityHigh,
} SigMeshMessageSecurity;

/// Locations defined by Bluetooth SIG.
/// Imported from: https://www.bluetooth.com/specifications/assigned-numbers/gatt-namespace-descriptors
typedef enum : UInt16 {
    SigLocation_auxiliary                   = 0x0108,
    SigLocation_back                        = 0x0101,
    SigLocation_backup                      = 0x0107,
    SigLocation_bottom                      = 0x0103,
    SigLocation_eighteenth                  = 0x0012,
    SigLocation_eighth                      = 0x0008,
    SigLocation_eightieth                   = 0x0050,
    SigLocation_eightyEighth                = 0x0058,
    SigLocation_eightyFifth                 = 0x0055,
    SigLocation_eightyFirst                 = 0x0051,
    SigLocation_eightyFourth                = 0x0054,
    SigLocation_eightyNinth                 = 0x0059,
    SigLocation_eightySecond                = 0x0052,
    SigLocation_eightySeventh               = 0x0057,
    SigLocation_eightySixth                 = 0x0056,
    SigLocation_eightyThird                 = 0x0053,
    SigLocation_eleventh                    = 0x000b,
    SigLocation_external                    = 0x0110,
    SigLocation_fifteenth                   = 0x000f,
    SigLocation_fifth                       = 0x0005,
    SigLocation_fiftieth                    = 0x0032,
    SigLocation_fiftyEighth                 = 0x003a,
    SigLocation_fiftyFifth                  = 0x0037,
    SigLocation_fiftyFirst                  = 0x0033,
    SigLocation_fiftyFourth                 = 0x0036,
    SigLocation_fiftyNinth                  = 0x003b,
    SigLocation_fiftySecond                 = 0x0034,
    SigLocation_fiftySeventh                = 0x0039,
    SigLocation_fiftySixth                  = 0x0038,
    SigLocation_fiftyThird                  = 0x0035,
    SigLocation_first                       = 0x0001,
    SigLocation_flash                       = 0x010A,
    SigLocation_fortieth                    = 0x0028,
    SigLocation_fourteenth                  = 0x000e,
    SigLocation_fourth                      = 0x0004,
    SigLocation_fortyEighth                 = 0x0030,
    SigLocation_fortyFifth                  = 0x002d,
    SigLocation_fortyFirst                  = 0x0029,
    SigLocation_fortyFourth                 = 0x002c,
    SigLocation_fortyNinth                  = 0x0031,
    SigLocation_fortySecond                 = 0x002a,
    SigLocation_fortySeventh                = 0x002f,
    SigLocation_fortySixth                  = 0x002e,
    SigLocation_fortyThird                  = 0x002b,
    SigLocation_front                       = 0x0100,
    SigLocation_inside                      = 0x010B,
    SigLocation_internal                    = 0x010F,
    SigLocation_left                        = 0x010D,
    SigLocation_lower                       = 0x0105,
    SigLocation_main                        = 0x0106,
    SigLocation_nineteenth                  = 0x0013,
    SigLocation_ninth                       = 0x0009,
    SigLocation_ninetieth                   = 0x005a,
    SigLocation_ninetyEighth                = 0x0062,
    SigLocation_ninetyFifth                 = 0x005f,
    SigLocation_ninetyFirst                 = 0x005b,
    SigLocation_ninetyFourth                = 0x005e,
    SigLocation_ninetyNinth                 = 0x0063,
    SigLocation_ninetySecond                = 0x005c,
    SigLocation_ninetySeventh               = 0x0061,
    SigLocation_ninetySixth                 = 0x0060,
    SigLocation_ninetyThird                 = 0x005d,
    SigLocation_oneHundredAndEighteenth     = 0x0076,
    SigLocation_oneHundredAndEighth         = 0x006c,
    SigLocation_oneHundredAndEightyEighth   = 0x00bc,
    SigLocation_oneHundredAndEightyFifth    = 0x00b9,
    SigLocation_oneHundredAndEightyFirst    = 0x00b5,
    SigLocation_oneHundredAndEightyFourth   = 0x00b8,
    SigLocation_oneHundredAndEightyNinth    = 0x00bd,
    SigLocation_oneHundredAndEightySecond   = 0x00b6,
    SigLocation_oneHundredAndEightySeventh  = 0x00bb,
    SigLocation_oneHundredAndEightySixth    = 0x00ba,
    SigLocation_oneHundredAndEightyThird    = 0x00b7,
    SigLocation_oneHundredAndEleventh       = 0x006f,
    SigLocation_oneHundredAndFifteenth      = 0x0073,
    SigLocation_oneHundredAndFifth          = 0x0069,
    SigLocation_oneHundredAndFiftyEighth    = 0x009e,
    SigLocation_oneHundredAndFiftyFifth     = 0x009b,
    SigLocation_oneHundredAndFiftyFirst     = 0x0097,
    SigLocation_oneHundredAndFiftyFourth    = 0x009a,
    SigLocation_oneHundredAndFiftyNinth     = 0x009f,
    SigLocation_oneHundredAndFiftySecond    = 0x0098,
    SigLocation_oneHundredAndFiftySeventh   = 0x009d,
    SigLocation_oneHundredAndFiftySixth     = 0x009c,
    SigLocation_oneHundredAndFiftyThird     = 0x0099,
    SigLocation_oneHundredAndFirst          = 0x0065,
    SigLocation_oneHundredAndFourteenth     = 0x0072,
    SigLocation_oneHundredAndFourth         = 0x0068,
    SigLocation_oneHundredAndFortyEighth    = 0x0094,
    SigLocation_oneHundredAndFortyFifth     = 0x0091,
    SigLocation_oneHundredAndFortyFirst     = 0x008d,
    SigLocation_oneHundredAndFortyFourth    = 0x0090,
    SigLocation_oneHundredAndFortyNinth     = 0x0095,
    SigLocation_oneHundredAndFortySecond    = 0x008e,
    SigLocation_oneHundredAndFortySeventh   = 0x0093,
    SigLocation_oneHundredAndFortySixth     = 0x0092,
    SigLocation_oneHundredAndFortyThird     = 0x008f,
    SigLocation_oneHundredAndNineteenth     = 0x0077,
    SigLocation_oneHundredAndNinth          = 0x006d,
    SigLocation_oneHundredAndNinetyEighth   = 0x00c6,
    SigLocation_oneHundredAndNinetyFifth    = 0x00c3,
    SigLocation_oneHundredAndNinetyFirst    = 0x00bf,
    SigLocation_oneHundredAndNinetyFourth   = 0x00c2,
    SigLocation_oneHundredAndNinetyNinth    = 0x00c7,
    SigLocation_oneHundredAndNinetySecond   = 0x00c0,
    SigLocation_oneHundredAndNinetySeventh  = 0x00c5,
    SigLocation_oneHundredAndNinetySixth    = 0x00c4,
    SigLocation_oneHundredAndNinetyThird    = 0x00c1,
    SigLocation_oneHundredAndSecond         = 0x0066,
    SigLocation_oneHundredAndSeventeenth    = 0x0075,
    SigLocation_oneHundredAndSeventh        = 0x006b,
    SigLocation_oneHundredAndSeventyEighth  = 0x00b2,
    SigLocation_oneHundredAndSeventyFifth   = 0x00af,
    SigLocation_oneHundredAndSeventyFirst   = 0x00ab,
    SigLocation_oneHundredAndSeventyFourth  = 0x00ae,
    SigLocation_oneHundredAndSeventyNinth   = 0x00b3,
    SigLocation_oneHundredAndSeventySecond  = 0x00ac,
    SigLocation_oneHundredAndSeventySeventh = 0x00b1,
    SigLocation_oneHundredAndSeventySixth   = 0x00b0,
    SigLocation_oneHundredAndSeventyThird   = 0x00ad,
    SigLocation_oneHundredAndSixteenth      = 0x0074,
    SigLocation_oneHundredAndSixth          = 0x006a,
    SigLocation_oneHundredAndSixtyEighth    = 0x00a8,
    SigLocation_oneHundredAndSixtyFifth     = 0x00a5,
    SigLocation_oneHundredAndSixtyFirst     = 0x00a1,
    SigLocation_oneHundredAndSixtyFourth    = 0x00a4,
    SigLocation_oneHundredAndSixtyNinth     = 0x00a9,
    SigLocation_oneHundredAndSixtySecond    = 0x00a2,
    SigLocation_oneHundredAndSixtySeventh   = 0x00a7,
    SigLocation_oneHundredAndSixtySixth     = 0x00a6,
    SigLocation_oneHundredAndSixtyThird     = 0x00a3,
    SigLocation_oneHundredAndTenth          = 0x006e,
    SigLocation_oneHundredAndThird          = 0x0067,
    SigLocation_oneHundredAndThirteenth     = 0x0071,
    SigLocation_oneHundredAndThirtyEighth   = 0x008a,
    SigLocation_oneHundredAndThirtyFifth    = 0x0087,
    SigLocation_oneHundredAndThirtyFirst    = 0x0083,
    SigLocation_oneHundredAndThirtyFourth   = 0x0086,
    SigLocation_oneHundredAndThirtyNinth    = 0x008b,
    SigLocation_oneHundredAndThirtySecond   = 0x0084,
    SigLocation_oneHundredAndThirtySeventh  = 0x0089,
    SigLocation_oneHundredAndThirtySixth    = 0x0088,
    SigLocation_oneHundredAndThirtyThird    = 0x0085,
    SigLocation_oneHundredAndTwelveth       = 0x0070,
    SigLocation_oneHundredAndTwentyEighth   = 0x0080,
    SigLocation_oneHundredAndTwentyFifth    = 0x007d,
    SigLocation_oneHundredAndTwentyFirst    = 0x0079,
    SigLocation_oneHundredAndTwentyFourth   = 0x007c,
    SigLocation_oneHundredAndTwentyNinth    = 0x0081,
    SigLocation_oneHundredAndTwentySecond   = 0x007a,
    SigLocation_oneHundredAndTwentySeventh  = 0x007f,
    SigLocation_oneHundredAndTwentySixth    = 0x007e,
    SigLocation_oneHundredAndTwentyThird    = 0x007b,
    SigLocation_oneHundredEightieth         = 0x00b4,
    SigLocation_oneHundredFiftieth          = 0x0096,
    SigLocation_oneHundredFortieth          = 0x008c,
    SigLocation_oneHundredNinetieth         = 0x00be,
    SigLocation_oneHundredSeventieth        = 0x00aa,
    SigLocation_oneHundredSixtieth          = 0x00a0,
    SigLocation_oneHundredThirtieth         = 0x0082,
    SigLocation_oneHundredTwentieth         = 0x0078,
    SigLocation_oneHundredth                = 0x0064,
    SigLocation_outside                     = 0x010C,
    SigLocation_right                       = 0x010E,
    SigLocation_second                      = 0x0002,
    SigLocation_seventeenth                 = 0x0011,
    SigLocation_seventh                     = 0x0007,
    SigLocation_seventieth                  = 0x0046,
    SigLocation_seventyEighth               = 0x004e,
    SigLocation_seventyFifth                = 0x004b,
    SigLocation_seventyFirst                = 0x0047,
    SigLocation_seventyFourth               = 0x004a,
    SigLocation_seventyNinth                = 0x004f,
    SigLocation_seventySecond               = 0x0048,
    SigLocation_seventySeventh              = 0x004d,
    SigLocation_seventySixth                = 0x004c,
    SigLocation_seventyThird                = 0x0049,
    SigLocation_sixteenth                   = 0x0010,
    SigLocation_sixth                       = 0x0006,
    SigLocation_sixtieth                    = 0x003c,
    SigLocation_sixtyEighth                 = 0x0044,
    SigLocation_sixtyFifth                  = 0x0041,
    SigLocation_sixtyFirst                  = 0x003d,
    SigLocation_sixtyFourth                 = 0x0040,
    SigLocation_sixtyNinth                  = 0x0045,
    SigLocation_sixtySecond                 = 0x003e,
    SigLocation_sixtySeventh                = 0x0043,
    SigLocation_sixtySixth                  = 0x0042,
    SigLocation_sixtyThird                  = 0x003f,
    SigLocation_supplementary               = 0x0109,
    SigLocation_tenth                       = 0x000a,
    SigLocation_third                       = 0x0003,
    SigLocation_thirteenth                  = 0x000d,
    SigLocation_thirtieth                   = 0x001e,
    SigLocation_thirtyEighth                = 0x0026,
    SigLocation_thirtyFifth                 = 0x0023,
    SigLocation_thirtyFirst                 = 0x001f,
    SigLocation_thirtyFourth                = 0x0022,
    SigLocation_thirtyNinth                 = 0x0027,
    SigLocation_thirtySecond                = 0x0020,
    SigLocation_thirtySeventh               = 0x0025,
    SigLocation_thirtySixth                 = 0x0024,
    SigLocation_thirtyThird                 = 0x0021,
    SigLocation_top                         = 0x0102,
    SigLocation_twelveth                    = 0x000c,
    SigLocation_twentieth                   = 0x0014,
    SigLocation_twentyEighth                = 0x001c,
    SigLocation_twentyFifth                 = 0x0019,
    SigLocation_twentyFirst                 = 0x0015,
    SigLocation_twentyFourth                = 0x0018,
    SigLocation_twentyNinth                 = 0x001d,
    SigLocation_twentySecond                = 0x0016,
    SigLocation_twentySeventh               = 0x001b,
    SigLocation_twentySixth                 = 0x001a,
    SigLocation_twentyThird                 = 0x0017,
    SigLocation_twoHundredAndEighteenth     = 0x00da,
    SigLocation_twoHundredAndEighth         = 0x00d0,
    SigLocation_twoHundredAndEleventh       = 0x00d3,
    SigLocation_twoHundredAndFifteenth      = 0x00d7,
    SigLocation_twoHundredAndFifth          = 0x00cd,
    SigLocation_twoHundredAndFiftyFifth     = 0x00ff,
    SigLocation_twoHundredAndFiftyFirst     = 0x00fb,
    SigLocation_twoHundredAndFiftyFourth    = 0x00fe,
    SigLocation_twoHundredAndFiftySecond    = 0x00fc,
    SigLocation_twoHundredAndFiftyThird     = 0x00fd,
    SigLocation_twoHundredAndFirst          = 0x00c9,
    SigLocation_twoHundredAndFourteenth     = 0x00d6,
    SigLocation_twoHundredAndFourth         = 0x00cc,
    SigLocation_twoHundredAndFortyEighth    = 0x00f8,
    SigLocation_twoHundredAndFortyFifth     = 0x00f5,
    SigLocation_twoHundredAndFortyFirst     = 0x00f1,
    SigLocation_twoHundredAndFortyFourth    = 0x00f4,
    SigLocation_twoHundredAndFortyNinth     = 0x00f9,
    SigLocation_twoHundredAndFortySecond    = 0x00f2,
    SigLocation_twoHundredAndFortySeventh   = 0x00f7,
    SigLocation_twoHundredAndFortySixth     = 0x00f6,
    SigLocation_twoHundredAndFortyThird     = 0x00f3,
    SigLocation_twoHundredAndNineteenth     = 0x00db,
    SigLocation_twoHundredAndNinth          = 0x00d1,
    SigLocation_twoHundredAndSecond         = 0x00ca,
    SigLocation_twoHundredAndSeventeenth    = 0x00d9,
    SigLocation_twoHundredAndSeventh        = 0x00cf,
    SigLocation_twoHundredAndSixteenth      = 0x00d8,
    SigLocation_twoHundredAndSixth          = 0x00ce,
    SigLocation_twoHundredAndTenth          = 0x00d2,
    SigLocation_twoHundredAndThird          = 0x00cb,
    SigLocation_twoHundredAndThirteenth     = 0x00d5,
    SigLocation_twoHundredAndThirtyEighth   = 0x00ee,
    SigLocation_twoHundredAndThirtyFifth    = 0x00eb,
    SigLocation_twoHundredAndThirtyFirst    = 0x00e7,
    SigLocation_twoHundredAndThirtyFourth   = 0x00ea,
    SigLocation_twoHundredAndThirtyNinth    = 0x00ef,
    SigLocation_twoHundredAndThirtySecond   = 0x00e8,
    SigLocation_twoHundredAndThirtySeventh  = 0x00ed,
    SigLocation_twoHundredAndThirtySixth    = 0x00ec,
    SigLocation_twoHundredAndThirtyThird    = 0x00e9,
    SigLocation_twoHundredAndTwelveth       = 0x00d4,
    SigLocation_twoHundredAndTwentyEighth   = 0x00e4,
    SigLocation_twoHundredAndTwentyFifth    = 0x00e1,
    SigLocation_twoHundredAndTwentyFirst    = 0x00dd,
    SigLocation_twoHundredAndTwentyFourth   = 0x00e0,
    SigLocation_twoHundredAndTwentyNinth    = 0x00e5,
    SigLocation_twoHundredAndTwentySecond   = 0x00de,
    SigLocation_twoHundredAndTwentySeventh  = 0x00e3,
    SigLocation_twoHundredAndTwentySixth    = 0x00e2,
    SigLocation_twoHundredAndTwentyThird    = 0x00df,
    SigLocation_twoHundredFiftieth          = 0x00fa,
    SigLocation_twoHundredFortieth          = 0x00f0,
    SigLocation_twoHundredThirtieth         = 0x00e6,
    SigLocation_twoHundredTwentieth         = 0x00dc,
    SigLocation_twoHundredth                = 0x00c8,
    SigLocation_unknown                     = 0x0000,
    SigLocation_upper                       = 0x0104,
} SigLocation;

/// Table 3.6: Generic OnPowerUp states.
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.31)
typedef enum : UInt8 {
    /// Off. After being powered up, the element is in an off state.
    SigOnPowerUpOff     = 0x00,
    /// Default. After being powered up, the element is in an On state and uses default state values.
    SigOnPowerUpDefault = 0x01,
    /// Restore. If a transition was in progress when powered down,
    /// the element restores the target state when powered up.
    /// Otherwise the element restores the state it was in when powered down.
    SigOnPowerUpRestore = 0x02,
} SigOnPowerUp;

/// Table 7.2: Summary of status codes.
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.303)
typedef enum : UInt8 {
    /// Command successfully processed.
    SigGenericMessageStatusSuccess           = 0x00,
    /// The provided value for Range Min cannot be set.
    SigGenericMessageStatusCannotSetRangeMin = 0x01,
    /// The provided value for Range Max cannot be set.
    SigGenericMessageStatusCannotSetRangeMax = 0x02,
    /// Reserved for Future Use, RFU, 0x03–0xFF.
} SigGenericMessageStatus;

/// Table 3.15: Generic Battery Flags Presence states.
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.35)
typedef enum : UInt8 {
    /// The battery is not present.
    SigBatteryPresenceNotPresent   = 0b00,
    /// The battery is present and is removable.
    SigBatteryPresenceRemovable    = 0b01,
    /// The battery is present and is non-removable.
    SigBatteryPresenceNotRemovable = 0b10,
    /// The battery presence is unknown.
    SigBatteryPresenceUnknown      = 0b11,
} SigBatteryPresence;

/// Table 3.16: Generic Battery Flags Indicator states.
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.35)
typedef enum : UInt8 {
    /// The battery charge is Critically Low Level.
    SigBatteryIndicatorCriticallyLow = 0b00,
    /// The battery charge is Low Level.
    SigBatteryIndicatorLow           = 0b01,
    /// The battery charge is Good Level.
    SigBatteryIndicatorGood          = 0b10,
    /// The battery charge is unknown.
    SigBatteryIndicatorUnknown       = 0b11,
} SigBatteryIndicator;

/// Table 3.17: Generic Battery Flags Charging states.
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.35)
typedef enum : UInt8 {
    /// The battery is not chargeable.
    SigBatteryChargingStateNotChargeable = 0b00,
    /// The battery is chargeable and is not charging.
    SigBatteryChargingStateNotCharging   = 0b01,
    /// The battery is chargeable and is charging.
    SigBatteryChargingStateCharging      = 0b10,
    /// The battery charging state is unknown.
    SigBatteryChargingStateUnknown       = 0b11,
} SigBatteryChargingState;

/// Table 3.18: Generic Battery Flags Serviceability states.
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.35)
typedef enum : UInt8 {
    /// Reserved for Future Use
    SigBatteryServiceabilityReservedForFutureUse = 0b00,
    /// The battery does not require service.
    SigBatteryServiceabilityServiceNotRequired   = 0b01,
    /// The battery requires service.
    SigBatteryServiceabilityServiceRequired      = 0b10,
    /// The battery serviceability is unknown.
    SigBatteryServiceabilityUnknown              = 0b11,
} SigBatteryServiceability;

/// 5.1.2 Time Role
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.136)
typedef enum : UInt8 {
    /// The element does not participate in propagation of time information.
    SigTimeRoleNode              = 0x00,
    /// The element publishes Time Status messages but does not process received Time Status messages.
    SigTimeRoleMeshTimeAuthority = 0x01,
    /// The element processes received and publishes Time Status messages.
    SigTimeRoleMeshTimeRelay     = 0x02,
    /// The element does not publish but processes received Time Status messages.
    SigTimeRoleMeshTimeClient    = 0x03,
} SigTimeRole;

/// 5.2.2.11 Summary of status codes
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.149)
typedef enum : UInt8 {
    /// Success
    SigSceneResponseStatus_success           = 0x00,
    /// Scene Register Full
    SigSceneResponseStatus_sceneRegisterFull = 0x01,
    /// Scene Not Found
    SigSceneResponseStatus_sceneNotFound     = 0x02,
    /// Reserved for Future Use, 0x03–0xFF.
} SigSceneResponseStatus;

/// 4.1.1.4 Sensor Sampling Function
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.106)
typedef enum : UInt8 {
    SigSensorSamplingFunctionType_unspecified    = 0x00,
    SigSensorSamplingFunctionType_instantaneous  = 0x01,
    SigSensorSamplingFunctionType_arithmeticMean = 0x02,
    SigSensorSamplingFunctionType_RMS            = 0x03,
    SigSensorSamplingFunctionType_maximum        = 0x04,
    SigSensorSamplingFunctionType_minimum        = 0x05,
    SigSensorSamplingFunctionType_accumulated    = 0x06,
    SigSensorSamplingFunctionType_count          = 0x07,
} SigSensorSamplingFunctionType;

/// 4.1.2.3 Sensor Setting Access
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.109)
typedef enum : UInt8 {
    //The device property can be read.
    SigSensorSettingAccessType_read      = 0x01,
    //The device property can be read and written.
    SigSensorSettingAccessType_readWrite = 0x03,
} SigSensorSettingAccessType;

/// 4.2.8 Relay
/// - seeAlso: Mesh_v1.0.pdf  (page.140)
typedef enum : UInt8 {
    //The node support Relay feature that is disabled.
    SigNodeRelayState_notEnabled   = 0,
    //The node supports Relay feature that is enabled.
    SigNodeRelayState_enabled      = 1,
    //Relay feature is not supported.
    SigNodeRelayState_notSupported = 2,
} SigNodeRelayState;

/// 4.2.10 SecureNetworkBeacon
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.141)
typedef enum : UInt8 {
    //The node is broadcasting a Secure Network beacon.
    SigSecureNetworkBeaconState_close = 0,
    //The node is not broadcasting a Secure Network beacon.
    SigSecureNetworkBeaconState_open  = 1,
} SigSecureNetworkBeaconState;

/// 4.2.11 GATTProxy
/// - seeAlso: Mesh_v1.0.pdf  (page.141)
typedef enum : UInt8 {
    //The Mesh Proxy Service is running, Proxy feature is disabled.
    SigNodeGATTProxyState_notEnabled   = 0,
    //The Mesh Proxy Service is running, Proxy feature is enabled.
    SigNodeGATTProxyState_enabled      = 1,
    //The Mesh Proxy Service is not supported, Proxy feature is not supported.
    SigNodeGATTProxyState_notSupported = 2,
} SigNodeGATTProxyState;

/// 4.2.12 NodeIdentity
/// - seeAlso: Mesh_v1.0.pdf  (page.142)
typedef enum : UInt8 {
    //Node Identity for a subnet is stopped.
    SigNodeIdentityState_notEnabled   = 0,
    //Node Identity for a subnet is running.
    SigNodeIdentityState_enabled      = 1,
    //Node Identity is not supported.
    SigNodeIdentityState_notSupported = 2,
} SigNodeIdentityState;

/// 4.2.13 Friend
/// - seeAlso: Mesh_v1.0.pdf  (page.142)
typedef enum : UInt8 {
    //The node supports Friend feature that is disabled.
    SigNodeFeaturesState_notEnabled   = 0,
    //The node supports Friend feature that is enabled.
    SigNodeFeaturesState_enabled      = 1,
    //The Friend feature is not supported.
    SigNodeFeaturesState_notSupported = 2,
} SigNodeFeaturesState;

/// 4.2.14 Key Refresh Phase.Table 4.17: Key Refresh Phase state values.
/// - seeAlso: Mesh_v1.0.pdf  (page.143)
typedef enum : UInt8 {
    /// Phase 0: Normal Operation.
    normalOperation  = 0,
    /// Phase 1: Distributing new keys to all nodes. Nodes will transmit using
    /// old keys, but can receive using old and new keys.
    distributingKeys = 1,
    /// Phase 2: Transmitting a Secure Network beacon that signals to the network
    /// that all nodes have the new keys. The nodes will then transmit using
    /// the new keys but can receive using the old or new keys.
    finalizing       = 2,
} KeyRefreshPhase;

/// Table 6.5: Proxy Configuration message opcodes
/// - seeAlso: Mesh_v1.0.pdf  (page.263)
typedef enum : UInt8 {
    /// Sent by a Proxy Client to set the proxy filter type.
    SigProxyFilerOpcode_setFilterType                   = 0x00,
    /// Sent by a Proxy Client to add addresses to the proxy filter list.
    SigProxyFilerOpcode_addAddressesToFilter            = 0x01,
    /// Sent by a Proxy Client to remove addresses from the proxy filter list.
    SigProxyFilerOpcode_removeAddressesFromFilter       = 0x02,
    /// Acknowledgment by a Proxy Server to a Proxy Client to report
    /// the status of the proxy filter list.
    SigProxyFilerOpcode_filterStatus                    = 0x03,
    /// Acknowledgment by a Directed Proxy Server to report current
    /// Directed Proxy capabilities in a subnet.
    SigProxyFilerOpcode_directedProxyCapabilitiesStatus = 0x04,
    /// sent by a Directed Proxy Client to set whether or not the Directed Proxy Server uses
    /// directed forwarding for Directed Proxy Client messages for a specified range of unicast addresses.
    SigProxyFilerOpcode_directedProxyControl            = 0x05,
} SigProxyFilerOpcode;

/// Table 6.7: FilterType Values
/// - seeAlso: Mesh_v1.0.pdf  (page.263)
typedef enum : UInt8 {
    /// A white list filter has an associated white list, which is a list of destination addresses that are
    /// of interest for the Proxy Client. The white list filter blocks all destination addresses except
    /// those that have been added to the white list.
    SigProxyFilerType_whitelist = 0x00,
    /// A black list filter has an associated black list, which is a list of destination addresses that the
    /// Proxy Client does not want to receive. The black list filter accepts all destination addresses
    /// except those that have been added to the black list.
    SigProxyFilerType_blacklist = 0x01,
} SigProxyFilerType;

/// Firmware Distribution Status Values
/// - seeAlso: Mesh_Firmware_update_20180228_d05r05.pdf  (page.24)
typedef enum : UInt8 {
    /// ready, distribution is not active
    SigFirmwareDistributionStatusType_notActive             = 0x00,
    /// distribution is active
    SigFirmwareDistributionStatusType_active                = 0x01,
    /// no such Company ID and Firmware ID combin
    SigFirmwareDistributionStatusType_noSuchId              = 0x02,
    /// busy with different distribution
    SigFirmwareDistributionStatusType_busyWithDifferent     = 0x03,
    /// update nodes list is too long
    SigFirmwareDistributionStatusType_updateNodeListTooLong = 0x04,
} SigFirmwareDistributionStatusType;

/// Update status code values:
/// - seeAlso: Mesh_Firmware_update_20180228_d05r05.pdf  (page.25)
typedef enum : UInt8 {
    /// successfully updated
    SigUpdateStatusType_success    = 0x00,
    /// in progress
    SigUpdateStatusType_inProgress = 0x01,
    /// canceled
    SigUpdateStatusType_cancel     = 0x02,
} SigUpdateStatusType;

/// Table 8.17: Update Policy state values
/// - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf  (page.87)
typedef enum : UInt8 {
    /// The Firmware Distribution Server verifies that firmware image distribution completed successfully
    /// but does not apply the update. The Initiator (the Firmware Distribution Client) initiates firmware
    /// image application.
    SigUpdatePolicyType_verifyOnly     = 0x00,
    /// The Firmware Distribution Server verifies that firmware image distribution completed successfully
    /// and then applies the firmware update.
    SigUpdatePolicyType_verifyAndApply = 0x01,
} SigUpdatePolicyType;

/// Firmware Update Status Values
/// - seeAlso: Mesh_Firmware_update_20180228_d05r05.pdf  (page.28)
typedef enum : UInt8 {
    /// success
    SigFirmwareUpdateStatusType_success                 = 0x00,
    /// wrong Company ID and Firmware ID combination
    SigFirmwareUpdateStatusType_IdCombinationWrong      = 0x01,
    /// different object transfer already ongoing
    SigFirmwareUpdateStatusType_busyWithDifferentObject = 0x02,
    /// Company ID and Firmware ID combination apply failed
    SigFirmwareUpdateStatusType_IdCombinationApplyFail  = 0x03,
    /// Company ID and Firmware ID combination permanently rejected, newer firmware version present
    SigFirmwareUpdateStatusType_combinationAlwaysReject = 0x04,
    /// Company ID and Firmware ID combination temporary rejected, node is not able to accept new firmware now, try again later
    SigFirmwareUpdateStatusType_combinationTempReject   = 0x05,
} SigFirmwareUpdateStatusType;

/// The Block Checksum Algorithm values
/// - seeAlso: Mesh_Firmware_update_20180228_d05r05.pdf  (page.31)
typedef enum : UInt8 {
    /// Details TBD, Block Checksum Value len is 4.
    SigBlockChecksumAlgorithmType_CRC32 = 0x00,
    /// Reserved for Future Use, 0x01-0xFF
} SigBlockChecksumAlgorithmType;

/// Table 8.22: Status codes for the Firmware Update Server model and Firmware Update Client model
/// - seeAlso: MshMDL_DFU_MBT_CR_R04_LbL25.pdf  (page.80)
typedef enum : UInt8 {
    /// The message was processed successfully.
    SigFirmwareUpdateServerAndClientModelStatusType_success                = 0x00,
    /// Insufficient resources on the node.
    SigFirmwareUpdateServerAndClientModelStatusType_insufficientResources  = 0x01,
    /// The operation cannot be performed while the server is in the current phase.
    SigFirmwareUpdateServerAndClientModelStatusType_wrongPhase             = 0x02,
    /// An internal error occurred on the node.
    SigFirmwareUpdateServerAndClientModelStatusType_internalError          = 0x03,
    /// The message contains a firmware index value that is not expected.
    SigFirmwareUpdateServerAndClientModelStatusType_wrongFirmwareIndex     = 0x04,
    /// The metadata check failed.
    SigFirmwareUpdateServerAndClientModelStatusType_metadataCheckFailed    = 0x05,
    /// The server cannot start a firmware update.
    SigFirmwareUpdateServerAndClientModelStatusType_temporarilyUnavailable = 0x06,
    /// Another BLOB transfer is in progress.
    SigFirmwareUpdateServerAndClientModelStatusType_BLOBTransferBusy       = 0x07,
} SigFirmwareUpdateServerAndClientModelStatusType;

/// Table 8.24: Status codes for the Firmware Distribution Server model and Firmware Distribution Client model
/// - seeAlso: MshMDL_DFU_MBT_CR_R06  (page.92)
typedef enum : UInt8 {
    /// The message was processed successfully.
    SigFirmwareDistributionServerAndClientModelStatusType_success               = 0x00,
    /// Insufficient resources on the node.
    SigFirmwareDistributionServerAndClientModelStatusType_insufficientResources = 0x01,
    /// The operation cannot be performed while the server is in the current phase.
    SigFirmwareDistributionServerAndClientModelStatusType_wrongPhase            = 0x02,
    /// An internal error occurred on the node.
    SigFirmwareDistributionServerAndClientModelStatusType_internalError         = 0x03,
    /// The requested firmware image is not stored on the Distributor.
    SigFirmwareDistributionServerAndClientModelStatusType_firmwareNotFound      = 0x04,
    /// The AppKey identified by the AppKey Index is not known to the node.
    SigFirmwareDistributionServerAndClientModelStatusType_invalidAppKeyIndex    = 0x05,
    /// There are no Updating nodes in the Distribution Receivers List state.
    SigFirmwareDistributionServerAndClientModelStatusType_receiversListEmpty    = 0x06,
    /// Another firmware image distribution is in progress.
    SigFirmwareDistributionServerAndClientModelStatusType_busyWithDistribution  = 0x07,
    /// Another upload is in progress.
    SigFirmwareDistributionServerAndClientModelStatusType_busyWithUpload        = 0x08,
    /// The URI scheme name indicated by the Update URI is not supported.
    SigFirmwareDistributionServerAndClientModelStatusType_URINotSupported       = 0x09,
    /// The format of the Update URI is invalid.
    SigFirmwareDistributionServerAndClientModelStatusType_URIMalformed          = 0x0A,
    /// Reserved For Future Use, 0x0B–0xFF.
} SigFirmwareDistributionServerAndClientModelStatusType;

/// Table 8.18: Upload Phase state values
/// - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf  (page.89)
typedef enum       : UInt8 {
    /// No firmware upload is in progress.
    SigFirmwareUploadPhaseStateType_idle              = 0x00,
    /// The Store Firmware procedure is being executed.
    SigFirmwareUploadPhaseStateType_transferActive    = 0x01,
//    /// The Store Firmware OOB procedure is being executed.
//    SigFirmwareUploadPhaseStateType_OOBTransferActive = 0x02,
    /// The Store Firmware procedure or Store Firmware OOB procedure failed.
    SigFirmwareUploadPhaseStateType_transferError     = 0x02,
    /// The Store Firmware procedure or the Store Firmware OOB procedure completed successfully.
    SigFirmwareUploadPhaseStateType_transferSuccess   = 0x03,
    /// Prohibited : 0x05–0xFF
} SigFirmwareUploadPhaseStateType;

/// Table 8.8: Firmware Update Additional Information state values
/// - seeAlso: MshMDL_DFU_MBT_CR_R04_LbL25.pdf  (page.71)
typedef enum                    : UInt8 {
    /// No changes to node composition data.
    SigFirmwareUpdateAdditionalInformationStatusType_noChangeCompositionData              = 0x00,
    /// Node composition data changed. The node does not support remote provisioning.
    SigFirmwareUpdateAdditionalInformationStatusType_changeCompositionDataUnsupportedRemote = 0x01,
    /// Node composition data changed, and remote provisioning is supported. The node supports remote provisioning
    /// and composition data page 0x80. Page 0x80 contains different composition data than page 0x0.
    SigFirmwareUpdateAdditionalInformationStatusType_changeCompositionDataSupportRemote   = 0x02,
    /// Node unprovisioned. The node is unprovisioned after successful application of a verified firmware image.
    SigFirmwareUpdateAdditionalInformationStatusType_nodeUnprovisioned                    = 0x03,
    /// Reserved for Future Use : 0x4–0x1F
} SigFirmwareUpdateAdditionalInformationStatusType;

/// Table 4.8: Retrieved Update Phase field values
/// - seeAlso: MshDFU_v1.0.pdf  (page.32)
typedef enum : UInt8 {
    /// Ready to start a Receive Firmware procedure..
    SigFirmwareUpdatePhaseType_idle                = 0x00,
    /// The Transfer BLOB procedure failed.
    SigFirmwareUpdatePhaseType_transferError       = 0x01,
    /// The Receive Firmware procedure is being executed.
    SigFirmwareUpdatePhaseType_transferActive      = 0x02,
    /// The Verify Firmware procedure is being executed.
    SigFirmwareUpdatePhaseType_verifyingUpdate     = 0x03,
    /// The Verify Firmware procedure completed successfully.
    SigFirmwareUpdatePhaseType_verificationSuccess = 0x04,
    /// The Verify Firmware procedure failed.
    SigFirmwareUpdatePhaseType_verificationFailed  = 0x05,
    /// The Apply New Firmware procedure is being executed.
    SigFirmwareUpdatePhaseType_applyingUpdate      = 0x06,
    /// Firmware transfer has been canceled.
    SigFirmwareUpdatePhaseType_transferCanceled    = 0x07,
    /// Firmware applying succeeded
    SigFirmwareUpdatePhaseType_applySuccess        = 0x08,
    /// Firmware applying failed.
    SigFirmwareUpdatePhaseType_applyFailed         = 0x09,
    /// Phase of a node was not yet retrieved
    SigFirmwareUpdatePhaseType_unknown             = 0x0A,
    /// RFU. 0xB–0xF.
} SigFirmwareUpdatePhaseType;

/// Table 3.10 Status codes used by the BLOB Transfer models
/// - seeAlso: MshMDL_BLOB_CR_Vienna_IOP.pdf  (page.16)
typedef enum : UInt8 {
    /// The message was processed successfully.
    SigBLOBBlockStatusType_success                 = 0x00,
    /// The Block Number field value is not within range.
    SigBLOBBlockStatusType_invalidBlockNumber      = 0x01,
    /// The block size is lower than the size represented by Min Block Size Log,
    /// or the block size is higher than the size represented by Max Block Size Log.
    SigBLOBBlockStatusType_wrongBlockSize          = 0x02,
    /// Chunk size exceeds the size represented by Max Chunk Size,
    /// or the number of chunks exceeds the number specified by Max Chunks Number.
    SigBLOBBlockStatusType_wrongChunkSize          = 0x03,
    /// The model is in a state where it cannot process the message.
    SigBLOBBlockStatusType_invalidState            = 0x04,
    /// A parameter value in the message cannot be accepted.
    SigBLOBBlockStatusType_invalidParameter        = 0x05,
    /// The requested BLOB ID is not expected.
    SigBLOBBlockStatusType_wrongBLOBID             = 0x06,
    /// There is not enough space available in memory to receive the BLOB.
    SigBLOBBlockStatusType_BLOBTooLarge            = 0x07,
    /// The transfer mode is not supported by the BLOB Transfer Server model.
    SigBLOBBlockStatusType_unsupportedTransferMode = 0x08,
    /// An internal error occurred on the node.
    SigBLOBBlockStatusType_internalError           = 0x09,
    /// Prohibited, 0xA-0xF
} SigBLOBBlockStatusType;

/// Table 3.16 Format field enumeration values
/// - seeAlso: MshMDL_BLOB_CR_Vienna_IOP.pdf  (page.20)
typedef enum : UInt8 {
    /// All chunks in the block are missing.
    SigBLOBBlockFormatType_allChunksMissing     = 0x00,
    /// All chunks in the block have been received.
    SigBLOBBlockFormatType_noMissingChunks      = 0x01,
    /// At least one chunk has been received and at least one chunk is missing.
    SigBLOBBlockFormatType_someChunksMissing    = 0x02,
    /// List of chunks requested by the server.
    SigBLOBBlockFormatType_encodedMissingChunks = 0x03,
} SigBLOBBlockFormatType;

/// Table 7.17 Status codes used by the BLOB Transfer Server and the BLOB Transfer Client models
/// - seeAlso: MshMDL_DFU_MBT_CR_R06  (page.27)
typedef enum       : UInt8 {
    /// The message was processed successfully.
    SigBLOBTransferStatusType_success                 = 0x00,
    /// The Block Number field value is not within the range of blocks being transferred.
    SigBLOBTransferStatusType_invalidBlockNumber      = 0x01,
    /// The block size is smaller than the size indicated by the Min Block Size Log state
    /// or is larger than the size indicated by the Max Block Size Log state.
    SigBLOBTransferStatusType_invalidBlockSize        = 0x02,
    /// The chunk size exceeds the size indicated by the Max Chunk Size state,
    /// or the number of chunks exceeds the number specified by the Max Total Chunks state.
    SigBLOBTransferStatusType_invalidChunkSize        = 0x03,
    /// The operation cannot be performed while the server is in the current phase.
    SigBLOBTransferStatusType_wrongPhase              = 0x04,
    /// A parameter value in the message cannot be accepted.
    SigBLOBTransferStatusType_invalidParameter        = 0x05,
    /// The message contains a BLOB ID value that is not expected.
    SigBLOBTransferStatusType_wrongBLOBID             = 0x06,
    /// There is not enough space available in memory to receive the BLOB.
    SigBLOBTransferStatusType_BLOBTooLarge            = 0x07,
    /// The transfer mode is not supported by the BLOB Transfer Server model.
    SigBLOBTransferStatusType_unsupportedTransferMode = 0x08,
    /// An internal error occurred on the node.
    SigBLOBTransferStatusType_internalError           = 0x09,
    /// The requested information cannot be provided while the server is in the current phase.
    SigBLOBTransferStatusType_informationUnavailable  = 0x0A,
    /// Prohibited : 0xB–0xF
} SigBLOBTransferStatusType;

/// Table 7.5: Transfer Mode state values
/// - seeAlso: MshMDL_DFU_MBT_CR_R04_LbL25.pdf  (page.21)
typedef enum : UInt8 {
    /// No Active Transfer.
    SigTransferModeState_noActiveTransfer     = 0x00,
    /// Push BLOB Transfer Mode (see Section 7.1.1.1).
    SigTransferModeState_pushBLOBTransferMode = 0x01,
    /// Pull BLOB Transfer Mode (see Section 7.1.1.1).
    SigTransferModeState_pullBLOBTransferMode = 0x02,
    /// Prohibited.
    SigTransferModeState_prohibited           = 0x03,
} SigTransferModeState;

/// Table 7.6: Transfer Phase state values
/// - seeAlso: MshMDL_DFU_MBT_CR_R04_LbL25.pdf  (page.22)
typedef enum       : UInt8 {
    /// The BLOB Transfer Server is awaiting configuration and cannot receive a BLOB.
    SigTransferPhaseState_inactive                = 0x00,
    /// The BLOB Transfer Server is ready to receive the BLOB identified by the Expected BLOB ID.
    SigTransferPhaseState_waitingForTransferStart = 0x01,
    /// The BLOB Transfer Server is waiting for the next block of data.
    SigTransferPhaseState_waitingForNextBlock     = 0x02,
    /// The BLOB Transfer Server is waiting for the next chunk of data.
    SigTransferPhaseState_waitingForNextChunk     = 0x03,
    /// The BLOB was transferred successfully.
    SigTransferPhaseState_complete                = 0x04,
    /// The Initialize and Receive BLOB procedure is paused.
    SigTransferPhaseState_suspended               = 0x05,
    /// Prohibited : 0x06–0xFF
} SigTransferPhaseState;

/// Table 8.16: Distribution Phase state values
/// - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf  (page.87)
typedef enum       : UInt8 {
    /// No firmware distribution is in progress.
    SigDistributionPhaseState_idle            = 0x00,
    /// Firmware distribution is in progress.
    SigDistributionPhaseState_transferActive  = 0x01,
    /// The Transfer BLOB procedure has completed successfully.
    SigDistributionPhaseState_transferSuccess = 0x02,
    /// The Apply Firmware on Updating Nodes procedure is being executed.
    SigDistributionPhaseState_applyingUpdate  = 0x03,
    /// The Distribute Firmware procedure has completed successfully.
    SigDistributionPhaseState_completed       = 0x04,
    /// The Distribute Firmware procedure has failed.
    SigDistributionPhaseState_failed          = 0x05,
    /// The Cancel Firmware Update procedure is being executed.
    SigDistributionPhaseState_cancelingUpdate = 0x06,
    /// Prohibited : 0x07–0xFF
} SigDistributionPhaseState;

/// Table 4.22 defines status codes for Remote Provisioning Server messages that contain a status code.
/// - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.docx  (page.27)
typedef enum : UInt8 {
    SigRemoteProvisioningStatus_success                                    = 0x00,
    SigRemoteProvisioningStatus_scanningCannotStart                        = 0x01,
    SigRemoteProvisioningStatus_invalidState                               = 0x02,
    SigRemoteProvisioningStatus_limitedResources                           = 0x03,
    SigRemoteProvisioningStatus_linkCannotOpen                             = 0x04,
    SigRemoteProvisioningStatus_linkOpenFailed                             = 0x05,
    SigRemoteProvisioningStatus_linkClosedByDevice                         = 0x06,
    SigRemoteProvisioningStatus_linkClosedByServer                         = 0x07,
    SigRemoteProvisioningStatus_linkClosedByClient                         = 0x08,
    SigRemoteProvisioningStatus_linkClosedAsCannotReceivePDU               = 0x09,
    SigRemoteProvisioningStatus_linkClosedAsCannotSendPDU                  = 0x0A,
    SigRemoteProvisioningStatus_linkClosedAsCannotDeliverPDUReport         = 0x0B,
    SigRemoteProvisioningStatus_linkClosedAsCannotDeliverPDUOutboundReport = 0x0C,
    /// Reserved for Future Use, 0x0D-0xFF
} SigRemoteProvisioningStatus;

/// Table 4.15: Reason field values for a Remote Provisioning Link Close message
/// - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.docx  (page.23)
typedef enum : UInt8 {
    /// The provisioning or Device Key Refresh procedure completed successfully.
    SigRemoteProvisioningLinkCloseStatus_success    = 0x00,
    /// Prohibited
    SigRemoteProvisioningLinkCloseStatus_prohibited = 0x01,
    /// The provisioning or Device Key Refresh procedure failed.
    SigRemoteProvisioningLinkCloseStatus_fail       = 0x02,
    /// Reserved for Future Use, 0x03-0xFF
} SigRemoteProvisioningLinkCloseStatus;

/// Fast provision status
/// - seeAlso: fast provision流程简介.pdf  (page.1)
typedef enum : UInt8 {
    SigFastProvisionStatus_idle            = 0x00,
    SigFastProvisionStatus_start           = 0x01,
    SigFastProvisionStatus_resetNetwork    = 0x02,
    SigFastProvisionStatus_getAddress      = 0x03,
    SigFastProvisionStatus_getAddressRetry = 0x04,
    SigFastProvisionStatus_setAddress      = 0x05,
    SigFastProvisionStatus_setNetworkInfo  = 0x06,
    SigFastProvisionStatus_confirm         = 0x07,
    SigFastProvisionStatus_confirmOk       = 0x08,
    SigFastProvisionStatus_complete        = 0x09,
    SigFastProvisionStatus_timeout         = 0x0A,
} SigFastProvisionStatus;

/// Table 4.18: Controllable Key Refresh transition values
/// - seeAlso: Mesh_v1.0.pdf  (page.143)
typedef enum : UInt8 {
    SigControllableKeyRefreshTransitionValues_two   = 0x02,
    SigControllableKeyRefreshTransitionValues_three = 0x03,
} SigControllableKeyRefreshTransitionValues;

/// Table 4.Y+0: Subnet Bridge state values (The default value of the Subnet Bridge state shall be 0x00.)
/// - seeAlso: MshPRF_SBR_CR_r03.pdf  (page.8)
typedef enum : UInt8 {
    /// Subnet bridge functionality is disabled.
    SigSubnetBridgeStateValues_disabled = 0x00,
    /// Subnet bridge functionality is enabled.
    SigSubnetBridgeStateValues_enabled  = 0x01,
    /// Prohibited, 0x02–0xFF.
} SigSubnetBridgeStateValues;

/// Table 4.Y+2: Directions field values
/// - seeAlso: MshPRF_SBR_CR_r03.pdf  (page.9)
typedef enum : UInt8 {
    /// Prohibited.
    SigDirectionsFieldValues_prohibited     = 0x00,
    /// Bridging is allowed only for messages with Address1 as the source address
    /// and Address2 as the destination address. (单向)
    SigDirectionsFieldValues_unidirectional = 0x01,
    /// Bridging is allowed for messages with Address1 as the source address and
    /// Address2 as the destination address, and messages with Address2 as
    /// the source address and Address1 as the destination address. (双向)
    SigDirectionsFieldValues_bidirectional  = 0x02,
    /// Prohibited, 0x03–0xFF.
} SigDirectionsFieldValues;

/// Table 4.Y+10: Filter field values
/// - seeAlso: MshPRF_SBR_CR_r03.pdf  (page.13)
typedef enum : UInt8 {
    /// Report all pairs of NetKey Indexes extracted from the Bridging Table state entries.
    SigFilterFieldValues_all                   = 0b00,
    /// Report pairs of NetKey Indexes extracted from the Bridging Table state entries
    /// with the NetKey Index of the first subnet that matches the NetKeyIndex field value.
    SigFilterFieldValues_first                 = 0b01,
    /// Report pairs of NetKey Indexes extracted from the Bridging Table state entries
    /// with the NetKey Index of the second subnet that matches the NetKeyIndex field value.
    SigFilterFieldValues_second                = 0b10,
    /// Report pairs of NetKey Indexes extracted from the Bridging Table state entries
    /// with one of the NetKey Indexes that matches the NetKeyIndex field.
    SigFilterFieldValues_netKeyIndexes         = 0b11,
} SigFilterFieldValues;

/// Table 5.45: Status codes for the Provisioning Record Response PDU
/// - seeAlso: MshPRFd1.1r11_clean.pdf.pdf  (page.496)
typedef enum : UInt8 {
    /// Success.
    SigProvisioningRecordResponseStatus_success                      = 0x00,
    /// Requested Record Is Not Present.
    SigProvisioningRecordResponseStatus_requestedRecordIsNotPresent  = 0x01,
    /// Requested Offset Is Out Of Bounds.
    SigProvisioningRecordResponseStatus_requestedOffsetIsOutOfBounds = 0x02,
    /// 0x03–0xFF Reserved for Future Use.
} SigProvisioningRecordResponseStatus;

typedef enum : UInt8 {
    /// BTM_ECDH_P256_CMAC_AES128_AES_CCM，如果设备端不支持该算法就会出现provision失败。
    SigFipsP256EllipticCurve_CMAC_AES128 = 0x01,
    /// BTM_ECDH_P256_HMAC_SHA256_AES_CCM，如果设备端不支持该算法就会出现provision失败。
    SigFipsP256EllipticCurve_HMAC_SHA256 = 0x02,
    /// APP端根据从设备端实时读取数据来判定使用哪一种算法进行provision流程，如果设备端同时支持多种算法，默认使用value最大的算法进行provision。
    SigFipsP256EllipticCurve_auto        = 0xFF,
} SigFipsP256EllipticCurve;

/// telink私有定义的Extend Bearer Mode，SDK默认是使用0，特殊用户需要用到2。
typedef enum : UInt8 {
    /// segment发包中的单个分包的UpperTransportPDU最大长度都是标准sig定义的12字节。
    SigTelinkExtendBearerMode_noExtend         = 0x00,
    /// 非直连节点使用的是上述标准sig定义的12字节。直连节点使用SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength最为单个分包的UpperTransportPDU最大长度。
    SigTelinkExtendBearerMode_extendGATTOnly   = 0x01,
    /// segment发包中的单个分包的UpperTransportPDU最大长度都是telink自定义的SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength。
    SigTelinkExtendBearerMode_extendGATTAndAdv = 0x02,
} SigTelinkExtendBearerMode;

/// Table 4.295: Summary of status codes for Opcodes Aggregator messages
/// - seeAlso: MshPRFd1.1r13_clean.pdf  (page.418)
typedef enum : UInt8 {
    /// Success
    SigOpcodesAggregatorMessagesStatus_success              = 0,
    /// The unicast address provided in the Element_Address field is not known to the node.
    SigOpcodesAggregatorMessagesStatus_invalidAddress       = 1,
    /// The model identified by the Element_Address field and Item #0 opcode is not found in the identified element.
    SigOpcodesAggregatorMessagesStatus_invalidModel         = 2,
    /// 1.The message is encrypted with an application key, and the identified model is not bound to the message’s
    /// application key, or the identified model’s access layer security is not using application keys.
    /// 2.The message is encrypted with a device key, and the identified model’s access layer security is not using a device key.
    SigOpcodesAggregatorMessagesStatus_wrongAccessKey       = 3,
    /// At least one of the items from the message request list contains an opcode that is not supported by the identified model.
    SigOpcodesAggregatorMessagesStatus_wrongOpCode          = 4,
    /// An access message has a valid opcode but is not understood by the identified model (see Section 3.7.4.4)
    SigOpcodesAggregatorMessagesStatus_messageNotUnderstood = 5,
    /// Reserved for Future Use, 0x06–0xFF.
} SigOpcodesAggregatorMessagesStatus;

/// Table 3.109: Day of Week field
/// - seeAlso: GATT_Specification_Supplement_v5.pdf  (page.105)
typedef enum : UInt8 {
    GattDayOfWeek_Unknown   = 0,
    GattDayOfWeek_Monday    = 1,
    GattDayOfWeek_Tuesday   = 2,
    GattDayOfWeek_Wednesday = 3,
    GattDayOfWeek_Thursday  = 4,
    GattDayOfWeek_Friday    = 5,
    GattDayOfWeek_Saturday  = 6,
    GattDayOfWeek_Sunday    = 7,
    //Reserved for Future Use 8–255
} GattDayOfWeek;

/// Table 7.8: Identification Type values
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.634)
typedef enum : UInt8 {
    SigIdentificationType_networkID              = 0,
    SigIdentificationType_nodeIdentity           = 1,
    SigIdentificationType_privateNetworkIdentity = 2,
    SigIdentificationType_privateNodeIdentity    = 3,
    //Reserved for Future Use 0x04–0xFF
} SigIdentificationType;

/// Table 4.65: Private Beacon state
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.276)
typedef enum : UInt8 {
    SigPrivateBeaconState_disable = 0,
    SigPrivateBeaconState_enable  = 1,
    //Prohibited 0x02–0xFF
} SigPrivateBeaconState;

/// Table 4.67: Private GATT Proxy state values
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.280)
typedef enum : UInt8 {
    SigPrivateGattProxyState_disable      = 0,
    SigPrivateGattProxyState_enable       = 1,
    SigPrivateGattProxyState_notSupported = 2,
    //Prohibited 0x03–0xFF
} SigPrivateGattProxyState;

/// Table 4.68: Private Node Identity values
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.280)
typedef enum : UInt8 {
    //Node Identity for a subnet is stopped.
    SigPrivateNodeIdentityState_notEnabled   = 0,
    //Node Identity for a subnet is running.
    SigPrivateNodeIdentityState_enabled      = 1,
    //Node Identity is not supported.
    SigPrivateNodeIdentityState_notSupported = 2,
    //0x03–0xFF Prohibited
} SigPrivateNodeIdentityState;

/// app端发送的beacon类型
typedef enum : UInt8 {
    AppSendBeaconType_auto,//根据设备返回的beacon类型来返回
    AppSendBeaconType_secureNetwork,//强制发送secureNetworkBeacon
    AppSendBeaconType_meshPrivateBeacon,//强制发送meshPrivateBeacon
} AppSendBeaconType;

/// v3.3.3.6新增Telink自定义PID结构体
typedef enum : UInt8 {
    CHIP_TYPE_8258 = 0,
    CHIP_TYPE_8278 = 1,
    CHIP_TYPE_8269 = 2,
    CHIP_TYPE_9518 = 3,
} CHIP_TYPE;
typedef enum : UInt8 {
    MajorProductType_light     = 0,
    MajorProductType_gateway   = 1,
    MajorProductType_LPN       = 2,
    MajorProductType_switch    = 3,
    MajorProductType_spiritLPN = 4,
} MajorProductType;
/// 1.3 LIGHT_TYPE_SEL 介绍
/// - seeAlso: AN-17120401-C_Telink SIG Mesh SDK Developer Handbook.pdf  (page.29)
typedef enum : UInt8 {
    SigNodePID_NONE = 0,
    /// CT 是色温灯的简写，对应的产品类型是色温灯，包含色温相关的 model，
    /// 比如 Light CTL Server，Light CTL SetupServer，LightCTLTemperatureServer
    /// 以及对应的extendmodel，比如:GenericOnOffServer，Generic Level Server，Light Lightness Server 等。
    SigNodePID_CT = 1,
    /// 对应的产品类型是彩色灯 (HSL 灯)，包含 Light HSL Server，Light HSL Hue Server，Light HSL Saturation Server，Light HSL Setup Server，
    /// 以及对应的 extend model，比如:Generic OnOff Server，Generic Level Server，Light Lightness Server 等。
    SigNodePID_HSL = 2,
    /// 对应的产品类型是 XYL 灯，包含 Light xyL Server，Light xyL Setup Server，
    /// 以及对应的 extend model，比 如:Generic OnOff Server，Generic Level Server，Light Lightness Server 等。
    SigNodePID_XYL = 3,
    /// 对应的产品类型是 power adapter 等，包含 Generic Power Level Server，Generic Power Level Setup Server， 
    /// 以及对应的 extend model，比如:Generic OnOff Server，Generic Level Server 等。
    SigNodePID_POWER = 4,
    /// 对应的产品类型是色温灯 + 彩色灯 (HSL 灯)，包含色温以及 HSL 对应的 model，
    /// 以及 Generic OnOff Server， Generic Level Server，Light Lightness Server 等，
    /// 其中色温灯珠和 HSL 灯珠共用一个 lightness 和 onoff 值。 另外，同一时间只有一种灯珠在点亮。
    SigNodePID_CT_HSL = 5,
    /// 对应的产品类型是调光灯，包含 Light Lightness Server，Light Lightness Setup Server 
    /// 以及对应的 extend model，比如:Generic OnOff Server，Generic Level Server 等。
    SigNodePID_DIM = 6,// only single PWM
    /// 对应的产品类型是开关面板，该面板处于 server ⻆色，也就是被 app 等设备控制并执行 onoff 切换。
    /// 包含 Generic OnOff Server model。默认开关个数是 3 个(由 LIGHT_CNT 定义)。
    SigNodePID_PANEL = 7,// only ON/OFF model
    /// 对应的产品类型是 LPN 设备，默认包含 Generic OnOff Server model 等，mesh OTA model 关闭。
    /// 主要用于 demo LPN 的功能。
    SigNodePID_LPN_ON_OFF_LEVEL = 8,// only ON/OFF  , LEVEL model
    /// 对应的产品类型是客戶定制的产品。
    SigNodePID_BRUSH = 9,
    /// 对应的产品类型是 Mesh V1.1 的 NLC feature，详⻅此章节DICNLCP。
    SigNodePID_NLC_CTRL_CLIENT = 10,
    /// 对应的产品类型是 Mesh V1.1 的 NLC feature，详⻅此章节OCSSNLCP，ALSNLCP 和ENMNLCP。
    SigNodePID_NLC_SENSOR = 11,
} SigNodePID;

/// 3.3 Property identifiers
/// - seeAlso: Device_Properties.pdf  (page.53)
typedef enum : UInt16 {
    // 0x0000 Prohibited
    /// Average Ambient Temperature in a Period of Day
    DevicePropertyID_AverageAmbientTemperatureIn_a_PeriodOfDay = 0x0001,
    /// Average Input Current
    DevicePropertyID_AverageInputCurrent = 0x0002,
    /// Average Input Voltage
    DevicePropertyID_AverageInputVoltage = 0x0003,
    /// Average Output Current
    DevicePropertyID_AverageOutputCurrent = 0x0004,
    /// Average Output Voltage
    DevicePropertyID_AverageOutputVoltage = 0x0005,
    /// Center Beam Intensity at Full Power
    DevicePropertyID_CenterBeamIntensityAtFullPower = 0x0006,
    /// Chromaticity Tolerance
    DevicePropertyID_ChromaticityTolerance = 0x0007,
    /// Color Rendering Index R9
    DevicePropertyID_ColorRenderingIndexR9 = 0x0008,
    /// Color Rendering Index Ra
    DevicePropertyID_ColorRenderingIndexRa = 0x0009,
    /// Device Appearance
    DevicePropertyID_DeviceAppearance = 0x000A,
    /// Device Country of Origin
    DevicePropertyID_DeviceCountryOfOrigin = 0x000B,
    /// Device Date of Manufacture
    DevicePropertyID_DeviceDateOfManufacture = 0x000C,
    /// Device Energy Use Since Turn On
    DevicePropertyID_DeviceEnergyUseSinceTurnOn = 0x000D,
    /// Device Firmware Revision
    DevicePropertyID_DeviceFirmwareRevision = 0x000E,
    /// Device Global Trade Item Number
    DevicePropertyID_DeviceGlobalTradeItemNumber = 0x000F,
    /// Device Hardware Revision
    DevicePropertyID_DeviceHardwareRevision = 0x0010,
    /// Device Manufacturer Name
    DevicePropertyID_DeviceManufacturerName = 0x0011,
    /// Device Model Number
    DevicePropertyID_DeviceModelNumber = 0x0012,
    /// Device Operating Temperature Range Specification
    DevicePropertyID_DeviceOperatingTemperatureRangeSpecification = 0x0013,
    /// Device Operating Temperature Statistical Values
    DevicePropertyID_DeviceOperatingTemperatureStatisticalValues = 0x0014,
    /// Device Over Temperature Event Statistics
    DevicePropertyID_DeviceOverTemperatureEventStatistics = 0x0015,
    /// Device Power Range Specification
    DevicePropertyID_DevicePowerRangeSpecification = 0x0016,
    /// Device Runtime Since Turn On
    DevicePropertyID_DeviceRuntimeSinceTurnOn = 0x0017,
    /// Device Runtime Warranty
    DevicePropertyID_DeviceRuntimeWarranty = 0x0018,
    /// Device Serial Number
    DevicePropertyID_DeviceSerialNumber = 0x0019,
    /// Device Software Revision
    DevicePropertyID_DeviceSoftwareRevision = 0x001A,
    /// Device Under Temperature Event Statistics
    DevicePropertyID_DeviceUnderTemperatureEventStatistics = 0x001B,
    /// Indoor Ambient Temperature Statistical Values
    DevicePropertyID_IndoorAmbientTemperatureStatisticalValues = 0x001C,
    /// Initial CIE 1931 Chromaticity Coordinates
    DevicePropertyID_Initial_CIE_1931_ChromaticityCoordinates = 0x001D,
    /// Initial Correlated Color Temperature
    DevicePropertyID_InitialCorrelatedColorTemperature = 0x001E,
    /// Initial Luminous Flux
    DevicePropertyID_InitialLuminousFlux = 0x001F,
    /// Initial Planckian Distance
    DevicePropertyID_InitialPlanckianDistance = 0x0020,
    /// Input Current Range Specification
    DevicePropertyID_InputCurrentRangeSpecification = 0x0021,
    /// Input Current Statistics
    DevicePropertyID_InputCurrentStatistics = 0x0022,
    /// Input Over Current Event Statistics
    DevicePropertyID_InputOverCurrentEventStatistics = 0x0023,
    /// Input Over Ripple Voltage Event Statistics
    DevicePropertyID_InputOverRippleVoltageEventStatistics = 0x0024,
    /// Input Over Voltage Event Statistics
    DevicePropertyID_InputOverVoltageEventStatistics = 0x0025,
    /// Input Under Current Event Statistics
    DevicePropertyID_InputUnderCurrentEventStatistics = 0x0026,
    /// Input Under Voltage Event Statistics
    DevicePropertyID_InputUnderVoltageEventStatistics = 0x0027,
    /// Input Voltage Range Specification
    DevicePropertyID_InputVoltageRangeSpecification = 0x0028,
    /// Input Voltage Ripple Specification
    DevicePropertyID_InputVoltageRippleSpecification = 0x0029,
    /// Input Voltage Statistics
    DevicePropertyID_InputVoltageStatistics = 0x002A,
    /// Light Control Ambient LuxLevel On
    DevicePropertyID_LightControlAmbientLuxLevelOn = 0x002B,
    /// Light Control Ambient LuxLevel Prolong
    DevicePropertyID_LightControlAmbientLuxLevelProlong = 0x002C,
    /// Light Control Ambient LuxLevel Standby
    DevicePropertyID_LightControlAmbientLuxLevelStandby = 0x002D,
    /// Light Control Lightness On
    DevicePropertyID_LightControlLightnessOn = 0x002E,
    /// Light Control Lightness Prolong
    DevicePropertyID_LightControlLightnessProlong = 0x002F,
    /// Light Control Lightness Standby
    DevicePropertyID_LightControlLightnessStandby = 0x0030,
    /// Light Control Regulator Accuracy
    DevicePropertyID_LightControlRegulatorAccuracy = 0x0031,
    /// Light Control Regulator Kid
    DevicePropertyID_LightControlRegulatorKid = 0x0032,
    /// Light Control Regulator Kiu
    DevicePropertyID_LightControlRegulatorKiu = 0x0033,
    /// Light Control Regulator Kpd
    DevicePropertyID_LightControlRegulatorKpd = 0x0034,
    /// Light Control Regulator Kpu
    DevicePropertyID_LightControlRegulatorKpu = 0x0035,
    /// Light Control Time Fade
    DevicePropertyID_LightControlTimeFade = 0x0036,
    /// Light Control Time Fade On
    DevicePropertyID_LightControlTimeFadeOn = 0x0037,
    /// Light Control Time Fade Standby Auto
    DevicePropertyID_LightControlTimeFadeStandbyAuto = 0x0038,
    /// Light Control Time Fade Standby Manual
    DevicePropertyID_LightControlTimeFadeStandbyManual = 0x0039,
    /// Light Control Time Occupancy Delay
    DevicePropertyID_LightControlTimeOccupancyDelay = 0x003A,
    /// Light Control Time Prolong
    DevicePropertyID_LightControlTimeProlong = 0x003B,
    /// Light Control Time Run On
    DevicePropertyID_LightControlTimeRunOn = 0x003C,
    /// Lumen Maintenance Factor
    DevicePropertyID_LumenMaintenanceFactor = 0x003D,
    /// Luminous Efficacy
    DevicePropertyID_LuminousEfficacy = 0x003E,
    /// Luminous Energy Since Turn On
    DevicePropertyID_LuminousEnergySinceTurnOn = 0x003F,
    /// Luminous Exposure
    DevicePropertyID_LuminousExposure = 0x0040,
    /// Luminous Flux Range
    DevicePropertyID_LuminousFluxRange = 0x0041,
    /// Motion Sensed
    DevicePropertyID_MotionSensed = 0x0042,
    /// Motion Threshold
    DevicePropertyID_MotionThreshold = 0x0043,
    /// Open Circuit Event Statistics
    DevicePropertyID_OpenCircuitEventStatistics = 0x0044,
    /// Outdoor Statistical Values
    DevicePropertyID_OutdoorStatisticalValues = 0x0045,
    /// Output Current Range
    DevicePropertyID_OutputCurrentRange = 0x0046,
    /// Output Current Statistics
    DevicePropertyID_OutputCurrentStatistics = 0x0047,
    /// Output Ripple Voltage Specification
    DevicePropertyID_OutputRippleVoltageSpecification = 0x0048,
    /// Output Voltage Range
    DevicePropertyID_OutputVoltageRange = 0x0049,
    /// Output Voltage Statistics
    DevicePropertyID_OutputVoltageStatistics = 0x004A,
    /// Over Output Ripple Voltage Event Statistics
    DevicePropertyID_OverOutputRippleVoltageEventStatistics = 0x004B,
    /// People Count
    DevicePropertyID_PeopleCount = 0x004C,
    /// Presence Detected
    DevicePropertyID_PresenceDetected = 0x004D,
    /// Present Ambient Light Level
    DevicePropertyID_PresentAmbientLightLevel = 0x004E,
    /// Present Ambient Temperature
    DevicePropertyID_PresentAmbientTemperature = 0x004F,
    /// Present CIE 1931 Chromaticity Coordinates
    DevicePropertyID_Present_CIE_1931_ChromaticityCoordinates = 0x0050,
    /// Present Correlated Color Temperature
    DevicePropertyID_PresentCorrelatedColorTemperature = 0x0051,
    /// Present Device Input Power
    DevicePropertyID_PresentDeviceInputPower = 0x0052,
    /// Present Device Operating Efficiency
    DevicePropertyID_PresentDeviceOperatingEfficiency = 0x0053,
    /// Present Device Operating Temperature
    DevicePropertyID_PresentDeviceOperatingTemperature = 0x0054,
    /// Present Illuminance
    DevicePropertyID_PresentIlluminance = 0x0055,
    /// Present Indoor Ambient Temperature
    DevicePropertyID_PresentIndoorAmbientTemperature = 0x0056,
    /// Present Input Current
    DevicePropertyID_PresentInputCurrent = 0x0057,
    /// Present Input Ripple Voltage
    DevicePropertyID_PresentInputRippleVoltage = 0x0058,
    /// Present Input Voltage
    DevicePropertyID_PresentInputVoltage = 0x0059,
    /// Present Luminous Flux
    DevicePropertyID_PresentLuminousFlux = 0x005A,
    /// Present Outdoor Ambient Temperature
    DevicePropertyID_PresentOutdoorAmbientTemperature = 0x005B,
    /// Present Output Current
    DevicePropertyID_PresentOutputCurrent = 0x005C,
    /// Present Output Voltage
    DevicePropertyID_PresentOutputVoltage = 0x005D,
    /// Present Planckian Distance
    DevicePropertyID_PresentPlanckianDistance = 0x005E,
    /// Present Relative Output Ripple Voltage
    DevicePropertyID_PresentRelativeOutputRippleVoltage = 0x005F,
    /// Relative Device Energy Use in a Period of Day
    DevicePropertyID_RelativeDeviceEnergyUseIn_a_PeriodOfDay = 0x0060,
    /// Relative Device Runtime in a Generic Level Range
    DevicePropertyID_RelativeDeviceRuntimeIn_a_GenericLevelRange = 0x0061,
    /// Relative Exposure Time in an Illuminance Range
    DevicePropertyID_RelativeExposureTimeIn_an_IlluminanceRange = 0x0062,
    /// Relative Runtime in a Correlated Color Temperature Range
    DevicePropertyID_RelativeRuntimeIn_a_CorrelatedColorTemperatureRange = 0x0063,
    /// Relative Runtime in a Device Operating Temperature Range
    DevicePropertyID_RelativeRuntimeIn_a_DeviceOperatingTemperatureRange = 0x0064,
    /// Relative Runtime in an Input Current Range
    DevicePropertyID_RelativeRuntimeIn_an_InputCurrentRange = 0x0065,
    /// Relative Runtime in an Input Voltage Range
    DevicePropertyID_RelativeRuntimeIn_an_InputVoltageRange = 0x0066,
    /// Short Circuit Event Statistics
    DevicePropertyID_ShortCircuitEventStatistics = 0x0067,
    /// Time Since Motion Sensed
    DevicePropertyID_TimeSinceMotionSensed = 0x0068,
    /// Time Since Presence Detected
    DevicePropertyID_TimeSincePresenceDetected = 0x0069,
    /// Total Device Energy Use
    DevicePropertyID_TotalDeviceEnergyUse = 0x006A,
    /// Total Device Off On Cycles
    DevicePropertyID_TotalDeviceOffOnCycles = 0x006B,
    /// Total Device Power On Cycles
    DevicePropertyID_TotalDevicePowerOnCycles = 0x006C,
    /// Total Device Power On Time
    DevicePropertyID_TotalDevicePowerOnTime = 0x006D,
    /// Total Device Runtime
    DevicePropertyID_TotalDeviceRuntime = 0x006E,
    /// Total Light Exposure Time
    DevicePropertyID_TotalLightExposureTime = 0x006F,
    /// Total Luminous Energy
    DevicePropertyID_TotalLuminousEnergy = 0x0070,
    /// Desired Ambient Temperature
    DevicePropertyID_DesiredAmbientTemperature = 0x0071,
    /// Precise Total Device Energy Use
    DevicePropertyID_PreciseTotalDeviceEnergyUse = 0x0072,
    /// Power Factor
    DevicePropertyID_PowerFactor = 0x0073,
    /// Sensor Gain
    DevicePropertyID_SensorGain = 0x0074,
    /// Precise Present Ambient Temperature
    DevicePropertyID_PrecisePresentAmbientTemperature = 0x0075,
    /// Present Ambient Relative Humidity
    DevicePropertyID_PresentAmbientRelativeHumidity = 0x0076,
    /// Present Ambient Carbon Dioxide Concentration
    DevicePropertyID_PresentAmbientCarbonDioxideConcentration = 0x0077,
    /// Present Ambient Volatile Organic Compounds Concentration
    DevicePropertyID_PresentAmbientVolatileOrganicCompoundsConcentration = 0x0078,
    /// Present Ambient Noise
    DevicePropertyID_PresentAmbientNoise = 0x0079,
    /// Active Energy Loadside
    DevicePropertyID_ActiveEnergyLoadside = 0x0080,
    /// Active Power Loadside
    DevicePropertyID_ActivePowerLoadside = 0x0081,
    /// Air Pressure
    DevicePropertyID_AirPressure = 0x0082,
    /// Apparent Energy
    DevicePropertyID_ApparentEnergy = 0x0083,
    /// Apparent Power
    DevicePropertyID_ApparentPower = 0x0084,
    /// Apparent Wind Direction
    DevicePropertyID_ApparentWindDirection = 0x0085,
    /// Apparent Wind Speed
    DevicePropertyID_ApparentWindSpeed = 0x0086,
    /// Dew Point
    DevicePropertyID_DewPoint = 0x0087,
    /// External Supply Voltage
    DevicePropertyID_ExternalSupplyVoltage = 0x0088,
    /// External Supply Voltage Frequency
    DevicePropertyID_ExternalSupplyVoltageFrequency = 0x0089,
    /// Gust Factor
    DevicePropertyID_GustFactor = 0x008A,
    /// Heat Index
    DevicePropertyID_HeatIndex = 0x008B,
    /// Light Distribution
    DevicePropertyID_LightDistribution = 0x008C,
    /// Light Source Current
    DevicePropertyID_LightSourceCurrent = 0x008D,
    /// Light Source On Time Not Resettable
    DevicePropertyID_LightSourceOnTimeNotResettable = 0x008E,
    /// Light Source On Time Resettable
    DevicePropertyID_LightSourceOnTimeResettable = 0x008F,
    /// Light Source Open Circuit Statistics
    DevicePropertyID_LightSourceOpenCircuitStatistics = 0x0090,
    /// Light Source Overall Failures Statistics
    DevicePropertyID_LightSourceOverallFailuresStatistics = 0x0091,
    /// Light Source Short Circuit Statistics
    DevicePropertyID_LightSourceShortCircuitStatistics = 0x0092,
    /// Light Source Start Counter Resettable
    DevicePropertyID_LightSourceStartCounterResettable = 0x0093,
    /// Light Source Temperature
    DevicePropertyID_LightSourceTemperature = 0x0094,
    /// Light Source Thermal Derating Statistics
    DevicePropertyID_LightSourceThermalDeratingStatistics = 0x0095,
    /// Light Source Thermal Shutdown Statistics
    DevicePropertyID_LightSourceThermalShutdownStatistics = 0x0096,
    /// Light Source Total Power On Cycles
    DevicePropertyID_LightSourceTotalPowerOnCycles = 0x0097,
    /// Light Source Voltage
    DevicePropertyID_LightSourceVoltage = 0x0098,
    /// Luminaire Color
    DevicePropertyID_LuminaireColor = 0x0099,
    /// Luminaire Identification Number
    DevicePropertyID_LuminaireIdentificationNumber = 0x009A,
    /// Luminaire Manufacturer GTIN
    DevicePropertyID_LuminaireManufacturerGTIN = 0x009B,
    /// Luminaire Nominal Input Power
    DevicePropertyID_LuminaireNominalInputPower = 0x009C,
    /// Luminaire Nominal Maximum AC Mains Voltage
    DevicePropertyID_LuminaireNominalMaximum_AC_MainsVoltage = 0x009D,
    /// Luminaire Nominal Minimum AC Mains Voltage
    DevicePropertyID_LuminaireNominalMinimum_AC_MainsVoltage = 0x009E,
    /// Luminaire Power at Minimum Dim Level
    DevicePropertyID_LuminairePowerAtMinimumDimLevel = 0x009F,
    /// Luminaire Time of Manufacture
    DevicePropertyID_LuminaireTimeOfManufacture = 0x00A0,
    /// Magnetic Declination
    DevicePropertyID_MagneticDeclination = 0x00A1,
    /// Magnetic Flux Density - 2D
    DevicePropertyID_MagneticFluxDensity2D = 0x00A2,
    /// Magnetic Flux Density - 3D
    DevicePropertyID_MagneticFluxDensity3D = 0x00A3,
    /// Nominal Light Output
    DevicePropertyID_NominalLightOutput = 0x00A4,
    /// Overall Failure Condition
    DevicePropertyID_OverallFailureCondition = 0x00A5,
    /// Pollen Concentration
    DevicePropertyID_PollenConcentration = 0x00A6,
    /// Present Indoor Relative Humidity
    DevicePropertyID_PresentIndoorRelativeHumidity = 0x00A7,
    /// Present Outdoor Relative Humidity
    DevicePropertyID_PresentOutdoorRelativeHumidity = 0x00A8,
    /// Pressure
    DevicePropertyID_Pressure = 0x00A9,
    /// Rainfall
    DevicePropertyID_Rainfall = 0x00AA,
    /// Rated Median Useful Life of Luminaire
    DevicePropertyID_RatedMedianUsefulLifeOfLuminaire = 0x00AB,
    /// Rated Median Useful Light Source Starts
    DevicePropertyID_RatedMedianUsefulLightSourceStarts = 0x00AC,
    /// Reference Temperature
    DevicePropertyID_ReferenceTemperature = 0x00AD,
    /// Total Device Starts
    DevicePropertyID_TotalDeviceStarts = 0x00AE,
    /// True Wind Direction
    DevicePropertyID_TrueWindDirection = 0x00AF,
    /// True Wind Speed
    DevicePropertyID_TrueWindSpeed = 0x00B0,
    /// UV Index
    DevicePropertyID_UV_Index = 0x00B1,
    /// Wind Chill
    DevicePropertyID_WindChill = 0x00B2,
    /// Light Source Type
    DevicePropertyID_LightSourceType = 0x00B3,
    /// Luminaire Identification String
    DevicePropertyID_LuminaireIdentificationString = 0x00B4,
    /// Output Power Limitation
    DevicePropertyID_OutputPowerLimitation = 0x00B5,
    /// Thermal Derating
    DevicePropertyID_ThermalDerating = 0x00B6,
    /// Output Current Percent
    DevicePropertyID_OutputCurrentPercent = 0x00B7,
    //All other values, Reserved for Future Use
} DevicePropertyID;

/// The Format field is a 1-bit bit field that identifies the format of the Length and Property ID fields.
/// Table 4.34: Sensor Data Format values
/// - seeAlso: MshMDL_v1.1.pdf  (page.132)
typedef enum : UInt8 {
    /// Format A is defined as a 4-bit Length field and an 11-bit Property ID field, as defined in Table 4.35.
    /// This format may be used for Property Values that are not longer than 16 octets and for Property IDs
    /// less than 0x0800.
    SigSensorDataFormatA = 0,
    /// Format B is defined as a 7-bit Length field and a 16-bit Property ID field, as described in Table 4.36.
    /// This format may be used for Property Values not longer than 127 octets and for any Property IDs.
    SigSensorDataFormatB = 1,
} SigSensorDataFormat;

typedef enum : UInt8 {
    SigSortType_sortByAddressAscending = 0,
    SigSortType_sortByAddressDescending = 1,
    SigSortType_sortByNameAscending = 2,
    SigSortType_sortByNameDescending = 3,
} SigSortType;

#endif /* SigEnumeration_h */
/// 0x83 0x08 Firmware Update Information Get
