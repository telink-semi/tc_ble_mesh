/********************************************************************************************************
 * @file     SigAddDeviceManager.m
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
//  SigAddDeviceManager.m
//  SigMeshLib
//
//  Created by Liangjiazhi on 2019/9/4.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigAddDeviceManager.h"

typedef enum : NSUInteger {
    SigAddStatusScanning,
    SigAddStatusConnectFirst,
    SigAddStatusProvisioning,
    SigAddStatusConnectSecond,
    SigAddStatusKeyBinding,
} SigAddStatus;
/*
 needDisconnectBetweenProvisionToKeyBind：标记是否需要SigAddStatusConnectSecond
 retryCount：用于SigAddStatusConnectFirst和SigAddStatusConnectSecond的总共重连次数
 function1与function2的区别：function1存在扫描流程SigAddStatusScanning，function2跳过了扫描流程直接进入连接流程SigAddStatusConnectFirst。
 */

@interface SigAddDeviceManager ()
@property (nonatomic,assign) UInt16 unicastAddress;
@property (nonatomic,strong) NSData *networkKey;
@property (nonatomic,assign) UInt16 netkeyIndex;
@property (nonatomic,strong) SigAppkeyModel *appkeyModel;
@property (nonatomic,assign) KeyBindTpye keyBindType;
@property (nonatomic,copy) addDevice_prvisionSuccessCallBack provisionSuccessBlock;
@property (nonatomic,copy) addDevice_keyBindSuccessCallBack keyBindSuccessBlock;
@property (nonatomic,copy) ErrorBlock provisionFailBlock;
@property (nonatomic,copy) ErrorBlock keyBindFailBlock;
@property (nonatomic,copy) prvisionFinishCallBack finishBlock;
@property (nonatomic,assign) SigAddStatus addStatus;
// it is need to call start scan after add one node successful. default is NO.
@property (nonatomic,assign) BOOL isAutoAddDevice;
//contains provsion fail list
@property (nonatomic,strong) NSMutableArray <NSString *>*tempProvisionFailList;
@property (nonatomic,strong) CBPeripheral *curPeripheral;

@property (nonatomic,assign) UInt16 fastKeybindProductID;
@property (nonatomic,strong) NSData *fastKeybindCpsData;
@property (nonatomic,assign) ProvisionTpye provisionType;
@property (nonatomic,strong) NSData *staticOOBData;

@end

@implementation SigAddDeviceManager

+ (SigAddDeviceManager *)share {
    static SigAddDeviceManager *shareManager = nil;
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        shareManager = [[SigAddDeviceManager alloc] init];
        shareManager.needDisconnectBetweenProvisionToKeyBind = YES;
        shareManager.retryCount = 3;
        shareManager.addStatus = SigAddStatusScanning;
        shareManager.isAutoAddDevice = NO;
    });
    return shareManager;
}

