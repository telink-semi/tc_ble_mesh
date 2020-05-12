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

//优先添加大于阈值的设备
#define kRSSIThresholdValue (-80)

/*
 Developer can get more detail by read doucment named "SIG Mesh iOS APP(OC版本)使用以及开发手册.docx".
 */

@interface RemoteAddVC ()<UICollectionViewDelegate,UICollectionViewDataSource>
@property (weak, nonatomic) IBOutlet UICollectionView *collectionView;
@property (weak, nonatomic) IBOutlet UIButton *goBackButton;
@property (strong, nonatomic) NSMutableArray <AddDeviceModel *>*source;
@property (strong, nonatomic) UIBarButtonItem *refreshItem;
@property (strong, nonatomic) NSMutableArray <SigRemoteScanRspModel *>*remoteSource;
@property (strong, nonatomic) NSMutableArray <SigRemoteScanRspModel *>*failSource;
@property (assign, nonatomic) UInt8 maxItemsCount;
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

//==========test==========//
//- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath {
//    AddDeviceModel *addModel = self.source[indexPath.item];
//    SigRemoteScanRspModel *model = nil;
//    if (self.failSource.count > indexPath.item) {
//        model = self.failSource[indexPath.item];
//    } else if (self.remoteSource.count > indexPath.row) {
//        model = self.remoteSource[indexPath.item];
//    }
//    if (addModel.state == AddDeviceModelStateScaned || addModel.state == AddDeviceModelStateProvisionFail  || addModel.state == AddDeviceModelStateProvisioning) {
//        addModel.state = AddDeviceModelStateProvisioning;
//        [collectionView reloadItemsAtIndexPaths:@[indexPath]];
//        [self remoteProvisionNodeWithRemoteScanRspModel:model];
//    }
//}
//==========test==========//

#pragma mark - RP-GATT: provision+keybind
- (void)addOneNodeInGATT{
    NSNumber *type = [[NSUserDefaults standardUserDefaults] valueForKey:kKeyBindType];
    __weak typeof(self) weakSelf = self;
    UInt16 provisionAddress = [SigDataSource.share provisionAddress];
    [SDKLibCommand startAddDeviceWithNextAddress:provisionAddress networkKey:SigDataSource.share.curNetKey netkeyIndex:SigDataSource.share.curNetkeyModel.index appkeyModel:SigDataSource.share.curAppkeyModel unicastAddress:0 uuid:nil keyBindType:type.integerValue productID:0 cpsData:nil isAutoAddNextDevice:NO provisionSuccess:^(NSString * _Nonnull identify, UInt16 address) {
        if (identify && address != 0) {
            SigScanRspModel *scanRsp = [SigDataSource.share getScanRspModelWithUUID:identify];
            NSString *mac = @"";
            if (scanRsp) {
                mac = scanRsp.macAddress;
            }
            [weakSelf updateWithPeripheralUUID:identify macAddress:mac address:address provisionResult:YES];
            TeLogInfo(@"RP-GATT:provision success, %@->0X%X",identify,address);
        }
    } provisionFail:^(NSError * _Nonnull error) {
        NSString *identify = SigBearer.share.getCurrentPeripheral.identifier.UUIDString;
        SigScanRspModel *scanRsp = [SigDataSource.share getScanRspModelWithUUID:identify];
        NSString *mac = @"";
        if (scanRsp) {
            mac = scanRsp.macAddress;
        }
        [weakSelf updateWithPeripheralUUID:identify macAddress:mac address:provisionAddress provisionResult:NO];
        TeLogInfo(@"RP-GATT:provision fail, error:%@",error);
    } keyBindSuccess:^(NSString * _Nonnull identify, UInt16 address) {
        if (identify && address != 0) {
            SigScanRspModel *scanRsp = [SigDataSource.share getScanRspModelWithUUID:identify];
            NSString *mac = @"";
            if (scanRsp) {
                mac = scanRsp.macAddress;
            }
            [weakSelf updateWithPeripheralUUID:identify macAddress:mac address:provisionAddress keyBindResult:YES];
            TeLogInfo(@"RP-GATT:keybind success, %@->0X%X",identify,address);
            [weakSelf startRemoteProvisionScan];
        }
    } keyBindFail:^(NSError * _Nonnull error) {
        NSString *identify = SigBearer.share.getCurrentPeripheral.identifier.UUIDString;
        SigScanRspModel *scanRsp = [SigDataSource.share getScanRspModelWithUUID:identify];
        NSString *mac = @"";
        if (scanRsp) {
            mac = scanRsp.macAddress;
        }
        [weakSelf updateWithPeripheralUUID:identify macAddress:mac address:provisionAddress keyBindResult:NO];
        TeLogInfo(@"RP-GATT:keybind fail, error:%@",error);
    } finish:^{
        TeLogInfo(@"RP-GATT: finish.");
    }];
}

