/********************************************************************************************************
 * @file     SigAddDeviceManager.h
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
//  SigAddDeviceManager.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/4.
//  Copyright © 2019 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class SigAppkeyModel;

@interface SigAddDeviceManager : NSObject
@property (nonatomic, assign) BOOL needDisconnectBetweenProvisionToKeyBind;//default YES
@property (nonatomic, assign) UInt8 retryCount;//retry count of provision and keybind, default 3


+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));


+ (SigAddDeviceManager *)share;

/// function1 :add bluetooth devices (auto add)
- (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel unicastAddress:(UInt16)unicastAddress uuid:(nullable NSData *)uuid keyBindType:(KeyBindTpye)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData isAutoAddNextDevice:(BOOL)isAuto provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail finish:(AddDeviceFinishCallBack)finish;

/// function2 :add bluetooth device (single add)
- (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel peripheral:(CBPeripheral *)peripheral provisionType:(ProvisionTpye)provisionType staticOOBData:(nullable NSData *)staticOOBData keyBindType:(KeyBindTpye)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail;

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

@end

NS_ASSUME_NONNULL_END
