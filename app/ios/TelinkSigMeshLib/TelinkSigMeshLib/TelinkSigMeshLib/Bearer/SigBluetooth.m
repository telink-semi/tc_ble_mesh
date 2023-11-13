/********************************************************************************************************
 * @file     SigBluetooth.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/8/16
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

#import "SigBluetooth.h"

#define kNeedDiscoverDescriptors    (YES)

@interface SigBluetooth ()<CBCentralManagerDelegate, CBPeripheralDelegate>
@property (nonatomic,strong) CBCentralManager *manager;
@property (nonatomic,strong,nullable) CBPeripheral *currentPeripheral;
@property (nonatomic,strong) NSMutableArray <CBPeripheral *>*connectedPeripherals;
@property (nonatomic,strong) CBCharacteristic *currentCharacteristic;
@property (nonatomic,strong) CBCharacteristic *OTACharacteristic;
@property (nonatomic,strong) CBCharacteristic *PBGATT_OutCharacteristic;
@property (nonatomic,strong) CBCharacteristic *PBGATT_InCharacteristic;
@property (nonatomic,strong) CBCharacteristic *PROXY_OutCharacteristic;
@property (nonatomic,strong) CBCharacteristic *PROXY_InCharacteristic;
@property (nonatomic,strong) CBCharacteristic *OnlineStatusCharacteristic;//私有定制，上报节点的状态的特征
@property (nonatomic,strong) CBCharacteristic *MeshOTACharacteristic;
@property (nonatomic,strong) CBCharacteristic *readCharacteristic;

@property (nonatomic,assign) BOOL isInitFinish;
@property (nonatomic,strong) NSMutableArray <CBUUID *>*scanServiceUUIDs;
@property (nonatomic,assign) BOOL checkNetworkEnable;
@property (nonatomic,strong) NSString *scanPeripheralUUID;

@property (nonatomic,copy) bleInitSuccessCallback bluetoothInitSuccessCallback;
//@property (nonatomic,copy) bleEnableCallback bluetoothEnableCallback;
@property (nonatomic,copy) bleScanPeripheralCallback bluetoothScanPeripheralCallback;
@property (nonatomic,copy) bleScanSpecialPeripheralCallback bluetoothScanSpecialPeripheralCallback;
@property (nonatomic,copy) bleConnectPeripheralCallback bluetoothConnectPeripheralCallback;
@property (nonatomic,copy) bleConnectPeripheralWithErrorCallback bluetoothConnectPeripheralWithErrorCallback;
@property (nonatomic,copy) bleDiscoverServicesCallback bluetoothDiscoverServicesCallback;
@property (nonatomic,copy) bleCharacteristicResultCallback bluetoothOpenNotifyCallback;
@property (nonatomic,copy) bleReadOTACharachteristicCallback bluetoothReadOTACharachteristicCallback;
@property (nonatomic,copy) bleReadOTACharachteristicCallback bluetoothReadCharachteristicCallback;
@property (nonatomic,copy) bleCancelConnectCallback bluetoothCancelConnectCallback;
@property (nonatomic,copy) bleCentralUpdateStateCallback bluetoothCentralUpdateStateCallback;
@property (nonatomic,copy) bleIsReadyToSendWriteWithoutResponseCallback bluetoothIsReadyToSendWriteWithoutResponseCallback;
@property (nonatomic,copy) bleDidUpdateValueForCharacteristicCallback bluetoothDidUpdateValueForCharacteristicCallback;
@property (nonatomic,copy) bleDidUpdateValueForCharacteristicCallback bluetoothDidUpdateOnlineStatusValueCallback;
/// 打开通道的回调
@property (nonatomic, copy, nullable) openChannelResultCallback didOpenChannelResultBlock;

@end

@implementation SigBluetooth

/**
 *  @brief  Singleton method
 *
 *  @return the default singleton instance.
 */
+ (instancetype)share {
    /// Singleton instance
    static SigBluetooth *shareBLE = nil;
    /// Note: The dispatch_once function can ensure that a certain piece
    /// of code is only executed once in the entire application life cycle!
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        /// Initialize the Singleton configure parameters.
        shareBLE = [[SigBluetooth alloc] init];
        [shareBLE configBluetooth];
    });
    return shareBLE;
}

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        [self configBluetooth];
    }
    return self;
}

- (void)configBluetooth {
    _manager = [[CBCentralManager alloc] initWithDelegate:self queue:dispatch_get_main_queue()];
    _isInitFinish = NO;
    _waitScanRseponseEnable = NO;
    _checkNetworkEnable = NO;
    _connectedPeripherals = [NSMutableArray array];
    _scanServiceUUIDs = [NSMutableArray array];
}

#pragma  mark - Public

- (void)bleInit:(bleInitSuccessCallback)result {
    TelinkLogVerbose(@"start init SigBluetooth.");
    self.bluetoothInitSuccessCallback = result;
}

/**
 * @brief   Returns whether Bluetooth has been initialized.
 * @return  YES means Bluetooth has been initialized, NO means other.
 */
- (BOOL)isBLEInitFinish {
    return self.manager.state == CBCentralManagerStatePoweredOn;
}

/**
 * @brief   Set handle of BluetoothCentralUpdateState.
 * @param   bluetoothCentralUpdateStateCallback callback of BluetoothCentralUpdateState.
 */
- (void)setBluetoothCentralUpdateStateCallback:(_Nullable bleCentralUpdateStateCallback)bluetoothCentralUpdateStateCallback {
    _bluetoothCentralUpdateStateCallback = bluetoothCentralUpdateStateCallback;
}

- (void)setBluetoothIsReadyToSendWriteWithoutResponseCallback:(bleIsReadyToSendWriteWithoutResponseCallback)bluetoothIsReadyToSendWriteWithoutResponseCallback {
    _bluetoothIsReadyToSendWriteWithoutResponseCallback = bluetoothIsReadyToSendWriteWithoutResponseCallback;
}

- (void)setBluetoothDidUpdateValueForCharacteristicCallback:(bleDidUpdateValueForCharacteristicCallback)bluetoothDidUpdateValueForCharacteristicCallback {
    _bluetoothDidUpdateValueForCharacteristicCallback = bluetoothDidUpdateValueForCharacteristicCallback;
}

- (void)setBluetoothDidUpdateOnlineStatusValueCallback:(bleDidUpdateValueForCharacteristicCallback)bluetoothDidUpdateOnlineStatusValueCallback {
    _bluetoothDidUpdateOnlineStatusValueCallback = bluetoothDidUpdateOnlineStatusValueCallback;
}

/**
 * @brief   Scan unprovisioned devices.
 * @param   result Report once when a device is scanned.
 */
