/********************************************************************************************************
 * @file     SigConst.h
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/11/27
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
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

#pragma mark - Const string

UIKIT_EXTERN NSString * const kTelinkSigMeshLibVersion;

UIKIT_EXTERN NSString * const kNotifyCommandIsBusyOrNot;
UIKIT_EXTERN NSString * const kCommandIsBusyKey;

/// Error thrown when the local Provisioner does not have a Unicast Address specified and is not able to send requested message.
UIKIT_EXTERN NSString * const AccessError_invalidSource;
/// Thrown when trying to send a message using an Element that does not belong to the local Provisioner's Node.
UIKIT_EXTERN NSString * const AccessError_invalidElement;
/// Thrown when the given TTL is not valid. Valid TTL must be 0 or in range 2...127.
UIKIT_EXTERN NSString * const AccessError_invalidTtl;
/// Thrown when the destination Address is not known and the library cannot determine the Network Key to use.
UIKIT_EXTERN NSString * const AccessError_invalidDestination;
/// Thrown when trying to send a message from a Model that does not have any Application Key bound to it.
UIKIT_EXTERN NSString * const AccessError_modelNotBoundToAppKey;
/// Error thrown when the Provisioner is trying to delete the last Network Key from the Node.
UIKIT_EXTERN NSString * const AccessError_cannotDelete;
/// Thrown, when the acknowledgment has not been received until the time run out.
UIKIT_EXTERN NSString * const AccessError_timeout;


//service
UIKIT_EXTERN NSString * const kObjectTransferService;
UIKIT_EXTERN NSString * const kPBGATTService;
UIKIT_EXTERN NSString * const kPROXYService;
//SIGCharacteristicsIDs
UIKIT_EXTERN NSString * const kPBGATT_Out_CharacteristicsID;
UIKIT_EXTERN NSString * const kPBGATT_In_CharacteristicsID;
UIKIT_EXTERN NSString * const kPROXY_Out_CharacteristicsID;
UIKIT_EXTERN NSString * const kPROXY_In_CharacteristicsID;
UIKIT_EXTERN NSString * const kOnlineStatusCharacteristicsID;
/// update firmware
UIKIT_EXTERN NSString * const kOTA_CharacteristicsID;
UIKIT_EXTERN NSString * const kMeshOTA_CharacteristicsID;
UIKIT_EXTERN NSString * const kFirmwareRevisionCharacteristicsID;


//存储数据的key
//mesh
UIKIT_EXTERN NSString * const kScanList_key;
UIKIT_EXTERN NSString * const kJsonMeshUUID_key;
UIKIT_EXTERN NSString * const kCurrentProvisionerUUID_key;
UIKIT_EXTERN NSString * const kLocationIvIndexAndSequenceNumberDictionary_key;//缓存当前首页的所有mesh的provisioner与ivIndex+sequenceNumber的映射字典。
UIKIT_EXTERN NSString * const kLocalPrivateBeaconDictionary_key;//缓存private beacon页面的数据。
UIKIT_EXTERN NSString * const kLocalPrivateGattProxy_key;//缓存private beacon页面的数据。
UIKIT_EXTERN NSString * const kLocalPrivateBeacon_key;//缓存private beacon页面的数据。
UIKIT_EXTERN NSString * const kLocalConfigGattProxy_key;//缓存private beacon页面的数据。
UIKIT_EXTERN NSString * const kLocalConfigBeacon_key;//缓存private beacon页面的数据。

//SigScanRspModel
UIKIT_EXTERN NSString * const kSigScanRspModel_uuid_key;
UIKIT_EXTERN NSString * const kSigScanRspModel_address_key;
UIKIT_EXTERN NSString * const kSigScanRspModel_mac_key;
//UIKIT_EXTERN NSString * const kSigScanRspModel_nodeIdentityData_key;
//UIKIT_EXTERN NSString * const kSigScanRspModel_networkIDData_key;
UIKIT_EXTERN NSString * const kSigScanRspModel_advertisementDataServiceData_key;
//meshOTA
UIKIT_EXTERN NSString * const kSaveMeshOTADictKey;
/*存储在本地的数据的key，不再存储在cache中，因为苹果设备的存储快满的时候，系统会删除cache文件夹的数据*/
UIKIT_EXTERN NSString * const kSaveLocationDataKey;//@"mesh.json"
UIKIT_EXTERN NSString * const kExtendBearerMode;//@"kExtendBearerMode"
UIKIT_EXTERN UInt8 const kDLEUnsegmentLength;// 229

//oob
UIKIT_EXTERN NSString * const kSigOOBModel_sourceType_key;
UIKIT_EXTERN NSString * const kSigOOBModel_UUIDString_key;
UIKIT_EXTERN NSString * const kSigOOBModel_OOBString_key;
UIKIT_EXTERN NSString * const kSigOOBModel_lastEditTimeString_key;
UIKIT_EXTERN NSString * const kOOBStoreKey;

