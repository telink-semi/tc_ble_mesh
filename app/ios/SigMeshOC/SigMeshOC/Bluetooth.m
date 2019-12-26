/********************************************************************************************************
 * @file     Bluetooth.m
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/
//
//  Bluetooth.m
//  SigMeshOC
//
//  Created by Liangjiazhi on 2018/7/12.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "Bluetooth.h"
#import "XHelp.h"
#import "LibHandle.h"

@interface Bluetooth()
//current provision node address
@property (nonatomic,assign) UInt16 currentProvisionIndex;
//current provision netkey index
@property (nonatomic,assign) UInt16 currentNetkeyIndex;
//current provision node's element count
@property (nonatomic,assign) UInt8 currentElementCount;
//contains provision success list
@property (nonatomic,strong) NSMutableDictionary <NSString *,NSNumber *>*tempProvisionSuccessList;
//contains provsion fail list
@property (nonatomic,strong) NSMutableArray <NSString *>*tempProvisionFailList;
//Dictionary of timer that check node off line.
@property (nonatomic,strong) NSMutableDictionary <NSNumber *,NSTimer *>*checkOfflineTimerDict;

@property (nonatomic,strong) NSTimer *provisionScanTimer;
@property (nonatomic,strong) NSTimer *autoConnectScanTimer;
@property (nonatomic,assign) int connectWithUUIDCount;

// current key bind type. default is normal.
@property (nonatomic,assign) KeyBindTpye currentAddType;
// it is need to call start scan after add one node successful. default is NO.
@property (nonatomic,assign) BOOL isAutoAddDevice;
// unicastAddress for remote add
@property (nonatomic,assign) UInt16 unicastAddress;
// uuid for remote add
@property (nonatomic,strong) NSData *uuid;

@property (nonatomic, strong) NSThread *anasislyResponseThread;
@property (nonatomic,assign) BOOL isInitFinish;
@property (nonatomic,assign) BOOL isScanDelaying;
@property (nonatomic,assign) BOOL isScanEnoughTime;//if scan node that has macAddress, handle it; if scan node that has not macAddress, delay kScanMacAddressDelay.

@end

//记录上一次发送非OTA包的时间
static NSTimeInterval commentTime;

@implementation Bluetooth

+ (Bluetooth *)share{
    static Bluetooth *shareBLE = nil;
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        shareBLE = [[Bluetooth alloc] init];
        shareBLE.manager = [[CBCentralManager alloc] initWithDelegate:shareBLE queue:dispatch_get_main_queue()];
        shareBLE.tempProvisionSuccessList = [[NSMutableDictionary alloc] init];
        shareBLE.tempProvisionFailList = [[NSMutableArray alloc] init];
        shareBLE.checkOfflineTimerDict = [[NSMutableDictionary alloc] init];
        shareBLE.currentProvisionIndex = 0;
        shareBLE.currentElementCount = 0;
        shareBLE.connectWithUUIDCount = 0;
        shareBLE.currentNetkeyIndex = 0;
        shareBLE.currentAddType = KeyBindTpye_Normal;
        shareBLE.isAutoAddDevice = NO;
        shareBLE.isInitFinish = NO;
        commentTime = 0;

        [shareBLE createThread];
    });
    return shareBLE;
}

- (void)createThread{
    // the permanent thread, used to anasisly online statue data from OnlineStatusCharacteristic
    self.anasislyResponseThread = [[NSThread alloc] initWithTarget:self selector:@selector(startThread) object:nil];
    [self.anasislyResponseThread start];
}

#pragma mark - Private
- (void)startThread{
    [NSTimer scheduledTimerWithTimeInterval:[[NSDate distantFuture] timeIntervalSinceNow] target:self selector:@selector(nullFunc) userInfo:nil repeats:NO];
    while (1) {
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
    }
}

- (void)nullFunc{}

#pragma mark - CBCentralManagerDelegate
- (void)centralManagerDidUpdateState:(CBCentralManager *)central{
    if (self.manager.state == CBCentralManagerStatePoweredOn) {
        if (_isInitFinish) {
            [self startAutoConnect];
        }
    } else {
        [self stopAutoConnect];
    }
    if (_isInitFinish) {
        if (self.bleCentralUpdateStateCallBack) {
            self.bleCentralUpdateStateCallBack((CBCentralManagerState)central.state);
        }
    }
    _isInitFinish = YES;
}

- (void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary<NSString *,id> *)advertisementData RSSI:(NSNumber *)RSSI{
    if ([advertisementData.allKeys containsObject:CBAdvertisementDataServiceUUIDsKey]) {
        NSArray *suuids = advertisementData[CBAdvertisementDataServiceUUIDsKey];
        if (!suuids || suuids.count == 0) {
            return;
        }
        /// there is invalid node when RSSI is greater than or equal to 0.
        if (RSSI.intValue >=0) {
            return;
        }

        NSString *suuidString = ((CBUUID *)suuids.firstObject).UUIDString;
        
        /// which means the device can be add to a new mesh(没有入网)
        BOOL provisionAble = [suuidString  isEqualToString: kPBGATTService];
        
        /// which means the device has been add to a mesh(已经入网)
        BOOL unProvisionAble = [suuidString isEqualToString:kPROXYService];
        
        if (!provisionAble && !unProvisionAble) {
            return;
        }
        if ((self.state != StateAddDevice_provision && provisionAble)||(self.state == StateAddDevice_provision && unProvisionAble)) {
            return;
        }
        //=================test==================//
//        if (RSSI.intValue < -50) {
//            return;
//        }
        //=================test==================//
        
        SigScanRspModel *scanRspModel = [[SigScanRspModel alloc] initWithPeripheral:peripheral advertisementData:advertisementData];
        BOOL shouldDelay = scanRspModel.macAddress == nil || scanRspModel.macAddress.length == 0;
        if (![self setScanDelayFlagEnable:shouldDelay]) {
//            TeLog(@"this node uuid=%@ has not MacAddress, dalay and return.",peripheral.identifier.UUIDString);
            return;
        }

        if (unProvisionAble) {
            BOOL isExist = [SigDataSource.share existScanRspModel:scanRspModel];
            if (!isExist) {
                return;
            }
        }
        NSOperationQueue *oprationQueue = [[NSOperationQueue alloc] init];
        [oprationQueue addOperationWithBlock:^{
//            TeLog(@"discover mac：%@ state=%@ advertisementData=%@",scanRspModel.macAddress,provisionAble?@"1827":@"1828",advertisementData);
            [SigDataSource.share updateScanRspModelToDataSource:scanRspModel];
        }];

        [self.store saveToLocal:peripheral];
        if (provisionAble) {
            kEndTimer(_provisionScanTimer);
            if (self.bleScanNewDeviceCallBack) {
                self.bleScanNewDeviceCallBack(peripheral, YES);
            }
        }else if (unProvisionAble){
            if (self.state != StateAddDevice_provision && self.state != StateAddDevice_keyBind) {
                if (self.state == StateOTA) {
                    if (self.bleScanNewDeviceCallBack) {
                        self.bleScanNewDeviceCallBack(peripheral, NO);
                    }
                }else if (self.state == StateNormal){
                    [self.store addRSSIWithPeripheral:peripheral RSSI:RSSI.intValue];
                    if (RSSI.intValue > -60) {
                        dispatch_async(dispatch_get_main_queue(), ^{
                            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(rssiHightest) object:nil];
                            [self rssiHightest];
                        });
                    }
                }
            }else if (self.state == StateAddDevice_keyBind){
                if (self.bleScanNewDeviceCallBack) {
                    self.bleScanNewDeviceCallBack(peripheral, NO);
                }
            }
        }
    }
}

- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral{
    TeLog(@"");
    [self stopAutoConnect];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(connectWithUUIDTimeout:) object:peripheral.identifier.UUIDString];
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(connectPeripheralTimeOutInAddDevice) object:nil];
    });
    [self.manager stopScan];
    [self.store ressetParameters];
    self.store.peripheral = peripheral;
    self.store.peripheral.delegate = self;
    [self.store.peripheral discoverServices:nil];
}


#pragma mark - CBPeripheralDelegate
- (void)peripheral:(CBPeripheral *)peripheral didDiscoverServices:(NSError *)error{
    TeLog(@"didDiscoverServices");
    for (CBService *s in peripheral.services) {
        [self.store.peripheral discoverCharacteristics:nil forService:s];
    }
}

- (void)peripheral:(CBPeripheral *)peripheral didUpdateNotificationStateForCharacteristic:(CBCharacteristic *)characteristic error:(nullable NSError *)error{
    if (characteristic.isNotifying) {
        TeLog(@"characteristic uuid=%@ isNotifying=%d",characteristic.UUID.UUIDString,characteristic.isNotifying);
    }
}

- (void)peripheral:(CBPeripheral *)peripheral didDiscoverCharacteristicsForService:(CBService *)service error:(NSError *)error{
    for (CBCharacteristic *c in service.characteristics) {
        [peripheral setNotifyValue:YES forCharacteristic:c];
        [peripheral discoverDescriptorsForCharacteristic:c];
        if ([c.UUID.UUIDString isEqualToString:kOTA_CharacteristicsID]) {
            self.store.OTACharacteristic = c;
        }else if ([c.UUID.UUIDString isEqualToString:kPBGATT_Out_CharacteristicsID]){
            self.store.PBGATT_OutCharacteristic = c;
        }else if ([c.UUID.UUIDString isEqualToString:kPBGATT_In_CharacteristicsID]){
            self.store.PBGATT_InCharacteristic = c;
        }else if ([c.UUID.UUIDString isEqualToString:kPROXY_Out_CharacteristicsID]){
            self.store.PROXY_OutCharacteristic = c;
        }else if ([c.UUID.UUIDString isEqualToString:kPROXY_In_CharacteristicsID]){
            self.store.PROXY_InCharacteristic = c;
        }else if ([c.UUID.UUIDString isEqualToString:kOnlineStatusCharacteristicsID]){
            self.store.OnlineStatusCharacteristic = c;
        }else if ([c.UUID.UUIDString isEqualToString:kMeshOTA_CharacteristicsID]){
            self.store.MeshOTACharacteristic = c;
        }
    }
    
    //WithResponse=512,WithoutResponse=20；MTU250时，WithResponse=512,WithoutResponse=182
//    TeLog(@"2.uuid=%@ max length WithResponse=%d,WithoutResponse=%d",peripheral.identifier.UUIDString,[peripheral maximumWriteValueLengthForType:CBCharacteristicWriteWithResponse],[peripheral maximumWriteValueLengthForType:CBCharacteristicWriteWithoutResponse]);
}

- (void)peripheral:(CBPeripheral *)peripheral didDiscoverDescriptorsForCharacteristic:(CBCharacteristic *)characteristic error:(nullable NSError *)error{
    //Attention：change since v3.0.0
    //Attention: SDK read ATT list completed when last characteristic's descriptors had callback.
    if (peripheral.services.lastObject.characteristics.lastObject == characteristic) {
        [self readGattFinishWithPeripheral:peripheral];
    }
}

- (void)readGattFinishWithPeripheral:(CBPeripheral *)peripheral{
    SEL scanATTListMethod = @selector(connectAndReadAttTimeOutWithUUID:);
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:scanATTListMethod object:peripheral.identifier.UUIDString];
    });
    if (self.state == StateAddDevice_provision || self.state == StateAddDevice_keyBind || self.state == StateOTA) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(delayWriteForProvision) object:nil];
            [self performSelector:@selector(delayWriteForProvision) withObject:nil afterDelay:kLoopWriteForBeaconDelayTime];
        });
    } else {
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(delayMeshProxyInit) object:nil];
            [self performSelector:@selector(delayMeshProxyInit) withObject:nil afterDelay:kMeshProxyInitDelayTime];
        });
    }
}

- (void)peripheral:(CBPeripheral *)peripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error{
    if ([characteristic.UUID.UUIDString isEqualToString:kPROXY_Out_CharacteristicsID] || [characteristic.UUID.UUIDString isEqualToString:kPBGATT_Out_CharacteristicsID]) {
        TeLog(@"app didUpdateValueFor:%@",characteristic.value);
        dealNotifyData(characteristic.value);
    }
    if ([characteristic.UUID.UUIDString isEqualToString:kOTA_CharacteristicsID]) {
        if (self.bleReadOTACharachteristicCallBack) {
            self.bleReadOTACharachteristicCallBack(characteristic);
        }
    }
    if ([characteristic.UUID.UUIDString isEqualToString:kOnlineStatusCharacteristicsID]) {
        [self performSelector:@selector(anasislyOnlineStatueDataFromUUID:) onThread:self.anasislyResponseThread withObject:characteristic.value waitUntilDone:NO];
    }
}

- (void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error{
    master_terminate_ble_callback();
    if((hci_tx_fifo.wptr-hci_tx_fifo.rptr) != 0){
        LOG_MSG_INFO(TL_LOG_COMMON,0, 0,"hci fifo not empty!!!!!! wptr:%d rptr:%d",hci_tx_fifo.wptr,hci_tx_fifo.rptr);
    }

    //Attention: SDK can't set store.peripheral to nil, because bleDisconnectOrConnectFailCallBack need this peripheral.
    //        store.peripheral = nil

    BOOL shouldRetry = YES;
    if (error) {
        SigScanRspModel *scanModel = [SigDataSource.share getScanRspModelWithUUID:peripheral.identifier.UUIDString];
        TeLog(@"disconnect uuid error：%@，%@，mac=%@",peripheral.identifier.UUIDString,error.description,scanModel.macAddress);
        [self disconnectOrConnectFailDo:peripheral];
        [self startAutoConnect];
    } else {
        TeLog(@"disconnect uuid：%@",peripheral.identifier.UUIDString);
        if (self.state == StateAddDevice_provision) {
//            TeLog(@"shouldRetry = NO");
            shouldRetry = NO;
        }
        if (self.cancelConnectCallBack) {
            self.cancelConnectCallBack();
        }
    }
    
    //perfect logic: when state of SDK is keyBind, SDK will retry 3 times when node connect fail or disconnect with error.
    if (shouldRetry) {
        if (self.addDevice_prvisionSuccessCallBack != nil && self.addDevice_keyBindSuccessCallBack != nil && self.state == StateAddDevice_keyBind && self.connectWithUUIDCount > 0) {
            TeLog(@"reconnect on keyBind");
            self.connectWithUUIDCount --;
            [self connectPeripheralWithUUID:peripheral.identifier.UUIDString];
        }
    }

    //set all nodes outline, stop all chect outline timers, callback to update UI.
    if (self.state == StateNormal) {
        while (_checkOfflineTimerDict.count > 0) {
            NSNumber *address = _checkOfflineTimerDict.allKeys.firstObject;
            [self stopCheckOfflineTimerWithAddress:address];
        }
        [SigDataSource.share setAllDevicesOutline];
        if (self.commandHandle.checkOfflineCallBack) {
            SigNodeModel *device = [SigDataSource.share getNodeWithUUID:peripheral.identifier.UUIDString];
            if (device) {
                self.commandHandle.checkOfflineCallBack(@(device.address));
            }
        }
    }
}

- (void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error{
    master_terminate_ble_callback();
    
    TeLog(@"");
    [self disconnectOrConnectFailDo:peripheral];
    [self startAutoConnect];
}

#pragma mark - API Private
- (void)disconnectOrConnectFailDo:(CBPeripheral *)peripheral{
    [self.store ressetParameters];
    if (self.bleDisconnectOrConnectFailCallBack) {
        self.bleDisconnectOrConnectFailCallBack(peripheral);
    }
}

- (void)delayMeshProxyInit{
    TeLog(@"");
    //old code before v2.8.1
//    mesh_proxy_set_filter_init(SigDataSource.share.curProvisionerModel.allocatedUnicastRange.firstObject.lowIntAddress);
//    mesh_tx_sec_nw_beacon_all_net(1);//send beacon, blt_sts can only be 0 or 1.
//
//    dispatch_async(dispatch_get_main_queue(), ^{
//        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(delayWriteForProvision) object:nil];
//        [self performSelector:@selector(delayWriteForProvision) withObject:nil afterDelay:kLoopWriteForBeaconDelayTime];
//    });
    
    //change since v2.8.2
    __block int responseCount = 0;
    __weak typeof(self) weakSelf = self;
    [self setFilterWithLocationAddress:SigDataSource.share.curProvisionerModel.allocatedUnicastRange.firstObject.lowIntAddress complete:^{
        responseCount ++;
        if (responseCount == kSetFilterPacketCount) {
            [weakSelf cancelSetFilterWithLocationAddressTimeout];
            set_pair_login_ok(1);
            //change since v3.1.0
            if (SigDataSource.share.hasNodeExistTimeModelID) {
                [weakSelf.commandHandle statusNowTime];
            }
            [weakSelf cancelSetFilterWithLocationAddressTimeout];
            [weakSelf delayWriteForProvision];
            weakSelf.setFilterResponseCallBack = nil;
        }
    }fail:^{
        TeLog(@"setFilter fail.");
    }];
    mesh_tx_sec_nw_beacon_all_net(1);//send beacon, blt_sts can only be 0 or 1.
}

- (void)delayWriteForProvision{
    dispatch_async(dispatch_get_main_queue(), ^{
        if (self.bleFinishScanedCharachteristicCallBack && self.store.peripheral) {
//            set_pair_login_ok(1);
            self.bleFinishScanedCharachteristicCallBack(self.store.peripheral);
        }
    });
}

- (void)clearCachelist{
    [self.tempProvisionFailList removeAllObjects];
    [self.tempProvisionSuccessList removeAllObjects];
}

- (void)rssiHightest{
    [self stopScan];
    if (self.bleScanNewDeviceCallBack) {
        CBPeripheral *peripheral = [self.store hightestRSSI];
        if (peripheral) {
            self.bleScanNewDeviceCallBack(peripheral, NO);
        }else{
            [self startAutoConnect];
        }
    }
}

- (void)startAutoConnect {
    [self stopAutoConnect];
    if (SigDataSource.share.nodes.count > 1) {
        self.autoConnectScanTimer = [NSTimer scheduledTimerWithTimeInterval:kScanForAutoConnectInterval target:self selector:@selector(scanForAutoConnect) userInfo:nil repeats:YES];
        [[NSRunLoop currentRunLoop] addTimer:self.autoConnectScanTimer forMode:NSRunLoopCommonModes];
        [self.autoConnectScanTimer fire];
    }
}

/// Stop auto connect(停止自动连接流程)
- (void)stopAutoConnect {
    kEndTimer(self.autoConnectScanTimer);
}

- (void)scanForAutoConnect{
    TeLog(@"");
    if (self.state == StateNormal) {
        CBPeripheral *tem = [self currentPeripheral];
        if (tem && (tem.state == CBPeripheralStateConnected || tem.state == CBPeripheralStateConnecting)) {
            return;
        }
        
        TeLog(@"auto reconnect");
        if (self.manager.isScanning) {
            [self.manager stopScan];
        }
        [self startWorkNormalWithComplete:nil];
    }
}

- (BOOL)setScanDelayFlagEnable:(BOOL)enable {
    if (enable && !_isScanEnoughTime) {
        if (!_isScanDelaying) {
            _isScanDelaying = YES;
            dispatch_async(dispatch_get_main_queue(), ^{
                [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(scanEnoughTime) object:nil];
                [self performSelector:@selector(scanEnoughTime) withObject:nil afterDelay:kScanMacAddressDelay];
            });
        }
        return NO;
    }
    return YES;
}

- (void)scanEnoughTime {
    _isScanEnoughTime = YES;
    _isScanDelaying = NO;
}

#pragma mark - API SDK lib need
- (void)writeForPROXYIn:(NSData *)d{
    commentTime = [[NSDate date] timeIntervalSince1970];
    [self.store writeForPROXYIn:d];
}

- (void)writeForPBGATTIn:(NSData *)d{
    commentTime = [[NSDate date] timeIntervalSince1970];
    [self.store writeForPBGATTIn:d];
}

- (void)writeForOnlineStatus:(NSData *)d{
    commentTime = [[NSDate date] timeIntervalSince1970];
    [self.store writeForOnlineStatus:d];
}

- (void)writeOTAData:(NSData *)d{
    [self.store writeOTAData:d];
}

- (CBPeripheral *)currentPeripheral{
    return self.store.peripheral;
}

/// Set element count of current provision device.(记录当前provision的设备的element个数)
- (void)setElementCount:(UInt8)ele_count{
    self.currentElementCount = ele_count;
}

/// Get address of current provision device.(获取当前provision的设备的短地址)
- (UInt16)getCurrentProvisionAddress{
    return _currentProvisionIndex;
}

/// Get current key bind type.(获取当前的添加模式)
- (KeyBindTpye)getCurrentKeyBindType{
    return _currentAddType;
}


/**
 when command need response ,and it'll update information here
 
 @param m model
 */
