/********************************************************************************************************
 * @file     RemoteAddVC.m
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
//  RemoteAddVC.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2019/3/25.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import "RemoteAddVC.h"
#import "AddDeviceItemCell.h"
#import "UIViewController+Message.h"

/*
 Developer can get more detail by read doucment named "SIG Mesh iOS APP(OC版本)使用以及开发手册.docx".
 */

@interface RemoteAddVC ()<UICollectionViewDelegate,UICollectionViewDataSource>
@property (weak, nonatomic) IBOutlet UICollectionView *collectionView;
@property (weak, nonatomic) IBOutlet UIButton *goBackButton;
@property (strong, nonatomic) NSMutableArray <AddDeviceModel *>*source;
@property (strong, nonatomic) UIBarButtonItem *refreshItem;
@property (strong, nonatomic) NSMutableArray <SigRemoteScanRspModel *>*remoteScanSource;
//@property (strong, nonatomic) NSMutableArray <SigRemoteScanRspModel *>*remoteUISource;
@property (strong, nonatomic) NSMutableArray <SigRemoteScanRspModel *>*failSource;
@property (assign, nonatomic) BOOL isAddInGATT;
//@property (strong, nonatomic) NSMutableArray <NSNumber *>*scanAddressList;
@property (assign, nonatomic) UInt8 maxItemsCount;
@property (assign, nonatomic) BOOL hasRemoteUUIDs;
@end

@implementation RemoteAddVC

#pragma mark - UICollectionViewDelegate
- (nonnull __kindof UICollectionViewCell *)collectionView:(nonnull UICollectionView *)collectionView cellForItemAtIndexPath:(nonnull NSIndexPath *)indexPath {
    AddDeviceItemCell *cell = (AddDeviceItemCell *)[collectionView dequeueReusableCellWithReuseIdentifier:CellIdentifiers_AddDeviceItemCellID forIndexPath:indexPath];
    AddDeviceModel *model = self.source[indexPath.row];
    [cell updateContent:model];
    return cell;
}

- (NSInteger)collectionView:(nonnull UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    return self.source.count;
}

- (void)showTips:(NSString *)message{
    __weak typeof(self) weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        [weakSelf showAlertSureWithTitle:@"Hits" message:message sure:^(UIAlertAction *action) {
            
        }];
    });
}

#pragma mark - Event
- (void)startAddDevice{
    self.refreshItem.enabled = NO;
    self.goBackButton.enabled = NO;
    [self.goBackButton setBackgroundColor:[UIColor lightGrayColor]];
    
    UInt16 provisionAddress = [SigDataSource.share provisionAddress];
    if (provisionAddress == 0) {
        TeLog(@"warning: address has run out.");
        return;
    }else if (provisionAddress > SigDataSource.share.curProvisionerModel.allocatedUnicastRange.firstObject.hightIntAddress) {
        [self showTips:@"warning: address out of range."];
        return;
    }
    [self.remoteScanSource removeAllObjects];
    [self.failSource removeAllObjects];
    
    if (self.ble.isConnected) {
        //remote add
        [self startRemoteAdd];
    } else {
        //GATT add
        self.isAddInGATT = YES;
        [self addOneDeviceInGATT];
    }
}

- (void)startRemoteAdd{
    [self.ble setNormalState];
//    SigNodeModel *model = [SigDataSource.share getNodeWithUUID:self.ble.currentPeripheral.identifier.UUIDString];
//    self.scanAddressList = [NSMutableArray arrayWithObject:@(model.address)];
    __weak typeof(self) weakSelf = self;
    [self.ble.commandHandle getRemoteProvisioningMaxScannedItemsWithCallback:^(u8 maxItems, u8 activeScan) {
        TeLog(@"maxItems=%d,activeScan=%d",maxItems,activeScan);
        weakSelf.maxItemsCount = maxItems;
        //delay 500ms, then scan uuids.
        [weakSelf performSelector:@selector(startRemoteProvisionScan) withObject:nil afterDelay:0.5];
    }];
    [self performSelector:@selector(getRemoteProvisioningMaxTimeout) withObject:nil afterDelay:6.0];
}

- (void)getRemoteProvisioningMaxTimeout {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(getRemoteProvisioningMaxTimeout) object:nil];
        [self addDeviceFinish];
        [self showTips:@"Reomte scan fail: current connected node is no support remote provision scan."];
    });
}