UIKIT_EXTERN NSString * const kFeatureString_relay;
UIKIT_EXTERN NSString * const kFeatureString_proxy;
UIKIT_EXTERN NSString * const kFeatureString_friend;
UIKIT_EXTERN NSString * const kFeatureString_lowPower;

/// Generic
UIKIT_EXTERN NSString * const kSigModelGroup_Generic_Describe;
/// Sensors
UIKIT_EXTERN NSString * const kSigModelGroup_Sensors_Describe;
/// Time Server
UIKIT_EXTERN NSString * const kSigModelGroup_TimeServer_Describe;
/// Lighting
UIKIT_EXTERN NSString * const kSigModelGroup_Lighting_Describe;
/// Configuration
UIKIT_EXTERN NSString * const kSigModelGroup_Configuration_Describe;
/// Health
UIKIT_EXTERN NSString * const kSigModelGroup_Health_Describe;
/// Remote Provision
UIKIT_EXTERN NSString * const kSigModelGroup_RemoteProvision_Describe;
/// Firmware Update
UIKIT_EXTERN NSString * const kSigModelGroup_FirmwareUpdate_Describe;
/// Firmware Distribution
UIKIT_EXTERN NSString * const kSigModelGroup_FirmwareDistribution_Describe;
/// BLOB Transfer
UIKIT_EXTERN NSString * const kSigModelGroup_BLOBTransfer_Describe;
/// Directed Forwarding Configuration
UIKIT_EXTERN NSString * const kSigModelGroup_DirectedForwardingConfiguration_Describe;
/// Bridge Configuration
UIKIT_EXTERN NSString * const kSigModelGroup_BridgeConfiguration_Describe;
/// Mesh Private Beacon
UIKIT_EXTERN NSString * const kSigModelGroup_MeshPrivateBeacon_Describe;
/// On-Demand Private Proxy
UIKIT_EXTERN NSString * const kSigModelGroup_OnDemandPrivateProxy_Describe;
/// SAR Configuration
UIKIT_EXTERN NSString * const kSigModelGroup_SARConfiguration_Describe;
/// Opcodes Aggregator
UIKIT_EXTERN NSString * const kSigModelGroup_OpcodesAggregator_Describe;
/// Large Composition Data
UIKIT_EXTERN NSString * const kSigModelGroup_LargeCompositionData_Describe;
/// Solicitation PDU RPL Configuration
UIKIT_EXTERN NSString * const kSigModelGroup_SolicitationPDURPLConfiguration_Describe;
/// Configuration Server
UIKIT_EXTERN NSString * const kSigModel_ConfigurationServer_Describe;
/// Configuration Client
UIKIT_EXTERN NSString * const kSigModel_ConfigurationClient_Describe;
/// Health Server
UIKIT_EXTERN NSString * const kSigModel_HealthServer_Describe;
/// Health Client
UIKIT_EXTERN NSString * const kSigModel_HealthClient_Describe;
/// Remote Provision Server
UIKIT_EXTERN NSString * const kSigModel_RemoteProvisionServer_Describe;
/// Remote Provision Client
UIKIT_EXTERN NSString * const kSigModel_RemoteProvisionClient_Describe;
/// Generic OnOff Server
UIKIT_EXTERN NSString * const kSigModel_GenericOnOffServer_Describe;
/// Generic OnOff Client
UIKIT_EXTERN NSString * const kSigModel_GenericOnOffClient_Describe;
/// Generic Level Server
UIKIT_EXTERN NSString * const kSigModel_GenericLevelServer_Describe;
/// Generic Level Client
UIKIT_EXTERN NSString * const kSigModel_GenericLevelClient_Describe;
/// Generic Default Transition Time Server
UIKIT_EXTERN NSString * const kSigModel_GenericDefaultTransitionTimeServer_Describe;
/// Generic Default Transition Time Client
UIKIT_EXTERN NSString * const kSigModel_GenericDefaultTransitionTimeClient_Describe;
/// Generic Power OnOff Server
UIKIT_EXTERN NSString * const kSigModel_GenericPowerOnOffServer_Describe;
/// Generic Power OnOff Setup Server
UIKIT_EXTERN NSString * const kSigModel_GenericPowerOnOffSetupServer_Describe;
/// Generic Power OnOff Client
UIKIT_EXTERN NSString * const kSigModel_GenericPowerOnOffClient_Describe;
/// Generic Power Level Server
UIKIT_EXTERN NSString * const kSigModel_GenericPowerLevelServer_Describe;
/// Generic Power Level Setup Server
UIKIT_EXTERN NSString * const kSigModel_GenericPowerLevelSetupServer_Describe;
/// Generic Power Level Client
UIKIT_EXTERN NSString * const kSigModel_GenericPowerLevelClient_Describe;
/// Generic Battery Server
UIKIT_EXTERN NSString * const kSigModel_GenericBatteryServer_Describe;
/// Generic Battery Client
UIKIT_EXTERN NSString * const kSigModel_GenericBatteryClient_Describe;
/// Generic Location Server
UIKIT_EXTERN NSString * const kSigModel_GenericLocationServer_Describe;
/// Generic Location Setup Server
UIKIT_EXTERN NSString * const kSigModel_GenericLocationSetupServer_Describe;
/// Generic Location Client
UIKIT_EXTERN NSString * const kSigModel_GenericLocationClient_Describe;
/// Generic Admin Property Server
UIKIT_EXTERN NSString * const kSigModel_GenericAdminPropertyServer_Describe;
/// Generic Manufacturer Property Server
UIKIT_EXTERN NSString * const kSigModel_GenericManufacturerPropertyServer_Describe;
/// Generic User Property Server
UIKIT_EXTERN NSString * const kSigModel_GenericUserPropertyServer_Describe;
/// Generic Client Property Server
UIKIT_EXTERN NSString * const kSigModel_GenericClientPropertyServer_Describe;
/// Generic Property Client
UIKIT_EXTERN NSString * const kSigModel_GenericPropertyClient_Describe;
/// Sensor Server
UIKIT_EXTERN NSString * const kSigModel_SensorServer_Describe;
/// Sensor Setup Server
UIKIT_EXTERN NSString * const kSigModel_SensorSetupServer_Describe;
/// Sensor Client
UIKIT_EXTERN NSString * const kSigModel_SensorClient_Describe;
/// Time Server
UIKIT_EXTERN NSString * const kSigModel_TimeServer_Describe;
/// Time Setup Server
UIKIT_EXTERN NSString * const kSigModel_TimeSetupServer_Describe;
/// Time Client
UIKIT_EXTERN NSString * const kSigModel_TimeClient_Describe;
/// Scene Server
UIKIT_EXTERN NSString * const kSigModel_SceneServer_Describe;
/// Scene Setup Server
UIKIT_EXTERN NSString * const kSigModel_SceneSetupServer_Describe;
/// Scene Client
UIKIT_EXTERN NSString * const kSigModel_SceneClient_Describe;
/// Scheduler
UIKIT_EXTERN NSString * const kSigModel_SchedulerServer_Describe;
/// Scheduler Setup Server
UIKIT_EXTERN NSString * const kSigModel_SchedulerSetupServer_Describe;
/// Scheduler Client
UIKIT_EXTERN NSString * const kSigModel_SchedulerClient_Describe;
/// Light Lightness Server
UIKIT_EXTERN NSString * const kSigModel_LightLightnessServer_Describe;
/// Light Lightness Setup Server
UIKIT_EXTERN NSString * const kSigModel_LightLightnessSetupServer_Describe;
/// Light Lightness Client
UIKIT_EXTERN NSString * const kSigModel_LightLightnessClient_Describe;
/// Light CTL Server
UIKIT_EXTERN NSString * const kSigModel_LightCTLServer_Describe;
/// Light CTL Setup Server
UIKIT_EXTERN NSString * const kSigModel_LightCTLSetupServer_Describe;
/// Light CTL Client
UIKIT_EXTERN NSString * const kSigModel_LightCTLClient_Describe;
/// Light CTL Temperature Server
UIKIT_EXTERN NSString * const kSigModel_LightCTLTemperatureServer_Describe;
/// Light HSL Server
UIKIT_EXTERN NSString * const kSigModel_LightHSLServer_Describe;
/// Light HSL Setup Server
UIKIT_EXTERN NSString * const kSigModel_LightHSLSetupServer_Describe;
/// Light HSL Client
UIKIT_EXTERN NSString * const kSigModel_LightHSLClient_Describe;
/// Light HSL Hue Server
UIKIT_EXTERN NSString * const kSigModel_LightHSLHueServer_Describe;
/// Light HSL Saturation Server
UIKIT_EXTERN NSString * const kSigModel_LightHSLSaturationServer_Describe;
/// Light xyL Server
UIKIT_EXTERN NSString * const kSigModel_LightxyLServer_Describe;
/// Light xyL Setup Server
UIKIT_EXTERN NSString * const kSigModel_LightxyLSetupServer_Describe;
/// Light xyL Client
UIKIT_EXTERN NSString * const kSigModel_LightxyLClient_Describe;
/// Light LC Server
UIKIT_EXTERN NSString * const kSigModel_LightLCServer_Describe;
/// Light LC Setup Server
UIKIT_EXTERN NSString * const kSigModel_LightLCSetupServer_Describe;
/// Light LC Client
UIKIT_EXTERN NSString * const kSigModel_LightLCClient_Describe;
/// Firmware Update Server
UIKIT_EXTERN NSString * const kSigModel_FirmwareUpdateServer_Describe;
/// Firmware Update Client
UIKIT_EXTERN NSString * const kSigModel_FirmwareUpdateClient_Describe;
/// Firmware Distribution Server
UIKIT_EXTERN NSString * const kSigModel_FirmwareDistributionServer_Describe;
/// Firmware Distribution Client
UIKIT_EXTERN NSString * const kSigModel_FirmwareDistributionClient_Describe;
/// BLOB Transfer Server
UIKIT_EXTERN NSString * const kSigModel_BLOBTransferServer_Describe;
/// BLOB Transfer Client
UIKIT_EXTERN NSString * const kSigModel_BLOBTransferClient_Describe;
/// Directed Forwarding Configuration Server
UIKIT_EXTERN NSString * const kSigModel_DirectedForwardingConfigurationServer_Describe;
/// Directed Forwarding Configuration Client
UIKIT_EXTERN NSString * const kSigModel_DirectedForwardingConfigurationClient_Describe;
/// Bridge Configuration Server
UIKIT_EXTERN NSString * const kSigModel_BridgeConfigurationServer_Describe;
/// Bridge Configuration Client
UIKIT_EXTERN NSString * const kSigModel_BridgeConfigurationClient_Describe;
/// Mesh Private Beacon Server
UIKIT_EXTERN NSString * const kSigModel_MeshPrivateBeaconServer_Describe;
/// Mesh Private Beacon Client
UIKIT_EXTERN NSString * const kSigModel_MeshPrivateBeaconClient_Describe;
/// On-Demand Private Proxy Server
UIKIT_EXTERN NSString * const kSigModel_OnDemandPrivateProxyServer_Describe;
/// On-Demand Private Proxy Client
UIKIT_EXTERN NSString * const kSigModel_OnDemandPrivateProxyClient_Describe;
/// SAR Configuration Server
UIKIT_EXTERN NSString * const kSigModel_SARConfigurationServer_Describe;
/// SAR Configuration Client
UIKIT_EXTERN NSString * const kSigModel_SARConfigurationClient_Describe;
/// Opcodes Aggregator Server
UIKIT_EXTERN NSString * const kSigModel_OpcodesAggregatorServer_Describe;
/// Opcodes Aggregator Client
UIKIT_EXTERN NSString * const kSigModel_OpcodesAggregatorClient_Describe;
/// Large Composition Data Server
UIKIT_EXTERN NSString * const kSigModel_LargeCompositionDataServer_Describe;
/// Large Composition Data Client
UIKIT_EXTERN NSString * const kSigModel_LargeCompositionDataClient_Describe;
/// Solicitation PDU RPL Configuration Server
UIKIT_EXTERN NSString * const kSigModel_SolicitationPDURPLConfigurationServer_Describe;
/// Solicitation PDU RPL Configuration Client
UIKIT_EXTERN NSString * const kSigModel_SolicitationPDURPLConfigurationClient_Describe;

