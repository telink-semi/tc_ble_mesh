/********************************************************************************************************
 * @file     SDKLibCommand.h 
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
//  SDKLibCommand.h
//  SigMeshOC
//
//  Created by Liangjiazhi on 2018/7/12.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "LibHandle.h"
#import "BLECallBackAPI.h"

typedef void(^responseModelCallBack)(ResponseModel *m);// callback nil when command had timeout.
typedef void(^responseVendorModelCallBack)(VendorResponseModel *m);// callback nil when command had timeout.
typedef void(^resultCallBack)(void);
typedef void(^addressCallBack)(NSNumber *address);
typedef void(^remoteProvisioningMaxScannedItemsCallBack)(u8 maxItems,u8 activeScan);
typedef void(^remoteProvisioningScanCallBack)(SigRemoteScanRspModel *scanRemoteModel);
typedef void(^addSingleDeviceSuccessCallBack)(NSData *deviceKey,UInt16 address,UInt16 pid);
typedef void(^discoverDeviceBlock)(CBPeripheral *peripheral,SigScanRspModel *model,BOOL provisionAble);

@interface SDKLibCommand : NSObject

@property (nonatomic,strong) NSTimer *busyTimer;

#pragma mark - Save call back

//callback all response data
@property (nonatomic,copy) responseModelCallBack responseAllDataCallBack;
//turn on / off group or device
@property (nonatomic,copy) responseModelCallBack switchOnOffCallBack;
//change brightness
@property (nonatomic,copy) responseModelCallBack changeBrightnessCallBack;
//change temperature
@property (nonatomic,copy) responseModelCallBack changeTemperatureCallBack;
//change HSL
@property (nonatomic,copy) responseModelCallBack changeHSLCallBack;
//change level
@property (nonatomic,copy) responseModelCallBack changeLevelCallBack;

//Subscribe groups list , which mean you can control in the group if your device subscribe the group(address)
//GET
@property (nonatomic,copy) responseModelCallBack getSubscribeListCallBack;
//EDIT Sub
@property (nonatomic,copy) responseModelCallBack editSubscribeListCallBack;

//EDIT Pub
@property (nonatomic,copy) responseModelCallBack publishListResponseCallBack;

//Get device current mesh online
@property (nonatomic,copy) responseModelCallBack getOnlineStatusCallBack;
@property (nonatomic,copy) responseModelCallBack notifyOnlineStatusCallBack;
@property (nonatomic,copy) responseModelCallBack notifyPublishStatusCallBack;
@property (nonatomic,copy) addressCallBack checkOfflineCallBack;

//response of vendorID
@property (nonatomic,copy) responseVendorModelCallBack responseVendorIDCallBack;

//response of delect device
@property (nonatomic,copy) resultCallBack delectDeviceCallBack;
//response of setTime
@property (nonatomic,copy) resultCallBack setTimeCallBack;
//save scene
@property (nonatomic,copy) responseModelCallBack saveSceneCallBack;
//recall scene
@property (nonatomic,copy) responseModelCallBack recallSceneCallBack;
//del scene
@property (nonatomic,copy) responseModelCallBack delSceneCallBack;
//get scene register status
@property (nonatomic,copy) responseModelCallBack getSceneRegisterStatusCallBack;
//set Scheduler Action
@property (nonatomic,copy) responseModelCallBack setSchedulerActionCallBack;
//get Scheduler status
@property (nonatomic,copy) responseModelCallBack getSchedulerStatusCallBack;
//get Scheduler Action
@property (nonatomic,copy) responseModelCallBack getSchedulerActionCallBack;

//get max scan items number callback
@property (nonatomic,copy) remoteProvisioningMaxScannedItemsCallBack maxScannedItemsCallBack;
//get remote UUID callback
@property (nonatomic,copy) remoteProvisioningScanCallBack remoteScanRspCallBack;
//setting UUID result callback
@property (nonatomic,copy) responseModelCallBack setUUIDResultCallBack;
//get fw info
@property (nonatomic,copy) responseModelCallBack getFwInfoCallBack;
//fast provision add single device success callback
@property (nonatomic,copy) addSingleDeviceSuccessCallBack fastProvisionAddSingleDeviceSuccessCallBack;
//fast provision add device finish callback
@property (nonatomic,copy) prvisionFinishCallBack fastProvisionAddDeviceFinishCallBack;
@property (nonatomic,assign) BOOL isSingleProvision;
@property (nonatomic,copy) addDevice_prvisionSuccessCallBack singlePrvisionSuccessCallBack;
@property (nonatomic,copy) addDevice_keyBindSuccessCallBack singleKeyBindSuccessCallBack;
@property (nonatomic,copy) ErrorBlock singlePrvisionFailCallBack;
@property (nonatomic,copy) ErrorBlock singleKeyBindFailCallBack;
@property (nonatomic,strong) NSData *staticOOBData;
@property (nonatomic,assign) UInt16 fastKeybindProductID;
@property (nonatomic,strong) NSData *fastKeybindCpsData;

/// 启动meshSDK
+ (void)startMeshSDK;

/// Whether or not SDK is waiting response data.
- (BOOL)isBusy;

/// function :add bluetooth devices
- (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex unicastAddress:(UInt16)unicastAddress uuid:(NSData *)uuid keyBindType:(KeyBindTpye)type isAutoAddNextDevice:(BOOL)isAuto provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess fail:(prvisionFailCallBack)fail finish:(prvisionFinishCallBack)finish;

/// function :add bluetooth devices
- (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex peripheral:(CBPeripheral *)peripheral keyBindType:(KeyBindTpye)type provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess fail:(prvisionFailCallBack)fail finish:(prvisionFinishCallBack)finish;

/// get max scan items count
- (void)getRemoteProvisioningMaxScannedItemsWithCallback:(remoteProvisioningMaxScannedItemsCallBack)callback;

/// get uuid of remote
- (void)startRemoteProvisioningScanWithAddress:(UInt16)address limit:(UInt8)limit timeout:(NSTimeInterval)timeout callback:(remoteProvisioningScanCallBack)callback;

///before remote add，set remote add device info
- (void)setRemoteAddDeviceInfoWithUnicastAddress:(UInt16)unicastAddress uuid:(NSData *)uuid complete:(responseModelCallBack)complete;

/// function :add bluetooth devices by fast provision and fast keybind
- (void)startFastProvisionAddDevicesWithPID:(UInt16)pid addSingleDeviceSuccessCallback:(addSingleDeviceSuccessCallBack)singleSuccess finish:(prvisionFinishCallBack)finish;

/////function :connect bluetooth device, old API befor 2.9.0
- (void)startWorkNormal:(NSArray <NSString *>*)availableList CanControl:(workWithPeripheralCallBack)canControl NS_DEPRECATED_IOS(2_0, 9_0, "Use -startWorkNormalWithComplete:") __TVOS_PROHIBITED;

///function :connect bluetooth device, new API after 3.0.0
- (void)startWorkNormalWithComplete:(workWithPeripheralCallBack)complete;

///function :turn on or turn off the lights part
- (void)switchOnOffWithExecuteCommand:(BOOL)executeCommand on:(BOOL)on address:(UInt16)address resMax:(int)resMax ack:(BOOL)ack Completation:(responseModelCallBack)complete;

///change brightness call back
- (void)changeBrightnessWithExecuteCommand:(BOOL)executeCommand address:(UInt16)address para:(float)para isGet:(BOOL)isGet respondMax:(int)respondMax ack:(BOOL)ack Completation:(responseModelCallBack)complete;

/// change temprature call back
- (void)changeTempratureWithExecuteCommand:(BOOL)executeCommand address:(UInt16)address para:(float)para isGet:(BOOL)isGet respondMax:(int)respondMax ack:(BOOL)ack Completation:(responseModelCallBack)complete;

/// change HSL call back
- (void)changeHSLWithExecuteCommand:(BOOL)executeCommand address:(UInt16)address hue:(UInt8)hue saturation:(UInt8)saturation brightness:(UInt8)brightness isGet:(BOOL)isGet respondMax:(int)respondMax ack:(BOOL)ack Completation:(responseModelCallBack)complete;

/// change level call back
- (void)changeLevelWithExecuteCommand:(BOOL)executeCommand address:(UInt16)address level:(s16)level isGet:(BOOL)isGet respondMax:(int)respondMax ack:(BOOL)ack Completation:(responseModelCallBack)complete;

/// get subscribe list（option is onoff sig modelID）
- (void)getSubscribeList:(BOOL)executeCommand address:(UInt16)address isClient:(BOOL)isClient complete:(responseModelCallBack)complete;

///change subscribe list（develop can see mesh_node.h line129 to get more detail of option）
- (void)editSubscribeListWithExecuteCommand:(BOOL)executeCommand groAdd:(UInt16)groAdd nodeAddress:(UInt16)nodeAddress eleAddress:(UInt16)eleAddress add:(BOOL)add option:(UInt32)option Completation:(responseModelCallBack)complete;

///change publish list
- (void)editPublishListWithExecuteCommand:(BOOL)executeCommand publishAddress:(UInt16)pub_adr nodeAddress:(UInt16)nodeAddress eleAddress:(UInt16)eleAddress option:(UInt32)option period:(mesh_pub_period_t)period Completation:(responseModelCallBack)complete;

///get Publish Address
- (void)getPublishAddressWithExecuteCommand:(BOOL)executeCommand nodeAddress:(UInt16)nodeAddress eleAddress:(UInt16)eleAddress option:(UInt32)option Completation:(responseModelCallBack)complete;

/// Get Online device, publish use access_cmd_onoff_get()
- (BOOL)getOnlineStatusWithExecuteCommand:(BOOL)executeCommand reqCount:(int)reqCount Completation:(responseModelCallBack)complete;

/// Get Online device, private use OnlineStatusCharacteristic
- (BOOL)getOnlineStatusFromUUIDWithCompletation:(responseModelCallBack)complete;

///Get lum
- (BOOL)getLumWithExecuteCommand:(BOOL)executeCommand nodeAddress:(UInt16)nodeAddress reqCount:(int)reqCount Completation:(responseModelCallBack)complete;

/// Get CTL, (Lum and Temprature)
- (BOOL)getCTLWithExecuteCommand:(BOOL)executeCommand nodeAddress:(UInt16)nodeAddress reqCount:(int)reqCount Completation:(responseModelCallBack)complete;

///Get provision Status(已经废弃)
//- (void)provisionStatus:(resultCallBack)success fail:(resultCallBack)fail;

///Kick out
- (void)kickoutDevice:(UInt16)address complete:(resultCallBack)complete;

///provisionLocation
///- before you do provision, you should do local provision first, if you did it, and you need't do it again
- (void)provisionLocation:(NSData *)netkeydata withLocationAddress:(int)locationAddress netketIndex:(u16)netketIndex;

///provision
- (void)provisionMesh:(NSData *)netkeydata address:(UInt16)address netkeyIndex:(u16)netkeyIndex;

///sendOTAData
- (void)sendOTAData:(NSData *)data index:(int)index;

- (void)keyBind:(u16)address appkey:(NSData *)appkey appkeyIndex:(u16)appkeyIndex netkeyIndex:(u16)netkeyIndex keyBindType:(KeyBindTpye)type keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess fail:(prvisionFailCallBack)fail;

///refresh modelID
- (void)refreshModelIDList;

///sendCommandData
- (int)sendCommandData:(UInt16)address rsp_max:(UInt8)rsp_max opcode:(UInt16)opcode data:(NSData *)data;

/// setNowTime
- (void)setNowTimeWithComplete:(resultCallBack)complete;
/// status NowTime, without response
- (void)statusNowTime;

/// save scene
- (void)saveSceneWithAddress:(u16)address resMax:(u32)resMax sceneId:(u16)sceneId Completation:(responseModelCallBack)complete;

/// recall scene
- (void)recallSceneWithAddress:(u16)address resMax:(u32)resMax sceneId:(u16)sceneId Completation:(responseModelCallBack)complete;

/// dellete scene
- (void)delSceneWithAddress:(u16)address resMax:(u32)resMax sceneId:(u16)sceneId Completation:(responseModelCallBack)complete;

/// get scene register status
- (void)getSceneRegisterStatusWithAddress:(u16)address resMax:(u32)resMax Completation:(responseModelCallBack)complete;

/// set scheduler action
- (void)setSchedulerActionWithAddress:(u16)address resMax:(u32)resMax schedulerModel:(SchedulerModel *)schedulerModel Completation:(responseModelCallBack)complete;

/// get scheduler status
- (void)getSchedulerStatusWithAddress:(u16)address resMax:(u32)resMax Completation:(responseModelCallBack)complete;

/// get scheduler action
- (void)getSchedulerActionWithAddress:(u16)address resMax:(u32)resMax schedulerModelID:(u8)schedulerModelID Completation:(responseModelCallBack)complete;

/// get fw info
- (void)getFwInfoWithAddress:(u16)address Completation:(responseModelCallBack)complete;


#pragma mark - new api since v3.2.0


/// start scan with timeout
/// @param provisionAble YES means scan device for provision, NO means scan device for normal/keyBind/OTA.
/// @param timeout timeout can't be 0.
/// @param discoverDevice callback when SDK discover device.
/// @param finish callback when timeout reached.
- (void)startScanWithProvisionAble:(BOOL)provisionAble timeout:(NSTimeInterval)timeout discoverDevice:(discoverDeviceBlock)discoverDevice finish:(resultCallBack)finish;


/// stop discover device, cancel timeout, set block of scan api to be nil.
- (void)stopScan;


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
/// Add Device (provision+keyBind)
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


/// provision (add callback of start send provision data)
/// @param peripheral CBPeripheral of CoreBluetooth will be provision.
/// @param unicastAddress address of new device.
/// @param networkKey networkKey
/// @param netkeyIndex netkeyIndex
/// @param provisionType ProvisionTpye_NoOOB means oob data is 16 bytes zero data, ProvisionTpye_StaticOOB means oob data is get from HTTP API.
/// @param staticOOBData oob data get from HTTP API when provisionType is ProvisionTpye_StaticOOB.
/// @param startSendProvision callback when provision packet begin send.
/// @param provisionSuccess callback when provision success.
/// @param fail callback when provision fail.
- (void)startProvisionWithPeripheral:(CBPeripheral *)peripheral unicastAddress:(UInt16)unicastAddress networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex provisionType:(ProvisionTpye)provisionType staticOOBData:(NSData *)staticOOBData startSendProvision:(bleFinishScanedCharachteristicCallBack)startSendProvision provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail;


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


/// keybind (with retry)
/// @param peripheral CBPeripheral of CoreBluetooth will be keybind.
/// @param unicastAddress address of new device.
/// @param appkey appkey
/// @param appkeyIndex appkeyIndex
/// @param netkeyIndex netkeyIndex
/// @param keyBindType KeyBindTpye_Normal means add appkey and model bind, KeyBindTpye_Fast means just add appkey.
/// @param retryCount retry count of keybind.
/// @param productID the productID info need to save in node when keyBindType is KeyBindTpye_Fast.
/// @param cpsData the elements info need to save in node when keyBindType is KeyBindTpye_Fast.
/// @param keyBindSuccess callback when keybind success.
/// @param fail callback when provision fail.
- (void)startKeyBindWithPeripheral:(CBPeripheral *)peripheral unicastAddress:(UInt16)unicastAddress appKey:(NSData *)appkey appkeyIndex:(UInt16)appkeyIndex netkeyIndex:(UInt16)netkeyIndex keyBindType:(KeyBindTpye)keyBindType retryCount:(int)retryCount productID:(UInt16)productID cpsData:(NSData *)cpsData keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess fail:(ErrorBlock)fail;


/// set filter
/// @param locationAddress current provisioner's nodeAddress
/// @param timeout timeout can't be 0.
/// @param result callback when set filter successful or timeout.
- (void)setFilterWithLocationAddress:(UInt16)locationAddress timeout:(NSTimeInterval)timeout result:(bleSetFilterResultCallBack)result;


/// cancel set filter
- (void)cancelSetFilterWithTimeout;


/// send sig model ini command or send vendor model ini command.
/// @param command config of sig model command or vendor model command. sig model struct: mesh_bulk_cmd_par_t, vendor model struct: mesh_vendor_par_ini_t. sig model config: netkeyIndex, appkeyIndex, retryCount, responseMax, address, opcode, commandData.
/// @param responseCallback callback when SDK receive response data of this command. And this callback will remove from SDK when all responses had received or command had timeout. Attention: this callback will not callback forever when command.responseOpcode is 0.
- (void)sendIniCommand:(IniCommandModel *)command responseCallback:(responseModelCallBack)responseCallback;


/// clean commands cache, because SDK may has many commands in queue when app change mesh.
- (void)cleanCommandsCache;

@end
