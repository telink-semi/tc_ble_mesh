/********************************************************************************************************
 * @file     CDTPClientModel.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/6/30
 *
 * @par     Copyright (c) 2023, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "CDTPClientModel.h"
#import "CBCharacteristic+UUID.h"
#import "OTSBaseModel.h"
#import "OTSHandle.h"

@interface CDTPClientModel ()
/// The private SigBluetooth object for scan CDTP Service.
@property (nonatomic, strong) SigBluetooth *privateBluetooth;
/// The CBPeripheral object of CDTP Service.
@property (nonatomic, strong) CBPeripheral *servicePeripheral;
/// 3.1 OTSFeatureCharacteristic
/// The OTS Feature characteristic exposes which optional features are supported by the Server implementation.
@property (nonatomic, strong) CBCharacteristic *otsFeatureCharacteristic;
/// 3.2.2 Object Name Characteristic
/// The Object Name characteristic exposes the name of the Current Object.
@property (nonatomic, strong) CBCharacteristic *objectNameCharacteristic;
/// 3.2.3 Object Type Characteristic
/// This characteristic exposes the type of the Current Object, identifying the object type by UUID.
/// Relevant object types that are to be used for a given application may be specified by a higher-level specification.
@property (nonatomic, strong) CBCharacteristic *objectTypeCharacteristic;
/// 3.2.4 Object Size Characteristic
/// The Object Size characteristic exposes the current size as well as the allocated size of the Current Object.
@property (nonatomic, strong) CBCharacteristic *objectSizeCharacteristic;
/// 3.2.5 Object First-Created Characteristic
/// The Object First-Created characteristic exposes a value representing a date and time when the object contents were
/// first created. Note that this may be an earlier date than the date of the creation of the object on the present Server; it
/// can relate to the original file creation date of a firmware release, for instance, rather than the time that the object was
/// written to the Server.
/// The date and time value shall be represented in Coordinated Universal Time (UTC).
/// Note that, once the value of the object first-created metadata has been set to a valid date and time (i.e., populated with
/// non-zero values), its value is not intended to change any further, since the time of first creation of an object is a constant
/// by definition.
@property (nonatomic, strong) CBCharacteristic *objectFirstCreatedCharacteristic;
/// 3.2.6 Object Last-Modified Characteristic
/// The Object Last-Modified characteristic exposes a value representing a date and time when the object content was last
/// modified.
/// The date and time value shall be represented in Coordinated Universal Time (UTC).
@property (nonatomic, strong) CBCharacteristic *objectLastModifiedCharacteristic;
/// 3.2.8 Object Properties Characteristic
/// This characteristic exposes a bit field representing the properties of the Current Object. When a bit is set to 1 (True),
/// the Current Object possesses the associated property as defined in [3].
/// An object’s properties determine the operations that are permitted to be performed on that object, as described in Section 3.3.
/// For example, an attempt to use the OACP Write Op Code when the Write bit of the properties for the Current Object is 0 (False)
/// will result in an error response.
@property (nonatomic, strong) CBCharacteristic *objectPropertiesCharacteristic;
/// 3.3 ObjectActionControlPoint(OACP)
@property (nonatomic, strong) CBCharacteristic *objectActionControlPointCharacteristic;
/// Description
/// An L2CAP channel is a duplex byte stream interface (similar to a network socket) that can be used for much more efficient
/// binary data transfer. This is used in some streaming applications, such as the Bluetooth Object Transfer Service.
/// The PSM (protocol/service multiplexer) is specified by the peripheral when it opens the channel. Some channels have predefined
/// identifiers controlled by Bluetooth organisation. Apple has also outlined a generic design for PSM exchange. To advertise an
/// L2CAP channel on a specific service, a characteristic with the UUID "ABDD3056-28FA-441D-A470-55A75A52553A" is added to
/// that service, and updated by the peripheral when the L2CAP channel is opened.
/// - seeAlso: https://classic.yarnpkg.com/en/package/cordova-plugin-ble-central#readme,l2cap.open.
@property (nonatomic, strong) CBCharacteristic *psmCharacteristic;
/// The ChannelModel object.
@property (nonatomic, strong, nullable) ChannelModel *channelModel;
/// Marks whether the CDTP client is reading Object data.
@property (nonatomic, assign) BOOL isReading;
/// Marks whether the CDTP client is writing Object data.
@property (nonatomic, assign) BOOL isWriting;
@property (nonatomic, retain) dispatch_semaphore_t semaphore;
/// Cache the value of oacp feature.
@property (nonatomic, assign) struct OACPFeatures serviceOacpFeature;
@end

@implementation CDTPClientModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _privateBluetooth = [[SigBluetooth alloc] init];
    }
    return self;
}

- (void)respondsClientReadFailWithError:(NSError *)error {
    OTSHandle.share.bluetooth = nil;
    [self cancelSignal];
    dispatch_async(dispatch_get_main_queue(), ^{
        if ([self.delegate respondsToSelector:@selector(onClientReadFinishWithData:error:)]) {
            [self.delegate onClientReadFinishWithData:nil error:error];
        }
    });
}

- (void)respondsClientReadSuccessfulWithData:(NSData *)data {
    if (self.serviceOacpFeature.OACPCalculateChecksumOpCodeSupported == 1) {
        __weak typeof(self) weakSelf = self;
        [self writeOACPCalculateChecksumWithOffset:0 length:(UInt32)data.length complete:^(OACPResponseCodeModel * _Nullable responseMessage, NSError * _Nullable error) {
            if (error == nil && responseMessage.resultCode == OACPResultCode_Success) {
                NSData *checksumOfService = responseMessage.responseParameter;
                UInt32 crc = [LibTools getCRC32OfData:data];
                NSData *checksumOfClient = [NSData dataWithBytes:&crc length:4];
                if ([checksumOfService isEqualToData:checksumOfClient]) {
                    [weakSelf clientReadSuccessAction:data];
                } else {
                    NSError *error = [NSError errorWithDomain:@"Compare Checksum fail" code:-1 userInfo:nil];
                    [weakSelf respondsClientReadFailWithError:error];
                }
            } else {
                NSError *error = [NSError errorWithDomain:@"Get Checksum fail" code:-1 userInfo:nil];
                [weakSelf respondsClientReadFailWithError:error];
            }
        }];
    } else {
        [self clientReadSuccessAction:data];
    }
}

- (void)clientReadSuccessAction:(NSData *)data {
    OTSHandle.share.bluetooth = nil;
    dispatch_async(dispatch_get_main_queue(), ^{
        if ([self.delegate respondsToSelector:@selector(onClientReadFinishWithData:error:)]) {
            [self.delegate onClientReadFinishWithData:data error:nil];
        }
    });
}

- (void)respondsClientWriteFailWithError:(NSError *)error {
    OTSHandle.share.bluetooth = nil;
    [self cancelSignal];
    dispatch_async(dispatch_get_main_queue(), ^{
        if ([self.delegate respondsToSelector:@selector(onClientWriteFinishWithError:)]) {
            [self.delegate onClientWriteFinishWithError:error];
        }
    });
}

- (void)respondsClientWriteSuccessful {
    OTSHandle.share.bluetooth = nil;
    dispatch_async(dispatch_get_main_queue(), ^{
        if ([self.delegate respondsToSelector:@selector(onClientWriteFinishWithError:)]) {
            [self.delegate onClientWriteFinishWithError:nil];
        }
    });
}

- (void)readOTSFeature {
    TelinkLogInfo(@"");
    __weak typeof(self) weakSelf = self;
    [self.privateBluetooth readCharachteristicWithCharacteristic:self.otsFeatureCharacteristic ofPeripheral:self.servicePeripheral timeout:60.0 complete:^(CBCharacteristic * _Nonnull characteristic, BOOL successful) {
        if (characteristic == self.otsFeatureCharacteristic && characteristic.value.length >= 4) {
            TelinkLogInfo(@"");
            struct OACPFeatures feature = {};
            feature.value = 0;
            Byte *dataByte = (Byte *)characteristic.value.bytes;
            memcpy(&feature.value, dataByte, 4);
            weakSelf.serviceOacpFeature = feature;
            [weakSelf performSelector:@selector(readAction) withObject:nil afterDelay:0.2];
        }
    }];
}

- (void)readAction {
    TelinkLogInfo(@"");
    [self.channelModel openStreamAction];
    __weak typeof(self) weakSelf = self;
    [self.privateBluetooth readCharachteristicWithCharacteristic:self.objectSizeCharacteristic ofPeripheral:self.servicePeripheral timeout:60.0 complete:^(CBCharacteristic * _Nonnull characteristic, BOOL successful) {
        if (characteristic == self.objectSizeCharacteristic) {
            TelinkLogInfo(@"");
            if (characteristic.value && characteristic.value.length >= 4) {
                UInt32 size = 0;
                Byte *dataByte = (Byte *)characteristic.value.bytes;
                memcpy(&size, dataByte, 4);
                [weakSelf readObjectWithSize:size];
            }
        }
    }];
}

- (void)readObjectWithSize:(UInt32)size {
    TelinkLogInfo(@"back object size = %d", size);
    __weak typeof(self) weakSelf = self;
    [weakSelf writeOACPReadWithOffset:0 length:size progress:^(float progress) {
        dispatch_async(dispatch_get_main_queue(), ^{
            if ([weakSelf.delegate respondsToSelector:@selector(onClientReadProgress:)]) {
                [weakSelf.delegate onClientReadProgress:progress];
            }
        });
    } complete:^(NSData * _Nullable data) {
        if (data == nil || data.length != size) {
            weakSelf.isReading = NO;
            NSError *error = [NSError errorWithDomain:@"Read Stream is error" code:-1 userInfo:nil];
            [weakSelf respondsClientReadFailWithError:error];
        } else {
            //read finish
            weakSelf.isReading = NO;
            [weakSelf respondsClientReadSuccessfulWithData:data];
        }
    }];
}

- (void)writeObjectWithObjectModel:(ObjectModel *)object {
    __weak typeof(self) weakSelf = self;
    NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
    [operationQueue addOperationWithBlock:^{
        __block BOOL isFail = NO;
        weakSelf.semaphore = dispatch_semaphore_create(0);

        [weakSelf.privateBluetooth readCharachteristicWithCharacteristic:weakSelf.objectSizeCharacteristic ofPeripheral:weakSelf.servicePeripheral timeout:60.0 complete:^(CBCharacteristic * _Nonnull characteristic, BOOL successful) {
            if (characteristic == weakSelf.objectSizeCharacteristic) {
                if (characteristic.value && characteristic.value.length >= 8) {
                    UInt32 size = 0;
                    Byte *dataByte = (Byte *)characteristic.value.bytes;
                    memcpy(&size, dataByte+4, 4);
                    if (size >= object.objectSize) {
                        //service端有足够空间来写入Object。
                    } else {
                        isFail = YES;
                    }
                } else {
                    isFail = YES;
                }
                dispatch_semaphore_signal(weakSelf.semaphore);
            }
        }];
        dispatch_semaphore_wait(weakSelf.semaphore, DISPATCH_TIME_FOREVER);
        if (isFail) {
            weakSelf.isWriting = NO;
            NSError *error = [NSError errorWithDomain:@"Read objectSizeCharacteristic is error" code:-1 userInfo:nil];
            [weakSelf respondsClientWriteFailWithError:error];
            return;
        }

        [weakSelf writeOACPWriteWithObjectData:object.objectData offset:0 length:object.objectSize truncate:NO progress:^(float progress) {
            dispatch_async(dispatch_get_main_queue(), ^{
                if ([weakSelf.delegate respondsToSelector:@selector(onClientWriteProgress:)]) {
                    [weakSelf.delegate onClientWriteProgress:progress];
                }
            });
        } complete:^(NSData * _Nullable data) {
            if (data == nil || data.length != object.objectSize) {
                isFail = YES;
            }
            dispatch_semaphore_signal(weakSelf.semaphore);
        }];
        dispatch_semaphore_wait(weakSelf.semaphore, DISPATCH_TIME_FOREVER);
        if (isFail) {
            weakSelf.isWriting = NO;
            NSError *error = [NSError errorWithDomain:@"Write Stream is error" code:-1 userInfo:nil];
            [weakSelf respondsClientWriteFailWithError:error];
        } else {
            //write finish, 不延时，设备会在接收完data后在回复response。
            [weakSelf writePrivateOACPCalculateChecksumWithOffset:0 length:object.objectSize crc32:[LibTools getCRC32OfData:object.objectData] complete:^(OACPResponseCodeModel * _Nullable responseMessage, NSError * _Nullable error) {
                if (error == nil && responseMessage.resultCode == OACPResultCode_Success) {
                    NSData *checksumOfService = responseMessage.responseParameter;
                    UInt32 crc = [LibTools getCRC32OfData:object.objectData];
                    NSData *checksumOfClient = [NSData dataWithBytes:&crc length:4];
                    if ([checksumOfService isEqualToData:checksumOfClient]) {
                        [weakSelf respondsClientWriteSuccessful];
                    } else {
                        NSError *error = [NSError errorWithDomain:@"Compare Checksum fail" code:-1 userInfo:nil];
                        [weakSelf respondsClientWriteFailWithError:error];
                    }
                } else {
                    NSError *error = [NSError errorWithDomain:@"Get Checksum fail" code:-1 userInfo:nil];
                    [weakSelf respondsClientWriteFailWithError:error];
                }
            }];
        }
    }];
}

- (void)connectAndOpenNotifyOpenChannelWithResultBlock:(bleConnectPeripheralCallback)block {
    __weak typeof(self) weakSelf = self;
    NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
    [operationQueue addOperationWithBlock:^{
        [weakSelf.privateBluetooth connectPeripheralWithError:self.servicePeripheral timeout:60.0 resultBlock:^(CBPeripheral * _Nonnull peripheral, NSError * _Nullable error) {
            if (error == nil) {
                [weakSelf.privateBluetooth discoverServicesOfPeripheral:peripheral timeout:60.0 resultBlock:^(CBPeripheral * _Nonnull peripheral, BOOL successful) {
                    if (successful) {
                        weakSelf.otsFeatureCharacteristic = [weakSelf.privateBluetooth getCharacteristicWithUUIDString:[NSString stringWithFormat:@"%04X",Bluetooth16BitsUUID_OTSFeature] OfPeripheral:weakSelf.servicePeripheral];
                        weakSelf.objectNameCharacteristic = [weakSelf.privateBluetooth getCharacteristicWithUUIDString:[NSString stringWithFormat:@"%04X",Bluetooth16BitsUUID_objectName] OfPeripheral:weakSelf.servicePeripheral];
                        weakSelf.objectTypeCharacteristic = [weakSelf.privateBluetooth getCharacteristicWithUUIDString:[NSString stringWithFormat:@"%04X",Bluetooth16BitsUUID_objectType] OfPeripheral:weakSelf.servicePeripheral];
                        weakSelf.objectSizeCharacteristic = [weakSelf.privateBluetooth getCharacteristicWithUUIDString:[NSString stringWithFormat:@"%04X",Bluetooth16BitsUUID_objectSize] OfPeripheral:weakSelf.servicePeripheral];
                        weakSelf.objectFirstCreatedCharacteristic = [weakSelf.privateBluetooth getCharacteristicWithUUIDString:[NSString stringWithFormat:@"%04X",Bluetooth16BitsUUID_objectFirstCreated] OfPeripheral:weakSelf.servicePeripheral];
                        weakSelf.objectLastModifiedCharacteristic= [weakSelf.privateBluetooth getCharacteristicWithUUIDString:[NSString stringWithFormat:@"%04X",Bluetooth16BitsUUID_objectLastModified] OfPeripheral:weakSelf.servicePeripheral];
                        weakSelf.objectPropertiesCharacteristic = [weakSelf.privateBluetooth getCharacteristicWithUUIDString:[NSString stringWithFormat:@"%04X",Bluetooth16BitsUUID_objectProperties] OfPeripheral:weakSelf.servicePeripheral];
                        weakSelf.objectActionControlPointCharacteristic = [weakSelf.privateBluetooth getCharacteristicWithUUIDString:[NSString stringWithFormat:@"%04X",Bluetooth16BitsUUID_objectActionControlPoint] OfPeripheral:weakSelf.servicePeripheral];
                        weakSelf.psmCharacteristic = [weakSelf.privateBluetooth getCharacteristicWithUUIDString:CBUUIDL2CAPPSMCharacteristicString OfPeripheral:weakSelf.servicePeripheral];

                        if (weakSelf.objectActionControlPointCharacteristic && ((weakSelf.objectActionControlPointCharacteristic.properties & CBCharacteristicPropertyNotify) || (weakSelf.objectActionControlPointCharacteristic.properties & CBCharacteristicPropertyIndicate))) {
                            [weakSelf.privateBluetooth changeNotifyToState:YES Peripheral:peripheral characteristic:weakSelf.objectActionControlPointCharacteristic timeout:60.0 resultBlock:^(CBPeripheral * _Nonnull peripheral, CBCharacteristic * _Nonnull characteristic, NSError * _Nullable error) {
                                if (error == nil) {
                                    if (weakSelf.psmCharacteristic) {
                                        [weakSelf.privateBluetooth readCharachteristicWithCharacteristic:weakSelf.psmCharacteristic ofPeripheral:weakSelf.servicePeripheral timeout:60.0 complete:^(CBCharacteristic * _Nonnull characteristic, BOOL successful) {
                                            if (successful) {
                                                [weakSelf openChannelWithPSM:[LibTools uint16FromBytes:characteristic.value] ResultBlock:block];
                                            } else {
                                                //弹出配对框，不点，直到超时，进入这里。
                                                if (block) {
                                                    block(peripheral, NO);
                                                }
                                            }
                                        }];
                                    } else {
                                        [weakSelf openChannelWithPSM:0x25 ResultBlock:block];
                                    }
                                } else {
                                    if (block) {
                                        block(peripheral, NO);
                                    }
                                }
                            }];
                        } else {
                            if (block) {
                                block(peripheral, NO);
                            }
                        }
                    } else {
                        if (block) {
                            block(peripheral, NO);
                        }
                    }
                }];
            } else {
                [weakSelf respondsClientReadFailWithError:error];
            }
        }];
    }];
}

- (void)openChannelWithPSM:(CBL2CAPPSM)psm ResultBlock:(bleConnectPeripheralCallback)block {
    //    TelinkLogError(@"不触发openChannel！！！！！！");
    __weak typeof(self) weakSelf = self;
    [self.privateBluetooth openChannelWithPeripheral:self.servicePeripheral PSM:psm timeout:60.0 resultBlock:^(CBPeripheral * _Nonnull peripheral, CBL2CAPChannel * _Nullable channel, NSError * _Nullable error) {
        if (error == nil) {
            weakSelf.channelModel = [[ChannelModel alloc] initWithChannel:channel];
            if (block) {
                block(weakSelf.servicePeripheral, YES);
            }
        } else {
            weakSelf.channelModel = nil;
            if (block) {
                block(weakSelf.servicePeripheral, NO);
            }
        }
    }];
}

- (void)cancelSignal {
    if (self.semaphore) {
        dispatch_semaphore_signal(self.semaphore);
    }
}

#pragma clang diagnostic pop

#pragma mark - BLE

- (void)writeDataWithCBCharacteristic:(CBCharacteristic *)characteristic data:(NSData *)data {
    if (characteristic) {
        [self.servicePeripheral writeValue:data forCharacteristic:characteristic type:CBCharacteristicWriteWithResponse];
    }
}

- (void)readObjectNameWithComplete:(bleReadOTACharachteristicCallback)complete {
    [self.privateBluetooth readCharachteristicWithCharacteristic:self.objectNameCharacteristic ofPeripheral:self.servicePeripheral timeout:60.0 complete:complete];
}

- (void)readOTSFeatureWithComplete:(bleReadOTACharachteristicCallback)complete {
    [self.privateBluetooth readCharachteristicWithCharacteristic:self.otsFeatureCharacteristic ofPeripheral:self.servicePeripheral timeout:60.0 complete:complete];
}

- (void)readObjectPropertiesWithComplete:(bleReadOTACharachteristicCallback)complete {
    [self.privateBluetooth readCharachteristicWithCharacteristic:self.objectPropertiesCharacteristic ofPeripheral:self.servicePeripheral timeout:60.0 complete:complete];
}

- (void)readObjectTypeWithComplete:(bleReadOTACharachteristicCallback)complete {
    [self.privateBluetooth readCharachteristicWithCharacteristic:self.objectTypeCharacteristic ofPeripheral:self.servicePeripheral timeout:60.0 complete:complete];
}

- (void)readObjectSizeWithComplete:(bleReadOTACharachteristicCallback)complete {
    [self.privateBluetooth readCharachteristicWithCharacteristic:self.objectSizeCharacteristic ofPeripheral:self.servicePeripheral timeout:60.0 complete:complete];
}

- (void)readObjectFirstCreatedWithComplete:(bleReadOTACharachteristicCallback)complete {
    [self.privateBluetooth readCharachteristicWithCharacteristic:self.objectFirstCreatedCharacteristic ofPeripheral:self.servicePeripheral timeout:60.0 complete:complete];
}

- (void)readObjectLastModifiedWithComplete:(bleReadOTACharachteristicCallback)complete {
    [self.privateBluetooth readCharachteristicWithCharacteristic:self.objectLastModifiedCharacteristic ofPeripheral:self.servicePeripheral timeout:60.0 complete:complete];
}

- (void)writeObjectName:(NSData *)nameData {
    [self writeDataWithCBCharacteristic:self.objectNameCharacteristic data:nameData];
}

/*
 //2017-11-12 01:00:30
 GattDateTimeModel *model = [[GattDateTimeModel alloc] initWithYear:2017 month:11 day:12 hours:1 minutes:0 seconds:30];
 */