- (void)scanUnprovisionedDevicesWithResult:(bleScanPeripheralCallback)result {
//    TelinkLogInfo(@"");
    [self scanWithServiceUUIDs:@[[CBUUID UUIDWithString:kPBGATTService]] checkNetworkEnable:NO result:result];
}

/**
 * @brief   Scan provisioned devices.
 * @param   result Report once when a device is scanned.
 */
- (void)scanProvisionedDevicesWithResult:(bleScanPeripheralCallback)result {
//    TelinkLogInfo(@"");
    [self scanWithServiceUUIDs:@[[CBUUID UUIDWithString:kPROXYService]] checkNetworkEnable:YES result:result];
}

/**
 * @brief   Scan devices with ServiceUUIDs.
 * @param   UUIDs ServiceUUIDs.
 * @param   checkNetworkEnable YES means the device must belong current mesh network.
 * @param   result Report once when a device is scanned.
 */
- (void)scanWithServiceUUIDs:(NSArray <CBUUID *>* _Nonnull)UUIDs checkNetworkEnable:(BOOL)checkNetworkEnable result:(bleScanPeripheralCallback)result {
    if (self.isInitFinish) {
        self.checkNetworkEnable = checkNetworkEnable;
        self.scanServiceUUIDs = [NSMutableArray arrayWithArray:UUIDs];
        self.bluetoothScanPeripheralCallback = result;
        [self.manager scanForPeripheralsWithServices:UUIDs options:nil];
    } else {
        TelinkLogError(@"Bluetooth is not power on.");
    }
}

/**
 * @brief   Scan devices with specified UUID.
 * @param   peripheralUUID uuid of peripheral.
 * @param   timeout timeout of scan.
 * @param   block Report when the specified device is scanned.
 */
- (void)scanMeshNodeWithPeripheralUUID:(NSString *)peripheralUUID timeout:(NSTimeInterval)timeout resultBlock:(bleScanSpecialPeripheralCallback)block {
    self.bluetoothScanSpecialPeripheralCallback = block;
    __weak typeof(self) weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(scanWithPeripheralUUIDTimeout) object:nil];
        [self performSelector:@selector(scanWithPeripheralUUIDTimeout) withObject:nil afterDelay:timeout];
    });
    [self scanProvisionedDevicesWithResult:^(CBPeripheral * _Nonnull peripheral, NSDictionary<NSString *,id> * _Nonnull advertisementData, NSNumber * _Nonnull RSSI, BOOL unprovisioned) {
        if ([peripheral.identifier.UUIDString isEqualToString:peripheralUUID]) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(scanWithPeripheralUUIDTimeout) object:nil];
            });
            [weakSelf stopScan];
            if (block) {
                block(peripheral,advertisementData,RSSI,YES);
            }
            weakSelf.bluetoothScanSpecialPeripheralCallback = nil;
        }
    }];
}

/**
 * @brief   Stop scan action.
 */
- (void)stopScan {
    [self.scanServiceUUIDs removeAllObjects];
    self.scanPeripheralUUID = nil;
    self.bluetoothScanPeripheralCallback = nil;
    if (self.manager.isScanning) {
        [self.manager stopScan];
    }
}

- (void)connectPeripheral:(CBPeripheral *)peripheral timeout:(NSTimeInterval)timeout resultBlock:(bleConnectPeripheralCallback)block {
    if (self.manager.state != CBCentralManagerStatePoweredOn) {
        TelinkLogError(@"Bluetooth is not power on.");
        if (block) {
            block(peripheral,NO);
        }
        return;
    }
    if (peripheral.state == CBPeripheralStateConnected) {
        if (block) {
            block(peripheral,YES);
        }
        return;
    }
    self.bluetoothConnectPeripheralCallback = block;
    self.currentPeripheral = peripheral;
    TelinkLogVerbose(@"call system connectPeripheral: uuid=%@",peripheral.identifier.UUIDString);
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(connectPeripheralTimeout) object:nil];
        [self performSelector:@selector(connectPeripheralTimeout) withObject:nil afterDelay:timeout];
    });
    [self.manager connectPeripheral:peripheral options:nil];
}

- (void)connectPeripheralWithError:(CBPeripheral *)peripheral timeout:(NSTimeInterval)timeout resultBlock:(bleConnectPeripheralWithErrorCallback)block {
    if (self.manager.state != CBCentralManagerStatePoweredOn) {
        TelinkLogError(@"Bluetooth is not power on.");
        NSError *error = [NSError errorWithDomain:@"Bluetooth is not power on." code:-1 userInfo:nil];
        if (block) {
            block(peripheral, error);
        }
        return;
    }
    if (peripheral.state == CBPeripheralStateConnected) {
        if (block) {
            block(peripheral, nil);
        }
        return;
    }
    self.bluetoothConnectPeripheralWithErrorCallback = block;
    self.currentPeripheral = peripheral;
    TelinkLogVerbose(@"call system connectPeripheral: uuid=%@",peripheral.identifier.UUIDString);
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(connectPeripheralTimeout) object:nil];
        [self performSelector:@selector(connectPeripheralTimeout) withObject:nil afterDelay:timeout];
    });
    [self.manager connectPeripheral:peripheral options:nil];
}

/// if timeout is 0,means will not timeout forever.
- (void)discoverServicesOfPeripheral:(CBPeripheral *)peripheral timeout:(NSTimeInterval)timeout resultBlock:(bleDiscoverServicesCallback)block {
    if (self.manager.state != CBCentralManagerStatePoweredOn) {
        TelinkLogError(@"Bluetooth is not power on.");
        if (block) {
            block(peripheral,NO);
        }
        return;
    }
    if (peripheral.state != CBPeripheralStateConnected) {
        TelinkLogError(@"peripheral is not connected.");
        if (block) {
            block(peripheral,NO);
        }
        return;
    }
    self.bluetoothDiscoverServicesCallback = block;
    self.currentPeripheral = peripheral;
    self.currentPeripheral.delegate = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(discoverServicesOfPeripheralTimeout) object:nil];
        [self performSelector:@selector(discoverServicesOfPeripheralTimeout) withObject:nil afterDelay:timeout];
    });
    [self.currentPeripheral discoverServices:nil];
}