- (void)responseBack:(ResponseModel *)m{
    __block BOOL hasUpdateResponse = NO;
    NSOperationQueue *oprationQueue = [[NSOperationQueue alloc] init];
    [oprationQueue addOperationWithBlock:^{
        ResponseModel *tempResponseModel = m;
        Opcode op = m.opcode;
        
        switch (op) {
                //response of access_get_lum()
            case OpcodeBrightnessGetResponse:
            {
                tempResponseModel.currentValue = m.currentValue;
                tempResponseModel.pointValue = m.pointValue;
                tempResponseModel.currentState = m.currentValue != 0;
                //get brightness in call access_get_lum()
                if (self.commandHandle.changeBrightnessCallBack) {
                    [[SigDataSource share] updateResponseModelWithResponse:tempResponseModel];
                    dispatch_async(dispatch_get_main_queue(), ^{
                        self.commandHandle.changeBrightnessCallBack(tempResponseModel);
                    });
                    hasUpdateResponse = YES;
                }
                //Attention: node will notify data that opcode is 0x4E82, when node that isn't store.peripheral power to electricity.
                if (self.commandHandle.notifyOnlineStatusCallBack) {
                    tempResponseModel.currentState = m.currentValue != 0;
                    dispatch_async(dispatch_get_main_queue(), ^{
                        self.commandHandle.notifyOnlineStatusCallBack(tempResponseModel);
                    });
                }
            }
                break;
                //response of access_cmd_onoff_get()
            case OpcodeOnOffStatusResponse:
            {
                tempResponseModel.currentState = m.currentState;
                tempResponseModel.pointState = m.pointState;
                if (self.commandHandle.switchOnOffCallBack) {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        self.commandHandle.switchOnOffCallBack(tempResponseModel);
                    });
                }
                if (self.commandHandle.getOnlineStatusCallBack) {
                    [[SigDataSource share] updateResponseModelWithResponse:tempResponseModel];
                    hasUpdateResponse = YES;
                    dispatch_async(dispatch_get_main_queue(), ^{
                        self.commandHandle.getOnlineStatusCallBack(tempResponseModel);
                    });
                }
            }
                break;
            case OpcodeSubListGetResponse:
            {
                [tempResponseModel updateResponseSubscrbeList:m isClient:m.isClient];
                if (self.commandHandle.getSubscribeListCallBack) {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        self.commandHandle.getSubscribeListCallBack(tempResponseModel);
                    });
                }
            }
                break;
            case OpcodeEditSubListResponse:
            {
                [tempResponseModel updateResponseSubscrbeList:m isClient:m.isClient];
                if (self.commandHandle.editSubscribeListCallBack) {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        self.commandHandle.editSubscribeListCallBack(tempResponseModel);
                    });
                }
                //                self.commandHandle.editSubscribeListCallBack = nil;
            }
                break;
            case OpcodePublishAddressResponse:
            {
                if (self.commandHandle.publishListResponseCallBack) {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        self.commandHandle.publishListResponseCallBack(m);
                    });
                }
            }
                break;
                //response of access_cmd_get_light_ctl()
            case OpcodeCurrentCTLResponse:
            {
                if(m.currentValue != 0){
                    tempResponseModel.currentValue = m.currentValue;
                }
                tempResponseModel.currentState = m.currentValue != 0;
                tempResponseModel.pointValue = m.pointValue;
                
                BOOL change = [[SigDataSource share] updateResponseModelWithResponse:tempResponseModel];
                hasUpdateResponse = YES;

                if (self.commandHandle.changeTemperatureCallBack) {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        self.commandHandle.changeTemperatureCallBack(tempResponseModel);
                    });
                }
                
                //when publish's modelID is SIG_MD_LIGHT_CTL_S.
                if (change && self.commandHandle.notifyPublishStatusCallBack) {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        self.commandHandle.notifyPublishStatusCallBack(tempResponseModel);
                    });
                }
                SigNodeModel *device = [SigDataSource.share getNodeWithAddress:m.address];
                if (device) {
                    [self startCheckOfflineTimerWithAddress:@(device.address)];
                }
                
            }
                break;
            case OpcodeHSLNotifyResponse:
            {
                SigNodeModel *device = [SigDataSource.share getNodeWithAddress:m.address];
                //when publish's modelID is SIG_MD_LIGHT_HSL_S.
                if (self.commandHandle.notifyPublishStatusCallBack) {
                    if (device && device.state == DeviceStateOutOfLine) {
                        dispatch_async(dispatch_get_main_queue(), ^{
                            self.commandHandle.notifyPublishStatusCallBack(tempResponseModel);
                        });
                    }
                }
                if (self.commandHandle.changeHSLCallBack) {
                    [[SigDataSource share] updateResponseModelWithResponse:tempResponseModel];
                    hasUpdateResponse = YES;
                    dispatch_async(dispatch_get_main_queue(), ^{
                        self.commandHandle.changeHSLCallBack(tempResponseModel);
                        self.commandHandle.changeHSLCallBack = nil;
                    });
                }
                if (device) {
                    [self startCheckOfflineTimerWithAddress:@(device.address)];
                }
            }
                break;
            case OpcodeDelectDeviceResponse:
            {
                if (self.commandHandle.delectDeviceCallBack) {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        self.commandHandle.delectDeviceCallBack();
                    });
                }
            }
                break;
            case OpcodeSetTimeResponse:
            {
                if (self.commandHandle.setTimeCallBack) {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        self.commandHandle.setTimeCallBack();
                        self.commandHandle.setTimeCallBack = nil;
                    });
                }
            }
                break;
            case OpcodeSceneRegisterStatusResponse:
            {
                Byte *pu = (Byte *)[m.rspData bytes];
                unsigned int stateCode = 0;
                memcpy(&stateCode, pu + 8+1, 1);
                //state:0success,1Scene Register Full,2Scene Not Found
                if (stateCode == 0) {
                    //add or edit scene
                    if (self.commandHandle.saveSceneCallBack) {
                        dispatch_async(dispatch_get_main_queue(), ^{
                            self.commandHandle.saveSceneCallBack(m);
                            self.commandHandle.saveSceneCallBack = nil;
                        });
                    }
                    //delete scene
                    if (self.commandHandle.delSceneCallBack) {
                        dispatch_async(dispatch_get_main_queue(), ^{
                            self.commandHandle.delSceneCallBack(m);
                            self.commandHandle.delSceneCallBack = nil;
                        });
                    }
                    //get scene register status
                    if (self.commandHandle.getSceneRegisterStatusCallBack) {
                        dispatch_async(dispatch_get_main_queue(), ^{
                            self.commandHandle.getSceneRegisterStatusCallBack(m);
                            self.commandHandle.getSceneRegisterStatusCallBack = nil;
                        });
                    }
                }
            }
                break;
            case OpcodeRecallSceneResponse:
            {
                Byte *pu = (Byte *)[m.rspData bytes];
                unsigned int stateCode = 0;
                memcpy(&stateCode, pu + 7+1, 1);
                //state:0success,1Scene Register Full,2Scene Not Found
                if (stateCode == 0) {
                    if (self.commandHandle.recallSceneCallBack) {
                        dispatch_async(dispatch_get_main_queue(), ^{
                            self.commandHandle.recallSceneCallBack(tempResponseModel);
                        });
                    }
                }
            }
                break;
            case OpcodeSetSchedulerResponse:
            {
                if (self.commandHandle.setSchedulerActionCallBack) {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        self.commandHandle.setSchedulerActionCallBack(tempResponseModel);
                    });
                }
            }
                break;
            case OpcodeChangeLevelResponse:
            {
                if (self.commandHandle.changeLevelCallBack) {
                    [[SigDataSource share] updateResponseModelWithResponse:tempResponseModel];
                    hasUpdateResponse = YES;
                    dispatch_async(dispatch_get_main_queue(), ^{
                        self.commandHandle.changeLevelCallBack(tempResponseModel);
                    });
                }
            }
                break;
            case OpcodeCTLTemperatureStatusResponse:
            {
                tempResponseModel.currentValue = m.currentValue;
                tempResponseModel.pointValue = m.pointValue;
                if (self.commandHandle.changeTemperatureCallBack) {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        self.commandHandle.changeTemperatureCallBack(tempResponseModel);
                    });
                }
            }
                break;
            case OpcodeSetUUIDResponse:
            {
                tempResponseModel.currentValue = m.currentValue;
                tempResponseModel.pointValue = m.pointValue;
                if (self.commandHandle.setUUIDResultCallBack) {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        self.commandHandle.setUUIDResultCallBack(tempResponseModel);
                        self.commandHandle.setUUIDResultCallBack = nil;
                    });
                }
            }
                break;
            case OpcodeGetSchedulerStatusResponse:
            {
                if (self.commandHandle.getSchedulerStatusCallBack) {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        self.commandHandle.getSchedulerStatusCallBack(tempResponseModel);
                    });
                }
            }
            case OpcodeGetFwInfoResponse:
            {
                if (self.commandHandle.getFwInfoCallBack) {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        self.commandHandle.getFwInfoCallBack(tempResponseModel);
                    });
                }
            }
                break;
            default:
                NSLog(@"that response data isn't anasisly，OPCode:%d",tempResponseModel.opcode);
                saveLogData([NSString stringWithFormat:@"that response data isn't anasisly，OPCode:%d",tempResponseModel.opcode]);
                break;
        }
        //Attention: all responseModel need update to SigDataSource. eg: group onoff response data will update to SigDataSource in the following code.
        if (!hasUpdateResponse) {
            [[SigDataSource share] updateResponseModelWithResponse:tempResponseModel];
        }
     }];
}

