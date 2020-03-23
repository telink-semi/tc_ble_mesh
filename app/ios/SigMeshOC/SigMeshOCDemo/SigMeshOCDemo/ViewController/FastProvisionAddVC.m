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
    UInt16 provisionAddress = [SigDataSource.share provisionAddress];
    if (provisionAddress == 0) {
        [self showTips:@"warning: address has run out."];
        return;
    }else if (provisionAddress > SigDataSource.share.curProvisionerModel.allocatedUnicastRange.firstObject.hightIntAddress) {
        [self showTips:@"warning: address out of range."];
        return;
    }

//    self.ble.commandHandle.responseVendorIDCallBack = nil;
    [Bluetooth.share stopAutoConnect];
    [Bluetooth.share clearCachelist];
    
    [self userAbled:NO];

    [self blockState];
    [Bluetooth.share setProvisionState];
    [Bluetooth.share startScan];
    dispatch_async(dispatch_get_main_queue(), ^{
        [self performSelector:@selector(scanTimeout) withObject:nil afterDelay:10.0];
    });
}

- (void)scanTimeout{
    [Bluetooth.share setNormalState];
    [Bluetooth.share stopScan];
    [self userAbled:YES];
    [self showTips:@"scan timeout."];
}

- (void)connectPeripheralTimeout{
    [Bluetooth.share setNormalState];
    [self userAbled:YES];
    [self showTips:@"connect timeout."];
}

- (void)userAbled:(BOOL)able{
    dispatch_async(dispatch_get_main_queue(), ^{
        self.refreshItem.enabled = able;
        self.goBackButton.enabled = able;
        [self.goBackButton setBackgroundColor:able ? kDefultColor : kDefultUnableColor];
    });
}

- (void)showTips:(NSString *)message{
    [self showAlertSureWithTitle:@"Hits" message:message sure:^(UIAlertAction *action) {
        
    }];
}

- (void)blockState{
    [super blockState];
    
    __weak typeof(self) weakSelf = self;
    [self.ble setBleScanNewDeviceCallBack:^(CBPeripheral *peripheral, BOOL provisioned) {
        if (peripheral && provisioned) {
            [weakSelf.ble stopScan];
            dispatch_async(dispatch_get_main_queue(), ^{
                [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(scanTimeout) object:nil];
                [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(connectPeripheralTimeout) object:nil];
                [weakSelf performSelector:@selector(connectPeripheralTimeout) withObject:nil afterDelay:10.0];
            });
            weakSelf.currentUUID = peripheral.identifier.UUIDString;
            [weakSelf.ble connectPeripheral:peripheral];
        }
    }];
    [self.ble bleFinishScanedCharachteristic:^(CBPeripheral *peripheral) {
        if (weakSelf.ble.state == StateAddDevice_provision && [peripheral.identifier.UUIDString isEqualToString:weakSelf.currentUUID]) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(connectPeripheralTimeout) object:nil];
            });
            [weakSelf startFastProvision];
        }
    }];
}

- (void)startFastProvision {
    __weak typeof(self) weakSelf = self;
    [self.ble.commandHandle startFastProvisionAddDevicesWithPID:SigNodePID_CT addSingleDeviceSuccessCallback:^(NSData *deviceKey, UInt16 address, UInt16 pid) {
        TeLog(@"deviceKey=%@,address=%d,pid=%d",deviceKey,address,pid);
        [weakSelf saveNodeToLocationWithDeviceKey:deviceKey address:address pid:pid];
        [weakSelf updateDeviceSuccessWithDeviceKey:deviceKey address:address pid:pid];
    } finish:^{
        TeLog(@"finish");
        [weakSelf addDeviceFinish];
    }];
}

- (void)saveNodeToLocationWithDeviceKey:(NSData *)deviceKey address:(UInt16)address pid:(UInt16)pid {
    SigNodeModel *model = [[SigNodeModel alloc] init];
    [model setAddress:address];
    model.deviceKey = [LibTools convertDataToHexStr:deviceKey];
    model.peripheralUUID = nil;
    NSData *macData = [LibTools turnOverData:[deviceKey subdataWithRange:NSMakeRange(0, 6)]];
    model.macAddress = [LibTools convertDataToHexStr:macData];
    SigAppkeyModel *appkey = [SigDataSource.share curAppkeyModel];
    SigNodeKeyModel *nodeAppkey = [[SigNodeKeyModel alloc] init];
    nodeAppkey.index = appkey.index;
    if (![model.appKeys containsObject:nodeAppkey]) {
        [model.appKeys addObject:nodeAppkey];
    }

    VC_node_info_t info = model.nodeInfo;
    info.element_cnt = [SigDataSource.share getElementCountOfPid:pid];
    info.cps.page0_head.pid = pid;
    if (pid == SigNodePID_CT || pid == SigNodePID_Panel) {
        DeviceTypeModel *typeModel = [[DeviceTypeModel alloc] initWithCID:0x0211 PID:pid];
        VC_node_info_t tem = typeModel.defultNodeInfo;
        memcpy(&tem, &info, 22);
        model.nodeInfo = tem;
    }else{
        TeLog(@"\n\n\t[warning]:pid is not SigNodePID_CT or SigNodePID_Panel,please fix api initWithCID:PID:\n\n");
    }

    [SigDataSource.share saveDeviceWithDeviceModel:model];
}

- (void)updateDeviceSuccessWithDeviceKey:(NSData *)deviceKey address:(UInt16)address pid:(UInt16)pid {
    AddDeviceModel *model = [[AddDeviceModel alloc] init];
    SigScanRspModel *scanModel = [[SigScanRspModel alloc] init];
    NSData *macData = [LibTools turnOverData:[deviceKey subdataWithRange:NSMakeRange(0, 6)]];
    scanModel.macAddress = [LibTools convertDataToHexStr:macData];
    model.scanRspModel = scanModel;
    model.scanRspModel.address = address;
    model.state = AddDeviceModelStateBindSuccess;
    if (![self.source containsObject:model]) {
        [self.source addObject:model];
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

- (void)addDeviceFinish{
    [self userAbled:YES];
}

- (IBAction)clickGoBack:(UIButton *)sender {
    __weak typeof(self) weakSelf = self;
    [self.ble.commandHandle setFilterWithLocationAddress:SigDataSource.share.curLocationNodeModel.address timeout:kSetFilterTimeout result:^(BOOL isSuccess) {
        if (isSuccess) {
            TeLog(@"setFilter success.");
        } else {
            TeLog(@"setFilter fail.");
        }
        dispatch_async(dispatch_get_main_queue(), ^{
            [weakSelf.navigationController popViewControllerAnimated:YES];
        });
    }];
}

#pragma mark - Life method
- (void)normalSetting{
    [super normalSetting];
    self.title = @"Fast provision";
    self.source = [[NSMutableArray alloc] init];

    [self.collectionView registerNib:[UINib nibWithNibName:CellIdentifiers_AddDeviceItemCellID bundle:nil] forCellWithReuseIdentifier:CellIdentifiers_AddDeviceItemCellID];
    
    self.refreshItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh target:self action:@selector(startAddDevice)];
    self.navigationItem.rightBarButtonItem = self.refreshItem;

}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
    self.navigationItem.hidesBackButton = YES;
    
    [self startAddDevice];
}

-(void)dealloc{
    TeLog(@"%s",__func__);
}

@end
