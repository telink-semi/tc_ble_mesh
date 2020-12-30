/********************************************************************************************************
* @file     SigConst.h
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
//  SigConst.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/11/27.
//  Copyright © 2019 Telink. All rights reserved.
//

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
/// Throwm when the given TTL is not valid. Valid TTL must be 0 or in range 2...127.
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


//存储数据的key
//mesh
UIKIT_EXTERN NSString * const kScanList_key;
UIKIT_EXTERN NSString * const kMatchsList_key;
UIKIT_EXTERN NSString * const kNoMatchsList_key;
UIKIT_EXTERN NSString * const kJsonMeshUUID_key;
UIKIT_EXTERN NSString * const kCurrenProvisionerUUID_key;
UIKIT_EXTERN NSString * const kCurrenProvisionerSno_key;
//homes
UIKIT_EXTERN NSString * const kCurrentMeshProvisionAddress_key;
//SigScanRspModel
UIKIT_EXTERN NSString * const kSigScanRspModel_uuid_key;
UIKIT_EXTERN NSString * const kSigScanRspModel_address_key;
UIKIT_EXTERN NSString * const kSigScanRspModel_mac_key;
UIKIT_EXTERN NSString * const kSigScanRspModel_nodeIdentityData_key;
UIKIT_EXTERN NSString * const kSigScanRspModel_networkIDData_key;
//meshOTA
UIKIT_EXTERN NSString * const kSaveMeshOTADictKey;
/*存储在本地的数据的key，不再存储在cache中，因为苹果设备的存储快满的时候，系统会删除cache文件夹的数据*/
UIKIT_EXTERN NSString * const kSaveLocationDataKey;//@"mesh.json"
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

//sig model
UIKIT_EXTERN UInt16 const SIG_MD_CFG_SERVER;// 0x0000
UIKIT_EXTERN UInt16 const SIG_MD_CFG_CLIENT;// 0x0001
UIKIT_EXTERN UInt16 const SIG_MD_HEALTH_SERVER;// 0x0002
UIKIT_EXTERN UInt16 const SIG_MD_HEALTH_CLIENT;// 0x0003
UIKIT_EXTERN UInt16 const SIG_MD_REMOTE_PROV_SERVER;// 0x0004
UIKIT_EXTERN UInt16 const SIG_MD_REMOTE_PROV_CLIENT;// 0x0005