- (void)responseBackOfVendorID:(ResponseModel *)m{
    if (self.commandHandle.responseVendorIDCallBack) {
        VendorResponseModel *tempResponseModel = [[VendorResponseModel alloc] initWithResponseData:m.rspData];
        self.commandHandle.responseVendorIDCallBack(tempResponseModel);
    }
}

/**
 if you send a command which need response, and you can receive response here
 !attention: before last command response back , it'll not corresponding new command
 
 @param opcode command opcode
 @param requestCount device count you want corresponding
 @param responseCount device count corresponding actually
 */
- (void)meshReliableResponseBack:(u16)opcode requestCount:(u32)requestCount responseCount:(u32)responseCount{
    TeLog(@"response back -> op: %d, request :%d, response :%d",opcode,requestCount,responseCount);
    [self resetMeshBusyTimer];
}

///app should reset mesh busyTimer when change mesh.(重置isBusy定时器，切换mesh网络或者初始化mesh时使用)
- (void)resetMeshBusyTimer{
    if (self.commandHandle.busyTimer) {
        [[NSNotificationCenter defaultCenter] postNotificationName:NotifyCommandIsBusyOrNot object:nil userInfo:@{CommandIsBusyKey : @NO}];
    }
    kEndTimer(self.commandHandle.busyTimer);
}

