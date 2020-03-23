/********************************************************************************************************
 * @file     DeviceSettingViewController.m 
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
//  DeviceSettingViewController.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2018/10/10.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "DeviceSettingViewController.h"
#import "SettingItemCell.h"
#import "SchedulerListViewController.h"
#import "SingleOTAViewController.h"
#import "DeviceSubscriptionListViewController.h"
#import "UIViewController+Message.h"

@interface DeviceSettingViewController ()<UITableViewDelegate,UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UILabel *macLabel;
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIButton *kickOutButton;
@property (nonatomic, assign) BOOL hasClickKickout;
@end

@implementation DeviceSettingViewController

- (IBAction)kickOut:(UIButton *)sender {
    TeLog(@"");
    self.hasClickKickout = YES;
    [ShowTipsHandle.share show:Tip_KickoutDevice];
    
    [SigDataSource.share removeModelWithDeviceAddress:self.model.address];
    if (self.model.hasPublishFunction && self.model.hasOpenPublish) {
        [Bluetooth.share stopCheckOfflineTimerWithAddress:@(self.model.address)];
    }
    
    if (self.ble.isConnected) {
        [self kickoutAction];
    } else {
        [self pop];
    }
}

- (void)kickoutAction{
    TeLog(@"send kickout.");
    __weak typeof(self) weakSelf = self;
    [DemoCommand kickoutDevice:self.model.address complete:^{
        TeLog(@"kickout success.");
        [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf];
        [weakSelf pop];
    }];
    
    if (self.model && [self.model.peripheralUUID isEqualToString:self.ble.currentPeripheral.identifier.UUIDString]) {
        //if node is Bluetooth.share.currentPeripheral, wait node didDisconnectPeripheral, delay 1.5s and pop.
    } else {
        //if node isn't Bluetooth.share.currentPeripheral, delay 5s and pop.
        [self performSelector:@selector(pop) withObject:nil afterDelay:TimeOut_KickoutConnectedDelayResponseTime];
    }
}

- (void)pop{
    self.ble.commandHandle.delectDeviceCallBack = nil;
    [ShowTipsHandle.share hidden];
    [self.navigationController popViewControllerAnimated:YES];
}

- (void)blockState{
    [super blockState];
    __weak typeof(self) weakSelf = self;
    [self.ble bleDisconnectOrConnectFail:^(CBPeripheral *peripheral) {
        if (weakSelf.hasClickKickout) {
            [ShowTipsHandle.share show:Tip_DisconnectOrConnectFail];
            [weakSelf performSelector:@selector(pop) withObject:nil afterDelay:TimeOut_KickoutDelayResponseTime];
        }
    }];
}

- (void)nilBlock{
    [super nilBlock];
    self.ble.bleDisconnectOrConnectFailCallBack = nil;
}

#pragma mark - Life method
- (void)normalSetting{
    [super normalSetting];
//    self.macLabel.hidden = YES;
    self.macLabel.text = [NSString stringWithFormat:@"MAC:%@",[LibTools getMacStringWithMac:self.model.macAddress]];
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
}

#pragma mark - UITableView
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return 4;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    SettingItemCell *cell = (SettingItemCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_SettingItemCellID forIndexPath:indexPath];
    cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    cell.stateSwitch.hidden = YES;
    switch (indexPath.row) {
        case 0:
        {
            cell.nameLabel.text = @"Scheduler Setting";
            cell.iconImageView.image = [UIImage imageNamed:@"ic_alarm"];
        }
            break;
        case 1:
        {
            cell.nameLabel.text = @"Subscription Models";
            cell.iconImageView.image = [UIImage imageNamed:@"ic_setting"];
        }
            break;
        case 2:
        {
            cell.nameLabel.text = @"Device OTA";
            cell.iconImageView.image = [UIImage imageNamed:@"ic_update"];
        }
            break;
        case 3:
        {
            cell.nameLabel.text = @"Publish Model";
            cell.iconImageView.image = [UIImage imageNamed:@"ic_pub"];
            cell.accessoryType = UITableViewCellAccessoryNone;
            cell.stateSwitch.hidden = NO;
            cell.stateSwitch.enabled = self.model.hasPublishFunction;
            cell.stateSwitch.on = self.model.hasOpenPublish;
            __weak typeof(self) weakSelf = self;
            [cell setChangeStateBlock:^(UISwitch * _Nonnull stateSwitch) {
                UInt32 option = weakSelf.model.publishModelID;
                UInt16 eleAdr = [weakSelf.model.publishAddress.firstObject intValue];
                //周期，5秒上报一次。
                mesh_pub_period_t period;
                period.steps = kPublishInterval;//Range：0x01-0x3F
                period.res = 1;
                [DemoCommand editPublishList:YES publishAddress:stateSwitch.isOn ? kAllDo_SIGParameters : 0x0 nodeAddress:weakSelf.model.address eleAddress:eleAdr option:option period:period complete:^(ResponseModel *m) {
                    TeLog(@"editPublishList callback");
                    PublishResponseModel *pubModel = [[PublishResponseModel alloc] initWithResponseData:m.rspData];
                    if (pubModel.status == 0 && pubModel.elementAddress == eleAdr) {
                        if (pubModel.publishAddress == kAllDo_SIGParameters) {
                            [weakSelf.model openPublish];
                            [Bluetooth.share startCheckOfflineTimerWithAddress:@(weakSelf.model.address)];
                        }else{
                            [weakSelf.model closePublish];
                            [Bluetooth.share stopCheckOfflineTimerWithAddress:@(weakSelf.model.address)];
                        }
                        [SigDataSource.share saveLocationData];
                    }
                }];
            }];
        }
            break;
        default:
            break;
    }
    return cell;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 51.0;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    switch (indexPath.row) {
        case 0:
        {
            if (self.model.schedulerAddress.count > 0) {
                [self pushToSchedulerVC];
            } else {
                [self showTips:@"Node hasn't scheduler model"];
            }
        }
            break;
        case 1:
        {
            [self pushToSubscriptionVC];
        }
            break;
        case 2:
        {
            [self pushToDeviceOTA];
        }
            case 3:
        {
            if (self.model.publishAddress.count == 0) {
                [self showTips:@"Node hasn't publish model"];
            }
        }
            break;
        default:
            break;
    }
    UITableViewCell *cell = [tableView cellForRowAtIndexPath:indexPath];
    cell.selected = NO;
}

- (void)pushToSchedulerVC{
    SchedulerListViewController *vc = (SchedulerListViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_SchedulerListViewControllerID storybroad:@"Setting"];
    vc.model = self.model;
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)pushToSubscriptionVC{
    DeviceSubscriptionListViewController *vc = (DeviceSubscriptionListViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_DeviceSubscriptionListViewControllerID storybroad:@"DeviceSetting"];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)pushToDeviceOTA{
    SingleOTAViewController *vc = (SingleOTAViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_SingleOTAViewControllerID];
    vc.model = self.model;
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)showTips:(NSString *)message{
    [self showAlertSureWithTitle:@"Hits" message:message sure:^(UIAlertAction *action) {
        
    }];
}

-(void)dealloc{
    TeLog(@"");
}

@end
