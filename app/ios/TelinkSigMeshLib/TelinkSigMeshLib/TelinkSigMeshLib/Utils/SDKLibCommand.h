/********************************************************************************************************
 * @file     SDKLibCommand.h
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/9/4
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

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class SigMessageHandle,SigTransitionTime,SigMeshMessage,SigBaseMeshMessage,SigPublish,SigTimeModel,SigFilterStatus,SigProvisioningCapabilitiesPdu;
@class SigConfigAppKeyStatus,SigConfigAppKeyList,SigConfigBeaconStatus,SigConfigCompositionDataStatus,SigConfigDefaultTtlStatus,SigConfigFriendStatus,SigConfigGATTProxyStatus,SigConfigKeyRefreshPhaseStatus,SigConfigModelPublicationStatus,SigConfigModelSubscriptionStatus,SigConfigNetworkTransmitStatus,SigConfigRelayStatus,SigConfigSIGModelSubscriptionList,SigConfigVendorModelSubscriptionList,SigConfigLowPowerNodePollTimeoutStatus,SigConfigHeartbeatPublicationStatus,SigConfigHeartbeatSubscriptionStatus,SigConfigModelAppStatus,SigConfigNetKeyStatus,SigConfigNetKeyList,SigConfigNodeIdentityStatus,SigConfigNodeResetStatus,SigConfigSIGModelAppList,SigConfigVendorModelAppList;
@class SigGenericOnOffStatus,SigGenericLevelStatus,SigGenericDefaultTransitionTimeStatus,SigGenericOnPowerUpStatus,SigGenericPowerLevelStatus,SigGenericPowerLastStatus,SigGenericPowerDefaultStatus,SigGenericPowerRangeStatus,SigGenericBatteryStatus,SigSensorDescriptorStatus,SigSensorStatus,SigSensorColumnStatus,SigSensorSeriesStatus,SigSensorCadenceStatus,SigSensorSettingsStatus,SigSensorSettingStatus,SigTimeStatus,SigTimeRoleStatus,SigTimeZoneStatus,SigTAI_UTC_DeltaStatus,SigSceneStatus,SigSceneRegisterStatus,SigSchedulerActionStatus,SigSchedulerStatus,SigLightLightnessStatus,SigLightLightnessLinearStatus,SigLightLightnessLastStatus,SigLightLightnessDefaultStatus,SigLightLightnessRangeStatus,SigLightCTLStatus,SigLightCTLTemperatureRangeStatus,SigLightCTLTemperatureStatus,SigLightCTLDefaultStatus,SigLightHSLHueStatus,SigLightHSLSaturationStatus,SigLightHSLStatus,SigLightHSLTargetStatus,SigLightHSLDefaultStatus,SigLightHSLRangeStatus,SigLightXyLStatus,SigLightXyLTargetStatus,SigLightXyLDefaultStatus,SigLightXyLRangeStatus,SigLightLCModeStatus,SigLightLCOMStatus,SigLightLCLightOnOffStatus,SigLightLCPropertyStatus;
@class SigFirmwareUpdateInformationStatus,SigFirmwareUpdateFirmwareMetadataStatus,SigFirmwareUpdateStatus,SigFirmwareDistributionStatus,SigFirmwareDistributionReceiversList,SigFirmwareDistributionReceiversStatus,SigFirmwareDistributionReceiversList,SigFirmwareDistributionCapabilitiesStatus,SigFirmwareDistributionUploadStatus,SigFirmwareDistributionFirmwareStatus;
@class SigBLOBTransferStatus,SigBLOBBlockStatus,SigBLOBInformationStatus;
@class SigRemoteProvisioningScanCapabilitiesStatus,SigRemoteProvisioningScanStatus,SigRemoteProvisioningLinkStatus;
@class SigSubnetBridgeStatus,SigBridgeTableStatus,SigBridgeSubnetsList,SigBridgeTableList,SigBridgingTableSizeStatus;
@class SigOpcodesAggregatorSequence,SigOpcodesAggregatorStatus;
@class SigPrivateBeaconStatus, SigPrivateGattProxyStatus, SigPrivateNodeIdentityStatus;

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
typedef void(^responseConfigKeyRefreshPhaseStatusMessageBlock)(UInt16 source,UInt16 destination,SigConfigKeyRefreshPhaseStatus *responseMessage);
typedef void(^responseConfigModelPublicationStatusMessageBlock)(UInt16 source,UInt16 destination,SigConfigModelPublicationStatus *responseMessage);
typedef void(^responseConfigModelSubscriptionStatusMessageBlock)(UInt16 source,UInt16 destination,SigConfigModelSubscriptionStatus *responseMessage);
typedef void(^responseConfigNetworkTransmitStatusMessageBlock)(UInt16 source,UInt16 destination,SigConfigNetworkTransmitStatus *responseMessage);
typedef void(^responseConfigRelayStatusMessageBlock)(UInt16 source,UInt16 destination,SigConfigRelayStatus *responseMessage);
typedef void(^responseConfigSIGModelSubscriptionListMessageBlock)(UInt16 source,UInt16 destination,SigConfigSIGModelSubscriptionList *responseMessage);
typedef void(^responseConfigVendorModelSubscriptionListMessageBlock)(UInt16 source,UInt16 destination,SigConfigVendorModelSubscriptionList *responseMessage);
typedef void(^responseConfigLowPowerNodePollTimeoutStatusMessageBlock)(UInt16 source,UInt16 destination,SigConfigLowPowerNodePollTimeoutStatus *responseMessage);
typedef void(^responseConfigHeartbeatPublicationStatusMessageBlock)(UInt16 source,UInt16 destination,SigConfigHeartbeatPublicationStatus *responseMessage);
typedef void(^responseConfigHeartbeatSubscriptionStatusMessageBlock)(UInt16 source,UInt16 destination,SigConfigHeartbeatSubscriptionStatus *responseMessage);
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
typedef void(^responseFirmwareDistributionReceiversStatusMessageBlock)(UInt16 source,UInt16 destination,SigFirmwareDistributionReceiversStatus *responseMessage);
typedef void(^responseFirmwareDistributionReceiversListMessageBlock)(UInt16 source,UInt16 destination,SigFirmwareDistributionReceiversList *responseMessage);
typedef void(^responseFirmwareDistributionCapabilitiesStatusMessageBlock)(UInt16 source,UInt16 destination,SigFirmwareDistributionCapabilitiesStatus *responseMessage);
typedef void(^responseFirmwareDistributionUploadStatusMessageBlock)(UInt16 source,UInt16 destination,SigFirmwareDistributionUploadStatus *responseMessage);
typedef void(^responseFirmwareDistributionFirmwareStatusMessageBlock)(UInt16 source,UInt16 destination,SigFirmwareDistributionFirmwareStatus *responseMessage);

// callback about BLOB Transfer Messages
typedef void(^responseBLOBTransferStatusMessageBlock)(UInt16 source,UInt16 destination,SigBLOBTransferStatus *responseMessage);
typedef void(^responseBLOBBlockStatusMessageBlock)(UInt16 source,UInt16 destination,SigBLOBBlockStatus *responseMessage);
typedef void(^responseBLOBInformationStatusMessageBlock)(UInt16 source,UInt16 destination,SigBLOBInformationStatus *responseMessage);
typedef void(^responseSubnetBridgeStatusMessageBlock)(UInt16 source,UInt16 destination,SigSubnetBridgeStatus *responseMessage);
typedef void(^responseBridgeTableStatusMessageBlock)(UInt16 source,UInt16 destination,SigBridgeTableStatus *responseMessage);
typedef void(^responseBridgeSubnetsListMessageBlock)(UInt16 source,UInt16 destination,SigBridgeSubnetsList *responseMessage);
typedef void(^responseBridgeTableListMessageBlock)(UInt16 source,UInt16 destination,SigBridgeTableList *responseMessage);
typedef void(^responseBridgingTableSizeStatusMessageBlock)(UInt16 source,UInt16 destination,SigBridgingTableSizeStatus *responseMessage);

// callback about Remote Provision
typedef void(^responseRemoteProvisioningScanCapabilitiesStatusMessageBlock)(UInt16 source,UInt16 destination,SigRemoteProvisioningScanCapabilitiesStatus *responseMessage);
typedef void(^responseRemoteProvisioningScanStatusMessageBlock)(UInt16 source,UInt16 destination,SigRemoteProvisioningScanStatus *responseMessage);
typedef void(^responseRemoteProvisioningLinkStatusMessageBlock)(UInt16 source,UInt16 destination,SigRemoteProvisioningLinkStatus *responseMessage);

// callback about Opcodes Aggregator Sequence Message
typedef void(^responseOpcodesAggregatorStatusMessageBlock)(UInt16 source,UInt16 destination,SigOpcodesAggregatorStatus *responseMessage);

// callback about Private Beacon Message
typedef void(^responsePrivateBeaconStatusMessageBlock)(UInt16 source,UInt16 destination,SigPrivateBeaconStatus *responseMessage);
typedef void(^responsePrivateGattProxyStatusMessageBlock)(UInt16 source,UInt16 destination,SigPrivateGattProxyStatus *responseMessage);
typedef void(^responsePrivateNodeIdentityStatusMessageBlock)(UInt16 source,UInt16 destination,SigPrivateNodeIdentityStatus *responseMessage);

typedef enum : UInt8 {
    SigCommandType_meshMessage,
    SigCommandType_configMessage,
    SigCommandType_proxyConfigurationMessage,
    SigCommandType_unknownMessage,
} SigCommandType;

@interface SDKLibCommand : NSObject
@property (nonatomic,strong) SigBaseMeshMessage *curMeshMessage;
@property (nonatomic,strong) SigElementModel *source;
@property (nonatomic,strong) SigMeshAddress *destination;
@property (nonatomic,assign) UInt8 initialTtl;
@property (nonatomic,assign) SigCommandType commandType;
@property (nonatomic,strong) SigMessageHandle *messageHandle;
@property (nonatomic,assign) NSInteger responseMaxCount;
@property (nonatomic,strong) NSMutableArray <NSNumber *>*responseSourceArray;
@property (nonatomic,assign) UInt8 retryCount;//default is 2.
@property (nonatomic,assign) NSTimeInterval timeout;//default is 1.28s,SigDataSource.share.defaultReliableIntervalOfNotLPN.
@property (nonatomic,assign) UInt8 hadRetryCount;//default is 0.
@property (nonatomic, assign) UInt8 tidPosition;//default is 0.
@property (nonatomic, assign) BOOL hadReceiveAllResponse;//default is NO.
@property (nonatomic, assign) UInt8 tid;//default is 0.
@property (nonatomic,strong,nullable) BackgroundTimer *retryTimer;
//这3个参数的作用是配置当前SDKLibCommand指令实际使用到的key和ivIndex，只有fastProvision流程使用了特殊的key和ivIndex，其它指令使用默认值。
@property (nonatomic,strong) SigNetkeyModel *curNetkey;
@property (nonatomic,strong) SigAppkeyModel *curAppkey;
@property (nonatomic,strong) SigIvIndex *curIvIndex;
//v3.3.3.6之后才新增的两个参数
/// NO标识优先使用unSegment数据包进行发送，YES标识优先使用segment数据包进行发送。SDK默认会优先使用unSegment发送数据，如果超出SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength的长度，则会使用segment发送数据。
@property (nonatomic, assign) BOOL sendBySegmentPDU;
/// 值不可为0，且非零值为当前指令不进行分包时单个unSegment包的最大LowerTransportPDU长度。(非零值-3)为当前指令进行分包时单个发包的最大LowerTransportPDU长度。默认为SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength。
@property (nonatomic, assign) UInt16 unsegmentedMessageLowerTransportPDUMaxLength;

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
@property (nonatomic,copy) responseConfigKeyRefreshPhaseStatusMessageBlock responseKeyRefreshPhaseStatusCallBack;
@property (nonatomic,copy) responseConfigModelPublicationStatusMessageBlock responseModelPublicationStatusCallBack;
@property (nonatomic,copy) responseConfigModelSubscriptionStatusMessageBlock responseModelSubscriptionStatusCallBack;
@property (nonatomic,copy) responseConfigNetworkTransmitStatusMessageBlock responseNetworkTransmitStatusCallBack;
@property (nonatomic,copy) responseConfigRelayStatusMessageBlock responseRelayStatusCallBack;
@property (nonatomic,copy) responseConfigSIGModelSubscriptionListMessageBlock responseSIGModelSubscriptionListCallBack;
@property (nonatomic,copy) responseConfigVendorModelSubscriptionListMessageBlock responseVendorModelSubscriptionListCallBack;
@property (nonatomic,copy) responseConfigLowPowerNodePollTimeoutStatusMessageBlock responseLowPowerNodePollTimeoutStatusCallBack;
@property (nonatomic,copy) responseConfigHeartbeatPublicationStatusMessageBlock responseHeartbeatPublicationStatusCallBack;
@property (nonatomic,copy) responseConfigHeartbeatSubscriptionStatusMessageBlock responseHeartbeatSubscriptionStatusCallBack;
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
@property (nonatomic,copy) responseFirmwareDistributionReceiversStatusMessageBlock responseFirmwareDistributionReceiversStatusCallBack;
@property (nonatomic,copy) responseFirmwareDistributionReceiversListMessageBlock responseFirmwareDistributionReceiversListCallBack;
@property (nonatomic,copy) responseFirmwareDistributionCapabilitiesStatusMessageBlock responseFirmwareDistributionCapabilitiesStatusCallBack;
@property (nonatomic,copy) responseFirmwareDistributionUploadStatusMessageBlock responseFirmwareDistributionUploadStatusCallBack;
@property (nonatomic,copy) responseFirmwareDistributionFirmwareStatusMessageBlock responseFirmwareDistributionFirmwareStatusCallBack;
// callback about BLOB Transfer Messages
@property (nonatomic,copy) responseBLOBTransferStatusMessageBlock responseBLOBTransferStatusCallBack;
@property (nonatomic,copy) responseBLOBBlockStatusMessageBlock responseBLOBBlockStatusCallBack;
@property (nonatomic,copy) responseBLOBInformationStatusMessageBlock responseBLOBInformationStatusCallBack;

// callback about subnet bridge
@property (nonatomic,copy) responseSubnetBridgeStatusMessageBlock responseSubnetBridgeStatusCallBack;
@property (nonatomic,copy) responseBridgeTableStatusMessageBlock responseBridgeTableStatusCallBack;
@property (nonatomic,copy) responseBridgeSubnetsListMessageBlock responseBridgeSubnetsListCallBack;
@property (nonatomic,copy) responseBridgeTableListMessageBlock responseBridgeTableListCallBack;
@property (nonatomic,copy) responseBridgingTableSizeStatusMessageBlock responseBridgingTableSizeStatusCallBack;

// callback about Remote Provision
@property (nonatomic,copy) responseRemoteProvisioningScanCapabilitiesStatusMessageBlock responseRemoteProvisioningScanCapabilitiesStatusCallBack;
@property (nonatomic,copy) responseRemoteProvisioningScanStatusMessageBlock responseRemoteProvisioningScanStatusCallBack;
@property (nonatomic,copy) responseRemoteProvisioningLinkStatusMessageBlock responseRemoteProvisioningLinkStatusCallBack;
@property (nonatomic,copy) responseOpcodesAggregatorStatusMessageBlock responseOpcodesAggregatorStatusCallBack;

// callback about Private Beacon
@property (nonatomic,copy) responsePrivateBeaconStatusMessageBlock responsePrivateBeaconStatusCallBack;
@property (nonatomic,copy) responsePrivateGattProxyStatusMessageBlock responsePrivateGattProxyStatusCallBack;
@property (nonatomic,copy) responsePrivateNodeIdentityStatusMessageBlock responsePrivateNodeIdentityStatusCallBack;

/**
 * @brief   Initialize SDKLibCommand object.
 * @param   message    the message of command.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   responseAllMessageCallBack    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  return `nil` when initialize SigScanRspModel object fail.
 */