- (void)addDevice_provisionResultBack:(BOOL)isSuccess{
    TeLog(@"addDevice_provision Result back isSuccess:%d",isSuccess);
    dispatch_async(dispatch_get_main_queue(), ^{
        if (self.store.peripheral) {
            if (isSuccess) {
                if (self.addDevice_prvisionSuccessCallBack) {
                    self.addDevice_prvisionSuccessCallBack(self.store.peripheral.identifier.UUIDString, self.currentProvisionIndex);
                }
             } else {
                [self doWhenAddDeviceFail:self.store.peripheral];
            }
        }
    });
}

- (void)addDevice_keyBindResultBack:(BOOL)isSuccess{
    TeLog(@"addDevice_keybind Result back isSuccess:%d",isSuccess);
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(reKeyBindTimeout) object:nil];
        if (self.store.peripheral) {
            if (isSuccess) {
                [self doWhenKeyBindSuccess:self.store.peripheral];
            } else {
                [self doWhenAddDeviceFail:self.store.peripheral];
            }
        }
        //In add process, when provision success, self.currentProvisionIndex need add self.currentElementCount no matter keyBind success or fail, then scan next node.
        if (self.addDevice_prvisionSuccessCallBack != nil && self.addDevice_keyBindSuccessCallBack != nil) {
            self.currentProvisionIndex += self.currentElementCount;
            self.currentElementCount = 0;
        }
    });
}

