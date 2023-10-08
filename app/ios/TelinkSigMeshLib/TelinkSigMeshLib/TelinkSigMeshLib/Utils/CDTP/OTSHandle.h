/********************************************************************************************************
 * @file     OTSHandle.h
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2021/11/29
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
#import "OTSBaseModel.h"
#import "OTSCommand.h"
@class OTSCommand;
NS_ASSUME_NONNULL_BEGIN
@interface OTSHandle : NSObject
@property (nonatomic, strong) CBPeripheral *peripheral;
@property (nonatomic, assign) UInt32 currentSize;

/// command list cache.
@property (nonatomic, strong) NSMutableArray <OTSCommand *>*commands;
/// A queue to handle incoming and outgoing messages.
@property (nonatomic, strong) dispatch_queue_t queue;
@property (nonatomic, strong, nullable) SigBluetooth *bluetooth;

+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));


/**
 *  @brief  Singleton method
 *
 *  @return the default singleton instance. You are not allowed to create your own instances of this class.
 */
+ (instancetype)share;


/// send OTSCommand api
/// @param command command
- (void)sendOTSCommand:(OTSCommand *)command;

/// Cancels sending the message with the given OTSCommand.
/// @param command The message identifier.
- (void)cancelOTSCommand:(OTSCommand *)command;

/// cancel all commands.
- (void)cleanAllCommands;

/// Returns whether OTSHandle is busy. YES means busy.
- (BOOL)isBusyNow;

/**
 * @brief   Get Characteristic of peripheral.
 * @param   uuidString UUIDString of Characteristic.
 * @param   peripheral the CBPeripheral object.
 * @return  A CBCharacteristic object.
 */
- (nullable CBCharacteristic *)getCBCharacteristicWithUUIDString:(NSString *)uuidString ofPeripheral:(CBPeripheral *)peripheral;

@end

NS_ASSUME_NONNULL_END