/// 3.2.5 Object First-Created Characteristic
/// - seeAlso: OTS_v10.pdf  (page.20)
- (void)writeObjectFirstCreatedWithGattDateTimeModel:(GattDateTimeModel *)dateTimeModel {
    NSData *data = dateTimeModel.parameters;
    [self writeDataWithCBCharacteristic:self.objectFirstCreatedCharacteristic data:data];
}

/*
 //2017-11-12 01:00:30
 GattDateTimeModel *model = [[GattDateTimeModel alloc] initWithYear:2017 month:11 day:12 hours:1 minutes:0 seconds:30];
 */
/// 3.2.6 Object Last-Modified Characteristic
/// - seeAlso: OTS_v10.pdf  (page.20)
- (void)writeObjectLastModifiedWithGattDateTimeModel:(GattDateTimeModel *)dateTimeModel {
    NSData *data = dateTimeModel.parameters;
    [self writeDataWithCBCharacteristic:self.objectLastModifiedCharacteristic data:data];
}

- (void)writeObjectPropertiesWithObjectProperties:(struct ObjectProperties)properties {
    NSData *data = [NSData dataWithBytes:&properties length:sizeof(properties)];
    [self writeDataWithCBCharacteristic:self.objectPropertiesCharacteristic data:data];
}