- (instancetype)initWithMessage:(SigBaseMeshMessage *)message retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock _Nullable)responseAllMessageCallBack resultCallback:(resultBlock)resultCallback;


#pragma mark - 4.3.2 Configuration messages

/**
 * @brief   Config AppKey Add.
 * @param   destination    the unicastAddress of destination.
 * @param   appkeyModel    the appkeyModel of mesh.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.37 Config AppKey Add, seeAlso: Mesh_v1.0.pdf  (page.169)
 */
+ (SigMessageHandle *)configAppKeyAddWithDestination:(UInt16)destination appkeyModel:(SigAppkeyModel *)appkeyModel retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigAppKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config AppKey Add.
 * @param   destination    the unicastAddress of destination.
 * @param   appkeyModel    the appkeyModel of mesh.
 * @param   sendBySegmentPdu    Whether to use segment to send this command.
 * @param   unsegmentedMessageLowerTransportPDUMaxLength    the appkeyModel of mesh.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.37 Config AppKey Add, seeAlso: Mesh_v1.0.pdf  (page.169)
 */
+ (SigMessageHandle *)configAppKeyAddWithDestination:(UInt16)destination appkeyModel:(SigAppkeyModel *)appkeyModel sendBySegmentPdu:(BOOL)sendBySegmentPdu unsegmentedMessageLowerTransportPDUMaxLength:(UInt16)unsegmentedMessageLowerTransportPDUMaxLength retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigAppKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config AppKey Update.
 * @param   destination    the unicastAddress of destination.
 * @param   appkeyModel    the appkeyModel of mesh.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.38 Config AppKey Update, seeAlso: Mesh_v1.0.pdf  (page.169)
 */
+ (SigMessageHandle *)configAppKeyUpdateWithDestination:(UInt16)destination appkeyModel:(SigAppkeyModel *)appkeyModel retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigAppKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config AppKey Delete.
 * @param   destination    the unicastAddress of destination.
 * @param   appkeyModel    the appkeyModel of mesh.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.39 Config AppKey Delete , seeAlso: Mesh_v1.0.pdf  (page.170)
 */
+ (SigMessageHandle *)configAppKeyDeleteWithDestination:(UInt16)destination appkeyModel:(SigAppkeyModel *)appkeyModel retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigAppKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config AppKey Get.
 * @param   destination    the unicastAddress of destination.
 * @param   networkKeyIndex    the networkKeyIndex of mesh.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.41 Config AppKey Get , seeAlso: Mesh_v1.0.pdf  (page.171)
 */
+ (SigMessageHandle *)configAppKeyGetWithDestination:(UInt16)destination networkKeyIndex:(UInt16)networkKeyIndex retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigAppKeyListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Beacon Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.1 Config Beacon Get , seeAlso: Mesh_v1.0.pdf  (page.154)
 */
+ (SigMessageHandle *)configBeaconGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigBeaconStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Beacon Set.
 * @param   destination    the unicastAddress of destination.
 * @param   secureNetworkBeaconState    New Secure Network Beacon state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.2 Config Beacon Set , seeAlso: Mesh_v1.0.pdf  (page.154)
 */
+ (SigMessageHandle *)configBeaconSetWithDestination:(UInt16)destination secureNetworkBeaconState:(SigSecureNetworkBeaconState)secureNetworkBeaconState retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigBeaconStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Composition Data Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.4 Config Composition Data Get, seeAlso: Mesh_v1.0.pdf  (page.154)
 */
+ (SigMessageHandle *)configCompositionDataGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigCompositionDataStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Default TTL Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.6 Config Default TTL Get, seeAlso: Mesh_v1.0.pdf  (page.155)
 */
+ (SigMessageHandle *)configDefaultTtlGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigDefaultTtlStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Default TTL Set.
 * @param   destination    the unicastAddress of destination.
 * @param   ttl    New Default TTL value.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.7 Config Default TTL Set, seeAlso: Mesh_v1.0.pdf  (page.155)
 */
+ (SigMessageHandle *)configDefaultTtlSetWithDestination:(UInt16)destination ttl:(UInt8)ttl retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigDefaultTtlStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Friend Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.55 Config Friend Get, seeAlso: Mesh_v1.0.pdf  (page.176)
 */
+ (SigMessageHandle *)configFriendGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigFriendStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Friend Set.
 * @param   destination    the unicastAddress of destination.
 * @param   nodeFeaturesState    New Friend state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.56 Config Friend Set, seeAlso: Mesh_v1.0.pdf  (page.176)
 */
+ (SigMessageHandle *)configFriendSetWithDestination:(UInt16)destination nodeFeaturesState:(SigNodeFeaturesState)nodeFeaturesState retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigFriendStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config GATT Proxy Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.9 Config GATT Proxy Get, seeAlso: Mesh_v1.0.pdf  (page.156)
 */
+ (SigMessageHandle *)configGATTProxyGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigGATTProxyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config GATT Proxy Set.
 * @param   destination    the unicastAddress of destination.
 * @param   nodeGATTProxyState    New GATT Proxy state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.10 Config GATT Proxy Set, seeAlso: Mesh_v1.0.pdf  (page.156)
 */
+ (SigMessageHandle *)configGATTProxySetWithDestination:(UInt16)destination nodeGATTProxyState:(SigNodeGATTProxyState)nodeGATTProxyState retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigGATTProxyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Key Refresh Phase Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.58 Config Key Refresh Phase Get, seeAlso: Mesh_v1.0.pdf  (page.177)
 */
+ (SigMessageHandle *)configKeyRefreshPhaseGetWithDestination:(UInt16)destination netKeyIndex:(UInt16)netKeyIndex retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigKeyRefreshPhaseStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Key Refresh Phase Set.
 * @param   destination    the unicastAddress of destination.
 * @param   netKeyIndex    NetKey Index.
 * @param   transition    New Key Refresh Phase Transition.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.59 Config Key Refresh Phase Set, seeAlso: Mesh_v1.0.pdf  (page.177)
 */
+ (SigMessageHandle *)configKeyRefreshPhaseSetWithDestination:(UInt16)destination netKeyIndex:(UInt16)netKeyIndex transition:(SigControllableKeyRefreshTransitionValues)transition retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigKeyRefreshPhaseStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Model Publication Get.
 * @param   destination    the unicastAddress of destination.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.15 Config Model Publication Get, seeAlso: Mesh_v1.0.pdf  (page.157)
 */