#pragma mark RP-Remote: remote scan
- (void)startRemoteProvisionScan {
    TeLogInfo(@"RP-Remote: start scan.");
    if (kExistRemoteProvision) {
        __weak typeof(self) weakSelf = self;
        [SigRemoteAddManager.share startRemoteProvisionScanWithReportCallback:^(SigRemoteScanRspModel * _Nonnull scanRemoteModel) {
            TeLogInfo(@"RP-Remote:reportNodeAddress=0x%x,uuid=%@,rssi=%d,oob=%d,mac=%@",scanRemoteModel.reportNodeAddress,scanRemoteModel.reportNodeUUID,scanRemoteModel.RSSI,scanRemoteModel.oob.value,scanRemoteModel.macAddress);
            [weakSelf addAndShowSigRemoteScanRspModelToUI:scanRemoteModel];
        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
            TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
            if (error) {
                [weakSelf showRemoteProvisionError:error];
            } else {
                if (weakSelf.remoteSource && weakSelf.remoteSource.count > 0) {
                    //remote扫描到设备，开始添加
                    //优化：先过滤阈值的设备，提高添加成功率。
                    if (weakSelf.remoteSource.count > 1) {
                        NSArray *oldSource = [NSArray arrayWithArray:weakSelf.remoteSource];
                        NSMutableArray *newSource = [NSMutableArray array];
                        for (SigRemoteScanRspModel *tem in oldSource) {
                            if (tem.RSSI >= kRSSIThresholdValue) {
                                [newSource addObject:tem];
                            }
                        }
                        if (newSource.count == 0 && oldSource.count > 0) {
                            [newSource addObject:oldSource.firstObject];
                        }
                        weakSelf.remoteSource = [NSMutableArray arrayWithArray:newSource];
                    }
                    [weakSelf addNodeByRemoteProvision];
                } else {
                    //remote未扫描到设备
                    if (weakSelf.source.count > 0) {
                        [weakSelf addDeviceFinish];
                    } else {
                        NSString *errstr = @"Reomte scan timeout: no support remote provision device.";
                        TeLogError(@"%@",errstr);
                        NSError *err = [NSError errorWithDomain:errstr code:-1 userInfo:nil];
                        [weakSelf showRemoteProvisionError:err];
                    }
                }
            }
        }];
    }
}

- (void)addNodeByRemoteProvision {
    if (self.remoteSource && self.remoteSource.count > 0) {
        //存在remote扫描到的设备，开始添加
        SigRemoteScanRspModel *model = self.remoteSource.firstObject;
        [self updateUIOfStartProvisionWithPeripheralUUID:[LibTools convertDataToHexStr:model.reportNodeUUID] macAddress:model.macAddress address:SigDataSource.share.provisionAddress];
        [self remoteProvisionNodeWithRemoteScanRspModel:model];
    } else {
        //不存在remote扫描到的设备，继续扫描
        [self startRemoteProvisionScan];
    }
}

#pragma mark RP-Remote: start provision
- (void)remoteProvisionNodeWithRemoteScanRspModel:(SigRemoteScanRspModel *)model {
    __weak typeof(self) weakSelf = self;
    UInt16 provisionAddress = SigDataSource.share.provisionAddress;
    TeLogInfo(@"RP-Remote: start provision, uuid:%@,macAddress:%@->0x%x.",model.reportNodeUUID,model.macAddress,provisionAddress);
    if (kExistRemoteProvision) {
        [SigRemoteAddManager.share remoteProvisionWithNextProvisionAddress:provisionAddress reportNodeAddress:model.reportNodeAddress reportNodeUUID:model.reportNodeUUID networkKey:SigDataSource.share.curNetKey netkeyIndex:SigDataSource.share.curNetkeyModel.index provisionType:ProvisionTpye_NoOOB staticOOBData:nil provisionSuccess:^(NSString * _Nonnull identify, UInt16 address) {
            [weakSelf updateWithPeripheralUUID:[LibTools convertDataToHexStr:model.reportNodeUUID] macAddress:model.macAddress address:provisionAddress provisionResult:YES];
                TeLogInfo(@"RP-Remote:provision success, %@->0X%X",identify,address);
            SigNodeModel *node = [SigDataSource.share getNodeWithAddress:provisionAddress];
            if (node) {
                [SDKLibCommand remoteProvisioningLinkCloseWithDestination:model.reportNodeAddress reason:SigRemoteProvisioningLinkCloseStatus_success retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigRemoteProvisioningLinkStatus * _Nonnull responseMessage) {
                    
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    if (error != nil && isResponseAll == NO) {
                        TeLogError(@"link close fail.");
                    }
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(keyBindWithNode:) object:node];
                        [weakSelf performSelector:@selector(keyBindWithNode:) withObject:node afterDelay:2.0];
                    });
                }];
            } else {
                TeLogError(@"node = nil.");
            }
        } fail:^(NSError * _Nonnull error) {
            TeLogDebug(@"RP-Remote:provision fail.");
            [SDKLibCommand remoteProvisioningLinkCloseWithDestination:model.reportNodeAddress reason:SigRemoteProvisioningLinkCloseStatus_fail retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigRemoteProvisioningLinkStatus * _Nonnull responseMessage) {
                
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                if (error != nil && isResponseAll == NO) {
                    TeLogError(@"link close fail.");
                }
                [weakSelf.failSource addObject:model];
                [weakSelf remoteAddSingleDeviceFinish];
                [weakSelf updateWithPeripheralUUID:[LibTools convertDataToHexStr:model.reportNodeUUID] macAddress:model.macAddress address:provisionAddress provisionResult:NO];
                TeLogInfo(@"RP-Remote:provision fail, error=%@",error);
                [weakSelf addNodeByRemoteProvision];
            }];
        }];
    }
}