#pragma mark - OACP

- (void)writeOACPCreateObjectWithSize:(UInt32)size type:(NSData *)type complete:(_Nullable ResponseOfOACPMessageBlock)block {
    OTSCommand *command = [[OTSCommand alloc] initOACPCreateCommandWithSize:size type:type timeout:5.0 responseCallback:block];
    [OTSHandle.share sendOTSCommand:command];
}

- (void)writeOACPDeleteObjectWithComplete:(_Nullable ResponseOfOACPMessageBlock)block {
    OTSCommand *command = [[OTSCommand alloc] initOACPDeleteCommandWithTimeout:5.0 responseCallback:^(OACPResponseCodeModel * _Nullable responseMessage, NSError * _Nullable error) {
        TelinkLogInfo(@"OACPDeleteCommand response=%@, payload=%@, error=%@", responseMessage, responseMessage.payload, error);
        if (block) {
            block(responseMessage, error);
        }
    }];
    [OTSHandle.share sendOTSCommand:command];
}

- (void)writeOACPCalculateChecksumWithOffset:(UInt32)offset length:(UInt32)length complete:(_Nullable ResponseOfOACPMessageBlock)block {
    OTSCommand *command = [[OTSCommand alloc] initOACPCalculateChecksumCommandWithOffset:offset length:length timeout:5.0 responseCallback:block];
    [OTSHandle.share sendOTSCommand:command];
}