#pragma mark - Const bool

/// 标记是否添加未广播蓝牙mac地址的设备，默认不添加
UIKIT_EXTERN BOOL const kAddNotAdvertisementMac;
/// json中是否保存MacAddress，默认保存
UIKIT_EXTERN BOOL const kSaveMacAddressToJson;


#pragma mark - Const int

UIKIT_EXTERN UInt16 const CTL_TEMP_MIN;// 800
UIKIT_EXTERN UInt16 const CTL_TEMP_MAX;// 20000
UIKIT_EXTERN UInt8 const TTL_DEFAULT;// 10, max relay count = TTL_DEFAULT - 1
UIKIT_EXTERN UInt16 const LEVEL_OFF;// -32768
UIKIT_EXTERN UInt16 const LUM_OFF;// 0

#pragma mark - 4.1 Mesh Model Identifiers

/// 4.1.1 Mesh Model Identifiers by Value
/// - seeAlso: Assigned_Numbers.pdf (page.132),

/// Configuration Server
UIKIT_EXTERN UInt16 const kSigModel_ConfigurationServer_ID;//               = 0x0000
/// Configuration Client
UIKIT_EXTERN UInt16 const kSigModel_ConfigurationClient_ID;//               = 0x0001
/// Health Server
UIKIT_EXTERN UInt16 const kSigModel_HealthServer_ID;//                      = 0x0002;
/// Health Client
UIKIT_EXTERN UInt16 const kSigModel_HealthClient_ID;//                      = 0x0003;
/// Remote Provisioning Server
UIKIT_EXTERN UInt16 const kSigModel_RemoteProvisionServer_ID;//             = 0x0004;
/// Remote Provisioning Client
UIKIT_EXTERN UInt16 const kSigModel_RemoteProvisionClient_ID;//             = 0x0005;
/// Directed Forwarding Configuration Server
UIKIT_EXTERN UInt16 const kSigModel_DirectedForwardingConfigurationServer_ID;//= 0x0006;
/// Directed Forwarding Configuration Client
UIKIT_EXTERN UInt16 const kSigModel_DirectedForwardingConfigurationClient_ID;//= 0x0007;
/// Bridge Configuration Server
UIKIT_EXTERN UInt16 const kSigModel_BridgeConfigurationServer_ID;//= 0x0008;
/// Bridge Configuration Client
UIKIT_EXTERN UInt16 const kSigModel_BridgeConfigurationClient_ID;//= 0x0009;
/// Mesh Private Beacon Server
UIKIT_EXTERN UInt16 const kSigModel_MeshPrivateBeaconServer_ID;//= 0x000A;
/// Mesh Private Beacon Client
UIKIT_EXTERN UInt16 const kSigModel_MeshPrivateBeaconClient_ID;//= 0x000B;
/// On-Demand Private Proxy Server
UIKIT_EXTERN UInt16 const kSigModel_OnDemandPrivateProxyServer_ID;//= 0x000C;
/// On-Demand Private Proxy Client
UIKIT_EXTERN UInt16 const kSigModel_OnDemandPrivateProxyClient_ID;//= 0x000D;
/// SAR Configuration Server
UIKIT_EXTERN UInt16 const kSigModel_SARConfigurationServer_ID;//= 0x000E;
/// SAR Configuration Client
UIKIT_EXTERN UInt16 const kSigModel_SARConfigurationClient_ID;//= 0x000F;
/// Opcodes Aggregator Server
UIKIT_EXTERN UInt16 const kSigModel_OpcodesAggregatorServer_ID;//= 0x0010;
/// Opcodes Aggregator Client
UIKIT_EXTERN UInt16 const kSigModel_OpcodesAggregatorClient_ID;//= 0x0011;
/// Large Composition Data Server
UIKIT_EXTERN UInt16 const kSigModel_LargeCompositionDataServer_ID;//= 0x0012;
/// Large Composition Data Client
UIKIT_EXTERN UInt16 const kSigModel_LargeCompositionDataClient_ID;//= 0x0013;
/// Solicitation PDU RPL Configuration Server
UIKIT_EXTERN UInt16 const kSigModel_SolicitationPDURPLConfigurationServer_ID;//= 0x0014;
/// Solicitation PDU RPL Configuration Client
UIKIT_EXTERN UInt16 const kSigModel_SolicitationPDURPLConfigurationClient_ID;//= 0x0015;
/// Generic OnOff Server
UIKIT_EXTERN UInt16 const kSigModel_GenericOnOffServer_ID;//                = 0x1000;
/// Generic OnOff Client
UIKIT_EXTERN UInt16 const kSigModel_GenericOnOffClient_ID;//                = 0x1001;
/// Generic Level Server
UIKIT_EXTERN UInt16 const kSigModel_GenericLevelServer_ID;//                = 0x1002;
/// Generic Level Client
UIKIT_EXTERN UInt16 const kSigModel_GenericLevelClient_ID;//                = 0x1003;
/// Generic Default Transition Time Server
UIKIT_EXTERN UInt16 const kSigModel_GenericDefaultTransitionTimeServer_ID;//= 0x1004;
/// Generic Default Transition Time Client
UIKIT_EXTERN UInt16 const kSigModel_GenericDefaultTransitionTimeClient_ID;//= 0x1005;
/// Generic Power OnOff Server
UIKIT_EXTERN UInt16 const kSigModel_GenericPowerOnOffServer_ID;//           = 0x1006;
/// Generic Power OnOff Setup Server
UIKIT_EXTERN UInt16 const kSigModel_GenericPowerOnOffSetupServer_ID;//      = 0x1007;
/// Generic Power OnOff Client
UIKIT_EXTERN UInt16 const kSigModel_GenericPowerOnOffClient_ID;//           = 0x1008;
/// Generic Power Level Server
UIKIT_EXTERN UInt16 const kSigModel_GenericPowerLevelServer_ID;//           = 0x1009;
/// Generic Power Level Setup Server
UIKIT_EXTERN UInt16 const kSigModel_GenericPowerLevelSetupServer_ID;//      = 0x100A;
/// Generic Power Level Client
UIKIT_EXTERN UInt16 const kSigModel_GenericPowerLevelClient_ID;//           = 0x100B;
/// Generic Battery Server
UIKIT_EXTERN UInt16 const kSigModel_GenericBatteryServer_ID;//              = 0x100C;
/// Generic Battery Client
UIKIT_EXTERN UInt16 const kSigModel_GenericBatteryClient_ID;//              = 0x100D;
/// Generic Location Server
UIKIT_EXTERN UInt16 const kSigModel_GenericLocationServer_ID;//             = 0x100E;
/// Generic Location Setup Server
UIKIT_EXTERN UInt16 const kSigModel_GenericLocationSetupServer_ID;//        = 0x100F;
/// Generic Location Client
UIKIT_EXTERN UInt16 const kSigModel_GenericLocationClient_ID;//             = 0x1010;
/// Generic Admin Property Server
UIKIT_EXTERN UInt16 const kSigModel_GenericAdminPropertyServer_ID;//        = 0x1011;
/// Generic Manufacturer Property Server
UIKIT_EXTERN UInt16 const kSigModel_GenericManufacturerPropertyServer_ID;// = 0x1012;
/// Generic User Property Server
UIKIT_EXTERN UInt16 const kSigModel_GenericUserPropertyServer_ID;//         = 0x1013;
/// Generic Client Property Server
UIKIT_EXTERN UInt16 const kSigModel_GenericClientPropertyServer_ID;//       = 0x1014;
/// Generic Property Client
UIKIT_EXTERN UInt16 const kSigModel_GenericPropertyClient_ID;//             = 0x1015;
/// Sensor Server
UIKIT_EXTERN UInt16 const kSigModel_SensorServer_ID;//                      = 0x1100;
/// Sensor Setup Server
UIKIT_EXTERN UInt16 const kSigModel_SensorSetupServer_ID;//                 = 0x1101;
/// Sensor Client
UIKIT_EXTERN UInt16 const kSigModel_SensorClient_ID;//                      = 0x1102;
/// Time Server
UIKIT_EXTERN UInt16 const kSigModel_TimeServer_ID;//                        = 0x1200;
/// Time Setup Server
UIKIT_EXTERN UInt16 const kSigModel_TimeSetupServer_ID;//                   = 0x1201;
/// Time Client
UIKIT_EXTERN UInt16 const kSigModel_TimeClient_ID;//                        = 0x1202;
/// Scene Server
UIKIT_EXTERN UInt16 const kSigModel_SceneServer_ID;//                       = 0x1203;
/// Scene Setup Server
UIKIT_EXTERN UInt16 const kSigModel_SceneSetupServer_ID;//                  = 0x1204;
/// Scene Client
UIKIT_EXTERN UInt16 const kSigModel_SceneClient_ID;//                       = 0x1205;
/// Scheduler Server
UIKIT_EXTERN UInt16 const kSigModel_SchedulerServer_ID;//                   = 0x1206;
/// Scheduler Setup Server
UIKIT_EXTERN UInt16 const kSigModel_SchedulerSetupServer_ID;//              = 0x1207;
/// Scheduler Client
UIKIT_EXTERN UInt16 const kSigModel_SchedulerClient_ID;//                   = 0x1208;
/// Light Lightness Server
UIKIT_EXTERN UInt16 const kSigModel_LightLightnessServer_ID;//              = 0x1300;
/// Light Lightness Setup Server
UIKIT_EXTERN UInt16 const kSigModel_LightLightnessSetupServer_ID;//         = 0x1301;
/// Light Lightness Client
UIKIT_EXTERN UInt16 const kSigModel_LightLightnessClient_ID;//              = 0x1302;
/// Light CTL Server
UIKIT_EXTERN UInt16 const kSigModel_LightCTLServer_ID;//                    = 0x1303;
/// Light CTL Setup Server
UIKIT_EXTERN UInt16 const kSigModel_LightCTLSetupServer_ID;//               = 0x1304;
/// Light CTL Client
UIKIT_EXTERN UInt16 const kSigModel_LightCTLClient_ID;//                    = 0x1305;
/// Light CTL Temperature Server
UIKIT_EXTERN UInt16 const kSigModel_LightCTLTemperatureServer_ID;//         = 0x1306;
/// Light HSL Server
UIKIT_EXTERN UInt16 const kSigModel_LightHSLServer_ID;//                    = 0x1307;
/// Light HSL Setup Server
UIKIT_EXTERN UInt16 const kSigModel_LightHSLSetupServer_ID;//               = 0x1308;
/// Light HSL Client
UIKIT_EXTERN UInt16 const kSigModel_LightHSLClient_ID;//                    = 0x1309;
/// Light HSL Hue Server
UIKIT_EXTERN UInt16 const kSigModel_LightHSLHueServer_ID;//                 = 0x130A;
/// Light HSL Saturation Server
UIKIT_EXTERN UInt16 const kSigModel_LightHSLSaturationServer_ID;//          = 0x130B;
/// Light xyL Server
UIKIT_EXTERN UInt16 const kSigModel_LightxyLServer_ID;//                    = 0x130C;
/// Light xyL Setup Server
UIKIT_EXTERN UInt16 const kSigModel_LightxyLSetupServer_ID;//               = 0x130D;
/// Light xyL Client
UIKIT_EXTERN UInt16 const kSigModel_LightxyLClient_ID;//                    = 0x130E;
/// Light LC Server
UIKIT_EXTERN UInt16 const kSigModel_LightLCServer_ID;//                     = 0x130F;
/// Light LC Setup Server
UIKIT_EXTERN UInt16 const kSigModel_LightLCSetupServer_ID;//                = 0x1310;
/// Light LC Client
UIKIT_EXTERN UInt16 const kSigModel_LightLCClient_ID;//                     = 0x1311;
/// IEC 62386-104 Model
UIKIT_EXTERN UInt16 const kSigModel_IEC62386_104Model_ID;//= 0x1312;
/// BLOB Transfer Server
UIKIT_EXTERN UInt16 const kSigModel_BLOBTransferServer_ID;//= 0x1400;
/// BLOB Transfer Client
UIKIT_EXTERN UInt16 const kSigModel_BLOBTransferClient_ID;//= 0x1401;
/// Firmware Update Server
UIKIT_EXTERN UInt16 const kSigModel_FirmwareUpdateServer_ID;//= 0x1402;
/// Firmware Update Client
UIKIT_EXTERN UInt16 const kSigModel_FirmwareUpdateClient_ID;//= 0x1403;
/// Firmware Distribution Server
UIKIT_EXTERN UInt16 const kSigModel_FirmwareDistributionServer_ID;//= 0x1404;
/// Firmware Distribution Client
UIKIT_EXTERN UInt16 const kSigModel_FirmwareDistributionClient_ID;//= 0x1405;

