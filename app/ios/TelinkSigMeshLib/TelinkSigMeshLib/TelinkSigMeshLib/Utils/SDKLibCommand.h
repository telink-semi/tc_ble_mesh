/********************************************************************************************************
 * @file     SDKLibCommand.h
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
//  SDKLibCommand.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/4.
//  Copyright © 2019 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class SigMessageHandle,SigTransitionTime,SigMeshMessage,SigBaseMeshMessage,SigPublish,SigTimeModel,SigFilterStatus;
@class SigConfigAppKeyStatus,SigConfigAppKeyList,SigConfigBeaconStatus,SigConfigCompositionDataStatus,SigConfigDefaultTtlStatus,SigConfigFriendStatus,SigConfigGATTProxyStatus,SigConfigModelPublicationStatus,SigConfigModelSubscriptionStatus,SigConfigNetworkTransmitStatus,SigConfigRelayStatus,SigConfigSIGModelSubscriptionList,SigConfigVendorModelSubscriptionList,SigConfigModelAppStatus,SigConfigNetKeyStatus,SigConfigNetKeyList,SigConfigNodeIdentityStatus,SigConfigNodeResetStatus,SigConfigSIGModelAppList,SigConfigVendorModelAppList;
@class SigGenericOnOffStatus,SigGenericLevelStatus,SigGenericDefaultTransitionTimeStatus,SigGenericOnPowerUpStatus,SigGenericPowerLevelStatus,SigGenericPowerLastStatus,SigGenericPowerDefaultStatus,SigGenericPowerRangeStatus,SigGenericBatteryStatus,SigSensorDescriptorStatus,SigSensorStatus,SigSensorColumnStatus,SigSensorSeriesStatus,SigSensorCadenceStatus,SigSensorSettingsStatus,SigSensorSettingStatus,SigTimeStatus,SigTimeRoleStatus,SigTimeZoneStatus,SigTAI_UTC_DeltaStatus,SigSceneStatus,SigSceneRegisterStatus,SigSchedulerActionStatus,SigSchedulerStatus,SigLightLightnessStatus,SigLightLightnessLinearStatus,SigLightLightnessLastStatus,SigLightLightnessDefaultStatus,SigLightLightnessRangeStatus,SigLightCTLStatus,SigLightCTLTemperatureRangeStatus,SigLightCTLTemperatureStatus,SigLightCTLDefaultStatus,SigLightHSLHueStatus,SigLightHSLSaturationStatus,SigLightHSLStatus,SigLightHSLTargetStatus,SigLightHSLDefaultStatus,SigLightHSLRangeStatus,SigLightXyLStatus,SigLightXyLTargetStatus,SigLightXyLDefaultStatus,SigLightXyLRangeStatus,SigLightLCModeStatus,SigLightLCOMStatus,SigLightLCLightOnOffStatus,SigLightLCPropertyStatus;
@class SigFirmwareUpdateInformationStatus,SigFirmwareUpdateFirmwareMetadataStatus,SigFirmwareUpdateStatus,SigFirmwareDistributionStatus,SigFirmwareDistributionDetailsList;
@class SigBLOBTransferStatus,SigObjectBlockTransferStatus,SigBLOBBlockStatus,SigBLOBInformationStatus;
@class SigRemoteProvisioningScanCapabilitiesStatus,SigRemoteProvisioningScanStatus,SigRemoteProvisioningLinkStatus;

typedef void(^resultBlock)(BOOL isResponseAll, NSError * _Nullable error);
typedef void(^ErrorBlock)(NSError * _Nullable error);

typedef void(^responseAllMessageBlock)(UInt16 source,UInt16 destination,SigMeshMessage *responseMessage);

typedef void(^responseFilterStatusMessageBlock)(UInt16 source,UInt16 destination,SigFilterStatus *responseMessage);

typedef void(^responseConfigAppKeyStatusMessageBlock)(UInt16 source,UInt16 destination,SigConfigAppKeyStatus *responseMessage);
typedef void(^responseConfigAppKeyListMessageBlock)(UInt16 source,UInt16 destination,SigConfigAppKeyList *responseMessage);
typedef void(^responseConfigBeaconStatusMessageBlock)(UInt16 source,UInt16 destination,SigConfigBeaconStatus *responseMessage);
typedef void(^responseConfigCompositionDataStatusMessageBlock)(UInt16 source,UInt16 destination,SigConfigCompositionDataStatus *responseMessage);
typedef void(^responseConfigDefaultTtlStatusMessageBlock)(UInt16 source,UInt16 destination,SigConfigDefaultTtlStatus *responseMessage);
typedef void(^responseConfigFriendStatusMessageBlock)(UInt16 source,UInt16 destination,SigConfigFriendStatus *responseMessage);
typedef void(^responseConfigGATTProxyStatusMessageBlock)(UInt16 source,UInt16 destination,SigConfigGATTProxyStatus *responseMessage);
typedef void(^responseConfigModelPublicationStatusMessageBlock)(UInt16 source,UInt16 destination,SigConfigModelPublicationStatus *responseMessage);
typedef void(^responseConfigModelSubscriptionStatusMessageBlock)(UInt16 source,UInt16 destination,SigConfigModelSubscriptionStatus *responseMessage);
typedef void(^responseConfigNetworkTransmitStatusMessageBlock)(UInt16 source,UInt16 destination,SigConfigNetworkTransmitStatus *responseMessage);
typedef void(^responseConfigRelayStatusMessageBlock)(UInt16 source,UInt16 destination,SigConfigRelayStatus *responseMessage);
typedef void(^responseConfigSIGModelSubscriptionListMessageBlock)(UInt16 source,UInt16 destination,SigConfigSIGModelSubscriptionList *responseMessage);
typedef void(^responseConfigVendorModelSubscriptionListMessageBlock)(UInt16 source,UInt16 destination,SigConfigVendorModelSubscriptionList *responseMessage);
typedef void(^responseConfigModelAppStatusMessageBlock)(UInt16 source,UInt16 destination,SigConfigModelAppStatus *responseMessage);
typedef void(^responseConfigNetKeyStatusMessageBlock)(UInt16 source,UInt16 destination,SigConfigNetKeyStatus *responseMessage);
typedef void(^responseConfigNetKeyListMessageBlock)(UInt16 source,UInt16 destination,SigConfigNetKeyList *responseMessage);
typedef void(^responseConfigNodeIdentityStatusMessageBlock)(UInt16 source,UInt16 destination,SigConfigNodeIdentityStatus *responseMessage);
typedef void(^responseConfigNodeResetStatusMessageBlock)(UInt16 source,UInt16 destination,SigConfigNodeResetStatus *responseMessage);
typedef void(^responseConfigSIGModelAppListMessageBlock)(UInt16 source,UInt16 destination,SigConfigSIGModelAppList *responseMessage);
typedef void(^responseConfigVendorModelAppListMessageBlock)(UInt16 source,UInt16 destination,SigConfigVendorModelAppList *responseMessage);

typedef void(^responseGenericOnOffStatusMessageBlock)(UInt16 source,UInt16 destination,SigGenericOnOffStatus *responseMessage);
typedef void(^responseGenericLevelStatusMessageBlock)(UInt16 source,UInt16 destination,SigGenericLevelStatus *responseMessage);
typedef void(^responseGenericDefaultTransitionTimeStatusMessageBlock)(UInt16 source,UInt16 destination,SigGenericDefaultTransitionTimeStatus *responseMessage);
typedef void(^responseGenericOnPowerUpStatusMessageBlock)(UInt16 source,UInt16 destination,SigGenericOnPowerUpStatus *responseMessage);
typedef void(^responseGenericPowerLevelStatusMessageBlock)(UInt16 source,UInt16 destination,SigGenericPowerLevelStatus *responseMessage);
typedef void(^responseGenericPowerLastStatusMessageBlock)(UInt16 source,UInt16 destination,SigGenericPowerLastStatus *responseMessage);
typedef void(^responseGenericPowerDefaultStatusMessageBlock)(UInt16 source,UInt16 destination,SigGenericPowerDefaultStatus *responseMessage);
typedef void(^responseGenericPowerRangeStatusMessageBlock)(UInt16 source,UInt16 destination,SigGenericPowerRangeStatus *responseMessage);
typedef void(^responseGenericBatteryStatusMessageBlock)(UInt16 source,UInt16 destination,SigGenericBatteryStatus *responseMessage);
typedef void(^responseSensorDescriptorStatusMessageBlock)(UInt16 source,UInt16 destination,SigSensorDescriptorStatus *responseMessage);
typedef void(^responseSensorStatusMessageBlock)(UInt16 source,UInt16 destination,SigSensorStatus *responseMessage);
typedef void(^responseSensorColumnStatusMessageBlock)(UInt16 source,UInt16 destination,SigSensorColumnStatus *responseMessage);
typedef void(^responseSensorSeriesStatusMessageBlock)(UInt16 source,UInt16 destination,SigSensorSeriesStatus *responseMessage);
typedef void(^responseSensorCadenceStatusMessageBlock)(UInt16 source,UInt16 destination,SigSensorCadenceStatus *responseMessage);
typedef void(^responseSensorSettingsStatusMessageBlock)(UInt16 source,UInt16 destination,SigSensorSettingsStatus *responseMessage);
typedef void(^responseSensorSettingStatusMessageBlock)(UInt16 source,UInt16 destination,SigSensorSettingStatus *responseMessage);
typedef void(^responseTimeStatusMessageBlock)(UInt16 source,UInt16 destination,SigTimeStatus *responseMessage);
typedef void(^responseTimeRoleStatusMessageBlock)(UInt16 source,UInt16 destination,SigTimeRoleStatus *responseMessage);
typedef void(^responseTimeZoneStatusMessageBlock)(UInt16 source,UInt16 destination,SigTimeZoneStatus *responseMessage);
typedef void(^responseTAI_UTC_DeltaStatusMessageBlock)(UInt16 source,UInt16 destination,SigTAI_UTC_DeltaStatus *responseMessage);
typedef void(^responseSceneStatusMessageBlock)(UInt16 source,UInt16 destination,SigSceneStatus *responseMessage);
typedef void(^responseSceneRegisterStatusMessageBlock)(UInt16 source,UInt16 destination,SigSceneRegisterStatus *responseMessage);
typedef void(^responseSchedulerActionStatusMessageBlock)(UInt16 source,UInt16 destination,SigSchedulerActionStatus *responseMessage);
typedef void(^responseSchedulerStatusMessageBlock)(UInt16 source,UInt16 destination,SigSchedulerStatus *responseMessage);
typedef void(^responseLightLightnessStatusMessageBlock)(UInt16 source,UInt16 destination,SigLightLightnessStatus *responseMessage);
typedef void(^responseLightLightnessLinearStatusMessageBlock)(UInt16 source,UInt16 destination,SigLightLightnessLinearStatus *responseMessage);
typedef void(^responseLightLightnessLastStatusMessageBlock)(UInt16 source,UInt16 destination,SigLightLightnessLastStatus *responseMessage);
typedef void(^responseLightLightnessDefaultStatusMessageBlock)(UInt16 source,UInt16 destination,SigLightLightnessDefaultStatus *responseMessage);
typedef void(^responseLightLightnessRangeStatusMessageBlock)(UInt16 source,UInt16 destination,SigLightLightnessRangeStatus *responseMessage);
typedef void(^responseLightCTLStatusMessageBlock)(UInt16 source,UInt16 destination,SigLightCTLStatus *responseMessage);
typedef void(^responseLightCTLTemperatureRangeStatusMessageBlock)(UInt16 source,UInt16 destination,SigLightCTLTemperatureRangeStatus *responseMessage);
typedef void(^responseLightCTLTemperatureStatusMessageBlock)(UInt16 source,UInt16 destination,SigLightCTLTemperatureStatus *responseMessage);
typedef void(^responseLightCTLDefaultStatusMessageBlock)(UInt16 source,UInt16 destination,SigLightCTLDefaultStatus *responseMessage);
typedef void(^responseLightHSLHueStatusMessageBlock)(UInt16 source,UInt16 destination,SigLightHSLHueStatus *responseMessage);
typedef void(^responseLightHSLSaturationStatusMessageBlock)(UInt16 source,UInt16 destination,SigLightHSLSaturationStatus *responseMessage);
typedef void(^responseLightHSLStatusMessageBlock)(UInt16 source,UInt16 destination,SigLightHSLStatus *responseMessage);
typedef void(^responseLightHSLTargetStatusMessageBlock)(UInt16 source,UInt16 destination,SigLightHSLTargetStatus *responseMessage);
typedef void(^responseLightHSLDefaultStatusMessageBlock)(UInt16 source,UInt16 destination,SigLightHSLDefaultStatus *responseMessage);
typedef void(^responseLightHSLRangeStatusMessageBlock)(UInt16 source,UInt16 destination,SigLightHSLRangeStatus *responseMessage);
typedef void(^responseLightXyLStatusMessageBlock)(UInt16 source,UInt16 destination,SigLightXyLStatus *responseMessage);
typedef void(^responseLightXyLTargetStatusMessageBlock)(UInt16 source,UInt16 destination,SigLightXyLTargetStatus *responseMessage);
typedef void(^responseLightXyLDefaultStatusMessageBlock)(UInt16 source,UInt16 destination,SigLightXyLDefaultStatus *responseMessage);
typedef void(^responseLightXyLRangeStatusMessageBlock)(UInt16 source,UInt16 destination,SigLightXyLRangeStatus *responseMessage);
typedef void(^responseLightLCModeStatusMessageBlock)(UInt16 source,UInt16 destination,SigLightLCModeStatus *responseMessage);
typedef void(^responseLightLCOMStatusMessageBlock)(UInt16 source,UInt16 destination,SigLightLCOMStatus *responseMessage);
typedef void(^responseLightLCLightOnOffStatusMessageBlock)(UInt16 source,UInt16 destination,SigLightLCLightOnOffStatus *responseMessage);
typedef void(^responseLightLCPropertyStatusMessageBlock)(UInt16 source,UInt16 destination,SigLightLCPropertyStatus *responseMessage);

// callback about Firmware Update Messages
typedef void(^responseFirmwareInformationStatusMessageBlock)(UInt16 source,UInt16 destination,SigFirmwareUpdateInformationStatus *responseMessage);
typedef void(^responseFirmwareUpdateFirmwareMetadataStatusMessageBlock)(UInt16 source,UInt16 destination,SigFirmwareUpdateFirmwareMetadataStatus *responseMessage);
typedef void(^responseFirmwareUpdateStatusMessageBlock)(UInt16 source,UInt16 destination,SigFirmwareUpdateStatus *responseMessage);
typedef void(^responseFirmwareDistributionStatusMessageBlock)(UInt16 source,UInt16 destination,SigFirmwareDistributionStatus *responseMessage);
typedef void(^responseFirmwareDistributionDetailsListMessageBlock)(UInt16 source,UInt16 destination,SigFirmwareDistributionDetailsList *responseMessage);

// callback about BLOB Transfer Messages
typedef void(^responseBLOBTransferStatusMessageBlock)(UInt16 source,UInt16 destination,SigBLOBTransferStatus *responseMessage);
typedef void(^responseObjectBlockTransferStatusMessageBlock)(UInt16 source,UInt16 destination,SigObjectBlockTransferStatus *responseMessage);
typedef void(^responseBLOBBlockStatusMessageBlock)(UInt16 source,UInt16 destination,SigBLOBBlockStatus *responseMessage);
typedef void(^responseBLOBInformationStatusMessageBlock)(UInt16 source,UInt16 destination,SigBLOBInformationStatus *responseMessage);

// callback about Remote Provision
typedef void(^responseRemoteProvisioningScanCapabilitiesStatusMessageBlock)(UInt16 source,UInt16 destination,SigRemoteProvisioningScanCapabilitiesStatus *responseMessage);
typedef void(^responseRemoteProvisioningScanStatusMessageBlock)(UInt16 source,UInt16 destination,SigRemoteProvisioningScanStatus *responseMessage);
typedef void(^responseRemoteProvisioningLinkStatusMessageBlock)(UInt16 source,UInt16 destination,SigRemoteProvisioningLinkStatus *responseMessage);

// callback about addDevice
typedef void(^addDevice_prvisionSuccessCallBack)(NSString *identify,UInt16 address);
typedef void(^addDevice_keyBindSuccessCallBack)(NSString *identify,UInt16 address);
typedef void(^AddDeviceFinishCallBack)(void);

typedef enum : UInt8 {
    SigCommandType_meshMessage,
    SigCommandType_configMessage,
    SigCommandType_proxyConfigurationMessage,
    SigCommandType_unknownMessage,
} SigCommandType;

@interface SDKLibCommand : NSObject
@property (nonatomic,strong) SigBaseMeshMessage *curMeshMessage;
//@property (nonatomic,strong) SigMeshMessage *curMeshMessage;

@property (nonatomic,strong) SigElementModel *source;
@property (nonatomic,strong) SigMeshAddress *destination;
@property (nonatomic,assign) UInt8 initialTtl;
@property (nonatomic,assign) SigCommandType commandType;
@property (nonatomic,strong) SigMessageHandle *messageHandle;

@property (nonatomic,assign) NSInteger responseMaxCount;
@property (nonatomic,strong) NSMutableArray <NSNumber *>*responseSourceArray;
@property (nonatomic,assign) UInt8 retryCount;//default is 2.
@property (nonatomic,assign) NSTimeInterval timeout;//default is 1s,kSDKLibCommandTimeout.
@property (nonatomic,assign) UInt8 hadRetryCount;//default is 0.
@property (nonatomic, assign) BOOL needTid;//default is NO.
@property (nonatomic, assign) UInt8 tid;//default is 0.
@property (nonatomic,strong,nullable) BackgroundTimer *retryTimer;
@property (nonatomic,strong) SigNetkeyModel *netkeyA;
@property (nonatomic,strong) SigAppkeyModel *appkeyA;
@property (nonatomic,strong) SigIvIndex *ivIndexA;

#pragma mark - Save call back
@property (nonatomic,copy) resultBlock resultCallback;
@property (nonatomic,copy) responseAllMessageBlock responseAllMessageCallBack;

@property (nonatomic,copy) responseFilterStatusMessageBlock responseFilterStatusCallBack;

@property (nonatomic,copy) responseConfigAppKeyStatusMessageBlock responseAppKeyStatusCallBack;
@property (nonatomic,copy) responseConfigAppKeyListMessageBlock responseAppKeyListCallBack;
@property (nonatomic,copy) responseConfigBeaconStatusMessageBlock responseBeaconStatusCallBack;
@property (nonatomic,copy) responseConfigCompositionDataStatusMessageBlock responseCompositionDataStatusCallBack;
@property (nonatomic,copy) responseConfigDefaultTtlStatusMessageBlock responseDefaultTtlStatusCallBack;
@property (nonatomic,copy) responseConfigFriendStatusMessageBlock responseFriendStatusCallBack;
@property (nonatomic,copy) responseConfigGATTProxyStatusMessageBlock responseGATTProxyStatusCallBack;
@property (nonatomic,copy) responseConfigModelPublicationStatusMessageBlock responseModelPublicationStatusCallBack;
@property (nonatomic,copy) responseConfigModelSubscriptionStatusMessageBlock responseModelSubscriptionStatusCallBack;
@property (nonatomic,copy) responseConfigNetworkTransmitStatusMessageBlock responseNetworkTransmitStatusCallBack;
@property (nonatomic,copy) responseConfigRelayStatusMessageBlock responseRelayStatusCallBack;
@property (nonatomic,copy) responseConfigSIGModelSubscriptionListMessageBlock responseSIGModelSubscriptionListCallBack;
@property (nonatomic,copy) responseConfigVendorModelSubscriptionListMessageBlock responseVendorModelSubscriptionListCallBack;
@property (nonatomic,copy) responseConfigModelAppStatusMessageBlock responseModelAppStatusCallBack;
@property (nonatomic,copy) responseConfigNetKeyStatusMessageBlock responseNetKeyStatusCallBack;
@property (nonatomic,copy) responseConfigNetKeyListMessageBlock responseNetKeyListCallBack;
@property (nonatomic,copy) responseConfigNodeIdentityStatusMessageBlock responseNodeIdentityStatusCallBack;
@property (nonatomic,copy) responseConfigNodeResetStatusMessageBlock responseNodeResetStatusCallBack;
@property (nonatomic,copy) responseConfigSIGModelAppListMessageBlock responseSIGModelAppListCallBack;
@property (nonatomic,copy) responseConfigVendorModelAppListMessageBlock responseVendorModelAppListCallBack;

@property (nonatomic,copy) responseGenericOnOffStatusMessageBlock responseOnOffStatusCallBack;
@property (nonatomic,copy) responseGenericLevelStatusMessageBlock responseLevelStatusCallBack;
@property (nonatomic,copy) responseGenericDefaultTransitionTimeStatusMessageBlock responseDefaultTransitionTimeStatusCallBack;
@property (nonatomic,copy) responseGenericOnPowerUpStatusMessageBlock responseOnPowerUpStatusCallBack;
@property (nonatomic,copy) responseGenericPowerLevelStatusMessageBlock responsePowerLevelStatusCallBack;
@property (nonatomic,copy) responseGenericPowerLastStatusMessageBlock responsePowerLastStatusCallBack;
@property (nonatomic,copy) responseGenericPowerDefaultStatusMessageBlock responsePowerDefaultStatusCallBack;
@property (nonatomic,copy) responseGenericPowerRangeStatusMessageBlock responsePowerRangeStatusCallBack;
@property (nonatomic,copy) responseGenericBatteryStatusMessageBlock responseBatteryStatusCallBack;
@property (nonatomic,copy) responseSensorDescriptorStatusMessageBlock responseSensorDescriptorStatusCallBack;
@property (nonatomic,copy) responseSensorStatusMessageBlock responseSensorStatusCallBack;
@property (nonatomic,copy) responseSensorColumnStatusMessageBlock responseSensorColumnStatusCallBack;
@property (nonatomic,copy) responseSensorSeriesStatusMessageBlock responseSensorSeriesStatusCallBack;
@property (nonatomic,copy) responseSensorCadenceStatusMessageBlock responseSensorCadenceStatusCallBack;
@property (nonatomic,copy) responseSensorSettingsStatusMessageBlock responseSensorSettingsStatusCallBack;
@property (nonatomic,copy) responseSensorSettingStatusMessageBlock responseSensorSettingStatusCallBack;
@property (nonatomic,copy) responseTimeStatusMessageBlock responseTimeStatusCallBack;
@property (nonatomic,copy) responseTimeRoleStatusMessageBlock responseTimeRoleStatusCallBack;
@property (nonatomic,copy) responseTimeZoneStatusMessageBlock responseTimeZoneStatusCallBack;
@property (nonatomic,copy) responseTAI_UTC_DeltaStatusMessageBlock responseTAI_UTC_DeltaStatusCallBack;
@property (nonatomic,copy) responseSceneStatusMessageBlock responseSceneStatusCallBack;
@property (nonatomic,copy) responseSceneRegisterStatusMessageBlock responseSceneRegisterStatusCallBack;
@property (nonatomic,copy) responseSchedulerActionStatusMessageBlock responseSchedulerActionStatusCallBack;
@property (nonatomic,copy) responseSchedulerStatusMessageBlock responseSchedulerStatusCallBack;
@property (nonatomic,copy) responseLightLightnessStatusMessageBlock responseLightLightnessStatusCallBack;
@property (nonatomic,copy) responseLightLightnessLinearStatusMessageBlock responseLightLightnessLinearStatusCallBack;
@property (nonatomic,copy) responseLightLightnessLastStatusMessageBlock responseLightLightnessLastStatusCallBack;
@property (nonatomic,copy) responseLightLightnessDefaultStatusMessageBlock responseLightLightnessDefaultStatusCallBack;
@property (nonatomic,copy) responseLightLightnessRangeStatusMessageBlock responseLightLightnessRangeStatusCallBack;
@property (nonatomic,copy) responseLightCTLStatusMessageBlock responseLightCTLStatusCallBack;
@property (nonatomic,copy) responseLightCTLTemperatureRangeStatusMessageBlock responseLightCTLTemperatureRangeStatusCallBack;
@property (nonatomic,copy) responseLightCTLTemperatureStatusMessageBlock responseLightCTLTemperatureStatusCallBack;
@property (nonatomic,copy) responseLightCTLDefaultStatusMessageBlock responseLightCTLDefaultStatusCallBack;
@property (nonatomic,copy) responseLightHSLHueStatusMessageBlock responseLightHSLHueStatusCallBack;
@property (nonatomic,copy) responseLightHSLSaturationStatusMessageBlock responseLightHSLSaturationStatusCallBack;
@property (nonatomic,copy) responseLightHSLStatusMessageBlock responseLightHSLStatusCallBack;
@property (nonatomic,copy) responseLightHSLTargetStatusMessageBlock responseLightHSLTargetStatusCallBack;
@property (nonatomic,copy) responseLightHSLDefaultStatusMessageBlock responseLightHSLDefaultStatusCallBack;
@property (nonatomic,copy) responseLightHSLRangeStatusMessageBlock responseLightHSLRangeStatusCallBack;
@property (nonatomic,copy) responseLightXyLStatusMessageBlock responseLightXyLStatusCallBack;
@property (nonatomic,copy) responseLightXyLTargetStatusMessageBlock responseLightXyLTargetStatusCallBack;
@property (nonatomic,copy) responseLightXyLDefaultStatusMessageBlock responseLightXyLDefaultStatusCallBack;
@property (nonatomic,copy) responseLightXyLRangeStatusMessageBlock responseLightXyLRangeStatusCallBack;
@property (nonatomic,copy) responseLightLCModeStatusMessageBlock responseLightLCModeStatusCallBack;
@property (nonatomic,copy) responseLightLCOMStatusMessageBlock responseLightLCOMStatusCallBack;
@property (nonatomic,copy) responseLightLCLightOnOffStatusMessageBlock responseLightLCLightOnOffStatusCallBack;
@property (nonatomic,copy) responseLightLCPropertyStatusMessageBlock responseLightLCPropertyStatusCallBack;
// callback about Firmware Update Messages
@property (nonatomic,copy) responseFirmwareInformationStatusMessageBlock responseFirmwareInformationStatusCallBack;
@property (nonatomic,copy) responseFirmwareUpdateFirmwareMetadataStatusMessageBlock responseFirmwareUpdateFirmwareMetadataStatusCallBack;
@property (nonatomic,copy) responseFirmwareUpdateStatusMessageBlock responseFirmwareUpdateStatusCallBack;
@property (nonatomic,copy) responseFirmwareDistributionStatusMessageBlock responseFirmwareDistributionStatusCallBack;
@property (nonatomic,copy) responseFirmwareDistributionDetailsListMessageBlock responseFirmwareDistributionDetailsListCallBack;
// callback about BLOB Transfer Messages
@property (nonatomic,copy) responseBLOBTransferStatusMessageBlock responseBLOBTransferStatusCallBack;
@property (nonatomic,copy) responseObjectBlockTransferStatusMessageBlock responseObjectBlockTransferStatusCallBack;
@property (nonatomic,copy) responseBLOBBlockStatusMessageBlock responseBLOBBlockStatusCallBack;
@property (nonatomic,copy) responseBLOBInformationStatusMessageBlock responseBLOBInformationStatusCallBack;
// callback about Remote Provision
@property (nonatomic,copy) responseRemoteProvisioningScanCapabilitiesStatusMessageBlock responseRemoteProvisioningScanCapabilitiesStatusCallBack;
@property (nonatomic,copy) responseRemoteProvisioningScanStatusMessageBlock responseRemoteProvisioningScanStatusCallBack;
@property (nonatomic,copy) responseRemoteProvisioningLinkStatusMessageBlock responseRemoteProvisioningLinkStatusCallBack;


+ (SigMessageHandle *)configAppKeyAddWithDestination:(UInt16)destination appkeyModel:(SigAppkeyModel *)appkeyModel retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigAppKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configAppKeyUpdateWithDestination:(UInt16)destination appkeyModel:(SigAppkeyModel *)appkeyModel retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigAppKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configAppKeyDeleteWithDestination:(UInt16)destination appkeyModel:(SigAppkeyModel *)appkeyModel retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigAppKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configAppKeyGetWithDestination:(UInt16)destination networkKeyIndex:(UInt16)networkKeyIndex retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigAppKeyListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configBeaconGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigBeaconStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configBeaconSetWithDestination:(UInt16)destination secureNetworkBeaconState:(SigSecureNetworkBeaconState)secureNetworkBeaconState retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigBeaconStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configCompositionDataGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigCompositionDataStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configDefaultTtlGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigDefaultTtlStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configDefaultTtlSetWithDestination:(UInt16)destination ttl:(UInt8)ttl retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigDefaultTtlStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configFriendGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigFriendStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configFriendSetWithDestination:(UInt16)destination nodeFeaturesState:(SigNodeFeaturesState)nodeFeaturesState retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigFriendStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configGATTProxyGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigGATTProxyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configGATTProxySetWithDestination:(UInt16)destination nodeGATTProxyState:(SigNodeGATTProxyState)nodeGATTProxyState retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigGATTProxyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configModelPublicationGetWithDestination:(UInt16)destination elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configModelPublicationSetWithDestination:(UInt16)destination publish:(SigPublish *)publish elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configModelPublicationVirtualAddressSetWithDestination:(UInt16)destination publish:(SigPublish *)publish elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configModelSubscriptionAddWithDestination:(UInt16)destination toGroupAddress:(UInt16)groupAddress elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configModelSubscriptionDeleteWithDestination:(UInt16)destination groupAddress:(UInt16)groupAddress elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configModelSubscriptionDeleteAllWithDestination:(UInt16)destination elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configModelSubscriptionOverwriteWithDestination:(UInt16)destination groupAddress:(UInt16)groupAddress elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configModelSubscriptionVirtualAddressAddWithDestination:(UInt16)destination virtualLabel:(CBUUID *)virtualLabel elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configModelSubscriptionVirtualAddressDeleteWithDestination:(UInt16)destination virtualLabel:(CBUUID *)virtualLabel elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configModelSubscriptionVirtualAddressOverwriteWithDestination:(UInt16)destination virtualLabel:(CBUUID *)virtualLabel elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configNetworkTransmitGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNetworkTransmitStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configNetworkTransmitSetWithDestination:(UInt16)destination networkTransmitCount:(UInt8)networkTransmitCount networkTransmitIntervalSteps:(UInt8)networkTransmitIntervalSteps retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNetworkTransmitStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configRelayGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigRelayStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configRelaySetWithDestination:(UInt16)destination relay:(SigNodeRelayState)relay networkTransmitCount:(UInt8)networkTransmitCount networkTransmitIntervalSteps:(UInt8)networkTransmitIntervalSteps retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigRelayStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configSIGModelSubscriptionGetWithDestination:(UInt16)destination elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigSIGModelSubscriptionListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configVendorModelSubscriptionGetWithDestination:(UInt16)destination elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigVendorModelSubscriptionListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configModelAppBindWithDestination:(UInt16)destination applicationKeyIndex:(UInt16)applicationKeyIndex elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelAppStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configModelAppUnbindWithDestination:(UInt16)destination applicationKeyIndex:(UInt16)applicationKeyIndex elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelAppStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configNetKeyAddWithDestination:(UInt16)destination NetworkKeyIndex:(UInt16)networkKeyIndex networkKeyData:(NSData *)networkKeyData retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNetKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configNetKeyDeleteWithDestination:(UInt16)destination NetworkKeyIndex:(UInt16)networkKeyIndex retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNetKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configNetKeyGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNetKeyListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configNetKeyUpdateWithDestination:(UInt16)destination networkKeyIndex:(UInt16)networkKeyIndex networkKeyData:(NSData *)networkKeyData retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNetKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configNodeIdentityGetWithDestination:(UInt16)destination netKeyIndex:(UInt16)netKeyIndex retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNodeIdentityStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configNodeIdentitySetWithDestination:(UInt16)destination netKeyIndex:(UInt16)netKeyIndex identity:(SigNodeIdentityState)identity retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNodeIdentityStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)resetNodeWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNodeResetStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configSIGModelAppGetWithDestination:(UInt16)destination elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigSIGModelAppListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configVendorModelAppGetWithDestination:(UInt16)destination elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigVendorModelAppListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;




    //Generic OnOff
+ (SigMessageHandle *)genericOnOffGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// OnOffSet with transitionTime
+ (SigMessageHandle *)genericOnOffSetWithDestination:(UInt16)destination isOn:(BOOL)isOn retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// OnOffSet without transitionTime
+ (SigMessageHandle *)genericOnOffSetDestination:(UInt16)destination isOn:(BOOL)isOn transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

    //Generic Level
+ (SigMessageHandle *)genericLevelGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// LevelSet with transitionTime
+ (SigMessageHandle *)genericLevelSetWithDestination:(UInt16)destination level:(UInt16)level transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// LevelSet without transitionTime
+ (SigMessageHandle *)genericLevelSetWithDestination:(UInt16)destination level:(UInt16)level retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// DeltaSet with transitionTime
+ (SigMessageHandle *)genericDeltaSetWithDestination:(UInt16)destination delta:(UInt32)delta transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// DeltaSet without transitionTime
+ (SigMessageHandle *)genericDeltaSetWithDestination:(UInt16)destination delta:(UInt32)delta retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// MoveSet with transitionTime
+ (SigMessageHandle *)genericMoveSetWithDestination:(UInt16)destination deltaLevel:(UInt16)deltaLevel transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// MoveSet without transitionTime
+ (SigMessageHandle *)genericMoveSetWithDestination:(UInt16)destination deltaLevel:(UInt16)deltaLevel retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Generic Default Transition Time
+ (SigMessageHandle *)genericDefaultTransitionTimeGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericDefaultTransitionTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericDefaultTransitionTimeSetWithDestination:(UInt16)destination defaultTransitionTime:(nonnull SigTransitionTime *)defaultTransitionTime retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericDefaultTransitionTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Generic Power OnOff
+ (SigMessageHandle *)genericOnPowerUpGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericOnPowerUpStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericOnPowerUpSetWithDestination:(UInt16)destination onPowerUp:(SigOnPowerUp)onPowerUp retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericOnPowerUpStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Generic Power Level
+ (SigMessageHandle *)genericPowerLevelGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericPowerLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// PowerLevelSet with transitionTime
+ (SigMessageHandle *)genericPowerLevelSetWithDestination:(UInt16)destination power:(UInt16)power transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// PowerLevelSet without transitionTime
+ (SigMessageHandle *)genericPowerLevelSetWithDestination:(UInt16)destination power:(UInt16)power retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericPowerLastGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericPowerLastStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericPowerDefaultGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericPowerDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericPowerRangeGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericPowerRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Generic Power Level Setup
+ (SigMessageHandle *)genericPowerDefaultSetWithDestination:(UInt16)destination power:(UInt16)power retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericPowerDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericPowerRangeSetWithDestination:(UInt16)destination rangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericPowerRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Generic Battery
+ (SigMessageHandle *)genericBatteryGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericBatteryStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Sensor
+ (SigMessageHandle *)sensorDescriptorGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorDescriptorStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sensorDescriptorGetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorDescriptorStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sensorGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sensorGetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sensorColumnGetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID rawValueX:(NSData *)rawValueX retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorColumnStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sensorSeriesGetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID rawValueX1Data:(NSData *)rawValueX1Data rawValueX2Data:(NSData *)rawValueX2Data retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorSeriesStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Sensor Setup
+ (SigMessageHandle *)sensorCadenceGetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorCadenceStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sensorCadenceSetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID cadenceData:(NSData *)cadenceData retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseSensorCadenceStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sensorSettingsGetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorSettingsStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sensorSettingGetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID settingPropertyID:(UInt16)settingPpropertyID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorSettingStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sensorSettingSetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID settingPropertyID:(UInt16)settingPpropertyID settingRaw:(NSData *)settingRaw retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseSensorSettingStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Time
+ (SigMessageHandle *)timeGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)timeSetWithDestination:(UInt16)destination timeModel:(SigTimeModel *)timeModel retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)timeStatusWithDestination:(UInt16)destination timeModel:(SigTimeModel *)timeModel retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(_Nullable responseTimeStatusMessageBlock)successCallback resultCallback:(_Nullable resultBlock)resultCallback;
+ (SigMessageHandle *)timeRoleGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseTimeRoleStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)timeRoleSetWithDestination:(UInt16)destination timeRole:(SigTimeRole)timeRole retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseTimeRoleStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)timeZoneGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseTimeZoneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)timeZoneSetWithDestination:(UInt16)destination timeZoneOffsetNew:(UInt8)timeZoneOffsetNew TAIOfZoneChange:(UInt64)TAIOfZoneChange retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseTimeZoneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)TAI_UTC_DeltaGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseTAI_UTC_DeltaStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)TAI_UTC_DeltaSetWithDestination:(UInt16)destination TAI_UTC_DeltaNew:(UInt16)TAI_UTC_DeltaNew padding:(UInt8)padding TAIOfDeltaChange:(UInt64)TAIOfDeltaChange retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseTAI_UTC_DeltaStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Scene
+ (SigMessageHandle *)sceneGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSceneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// sceneRecall with transitionTime
+ (SigMessageHandle *)sceneRecallWithDestination:(UInt16)destination sceneNumber:(UInt16)sceneNumber transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseSceneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// sceneRecall without transitionTime
+ (SigMessageHandle *)sceneRecallWithDestination:(UInt16)destination sceneNumber:(UInt16)sceneNumber retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseSceneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sceneRegisterGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Scene Setup
+ (SigMessageHandle *)sceneStoreWithDestination:(UInt16)destination sceneNumber:(UInt16)sceneNumber retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sceneDeleteWithDestination:(UInt16)destination sceneNumber:(UInt16)sceneNumber retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Scheduler
+ (SigMessageHandle *)schedulerActionGetWithDestination:(UInt16)destination schedulerIndex:(UInt8)schedulerIndex retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSchedulerActionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)schedulerGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSchedulerStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Scheduler Setup
+ (SigMessageHandle *)schedulerActionSetWithDestination:(UInt16)destination schedulerModel:(SchedulerModel *)schedulerModel retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseSchedulerActionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Light Lightness
+ (SigMessageHandle *)lightLightnessGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLightnessStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// lightLightnessSet with transitionTime
+ (SigMessageHandle *)lightLightnessSetWithDestination:(UInt16)destination lightness:(UInt16)lightness transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLightnessStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// lightLightnessSet without transitionTime
+ (SigMessageHandle *)lightLightnessSetWithDestination:(UInt16)destination lightness:(UInt16)lightness retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLightnessStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLightnessLinearGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLightnessLinearStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// lightLightnessLinearSet with transitionTime
+ (SigMessageHandle *)lightLightnessLinearSetWithDestination:(UInt16)destination lightness:(UInt16)lightness transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLightnessLinearStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// lightLightnessLinearSet without transitionTime
+ (SigMessageHandle *)lightLightnessLinearSetWithDestination:(UInt16)destination lightness:(UInt16)lightness retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLightnessLinearStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLightnessLastGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLightnessLastStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLightnessDefaultGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLightnessDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLightnessRangeGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLightnessRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Light Lightness Setup
+ (SigMessageHandle *)lightLightnessDefaultSetWithDestination:(UInt16)destination lightness:(UInt16)lightness retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLightnessDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLightnessRangeSetWithDestination:(UInt16)destination rangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLightnessRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Light CTL
+ (SigMessageHandle *)lightCTLGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightCTLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// lightCTLSet with transitionTime
+ (SigMessageHandle *)lightCTLSetWithDestination:(UInt16)destination lightness:(UInt16)lightness temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightCTLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// lightCTLSet without transitionTime
+ (SigMessageHandle *)lightCTLSetWithDestination:(UInt16)destination lightness:(UInt16)lightness temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightCTLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightCTLTemperatureGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightCTLTemperatureStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightCTLTemperatureRangeGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightCTLTemperatureRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// lightCTLTemperatureSet with transitionTime
+ (SigMessageHandle *)lightCTLTemperatureSetWithDestination:(UInt16)destination temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightCTLTemperatureStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// lightCTLTemperatureSet without transitionTime
+ (SigMessageHandle *)lightCTLTemperatureSetWithDestination:(UInt16)destination temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightCTLTemperatureStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightCTLDefaultGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightCTLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Light CTL Setup
+ (SigMessageHandle *)lightCTLDefaultSetWithDestination:(UInt16)destination lightness:(UInt16)lightness temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightCTLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightCTLTemperatureRangeSetWithDestination:(UInt16)destination rangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightCTLTemperatureRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Light HSL
+ (SigMessageHandle *)lightHSLGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightHSLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightHSLHueGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightHSLHueStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// lightHSLHueSet with transitionTime
+ (SigMessageHandle *)lightHSLHueSetWithDestination:(UInt16)destination hue:(UInt16)hue transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLHueStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// lightHSLHueSet without transitionTime
+ (SigMessageHandle *)lightHSLHueSetWithDestination:(UInt16)destination hue:(UInt16)hue retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLHueStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightHSLSaturationGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightHSLSaturationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// lightHSLSaturationSet with transitionTime
+ (SigMessageHandle *)lightHSLSaturationSetWithDestination:(UInt16)destination saturation:(UInt16)saturation transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLSaturationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// lightHSLSaturationSet without transitionTime
+ (SigMessageHandle *)lightHSLSaturationSetWithDestination:(UInt16)destination saturation:(UInt16)saturation retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLSaturationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// lightHSLSet with transitionTime
+ (SigMessageHandle *)lightHSLSetWithDestination:(UInt16)destination HSLLight:(UInt16)HSLLight HSLHue:(UInt16)HSLHue HSLSaturation:(UInt16)HSLSaturation transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// lightHSLSet without transitionTime
+ (SigMessageHandle *)lightHSLSetWithDestination:(UInt16)destination HSLLight:(UInt16)HSLLight HSLHue:(UInt16)HSLHue HSLSaturation:(UInt16)HSLSaturation retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightHSLTargetGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightHSLTargetStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightHSLDefaultGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightHSLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightHSLRangeGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightHSLRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Light HSL Setup
+ (SigMessageHandle *)lightHSLDefaultSetWithDestination:(UInt16)destination light:(UInt16)light hue:(UInt16)hue saturation:(UInt16)saturation retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightHSLRangeSetWithDestination:(UInt16)destination hueRangeMin:(UInt16)hueRangeMin hueRangeMax:(UInt16)hueRangeMax saturationRangeMin:(UInt16)saturationRangeMin saturationRangeMax:(UInt16)saturationRangeMax retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Light xyL
+ (SigMessageHandle *)lightXyLGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightXyLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// lightXyLSet with transitionTime
+ (SigMessageHandle *)lightXyLSetWithDestination:(UInt16)destination xyLLightness:(UInt16)xyLLightness xyLx:(UInt16)xyLx xyLy:(UInt16)xyLy transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightXyLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// lightXyLSet without transitionTime
+ (SigMessageHandle *)lightXyLSetWithDestination:(UInt16)destination xyLLightness:(UInt16)xyLLightness xyLx:(UInt16)xyLx xyLy:(UInt16)xyLy retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightXyLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightXyLTargetGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightXyLTargetStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightXyLDefaultGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightXyLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightXyLRangeGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightXyLRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Light xyL Setup
+ (SigMessageHandle *)lightXyLDefaultSetWithDestination:(UInt16)destination lightness:(UInt16)lightness xyLx:(UInt16)xyLx xyLy:(UInt16)xyLy retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightXyLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightXyLRangeSetWithDestination:(UInt16)destination xyLxRangeMin:(UInt16)xyLxRangeMin xyLxRangeMax:(UInt16)xyLxRangeMax xyLyRangeMin:(UInt16)xyLyRangeMin xyLyRangeMax:(UInt16)xyLyRangeMax retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightXyLRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Light Control
+ (SigMessageHandle *)lightLCModeGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLCModeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLCModeSetWithDestination:(UInt16)destination enable:(BOOL)enable retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLCModeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLCOMGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLCOMStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLCOMSetWithDestination:(UInt16)destination enable:(BOOL)enable retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLCOMStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLCLightOnOffGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLCLightOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// lightLCLightOnOffSet with transitionTime
+ (SigMessageHandle *)lightLCLightOnOffSetWithDestination:(UInt16)destination isOn:(BOOL)isOn transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLCLightOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// lightLCLightOnOffSet without transitionTime
+ (SigMessageHandle *)lightLCLightOnOffSetWithDestination:(UInt16)destination isOn:(BOOL)isOn retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLCLightOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Light Control Setup
+ (SigMessageHandle *)lightLCPropertyGetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLCPropertyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLCPropertySetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID propertyValue:(NSData *)propertyValue retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLCPropertyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

#pragma mark - Filter

/// The Set Filter Type message can be sent by a Proxy Client to change the proxy filter type and clear the proxy filter list.
+ (void)setType:(SigProxyFilerType)type successCallback:(responseFilterStatusMessageBlock)successCallback failCallback:(resultBlock)failCallback;

/// Resets the filter to an empty whitelist filter.
+ (void)resetFilterWithSuccessCallback:(responseFilterStatusMessageBlock)successCallback failCallback:(resultBlock)failCallback;

/// The Add Addresses to Filter message is sent by a Proxy Client to add destination addresses to the proxy filter list.
+ (void)addAddressesToFilterWithAddresses:(NSArray <NSNumber *>*)addresses successCallback:(responseFilterStatusMessageBlock)successCallback failCallback:(resultBlock)failCallback;

/// The Remove Addresses from Filter message is sent by a Proxy Client to remove destination addresses from the proxy filter list.
+ (void)removeAddressesFromFilterWithAddresses:(NSArray <NSNumber *>*)addresses successCallback:(responseFilterStatusMessageBlock)successCallback failCallback:(resultBlock)failCallback;

/// Adds all the addresses the Provisioner is subscribed to to the Proxy Filter.
+ (void)setFilterForProvisioner:(SigProvisionerModel *)provisioner successCallback:(responseFilterStatusMessageBlock)successCallback finishCallback:(resultBlock)failCallback;

#pragma mark - Mesh Firmware update (Mesh OTA)

/* Firmware Update Messages */