- (void)writePrivateOACPCalculateChecksumWithOffset:(UInt32)offset length:(UInt32)length crc32:(UInt32)crc32 complete:(_Nullable ResponseOfOACPMessageBlock)block {
    OTSCommand *command = [[OTSCommand alloc] initPrivateOACPCalculateChecksumCommandWithOffset:offset length:length crc32:crc32 timeout:30.0 responseCallback:block];
    [OTSHandle.share sendOTSCommand:command];
}

- (void)writeOACPExecuteObject {
    OTSCommand *command = [[OTSCommand alloc] initOACPExecuteCommandWithTimeout:5.0 responseCallback:^(OACPResponseCodeModel * _Nullable responseMessage, NSError * _Nullable error) {
        TelinkLogInfo(@"OACPExecuteCommand response=%@, payload=%@, error=%@", responseMessage, responseMessage.payload, error);
    }];
    [OTSHandle.share sendOTSCommand:command];
}

- (void)writeOACPReadWithOffset:(UInt32)offset length:(UInt32)length progress:(ProgressCallback)progress complete:(_Nullable CompleteCallback)block {
    TelinkLogInfo(@"");
    __weak typeof(self) weakSelf = self;
    OTSCommand *command = [[OTSCommand alloc] initOACPReadCommandWithOffset:offset length:length timeout:5.0 responseCallback:^(OACPResponseCodeModel * _Nullable responseMessage, NSError * _Nullable error) {
        TelinkLogInfo(@"OACPReadCommand response=%@, payload=%@, error=%@", responseMessage, responseMessage.payload, error);
        if (responseMessage.resultCode == OACPResultCode_Success) {
            [weakSelf.channelModel readStreamWithSize:length progress:progress complete:block];
        } else {
            if (block) {
                block(nil);
            }
        }
    }];
    [OTSHandle.share sendOTSCommand:command];
}