UIKIT_EXTERN UInt16 const kMeshAddress_unassignedAddress;// 0x0000
UIKIT_EXTERN UInt16 const kMeshAddress_minUnicastAddress;// 0x0001
UIKIT_EXTERN UInt16 const kMeshAddress_maxUnicastAddress;// 0x7FFF
UIKIT_EXTERN UInt16 const kMeshAddress_minVirtualAddress;// 0x8000
UIKIT_EXTERN UInt16 const kMeshAddress_maxVirtualAddress;// 0xBFFF
UIKIT_EXTERN UInt16 const kMeshAddress_minGroupAddress;// 0xC000
UIKIT_EXTERN UInt16 const kMeshAddress_maxGroupAddress;// 0xFEFF
UIKIT_EXTERN UInt16 const kMeshAddress_allProxies;// 0xFFFC
UIKIT_EXTERN UInt16 const kMeshAddress_allFriends;// 0xFFFD
UIKIT_EXTERN UInt16 const kMeshAddress_allRelays;// 0xFFFE
UIKIT_EXTERN UInt16 const kMeshAddress_allNodes;// 0xFFFF

UIKIT_EXTERN UInt8 const kGetATTListTime;// 5
UIKIT_EXTERN UInt8 const kScanUnprovisionDeviceTimeout;// 10
UIKIT_EXTERN UInt8 const kGetCapabilitiesTimeout;// 5
UIKIT_EXTERN UInt8 const kStartProvisionAndPublicKeyTimeout;// 5
UIKIT_EXTERN UInt8 const kProvisionConfirmationTimeout;// 5
UIKIT_EXTERN UInt8 const kProvisionRandomTimeout;// 5
UIKIT_EXTERN UInt8 const kSentProvisionEncryptedDataWithMicTimeout;// 5
UIKIT_EXTERN UInt8 const kStartMeshConnectTimeout;// 10
UIKIT_EXTERN UInt8 const kProvisioningRecordRequestTimeout;// 10
UIKIT_EXTERN UInt8 const kProvisioningRecordsGetTimeout;// 10

