/********************************************************************************************************
 * @file     OTAManager.m 
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
//  OTAManager.m
//  SigMeshOC
//
//  Created by Liangjiazhi on 2018/7/18.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "OTAManager.h"
#import "LibHandle.h"

@interface OTAManager()

@property (nonatomic,strong) Bluetooth *ble;

@property (nonatomic,assign) NSTimeInterval connectPeripheralWithUUIDTimeoutInterval;//timeout of connect peripheral
@property (nonatomic,assign) NSTimeInterval writeOTAInterval;//interval of write ota data, default is 6ms
@property (nonatomic,assign) NSTimeInterval readTimeoutInterval;//timeout of read OTACharacteristic(write 8 packet, read one time)

@property (nonatomic,strong) SigNodeModel *currentModel;
@property (nonatomic,strong) NSString *currentUUID;
@property (nonatomic,strong) NSMutableArray <SigNodeModel *>*allModels;
@property (nonatomic,assign) NSInteger currentIndex;

@property (nonatomic,copy) singleDeviceCallBack singleSuccessCallBack;
@property (nonatomic,copy) singleDeviceCallBack singleFailCallBack;
@property (nonatomic,copy) singleProgressCallBack singleProgressCallBack;
@property (nonatomic,copy) finishCallBack finishCallBack;
@property (nonatomic,strong) NSMutableArray <SigNodeModel *>*successModels;
@property (nonatomic,strong) NSMutableArray <SigNodeModel *>*failModels;

@property (nonatomic,assign) BOOL OTAing;
@property (nonatomic,assign) BOOL stopOTAFlag;
@property (nonatomic,assign) NSInteger offset;
@property (nonatomic,assign) NSInteger otaIndex;//index of current ota packet
@property (nonatomic,strong) NSData *localData;
@property (nonatomic,assign) BOOL sendFinish;



@end

@implementation OTAManager

+ (OTAManager *)share{
    static OTAManager *shareOTA = nil;
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        shareOTA = [[OTAManager alloc] init];
        [shareOTA initData];
    });
    return shareOTA;
}

- (void)initData{
    _ble = Bluetooth.share;
    
    _connectPeripheralWithUUIDTimeoutInterval = 10.0;
    _writeOTAInterval = 0.006;
    _readTimeoutInterval = 5.0;
    
    _currentUUID = @"";
    _currentIndex = 0;
    
    _OTAing = NO;
    _stopOTAFlag = NO;
    _offset = 0;
    _otaIndex = -1;
    _sendFinish = NO;
    
    _allModels = [[NSMutableArray alloc] init];
    _successModels = [[NSMutableArray alloc] init];
    _failModels = [[NSMutableArray alloc] init];
}


/**
 OTA，can not call repeat when app is OTAing

 @param otaData data for OTA
 @param models models for OTA
 @param singleSuccessAction callback when single model OTA  success
 @param singleFailAction callback when single model OTA  fail
 @param singleProgressAction callback with single model OTA progress
 @param finishAction callback when all models OTA finish
 @return use API success is ture;user API fail is false.
 */
- (BOOL)startOTAWithOtaData:(NSData *)otaData models:(NSArray <SigNodeModel *>*)models singleSuccessAction:(singleDeviceCallBack)singleSuccessAction singleFailAction:(singleDeviceCallBack)singleFailAction singleProgressAction:(singleProgressCallBack)singleProgressAction finishAction:(finishCallBack)finishAction{
    if (_OTAing) {
        TeLog(@"OTAing, can't call repeated.");
        return NO;
    }
    if (!otaData || otaData.length == 0) {
        TeLog(@"OTA data is invalid.");
        return NO;
    }
    if (models.count == 0) {
        TeLog(@"OTA devices list is invaid.");
        return NO;
    }
    
    _localData = otaData;
    [_allModels removeAllObjects];
    [_allModels addObjectsFromArray:models];
    _currentIndex = 0;
    _singleSuccessCallBack = singleSuccessAction;
    _singleFailCallBack = singleFailAction;
    _singleProgressCallBack = singleProgressAction;
    _finishCallBack = finishAction;
    [_successModels removeAllObjects];
    [_failModels removeAllObjects];
    [self blockState];
    [self refreshCurrentModel];
    [self otaNext];
    
    return YES;
}