- (void)writeOACPWriteWithObjectData:(NSData *)objectData offset:(UInt32)offset length:(UInt32)length truncate:(BOOL)truncate progress:(ProgressCallback)progress complete:(_Nullable CompleteCallback)block {
    TelinkLogInfo(@"");
    __weak typeof(self) weakSelf = self;
    OTSCommand *command = [[OTSCommand alloc] initOACPWriteCommandWithOffset:offset length:length truncate:truncate timeout:60.0 responseCallback:^(OACPResponseCodeModel * _Nullable responseMessage, NSError * _Nullable error) {
        //触发write object content成功，开始通过outputStream写object content。
        //{length = 3, bytes = 0x600601}
        TelinkLogInfo(@"OACPWriteCommand response=%@, payload=%@, error=%@", responseMessage, responseMessage.payload, error);
        if (responseMessage.resultCode == OACPResultCode_Success) {
            [weakSelf.channelModel writeStreamWithData:objectData progress:progress complete:block];
        } else {
            if (block) {
                block(nil);
            }
        }
    }];
    [OTSHandle.share sendOTSCommand:command];
}

- (void)writeOACPAbortObject {
    OTSCommand *command = [[OTSCommand alloc] initOACPAbortCommandWithTimeout:5.0 responseCallback:^(OACPResponseCodeModel * _Nullable responseMessage, NSError * _Nullable error) {
        TelinkLogInfo(@"OACPAbortCommand response=%@, payload=%@, error=%@", responseMessage, responseMessage.payload, error);
    }];
    [OTSHandle.share sendOTSCommand:command];
}

