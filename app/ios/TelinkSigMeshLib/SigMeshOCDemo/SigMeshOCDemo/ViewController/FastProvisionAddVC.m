/********************************************************************************************************
 * @file     FastProvisionAddVC.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/9/19
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
    [SDKLibCommand stopMeshConnectWithComplete:^(BOOL successful) {
        if (weakSelf.isAdding) {
            if (successful) {
                dispatch_async(dispatch_get_main_queue(), ^{
                    [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(scanSingleUnProvisionNodeTimeout) object:nil];
                    [weakSelf performSelector:@selector(scanSingleUnProvisionNodeTimeout) withObject:nil afterDelay:5.0];
                });
                [SDKLibCommand scanUnprovisionedDevicesWithResult:^(CBPeripheral * _Nonnull peripheral, NSDictionary<NSString *,id> * _Nonnull advertisementData, NSNumber * _Nonnull RSSI, BOOL unprovisioned) {
                        if (unprovisioned) {
//                            //RSSI太弱会容易出现连接失败。
//                            if (RSSI.intValue <= -70) {
//                                return;
//                            }
                        TeLogInfo(@"advertisementData=%@,rssi=%@,unprovisioned=%@",advertisementData,RSSI,unprovisioned?@"没有入网":@"已经入网");
                        dispatch_async(dispatch_get_main_queue(), ^{
                            [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(scanSingleUnProvisionNodeTimeout) object:nil];
                        });
                        [SDKLibCommand stopScan];
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
    [self showTips:@"There is no unprovision device nearby!"];
    [self userAbled:YES];
    self.isAdding = NO;
}

- (void)startFastProvision {
    UInt16 provisionAddress = SigDataSource.share.provisionAddress;
    __weak typeof(self) weakSelf = self;
//    [SigFastProvisionAddManager.share startFastProvisionWithProvisionAddress:provisionAddress productId:SigNodePID_CT compositionData:[NSData dataWithBytes:CTByte length:sizeof(CTByte)] currentConnectedNodeIsUnprovisioned:self.currentConnectedNodeIsUnprovisioned scanResponseCallback:^(NSData * _Nonnull deviceKey, NSString * _Nonnull macAddress, UInt16 address, UInt16 pid) {
//        [weakSelf updateScanedDeviceWithDeviceKey:deviceKey macAddress:macAddress address:address pid:pid];
//    } startProvisionCallback:^{
//        [weakSelf updateStartProvision];
//    } addSingleDeviceSuccessCallback:^(NSData * _Nonnull deviceKey, NSString * _Nonnull macAddress, UInt16 address, UInt16 pid) {
//        TeLogInfo(@"fast provision single success, deviceKey=%@, macAddress=%@, address=0x%x, pid=%d",[LibTools convertDataToHexStr:deviceKey],macAddress,address,pid);
//        [weakSelf updateDeviceSuccessWithDeviceKey:deviceKey macAddress:macAddress address:address pid:pid];
//    } finish:^(NSError * _Nullable error) {
//        TeLogInfo(@"error=%@",error);
//        [weakSelf addFinish];
//        [SDKLibCommand startMeshConnectWithComplete:nil];
//        [weakSelf userAbled:YES];
//        weakSelf.isAdding = NO;
//    }];
    //注意：如果客户的compositionData不是默认的数据，需要开发者新增或者修改SigDataSource.share.defaultNodeInfos里面的数据。
    //添加所有已经存在cpsData的pid的设备
    NSMutableArray *productIds = [NSMutableArray array];
    for (DeviceTypeModel *model in SigDataSource.share.defaultNodeInfos) {
        [productIds addObject:@(model.PID)];
    }
    [SigFastProvisionAddManager.share startFastProvisionWithProvisionAddress:provisionAddress productIds:productIds currentConnectedNodeIsUnprovisioned:self.currentConnectedNodeIsUnprovisioned scanResponseCallback:^(NSData * _Nonnull deviceKey, NSString * _Nonnull macAddress, UInt16 address, UInt16 pid) {
        [weakSelf updateScanedDeviceWithDeviceKey:deviceKey macAddress:macAddress address:address pid:pid];
    } startProvisionCallback:^{
        [weakSelf updateStartProvision];
    } addSingleDeviceSuccessCallback:^(NSData * _Nonnull deviceKey, NSString * _Nonnull macAddress, UInt16 address, UInt16 pid) {
        TeLogInfo(@"fast provision single success, deviceKey=%@, macAddress=%@, address=0x%x, pid=%d",[LibTools convertDataToHexStr:deviceKey],macAddress,address,pid);
        [weakSelf updateDeviceSuccessWithDeviceKey:deviceKey macAddress:macAddress address:address pid:pid];
    } finish:^(NSError * _Nullable error) {
        TeLogInfo(@"error=%@",error);
        if (error) {
            [weakSelf showTips:error.domain];
        }
        [weakSelf addFinish];
        [SDKLibCommand startMeshConnectWithComplete:nil];
        [weakSelf userAbled:YES];
        weakSelf.isAdding = NO;
    }];
}

- (void)userAbled:(BOOL)able{
    dispatch_async(dispatch_get_main_queue(), ^{
        self.refreshItem.enabled = able;
        self.goBackButton.enabled = able;
        [self.goBackButton setBackgroundColor:able ? UIColor.telinkButtonBlue : UIColor.telinkButtonUnableBlue];
    });
}

- (void)updateScanedDeviceWithDeviceKey:(NSData *)deviceKey macAddress:(NSString *)macAddress address:(UInt16)address pid:(UInt16)pid {
    AddDeviceModel *model = [[AddDeviceModel alloc] init];
    SigScanRspModel *scanModel = [[SigScanRspModel alloc] init];
    scanModel.macAddress = macAddress;
    model.scanRspModel = scanModel;
    model.scanRspModel.address = address;
    model.state = AddDeviceModelStateScanned;
    if (![self.source containsObject:model]) {
        [self.source addObject:model];
    } else {
        [self.source replaceObjectAtIndex:[self.source indexOfObject:model] withObject:model];
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.collectionView reloadData];
        [self scrollToBottom];
    });
}

- (void)updateStartProvision {
    NSArray *array = [NSArray arrayWithArray:self.source];
    for (AddDeviceModel *model in array) {
        model.state = AddDeviceModelStateProvisioning;
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.collectionView reloadData];
        [self scrollToBottom];
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
        [self scrollToBottom];
    });
}

- (void)addFinish {
    BOOL needRefresh = NO;
    NSArray *array = [NSArray arrayWithArray:self.source];
    for (AddDeviceModel *model in array) {
        if (model.state != AddDeviceModelStateBindSuccess) {
            model.state = AddDeviceModelStateProvisionFail;
            needRefresh = YES;
        }
    }
    if (needRefresh) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [self.collectionView reloadData];
            [self scrollToBottom];
        });
    }
}

- (void)scrollToBottom{
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
                    [SDKLibCommand stopMeshConnectWithComplete:^(BOOL successful) {
                        dispatch_async(dispatch_get_main_queue(), ^{
                            [weakSelf.navigationController popViewControllerAnimated:YES];
                        });
                    }];
                } else {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        //修复添加设备完成后返回首页断开直连设备不会回连的bug.
                        //Fix the bug where disconnecting a directly connected device after adding it and returning to the homepage will not cause it to reconnect.
                        if (SigBearer.share.isOpen) {
                            [SDKLibCommand startMeshConnectWithComplete:nil];
                        }
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
