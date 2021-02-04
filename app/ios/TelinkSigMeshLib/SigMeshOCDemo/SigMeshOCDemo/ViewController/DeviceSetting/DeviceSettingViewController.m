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
//  Created by 梁家誌 on 2018/10/10.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "DeviceSettingViewController.h"
#import "SettingItemCell.h"
#import "SchedulerListViewController.h"
#import "SingleOTAViewController.h"
#import "DeviceSubscriptionListViewController.h"
#import "UIViewController+Message.h"
#import "SensorVC.h"
#import "DeviceCompositionDataVC.h"
#import "DeviceNetKeyListVC.h"
#import "DeviceAppKeyListVC.h"
#import "SubnetBridgeListVC.h"

@interface DeviceSettingViewController ()<UITableViewDelegate,UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UILabel *macLabel;
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIButton *kickOutButton;
@property (nonatomic, assign) BOOL hasClickKickout;
@property (nonatomic, strong) SigMessageHandle *messageHandle;
@end

@implementation DeviceSettingViewController

- (IBAction)kickOut:(UIButton *)sender {
    TeLogDebug(@"");
    self.hasClickKickout = YES;
    [ShowTipsHandle.share show:Tip_KickoutDevice];
    
    if (self.model.hasPublishFunction && self.model.hasOpenPublish) {
        [SigPublishManager.share stopCheckOfflineTimerWithAddress:@(self.model.address)];
    }
    
    if (SigBearer.share.isOpen) {
        [self kickoutAction];
    } else {
        [SigDataSource.share deleteNodeFromMeshNetworkWithDeviceAddress:self.model.address];
        [self pop];
    }
}

- (void)kickoutAction{
    TeLogDebug(@"send kickout.");
    __weak typeof(self) weakSelf = self;
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for kick out, but busy now.");
        [self showTips:@"app is busy now, try again later."];
    } else {
        TeLogInfo(@"send request for kick out address:%d",self.model.address);
        _messageHandle = [SDKLibCommand resetNodeWithDestination:self.model.address retryCount:0 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigNodeResetStatus * _Nonnull responseMessage) {
            
        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
            if (isResponseAll) {
                TeLogDebug(@"kickout success.");
            } else {
                TeLogDebug(@"kickout fail.");
            }
            [SigDataSource.share deleteNodeFromMeshNetworkWithDeviceAddress:weakSelf.model.address];
            [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf];
            [weakSelf pop];
        }];
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(resetNodeTimeout) object:nil];
            [self performSelector:@selector(resetNodeTimeout) withObject:nil afterDelay:5.0];
        });
    }
}

- (void)resetNodeTimeout {
    [SigDataSource.share deleteNodeFromMeshNetworkWithDeviceAddress:self.model.address];
    [NSObject cancelPreviousPerformRequestsWithTarget:self];
    [self pop];
}

- (void)pop{
    if (SigDataSource.share.unicastAddressOfConnected == self.model.address) {
        __weak typeof(self) weakSelf = self;
        [SigBearer.share stopMeshConnectWithComplete:^(BOOL successful) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [ShowTipsHandle.share hidden];
                [weakSelf.navigationController popViewControllerAnimated:YES];
            });
        }];
    } else {
        dispatch_async(dispatch_get_main_queue(), ^{
            [ShowTipsHandle.share hidden];
            [self.navigationController popViewControllerAnimated:YES];
        });
    }
}

- (void)blockState{
    [super blockState];
}

- (void)nilBlock{
    [super nilBlock];
}

#pragma mark - Life method
- (void)normalSetting{
    [super normalSetting];
//    self.macLabel.hidden = YES;
//    self.macLabel.text = [NSString stringWithFormat:@"MAC:%@",[LibTools getMacStringWithMac:self.model.macAddress]];
    self.macLabel.text = [NSString stringWithFormat:@"UUID:%@",self.model.UUID];
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
}

- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
    if (_messageHandle) {
        [_messageHandle cancel];
    }
}

#pragma mark - UITableView
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    if (self.model.isSensor) {
        return 9;
    } else {
        return 8;
    }
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    SettingItemCell *cell = (SettingItemCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_SettingItemCellID forIndexPath:indexPath];
    cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    cell.stateSwitch.hidden = YES;
#ifndef kExist
    cell.contentView.hidden = indexPath.row == 3;