+ (SigMessageHandle *)configModelPublicationGetWithDestination:(UInt16)destination elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Model Publication Set.
 * @param   destination    the unicastAddress of destination.
 * @param   publish    new publosh config value of node.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.16 Config Model Publication Set, seeAlso: Mesh_v1.0.pdf  (page.158)
 */
+ (SigMessageHandle *)configModelPublicationSetWithDestination:(UInt16)destination publish:(SigPublish *)publish elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Model Publication Virtual Address Set.
 * @param   destination    the unicastAddress of destination.
 * @param   publish    new publosh config value of node.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.17 Config Model Publication Virtual Address Set, seeAlso: Mesh_v1.0.pdf  (page.159)
 */
+ (SigMessageHandle *)configModelPublicationVirtualAddressSetWithDestination:(UInt16)destination publish:(SigPublish *)publish elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Model Subscription Add.
 * @param   destination    the unicastAddress of destination.
 * @param   groupAddress    Value of the address.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.19 Config Model Subscription Add, seeAlso: Mesh_v1.0.pdf  (page.161)
 */
+ (SigMessageHandle *)configModelSubscriptionAddWithDestination:(UInt16)destination toGroupAddress:(UInt16)groupAddress elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Model Subscription Delete.
 * @param   destination    the unicastAddress of destination.
 * @param   groupAddress    Value of the address.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.21 Config Model Subscription Delete, seeAlso: Mesh_v1.0.pdf  (page.162)
 */
+ (SigMessageHandle *)configModelSubscriptionDeleteWithDestination:(UInt16)destination groupAddress:(UInt16)groupAddress elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Model Subscription Delete All.
 * @param   destination    the unicastAddress of destination.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.25 Config Model Subscription Delete All, seeAlso: Mesh_v1.0.pdf  (page.164)
 */
+ (SigMessageHandle *)configModelSubscriptionDeleteAllWithDestination:(UInt16)destination elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Model Subscription Overwrite.
 * @param   destination    the unicastAddress of destination.
 * @param   groupAddress    Value of the address.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.23 Config Model Subscription Overwrite, seeAlso: Mesh_v1.0.pdf  (page.163)
 */
+ (SigMessageHandle *)configModelSubscriptionOverwriteWithDestination:(UInt16)destination groupAddress:(UInt16)groupAddress elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Model Subscription Virtual Address Add.
 * @param   destination    the unicastAddress of destination.
 * @param   virtualLabel    Value of the Label UUID.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.20 Config Model Subscription Virtual Address Add, seeAlso: Mesh_v1.0.pdf  (page.162)
 */
+ (SigMessageHandle *)configModelSubscriptionVirtualAddressAddWithDestination:(UInt16)destination virtualLabel:(CBUUID *)virtualLabel elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Model Subscription Virtual Address Delete.
 * @param   destination    the unicastAddress of destination.
 * @param   virtualLabel    Value of the Label UUID.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.22 Config Model Subscription Virtual Address Delete, seeAlso: Mesh_v1.0.pdf  (page.163)
 */
+ (SigMessageHandle *)configModelSubscriptionVirtualAddressDeleteWithDestination:(UInt16)destination virtualLabel:(CBUUID *)virtualLabel elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Model Subscription Virtual Address Overwrite.
 * @param   destination    the unicastAddress of destination.
 * @param   virtualLabel    Value of the Label UUID.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.24 Config Model Subscription Virtual Address Overwrite, seeAlso: Mesh_v1.0.pdf  (page.164)
 */
+ (SigMessageHandle *)configModelSubscriptionVirtualAddressOverwriteWithDestination:(UInt16)destination virtualLabel:(CBUUID *)virtualLabel elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Network Transmit Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.69 Config Network Transmit Get, seeAlso: Mesh_v1.0.pdf  (page.182)
 */
+ (SigMessageHandle *)configNetworkTransmitGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNetworkTransmitStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Network Transmit Set.
 * @param   destination    the unicastAddress of destination.
 * @param   networkTransmitCount    Number of transmissions for each Network PDU originating from the node.
 * @param   networkTransmitIntervalSteps    Number of 10-millisecond steps between transmissions.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.70 Config Network Transmit Set, seeAlso: Mesh_v1.0.pdf  (page.182)
 */
+ (SigMessageHandle *)configNetworkTransmitSetWithDestination:(UInt16)destination networkTransmitCount:(UInt8)networkTransmitCount networkTransmitIntervalSteps:(UInt8)networkTransmitIntervalSteps retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNetworkTransmitStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Relay Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.12 Config Relay Get, seeAlso: Mesh_v1.0.pdf  (page.156)
 */
+ (SigMessageHandle *)configRelayGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigRelayStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Relay Set.
 * @param   destination    the unicastAddress of destination.
 * @param   relay    the value of relay.
 * @param   networkTransmitCount    Number of retransmissions on advertising bearer for each Network PDU relayed by the node.
 * @param   networkTransmitIntervalSteps    Number of 10-millisecond steps between retransmissions.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.13 Config Relay Set, seeAlso: Mesh_v1.0.pdf  (page.156)
 */
+ (SigMessageHandle *)configRelaySetWithDestination:(UInt16)destination relay:(SigNodeRelayState)relay networkTransmitCount:(UInt8)networkTransmitCount networkTransmitIntervalSteps:(UInt8)networkTransmitIntervalSteps retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigRelayStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config SIG Model Subscription Get.
 * @param   destination    the unicastAddress of destination.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.27 Config SIG Model Subscription Get, seeAlso: Mesh_v1.0.pdf  (page.165)
 */
+ (SigMessageHandle *)configSIGModelSubscriptionGetWithDestination:(UInt16)destination elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigSIGModelSubscriptionListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Vendor Model Subscription Get.
 * @param   destination    the unicastAddress of destination.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.29 Config Vendor Model Subscription Get, seeAlso: Mesh_v1.0.pdf  (page.166)
 */
+ (SigMessageHandle *)configVendorModelSubscriptionGetWithDestination:(UInt16)destination elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigVendorModelSubscriptionListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Low Power Node PollTimeout Get.
 * @param   destination    the unicastAddress of destination.
 * @param   LPNAddress    The unicast address of the Low Power node.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.67 Config Low Power Node PollTimeout Get, seeAlso: Mesh_v1.0.pdf  (page.181)
 */
+ (SigMessageHandle *)configLowPowerNodePollTimeoutGetWithDestination:(UInt16)destination LPNAddress:(UInt16)LPNAddress retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigLowPowerNodePollTimeoutStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Heartbeat Publication Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.61 Config Heartbeat Publication Get, seeAlso: Mesh_v1.0.pdf  (page.178)
 */
+ (SigMessageHandle *)configHeartbeatPublicationGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigHeartbeatPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Heartbeat Publication Set.
 * @param   destination    the unicastAddress of destination.
 * @param   countLog    Number of Heartbeat messages to be sent.
 * @param   periodLog    Period for sending Heartbeat messages.
 * @param   ttl    TTL to be used when sending Heartbeat messages.
 * @param   features    Bit field indicating features that trigger Heartbeat messages when changed.
 * @param   netKeyIndex    NetKey Index.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.62 Config Heartbeat Publication Set, seeAlso: Mesh_v1.0.pdf  (page.179)
 */
+ (SigMessageHandle *)configHeartbeatPublicationSetWithDestination:(UInt16)destination countLog:(UInt8)countLog periodLog:(UInt8)periodLog ttl:(UInt8)ttl features:(SigFeatures)features netKeyIndex:(UInt16)netKeyIndex retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigHeartbeatPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Heartbeat Subscription Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.64 Config Heartbeat Subscription Get, seeAlso: Mesh_v1.0.pdf  (page.180)
 */
+ (SigMessageHandle *)configHeartbeatSubscriptionGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigHeartbeatSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Heartbeat Subscription Set.
 * @param   destination    the unicastAddress of destination.
 * @param   source    Source address for Heartbeat messages.
 * @param   periodLog    Period for receiving Heartbeat messages.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.65 Config Heartbeat Subscription Set, seeAlso: Mesh_v1.0.pdf  (page.180)
 */
+ (SigMessageHandle *)configHeartbeatSubscriptionSetWithDestination:(UInt16)destination source:(UInt16)source periodLog:(UInt8)periodLog retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigHeartbeatSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Model App Bind.
 * @param   destination    the unicastAddress of destination.
 * @param   applicationKeyIndex    Index of the AppKey.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.46 Config Model App Bind, seeAlso: Mesh_v1.0.pdf  (page.173)
 */
+ (SigMessageHandle *)configModelAppBindWithDestination:(UInt16)destination applicationKeyIndex:(UInt16)applicationKeyIndex elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelAppStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Model App Unbind.
 * @param   destination    the unicastAddress of destination.
 * @param   applicationKeyIndex    Index of the AppKey.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.47 Config Model App Unbind, seeAlso: Mesh_v1.0.pdf  (page.173)
 */
+ (SigMessageHandle *)configModelAppUnbindWithDestination:(UInt16)destination applicationKeyIndex:(UInt16)applicationKeyIndex elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelAppStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Model App Bind.
 * @param   destination    the unicastAddress of destination.
 * @param   applicationKeyIndex    Index of the AppKey.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   isVendorModelID    YES means Vendor Model ID, NO means SIG Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.46 Config Model App Bind, seeAlso: Mesh_v1.0.pdf  (page.173)
 */
+ (SigMessageHandle *)configModelAppBindWithDestination:(UInt16)destination applicationKeyIndex:(UInt16)applicationKeyIndex elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier isVendorModelID:(BOOL)isVendorModelID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelAppStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Model App Unbind.
 * @param   destination    the unicastAddress of destination.
 * @param   applicationKeyIndex    Index of the AppKey.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   isVendorModelID    YES means Vendor Model ID, NO means SIG Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.47 Config Model App Unbind, seeAlso: Mesh_v1.0.pdf  (page.173)
 */
+ (SigMessageHandle *)configModelAppUnbindWithDestination:(UInt16)destination applicationKeyIndex:(UInt16)applicationKeyIndex elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier isVendorModelID:(BOOL)isVendorModelID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelAppStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config NetKey Add.
 * @param   destination    the unicastAddress of destination.
 * @param   networkKeyIndex    NetKey Index.
 * @param   networkKeyData    NetKey.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.31 Config NetKey Add, seeAlso: Mesh_v1.0.pdf  (page.167)
 */
+ (SigMessageHandle *)configNetKeyAddWithDestination:(UInt16)destination networkKeyIndex:(UInt16)networkKeyIndex networkKeyData:(NSData *)networkKeyData retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNetKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config NetKey Delete.
 * @param   destination    the unicastAddress of destination.
 * @param   networkKeyIndex    NetKey Index.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.33 Config NetKey Delete, seeAlso: Mesh_v1.0.pdf  (page.168)
 */
+ (SigMessageHandle *)configNetKeyDeleteWithDestination:(UInt16)destination networkKeyIndex:(UInt16)networkKeyIndex retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNetKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config NetKey Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.35 Config NetKey Get, seeAlso: Mesh_v1.0.pdf  (page.169)
 */
