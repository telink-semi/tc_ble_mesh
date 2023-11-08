/********************************************************************************************************
 * @file     DeviceSettingViewController.m 
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/10/10
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
#import "DeviceConfigVC.h"
#import "PTSViewController.h"

#define kDeviceConfig   @"Device Config"
#define kCompositionData   @"Composition Data"
#define kNetKeyList   @"NetKey List"
#define kAppKeyList   @"AppKey List"
#define kSubnetBridgeSetting   @"Subnet Bridge Setting"
#define kSchedulerSetting   @"Scheduler Setting"
#define kSubscriptionModels   @"Subscription Models"
#define kDeviceOTA   @"Device OTA"
#define kPublication   @"Publication"
#define kLPN   @"LPN"
#define kPTStest   @"PTS test"

@interface DeviceSettingViewController ()<UITableViewDelegate,UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UILabel *macLabel;
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIButton *kickOutButton;
@property (nonatomic, assign) BOOL hasClickKickout;
@property (nonatomic, strong) SigMessageHandle *messageHandle;
@property (nonatomic, strong) NSMutableArray <NSString *>*iconArray;
@property (nonatomic, strong) NSMutableArray <NSString *>*titleArray;
@end

@implementation DeviceSettingViewController

- (IBAction)kickOut:(UIButton *)sender {
    TeLogDebug(@"");
    //add a alert for kickout device.
    __weak typeof(self) weakSelf = self;
    UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"Warn" message:@"Confirm to remove device?" preferredStyle:UIAlertControllerStyleAlert];
    UIAlertAction *action = [UIAlertAction actionWithTitle:@"Confirm" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        weakSelf.hasClickKickout = YES;
        [ShowTipsHandle.share show:Tip_KickoutDevice];
        
        if (weakSelf.model.hasPublishFunction && weakSelf.model.hasOpenPublish) {
            [SigPublishManager.share stopCheckOfflineTimerWithAddress:@(weakSelf.model.address)];
        }
        
        if (SigBearer.share.isOpen) {
            [weakSelf kickoutAction];
        } else {
            [SigDataSource.share deleteNodeFromMeshNetworkWithDeviceAddress:weakSelf.model.address];
            [weakSelf pop];
        }
    }];
    UIAlertAction *action2 = [UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:nil];
    [alert addAction:action];
    [alert addAction:action2];
    [self presentViewController:alert animated:YES completion:nil];
}

- (void)kickoutAction{
    TeLogDebug(@"send kickout.");
    __weak typeof(self) weakSelf = self;
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for kick out, but busy now.");
        [self showTips:@"app is busy now, try again later."];
    } else {
        TeLogInfo(@"send request for kick out address:%d",self.model.address);
        _messageHandle = [SDKLibCommand resetNodeWithDestination:self.model.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigNodeResetStatus * _Nonnull responseMessage) {
            
        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
            if (isResponseAll) {
                TeLogDebug(@"kickout success.");
            } else {
                TeLogDebug(@"kickout fail.");
            }
            [SigDataSource.share deleteNodeFromMeshNetworkWithDeviceAddress:weakSelf.model.address];
            dispatch_async(dispatch_get_main_queue(), ^{
                [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf];
                [weakSelf pop];
            });
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
        [SDKLibCommand stopMeshConnectWithComplete:^(BOOL successful) {
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

#pragma mark - Life method
- (void)normalSetting{
    [super normalSetting];
    self.macLabel.text = [NSString stringWithFormat:@"UUID:%@",self.model.UUID];
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    self.kickOutButton.backgroundColor = UIColor.telinkButtonRed;
    self.titleArray = [NSMutableArray array];
    self.iconArray = [NSMutableArray array];
    [self.titleArray addObject:kDeviceConfig];
    [self.iconArray addObject:@"ic_setting"];
    [self.titleArray addObject:kCompositionData];
    [self.iconArray addObject:@"ic_setting"];
    [self.titleArray addObject:kNetKeyList];
    [self.iconArray addObject:@"ic_setting"];
    [self.titleArray addObject:kAppKeyList];
    [self.iconArray addObject:@"ic_setting"];
    [self.titleArray addObject:kSubnetBridgeSetting];
    [self.iconArray addObject:@"ic_setting"];
    [self.titleArray addObject:kSchedulerSetting];
    [self.iconArray addObject:@"ic_alarm"];
    [self.titleArray addObject:kSubscriptionModels];
    [self.iconArray addObject:@"ic_setting"];
    [self.titleArray addObject:kDeviceOTA];
    [self.iconArray addObject:@"ic_update"];
    [self.titleArray addObject:kPublication];
    [self.iconArray addObject:@"ic_pub"];
    if (self.model.isSensor) {
        [self.titleArray addObject:kLPN];
        [self.iconArray addObject:@"ic_battery-20-bluetooth"];
    }
    //测试PTS才打开下面注释的代码
//        [self.titleArray addObject:kPTStest];
//        [self.iconArray addObject:@"ic_setting"];
}

- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
    if (_messageHandle) {
        [_messageHandle cancel];
    }
}

#pragma mark - UITableView
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.titleArray.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    SettingItemCell *cell = (SettingItemCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_SettingItemCellID forIndexPath:indexPath];
    cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    cell.stateSwitch.hidden = YES;
    NSString *title = self.titleArray[indexPath.row];
    if ([title isEqualToString:kPublication]) {
        UInt16 eleAdr = [self.model.publishAddress.firstObject intValue];
        UInt16 option = self.model.publishModelID;
        ModelIDModel *modelIDModel = [SigDataSource.share getModelIDModel:@(option)];
        cell.nameLabel.text = [NSString stringWithFormat:@"%@\n(ele:0x%04X,model:%@)",kPublication , eleAdr, modelIDModel.modelName];
        cell.accessoryType = UITableViewCellAccessoryNone;
        cell.stateSwitch.hidden = NO;
        cell.stateSwitch.enabled = self.model.hasPublishFunction;
        cell.stateSwitch.on = self.model.hasOpenPublish;
        __weak typeof(self) weakSelf = self;
        [cell setChangeStateBlock:^(UISwitch * _Nonnull stateSwitch) {
            /* 周期，20秒上报一次(periodSteps:kPublishIntervalOfDemo,:Range：0x01-0x3F; periodResolution:1) */
            BOOL result = [DemoCommand editPublishListWithPublishAddress:stateSwitch.isOn ? kMeshAddress_allNodes : kMeshAddress_unassignedAddress nodeAddress:weakSelf.model.address elementAddress:eleAdr modelIdentifier:option companyIdentifier:0 periodSteps:SigDataSource.share.defaultPublishPeriodModel.numberOfSteps periodResolution:[LibTools getSigStepResolutionWithSigPeriodModel:SigDataSource.share.defaultPublishPeriodModel] retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigModelPublicationStatus * _Nonnull responseMessage) {
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
            if (result == NO) {
                [weakSelf showTips:@"app is busy now, try again later."];
            }
        }];
    } else {
        cell.nameLabel.text = title;
    }
    cell.iconImageView.image = [UIImage imageNamed:self.iconArray[indexPath.row]];
    return cell;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {
    return 51.0;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    NSString *title = self.titleArray[indexPath.row];
    if ([title isEqualToString:kDeviceConfig]) {
        [self pushToDeviceConfigVC];
    } else if ([title isEqualToString:kCompositionData]) {
        [self pushToCompositionDataVC];
    } else if ([title isEqualToString:kNetKeyList]) {
        [self pushToNetKeyListVC];
    } else if ([title isEqualToString:kAppKeyList]) {
        [self pushToAppKeyListVC];
    } else if ([title isEqualToString:kSubnetBridgeSetting]) {
        if (self.model.subnetBridgeServerAddress.count > 0) {
            [self pushToSubnetBridgeSettingVC];
        } else {
            [self showTips:@"Node hasn't subnet bridge server modelID"];
        }
    } else if ([title isEqualToString:kSchedulerSetting]) {
        if (self.model.schedulerAddress.count > 0) {
            [self pushToSchedulerVC];
        } else {
            [self showTips:@"Node hasn't scheduler model"];
        }
    } else if ([title isEqualToString:kSubscriptionModels]) {
        [self pushToSubscriptionVC];
    } else if ([title isEqualToString:kDeviceOTA]) {
        [self pushToDeviceOTA];
    } else if ([title isEqualToString:kPublication]) {
        if (self.model.publishAddress.count == 0) {
            [self showTips:@"Node hasn't publish model"];
        }
    } else if ([title isEqualToString:kLPN]) {
        [self pushToSensorVC];
    } else if ([title isEqualToString:kPTStest]) {
        [self pushToPTSVC];
    }
    UITableViewCell *cell = [tableView cellForRowAtIndexPath:indexPath];
    cell.selected = NO;
}

