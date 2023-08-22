/********************************************************************************************************
 * @file     RemoteAddVC.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/3/25
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
@property (strong, nonatomic) NSMutableArray <SigRemoteScanRspModel *>*remoteSource;//缓存扫描到准备添加的设备
@property (strong, nonatomic) NSMutableArray <SigRemoteScanRspModel *>*failSource;
@property (strong, nonatomic) NSMutableArray <SigRemoteScanRspModel *>*scanReportSource;//缓存扫描到本轮不准备添加的设备
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
//    if (addModel.state == AddDeviceModelStateScanned || addModel.state == AddDeviceModelStateProvisionFail  || addModel.state == AddDeviceModelStateProvisioning) {
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
    NSData *key = [SigDataSource.share curNetKey];
    if (SigDataSource.share.curNetkeyModel.phase == distributingKeys) {
        if (SigDataSource.share.curNetkeyModel.oldKey) {
            key = [LibTools nsstringToHex:SigDataSource.share.curNetkeyModel.oldKey];
        }
    }

    __block UInt16 currentProvisionAddress = provisionAddress;
    __block NSString *currentAddUUID = nil;
    [SDKLibCommand startAddDeviceWithNextAddress:provisionAddress networkKey:key netkeyIndex:SigDataSource.share.curNetkeyModel.index appkeyModel:SigDataSource.share.curAppkeyModel unicastAddress:0 uuid:nil keyBindType:type.integerValue productID:0 cpsData:nil isAutoAddNextDevice:NO provisionSuccess:^(NSString * _Nonnull identify, UInt16 address) {
        if (identify && address != 0) {
            currentAddUUID = identify;
            currentProvisionAddress = address;
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
        [weakSelf showRemoteProvisionError:error];
    } keyBindSuccess:^(NSString * _Nonnull identify, UInt16 address) {
        if (identify && address != 0) {
            currentProvisionAddress = address;
            SigScanRspModel *scanRsp = [SigDataSource.share getScanRspModelWithUUID:currentAddUUID];
            NSString *mac = @"";
            if (scanRsp) {
                mac = scanRsp.macAddress;
            }
            [weakSelf updateWithPeripheralUUID:currentAddUUID macAddress:mac address:currentProvisionAddress keyBindResult:YES];
            SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
            if (node && node.isRemote) {
                [node addDefaultPublicAddressToRemote];
                [SigDataSource.share saveLocationData];
            }
            TeLogInfo(@"RP-GATT:keybind success, %@->0X%X",currentAddUUID,currentProvisionAddress);
            [weakSelf startRemoteProvisionScan];
        }
    } keyBindFail:^(NSError * _Nonnull error) {
        SigScanRspModel *scanRsp = [SigDataSource.share getScanRspModelWithUUID:currentAddUUID];
        NSString *mac = @"";
        if (scanRsp) {
            mac = scanRsp.macAddress;
        }
        [weakSelf updateWithPeripheralUUID:currentAddUUID macAddress:mac address:currentProvisionAddress keyBindResult:NO];
        TeLogInfo(@"RP-GATT:keybind fail, error:%@",error);
        [weakSelf showRemoteProvisionError:error];
    } finish:^{
        TeLogInfo(@"RP-GATT: finish.");
    }];
}

#pragma mark RP-Remote: remote scan
- (void)startRemoteProvisionScan {
    TeLogInfo(@"RP-Remote: start scan.");
    #ifdef kExist
    if (kExistRemoteProvision) {
        __weak typeof(self) weakSelf = self;
        for (SigRemoteScanRspModel *model in self.scanReportSource) {
            if (![self.remoteSource containsObject:model]) {
                [self.remoteSource addObject:model];
            }
        }
        [SigRemoteAddManager.share startRemoteProvisionScanWithReportCallback:^(SigRemoteScanRspModel * _Nonnull scanRemoteModel) {
            TeLogInfo(@"RP-Remote:reportNodeAddress=0x%x,uuid=%@,rssi=%d,oob=%d,mac=%@",scanRemoteModel.reportNodeAddress,scanRemoteModel.reportNodeUUID,scanRemoteModel.RSSI,scanRemoteModel.oob.value,scanRemoteModel.macAddress);
//            if (scanRemoteModel.reportNodeAddress != 2) {
//                TeLogInfo(@"=========return");
//                return;
//            }
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
                    TeLogInfo(@"Reomte add list:%@", weakSelf.remoteSource);
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
    #endif
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
    UInt16 provisionAddress = SigDataSource.share.provisionAddress;
    [self.scanReportSource removeObject:model];
    TeLogInfo(@"RP-Remote: start provision, uuid:%@,macAddress:%@->0x%x.",model.reportNodeUUID,model.macAddress,provisionAddress);
    #ifdef kExist
    __weak typeof(self) weakSelf = self;
    if (kExistRemoteProvision) {
        [SigRemoteAddManager.share remoteProvisionWithNextProvisionAddress:provisionAddress reportNodeAddress:model.reportNodeAddress reportNodeUUID:model.reportNodeUUID networkKey:SigDataSource.share.curNetKey netkeyIndex:SigDataSource.share.curNetkeyModel.index provisionType:ProvisionType_NoOOB staticOOBData:nil provisionSuccess:^(NSString * _Nonnull identify, UInt16 address) {
            [weakSelf updateWithPeripheralUUID:[LibTools convertDataToHexStr:model.reportNodeUUID] macAddress:model.macAddress address:provisionAddress provisionResult:YES];
                TeLogInfo(@"RP-Remote:provision success, %@->0X%X",identify,address);
            SigNodeModel *node = [SigDataSource.share getNodeWithAddress:provisionAddress];
            if (node) {
                __block BOOL hasStartKeyBind = NO;
                [SDKLibCommand remoteProvisioningLinkCloseWithDestination:model.reportNodeAddress reason:SigRemoteProvisioningLinkCloseStatus_success retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigRemoteProvisioningLinkStatus * _Nonnull responseMessage) {

                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    if (!hasStartKeyBind) {
                        hasStartKeyBind = YES;
                        if (error != nil && isResponseAll == NO) {
                            TeLogError(@"link close fail.");
                        }
                        dispatch_async(dispatch_get_main_queue(), ^{
                            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(keyBindWithNode:) object:node];
                            [weakSelf performSelector:@selector(keyBindWithNode:) withObject:node afterDelay:2.0];
                        });
                    }
                }];
            } else {
                TeLogError(@"node = nil.");
            }
        } fail:^(NSError * _Nonnull error) {
            TeLogDebug(@"RP-Remote:provision fail.");
            if (![weakSelf.failSource containsObject:model]) {
                [weakSelf.failSource addObject:model];
            }
            [weakSelf remoteAddSingleDeviceFinish];
            [weakSelf updateWithPeripheralUUID:[LibTools convertDataToHexStr:model.reportNodeUUID] macAddress:model.macAddress address:provisionAddress provisionResult:NO];

            if (SigBearer.share.isOpen) {
                [SDKLibCommand remoteProvisioningLinkCloseWithDestination:model.reportNodeAddress reason:SigRemoteProvisioningLinkCloseStatus_fail retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigRemoteProvisioningLinkStatus * _Nonnull responseMessage) {

                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    if (error != nil && isResponseAll == NO) {
                        TeLogError(@"link close fail.");
                    }
                    TeLogInfo(@"RP-Remote:provision fail, error=%@",error);
                    [weakSelf addNodeByRemoteProvision];
                }];
            } else {
                [weakSelf addNodeByRemoteProvision];
            }
        }];
    }
    #endif
}

#pragma mark RP-Remote: start keybind
- (void)keyBindWithNode:(SigNodeModel *)node {
    TeLogInfo(@"RP-Remote: start keybind");
    [SigMeshLib.share cleanAllCommandsAndRetry];
    __weak typeof(self) weakSelf = self;
    if (SigBearer.share.isOpen) {
        AddDeviceModel *model = [self getAddDeviceModelWithMacAddress:node.macAddress];
        NSNumber *type = [[NSUserDefaults standardUserDefaults] valueForKey:kKeyBindType];
        UInt8 keyBindType = type.integerValue;
        UInt16 productID = [LibTools uint16From16String:node.pid];
        DeviceTypeModel *deviceType = [SigDataSource.share getNodeInfoWithCID:kCompanyID PID:productID];
        NSData *cpsData = deviceType.defaultCompositionData.parameters;
        if (keyBindType == KeyBindType_Fast) {
            if (cpsData == nil || cpsData.length == 0) {
                keyBindType = KeyBindType_Normal;
            }
        }
        if (cpsData && cpsData.length > 0) {
            cpsData = [cpsData subdataWithRange:NSMakeRange(1, cpsData.length - 1)];
        }

        [SDKLibCommand keyBind:node.address appkeyModel:SigDataSource.share.curAppkeyModel keyBindType:keyBindType productID:productID cpsData:cpsData keyBindSuccess:^(NSString * _Nonnull identify, UInt16 address) {
            [weakSelf remoteAddSingleDeviceFinish];
            [weakSelf updateWithPeripheralUUID:model.scanRspModel.uuid macAddress:model.scanRspModel.macAddress address:node.address keyBindResult:YES];
            SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
            if (node && node.isRemote) {
                [node addDefaultPublicAddressToRemote];
                [SigDataSource.share saveLocationData];
            }
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
        __weak typeof(self) weakSelf = self;
        //GATT add
        [SDKLibCommand stopMeshConnectWithComplete:^(BOOL successful) {
            [weakSelf addOneNodeInGATT];
        }];
    }
}

- (IBAction)clickGoBack:(UIButton *)sender {
    [self.navigationController popViewControllerAnimated:YES];
}

#pragma mark - UI refresh

- (void)addAndShowSigRemoteScanRspModelToUI:(SigRemoteScanRspModel *)scanRemoteModel {
    //=================test==================//
//    if (![scanRemoteModel.macAddress hasPrefix:@"66666666"]) {
//        return;
//    }
    //=================test==================//
    if (![self.remoteSource containsObject:scanRemoteModel]) {
        if (![self.failSource containsObject:scanRemoteModel]) {
            [self.remoteSource addObject:scanRemoteModel];
            if (![self.scanReportSource containsObject:scanRemoteModel]) {
                [self.scanReportSource addObject:scanRemoteModel];
            }
        } else {
            return;//provision失败，重新扫描到，不可显示为scaned状态。
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
    [self updateUIOfScanResponseWithPeripheralUUID:[LibTools convertDataToHexStr:scanRemoteModel.reportNodeUUID] macAddress:scanRemoteModel.macAddress address:0x0000];
}

- (void)updateUIOfScanResponseWithPeripheralUUID:(NSString *)peripheralUUID macAddress:(NSString *)macAddress address:(UInt16)address {
    [self checkExistAddModelWithPeripheralUUID:peripheralUUID macAddress:macAddress address:address];
    AddDeviceModel *model = [self getAddDeviceModelWithPeripheralUUID:peripheralUUID];
    model.state = AddDeviceModelStateScanned;
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.collectionView reloadData];
    });
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
    [SDKLibCommand startMeshConnectWithComplete:nil];
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
    self.scanReportSource = [NSMutableArray array];
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
        model.state = AddDeviceModelStateScanned;
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
