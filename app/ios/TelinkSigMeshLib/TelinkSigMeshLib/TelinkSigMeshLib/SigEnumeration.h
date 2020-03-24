//
//  SigEnumeration.h
//  SigMeshLib
//
//  Created by Liangjiazhi on 2019/9/6.
//  Copyright © 2019 Telink. All rights reserved.
//

#ifndef SigEnumeration_h
#define SigEnumeration_h

typedef enum : NSUInteger {
    ProvisionTpye_NoOOB,//普通添加模式
    ProvisionTpye_StaticOOB,//云端校验添加模式（阿里的天猫精灵设备、小米的小爱同学设备）
    ProvisionTpye_Reserved,//预留
} ProvisionTpye;

typedef enum : NSUInteger {
    KeyBindTpye_Normal,//普通添加模式
    KeyBindTpye_Fast,//快速添加模式
    KeyBindTpye_Reserved,//预留
} KeyBindTpye;

typedef enum : NSUInteger {
    SigStepResolution_hundredsOfMilliseconds = 0b00,
    SigStepResolution_seconds                = 0b01,
    SigStepResolution_tensOfSeconds          = 0b10,
    SigStepResolution_tensOfMinutes          = 0b11,
} SigStepResolution;

/// The status of a Config operation.
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
} SigConfigMessageStatus;