- (void)pushToDeviceConfigVC {
    DeviceConfigVC *vc = [[DeviceConfigVC alloc] init];
    vc.model = self.model;
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)pushToCompositionDataVC{
    DeviceCompositionDataVC *vc = (DeviceCompositionDataVC *)[UIStoryboard initVC:ViewControllerIdentifiers_DeviceCompositionDataVCID storyboard:@"DeviceSetting"];
    vc.model = self.model;
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)pushToSubnetBridgeSettingVC{
    SubnetBridgeListVC *vc = (SubnetBridgeListVC *)[UIStoryboard initVC:ViewControllerIdentifiers_SubnetBridgeListVCID storyboard:@"DeviceSetting"];
    vc.model = self.model;
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)pushToSchedulerVC{
    SchedulerListViewController *vc = (SchedulerListViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_SchedulerListViewControllerID storyboard:@"Setting"];
    vc.model = self.model;
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)pushToSubscriptionVC{
    DeviceSubscriptionListViewController *vc = (DeviceSubscriptionListViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_DeviceSubscriptionListViewControllerID storyboard:@"DeviceSetting"];
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
    SensorVC *vc = (SensorVC *)[UIStoryboard initVC:ViewControllerIdentifiers_SensorVCID storyboard:@"Main"];
    vc.model = self.model;
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)pushToPTSVC {
    PTSViewController *vc = [[PTSViewController alloc] init];
    vc.model = self.model;
    [self.navigationController pushViewController:vc animated:YES];
}

-(void)dealloc{
    TeLogDebug(@"");
}

@end
