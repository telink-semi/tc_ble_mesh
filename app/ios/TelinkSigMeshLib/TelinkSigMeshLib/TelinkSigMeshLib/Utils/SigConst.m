/********************************************************************************************************
 * @file     SigConst.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/11/27
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

#import "SigConst.h"

#pragma mark - Const string

NSString * const kTelinkSigMeshLibVersion = @"v4.1.0.0";
NSString * const kNotifyCommandIsBusyOrNot = @"CommandIsBusyOrNot";
NSString * const kCommandIsBusyKey = @"IsBusy";
NSString * const AccessError_invalidSource = @"Local Provisioner does not have Unicast Address specified.";
NSString * const AccessError_invalidElement = @"Element does not belong to the local Node.";
NSString * const AccessError_invalidTtl = @"Invalid TTL.";
NSString * const AccessError_invalidDestination = @"The destination address is unknown.";
NSString * const AccessError_modelNotBoundToAppKey = @"No Application Key bound to the given Model.";
NSString * const AccessError_cannotDelete = @"Cannot delete the last Network Key.";
NSString * const AccessError_timeout = @"Request timed out.";

//service
NSString * const kObjectTransferService = @"1825";
NSString * const kPBGATTService = @"1827";
NSString * const kPROXYService = @"1828";
//SIGCharacteristicsIDs
NSString * const kPBGATT_Out_CharacteristicsID = @"2ADC";
NSString * const kPBGATT_In_CharacteristicsID = @"2ADB";
NSString * const kPROXY_Out_CharacteristicsID = @"2ADE";
NSString * const kPROXY_In_CharacteristicsID = @"2ADD";
NSString * const kOnlineStatusCharacteristicsID = @"00010203-0405-0607-0809-0A0B0C0D1A11";
/// update firmware
NSString * const kOTA_CharacteristicsID = @"00010203-0405-0607-0809-0A0B0C0D2B12";
NSString * const kMeshOTA_CharacteristicsID = @"00010203-0405-0607-0809-0A0B0C0D7FDF";
NSString * const kFirmwareRevisionCharacteristicsID = @"2A26";

//存储数据的key
//mesh
NSString * const kScanList_key = @"scanList_key";
NSString * const kJsonMeshUUID_key = @"MeshUUID";
NSString * const kCurrenProvisionerUUID_key = @"currenProvisionerUUID_key";
NSString * const kCurrenProvisionerSno_key = @"currenProvisionerSno_key";
NSString * const kLocationIvIndexAndSequenceNumberDictionary_key = @"locationIvIndexAndSequenceNumberDictionary_key";
NSString * const kLocalPrivateBeaconDictionary_key = @"kLocalPrivateBeaconDictionary_key";//缓存private beacon页面的数据。
NSString * const kLocalPrivateGattProxy_key = @"kLocalPrivateGattProxy_key";//缓存private beacon页面的数据。
NSString * const kLocalPrivateBeacon_key = @"kLocalPrivateBeacon_key";//缓存private beacon页面的数据。
NSString * const kLocalConfigGattProxy_key = @"kLocalConfigGattProxy_key";//缓存private beacon页面的数据。
NSString * const kLocalConfigBeacon_key = @"kLocalConfigBeacon_key";//缓存private beacon页面的数据。

//SigScanRspModel
NSString * const kSigScanRspModel_uuid_key = @"sigScanRspModel_uuid_key";
NSString * const kSigScanRspModel_address_key = @"sigScanRspModel_address_key";
NSString * const kSigScanRspModel_mac_key = @"sigScanRspModel_mac_key";
//NSString * const kSigScanRspModel_nodeIdentityData_key = @"sigScanRspModel_nodeIdentityData_key";
//NSString * const kSigScanRspModel_networkIDData_key = @"sigScanRspModel_networkIDData_key";
NSString * const kSigScanRspModel_advertisementDataServiceData_key = @"sigScanRspModel_advertisementDataServiceData_key";
//meshOTA
NSString * const kSaveMeshOTADictKey = @"kSaveMeshOTADictKey";
/*存储在本地的数据的key，不再存储在cache中，以为苹果设备的存储快满的时候，系统会删除cache文件夹的数据*/
NSString * const kSaveLocationDataKey = @"mesh.json";
NSString * const kExtendBearerMode = @"kExtendBearerMode";
UInt8 const kDLEUnsegmentLength = 229;