+ (SigMessageHandle *)configNetKeyGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNetKeyListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config NetKey Update.
 * @param   destination    the unicastAddress of destination.
 * @param   networkKeyIndex    NetKey Index.
 * @param   networkKeyData    NetKey.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.32 Config NetKey Update, seeAlso: Mesh_v1.0.pdf  (page.167)
 */
+ (SigMessageHandle *)configNetKeyUpdateWithDestination:(UInt16)destination networkKeyIndex:(UInt16)networkKeyIndex networkKeyData:(NSData *)networkKeyData retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNetKeyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Node Identity Get.
 * @param   destination    the unicastAddress of destination.
 * @param   netKeyIndex    Index of the NetKey.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.43 Config Node Identity Get, seeAlso: Mesh_v1.0.pdf  (page.171)
 */
+ (SigMessageHandle *)configNodeIdentityGetWithDestination:(UInt16)destination netKeyIndex:(UInt16)netKeyIndex retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNodeIdentityStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Node Identity Set.
 * @param   destination    the unicastAddress of destination.
 * @param   netKeyIndex    Index of the NetKey.
 * @param   identity    New Node Identity state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.44 Config Node Identity Set, seeAlso: Mesh_v1.0.pdf  (page.172)
 */
+ (SigMessageHandle *)configNodeIdentitySetWithDestination:(UInt16)destination netKeyIndex:(UInt16)netKeyIndex identity:(SigNodeIdentityState)identity retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNodeIdentityStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Node Reset.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.53 Config Node Reset, seeAlso: Mesh_v1.0.pdf  (page.176)
 */
+ (SigMessageHandle *)resetNodeWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNodeResetStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config SIG Model App Get.
 * @param   destination    the unicastAddress of destination.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.49 Config SIG Model App Get, seeAlso: Mesh_v1.0.pdf  (page.174)
 */
+ (SigMessageHandle *)configSIGModelAppGetWithDestination:(UInt16)destination elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigSIGModelAppListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Config Vendor Model App Get.
 * @param   destination    the unicastAddress of destination.
 * @param   elementAddress    Address of the element.
 * @param   modelIdentifier    modelIdentifier of SIG Model ID or Vendor Model ID.
 * @param   companyIdentifier    companyIdentifier of Vendor Model ID.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.3.2.51 Config Vendor Model App Get, seeAlso: Mesh_v1.0.pdf  (page.175)
 */
+ (SigMessageHandle *)configVendorModelAppGetWithDestination:(UInt16)destination elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigVendorModelAppListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark - 3.2 Generic messages


#pragma mark - 3.2.1 Generic OnOff messages


/**
 * @brief   Generic OnOff Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.1.1 Generic OnOff Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.42)
 */
+ (SigMessageHandle *)genericOnOffGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Generic OnOff Set without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   isOn    The target value of the Generic OnOff state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.1.2 Generic OnOff Set and 3.2.1.3 Generic OnOff Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.43)
 */
+ (SigMessageHandle *)genericOnOffSetWithDestination:(UInt16)destination isOn:(BOOL)isOn retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Generic OnOff Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   isOn    The target value of the Generic OnOff state.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.1.2 Generic OnOff Set and 3.2.1.3 Generic OnOff Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.43)
 */
+ (SigMessageHandle *)genericOnOffSetDestination:(UInt16)destination isOn:(BOOL)isOn transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark - 3.2.2 Generic Level messages


/**
 * @brief   Generic Level Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.2.1 Generic Level Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.43)
 */
+ (SigMessageHandle *)genericLevelGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Generic Level Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   level    The target value of the Generic Level state.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.2.2 Generic Level Set and 3.2.2.3 Generic Level Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.44)
 */
+ (SigMessageHandle *)genericLevelSetWithDestination:(UInt16)destination level:(UInt16)level transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Generic Level Set without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   level    The target value of the Generic Level state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.2.2 Generic Level Set and 3.2.2.3 Generic Level Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.44)
 */
+ (SigMessageHandle *)genericLevelSetWithDestination:(UInt16)destination level:(UInt16)level retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Generic Delta Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   delta    The Delta change of the Generic Level state.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.2.4 Generic Delta Set and 3.2.2.5 Generic Delta Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.46)
 */
+ (SigMessageHandle *)genericDeltaSetWithDestination:(UInt16)destination delta:(UInt32)delta transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Generic Delta Set without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   delta    The Delta change of the Generic Level state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.2.4 Generic Delta Set and 3.2.2.5 Generic Delta Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.46)
 */
+ (SigMessageHandle *)genericDeltaSetWithDestination:(UInt16)destination delta:(UInt32)delta retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Generic Move Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   deltaLevel    The Delta Level step to calculate Move speed for the Generic Level state..
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.2.6 Generic Move Set and 3.2.2.7 Generic Move Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.47)
 */
+ (SigMessageHandle *)genericMoveSetWithDestination:(UInt16)destination deltaLevel:(UInt16)deltaLevel transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Generic Move Set without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   deltaLevel    The Delta Level step to calculate Move speed for the Generic Level state..
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.2.6 Generic Move Set and 3.2.2.7 Generic Move Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.47)
 */
+ (SigMessageHandle *)genericMoveSetWithDestination:(UInt16)destination deltaLevel:(UInt16)deltaLevel retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark - 3.2.3 Generic Default Transition Time messages


/**
 * @brief   Generic Default Transition Time Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.3.1 Generic Default Transition Time Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.48)
 */
+ (SigMessageHandle *)genericDefaultTransitionTimeGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericDefaultTransitionTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Generic Default Transition Time Set.
 * @param   destination    the unicastAddress of destination.
 * @param   defaultTransitionTime    The value of the Generic Default Transition Time state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.3.2 Generic Default Transition Time Set and 3.2.3.3 Generic Default Transition Time Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.49)
 */
+ (SigMessageHandle *)genericDefaultTransitionTimeSetWithDestination:(UInt16)destination defaultTransitionTime:(nonnull SigTransitionTime *)defaultTransitionTime retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericDefaultTransitionTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark - 3.2.4 Generic OnPowerUp messages


/**
 * @brief   Generic OnPowerUp Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.4.1 Generic OnPowerUp Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.49)
 */
+ (SigMessageHandle *)genericOnPowerUpGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericOnPowerUpStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Generic OnPowerUp Set.
 * @param   destination    the unicastAddress of destination.
 * @param   onPowerUp    The value of the Generic OnPowerUp state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.4.2 Generic OnPowerUp Set and 3.2.4.3 Generic OnPowerUp Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.49)
 */
+ (SigMessageHandle *)genericOnPowerUpSetWithDestination:(UInt16)destination onPowerUp:(SigOnPowerUp)onPowerUp retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericOnPowerUpStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark - 3.2.5 Generic Power Level messages


/**
 * @brief   Generic Power Level Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.5.1 Generic Power Level Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.50)
 */
+ (SigMessageHandle *)genericPowerLevelGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericPowerLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Generic Power Level Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   power    The target value of the Generic Power Actual state.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.5.2 Generic Power Level Set and 3.2.5.3 Generic Power Level Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.51)
 */
+ (SigMessageHandle *)genericPowerLevelSetWithDestination:(UInt16)destination power:(UInt16)power transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Generic Power Level Set without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   power    The target value of the Generic Power Actual state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.5.2 Generic Power Level Set and 3.2.5.3 Generic Power Level Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.51)
 */
+ (SigMessageHandle *)genericPowerLevelSetWithDestination:(UInt16)destination power:(UInt16)power retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Generic Power Last Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.5.5 Generic Power Last Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.52)
 */
+ (SigMessageHandle *)genericPowerLastGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericPowerLastStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Generic Power Default Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.5.7 Generic Power Default Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.53)
 */
+ (SigMessageHandle *)genericPowerDefaultGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericPowerDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Generic Power Range Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.5.11 Generic Power Range Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.54)
 */
+ (SigMessageHandle *)genericPowerRangeGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericPowerRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Generic Power Default Set.
 * @param   destination    the unicastAddress of destination.
 * @param   power    The value of the Generic Power Default state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.5.8 Generic Power Default Set and 3.2.5.8 Generic Power Default Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.53)
 */
+ (SigMessageHandle *)genericPowerDefaultSetWithDestination:(UInt16)destination power:(UInt16)power retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericPowerDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Generic Power Range Set.
 * @param   destination    the unicastAddress of destination.
 * @param   rangeMin    The value of the Generic Power Min field of the Generic Power Range state.
 * @param   rangeMax    The value of the Generic Power Range Max field of the Generic Power Range state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.5.12 Generic Power Range Set and 3.2.5.13 Generic Power Range Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.54)
 */
+ (SigMessageHandle *)genericPowerRangeSetWithDestination:(UInt16)destination rangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericPowerRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark - 3.2.6 Generic Battery messages


/**
 * @brief   Generic Battery Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    3.2.6.1 Generic Battery Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.55)
 */
+ (SigMessageHandle *)genericBatteryGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseGenericBatteryStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark - 4.2 Sensor messages


/**
 * @brief   Sensor Descriptor Get without propertyID.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.2.1 Sensor Descriptor Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.114)
 */
+ (SigMessageHandle *)sensorDescriptorGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorDescriptorStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Sensor Descriptor Get with propertyID.
 * @param   destination    the unicastAddress of destination.
 * @param   propertyID    Property ID for the sensor (Optional).
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.2.1 Sensor Descriptor Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.114)
 */
+ (SigMessageHandle *)sensorDescriptorGetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorDescriptorStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Sensor Get without propertyID.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.2.13 Sensor Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.120)
 */
+ (SigMessageHandle *)sensorGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Sensor Get with propertyID.
 * @param   destination    the unicastAddress of destination.
 * @param   propertyID    Property ID for the sensor (Optional).
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.2.13 Sensor Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.120)
 */
+ (SigMessageHandle *)sensorGetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Sensor Column Get.
 * @param   destination    the unicastAddress of destination.
 * @param   propertyID    Property identifying a sensor.
 * @param   rawValueX    Raw value identifying a column.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.2.15 Sensor Column Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.122)
 */
+ (SigMessageHandle *)sensorColumnGetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID rawValueX:(NSData *)rawValueX retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorColumnStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Sensor Series Get.
 * @param   destination    the unicastAddress of destination.
 * @param   propertyID    Property identifying a sensor.
 * @param   rawValueX1Data    Raw value identifying a starting column. (Optional).
 * @param   rawValueX2Data    Raw value identifying an ending column. (C.1).
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.2.17 Sensor Series Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.123)
 */
+ (SigMessageHandle *)sensorSeriesGetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID rawValueX1Data:(NSData *)rawValueX1Data rawValueX2Data:(NSData *)rawValueX2Data retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorSeriesStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Sensor Cadence Get.
 * @param   destination    the unicastAddress of destination.
 * @param   propertyID    Property ID for the sensor..
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.2.3 Sensor Cadence Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.115)
 */