typedef enum : UInt32 {

    /// 4.3.4 Messages summary
    /// - seeAlso: Mesh_v1.0.pdf  (page.188)

    SigOpCode_configAppKeyAdd                                = 0x00,
    SigOpCode_configAppKeyDelete                             = 0x8000,
    SigOpCode_configAppKeyGet                                = 0x8001,
    SigOpCode_configAppKeyList                               = 0x8002,
    SigOpCode_configAppKeyStatus                             = 0x8003,
    SigOpCode_configAppKeyUpdate                             = 0x01,

    SigOpCode_configBeaconGet                                = 0x8009,
    SigOpCode_configBeaconSet                                = 0x800A,
    SigOpCode_configBeaconStatus                             = 0x800B,

    SigOpCode_configCompositionDataGet                       = 0x8008,
    SigOpCode_configCompositionDataStatus                    = 0x02,

    SigOpCode_configDefaultTtlGet                            = 0x800C,
    SigOpCode_configDefaultTtlSet                            = 0x800D,
    SigOpCode_configDefaultTtlStatus                         = 0x800E,

    SigOpCode_configFriendGet                                = 0x800F,
    SigOpCode_configFriendSet                                = 0x8010,
    SigOpCode_configFriendStatus                             = 0x8011,

    SigOpCode_configGATTProxyGet                             = 0x8012,
    SigOpCode_configGATTProxySet                             = 0x8013,
    SigOpCode_configGATTProxyStatus                          = 0x8014,

    SigOpCode_configModelPublicationGet                      = 0x8018,
    SigOpCode_configModelPublicationSet                      = 0x03,
    SigOpCode_configModelPublicationStatus                   = 0x8019,
    SigOpCode_configModelPublicationVirtualAddressSet        = 0x801A,

    SigOpCode_configModelSubscriptionAdd                     = 0x801B,
    SigOpCode_configModelSubscriptionDelete                  = 0x801C,
    SigOpCode_configModelSubscriptionDeleteAll               = 0x801D,
    SigOpCode_configModelSubscriptionOverwrite               = 0x801E,
    SigOpCode_configModelSubscriptionStatus                  = 0x801F,
    SigOpCode_configModelSubscriptionVirtualAddressAdd       = 0x8020,
    SigOpCode_configModelSubscriptionVirtualAddressDelete    = 0x8021,
    SigOpCode_configModelSubscriptionVirtualAddressOverwrite = 0x8022,

    SigOpCode_configNetworkTransmitGet                       = 0x8023,
    SigOpCode_configNetworkTransmitSet                       = 0x8024,
    SigOpCode_configNetworkTransmitStatus                    = 0x8025,

    SigOpCode_configRelayGet                                 = 0x8026,
    SigOpCode_configRelaySet                                 = 0x8027,
    SigOpCode_configRelayStatus                              = 0x8028,

    SigOpCode_configSIGModelSubscriptionGet                  = 0x8029,
    SigOpCode_configSIGModelSubscriptionList                 = 0x802A,
    SigOpCode_configVendorModelSubscriptionGet               = 0x802B,
    SigOpCode_configVendorModelSubscriptionList              = 0x802C,
    SigOpCode_configModelAppBind                             = 0x803D,
    SigOpCode_configModelAppStatus                           = 0x803E,
    SigOpCode_configModelAppUnbind                           = 0x803F,

    SigOpCode_configNetKeyAdd                                = 0x8040,
    SigOpCode_configNetKeyDelete                             = 0x8041,
    SigOpCode_configNetKeyGet                                = 0x8042,
    SigOpCode_configNetKeyList                               = 0x8043,
    SigOpCode_configNetKeyStatus                             = 0x8044,
    SigOpCode_configNetKeyUpdate                             = 0x8045,

    SigOpCode_configNodeReset                                = 0x8049,
    SigOpCode_configNodeResetStatus                          = 0x804A,
    SigOpCode_configSIGModelAppGet                           = 0x804B,
    SigOpCode_configSIGModelAppList                          = 0x804C,
    SigOpCode_configVendorModelAppGet                        = 0x804D,
    SigOpCode_configVendorModelAppList                       = 0x804E,

    /// 7.1 Messages summary
    /// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.298)

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
    SigOpCode_sensorDescriptorStatus                         = 0x51,
    SigOpCode_sensorGet                                      = 0x8231,
    SigOpCode_sensorStatus                                   = 0x52,
    SigOpCode_sensorColumnGet                                = 0x8232,
    SigOpCode_sensorColumnStatus                             = 0x53,
    SigOpCode_sensorSeriesGet                                = 0x8233,
    SigOpCode_sensorSeriesStatus                             = 0x54,
    //Sensor Setup
    SigOpCode_sensorCadenceGet                               = 0x8234,
    SigOpCode_sensorCadenceSet                               = 0x55,
    SigOpCode_sensorCadenceSetUnacknowledged                 = 0x56,
    SigOpCode_sensorCadenceStatus                            = 0x57,
    SigOpCode_sensorSettingsGet                              = 0x8235,
    SigOpCode_sensorSettingsStatus                           = 0x58,
    SigOpCode_sensorSettingGet                               = 0x8236,
    SigOpCode_sensorSettingSet                               = 0x59,
    SigOpCode_sensorSettingSetUnacknowledged                 = 0x5A,
    SigOpCode_sensorSettingStatus                            = 0x5B,

    //Time
    SigOpCode_timeGet                                        = 0x8237,
    SigOpCode_timeSet                                        = 0x5C,
    SigOpCode_timeStatus                                     = 0x5D,
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
    SigOpCode_sceneStatus                                    = 0x5E,
    SigOpCode_sceneRegisterGet                               = 0x8244,
    SigOpCode_sceneRegisterStatus                            = 0x8245,
    //Scene Setup
    SigOpCode_sceneStore                                     = 0x8246,
    SigOpCode_sceneStoreUnacknowledged                       = 0x8247,
    SigOpCode_sceneDelete                                    = 0x829E,
    SigOpCode_sceneDeleteUnacknowledged                      = 0x829F,

    //Scheduler
    SigOpCode_schedulerActionGet                             = 0x8248,
    SigOpCode_schedulerActionStatus                          = 0x5F,
    SigOpCode_schedulerGet                                   = 0x8249,
    SigOpCode_schedulerStatus                                = 0x824A,
    //Scheduler Setup
    SigOpCode_schedulerActionSet                             = 0x60,
    SigOpCode_schedulerActionSetUnacknowledged               = 0x61,

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
    SigOpCode_LightLCPropertySet                             = 0x62,
    SigOpCode_LightLCPropertySetUnacknowledged               = 0x63,
    SigOpCode_LightLCPropertyStatus                          = 0x64,

    /// 3.1.1 Firmware Update Messages
    /// - seeAlso: Mesh_Firmware_update_20180228_d05r05.pdf  (page.20)

    //Firmware Update Messages
    SigOpCode_FirmwareInformationGet                         = 0xB601,
    SigOpCode_FirmwareInformationStatus                      = 0xB602,
    SigOpCode_FirmwareUpdateGet                              = 0xB603,
    SigOpCode_FirmwareUpdatePrepare                          = 0xB604,
    SigOpCode_FirmwareUpdateStart                            = 0xB605,
    SigOpCode_FirmwareUpdateAbort                            = 0xB606,
    SigOpCode_FirmwareUpdateApply                            = 0xB607,
    SigOpCode_FirmwareUpdateStatus                           = 0xB608,
    SigOpCode_FirmwareDistributionGet                        = 0xB609,
    SigOpCode_FirmwareDistributionStart                      = 0xB60A,
    SigOpCode_FirmwareDistributionStop                       = 0xB60B,
    SigOpCode_FirmwareDistributionStatus                     = 0xB60C,
    SigOpCode_FirmwareDistributionDetailsGet                 = 0xB60D,
    SigOpCode_FirmwareDistributionDetailsList                = 0xB60E,

    /// 3.1.2 Object Transfer Messages
    /// - seeAlso: Mesh_Firmware_update_20180228_d05r05.pdf  (page.20)

    //Object Transfer Messages
    SigOpCode_ObjectTransferGet                              = 0xB701,
    SigOpCode_ObjectTransferStart                            = 0xB702,
    SigOpCode_ObjectTransferAbort                            = 0xB703,
    SigOpCode_ObjectTransferStatus                           = 0xB704,
    SigOpCode_ObjectBlockTransferStart                       = 0xB705,
    SigOpCode_ObjectBlockTransferStatus                      = 0xB706,
    SigOpCode_ObjectChunkTransfer                            = 0x7D,
    SigOpCode_ObjectBlockGet                                 = 0x7E,
    SigOpCode_ObjectBlockStatus                              = 0xB709,
    SigOpCode_ObjectInformationGet                           = 0xB70A,
    SigOpCode_ObjectInformationStatus                        = 0xB70B,
    
} SigOpCode;

typedef enum : UInt8 {
    /// The segmented message has not been acknowledged before the timeout occurred.
    SigLowerTransportError_timeout = 0,
    /// The target device is busy at the moment and could not accept the message.
    SigLowerTransportError_busy    = 1,
} SigLowerTransportError;

typedef enum : UInt8 {
    SigLowerTransportPduType_accessMessage  = 0,
    SigLowerTransportPduType_controlMessage = 1,
} SigLowerTransportPduType;