- (void)doWhenAddDeviceFail:(CBPeripheral *)p{
    if (self.addDevice_prvisionSuccessCallBack != nil && self.addDevice_keyBindSuccessCallBack != nil) {
        //add device process
        TeLog(@"add device fail");
        [self.tempProvisionFailList addObject:p.identifier.UUIDString];
        if (self.prvisionFailCallBack) {
            self.prvisionFailCallBack(p.identifier.UUIDString);
        }
        self.state = StateAddDevice_provision;
        if (self.isAutoAddDevice) {
            [self scanNextDeviceForProvision];
        } else {
            [self scanForProvisionDeviceTimeOut];
        }
    } else {
        //reKeyBind process
        if (self.prvisionFailCallBack) {
            self.prvisionFailCallBack(p.identifier.UUIDString);
        }
        [self cleanReKeyBindCallback];
    }
}

- (void)doWhenKeyBindSuccess:(CBPeripheral *)p{
    TeLog(@"keybind success");
    if (![self.tempProvisionSuccessList.allKeys containsObject:p.identifier.UUIDString] || self.isRemoteAdd) {
        self.tempProvisionSuccessList[p.identifier.UUIDString] = @(self.currentProvisionIndex);
        if (self.addDevice_keyBindSuccessCallBack) {
            self.addDevice_keyBindSuccessCallBack(p.identifier.UUIDString, self.currentProvisionIndex);
        }
    }
    self.state = StateAddDevice_provision;

    if (self.addDevice_prvisionSuccessCallBack != nil && self.addDevice_keyBindSuccessCallBack != nil) {
        //add device process
        if (self.isAutoAddDevice) {
            [self scanNextDeviceForProvision];
        } else {
            if (self.prvisionFinishCallBack) {
                self.prvisionFinishCallBack();
                self.currentAddType = KeyBindTpye_Normal;
            }
            //只添加一个，不需要断开
//            [self scanForProvisionDeviceTimeOut];
        }
    } else {
        //reKeyBind process
        [self cancelConnection:self.store.peripheral complete:nil];
        [self cleanReKeyBindCallback];
    }
}

- (void)scanNextDeviceForProvision{
    __weak typeof(self) weakSelf = self;
    [self cancelConnection:self.store.peripheral complete:^{
        TeLog(@"Scan for Provision");
        kEndTimer(weakSelf.provisionScanTimer);
        weakSelf.provisionScanTimer = [NSTimer scheduledTimerWithTimeInterval:kScanForProvisionDeviceInterval target:weakSelf selector:@selector(startScan) userInfo:nil repeats:YES];
        [[NSRunLoop currentRunLoop] addTimer:weakSelf.provisionScanTimer forMode:NSRunLoopCommonModes];
        [weakSelf.provisionScanTimer fire];
        
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(connectPeripheralTimeOutInAddDevice) object:nil];
            [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(scanForProvisionDeviceTimeOut) object:nil];
            [weakSelf performSelector:@selector(scanForProvisionDeviceTimeOut) withObject:nil afterDelay:kScanTimeOutInAddDevice];
        });
    }];
}

- (void)scanForProvisionDeviceTimeOut{
    TeLog(@"clean block");
    [self stopScan];
    [self cancelAllConnecttionWithComplete:nil];

    self.state = StateNormal;
    if (self.prvisionFinishCallBack) {
        self.prvisionFinishCallBack();
        self.currentAddType = KeyBindTpye_Normal;
    }
    if (self.addDevice_prvisionSuccessCallBack != nil && self.addDevice_keyBindSuccessCallBack != nil) {
        //add device process
        if (self.isAutoAddDevice) {
            [self cleanProvisionCallback];
        }
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(connectPeripheralTimeOutInAddDevice) object:nil];
    });
}

- (void)connectPeripheralTimeOutInAddDevice{
    [self scanNextDeviceForProvision];
}

- (void)cleanProvisionCallback{
    self.addDevice_prvisionSuccessCallBack = nil;
    self.addDevice_keyBindSuccessCallBack = nil;
    self.prvisionFailCallBack = nil;
    self.prvisionFinishCallBack = nil;
    self.isAutoAddDevice = NO;
}

- (void)cleanReKeyBindCallback{
    self.addDevice_keyBindSuccessCallBack = nil;
    self.prvisionFailCallBack = nil;
    self.isAutoAddDevice = NO;
}

#pragma mark check outline timer
- (void)setDeviceOffline:(NSTimer *)timer{
    UInt16 adr = [timer.userInfo[@"address"] intValue];
    
    [self stopCheckOfflineTimerWithAddress:@(adr)];
    
    SigNodeModel *device = [SigDataSource.share getNodeWithAddress:adr];
    if (device) {
        if (device.hasPublishFunction && device.hasOpenPublish) {
            TeLog(@"setDeviceOffline:0x%02X",adr);
            device.state = DeviceStateOutOfLine;
            NSString *str = [NSString stringWithFormat:@"======================device offline:0x%02X======================",adr];
            saveLogData(str);
            if (self.commandHandle.checkOfflineCallBack) {
                self.commandHandle.checkOfflineCallBack(@(adr));
            }
        }
    }
}

- (void)startCheckOfflineTimerWithAddress:(NSNumber *)address{
    SigNodeModel *device = [SigDataSource.share getNodeWithAddress:address.intValue];
    if (device && device.hasPublishFunction && device.hasOpenPublish && device.hasPublishPeriod) {
        [self stopCheckOfflineTimerWithAddress:address];
        NSTimer *timer = [NSTimer scheduledTimerWithTimeInterval:kOfflineInterval target:self selector:@selector(setDeviceOffline:) userInfo:@{@"address":address} repeats:NO];
        [[NSRunLoop currentRunLoop] addTimer:timer forMode:NSRunLoopCommonModes];
        _checkOfflineTimerDict[address] = timer;
    }
}

- (void)stopCheckOfflineTimerWithAddress:(NSNumber *)address{
    NSTimer *timer = _checkOfflineTimerDict[address];
    if (timer) {
        [_checkOfflineTimerDict removeObjectForKey:address];
    }
    kEndTimer(timer);
}

#pragma mark - API Public
- (void)startScan{
    _isScanEnoughTime = NO;
    _isScanDelaying = NO;
    if (self.state != normal) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(rssiHightest) object:nil];
        });
    }
    if (self.manager.state == CBCentralManagerStateUnknown) {
        TeLog(@"startScan afterDelay:0.1");
        [self performSelector:@selector(startScan) withObject:nil afterDelay:0.1];
    }else if (self.manager.state == CBCentralManagerStatePoweredOn){
        TeLog(@"scanForPeripheralsWithServices");
        [self.manager scanForPeripheralsWithServices:@[[CBUUID UUIDWithString:kPBGATTService],[CBUUID UUIDWithString:kPROXYService]] options:@{CBCentralManagerScanOptionAllowDuplicatesKey:@YES}];
    }
}

- (void)stopScan{
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(startScan) object:nil];
    });
    kEndTimer(self.provisionScanTimer);
    
    if (self.manager && self.manager.isScanning) {
        TeLog(@"stopScan");
        [self.manager stopScan];
    }
}

- (BOOL)isConnected{
    if (self.store.peripheral) {
        return self.store.peripheral.state == CBPeripheralStateConnected ? YES : NO;
    }
    return NO;
}

