/********************************************************************************************************
 * @file     Model.h 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
//
//  Model.h
//  SigMeshOC
//
//  Created by Liangjiazhi on 2018/7/12.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "LibHandle.h"

@class SIGModel,BLEStore,SigNodeModel;
typedef void(^BeaconBackCallBack)(BOOL available);

///Agreement document for reference, “Mesh_Model_Specification v1.0.pdf”，page298.
typedef enum : NSUInteger {
    OpcodeOnOffStatusResponse = 0x0482,
    OpcodeCurrentCTLResponse = 0x6082,
    OpcodeCTLTemperatureStatusResponse = 0x6682,
    OpcodeHSLNotifyResponse = 0x7882,//publish modelID is SIG_MD_LIGHT_HSL_S，notify callback OPCode
    OpcodeSubListGetResponse = 0x2A80,
    
    OpcodeBrightnessGetResponse = 0x4E82,
    OpcodeChangeLevelResponse = 0x0882,
    
    // eg:<f00d0140 0040801f 0001401f c00010>
    OpcodeEditSubListResponse = 0x1F80,//Agreement document for reference, Mesh_v1.0.pdf, 4.3.2.26, Config Model Subscription Status
    OpcodePublishAddressResponse = 0x1980,
    
    OpcodeOnOffG = 0x0182,
    OpcodeOnOffS = 0x0282,
    
    OpcodeSubscriptorAddRequest = 0x1B80,
    OpcodeSubscriptorDelRequest = 0x1C80,
    
    OpcodeDelectDeviceResponse = 0x4A80,

    OpcodeGetSchedulerStatusResponse = 0x4A82,

    //add address:2 to sceneID:3<f00f0200 01008245 00030001 00020003 00>
    //del address:2<f00d0200 01008245 00020001 000200>
    //add scene and delete scene is same OPCode.
    OpcodeSceneRegisterStatusResponse = 0x4582,
    
    OpcodeSetTimeResponse = 0x5D,
    OpcodeRecallSceneResponse = 0x5E,
    OpcodeSetSchedulerResponse = 0x5F,
    
    OpcodeSetUUIDResponse = 0x5B80,//remote add, the success opcode of api setRemoteAddDeviceInfoWithUnicastAddress
    
    OpcodeGetFwInfoResponse = 0x02B6,
    
} Opcode;

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
    
} SigOpCode;

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
    DeviceStateOn,
    DeviceStateOff,
    DeviceStateOutOfLine,
} DeviceState;//设备状态

typedef enum : NSUInteger {
    SchedulerTypeOff = 0x0,
    SchedulerTypeOn = 0x1,
    SchedulerTypeScene = 0x2,
    SchedulerTypeNoAction = 0xF,
} SchedulerType;//闹钟类型

typedef enum : NSUInteger {
    AddDeviceModelStateBinding,
    AddDeviceModelStateBindSuccess,
    AddDeviceModelStateBindFail,
} AddDeviceModelState;//添加的设备的状态

typedef enum : UInt16 {
    SigNodePID_CT = 1,
    SigNodePID_Panel = 7,
} SigNodePID;

@interface Model : NSObject

@end


@interface SIGModel : Model
@property (nonatomic,strong) ResponseModel *currentModel;
@property (nonatomic,strong) NSMutableArray <ResponseModel *>*responseModels;
@end


@interface ModelIDModel : Model
@property (nonatomic,strong) NSString *modelGroup;
@property (nonatomic,strong) NSString *modelName;
@property (nonatomic,assign) NSInteger sigModelID;

-(instancetype)initWithModelGroup:(NSString *)modelGroup modelName:(NSString *)modelName sigModelID:(NSInteger)sigModelID;

@end


@interface ModelIDs : Model
@property (nonatomic,assign) NSInteger modelIDCount;
@property (nonatomic,strong) ModelIDModel *model1;
@property (nonatomic,strong) ModelIDModel *model2;
@property (nonatomic,strong) ModelIDModel *model3;
@property (nonatomic,strong) ModelIDModel *model4;
@property (nonatomic,strong) ModelIDModel *model5;
@property (nonatomic,strong) ModelIDModel *model6;
@property (nonatomic,strong) ModelIDModel *model7;
@property (nonatomic,strong) ModelIDModel *model8;
@property (nonatomic,strong) ModelIDModel *model9;
@property (nonatomic,strong) ModelIDModel *model10;
@property (nonatomic,strong) ModelIDModel *model11;
@property (nonatomic,strong) ModelIDModel *model12;
@property (nonatomic,strong) ModelIDModel *model13;
@property (nonatomic,strong) ModelIDModel *model14;
@property (nonatomic,strong) ModelIDModel *model15;
@property (nonatomic,strong) ModelIDModel *model16;
@property (nonatomic,strong) ModelIDModel *model17;
@property (nonatomic,strong) ModelIDModel *model18;
@property (nonatomic,strong) ModelIDModel *model19;
@property (nonatomic,strong) ModelIDModel *model20;
@property (nonatomic,strong) ModelIDModel *model21;
@property (nonatomic,strong) ModelIDModel *model22;
@property (nonatomic,strong) ModelIDModel *model23;
@property (nonatomic,strong) ModelIDModel *model24;
@property (nonatomic,strong) ModelIDModel *model25;
@property (nonatomic,strong) ModelIDModel *model26;
@property (nonatomic,strong) ModelIDModel *model27;
@property (nonatomic,strong) ModelIDModel *model28;
@property (nonatomic,strong) ModelIDModel *model29;
@property (nonatomic,strong) ModelIDModel *model30;
@property (nonatomic,strong) ModelIDModel *model31;
@property (nonatomic,strong) ModelIDModel *model32;
@property (nonatomic,strong) ModelIDModel *model33;
@property (nonatomic,strong) ModelIDModel *model34;
@property (nonatomic,strong) ModelIDModel *model35;
@property (nonatomic,strong) ModelIDModel *model36;
@property (nonatomic,strong) ModelIDModel *model37;
@property (nonatomic,strong) ModelIDModel *model38;
@property (nonatomic,strong) ModelIDModel *model39;
@property (nonatomic,strong) ModelIDModel *model40;
@property (nonatomic,strong) ModelIDModel *model41;
@property (nonatomic,strong) ModelIDModel *model42;
@property (nonatomic,strong) ModelIDModel *model43;
@property (nonatomic,strong) ModelIDModel *model44;
@property (nonatomic,strong) ModelIDModel *model45;
@property (nonatomic,strong) ModelIDModel *model46;
@property (nonatomic,strong) ModelIDModel *model47;
@property (nonatomic,strong) ModelIDModel *model48;
@property (nonatomic,strong) ModelIDModel *model49;
@property (nonatomic,strong) ModelIDModel *model50;
@property (nonatomic,strong) ModelIDModel *model51;
@property (nonatomic,strong) ModelIDModel *model52;
@property (nonatomic,strong) NSArray <ModelIDModel *>*modelIDs;//所有的modelID
@property (nonatomic,strong) NSArray <ModelIDModel *>*defaultModelIDs;//默认keyBind的modelID
@end


@interface Groups : Model
@property (nonatomic,assign) NSInteger groupCount;
@property (nonatomic,strong) NSString *name1;
@property (nonatomic,strong) NSString *name2;
@property (nonatomic,strong) NSString *name3;
@property (nonatomic,strong) NSString *name4;
@property (nonatomic,strong) NSString *name5;
@property (nonatomic,strong) NSString *name6;
@property (nonatomic,strong) NSString *name7;
@property (nonatomic,strong) NSString *name8;
@property (nonatomic,strong) NSArray <NSString *>*names;
@end


@interface SchedulerModel : Model<NSCopying>
@property (nonatomic,assign) UInt8 schedulerID;//range: 0x0~0xF.
@property (nonatomic,assign) UInt64 valid_flag_or_idx;
@property (nonatomic,assign) UInt64 year;
@property (nonatomic,assign) UInt64 month;
@property (nonatomic,assign) UInt64 day;
@property (nonatomic,assign) UInt64 hour;
@property (nonatomic,assign) UInt64 minute;
@property (nonatomic,assign) UInt64 second;
@property (nonatomic,assign) UInt64 week;
@property (nonatomic,assign) SchedulerType action;
@property (nonatomic,assign) UInt64 transitionTime;
@property (nonatomic,assign) UInt64 schedulerData;
@property (nonatomic,assign) UInt64 sceneId;
- (NSDictionary *)getDictionaryOfSchedulerModel;
- (void)setDictionaryToSchedulerModel:(NSDictionary *)dictionary;
@end


//缓存蓝牙扫描回调的模型
@interface SigScanRspModel : NSObject
@property (nonatomic, strong) NSString *macAddress;
@property (nonatomic, assign) UInt16 CID;//企业ID，默认为0x0211，十进制为529.
@property (nonatomic, assign) UInt16 PID;//产品ID，CT灯为1，面板panel为7.

@property (nonatomic, strong) NSString *uuid;
@property (nonatomic, assign) UInt16 address;
@property (nonatomic, strong) NSData *nodeIdentityData;//byte[0]:type=0x01,byte[1~17]:data
@property (nonatomic, strong) NSData *networkIDData;//byte[0]:type=0x00,byte[1~9]:data

@property (nonatomic, strong) NSDictionary<NSString *,id> *advertisementData;//扫描到的蓝牙设备广播包完整数据

- (instancetype)initWithPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary<NSString *,id> *)advertisementData;
@end


//缓存Remot add扫描回调的模型
@interface SigRemoteScanRspModel : NSObject
@property (nonatomic, assign) UInt16 unicastAddress;
@property (nonatomic, strong) NSData *uuid;
@property (nonatomic, assign) int RSSI;//负值
@property (nonatomic, assign) UInt16 oob;
@property (nonatomic, strong) NSString *macAddress;
- (instancetype)initWithPar:(u8 *)par len:(u8)len;
@end


@interface AddDeviceModel : Model
@property (nonatomic, strong) SigScanRspModel *scanRspModel;
@property (nonatomic, assign) AddDeviceModelState state;
@end

@interface PublishResponseModel : NSObject
@property (nonatomic, assign) UInt8 status;
@property (nonatomic, assign) UInt16 elementAddress;
@property (nonatomic, assign) UInt16 publishAddress;
@property (nonatomic, assign) UInt16 appKeyIndex;
@property (nonatomic, assign) UInt8 credentialFlag;
@property (nonatomic, assign) UInt8 RFU;
@property (nonatomic, assign) UInt8 publishTTL;
@property (nonatomic, assign) UInt8 publishPeriod;
@property (nonatomic, assign) UInt8 publishRetransmitCount;
@property (nonatomic, assign) UInt8 publishRetransmitIntervalSteps;
@property (nonatomic, assign) BOOL isVendorModelID;
@property (nonatomic, assign) UInt32 modelIdentifier;

- (instancetype)initWithResponseData:(NSData *)rspData;

@end

@interface ActionModel : Model
@property (nonatomic,assign) UInt16 address;
@property (nonatomic,assign) DeviceState state;
@property (nonatomic,assign) UInt8 trueBrightness;//1-100
@property (nonatomic,assign) UInt8 trueTemperature;//0-100
- (instancetype)initWithNode:(SigNodeModel *)node;
- (BOOL)isSameActionWithAction:(ActionModel *)action;
- (NSDictionary *)getDictionaryOfActionModel;
- (void)setDictionaryToActionModel:(NSDictionary *)dictionary;
@end

@interface BLEStore : SIGModel

@property (nonatomic,strong) NSMutableArray <CBPeripheral *>*scanedPeripherals;
@property (nonatomic,strong) CBPeripheral *peripheral; //当前直连的设备
@property (nonatomic,strong) CBCharacteristic *OTACharacteristic;
@property (nonatomic,strong) CBCharacteristic *PBGATT_OutCharacteristic;
@property (nonatomic,strong) CBCharacteristic *PBGATT_InCharacteristic;
@property (nonatomic,strong) CBCharacteristic *PROXY_OutCharacteristic;
@property (nonatomic,strong) CBCharacteristic *PROXY_InCharacteristic;
@property (nonatomic,strong) CBCharacteristic *OnlineStatusCharacteristic;//私有定制，上报节点的状态的特征
@property (nonatomic,strong) CBCharacteristic *MeshOTACharacteristic;
@property (nonatomic,copy) BeaconBackCallBack beaconBackCallBack;

- (BOOL)isWorkNormal;

- (void)saveToLocal:(CBPeripheral *)tempPeripheral;

- (void)addRSSIWithPeripheral:(CBPeripheral *)peripheral RSSI:(int)rssi;

- (void)clearRecord;

- (CBPeripheral *)hightestRSSI;

- (void)ressetParameters;

- (void)libBeaconBack:(BOOL)available;

- (void)writeForPROXYIn:(NSData *)d;

- (void)writeForPBGATTIn:(NSData *)d;

- (void)writeForOnlineStatus:(NSData *)d;

- (void)writeOTAData:(NSData *)d;

- (void)writeMeshOTAData:(NSData *)d;

- (void)readOTACharachteristicForContinue;

@end

@interface RangeModel : Model

@property (nonatomic, strong) NSString *lowAddress;

@property (nonatomic, strong) NSString *highAddress;

@end

static Byte CTByte[] = {(Byte) 0x11, (Byte) 0x02, (Byte) 0x01, (Byte) 0x00, (Byte) 0x32, (Byte) 0x37, (Byte) 0x69, (Byte) 0x00, (Byte) 0x07, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x19, (Byte) 0x01, (Byte) 0x00, (Byte) 0x00, (Byte) 0x02, (Byte) 0x00, (Byte) 0x03, (Byte) 0x00, (Byte) 0x04, (Byte) 0x00, (Byte) 0x05, (Byte) 0x00, (Byte) 0x00, (Byte) 0xfe, (Byte) 0x01, (Byte) 0xfe, (Byte) 0x02, (Byte) 0xfe, (Byte) 0x00, (Byte) 0xff, (Byte) 0x01, (Byte) 0xff, (Byte) 0x00, (Byte) 0x12, (Byte) 0x01, (Byte) 0x12, (Byte) 0x00, (Byte) 0x10, (Byte) 0x02, (Byte) 0x10, (Byte) 0x04, (Byte) 0x10, (Byte) 0x06, (Byte) 0x10, (Byte) 0x07, (Byte) 0x10, (Byte) 0x03, (Byte) 0x12, (Byte) 0x04, (Byte) 0x12, (Byte) 0x06, (Byte) 0x12, (Byte) 0x07, (Byte) 0x12, (Byte) 0x00, (Byte) 0x13, (Byte) 0x01, (Byte) 0x13, (Byte) 0x03, (Byte) 0x13, (Byte) 0x04, (Byte) 0x13, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x02, (Byte) 0x00, (Byte) 0x02, (Byte) 0x10, (Byte) 0x06, (Byte) 0x13};
static Byte PanelByte[] = {(Byte) 0x11, (Byte) 0x02, (Byte) 0x07, (Byte) 0x00, (Byte) 0x32, (Byte) 0x37, (Byte) 0x69, (Byte) 0x00, (Byte) 0x07, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00, (Byte) 0x02, (Byte) 0x00, (Byte) 0x03, (Byte) 0x00, (Byte) 0x04, (Byte) 0x00, (Byte) 0x05, (Byte) 0x00, (Byte) 0x00, (Byte) 0xfe, (Byte) 0x01, (Byte) 0xfe, (Byte) 0x02, (Byte) 0xfe, (Byte) 0x00, (Byte) 0xff, (Byte) 0x01, (Byte) 0xff, (Byte) 0x00, (Byte) 0x12, (Byte) 0x01, (Byte) 0x12, (Byte) 0x00, (Byte) 0x10, (Byte) 0x03, (Byte) 0x12, (Byte) 0x04, (Byte) 0x12, (Byte) 0x06, (Byte) 0x12, (Byte) 0x07, (Byte) 0x12, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00, (Byte) 0x11, (Byte) 0x02, (Byte) 0x01, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x05, (Byte) 0x01, (Byte) 0x00, (Byte) 0x10, (Byte) 0x03, (Byte) 0x12, (Byte) 0x04, (Byte) 0x12, (Byte) 0x06, (Byte) 0x12, (Byte) 0x07, (Byte) 0x12, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x05, (Byte) 0x01, (Byte) 0x00, (Byte) 0x10, (Byte) 0x03, (Byte) 0x12, (Byte) 0x04, (Byte) 0x12, (Byte) 0x06, (Byte) 0x12, (Byte) 0x07, (Byte) 0x12, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00};

@interface DeviceTypeModel : Model
@property (nonatomic, assign) UInt16 CID;

@property (nonatomic, assign) SigNodePID PID;

@property (nonatomic, assign) VC_node_info_t defultNodeInfo;

- (instancetype)initWithCID:(UInt16)cid PID:(SigNodePID)pid;
- (void)setDefultNodeInfoData:(NSData *)data;

@end


@interface SigAddConfigModel : Model
@property (nonatomic, strong) CBPeripheral *peripheral;
@property (nonatomic, assign) UInt16 unicastAddress;
@property (nonatomic, strong) NSData *networkKey;
@property (nonatomic, assign) UInt16 netkeyIndex;
@property (nonatomic, strong) NSData *appKey;
@property (nonatomic, assign) UInt16 appkeyIndex;
@property (nonatomic, assign) ProvisionTpye provisionType;
@property (nonatomic, strong) NSData *staticOOBData;
@property (nonatomic, assign) KeyBindTpye keyBindType;
@property (nonatomic, assign) UInt16 productID;
@property (nonatomic, strong) NSData *cpsData;

- (instancetype)initWithCBPeripheral:(CBPeripheral *)peripheral unicastAddress:(UInt16)unicastAddress networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appKey:(NSData *)appkey appkeyIndex:(UInt16)appkeyIndex provisionType:(ProvisionTpye)provisionType staticOOBData:(NSData *)staticOOBData keyBindType:(KeyBindTpye)keyBindType productID:(UInt16)productID cpsData:(NSData *)cpsData;

@end


/// sig model struct: mesh_bulk_cmd_par_t, vendor model struct: mesh_vendor_par_ini_t. It is sig model command when vendorId is 0, and It is vendor model command when vendorId isn't 0. sig model config: netkeyIndex, appkeyIndex, retryCount, responseMax, address, opcode, commandData.
@interface IniCommandModel : Model
@property (nonatomic, assign) UInt16 netkeyIndex;
@property (nonatomic, assign) UInt16 appkeyIndex;
@property (nonatomic, assign) UInt8 retryCount;// only for reliable command
@property (nonatomic, assign) UInt8 responseMax;// only for reliable command
@property (nonatomic, assign) UInt16 address;
@property (nonatomic, assign) UInt16 opcode;// SigGenericOnOffSet:0x8202. SigGenericOnOffSetUnacknowledged:0x8203. VendorOnOffSet:0xC4, VendorOnOffSetUnacknowledged:0xC3.
@property (nonatomic, assign) UInt16 vendorId;// 0 means sig model command, other means vendor model command.
@property (nonatomic, assign) UInt8 responseOpcode;// response of VendorOnOffSet:0xC4.
@property (nonatomic, assign) BOOL needTid;
@property (nonatomic, assign) UInt8 tid;
@property (nonatomic, strong) NSData *commandData;//max length is MESH_CMD_ACCESS_LEN_MAX. SigGenericOnOffSet: commandData of turn on without TransitionTime and delay is {0x01,0x00,0x00}. commandData of turn off without TransitionTime and delay is {0x00,0x00,0x00}

@property (nonatomic,copy) responseModelCallBack responseCallBack;
@property (nonatomic, assign) BOOL hasReceiveResponse;

- (instancetype)initSigModelIniCommandWithNetkeyIndex:(UInt16)netkeyIndex appkeyIndex:(UInt16)appkeyIndex retryCount:(UInt8)retryCount responseMax:(UInt8)responseMax address:(UInt16)address opcode:(UInt16)opcode commandData:(NSData *)commandData;
- (instancetype)initVendorModelIniCommandWithNetkeyIndex:(UInt16)netkeyIndex appkeyIndex:(UInt16)appkeyIndex retryCount:(UInt8)retryCount responseMax:(UInt8)responseMax address:(UInt16)address opcode:(UInt8)opcode vendorId:(UInt16)vendorId responseOpcode:(UInt8)responseOpcode needTid:(BOOL)needTid tid:(UInt8)tid commandData:(NSData *)commandData;


/// create model by ini data
/// @param iniCommandData ini data, eg: "a3ff000000000200ffffc21102c4020100"
- (instancetype)initWithIniCommandData:(NSData *)iniCommandData;

@end
