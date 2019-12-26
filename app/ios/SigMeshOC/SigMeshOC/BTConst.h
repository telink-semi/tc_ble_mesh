/********************************************************************************************************
 * @file     BTConst.h 
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
//  BTConst.h
//  SigMeshOC
//
//  Created by Liangjiazhi on 2018/7/12.
//  Copyright © 2018年 Telink. All rights reserved.
//

#ifndef BTConst_h
#define BTConst_h

#define kTelinkSigMeshLibVersion    @"v3.1.0"

#define kLoopTimeInterval                           (0.002)
#define kLoopWriteForBeaconDelayTime    (0.5)
#define kMeshProxyInitDelayTime               (0.5)
#define kConnectAndReciveATTListTime    (10.0)
#define kConnectWithUUIDTimeout             (10.0)
#define kScanForReKeyBindTimeout             (10.0)
#define kReKeyBindTimeout             (20.0)
#define kScanTimeOutInAddDevice        (10.0)
#define kScanForProvisionDeviceInterval    (6.0)
#define kScanForAutoConnectInterval        (5.0)
#define kScanForHightestRSSICountMax    (3)
#define kCMDInterval                                    (0.32)
#define kShowLogInterval                                    (0.5)
#define kScanForHightestRSSIInterval        (3.0)//正常连接时，为了获取最大的RSSI，扫描的时长
#define kDelayForSetRemoteAddDeviceInfo        (1.0)//remote加灯时，set uuid的延时等待处理时间
#define kScanMacAddressDelay                                    (2.0)//为了等待蓝牙的广播响应包scanRsp添加的延时
#define kSetFilterTimeout                                    (2.0)//连接provision成功的设备，需要添加白名单setFilter

//time model设置的上报周期
#define kTimePublishInterval                                (20)
//publish设置的上报周期
#define kPublishInterval                                (20)
//离线检测的时长
#define kOfflineInterval                                (kPublishInterval * 3 + 1)
//设置设备分组订阅的modelID，可以通过设置SigDataSource.share.defaultGroupSubscriptionModels进行修改
#define kSubscriptionModels @[@(SIG_MD_G_ONOFF_S),@(SIG_MD_LIGHTNESS_S),@(SIG_MD_LIGHT_CTL_S),@(SIG_MD_LIGHT_CTL_TEMP_S),@(SIG_MD_LIGHT_HSL_S)]
//Telink默认的企业id
#define kCompanyID  (0x0211)

//SIGCharacteristicsIDs
#define kPBGATT_Out_CharacteristicsID  @"2ADC"
#define kPBGATT_In_CharacteristicsID   @"2ADB"
#define kPROXY_Out_CharacteristicsID   @"2ADE"
#define kPROXY_In_CharacteristicsID    @"2ADD"
#define kOnlineStatusCharacteristicsID @"00010203-0405-0607-0809-0A0B0C0D1A11"
/// update firmware
#define kOTA_CharacteristicsID    @"00010203-0405-0607-0809-0A0B0C0D2B12"
#define kMeshOTA_CharacteristicsID    @"00010203-0405-0607-0809-0A0B0C0D7FDF"

//SIGServicesIDs
#define kSIGServicesCount    (5)
#define kOTAService  @"00010203-0405-0607-0809-0A0B0C0D1912"
#define kPBGATTService  @"1827"
#define kPROXYService  @"1828"

//SIGParameters
#define kSigmodel_SIGParameters (1)
//kCmdReliable_SIGParameters: 1 means send reliable cmd ,and the node will send rsp ,0 means unreliable ,will not send
#define kCmdReliable_SIGParameters (1)
#define kCmdUnReliable_SIGParameters (0)
#define kAllDo_SIGParameters (0xffff)

#define kSetFilterPacketCount   (3)

//模拟外设的固定服务和特征(分享中使用)
#define kServiceUUID  @"33333333-0000-1000-8000-111111111111"
#define kWriteNotiyCharacteristicUUID  @"22222222-0000-1000-8000-111111111111"
//一个蓝牙json数据包的有效数据长度，测试结果长度为100时，使用时间较短
#define kPacketJsonLength   (100)
#define kPacketAllLength    (kPacketJsonLength+4)
//json数据导入本地，本地地址
#define kLocationAddress    (1)
//json数据生成，生成默认的短地址范围、组地址范围、场景id范围(当前默认一个provisioner，且所有平台使用同一个provisioner)
#define kAllocatedUnicastRangeLowAddress (1)
#define kAllocatedUnicastRangeHighAddress (0x400)//1024

#define kAllocatedGroupRangeLowAddress   (0xC000)
#define kAllocatedGroupRangeHighAddress   (0xC0ff)

#define kAllocatedSceneRangeLowAddress   (1)
#define kAllocatedSceneRangeHighAddress   (0xf)

//存储数据的key
//单灯TBDevice
#define kIsKeyBindSuccess_key @"isKeyBindSuccess_key"
#define kAddress_key @"address_key"
#define kKeyBindModelIDs_key @"keyBindModelIDs_key"
#define kUuid_key @"uuid_key"
#define kMac_key @"mac_key"
#define kGroupIDs_key @"groupIDs_key"
#define kSchedulerList_key @"schedulerList_key"
#define kNodeInfo_key @"nodeInfo_key"
#define kDeviceKey_key @"deviceKey_key"
#define kPublishModels_key @"publishModels_key"
//mesh
#define kLocationData_key @"locationData_key"
#define kLocationAddress_key @"locationAddress_key"
#define kUnicastAddress_key @"unicastAddress_key"
#define kNetworkKey_key @"networkKey_key"
#define kAppkey_key @"appkey_key"
#define kMeshKeyBindModelIDs_key @"meshKeyBindModelIDs_key"
#define kDeviceList_key @"deviceList_key"
#define kGroupList_key @"groupList_key"
#define kSceneList_key @"sceneList_key"
#define kScanList_key @"scanList_key"
#define kMatchsList_key @"matchsList_key"
#define kNoMatchsList_key @"noMatchsList_key"
#define kMeshUUID_key @"MeshUUID_key"
#define kProvisionerUUID_key @"provisionerUUID_key"
#define kJsonMeshUUID_key @"MeshUUID"
#define kJsonRangeLow_key @"lowAddress"
#define kJsonRangeHigh_key @"highAddress"
#define kAllocatedUnicastRange_key @"allocatedUnicastRange"
#define kAllocatedGroupRange_key @"allocatedGroupRange"
#define kAllocatedSceneRange_key @"allocatedSceneRange"
#define kCurrenProvisionerUUID_key @"currenProvisionerUUID_key"
#define kCurrenProvisionerSno_key @"currenProvisionerSno_key"

//homes
#define kCurrentMeshUUID_key @"currentMeshUUID_key"
#define kCurrentMeshProvisionAddress_key @"currentMeshProvisionAddress_key"

//scene
#define kActionList_key @"actionList_key"
#define kSceneAddress_key @"sceneAddress_key"
#define kSceneName_key @"sceneName_key"
//schedulerId
#define kSchedulerIdDict_key @"schedulerIdDict_key"
#define kSchedulerID_key @"schedulerID_key"
#define kSchedulerData_key @"schedulerData_key"
#define kSceneId_key @"sceneId_key"
//SigScanRspModel
#define kSigScanRspModel_uuid_key @"sigScanRspModel_uuid_key"
#define kSigScanRspModel_address_key @"sigScanRspModel_address_key"
#define kSigScanRspModel_mac_key @"sigScanRspModel_mac_key"
#define kSigScanRspModel_CID_key @"sigScanRspModel_CID_key"
#define kSigScanRspModel_PID_key @"sigScanRspModel_PID_key"
#define kSigScanRspModel_nodeIdentityData_key @"sigScanRspModel_nodeIdentityData_key"
#define kSigScanRspModel_networkIDData_key @"sigScanRspModel_networkIDData_key"
//SigEncryptedModel
#define kSigEncryptedModel_identityData_key @"sigEncryptedModel_identityData_key"
#define kSigEncryptedModel_hashData_key @"sigEncryptedModel_hashData_key"
#define kSigEncryptedModel_randomData_key @"sigEncryptedModel_randomData_key"
#define kSigEncryptedModel_peripheralUUID_key @"sigEncryptedModel_peripheralUUID_key"
#define kSigEncryptedModel_encryptedData_key @"sigEncryptedModel_encryptedData_key"
#define kSigEncryptedModel_address_key @"sigEncryptedModel_address_key"

//标记是否添加未广播蓝牙mac地址的设备，默认不添加
//#define kAddNotAdvertisementMac YES
//json中是否保存MacAddress，默认保存
#define kSaveMacAddressToJson YES

//为了静默meshOTA新增通知
#define kOnConnectionDevFirmWareNotify            @"kOnConnectionDevFirmWareNotify"
#define kOnDevChangeNotify                                 @"kOnDevChangeNotify"
#define kOnDevNotifyNotify                                    @"kOnDevNotifyNotify"
/*meshOTA状态存储的字典内容为：{@"meshState":@(2) ,@"address":@(1) ,@"deviceType": @(4)}*/
#define kSaveMeshOTADictKey @"kSaveMeshOTADictKey"

/*存储在本地的数据的key，不再存储在cache中，以为苹果设备的存储快满的时候，系统会删除cache文件夹的数据*/
#define kSaveLocationDataKey @"mesh.json"



//标记是否显示SDK的log
#define kShowSDKLog YES
#ifdef kShowSDKLog
//#define TeLog(fmt, ...) NSLog((@"[文件名:%s]\n" "[函数名:%s]\n" "[行号:%d] \n" fmt), __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define TeLog(fmt, ...) XHelp.share.saveLogObject = [NSString stringWithFormat:(@"[%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__];\
NSLog(@"%@",XHelp.share.saveLogObject);\
saveLogData(XHelp.share.saveLogObject)
#else
#define TeLog(...)
#endif

#define kEndTimer(timer) \
if (timer) { \
[timer invalidate]; \
timer = nil; \
}

#endif /* BTConst_h */