- (void)changeNotifyToState:(BOOL)state Peripheral:(CBPeripheral *)peripheral characteristic:(CBCharacteristic *)characteristic timeout:(NSTimeInterval)timeout resultBlock:(bleCharacteristicResultCallback)block {
    if (self.manager.state != CBCentralManagerStatePoweredOn) {
        TelinkLogError(@"Bluetooth is not power on.");
        if (block) {
            NSError *error = [NSError errorWithDomain:@"Bluetooth is not power on." code:-1 userInfo:nil];
            block(peripheral, characteristic, error);
        }
        return;
    }
    if (peripheral.state != CBPeripheralStateConnected) {
        TelinkLogError(@"peripheral is not connected.");
        if (block) {
            NSError *error = [NSError errorWithDomain:@"Bluetooth is not power on." code:-1 userInfo:nil];
            block(peripheral, characteristic, error);
        }
        return;
    }
    self.bluetoothOpenNotifyCallback = block;
    self.currentPeripheral = peripheral;
    self.currentCharacteristic = characteristic;
    self.currentPeripheral.delegate = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(openNotifyOfPeripheralTimeout) object:nil];
        [self performSelector:@selector(openNotifyOfPeripheralTimeout) withObject:nil afterDelay:timeout];
    });
    [peripheral setNotifyValue:state forCharacteristic:characteristic];
}

- (void)cancelAllConnectionWithComplete:(bleCancelAllConnectCallback)complete{
    if (self.manager.state != CBCentralManagerStatePoweredOn) {
        TelinkLogError(@"Bluetooth is not power on.")
        dispatch_async(dispatch_get_main_queue(), ^{
            if (complete) {
                complete();
            }
        });
        return;
    }
    self.bluetoothConnectPeripheralCallback = nil;
    __weak typeof(self) weakSelf = self;
    NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
    [operationQueue addOperationWithBlock:^{
        //这个block语句块在子线程中执行
        if (weakSelf.connectedPeripherals && weakSelf.connectedPeripherals.count > 0) {
            NSArray *tem = [NSArray arrayWithArray:weakSelf.connectedPeripherals];
            for (CBPeripheral *p in tem) {
                if (p.state == CBPeripheralStateConnected || p.state == CBPeripheralStateConnecting) {
                    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
                    [weakSelf cancelConnectionPeripheral:p timeout:2-0.1 resultBlock:^(CBPeripheral * _Nonnull peripheral, BOOL successful) {
                        dispatch_semaphore_signal(semaphore);
                    }];
                    //Most provide 2 seconds to disconnect bluetooth connection
                    dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 2.0));
                }
            }
        }
        if (weakSelf.currentPeripheral) {
            [weakSelf cancelConnectionPeripheral:weakSelf.currentPeripheral timeout:2.0 resultBlock:^(CBPeripheral * _Nonnull peripheral, BOOL successful) {
                [weakSelf ressetParameters];
                weakSelf.currentPeripheral = nil;
                dispatch_async(dispatch_get_main_queue(), ^{
                    if (complete) {
                        complete();
                    }
                });
            }];
        }else{
            dispatch_async(dispatch_get_main_queue(), ^{
                if (complete) {
                    complete();
                }
            });
        }
    }];
}

- (void)cancelConnectionPeripheral:(CBPeripheral *)peripheral timeout:(NSTimeInterval)timeout resultBlock:(bleCancelConnectCallback)block{
    self.bluetoothCancelConnectCallback = block;
//    if (peripheral && peripheral.state != CBPeripheralStateDisconnected && peripheral.state != CBPeripheralStateDisconnecting) {
    if (peripheral && peripheral.state != CBPeripheralStateDisconnected) {
        TelinkLogDebug(@"cancel single connection");
        self.currentPeripheral = peripheral;
        self.currentPeripheral.delegate = self;
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(cancelConnectPeripheralTimeout) object:nil];
            [self performSelector:@selector(cancelConnectPeripheralTimeout) withObject:nil afterDelay:timeout];
        });
        [self.manager cancelPeripheralConnection:peripheral];
    }else{
        if (peripheral.state == CBPeripheralStateDisconnected) {
            if (self.bluetoothCancelConnectCallback) {
                self.bluetoothCancelConnectCallback(peripheral,YES);
            }
            self.bluetoothCancelConnectCallback = nil;
        }
    }
}

- (void)readOTACharachteristicWithTimeout:(NSTimeInterval)timeout complete:(bleReadOTACharachteristicCallback)complete {
    if (self.OTACharacteristic) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(readOTACharachteristicTimeout) object:nil];
            [self performSelector:@selector(readOTACharachteristicTimeout) withObject:nil afterDelay:timeout];
        });
        self.bluetoothReadOTACharachteristicCallback = complete;
        TelinkLogVerbose(@"readOTACharachteristic");
        [self.currentPeripheral readValueForCharacteristic:self.OTACharacteristic];
    }else{
        TelinkLogInfo(@"app don't found OTACharacteristic");
    }
}

- (void)cancelReadOTACharachteristic {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(readOTACharachteristicTimeout) object:nil];
    });
    self.bluetoothReadOTACharachteristicCallback = nil;
}

- (nullable CBPeripheral *)getPeripheralWithUUID:(NSString *)uuidString {
    NSMutableArray *identiferArray = [[NSMutableArray alloc] init];
    
    [identiferArray addObject:[CBUUID UUIDWithString:uuidString]];
    NSArray *knownPeripherals = [self.manager retrievePeripheralsWithIdentifiers:identiferArray];
    if (knownPeripherals.count > 0) {
        TelinkLogInfo(@"get peripherals from uuid:%@ count: %lu",uuidString,(unsigned long)knownPeripherals.count);
        return knownPeripherals.firstObject;
    }
    return nil;
}

/**
 * @brief   Get Characteristic of peripheral.
 * @param   uuid UUIDString of Characteristic.
 * @param   peripheral the CBPeripheral object.
 * @return  A CBCharacteristic object.
 */
- (nullable CBCharacteristic *)getCharacteristicWithUUIDString:(NSString *)uuid OfPeripheral:(CBPeripheral *)peripheral {
    CBCharacteristic *tem = nil;
    for (CBService *s in peripheral.services) {
        for (CBCharacteristic *c in s.characteristics) {
            if ([c.UUID.UUIDString isEqualToString:uuid.uppercaseString]) {
                tem = c;
                break;
            }
        }
        if (tem != nil) {
            break;
        }
    }
    return tem;
}

- (BOOL)isWorkNormal {
    if (self.OTACharacteristic != nil && self.PBGATT_InCharacteristic != nil && self.PROXY_InCharacteristic != nil && self.PBGATT_OutCharacteristic != nil && self.PROXY_OutCharacteristic != nil) {
        return YES;
    }
    return NO;
}