+ (SigMessageHandle *)sensorCadenceGetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorCadenceStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Sensor Cadence Set.
 * @param   destination    the unicastAddress of destination.
 * @param   propertyID    Property ID for the sensor..
 * @param   cadenceData    Sensor Cadence config data.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.2.4 Sensor Cadence Set and 4.2.5 Sensor Cadence Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.115)
 */
+ (SigMessageHandle *)sensorCadenceSetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID cadenceData:(NSData *)cadenceData retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseSensorCadenceStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Sensor Settings Get.
 * @param   destination    the unicastAddress of destination.
 * @param   propertyID    Property ID identifying a sensor.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.2.7 Sensor Settings Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.117)
 */
+ (SigMessageHandle *)sensorSettingsGetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorSettingsStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Sensor Setting Get.
 * @param   destination    the unicastAddress of destination.
 * @param   propertyID    Property ID identifying a sensor.
 * @param   settingPropertyID    Setting Property ID identifying a setting within a sensor.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.2.9 Sensor Setting Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.118)
 */
+ (SigMessageHandle *)sensorSettingGetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID settingPropertyID:(UInt16)settingPropertyID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSensorSettingStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Sensor Setting Set.
 * @param   destination    the unicastAddress of destination.
 * @param   propertyID    Property ID identifying a sensor.
 * @param   settingPropertyID    Setting Property ID identifying a setting within a sensor.
 * @param   settingRaw    Raw value for the setting.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    4.2.10 Sensor Setting Set, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.118)
 */
+ (SigMessageHandle *)sensorSettingSetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID settingPropertyID:(UInt16)settingPropertyID settingRaw:(NSData *)settingRaw retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseSensorSettingStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark - 5.2.1 Time messages


/**
 * @brief   Time Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.1.1 Time Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.142)
 */
+ (SigMessageHandle *)timeGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Time Set.
 * @param   destination    the unicastAddress of destination.
 * @param   timeModel    config time data object.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.1.2 Time Set, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.143)
 */
+ (SigMessageHandle *)timeSetWithDestination:(UInt16)destination timeModel:(SigTimeModel *)timeModel retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Time Status.
 * @param   destination    the unicastAddress of destination.
 * @param   timeModel    config time data object.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.1.3 Time Status, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.143)
 */
+ (SigMessageHandle *)timeStatusWithDestination:(UInt16)destination timeModel:(SigTimeModel *)timeModel retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(_Nullable responseTimeStatusMessageBlock)successCallback resultCallback:(_Nullable resultBlock)resultCallback;

/**
 * @brief   Time Role Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.1.10 Time Role Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.145)
 */
+ (SigMessageHandle *)timeRoleGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseTimeRoleStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Time Role Set.
 * @param   destination    the unicastAddress of destination.
 * @param   timeRole    The Time Role for the element.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.1.11 Time Role Set, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.146)
 */
+ (SigMessageHandle *)timeRoleSetWithDestination:(UInt16)destination timeRole:(SigTimeRole)timeRole retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseTimeRoleStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Time Zone Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.1.4 Time Zone Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.144)
 */
+ (SigMessageHandle *)timeZoneGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseTimeZoneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Time Zone Set.
 * @param   destination    the unicastAddress of destination.
 * @param   timeZoneOffsetNew    Upcoming local time zone offset.
 * @param   TAIOfZoneChange    TAI Seconds time of the upcoming Time Zone Offset change.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.1.5 Time Zone Set, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.144)
 */
+ (SigMessageHandle *)timeZoneSetWithDestination:(UInt16)destination timeZoneOffsetNew:(UInt8)timeZoneOffsetNew TAIOfZoneChange:(UInt64)TAIOfZoneChange retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseTimeZoneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   TAI-UTC Delta Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.1.7 TAI-UTC Delta Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.145)
 */
+ (SigMessageHandle *)TAI_UTC_DeltaGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseTAI_UTC_DeltaStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   TAI-UTC Delta Set.
 * @param   destination    the unicastAddress of destination.
 * @param   TAI_UTC_DeltaNew    Upcoming difference between TAI and UTC in seconds.
 * @param   padding    Always 0b0. Other values are Prohibited..
 * @param   TAIOfDeltaChange    TAI Seconds time of the upcoming TAI-UTC Delta change.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.1.8 TAI-UTC Delta Set, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.145)
 */
+ (SigMessageHandle *)TAI_UTC_DeltaSetWithDestination:(UInt16)destination TAI_UTC_DeltaNew:(UInt16)TAI_UTC_DeltaNew padding:(UInt8)padding TAIOfDeltaChange:(UInt64)TAIOfDeltaChange retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseTAI_UTC_DeltaStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark - 5.2.2 Scene messages


/**
 * @brief   Scene Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.2.5 Scene Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.148)
 */
+ (SigMessageHandle *)sceneGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSceneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Scene Recall with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   sceneNumber    The number of the scene to be recalled.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps. (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.2.3 Scene Recall and 5.2.2.4 Scene Recall Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.148)
 */
+ (SigMessageHandle *)sceneRecallWithDestination:(UInt16)destination sceneNumber:(UInt16)sceneNumber transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseSceneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Scene Recall without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   sceneNumber    The number of the scene to be recalled.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.2.3 Scene Recall and 5.2.2.4 Scene Recall Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.148)
 */
+ (SigMessageHandle *)sceneRecallWithDestination:(UInt16)destination sceneNumber:(UInt16)sceneNumber retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseSceneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Scene Register Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.2.7 Scene Register Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.148)
 */
+ (SigMessageHandle *)sceneRegisterGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Scene Store.
 * @param   destination    the unicastAddress of destination.
 * @param   sceneNumber    The number of the scene to be stored.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.2.1 Scene Store and 5.2.2.2 Scene Store Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.146)
 */
+ (SigMessageHandle *)sceneStoreWithDestination:(UInt16)destination sceneNumber:(UInt16)sceneNumber retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Scene Delete.
 * @param   destination    the unicastAddress of destination.
 * @param   sceneNumber    The number of the scene to be deleted.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.2.9 Scene Delete and 5.2.2.10 Scene Delete Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.149)
 */
+ (SigMessageHandle *)sceneDeleteWithDestination:(UInt16)destination sceneNumber:(UInt16)sceneNumber retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark - 5.2.3 Scheduler messages


/**
 * @brief   Scheduler Action Get.
 * @param   destination    the unicastAddress of destination.
 * @param   schedulerIndex    Index of the Schedule Register entry to get.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.3.3 Scheduler Action Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.150)
 */
+ (SigMessageHandle *)schedulerActionGetWithDestination:(UInt16)destination schedulerIndex:(UInt8)schedulerIndex retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSchedulerActionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Scheduler Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.3.1 Scheduler Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.150)
 */
+ (SigMessageHandle *)schedulerGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSchedulerStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Scheduler Action Set.
 * @param   destination    the unicastAddress of destination.
 * @param   schedulerModel    Bit field defining an entry in the Schedule Register (see Section 5.1.4.2)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    5.2.3.4 Scheduler Action Set and 5.2.3.5 Scheduler Action Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.150)
 */
+ (SigMessageHandle *)schedulerActionSetWithDestination:(UInt16)destination schedulerModel:(SchedulerModel *)schedulerModel retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseSchedulerActionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark - 6.3.1 Light Lightness messages


/**
 * @brief   Light Lightness Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.1.1 Light Lightness Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.199)
 */
+ (SigMessageHandle *)lightLightnessGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLightnessStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light Lightness Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   lightness    The target value of the Light Lightness Actual state.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps. (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.1.2 Light Lightness Set and 6.3.1.3 Light Lightness Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.200)
 */
+ (SigMessageHandle *)lightLightnessSetWithDestination:(UInt16)destination lightness:(UInt16)lightness transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLightnessStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light Lightness Set without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   lightness    The target value of the Light Lightness Actual state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.1.2 Light Lightness Set and 6.3.1.3 Light Lightness Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.200)
 */
+ (SigMessageHandle *)lightLightnessSetWithDestination:(UInt16)destination lightness:(UInt16)lightness retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLightnessStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light Lightness Linear Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.1.5 Light Lightness Linear Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.201)
 */
+ (SigMessageHandle *)lightLightnessLinearGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLightnessLinearStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light Lightness Linear Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   lightness    The target value of the Light Lightness Linear state.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps. (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.1.6 Light Lightness Linear Set and 6.3.1.7 Light Lightness Linear Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.201)
 */
+ (SigMessageHandle *)lightLightnessLinearSetWithDestination:(UInt16)destination lightness:(UInt16)lightness transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLightnessLinearStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light Lightness Linear Set without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   lightness    The target value of the Light Lightness Linear state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.1.6 Light Lightness Linear Set and 6.3.1.7 Light Lightness Linear Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.201)
 */
+ (SigMessageHandle *)lightLightnessLinearSetWithDestination:(UInt16)destination lightness:(UInt16)lightness retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLightnessLinearStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light Lightness Last Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.1.9 Light Lightness Last Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.203)
 */
+ (SigMessageHandle *)lightLightnessLastGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLightnessLastStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light Lightness Default Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.1.11 Light Lightness Default Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.204)
 */
+ (SigMessageHandle *)lightLightnessDefaultGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLightnessDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light Lightness Range Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.1.15 Light Lightness Range Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.205)
 */
+ (SigMessageHandle *)lightLightnessRangeGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLightnessRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light Lightness Default Set.
 * @param   destination    the unicastAddress of destination.
 * @param   lightness    The value of the Light Lightness Default state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.1.12 Light Lightness Default Set and 6.3.1.13 Light Lightness Default Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.204)
 */
+ (SigMessageHandle *)lightLightnessDefaultSetWithDestination:(UInt16)destination lightness:(UInt16)lightness retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLightnessDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light Lightness Range Set.
 * @param   destination    the unicastAddress of destination.
 * @param   rangeMin    The value of the Lightness Range Min field of the Light Lightness Range state.
 * @param   rangeMax    The value of the Lightness Range Max field of the Light Lightness Range state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.1.16 Light Lightness Range Set and 6.3.1.17 Light Lightness Range Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.205)
 */
+ (SigMessageHandle *)lightLightnessRangeSetWithDestination:(UInt16)destination rangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLightnessRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark - 6.3.2 Light CTL Messages


/**
 * @brief   Light CTL Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.2.1 Light CTL Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.206)
 */
+ (SigMessageHandle *)lightCTLGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightCTLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light CTL Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   lightness    The target value of the Light CTL Lightness state.
 * @param   temperature    The target value of the Light CTL Temperature state.
 * @param   deltaUV    The target value of the Light CTL Delta UV state.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps. (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.2.2 Light CTL Set and 6.3.2.3 Light CTL Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.206)
 */
+ (SigMessageHandle *)lightCTLSetWithDestination:(UInt16)destination lightness:(UInt16)lightness temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightCTLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light CTL Set without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   lightness    The target value of the Light CTL Lightness state.
 * @param   temperature    The target value of the Light CTL Temperature state.
 * @param   deltaUV    The target value of the Light CTL Delta UV state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.2.2 Light CTL Set and 6.3.2.3 Light CTL Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.206)
 */
+ (SigMessageHandle *)lightCTLSetWithDestination:(UInt16)destination lightness:(UInt16)lightness temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightCTLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light CTL Temperature Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.2.5 Light CTL Temperature Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.208)
 */