+ (SigMessageHandle *)firmwareUpdateInformationGetWithDestination:(UInt16)destination firstIndex:(UInt8)firstIndex entriesLimit:(UInt8)entriesLimit retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareInformationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)firmwareUpdateFirmwareMetadataCheckWithDestination:(UInt16)destination updateFirmwareImageIndex:(UInt8)updateFirmwareImageIndex incomingFirmwareMetadata:(nullable NSData *)incomingFirmwareMetadata retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareUpdateFirmwareMetadataStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)firmwareUpdateGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareUpdateStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)firmwareUpdateStartWithDestination:(UInt16)destination updateTTL:(UInt8)updateTTL updateTimeoutBase:(UInt16)updateTimeoutBase updateBLOBID:(UInt64)updateBLOBID updateFirmwareImageIndex:(UInt8)updateFirmwareImageIndex incomingFirmwareMetadata:(nullable NSData *)incomingFirmwareMetadata retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareUpdateStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)firmwareUpdateCancelWithDestination:(UInt16)destination companyID:(UInt16)companyID firmwareID:(NSData *)firmwareID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareUpdateStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)firmwareUpdateApplyWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareUpdateStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)firmwareDistributionGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareDistributionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)firmwareDistributionStartWithDestination:(UInt16)destination distributionAppKeyIndex:(UInt16)distributionAppKeyIndex distributionTTL:(UInt8)distributionTTL distributionTimeoutBase:(UInt16)distributionTimeoutBase distributionTransferMode:(SigTransferModeState)distributionTransferMode updatePolicy:(BOOL)updatePolicy RFU:(UInt8)RFU distributionFirmwareImageIndex:(UInt16)distributionFirmwareImageIndex distributionMulticastAddress:(NSData *)distributionMulticastAddress retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareDistributionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)firmwareDistributionCancelWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareDistributionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//+ (SigMessageHandle *)firmwareDistributionDetailsGetWithDestination:(UInt16)destination status:(SigFirmwareDistributionStatusType)status companyID:(UInt16)companyID firmwareID:(NSData *)firmwareID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareDistributionDetailsListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/* BLOB Transfer Messages */