#pragma mark - new gatt api since v3.2.3
- (BOOL)readCharachteristic:(CBCharacteristic *)characteristic ofPeripheral:(CBPeripheral *)peripheral {
    if (peripheral.state != CBPeripheralStateConnected) {
        TelinkLogError(@"peripheral is not CBPeripheralStateConnected, can't read.")
        return NO;
    }
    TelinkLogInfo(@"%@--->read",characteristic.UUID.UUIDString);
    self.currentPeripheral = peripheral;
    self.currentPeripheral.delegate = self;
    [self.currentPeripheral readValueForCharacteristic:characteristic];
    return YES;
}

- (BOOL)writeValue:(NSData *)value toPeripheral:(CBPeripheral *)peripheral forCharacteristic:(CBCharacteristic *)characteristic type:(CBCharacteristicWriteType)type {
    if (peripheral.state != CBPeripheralStateConnected) {
        TelinkLogError(@"peripheral is not CBPeripheralStateConnected, can't write.")
        return NO;
    }
    TelinkLogInfo(@"%@--->0x%@",characteristic.UUID.UUIDString,[LibTools convertDataToHexStr:value]);
    self.currentPeripheral = peripheral;
    self.currentPeripheral.delegate = self;
    [self.currentPeripheral writeValue:value forCharacteristic:characteristic type:type];
    return YES;
}

#pragma mark - new gatt api since v3.3.3
- (void)readCharachteristicWithCharacteristic:(CBCharacteristic *)characteristic ofPeripheral:(CBPeripheral *)peripheral timeout:(NSTimeInterval)timeout complete:(bleReadOTACharachteristicCallback)complete {
    TelinkLogInfo(@"");
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(readCharachteristicTimeout) object:nil];
        [self performSelector:@selector(readCharachteristicTimeout) withObject:nil afterDelay:timeout];
    });
    self.readCharacteristic = characteristic;
    self.bluetoothReadCharachteristicCallback = complete;
    peripheral.delegate = self;
    [peripheral readValueForCharacteristic:characteristic];
}

#pragma mark - new gatt api since v3.3.5

/// 打开蓝牙通道
- (void)openChannelWithPeripheral:(CBPeripheral *)peripheral PSM:(CBL2CAPPSM)psm timeout:(NSTimeInterval)timeout resultBlock:(openChannelResultCallback)block {
    if (self.manager.state != CBCentralManagerStatePoweredOn) {
        TelinkLogError(@"Bluetooth is not power on.")
        if (block) {
            NSError *error = [NSError errorWithDomain:@"Bluetooth is not power on." code:-1 userInfo:nil];
            block(peripheral,nil,error);
        }
        return;
    }
    if (peripheral.state != CBPeripheralStateConnected) {
        TelinkLogError(@"peripheral is not connected.")
        if (block) {
            NSError *error = [NSError errorWithDomain:@"peripheral is not connected." code:-1 userInfo:nil];
            block(peripheral,nil,error);
        }
        return;
    }
    if (@available(iOS 11.0, *)) {
        self.didOpenChannelResultBlock = block;
        self.currentPeripheral = peripheral;
        self.currentPeripheral.delegate = self;
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(openChannelOfPeripheralTimeout) object:nil];
            [self performSelector:@selector(openChannelOfPeripheralTimeout) withObject:nil afterDelay:timeout];
        });
        TelinkLogInfo(@"openL2CAPChannel=%d", psm);
        [peripheral openL2CAPChannel:psm];
    } else {
        TelinkLogError(@"The iOS system is lower than 11.0.")
        if (block) {
            NSError *error = [NSError errorWithDomain:@"The iOS system is lower than 11.0." code:-1 userInfo:nil];
            block(peripheral,nil,error);
        }
    }
}

- (void)openChannelOfPeripheralTimeout {
    TelinkLogInfo(@"peripheral open channel timeout.")
    if (self.didOpenChannelResultBlock) {
        NSError *error = [NSError errorWithDomain:@"peripheral open channel timeout." code:-1 userInfo:nil];
        self.didOpenChannelResultBlock(self.currentPeripheral,nil,error);
        self.didOpenChannelResultBlock = nil;
    }
}

#pragma  mark - Private

- (void)scanWithPeripheralUUIDTimeout {
    TelinkLogInfo(@"peripheral connect fail.")
    if (self.currentPeripheral) {
        [self.manager cancelPeripheralConnection:self.currentPeripheral];
    }
    if (self.bluetoothScanSpecialPeripheralCallback) {
        CBPeripheral *tem = nil;
        self.bluetoothScanSpecialPeripheralCallback(tem,@{},@0,NO);
    }
    self.bluetoothScanSpecialPeripheralCallback = nil;
}

- (void)connectPeripheralFail {
    [self connectPeripheralTimeout];
}

- (void)connectPeripheralTimeout {
    if (self.currentPeripheral) {
        [self.manager cancelPeripheralConnection:self.currentPeripheral];
    }
    if (self.bluetoothConnectPeripheralCallback) {
        TelinkLogInfo(@"peripheral connect fail.")
        self.bluetoothConnectPeripheralCallback(self.currentPeripheral,NO);
    }
    self.bluetoothConnectPeripheralCallback = nil;
}

- (void)connectPeripheralFinish {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(connectPeripheralTimeout) object:nil];
    });
    if (self.bluetoothConnectPeripheralCallback) {
        self.bluetoothConnectPeripheralCallback(self.currentPeripheral, YES);
    }
    self.bluetoothConnectPeripheralCallback = nil;
    if (self.bluetoothConnectPeripheralWithErrorCallback) {
        self.bluetoothConnectPeripheralWithErrorCallback(self.currentPeripheral, nil);
    }
    self.bluetoothConnectPeripheralWithErrorCallback = nil;
}

- (void)discoverServicesOfPeripheralTimeout {
    TelinkLogInfo(@"peripheral discoverServices fail.")
    if (self.bluetoothDiscoverServicesCallback) {
        self.bluetoothDiscoverServicesCallback(self.currentPeripheral,NO);
    }
    self.bluetoothDiscoverServicesCallback = nil;
}

- (void)discoverServicesFinish {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(discoverServicesOfPeripheralTimeout) object:nil];
    });
    if (self.bluetoothDiscoverServicesCallback) {
        self.bluetoothDiscoverServicesCallback(self.currentPeripheral, YES);
    }
    self.bluetoothDiscoverServicesCallback = nil;
}

- (void)openNotifyOfPeripheralTimeout {
    TelinkLogInfo(@"peripheral open notify timeout.")
    if (self.bluetoothOpenNotifyCallback) {
        NSError *error = [NSError errorWithDomain:@"peripheral open notify timeout." code:-1 userInfo:nil];
        self.bluetoothOpenNotifyCallback(self.currentPeripheral, self.currentCharacteristic, error);
    }
//    self.bluetoothOpenNotifyCallback = nil;
}

