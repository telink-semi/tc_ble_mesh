/********************************************************************************************************
 * @file     AddDeviceViewController.m 
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
//  AddDeviceViewController.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2018/10/17.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "AddDeviceViewController.h"
#import "AddDeviceItemCell.h"

@interface AddDeviceViewController ()<UICollectionViewDelegate,UICollectionViewDataSource>
@property (weak, nonatomic) IBOutlet UICollectionView *collectionView;
@property (weak, nonatomic) IBOutlet UIButton *goBackButton;
@property (strong, nonatomic) NSMutableArray <AddDeviceModel *>*source;
@property (strong, nonatomic) UIBarButtonItem *refreshItem;
@end

@implementation AddDeviceViewController

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
//    self.ble.commandHandle.responseVendorIDCallBack = nil;
//    [Bluetooth.share stopAutoConnect];
//    [Bluetooth.share cancelAllConnecttionWithComplete:nil];
//    [Bluetooth.share clearCachelist];
    
    self.refreshItem.enabled = NO;
    self.goBackButton.enabled = NO;
    [self.goBackButton setBackgroundColor:[UIColor lightGrayColor]];
    NSData *key = [SigDataSource.share curNetKey];
    UInt16 provisionAddress = [SigDataSource.share provisionAddress];

    if (provisionAddress == 0) {
        TeLogDebug(@"warning: address has run out.");
        return;
    }
    
    __weak typeof(self) weakSelf = self;
    NSNumber *type = [[NSUserDefaults standardUserDefaults] valueForKey:kKeyBindType];

    [SigBearer.share stopMeshConnectWithComplete:^(BOOL successful) {
        if (successful) {
            TeLogDebug(@"stop mesh success.");
            [SDKLibCommand startAddDeviceWithNextAddress:provisionAddress networkKey:key netkeyIndex:SigDataSource.share.curNetkeyModel.index appkeyModel:SigDataSource.share.curAppkeyModel unicastAddress:0 uuid:nil keyBindType:type.integerValue productID:0 cpsData:nil isAutoAddNextDevice:YES provisionSuccess:^(NSString * _Nonnull identify, UInt16 address) {
                if (identify && address != 0) {
                    
                    [weakSelf updateDeviceProvisionSuccess:identify address:address];
                    TeLogDebug(@"addDevice_provision success : %@->0X%X",identify,address);
                }
            } provisionFail:^(NSError * _Nonnull error) {
                [weakSelf updateDeviceProvisionFail:SigBearer.share.getCurrentPeripheral.identifier.UUIDString];
                TeLogDebug(@"addDevice provision fail error:%@",error);
            } keyBindSuccess:^(NSString * _Nonnull identify, UInt16 address) {
                if (identify && address != 0) {
                    [weakSelf updateDeviceKeyBind:identify isSuccess:YES];
                    TeLogDebug(@"addDevice_provision success : %@->0X%X",identify,address);
                }
            } keyBindFail:^(NSError * _Nonnull error) {
                [weakSelf updateDeviceKeyBind:SigBearer.share.getCurrentPeripheral.identifier.UUIDString isSuccess:NO];
                TeLogDebug(@"addDevice keybind fail error:%@",error);
            } finish:^{
                TeLogDebug(@"addDevice finish.");
                [weakSelf performSelectorOnMainThread:@selector(addDeviceFinish) withObject:nil waitUntilDone:YES];
            }];
        } else {
            TeLogDebug(@"stop mesh fail.");
            
        }
    }];    
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

- (void)updateDeviceProvisionFail:(NSString *)uuid {
    SigScanRspModel *scanModel = [SigDataSource.share getScanRspModelWithUUID:uuid];
    AddDeviceModel *model = [[AddDeviceModel alloc] init];
    if (scanModel == nil) {
        scanModel = [[SigScanRspModel alloc] init];
        scanModel.uuid = uuid;
    }
    model.scanRspModel = scanModel;
//    model.scanRspModel.address = address;
    model.state = AddDeviceModelStateProvisionFail;
    if (![self.source containsObject:model]) {
        [self.source addObject:model];
    }
    [self.collectionView reloadData];
    [self scrollowToBottom];
}

- (void)updateDeviceKeyBind:(NSString *)uuid isSuccess:(BOOL)isSuccess{
    for (AddDeviceModel *model in self.source) {
        if ([model.scanRspModel.uuid isEqualToString:uuid]) {
            if (isSuccess) {
                model.state = AddDeviceModelStateBindSuccess;
            } else {
                model.state = AddDeviceModelStateBindFail;
            }
            break;
        }
    }
    [self.collectionView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
}

- (void)addDeviceFinish{
    self.refreshItem.enabled = YES;
    self.goBackButton.enabled = YES;
    [self.goBackButton setBackgroundColor:kDefultColor];
}

- (IBAction)clickGoBack:(UIButton *)sender {
    [self.navigationController popViewControllerAnimated:YES];
}

#pragma mark - Life method
- (void)normalSetting{
    [super normalSetting];
    
    [self.collectionView registerNib:[UINib nibWithNibName:CellIdentifiers_AddDeviceItemCellID bundle:nil] forCellWithReuseIdentifier:CellIdentifiers_AddDeviceItemCellID];
    
    self.refreshItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh target:self action:@selector(startAddDevice)];
    self.navigationItem.rightBarButtonItem = self.refreshItem;
    self.title = @"Device Scan";
    
    self.source = [[NSMutableArray alloc] init];
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