UIKIT_EXTERN UInt8 const kScanNodeIdentityBeforeKeyBindTimeout;// 3

/// publish设置的上报周期
UIKIT_EXTERN UInt8 const kPublishInterval;// 20
/// time model设置的上报周期
UIKIT_EXTERN UInt8 const kTimePublishInterval;// 20
/// 离线检测的时长
UIKIT_EXTERN UInt8 const kOfflineInterval;// = (kPublishInterval * 3 + 1)

/// kCmdReliable_SIGParameters: 1 means send reliable cmd ,and the node will send rsp ,0 means unreliable ,will not send
UIKIT_EXTERN UInt8 const kCmdReliable_SIGParameters;// 1
UIKIT_EXTERN UInt8 const kCmdUnReliable_SIGParameters;// 0

/// Telink默认的企业id
UIKIT_EXTERN UInt16 const kCompanyID;// 0x0211

/// json数据导入本地，本地地址
UIKIT_EXTERN UInt8 const kLocationAddress;// 1
/// json数据生成，生成默认的短地址范围、组地址范围、场景id范围(当前默认一个provisioner，且所有平台使用同一个provisioner)
UIKIT_EXTERN UInt8 const kAllocatedUnicastRangeLowAddress;// 1
UIKIT_EXTERN UInt16 const kAllocatedUnicastRangeHighAddress;// 0x400

