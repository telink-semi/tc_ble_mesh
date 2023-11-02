/********************************************************************************************************
 * @file     AutoAddDeviceVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/9/28
 *
 * @par     Copyright (c) [2023], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "AutoAddDeviceVC.h"
#import "AddDeviceItemCell.h"

@interface AutoAddDeviceVC ()<UICollectionViewDelegate,UICollectionViewDataSource>
@property (weak, nonatomic) IBOutlet UICollectionView *collectionView;
@property (weak, nonatomic) IBOutlet UIButton *goBackButton;
@property (strong, nonatomic) NSMutableArray <AddDeviceModel *>*source;
@property (strong, nonatomic) UIBarButtonItem *refreshItem;
@end

@implementation AutoAddDeviceVC

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
    TeLogVerbose(@"");
    self.refreshItem.enabled = NO;
    self.goBackButton.enabled = NO;
    [self.goBackButton setBackgroundColor:[UIColor lightGrayColor]];
    NSData *key = [SigDataSource.share curNetKey];
    if (SigDataSource.share.curNetkeyModel.phase == distributingKeys) {
        if (SigDataSource.share.curNetkeyModel.oldKey) {
            key = [LibTools nsstringToHex:SigDataSource.share.curNetkeyModel.oldKey];
        }
    }
    UInt16 provisionAddress = [SigDataSource.share provisionAddress];

    if (provisionAddress == 0) {
        TeLogDebug(@"warning: address has run out.");
        return;
    }
    
    __weak typeof(self) weakSelf = self;
    NSNumber *type = [[NSUserDefaults standardUserDefaults] valueForKey:kKeyBindType];
    [SDKLibCommand stopMeshConnectWithComplete:^(BOOL successful) {
        TeLogVerbose(@"successful=%d",successful);
        if (successful) {
            TeLogInfo(@"stop mesh success.");
            __block UInt16 currentProvisionAddress = provisionAddress;
            __block NSString *currentAddUUID = nil;
            //旧接口，没有startConnect和startProvision。
//            [SDKLibCommand startAddDeviceWithNextAddress:provisionAddress networkKey:key netkeyIndex:SigDataSource.share.curNetkeyModel.index appkeyModel:SigDataSource.share.curAppkeyModel unicastAddress:0 uuid:nil keyBindType:type.integerValue productID:0 cpsData:nil isAutoAddNextDevice:YES provisionSuccess:^(NSString * _Nonnull identify, UInt16 address) {
//                if (identify && address != 0) {
//                    currentAddUUID = identify;
//                    currentProvisionAddress = address;
//                    [weakSelf refreshUIOfProvisionSuccessWithUUID:identify address:address];
//                    TeLogInfo(@"addDevice_provision success : %@->0X%X",identify,address);
//                }
//            } provisionFail:^(NSError * _Nonnull error) {
//                [weakSelf refreshUIOfProvisionFailWithUUID:SigBearer.share.getCurrentPeripheral.identifier.UUIDString];
//                TeLogInfo(@"addDevice provision fail error:%@",error);
//            } keyBindSuccess:^(NSString * _Nonnull identify, UInt16 address) {
//                if (identify && address != 0) {
//                    currentProvisionAddress = address;
//                    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
//                    if (node && node.isRemote) {
//                        [node addDefaultPublicAddressToRemote];
//                        [SigDataSource.share saveLocationData];
//                    }
//                    [weakSelf refreshUIOfKeyBindSuccessWithUUID:identify address:address];
//                    TeLogInfo(@"addDevice_provision success : %@->0X%X",identify,address);
//                }
//            } keyBindFail:^(NSError * _Nonnull error) {
//                [weakSelf refreshUIOfKeyBindFailWithUUID:currentAddUUID address:currentProvisionAddress];
//                TeLogInfo(@"addDevice keybind fail error:%@",error);
//            } finish:^{
//                TeLogInfo(@"addDevice finish.");
//                [SDKLibCommand startMeshConnectWithComplete:nil];
//                dispatch_async(dispatch_get_main_queue(), ^{
//                    [weakSelf addDeviceFinish];
//                });
//            }];
            //v3.3.3.6及之后版本，新增startConnect和startProvision。
            [SDKLibCommand startAddDeviceWithNextAddress:provisionAddress networkKey:key netkeyIndex:SigDataSource.share.curNetkeyModel.index appkeyModel:SigDataSource.share.curAppkeyModel unicastAddress:0 uuid:nil keyBindType:type.integerValue productID:0 cpsData:nil isAutoAddNextDevice:YES startConnect:^(NSString * _Nonnull identify) {
                [weakSelf refreshUIOfStartConnectWithUUID:identify];
            } startProvision:^(NSString * _Nonnull identify, UInt16 address) {
                [weakSelf refreshUIOfStartProvisionWithUUID:identify address:address];
            } provisionSuccess:^(NSString * _Nonnull identify, UInt16 address) {
                if (identify && address != 0) {
                    currentAddUUID = identify;
                    currentProvisionAddress = address;
                    [weakSelf refreshUIOfProvisionSuccessWithUUID:identify address:address];
                    TeLogInfo(@"addDevice_provision success : %@->0X%X",identify,address);
                }
            } provisionFail:^(NSError * _Nullable error) {
                [weakSelf refreshUIOfProvisionFailWithUUID:SigBearer.share.getCurrentPeripheral.identifier.UUIDString];
                TeLogInfo(@"addDevice provision fail error:%@",error);
            } keyBindSuccess:^(NSString * _Nonnull identify, UInt16 address) {
                if (identify && address != 0) {
                    currentProvisionAddress = address;
                    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
                    if (node && node.isRemote) {
                        [node addDefaultPublicAddressToRemote];
                        [SigDataSource.share saveLocationData];
                    }
                    [weakSelf refreshUIOfKeyBindSuccessWithUUID:identify address:address];
                    TeLogInfo(@"addDevice_provision success : %@->0X%X",identify,address);
                }
            } keyBindFail:^(NSError * _Nullable error) {
                [weakSelf refreshUIOfKeyBindFailWithUUID:currentAddUUID address:currentProvisionAddress];
                TeLogInfo(@"addDevice keybind fail error:%@",error);
            } finish:^{
                TeLogInfo(@"addDevice finish.");
                [SDKLibCommand startMeshConnectWithComplete:nil];
                dispatch_async(dispatch_get_main_queue(), ^{
                    [weakSelf addDeviceFinish];
                });
            }];
        } else {
            TeLogInfo(@"stop mesh fail.");
        }
    }];
}

- (void)scrollToBottom{
    NSInteger item = [self.collectionView numberOfItemsInSection:0] - 1;
    NSIndexPath *lastItemIndex = [NSIndexPath indexPathForItem:item inSection:0];
    [self.collectionView scrollToItemAtIndexPath:lastItemIndex atScrollPosition:UICollectionViewScrollPositionTop animated:NO];
}

- (AddDeviceModel *)getAddDeviceModelWithUUID:(NSString *)uuid {
    AddDeviceModel *tem = nil;
    NSArray *source = [NSArray arrayWithArray:self.source];
    for (AddDeviceModel *model in source) {
        if ([model.scanRspModel.uuid isEqualToString:uuid]) {
            tem = model;
            break;
        }
    }
    if (tem == nil) {
        tem = [[AddDeviceModel alloc] init];
        [self.source addObject:tem];
    }
    SigScanRspModel *scanModel = [SigDataSource.share getScanRspModelWithUUID:uuid];
    if (scanModel == nil) {
        scanModel = [[SigScanRspModel alloc] init];
        scanModel.uuid = uuid;
    }
    tem.scanRspModel = scanModel;
    return tem;
}

- (void)refreshUIOfStartConnectWithUUID:(NSString *)uuid {
    AddDeviceModel *model = [self getAddDeviceModelWithUUID:uuid];
    model.scanRspModel.address = 0;
    model.state = AddDeviceModelStateConnecting;
    [self refreshCollectionView];
}

- (void)refreshUIOfStartProvisionWithUUID:(NSString *)uuid address:(UInt16)address {
    AddDeviceModel *model = [self getAddDeviceModelWithUUID:uuid];
    model.scanRspModel.address = address;
    model.state = AddDeviceModelStateProvisioning;
    [self refreshCollectionView];
}

- (void)refreshUIOfProvisionSuccessWithUUID:(NSString *)uuid address:(UInt16)address {
    AddDeviceModel *model = [self getAddDeviceModelWithUUID:uuid];
    model.scanRspModel.address = address;
    model.state = AddDeviceModelStateBinding;
    [self refreshCollectionView];
}

- (void)refreshUIOfProvisionFailWithUUID:(NSString *)uuid {
    AddDeviceModel *model = [self getAddDeviceModelWithUUID:uuid];
    model.state = AddDeviceModelStateProvisionFail;
    [self refreshCollectionView];
}

- (void)refreshUIOfKeyBindSuccessWithUUID:(NSString *)uuid address:(UInt16)address {
    AddDeviceModel *model = [self getAddDeviceModelWithUUID:uuid];
    model.scanRspModel.address = address;
    model.state = AddDeviceModelStateBindSuccess;
    [self refreshCollectionView];
}

- (void)refreshUIOfKeyBindFailWithUUID:(NSString *)uuid address:(UInt16)address {
    AddDeviceModel *model = [self getAddDeviceModelWithUUID:uuid];
    model.scanRspModel.address = address;
    model.state = AddDeviceModelStateBindFail;
    [self refreshCollectionView];
}

- (void)refreshCollectionView {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.collectionView reloadData];
        [self scrollToBottom];
    });
}

- (void)addDeviceFinish{
    self.refreshItem.enabled = YES;
    self.goBackButton.enabled = YES;
    [self.goBackButton setBackgroundColor:[UIColor telinkBlue]];
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
    self.title = @"Device Scan(Auto)";
    
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
