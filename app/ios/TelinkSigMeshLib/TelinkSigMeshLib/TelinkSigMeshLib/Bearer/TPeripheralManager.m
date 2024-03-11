/********************************************************************************************************
 * @file     TPeripheralManager.m
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

#import "TPeripheralManager.h"

//一个广播包，默认广播的时长
#define kAdvertisingInterval (0.15)

@interface TPeripheralManager()<CBPeripheralManagerDelegate>
@property (nonatomic,copy) advertisingResult resultBlock;
@property (nonatomic,assign) NSTimeInterval advertisingInterval;
@property (nonatomic,strong) NSData *advertisingData;
@end

@implementation TPeripheralManager

/**
 *  @brief  Singleton method
 *
 *  @return the default singleton instance.
 */
+ (instancetype)share {
    /// Singleton instance
    static TPeripheralManager *share = nil;
    /// Note: The dispatch_once function can ensure that a certain piece
    /// of code is only executed once in the entire application life cycle!
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        /// Initialize the Singleton configure parameters.
        share = [[TPeripheralManager alloc] init];
    });
    return share;
}

/// Initialize SDK, including initializing service list.
- (void)initSDK {
    _advertisingInterval = kAdvertisingInterval;
    self.peripheralManager = [[CBPeripheralManager alloc] initWithDelegate:self queue:dispatch_get_main_queue()];
    if (self.peripheralManager.state == CBPeripheralManagerStatePoweredOn) {
        [self initServices];
    } else if (self.peripheralManager.state == CBPeripheralManagerStateUnknown) {
        [self performSelector:@selector(initServices) withObject:nil afterDelay:0.2];
    } else {
        TelinkLogInfo(@"Please open Bluetooth.");
    }
}

/// Get Bluetooth status
- (BOOL)isPoweredOn {
    return self.peripheralManager.state == CBPeripheralManagerStatePoweredOn;
}

/// Initialize Bluetooth service list.
- (void)initServices {
    [self.peripheralManager removeAllServices];
}

/// Stop broadcast
- (void)stopAdvertising {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(advertisingSuccess:) object:@(YES)];
    });
    [self.peripheralManager stopAdvertising];
}

/// Start broadcast with NSDictionary
/// - Parameters:
///   - dict: broadcast AD NSDictionary.
///   - interval: broadcast interval.
///   - result: broadcast result handle.
- (void)advertisingDictionary:(NSDictionary *)dict advertisingInterval:(NSTimeInterval)interval result:(nullable advertisingResult)result {
    self.resultBlock = result;
    self.advertisingInterval = interval;
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(advertisingSuccess:) object:@(YES)];
    });
    //注意0：前3字节固定为“02011A”
    //注意1：将自定义数据放在蓝牙广播包的CBAdvertisementDataServiceUUIDsKey中，UUID长度是16字节则蓝牙包的type为0x07，UUID长度是4字节则蓝牙包的type为0x05，UUID长度是2字节则蓝牙包的type为0x03。
    //注意2：将自定义数据放在蓝牙广播包的CBAdvertisementDataLocalNameKey中，蓝牙包的type为0x09。如果名称大于26字节，则蓝牙包的type为0x08。且系统会自动在ASCII=0x00的位置截断字符串。
    //注意3：将自定义数据放在蓝牙广播包的@"kCBAdvDataAppleBeaconKey"中，蓝牙包的type为0xFF。且长度固定是21字节，不可增减。

    if (self.peripheralManager.isAdvertising) {
        [self.peripheralManager stopAdvertising];
    }
    TelinkLogDebug(@"startAdvertising:%@",dict);
    [self.peripheralManager startAdvertising:dict];
}

/// Start broadcast with Manufacturer Data
/// - Parameters:
///   - data: Manufacturer Data
///   - interval: broadcast interval.
///   - result: broadcast result handle.
- (void)advertisingManufacturerData:(NSData *)data advertisingInterval:(NSTimeInterval)interval result:(nullable advertisingResult)result {
    NSDictionary *dictionary = @{@"kCBAdvDataAppleBeaconKey":data};
    self.advertisingData = data;
    [self advertisingDictionary:dictionary advertisingInterval:interval result:result];
}

/// Handle of broadcast finish.
/// - Parameter isSuccess: broadcast result.
- (void)advertisingSuccess:(NSNumber *)isSuccess {
    if (self.resultBlock) {
        self.resultBlock([isSuccess boolValue]);
        [self stopAdvertising];
    }
    if ([self.delegate respondsToSelector:@selector(onPeripheralManagerDidEndAdvertising:error:advertisingData:)]) {
        [self.delegate onPeripheralManagerDidEndAdvertising:self.peripheralManager error:nil advertisingData:self.advertisingData];
    }
}

#pragma
#pragma mark - CBPeripheralManagerDelegate

/*!
 *  @method peripheralManagerDidUpdateState:
 *
 *  @param peripheral   The peripheral manager whose state has changed.
 *
 *  @discussion         Invoked whenever the peripheral manager's state has been updated. Commands should only be issued when the state is
 *                      <code>CBPeripheralManagerStatePoweredOn</code>. A state below <code>CBPeripheralManagerStatePoweredOn</code>
 *                      implies that advertisement has paused and any connected centrals have been disconnected. If the state moves below
 *                      <code>CBPeripheralManagerStatePoweredOff</code>, advertisement is stopped and must be explicitly restarted, and the
 *                      local database is cleared and all services must be re-added.
 *
 *  @see                state
 *
 */
- (void)peripheralManagerDidUpdateState:(CBPeripheralManager *)peripheral {
    TelinkLogDebug(@"callback peripheralManagerDidUpdateState=%d", peripheral.state);
    if (_blePeripheralUpdateStateBlock) {
        _blePeripheralUpdateStateBlock(peripheral);
    }
}

/*!
 *  @method peripheralManagerDidStartAdvertising:error:
 *
 *  @param peripheral   The peripheral manager providing this information.
 *  @param error        If an error occurred, the cause of the failure.
 *
 *  @discussion         This method returns the result of a @link startAdvertising: @/link call. If advertisement could
 *                      not be started, the cause will be detailed in the <i>error</i> parameter.
 *
 */
- (void)peripheralManagerDidStartAdvertising:(CBPeripheralManager *)peripheral error:(NSError *)error {
//    TeLogDebug(@"callback peripheralManagerDidStartAdvertising,self.advertisingInterval=%f",self.advertisingInterval);
    if (error) {
        TelinkLogDebug(@"error =%@",error.localizedDescription);
        [self advertisingSuccess:@(NO)];
        if ([self.delegate respondsToSelector:@selector(onPeripheralManagerDidEndAdvertising:error:advertisingData:)]) {
            [self.delegate onPeripheralManagerDidEndAdvertising:peripheral error:error advertisingData:self.advertisingData];
        }
    } else {
        dispatch_async(dispatch_get_main_queue(), ^{
            [self performSelector:@selector(advertisingSuccess:) withObject:@(YES) afterDelay:self.advertisingInterval];
        });
    }
}

@end