UIKIT_EXTERN UInt16 const kAllocatedGroupRangeLowAddress;// 0xC000
UIKIT_EXTERN UInt16 const kAllocatedGroupRangeHighAddress;// 0xC0ff

UIKIT_EXTERN UInt8 const kAllocatedSceneRangeFirstAddress;// 1
UIKIT_EXTERN UInt8 const kAllocatedSceneRangeLastAddress;// 0xf

/// 需要response的指令的默认重试次数，默认为3，客户可修改
UIKIT_EXTERN UInt8 const kAcknowledgeMessageDefaultRetryCount;// 0x3

/*SDK的command list存在需要response的指令，正在等待response或者等待超时。*/
UIKIT_EXTERN UInt32 const kSigMeshLibIsBusyErrorCode;// 0x02110100
UIKIT_EXTERN NSString * const kSigMeshLibIsBusyErrorMessage;// busy

/*当前连接的设备不存在私有特征OnlineStatusCharacteristic*/
UIKIT_EXTERN UInt32 const kSigMeshLibNoFoundOnlineStatusCharacteristicErrorCode;// 0x02110101
UIKIT_EXTERN NSString * const kSigMeshLibNoFoundOnlineStatusCharacteristicErrorMessage;// no found onlineStatus characteristic

/*当前的mesh数据源未创建*/
UIKIT_EXTERN UInt32 const kSigMeshLibNoCreateMeshNetworkErrorCode;// 0x02110102
UIKIT_EXTERN NSString * const kSigMeshLibNoCreateMeshNetworkErrorMessage;// No create mesh