#pragma mark RP-Remote: start keybind
- (void)keyBindWithNode:(SigNodeModel *)node {
    TeLogInfo(@"RP-Remote: start keybind");
    __weak typeof(self) weakSelf = self;
    if (SigBearer.share.isOpen) {
        AddDeviceModel *model = [self getAddDeviceModelWithMacAddress:node.macAddress];
        NSNumber *type = [[NSUserDefaults standardUserDefaults] valueForKey:kKeyBindType];
        [SigKeyBindManager.share keyBind:node.address appkeyModel:SigDataSource.share.curAppkeyModel keyBindType:type.integerValue productID:0 cpsData:nil keyBindSuccess:^(NSString * _Nonnull identify, UInt16 address) {
            [weakSelf remoteAddSingleDeviceFinish];
            [weakSelf updateWithPeripheralUUID:model.scanRspModel.uuid macAddress:model.scanRspModel.macAddress address:node.address keyBindResult:YES];
            TeLogInfo(@"RP-Remote:keybind success, %@->0X%X",model.scanRspModel.uuid,node.address);
            [weakSelf addNodeByRemoteProvision];
        } fail:^(NSError * _Nonnull error) {
            [weakSelf remoteAddSingleDeviceFinish];
            [weakSelf updateWithPeripheralUUID:model.scanRspModel.uuid macAddress:model.scanRspModel.macAddress address:node.address keyBindResult:NO];
            TeLogInfo(@"RP-Remote:keybind fail, error:%@",error);
            [weakSelf addNodeByRemoteProvision];
        }];
    }
}

#pragma mark - Event
- (void)startAddDevice{
    self.refreshItem.enabled = NO;
    self.goBackButton.enabled = NO;
    [self.goBackButton setBackgroundColor:[UIColor lightGrayColor]];
    
    UInt16 provisionAddress = [SigDataSource.share provisionAddress];
    if (provisionAddress == 0) {
        TeLogInfo(@"warning: address has run out.");
        return;
    }
    [self.remoteSource removeAllObjects];
    
    if (SigBearer.share.isOpen) {
        //remote add
        [self startRemoteProvisionScan];
    } else {
        //GATT add
        [self addOneNodeInGATT];
    }
}

- (IBAction)clickGoBack:(UIButton *)sender {
    [self.navigationController popViewControllerAnimated:YES];
}

#pragma mark - UI refresh

- (void)addAndShowSigRemoteScanRspModelToUI:(SigRemoteScanRspModel *)scanRemoteModel {
    if (![self.remoteSource containsObject:scanRemoteModel]) {
        if (![self.failSource containsObject:scanRemoteModel]) {
            [self.remoteSource addObject:scanRemoteModel];
        }
    } else {
        NSInteger index = [self.remoteSource indexOfObject:scanRemoteModel];
        SigRemoteScanRspModel *tem = [self.remoteSource objectAtIndex:index];
        if (tem.RSSI < scanRemoteModel.RSSI) {
            [self.remoteSource replaceObjectAtIndex:index withObject:scanRemoteModel];
        }
    }
    [self.remoteSource sortUsingComparator:^NSComparisonResult(id  _Nonnull obj1, id  _Nonnull obj2) {
        return [(SigRemoteScanRspModel *)obj1 RSSI] < [(SigRemoteScanRspModel *)obj2 RSSI];
    }];
}

- (void)updateUIOfStartProvisionWithPeripheralUUID:(NSString *)peripheralUUID macAddress:(NSString *)macAddress address:(UInt16)address {
    [self checkExistAddModelWithPeripheralUUID:peripheralUUID macAddress:macAddress address:address];
    AddDeviceModel *model = [self getAddDeviceModelWithPeripheralUUID:peripheralUUID];
    model.state = AddDeviceModelStateProvisioning;
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.collectionView reloadData];
    });
}