typedef enum : NSUInteger {
    /// Provisioning Manager is ready to start.
    ProvisionigState_ready,
    /// The manager is requesting Provisioioning Capabilities from the device.
    ProvisionigState_requestingCapabilities,
    /// Provisioning Capabilities were received.
    ProvisionigState_capabilitiesReceived,
    /// Provisioning has been started.
    ProvisionigState_provisioning,
    /// The provisioning process is complete.
    ProvisionigState_complete,
    /// The provisioning has failed because of a local error.
    ProvisionigState_fail,
} ProvisionigState;

typedef enum : NSUInteger {
    /// Thrown when the ProvisioningManager is in invalid state.
    ProvisioningError_invalidState,
    /// The received PDU is invalid.
    ProvisioningError_invalidPdu,
    /// Thrown when an unsupported algorighm has been selected for provisioning.
    ProvisioningError_unsupportedAlgorithm,
    /// Thrown when the Unprovisioned Device is not supported by the manager.
    ProvisioningError_unsupportedDevice,
    /// Thrown when the provided alphanumberic value could not be converted into
    /// bytes using ASCII encoding.
    ProvisioningError_invalidOobValueFormat,
    /// Thrown when no available Unicast Address was found in the Provisioner's
    /// range that could be allocated for the device.
    ProvisioningError_noAddressAvailable,
    /// Throws when the Unicast Address has not been set.
    ProvisioningError_addressNotSpecified,
    /// Throws when the Network Key has not been set.
    ProvisioningError_networkKeyNotSpecified,
    /// Thrown when confirmation value received from the device does not match
    /// calculated value. Authentication failed.
    ProvisioningError_confirmationFailed,
    /// Thrown when the remove device sent a failure indication.
    ProvisioningError_remoteError,
    /// Thrown when the key pair generation has failed.
    ProvisioningError_keyGenerationFailed,
} ProvisioningError;

