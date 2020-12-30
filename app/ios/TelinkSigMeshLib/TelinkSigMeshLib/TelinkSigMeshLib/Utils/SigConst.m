/********************************************************************************************************
* @file     SigConst.m
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
//  SigConst.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/11/27.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigConst.h"

#pragma mark - Const string

NSString * const kTelinkSigMeshLibVersion = @"v3.3.0";

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

//存储数据的key
//mesh
NSString * const kScanList_key = @"scanList_key";
NSString * const kMatchsList_key = @"matchsList_key";
NSString * const kNoMatchsList_key = @"noMatchsList_key";
NSString * const kJsonMeshUUID_key = @"MeshUUID";
NSString * const kCurrenProvisionerUUID_key = @"currenProvisionerUUID_key";
NSString * const kCurrenProvisionerSno_key = @"currenProvisionerSno_key";

//homes
NSString * const kCurrentMeshProvisionAddress_key = @"currentMeshProvisionAddress_key";
//SigScanRspModel
NSString * const kSigScanRspModel_uuid_key = @"sigScanRspModel_uuid_key";
NSString * const kSigScanRspModel_address_key = @"sigScanRspModel_address_key";
NSString * const kSigScanRspModel_mac_key = @"sigScanRspModel_mac_key";
NSString * const kSigScanRspModel_nodeIdentityData_key = @"sigScanRspModel_nodeIdentityData_key";
NSString * const kSigScanRspModel_networkIDData_key = @"sigScanRspModel_networkIDData_key";
//meshOTA
NSString * const kSaveMeshOTADictKey = @"kSaveMeshOTADictKey";
/*存储在本地的数据的key，不再存储在cache中，以为苹果设备的存储快满的时候，系统会删除cache文件夹的数据*/
NSString * const kSaveLocationDataKey = @"mesh.json";
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

#pragma mark - Const bool

BOOL const kAddNotAdvertisementMac = NO;
BOOL const kSaveMacAddressToJson = YES;


#pragma mark - Const int

UInt16 const CTL_TEMP_MIN = 0x0320;// 800
UInt16 const CTL_TEMP_MAX = 0x4E20;// 20000
UInt8 const TTL_DEFAULT = 10;// max relay count = TTL_DEFAULT - 1
UInt16 const LEVEL_OFF = -32768;
UInt16 const LUM_OFF = 0;

//sig model
UInt16 const SIG_MD_CFG_SERVER = 0x0000;//for test high byte
UInt16 const SIG_MD_CFG_CLIENT             = 0x0001;
UInt16 const SIG_MD_HEALTH_SERVER          = 0x0002;
UInt16 const SIG_MD_HEALTH_CLIENT          = 0x0003;
UInt16 const SIG_MD_REMOTE_PROV_SERVER     = 0x0004;
UInt16 const SIG_MD_REMOTE_PROV_CLIENT     = 0x0005;