- (void)openNotifyOfPeripheralFinish {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(openNotifyOfPeripheralTimeout) object:nil];
    });
    if (self.bluetoothOpenNotifyCallback) {
        self.bluetoothOpenNotifyCallback(self.currentPeripheral, self.currentCharacteristic, nil);
    }
//    self.bluetoothOpenNotifyCallback = nil;
}

- (void)cancelConnectPeripheralTimeout {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(cancelConnectPeripheralTimeout) object:nil];
    });
    if (self.bluetoothCancelConnectCallback && self.currentPeripheral) {
        TelinkLogInfo(@"cancelConnect peripheral timeout.")
//        self.bluetoothCancelConnectCallback(self.currentPeripheral,NO);
        //修复上一个设备断开异常后，导致之后一直走不到连接下一个设备的逻辑的bug。
        self.bluetoothCancelConnectCallback(self.currentPeripheral,YES);
    }
    self.bluetoothCancelConnectCallback = nil;
}

- (void)cancelConnectPeripheralFinish {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(cancelConnectPeripheralTimeout) object:nil];
    });
    if (self.bluetoothCancelConnectCallback && self.currentPeripheral) {
        self.bluetoothCancelConnectCallback(self.currentPeripheral, YES);
    }
    self.bluetoothCancelConnectCallback = nil;
}

- (void)readOTACharachteristicTimeout {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(readOTACharachteristicTimeout) object:nil];
    });
    if (self.bluetoothReadOTACharachteristicCallback) {
        self.bluetoothReadOTACharachteristicCallback(self.OTACharacteristic, NO);
        self.bluetoothReadOTACharachteristicCallback = nil;
    }
}

- (void)readOTACharachteristicFinish:(CBCharacteristic *)characteristic {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(readOTACharachteristicTimeout) object:nil];
    });
    if (self.bluetoothReadOTACharachteristicCallback) {
        self.bluetoothReadOTACharachteristicCallback(characteristic, YES);
        self.bluetoothReadOTACharachteristicCallback = nil;
    }
}

- (void)readCharachteristicTimeout {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(readCharachteristicTimeout) object:nil];
    });
    if (self.bluetoothReadCharachteristicCallback) {
        self.bluetoothReadCharachteristicCallback(self.readCharacteristic, NO);
        self.bluetoothReadCharachteristicCallback = nil;
    }
}

- (void)readCharachteristicFinish:(CBCharacteristic *)characteristic {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(readCharachteristicTimeout) object:nil];
    });
    if (self.bluetoothReadCharachteristicCallback) {
        self.bluetoothReadCharachteristicCallback(characteristic, YES);
        self.bluetoothReadCharachteristicCallback = nil;
    }
}

- (void)addConnectedPeripheralToLocations:(CBPeripheral *)peripheral {
    if (![self.connectedPeripherals containsObject:peripheral]) {
        [self.connectedPeripherals addObject:peripheral];
    }
}

- (void)removeConnectedPeripheralFromLocations:(CBPeripheral *)peripheral {
    if ([self.connectedPeripherals containsObject:peripheral]) {
        [self.connectedPeripherals removeObject:peripheral];
    }
}

- (void)ressetParameters {
    self.OTACharacteristic = nil;
    self.PBGATT_InCharacteristic = nil;
    self.PBGATT_OutCharacteristic = nil;
    self.PROXY_InCharacteristic = nil;
    self.PROXY_OutCharacteristic = nil;
}

- (void)callbackDisconnectOfPeripheral:(CBPeripheral *)peripheral error:(nullable NSError *)error {
    if (self.bluetoothDisconnectCallback) {
        self.bluetoothDisconnectCallback(peripheral,error);
    }
}

#pragma mark - CBCentralManagerDelegate
/*
 The delegate of a {@link CBCentralManager} object must adopt the <code>CBCentralManagerDelegate</code> protocol. The
 single required method indicates the availability of the central manager, while the optional methods allow for the discovery and
 connection of peripherals.
 */

/*!
 *  @method centralManagerDidUpdateState:
 *
 *  @param central  The central manager whose state has changed.
 *
 *  @discussion     Invoked whenever the central manager's state has been updated. Commands should only be issued when the state is
 *                  <code>CBCentralManagerStatePoweredOn</code>. A state below <code>CBCentralManagerStatePoweredOn</code>
 *                  implies that scanning has stopped and any connected peripherals have been disconnected. If the state moves below
 *                  <code>CBCentralManagerStatePoweredOff</code>, all <code>CBPeripheral</code> objects obtained from this central
 *                  manager become invalid and must be retrieved or discovered again.
 *
 *  @see            state
 *
 */
- (void)centralManagerDidUpdateState:(CBCentralManager *)central {
//    TelinkLogInfo(@"state=%ld",(long)central.state)
    if (self.manager.state == CBCentralManagerStatePoweredOn) {
        if (_isInitFinish) {
//            if (self.bluetoothEnableCallback) {
//                self.bluetoothEnableCallback(central,YES);
//            }
        }else{
            _isInitFinish = YES;
            if (self.bluetoothInitSuccessCallback) {
                self.bluetoothInitSuccessCallback(central);
            }
            self.bluetoothInitSuccessCallback = nil;
        }
    } else {
//        if (self.bluetoothEnableCallback) {
//            self.bluetoothEnableCallback(central,NO);
//        }
        if (self.currentPeripheral) {
            NSError *err = [NSError errorWithDomain:@"CBCentralManager.state is not CBCentralManagerStatePoweredOn!" code:-1 userInfo:nil];
            [self callbackDisconnectOfPeripheral:self.currentPeripheral error:err];
        }
//            [self stopAutoConnect];
    }
    if (self.bluetoothCentralUpdateStateCallback) {
        self.bluetoothCentralUpdateStateCallback((CBCentralManagerState)central.state);
    }
}

/*!
 *  @method centralManager:didDiscoverPeripheral:advertisementData:RSSI:
 *
 *  @param central              The central manager providing this update.
 *  @param peripheral           A <code>CBPeripheral</code> object.
 *  @param advertisementData    A dictionary containing any advertisement and scan response data.
 *  @param RSSI                 The current RSSI of <i>peripheral</i>, in dBm. A value of <code>127</code> is reserved and indicates the RSSI
 *                                was not available.
 *
 *  @discussion                 This method is invoked while scanning, upon the discovery of <i>peripheral</i> by <i>central</i>. A discovered peripheral must
 *                              be retained in order to use it; otherwise, it is assumed to not be of interest and will be cleaned up by the central manager. For
 *                              a list of <i>advertisementData</i> keys, see {@link CBAdvertisementDataLocalNameKey} and other similar constants.
 *
 *  @seealso                    CBAdvertisementData.h
 *
 */
