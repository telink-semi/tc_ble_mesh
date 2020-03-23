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

@interface OTAManager()<SigBearerDataDelegate>

@property (nonatomic,strong) SigBearer *bearer;

@property (nonatomic,assign) NSTimeInterval connectPeripheralWithUUIDTimeoutInterval;//timeout of connect peripheral
@property (nonatomic,assign) NSTimeInterval writeOTAInterval;//interval of write ota data, default is 6ms
@property (nonatomic,assign) NSTimeInterval readTimeoutInterval;//timeout of read OTACharacteristic(write 8 packet, read one time), default is 5s.

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
    _bearer = SigBearer.share;
    
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
        TeLogInfo(@"OTAing, can't call repeated.");
        return NO;
    }
    if (!otaData || otaData.length == 0) {
        TeLogInfo(@"OTA data is invalid.");
        return NO;
    }
    if (models.count == 0) {
        TeLogInfo(@"OTA devices list is invaid.");
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
    
    #warning 2019年12月17日15:44:23，需要设置dataDelegate到OTAmanager。
    _bearer.dataDelegate = self;
    
    [self refreshCurrentModel];
    [self otaNext];
    
    return YES;
}

/// stop OTA
- (void)stopOTA{
    [SigBluetooth.share stopScan];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self];
    });
    _singleSuccessCallBack = nil;
    _singleFailCallBack = nil;
    _singleProgressCallBack = nil;
    _finishCallBack = nil;
    _stopOTAFlag = YES;
    _OTAing = NO;
    [_bearer startMeshConnectWithTimeOut:kStartMeshConnectTimeout complete:nil];
    #warning 2019年12月17日15:44:23，需要设置dataDelegate到OTA前。
    _bearer.dataDelegate = nil;
}

- (void)connectDevice{
    if (!_currentUUID && _currentUUID.length == 0) {
        TeLogInfo(@"还未扫描到设备");
        [self startScanForOTA];
    } else {
        if ([_bearer.getCurrentPeripheral.identifier.UUIDString isEqualToString:_currentUUID] && _bearer.isOpen) {
            [self dalayToSetFilter];
        }else{
            __weak typeof(self) weakSelf = self;
            [_bearer closeWithResult:^(BOOL successful) {
                [weakSelf startConnectForOTA];
            }];
        }
    }
}

- (void)dalayToSetFilter{
    TeLogDebug(@"");
    __weak typeof(self) weakSelf = self;
    if (_bearer.isOpen) {
        SigProvisionerModel *provisioner = SigDataSource.share.curProvisionerModel;
        [SDKLibCommand setFilterForProvisioner:provisioner successCallback:^(UInt16 source, UInt16 destination, SigFilterStatus * _Nonnull responseMessage) {
            [weakSelf sendOTAAfterSetFilter];
        } failCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
            TeLogError(@"setFilter fail!!!");
            //失败后逻辑：断开连接，扫描，连接(当前直接失败)
    #warning 2019年12月17日16:21:13
            [weakSelf connectPeripheralWithUUIDTimeout];
        }];
    #warning 2019年12月17日16:24:42，待确认是否需要mesh_tx_sec_nw_beacon_all_net(1);//send beacon, blt_sts can only be 0 or 1.

    }
}

- (void)sendOTAAfterSetFilter{
    [self sendPartData];
}

- (void)startConnectForOTA{
    __weak typeof(self) weakSelf = self;
    if (_currentUUID != nil && _currentUUID.length > 0) {
        CBPeripheral *p = [SigBluetooth.share getPeripheralWithUUID:_currentUUID];
        if (p) {
            [_bearer changePeripheral:p result:^(BOOL successful) {
                if (successful) {
                    TeLogDebug(@"切换设备成功");
                    [weakSelf.bearer openWithResult:^(BOOL successful) {
                        if (successful) {
                            TeLogDebug(@"读服务列表成功");

                        } else {
                            //失败后逻辑：断开连接，扫描，连接(当前直接失败)
                            #warning 2019年12月17日16:21:13
                            [weakSelf connectPeripheralWithUUIDTimeout];
                        }
                    }];
                } else {
                    //失败后逻辑：断开连接，扫描，连接(当前直接失败)
                    #warning 2019年12月17日16:21:13
                    [weakSelf connectPeripheralWithUUIDTimeout];
                }
            }];
        } else {
            TeLogInfo(@"get CBPeripheral is nil.");
        }
    }else{
        TeLogInfo(@"error");
    }
}