- (BOOL)handleCheckSumData:(NSData *)checkSumData objectContentsData:(NSData *)objectContentsData {
    UInt32 tem32 = 0;
    Byte *dataByte = (Byte *)checkSumData.bytes;
    memcpy(&tem32, dataByte, 4);
    UInt32 crc32 = [LibTools getCRC32OfData:objectContentsData];
    return crc32 == tem32;
}

#pragma mark - OLCP

- (void)writeOLCPFirst {
    OTSCommand *command = [[OTSCommand alloc] initOLCPFirstCommandWithTimeout:5.0 responseCallback:^(OLCPResponseCodeModel * _Nullable responseMessage, NSError * _Nullable error) {
        TelinkLogInfo(@"OLCPFirstCommand response=%@, payload=%@, error=%@", responseMessage, responseMessage.payload, error);
    }];
    [OTSHandle.share sendOTSCommand:command];
}

- (void)writeOLCPLast {
    OTSCommand *command = [[OTSCommand alloc] initOLCPLastCommandWithTimeout:5.0 responseCallback:^(OLCPResponseCodeModel * _Nullable responseMessage, NSError * _Nullable error) {
        TelinkLogInfo(@"OLCPLastCommand response=%@, payload=%@, error=%@", responseMessage, responseMessage.payload, error);
    }];
    [OTSHandle.share sendOTSCommand:command];
}