typedef enum : NSUInteger {
    /// The provisioning protocol PDU is not recognized by the device.
    RemoteProvisioningError_invalidPdu = 1,
    /// The arguments of the protocol PDUs are outside expected values
    /// or the length of the PDU is different than expected.
    RemoteProvisioningError_invalidFormat = 2,
    /// The PDU received was not expected at this moment of the procedure.
    RemoteProvisioningError_unexpectedPdu = 3,
    /// The computed confirmation value was not successfully verified.
    RemoteProvisioningError_confirmationFailed = 4,
    /// The provisioning protocol cannot be continued due to insufficient
    /// resources in the device.
    RemoteProvisioningError_outOfResources = 5,
    /// The Data block was not successfully decrypted.
    RemoteProvisioningError_decryptionFailed = 6,
    /// An unexpected error occurred that may not be recoverable.
    RemoteProvisioningError_unexpectedError = 7,
    /// The device cannot assign consecutive unicast addresses to all elements.
    RemoteProvisioningError_cannotAcssignAddresses = 8,
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
    SigLocation_auxiliary                    = 0x0108,
    SigLocation_back                         = 0x0101,
    SigLocation_backup                       = 0x0107,
    SigLocation_bottom                       = 0x0103,
    SigLocation_eighteenth                   = 0x0012,
    SigLocation_eighth                       = 0x0008,
    SigLocation_eightieth                    = 0x0050,
    SigLocation_eightyEighth                 = 0x0058,
    SigLocation_eightyFifth                  = 0x0055,
    SigLocation_eightyFirst                  = 0x0051,
    SigLocation_eightyFourth                 = 0x0054,
    SigLocation_eightyNineth                 = 0x0059,
    SigLocation_eightySecond                 = 0x0052,
    SigLocation_eightySeventh                = 0x0057,
    SigLocation_eightySixth                  = 0x0056,
    SigLocation_eightyThird                  = 0x0053,
    SigLocation_eleventh                     = 0x000b,
    SigLocation_external                     = 0x0110,
    SigLocation_fifteenth                    = 0x000f,
    SigLocation_fifth                        = 0x0005,
    SigLocation_fiftieth                     = 0x0032,
    SigLocation_fiftyEighth                  = 0x003a,
    SigLocation_fiftyFifth                   = 0x0037,
    SigLocation_fiftyFirst                   = 0x0033,
    SigLocation_fiftyFourth                  = 0x0036,
    SigLocation_fiftyNineth                  = 0x003b,
    SigLocation_fiftySecond                  = 0x0034,
    SigLocation_fiftySeventh                 = 0x0039,
    SigLocation_fiftySixth                   = 0x0038,
    SigLocation_fiftyThird                   = 0x0035,
    SigLocation_first                        = 0x0001,
    SigLocation_flash                        = 0x010A,
    SigLocation_fortieth                     = 0x0028,
    SigLocation_fourteenth                   = 0x000e,
    SigLocation_fourth                       = 0x0004,
    SigLocation_fourtyEighth                 = 0x0030,
    SigLocation_fourtyFifth                  = 0x002d,
    SigLocation_fourtyFirst                  = 0x0029,
    SigLocation_fourtyFourth                 = 0x002c,
    SigLocation_fourtyNineth                 = 0x0031,
    SigLocation_fourtySecond                 = 0x002a,
    SigLocation_fourtySeventh                = 0x002f,
    SigLocation_fourtySixth                  = 0x002e,
    SigLocation_fourtyThird                  = 0x002b,
    SigLocation_front                        = 0x0100,
    SigLocation_inside                       = 0x010B,
    SigLocation_internal                     = 0x010F,
    SigLocation_left                         = 0x010D,
    SigLocation_lower                        = 0x0105,
    SigLocation_main                         = 0x0106,
    SigLocation_nineteenth                   = 0x0013,
    SigLocation_nineth                       = 0x0009,
    SigLocation_ninetieth                    = 0x005a,
    SigLocation_ninetyEighth                 = 0x0062,
    SigLocation_ninetyFifth                  = 0x005f,
    SigLocation_ninetyFirst                  = 0x005b,
    SigLocation_ninetyFourth                 = 0x005e,
    SigLocation_ninetyNineth                 = 0x0063,
    SigLocation_ninetySecond                 = 0x005c,
    SigLocation_ninetySeventh                = 0x0061,
    SigLocation_ninetySixth                  = 0x0060,
    SigLocation_ninetyThird                  = 0x005d,
    SigLocation_oneHundredAndEighteenth      = 0x0076,
    SigLocation_oneHundredAndEighth          = 0x006c,
    SigLocation_oneHundredAndEightyEighth    = 0x00bc,
    SigLocation_oneHundredAndEightyFifth     = 0x00b9,
    SigLocation_oneHundredAndEightyFirst     = 0x00b5,
    SigLocation_oneHundredAndEightyFourth    = 0x00b8,
    SigLocation_oneHundredAndEightyNineth    = 0x00bd,
    SigLocation_oneHundredAndEightySecond    = 0x00b6,
    SigLocation_oneHundredAndEightySeventh   = 0x00bb,
    SigLocation_oneHundredAndEightySixth     = 0x00ba,
    SigLocation_oneHundredAndEightyThird     = 0x00b7,
    SigLocation_oneHundredAndEleventh        = 0x006f,
    SigLocation_oneHundredAndFifteenth       = 0x0073,
    SigLocation_oneHundredAndFifth           = 0x0069,
    SigLocation_oneHundredAndFiftyEighth     = 0x009e,
    SigLocation_oneHundredAndFiftyFifth      = 0x009b,
    SigLocation_oneHundredAndFiftyFirst      = 0x0097,
    SigLocation_oneHundredAndFiftyFourth     = 0x009a,
    SigLocation_oneHundredAndFiftyNineth     = 0x009f,
    SigLocation_oneHundredAndFiftySecond     = 0x0098,
    SigLocation_oneHundredAndFiftySeventh    = 0x009d,
    SigLocation_oneHundredAndFiftySixth      = 0x009c,
    SigLocation_oneHundredAndFiftyThird      = 0x0099,
    SigLocation_oneHundredAndFirst           = 0x0065,
    SigLocation_oneHundredAndFourteenth      = 0x0072,
    SigLocation_oneHundredAndFourth          = 0x0068,
    SigLocation_oneHundredAndFourtyEighth    = 0x0094,
    SigLocation_oneHundredAndFourtyFifth     = 0x0091,
    SigLocation_oneHundredAndFourtyFirst     = 0x008d,
    SigLocation_oneHundredAndFourtyFourth    = 0x0090,
    SigLocation_oneHundredAndFourtyNineth    = 0x0095,
    SigLocation_oneHundredAndFourtySecond    = 0x008e,
    SigLocation_oneHundredAndFourtySeventh   = 0x0093,
    SigLocation_oneHundredAndFourtySixth     = 0x0092,
    SigLocation_oneHundredAndFourtyThird     = 0x008f,
    SigLocation_oneHundredAndNineteenth      = 0x0077,
    SigLocation_oneHundredAndNineth          = 0x006d,
    SigLocation_oneHundredAndNinetyEighth    = 0x00c6,
    SigLocation_oneHundredAndNinetyFifth     = 0x00c3,
    SigLocation_oneHundredAndNinetyFirst     = 0x00bf,
    SigLocation_oneHundredAndNinetyFourth    = 0x00c2,
    SigLocation_oneHundredAndNinetyNineth    = 0x00c7,
    SigLocation_oneHundredAndNinetySecond    = 0x00c0,
    SigLocation_oneHundredAndNinetySeventh   = 0x00c5,
    SigLocation_oneHundredAndNinetySixth     = 0x00c4,
    SigLocation_oneHundredAndNinetyThird     = 0x00c1,
    SigLocation_oneHundredAndSecond          = 0x0066,
    SigLocation_oneHundredAndSeventeenth     = 0x0075,
    SigLocation_oneHundredAndSeventh         = 0x006b,
    SigLocation_oneHundredAndSeventyEighth   = 0x00b2,
    SigLocation_oneHundredAndSeventyFifth    = 0x00af,
    SigLocation_oneHundredAndSeventyFirst    = 0x00ab,
    SigLocation_oneHundredAndSeventyFourth   = 0x00ae,
    SigLocation_oneHundredAndSeventyNineth   = 0x00b3,
    SigLocation_oneHundredAndSeventySecond   = 0x00ac,
    SigLocation_oneHundredAndSeventySeventh  = 0x00b1,
    SigLocation_oneHundredAndSeventySixth    = 0x00b0,
    SigLocation_oneHundredAndSeventyThird    = 0x00ad,
    SigLocation_oneHundredAndSixteenth       = 0x0074,
    SigLocation_oneHundredAndSixth           = 0x006a,
    SigLocation_oneHundredAndSixtyEighth     = 0x00a8,
    SigLocation_oneHundredAndSixtyFifth      = 0x00a5,
    SigLocation_oneHundredAndSixtyFirst      = 0x00a1,
    SigLocation_oneHundredAndSixtyFourth     = 0x00a4,
    SigLocation_oneHundredAndSixtyNineth     = 0x00a9,
    SigLocation_oneHundredAndSixtySecond     = 0x00a2,
    SigLocation_oneHundredAndSixtySeventh    = 0x00a7,
    SigLocation_oneHundredAndSixtySixth      = 0x00a6,
    SigLocation_oneHundredAndSixtyThird      = 0x00a3,
    SigLocation_oneHundredAndTenth           = 0x006e,
    SigLocation_oneHundredAndThird           = 0x0067,
    SigLocation_oneHundredAndThirteenth      = 0x0071,
    SigLocation_oneHundredAndThirtyEighth    = 0x008a,
    SigLocation_oneHundredAndThirtyFifth     = 0x0087,
    SigLocation_oneHundredAndThirtyFirst     = 0x0083,
    SigLocation_oneHundredAndThirtyFourth    = 0x0086,
    SigLocation_oneHundredAndThirtyNineth    = 0x008b,
    SigLocation_oneHundredAndThirtySecond    = 0x0084,
    SigLocation_oneHundredAndThirtySeventh   = 0x0089,
    SigLocation_oneHundredAndThirtySixth     = 0x0088,
    SigLocation_oneHundredAndThirtyThird     = 0x0085,
    SigLocation_oneHundredAndTwelveth        = 0x0070,
    SigLocation_oneHundredAndTwentyEighth    = 0x0080,
    SigLocation_oneHundredAndTwentyFifth     = 0x007d,
    SigLocation_oneHundredAndTwentyFirst     = 0x0079,
    SigLocation_oneHundredAndTwentyFourth    = 0x007c,
    SigLocation_oneHundredAndTwentyNineth    = 0x0081,
    SigLocation_oneHundredAndTwentySecond    = 0x007a,
    SigLocation_oneHundredAndTwentySeventh   = 0x007f,
    SigLocation_oneHundredAndTwentySixth     = 0x007e,
    SigLocation_oneHundredAndTwentyThird     = 0x007b,
    SigLocation_oneHundredEightieth          = 0x00b4,
    SigLocation_oneHundredFiftieth           = 0x0096,
    SigLocation_oneHundredFortieth           = 0x008c,
    SigLocation_oneHundredNinetieth          = 0x00be,
    SigLocation_oneHundredSeventieth         = 0x00aa,
    SigLocation_oneHundredSixtieth           = 0x00a0,
    SigLocation_oneHundredThirtieth          = 0x0082,
    SigLocation_oneHundredTwentieth          = 0x0078,
    SigLocation_oneHundredth                 = 0x0064,
    SigLocation_outside                      = 0x010C,
    SigLocation_right                        = 0x010E,
    SigLocation_second                       = 0x0002,
    SigLocation_seventeenth                  = 0x0011,
    SigLocation_seventh                      = 0x0007,
    SigLocation_seventieth                   = 0x0046,
    SigLocation_seventyEighth                = 0x004e,
    SigLocation_seventyFifth                 = 0x004b,
    SigLocation_seventyFirst                 = 0x0047,
    SigLocation_seventyFourth                = 0x004a,
    SigLocation_seventyNineth                = 0x004f,
    SigLocation_seventySecond                = 0x0048,
    SigLocation_seventySeventh               = 0x004d,
    SigLocation_seventySixth                 = 0x004c,
    SigLocation_seventyThird                 = 0x0049,
    SigLocation_sixteenth                    = 0x0010,
    SigLocation_sixth                        = 0x0006,
    SigLocation_sixtieth                     = 0x003c,
    SigLocation_sixtyEighth                  = 0x0044,
    SigLocation_sixtyFifth                   = 0x0041,
    SigLocation_sixtyFirst                   = 0x003d,
    SigLocation_sixtyFourth                  = 0x0040,
    SigLocation_sixtyNineth                  = 0x0045,
    SigLocation_sixtySecond                  = 0x003e,
    SigLocation_sixtySeventh                 = 0x0043,
    SigLocation_sixtySixth                   = 0x0042,
    SigLocation_sixtyThird                   = 0x003f,
    SigLocation_supplementary                = 0x0109,
    SigLocation_tenth                        = 0x000a,
    SigLocation_third                        = 0x0003,
    SigLocation_thirteenth                   = 0x000d,
    SigLocation_thirtieth                    = 0x001e,
    SigLocation_thirtyEighth                 = 0x0026,
    SigLocation_thirtyFifth                  = 0x0023,
    SigLocation_thirtyFirst                  = 0x001f,
    SigLocation_thirtyFourth                 = 0x0022,
    SigLocation_thirtyNineth                 = 0x0027,
    SigLocation_thirtySecond                 = 0x0020,
    SigLocation_thirtySeventh                = 0x0025,
    SigLocation_thirtySixth                  = 0x0024,
    SigLocation_thirtyThird                  = 0x0021,
    SigLocation_top                          = 0x0102,
    SigLocation_twelveth                     = 0x000c,
    SigLocation_twentieth                    = 0x0014,
    SigLocation_twentyEighth                 = 0x001c,
    SigLocation_twentyFifth                  = 0x0019,
    SigLocation_twentyFirst                  = 0x0015,
    SigLocation_twentyFourth                 = 0x0018,
    SigLocation_twentyNineth                 = 0x001d,
    SigLocation_twentySecond                 = 0x0016,
    SigLocation_twentySeventh                = 0x001b,
    SigLocation_twentySixth                  = 0x001a,
    SigLocation_twentyThird                  = 0x0017,
    SigLocation_twoHundredAndEighteenth      = 0x00da,
    SigLocation_twoHundredAndEighth          = 0x00d0,
    SigLocation_twoHundredAndEleventh        = 0x00d3,
    SigLocation_twoHundredAndFifteenth       = 0x00d7,
    SigLocation_twoHundredAndFifth           = 0x00cd,
    SigLocation_twoHundredAndFiftyFifth      = 0x00ff,
    SigLocation_twoHundredAndFiftyFirst      = 0x00fb,
    SigLocation_twoHundredAndFiftyFourth     = 0x00fe,
    SigLocation_twoHundredAndFiftySecond     = 0x00fc,
    SigLocation_twoHundredAndFiftyThird      = 0x00fd,
    SigLocation_twoHundredAndFirst           = 0x00c9,
    SigLocation_twoHundredAndFourteenth      = 0x00d6,
    SigLocation_twoHundredAndFourth          = 0x00cc,
    SigLocation_twoHundredAndFourtyEighth    = 0x00f8,
    SigLocation_twoHundredAndFourtyFifth     = 0x00f5,
    SigLocation_twoHundredAndFourtyFirst     = 0x00f1,
    SigLocation_twoHundredAndFourtyFourth    = 0x00f4,
    SigLocation_twoHundredAndFourtyNineth    = 0x00f9,
    SigLocation_twoHundredAndFourtySecond    = 0x00f2,
    SigLocation_twoHundredAndFourtySeventh   = 0x00f7,
    SigLocation_twoHundredAndFourtySixth     = 0x00f6,
    SigLocation_twoHundredAndFourtyThird     = 0x00f3,
    SigLocation_twoHundredAndNineteenth      = 0x00db,
    SigLocation_twoHundredAndNineth          = 0x00d1,
    SigLocation_twoHundredAndSecond          = 0x00ca,
    SigLocation_twoHundredAndSeventeenth     = 0x00d9,
    SigLocation_twoHundredAndSeventh         = 0x00cf,
    SigLocation_twoHundredAndSixteenth       = 0x00d8,
    SigLocation_twoHundredAndSixth           = 0x00ce,
    SigLocation_twoHundredAndTenth           = 0x00d2,
    SigLocation_twoHundredAndThird           = 0x00cb,
    SigLocation_twoHundredAndThirteenth      = 0x00d5,
    SigLocation_twoHundredAndThirtyEighth    = 0x00ee,
    SigLocation_twoHundredAndThirtyFifth     = 0x00eb,
    SigLocation_twoHundredAndThirtyFirst     = 0x00e7,
    SigLocation_twoHundredAndThirtyFourth    = 0x00ea,
    SigLocation_twoHundredAndThirtyNineth    = 0x00ef,
    SigLocation_twoHundredAndThirtySecond    = 0x00e8,
    SigLocation_twoHundredAndThirtySeventh   = 0x00ed,
    SigLocation_twoHundredAndThirtySixth     = 0x00ec,
    SigLocation_twoHundredAndThirtyThird     = 0x00e9,
    SigLocation_twoHundredAndTwelveth        = 0x00d4,
    SigLocation_twoHundredAndTwentyEighth    = 0x00e4,
    SigLocation_twoHundredAndTwentyFifth     = 0x00e1,
    SigLocation_twoHundredAndTwentyFirst     = 0x00dd,
    SigLocation_twoHundredAndTwentyFourth    = 0x00e0,
    SigLocation_twoHundredAndTwentyNineth    = 0x00e5,
    SigLocation_twoHundredAndTwentySecond    = 0x00de,
    SigLocation_twoHundredAndTwentySeventh   = 0x00e3,
    SigLocation_twoHundredAndTwentySixth     = 0x00e2,
    SigLocation_twoHundredAndTwentyThird     = 0x00df,
    SigLocation_twoHundredFiftieth           = 0x00fa,
    SigLocation_twoHundredFortieth           = 0x00f0,
    SigLocation_twoHundredThirtieth          = 0x00e6,
    SigLocation_twoHundredTwentieth          = 0x00dc,
    SigLocation_twoHundredth                 = 0x00c8,
    SigLocation_unknown                      = 0x0000,
    SigLocation_upper                        = 0x0104,
} SigLocation;