/// Is remote add type(当前是否是remote添加状态)
- (BOOL)isRemoteAdd{
    if (self.uuid && self.uuid.length) {
        return YES;
    }
    return NO;
}

/*
 *  CBConnectPeripheralOptionNotifyOnConnectionKey
 A Boolean value that specifies whether the system should display an alert for a given peripheral if the app is suspended when a successful connection is made
 *  CBConnectPeripheralOptionNotifyOnDisconnectionKey
 A Boolean value that specifies whether the system should display a disconnection alert for a given peripheral if the app is suspended at the time of the disconnection.
 *  CBConnectPeripheralOptionNotifyOnNotificationKey
 A Boolean value that specifies whether the system should display an alert for all notifications received from a given peripheral if the app is suspended at the time.
 */
- (void)connectPeripheral:(CBPeripheral *)p{
    SigScanRspModel *scanModel = [SigDataSource.share getScanRspModelWithUUID:p.identifier.UUIDString];

    __weak typeof(self) weakSelf = self;
    [self cancelAllConnecttionWithComplete:^{
        if (p.state != CBPeripheralStateConnecting && p.state != CBPeripheralStateConnected) {
            if (SigDataSource.share.scanList.count == 0) {
                TeLog(@"scanList is empty.");
            }
            TeLog(@"mac = %@",scanModel.macAddress);
            weakSelf.store.peripheral = p;
            [weakSelf.manager connectPeripheral:p options:nil];
            dispatch_async(dispatch_get_main_queue(), ^{
                [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(connectAndReadAttTimeOutWithUUID:) object:p.identifier.UUIDString];
                [weakSelf performSelector:@selector(connectAndReadAttTimeOutWithUUID:) withObject:p.identifier.UUIDString afterDelay:kConnectAndReciveATTListTime];
            });
        }
    }];
}

- (void)connectPeripheralWithUUID:(NSString *)uuidString{
    CBPeripheral *p = [self getPeripheralWithUUID:uuidString];
    if (p) {
        SigScanRspModel *scanModel = [SigDataSource.share getScanRspModelWithUUID:p.identifier.UUIDString];

        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(connectWithUUIDTimeout:) object:uuidString];
            [self performSelector:@selector(connectWithUUIDTimeout:) withObject:uuidString afterDelay:kConnectWithUUIDTimeout];
            //Attention: SDK need quote peripheral to avoid the system reasel peripheral. Fix bug log: [CoreBluetooth] API MISUSE: Cancelling connection for unused peripheral.
            TeLog(@"mac = %@",scanModel.macAddress);
            self.store.peripheral = p;
            [self.manager connectPeripheral:p options:nil];
        });
    }
}

- (void)connectAndReadAttTimeOutWithUUID:(NSString *)uuid{
    [self connectWithUUIDTimeout:uuid];
    if (self.state == StateAddDevice_provision && self.isAutoAddDevice) {
        [self scanNextDeviceForProvision];
    }
}

- (void)connectWithUUIDTimeout:(NSString *)uuid{
    CBPeripheral *p = [self getPeripheralWithUUID:uuid];
    if (p) {
        TeLog(@"connectWithUUIDTimeout uuid:%@",uuid);
        [self.manager cancelPeripheralConnection:p];
        if ([self.currentPeripheral.identifier.UUIDString isEqualToString:uuid]) {
            dispatch_async(dispatch_get_main_queue(), ^{
                if (self.state == StateAddDevice_keyBind) {
                    [self addDevice_keyBindResultBack:NO];
                }else{
                    TeLog(@"self.state =%lu",(unsigned long)self.state);
                    if (![self.tempProvisionFailList containsObject:uuid]) {
                        [self.tempProvisionFailList addObject:uuid];
                    }
                }
            });
        }
    }
}

- (void)cancelConnection:(CBPeripheral *)p complete:(bleCancelConnectCallBack)complete{
    self.cancelConnectCallBack = complete;
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(connectWithUUIDTimeout:) object:p.identifier.UUIDString];
    });
    if (p && p.state != CBPeripheralStateDisconnected && p.state != CBPeripheralStateDisconnecting) {
        TeLog(@"cancel single connection");
        [self.manager cancelPeripheralConnection:p];
    }else{
        if (p.state == CBPeripheralStateDisconnected) {
            if (self.cancelConnectCallBack) {
                self.cancelConnectCallBack();
            }
        }
    }
}

- (void)cancelAllConnecttionWithComplete:(bleCancelConnectCallBack)complete{
//    TeLog(@"");
    for (CBPeripheral *p in self.store.scanedPeripherals) {
        dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
        [self cancelConnection:p complete:^{
            dispatch_semaphore_signal(semaphore);
        }];
        //Most provide 4 seconds to disconnect bluetooth connection
        dispatch_semaphore_wait(semaphore, 4);
    }
    if (self.store.peripheral) {
        __weak typeof(self) weakSelf = self;
        [self cancelConnection:self.store.peripheral complete:^{
            [weakSelf.store ressetParameters];
            weakSelf.store.peripheral = nil;
            if (complete) {
                complete();
            }
        }];
    }else{
        if (complete) {
            complete();
        }
    }
}

- (CBPeripheral *)getPeripheralWithUUID:(NSString *)uuidString{
    NSMutableArray *identiferArray = [[NSMutableArray alloc] init];
    [identiferArray addObject:[[NSUUID alloc] initWithUUIDString:uuidString]];
    NSArray *knownPeripherals = [self.manager retrievePeripheralsWithIdentifiers:identiferArray];
    if (knownPeripherals.count > 0) {
        TeLog(@"get peripherals from uuid:%@ count: %lu",uuidString,(unsigned long)knownPeripherals.count);
        return knownPeripherals.firstObject;
    }
    return nil;
}

/**
 if you did provision process , you could call this method, and it'll call back what you need

 @param availableList the devices you added(注意：SDK收到非该数组的设备的数据是不会处理的)
 @param canControl you can send command to theos devices
 */
- (void)startWorkNormal:(NSArray <NSString *>*)availableList canControl:(workWithPeripheralCallBack)canControl NS_DEPRECATED_IOS(2_0, 9_0, "Use -startWorkNormalWithComplete:") __TVOS_PROHIBITED {
    [self startWorkNormalWithComplete:canControl];
}


/**
 if you did provision process , you could call this method, and it'll call back what you need
 
 @param complete callback UUIDstring of direct connection node
 */
- (void)startWorkNormalWithComplete:(workWithPeripheralCallBack)complete {
    self.state = StateNormal;
    [self.store clearRecord];
    if (complete) {
        self.workWithPeripheralCallBack = complete;
    }
    [self blockState];
    if (self.store.peripheral && self.store.peripheral.state == CBPeripheralStateConnected && [self.store isWorkNormal] && [SigDataSource.share existPeripheralUUIDString:self.store.peripheral.identifier.UUIDString]) {
        if (self.workWithPeripheralCallBack) {
            self.workWithPeripheralCallBack(self.store.peripheral.identifier.UUIDString);
        }
    } else {
        __weak typeof(self) weakSelf = self;
        [self cancelAllConnecttionWithComplete:^{
            [weakSelf startScan];
            dispatch_async(dispatch_get_main_queue(), ^{
                [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(rssiHightest) object:nil];
                [weakSelf performSelector:@selector(rssiHightest) withObject:nil afterDelay:kScanForHightestRSSIInterval];
            });
        }];
    }
}

/**
 if you need do provision , you should call this method, and it'll call back what you need
 
 @param address address of new device
 @param networkKey network key, which provsion need, you can see it as password of the mesh
 @param netkeyIndex netkey index
 @param unicastAddress address of remote device
 @param uuid uuid of remote device
 @param type KeyBindTpye_Normal是普通添加模式，KeyBindTpye_Quick是快速添加模式
 @param isAuto 添加完成一个设备后，是否自动扫描添加下一个设备
 @param provisionSuccess call back when a device provision successful
 @param keyBindSuccess call back when a device keybind successful
 @param fail call back when a device add to the mesh fail
 @param finish finish add the available devices list to the mesh
 */