//oob
NSString * const kSigOOBModel_sourceType_key = @"kSigOOBModel_sourceType_key";
NSString * const kSigOOBModel_UUIDString_key = @"kSigOOBModel_UUIDString_key";
NSString * const kSigOOBModel_OOBString_key = @"kSigOOBModel_OOBString_key";
NSString * const kSigOOBModel_lastEditTimeString_key = @"kSigOOBModel_lastEditTimeString_key";
NSString * const kOOBStoreKey = @"kOOBStoreKey";

NSString * const kFeatureString_relay = @"relay";
NSString * const kFeatureString_proxy = @"proxy";
NSString * const kFeatureString_friend = @"friend";
NSString * const kFeatureString_lowPower = @"lowPower";

/// Generic
NSString * const kSigModelGroup_Generic_Describe = @"Generic";
/// Sensors
NSString * const kSigModelGroup_Sensors_Describe = @"Sensors";
/// Time Server
NSString * const kSigModelGroup_TimeServer_Describe = @"Time Server";
/// Lighting
NSString * const kSigModelGroup_Lighting_Describe = @"Lighting";
/// Configuration
NSString * const kSigModelGroup_Configuration_Describe = @"Configuration";
/// Health
NSString * const kSigModelGroup_Health_Describe = @"Health";
/// Remote Provision
NSString * const kSigModelGroup_RemoteProvision_Describe = @"Remote Provision";
/// Firmware Update
NSString * const kSigModelGroup_FirmwareUpdate_Describe = @"Firmware Update";
/// Firmware Distribution
NSString * const kSigModelGroup_FirmwareDistribution_Describe = @"Firmware Distribution";
/// BLOB Transfer
NSString * const kSigModelGroup_BLOBTransfer_Describe = @"BLOB Transfer";
/// Directed Forwarding Configuration
NSString * const kSigModelGroup_DirectedForwardingConfiguration_Describe = @"Directed Forwarding Configuration";
/// Bridge Configuration
NSString * const kSigModelGroup_BridgeConfiguration_Describe = @"Bridge Configuration";
/// Mesh Private Beacon
NSString * const kSigModelGroup_MeshPrivateBeacon_Describe = @"Mesh Private Beacon";
/// On-Demand Private Proxy
NSString * const kSigModelGroup_OnDemandPrivateProxy_Describe = @"On-Demand Private Proxy";
/// SAR Configuration
NSString * const kSigModelGroup_SARConfiguration_Describe = @"SAR Configuration";
/// Opcodes Aggregator
NSString * const kSigModelGroup_OpcodesAggregator_Describe = @"Opcodes Aggregator";
/// Large Composition Data
NSString * const kSigModelGroup_LargeCompositionData_Describe = @"Large Composition Data";
/// Solicitation PDU RPL Configuration
NSString * const kSigModelGroup_SolicitationPDURPLConfiguration_Describe = @"Solicitation PDU RPL Configuration";
/// Configuration Server
NSString * const kSigModel_ConfigurationServer_Describe = @"Configuration Server";
/// Configuration Client
NSString * const kSigModel_ConfigurationClient_Describe             = @"Configuration Client";
/// Health Server
NSString * const kSigModel_HealthServer_Describe          = @"Health Server";
/// Health Client
NSString * const kSigModel_HealthClient_Describe          = @"Health Client";
/// Remote Provision Server
NSString * const kSigModel_RemoteProvisionServer_Describe     = @"Remote Provision Server";
/// Remote Provision Client
NSString * const kSigModel_RemoteProvisionClient_Describe     = @"Remote Provision Client";
/// Generic OnOff Server
NSString * const kSigModel_GenericOnOffServer_Describe              = @"Generic OnOff Server";
/// Generic OnOff Client
NSString * const kSigModel_GenericOnOffClient_Describe              = @"Generic OnOff Client";
/// Generic Level Server
NSString * const kSigModel_GenericLevelServer_Describe              = @"Generic Level Server";
/// Generic Level Client
NSString * const kSigModel_GenericLevelClient_Describe              = @"Generic Level Client";
/// Generic Default Transition Time Server
NSString * const kSigModel_GenericDefaultTransitionTimeServer_Describe   = @"Generic Default Transition Time Server";
/// Generic Default Transition Time Client
NSString * const kSigModel_GenericDefaultTransitionTimeClient_Describe   = @"Generic Default Transition Time Client";
/// Generic Power OnOff Server
NSString * const kSigModel_GenericPowerOnOffServer_Describe        = @"Generic Power OnOff Server";
/// Generic Power OnOff Setup Server
NSString * const kSigModel_GenericPowerOnOffSetupServer_Describe  = @"Generic Power OnOff Setup Server";
/// Generic Power OnOff Client
NSString * const kSigModel_GenericPowerOnOffClient_Describe        = @"Generic Power OnOff Client";
/// Generic Power Level Server
NSString * const kSigModel_GenericPowerLevelServer_Describe        = @"Generic Power Level Server";
/// Generic Power Level Setup Server
NSString * const kSigModel_GenericPowerLevelSetupServer_Describe  = @"Generic Power Level Setup Server";
/// Generic Power Level Client
NSString * const kSigModel_GenericPowerLevelClient_Describe        = @"Generic Power Level Client";
/// Generic Battery Server
NSString * const kSigModel_GenericBatteryServer_Describe                = @"Generic Battery Server";
/// Generic Battery Client
NSString * const kSigModel_GenericBatteryClient_Describe                = @"Generic Battery Client";
/// Generic Location Server
NSString * const kSigModel_GenericLocationServer_Describe           = @"Generic Location Server";
/// Generic Location Setup Server
NSString * const kSigModel_GenericLocationSetupServer_Describe     = @"Generic Location Setup Server";
/// Generic Location Client
NSString * const kSigModel_GenericLocationClient_Describe           = @"Generic Location Client";
/// Generic Admin Property Server
NSString * const kSigModel_GenericAdminPropertyServer_Describe         = @"Generic Admin Property Server";
/// Generic Manufacturer Property Server
NSString * const kSigModel_GenericManufacturerPropertyServer_Describe           = @"Generic Manufacturer Property Server";
/// Generic User Property Server
NSString * const kSigModel_GenericUserPropertyServer_Describe          = @"Generic User Property Server";
/// Generic Client Property Server
NSString * const kSigModel_GenericClientPropertyServer_Describe        = @"Generic Client Property Server";
/// Generic Property Client
NSString * const kSigModel_GenericPropertyClient_Describe               = @"Generic Property Client";
/// Sensor Server
NSString * const kSigModel_SensorServer_Describe               = @"Sensor Server";
/// Sensor Setup Server
NSString * const kSigModel_SensorSetupServer_Describe         = @"Sensor Setup Server";
/// Sensor Client
NSString * const kSigModel_SensorClient_Describe               = @"Sensor Client";
/// Time Server
NSString * const kSigModel_TimeServer_Describe                 = @"Time Server";
/// Time Setup Server
NSString * const kSigModel_TimeSetupServer_Describe           = @"Time Setup Server";
/// Time Client
NSString * const kSigModel_TimeClient_Describe                 = @"Time Client";
/// Scene Server
NSString * const kSigModel_SceneServer_Describe                = @"Scene Server";
/// Scene Setup Server
NSString * const kSigModel_SceneSetupServer_Describe          = @"Scene Setup Server";
/// Scene Client
NSString * const kSigModel_SceneClient_Describe                = @"Scene Client";
/// Scheduler
NSString * const kSigModel_SchedulerServer_Describe                = @"Scheduler Server";
/// Scheduler Setup Server
NSString * const kSigModel_SchedulerSetupServer_Describe          = @"Scheduler Setup Server";
/// Scheduler Client
NSString * const kSigModel_SchedulerClient_Describe                = @"Scheduler Client";
/// Light Lightness Server
NSString * const kSigModel_LightLightnessServer_Describe            = @"Light Lightness Server";
/// Light Lightness Setup Server
NSString * const kSigModel_LightLightnessSetupServer_Describe      = @"Light Lightness Setup Server";
/// Light Lightness Client
NSString * const kSigModel_LightLightnessClient_Describe            = @"Light Lightness Client";
/// Light CTL Server
NSString * const kSigModel_LightCTLServer_Describe            = @"Light CTL Server";
/// Light CTL Setup Server
NSString * const kSigModel_LightCTLSetupServer_Describe      = @"Light CTL Setup Server";
/// Light CTL Client
NSString * const kSigModel_LightCTLClient_Describe            = @"Light CTL Client";
/// Light CTL Temperature Server
NSString * const kSigModel_LightCTLTemperatureServer_Describe       = @"Light CTL Temperature Server";
/// Light HSL Server
NSString * const kSigModel_LightHSLServer_Describe            = @"Light HSL Server";
/// Light HSL Setup Server
NSString * const kSigModel_LightHSLSetupServer_Describe      = @"Light HSL Setup Server";
/// Light HSL Client
NSString * const kSigModel_LightHSLClient_Describe            = @"Light HSL Client";
/// Light HSL Hue Server
NSString * const kSigModel_LightHSLHueServer_Describe        = @"Light HSL Hue Server";
/// Light HSL Saturation Server
NSString * const kSigModel_LightHSLSaturationServer_Describe        = @"Light HSL Saturation Server";
/// Light xyL Server
NSString * const kSigModel_LightxyLServer_Describe            = @"Light xyL Server";
/// Light xyL Setup Server
NSString * const kSigModel_LightxyLSetupServer_Describe      = @"Light xyL Setup Server";
/// Light xyL Client
NSString * const kSigModel_LightxyLClient_Describe            = @"Light xyL Client";
/// Light LC Server
NSString * const kSigModel_LightLCServer_Describe             = @"Light LC Server";
/// Light LC Setup Server
NSString * const kSigModel_LightLCSetupServer_Describe       = @"Light LC Setup Server";
/// Light LC Client
NSString * const kSigModel_LightLCClient_Describe             = @"Light LC Client";
/// Firmware Update Server
NSString * const kSigModel_FirmwareUpdateServer_Describe            = @"Firmware Update Server";
/// Firmware Update Client
NSString * const kSigModel_FirmwareUpdateClient_Describe            = @"Firmware Update Client";
/// Firmware Distribution Server
NSString * const kSigModel_FirmwareDistributionServer_Describe         = @"Firmware Distribution Server";
/// Firmware Distribution Client
NSString * const kSigModel_FirmwareDistributionClient_Describe         = @"Firmware Distribution Client";
/// BLOB Transfer Server
NSString * const kSigModel_BLOBTransferServer_Describe         = @"BLOB Transfer Server";
/// BLOB Transfer Client
NSString * const kSigModel_BLOBTransferClient_Describe         = @"BLOB Transfer Client";
/// Directed Forwarding Configuration Server
NSString * const kSigModel_DirectedForwardingConfigurationServer_Describe = @"Directed Forwarding Configuration Server";
/// Directed Forwarding Configuration Client
NSString * const kSigModel_DirectedForwardingConfigurationClient_Describe = @"Directed Forwarding Configuration Client";
/// Bridge Configuration Server
NSString * const kSigModel_BridgeConfigurationServer_Describe = @"Bridge Configuration Server";
/// Bridge Configuration Client
NSString * const kSigModel_BridgeConfigurationClient_Describe = @"Bridge Configuration Client";
/// Mesh Private Beacon Server
NSString * const kSigModel_MeshPrivateBeaconServer_Describe = @"Mesh Private Beacon Server";
/// Mesh Private Beacon Client
NSString * const kSigModel_MeshPrivateBeaconClient_Describe = @"Mesh Private Beacon Client";
/// On-Demand Private Proxy Server
NSString * const kSigModel_OnDemandPrivateProxyServer_Describe = @"On-Demand Private Proxy Server";
/// On-Demand Private Proxy Client
NSString * const kSigModel_OnDemandPrivateProxyClient_Describe = @"On-Demand Private Proxy Client";
/// SAR Configuration Server
NSString * const kSigModel_SARConfigurationServer_Describe = @"SAR Configuration Server";
/// SAR Configuration Client
NSString * const kSigModel_SARConfigurationClient_Describe = @"SAR Configuration Client";
/// Opcodes Aggregator Server
NSString * const kSigModel_OpcodesAggregatorServer_Describe = @"Opcodes Aggregator Server";
/// Opcodes Aggregator Client
NSString * const kSigModel_OpcodesAggregatorClient_Describe = @"Opcodes Aggregator Client";
/// Large Composition Data Server
NSString * const kSigModel_LargeCompositionDataServer_Describe = @"Large Composition Data Server";
/// Large Composition Data Client
NSString * const kSigModel_LargeCompositionDataClient_Describe = @"Large Composition Data Client";
/// Solicitation PDU RPL Configuration Server
NSString * const kSigModel_SolicitationPDURPLConfigurationServer_Describe = @"Solicitation PDU RPL Configuration Server";
/// Solicitation PDU RPL Configuration Client
NSString * const kSigModel_SolicitationPDURPLConfigurationClient_Describe = @"Solicitation PDU RPL Configuration Client";