+ (SigMessageHandle *)lightCTLTemperatureGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightCTLTemperatureStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light CTL Temperature Range Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.2.9 Light CTL Temperature Range Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.210)
 */
+ (SigMessageHandle *)lightCTLTemperatureRangeGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightCTLTemperatureRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light CTL Temperature Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   temperature    The target value of the Light CTL Temperature state.
 * @param   deltaUV    The target value of the Light CTL Delta UV state.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps. (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.2.6 Light CTL Temperature Set and 6.3.2.7 Light CTL Temperature Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.208)
 */
+ (SigMessageHandle *)lightCTLTemperatureSetWithDestination:(UInt16)destination temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightCTLTemperatureStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light CTL Temperature Set without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   temperature    The target value of the Light CTL Temperature state.
 * @param   deltaUV    The target value of the Light CTL Delta UV state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.2.6 Light CTL Temperature Set and 6.3.2.7 Light CTL Temperature Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.208)
 */
+ (SigMessageHandle *)lightCTLTemperatureSetWithDestination:(UInt16)destination temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightCTLTemperatureStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light CTL Default Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.2.13 Light CTL Default Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.212)
 */
+ (SigMessageHandle *)lightCTLDefaultGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightCTLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light CTL Default Set.
 * @param   destination    the unicastAddress of destination.
 * @param   lightness    The value of the Light Lightness Default state.
 * @param   temperature    The value of the Light CTL Temperature Default state.
 * @param   deltaUV    The value of the Light CTL Delta UV Default state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.2.14 Light CTL Default Set and 6.3.2.15 Light CTL Default Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.212)
 */
+ (SigMessageHandle *)lightCTLDefaultSetWithDestination:(UInt16)destination lightness:(UInt16)lightness temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightCTLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light CTL Temperature Range Set.
 * @param   destination    the unicastAddress of destination.
 * @param   rangeMin    The value of the Temperature Range Min field of the Light CTL Temperature Range state.
 * @param   rangeMax    The value of the Temperature Range Max field of the Light CTL Temperature Range state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.2.10 Light CTL Temperature Range Set and 6.3.2.11 Light CTL Temperature Range Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.210)
 */
+ (SigMessageHandle *)lightCTLTemperatureRangeSetWithDestination:(UInt16)destination rangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightCTLTemperatureRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark - 6.3.3 Light HSL messages


/**
 * @brief   Light HSL Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.1 Light HSL Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.213)
 */
+ (SigMessageHandle *)lightHSLGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightHSLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light HSL Hue Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.7 Light HSL Hue Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.216)
 */
+ (SigMessageHandle *)lightHSLHueGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightHSLHueStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light HSL Hue Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   hue    The target value of the Light HSL Hue state.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps. (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.8 Light HSL Hue Set and 6.3.3.9 Light HSL Hue Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.216)
 */
+ (SigMessageHandle *)lightHSLHueSetWithDestination:(UInt16)destination hue:(UInt16)hue transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLHueStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light HSL Hue Set without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   hue    The target value of the Light HSL Hue state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.8 Light HSL Hue Set and 6.3.3.9 Light HSL Hue Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.216)
 */
+ (SigMessageHandle *)lightHSLHueSetWithDestination:(UInt16)destination hue:(UInt16)hue retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLHueStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light HSL Saturation Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.11 Light HSL Saturation Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.217)
 */
+ (SigMessageHandle *)lightHSLSaturationGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightHSLSaturationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light HSL Saturation Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   saturation    The target value of the Light HSL Saturation state.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps. (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.12 Light HSL Saturation Set and 6.3.3.13 Light HSL Saturation Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.217)
 */
+ (SigMessageHandle *)lightHSLSaturationSetWithDestination:(UInt16)destination saturation:(UInt16)saturation transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLSaturationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light HSL Saturation Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   saturation    The target value of the Light HSL Saturation state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.12 Light HSL Saturation Set and 6.3.3.13 Light HSL Saturation Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.217)
 */
+ (SigMessageHandle *)lightHSLSaturationSetWithDestination:(UInt16)destination saturation:(UInt16)saturation retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLSaturationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light HSL Saturation Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   HSLLight    The target value of the Light HSL Lightness state.
 * @param   HSLHue    The target value of the Light HSL Hue state.
 * @param   HSLSaturation    The target value of the Light HSL Saturation state.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps. (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.2 Light HSL Set and 6.3.3.3 Light HSL Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.213)
 */
+ (SigMessageHandle *)lightHSLSetWithDestination:(UInt16)destination HSLLight:(UInt16)HSLLight HSLHue:(UInt16)HSLHue HSLSaturation:(UInt16)HSLSaturation transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light HSL Saturation Set without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   HSLLight    The target value of the Light HSL Lightness state.
 * @param   HSLHue    The target value of the Light HSL Hue state.
 * @param   HSLSaturation    The target value of the Light HSL Saturation state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.2 Light HSL Set and 6.3.3.3 Light HSL Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.213)
 */
+ (SigMessageHandle *)lightHSLSetWithDestination:(UInt16)destination HSLLight:(UInt16)HSLLight HSLHue:(UInt16)HSLHue HSLSaturation:(UInt16)HSLSaturation retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light HSL Target Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.5 Light HSL Target Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.215)
 */
+ (SigMessageHandle *)lightHSLTargetGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightHSLTargetStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light HSL Default Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.15 Light HSL Default Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.219)
 */
+ (SigMessageHandle *)lightHSLDefaultGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightHSLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light HSL Default Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.15 Light HSL Default Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.219)
 */
+ (SigMessageHandle *)lightHSLRangeGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightHSLRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light HSL Default Set.
 * @param   destination    the unicastAddress of destination.
 * @param   light    The value of the Light Lightness Default state.
 * @param   hue    The value of the Light HSL Hue Default state.
 * @param   saturation    The value of the Light HSL Saturation Default state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.16 Light HSL Default Set and 6.3.3.17 Light HSL Default Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.219)
 */
+ (SigMessageHandle *)lightHSLDefaultSetWithDestination:(UInt16)destination light:(UInt16)light hue:(UInt16)hue saturation:(UInt16)saturation retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light HSL Range Set.
 * @param   destination    the unicastAddress of destination.
 * @param   hueRangeMin    The value of the Hue Range Min field of the Light HSL Hue Range state.
 * @param   hueRangeMax    The value of the Hue Range Max field of the Light HSL Hue Range state.
 * @param   saturationRangeMin    The value of the Saturation Range Min field of the Light HSL Saturation Range state.
 * @param   saturationRangeMax    The value of the Saturation Range Max field of the Light HSL Saturation Range state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.3.20 Light HSL Range Set and 6.3.3.21 Light HSL Range Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.221)
 */
+ (SigMessageHandle *)lightHSLRangeSetWithDestination:(UInt16)destination hueRangeMin:(UInt16)hueRangeMin hueRangeMax:(UInt16)hueRangeMax saturationRangeMin:(UInt16)saturationRangeMin saturationRangeMax:(UInt16)saturationRangeMax retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark - 6.3.4 Light xyL messages


/**
 * @brief   Light xyL Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.4.1 Light xyL Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.223)
 */
+ (SigMessageHandle *)lightXyLGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightXyLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light xyL Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   xyLLightness    The target value of the Light xyL Lightness state.
 * @param   xyLx    The target value of the Light xyL x state.
 * @param   xyLy    The target value of the Light xyL y state.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps. (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.4.2 Light xyL Set and 6.3.4.3 Light xyL Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.213)
 */
+ (SigMessageHandle *)lightXyLSetWithDestination:(UInt16)destination xyLLightness:(UInt16)xyLLightness xyLx:(UInt16)xyLx xyLy:(UInt16)xyLy transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightXyLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light xyL Set without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   xyLLightness    The target value of the Light xyL Lightness state.
 * @param   xyLx    The target value of the Light xyL x state.
 * @param   xyLy    The target value of the Light xyL y state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.4.2 Light xyL Set and 6.3.4.3 Light xyL Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.213)
 */
+ (SigMessageHandle *)lightXyLSetWithDestination:(UInt16)destination xyLLightness:(UInt16)xyLLightness xyLx:(UInt16)xyLx xyLy:(UInt16)xyLy retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightXyLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light xyL Target Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.4.5 Light xyL Target Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.225)
 */
+ (SigMessageHandle *)lightXyLTargetGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightXyLTargetStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light xyL Default Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.4.7 Light xyL Default Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.226)
 */
+ (SigMessageHandle *)lightXyLDefaultGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightXyLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light xyL Range Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.4.11 Light xyL Range Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.227)
 */
+ (SigMessageHandle *)lightXyLRangeGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightXyLRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light xyL Default Set.
 * @param   destination    the unicastAddress of destination.
 * @param   lightness    The target value of the Light xyL Lightness state.
 * @param   xyLx    The target value of the Light xyL x state.
 * @param   xyLy    The target value of the Light xyL y state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.4.8 Light xyL Default Set and 6.3.4.9 Light xyL Default Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.226)
 */
+ (SigMessageHandle *)lightXyLDefaultSetWithDestination:(UInt16)destination lightness:(UInt16)lightness xyLx:(UInt16)xyLx xyLy:(UInt16)xyLy retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightXyLDefaultStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light xyL Range Set.
 * @param   destination    the unicastAddress of destination.
 * @param   xyLxRangeMin    The value of the xyL x Range Min field of the Light xyL x Range state.
 * @param   xyLxRangeMax    The value of the xyL x Range Max field of the Light xyL x Range state.
 * @param   xyLyRangeMin    The value of the xyL y Range Min field of the Light xyL y Range state.
 * @param   xyLyRangeMax    The value of the xyL y Range Max field of the Light xyL y Range state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.4.12 Light xyL Range Set and 6.3.4.13 Light xyL Range Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.227)
 */
+ (SigMessageHandle *)lightXyLRangeSetWithDestination:(UInt16)destination xyLxRangeMin:(UInt16)xyLxRangeMin xyLxRangeMax:(UInt16)xyLxRangeMax xyLyRangeMin:(UInt16)xyLyRangeMin xyLyRangeMax:(UInt16)xyLyRangeMax retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightXyLRangeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark - 6.3.5 Light LC messages


#pragma mark 6.3.5.1 Light LC Mode messages


/**
 * @brief   Light LC Mode Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.5.1.1 Light LC Mode Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.229)
 */
+ (SigMessageHandle *)lightLCModeGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLCModeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light LC Mode Set.
 * @param   destination    the unicastAddress of destination.
 * @param   enable    The target value of the Light LC Mode state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.5.1.2 Light LC Mode Set and 6.3.5.1.3 Light LC Mode Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.229)
 */
+ (SigMessageHandle *)lightLCModeSetWithDestination:(UInt16)destination enable:(BOOL)enable retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLCModeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark 6.3.5.2 Light LC Occupancy Mode messages


/**
 * @brief   Light LC OM Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.5.2.1 Light LC OM Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.230)
 */
+ (SigMessageHandle *)lightLCOMGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLCOMStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light LC OM Set.
 * @param   destination    the unicastAddress of destination.
 * @param   enable    The target value of the Light LC Occupancy Mode state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.5.2.2 Light LC OM Set and 6.3.5.2.3 Light LC OM Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.230)
 */