/// function1 :add bluetooth devices (auto add)
- (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel unicastAddress:(UInt16)unicastAddress uuid:(nullable NSData *)uuid keyBindType:(KeyBindTpye)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData isAutoAddNextDevice:(BOOL)isAuto provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail finish:(prvisionFinishCallBack)finish {
    self.unicastAddress = address;
    self.networkKey = networkKey;
    self.netkeyIndex = netkeyIndex;
    self.appkeyModel = appkeyModel;
    self.keyBindType = type;
    self.provisionSuccessBlock = provisionSuccess;
    self.provisionFailBlock = provisionFail;
    self.keyBindSuccessBlock = keyBindSuccess;
    self.keyBindFailBlock = keyBindFail;
    self.finishBlock = finish;
    self.isAutoAddDevice = isAuto;
    self.retryCount = 3;
    self.addStatus = SigAddStatusScanning;
    self.fastKeybindCpsData = cpsData;
    self.fastKeybindProductID = productID;
    self.provisionType = ProvisionTpye_NoOOB;
    self.staticOOBData = nil;
    [self startScan];
}

/// function2 :add bluetooth device (single add)
- (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel peripheral:(CBPeripheral *)peripheral provisionType:(ProvisionTpye)provisionType staticOOBData:(nullable NSData *)staticOOBData keyBindType:(KeyBindTpye)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail {
    self.unicastAddress = address;
    self.networkKey = networkKey;
    self.netkeyIndex = netkeyIndex;
    self.appkeyModel = appkeyModel;
    self.keyBindType = type;
    self.provisionSuccessBlock = provisionSuccess;
    self.provisionFailBlock = provisionFail;
    self.keyBindSuccessBlock = keyBindSuccess;
    self.keyBindFailBlock = keyBindFail;
    self.isAutoAddDevice = NO;
    self.retryCount = 3;
    self.addStatus = SigAddStatusConnectFirst;
    self.fastKeybindCpsData = cpsData;
    self.fastKeybindProductID = productID;
    self.provisionType = provisionType;
    self.staticOOBData = staticOOBData;
    if (provisionType == ProvisionTpye_NoOOB) {
        self.staticOOBData = nil;
    }
    [self startAddPeripheral:peripheral];
}

/// Add Single Device (provision+keyBind)
/// @param configModel all config message of add device.
/// @param provisionSuccess callback when provision success.
/// @param provisionFail callback when provision fail.
/// @param keyBindSuccess callback when keybind success.
/// @param keyBindFail callback when keybind fail.
- (void)startAddDeviceWithSigAddConfigModel:(SigAddConfigModel *)configModel provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail {
    SigAppkeyModel *appkeyModel = [SigDataSource.share getAppkeyModelWithAppkeyIndex:configModel.appkeyIndex];
    if (!appkeyModel || ![appkeyModel.getDataKey isEqualToData:configModel.appKey]) {
        TeLogVerbose(@"appKey is error.");
        if (provisionFail) {
            NSError *err = [NSError errorWithDomain:@"appKey is error." code:-1 userInfo:nil];
            provisionFail(err);
        }
        return;
    }
    [self startAddDeviceWithNextAddress:configModel.unicastAddress networkKey:configModel.networkKey netkeyIndex:configModel.netkeyIndex appkeyModel:appkeyModel peripheral:configModel.peripheral provisionType:configModel.provisionType staticOOBData:configModel.staticOOBData keyBindType:configModel.keyBindType productID:configModel.productID cpsData:configModel.cpsData provisionSuccess:provisionSuccess provisionFail:provisionFail keyBindSuccess:keyBindSuccess keyBindFail:keyBindFail];
}

- (void)startScan {
    self.addStatus = SigAddStatusScanning;
    __weak typeof(self) weakSelf = self;
    [SigBluetooth.share scanUnprovisionedDevicesWithResult:^(CBPeripheral * _Nonnull peripheral, NSDictionary<NSString *,id> * _Nonnull advertisementData, NSNumber * _Nonnull RSSI, BOOL unprovisioned) {
        if (unprovisioned) {
            weakSelf.addStatus = SigAddStatusConnectFirst;
            [SigBluetooth.share stopScan];
            [weakSelf startAddPeripheral:peripheral];
        }
    }];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(scanTimeout) object:nil];
        [self performSelector:@selector(scanTimeout) withObject:nil afterDelay:kScanUnprovisionDeviceTimeout];
    });
}

- (void)keybind{
    __weak typeof(self) weakSelf = self;
    CBPeripheral *peripheral = SigBearer.share.getCurrentPeripheral;
    [SDKLibCommand setFilterForProvisioner:SigDataSource.share.curProvisionerModel successCallback:^(UInt16 source, UInt16 destination, SigFilterStatus * _Nonnull responseMessage) {
        TeLogInfo(@"start keyBind.");
        weakSelf.addStatus = SigAddStatusKeyBinding;
        [SigKeyBindManager.share keyBind:weakSelf.unicastAddress appkeyModel:weakSelf.appkeyModel keyBindType:weakSelf.keyBindType productID:weakSelf.fastKeybindProductID cpsData:weakSelf.fastKeybindCpsData keyBindSuccess:^(NSString * _Nonnull identify, UInt16 address) {
            if (weakSelf.keyBindSuccessBlock) {
                weakSelf.keyBindSuccessBlock(identify, address);
            }
            [weakSelf refreshNextUnicastAddress];
            [weakSelf addPeripheralSuccess:weakSelf.curPeripheral];
        } fail:^(NSError * _Nonnull error) {
            TeLogDebug(@"model bind fail, so keybind fail.");
            if (weakSelf.keyBindFailBlock) {
                NSError *err = [NSError errorWithDomain:@"model bind fail, so keybind fail." code:0 userInfo:nil];
                weakSelf.keyBindFailBlock(err);
            }
            [weakSelf refreshNextUnicastAddress];
            [weakSelf addPeripheralFail:peripheral];
        }];
    } failCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        if (weakSelf.retryCount > 0) {
            TeLogDebug(@"retry setFilter peripheral=%@,retry count=%d",peripheral,weakSelf.retryCount);
            weakSelf.retryCount --;
            [weakSelf keybind];
        } else {
            TeLogDebug(@"setFilter fail, so keybind fail.");
            if (weakSelf.keyBindFailBlock) {
                NSError *err = [NSError errorWithDomain:@"setFilter fail, so keybind fail." code:0 userInfo:nil];
                weakSelf.keyBindFailBlock(err);
            }
            [weakSelf refreshNextUnicastAddress];
            [weakSelf addPeripheralFail:peripheral];
        }
    }];
}