- (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex unicastAddress:(UInt16)unicastAddress uuid:(NSData *)uuid keyBindType:(KeyBindTpye)type isAutoAddNextDevice:(BOOL)isAuto provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess fail:(prvisionFailCallBack)fail finish:(prvisionFinishCallBack)finish{
    self.unicastAddress = unicastAddress;
    self.uuid = uuid;
    self.state = StateAddDevice_provision;
    self.currentAddType = type;
    self.isAutoAddDevice = isAuto;
    
    self.addDevice_prvisionSuccessCallBack = provisionSuccess;
    self.addDevice_keyBindSuccessCallBack = keyBindSuccess;
    self.prvisionFailCallBack = fail;
    self.prvisionFinishCallBack = finish;
    
    self.currentProvisionIndex = address;
    self.currentElementCount = 0;
    self.currentNetkeyIndex = netkeyIndex;
    self.NetworkKey = [NSData dataWithData:networkKey];
    [self blockState];
    if (uuid == nil) {
        //normal add
        if (self.isConnected) {
            [self provisionAction];
            TeLog(@"do provisioning");
        } else {
            [self scanNextDeviceForProvision];
            TeLog(@"scan for provisioning");
        }
    } else {
        //remote add
        __weak typeof(self) weakSelf = self;
        [self.commandHandle setRemoteAddDeviceInfoWithUnicastAddress:self.unicastAddress uuid:self.uuid complete:^(ResponseModel *m) {
            TeLog(@"setRemoteAddDeviceInfoWithUnicastAddress success");
            [weakSelf provisionAction];
        }];
    }
}

/**
 if you need do provision , you should call this method, and it'll call back what you need
 
 @param address address of new device
 @param networkKey network key, which provsion need, you can see it as password of the mesh
 @param netkeyIndex netkey index
 @param peripheral device need add to mesh
 @param type KeyBindTpye_Normal是普通添加模式，KeyBindTpye_Quick是快速添加模式
 @param provisionSuccess call back when a device provision successful
 @param keyBindSuccess call back when a device keybind successful
 @param fail call back when a device add to the mesh fail
 @param finish finish add the available devices list to the mesh
 */
- (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex peripheral:(CBPeripheral *)peripheral keyBindType:(KeyBindTpye)type provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess fail:(prvisionFailCallBack)fail finish:(prvisionFinishCallBack)finish{
    self.state = StateAddDevice_provision;
    self.currentAddType = type;
    self.isAutoAddDevice = NO;
    
    self.addDevice_prvisionSuccessCallBack = provisionSuccess;
    self.addDevice_keyBindSuccessCallBack = keyBindSuccess;
    self.prvisionFailCallBack = fail;
    self.prvisionFinishCallBack = finish;
    
    self.currentProvisionIndex = address;
    self.currentElementCount = 0;
    self.currentNetkeyIndex = netkeyIndex;
    self.NetworkKey = [NSData dataWithData:networkKey];
    [self blockState];
    [self connectPeripheral:peripheral];    
}

- (void)provisionAction{
    [self.commandHandle provisionMesh:self.NetworkKey address:self.currentProvisionIndex netkeyIndex:self.currentNetkeyIndex];
    TeLog(@"do provisionAction");
}

/// Do key bound(纯keyBind接口)
- (void)keyBind:(u16)address appkey:(NSData *)appkey appkeyIndex:(u16)appkeyIndex netkeyIndex:(u16)netkeyIndex keyBindType:(KeyBindTpye)type keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess fail:(prvisionFailCallBack)fail{
    _currentProvisionIndex = address;
    self.state = StateAddDevice_keyBind;
    self.currentAddType = type;
    self.isAutoAddDevice = NO;

    self.addDevice_keyBindSuccessCallBack = keyBindSuccess;
    self.prvisionFailCallBack = fail;
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(reKeyBindTimeout) object:nil];
        [self performSelector:@selector(reKeyBindTimeout) withObject:nil afterDelay:kReKeyBindTimeout];
    });
    if (type == KeyBindTpye_Normal) {
        //normal keyBind
        doKeyBind(address,appkey,appkeyIndex,netkeyIndex,0);
    }else if (type == KeyBindTpye_Quick) {
        //fast bind
        doKeyBind(address,appkey,appkeyIndex,netkeyIndex,1);
    }
}

- (void)reKeyBindTimeout{
    if (self.prvisionFailCallBack) {
        self.prvisionFailCallBack(self.currentPeripheral.identifier.UUIDString);
    }
    [self cleanReKeyBindCallback];
}

- (void)blockState{
//    TeLog(@"");
    __weak typeof(self) weakSelf = self;
    [self setBleScanNewDeviceCallBack:^(CBPeripheral *peripheral, BOOL provisioned) {
//        TeLog(@"uuid:%@,able:%d,state:%lu",peripheral.identifier.UUIDString,provisioned,(unsigned long)weakSelf.state);
        if (weakSelf.state == StateAddDevice_provision) {
            if ([weakSelf.tempProvisionSuccessList.allKeys containsObject:peripheral.identifier.UUIDString] ||
                [weakSelf.tempProvisionFailList containsObject:peripheral.identifier.UUIDString] ||
                !provisioned) {
                return;
            }
        }else if (weakSelf.state == StateAddDevice_keyBind) {
            //无需扫描
        }else{
            if (provisioned) {
                return;
            }
        }
        
        [weakSelf stopScan];
        [weakSelf connectPeripheral:peripheral];
    }];
    
    [self setBleFinishScanedCharachteristicCallBack:^(CBPeripheral *peripheral) {
        TeLog(@"call BleFinishScanedCharachteristicCallBack");
        
        if (weakSelf.state == StateAddDevice_provision) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(scanForProvisionDeviceTimeOut) object:nil];
            });
            [weakSelf.commandHandle provisionMesh:weakSelf.NetworkKey address:weakSelf.currentProvisionIndex netkeyIndex:weakSelf.currentNetkeyIndex];
            TeLog(@"do provisioning");
        }else if (weakSelf.state == StateAddDevice_keyBind) {
            [weakSelf delayForSetFilterBeforKeyBind];
        }else{
            if (weakSelf.workWithPeripheralCallBack) {
                weakSelf.workWithPeripheralCallBack(peripheral.identifier.UUIDString);
            }
        }
    }];
    
    [self setBleDisconnectOrConnectFailCallBack:^(CBPeripheral *peripheral) {
        if (weakSelf.state == StateAddDevice_provision) {
            [weakSelf addDevice_provisionResultBack:NO];
        }else if (weakSelf.state == StateAddDevice_keyBind) {
            if (weakSelf.connectWithUUIDCount == 0) {
                [weakSelf addDevice_keyBindResultBack:NO];
            }
        }else{
            
        }
    }];
}

- (void)kickOut:(UInt16)address{
    NSString *key = nil;
    for (NSString *string in self.tempProvisionSuccessList.allKeys) {
        NSNumber *tem = self.tempProvisionSuccessList[string];
        if (tem.intValue == address) {
            key = string;
            break;
        }
    }
    TeLog(@"successlist:%@,failList:%@",self.tempProvisionSuccessList,self.tempProvisionFailList);
    if (key) {
        [self.tempProvisionSuccessList removeObjectForKey:key];
    }
    TeLog(@"successlist:%@,failList:%@",self.tempProvisionSuccessList,self.tempProvisionFailList);
    cfg_cmd_reset_node(address);
}

- (void)setProvisionState{
    TeLog(@"");
    self.state = StateAddDevice_provision;
}

- (void)setKeyBindState{
    TeLog(@"");
    self.connectWithUUIDCount = 3;
    self.state = StateAddDevice_keyBind;
}

- (void)setOTAState{
    TeLog(@"");
    self.state = StateOTA;
}

- (void)setNormalState{
    TeLog(@"");
    self.state = StateNormal;
}

- (void)keyBindAfterProvisionSuccess{
    [self addDevice_provisionResultBack:YES];
    CBPeripheral *peripheral = [self currentPeripheral];
    
    //Attention: first, set connectWithUUIDCount to 3; second, set state to StateAddDevice_keyBind; third, disconnect peripheral.
    [self setKeyBindState];
    [self cancelConnection:peripheral complete:nil];
}

