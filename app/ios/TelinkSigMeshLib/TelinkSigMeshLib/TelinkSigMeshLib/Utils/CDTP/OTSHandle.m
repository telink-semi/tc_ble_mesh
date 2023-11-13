/********************************************************************************************************
 * @file     OTSHandle.m
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

#import "OTSHandle.h"

@implementation OTSHandle

- (void)setBlock {
    __weak typeof(self) weakSelf = self;
    [self.bluetooth setBluetoothDidUpdateValueCallback:^(CBPeripheral * _Nonnull peripheral, CBCharacteristic * _Nonnull characteristic, NSError * _Nullable error) {
        TelinkLogInfo(@"characteristic=%@ value=%@",characteristic.UUID.UUIDString,characteristic.value);
        if (characteristic.value) {
            if ([characteristic.UUID.UUIDString.uppercaseString isEqualToString:[NSString stringWithFormat:@"%04X",Bluetooth16BitsUUID_objectActionControlPoint]]) {
                //OACP
                OACPResponseCodeModel *response = [[OACPResponseCodeModel alloc] initWithParameters:characteristic.value];
                if (response) {
                    NSArray *temArray = [NSArray arrayWithArray:weakSelf.commands];
                    for (OTSCommand *com in temArray) {
                        if ([com.curOTSMessage.UUIDString isEqualToString:[NSString stringWithFormat:@"%04X",Bluetooth16BitsUUID_objectActionControlPoint]] && com.curOTSMessage.opCode == response.requestOpCode) {
                            if (com.responseCallback) {
                                com.responseCallback((OACPBaseModel *)response, nil);
                            }
                            [weakSelf cleanCommand:com];
                            [weakSelf trySendNextCommand];
                            break;
                        }
                    }
                }
            } else if ([characteristic.UUID.UUIDString.uppercaseString isEqualToString:[NSString stringWithFormat:@"%04X",Bluetooth16BitsUUID_objectListControlPoint]]) {
                //OLCP
                OLCPResponseCodeModel *response = [[OLCPResponseCodeModel alloc] initWithParameters:characteristic.value];
                if (response) {
                    NSArray *temArray = [NSArray arrayWithArray:weakSelf.commands];
                    for (OTSCommand *com in temArray) {
                        if ([com.curOTSMessage.UUIDString isEqualToString:[NSString stringWithFormat:@"%04X",Bluetooth16BitsUUID_objectListControlPoint]] && com.curOTSMessage.opCode == response.requestOpCode) {
                            if (com.responseCallback) {
                                com.responseCallback((OACPBaseModel *)response, nil);
                            }
                            [weakSelf cleanCommand:com];
                            [weakSelf trySendNextCommand];
                            break;
                        }
                    }
                }
            }
        }
    }];
}

- (void)setClickForOTSCommandTimeout:(OTSCommand *)command {
    __weak typeof(self) weakSelf = self;
    BackgroundTimer *timer = [BackgroundTimer scheduledTimerWithTimeInterval:command.timeout repeats:NO block:^(BackgroundTimer * _Nonnull t) {
        [weakSelf cleanCommand:command];
        TelinkLogDebug(@"timeout command:%@-%@",command.curOTSMessage,command.curOTSMessage.payload);
        NSError *error = [NSError errorWithDomain:@"stop wait response, because command is timeout." code:-1 userInfo:nil];
        if (command && command.responseCallback) {
            dispatch_async(dispatch_get_main_queue(), ^{
                command.responseCallback(nil, error);
            });
        }
        [weakSelf trySendNextCommand];
    }];
    command.timeoutTimer = timer;
}

- (void)trySendNextCommand {
    if (_commands && _commands.count > 0) {
        [self sendOTSCommand:_commands.firstObject];
    }
}

- (void)cleanCommand:(OTSCommand *)command {
    if (command.timeoutTimer) {
        [command.timeoutTimer invalidate];
        command.timeoutTimer = nil;
    }
    [self.commands removeObject:command];
}

#pragma mark - Public

/**
 *  @brief  Singleton method
 *
 *  @return the default singleton instance. You are not allowed to create your own instances of this class.
 */
+ (instancetype)share {
    /// Singleton instance
    static OTSHandle *shareHandle = nil;
    /// Note: The dispatch_once function can ensure that a certain piece
    /// of code is only executed once in the entire application life cycle!
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        /// Initialize the Singleton configure parameters.
        shareHandle = [[OTSHandle alloc] init];
        shareHandle.commands = [NSMutableArray array];
        shareHandle.queue = dispatch_queue_create("OTSHandle.queue(OTS消息收发队列)", DISPATCH_QUEUE_SERIAL);
    });
    return shareHandle;
}

/// send OTSCommand api
/// @param command command
- (void)sendOTSCommand:(OTSCommand *)command {
    [self setBlock];
    [self.commands addObject:command];
    if (_commands.count > 1) {
        TelinkLogInfo(@"The current command has been added to the queue, and there are %d %@ ahead. Please wait until the previous command processing is completed.",self.commands.count-1,self.commands.count-1>1?@"commands":@"command");
    } else {
        CBCharacteristic *c = [self getCBCharacteristicWithUUIDString:command.curOTSMessage.UUIDString ofPeripheral:self.peripheral];
        if (c) {
            __weak typeof(self) weakSelf = self;
            dispatch_async(_queue, ^{
                [weakSelf.bluetooth writeValue:command.curOTSMessage.payload toPeripheral:weakSelf.peripheral forCharacteristic:c type:CBCharacteristicWriteWithResponse];
                [weakSelf setClickForOTSCommandTimeout:command];
            });
        }
    }
}

/// Cancels sending the message with the given OTSCommand.
/// @param command The message identifier.
- (void)cancelOTSCommand:(OTSCommand *)command {
    [self cleanCommand:command];
}

/// cancel all commands.
- (void)cleanAllCommands {
    NSArray *commands = [NSArray arrayWithArray:_commands];
    for (OTSCommand *com in commands) {
        [self cleanCommand:com];
        [self.commands removeObject:com];
    }
}

/// Returns whether OTSHandle is busy. YES means busy.
- (BOOL)isBusyNow {
    return _commands.count > 0;
}

/**
 * @brief   Get Characteristic of peripheral.
 * @param   uuidString UUIDString of Characteristic.
 * @param   peripheral the CBPeripheral object.
 * @return  A CBCharacteristic object.
 */
- (nullable CBCharacteristic *)getCBCharacteristicWithUUIDString:(NSString *)uuidString ofPeripheral:(CBPeripheral *)peripheral {
    CBCharacteristic *c = nil;
    for (CBService *ser in peripheral.services) {
        for (CBCharacteristic *cha in ser.characteristics) {
            if ([cha.UUID.UUIDString.uppercaseString isEqualToString:uuidString.uppercaseString]) {
                c = cha;
                break;
            }
        }
        if (c) {
            break;
        }
    }
    return c;
}

@end
