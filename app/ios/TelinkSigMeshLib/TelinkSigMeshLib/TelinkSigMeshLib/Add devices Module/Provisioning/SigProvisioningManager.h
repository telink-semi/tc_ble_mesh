/********************************************************************************************************
 * @file     SigProvisioningManager.h
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
//  SigProvisioningManager.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/8/19.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class SigProvisioningData,SigProvisioningResponse,SigAuthenticationModel;

typedef void(^prvisionResponseCallBack)(SigProvisioningResponse * _Nullable response);

@interface SigProvisioningManager : NSObject
@property (nonatomic,assign) AuthenticationMethod authenticationMethod;
@property (nonatomic,strong) SigAuthenticationModel *authenticationModel;
@property (nonatomic,strong) SigProvisioningData *provisioningData;
@property (nonatomic,copy,nullable) prvisionResponseCallBack provisionResponseBlock;

#pragma mark - Public properties

/// The provisioning capabilities of the device. This information is retrieved from the remote device during identification process.
@property (nonatomic, assign) struct ProvisioningCapabilities provisioningCapabilities;

/// The Network Key to be sent to the device during provisioning.
@property (nonatomic, strong) SigNetkeyModel *networkKey;

/// The current state of the provisioning process.
@property (nonatomic, assign) ProvisionigState state;//init with ready


+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));


+ (SigProvisioningManager *)share;

/// founcation1: no oob provision (CBPeripheral's state mush be CBPeripheralStateConnected.)
- (void)provisionWithUnicastAddress:(UInt16)unicastAddress networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail;

/// founcation2: static oob provision (CBPeripheral's state mush be CBPeripheralStateConnected.)
- (void)provisionWithUnicastAddress:(UInt16)unicastAddress networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex staticOobData:(NSData *)oobData provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail;

/// founcation3: provision (If CBPeripheral's state isn't CBPeripheralStateConnected, SDK will connect CBPeripheral in this api. )
/// @param peripheral CBPeripheral of CoreBluetooth will be provision.
/// @param unicastAddress address of new device.
/// @param networkKey networkKey
/// @param netkeyIndex netkeyIndex
/// @param provisionType ProvisionTpye_NoOOB means oob data is 16 bytes zero data, ProvisionTpye_StaticOOB means oob data is get from HTTP API.
/// @param staticOOBData oob data get from HTTP API when provisionType is ProvisionTpye_StaticOOB.
/// @param provisionSuccess callback when provision success.
/// @param fail callback when provision fail.
- (void)provisionWithPeripheral:(CBPeripheral *)peripheral unicastAddress:(UInt16)unicastAddress networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex provisionType:(ProvisionTpye)provisionType staticOOBData:(nullable NSData *)staticOOBData provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail;

@end

NS_ASSUME_NONNULL_END