typedef enum : UInt8 {
    SigOnPowerUpOff                            = 0x00,
    SigOnPowerUpDefault                        = 0x01,
    SigOnPowerUpRestore                        = 0x02,
} SigOnPowerUp;

typedef enum : UInt8 {
    SigGenericMessageStatusSuccess             = 0x00,
    SigGenericMessageStatusCannotSetRangeMin   = 0x01,
    SigGenericMessageStatusCannotSetRangeMax   = 0x02,
} SigGenericMessageStatus;

typedef enum : UInt8 {
    SigBatteryPresenceNotPresent               = 0b00,
    SigBatteryPresenceRemovable                = 0b01,
    SigBatteryPresenceNotRemovable             = 0b10,
    SigBatteryPresenceUnknown                  = 0b11,
} SigBatteryPresence;

typedef enum : UInt8 {
    SigBatteryIndicatorCritiallyLow            = 0b00,
    SigBatteryIndicatorLow                     = 0b01,
    SigBatteryIndicatorGood                    = 0b10,
    SigBatteryIndicatorUnknown                 = 0b11,
} SigBatteryIndicator;

typedef enum : UInt8 {
    SigBatteryChargingStateNotChargable        = 0b00,
    SigBatteryChargingStateNotCharging         = 0b01,
    SigBatteryChargingStateCharging            = 0b10,
    SigBatteryChargingStateUnknown             = 0b11,
} SigBatteryChargingState;

