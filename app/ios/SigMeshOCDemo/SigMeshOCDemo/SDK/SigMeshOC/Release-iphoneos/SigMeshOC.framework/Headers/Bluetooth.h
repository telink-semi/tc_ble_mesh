/********************************************************************************************************
 * @file     Bluetooth.h 
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
//  Bluetooth.h
//  SigMeshOC
//
//  Created by Liangjiazhi on 2018/7/12.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "BLECallBackAPI.h"
#import <CoreBluetooth/CoreBluetooth.h>

@interface Bluetooth : BLECallBackAPI<CBCentralManagerDelegate, CBPeripheralDelegate>

+ (Bluetooth *)share;

#pragma  mark - Public
- (CBPeripheral *)currentPeripheral;

/// Get current Bluethooth's connect status.(当前蓝牙是否处于连接状态)
- (BOOL)isConnected;

/// Is remote add type?(当前是否是remote添加状态)
- (BOOL)isRemoteAdd;

/// Clear cache, it is always call before add device.(添加设备前，清零缓存)
- (void)clearCachelist;

/// Start scan(开始扫描)
- (void)startScan;

/// Stop scan(停止扫描)
- (void)stopScan;

/// Connect CBPeripheral(通过设备实体直连某设备，特殊情况如OTA需要使用)
- (void)connectPeripheral:(CBPeripheral *)p;

/// Connect CBPeripheral from uuid(通过UUID直连某设备，特殊情况如OTA需要使用)
- (void)connectPeripheralWithUUID:(NSString *)uuidString;

/// Disconnect CBPeripheral(断开某设备的连接，特殊情况如OTA需要使用)
- (void)cancelConnection:(CBPeripheral *)p complete:(bleCancelConnectCallBack)complete;

/// Disconnect all CBPeripheral(断开所有设备的连接)
- (void)cancelAllConnecttionWithComplete:(bleCancelConnectCallBack)complete;

/// Set default block(修改默认的block)
- (void)blockState;

/// Kick out(剔除设备)
- (void)kickOut:(UInt16)address;

/// Set current state to keybind state(修改SDK当前工作状态为keyBind)
- (void)setKeyBindState;

/// Set current state to provision state(修改SDK当前工作状态为provision)
- (void)setProvisionState;

/// Set current state to OTA state(修改SDK当前工作状态为OTA)
- (void)setOTAState;

/// Set current state to normal state(修改SDK当前工作状态为normal)
- (void)setNormalState;

/**
 if you did provision process , you could call this method, and it'll call back what you need

 @param availableList the devices you added(注意：SDK收到非该数组的设备的数据是不会处理的)
 @param canControl you can send command to theos devices
 */
- (void)startWorkNormal:(NSArray <NSString *>*)availableList canControl:(workWithPeripheralCallBack)canControl NS_DEPRECATED_IOS(2_0, 9_0, "Use -startWorkNormalWithComplete:") __TVOS_PROHIBITED;

/**
 if you did provision process , you could call this method, and it'll call back what you need

 @param complete callback UUIDstring of direct connection node
 */
- (void)startWorkNormalWithComplete:(workWithPeripheralCallBack)complete;

/**
 if you need do provision , you should call this method, and it'll call back what you need
 
 @param address address of new device
 @param networkKey network key, which provsion need, you can see it as password of the mesh
 @param netkeyIndex netkey index
 @param unicastAddress address of remote device
 @param uuid uuid of remote device
 @param type KeyBindTpye_Normal是普通添加模式，KeyBindTpye_Quick是快速添加模式
 @param isAuto 添加完成一个设备后，是否自动扫描添加下一个设备
 @param provisionSuccess call back when a device provision successful
 @param keyBindSuccess call back when a device keybind successful
 @param fail call back when a device add to the mesh fail
 @param finish finish add the available devices list to the mesh
 */
- (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex unicastAddress:(UInt16)unicastAddress uuid:(NSData *)uuid keyBindType:(KeyBindTpye)type isAutoAddNextDevice:(BOOL)isAuto provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess fail:(prvisionFailCallBack)fail finish:(prvisionFinishCallBack)finish;


/**
 if you need do provision , you should call this method, and it'll call back what you need

 @param address address of new device
 @param networkKey network key, which provsion need, you can see it as password of the mesh
 @param netkeyIndex netkey index
 @param peripheral device need add to mesh
 @param type KeyBindTpye_Normal是普通添加模式，KeyBindTpye_Quick是快速添加模式
 @param provisionSuccess call back when a device provision successful
 @param keyBindSuccess call back when a device keybind successful
 @param fail call back when a device add to the mesh fail
 @param finish finish add the available devices list to the mesh
 */
- (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex peripheral:(CBPeripheral *)peripheral keyBindType:(KeyBindTpye)type provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess fail:(prvisionFailCallBack)fail finish:(prvisionFinishCallBack)finish;


/**
 if you send a command which need response, and you can receive response here
 !attention: before last command response back , it'll not corresponding new command
 
 @param opcode command opcode
 @param requestCount device count you want corresponding
 @param responseCount device count corresponding actually
 */
- (void)meshReliableResponseBack:(u16)opcode requestCount:(u32)requestCount responseCount:(u32)responseCount;

/// Do key bound(纯keyBind接口)
- (void)keyBind:(u16)address appkey:(NSData *)appkey appkeyIndex:(u16)appkeyIndex netkeyIndex:(u16)netkeyIndex keyBindType:(KeyBindTpye)type keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess fail:(prvisionFailCallBack)fail;

/// Set element count of current provision device.(记录当前provision的设备的element个数)
- (void)setElementCount:(UInt8)ele_count;

/// Get address of current provision device.(获取当前provision的设备的短地址)
- (UInt16)getCurrentProvisionAddress;

/// Get current key bind type.(获取当前的添加模式)
- (KeyBindTpye)getCurrentKeyBindType;

/// Stop auto connect(停止自动连接流程)
- (void)stopAutoConnect;

#pragma mark - lib need api
/**
 when command need response ,and it'll update information here
 
 @param m model
 */
- (void)responseBack:(ResponseModel *)m;

- (void)responseBackOfVendorID:(ResponseModel *)m;

- (void)writeForPROXYIn:(NSData *)d;

- (void)writeForPBGATTIn:(NSData *)d;

- (void)writeOTAData:(NSData *)d;

- (void)addDevice_provisionResultBack:(BOOL)isSuccess;

- (void)addDevice_keyBindResultBack:(BOOL)isSuccess;

- (void)keyBindAfterProvisionSuccess;

///delay 500ms -> set_filter_init -> delay 500ms -> keyBind
- (void)delayForSetFilterBeforKeyBind;

/// Get Online device, private use OnlineStatusCharacteristic(获取当前mesh网络的所有设备的在线、开关、亮度、色温状态（私有定制，需要特定的OnlineStatusCharacteristic）)
- (void)getOnlineStatueFromUUID;

///app should reset mesh busyTimer when change mesh.(重置isBusy定时器，切换mesh网络或者初始化mesh时使用)
- (void)resetMeshBusyTimer;

- (void)startCheckOfflineTimerWithAddress:(NSNumber *)address;

- (void)stopCheckOfflineTimerWithAddress:(NSNumber *)address;

///set filter
- (void)setFilterWithLocationAddress:(UInt16)locationAddress complete:(bleSetFilterResponseCallBack)complete fail:(bleSetFilterFailCallBack)fail;

/// callback one time when SDK is not bust.
- (void)commondAfterNoBusy:(bleCompleteCallBack)complete;

@end