- (void)writeOLCPPrevious {
    OTSCommand *command = [[OTSCommand alloc] initOLCPPreviousCommandWithTimeout:5.0 responseCallback:^(OLCPResponseCodeModel * _Nullable responseMessage, NSError * _Nullable error) {
        TelinkLogInfo(@"OLCPPreviousCommand response=%@, payload=%@, error=%@", responseMessage, responseMessage.payload, error);
    }];
    [OTSHandle.share sendOTSCommand:command];
}

- (void)writeOLCPNext {
    OTSCommand *command = [[OTSCommand alloc] initOLCPNextCommandWithTimeout:5.0 responseCallback:^(OLCPResponseCodeModel * _Nullable responseMessage, NSError * _Nullable error) {
        TelinkLogInfo(@"OLCPNextCommand response=%@, payload=%@, error=%@", responseMessage, responseMessage.payload, error);
    }];
    [OTSHandle.share sendOTSCommand:command];
}

- (void)writeOLCPGoToWithObjectID:(NSData *)objectID {
    OTSCommand *command = [[OTSCommand alloc] initOLCPGoToCommandWithObjectID:objectID timeout:5.0 responseCallback:^(OLCPResponseCodeModel * _Nullable responseMessage, NSError * _Nullable error) {
        TelinkLogInfo(@"OLCPNextCommand response=%@, payload=%@, error=%@", responseMessage, responseMessage.payload, error);
    }];
    [OTSHandle.share sendOTSCommand:command];
}

- (void)writeOLCPOrderCommandWithOrder:(ListSortOrder)order {
    OTSCommand *command = [[OTSCommand alloc] initOLCPOrderCommandWithOrder:order timeout:5.0 responseCallback:^(OLCPResponseCodeModel * _Nullable responseMessage, NSError * _Nullable error) {
        TelinkLogInfo(@"OLCPOrderCommand response=%@, payload=%@, error=%@", responseMessage, responseMessage.payload, error);
    }];
    [OTSHandle.share sendOTSCommand:command];
}

- (void)writeOLCPRequestNumberOfObjects {
    OTSCommand *command = [[OTSCommand alloc] initOLCPRequestNumberOfObjectsCommandWithTimeout:5.0 responseCallback:^(OLCPResponseCodeModel * _Nullable responseMessage, NSError * _Nullable error) {
        TelinkLogInfo(@"OLCPRequestNumberOfObjectsCommand response=%@, payload=%@, error=%@", responseMessage, responseMessage.payload, error);
    }];
    [OTSHandle.share sendOTSCommand:command];
}

- (void)writeOLCPClearMarking {
    OTSCommand *command = [[OTSCommand alloc] initOLCPClearMarkingCommandWithTimeout:5.0 responseCallback:^(OLCPResponseCodeModel * _Nullable responseMessage, NSError * _Nullable error) {
        TelinkLogInfo(@"OLCPClearMarkingCommand response=%@, payload=%@, error=%@", responseMessage, responseMessage.payload, error);
    }];
    [OTSHandle.share sendOTSCommand:command];
}

#pragma mark - public

/**
 * @brief   Initialize CDTPClientModel object.
 * @param   result  The handle of Initialize ble success.
 * @return  return `nil` when initialize CDTPClientModel object fail.
 */