+ (SigMessageHandle *)BLOBTransferGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseBLOBTransferStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)BLOBTransferStartWithDestination:(UInt16)destination transferMode:(SigTransferModeState)transferMode BLOBID:(UInt64)BLOBID BLOBSize:(UInt32)BLOBSize BLOBBlockSizeLog:(UInt8)BLOBBlockSizeLog MTUSize:(UInt16)MTUSize retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseBLOBTransferStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)BLOBTransferAbortWithDestination:(UInt16)destination BLOBID:(UInt64)BLOBID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseBLOBTransferStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


/// BLOBBlockStart
/// @param destination node address
/// @param blockNumber block index, 0. 1. 2. 3. ··· n.
/// @param chunkSize size of every chunk, 0~256.
/// @param retryCount retry count
/// @param responseMaxCount max response count
/// @param successCallback success Callback
/// @param resultCallback end Callback of fail callback
+ (SigMessageHandle *)BLOBBlockStartWithDestination:(UInt16)destination blockNumber:(UInt16)blockNumber chunkSize:(UInt16)chunkSize retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseBLOBBlockStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)BLOBChunkTransferWithDestination:(UInt16)destination chunkNumber:(UInt16)chunkNumber chunkData:(NSData *)chunkData retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)BLOBBlockGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseBLOBBlockStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)BLOBInformationGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseBLOBInformationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark - Remote Provision