typedef enum : UInt8 {
    SigBatteryServiceabilityReserved           = 0b00,
    SigBatteryServiceabilityServiceNotRequired = 0b01,
    SigBatteryServiceabilityServiceRequired    = 0b10,
    SigBatteryServiceabilityUnknown            = 0b11,
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
    SigSceneResponseStatus_success           = 0x00,
    SigSceneResponseStatus_sceneRegisterFull = 0x01,
    SigSceneResponseStatus_sceneNotFound     = 0x02,
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
    SigNodeRelayState_notEnabled     = 0,
    //The node supports Relay feature that is enabled.
    SigNodeRelayState_enabled        = 1,
    //Relay feature is not supported.
    SigNodeRelayState_notSupported   = 2,
} SigNodeRelayState;

/// 4.2.10 SecureNetworkBeacon
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.141)
typedef enum : UInt8 {
    //The node is not broadcasting a Secure Network beacon.
    SigSecureNetworkBeaconState_open,
    //The node is broadcasting a Secure Network beacon.
    SigSecureNetworkBeaconState_close,
} SigSecureNetworkBeaconState;

/// 4.2.11 GATTProxy
/// - seeAlso: Mesh_v1.0.pdf  (page.141)
typedef enum : UInt8 {
    //The Mesh Proxy Service is running, Proxy feature is disabled.
    SigNodeGATTProxyState_notEnabled     = 0,
    //The Mesh Proxy Service is running, Proxy feature is enabled.
    SigNodeGATTProxyState_enabled        = 1,
    //The Mesh Proxy Service is not supported, Proxy feature is not supported.
    SigNodeGATTProxyState_notSupported   = 2,
} SigNodeGATTProxyState;