- (void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary<NSString *, id> *)advertisementData RSSI:(NSNumber *)RSSI {
    // 将127修正为-90，防止APP扫描不到设备。
    if (RSSI.intValue == 127) {
        RSSI = @(-90);
//        TelinkLogDebug(@"将127修正为-90，防止APP扫描不到设备。peripheral.identifier.UUIDString=%@",peripheral.identifier.UUIDString);
    }
    
    /// there is invalid node when RSSI is greater than or equal to 0.
    if (RSSI.intValue >=0) {
        return;
    }
    //=================test==================//
//    if (RSSI.intValue < -50) {
//        return;
//    }
    //=================test==================//

    if (![advertisementData.allKeys containsObject:CBAdvertisementDataServiceUUIDsKey]) {
        return;
    }
    
    NSArray *suuids = advertisementData[CBAdvertisementDataServiceUUIDsKey];
    if (!suuids || suuids.count == 0) {
        return;
    }
    
    NSString *suuidString = ((CBUUID *)suuids.firstObject).UUIDString;
    /// which means the device can be add to a new mesh(没有入网)
    BOOL provisionAble = [suuidString  isEqualToString:kPBGATTService] || [suuidString  isEqualToString:[LibTools change16BitsUUIDTO128Bits:kPBGATTService]];
    /// which means the device has been add to a mesh(已经入网)
    BOOL unProvisionAble = [suuidString isEqualToString:kPROXYService] || [suuidString  isEqualToString:[LibTools change16BitsUUIDTO128Bits:kPROXYService]];
    /// which means the device is a mesh CDTP Service
    BOOL isOtsService = [suuidString isEqualToString:kObjectTransferService] || [suuidString  isEqualToString:[LibTools change16BitsUUIDTO128Bits:kObjectTransferService]];

    if (!provisionAble && !unProvisionAble && !isOtsService) {
        return;
    }
    
    BOOL shouldReturn = YES;
    if (self.scanServiceUUIDs && ([self.scanServiceUUIDs containsObject:[CBUUID UUIDWithString:kPBGATTService]]) && provisionAble) {
        shouldReturn = NO;
    }
    if (self.scanServiceUUIDs && ([self.scanServiceUUIDs containsObject:[CBUUID UUIDWithString:kPROXYService]]) && unProvisionAble) {
        shouldReturn = NO;
    }
    if (self.scanServiceUUIDs && [self.scanServiceUUIDs containsObject:[CBUUID UUIDWithString:kObjectTransferService]]) {
        shouldReturn = NO;
    }
    if (shouldReturn) {
        return;
    }
    
    SigScanRspModel *scanRspModel = [[SigScanRspModel alloc] initWithPeripheral:peripheral advertisementData:advertisementData];
    
    if ([self.delegate respondsToSelector:@selector(needToBeFilteredNodeWithSigScanRspModel:provisioned:peripheral:advertisementData:RSSI:)]) {
        BOOL result = [self.delegate needToBeFilteredNodeWithSigScanRspModel:scanRspModel provisioned:unProvisionAble peripheral:peripheral advertisementData:advertisementData RSSI:RSSI];
        if (result) {
            return;
        }
    }
    
//    TelinkLogInfo(@"discover RSSI:%@ uuid:%@ mac：%@ state=%@ advertisementData=%@",RSSI,peripheral.identifier.UUIDString,scanRspModel.macAddress,provisionAble?@"1827":@"1828",advertisementData);
    BOOL shouldDelay = scanRspModel.macAddress == nil || scanRspModel.macAddress.length == 0;
    if (shouldDelay && self.waitScanRseponseEnable) {
        TelinkLogVerbose(@"this node uuid=%@ has not MacAddress, dalay and return.",peripheral.identifier.UUIDString);
        return;
    }

    if (unProvisionAble && self.checkNetworkEnable) {
        scanRspModel.uuid = peripheral.identifier.UUIDString;
        BOOL isExist = [SigMeshLib.share.dataSource existScanRspModelOfCurrentMeshNetwork:scanRspModel];
        // 注释该逻辑，假定设备都不广播MacAddress
//        if (isExist && scanRspModel.networkIDData && scanRspModel.networkIDData.length > 0) {
//            SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:scanRspModel.address];
//            isExist = node != nil;
//        }
        if (!isExist) {
            return;
        }
    }

    TelinkLogInfo(@"discover RSSI:%@ uuid:%@ mac：%@ state=%@ advertisementData=%@",RSSI,peripheral.identifier.UUIDString,scanRspModel.macAddress,provisionAble?@"1827":@"1828",advertisementData);
    [SigMeshLib.share.dataSource updateScanRspModelToDataSource:scanRspModel];
    
    if (self.bluetoothScanPeripheralCallback) {
        self.bluetoothScanPeripheralCallback(peripheral,advertisementData,RSSI,provisionAble);
    }
    
    
}

/*!
 *  @method centralManager:didConnectPeripheral:
 *
 *  @param central      The central manager providing this information.
 *  @param peripheral   The <code>CBPeripheral</code> that has connected.
 *
 *  @discussion         This method is invoked when a connection initiated by {@link connectPeripheral:options:} has succeeded.
 *
 */
- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral {
    TelinkLogInfo(@"uuid:%@",peripheral.identifier.UUIDString);
    //v3.3.0,用于过滤重复的sequenceNumber，设备连接成功则清除当前缓存的设备返回的旧的最大sequenceNumber字典。(因为所有设备断电时设备端的sequenceNumber会归零。)
    [SigMeshLib.share.dataSource.nodeSequenceNumberCacheList removeAllObjects];
    SigMeshLib.share.secureNetworkBeacon = nil;
    SigMeshLib.share.meshPrivateBeacon = nil;
    if ([peripheral isEqual:self.currentPeripheral]) {
        [self addConnectedPeripheralToLocations:peripheral];
        [self connectPeripheralFinish];
    }
}

/*!
 *  @method centralManager:didFailToConnectPeripheral:error:
 *
 *  @param central      The central manager providing this information.
 *  @param peripheral   The <code>CBPeripheral</code> that has failed to connect.
 *  @param error        The cause of the failure.
 *
 *  @discussion         This method is invoked when a connection initiated by {@link connectPeripheral:options:} has failed to complete. As connection attempts do not
 *                      timeout, the failure of a connection is atypical and usually indicative of a transient issue.
 *
 */