#pragma mark - Const bool

BOOL const kAddNotAdvertisementMac = NO;
BOOL const kSaveMacAddressToJson = YES;


#pragma mark - Const int

UInt16 const CTL_TEMP_MIN = 0x0320;// 800
UInt16 const CTL_TEMP_MAX = 0x4E20;// 20000
UInt8 const TTL_DEFAULT = 10;// max relay count = TTL_DEFAULT - 1
UInt16 const LEVEL_OFF = -32768;
UInt16 const LUM_OFF = 0;

#pragma mark - 4.1 Mesh Model Identifiers

/// 4.1.1 Mesh Model Identifiers by Value
/// - seeAlso: Assigned_Numbers.pdf (page.132),

/// Configuration Server
UInt16 const kSigModel_ConfigurationServer_ID                = 0x0000;
/// Configuration Client
UInt16 const kSigModel_ConfigurationClient_ID                = 0x0001;
/// Health Server
UInt16 const kSigModel_HealthServer_ID                       = 0x0002;
/// Health Client
UInt16 const kSigModel_HealthClient_ID                       = 0x0003;
/// Remote Provisioning Server
UInt16 const kSigModel_RemoteProvisionServer_ID              = 0x0004;
/// Remote Provisioning Client
UInt16 const kSigModel_RemoteProvisionClient_ID              = 0x0005;
/// Directed Forwarding Configuration Server
UInt16 const kSigModel_DirectedForwardingConfigurationServer_ID              = 0x0006;
/// Directed Forwarding Configuration Client
UInt16 const kSigModel_DirectedForwardingConfigurationClient_ID              = 0x0007;
/// Bridge Configuration Server
UInt16 const kSigModel_BridgeConfigurationServer_ID              = 0x0008;
/// Bridge Configuration Client
UInt16 const kSigModel_BridgeConfigurationClient_ID              = 0x0009;
/// Mesh Private Beacon Server
UInt16 const kSigModel_MeshPrivateBeaconServer_ID              = 0x000A;
/// Mesh Private Beacon Client
UInt16 const kSigModel_MeshPrivateBeaconClient_ID              = 0x000B;
/// On-Demand Private Proxy Server
UInt16 const kSigModel_OnDemandPrivateProxyServer_ID              = 0x000C;
/// On-Demand Private Proxy Client
UInt16 const kSigModel_OnDemandPrivateProxyClient_ID              = 0x000D;
/// SAR Configuration Server
UInt16 const kSigModel_SARConfigurationServer_ID              = 0x000E;
/// SAR Configuration Client
UInt16 const kSigModel_SARConfigurationClient_ID              = 0x000F;
/// Opcodes Aggregator Server
UInt16 const kSigModel_OpcodesAggregatorServer_ID              = 0x0010;
/// Opcodes Aggregator Client
UInt16 const kSigModel_OpcodesAggregatorClient_ID              = 0x0011;
/// Large Composition Data Server
UInt16 const kSigModel_LargeCompositionDataServer_ID              = 0x0012;
/// Large Composition Data Client
UInt16 const kSigModel_LargeCompositionDataClient_ID              = 0x0013;
/// Solicitation PDU RPL Configuration Server
UInt16 const kSigModel_SolicitationPDURPLConfigurationServer_ID              = 0x0014;
/// Solicitation PDU RPL Configuration Client
UInt16 const kSigModel_SolicitationPDURPLConfigurationClient_ID              = 0x0015;
/// Generic OnOff Server
UInt16 const kSigModel_GenericOnOffServer_ID                 = 0x1000;
/// Generic OnOff Client
UInt16 const kSigModel_GenericOnOffClient_ID                 = 0x1001;
/// Generic Level Server
UInt16 const kSigModel_GenericLevelServer_ID                 = 0x1002;
/// Generic Level Client
UInt16 const kSigModel_GenericLevelClient_ID                 = 0x1003;
/// Generic Default Transition Time Server
UInt16 const kSigModel_GenericDefaultTransitionTimeServer_ID = 0x1004;
/// Generic Default Transition Time Client
UInt16 const kSigModel_GenericDefaultTransitionTimeClient_ID = 0x1005;
/// Generic Power OnOff Server
UInt16 const kSigModel_GenericPowerOnOffServer_ID            = 0x1006;
/// Generic Power OnOff Setup Server
UInt16 const kSigModel_GenericPowerOnOffSetupServer_ID       = 0x1007;
/// Generic Power OnOff Client
UInt16 const kSigModel_GenericPowerOnOffClient_ID            = 0x1008;
/// Generic Power Level Server
UInt16 const kSigModel_GenericPowerLevelServer_ID            = 0x1009;
/// Generic Power Level Setup Server
UInt16 const kSigModel_GenericPowerLevelSetupServer_ID       = 0x100A;
/// Generic Power Level Client
UInt16 const kSigModel_GenericPowerLevelClient_ID            = 0x100B;
/// Generic Battery Server
UInt16 const kSigModel_GenericBatteryServer_ID               = 0x100C;
/// Generic Battery Client
UInt16 const kSigModel_GenericBatteryClient_ID               = 0x100D;
/// Generic Location Server
UInt16 const kSigModel_GenericLocationServer_ID              = 0x100E;
/// Generic Location Setup Server
UInt16 const kSigModel_GenericLocationSetupServer_ID         = 0x100F;
/// Generic Location Client
UInt16 const kSigModel_GenericLocationClient_ID              = 0x1010;
/// Generic Admin Property Server
UInt16 const kSigModel_GenericAdminPropertyServer_ID         = 0x1011;
/// Generic Manufacturer Property Server
UInt16 const kSigModel_GenericManufacturerPropertyServer_ID  = 0x1012;
/// Generic User Property Server
UInt16 const kSigModel_GenericUserPropertyServer_ID          = 0x1013;
/// Generic Client Property Server
UInt16 const kSigModel_GenericClientPropertyServer_ID        = 0x1014;
/// Generic Property Client
UInt16 const kSigModel_GenericPropertyClient_ID              = 0x1015;
/// Sensor Server
UInt16 const kSigModel_SensorServer_ID                       = 0x1100;
/// Sensor Setup Server
UInt16 const kSigModel_SensorSetupServer_ID                  = 0x1101;
/// Sensor Client
UInt16 const kSigModel_SensorClient_ID                       = 0x1102;
/// Time Server
UInt16 const kSigModel_TimeServer_ID                         = 0x1200;
/// Time Setup Server
UInt16 const kSigModel_TimeSetupServer_ID                    = 0x1201;
/// Time Client
UInt16 const kSigModel_TimeClient_ID                         = 0x1202;
/// Scene Server
UInt16 const kSigModel_SceneServer_ID                        = 0x1203;
/// Scene Setup Server
UInt16 const kSigModel_SceneSetupServer_ID                   = 0x1204;
/// Scene Client
UInt16 const kSigModel_SceneClient_ID                        = 0x1205;
/// Scheduler Server
UInt16 const kSigModel_SchedulerServer_ID                    = 0x1206;
/// Scheduler Setup Server
UInt16 const kSigModel_SchedulerSetupServer_ID               = 0x1207;
/// Scheduler Client
UInt16 const kSigModel_SchedulerClient_ID                    = 0x1208;
/// Light Lightness Server
UInt16 const kSigModel_LightLightnessServer_ID               = 0x1300;
/// Light Lightness Setup Server
UInt16 const kSigModel_LightLightnessSetupServer_ID          = 0x1301;
/// Light Lightness Client
UInt16 const kSigModel_LightLightnessClient_ID               = 0x1302;
/// Light CTL Server
UInt16 const kSigModel_LightCTLServer_ID                     = 0x1303;
/// Light CTL Setup Server
UInt16 const kSigModel_LightCTLSetupServer_ID                = 0x1304;
/// Light CTL Client
UInt16 const kSigModel_LightCTLClient_ID                     = 0x1305;
/// Light CTL Temperature Server
UInt16 const kSigModel_LightCTLTemperatureServer_ID          = 0x1306;
/// Light HSL Server
UInt16 const kSigModel_LightHSLServer_ID                     = 0x1307;
/// Light HSL Setup Server
UInt16 const kSigModel_LightHSLSetupServer_ID                = 0x1308;
/// Light HSL Client
UInt16 const kSigModel_LightHSLClient_ID                     = 0x1309;
/// Light HSL Hue Server
UInt16 const kSigModel_LightHSLHueServer_ID                  = 0x130A;
/// Light HSL Saturation Server
UInt16 const kSigModel_LightHSLSaturationServer_ID           = 0x130B;
/// Light xyL Server
UInt16 const kSigModel_LightxyLServer_ID                     = 0x130C;
/// Light xyL Setup Server
UInt16 const kSigModel_LightxyLSetupServer_ID                = 0x130D;
/// Light xyL Client
UInt16 const kSigModel_LightxyLClient_ID                     = 0x130E;
/// Light LC Server
UInt16 const kSigModel_LightLCServer_ID                      = 0x130F;
/// Light LC Setup Server
UInt16 const kSigModel_LightLCSetupServer_ID                 = 0x1310;
/// Light LC Client
UInt16 const kSigModel_LightLCClient_ID                      = 0x1311;
/// IEC 62386-104 Model
UInt16 const kSigModel_IEC62386_104Model_ID                      = 0x1312;
/// BLOB Transfer Server
UInt16 const kSigModel_BLOBTransferServer_ID                      = 0x1400;
/// BLOB Transfer Client
UInt16 const kSigModel_BLOBTransferClient_ID                      = 0x1401;
/// Firmware Update Server
UInt16 const kSigModel_FirmwareUpdateServer_ID                      = 0x1402;
/// Firmware Update Client
UInt16 const kSigModel_FirmwareUpdateClient_ID                      = 0x1403;
/// Firmware Distribution Server
UInt16 const kSigModel_FirmwareDistributionServer_ID                      = 0x1404;
/// Firmware Distribution Client
UInt16 const kSigModel_FirmwareDistributionClient_ID                      = 0x1405;