/// 4.2.12 NodeIdentity
/// - seeAlso: Mesh_v1.0.pdf  (page.142)
typedef enum : UInt8 {
    //Node Identity for a subnet is stopped.
    SigNodeIdentityState_notEnabled     = 0,
    //Node Identity for a subnet is running.
    SigNodeIdentityState_enabled        = 1,
    //Node Identity is not supported.
    SigNodeIdentityState_notSupported   = 2,
} SigNodeIdentityState;

/// 4.2.13 Friend
/// - seeAlso: Mesh_v1.0.pdf  (page.142)
typedef enum : UInt8 {
    //The node supports Friend feature that is disabled.
    SigNodeFeaturesState_notEnabled     = 0,
    //The node supports Friend feature that is enabled.
    SigNodeFeaturesState_enabled        = 1,
    //The Friend feature is not supported.
    SigNodeFeaturesState_notSupported   = 2,
} SigNodeFeaturesState;

/// 4.2.14 Key Refresh Phase
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
    //Sent by a Proxy Client to set the proxy filter type.
    SigProxyFilerOpcode_setFilterType             = 0x00,
    //Sent by a Proxy Client to add addresses to the proxy filter list.
    SigProxyFilerOpcode_addAddressesToFilter      = 0x01,
    //Sent by a Proxy Client to remove addresses from the proxy filter list.
    SigProxyFilerOpcode_removeAddressesFromFilter = 0x02,
    //Acknowledgment by a Proxy Server to a Proxy Client to report the status of the proxy filter list.
    SigProxyFilerOpcode_filterStatus              = 0x03,
} SigProxyFilerOpcode;

/// Table 6.7: FilterType Values
/// - seeAlso: Mesh_v1.0.pdf  (page.263)
typedef enum : UInt8 {
    /// A white list filter has an associated white list, which is a list of
    /// destination addresses that are of interest for the Proxy Client.
    /// The white list filter blocks all destination addresses except those
    /// that have been added to the white list.
    SigProxyFilerType_whitelist = 0x00,
    /// A black list filter has an associated black list, which is a list of
    /// destination addresses that the Proxy Client does not want to receive.
    /// The black list filter accepts all destination addresses except those
    /// that have been added to the black list.
    SigProxyFilerType_blacklist = 0x01,
} SigProxyFilerType;

/// Firmware Distribution Status Values
/// - seeAlso: Mesh_Firmware_update_20180228_d05r05.pdf  (page.24)
typedef enum : UInt8 {
    /// ready, distribution is not active
    SigFirmwareDistributionStatusType_notActive = 0x00,
    /// distribution is active
    SigFirmwareDistributionStatusType_active = 0x01,
    /// no such Company ID and Firmware ID combin
    SigFirmwareDistributionStatusType_noSuchId = 0x02,
    /// busy with different distribution
    SigFirmwareDistributionStatusType_busyWithDifferent = 0x03,
    /// update nodes list is too long
    SigFirmwareDistributionStatusType_updateNodeListTooLong = 0x04,
} SigFirmwareDistributionStatusType;

/// Update status code values:
/// - seeAlso: Mesh_Firmware_update_20180228_d05r05.pdf  (page.25)
typedef enum : UInt8 {
    /// successfully updated
    SigUpdateStatusType_success = 0x00,
    /// in progress
    SigUpdateStatusType_inProgress = 0x01,
    /// canceled
    SigUpdateStatusType_cancel = 0x02,
} SigUpdateStatusType;

