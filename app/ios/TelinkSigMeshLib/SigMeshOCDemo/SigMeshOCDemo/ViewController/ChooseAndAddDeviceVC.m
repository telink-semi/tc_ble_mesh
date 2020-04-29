/********************************************************************************************************
 * @file     ChooseAndAddDeviceVC.m
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
//  ChooseAndAddDeviceVC.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2019/7/4.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import "ChooseAndAddDeviceVC.h"
#import "MeshOTAItemCell.h"
#import "UIViewController+Message.h"
#import "UIButton+extension.h"

typedef enum : NSUInteger {
    AddStateScan,
    AddStateProvisioning,
    AddStateProvisionFail,
    AddStateKeybinding,
    AddStateKeybound,
    AddStateUnbound,
} AddState;
@interface AddDeviceStateModel : NSObject
@property (nonatomic,strong) CBPeripheral *peripheral;
@property (nonatomic,assign) AddState state;
@end

@implementation AddDeviceStateModel
- (BOOL)isEqual:(id)object{
    if ([object isKindOfClass:[AddDeviceStateModel class]]) {
        return [_peripheral.identifier.UUIDString isEqualToString:((AddDeviceStateModel *)object).peripheral.identifier.UUIDString];
    } else {
        return NO;
    }
}
@end

@interface ChooseAndAddDeviceVC ()<UITableViewDelegate,UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIButton *addButton;
@property (weak, nonatomic) IBOutlet UIButton *scanButton;
@property (nonatomic,strong) NSMutableArray <AddDeviceStateModel *>*allDevices;
@property (nonatomic, strong) NSMutableArray <AddDeviceStateModel *>*selectDevices;

@end

@implementation ChooseAndAddDeviceVC

- (IBAction)clickScan:(UIButton *)sender {
    self.allDevices = [NSMutableArray array];
    self.selectDevices = [NSMutableArray array];
    [self.tableView reloadData];
    [self userAbled:NO];

    __weak typeof(self) weakSelf = self;
    [SigBearer.share stopMeshConnectWithComplete:^(BOOL successful) {
        if (successful) {
            TeLogDebug(@"close success.");
            [SigBluetooth.share scanUnprovisionedDevicesWithResult:^(CBPeripheral * _Nonnull peripheral, NSDictionary<NSString *,id> * _Nonnull advertisementData, NSNumber * _Nonnull RSSI, BOOL unprovisioned) {
                if (unprovisioned) {
                    AddDeviceStateModel *model = [[AddDeviceStateModel alloc] init];
                    model.peripheral = peripheral;
                    model.state = AddStateScan;
                    if (![weakSelf.allDevices containsObject:model]) {
                        [weakSelf.allDevices addObject:model];
                        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                    }
                }
            }];
            dispatch_async(dispatch_get_main_queue(), ^{
                [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(scanFinish) object:nil];
                [weakSelf performSelector:@selector(scanFinish) withObject:nil afterDelay:5.0];
            });
        } else {
            TeLogDebug(@"close fail.");
            [weakSelf userAbled:YES];
        }
    }];
}

- (IBAction)clickAddDevice:(UIButton *)sender {
    if (self.allDevices.count == 0) {
        [self showTips:@"please scan device!"];
        return;
    }
    NSMutableArray *unprovisionList = [NSMutableArray array];
    for (AddDeviceStateModel *model in self.selectDevices) {
        if (model.state == AddStateScan) {
            [unprovisionList addObject:model];
        }
    }
    if (unprovisionList.count == 0) {
        [self showTips:@"please choose unprovision device!"];
        return;
    }
    self.selectDevices = [NSMutableArray arrayWithArray:unprovisionList];
    
    [self userAbled:NO];
    NSData *key = [SigDataSource.share curNetKey];
    
    __weak typeof(self) weakSelf = self;
    [SigBearer.share stopMeshConnectWithComplete:^(BOOL successful) {
        if (successful) {
            NSOperationQueue *oprationQueue = [[NSOperationQueue alloc] init];
            [oprationQueue addOperationWithBlock:^{
                for (AddDeviceStateModel *model in weakSelf.selectDevices) {
                    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
                    CBPeripheral *peripheral = model.peripheral;
                    UInt16 provisionAddress = [SigDataSource.share provisionAddress];
                    if (provisionAddress == 0) {
                        TeLogDebug(@"warning: address has run out.");
                        [weakSelf userAbled:YES];
                        return;
                    }
                    NSNumber *type = [[NSUserDefaults standardUserDefaults] valueForKey:kKeyBindType];
                    model.state = AddStateProvisioning;
                    [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                    [SDKLibCommand startAddDeviceWithNextAddress:provisionAddress networkKey:key netkeyIndex:SigDataSource.share.curNetkeyModel.index appkeyModel:SigDataSource.share.curAppkeyModel peripheral:peripheral provisionType:ProvisionTpye_NoOOB staticOOBData:nil keyBindType:type.integerValue productID:0 cpsData:nil provisionSuccess:^(NSString * _Nonnull identify, UInt16 address) {
                        model.state = AddStateKeybinding;
                        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                    } provisionFail:^(NSError * _Nonnull error) {
                        model.state = AddStateProvisionFail;
                        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                        dispatch_semaphore_signal(semaphore);
                    } keyBindSuccess:^(NSString * _Nonnull identify, UInt16 address) {
                        model.state = AddStateKeybound;
                        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                        dispatch_semaphore_signal(semaphore);
                    } keyBindFail:^(NSError * _Nonnull error) {
                        model.state = AddStateUnbound;
                        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                        dispatch_semaphore_signal(semaphore);
                    }];
                    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
                }
                dispatch_async(dispatch_get_main_queue(), ^{
                    [weakSelf userAbled:YES];
                });
            }];
        }else{
            TeLogDebug(@"stop mesh fail.");

        }
    }];
}

- (void)scanFinish{
    [SigBluetooth.share stopScan];
    [self userAbled:YES];
}

- (void)normalSetting{
    [super normalSetting];
    self.title = @"choose add device";
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_MeshOTAItemCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_MeshOTAItemCellID];
    self.allDevices = [NSMutableArray array];
    self.selectDevices = [NSMutableArray array];
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)viewDidDisappear:(BOOL)animated{
    [super viewDidDisappear:animated];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(scanFinish) object:nil];
    });
    [SigBluetooth.share stopScan];
}

- (void)userAbled:(BOOL)able{
    self.addButton.enabled = able;
    self.scanButton.enabled = able;
    self.tableView.userInteractionEnabled = able;
    self.addButton.backgroundColor = able ? kDefultColor : kDefultUnableColor;
    self.scanButton.backgroundColor = able ? kDefultColor : kDefultUnableColor;
}

- (void)showTips:(NSString *)message{
    __weak typeof(self) weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        [weakSelf showAlertSureWithTitle:@"Hits" message:message sure:^(UIAlertAction *action) {
            
        }];
    });
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifiers_MeshOTAItemCellID forIndexPath:indexPath];
    [self configureCell:cell forRowAtIndexPath:indexPath];
    return cell;
}

- (void)configureCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath{
    MeshOTAItemCell *itemCell = (MeshOTAItemCell *)cell;
    __weak typeof(self) weakSelf = self;
    
    if (indexPath.row == 0) {
        itemCell.titleLabel.text = @"choose all";
        itemCell.selectButton.selected = self.selectDevices.count == self.allDevices.count;
        if (self.allDevices.count == 0) {
            itemCell.selectButton.selected = NO;
        }
        [itemCell.selectButton addAction:^(UIButton *button) {
            if (!button.selected) {
                weakSelf.selectDevices = [NSMutableArray arrayWithArray:weakSelf.allDevices];
            } else {
                [weakSelf.selectDevices removeAllObjects];
            }
            [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
        }];
    }else{
        AddDeviceStateModel *model = self.allDevices[indexPath.row-1];
        SigScanRspModel *rsp = [SigDataSource.share getScanRspModelWithUUID:model.peripheral.identifier.UUIDString];
        NSString *state = @"";
        switch (model.state) {
            case AddStateScan:
                state = @"scan";
                break;
            case AddStateProvisioning:
                state = @"provisioning";
                break;
            case AddStateProvisionFail:
                state = @"provisionFail";
                break;
            case AddStateKeybinding:
                state = @"keybinding";
                break;
            case AddStateKeybound:
                state = @"keybound";
                break;
            case AddStateUnbound:
                state = @"unbound";
                break;
            default:
                break;
        }
        if (rsp.macAddress) {
            itemCell.titleLabel.text = [NSString stringWithFormat:@"mac:%@ state:%@",rsp.macAddress,state];
        } else {
            itemCell.titleLabel.text = [NSString stringWithFormat:@"uuid:%@ state:%@",model.peripheral.identifier.UUIDString,state];
        }
        if (self.selectDevices.count > 0) {
            itemCell.selectButton.selected = [self.selectDevices containsObject:model];
        } else {
            itemCell.selectButton.selected = NO;
        }
        
        [itemCell.selectButton addAction:^(UIButton *button) {
            if ([weakSelf.selectDevices containsObject:model]) {
                [weakSelf.selectDevices removeObject:model];
            }else{
                [weakSelf.selectDevices addObject:model];
            }
            [weakSelf.tableView reloadData];
        }];
    }
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.allDevices.count + 1;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 44.0;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.row == 0) {
        MeshOTAItemCell *itemCell = [tableView cellForRowAtIndexPath:indexPath];
        if (!itemCell.selectButton.selected) {
            self.selectDevices = [NSMutableArray arrayWithArray:self.allDevices];
        } else {
            [self.selectDevices removeAllObjects];
        }
    } else {
        AddDeviceStateModel *model = self.allDevices[indexPath.row - 1];
        if ([self.selectDevices containsObject:model]) {
            [self.selectDevices removeObject:model];
        }else{
            [self.selectDevices addObject:model];
        }
    }
    [self.tableView reloadData];
}

-(void)dealloc{
    TeLogDebug(@"");
}

@end