UInt16 const kMeshAddress_unassignedAddress = 0x0000;
UInt16 const kMeshAddress_minUnicastAddress = 0x0001;
UInt16 const kMeshAddress_maxUnicastAddress = 0x7FFF;
UInt16 const kMeshAddress_minVirtualAddress = 0x8000;
UInt16 const kMeshAddress_maxVirtualAddress = 0xBFFF;
UInt16 const kMeshAddress_minGroupAddress   = 0xC000;
UInt16 const kMeshAddress_maxGroupAddress   = 0xFEFF;
UInt16 const kMeshAddress_allProxies        = 0xFFFC;
UInt16 const kMeshAddress_allFriends        = 0xFFFD;
UInt16 const kMeshAddress_allRelays         = 0xFFFE;
UInt16 const kMeshAddress_allNodes          = 0xFFFF;

UInt8 const kGetATTListTime = 5;
UInt8 const kScanUnprovisionDeviceTimeout = 10;
UInt8 const kGetCapabilitiesTimeout = 5;
UInt8 const kStartProvisionAndPublicKeyTimeout = 5;
UInt8 const kProvisionConfirmationTimeout = 5;
UInt8 const kProvisionRandomTimeout = 5;
UInt8 const kSentProvisionEncryptedDataWithMicTimeout = 5;
UInt8 const kStartMeshConnectTimeout = 5;
UInt8 const kProvisioningRecordRequestTimeout = 10;
UInt8 const kProvisioningRecordsGetTimeout = 10;

