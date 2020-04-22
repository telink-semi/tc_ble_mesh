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
//  SigMeshLib
//
//  Created by Liangjiazhi on 2019/9/4.
//  Copyright © 2019 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>
//#import "SigAddDeviceManager.h"

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


@interface SDKLibCommand : NSObject
//@property (nonatomic,strong) SigMeshMessage *curMeshMessage;
@property (nonatomic,strong) SigBaseMeshMessage *curMeshMessage;
@property (nonatomic,assign) NSInteger responseMaxCount;
@property (nonatomic,strong) NSMutableArray <NSNumber *>*responseSourceArray;
@property (nonatomic,assign) UInt8 retryCount;//default is 2.
//@property (nonatomic,assign) UInt8 reSendCount;//default is 0.
@property (nonatomic,assign) NSTimeInterval timeout;//default is 1s,kSDKLibCommandTimeout.
@property (nonatomic,assign) UInt8 hadRetryCount;//default is 0.
@property (nonatomic,strong) BackgroundTimer *retryTimer;
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


+ (SigMessageHandle *)configAppKeyAddWithSigAppkeyModel:(SigAppkeyModel *)appkeyModel toAddress:(UInt16)address successCallback:(responseConfigAppKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configAppKeyAddWithSigAppkeyModel:(SigAppkeyModel *)appkeyModel toNode:(SigNodeModel *)node successCallback:(responseConfigAppKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configAppKeyUpdateWithSigAppkeyModel:(SigAppkeyModel *)appkeyModel toAddress:(UInt16)address successCallback:(responseConfigAppKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configAppKeyUpdateWithSigAppkeyModel:(SigAppkeyModel *)appkeyModel toNode:(SigNodeModel *)node successCallback:(responseConfigAppKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configAppKeyDeleteWithSigAppkeyModel:(SigAppkeyModel *)appkeyModel toAddress:(UInt16)address successCallback:(responseConfigAppKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configAppKeyDeleteWithSigAppkeyModel:(SigAppkeyModel *)appkeyModel toNode:(SigNodeModel *)node successCallback:(responseConfigAppKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configAppKeyGetWithNetworkKeyIndex:(UInt16)networkKeyIndex toAddress:(UInt16)address successCallback:(responseConfigAppKeyListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configAppKeyGetWithNetworkKeyIndex:(UInt16)networkKeyIndex toNode:(SigNodeModel *)node successCallback:(responseConfigAppKeyListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configBeaconGetWithAddress:(UInt16)address successCallback:(responseConfigBeaconStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configBeaconGetWithNode:(SigNodeModel *)node successCallback:(responseConfigBeaconStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configBeaconSetWithSigSecureNetworkBeaconState:(SigSecureNetworkBeaconState)secureNetworkBeaconState toAddress:(UInt16)address successCallback:(responseConfigBeaconStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configBeaconSetWithSigSecureNetworkBeaconState:(SigSecureNetworkBeaconState)secureNetworkBeaconState toNode:(SigNodeModel *)node successCallback:(responseConfigBeaconStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configCompositionDataGetWithAddress:(UInt16)address successCallback:(responseConfigCompositionDataStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configCompositionDataGetWithNode:(SigNodeModel *)node successCallback:(responseConfigCompositionDataStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configDefaultTtlGetWithAddress:(UInt16)address successCallback:(responseConfigDefaultTtlStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configDefaultTtlGetWithNode:(SigNodeModel *)node successCallback:(responseConfigDefaultTtlStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configDefaultTtlSetWithTtl:(UInt8)ttl toAddress:(UInt16)address successCallback:(responseConfigDefaultTtlStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configDefaultTtlSetWithTtl:(UInt8)ttl toNode:(SigNodeModel *)node successCallback:(responseConfigDefaultTtlStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configFriendGetWithAddress:(UInt16)address successCallback:(responseConfigFriendStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configFriendGetWithNode:(SigNodeModel *)node successCallback:(responseConfigFriendStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configFriendSetWithSigNodeFeaturesState:(SigNodeFeaturesState)nodeFeaturesState toAddress:(UInt16)address successCallback:(responseConfigFriendStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configFriendSetWithSigNodeFeaturesState:(SigNodeFeaturesState)nodeFeaturesState toNode:(SigNodeModel *)node successCallback:(responseConfigFriendStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configGATTProxyGetWithAddress:(UInt16)address successCallback:(responseConfigGATTProxyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configGATTProxyGetWithNode:(SigNodeModel *)node successCallback:(responseConfigGATTProxyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configGATTProxySetWithSigNodeGATTProxyState:(SigNodeGATTProxyState)nodeGATTProxyState toAddress:(UInt16)address successCallback:(responseConfigGATTProxyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configGATTProxySetWithSigNodeGATTProxyState:(SigNodeGATTProxyState)nodeGATTProxyState toNode:(SigNodeModel *)node successCallback:(responseConfigGATTProxyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configModelPublicationGetWithModelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigModelPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configModelPublicationSetWithSigPublish:(SigPublish *)publish modelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigModelPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configModelPublicationSetDisableWithModelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigModelPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configModelPublicationVirtualAddressSetWithSigPublish:(SigPublish *)publish modelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigModelPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configModelSubscriptionAddWithGroupAddress:(UInt16)groupAddress toNodeAddress:(UInt16)nodeAddress elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configModelSubscriptionAddWithSigGroupModel:(SigGroupModel *)groupModel modelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configModelSubscriptionDeleteWithSigGroupModel:(SigGroupModel *)groupModel modelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configModelSubscriptionDeleteAllWithSigModelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configModelSubscriptionOverwriteWithSigGroupModel:(SigGroupModel *)groupModel modelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configModelSubscriptionVirtualAddressAddWithSigGroupModel:(SigGroupModel *)groupModel modelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configModelSubscriptionVirtualAddressDeleteWithSigGroupModel:(SigGroupModel *)groupModel modelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configModelSubscriptionVirtualAddressOverwriteWithSigGroupModel:(SigGroupModel *)groupModel modelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configNetworkTransmitGetWithAddress:(UInt16)address successCallback:(responseConfigNetworkTransmitStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configNetworkTransmitGetWithNode:(SigNodeModel *)node successCallback:(responseConfigNetworkTransmitStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configNetworkTransmitSetWithAddress:(UInt16)address networkTransmitCount:(UInt8)networkTransmitCount networkTransmitIntervalSteps:(UInt8)networkTransmitIntervalSteps successCallback:(responseConfigNetworkTransmitStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configNetworkTransmitSetWithNode:(SigNodeModel *)node networkTransmitCount:(UInt8)networkTransmitCount networkTransmitIntervalSteps:(UInt8)networkTransmitIntervalSteps successCallback:(responseConfigNetworkTransmitStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configRelayGetWithAddress:(UInt16)address successCallback:(responseConfigRelayStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configRelayGetWithNode:(SigNodeModel *)node successCallback:(responseConfigRelayStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configRelaySetWithAddress:(UInt16)address relay:(SigNodeRelayState)relay networkTransmitCount:(UInt8)networkTransmitCount networkTransmitIntervalSteps:(UInt8)networkTransmitIntervalSteps successCallback:(responseConfigRelayStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configRelaySetWithNode:(SigNodeModel *)node relay:(SigNodeRelayState)relay networkTransmitCount:(UInt8)networkTransmitCount networkTransmitIntervalSteps:(UInt8)networkTransmitIntervalSteps successCallback:(responseConfigRelayStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configSIGModelSubscriptionGetWithModelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigSIGModelSubscriptionListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configVendorModelSubscriptionGetWithModelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigVendorModelSubscriptionListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configModelAppBindWithApplicationKeyIndex:(UInt16)applicationKeyIndex elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier toDestination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseConfigModelAppStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configModelAppBindWithSigAppkeyModel:(SigAppkeyModel *)appkeyModel toModelIDModel:(SigModelIDModel *)modelIDModel toNode:(SigNodeModel *)node successCallback:(responseConfigModelAppStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configModelAppUnbindWithSigAppkeyModel:(SigAppkeyModel *)appkeyModel toModelIDModel:(SigModelIDModel *)modelIDModel toNode:(SigNodeModel *)node successCallback:(responseConfigModelAppStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configNetKeyAddWithAddress:(UInt16)address networkKey:(SigNetkeyModel *)networkKey successCallback:(responseConfigNetKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configNetKeyAddWithNode:(SigNodeModel *)node networkKey:(SigNetkeyModel *)networkKey successCallback:(responseConfigNetKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configNetKeyDeleteWithAddress:(UInt16)address networkKey:(SigNetkeyModel *)networkKey successCallback:(responseConfigNetKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configNetKeyDeleteWithNode:(SigNodeModel *)node networkKey:(SigNetkeyModel *)networkKey successCallback:(responseConfigNetKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configNetKeyGetWithAddress:(UInt16)address networkKey:(SigNetkeyModel *)networkKey successCallback:(responseConfigNetKeyListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configNetKeyGetWithNode:(SigNodeModel *)node networkKey:(SigNetkeyModel *)networkKey successCallback:(responseConfigNetKeyListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configNetKeyUpdateWithNode:(SigNodeModel *)node networkKey:(SigNetkeyModel *)networkKey successCallback:(responseConfigNetKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configNodeIdentityGetWithDestination:(UInt16)destination netKeyIndex:(UInt16)netKeyIndex resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseConfigNodeIdentityStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configNodeIdentitySetWithDestination:(UInt16)destination netKeyIndex:(UInt16)netKeyIndex identity:(SigNodeIdentityState)identity resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseConfigNodeIdentityStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)resetNodeWithNodeAddress:(UInt16)nodeAddress successCallback:(responseConfigNodeResetStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)resetNode:(SigNodeModel *)node successCallback:(responseConfigNodeResetStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)configSIGModelAppGetWithModelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigSIGModelAppListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)configVendorModelAppGetWithModelIDModel:(SigModelIDModel *)modelIDModel successCallback:(responseConfigVendorModelAppListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;




    //Generic OnOff
+ (SigMessageHandle *)genericOnOffGetWithDestination:(UInt16)destination resMax:(int)resMax successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericOnOffGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericOnOffSetWithIsOn:(BOOL)isOn toDestination:(UInt16)destination resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericOnOffSet:(BOOL)isOn withNode:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericOnOffSet:(BOOL)isOn withNode:(SigNodeModel *)node transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

    //Generic Level
+ (SigMessageHandle *)genericLevelSet:(UInt16)level toDestination:(UInt16)destination transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericLevelGetWithDestination:(UInt16)destination resMax:(int)resMax successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericLevelGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericLevelSet:(UInt16)level withNode:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericLevelSet:(UInt16)level withNode:(SigNodeModel *)node transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericDeltaSet:(UInt32)delta withNode:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericDeltaSet:(UInt32)delta withNode:(SigNodeModel *)node transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericDeltaSet:(UInt32)delta toDestination:(UInt16)destination transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericMoveSet:(UInt16)deltaLevel withNode:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericMoveSet:(UInt16)deltaLevel withNode:(SigNodeModel *)node transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Generic Default Transition Time
+ (SigMessageHandle *)genericDefaultTransitionTimeGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseGenericDefaultTransitionTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericDefaultTransitionTimeSet:(nonnull SigTransitionTime *)transitionTime withNode:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericDefaultTransitionTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Generic Power OnOff
+ (SigMessageHandle *)genericOnPowerUpGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseGenericOnPowerUpStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericOnPowerUpSet:(SigOnPowerUp)onPowerUp withNode:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericOnPowerUpStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Generic Power Level
+ (SigMessageHandle *)genericPowerLevelGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseGenericPowerLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericPowerLevelSet:(UInt16)power withNode:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericPowerLevelSet:(UInt16)power withNode:(SigNodeModel *)node transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericPowerLastGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseGenericPowerLastStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericPowerDefaultGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseGenericPowerDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericPowerRangeGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseGenericPowerRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Generic Power Level Setup
+ (SigMessageHandle *)genericPowerDefaultSet:(UInt16)power withNode:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericPowerDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)genericPowerRangeSetWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseGenericPowerRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Generic Battery
+ (SigMessageHandle *)genericBatteryGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseGenericBatteryStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Sensor
+ (SigMessageHandle *)sensorDescriptorGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseSensorDescriptorStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sensorDescriptorGetWithPropertyID:(UInt16)propertyID node:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseSensorDescriptorStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sensorGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseSensorStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sensorGetWithPropertyID:(UInt16)propertyID node:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseSensorStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sensorColumnGetWithPropertyID:(UInt16)propertyID rawValueX:(NSData *)rawValueX node:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseSensorColumnStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sensorSeriesGetWithPropertyID:(UInt16)propertyID rawValueX1Data:(NSData *)rawValueX1Data rawValueX2Data:(NSData *)rawValueX2Data node:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseSensorSeriesStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Sensor Setup
+ (SigMessageHandle *)sensorCadenceGetWithPropertyID:(UInt16)propertyID node:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseSensorCadenceStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sensorCadenceSetWithPropertyID:(UInt16)propertyID cadenceData:(NSData *)cadenceData node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseSensorCadenceStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sensorSettingsGetWithPropertyID:(UInt16)propertyID node:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseSensorSettingsStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sensorSettingGetWithPropertyID:(UInt16)propertyID settingPropertyID:(UInt16)settingPpropertyID node:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseSensorSettingStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Time
+ (SigMessageHandle *)timeGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)timeSetWithSigTimeModel:(SigTimeModel *)timeModel toDestination:(UInt16)destination resMax:(int)resMax successCallback:(responseTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)timeSetWithSigTimeModel:(SigTimeModel *)timeModel node:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)timeStatusWithSigTimeModel:(SigTimeModel *)timeModel toDestination:(UInt16)destination resMax:(int)resMax successCallback:(_Nullable responseTimeStatusMessageBlock)successCallback resultCallback:(_Nullable resultBlock)resultCallback;
+ (SigMessageHandle *)timeStatusWithSigTimeModel:(SigTimeModel *)timeModel node:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)timeRoleGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseTimeRoleStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)timeRoleSetWithNode:(SigNodeModel *)node timeRole:(SigTimeRole)timeRole resMax:(int)resMax successCallback:(responseTimeRoleStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)timeZoneGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseTimeZoneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)timeZoneSetWithTimeZoneOffsetNew:(UInt8)timeZoneOffsetNew TAIOfZoneChange:(UInt64)TAIOfZoneChange node:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseTimeZoneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)TAI_UTC_DeltaGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseTAI_UTC_DeltaStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)TAI_UTC_DeltaSetWithTAI_UTC_DeltaNew:(UInt16)TAI_UTC_DeltaNew padding:(UInt8)padding TAIOfDeltaChange:(UInt64)TAIOfDeltaChange node:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseTAI_UTC_DeltaStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Scene
+ (SigMessageHandle *)sceneGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseSceneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sceneRecallWithSceneNumber:(UInt16)sceneNumber node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseSceneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sceneRecallWithSceneNumber:(UInt16)sceneNumber transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay toDestination:(UInt16)destination resMax:(int)resMax ack:(BOOL)ack successCallback:(responseSceneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sceneRecallWithSceneNumber:(UInt16)sceneNumber transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseSceneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sceneRegisterGetWithDestination:(UInt16)destination resMax:(int)resMax successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sceneRegisterGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Scene Setup
+ (SigMessageHandle *)sceneStoreWithSceneNumber:(UInt16)sceneNumber toDestination:(UInt16)destination resMax:(int)resMax ack:(BOOL)ack successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sceneStoreWithSceneNumber:(UInt16)sceneNumber node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sceneDeleteWithSceneNumber:(UInt16)sceneNumber toDestination:(UInt16)destination resMax:(int)resMax ack:(BOOL)ack successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)sceneDeleteWithSceneNumber:(UInt16)sceneNumber node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Scheduler
+ (SigMessageHandle *)schedulerActionGetWithSchedulerIndex:(UInt8)schedulerIndex toDestination:(UInt16)destination resMax:(int)resMax successCallback:(responseSchedulerActionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)schedulerActionGetWithSchedulerIndex:(UInt8)schedulerIndex node:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseSchedulerActionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)schedulerActionGetWithDestination:(UInt16)destination resMax:(int)resMax successCallback:(responseSchedulerStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)schedulerActionGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseSchedulerStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Scheduler Setup
+ (SigMessageHandle *)schedulerActionSetWithSchedulerModel:(SchedulerModel *)schedulerModel toDestination:(UInt16)destination resMax:(int)resMax ack:(BOOL)ack successCallback:(responseSchedulerActionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)schedulerActionSetWithSchedulerModel:(SchedulerModel *)schedulerModel node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseSchedulerActionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Light Lightness
+ (SigMessageHandle *)lightLightnessGetWithDestination:(UInt16)destination resMax:(int)resMax successCallback:(responseLightLightnessStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLightnessGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightLightnessStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLightnessSetWithLightness:(UInt16)lightness toDestination:(UInt16)destination resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightLightnessStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLightnessSetWithLightness:(UInt16)lightness node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightLightnessStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLightnessSetWithLightness:(UInt16)lightness transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightLightnessStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLightnessLinearGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightLightnessLinearStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLightnessLinearSetWithLightness:(UInt16)lightness node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightLightnessLinearStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLightnessLinearSetWithLightness:(UInt16)lightness transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightLightnessLinearStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLightnessLastGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightLightnessLastStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLightnessDefaultGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightLightnessDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLightnessRangeGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightLightnessRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Light Lightness Setup
+ (SigMessageHandle *)lightLightnessDefaultSetWithLightness:(UInt16)lightness node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightLightnessDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLightnessRangeSetWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightLightnessRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Light CTL
+ (SigMessageHandle *)lightCTLGetWithDestination:(UInt16)destination resMax:(int)resMax successCallback:(responseLightCTLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightCTLGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightCTLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightCTLSetWithLightness:(UInt16)lightness temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightCTLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightCTLSetWithLightness:(UInt16)lightness temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightCTLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightCTLTemperatureGetWithDestination:(UInt16)destination resMax:(int)resMax successCallback:(responseLightCTLTemperatureStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightCTLTemperatureGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightCTLTemperatureStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightCTLTemperatureRangeGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightCTLTemperatureRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightCTLTemperatureSetWithTemperature:(UInt16)temperature deltaUV:(UInt16)deltaUV node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightCTLTemperatureStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightCTLTemperatureSetWithTemperature:(UInt16)temperature deltaUV:(UInt16)deltaUV transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightCTLTemperatureStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightCTLTemperatureSetWithTemperature:(UInt16)temperature deltaUV:(UInt16)deltaUV transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay toDestination:(UInt16)destination resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightCTLTemperatureStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightCTLDefaultGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightCTLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Light CTL Setup
+ (SigMessageHandle *)lightCTLDefaultSetWithLightness:(UInt16)lightness temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightCTLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightCTLTemperatureRangeSetWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightCTLTemperatureRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Light HSL
+ (SigMessageHandle *)lightHSLGetWithDestination:(UInt16)destination resMax:(int)resMax successCallback:(responseLightHSLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightHSLGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightHSLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightHSLHueGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightHSLHueStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightHSLHueSetWithHue:(UInt16)hue node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightHSLHueStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightHSLHueSetWithHue:(UInt16)hue transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightHSLHueStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightHSLSaturationGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightHSLSaturationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightHSLSaturationSetWithHue:(UInt16)saturation node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightHSLSaturationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightHSLSaturationSetWithHue:(UInt16)saturation transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightHSLSaturationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightHSLSetWithHSLLight:(UInt16)HSLLight HSLHue:(UInt16)HSLHue HSLSaturation:(UInt16)HSLSaturation node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightHSLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightHSLSetWithHSLLight:(UInt16)HSLLight HSLHue:(UInt16)HSLHue HSLSaturation:(UInt16)HSLSaturation transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay toDestination:(UInt16)destination resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightHSLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightHSLSetWithHSLLight:(UInt16)HSLLight HSLHue:(UInt16)HSLHue HSLSaturation:(UInt16)HSLSaturation transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightHSLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightHSLTargetGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightHSLTargetStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightHSLDefaultGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightHSLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightHSLRangeGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightHSLRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Light HSL Setup
+ (SigMessageHandle *)lightHSLDefaultSetWithLight:(UInt16)light hue:(UInt16)hue saturation:(UInt16)saturation node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightHSLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightHSLRangeSetWithHueRangeMin:(UInt16)hueRangeMin hueRangeMax:(UInt16)hueRangeMax saturationRangeMin:(UInt16)saturationRangeMin saturationRangeMax:(UInt16)saturationRangeMax node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightHSLRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Light xyL
+ (SigMessageHandle *)lightXyLGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightXyLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightXyLSetWithXyLLightness:(UInt16)xyLLightness xyLx:(UInt16)xyLx xyLy:(UInt16)xyLy node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightXyLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightXyLSetWithXyLLightness:(UInt16)xyLLightness xyLx:(UInt16)xyLx xyLy:(UInt16)xyLy transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightXyLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightXyLTargetGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightXyLTargetStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightXyLDefaultGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightXyLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightXyLRangeGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightXyLRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Light xyL Setup
+ (SigMessageHandle *)lightXyLDefaultSetWithLightness:(UInt16)lightness xyLx:(UInt16)xyLx xyLy:(UInt16)xyLy node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightXyLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightXyLRangeSetWithXyLxRangeMin:(UInt16)xyLxRangeMin xyLxRangeMax:(UInt16)xyLxRangeMax xyLyRangeMin:(UInt16)xyLyRangeMin xyLyRangeMax:(UInt16)xyLyRangeMax node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightXyLRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Light Control
+ (SigMessageHandle *)lightLCModeGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightLCModeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLCModeSetWithModeEnable:(BOOL)enable node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightLCModeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLCOMGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightLCOMStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLCOMSetWithModeEnable:(BOOL)enable node:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightLCOMStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLCLightOnOffGetWithNode:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightLCLightOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLCLightOnOffSet:(BOOL)isOn withNode:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightLCLightOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLCLightOnOffSet:(BOOL)isOn withNode:(SigNodeModel *)node transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightLCLightOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//Light Control Setup
+ (SigMessageHandle *)lightLCPropertyGetWithPropertyID:(UInt16)propertyID node:(SigNodeModel *)node resMax:(int)resMax successCallback:(responseLightLCPropertyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (SigMessageHandle *)lightLCPropertySetWithPropertyID:(UInt16)propertyID propertyValue:(NSData *)propertyValue withNode:(SigNodeModel *)node resMax:(int)resMax ack:(BOOL)ack successCallback:(responseLightLCPropertyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

#pragma mark - Filter

+ (void)setType:(SigProxyFilerType)type successCallback:(responseFilterStatusMessageBlock)successCallback failCallback:(resultBlock)failCallback;
/// Resets the filter to an empty whitelist filter.
+ (void)resetFilterWithSuccessCallback:(responseFilterStatusMessageBlock)successCallback failCallback:(resultBlock)failCallback;
/// Adds the given Address to the active filter.
///
/// - parameter address: The address to add to the filter.
+ (void)addAddressToFilterWithAddress:(UInt16)address successCallback:(responseFilterStatusMessageBlock)successCallback failCallback:(resultBlock)failCallback;
/// Adds the given Addresses to the active filter.
///
/// - parameter addresses: The addresses to add to the filter.
+ (void)addAddressesToFilterWithAddresses:(NSArray <NSNumber *>*)addresses successCallback:(responseFilterStatusMessageBlock)successCallback failCallback:(resultBlock)failCallback;
/// Removes the given Address from the active filter.
///
/// - parameter address: The address to remove from the filter.
+ (void)removeAddressFromFilterWithAddress:(NSNumber *)address successCallback:(responseFilterStatusMessageBlock)successCallback failCallback:(resultBlock)failCallback;
/// Removes the given Addresses from the active filter.
///
/// - parameter addresses: The addresses to remove from the filter.
+ (void)removeAddressesFromFilterWithAddresses:(NSArray <NSNumber *>*)addresses successCallback:(responseFilterStatusMessageBlock)successCallback failCallback:(resultBlock)failCallback;
/// Adds all the addresses the Provisioner is subscribed to to the
/// Proxy Filter.
+ (void)setFilterForProvisioner:(SigProvisionerModel *)provisioner successCallback:(responseFilterStatusMessageBlock)successCallback finishCallback:(resultBlock)failCallback;

#pragma mark - Mesh Firmware update (Mesh OTA)

/* Firmware Update Messages */

+ (SigMessageHandle *)firmwareUpdateInformationGetWithDestination:(UInt16)destination firstIndex:(UInt8)firstIndex entriesLimit:(UInt8)entriesLimit resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseFirmwareInformationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)firmwareUpdateFirmwareMetadataCheckWithDestination:(UInt16)destination updateFirmwareImageIndex:(UInt8)updateFirmwareImageIndex incomingFirmwareMetadata:(nullable NSData *)incomingFirmwareMetadata resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseFirmwareUpdateFirmwareMetadataStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)firmwareUpdateGetWithDestination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseFirmwareUpdateStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)firmwareUpdateStartWithDestination:(UInt16)destination updateTTL:(UInt8)updateTTL updateTimeoutBase:(UInt16)updateTimeoutBase updateBLOBID:(UInt64)updateBLOBID updateFirmwareImageIndex:(UInt8)updateFirmwareImageIndex incomingFirmwareMetadata:(nullable NSData *)incomingFirmwareMetadata resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseFirmwareUpdateStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)firmwareUpdateCancelWithDestination:(UInt16)destination companyID:(UInt16)companyID firmwareID:(NSData *)firmwareID resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseFirmwareUpdateStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)firmwareUpdateApplyWithDestination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseFirmwareUpdateStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)firmwareDistributionGetWithDestination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseFirmwareDistributionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)firmwareDistributionStartWithDestination:(UInt16)destination distributionAppKeyIndex:(UInt16)distributionAppKeyIndex distributionTTL:(UInt8)distributionTTL distributionTimeoutBase:(UInt16)distributionTimeoutBase distributionTransferMode:(SigTransferModeState)distributionTransferMode updatePolicy:(BOOL)updatePolicy RFU:(UInt8)RFU distributionFirmwareImageIndex:(UInt16)distributionFirmwareImageIndex distributionMulticastAddress:(NSData *)distributionMulticastAddress resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseFirmwareDistributionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)firmwareDistributionCancelWithDestination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseFirmwareDistributionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

//+ (SigMessageHandle *)firmwareDistributionDetailsGetWithDestination:(UInt16)destination status:(SigFirmwareDistributionStatusType)status companyID:(UInt16)companyID firmwareID:(NSData *)firmwareID resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseFirmwareDistributionDetailsListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/* BLOB Transfer Messages */

+ (SigMessageHandle *)BLOBTransferGetWithDestination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseBLOBTransferStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)BLOBTransferStartWithDestination:(UInt16)destination transferMode:(SigTransferModeState)transferMode BLOBID:(UInt64)BLOBID BLOBSize:(UInt32)BLOBSize BLOBBlockSizeLog:(UInt8)BLOBBlockSizeLog MTUSize:(UInt16)MTUSize resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseBLOBTransferStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)BLOBTransferAbortWithDestination:(UInt16)destination BLOBID:(UInt64)BLOBID resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseBLOBTransferStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


/// BLOBBlockStart
/// @param destination node address
/// @param blockNumber block index, 0. 1. 2. 3. ··· n.
/// @param chunkSize size of every chunk, 0~256.
/// @param resMax max response count
/// @param retryCount retry count
/// @param successCallback success Callback
/// @param resultCallback end Callback of fail callback
+ (SigMessageHandle *)BLOBBlockStartWithBlockNumber:(UInt16)blockNumber chunkSize:(UInt16)chunkSize toDestination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseBLOBBlockStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)BLOBChunkTransferWithDestination:(UInt16)destination chunkNumber:(UInt16)chunkNumber chunkData:(NSData *)chunkData resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)BLOBBlockGetWithDestination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseBLOBBlockStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)BLOBInformationGetWithDestination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseBLOBInformationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark - Remote Provision

+ (SigMessageHandle *)remoteProvisioningScanCapabilitiesGetWithDestination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseRemoteProvisioningScanCapabilitiesStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)remoteProvisioningScanGetWithDestination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseRemoteProvisioningScanStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)remoteProvisioningScanStartWithScannedItemsLimit:(UInt8)scannedItemsLimit timeout:(UInt8)timeout UUID:(nullable NSData *)UUID destination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseRemoteProvisioningScanStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)remoteProvisioningScanStopWithDestination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseRemoteProvisioningScanStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)remoteProvisioningExtendedScanStartWithADTypeFilterCount:(UInt8)ADTypeFilterCount ADTypeFilter:(nullable NSData *)ADTypeFilter UUID:(nullable NSData *)UUID timeout:(UInt8)timeout destination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)remoteProvisioningLinkGetWithDestination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseRemoteProvisioningLinkStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)remoteProvisioningLinkOpenWithUUID:(nullable NSData *)UUID destination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseRemoteProvisioningLinkStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)remoteProvisioningLinkCloseWithReason:(SigRemoteProvisioningLinkCloseStatus)reason destination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount successCallback:(responseRemoteProvisioningLinkStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (SigMessageHandle *)remoteProvisioningPDUSendWithOutboundPDUNumber:(UInt8)outboundPDUNumber provisioningPDU:(NSData *)provisioningPDU destination:(UInt16)destination resMax:(NSInteger)resMax retryCount:(NSInteger)retryCount resultCallback:(resultBlock)resultCallback;

#pragma mark - API by Telink

+ (nullable NSError *)telinkApiGetOnlineStatueFromUUIDWithResponseMaxCount:(int)responseMaxCount successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (void)readOTACharachteristicWithTimeout:(NSTimeInterval)timeout complete:(bleReadOTACharachteristicCallback)complete;

+ (void)cancelReadOTACharachteristic;

+ (nullable NSError *)sendIniCommandModel:(IniCommandModel *)model successCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (nullable NSError *)sendOpINIData:(NSData *)iniData successCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (void)sendOpByINI:(UInt8 *)iniBuf length:(UInt32)length successCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

+ (void)startMeshSDK;

+ (BOOL)isBLEInitFinish;

+ (void)sendSecureNetworkBeacon;

/// status NowTime, without response
+ (void)statusNowTime;

+ (void)publishNodeTimeModelWithNodeAddress:(UInt16)address successCallback:(responseConfigModelPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

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
- (void)startAddDeviceWithSigAddConfigModel:(SigAddConfigModel *)configModel provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail;


/// provision
/// @param peripheral CBPeripheral of CoreBluetooth will be provision.
/// @param unicastAddress address of new device.
/// @param networkKey networkKey
/// @param netkeyIndex netkeyIndex
/// @param provisionType ProvisionTpye_NoOOB means oob data is 16 bytes zero data, ProvisionTpye_StaticOOB means oob data is get from HTTP API.
/// @param staticOOBData oob data get from HTTP API when provisionType is ProvisionTpye_StaticOOB.
/// @param provisionSuccess callback when provision success.
/// @param fail callback when provision fail.
- (void)startProvisionWithPeripheral:(CBPeripheral *)peripheral unicastAddress:(UInt16)unicastAddress networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex provisionType:(ProvisionTpye)provisionType staticOOBData:(NSData *)staticOOBData provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail;


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
- (void)startKeyBindWithPeripheral:(CBPeripheral *)peripheral unicastAddress:(UInt16)unicastAddress appKey:(NSData *)appkey appkeyIndex:(UInt16)appkeyIndex netkeyIndex:(UInt16)netkeyIndex keyBindType:(KeyBindTpye)keyBindType productID:(UInt16)productID cpsData:(NSData *)cpsData keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess fail:(ErrorBlock)fail;


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

@end

NS_ASSUME_NONNULL_END