- (void)startRemoteProvisionScan {
    self.hasRemoteUUIDs = NO;
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(getRemoteProvisioningMaxTimeout) object:nil];
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(endRemoteProvisionScan) object:nil];
        [self performSelector:@selector(endRemoteProvisionScan) withObject:nil afterDelay:4.0+2.0];
    });
    TeLog(@"call remoteScanUUID");
    __weak typeof(self) weakSelf = self;
    [self.ble.commandHandle startRemoteProvisioningScanWithAddress:ADR_ALL_NODES limit:self.maxItemsCount timeout:4 callback:^(SigRemoteScanRspModel *scanRemoteModel) {
        TeLog(@"callback scanRemoteModel");
        [weakSelf updateSigRemoteScanRspModel:scanRemoteModel];
    }];
}

- (void)endRemoteProvisionScan {
    if (self.remoteScanSource.count > 0) {
        //扫描到设备，开始添加
        self.hasRemoteUUIDs = YES;
        TeLog(@"self.remoteScanSource = %@",self.remoteScanSource);
        [self startAddDeviceInRemoteSource];
    } else {
        //为扫描到设备，添加完成
        [self addDeviceFinish];
    }
}

- (void)updateSigRemoteScanRspModel:(SigRemoteScanRspModel *)model{
    TeLog(@"model=%@",model);
    if (![self.failSource containsObject:model]) {
        //=================test==================//
//        if (model.RSSI < -60) {
//            return;
//        }
        //=================test==================//
        if (![self.remoteScanSource containsObject:model]) {
            [self.remoteScanSource addObject:model];
            [self sort];
        } else {
            NSInteger index = [self.remoteScanSource indexOfObject:model];
            SigRemoteScanRspModel *tem = [self.remoteScanSource objectAtIndex:index];
            if (tem.RSSI < model.RSSI) {
                [self.remoteScanSource replaceObjectAtIndex:index withObject:model];
                [self sort];
            }
        }
    }
    TeLog(@"self.remoteScanSource = %@",self.remoteScanSource);
    //=================test==================//
//    dispatch_async(dispatch_get_main_queue(), ^{
//        [self performSelector:@selector(testupdateSigRemoteScanRspModel:) withObject:model afterDelay:2.0];
//    });
    //=================test==================//
}

//=================test==================//
//- (void)testupdateSigRemoteScanRspModel:(SigRemoteScanRspModel *)model {
//    TeLog(@"model=%@",model);
//    if (![self.failSource containsObject:model]) {
//        if (![self.remoteScanSource containsObject:model]) {
//            [self.remoteScanSource addObject:model];
//            [self sort];
//        } else {
//            NSInteger index = [self.remoteScanSource indexOfObject:model];
//            SigRemoteScanRspModel *tem = [self.remoteScanSource objectAtIndex:index];
//            if (tem.RSSI < model.RSSI) {
//                [self.remoteScanSource replaceObjectAtIndex:index withObject:model];
//                [self sort];
//            }
//        }
//    }
//    TeLog(@"self.remoteScanSource = %@",self.remoteScanSource);
//}
//=================test==================//

- (void)sort{
    [self.remoteScanSource sortUsingComparator:^NSComparisonResult(id  _Nonnull obj1, id  _Nonnull obj2) {
        return [(SigRemoteScanRspModel *)obj1 RSSI] < [(SigRemoteScanRspModel *)obj2 RSSI];
    }];
}

- (void)startAddDeviceInRemoteSource{
    
    UInt16 provisionAddress = [SigDataSource.share provisionAddress];
    
    if (provisionAddress == 0) {
        TeLog(@"warning: address has run out.");
        return;
    }

    __weak typeof(self) weakSelf = self;
    NSNumber *type = [[NSUserDefaults standardUserDefaults] valueForKey:kKeyBindType];
    SigRemoteScanRspModel *tem = self.remoteScanSource.firstObject;
    NSData *key = [SigDataSource.share curNetKey];

    
    TeLog(@"startAddDeviceWithNextAddress provisionAddress=%d uuid=%@ macAddress=%@",provisionAddress,tem.uuid,tem.macAddress);
    
    [self.ble.commandHandle startAddDeviceWithNextAddress:provisionAddress networkKey:key netkeyIndex:SigDataSource.share.curNetkeyModel.index unicastAddress:tem.unicastAddress uuid:tem.uuid keyBindType:type.integerValue isAutoAddNextDevice:NO provisionSuccess:^(NSString *identify, UInt16 address) {
        if (identify && address != 0) {
            identify = [LibTools convertDataToHexStr:tem.uuid];
            [weakSelf updateDeviceProvisionSuccess:identify address:address macAddress:tem.macAddress];
            [weakSelf updateMacAddress:tem.macAddress nodeAddress:address];

            TeLog(@"addDevice_provision success : %@->%d",identify,address);
        }
    } keyBindSuccess:^(NSString *identify, UInt16 address) {
        if (identify && address != 0) {
            [weakSelf updateDeviceKeyBindWithMac:tem.macAddress isSuccess:YES];
            
            TeLog(@"addDevice_keybind success : %@->%d",identify,address);
        }
    } fail:^(NSString *errorString) {
        [weakSelf updateDeviceKeyBindWithMac:tem.macAddress isSuccess:NO];
        if (![weakSelf.failSource containsObject:tem]) {
            [weakSelf.failSource addObject:tem];
            TeLog(@"uuid:%@ into the blacklist.",tem.uuid);
        }
        TeLog(@"addDevice fail :%@",errorString);
    } finish:^{
        TeLog(@"finish remote provision");
        [weakSelf addSingleNodeFinish];
        [weakSelf performSelector:@selector(endAddDeviceInRemoteSource) withObject:nil afterDelay:1.0];
    }];
}