/// stop OTA
- (void)stopOTA{
    [self.ble stopScan];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self];
    });
    _singleSuccessCallBack = nil;
    _singleFailCallBack = nil;
    _singleProgressCallBack = nil;
    _finishCallBack = nil;
    _stopOTAFlag = YES;
    _OTAing = NO;
    [Bluetooth.share setNormalState];
}

- (void)connectDevice{
    if (!_currentUUID && _currentUUID.length == 0) {
        TeLog(@"还未扫描到设备");
        [self startScanForOTA];
    } else {
        if ([_ble.currentPeripheral.identifier.UUIDString isEqualToString:_currentUUID] && _ble.isConnected) {
            [self dalayToSetFilter];
        }else{
            __weak typeof(self) weakSelf = self;
            [_ble cancelConnection:_ble.currentPeripheral complete:^{
                [weakSelf startConnectForOTA];
            }];
        }
    }
}

- (void)dalayToSetFilter{
    TeLog(@"");
    if (_ble.isConnected) {
        //old code before v2.8.1
//        mesh_proxy_set_filter_init(SigDataSource.share.curProvisionerModel.allocatedUnicastRange.firstObject.lowIntAddress);
//        mesh_tx_sec_nw_beacon_all_net(1);
//
//        dispatch_async(dispatch_get_main_queue(), ^{
//            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sendOTAAfterSetFilter) object:nil];
//            [self performSelector:@selector(sendOTAAfterSetFilter) withObject:nil afterDelay:0.5];
//        });
        
        //change since v2.8.2
        __block int responseCount = 0;
        __weak typeof(self) weakSelf = self;
        [Bluetooth.share setFilterWithLocationAddress:SigDataSource.share.curProvisionerModel.allocatedUnicastRange.firstObject.lowIntAddress complete:^{
            responseCount ++;
            if (responseCount == 3) {
                set_pair_login_ok(1);
                [weakSelf sendOTAAfterSetFilter];
                Bluetooth.share.setFilterResponseCallBack = nil;
            }
        }fail:^{
            TeLog(@"setFilter fail.");
        }];
        mesh_tx_sec_nw_beacon_all_net(1);//send beacon, blt_sts can only be 0 or 1.
    }
}

- (void)sendOTAAfterSetFilter{
    [self sendPartData];
}

- (void)startConnectForOTA{
    if (_currentUUID != nil && _currentUUID.length > 0) {
        [_ble connectPeripheralWithUUID:_currentUUID];
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(connectPeripheralWithUUIDTimeout) object:nil];
            [self performSelector:@selector(connectPeripheralWithUUIDTimeout) withObject:nil afterDelay:self.connectPeripheralWithUUIDTimeoutInterval];
        });
    }else{
        TeLog(@"error");
    }
}

- (void)startScanForOTA{
    __weak typeof(self) weakSelf = self;
    [_ble cancelAllConnecttionWithComplete:^{
        [weakSelf.ble startScan];
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(scanPeripheralTimeout) object:nil];
            [weakSelf performSelector:@selector(scanPeripheralTimeout) withObject:nil afterDelay:10.0];
        });
    }];
}

- (void)connectPeripheralWithUUIDTimeout{
    self.OTAing = NO;
    [self otaFailAction];
}

- (void)scanPeripheralTimeout{
    self.OTAing = NO;
    [self otaFailAction];
}

- (void)sendPartData{
    if (self.stopOTAFlag) {
        return;
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(readTimeout) object:nil];
    });
    
    if (self.currentModel && _ble.currentPeripheral && _ble.currentPeripheral.state == CBPeripheralStateConnected) {
        NSInteger lastLength = _localData.length - _offset;
        
        //OTA 结束包特殊处理
        if (lastLength == 0) {
            Byte byte[] = {0x02,0xff};
            NSData *endData = [NSData dataWithBytes:byte length:2];
            sendOTAEndData(endData, (int)self.otaIndex);
            self.sendFinish = YES;
            return;
        }
        
        self.otaIndex ++;
        //OTA开始包特殊处理
        if (self.otaIndex == 0) {
            sendReadFirmwareVersion();
            sendStartOTA();
        }
        
        NSInteger writeLength = (lastLength >= 16) ? 16 : lastLength;
        NSData *writeData = [self.localData subdataWithRange:NSMakeRange(self.offset, writeLength)];
        sendData(writeData, (int)self.otaIndex);
        self.offset += writeLength;
        
        float progress = (self.offset * 100.0) / self.localData.length;
        if (self.singleProgressCallBack) {
            self.singleProgressCallBack(progress);
        }
        
        if ((self.otaIndex + 1) % 8 == 0) {
            __weak typeof(self) weakSelf = self;
            [_ble bleReadOTACharachteristic:YES complete:^(CBCharacteristic *characteristic) {
                [weakSelf sendPartData];
            }];
            dispatch_async(dispatch_get_main_queue(), ^{
                [self performSelector:@selector(readTimeout) withObject:nil afterDelay:self.readTimeoutInterval];
            });
            return;
        }
        //注意：index=0与index=1之间的时间间隔修改为300ms，让固件有充足的时间进行ota配置。
        NSTimeInterval timeInterval = self.writeOTAInterval;
        if (self.otaIndex == 0) {
            timeInterval = 0.3;
        }
        dispatch_async(dispatch_get_main_queue(), ^{
            [self performSelector:@selector(sendPartData) withObject:nil afterDelay:timeInterval];
        });
    }
}