/*当前组号不存在*/
UIKIT_EXTERN UInt32 const kSigMeshLibGroupAddressNoExistErrorCode;// 0x02110103
UIKIT_EXTERN NSString * const kSigMeshLibGroupAddressNoExistErrorMessage;// groupAddress is not exist

/*当前model不存在*/
UIKIT_EXTERN UInt32 const kSigMeshLibModelIDModelNoExistErrorCode;// 0x02110104
UIKIT_EXTERN NSString * const kSigMeshLibModelIDModelNoExistErrorMessage;// modelIDModel is not exist

/*指令超时*/
UIKIT_EXTERN UInt32 const kSigMeshLibCommandTimeoutErrorCode;// 0x02110105
UIKIT_EXTERN NSString * const kSigMeshLibCommandTimeoutErrorMessage;// command is timeout

/*NetKey Index 不存在*/
UIKIT_EXTERN UInt32 const kSigMeshLibCommandInvalidNetKeyIndexErrorCode;// 0x02110106
UIKIT_EXTERN NSString * const kSigMeshLibCommandInvalidNetKeyIndexErrorMessage;// Invalid NetKey Index

/*AppKey Index 不存在*/
UIKIT_EXTERN UInt32 const kSigMeshLibCommandInvalidAppKeyIndexErrorCode;// 0x02110107
UIKIT_EXTERN NSString * const kSigMeshLibCommandInvalidAppKeyIndexErrorMessage;// Invalid AppKey Index