UIKIT_EXTERN UInt16 const SIG_MD_G_ONOFF_S;// 0x1000
UIKIT_EXTERN UInt16 const SIG_MD_G_ONOFF_C;// 0x1001
UIKIT_EXTERN UInt16 const SIG_MD_G_LEVEL_S;// 0x1002
UIKIT_EXTERN UInt16 const SIG_MD_G_LEVEL_C;// 0x1003
UIKIT_EXTERN UInt16 const SIG_MD_G_DEF_TRANSIT_TIME_S;// 0x1004
UIKIT_EXTERN UInt16 const SIG_MD_G_DEF_TRANSIT_TIME_C;// 0x1005
UIKIT_EXTERN UInt16 const SIG_MD_G_POWER_ONOFF_S;// 0x1006
UIKIT_EXTERN UInt16 const SIG_MD_G_POWER_ONOFF_SETUP_S;// 0x1007
UIKIT_EXTERN UInt16 const SIG_MD_G_POWER_ONOFF_C;// 0x1008
UIKIT_EXTERN UInt16 const SIG_MD_G_POWER_LEVEL_S;// 0x1009
UIKIT_EXTERN UInt16 const SIG_MD_G_POWER_LEVEL_SETUP_S;// 0x100A
UIKIT_EXTERN UInt16 const SIG_MD_G_POWER_LEVEL_C;// 0x100B
UIKIT_EXTERN UInt16 const SIG_MD_G_BAT_S;// 0x100C
UIKIT_EXTERN UInt16 const SIG_MD_G_BAT_C;// 0x100D
UIKIT_EXTERN UInt16 const SIG_MD_G_LOCATION_S;// 0x100E
UIKIT_EXTERN UInt16 const SIG_MD_G_LOCATION_SETUP_S;// 0x100F
UIKIT_EXTERN UInt16 const SIG_MD_G_LOCATION_C;// 0x1010
UIKIT_EXTERN UInt16 const SIG_MD_G_ADMIN_PROP_S;// 0x1011
UIKIT_EXTERN UInt16 const SIG_MD_G_MFG_PROP_S;// 0x1012
UIKIT_EXTERN UInt16 const SIG_MD_G_USER_PROP_S;// 0x1013
UIKIT_EXTERN UInt16 const SIG_MD_G_CLIENT_PROP_S;// 0x1014
UIKIT_EXTERN UInt16 const SIG_MD_G_PROP_C;// 0x1015
// --------
UIKIT_EXTERN UInt16 const SIG_MD_SENSOR_S;// 0x1100
UIKIT_EXTERN UInt16 const SIG_MD_SENSOR_SETUP_S;// 0x1101
UIKIT_EXTERN UInt16 const SIG_MD_SENSOR_C;// 0x1102
// --------
UIKIT_EXTERN UInt16 const SIG_MD_TIME_S;// 0x1200
UIKIT_EXTERN UInt16 const SIG_MD_TIME_SETUP_S;// 0x1201
UIKIT_EXTERN UInt16 const SIG_MD_TIME_C;// 0x1202
UIKIT_EXTERN UInt16 const SIG_MD_SCENE_S;// 0x1203
UIKIT_EXTERN UInt16 const SIG_MD_SCENE_SETUP_S;// 0x1204
UIKIT_EXTERN UInt16 const SIG_MD_SCENE_C;// 0x1205
UIKIT_EXTERN UInt16 const SIG_MD_SCHED_S;// 0x1206
UIKIT_EXTERN UInt16 const SIG_MD_SCHED_SETUP_S;// 0x1207
UIKIT_EXTERN UInt16 const SIG_MD_SCHED_C;// 0x1208
// --------
UIKIT_EXTERN UInt16 const SIG_MD_LIGHTNESS_S;// 0x1300
UIKIT_EXTERN UInt16 const SIG_MD_LIGHTNESS_SETUP_S;// 0x1301
UIKIT_EXTERN UInt16 const SIG_MD_LIGHTNESS_C;// 0x1302
UIKIT_EXTERN UInt16 const SIG_MD_LIGHT_CTL_S;// 0x1303
UIKIT_EXTERN UInt16 const SIG_MD_LIGHT_CTL_SETUP_S;// 0x1304
UIKIT_EXTERN UInt16 const SIG_MD_LIGHT_CTL_C;// 0x1305
UIKIT_EXTERN UInt16 const SIG_MD_LIGHT_CTL_TEMP_S;// 0x1306
UIKIT_EXTERN UInt16 const SIG_MD_LIGHT_HSL_S;// 0x1307
UIKIT_EXTERN UInt16 const SIG_MD_LIGHT_HSL_SETUP_S;// 0x1308
UIKIT_EXTERN UInt16 const SIG_MD_LIGHT_HSL_C;// 0x1309
UIKIT_EXTERN UInt16 const SIG_MD_LIGHT_HSL_HUE_S;// 0x130A
UIKIT_EXTERN UInt16 const SIG_MD_LIGHT_HSL_SAT_S;// 0x130B
UIKIT_EXTERN UInt16 const SIG_MD_LIGHT_XYL_S;// 0x130C
UIKIT_EXTERN UInt16 const SIG_MD_LIGHT_XYL_SETUP_S;// 0x130D
UIKIT_EXTERN UInt16 const SIG_MD_LIGHT_XYL_C;// 0x130E
UIKIT_EXTERN UInt16 const SIG_MD_LIGHT_LC_S;// 0x130F
UIKIT_EXTERN UInt16 const SIG_MD_LIGHT_LC_SETUP_S;// 0x1310
UIKIT_EXTERN UInt16 const SIG_MD_LIGHT_LC_C;// 0x1311
// --------
UIKIT_EXTERN UInt16 const SIG_MD_FW_UPDATE_S;// 0xFE00
UIKIT_EXTERN UInt16 const SIG_MD_FW_UPDATE_C;// 0xFE01
UIKIT_EXTERN UInt16 const SIG_MD_FW_DISTRIBUT_S;// 0xFE02
UIKIT_EXTERN UInt16 const SIG_MD_FW_DISTRIBUT_C;// 0xFE03
UIKIT_EXTERN UInt16 const SIG_MD_BLOB_TRANSFER_S;// 0xFF00
UIKIT_EXTERN UInt16 const SIG_MD_BLOB_TRANSFER_C;// 0xFF01


