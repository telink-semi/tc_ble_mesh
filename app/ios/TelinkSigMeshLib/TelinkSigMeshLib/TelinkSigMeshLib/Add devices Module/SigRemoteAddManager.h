/********************************************************************************************************
* @file     SigRemoteAddManager.h
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
//  SigRemoteAddManager.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2020/3/26.
//  Copyright © 2020 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/*总超时kRemoteProgressTimeout为60秒，每个小步骤都重试kRemoteProgressRetryCount即0xFF次，除非直连灯断开连接，否则remote provision最大耗时为60秒。*/
#define kRemoteProgressRetryCount   (0xFF)
#define kRemoteProgressTimeout   (60)
#define kScannedItemsLimit   (2)
#define kScanCapabilitiesTimeout   (5)
#define kSendOneNodeScanTimeout   (3)
#define kScannedItemsTimeout   (5)

@class SigProvisioningData,SigAuthenticationModel;
typedef void(^remoteProvisioningScanReportCallBack)(SigRemoteScanRspModel *scanRemoteModel);

@interface SigRemoteAddManager : NSObject
@property (nonatomic, assign) AuthenticationMethod authenticationMethod;
@property (nonatomic, strong) SigAuthenticationModel *authenticationModel;
@property (nonatomic, strong) SigProvisioningData *provisioningData;

#pragma mark - Public properties

/// The provisioning capabilities of the device. This information is retrieved from the remote device during identification process.
@property (nonatomic, assign) struct ProvisioningCapabilities provisioningCapabilities;

/// The Network Key to be sent to the device during provisioning.
@property (nonatomic, strong) SigNetkeyModel *networkKey;

/// The current state of the provisioning process.
@property (nonatomic, assign) ProvisionigState state;//init with ready


+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));


+ (SigRemoteAddManager *)share;

- (void)startRemoteProvisionScanWithReportCallback:(remoteProvisioningScanReportCallBack)reportCallback resultCallback:(resultBlock)resultCallback;

/// founcation4: remote provision (SDK need connected provisioned node.)
/// @param provisionAddress address of new device.
/// @param reportNodeAddress address of node that report this uuid
/// @param reportNodeUUID identify node that need to provision.
/// @param networkKey networkKey
/// @param netkeyIndex netkeyIndex
/// @param provisionType ProvisionTpye_NoOOB means oob data is 16 bytes zero data, ProvisionTpye_StaticOOB means oob data is get from HTTP API.
/// @param staticOOBData oob data get from HTTP API when provisionType is ProvisionTpye_StaticOOB.
/// @param provisionSuccess callback when provision success.
/// @param fail callback when provision fail.
- (void)remoteProvisionWithNextProvisionAddress:(UInt16)provisionAddress reportNodeAddress:(UInt16)reportNodeAddress reportNodeUUID:(NSData *)reportNodeUUID networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex provisionType:(ProvisionTpye)provisionType staticOOBData:(nullable NSData *)staticOOBData provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail;

@end

NS_ASSUME_NONNULL_END
