/********************************************************************************************************
 * @file     AddDeviceVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/8/22
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

#import "AddDeviceVC.h"
#import "AddDeviceCell.h"
#import "UIButton+extension.h"

typedef void(^resultHandle)(NSError  * _Nullable error);

@interface AddDeviceVC ()<UITableViewDelegate,UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIButton *addAllButton;
@property (strong, nonatomic) UIBarButtonItem *rightItem;
@property (strong, nonatomic) UIBarButtonItem *leftItem;
@property (strong, nonatomic) NSMutableArray <AddDeviceModel *>*source;
@property (assign, nonatomic) BOOL userEnable;
@end

@implementation AddDeviceVC

#pragma mark - Life method

- (void)normalSetting{
    [super normalSetting];
    self.title = @"Device Scan";
    self.source = [[NSMutableArray alloc] init];

    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_AddDeviceCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_AddDeviceCellID];
    UIView *footerView = [[UIView alloc] initWithFrame:CGRectZero];
    self.tableView.tableFooterView = footerView;
    self.tableView.estimatedRowHeight = 50.0;
    self.tableView.allowsSelection = NO;

    self.rightItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh target:self action:@selector(clickScanButton)];
    self.navigationItem.rightBarButtonItem = self.rightItem;
    self.leftItem = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"back"] style:UIBarButtonItemStylePlain target:self action:@selector(clickBackButton)];
    self.navigationItem.leftBarButtonItem = self.leftItem;

    [self clickScanButton];
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
    self.navigationItem.hidesBackButton = YES;
}

-(void)dealloc{
    TelinkLogDebug(@"%s",__func__);
}

#pragma mark - UITableViewDelegate,UITableViewDataSource

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifiers_AddDeviceCellID forIndexPath:indexPath];
    [self configureCell:cell forRowAtIndexPath:indexPath];
    return cell;
}

- (void)configureCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath {
    AddDeviceCell *itemCell = (AddDeviceCell *)cell;
    AddDeviceModel *model = self.source[indexPath.row];
    [itemCell updateContent:model];
    itemCell.closeButton.hidden = !self.userEnable;
    itemCell.addButton.hidden = !self.userEnable;
    if (model.state == AddDeviceModelStateBindSuccess) {
        itemCell.closeButton.hidden = YES;
        itemCell.addButton.hidden = YES;
    }
    __weak typeof(self) weakSelf = self;
    [itemCell.closeButton addAction:^(UIButton *button) {
        [weakSelf.source removeObject:model];
        [weakSelf refreshTableView];
    }];
    [itemCell.addButton addAction:^(UIButton *button) {
        weakSelf.userEnable = NO;
        [weakSelf refreshTableView];
        [weakSelf addModel:model resultBlock:^(NSError * _Nullable error) {
            weakSelf.userEnable = YES;
            [weakSelf refreshTableView];
        }];
    }];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.source.count;
}

#pragma mark - Event

- (void)clickScanButton {
    self.userEnable = NO;
    self.source = [NSMutableArray array];
    [self.tableView reloadData];

    __weak typeof(self) weakSelf = self;
    [SDKLibCommand stopMeshConnectWithComplete:^(BOOL successful) {
        if (successful) {
            TelinkLogDebug(@"close success.");
            [SDKLibCommand scanUnprovisionedDevicesWithResult:^(CBPeripheral * _Nonnull peripheral, NSDictionary<NSString *,id> * _Nonnull advertisementData, NSNumber * _Nonnull RSSI, BOOL unprovisioned) {
                if (unprovisioned) {
                    SigScanRspModel *m = [SigDataSource.share getScanRspModelWithUUID:peripheral.identifier.UUIDString];
                    TelinkLogInfo(@"==========peripheral=%@,advertisementData=%@,RSSI=%@,unprovisioned=%d,advUuid=%@,macAddress=%@,calcUuid=%@",peripheral,advertisementData,RSSI,unprovisioned,m.advUuid,m.macAddress,[LibTools convertDataToHexStr:[LibTools calcUuidByMac:[LibTools nsstringToHex:m.macAddress]]]);
                    AddDeviceModel *model = [[AddDeviceModel alloc] init];
                    model.scanRspModel = m;
                    model.state = AddDeviceModelStateScanned;
                    if (![weakSelf.source containsObject:model]) {
                        [weakSelf.source addObject:model];
                    } else {
                        NSInteger index = [weakSelf.source indexOfObject:model];
                        [weakSelf.source replaceObjectAtIndex:index withObject:model];
                    }
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [weakSelf.tableView reloadData];
                        [weakSelf scrollToBottom];
                    });
                }
            }];
            dispatch_async(dispatch_get_main_queue(), ^{
                [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(scanFinish) object:nil];
                [weakSelf performSelector:@selector(scanFinish) withObject:nil afterDelay:5.0];
            });
        } else {
            TelinkLogDebug(@"close fail.");
            weakSelf.userEnable = YES;
            [weakSelf refreshTableView];
        }
    }];
}

- (void)clickBackButton {
    //修复添加设备完成后返回首页断开直连设备不会回连的bug.
    //Fix the bug where disconnecting a directly connected device after adding it and returning to the homepage will not cause it to reconnect.
    if (SigBearer.share.isOpen) {
        [SDKLibCommand startMeshConnectWithComplete:nil];
    }
    [self.navigationController popViewControllerAnimated:YES];
}

- (IBAction)clickAddAllButton:(UIButton *)sender {
    self.userEnable = NO;
    [self.tableView reloadData];
    NSMutableArray *mArray = [NSMutableArray array];
    NSArray *array = [NSArray arrayWithArray:self.source];
    //只添加AddDeviceModelStateScanned状态的设备
    for (AddDeviceModel *model in array) {
        if (model.state == AddDeviceModelStateScanned) {
            [mArray addObject:model];
        }
    }
    if (mArray.count > 0) {
        __weak typeof(self) weakSelf = self;
        NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
        [operationQueue addOperationWithBlock:^{
            //这个block语句块在子线程中执行
            dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
            for (AddDeviceModel *model in mArray) {
                [weakSelf addModel:model resultBlock:^(NSError * _Nullable error) {
                    dispatch_semaphore_signal(semaphore);
                }];
                dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
            }
            weakSelf.userEnable = YES;
        }];
    } else {
        self.userEnable = YES;
        [self showTips:@"There are no device that state is scanned."];
    }
}

#pragma mark call API of addDevice in SigMeshLib.a

- (void)addModel:(AddDeviceModel *)model resultBlock:(resultHandle)resultBlock {
    NSData *key = [SigDataSource.share curNetKey];
    if (SigDataSource.share.curNetkeyModel.phase == distributingKeys) {
        if (SigDataSource.share.curNetkeyModel.oldKey) {
            key = [LibTools nsstringToHex:SigDataSource.share.curNetkeyModel.oldKey];
        }
    }

    __weak typeof(self) weakSelf = self;
    [SDKLibCommand stopMeshConnectWithComplete:^(BOOL successful) {
        if (successful) {
            CBPeripheral *peripheral = model.scanRspModel.peripheral;
            NSNumber *type = [[NSUserDefaults standardUserDefaults] valueForKey:kKeyBindType];
            KeyBindType keyBindType = type.integerValue;
            model.state = AddDeviceModelStateConnecting;
            [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
            //选择添加新增逻辑：判断本地是否存在该UUID的OOB数据，存在则使用static OOB添加，不存在则使用no OOB添加。
            SigOOBModel *oobModel = [SigDataSource.share getSigOOBModelWithUUID:model.scanRspModel.advUuid];
            NSData *staticOOBData = nil;
            if (oobModel && oobModel.OOBString && (oobModel.OOBString.length == 32 || oobModel.OOBString.length == 64)) {
                staticOOBData = [LibTools nsstringToHex:oobModel.OOBString];
            }
            NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
            [operationQueue addOperationWithBlock:^{
                dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
                __block NSError *error = nil;
                if (model.scanRspModel.advOobInformation.supportForCertificateBasedProvisioning) {
                    [SDKLibCommand startCertificateBasedWithNetworkKey:key netkeyIndex:SigDataSource.share.curNetkeyModel.index appkeyModel:SigDataSource.share.curAppkeyModel peripheral:peripheral staticOOBData:staticOOBData keyBindType:keyBindType productID:0 cpsData:nil startConnect:^(NSString * _Nonnull identify) {
                        model.state = AddDeviceModelStateConnecting;
                        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                    } startProvision:^(NSString * _Nonnull identify, UInt16 address) {
                        model.state = AddDeviceModelStateProvisioning;
                        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                    } capabilitiesResponse:^UInt16(SigProvisioningCapabilitiesPdu * _Nonnull capabilitiesPdu) {
                        UInt16 unicastAddress = [SigDataSource.share getProvisionAddressWithElementCount:capabilitiesPdu.numberOfElements];
                        model.state = AddDeviceModelStateProvisioning;
                        model.scanRspModel.address = unicastAddress;
                        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                        return unicastAddress;
                    } provisionSuccess:^(NSString * _Nonnull identify, UInt16 address) {
                        model.state = AddDeviceModelStateBinding;
                        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                    } provisionFail:^(NSError * _Nullable error) {
                        model.state = AddDeviceModelStateProvisionFail;
                        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                        error = [NSError errorWithDomain:@"provision fail." code:-1 userInfo:nil];
                        dispatch_semaphore_signal(semaphore);
                    } keyBindSuccess:^(NSString * _Nonnull identify, UInt16 address) {
                        SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
                        if (node && node.isRemote) {
                            [node addDefaultPublicAddressToRemote];
                            [SigDataSource.share saveLocationData];
                        }
                        model.state = AddDeviceModelStateBindSuccess;
                        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                        dispatch_semaphore_signal(semaphore);
                    } keyBindFail:^(NSError * _Nullable error) {
                        model.state = AddDeviceModelStateBindFail;
                        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                        error = [NSError errorWithDomain:@"keybind fail." code:-1 userInfo:nil];
                        dispatch_semaphore_signal(semaphore);
                    }];
                } else {
                    SigAddConfigModel *config = [[SigAddConfigModel alloc] initWithCBPeripheral:peripheral networkKey:key netkeyIndex:SigDataSource.share.curNetkeyModel.index appKey:SigDataSource.share.curAppKey appkeyIndex:SigDataSource.share.curAppkeyModel.index staticOOBData:staticOOBData keyBindType:type.integerValue productID:0 cpsData:nil];
                    [config setStartConnectBlock:^(NSString * _Nonnull identify) {
                        model.state = AddDeviceModelStateConnecting;
                        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                    }];
                    [config setStartProvisionBlock:^(NSString * _Nonnull identify, UInt16 address) {
                        model.state = AddDeviceModelStateProvisioning;
                        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                    }];
                    [SDKLibCommand startAddDeviceWithSigAddConfigModel:config capabilitiesResponse:^UInt16(SigProvisioningCapabilitiesPdu * _Nonnull capabilitiesPdu) {
                        UInt16 unicastAddress = [SigDataSource.share getProvisionAddressWithElementCount:capabilitiesPdu.numberOfElements];
                        model.state = AddDeviceModelStateProvisioning;
                        model.scanRspModel.address = unicastAddress;
                        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                        return unicastAddress;
                    } provisionSuccess:^(NSString * _Nonnull identify, UInt16 address) {
                        model.state = AddDeviceModelStateBinding;
                        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                    } provisionFail:^(NSError * _Nullable error) {
                        model.state = AddDeviceModelStateProvisionFail;
                        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                        error = [NSError errorWithDomain:@"provision fail." code:-1 userInfo:nil];
                        dispatch_semaphore_signal(semaphore);
                    } keyBindSuccess:^(NSString * _Nonnull identify, UInt16 address) {
                        SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
                        if (node && node.isRemote) {
                            [node addDefaultPublicAddressToRemote];
                            [SigDataSource.share saveLocationData];
                        }
                        model.state = AddDeviceModelStateBindSuccess;
                        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                        dispatch_semaphore_signal(semaphore);
                    } keyBindFail:^(NSError * _Nullable error) {
                        model.state = AddDeviceModelStateBindFail;
                        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                        error = [NSError errorWithDomain:@"keybind fail." code:-1 userInfo:nil];
                        dispatch_semaphore_signal(semaphore);
                    }];
                }
                dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
                if (resultBlock) {
                    resultBlock(error);
                }
            }];
        }else{
            TelinkLogDebug(@"stop mesh fail.");
            [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
            if (resultBlock) {
                NSError *error = [NSError errorWithDomain:@"stop mesh fail." code:-1 userInfo:nil];
                resultBlock(error);
            }
        }
    }];
}

#pragma mark - other

- (void)scrollToBottom {
    if (self.source.count > 0) {
        NSInteger item = [self.tableView numberOfRowsInSection:0] - 1;
        NSIndexPath *lastItemIndex = [NSIndexPath indexPathForRow:item inSection:0];
        [self.tableView scrollToRowAtIndexPath:lastItemIndex atScrollPosition:UITableViewScrollPositionTop animated:YES];
    }
}

- (void)setUserEnable:(BOOL)userEnable {
    _userEnable = userEnable;
    dispatch_async(dispatch_get_main_queue(), ^{
        self.navigationItem.rightBarButtonItem = userEnable ? self.rightItem : nil;
        self.navigationItem.leftBarButtonItem = userEnable ? self.leftItem : nil;
        self.addAllButton.enabled = userEnable;
        [self.addAllButton setBackgroundColor:userEnable ? UIColor.telinkButtonBlue : UIColor.telinkButtonUnableBlue];
    });
}

- (void)scanFinish {
    [SDKLibCommand stopScan];
    self.userEnable = YES;
    [self refreshTableView];
}

- (void)refreshTableView {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.tableView reloadData];
    });
}

@end
