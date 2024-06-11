/********************************************************************************************************
 * @file     TPeripheralManager.h
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2021/12/2
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
#import <CoreBluetooth/CoreBluetooth.h>

NS_ASSUME_NONNULL_BEGIN

typedef void(^advertisingResult)(BOOL isSuccess);
typedef void(^BLEPeripheralUpdateStateCallback)(CBPeripheralManager *peripheralManager);


@protocol TPeripheralManagerDelegate <NSObject>
@optional
- (void)onPeripheralManagerDidEndAdvertising:(CBPeripheralManager *)peripheral error:(nullable NSError *)error advertisingData:(NSData *)advertisingData;
@end


@interface TPeripheralManager : NSObject
@property (nonatomic, weak) id <TPeripheralManagerDelegate>delegate;
@property (nonatomic,strong) CBPeripheralManager *peripheralManager;
@property (nonatomic,copy) BLEPeripheralUpdateStateCallback blePeripheralUpdateStateBlock;

/**
 *  @brief  Singleton method
 *
 *  @return the default singleton instance.
 */
+ (instancetype)share;

/// Initialize SDK, including initializing service list.
- (void)initSDK;

/// Get Bluetooth status
- (BOOL)isPoweredOn;

/// Start broadcast
/// - Parameters:
///   - dict: broadcast AD NSDictionary.
///   - interval: broadcast interval.
///   - result: broadcast result handle.
- (void)advertisingDictionary:(NSDictionary *)dict advertisingInterval:(NSTimeInterval)interval result:(nullable advertisingResult)result;

/// Start broadcast with Manufacturer Data
/// - Parameters:
///   - data: Manufacturer Data
///   - interval: broadcast interval.
///   - result: broadcast result handle.
- (void)advertisingManufacturerData:(NSData *)data advertisingInterval:(NSTimeInterval)interval result:(nullable advertisingResult)result;

/// Stop broadcast
- (void)stopAdvertising;

@end

NS_ASSUME_NONNULL_END