+ (SigMessageHandle *)lightLCOMSetWithDestination:(UInt16)destination enable:(BOOL)enable retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLCOMStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark 6.3.5.3 Light LC Light OnOff messages


/**
 * @brief   Light LC Light OnOff Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.5.3.1 Light LC Light OnOff Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.231)
 */
+ (SigMessageHandle *)lightLCLightOnOffGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLCLightOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light LC Light OnOff Set with transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   isOn    The target value of the Light xyL Lightness state.
 * @param   transitionTime    Format as defined in Section 3.1.3. (Optional)
 * @param   delay    Message execution delay in 5 millisecond steps. (C.1)
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.5.3.2 Light LC Light OnOff Set and 6.3.5.3.2 Light LC Light OnOff Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.231)
 */
+ (SigMessageHandle *)lightLCLightOnOffSetWithDestination:(UInt16)destination isOn:(BOOL)isOn transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLCLightOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light LC Light OnOff Set without transitionTime.
 * @param   destination    the unicastAddress of destination.
 * @param   isOn    The target value of the Light xyL Lightness state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.5.3.2 Light LC Light OnOff Set and 6.3.5.3.2 Light LC Light OnOff Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.231)
 */
+ (SigMessageHandle *)lightLCLightOnOffSetWithDestination:(UInt16)destination isOn:(BOOL)isOn retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLCLightOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark - 6.3.6 Light LC Property Messages


/**
 * @brief   Light LC Property Get.
 * @param   destination    the unicastAddress of destination.
 * @param   propertyID    Property ID identifying a Light LC Property.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.6.1 Light LC Property Get, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.232)
 */
+ (SigMessageHandle *)lightLCPropertyGetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLightLCPropertyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Light LC Property Set.
 * @param   destination    the unicastAddress of destination.
 * @param   propertyID    Property ID identifying a Light LC Property.
 * @param   propertyValue    Raw value for the Light LC Property.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   ack    YES means node will response status, NO means node will not response status.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    6.3.6.2 Light LC Property Set and 6.3.6.2 Light LC Property Set Unacknowledged, seeAlso: Mesh_Model_Specification v1.0.pdf  (page.233)
 */
+ (SigMessageHandle *)lightLCPropertySetWithDestination:(UInt16)destination propertyID:(UInt16)propertyID propertyValue:(NSData *)propertyValue retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLCPropertyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark - 6.5 Proxy configuration messages


/**
 * @brief   The Set Filter Type message can be sent by a Proxy Client to change the proxy filter type and clear the proxy filter list.
 * @param   type    the unicastAddress of destination.
 * @param   successCallback    callback when node response the status message.
 * @param   finishCallback    Callback when command sending finish.
 * @note    6.5.1 Set Filter Type, seeAlso: Mesh_v1.0.pdf  (page.263)
 */
+ (void)setType:(SigProxyFilerType)type successCallback:(responseFilterStatusMessageBlock)successCallback finishCallback:(resultBlock)finishCallback;

/**
 * @brief   The Add Addresses to Filter message is sent by a Proxy Client to add destination addresses to the proxy filter list.
 * @param   addresses    List of addresses where N is the number of addresses in this message..
 * @param   successCallback    callback when node response the status message.
 * @param   finishCallback    Callback when command sending finish.
 * @note    6.5.2 Add Addresses to Filter, seeAlso: Mesh_v1.0.pdf  (page.264)
 */
+ (void)addAddressesToFilterWithAddresses:(NSArray <NSNumber *>*)addresses successCallback:(responseFilterStatusMessageBlock)successCallback finishCallback:(resultBlock)finishCallback;

/**
 * @brief   The Remove Addresses from Filter message is sent by a Proxy Client to remove destination addresses from the proxy filter list.
 * @param   addresses    List of addresses where N is the number of addresses in this message..
 * @param   successCallback    callback when node response the status message.
 * @param   finishCallback    Callback when command sending finish.
 * @note    6.5.3 Remove Addresses from Filter, seeAlso: Mesh_v1.0.pdf  (page.264)
 */
+ (void)removeAddressesFromFilterWithAddresses:(NSArray <NSNumber *>*)addresses successCallback:(responseFilterStatusMessageBlock)successCallback finishCallback:(resultBlock)finishCallback;

/**
 * @brief   Adds all the addresses the Provisioner is subscribed to to the Proxy Filter.
 * @param   provisioner    the provisioner that need to set filter
 * @param   successCallback    callback when node response the status message.
 * @param   finishCallback    Callback when command sending finish.
 * @note    This API will auto call setFilterType+AddAddressList, seeAlso: Mesh_v1.0.pdf  (page.263)
 */
+ (void)setFilterForProvisioner:(SigProvisionerModel *)provisioner successCallback:(responseFilterStatusMessageBlock)successCallback finishCallback:(resultBlock)finishCallback;


#pragma mark - API by Telink


/**
 * @brief   Get Online device status by Telink private OnlineStatusCharacteristic.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    Get the online, on/off, brightness, and color temperature status of all devices in the current mesh network (private customization, specific OnlineStatusCharacteristic is required)
 */
+ (nullable NSError *)telinkApiGetOnlineStatueFromUUIDWithResponseMaxCount:(int)responseMaxCount successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Read the value of OTA Charachteristic.
 * @param   timeout    the value of timeout.
 * @param   complete    callback when value of OTA Charachteristic had response or timeout.
 */
+ (void)readOTACharachteristicWithTimeout:(NSTimeInterval)timeout complete:(bleReadOTACharachteristicCallback)complete;

/**
 * @brief   Cancel the timeout action of Read the value of OTA Charachteristic.
 * @note    callback when call stopOTA or trigger otaFailed.
 */
+ (void)cancelReadOTACharachteristic;

/**
 * @brief   Send customs command.
 * @param   model    customs command object.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 */
+ (nullable SigMessageHandle *)sendIniCommandModel:(IniCommandModel *)model successCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Send customs command by Hex data.(Hex data like "a3ff000000000200ffffc21102c4020100".)
 * @param   iniData    customs command Hex data.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 */
+ (nullable SigMessageHandle *)sendOpINIData:(NSData *)iniData successCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/**
 * @brief   Start Telink SigMesh SDK.
 */
+ (void)startMeshSDK;

/**
 * @brief   Returns whether Bluetooth has been initialized.
 * @return  YES means Bluetooth has been initialized, NO means other.
 */
+ (BOOL)isBLEInitFinish;

/**
 * @brief   Send Secure Network Beacon.
 * @note    SDK will auto send Secure Network Beacon or Mesh Private Beacon when app receive ivBeacon from node.
 */
+ (void)sendSecureNetworkBeacon;

/**
 * @brief   Send Mesh Private Beacon.
 * @note    SDK will auto send Secure Network Beacon or Mesh Private Beacon when app receive ivBeacon from node.
 */
+ (void)sendMeshPrivateBeacon;

/**
 * @brief   Update ivIndex by APP.
 * @note    SDK will update ivIndex automatically, when sequenceNumber of APP is more than 0xC00000.
 */
+ (void)updateIvIndexWithKeyRefreshFlag:(BOOL)keyRefreshFlag ivUpdateActive:(BOOL)ivUpdateActive networkId:(NSData *)networkId ivIndex:(UInt32)ivIndex usingNetworkKey:(SigNetkeyModel *)networkKey;

/**
 * @brief   Update the now time to node without response.
 * @note    SDK will call this api automatically, when SDK set filter success.
 */
+ (void)statusNowTime;

/**
 * @brief   Add Single Device (provision+keyBind)
 * @param   configModel all config message of add device.
 * @param   capabilitiesResponse callback when capabilities response.
 * @param   provisionSuccess callback when provision success.
 * @param   provisionFail callback when provision fail.
 * @param   keyBindSuccess callback when keybind success.
 * @param   keyBindFail callback when keybind fail.
 * @note    SDK will call this api automatically, when SDK set filter success.
 * @note    parameter of SigAddConfigModel:
 *  1.normal provision + normal keybind:
 *  peripheral+unicastAddress+networkKey+netkeyIndex+appKey+appkeyIndex+keyBindType:KeyBindType_Normal
 *  2.normal provision + fast keybind:
 *  peripheral+unicastAddress+networkKey+netkeyIndex+appKey+appkeyIndex+keyBindType:KeyBindType_Fast+productID+cpsData
 *  3.static oob provision(cloud oob) + normal keybind:
 *  peripheral+unicastAddress+networkKey+netkeyIndex+appKey+appkeyIndex+staticOOBData+keyBindType:KeyBindType_Normal
 *  4.static oob provision(cloud oob) + fast keybind:
 *  peripheral+unicastAddress+networkKey+netkeyIndex+appKey+appkeyIndex+staticOOBData+keyBindType:KeyBindType_Fast+productID+cpsData
 */
+ (void)startAddDeviceWithSigAddConfigModel:(SigAddConfigModel *)configModel capabilitiesResponse:(addDevice_capabilitiesWithReturnCallBack)capabilitiesResponse provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail;

/**
 * @brief   Provision Method, calculate unicastAddress when capabilities response from unProvision node. (If CBPeripheral's state isn't CBPeripheralStateConnected, SDK will connect CBPeripheral in this api. )
 * @param   peripheral CBPeripheral of CoreBluetooth will be provision.
 * @param   networkKey network key.
 * @param   netkeyIndex netkey index.
 * @param   staticOOBData oob data get from HTTP API when provisionType is ProvisionType_StaticOOB.
 * @param   capabilitiesResponse callback when capabilities response, app need return unicastAddress for provision process.
 * @param   provisionSuccess callback when provision success.
 * @param   fail callback when provision fail.
 * @note    1.get provision type from Capabilities response.
 * 2.Static OOB Type is NO_OOB, SDK will call no oob provision.
 * 3.Static OOB Type is Static_OOB, SDK will call static oob provision.
 * 4.if it is no static oob data when Static OOB Type is Static_OOB, SDK will try on oob provision when SigMeshLib.share.dataSource.addStaticOOBDeviceByNoOOBEnable is YES.
 */
+ (void)startProvisionWithPeripheral:(CBPeripheral *)peripheral networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex staticOOBData:(NSData *)staticOOBData capabilitiesResponse:(addDevice_capabilitiesWithReturnCallBack)capabilitiesResponse provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail;

/**
 * @brief   Keybind Method1. (If CBPeripheral's state isn't CBPeripheralStateConnected, SDK will connect CBPeripheral in this api. )
 * @param   peripheral CBPeripheral of CoreBluetooth will be provision.
 * @param   unicastAddress new unicastAddress of keybind node.
 * @param   appkey appkey
 * @param   appkeyIndex appkeyIndex
 * @param   netkeyIndex netkeyIndex
 * @param   keyBindType KeyBindType_Normal means add appkey and model bind, KeyBindType_Fast means just add appkey.
 * @param   productID the productID info need to save in node when keyBindType is KeyBindType_Fast.
 * @param   cpsData the elements info need to save in node when keyBindType is KeyBindType_Fast.
 * @param   keyBindSuccess callback when keybind success.
 * @param   fail callback when keybind fail.
 */
