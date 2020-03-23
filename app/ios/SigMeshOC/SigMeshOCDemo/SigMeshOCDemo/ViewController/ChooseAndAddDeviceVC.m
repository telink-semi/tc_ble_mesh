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
    
//    __weak typeof(self) weakSelf = self;
//    [Bluetooth.share cancelAllConnecttionWithComplete:^{
//        [weakSelf blockState];
//        [Bluetooth.share stopAutoConnect];
//        [Bluetooth.share setProvisionState];
//        [Bluetooth.share startScan];
//        [weakSelf userAbled:NO];
//
//        dispatch_async(dispatch_get_main_queue(), ^{
//            [weakSelf performSelector:@selector(scanFinish) withObject:nil afterDelay:5.0];
//        });
//    }];
    
    //since v3.2.0
    [self userAbled:NO];
    __weak typeof(self) weakSelf = self;
    [self.ble.commandHandle startScanWithProvisionAble:YES timeout:5.0 discoverDevice:^(CBPeripheral *peripheral, SigScanRspModel *model, BOOL provisionAble) {
        TeLog(@"peripheral=%@,model=%@,provisionAble=%@",peripheral,model,provisionAble?@"Add":@"Normal");
        if (provisionAble) {
            AddDeviceStateModel *model = [[AddDeviceStateModel alloc] init];
            model.peripheral = peripheral;
            model.state = AddStateScan;
            if (![weakSelf.allDevices containsObject:model]) {
                [weakSelf.allDevices addObject:model];
                [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
            }
        }
    } finish:^{
        [weakSelf userAbled:YES];
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

//    self.ble.commandHandle.responseVendorIDCallBack = nil;
    [Bluetooth.share stopAutoConnect];
    [Bluetooth.share cancelAllConnecttionWithComplete:nil];
    [Bluetooth.share clearCachelist];
    
    [self userAbled:NO];
    NSData *key = [SigDataSource.share curNetKey];
    
    __weak typeof(self) weakSelf = self;
    NSOperationQueue *oprationQueue = [[NSOperationQueue alloc] init];
    [oprationQueue addOperationWithBlock:^{
        for (AddDeviceStateModel *model in weakSelf.selectDevices) {
            dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
            CBPeripheral *peripheral = model.peripheral;
            UInt16 provisionAddress = [SigDataSource.share provisionAddress];
            if (provisionAddress == 0) {
                TeLog(@"warning: address has run out.");
                [weakSelf userAbled:YES];
                return;
            }else if (provisionAddress > SigDataSource.share.curProvisionerModel.allocatedUnicastRange.firstObject.hightIntAddress) {
                [weakSelf userAbled:YES];
                [weakSelf showTips:@"warning: address out of range."];
                return;
            }
            
            NSNumber *type = [[NSUserDefaults standardUserDefaults] valueForKey:kKeyBindType];
            model.state = AddStateProvisioning;
            [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
            
            
//            [weakSelf.ble.commandHandle startAddDeviceWithNextAddress:provisionAddress networkKey:key netkeyIndex:SigDataSource.share.curNetkeyModel.index peripheral:peripheral keyBindType:type.integerValue provisionSuccess:^(NSString *identify, UInt16 address) {
//                model.state = AddStateKeybinding;
//                [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
//            } keyBindSuccess:^(NSString *identify, UInt16 address) {
//                model.state = AddStateKeybound;
//                [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
//            } fail:^(NSString *errorString) {
//                model.state = AddStateUnbound;
//                [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
//            } finish:^{
//                    dispatch_semaphore_signal(semaphore);
//            }];
            
            
            //===============test1===============//
//            [weakSelf.ble.commandHandle startProvisionWithPeripheral:peripheral unicastAddress:provisionAddress networkKey:key netkeyIndex:SigDataSource.share.curNetkeyModel.index provisionType:ProvisionTpye_NoOOB staticOOBData:nil provisionSuccess:^(NSString *identify, UInt16 address) {
//                TeLog(@"provisionSuccess,identify=%@,address=%d",identify,address);
//                model.state = AddStateKeybinding;
//                [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
//                [weakSelf.ble.commandHandle startKeyBindWithPeripheral:peripheral unicastAddress:provisionAddress appKey:SigDataSource.share.curAppKey appkeyIndex:SigDataSource.share.curAppkeyModel.index netkeyIndex:SigDataSource.share.curNetkeyModel.index keyBindType:type.integerValue productID:0 cpsData:nil keyBindSuccess:^(NSString *identify, UInt16 address) {
//                    model.state = AddStateKeybound;
//                    [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
//                    dispatch_semaphore_signal(semaphore);
//                } fail:^(NSError *error) {
//                    TeLog(@"keybindFail,error=%@",error);
//                    model.state = AddStateUnbound;
//                    [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
//                    dispatch_semaphore_signal(semaphore);
//                }];
//            } fail:^(NSError *error) {
//                TeLog(@"provisionFail,error=%@",error);
//                model.state = AddStateUnbound;
//                [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
//                dispatch_semaphore_signal(semaphore);
//            }];
            //===============test1===============//

            
            //===============test2===============//
            /// 2.1 no oob+normal keybind
            SigAddConfigModel *config = [[SigAddConfigModel alloc] initWithCBPeripheral:peripheral unicastAddress:provisionAddress networkKey:key netkeyIndex:SigDataSource.share.curNetkeyModel.index appKey:SigDataSource.share.curAppKey appkeyIndex:SigDataSource.share.curAppkeyModel.index provisionType:ProvisionTpye_NoOOB staticOOBData:nil keyBindType:type.integerValue productID:0 cpsData:nil];
            /// 2.2 no oob+fast keybind
//            SigAddConfigModel *config = [[SigAddConfigModel alloc] initWithCBPeripheral:peripheral unicastAddress:provisionAddress networkKey:key netkeyIndex:SigDataSource.share.curNetkeyModel.index appKey:SigDataSource.share.curAppKey appkeyIndex:SigDataSource.share.curAppkeyModel.index provisionType:ProvisionTpye_NoOOB staticOOBData:nil keyBindType:KeyBindTpye_Fast productID:1 cpsData:[NSData dataWithBytes:CTByte length:76]];
            /// 2.3 static oob+normal keybind
//            NSString *oobString = @"000102030405060708090A0B0C0D0E0F";
//            NSData *oobData = [LibTools nsstringToHex:oobString];
//            SigAddConfigModel *config = [[SigAddConfigModel alloc] initWithCBPeripheral:peripheral unicastAddress:provisionAddress networkKey:key netkeyIndex:SigDataSource.share.curNetkeyModel.index appKey:SigDataSource.share.curAppKey appkeyIndex:SigDataSource.share.curAppkeyModel.index provisionType:ProvisionTpye_StaticOOB staticOOBData:oobData keyBindType:type.integerValue productID:0 cpsData:nil];
            [weakSelf.ble.commandHandle startAddDeviceWithSigAddConfigModel:config provisionSuccess:^(NSString *identify, UInt16 address) {
                TeLog(@"provisionSuccess,identify=%@,address=%d",identify,address);
                model.state = AddStateKeybinding;
                [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
            } provisionFail:^(NSError *error) {
                TeLog(@"provisionFail,error=%@",error);
                model.state = AddStateUnbound;
                [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                dispatch_semaphore_signal(semaphore);
            } keyBindSuccess:^(NSString *identify, UInt16 address) {
                model.state = AddStateKeybound;
                [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                dispatch_semaphore_signal(semaphore);
            } keyBindFail:^(NSError *error) {
                TeLog(@"keybindFail,error=%@",error);
                model.state = AddStateUnbound;
                [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                dispatch_semaphore_signal(semaphore);
            }];
            //===============test2===============//
            
            
            dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
        }
        dispatch_async(dispatch_get_main_queue(), ^{
            [weakSelf userAbled:YES];
        });
    }];
}

- (void)cancelAll{
    [Bluetooth.share cancelAllConnecttionWithComplete:^{
        TeLog(@"cancel all success");
    }];
}

//- (void)scanFinish{
//    [Bluetooth.share setNormalState];
//    [Bluetooth.share stopScan];
//    [self userAbled:YES];
//}

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
    [self.ble.commandHandle stopScan];
//    dispatch_async(dispatch_get_main_queue(), ^{
//        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(scanFinish) object:nil];
//    });
//    [Bluetooth.share setNormalState];
//    [Bluetooth.share stopScan];
}

- (void)blockState{
    [super blockState];
    
//    __weak typeof(self) weakSelf = self;
//    [Bluetooth.share setBleScanNewDeviceCallBack:^(CBPeripheral *peripheral, BOOL provisioned) {
//        if (provisioned) {
//            AddDeviceStateModel *model = [[AddDeviceStateModel alloc] init];
//            model.peripheral = peripheral;
//            model.state = AddStateScan;
//            if (![weakSelf.allDevices containsObject:model]) {
//                [weakSelf.allDevices addObject:model];
//                [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
//            }
//        }
//    }];
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
        itemCell.titleLabel.text = [NSString stringWithFormat:@"mac:%@ state:%@",rsp.macAddress,state];
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
    TeLog(@"");
}

@end
