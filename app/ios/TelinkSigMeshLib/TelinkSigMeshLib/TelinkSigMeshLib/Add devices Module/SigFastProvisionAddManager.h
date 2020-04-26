//
//  SigFastProvisionAddManager.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2020/4/2.
//  Copyright © 2020 梁家誌. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef void(^AddSingleDeviceSuccessOfFastProvisionCallBack)(NSData *deviceKey,NSString *macAddress,UInt16 address,UInt16 pid);

@interface SigFastProvisionAddManager : NSObject


+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));


+ (SigFastProvisionAddManager *)share;


/// start FastProvision
/// @param provisionAddress  new unicastAddress for unprovision device.
/// @param productId  product id of unprovision device, 0xffff means provision all unprovision device, but develop can't use 0xffff in this api.
/// @param compositionData compositionData of node in this productId.
/// @param unprovisioned current Connected Node Is Unprovisioned?
/// @param singleSuccess callback when SDK add single devcie successful.
/// @param finish callback when fast provision finish, fast provision successful when error is nil.
- (void)startFastProvisionWithProvisionAddress:(UInt16)provisionAddress productId:(UInt16)productId compositionData:(NSData *)compositionData currentConnectedNodeIsUnprovisioned:(BOOL)unprovisioned addSingleDeviceSuccessCallback:(AddSingleDeviceSuccessOfFastProvisionCallBack)singleSuccess finish:(ErrorBlock)finish;

#pragma mark - command

/// reset network
/// @param delayMillisecond  delay of reset network, the unit is millisecond.
- (void)fastProvisionResetNetworkWithDelayMillisecond:(UInt16)delayMillisecond successCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// get address
/// @param productId  product id of unprovision device, 0xffff means provision all unprovision device.
- (void)fastProvisionGetAddressWithProductId:(UInt16)productId successCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// set address
/// @param provisionAddress  new unicastAddress for unprovision device.
/// @param macAddressData  macAddress of unprovision device.
/// @param destination unicastAddress of connected node.
- (void)fastProvisionSetAddressWithProvisionAddress:(UInt16)provisionAddress macAddressData:(NSData *)macAddressData toDestination:(UInt16)destination successCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// get address retry
/// @param productId  product id of unprovision device, 0xffff means provision all unprovision device.
/// @param provisionAddress  new unicastAddress for unprovision device.
- (void)fastProvisionGetAddressRetryWithProductId:(UInt16)productId provisionAddress:(UInt16)provisionAddress successCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// set new network info
- (void)fastProvisionSetNetworkInfoWithSuccessCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// confirm of set network info
- (void)fastProvisionConfirmWithSuccessCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// complete, recovery to new network info.
/// @param delayMillisecond  delay of recovery to new network info, the unit is millisecond.
- (void)fastProvisionCompleteWithDelayMillisecond:(UInt16)delayMillisecond successCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

@end

NS_ASSUME_NONNULL_END