/// Update Policy field values:
/// - seeAlso: Mesh_Firmware_update_20180228_d05r05.pdf  (page.26)
typedef enum : UInt8 {
    /// Do not apply new firmware when Object transfer is completed.
    SigUpdatePolicyType_none = 0x00,
    /// Apply new firmware when Object transfer is completed.
    SigUpdatePolicyType_autoUpdate = 0x01,
    /// Reserved for Future Use, 0x02~0xFF
    SigUpdatePolicyType_RFU,
} SigUpdatePolicyType;

/// Firmware Update Status Values
/// - seeAlso: Mesh_Firmware_update_20180228_d05r05.pdf  (page.28)
typedef enum : UInt8 {
    /// success
    SigFirmwareUpdateStatusType_success = 0x00,
    /// wrong Company ID and Firmware ID combination
    SigFirmwareUpdateStatusType_IdCombinationWrong = 0x01,
    /// different object transfer already ongoing
    SigFirmwareUpdateStatusType_busyWithDifferentObject = 0x02,
    /// Company ID and Firmware ID combination apply failed
    SigFirmwareUpdateStatusType_IdCombinationApplyFail = 0x03,
    /// Company ID and Firmware ID combination permanently rejected, newer firmware version present
    SigFirmwareUpdateStatusType_combinationAlwaysReject = 0x04,
    /// Company ID and Firmware ID combination temporary rejected, node is not able to accept new firmware now, try again later
    SigFirmwareUpdateStatusType_combinationTempReject = 0x05,
} SigFirmwareUpdateStatusType;

/// Phase Values
/// - seeAlso: Mesh_Firmware_update_20180228_d05r05.pdf  (page.28)
typedef enum : UInt8 {
    /// No DFU update in progress
    SigFirmwareUpdatePhaseType_idle = 0x00,
    /// DFU update is prepared and awaiting start
    SigFirmwareUpdatePhaseType_prepare = 0x01,
    /// DFU update is in progress
    SigFirmwareUpdatePhaseType_inProgress = 0x02,
    /// DFU upload is finished and waiting to be applied
    SigFirmwareUpdatePhaseType_DFUReady = 0x03,
    /// just use internal
    SigFirmwareUpdatePhaseType_applyOk = 0x07,
} SigFirmwareUpdatePhaseType;

/// Object Transfer Status Values
/// - seeAlso: Mesh_Firmware_update_20180228_d05r05.pdf  (page.30)
typedef enum : UInt8 {
    /// ready, object transfer is not active
    SigObjectTransferStatusType_ready = 0x00,
    /// busy, object transfer is active
    SigObjectTransferStatusType_busy = 0x01,
    /// busy, with different transfer
    SigObjectTransferStatusType_busyWithDifferent = 0x02,
    /// object is too big to be stored
    SigObjectTransferStatusType_tooBig = 0x03,
} SigObjectTransferStatusType;

/// The Block Checksum Algorithm values
/// - seeAlso: Mesh_Firmware_update_20180228_d05r05.pdf  (page.31)
typedef enum : UInt8 {
    /// Details TBD, Block Checksum Value len is 4.
    SigBlockChecksumAlgorithmType_CRC32 = 0x00,
    /// Reserved for Future Use, 0x01-0xFF
} SigBlockChecksumAlgorithmType;

/// Object Block Transfer Status values
/// - seeAlso: Mesh_Firmware_update_20180228_d05r05.pdf  (page.32)
typedef enum : UInt8 {
    /// block transfer accepted
    SigObjectBlockTransferStatusType_accepted = 0x00,
    /// block already transferred
    SigObjectBlockTransferStatusType_alreadyRX = 0x01,
    /// invalid block number, no previous block
    SigObjectBlockTransferStatusType_invalidBlockNumber = 0x02,
    /// wrong current block size - bigger then Block Size Log [Object Transfer Start]
    SigObjectBlockTransferStatusType_wrongCurrentBlockSize = 0x03,
    /// wrong Chunk Size - bigger then Block Size divided by Max Chunks Number [Object Information Status]
    SigObjectBlockTransferStatusType_wrongChunkSize = 0x04,
    /// unknown checksum algorithm
    SigObjectBlockTransferStatusType_unknownChecksumAlgorithm = 0x05,
    /// block transfer rejected
    SigObjectBlockTransferStatusType_rejected = 0x0F,
} SigObjectBlockTransferStatusType;

/// The Object Block Status values
/// - seeAlso: Mesh_Firmware_update_20180228_d05r05.pdf  (page.34)
typedef enum : UInt8 {
    /// All chunks received, checksum is valid, ready for the next block.
    SigObjectBlockStatusType_allChunksReceived = 0x00,
    /// Not all chunks received, checksum is not computed.
    SigObjectBlockStatusType_notAllChunksReceived = 0x01,
    /// All chunks received, computed checksum value is not equal to expected value.
    SigObjectBlockStatusType_wrongChecksum = 0x02,
    /// Requested Object ID not active.
    SigObjectBlockStatusType_wrongObjectID = 0x03,
    /// Requested block not active.
    SigObjectBlockStatusType_wrongBlock = 0x04,
    /// Reserved for Future Use, 0x05-0xFF
} SigObjectBlockStatusType;


#endif /* SigEnumeration_h */