- (void)startScanForOTA{
    __weak typeof(self) weakSelf = self;
    [SigBluetooth.share cancelAllConnecttionWithComplete:^{
        [SigBluetooth.share scanProvisionedDevicesWithResult:^(CBPeripheral * _Nonnull peripheral, NSDictionary<NSString *,id> * _Nonnull advertisementData, NSNumber * _Nonnull RSSI, BOOL unprovisioned) {
            if (!unprovisioned && [SigDataSource.share getScanRspModelWithAddress:self.currentModel.address]) {
                //扫描到当前需要OTA的设备
                [SigBluetooth.share stopScan];
                //更新uuid
                [weakSelf refreshCurrentModel];
                [weakSelf startConnectForOTA];
            }
        }];
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
    
    if (self.currentModel && _bearer.getCurrentPeripheral && _bearer.getCurrentPeripheral.state == CBPeripheralStateConnected) {
        NSInteger lastLength = _localData.length - _offset;
        
        //OTA 结束包特殊处理
        if (lastLength == 0) {
            Byte byte[] = {0x02,0xff};
            NSData *endData = [NSData dataWithBytes:byte length:2];
            [self sendOTAEndData:endData index:(int)self.otaIndex];
            self.sendFinish = YES;
            return;
        }
        
        self.otaIndex ++;
        //OTA开始包特殊处理
        if (self.otaIndex == 0) {
            [self sendReadFirmwareVersion];
            [self sendStartOTA];
        }
        
        NSInteger writeLength = (lastLength >= 16) ? 16 : lastLength;
        NSData *writeData = [self.localData subdataWithRange:NSMakeRange(self.offset, writeLength)];
        [self sendOTAData:writeData index:(int)self.otaIndex];
        self.offset += writeLength;
        
        float progress = (self.offset * 100.0) / self.localData.length;
        if (self.singleProgressCallBack) {
            self.singleProgressCallBack(progress);
        }
        
        if ((self.otaIndex + 1) % 8 == 0) {
            __weak typeof(self) weakSelf = self;
            [SDKLibCommand readOTACharachteristicWithTimeout:self.readTimeoutInterval complete:^(CBCharacteristic * _Nonnull characteristic, BOOL successful) {
                if (successful) {
                    [weakSelf sendPartData];
                } else {
                    [weakSelf readTimeout];
                }
            }];
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
    if (_bearer.getCurrentPeripheral) {
        __weak typeof(self) weakSelf = self;
        [SigBluetooth.share cancelAllConnecttionWithComplete:^{
            [weakSelf otaFailAction];
        }];
    }
}

- (void)otaSuccessAction{
    self.OTAing = NO;
    self.sendFinish = NO;
    self.stopOTAFlag = YES;
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
    [SigBearer.share startMeshConnectWithTimeOut:kStartMeshConnectTimeout complete:nil];
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
        #warning 2019年12月17日15:44:23，OTA下一个设备的流程。
//        [self.ble setOTAState];
        [self connectDevice];
    }
}

- (void)connectWorkNormal{
    [_bearer startMeshConnectWithTimeOut:kStartMeshConnectTimeout complete:nil];
}

#pragma mark - SigBearerDataDelegate

- (void)bearer:(SigBearer *)bearer didCloseWithError:(NSError *)error {
    if ([_bearer.getCurrentPeripheral.identifier.UUIDString isEqualToString:self.currentUUID]) {
        if (self.sendFinish) {
            [self otaSuccessAction];
        } else {
            [self otaFailAction];
        }
    }
}

#pragma mark - OTA packet

- (void)sendOTAData:(NSData *)data index:(int)index {
    BOOL isEnd = data.length == 0;
    int countIndex = index;
    Byte *tempBytes = (Byte *)[data bytes];
    Byte resultBytes[20];
    
    memset(resultBytes, 0xff, 20);
    memcpy(resultBytes, &countIndex, 2);
    memcpy(resultBytes+2, tempBytes, data.length);
    uint16_t crc = crc16(resultBytes, isEnd ? 2 : 18);
    memcpy(isEnd ? (resultBytes + 2) : (resultBytes+18), &crc, 2);
    NSData *writeData = [NSData dataWithBytes:resultBytes length:isEnd ? 4 : 20];
    [_bearer sendOTAData:writeData];
}

/*
 packet of end OTA 6 bytes structure：1byte:0x02 + 1byte:0xff + 2bytes:index + 2bytes:~index
 */
- (void)sendOTAEndData:(NSData *)data index:(int)index {
    int negationIndex = ~index;
    Byte *tempBytes = (Byte *)[data bytes];
    Byte resultBytes[6];
    
    memset(resultBytes, 0xff, 6);
    memcpy(resultBytes, tempBytes, data.length);
    memcpy(resultBytes+2, &index, 2);
    memcpy(resultBytes+4, &negationIndex, 2);
    NSData *writeData = [NSData dataWithBytes:resultBytes length:6];
    TeLogInfo(@"sendOTAEndData -> %04x ,length:%lu,%@", index,(unsigned long)writeData.length,writeData);
    [_bearer sendOTAData:writeData];
}

- (void)sendReadFirmwareVersion {
    uint8_t buf[2] = {0x00,0xff};
    NSData *writeData = [NSData dataWithBytes:buf length:2];
    TeLogInfo(@"sendReadFirmwareVersion -> length:%lu,%@",(unsigned long)writeData.length,writeData);
    [_bearer sendOTAData:writeData];
}

- (void)sendStartOTA {
    uint8_t buf[2] = {0x01,0xff};
    NSData *writeData = [NSData dataWithBytes:buf length:2];
    TeLogInfo(@"sendReadStartOTA -> length:%lu,%@",(unsigned long)writeData.length,writeData);
    [_bearer sendOTAData:writeData];
}



@end