- (void)startAddPeripheral:(CBPeripheral *)peripheral {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(scanTimeout) object:nil];
    });
    self.curPeripheral = peripheral;
    __weak typeof(self) weakSelf = self;
    [[SigBearer share] connectAndReadServicesWithPeripheral:peripheral result:^(BOOL successful) {
        if (successful) {
            if (weakSelf.addStatus == SigAddStatusConnectFirst) {
                //it need delay 500ms before sent provision invite.
                dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.5 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                    weakSelf.addStatus = SigAddStatusProvisioning;
                    if (self.staticOOBData) {
                        [SigProvisioningManager.share provisionWithUnicastAddress:weakSelf.unicastAddress networkKey:weakSelf.networkKey netkeyIndex:weakSelf.netkeyIndex staticOobData:weakSelf.staticOOBData provisionSuccess:^(NSString * _Nonnull identify, UInt16 address) {
                            if (weakSelf.provisionSuccessBlock) {
                                weakSelf.provisionSuccessBlock(identify, address);
                            }
                            if (weakSelf.needDisconnectBetweenProvisionToKeyBind || [SigBluetooth.share getCharacteristicWithUUIDString:kPROXY_In_CharacteristicsID OfPeripheral:SigBearer.share.getCurrentPeripheral] == nil) {
                                weakSelf.addStatus = SigAddStatusConnectSecond;
                                [weakSelf startAddPeripheral:peripheral];
                            } else {
                                [weakSelf keybind];
                            }
                        } fail:^(NSError * _Nonnull error) {
                            if (weakSelf.provisionFailBlock) {
                                weakSelf.provisionFailBlock(error);
                            }
                            [weakSelf addPeripheralFail:peripheral];
                        }];
                    } else {
                        [SigProvisioningManager.share provisionWithUnicastAddress:weakSelf.unicastAddress networkKey:weakSelf.networkKey netkeyIndex:weakSelf.netkeyIndex provisionSuccess:^(NSString * _Nonnull identify, UInt16 address) {
                            if (weakSelf.provisionSuccessBlock) {
                                weakSelf.provisionSuccessBlock(identify, address);
                            }
                            if (weakSelf.needDisconnectBetweenProvisionToKeyBind || [SigBluetooth.share getCharacteristicWithUUIDString:kPROXY_In_CharacteristicsID OfPeripheral:SigBearer.share.getCurrentPeripheral] == nil) {
                                weakSelf.addStatus = SigAddStatusConnectSecond;
                                [weakSelf startAddPeripheral:peripheral];
                            } else {
                                [weakSelf keybind];
                            }
                        } fail:^(NSError * _Nonnull error) {
                            if (weakSelf.provisionFailBlock) {
                                weakSelf.provisionFailBlock(error);
                            }
                            [weakSelf addPeripheralFail:peripheral];
                        }];
                    }
                });
            } else if (weakSelf.addStatus == SigAddStatusConnectSecond) {
                dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.5 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                    [weakSelf keybind];
                });
            }else{
                TeLogError(@"error addStatus!!!!!!!!!");
            }
        } else {
            if (weakSelf.retryCount > 0) {
                TeLogDebug(@"retry connect peripheral=%@,retry count=%d",peripheral,weakSelf.retryCount);
                weakSelf.retryCount --;
                [weakSelf startAddPeripheral:peripheral];
            } else {
                if (weakSelf.addStatus == SigAddStatusConnectFirst) {
                    if (weakSelf.provisionFailBlock) {
                        NSError *err = [NSError errorWithDomain:@"connect or read ATT fail. provision fail." code:-1 userInfo:nil];
                        weakSelf.provisionFailBlock(err);
                    }
                } else if (weakSelf.addStatus == SigAddStatusConnectSecond) {
                    if (weakSelf.keyBindFailBlock) {
                        NSError *err = [NSError errorWithDomain:@"connect or read ATT fail. keybind fail." code:-1 userInfo:nil];
                        weakSelf.keyBindFailBlock(err);
                    }
                }
                [weakSelf addPeripheralFail:peripheral];
            }
        }
    }];
}

- (void)addPeripheralFail:(CBPeripheral *)peripheral {
    NSString *uuid = peripheral.identifier.UUIDString;
    if (![self.tempProvisionFailList containsObject:uuid]) {
        [self.tempProvisionFailList addObject:uuid];
    }
    if (self.isAutoAddDevice) {
        self.addStatus = SigAddStatusScanning;
        [self startScan];
    }else{
        if (self.finishBlock) {
            self.finishBlock();
        }
    }
}

- (void)addPeripheralSuccess:(CBPeripheral *)peripheral {
    if (self.isAutoAddDevice) {
        self.addStatus = SigAddStatusScanning;
        [self startScan];
    }else{
        if (self.finishBlock) {
            self.finishBlock();
        }
    }
}

- (void)scanTimeout {
    self.addStatus = SigAddStatusScanning;
    [SigBluetooth.share stopScan];
    if (self.finishBlock) {
        self.finishBlock();
    }
}

- (void)refreshNextUnicastAddress {
    self.unicastAddress += SigProvisioningManager.share.provisioningCapabilities.numberOfElements;
}

@end