UInt16 const SIG_MD_G_ONOFF_S              = 0x1000;
UInt16 const SIG_MD_G_ONOFF_C              = 0x1001;
UInt16 const SIG_MD_G_LEVEL_S              = 0x1002;
UInt16 const SIG_MD_G_LEVEL_C              = 0x1003;
UInt16 const SIG_MD_G_DEF_TRANSIT_TIME_S   = 0x1004;
UInt16 const SIG_MD_G_DEF_TRANSIT_TIME_C   = 0x1005;
UInt16 const SIG_MD_G_POWER_ONOFF_S        = 0x1006;
UInt16 const SIG_MD_G_POWER_ONOFF_SETUP_S  = 0x1007;
UInt16 const SIG_MD_G_POWER_ONOFF_C        = 0x1008;
UInt16 const SIG_MD_G_POWER_LEVEL_S        = 0x1009;
UInt16 const SIG_MD_G_POWER_LEVEL_SETUP_S  = 0x100A;
UInt16 const SIG_MD_G_POWER_LEVEL_C        = 0x100B;
UInt16 const SIG_MD_G_BAT_S                = 0x100C;
UInt16 const SIG_MD_G_BAT_C                = 0x100D;
UInt16 const SIG_MD_G_LOCATION_S           = 0x100E;
UInt16 const SIG_MD_G_LOCATION_SETUP_S     = 0x100F;
UInt16 const SIG_MD_G_LOCATION_C           = 0x1010;
UInt16 const SIG_MD_G_ADMIN_PROP_S         = 0x1011;
UInt16 const SIG_MD_G_MFG_PROP_S           = 0x1012;
UInt16 const SIG_MD_G_USER_PROP_S          = 0x1013;
UInt16 const SIG_MD_G_CLIENT_PROP_S        = 0x1014;
UInt16 const SIG_MD_G_PROP_C               = 0x1015;
// --------
UInt16 const SIG_MD_SENSOR_S               = 0x1100;
UInt16 const SIG_MD_SENSOR_SETUP_S         = 0x1101;
UInt16 const SIG_MD_SENSOR_C               = 0x1102;
// --------
UInt16 const SIG_MD_TIME_S                 = 0x1200;
UInt16 const SIG_MD_TIME_SETUP_S           = 0x1201;
UInt16 const SIG_MD_TIME_C                 = 0x1202;
UInt16 const SIG_MD_SCENE_S                = 0x1203;
UInt16 const SIG_MD_SCENE_SETUP_S          = 0x1204;
UInt16 const SIG_MD_SCENE_C                = 0x1205;
UInt16 const SIG_MD_SCHED_S                = 0x1206;
UInt16 const SIG_MD_SCHED_SETUP_S          = 0x1207;
UInt16 const SIG_MD_SCHED_C                = 0x1208;
// --------
UInt16 const SIG_MD_LIGHTNESS_S            = 0x1300;
UInt16 const SIG_MD_LIGHTNESS_SETUP_S      = 0x1301;
UInt16 const SIG_MD_LIGHTNESS_C            = 0x1302;
UInt16 const SIG_MD_LIGHT_CTL_S            = 0x1303;
UInt16 const SIG_MD_LIGHT_CTL_SETUP_S      = 0x1304;
UInt16 const SIG_MD_LIGHT_CTL_C            = 0x1305;
UInt16 const SIG_MD_LIGHT_CTL_TEMP_S       = 0x1306;
UInt16 const SIG_MD_LIGHT_HSL_S            = 0x1307;
UInt16 const SIG_MD_LIGHT_HSL_SETUP_S      = 0x1308;
UInt16 const SIG_MD_LIGHT_HSL_C            = 0x1309;
UInt16 const SIG_MD_LIGHT_HSL_HUE_S        = 0x130A;
UInt16 const SIG_MD_LIGHT_HSL_SAT_S        = 0x130B;
UInt16 const SIG_MD_LIGHT_XYL_S            = 0x130C;
UInt16 const SIG_MD_LIGHT_XYL_SETUP_S      = 0x130D;
UInt16 const SIG_MD_LIGHT_XYL_C            = 0x130E;
UInt16 const SIG_MD_LIGHT_LC_S             = 0x130F;
UInt16 const SIG_MD_LIGHT_LC_SETUP_S       = 0x1310;
UInt16 const SIG_MD_LIGHT_LC_C             = 0x1311;
// --------
UInt16 const SIG_MD_FW_UPDATE_S            = 0xFE00;
UInt16 const SIG_MD_FW_UPDATE_C            = 0xFE01;
UInt16 const SIG_MD_FW_DISTRIBUT_S         = 0xFE02;
UInt16 const SIG_MD_FW_DISTRIBUT_C         = 0xFE03;
UInt16 const SIG_MD_BLOB_TRANSFER_S         = 0xFF00;
UInt16 const SIG_MD_BLOB_TRANSFER_C         = 0xFF01;

UInt16 const kConfigurationServerModelId = 0x0000;
UInt16 const kConfigurationClientModelId = 0x0001;
UInt16 const kHealthServerModelId = 0x0002;
UInt16 const kHealthClientModelId = 0x0003;

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

UInt8 const kScanNodeIdentityBeforeKeyBindTimeout = 3;

//publish设置的上报周期
UInt8 const kPublishInterval = 20;
//time model设置的上报周期
UInt8 const kTimePublishInterval = 30;
//离线检测的时长
UInt8 const kOfflineInterval = (kPublishInterval * 3 + 1);

//SIGParameters
UInt8 const kSigmodel_SIGParameters = 1;
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
UInt32 const kSigMeshLibNofoundOnlineStatusCharacteristicErrorCode = 0x02110101;
NSString * const kSigMeshLibNofoundOnlineStatusCharacteristicErrorMessage = @"No found, because current device no found onlineStatusCharacteristic.";

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

/*模拟外设的固定服务和特征(分享中使用)*/
NSString * const kServiceUUID = @"33333333-0000-1000-8000-111111111111";
NSString * const kWriteNotiyCharacteristicUUID = @"22222222-0000-1000-8000-111111111111";
/*一个蓝牙json数据包的有效数据长度，测试结果长度为100时，使用时间较短*/
UInt16 const kPacketJsonLength = 100;
UInt16 const kPacketAllLength = kPacketJsonLength+4;

/*telink当前定义的两个设备类型*/
UInt16 const SigNodePID_CT = 1;
UInt16 const SigNodePID_Panel = 7;

float const kCMDInterval = 0.32;
float const kSDKLibCommandTimeout = 1.0;

/*读取json里面的mesh数据后，默认新增一个增量128; snox增加这个增量后存储一次本地json(当前只存储手机本地，无需存储在json)*/
UInt32 const kSnoIncrement = 128;

/*初始化json数据时的ivIndex的值*/
UInt32 const kDefaultIvIndex = 0x0;//0x0

/*默认一个unsegmented Access PDU的最大长度，大于该长度则需要进行segment分包，默认值为kUnsegmentedMessageLowerTransportPDUMaxLength（15）*/
UInt16 const kUnsegmentedMessageLowerTransportPDUMaxLength = 15;//15