UInt8 const kScanNodeIdentityBeforeKeyBindTimeout = 3;

//publish设置的上报周期
UInt8 const kPublishInterval = 20;
//time model设置的上报周期
UInt8 const kTimePublishInterval = 30;
//离线检测的时长
UInt8 const kOfflineInterval = (kPublishInterval * 3 + 1);

//kCmdReliable_SIGParameters: 1 means send reliable cmd ,and the node will send rsp ,0 means unreliable ,will not send
UInt8 const kCmdReliable_SIGParameters = 1;
UInt8 const kCmdUnReliable_SIGParameters = 0;

//Telink默认的企业id
UInt16 const kCompanyID = 0x0211;

//json数据导入本地，本地地址
UInt8 const kLocationAddress = 1;
//json数据生成，生成默认的短地址范围、组地址范围、场景id范围(当前默认一个provisioner，且所有平台使用同一个provisioner)
UInt8 const kAllocatedUnicastRangeLowAddress = 1;
UInt16 const kAllocatedUnicastRangeHighAddress = 0x400;//1024

UInt16 const kAllocatedGroupRangeLowAddress = 0xC000;
UInt16 const kAllocatedGroupRangeHighAddress = 0xC0ff;

UInt8 const kAllocatedSceneRangeFirstAddress = 1;
UInt8 const kAllocatedSceneRangeLastAddress = 0xf;