+ (SigMessageHandle *)remoteProvisioningScanCapabilitiesGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseRemoteProvisioningScanCapabilitiesStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)remoteProvisioningScanGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseRemoteProvisioningScanStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)remoteProvisioningScanStartWithDestination:(UInt16)destination scannedItemsLimit:(UInt8)scannedItemsLimit timeout:(UInt8)timeout UUID:(nullable NSData *)UUID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseRemoteProvisioningScanStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)remoteProvisioningScanStopWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseRemoteProvisioningScanStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)remoteProvisioningExtendedScanStartWithDestination:(UInt16)destination ADTypeFilterCount:(UInt8)ADTypeFilterCount ADTypeFilter:(nullable NSData *)ADTypeFilter UUID:(nullable NSData *)UUID timeout:(UInt8)timeout retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)remoteProvisioningLinkGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseRemoteProvisioningLinkStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)remoteProvisioningLinkOpenWithDestination:(UInt16)destination UUID:(nullable NSData *)UUID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseRemoteProvisioningLinkStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)remoteProvisioningLinkCloseWithDestination:(UInt16)destination reason:(SigRemoteProvisioningLinkCloseStatus)reason retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseRemoteProvisioningLinkStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)remoteProvisioningPDUSendWithDestination:(UInt16)destination OutboundPDUNumber:(UInt8)outboundPDUNumber provisioningPDU:(NSData *)provisioningPDU retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount resultCallback:(resultBlock)resultCallback;

