/********************************************************************************************************
 * @file     SigAddDeviceManager.h
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/9/4
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

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class SigAppkeyModel;

@interface SigAddDeviceManager : NSObject
@property (nonatomic, assign) BOOL needDisconnectBetweenProvisionToKeyBind;//default YES
@property (nonatomic, assign) UInt8 retryCount;//retry count of provision and keybind, default 3


+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));


/**
 *  @brief  Singleton method
 *
 *  @return the default singleton instance. You are not allowed to create your own instances of this class.
 */
+ (instancetype)share;

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
- (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel uuid:(nullable NSData *)uuid keyBindType:(KeyBindType)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData isAutoAddNextDevice:(BOOL)isAuto provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail finish:(AddDeviceFinishCallBack)finish DEPRECATED_MSG_ATTRIBUTE("Use 'startAddDeviceWithSigAddConfigModel:capabilitiesResponse:provisionSuccess:provisionFail:keyBindSuccess:keyBindFail:' instead");

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
- (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel uuid:(nullable NSData *)uuid keyBindType:(KeyBindType)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData isAutoAddNextDevice:(BOOL)isAuto startConnect:(addDevice_startConnectCallBack)startConnect startProvision:(addDevice_startProvisionCallBack)startProvision provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail finish:(AddDeviceFinishCallBack)finish DEPRECATED_MSG_ATTRIBUTE("Use 'startAddDeviceWithSigAddConfigModel:capabilitiesResponse:provisionSuccess:provisionFail:keyBindSuccess:keyBindFail:' instead");

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
- (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel peripheral:(CBPeripheral *)peripheral staticOOBData:(nullable NSData *)staticOOBData keyBindType:(KeyBindType)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail DEPRECATED_MSG_ATTRIBUTE("Use 'startAddDeviceWithSigAddConfigModel:capabilitiesResponse:provisionSuccess:provisionFail:keyBindSuccess:keyBindFail:' instead");

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
- (void)startAddDeviceWithNetworkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel peripheral:(CBPeripheral *)peripheral staticOOBData:(nullable NSData *)staticOOBData keyBindType:(KeyBindType)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData capabilitiesResponse:(nullable addDevice_capabilitiesWithReturnCallBack)capabilitiesResponse provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail DEPRECATED_MSG_ATTRIBUTE("Use 'startAddDeviceWithSigAddConfigModel:capabilitiesResponse:provisionSuccess:provisionFail:keyBindSuccess:keyBindFail:' instead");

/**
 * @brief   Add Single Device (provision+keyBind), the flag of Certificate Based in unProvision adv data is 1, SDK will get and verify the Certificate of node.
 * @param   networkKey network key, which provsion need, you can see it as password of the mesh.
 * @param   netkeyIndex netkey index.
 * @param   appkeyModel appkey model.
 * @param   peripheral the bluetooth Peripheral object of node..
 * @param   staticOOBData oob data get from HTTP API when provisionType is ProvisionType_StaticOOB.
 * @param   type KeyBindType_Normal是普通添加模式，KeyBindType_Quick是快速添加模式.
 * @param   productID product identify of node.
 * @param   cpsData composition data of node.
 * @param   startConnect callback when SDK start connect node.
 * @param   startProvision callback when SDK start provision node.
 * @param   capabilitiesResponse callback when capabilities response from unProvision node.
 * @param   provisionSuccess callback when provision success.
 * @param   provisionFail callback when provision fail.
 * @param   keyBindSuccess callback when keybind success.
 * @param   keyBindFail callback when keybind fail.
 */
- (void)startCertificateBasedWithNetworkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel peripheral:(CBPeripheral *)peripheral staticOOBData:(nullable NSData *)staticOOBData keyBindType:(KeyBindType)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData startConnect:(addDevice_startConnectCallBack)startConnect startProvision:(addDevice_startProvisionCallBack)startProvision capabilitiesResponse:(addDevice_capabilitiesWithReturnCallBack)capabilitiesResponse provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail;

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
- (void)startAddDeviceWithSigAddConfigModel:(SigAddConfigModel *)configModel capabilitiesResponse:(addDevice_capabilitiesWithReturnCallBack)capabilitiesResponse provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail;

@end

NS_ASSUME_NONNULL_END