//需要response的指令的默认重试次数，默认为2，客户可修改
UInt8 const kAcknowledgeMessageDefaultRetryCount = 0x2;

/*SDK的command list存在需要response的指令，正在等待response或者等待超时。*/
UInt32 const kSigMeshLibIsBusyErrorCode = 0x02110100;
NSString * const kSigMeshLibIsBusyErrorMessage = @"SDK is busy, because SigMeshLib.share.commands.count isn't empty.";

/*当前连接的设备不存在私有特征OnlineStatusCharacteristic*/
UInt32 const kSigMeshLibNoFoundOnlineStatusCharacteristicErrorCode = 0x02110101;
NSString * const kSigMeshLibNoFoundOnlineStatusCharacteristicErrorMessage = @"No found, because current device no found onlineStatusCharacteristic.";

/*当前的mesh数据源未创建*/
UInt32 const kSigMeshLibNoCreateMeshNetworkErrorCode = 0x02110102;
NSString * const kSigMeshLibNoCreateMeshNetworkErrorMessage = @"No create, because current meshNetwork is nil.";

/*当前组号不存在*/
UInt32 const kSigMeshLibGroupAddressNoExistErrorCode = 0x02110103;
NSString * const kSigMeshLibGroupAddressNoExistErrorMessage = @"No exist, because groupAddress is not exist.";

