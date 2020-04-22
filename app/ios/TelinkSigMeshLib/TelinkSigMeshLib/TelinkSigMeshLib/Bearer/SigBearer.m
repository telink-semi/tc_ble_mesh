/********************************************************************************************************
 * @file     SigBearer.m
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
//  SigBearer.m
//  SigMeshLib
//
//  Created by Liangjiazhi on 2019/8/23.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import "SigBearer.h"
#import "ProxyProtocolHandler.h"

@implementation SigPudModel
@end


@implementation SigTransmitter
- (void)sendData:(NSData *)data ofType:(SigPduType)type {
    
}
@end


@interface SigBearer ()<SigBearerDelegate>

#pragma  mark - Properties

@property (nonatomic, strong) SigBluetooth *ble;
@property (nonatomic, strong) CBPeripheral *peripheral;
@property (nonatomic, strong) CBCharacteristic *characteristic;
///// The protocol used for segmentation and reassembly.
@property (nonatomic, strong) ProxyProtocolHandler *protocolHandler;
/// The queue of PDUs to be sent. Used if the perpheral is busy.
@property (nonatomic, strong) NSMutableArray <NSData *>*queue;
/// A flag indicating whether `open()` method was called.
@property (nonatomic, assign) BOOL isOpened;//init NO.

@property (nonatomic,copy) bearerOperationResultCallback bearerOpenCallback;
@property (nonatomic,copy) bearerOperationResultCallback bearerCloseCallback;
@property (nonatomic,copy) startMeshConnectResultBlock startMeshConnectCallback;
@property (nonatomic,copy) stopMeshConnectResultBlock stopMeshConnectCallback;

/// flag current node whether had provisioned.
@property (nonatomic, assign) BOOL provisioned;//init YES.
@property (nonatomic,strong) BackgroundTimer *autoConnectScanTimer;
@property (nonatomic, assign) BOOL hasScaned1828;//init NO.
@property (nonatomic,strong) NSMutableDictionary <CBPeripheral *,NSNumber *>*scanedPeripheralDict;

@end

@implementation SigBearer

#pragma  mark - Computed properties

+ (SigBearer *)share {
    static SigBearer *shareManager = nil;
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        shareManager = [[SigBearer alloc] init];
        shareManager.ble = [SigBluetooth share];
        shareManager.queue = [NSMutableArray array];
        shareManager.delegate = shareManager;
        shareManager.provisioned = YES;
        shareManager.hasScaned1828 = NO;
        shareManager.protocolHandler = [[ProxyProtocolHandler alloc] init];
        shareManager.scanedPeripheralDict = [[NSMutableDictionary alloc] init];
    });
    return shareManager;
}

- (void)changePeripheral:(CBPeripheral *)peripheral result:(bearerChangePeripheralCallback)block {
    if (peripheral == nil) {
        if (block) {
            block(NO);
            return;
        }
    }
    if ([self.peripheral.identifier.UUIDString isEqualToString:peripheral.identifier.UUIDString]) {
        [self blockState];
        if (block) {
            block(YES);
        }
    }else{
        BOOL isNull = self.peripheral == nil;
        self.peripheral = peripheral;
        if (isNull) {
            [self blockState];
            if (block) {
                block(YES);
            }
        }else{
            [SigBluetooth.share cancelAllConnecttionWithComplete:^{
                [self blockState];
                if (block) {
                    block(YES);
                }
            }];
        }
    }
}

- (void)changePeripheralIdentifierUuid:(NSString *)uuid result:(bearerChangePeripheralCallback)block {
    CBPeripheral *p = [SigBluetooth.share getPeripheralWithUUID:uuid];
    [self changePeripheral:p result:block];
}

- (BOOL)isOpen {
    return [self.ble getCharacteristicWithUUIDString:kPROXY_Out_CharacteristicsID OfPeripheral:self.peripheral].isNotifying && [self.ble getCharacteristicWithUUIDString:kPBGATT_Out_CharacteristicsID OfPeripheral:self.peripheral].isNotifying;
}

- (BOOL)isProvisioned {
    return _provisioned;
}

- (CBPeripheral *)getCurrentPeripheral {
    return self.peripheral;
}

#pragma  mark - Private

- (void)blockState {
    TeLogDebug(@"");
    __weak typeof(self) weakSelf = self;
    [self.ble setBluetoothCentralUpdateStateCallback:^(CBCentralManagerState state) {
        if (weakSelf.isOpened) {
            [weakSelf openWithResult:^(BOOL successful) {
                TeLogInfo(@"ble power on, open bearer %@.",successful?@"success":@"fail");
            }];
        } else {
            TeLogInfo(@"ble power.");
        }
    }];
    [self.ble setBluetoothDisconnectCallback:^(CBPeripheral * _Nonnull peripheral, NSError * _Nonnull error) {
        if ([weakSelf.dataDelegate respondsToSelector:@selector(bearer:didCloseWithError:)]) {
            [weakSelf.dataDelegate bearer:weakSelf didCloseWithError:error];
        }
        if (weakSelf.isAutoReconnect && weakSelf.isProvisioned) {
            [weakSelf startAutoConnect];
        }
//        if (error && [SigDataSource.share getNodeWithUUID:peripheral.identifier.UUIDString]) {
//            [weakSelf startAutoConnect];
//        }
    }];
    [self.ble setBluetoothIsReadyToSendWriteWithoutResponseCallback:^(CBPeripheral * _Nonnull peripheral) {
        [weakSelf shouldSendNextPacketData];
    }];
    [self.ble setBluetoothDidUpdateValueForCharacteristicCallback:^(CBPeripheral * _Nonnull peripheral, CBCharacteristic * _Nonnull characteristic) {
        if (![peripheral isEqual:weakSelf.peripheral]) {
            TeLogDebug(@"value is not notify from currentPeripheral.");
            return;
        }
        if (!characteristic || characteristic.value.length == 0) {
            TeLogDebug(@"value is empty.");
            return;
        }

        SigPudModel *message = [weakSelf.protocolHandler reassembleData:characteristic.value];
        if (message) {
            if ([weakSelf.delegate respondsToSelector:@selector(bearer:didDeliverData:ofType:)]) {
                [weakSelf.delegate bearer:weakSelf didDeliverData:message.pduData ofType:message.pduType];
            }
        }
    }];
}

- (void)shouldSendNextPacketData {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(shouldSendNextPacketData) object:nil];
    });
    if (self.queue.count == 0) {
        if (self.sendPacketFinishBlock) {
            self.sendPacketFinishBlock();
        }
        return;
    }
    NSData *packet = self.queue.firstObject;
    [self.queue removeObjectAtIndex:0];
    [self showSendData:packet forCharacteristic:self.characteristic];
    [self.peripheral writeValue:packet forCharacteristic:self.characteristic type:CBCharacteristicWriteWithoutResponse];
}

- (void)showSendData:(NSData *)data forCharacteristic:(CBCharacteristic *)characteristic {
    if ([characteristic.UUID.UUIDString isEqualToString:kPBGATT_In_CharacteristicsID]) {
        TeLogInfo(@"---> to:GATT, length:%d,value:%@",data.length,[LibTools convertDataToHexStr:data]);
    } else if ([characteristic.UUID.UUIDString isEqualToString:kPROXY_In_CharacteristicsID]) {
        TeLogInfo(@"---> to:PROXY, length:%d",data.length);
    } else if ([characteristic.UUID.UUIDString isEqualToString:kOnlineStatusCharacteristicsID]) {
        TeLogInfo(@"---> to:OnlineStatusCharacteristic, length:%d,value:%@",data.length,[LibTools convertDataToHexStr:data]);
    } else if ([characteristic.UUID.UUIDString isEqualToString:kOTA_CharacteristicsID]) {
        TeLogInfo(@"---> to:GATT-OTA, length:%d",data.length);
    } else if ([characteristic.UUID.UUIDString isEqualToString:kMeshOTA_CharacteristicsID]) {
        TeLogInfo(@"---> to:MESH-OTA, length:%d",data.length);
    } else {
        TeLogInfo(@"---> to:%@, length:%d,value:%@",characteristic.UUID.UUIDString,data.length,[LibTools convertDataToHexStr:data]);
    }
}

#pragma  mark - Public API

- (void)openWithResult:(bearerOperationResultCallback)block {
    self.bearerOpenCallback = block;
    __weak typeof(self) weakSelf = self;
    TeLogDebug(@"start connected.");
    [self.ble connectPeripheral:self.peripheral timeout:5.0 resultBlock:^(CBPeripheral * _Nonnull peripheral, BOOL successful) {
//        TeLogDebug(@"callback connected peripheral=%@,successful=%d",peripheral,successful);
        if (successful) {
            [SigBluetooth.share discoverServicesOfPeripheral:peripheral timeout:5.0 resultBlock:^(CBPeripheral * _Nonnull peripheral, BOOL successful) {
//                TeLogDebug(@"callback discoverServicesOfPeripheral peripheral=%@,successful=%d",peripheral,successful);
                if (successful) {
                    [SigBluetooth.share changeNotifyToState:YES Peripheral:peripheral characteristic:[SigBluetooth.share getCharacteristicWithUUIDString:kPBGATT_Out_CharacteristicsID OfPeripheral:peripheral] timeout:5.0 resultBlock:^(CBPeripheral * _Nonnull peripheral, BOOL successful) {
//                        TeLogDebug(@"callback 1.openNotifyOfPeripheral characteristic=%@,successful=%d",[SigBluetooth.share getCharacteristicWithUUIDString:kPBGATT_Out_CharacteristicsID OfPeripheral:peripheral],successful);
                        if (successful) {
                            [SigBluetooth.share changeNotifyToState:YES Peripheral:peripheral characteristic:[SigBluetooth.share getCharacteristicWithUUIDString:kPROXY_Out_CharacteristicsID OfPeripheral:peripheral] timeout:5.0 resultBlock:^(CBPeripheral * _Nonnull peripheral, BOOL successful) {
//                                TeLogDebug(@"callback 2.openNotifyOfPeripheral characteristic=%@,successful=%d",[SigBluetooth.share getCharacteristicWithUUIDString:kPROXY_Out_CharacteristicsID OfPeripheral:peripheral],successful);
                                [weakSelf openResult:successful];
                            }];
                        }else{
                            [weakSelf openResult:NO];
                        }
                    }];
                }else{
                    [weakSelf openResult:NO];
                }
            }];
        }else{
            if (block) {
                [weakSelf openResult:NO];
            }
        }
    }];
    _isOpened = YES;
}

- (void)closeWithResult:(bearerOperationResultCallback)block {
    self.bearerCloseCallback = block;
    __weak typeof(self) weakSelf = self;
    [self.ble cancelConnectionPeripheral:self.peripheral timeout:5.0 resultBlock:^(CBPeripheral * _Nonnull peripheral, BOOL successful) {
        TeLogDebug(@"callback disconnected peripheral=%@,successful=%d",peripheral,successful);
        [weakSelf closeResult:successful];
    }];
    _isOpened = NO;
}

- (void)openResult:(BOOL)isSuccess {
    if (isSuccess) {
        [self stopAutoConnect];
    }
    if (self.bearerOpenCallback) {
        self.bearerOpenCallback(isSuccess);
    }
}

- (void)closeResult:(BOOL)isSuccess {
    if (self.bearerCloseCallback) {
        self.bearerCloseCallback(isSuccess);
    }
}

- (void)connectAndReadServicesWithPeripheral:(CBPeripheral *)peripheral result:(bearerOperationResultCallback)result {
    __weak typeof(self) weakSelf = self;
    if ([self.getCurrentPeripheral.identifier.UUIDString isEqualToString:peripheral.identifier.UUIDString] && peripheral.state == CBPeripheralStateConnected) {
        if (result) {
            result(YES);
        }
    } else {
        [self closeWithResult:^(BOOL successful) {
            if (successful) {
                [weakSelf changePeripheral:peripheral result:^(BOOL successful) {
                    if (successful) {
                        [weakSelf openWithResult:^(BOOL successful) {
                            if (successful) {
                                if (result) {
                                    result(YES);
                                }
                            } else {
                                if (result) {
                                    result(NO);
                                }
                            }
                        }];
                    }else{
                        if (result) {
                            result(NO);
                        }
                    }
                }];
            }else{
                if (result) {
                    result(NO);
                }
            }
        }];
    }
}

- (void)sentPcakets:(NSArray <NSData *>*)packets toCharacteristic:(CBCharacteristic *)characteristic type:(CBCharacteristicWriteType)type complete:(SendPacketsFinishCallback)complete {
    self.sendPacketFinishBlock = complete;
    [self sentPcakets:packets toCharacteristic:characteristic type:type];
}

- (void)sentPcakets:(NSArray <NSData *>*)packets toCharacteristic:(CBCharacteristic *)characteristic type:(CBCharacteristicWriteType)type {
    if (packets == nil || packets.count == 0) {
        TeLogError(@"current packets is empty.");
        return;
    }
    if (characteristic == nil) {
        TeLogError(@"current characteristic is empty.");
        return;
    }

    // On iOS 11+ only the first packet is sent here. When the peripheral is ready
    // to send more data, a `peripheralIsReady(toSendWriteWithoutResponse:)` callback
    // will be called, which will send the next packet.
    if (@available(iOS 11.0, *)) {
        @synchronized (self) {
            BOOL queueWasEmpty = _queue.count == 0;
            [_queue addObjectsFromArray:packets];
            self.characteristic = characteristic;
            
            // Don't look at `basePeripheral.canSendWriteWithoutResponse`. If often returns
            // `false` even when nothing was sent before and no callback is called afterwards.
            // Just assume, that the first packet can always be sent.
            if (queueWasEmpty) {
                NSData *packet = _queue.firstObject;
                [_queue removeObjectAtIndex:0];
                [self showSendData:packet forCharacteristic:characteristic];
                dispatch_async(dispatch_get_main_queue(), ^{
                    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(shouldSendNextPacketData) object:nil];
                    // if no call back of setBluetoothIsReadyToSendWriteWithoutResponseCallback within 500ms, send next packet.
                    [self performSelector:@selector(shouldSendNextPacketData) withObject:nil afterDelay:0.5];
                });
                [SigBearer.share.getCurrentPeripheral writeValue:packet forCharacteristic:characteristic type:type];
            }
        }
    } else {
        // For iOS versions before 11, the data must be just sent in a loop.
        // This may not work if there is more than ~20 packets to be sent, as a
        // buffer may overflow. The solution would be to add some delays, but
        // let's hope it will work as is. For now.
        // TODO: Handle very long packets on iOS 9 and 10.
        for (NSData *packet in packets) {
            [self showSendData:packet forCharacteristic:characteristic];
            [SigBearer.share.getCurrentPeripheral writeValue:packet forCharacteristic:characteristic type:type];
        }
    }

}

- (void)sendBlePdu:(SigPdu *)pdu ofType:(SigPduType)type {
    if (![self isOpen]) {
        TeLogError(@"current bearer is not in notitying.");
        return;
    }
    if (!pdu || !pdu.pduData || pdu.pduData.length == 0) {
        TeLogError(@"current data is empty.");
        return;
    }
    
//    NSInteger mtu = [self.getCurrentPeripheral maximumWriteValueLengthForType:CBCharacteristicWriteWithoutResponse];
    NSInteger mtu = 20;
    NSArray *packets = [self.protocolHandler segmentWithData:pdu.pduData messageType:type mtu:mtu];
    TeLogVerbose(@"pdu.pduData.length=%d,sentPcakets:%@",pdu.pduData.length,packets);

    CBCharacteristic *c = nil;
    if (type == SigPduType_provisioningPdu) {
        c = [SigBluetooth.share getCharacteristicWithUUIDString:kPBGATT_In_CharacteristicsID OfPeripheral:SigBearer.share.getCurrentPeripheral];
    } else {
        c = [SigBluetooth.share getCharacteristicWithUUIDString:kPROXY_In_CharacteristicsID OfPeripheral:SigBearer.share.getCurrentPeripheral];
    }
    //写法1.只负责压包，运行该函数完成不等于发送完成。
    [self sentPcakets:packets toCharacteristic:c type:CBCharacteristicWriteWithoutResponse];
    //写法2.等待所有压包都发送完成
//    for (NSData *pack in packets) {
//        dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
//        [self sentPcakets:@[pack] toCharacteristic:c type:CBCharacteristicWriteWithoutResponse complete:^{
//            dispatch_semaphore_signal(semaphore);
//        }];
//        dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
//    }
}

- (void)sendOTAData:(NSData *)data {
    CBPeripheral *p = SigBearer.share.getCurrentPeripheral;
    CBCharacteristic *c = [SigBluetooth.share getCharacteristicWithUUIDString:kOTA_CharacteristicsID OfPeripheral:p];
    [p writeValue:data forCharacteristic:c type:CBCharacteristicWriteWithoutResponse];
}

- (void)setBearerProvisioned:(BOOL)provisioned {
    _provisioned = provisioned;
}

#pragma  mark - delegate
- (void)bearer:(SigBearer *)bearer didDeliverData:(NSData *)data ofType:(SigPduType)type {
    if (type == SigPduType_provisioningPdu) {
        // Try parsing the response.
        SigProvisioningResponse *response = [[SigProvisioningResponse alloc] initWithData:data];
        if (!response) {
            TeLogDebug(@"parsing the response fail.");
            return;
        }
        if (!response.isValid) {
            TeLogDebug(@"the response is not valid.");
            return;
        }
        if (SigProvisioningManager.share.provisionResponseBlock) {
            SigProvisioningManager.share.provisionResponseBlock(response);
        }
    }else{
        [SigMeshLib.share bearerDidDeliverData:data type:type];
    }
}

#pragma  mark - auto reconnect

/// 开始连接SigDataSource这个单列的mesh网络。
- (void)startMeshConnectWithComplete:(nullable startMeshConnectResultBlock)complete {
    self.startMeshConnectCallback = complete;
    self.isAutoReconnect = YES;
    if (self.getCurrentPeripheral && self.getCurrentPeripheral.state == CBPeripheralStateConnected && [SigBluetooth.share isWorkNormal] && [SigDataSource.share existPeripheralUUIDString:self.getCurrentPeripheral.identifier.UUIDString]) {
        [self startMeshConnectSuccess];
    } else {
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(meshConnectTimeout) object:nil];
            [self performSelector:@selector(meshConnectTimeout) withObject:nil afterDelay:kStartMeshConnectTimeout];
        });
        self.hasScaned1828 = NO;
        __weak typeof(self) weakSelf = self;
        [SigBluetooth.share cancelAllConnecttionWithComplete:^{
            [weakSelf startScanMeshNode];
        }];
    }
}

/// 断开一个mesh网络的连接，切换不同的mesh网络时使用。
- (void)stopMeshConnectWithComplete:(nullable stopMeshConnectResultBlock)complete {
    self.isAutoReconnect = NO;
    self.stopMeshConnectCallback = complete;
    [SigBluetooth.share stopScan];
    [self.autoConnectScanTimer invalidate];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(meshConnectTimeout) object:nil];
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(connectRssiHightestPeripheral) object:nil];
    });
    __weak typeof(self) weakSelf = self;
    [SigBluetooth.share cancelAllConnecttionWithComplete:^{
        if (weakSelf.stopMeshConnectCallback) {
            weakSelf.stopMeshConnectCallback(YES);
        }
    }];
}

- (void)startScanMeshNode {
    __weak typeof(self) weakSelf = self;
    [SigBluetooth.share scanProvisionedDevicesWithResult:^(CBPeripheral * _Nonnull peripheral, NSDictionary<NSString *,id> * _Nonnull advertisementData, NSNumber * _Nonnull RSSI, BOOL unprovisioned) {
        if (!unprovisioned) {
            [weakSelf savePeripheralToLocal:peripheral rssi:RSSI];
            if (RSSI.intValue >= -50) {
                [weakSelf connectRssiHighterPeripheral:peripheral];
            }else{
                [weakSelf scanProvisionedDevicesSuccess];
            }
        }
    }];
}

- (void)savePeripheralToLocal:(CBPeripheral *)tempPeripheral rssi:(NSNumber *)rssi{
    self.scanedPeripheralDict[tempPeripheral] = rssi;
}

- (void)scanProvisionedDevicesSuccess {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(meshConnectTimeout) object:nil];
        if (!self.hasScaned1828) {
            self.hasScaned1828 = YES;
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(connectRssiHightestPeripheral) object:nil];
            [self performSelector:@selector(connectRssiHightestPeripheral) withObject:nil afterDelay:1.0];
        }
    });
}

- (void)meshConnectTimeout {
    TeLogDebug(@"");
    __weak typeof(self) weakSelf = self;
    [self stopMeshConnectWithComplete:^(BOOL successful) {
        [weakSelf startMeshConnectFail];
    }];
}

- (void)startAutoConnect {
    TeLogInfo(@"startAutoConnect");
    [self stopAutoConnect];
    if (SigDataSource.share.curNodes.count > 0) {
        [self startMeshConnectWithComplete:self.startMeshConnectCallback];
        __weak typeof(self) weakSelf = self;
        self.autoConnectScanTimer = [BackgroundTimer scheduledTimerWithTimeInterval:kStartMeshConnectTimeout repeats:YES block:^(BackgroundTimer * _Nonnull t) {
            [weakSelf startAutoConnect];
        }];
    }
}

/// Stop auto connect(停止自动连接流程)
- (void)stopAutoConnect {
    [self.autoConnectScanTimer invalidate];
}

/// 正常扫描流程：3秒内扫描到RSSI大于“-50”的设备，直接连接。
- (void)connectRssiHighterPeripheral:(CBPeripheral *)peripheral {
    [SigBluetooth.share stopScan];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(meshConnectTimeout) object:nil];
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(connectRssiHightestPeripheral) object:nil];
    });
    [self normalConnectPeripheral:peripheral];
}

/// 正常扫描流程：扫描到第一个设备1秒后连接RSSI最大的设备
- (void)connectRssiHightestPeripheral {
    [SigBluetooth.share stopScan];
    CBPeripheral *peripheral = [self getHightestRSSIPeripheral];
    if (peripheral) {
        [self normalConnectPeripheral:peripheral];
    } else {
        TeLogError(@"逻辑异常：SDK缓存中并未扫描到设备。");
    }
}

- (CBPeripheral *)getHightestRSSIPeripheral {
    if (!self.scanedPeripheralDict || self.scanedPeripheralDict.allKeys.count == 0) {
        return nil;
    }
    CBPeripheral *temP = self.scanedPeripheralDict.allKeys.firstObject;
    int temRssi = self.scanedPeripheralDict[temP].intValue;
    for (CBPeripheral *tem in self.scanedPeripheralDict.allKeys) {
        int value = self.scanedPeripheralDict[tem].intValue;
        if (value > temRssi) {
            temRssi = value;
            temP = tem;
        }
    }
    return temP;
}

- (void)normalConnectPeripheral:(CBPeripheral *)peripheral {
    __weak typeof(self) weakSelf = self;
    [self changePeripheral:peripheral result:^(BOOL successful) {
        if (successful) {
            TeLogDebug(@"change to uuid:%@ success.",peripheral.identifier.UUIDString);
            [weakSelf openWithResult:^(BOOL successful) {
                if (successful) {
                    TeLogDebug(@"read gatt list:%@ success.",peripheral.identifier.UUIDString);
                    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.5 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                        [SDKLibCommand setFilterForProvisioner:SigDataSource.share.curProvisionerModel successCallback:^(UInt16 source, UInt16 destination, SigFilterStatus * _Nonnull responseMessage) {
                            TeLogDebug(@"set filter:%@ success.",peripheral.identifier.UUIDString);
                            [weakSelf startMeshConnectSuccess];
                        } finishCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
                            if (error) {
                                TeLogDebug(@"set filter:%@ fail.",peripheral.identifier.UUIDString);
                                [weakSelf startMeshConnectFail];
        //                        [weakSelf startAutoConnect];
                            }
                        }];
                    });
                } else {
                    TeLogDebug(@"read gatt list:%@ fail.",peripheral.identifier.UUIDString);
                    [weakSelf startMeshConnectFail];
//                        [weakSelf startAutoConnect];
                }
            }];
        } else {
            TeLogDebug(@"change to uuid:%@ fail.",peripheral.identifier.UUIDString);
            [weakSelf startMeshConnectFail];
//            [weakSelf startAutoConnect];
        }
    }];
}

- (void)startMeshConnectSuccess {
    if (SigDataSource.share.hasNodeExistTimeModelID) {
        [SDKLibCommand statusNowTime];
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.3 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            if (self.startMeshConnectCallback) {
                self.startMeshConnectCallback(YES);
            }
        });
    }else{
        if (self.startMeshConnectCallback) {
            self.startMeshConnectCallback(YES);
        }
    }
}

- (void)startMeshConnectFail {
//    if (self.startMeshConnectCallback) {
//        self.startMeshConnectCallback(NO);
//    }
    [self startAutoConnect];
}

@end