- (instancetype)initWithBleInitResult:(bleInitSuccessCallback)result {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _privateBluetooth = [[SigBluetooth alloc] init];
        [_privateBluetooth bleInit:result];
        __weak typeof(self) weakSelf = self;
        [_privateBluetooth setBluetoothDisconnectCallback:^(CBPeripheral * _Nonnull peripheral, NSError * _Nonnull error) {
            if (peripheral == weakSelf.servicePeripheral) {
                NSError *error = [NSError errorWithDomain:@"The peripheral had disconnect!" code:-1 userInfo:nil];
                if (weakSelf.isReading) {
                    weakSelf.isReading = NO;
                    [weakSelf respondsClientReadFailWithError:error];
                } else if (weakSelf.isWriting) {
                    weakSelf.isWriting = NO;
                    [weakSelf respondsClientWriteFailWithError:error];
                }
            }
        }];
    }
    return self;
}

/// CDTP Client start scan CDTP Service.
- (void)startScanCDTPService {
    TelinkLogInfo(@"");
    __weak typeof(self) weakSelf = self;
    [_privateBluetooth scanWithServiceUUIDs:@[[CBUUID UUIDWithString:kObjectTransferService]] checkNetworkEnable:NO result:^(CBPeripheral * _Nonnull peripheral, NSDictionary<NSString *,id> * _Nonnull advertisementData, NSNumber * _Nonnull RSSI, BOOL unprovisioned) {
        if ([advertisementData.allKeys containsObject:CBAdvertisementDataServiceUUIDsKey]) {
            NSArray *suuids = advertisementData[CBAdvertisementDataServiceUUIDsKey];
            if (suuids && suuids.count != 1) {
                return;
            }
            NSString *suuidString = ((CBUUID *)suuids.firstObject).UUIDString;
            if ([suuidString isEqualToString:kObjectTransferService]) {
                dispatch_async(dispatch_get_main_queue(), ^{
                    if ([weakSelf.delegate respondsToSelector:@selector(onDiscoverServicePeripheral:advertisementData:RSSI:)]) {
                        [weakSelf.delegate onDiscoverServicePeripheral:peripheral advertisementData:advertisementData RSSI:RSSI];
                    }
                });
            }
        }
    }];
}

/// CDTP Client stop scan CDTP Service.
- (void)stopScanCDTPService {
    TelinkLogInfo(@"");
    [_privateBluetooth stopScan];
}

/// CDTP Client read mesh network object from CDTP Service.
/// - Parameter peripheral: the bluetooth Peripheral object of CDTP Service.
- (void)startReadMeshObjectFromServicePeripheral:(CBPeripheral *)peripheral {
    TelinkLogInfo(@"");
    self.servicePeripheral = peripheral;
    OTSHandle.share.peripheral = peripheral;
    OTSHandle.share.bluetooth = _privateBluetooth;
    [OTSHandle.share cleanAllCommands];
    self.isReading = YES;
    [self stopScanCDTPService];
    __weak typeof(self) weakSelf = self;
    if (self.channelModel == nil) {
        [self connectAndOpenNotifyOpenChannelWithResultBlock:^(CBPeripheral * _Nonnull peripheral, BOOL successful) {
            if (successful) {
                [weakSelf readOTSFeature];
            } else {
                weakSelf.isReading = NO;
                NSError *error = [NSError errorWithDomain:@"connect fail" code:-1 userInfo:nil];
                [weakSelf respondsClientReadFailWithError:error];
            }
        }];
    } else {
        [self readOTSFeature];
    }
}

/// CDTP Client write mesh network object to CDTP Service.
/// - Parameters:
///   - object: The mesh network CDTP object.
///   - peripheral: the bluetooth Peripheral object of CDTP Service.
- (void)startWriteMeshObject:(ObjectModel *)object toServicePeripheral:(CBPeripheral *)peripheral {
    TelinkLogInfo(@"");
    self.servicePeripheral = peripheral;
    OTSHandle.share.peripheral = peripheral;
    OTSHandle.share.bluetooth = _privateBluetooth;
    [OTSHandle.share cleanAllCommands];
    self.isWriting = YES;
    [self stopScanCDTPService];
    __weak typeof(self) weakSelf = self;
    if (self.channelModel == nil) {
        [self connectAndOpenNotifyOpenChannelWithResultBlock:^(CBPeripheral * _Nonnull peripheral, BOOL successful) {
            if (successful) {
                [weakSelf writeObjectWithObjectModel:object];
            } else {
                weakSelf.isWriting = NO;
                NSError *error = [NSError errorWithDomain:@"connect fail" code:-1 userInfo:nil];
                [weakSelf respondsClientWriteFailWithError:error];
            }
        }];
    } else {
        [self writeObjectWithObjectModel:object];
    }
}

/// CDTP Client stop CDTP Share progress.
- (void)endShare {
    if (self.channelModel) {
        [self.channelModel closeStreamAction];
    }
    if (self.servicePeripheral) {
        [self.privateBluetooth cancelConnectionPeripheral:self.servicePeripheral timeout:5.0 resultBlock:^(CBPeripheral * _Nonnull peripheral, BOOL successful) {

        }];
    }
    [OTSHandle.share cleanAllCommands];
}

@end