/*当前model不存在*/
UInt32 const kSigMeshLibModelIDModelNoExistErrorCode = 0x02110104;
NSString * const kSigMeshLibModelIDModelNoExistErrorMessage = @"No exist, because modelIDModel is not exist.";

/*指令超时*/
UInt32 const kSigMeshLibCommandTimeoutErrorCode = 0x02110105;
NSString * const kSigMeshLibCommandTimeoutErrorMessage = @"stop wait response, because command is timeout.";

/*NetKey Index 不存在*/
UInt32 const kSigMeshLibCommandInvalidNetKeyIndexErrorCode = 0x02110106;
NSString * const kSigMeshLibCommandInvalidNetKeyIndexErrorMessage = @"Invalid NetKey Index.";

/*AppKey Index 不存在*/
UInt32 const kSigMeshLibCommandInvalidAppKeyIndexErrorCode = 0x02110107;
NSString * const kSigMeshLibCommandInvalidAppKeyIndexErrorMessage = @"Invalid AppKey Index.";

/*Mesh未连接*/
UInt32 const kSigMeshLibCommandMeshDisconnectedErrorCode = 0x02110108;
NSString * const kSigMeshLibCommandMeshDisconnectedErrorMessage = @"Mesh Disconnected.";

/*telink当前定义的两个设备类型*/
UInt16 const SigNodePID_CT = 1;
UInt16 const SigNodePID_HSL = 2;
UInt16 const SigNodePID_Panel = 7;
UInt16 const SigNodePID_LPN = 0x0201;
UInt16 const SigNodePID_Switch = 0x0301;

float const kCMDInterval = 0.32;
float const kSDKLibCommandTimeout = 1.28;

/*读取json里面的mesh数据后，默认新增一个增量128; SequenceNumber增加这个增量后存储一次本地json(当前只存储手机本地，无需存储在json)*/
UInt32 const kSequenceNumberIncrement = 128;

/*初始化json数据时的ivIndex的值*/
UInt32 const kDefaultIvIndex = 0x0;//0x0

/*默认一个unsegmented Access PDU的最大长度，大于该长度则需要进行segment分包，默认值为kUnsegmentedMessageLowerTransportPDUMaxLength（15）*/
UInt16 const kUnsegmentedMessageLowerTransportPDUMaxLength = 15;//15