- (void)readTimeout{
    if (_ble.currentPeripheral) {
        __weak typeof(self) weakSelf = self;
        [_ble cancelConnection:_ble.currentPeripheral complete:^{
            [weakSelf otaFailAction];
        }];
    }
}

- (void)otaSuccessAction{
    self.OTAing = NO;
    self.sendFinish = NO;
    self.stopOTAFlag = YES;
    [_ble setNormalState];
    if (self.singleSuccessCallBack) {
        self.singleSuccessCallBack(self.currentModel);
    }
    [self.successModels addObject:self.currentModel];
    self.currentIndex ++;
    [self refreshCurrentModel];
    [self otaNext];
}

- (void)otaFailAction{
    self.OTAing = NO;
    self.sendFinish = NO;
    self.stopOTAFlag = YES;
    [_ble setNormalState];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self];
    });
    if (self.singleFailCallBack) {
        self.singleFailCallBack(self.currentModel);
    }
    [self.failModels addObject:self.currentModel];
    self.currentIndex ++;
    [self refreshCurrentModel];
    [self otaNext];
}

- (void)refreshCurrentModel{
    if (self.currentIndex < self.allModels.count) {
        self.currentModel = self.allModels[self.currentIndex];
        self.currentUUID = [SigDataSource.share getNodeWithAddress:self.currentModel.address].peripheralUUID;
    }else{
        //直连OTA设备超时，默认后台进行重连mesh操作
        [self connectWorkNormal];
    }
}

- (void)otaNext{
    if (self.currentIndex == self.allModels.count) {
        //all devices are OTA finished.
        if (self.finishCallBack) {
            self.finishCallBack(self.successModels,self.failModels);
        }
        //OTA完成后，默认后台进行重连mesh操作
        [self connectWorkNormal];
    } else {
        self.OTAing = YES;
        self.stopOTAFlag = NO;
        self.otaIndex = -1;
        self.offset = 0;
        [self.ble setOTAState];
        [self connectDevice];
    }
}

- (void)blockState{
    __weak typeof(self) weakSelf = self;
    [self.ble bleFinishScanedCharachteristic:^(CBPeripheral *peripheral) {
        if (weakSelf.ble.state == StateOTA && [peripheral.identifier.UUIDString isEqualToString:weakSelf.currentUUID]) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(connectPeripheralWithUUIDTimeout) object:nil];
                [weakSelf performSelector:@selector(dalayToSetFilter) withObject:nil afterDelay:0.5];
            });
        }
    }];
    [self.ble bleDisconnectOrConnectFail:^(CBPeripheral *peripheral) {
        if (weakSelf.ble.state == StateOTA && [peripheral.identifier.UUIDString isEqualToString:weakSelf.currentUUID]) {
            if (weakSelf.sendFinish) {
                [weakSelf otaSuccessAction];
            } else {
                [weakSelf otaFailAction];
            }
        }
    }];
    [self.ble bleScanNewDevice:^(CBPeripheral *peripheral, BOOL provisioned) {
        if (!provisioned && [SigDataSource.share getScanRspModelWithAddress:self.currentModel.address]) {
            //扫描到当前需要OTA的设备
            [weakSelf.ble stopScan];
            //更新uuid
            [weakSelf refreshCurrentModel];
            [weakSelf startConnectForOTA];
        }
    }];
}

- (void)nilBlock{
    self.ble.bleFinishScanedCharachteristicCallBack = nil;
    self.ble.bleDisconnectOrConnectFailCallBack = nil;
}

- (void)connectWorkNormal{
    [self.ble startWorkNormalWithComplete:nil];
}

@end