- (void)addSingleNodeFinish {
    [self.remoteScanSource removeObjectAtIndex:0];
}

- (void)endAddDeviceInRemoteSource{
    if (self.remoteScanSource.count > 0) {
        //还存在可以添加的设备，继续添加
        self.hasRemoteUUIDs = YES;
        TeLog(@"self.remoteScanSource = %@",self.remoteScanSource);
        [self startAddDeviceInRemoteSource];
    } else {
        //所有设备已经添加完成，开始下一轮扫描
        [self performSelectorOnMainThread:@selector(startRemoteProvisionScan) withObject:nil waitUntilDone:YES];
//        [self startRemoteProvisionScan];
    }
}

- (void)addOneDeviceInGATT{
    [self blockState];
    [self.ble stopAutoConnect];
    [self.ble setProvisionState];
    [self.ble startScan];
    [self performSelector:@selector(addDeviceFinish) withObject:nil afterDelay:kScanTimeOutInAddDevice];
}

- (void)connectOneDeviceInGATTTimeout{
    [self.ble stopAutoConnect];
    [self.ble.manager cancelPeripheralConnection:self.ble.currentPeripheral];
    [self addDeviceFinish];
}

- (void)scrollowToBottom{
    NSInteger item = [self.collectionView numberOfItemsInSection:0] - 1;
    NSIndexPath *lastItemIndex = [NSIndexPath indexPathForItem:item inSection:0];
    [self.collectionView scrollToItemAtIndexPath:lastItemIndex atScrollPosition:UICollectionViewScrollPositionTop animated:NO];
}

- (void)updateDeviceProvisionSuccess:(NSString *)uuid address:(UInt16)address{
    SigScanRspModel *scanModel = [SigDataSource.share getScanRspModelWithUUID:uuid];
    AddDeviceModel *model = [[AddDeviceModel alloc] init];
    if (scanModel == nil) {
        scanModel = [[SigScanRspModel alloc] init];
        scanModel.uuid = uuid;
    }
    model.scanRspModel = scanModel;
    model.scanRspModel.address = address;
    model.state = AddDeviceModelStateBinding;
    if (![self.source containsObject:model]) {
        [self.source addObject:model];
    }
    [self.collectionView reloadData];
    [self scrollowToBottom];
}

- (void)updateDeviceProvisionSuccess:(NSString *)uuid address:(UInt16)address macAddress:(NSString *)macAddress{
    [self updateDeviceProvisionSuccess:uuid address:address];
    AddDeviceModel *model = nil;
    for (AddDeviceModel *tem in self.source) {
        if (tem.scanRspModel.address == address) {
            tem.scanRspModel.macAddress = macAddress;
            model = tem;
            break;
        }
    }
    if (model) {
        [self.collectionView reloadData];
    }
}

- (void)updateDeviceKeyBindWithMac:(NSString *)mac isSuccess:(BOOL)isSuccess{
    for (AddDeviceModel *model in self.source) {
        if ([model.scanRspModel.macAddress isEqualToString:mac]) {
            if (isSuccess) {
                model.state = AddDeviceModelStateBindSuccess;
            } else {
                if (model.state != AddDeviceModelStateBindSuccess) {
                    model.state = AddDeviceModelStateBindFail;
                }else{
                    TeLog(@"警告：赋值异常，放弃赋值。");
                }
            }
            break;
        }
    }
    [self.collectionView reloadData];
}