#pragma mark - API by Telink

/// Get Online device, private use OnlineStatusCharacteristic(获取当前mesh网络的所有设备的在线、开关、亮度、色温状态(私有定制，需要特定的OnlineStatusCharacteristic))
+ (nullable NSError *)telinkApiGetOnlineStatueFromUUIDWithResponseMaxCount:(int)responseMaxCount successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (void)readOTACharachteristicWithTimeout:(NSTimeInterval)timeout complete:(bleReadOTACharachteristicCallback)complete;

+ (void)cancelReadOTACharachteristic;

+ (nullable NSError *)sendIniCommandModel:(IniCommandModel *)model successCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// iniData like "a3ff000000000200ffffc21102c4020100".
+ (nullable NSError *)sendOpINIData:(NSData *)iniData successCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (void)startMeshSDK;

+ (BOOL)isBLEInitFinish;

+ (void)sendSecureNetworkBeacon;

+ (void)updateIvIndexWithKeyRefreshFlag:(BOOL)keyRefreshFlag ivUpdateActive:(BOOL)ivUpdateActive networkId:(NSData *)networkId ivIndex:(UInt32)ivIndex usingNetworkKey:(SigNetkeyModel *)networkKey;

/// status NowTime, without response
+ (void)statusNowTime;

+ (void)publishNodeTimeModelWithNodeAddress:(UInt16)address successCallback:(responseConfigModelPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
function 1:AUTO if you need do provision , you should call this method, and it'll call back what you need

@param address address of new device
@param networkKey network key, which provsion need, you can see it as password of the mesh
@param netkeyIndex netkey index
@param appkeyModel appkey model
@param unicastAddress address of remote device
@param uuid uuid of remote device
@param type KeyBindTpye_Normal是普通添加模式，KeyBindTpye_Quick是快速添加模式
@param isAuto 添加完成一个设备后，是否自动扫描添加下一个设备
@param provisionSuccess call back when a device provision successful
@param provisionFail call back when a device provision fail
@param keyBindSuccess call back when a device keybind successful
@param keyBindFail call back when a device keybind fail
@param finish finish add the available devices list to the mesh
*/
+ (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel unicastAddress:(UInt16)unicastAddress uuid:(nullable NSData *)uuid keyBindType:(KeyBindTpye)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData isAutoAddNextDevice:(BOOL)isAuto provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail finish:(AddDeviceFinishCallBack)finish;


/**
function 1:special if you need do provision , you should call this method, and it'll call back what you need

@param address address of new device
@param networkKey network key, which provsion need, you can see it as password of the mesh
@param netkeyIndex netkey index
@param peripheral device need add to mesh
@param provisionType ProvisionTpye_NoOOB or ProvisionTpye_StaticOOB.
@param staticOOBData oob for ProvisionTpye_StaticOOB.
@param type KeyBindTpye_Normal是普通添加模式，KeyBindTpye_Quick是快速添加模式
@param provisionSuccess call back when a device provision successful
@param provisionFail call back when a device provision fail
@param keyBindSuccess call back when a device keybind successful
@param keyBindFail call back when a device keybind fail
*/
+ (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel peripheral:(CBPeripheral *)peripheral provisionType:(ProvisionTpye)provisionType staticOOBData:(nullable NSData *)staticOOBData keyBindType:(KeyBindTpye)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail;


/*
 parameter of SigAddConfigModel:
 
    1.normal provision + normal keybind:
peripheral+unicastAddress+networkKey+netkeyIndex+appKey+appkeyIndex+provisionType:ProvisionTpye_NoOOB+keyBindType:KeyBindTpye_Normal
    2.normal provision + fast keybind:
 peripheral+unicastAddress+networkKey+netkeyIndex+appKey+appkeyIndex+provisionType:ProvisionTpye_NoOOB+keyBindType:KeyBindTpye_Fast+productID+cpsData
    3.static oob provision(cloud oob) + normal keybind:
 peripheral+unicastAddress+networkKey+netkeyIndex+appKey+appkeyIndex+provisionType:ProvisionTpye_StaticOOB+staticOOBData+keyBindType:KeyBindTpye_Normal
    4.static oob provision(cloud oob) + fast keybind:
 peripheral+unicastAddress+networkKey+netkeyIndex+appKey+appkeyIndex+provisionType:ProvisionTpye_StaticOOB+staticOOBData+keyBindType:KeyBindTpye_Fast+productID+cpsData
 */
/// Add Single Device (provision+keyBind)
/// @param configModel all config message of add device.
/// @param provisionSuccess callback when provision success.
/// @param provisionFail callback when provision fail.
/// @param keyBindSuccess callback when keybind success.
/// @param keyBindFail callback when keybind fail.
+ (void)startAddDeviceWithSigAddConfigModel:(SigAddConfigModel *)configModel provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail;


/// provision
/// @param peripheral CBPeripheral of CoreBluetooth will be provision.
/// @param unicastAddress address of new device.
/// @param networkKey networkKey
/// @param netkeyIndex netkeyIndex
/// @param provisionType ProvisionTpye_NoOOB means oob data is 16 bytes zero data, ProvisionTpye_StaticOOB means oob data is get from HTTP API.
/// @param staticOOBData oob data get from HTTP API when provisionType is ProvisionTpye_StaticOOB.
/// @param provisionSuccess callback when provision success.
/// @param fail callback when provision fail.
+ (void)startProvisionWithPeripheral:(CBPeripheral *)peripheral unicastAddress:(UInt16)unicastAddress networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex provisionType:(ProvisionTpye)provisionType staticOOBData:(NSData *)staticOOBData provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail;


/// keybind
/// @param peripheral CBPeripheral of CoreBluetooth will be keybind.
/// @param unicastAddress address of new device.
/// @param appkey appkey
/// @param appkeyIndex appkeyIndex
/// @param netkeyIndex netkeyIndex
/// @param keyBindType KeyBindTpye_Normal means add appkey and model bind, KeyBindTpye_Fast means just add appkey.
/// @param productID the productID info need to save in node when keyBindType is KeyBindTpye_Fast.
/// @param cpsData the elements info need to save in node when keyBindType is KeyBindTpye_Fast.
/// @param keyBindSuccess callback when keybind success.
/// @param fail callback when provision fail.
+ (void)startKeyBindWithPeripheral:(CBPeripheral *)peripheral unicastAddress:(UInt16)unicastAddress appKey:(NSData *)appkey appkeyIndex:(UInt16)appkeyIndex netkeyIndex:(UInt16)netkeyIndex keyBindType:(KeyBindTpye)keyBindType productID:(UInt16)productID cpsData:(NSData *)cpsData keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess fail:(ErrorBlock)fail;

/// Do key bound(纯keyBind接口)
+ (void)keyBind:(UInt16)address appkeyModel:(SigAppkeyModel *)appkeyModel keyBindType:(KeyBindTpye)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess fail:(ErrorBlock)fail;

+ (CBCharacteristic *)getCharacteristicWithUUIDString:(NSString *)uuid OfPeripheral:(CBPeripheral *)peripheral;
+ (void)setBluetoothCentralUpdateStateCallback:(bleCentralUpdateStateCallback)bluetoothCentralUpdateStateCallback;

#pragma mark Scan API
+ (void)scanUnprovisionedDevicesWithResult:(bleScanPeripheralCallback)result;
+ (void)scanProvisionedDevicesWithResult:(bleScanPeripheralCallback)result;
+ (void)scanMeshNodeWithPeripheralUUID:(NSString *)peripheralUUID timeout:(NSTimeInterval)timeout resultBlock:(bleScanSpecialPeripheralCallback)block;
+ (void)stopScan;

@end

NS_ASSUME_NONNULL_END
