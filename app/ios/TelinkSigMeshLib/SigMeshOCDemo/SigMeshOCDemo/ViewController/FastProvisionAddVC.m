/********************************************************************************************************
 * @file     FastProvisionAddVC.m
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
//  FastProvisionAddVC.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2019/9/19.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "FastProvisionAddVC.h"
#import "AddDeviceItemCell.h"
#import "UIViewController+Message.h"

@interface FastProvisionAddVC ()<UICollectionViewDelegate,UICollectionViewDataSource>
@property (weak, nonatomic) IBOutlet UICollectionView *collectionView;
@property (weak, nonatomic) IBOutlet UIButton *goBackButton;
@property (strong, nonatomic) NSMutableArray <AddDeviceModel *>*source;
@property (strong, nonatomic) UIBarButtonItem *refreshItem;
@property (nonatomic,strong) NSString *currentUUID;
@property (nonatomic,assign) BOOL isAdding;
@property (nonatomic,assign) BOOL currentConnectedNodeIsUnprovisioned;
@end

@implementation FastProvisionAddVC

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

#pragma mark - Event
- (void)startAddDevice{
    [self userAbled:NO];
    _isAdding = YES;
    if (SigBearer.share.isOpen) {
        self.currentConnectedNodeIsUnprovisioned = NO;
        [self startFastProvision];
    } else {
        self.currentConnectedNodeIsUnprovisioned = YES;
        [self startScanSingleUnProvisionNode];
    }
}

- (void)startScanSingleUnProvisionNode {
    __weak typeof(self) weakSelf = self;
    SigBearer.share.isAutoReconnect = NO;
    [SigBearer.share stopMeshConnectWithComplete:^(BOOL successful) {
        if (weakSelf.isAdding) {
            if (successful) {
                dispatch_async(dispatch_get_main_queue(), ^{
                    [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(scanSingleUnProvisionNodeTimeout) object:nil];
                    [weakSelf performSelector:@selector(scanSingleUnProvisionNodeTimeout) withObject:nil afterDelay:5.0];
                });
                [SigBluetooth.share scanUnprovisionedDevicesWithResult:^(CBPeripheral * _Nonnull peripheral, NSDictionary<NSString *,id> * _Nonnull advertisementData, NSNumber * _Nonnull RSSI, BOOL unprovisioned) {
                    if (unprovisioned && RSSI.intValue > -50) {
                        TeLogInfo(@"advertisementData=%@,rssi=%@,unprovisioned=%@",advertisementData,RSSI,unprovisioned?@"没有入网":@"已经入网");
                        dispatch_async(dispatch_get_main_queue(), ^{
                            [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(scanSingleUnProvisionNodeTimeout) object:nil];
                        });
                        [SigBluetooth.share stopScan];
                        [SigBearer.share changePeripheral:peripheral result:^(BOOL successful) {
                            if (successful) {
                                [SigBearer.share openWithResult:^(BOOL successful) {
                                    if (successful) {
                                        [weakSelf startFastProvision];
                                    } else {
                                        NSString *str = @"connect fail.";
                                        TeLogError(@"%@",str);
                                        [weakSelf showTips:str];
                                        [weakSelf userAbled:YES];
                                        weakSelf.isAdding = NO;
                                    }
                                }];
                            } else {
                                NSString *str = @"change node fail.";
                                TeLogError(@"%@",str);
                                [weakSelf showTips:str];
                                [weakSelf userAbled:YES];
                                weakSelf.isAdding = NO;
                            }
                        }];
                    }
                }];
            } else {
                NSString *str = @"close fail.";
                TeLogError(@"%@",str);
                [weakSelf showTips:str];
                [weakSelf userAbled:YES];
                weakSelf.isAdding = NO;
            }
        }
    }];
}

- (void)scanSingleUnProvisionNodeTimeout {
    [self userAbled:YES];
    self.isAdding = NO;
}

- (void)startFastProvision {
    UInt16 provisionAddress = SigDataSource.share.provisionAddress;
    __weak typeof(self) weakSelf = self;
    [SigFastProvisionAddManager.share startFastProvisionWithProvisionAddress:provisionAddress productId:SigNodePID_CT compositionData:[NSData dataWithBytes:CTByte length:76] currentConnectedNodeIsUnprovisioned:self.currentConnectedNodeIsUnprovisioned scanResponseCallback:^(NSData * _Nonnull deviceKey, NSString * _Nonnull macAddress, UInt16 address, UInt16 pid) {
        [weakSelf updateScanedDeviceWithDeviceKey:deviceKey macAddress:macAddress address:address pid:pid];
    } startProvisionCallback:^{
        [weakSelf updateStartProvision];
    } addSingleDeviceSuccessCallback:^(NSData * _Nonnull deviceKey, NSString * _Nonnull macAddress, UInt16 address, UInt16 pid) {
        TeLogInfo(@"fast provision single success, deviceKey=%@, macAddress=%@, address=0x%x, pid=%d",[LibTools convertDataToHexStr:deviceKey],macAddress,address,pid);
        [weakSelf updateDeviceSuccessWithDeviceKey:deviceKey macAddress:macAddress address:address pid:pid];
    } finish:^(NSError * _Nullable error) {
        TeLogInfo(@"error=%@",error);
        [weakSelf userAbled:YES];
        weakSelf.isAdding = NO;
    }];
//    [SigFastProvisionAddManager.share startFastProvisionWithProvisionAddress:provisionAddress productId:SigNodePID_CT compositionData:[NSData dataWithBytes:CTByte length:76] currentConnectedNodeIsUnprovisioned:self.currentConnectedNodeIsUnprovisioned addSingleDeviceSuccessCallback:^(NSData * _Nonnull deviceKey, NSString * _Nonnull macAddress, UInt16 address, UInt16 pid) {
//        TeLogInfo(@"fast provision single success, deviceKey=%@, macAddress=%@, address=0x%x, pid=%d",[LibTools convertDataToHexStr:deviceKey],macAddress,address,pid);
//        [weakSelf updateDeviceSuccessWithDeviceKey:deviceKey macAddress:macAddress address:address pid:pid];
//    } finish:^(NSError * _Nullable error) {
//        TeLogInfo(@"error=%@",error);
//        [weakSelf userAbled:YES];
//        weakSelf.isAdding = NO;
//    }];
}

- (void)userAbled:(BOOL)able{
    dispatch_async(dispatch_get_main_queue(), ^{
        self.refreshItem.enabled = able;
        self.goBackButton.enabled = able;
        [self.goBackButton setBackgroundColor:able ? kDefultColor : kDefultUnableColor];
    });
}

- (void)showTips:(NSString *)message{
    dispatch_async(dispatch_get_main_queue(), ^{
        [self showAlertSureWithTitle:@"Hits" message:message sure:^(UIAlertAction *action) {

        }];
    });
}

- (void)updateScanedDeviceWithDeviceKey:(NSData *)deviceKey macAddress:(NSString *)macAddress address:(UInt16)address pid:(UInt16)pid {
    AddDeviceModel *model = [[AddDeviceModel alloc] init];
    SigScanRspModel *scanModel = [[SigScanRspModel alloc] init];
    scanModel.macAddress = macAddress;
    model.scanRspModel = scanModel;
    model.scanRspModel.address = address;
    model.state = AddDeviceModelStateScaned;
    if (![self.source containsObject:model]) {
        [self.source addObject:model];
    } else {
        [self.source replaceObjectAtIndex:[self.source indexOfObject:model] withObject:model];
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.collectionView reloadData];
        [self scrollowToBottom];
    });
}

- (void)updateStartProvision {
    NSArray *array = [NSArray arrayWithArray:self.source];
    for (AddDeviceModel *model in array) {
        model.state = AddDeviceModelStateProvisioning;
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.collectionView reloadData];
        [self scrollowToBottom];
    });
}

- (void)updateDeviceSuccessWithDeviceKey:(NSData *)deviceKey macAddress:(NSString *)macAddress address:(UInt16)address pid:(UInt16)pid {
    AddDeviceModel *model = [[AddDeviceModel alloc] init];
    SigScanRspModel *scanModel = [[SigScanRspModel alloc] init];
    scanModel.macAddress = macAddress;
    model.scanRspModel = scanModel;
    model.scanRspModel.address = address;
    model.state = AddDeviceModelStateBindSuccess;
    if (![self.source containsObject:model]) {
        [self.source addObject:model];
    } else {
        [self.source replaceObjectAtIndex:[self.source indexOfObject:model] withObject:model];
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.collectionView reloadData];
        [self scrollowToBottom];
    });
}

- (void)scrollowToBottom{
    NSInteger item = [self.collectionView numberOfItemsInSection:0] - 1;
    NSIndexPath *lastItemIndex = [NSIndexPath indexPathForItem:item inSection:0];
    [self.collectionView scrollToItemAtIndexPath:lastItemIndex atScrollPosition:UICollectionViewScrollPositionTop animated:NO];
}

- (IBAction)clickGoBack:(UIButton *)sender {
    if (SigBearer.share.isOpen) {
            __weak typeof(self) weakSelf = self;
            SigProvisionerModel *provisioner = SigDataSource.share.curProvisionerModel;
            [SDKLibCommand setFilterForProvisioner:provisioner successCallback:^(UInt16 source, UInt16 destination, SigFilterStatus * _Nonnull responseMessage) {
        //        dispatch_async(dispatch_get_main_queue(), ^{
        //            [weakSelf.navigationController popViewControllerAnimated:YES];
        //        });
            } finishCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
                if (error) {
                    TeLogError(@"setFilter fail!!!");
                    //失败后逻辑：断开连接，再返回
                    [SigBearer.share stopMeshConnectWithComplete:^(BOOL successful) {
                        dispatch_async(dispatch_get_main_queue(), ^{
                            [weakSelf.navigationController popViewControllerAnimated:YES];
                        });
                    }];
                } else {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [weakSelf.navigationController popViewControllerAnimated:YES];
                    });
                }
            }];
    } else {
        [self.navigationController popViewControllerAnimated:YES];
    }
}

#pragma mark - Life method
- (void)normalSetting{
    [super normalSetting];
    self.title = @"Fast provision";
    self.source = [[NSMutableArray alloc] init];

    [self.collectionView registerNib:[UINib nibWithNibName:CellIdentifiers_AddDeviceItemCellID bundle:nil] forCellWithReuseIdentifier:CellIdentifiers_AddDeviceItemCellID];
    
//    self.refreshItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh target:self action:@selector(startAddDevice)];
//    self.navigationItem.rightBarButtonItem = self.refreshItem;

}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
    self.navigationItem.hidesBackButton = YES;
    
    [self startAddDevice];
}

-(void)dealloc{
    TeLogDebug(@"%s",__func__);
}

@end