- (void)updateDeviceKeyBindWithUUID:(NSString *)uuid isSuccess:(BOOL)isSuccess{
    for (AddDeviceModel *model in self.source) {
        if ([model.scanRspModel.uuid isEqualToString:uuid]) {
            if (isSuccess) {
                model.state = AddDeviceModelStateBindSuccess;
            } else {
                if (model.state != AddDeviceModelStateBindSuccess) {
                    model.state = AddDeviceModelStateBindFail;
                }else{
                    TeLog(@"警告：赋值异常，放弃赋值。");
                }
            }
            break;
        }
    }
    [self.collectionView reloadData];
}

- (void)updateMacAddress:(NSString *)macAddress nodeAddress:(UInt16)nodeAddress{
    for (SigNodeModel *node in SigDataSource.share.curNodes) {
        if (node.address == nodeAddress) {
            node.macAddress = macAddress;
            break;
        }
    }
    [SigDataSource.share saveLocationData];
}

- (void)addDeviceFinish{
    self.refreshItem.enabled = YES;
    self.goBackButton.enabled = YES;
    [self.goBackButton setBackgroundColor:kDefultColor];
    [self.ble setNormalState];
}

- (IBAction)clickGoBack:(UIButton *)sender {
    [self.ble setNormalState];
    [self.navigationController popViewControllerAnimated:YES];
}

#pragma mark - Life method
- (void)normalSetting{
    [super normalSetting];
    
    [self.collectionView registerNib:[UINib nibWithNibName:CellIdentifiers_AddDeviceItemCellID bundle:nil] forCellWithReuseIdentifier:CellIdentifiers_AddDeviceItemCellID];
    
//    self.refreshItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh target:self action:@selector(startAddDevice)];
//    self.navigationItem.rightBarButtonItem = self.refreshItem;
    self.title = @"Device Scan(Remote)";
    
    self.source = [[NSMutableArray alloc] init];
    self.remoteScanSource = [NSMutableArray array];
    self.failSource = [NSMutableArray array];
    self.isAddInGATT = NO;
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
    self.navigationItem.hidesBackButton = YES;
    
    [self startAddDevice];
}

-(void)blockState{
    [super blockState];
    
    __weak typeof(self) weakSelf = self;
    [self.ble setBleScanNewDeviceCallBack:^(CBPeripheral *peripheral, BOOL provisioned) {
        if (weakSelf.isAddInGATT) {
            if (peripheral && provisioned) {
                dispatch_async(dispatch_get_main_queue(), ^{
                    [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(addDeviceFinish) object:nil];
                    [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(connectOneDeviceInGATTTimeout) object:nil];
                    [weakSelf performSelector:@selector(connectOneDeviceInGATTTimeout) withObject:nil afterDelay:kScanTimeOutInAddDevice];
                });
                [weakSelf.ble stopScan];
                [weakSelf.ble connectPeripheral:peripheral];
            }
        }
    }];
    [self.ble setBleFinishScanedCharachteristicCallBack:^(CBPeripheral *peripheral) {
        if (weakSelf.isAddInGATT) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(connectOneDeviceInGATTTimeout) object:nil];
            });
            UInt16 provisionAddress = [SigDataSource.share provisionAddress];
            if (provisionAddress == 0) {
                TeLog(@"warning: address has run out.");
                return;
            }

            NSNumber *type = [[NSUserDefaults standardUserDefaults] valueForKey:kKeyBindType];
            NSData *key = [SigDataSource.share curNetKey];
            [weakSelf.ble.commandHandle startAddDeviceWithNextAddress:provisionAddress networkKey:key netkeyIndex:SigDataSource.share.curNetkeyModel.index unicastAddress:0 uuid:nil keyBindType:type.integerValue isAutoAddNextDevice:NO provisionSuccess:^(NSString *identify, UInt16 address) {
                if (identify && address != 0) {
                    [weakSelf updateDeviceProvisionSuccess:identify address:address];
                    TeLog(@"addDevice_provision success : %@->%d",identify,address);
                }
            } keyBindSuccess:^(NSString *identify, UInt16 address) {
                if (identify && address != 0) {
                    [weakSelf updateDeviceKeyBindWithUUID:identify isSuccess:YES];
                    TeLog(@"addDevice_keybind success : %@->%d",identify,address);
                    [weakSelf startRemoteAdd];
                }
            } fail:^(NSString *errorString) {
                [weakSelf updateDeviceKeyBindWithUUID:weakSelf.ble.currentPeripheral.identifier.UUIDString isSuccess:NO];
                TeLog(@"addDevcie fail :%@",errorString);
                [weakSelf addDeviceFinish];
            } finish:^{
                TeLog(@"finish gatt provision");
            }];

        }
    }];
}

-(void)dealloc{
    TeLog(@"%s",__func__);
}

@end