+ (void)startKeyBindWithPeripheral:(CBPeripheral *)peripheral unicastAddress:(UInt16)unicastAddress appKey:(NSData *)appkey appkeyIndex:(UInt16)appkeyIndex netkeyIndex:(UInt16)netkeyIndex keyBindType:(KeyBindType)keyBindType productID:(UInt16)productID cpsData:(NSData *)cpsData keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess fail:(ErrorBlock)fail;

/**
 * @brief   Keybind Method2. (SDK need to connect mesh in first )
 * @param   address new unicastAddress of keybind node.
 * @param   appkeyModel appkeyModel
 * @param   type KeyBindType_Normal means add appkey and model bind, KeyBindType_Fast means just add appkey.
 * @param   productID the productID info need to save in node when keyBindType is KeyBindType_Fast.
 * @param   cpsData the elements info need to save in node when keyBindType is KeyBindType_Fast.
 * @param   keyBindSuccess callback when keybind success.
 * @param   fail callback when keybind fail.
 */
+ (void)keyBind:(UInt16)address appkeyModel:(SigAppkeyModel *)appkeyModel keyBindType:(KeyBindType)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess fail:(ErrorBlock)fail;

/**
 * @brief   Get Characteristic of peripheral.
 * @param   uuid UUIDString of Characteristic.
 * @param   peripheral the CBPeripheral object.
 * @return  A CBCharacteristic object.
 */
+ (CBCharacteristic *)getCharacteristicWithUUIDString:(NSString *)uuid OfPeripheral:(CBPeripheral *)peripheral;

/**
 * @brief   Set handle of BluetoothCentralUpdateState.
 * @param   bluetoothCentralUpdateStateCallback callback of BluetoothCentralUpdateState.
 */
+ (void)setBluetoothCentralUpdateStateCallback:(_Nullable bleCentralUpdateStateCallback)bluetoothCentralUpdateStateCallback;

/**
 * @brief   Start connecting to the single column mesh network of SigDataSource.
 * @param   complete callback of connect mesh complete.
 * @note    Internally, it will retry every 10 seconds until the connection is successful or the 'stopMeshConnectWithComplete' call is made to stop the connection:`
 */
+ (void)startMeshConnectWithComplete:(nullable startMeshConnectResultBlock)complete;

/**
 * @brief   Disconnect mesh connect.
 * @param   complete callback of connect mesh complete.
 * @note    It will use in switch mesh network.
 */
+ (void)stopMeshConnectWithComplete:(nullable stopMeshConnectResultBlock)complete;


#pragma mark - Scan API


/**
 * @brief   Scan unprovisioned devices.
 * @param   result Report once when a device is scanned.
 */
+ (void)scanUnprovisionedDevicesWithResult:(bleScanPeripheralCallback)result;

/**
 * @brief   Scan provisioned devices.
 * @param   result Report once when a device is scanned.
 */
+ (void)scanProvisionedDevicesWithResult:(bleScanPeripheralCallback)result;

/**
 * @brief   Scan devices with ServiceUUIDs.
 * @param   UUIDs ServiceUUIDs.
 * @param   checkNetworkEnable YES means the device must belong current mesh network.
 * @param   result Report once when a device is scanned.
 */
+ (void)scanWithServiceUUIDs:(NSArray <CBUUID *>* _Nonnull)UUIDs checkNetworkEnable:(BOOL)checkNetworkEnable result:(bleScanPeripheralCallback)result;

/**
 * @brief   Scan devices with specified UUID.
 * @param   peripheralUUID uuid of peripheral.
 * @param   timeout timeout of scan.
 * @param   block Report when the specified device is scanned.
 */
+ (void)scanMeshNodeWithPeripheralUUID:(NSString *)peripheralUUID timeout:(NSTimeInterval)timeout resultBlock:(bleScanSpecialPeripheralCallback)block;

/**
 * @brief   Stop scan action.
 */
+ (void)stopScan;

/**
 * @brief   advertising Manufacturer Data.
 * @param   data Manufacturer Data.
 * @param   interval advertising Interval.
 */
+ (void)advertisingManufacturerData:(NSData *)data advertisingInterval:(NSTimeInterval)interval;

#pragma mark - deprecated API


/**
 * @brief   Deprecated Add Device Method1 (auto add)
 * @param   address UnicastAddress of new device.
 * @param   networkKey network key, which provsion need, you can see it as password of the mesh.
 * @param   netkeyIndex netkey index.
 * @param   appkeyModel appkey model.
 * @param   uuid uuid of remote device.
 * @param   type KeyBindType_Normal是普通添加模式，KeyBindType_Quick是快速添加模式.
 * @param   productID product identify of node.
 * @param   cpsData composition data of node.
 * @param   isAuto 添加完成一个设备后，是否自动扫描添加下一个设备.
 * @param   provisionSuccess callback when provision success.
 * @param   provisionFail callback when provision fail.
 * @param   keyBindSuccess callback when keybind success.
 * @param   keyBindFail callback when keybind fail.
 * @param   finish callback when add device finish.
 */
+ (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel uuid:(nullable NSData *)uuid keyBindType:(KeyBindType)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData isAutoAddNextDevice:(BOOL)isAuto provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail finish:(AddDeviceFinishCallBack)finish DEPRECATED_MSG_ATTRIBUTE("Use 'startAddDeviceWithSigAddConfigModel:capabilitiesResponse:provisionSuccess:provisionFail:keyBindSuccess:keyBindFail:' instead");

/**
 * @brief   Deprecated Add Device Method2 (auto add), add new callback of startConnect and startProvision.
 * @param   address UnicastAddress of new device.
 * @param   networkKey network key, which provsion need, you can see it as password of the mesh.
 * @param   netkeyIndex netkey index.
 * @param   appkeyModel appkey model.
 * @param   uuid uuid of remote device.
 * @param   type KeyBindType_Normal是普通添加模式，KeyBindType_Quick是快速添加模式.
 * @param   productID product identify of node.
 * @param   cpsData composition data of node.
 * @param   isAuto 添加完成一个设备后，是否自动扫描添加下一个设备.
 * @param   startConnect callback when SDK start connect node.
 * @param   startProvision callback when SDK start provision node.
 * @param   provisionSuccess callback when provision success.
 * @param   provisionFail callback when provision fail.
 * @param   keyBindSuccess callback when keybind success.
 * @param   keyBindFail callback when keybind fail.
 * @param   finish callback when add device finish.
 */
+ (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel unicastAddress:(UInt16)unicastAddress uuid:(nullable NSData *)uuid keyBindType:(KeyBindType)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData isAutoAddNextDevice:(BOOL)isAuto startConnect:(addDevice_startConnectCallBack)startConnect startProvision:(addDevice_startProvisionCallBack)startProvision provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail finish:(AddDeviceFinishCallBack)finish DEPRECATED_MSG_ATTRIBUTE("Use 'startAddDeviceWithSigAddConfigModel:capabilitiesResponse:provisionSuccess:provisionFail:keyBindSuccess:keyBindFail:' instead");

/**
 * @brief   Deprecated Add Device Method3 (single add)
 * @param   address UnicastAddress of new device.
 * @param   networkKey network key, which provsion need, you can see it as password of the mesh.
 * @param   netkeyIndex netkey index.
 * @param   appkeyModel appkey model.
 * @param   peripheral the bluetooth Peripheral object of node..
 * @param   staticOOBData oob data get from HTTP API when provisionType is ProvisionType_StaticOOB.
 * @param   type KeyBindType_Normal是普通添加模式，KeyBindType_Quick是快速添加模式.
 * @param   productID product identify of node.
 * @param   cpsData composition data of node.
 * @param   provisionSuccess callback when provision success.
 * @param   provisionFail callback when provision fail.
 * @param   keyBindSuccess callback when keybind success.
 * @param   keyBindFail callback when keybind fail.
 */
+ (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel peripheral:(CBPeripheral *)peripheral staticOOBData:(nullable NSData *)staticOOBData keyBindType:(KeyBindType)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail DEPRECATED_MSG_ATTRIBUTE("Use 'startAddDeviceWithSigAddConfigModel:capabilitiesResponse:provisionSuccess:provisionFail:keyBindSuccess:keyBindFail:' instead");

/**
 * @brief   Deprecated Add Device Method4 (single add), calculate unicastAddress when capabilities response from unProvision node.
 * @param   networkKey network key, which provsion need, you can see it as password of the mesh.
 * @param   netkeyIndex netkey index.
 * @param   appkeyModel appkey model.
 * @param   peripheral the bluetooth Peripheral object of node..
 * @param   staticOOBData oob data get from HTTP API when provisionType is ProvisionType_StaticOOB.
 * @param   type KeyBindType_Normal是普通添加模式，KeyBindType_Quick是快速添加模式.
 * @param   productID product identify of node.
 * @param   cpsData composition data of node.
 * @param   capabilitiesResponse callback when capabilities response from unProvision node.
 * @param   provisionSuccess callback when provision success.
 * @param   provisionFail callback when provision fail.
 * @param   keyBindSuccess callback when keybind success.
 * @param   keyBindFail callback when keybind fail.
 */
+ (void)startAddDeviceWithNetworkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel peripheral:(CBPeripheral *)peripheral staticOOBData:(nullable NSData *)staticOOBData keyBindType:(KeyBindType)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData capabilitiesResponse:(nullable addDevice_capabilitiesWithReturnCallBack)capabilitiesResponse provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail DEPRECATED_MSG_ATTRIBUTE("Use 'startAddDeviceWithSigAddConfigModel:capabilitiesResponse:provisionSuccess:provisionFail:keyBindSuccess:keyBindFail:' instead");

/**
 * @brief   Deprecated Provision Method. (If CBPeripheral's state isn't CBPeripheralStateConnected, SDK will connect CBPeripheral in this api. )
 * @param   peripheral CBPeripheral of CoreBluetooth will be provision.
 * @param   unicastAddress new unicastAddress of provision node.
 * @param   networkKey network key.
 * @param   netkeyIndex netkey index.
 * @param   staticOOBData oob data get from HTTP API when provisionType is ProvisionType_StaticOOB.
 * @param   provisionSuccess callback when provision success.
 * @param   fail callback when provision fail.
 * @note    1.get provision type from Capabilities response.
 * 2.Static OOB Type is NO_OOB, SDK will call no oob provision.
 * 3.Static OOB Type is Static_OOB, SDK will call static oob provision.
 * 4.if it is no static oob data when Static OOB Type is Static_OOB, SDK will try on oob provision when SigMeshLib.share.dataSource.addStaticOOBDeviceByNoOOBEnable is YES.
 */
+ (void)startProvisionWithPeripheral:(CBPeripheral *)peripheral unicastAddress:(UInt16)unicastAddress networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex staticOOBData:(NSData *)staticOOBData provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail DEPRECATED_MSG_ATTRIBUTE("Use 'startProvisionWithPeripheral:networkKey:netkeyIndex:staticOOBData:capabilitiesResponse:provisionSuccess:fail:' instead");

@end

NS_ASSUME_NONNULL_END