- (void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(nullable NSError *)error {
    TelinkLogInfo(@"peripheral=%@,error=%@",peripheral,error);
    if ([peripheral isEqual:self.currentPeripheral]) {
        if (self.bluetoothConnectPeripheralWithErrorCallback) {
            TelinkLogInfo(@"peripheral connect fail.")
            self.bluetoothConnectPeripheralWithErrorCallback(self.currentPeripheral, error);
        }
        self.bluetoothConnectPeripheralWithErrorCallback = nil;
        [self connectPeripheralFail];
    }
}

/*!
 *  @method centralManager:didDisconnectPeripheral:error:
 *
 *  @param central      The central manager providing this information.
 *  @param peripheral   The <code>CBPeripheral</code> that has disconnected.
 *  @param error        If an error occurred, the cause of the failure.
 *
 *  @discussion         This method is invoked upon the disconnection of a peripheral that was connected by {@link connectPeripheral:options:}. If the disconnection
 *                      was not initiated by {@link cancelPeripheralConnection}, the cause will be detailed in the <i>error</i> parameter. Once this method has been
 *                      called, no more methods will be invoked on <i>peripheral</i>'s <code>CBPeripheralDelegate</code>.
 *
 */
- (void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(nullable NSError *)error {
    TelinkLogInfo(@"peripheral=%@,error=%@",peripheral,error);
    if ([peripheral isEqual:self.currentPeripheral]) {
        if (self.bluetoothConnectPeripheralWithErrorCallback) {
            TelinkLogInfo(@"peripheral connect fail.")
            self.bluetoothConnectPeripheralWithErrorCallback(self.currentPeripheral, error);
        }
        self.bluetoothConnectPeripheralWithErrorCallback = nil;

        [self connectPeripheralFail];
        [self cancelConnectPeripheralFinish];
        [self removeConnectedPeripheralFromLocations:peripheral];
        NSArray *curNodes = [NSArray arrayWithArray:SigMeshLib.share.dataSource.curNodes];
        for (SigNodeModel *node in curNodes) {
            if (node.hasOpenPublish) {
                [SigPublishManager.share stopCheckOfflineTimerWithAddress:@(node.address)];
            }
        }
        [self callbackDisconnectOfPeripheral:peripheral error:error];
    }
}

#pragma mark - CBPeripheralDelegate

/*!
 *  @method peripheral:didDiscoverServices:
 *
 *  @param peripheral    The peripheral providing this information.
 *    @param error        If an error occurred, the cause of the failure.
 *
 *  @discussion            This method returns the result of a @link discoverServices: @/link call. If the service(s) were read successfully, they can be retrieved via
 *                        <i>peripheral</i>'s @link services @/link property.
 *
 */
- (void)peripheral:(CBPeripheral *)peripheral didDiscoverServices:(nullable NSError *)error {
    TelinkLogInfo(@"");
    for (CBService *s in peripheral.services) {
        [self.currentPeripheral discoverCharacteristics:nil forService:s];
    }
}

/*!
 *  @method peripheral:didDiscoverCharacteristicsForService:error:
 *
 *  @param peripheral    The peripheral providing this information.
 *  @param service        The <code>CBService</code> object containing the characteristic(s).
 *    @param error        If an error occurred, the cause of the failure.
 *
 *  @discussion            This method returns the result of a @link discoverCharacteristics:forService: @/link call. If the characteristic(s) were read successfully,
 *                        they can be retrieved via <i>service</i>'s <code>characteristics</code> property.
 */
- (void)peripheral:(CBPeripheral *)peripheral didDiscoverCharacteristicsForService:(CBService *)service error:(nullable NSError *)error {
    for (CBCharacteristic *c in service.characteristics) {
        if ([c.UUID.UUIDString isEqualToString:kOTA_CharacteristicsID]) {
            self.OTACharacteristic = c;
        }else if ([c.UUID.UUIDString isEqualToString:kPBGATT_Out_CharacteristicsID]){
            self.PBGATT_OutCharacteristic = c;
        }else if ([c.UUID.UUIDString isEqualToString:kPBGATT_In_CharacteristicsID]){
            self.PBGATT_InCharacteristic = c;
        }else if ([c.UUID.UUIDString isEqualToString:kPROXY_Out_CharacteristicsID]){
            self.PROXY_OutCharacteristic = c;
        }else if ([c.UUID.UUIDString isEqualToString:kPROXY_In_CharacteristicsID]){
            self.PROXY_InCharacteristic = c;
        }else if ([c.UUID.UUIDString isEqualToString:kOnlineStatusCharacteristicsID]){
            [peripheral setNotifyValue:YES forCharacteristic:c];//不notify一下，APP获取不到onlineState数据
            self.OnlineStatusCharacteristic = c;
        }else if ([c.UUID.UUIDString isEqualToString:kMeshOTA_CharacteristicsID]){
            self.MeshOTACharacteristic = c;
        }
        if (kNeedDiscoverDescriptors) {
            [peripheral discoverDescriptorsForCharacteristic:c];
        } else {
            CBService *lastService = peripheral.services.lastObject;
            if (lastService == service) {
                [self discoverServicesFinish];
            }
        }
    }
}

/*!
 *  @method peripheral:didDiscoverDescriptorsForCharacteristic:error:
 *
 *  @param peripheral        The peripheral providing this information.
 *  @param characteristic    A <code>CBCharacteristic</code> object.
 *    @param error            If an error occurred, the cause of the failure.
 *
 *  @discussion                This method returns the result of a @link discoverDescriptorsForCharacteristic: @/link call. If the descriptors were read successfully,
 *                            they can be retrieved via <i>characteristic</i>'s <code>descriptors</code> property.
 */
- (void)peripheral:(CBPeripheral *)peripheral didDiscoverDescriptorsForCharacteristic:(CBCharacteristic *)characteristic error:(nullable NSError *)error {
    if (kNeedDiscoverDescriptors) {
        if ([peripheral isEqual:self.currentPeripheral]) {
            CBCharacteristic *lastCharacteristic = nil;
            for (CBService *s in peripheral.services) {
                if (s.characteristics && s.characteristics.count > 0) {
                    lastCharacteristic = s.characteristics.lastObject;
                }
            }
            if (lastCharacteristic == characteristic) {
                [self discoverServicesFinish];
            }
        }
    }
}

/*!
 *  @method peripheral:didWriteValueForCharacteristic:error:
 *
 *  @param peripheral        The peripheral providing this information.
 *  @param characteristic    A <code>CBCharacteristic</code> object.
 *    @param error            If an error occurred, the cause of the failure.
 *
 *  @discussion                This method returns the result of a {@link writeValue:forCharacteristic:type:} call, when the <code>CBCharacteristicWriteWithResponse</code> type is used.
 */
- (void)peripheral:(CBPeripheral *)peripheral didWriteValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error {
    if (self.bluetoothDidWriteValueCallback) {
        self.bluetoothDidWriteValueCallback(peripheral,characteristic,error);
    }
}

/*!
 *  @method peripheral:didUpdateValueForCharacteristic:error:
 *
 *  @param peripheral        The peripheral providing this information.
 *  @param characteristic    A <code>CBCharacteristic</code> object.
 *    @param error            If an error occurred, the cause of the failure.
 *
 *  @discussion                This method is invoked after a @link readValueForCharacteristic: @/link call, or upon receipt of a notification/indication.
 */
- (void)peripheral:(CBPeripheral *)peripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(nullable NSError *)error {
    // Reduce logs
    BOOL log = NO;
    if (self.bluetoothDidUpdateValueCallback) {
        self.bluetoothDidUpdateValueCallback(peripheral,characteristic,error);
    }
    if (([characteristic.UUID.UUIDString isEqualToString:kPROXY_Out_CharacteristicsID] && SigBearer.share.isProvisioned) || ([characteristic.UUID.UUIDString isEqualToString:kPBGATT_Out_CharacteristicsID] && !SigBearer.share.isProvisioned)) {
        if ([characteristic.UUID.UUIDString isEqualToString:kPROXY_Out_CharacteristicsID]) {
            log = YES;
            TelinkLogInfo(@"<--- from:PROXY, length:%d",characteristic.value.length);
            //            TelinkLogInfo(@"<--- from:PROXY, length:%d, data:%@",characteristic.value.length,[LibTools convertDataToHexStr:characteristic.value]);
        } else {
            log = YES;
            TelinkLogInfo(@"<--- from:GATT, length:%d",characteristic.value.length);
            //            TelinkLogInfo(@"<--- from:GATT, length:%d, data:%@",characteristic.value.length,[LibTools convertDataToHexStr:characteristic.value]);
        }
        if (self.bluetoothDidUpdateValueForCharacteristicCallback) {
            self.bluetoothDidUpdateValueForCharacteristicCallback(peripheral, characteristic,error);
        }
    }
    if ([characteristic.UUID.UUIDString isEqualToString:kOTA_CharacteristicsID]) {
        log = YES;
        TelinkLogVerbose(@"<--- from:OTA, %@", characteristic.value);
        if (self.bluetoothReadOTACharachteristicCallback) {
            self.bluetoothReadOTACharachteristicCallback(characteristic,YES);
            self.bluetoothReadOTACharachteristicCallback = nil;
        }
    }
    if ([characteristic.UUID.UUIDString isEqualToString:kOnlineStatusCharacteristicsID]) {
        log = YES;
        TelinkLogInfo(@"<--- from:OnlineStatusCharacteristics, length:%d",characteristic.value.length);
        if (self.bluetoothDidUpdateOnlineStatusValueCallback) {
            self.bluetoothDidUpdateOnlineStatusValueCallback(peripheral, characteristic,error);
        }
    }
    if ([characteristic isEqual:self.readCharacteristic]) {
        log = YES;
        TelinkLogInfo(@"<--- from:readCharacteristics, value:%@",characteristic.value);
        if (error) {
            [self readCharachteristicTimeout];
        } else {
            [self readCharachteristicFinish:characteristic];
        }
    }
    if (log == NO) {
        // Reduce logs
        TelinkLogInfo(@"<--- from:uuid:%@, length:%d",characteristic.UUID.UUIDString,characteristic.value.length);
    }
}

/*!
 *  @method peripheral:didUpdateNotificationStateForCharacteristic:error:
 *
 *  @param peripheral        The peripheral providing this information.
 *  @param characteristic    A <code>CBCharacteristic</code> object.
 *    @param error            If an error occurred, the cause of the failure.
 *
 *  @discussion                This method returns the result of a @link setNotifyValue:forCharacteristic: @/link call.
 */
- (void)peripheral:(CBPeripheral *)peripheral didUpdateNotificationStateForCharacteristic:(CBCharacteristic *)characteristic error:(nullable NSError *)error {
    TelinkLogInfo(@"uuid=%@ didUpdateNotification state=%d, error=%@",characteristic.UUID.UUIDString,characteristic.isNotifying,error);
    if ([peripheral isEqual:self.currentPeripheral] && [characteristic isEqual:self.currentCharacteristic]) {
        [self openNotifyOfPeripheralFinish];
    }
}

//since ios 11.0
/*!
 *  @method peripheralIsReadyToSendWriteWithoutResponse:
 *
 *  @param peripheral   The peripheral providing this update.
 *
 *  @discussion         This method is invoked after a failed call to @link writeValue:forCharacteristic:type: @/link, when <i>peripheral</i> is again
 *                      ready to send characteristic value updates.
 *
 */
- (void)peripheralIsReadyToSendWriteWithoutResponse:(CBPeripheral *)peripheral {
//    TelinkLogVerbose(@"since ios 11.0,peripheralIsReadyToSendWriteWithoutResponse");
    if ([peripheral isEqual:self.currentPeripheral]) {
        if (self.bluetoothIsReadyToSendWriteWithoutResponseCallback) {
            self.bluetoothIsReadyToSendWriteWithoutResponseCallback(peripheral);
        }else{
            TelinkLogError(@"bluetoothIsReadyToSendWriteWithoutResponseCallback = nil.");
        }
    }
}

//since ios 11.0
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunguarded-availability-new"
/*!
 *  @method peripheral:didOpenL2CAPChannel:error:
 *
 *  @param peripheral        The peripheral providing this information.
 *  @param channel            A <code>CBL2CAPChannel</code> object.
 *    @param error            If an error occurred, the cause of the failure.
 *
 *  @discussion                This method returns the result of a @link openL2CAPChannel: @link call.
 */
-(void)peripheral:(CBPeripheral *)peripheral didOpenL2CAPChannel:(CBL2CAPChannel *)channel error:(NSError *)error {
    TelinkLogInfo(@"[%@->%@]",NSStringFromClass([self class]), NSStringFromSelector(_cmd));
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(openChannelOfPeripheralTimeout) object:nil];
    });
    if (error) {
        TelinkLogError(@"error.localizedDescription = %@",error.localizedDescription);
        if (self.didOpenChannelResultBlock) {
            self.didOpenChannelResultBlock(self.currentPeripheral,channel,error);
            self.didOpenChannelResultBlock = nil;
        }
    } else {
        if (self.didOpenChannelResultBlock) {
            self.didOpenChannelResultBlock(self.currentPeripheral,channel,nil);
            self.didOpenChannelResultBlock = nil;
        }
    }
}
#pragma clang diagnostic pop

@end
