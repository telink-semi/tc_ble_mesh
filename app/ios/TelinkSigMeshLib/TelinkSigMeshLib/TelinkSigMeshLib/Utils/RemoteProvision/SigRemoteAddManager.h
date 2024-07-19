/********************************************************************************************************
 * @file     SigRemoteAddManager.h
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2020/3/26
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

/*总超时kRemoteProgressTimeout为120秒，每个小步骤都重试kRemoteProgressRetryCount即0xFF次，除非直连灯断开连接，否则remote provision最大耗时为120秒。*/
#define kRemoteProgressRetryCount   (0xFF)
#define kRemoteProgressTimeout   (120)
#define kScannedItemsLimit   (2)
#define kScanCapabilitiesTimeout   (5)
#define kSendOneNodeScanTimeout   (3)
#define kScannedItemsTimeout   (5)
#define kLinkOpenTimeout   (10)

@class SigProvisioningData,SigProvisioningCapabilitiesPdu,SigAuthenticationModel;
typedef void(^remoteProvisioningScanReportCallBack)(SigRemoteScanRspModel *scanRemoteModel);

@interface SigRemoteAddManager : NSObject
@property (nonatomic, assign) AuthenticationMethod authenticationMethod;
@property (nonatomic, strong, nullable) SigAuthenticationModel *authenticationModel;
@property (nonatomic, strong, nullable) SigProvisioningData *provisioningData;

/// - seeAlso: MshPRFv1.0.1.pdf  (page.240)
/// Attention Timer state (See Section 4.2.9), default is 0.
@property (nonatomic, assign) UInt8 attentionDuration;

#pragma mark - Public properties

/// The provisioning capabilities of the device. This information is retrieved from the remote device during identification process.
@property (nonatomic, strong, nullable) SigProvisioningCapabilitiesPdu *provisioningCapabilities;

/// The Network Key to be sent to the device during provisioning.
@property (nonatomic, strong) SigNetkeyModel *networkKey;

/// The current state of the provisioning process.
@property (nonatomic, assign) ProvisioningState state;//init with ready


+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));


/**
 *  @brief  Singleton method
 *
 *  @return the default singleton instance. You are not allowed to create your own instances of this class.
 */
+ (instancetype)share;

- (void)startRemoteProvisionScanWithReportCallback:(remoteProvisioningScanReportCallBack)reportCallback resultCallback:(resultBlock)resultCallback;

/// founcation4: remote provision (SDK need connected provisioned node.)
/// @param reportNodeAddress address of node that report this uuid
/// @param reportNodeUUID identify node that need to provision.
/// @param networkKey networkKey
/// @param netkeyIndex netkeyIndex
/// @param provisionType ProvisionType_NoOOB means oob data is 16 bytes zero data, ProvisionType_StaticOOB means oob data is get from HTTP API.
/// @param staticOOBData oob data get from HTTP API when provisionType is ProvisionType_StaticOOB.
/// @param capabilitiesResponse callback when capabilities response.
/// @param provisionSuccess callback when provision success.
/// @param fail callback when provision fail.
- (void)remoteProvisionWithNReportNodeAddress:(UInt16)reportNodeAddress reportNodeUUID:(NSData *)reportNodeUUID networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex provisionType:(ProvisionType)provisionType staticOOBData:(nullable NSData *)staticOOBData capabilitiesResponse:(nullable addDevice_capabilitiesWithReturnCallBack)capabilitiesResponse provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail;


/// founcation4: remote provision (SDK need connected provisioned node.)
/// @param provisionAddress address of new device.
/// @param reportNodeAddress address of node that report this uuid
/// @param reportNodeUUID identify node that need to provision.
/// @param networkKey networkKey
/// @param netkeyIndex netkeyIndex
/// @param provisionType ProvisionType_NoOOB means oob data is 16 bytes zero data, ProvisionType_StaticOOB means oob data is get from HTTP API.
/// @param staticOOBData oob data get from HTTP API when provisionType is ProvisionType_StaticOOB.
/// @param provisionSuccess callback when provision success.
/// @param fail callback when provision fail.
- (void)remoteProvisionWithNextProvisionAddress:(UInt16)provisionAddress reportNodeAddress:(UInt16)reportNodeAddress reportNodeUUID:(NSData *)reportNodeUUID networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex provisionType:(ProvisionType)provisionType staticOOBData:(nullable NSData *)staticOOBData provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail DEPRECATED_MSG_ATTRIBUTE("Use 'remoteProvisionWithNReportNodeAddress:reportNodeUUID:networkKey:netkeyIndex:provisionType:staticOOBData:capabilitiesResponse:provisionSuccess:fail:' instead");

@end

NS_ASSUME_NONNULL_END