///delay 500ms -> set_filter_init -> delay 500ms -> keyBind
- (void)delayForSetFilterBeforKeyBind{
    TeLog(@"");
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(delayKeyBindAfterProvisionSuccess) object:nil];
        [self performSelector:@selector(delayKeyBindAfterProvisionSuccess) withObject:nil afterDelay:kMeshProxyInitDelayTime];
    });
}

- (void)delayKeyBindAfterProvisionSuccess{
    TeLog(@"");
    if ([self isConnected]) {
        //old code before v2.8.1
//        mesh_proxy_set_filter_init(SigDataSource.share.curProvisionerModel.allocatedUnicastRange.firstObject.lowIntAddress);
//        mesh_tx_sec_nw_beacon_all_net(1);//send beacon, blt_sts can only be 0 or 1.
//
//        dispatch_async(dispatch_get_main_queue(), ^{
//            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(delayAfterSetFilter) object:nil];
//            [self performSelector:@selector(delayAfterSetFilter) withObject:nil afterDelay:kLoopWriteForBeaconDelayTime];
//        });
        
        //change since v2.8.2
        __block int responseCount = 0;
        __weak typeof(self) weakSelf = self;
        [self setFilterWithLocationAddress:SigDataSource.share.curProvisionerModel.allocatedUnicastRange.firstObject.lowIntAddress complete:^{
            responseCount ++;
            if (responseCount == kSetFilterPacketCount) {
                set_pair_login_ok(1);
                //change since v3.1.0
                if (SigDataSource.share.hasNodeExistTimeModelID) {
                    [weakSelf.commandHandle statusNowTime];
                }
                [weakSelf cancelSetFilterWithLocationAddressTimeout];
                [weakSelf delayAfterSetFilter];
                weakSelf.setFilterResponseCallBack = nil;
            }
        }fail:^{
            TeLog(@"setFilter fail.");
        }];
        mesh_tx_sec_nw_beacon_all_net(1);//send beacon, blt_sts can only be 0 or 1.
    }
}

- (void)delayAfterSetFilter{
    if ([self isConnected]) {
        TeLog(@"doKeyBind");
        [self keybindAction];
    }
}

- (void)keybindAction{
    if (self.currentAddType == KeyBindTpye_Normal) {
        doKeyBind(self.currentProvisionIndex, SigDataSource.share.curAppKey,SigDataSource.share.curAppkeyModel.index,SigDataSource.share.curNetkeyModel.index,0);
    }else if (self.currentAddType == KeyBindTpye_Quick) {
        SigScanRspModel *scanModel = [SigDataSource.share getScanRspModelWithUUID:self.currentPeripheral.identifier.UUIDString];
        DeviceTypeModel *deviceType = [SigDataSource.share getNodeInfoWithCID:scanModel.CID PID:scanModel.PID];
        if (scanModel && deviceType) {
            doKeyBind(self.currentProvisionIndex, SigDataSource.share.curAppKey, SigDataSource.share.curAppkeyModel.index, SigDataSource.share.curNetkeyModel.index,1);
        }else{
            TeLog(@"waring: this node isn't support fast_bind model, auto do key bound in normal model.");
            doKeyBind(self.currentProvisionIndex, SigDataSource.share.curAppKey,SigDataSource.share.curAppkeyModel.index,SigDataSource.share.curNetkeyModel.index,0);
        }
    }
}

/// Get Online device, private use OnlineStatusCharacteristic(获取当前mesh网络的所有设备的在线、开关、亮度、色温状态（私有定制，需要特定的OnlineStatusCharacteristic）)
- (void)getOnlineStatueFromUUID{
    uint8_t buffer[1]={1};
    NSData *data = [NSData dataWithBytes:buffer length:1];
    [self writeForOnlineStatus:data];
}

- (void)anasislyOnlineStatueDataFromUUID:(NSData *)data{
    u8 *byte = (u8 *)data.bytes;
    online_st_gatt_dec(byte, data.length);
    
    UInt8 opcodeInt=0,statusDataLength=6,statusCount=0;
    memcpy(&opcodeInt, byte, 1);
    memcpy(&statusDataLength, byte + 1, 1);
    statusCount = (UInt8)(data.length-4-2)/statusDataLength;//减去OPCode+length+snumber+CRC

    for (int i=0; i<statusCount; i++) {
        UInt16 address = 0;
        memcpy(&address, byte + 4 + statusDataLength*i, 2);
        if (address == 0) {
            continue;
        }
        SigNodeModel *device = [SigDataSource.share getNodeWithAddress:address];
        if (device) {
            UInt8 stateInt=0,bright100=0,temperature100=0;
            if (statusDataLength > 2) {
                memcpy(&stateInt, byte + 4 + statusDataLength*i + 2, 1);
            }
            if (statusDataLength > 3) {
                memcpy(&bright100, byte + 4 + statusDataLength*i + 3, 1);
            }
            if (statusDataLength > 4) {
                memcpy(&temperature100, byte + 4 + statusDataLength*i + 4, 1);
            }

            DeviceState state = stateInt == 0 ? DeviceStateOutOfLine : (bright100 == 0 ? DeviceStateOff : DeviceStateOn);
            [device updateOnlineStatusWithDeviceState:state  bright100:bright100 temperature100:temperature100];
            if (self.commandHandle.getOnlineStatusCallBack) {
                ResponseModel *model = [[ResponseModel alloc] init];
                model.rspData = [data subdataWithRange:NSMakeRange(4 + statusDataLength*i, statusDataLength)];
                model.address = address;
                model.opcode = opcodeInt;
                self.commandHandle.getOnlineStatusCallBack(model);
            }
            if (self.commandHandle.switchOnOffCallBack) {
                self.commandHandle.switchOnOffCallBack(nil);
            }
            if (self.commandHandle.changeBrightnessCallBack) {
                self.commandHandle.changeBrightnessCallBack(nil);
            }
            if (self.commandHandle.changeTemperatureCallBack) {
                self.commandHandle.changeTemperatureCallBack(nil);
            }
            if (self.commandHandle.changeHSLCallBack) {
                self.commandHandle.changeHSLCallBack(nil);
            }
            if (self.commandHandle.changeLevelCallBack) {
                self.commandHandle.changeLevelCallBack(nil);
                self.commandHandle.changeLevelCallBack = nil;
            }
        }
    }
}

///set filter
- (void)setFilterWithLocationAddress:(UInt16)locationAddress complete:(bleSetFilterResponseCallBack)complete fail:(bleSetFilterFailCallBack)fail {
    TeLog(@"");
    self.setFilterResponseCallBack = complete;
    self.setFilterFailCallBack = fail;
    mesh_proxy_set_filter_init((u16)locationAddress);
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(setFilterWithLocationAddressTimeout) object:nil];
        [self performSelector:@selector(setFilterWithLocationAddressTimeout) withObject:nil afterDelay:kSetFilterTimeout];
    });
}

- (void)setFilterWithLocationAddressTimeout {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(setFilterWithLocationAddressTimeout) object:nil];
    });
    //normal模式和keybind流程，keybind流程setFilter的重试包含在3次重试keyBind流程里面，normal模式则会断开并重试。
    if (self.state == StateNormal || (self.state == StateAddDevice_keyBind && self.connectWithUUIDCount > 0)) {
        if (self.state == StateAddDevice_keyBind) {
            TeLog(@"reconnect on keyBind, because setFilter fail.");
        }
        self.connectWithUUIDCount --;
        __weak typeof(self) weakSelf = self;
        [self cancelAllConnecttionWithComplete:^{
            [weakSelf startAutoConnect];
        }];
    }

    if (self.setFilterFailCallBack) {
        self.setFilterFailCallBack();
        self.setFilterFailCallBack = nil;
    }
}

- (void)cancelSetFilterWithLocationAddressTimeout {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(setFilterWithLocationAddressTimeout) object:nil];
    });
    self.setFilterFailCallBack = nil;
}

- (void)commondAfterNoBusy:(bleCompleteCallBack)complete {
    self.noBusyCallBack = complete;
    if (!self.commandHandle.busyTimer) {
        if (self.noBusyCallBack) {
            self.noBusyCallBack();
            self.noBusyCallBack = nil;
        }
    }
}

@end
