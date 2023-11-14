/********************************************************************************************************
 * @file     SigProvisioningManager.h
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/8/19
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

@class SigProvisioningData,SigAuthenticationModel;

typedef void(^prvisionResponseCallBack)(SigProvisioningPdu * _Nullable response);

@interface SigProvisioningManager : NSObject
@property (nonatomic,assign) AuthenticationMethod authenticationMethod;
@property (nonatomic,strong,nullable) SigAuthenticationModel *authenticationModel;
@property (nonatomic,strong,nullable) SigProvisioningData *provisioningData;
@property (nonatomic,copy,nullable) prvisionResponseCallBack provisionResponseBlock;
@property (nonatomic,copy,nullable) addDevice_capabilitiesWithReturnCallBack capabilitiesResponseBlock;

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

/// founcation1: no oob provision (CBPeripheral's state mush be CBPeripheralStateConnected.)
//- (void)provisionWithNetworkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex capabilitiesResponse:(addDevice_capabilitiesWithReturnCallBack)capabilitiesResponse provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail;

/// founcation2: static oob provision (CBPeripheral's state mush be CBPeripheralStateConnected.)
/**
 * @brief   Provision
 * @param   networkKey    the networkKey of mesh.
 * @param   netkeyIndex    the netkeyIndex of mesh.
 * @param   oobData    the staticOobData of node.
 * @param   capabilitiesResponse    callback when app receive capabilities pdu from node.
 * @param   provisionSuccess    callback when node provision success.
 * @param   fail    callback when node provision fail.
 * @note    oobData is nil means do no oob provision, oobData is not nil means do static oob provision.
 */
- (void)provisionWithNetworkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex staticOobData:(NSData * _Nullable)oobData capabilitiesResponse:(addDevice_capabilitiesWithReturnCallBack)capabilitiesResponse provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail;

/**
 * @brief   Provision Method, calculate unicastAddress when capabilities response from unProvision node. (If CBPeripheral's state isn't CBPeripheralStateConnected, SDK will connect CBPeripheral in this api. )
 * @param   peripheral CBPeripheral of CoreBluetooth will be provision.
 * @param   networkKey network key.
 * @param   netkeyIndex netkey index.
 * @param   staticOOBData oob data get from HTTP API when provisionType is ProvisionType_StaticOOB.
 * @param   capabilitiesResponse callback when capabilities response, app need return unicastAddress for provision process.
 * @param   provisionSuccess callback when provision success.
 * @param   fail callback when provision fail.
 * @note    1.get provision type from Capabilities response.
 * 2.Static OOB Type is NO_OOB, SDK will call no oob provision.
 * 3.Static OOB Type is Static_OOB, SDK will call static oob provision.
 * 4.if it is no static oob data when Static OOB Type is Static_OOB, SDK will try on oob provision when SigMeshLib.share.dataSource.addStaticOOBDeviceByNoOOBEnable is YES.
 */
- (void)provisionWithPeripheral:(CBPeripheral *)peripheral networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex staticOOBData:(nullable NSData *)staticOOBData capabilitiesResponse:(addDevice_capabilitiesWithReturnCallBack)capabilitiesResponse provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail;

/// founcation4: certificateBased provision
/// @param peripheral CBPeripheral of CoreBluetooth will be provision.
/// @param networkKey networkKey
/// @param netkeyIndex netkeyIndex
/// @param staticOOBData oob data get from HTTP API when provisionType is ProvisionType_StaticOOB.
/// @param provisionSuccess callback when provision success.
/// @param fail callback when provision fail.
- (void)certificateBasedProvisionWithPeripheral:(CBPeripheral *)peripheral networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex staticOOBData:(nullable NSData *)staticOOBData capabilitiesResponse:(addDevice_capabilitiesWithReturnCallBack)capabilitiesResponse provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail;


/// founcation1: no oob provision (CBPeripheral's state mush be CBPeripheralStateConnected.)
- (void)provisionWithUnicastAddress:(UInt16)unicastAddress networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail DEPRECATED_MSG_ATTRIBUTE("Use 'provisionWithNetworkKey:netkeyIndex:capabilitiesResponse:provisionSuccess:fail:' instead");

/// founcation2: static oob provision (CBPeripheral's state mush be CBPeripheralStateConnected.)
- (void)provisionWithUnicastAddress:(UInt16)unicastAddress networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex staticOobData:(NSData *)oobData provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail DEPRECATED_MSG_ATTRIBUTE("Use 'provisionWithNetworkKey:netkeyIndex:staticOobData:capabilitiesResponse:provisionSuccess:fail:' instead");

/// founcation3: provision (If CBPeripheral's state isn't CBPeripheralStateConnected, SDK will connect CBPeripheral in this api. )
/// @param peripheral CBPeripheral of CoreBluetooth will be provision.
/// @param unicastAddress address of new device.
/// @param networkKey networkKey
/// @param netkeyIndex netkeyIndex
/// @param staticOOBData oob data get from HTTP API when provisionType is ProvisionType_StaticOOB.
/// @param provisionSuccess callback when provision success.
/// @param fail callback when provision fail.
- (void)provisionWithPeripheral:(CBPeripheral *)peripheral unicastAddress:(UInt16)unicastAddress networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex staticOOBData:(nullable NSData *)staticOOBData provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail DEPRECATED_MSG_ATTRIBUTE("Use 'provisionWithPeripheral:networkKey:netkeyIndex:provisionType:staticOOBData:capabilitiesResponse:provisionSuccess:fail:' instead");

/// founcation4: provision (If CBPeripheral's state isn't CBPeripheralStateConnected, SDK will connect CBPeripheral in this api. )
/// @param peripheral CBPeripheral of CoreBluetooth will be provision.
/// @param unicastAddress address of new device.
/// @param networkKey networkKey
/// @param netkeyIndex netkeyIndex
/// @param staticOOBData oob data get from HTTP API when provisionType is ProvisionType_StaticOOB.
/// @param provisionSuccess callback when provision success.
/// @param fail callback when provision fail.
- (void)certificateBasedProvisionWithPeripheral:(CBPeripheral *)peripheral unicastAddress:(UInt16)unicastAddress networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex staticOOBData:(nullable NSData *)staticOOBData provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail DEPRECATED_MSG_ATTRIBUTE("Use 'certificateBasedProvisionWithPeripheral:networkKey:netkeyIndex:provisionType:staticOOBData:capabilitiesResponse:provisionSuccess:fail:' instead");

@end

NS_ASSUME_NONNULL_END