- (void)updateWithPeripheralUUID:(NSString *)peripheralUUID macAddress:(NSString *)macAddress address:(UInt16)address provisionResult:(BOOL)provisionResult {
    [self checkExistAddModelWithPeripheralUUID:peripheralUUID macAddress:macAddress address:address];
    AddDeviceModel *model = [self getAddDeviceModelWithPeripheralUUID:peripheralUUID];
    model.state = provisionResult ? AddDeviceModelStateBinding : AddDeviceModelStateProvisionFail;
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.collectionView reloadData];
    });
}

- (void)updateWithPeripheralUUID:(NSString *)peripheralUUID macAddress:(NSString *)macAddress address:(UInt16)address keyBindResult:(BOOL)keyBindResult {
    [self checkExistAddModelWithPeripheralUUID:peripheralUUID macAddress:macAddress address:address];
    AddDeviceModel *model = [self getAddDeviceModelWithPeripheralUUID:peripheralUUID];
    model.state = keyBindResult ? AddDeviceModelStateBindSuccess : AddDeviceModelStateBindFail;
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.collectionView reloadData];
    });
}

- (void)remoteAddSingleDeviceFinish {
    if (self.remoteSource && self.remoteSource.count > 0) {
        [self.remoteSource removeObjectAtIndex:0];
    }
}

- (void)addDeviceFinish{
    [SigBearer.share startMeshConnectWithComplete:nil];
    dispatch_async(dispatch_get_main_queue(), ^{
        self.refreshItem.enabled = YES;
        self.goBackButton.enabled = YES;
        [self.goBackButton setBackgroundColor:kDefultColor];
    });
}

- (void)showRemoteProvisionError:(NSError *)error {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self addDeviceFinish];
        [self showTips:error.domain];
    });
}

- (void)showTips:(NSString *)tips{
    dispatch_async(dispatch_get_main_queue(), ^{
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Hits" message:tips preferredStyle:UIAlertControllerStyleAlert];
        [alertController addAction:[UIAlertAction actionWithTitle:@"Sure" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            TeLogDebug(@"点击确认");
        }]];
        [self presentViewController:alertController animated:YES completion:nil];
    });
}

#pragma mark - Life method
- (void)normalSetting{
    [super normalSetting];
    
    [self.collectionView registerNib:[UINib nibWithNibName:CellIdentifiers_AddDeviceItemCellID bundle:nil] forCellWithReuseIdentifier:CellIdentifiers_AddDeviceItemCellID];
    
//    self.refreshItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh target:self action:@selector(startAddDevice)];
//    self.navigationItem.rightBarButtonItem = self.refreshItem;
    self.title = @"Device Scan(Remote)";
    
    self.source = [NSMutableArray array];
    self.remoteSource = [NSMutableArray array];
    self.failSource = [NSMutableArray array];
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

#pragma mark - other

- (void)checkExistAddModelWithPeripheralUUID:(NSString *)peripheralUUID macAddress:(NSString *)macAddress address:(UInt16)address {
    BOOL tem = NO;
    NSArray *source = [NSArray arrayWithArray:self.source];
    for (AddDeviceModel *model in source) {
        if ([model.scanRspModel.uuid isEqualToString:peripheralUUID]) {
            model.scanRspModel.address = address;
            model.scanRspModel.macAddress = macAddress;
            tem = YES;
            break;
        }
    }
    if (!tem) {
        SigScanRspModel *scanModel = [SigDataSource.share getScanRspModelWithUUID:peripheralUUID];
        AddDeviceModel *model = [[AddDeviceModel alloc] init];
        if (scanModel == nil) {
            scanModel = [[SigScanRspModel alloc] init];
            scanModel.uuid = peripheralUUID;
        }
        model.scanRspModel = scanModel;
        model.scanRspModel.address = address;
        model.scanRspModel.macAddress = macAddress;
        model.state = AddDeviceModelStateScaned;
        if (![self.source containsObject:model]) {
            [self.source addObject:model];
        }
    }
}

- (AddDeviceModel *)getAddDeviceModelWithPeripheralUUID:(NSString *)peripheralUUID {
    AddDeviceModel *tem = nil;
    NSArray *source = [NSArray arrayWithArray:self.source];
    for (AddDeviceModel *model in source) {
        if ([model.scanRspModel.uuid isEqualToString:peripheralUUID]) {
            tem = model;
            break;
        }
    }
    return tem;
}

- (AddDeviceModel *)getAddDeviceModelWithMacAddress:(NSString *)macAddress {
    AddDeviceModel *tem = nil;
    NSArray *source = [NSArray arrayWithArray:self.source];
    for (AddDeviceModel *model in source) {
        if ([model.scanRspModel.macAddress isEqualToString:macAddress]) {
            tem = model;
            break;
        }
    }
    return tem;
}

@end