UIKIT_EXTERN UInt16 const kConfigurationServerModelId;// 0x0000
UIKIT_EXTERN UInt16 const kConfigurationClientModelId;// 0x0001
UIKIT_EXTERN UInt16 const kHealthServerModelId;// 0x0002
UIKIT_EXTERN UInt16 const kHealthClientModelId;// 0x0003

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

UIKIT_EXTERN UInt8 const kScanNodeIdentityBeforeKeyBindTimeout;// 3

/// publish设置的上报周期
UIKIT_EXTERN UInt8 const kPublishInterval;// 20
/// time model设置的上报周期
UIKIT_EXTERN UInt8 const kTimePublishInterval;// 20
/// 离线检测的时长
UIKIT_EXTERN UInt8 const kOfflineInterval;// = (kPublishInterval * 3 + 1)

/// SIGParameters
UIKIT_EXTERN UInt8 const kSigmodel_SIGParameters;// 1
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
UIKIT_EXTERN UInt32 const kSigMeshLibNofoundOnlineStatusCharacteristicErrorCode;// 0x02110101
UIKIT_EXTERN NSString * const kSigMeshLibNofoundOnlineStatusCharacteristicErrorMessage;// nofound onlineStatus characteristic

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
UIKIT_EXTERN NSString * const kSigMeshLibCommandTimeoutErrorMessage;// modelIDModel is not exist

/*NetKey Index 不存在*/
UIKIT_EXTERN UInt32 const kSigMeshLibCommandInvalidNetKeyIndexErrorCode;// 0x02110106
UIKIT_EXTERN NSString * const kSigMeshLibCommandInvalidNetKeyIndexErrorMessage;// Invalid NetKey Index

/*AppKey Index 不存在*/
UIKIT_EXTERN UInt32 const kSigMeshLibCommandInvalidAppKeyIndexErrorCode;// 0x02110107
UIKIT_EXTERN NSString * const kSigMeshLibCommandInvalidAppKeyIndexErrorMessage;// Invalid AppKey Index

/*模拟外设的固定服务和特征(分享中使用)*/
UIKIT_EXTERN NSString * const kServiceUUID;// 33333333-0000-1000-8000-111111111111
UIKIT_EXTERN NSString * const kWriteNotiyCharacteristicUUID;// 22222222-0000-1000-8000-111111111111
/*一个蓝牙json数据包的有效数据长度，测试结果长度为100时，使用时间较短*/
UIKIT_EXTERN UInt16 const kPacketJsonLength;// 100
UIKIT_EXTERN UInt16 const kPacketAllLength;// (kPacketJsonLength+4)

/*telink当前定义的两个设备类型*/
UIKIT_EXTERN UInt16 const SigNodePID_CT;// 1
UIKIT_EXTERN UInt16 const SigNodePID_Panel;// 7

UIKIT_EXTERN float const kCMDInterval;// 0.32
UIKIT_EXTERN float const kSDKLibCommandTimeout;// 1.0

/*读取json里面的mesh数据后，默认新增一个增量128*/
UIKIT_EXTERN UInt32 const kSnoIncrement;//128

/*初始化json数据时的ivIndex的值*/
UIKIT_EXTERN UInt32 const kDefaultIvIndex;//0x0

/*默认一个unsegmented Access PDU的最大长度，大于该长度则需要进行segment分包，默认值为kUnsegmentedMessageLowerTransportPDUMaxLength（15）*/
UIKIT_EXTERN UInt16 const kUnsegmentedMessageLowerTransportPDUMaxLength;//15

NS_ASSUME_NONNULL_END