/*Mesh未连接*/
UIKIT_EXTERN UInt32 const kSigMeshLibCommandMeshDisconnectedErrorCode;// 0x02110108
UIKIT_EXTERN NSString * const kSigMeshLibCommandMeshDisconnectedErrorMessage;// Mesh Disconnected

/*telink当前定义的三个设备类型*/
UIKIT_EXTERN UInt16 const SigNodePID_CT;// 1
UIKIT_EXTERN UInt16 const SigNodePID_HSL;// 2
UIKIT_EXTERN UInt16 const SigNodePID_Panel;// 7
UIKIT_EXTERN UInt16 const SigNodePID_LPN;// 0x0201
UIKIT_EXTERN UInt16 const SigNodePID_Switch;// 0x0301

UIKIT_EXTERN float const kCMDInterval;// 0.32
UIKIT_EXTERN float const kSDKLibCommandTimeout;// 1.28

/*读取json里面的mesh数据后，默认新增一个增量128*/
UIKIT_EXTERN UInt32 const kSequenceNumberIncrement;//128

/*初始化json数据时的ivIndex的值*/
UIKIT_EXTERN UInt32 const kDefaultIvIndex;//0x0

/*默认一个unsegmented Access PDU的最大长度，大于该长度则需要进行segment分包，默认值为kUnsegmentedMessageLowerTransportPDUMaxLength（15）*/
UIKIT_EXTERN UInt16 const kUnsegmentedMessageLowerTransportPDUMaxLength;//15

NS_ASSUME_NONNULL_END