#endif
    switch (indexPath.row) {
        case 0:
        {
            cell.nameLabel.text = @"Composition Data";
            cell.iconImageView.image = [UIImage imageNamed:@"ic_setting"];
        }
            break;
        case 1:
        {
            cell.nameLabel.text = @"NetKey List";
            cell.iconImageView.image = [UIImage imageNamed:@"ic_setting"];
        }
            break;
        case 2:
        {
            cell.nameLabel.text = @"AppKey List";
            cell.iconImageView.image = [UIImage imageNamed:@"ic_setting"];
        }
            break;
        case 3:
        {
            cell.nameLabel.text = @"Subnet Bridge Setting";
            cell.iconImageView.image = [UIImage imageNamed:@"ic_setting"];
        }
            break;
        case 4:
        {
            cell.nameLabel.text = @"Scheduler Setting";
            cell.iconImageView.image = [UIImage imageNamed:@"ic_alarm"];
        }
            break;
        case 5:
        {
            cell.nameLabel.text = @"Subscription Models";
            cell.iconImageView.image = [UIImage imageNamed:@"ic_setting"];
        }
            break;
        case 6:
        {
            cell.nameLabel.text = @"Device OTA";
            cell.iconImageView.image = [UIImage imageNamed:@"ic_update"];
        }
            break;
        case 7:
        {
            UInt16 option = self.model.publishModelID;
            UInt16 eleAdr = [self.model.publishAddress.firstObject intValue];
            ModelIDModel *modelIDModel = [SigDataSource.share getModelIDModel:@(option)];
            cell.nameLabel.text = [NSString stringWithFormat:@"Publication\n(ele:0x%04X,model:%@)",eleAdr,modelIDModel.modelName];
            cell.iconImageView.image = [UIImage imageNamed:@"ic_pub"];
            cell.accessoryType = UITableViewCellAccessoryNone;
            cell.stateSwitch.hidden = NO;
            cell.stateSwitch.enabled = self.model.hasPublishFunction;
            cell.stateSwitch.on = self.model.hasOpenPublish;
            __weak typeof(self) weakSelf = self;
            [cell setChangeStateBlock:^(UISwitch * _Nonnull stateSwitch) {                
                /* 周期，20秒上报一次(periodSteps:kPublishIntervalOfDemo,:Range：0x01-0x3F; periodResolution:1) */
                [DemoCommand editPublishListWithPublishAddress:stateSwitch.isOn ? kMeshAddress_allNodes : kMeshAddress_unassignedAddress nodeAddress:weakSelf.model.address elementAddress:eleAdr modelIdentifier:option companyIdentifier:0 periodSteps:SigDataSource.share.defaultPublishPeriodModel.numberOfSteps periodResolution:[LibTools getSigStepResolutionWithSigPeriodModel:SigDataSource.share.defaultPublishPeriodModel] retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigModelPublicationStatus * _Nonnull responseMessage) {
                    TeLogDebug(@"editPublishList callback");
                    if (responseMessage.status == SigConfigMessageStatus_success && responseMessage.elementAddress == eleAdr) {
                        if (responseMessage.publish.publicationAddress.address == kMeshAddress_allNodes) {
                            [weakSelf.model openPublish];
                            [SigPublishManager.share startCheckOfflineTimerWithAddress:@(weakSelf.model.address)];
                        } else {
                            [weakSelf.model closePublish];
                            [SigPublishManager.share stopCheckOfflineTimerWithAddress:@(weakSelf.model.address)];
                        }
                        [SigDataSource.share saveLocationData];
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {

                }];
            }];
        }
            break;
        case 8:
        {
            cell.nameLabel.text = @"LPN";
            cell.iconImageView.image = [UIImage imageNamed:@"ic_battery-20-bluetooth"];
        }
            break;
        default:
            break;
    }
    return cell;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
#ifndef kExist
    if (indexPath.row == 3) {
        return 0.01f;
    }
#endif
    return 51.0;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    switch (indexPath.row) {
        case 0:
        {
            [self pushToCompositionDataVC];
        }
            break;
        case 1:
        {
            [self pushToNetKeyListVC];
        }
            break;
        case 2:
        {
            [self pushToAppKeyListVC];
        }
            break;
        case 3:
        {
            if (self.model.subnetBridgeServerAddress.count > 0) {
                [self pushToSubnetBridgeSettingVC];
            } else {
                [self showTips:@"Node hasn't subnet bridge server modelID"];
            }
        }
            break;
        case 4:
        {
            if (self.model.schedulerAddress.count > 0) {
                [self pushToSchedulerVC];
            } else {
                [self showTips:@"Node hasn't scheduler model"];
            }
        }
            break;
        case 5:
        {
            [self pushToSubscriptionVC];
        }
            break;
        case 6:
        {
            [self pushToDeviceOTA];
        }
            break;
        case 7:
        {
            if (self.model.publishAddress.count == 0) {
                [self showTips:@"Node hasn't publish model"];
            }
        }
            break;
        case 8:
        {
            [self pushToSensorVC];
        }
            break;
        default:
            break;
    }
    UITableViewCell *cell = [tableView cellForRowAtIndexPath:indexPath];
    cell.selected = NO;
}

- (void)pushToCompositionDataVC{
    DeviceCompositionDataVC *vc = (DeviceCompositionDataVC *)[UIStoryboard initVC:ViewControllerIdentifiers_DeviceCompositionDataVCID storybroad:@"DeviceSetting"];
    vc.model = self.model;
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)pushToSubnetBridgeSettingVC{
    SubnetBridgeListVC *vc = (SubnetBridgeListVC *)[UIStoryboard initVC:ViewControllerIdentifiers_SubnetBridgeListVCID storybroad:@"DeviceSetting"];
    vc.model = self.model;
    [self.navigationController pushViewController:vc animated:YES];
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

- (void)pushToNetKeyListVC {
    DeviceNetKeyListVC *vc = [[DeviceNetKeyListVC alloc] init];
    vc.model = self.model;
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)pushToAppKeyListVC {
    DeviceAppKeyListVC *vc = [[DeviceAppKeyListVC alloc] init];
    vc.model = self.model;
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)pushToSensorVC {
    SensorVC *vc = (SensorVC *)[UIStoryboard initVC:ViewControllerIdentifiers_SensorVCID storybroad:@"Main"];
    vc.model = self.model;
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)showTips:(NSString *)message{
    [self showAlertSureWithTitle:@"Hits" message:message sure:^(UIAlertAction *action) {
        
    }];
}

-(void)dealloc{
    TeLogDebug(@"");
}

@end
