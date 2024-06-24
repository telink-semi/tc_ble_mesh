/********************************************************************************************************
 * @file     DeviceSettingViewController.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/10/10
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#import "SchedulerListViewController.h"
#import "DeviceSubscriptionListViewController.h"
#import "UIViewController+Message.h"
#import "SensorVC.h"
#import "DeviceCompositionDataVC.h"
#import "DeviceNetKeyListVC.h"
#import "DeviceAppKeyListVC.h"
#import "SubnetBridgeListVC.h"
#import "DeviceConfigVC.h"
#import "PTSViewController.h"
#import "SettingTitleItemCell.h"
#import "EntryCell.h"
#import "UIButton+extension.h"
#import "PrivateBeaconVC.h"
#import "NSString+extension.h"

#ifdef kIsTelinkCloudSigMeshLib
#import "OTAVC.h"
#else
#import "SingleOTAViewController.h"
#endif

#define kDeviceRename   @"Device Rename"
#define kDeviceConfig   @"Device Config"
#define kCompositionData   @"Composition Data"
#define kNetKeyList   @"NetKey List"
#define kAppKeyList   @"AppKey List"
#define kSubnetBridgeSetting   @"Subnet Bridge Setting"
#define kSchedulerSetting   @"Scheduler Setting"
#define kSubscriptionModels   @"Subscription Models"
#define kDeviceOTA   @"Device OTA"
#define kPublication   @"Publication"
#define kPrivateBeacon   @"Private Beacon"
#define kLPN   @"LPN"
#define kPTStest   @"PTS test"

@interface DeviceSettingViewController ()<UITableViewDelegate,UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UILabel *macLabel;
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIButton *kickOutButton;
@property (nonatomic, assign) BOOL hasClickKickOut;
@property (nonatomic, strong) SigMessageHandle *messageHandle;
@property (nonatomic, strong) NSMutableArray <NSString *>*iconArray;
@property (nonatomic, strong) NSMutableArray <NSString *>*titleArray;
//the end time of broadcast Solicitation PDU.
@property (strong, nonatomic) NSDate *endDate;
@end

@implementation DeviceSettingViewController

- (IBAction)kickOut:(UIButton *)sender {
    TelinkLogDebug(@"");
    //add a alert for kickOut device.
    __weak typeof(self) weakSelf = self;
    [self showAlertSureAndCancelWithTitle:kDefaultAlertTitle message:@"Confirm to remove device?" sure:^(UIAlertAction *action) {
        weakSelf.hasClickKickOut = YES;
        [ShowTipsHandle.share show:Tip_KickOutDevice];

        if (weakSelf.model.hasPublishFunction && weakSelf.model.hasOpenPublish) {
            [SigPublishManager.share stopCheckOfflineTimerWithAddress:@(weakSelf.model.address)];
        }

        if (SigBearer.share.isOpen) {
            [weakSelf kickoutAction];
        } else {
            [SigDataSource.share deleteNodeFromMeshNetworkWithDeviceAddress:weakSelf.model.address];
#ifdef kIsTelinkCloudSigMeshLib
            [AppDataSource.share deleteNodeWithAddress:weakSelf.model.address resultBlock:^(NSError * _Nullable error) {
                TelinkLogInfo(@"error = %@", error);
            }];
#endif
            [weakSelf pop];
        }
    } cancel:nil];
}

- (void)kickoutAction{
    TelinkLogDebug(@"send kickout.");
    __weak typeof(self) weakSelf = self;
    if (SigMeshLib.share.isBusyNow) {
        TelinkLogInfo(@"send request for kick out, but busy now.");
        [self showTips:@"app is busy now, try again later."];
    } else {
        TelinkLogInfo(@"send request for kick out address:%d",self.model.address);
        _messageHandle = [SDKLibCommand resetNodeWithDestination:self.model.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigNodeResetStatus * _Nonnull responseMessage) {

        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
            if (isResponseAll) {
                TelinkLogDebug(@"kickout success.");
            } else {
                TelinkLogDebug(@"kickout fail.");
            }
#ifdef kIsTelinkCloudSigMeshLib
            [AppDataSource.share deleteNodeWithAddress:weakSelf.model.address resultBlock:^(NSError * _Nullable error) {
                TelinkLogInfo(@"error = %@", error);
            }];
#endif
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
    if (self.model.macAddress.length > 0) {
        self.macLabel.text = [NSString stringWithFormat:@"UUID:%@ MAC:%@", self.model.UUID, self.model.macAddress];
    } else {
        self.macLabel.text = [NSString stringWithFormat:@"UUID:%@",self.model.UUID];
    }
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(SettingTitleItemCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(SettingTitleItemCell.class)];
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(EntryCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(EntryCell.class)];
    self.kickOutButton.backgroundColor = UIColor.telinkButtonRed;
    self.titleArray = [NSMutableArray array];
    self.iconArray = [NSMutableArray array];
#ifdef kIsTelinkCloudSigMeshLib
    [self.titleArray addObject:kDeviceConfig];
    [self.iconArray addObject:@"ic_config"];
    [self.titleArray addObject:kCompositionData];
    [self.iconArray addObject:@"ic_composition"];
    [self.titleArray addObject:kSchedulerSetting];
    [self.iconArray addObject:@"ic_alarm"];
    [self.titleArray addObject:kSubscriptionModels];
    [self.iconArray addObject:@"ic_model"];
    [self.titleArray addObject:kDeviceOTA];
    [self.iconArray addObject:@"ic_update"];
    [self.titleArray addObject:kPublication];
    [self.iconArray addObject:@"ic_publish"];
#else
    [self.titleArray addObject:kDeviceRename];
    [self.iconArray addObject:@"ic_rename"];
    [self.titleArray addObject:kDeviceConfig];
    [self.iconArray addObject:@"ic_config"];
    [self.titleArray addObject:kCompositionData];
    [self.iconArray addObject:@"ic_composition"];
    [self.titleArray addObject:kNetKeyList];
    [self.iconArray addObject:@"ic_keys"];
    [self.titleArray addObject:kAppKeyList];
    [self.iconArray addObject:@"ic_keys"];
    [self.titleArray addObject:kSubnetBridgeSetting];
    [self.iconArray addObject:@"ic_bridge"];
    [self.titleArray addObject:kSchedulerSetting];
    [self.iconArray addObject:@"ic_alarm"];
    [self.titleArray addObject:kSubscriptionModels];
    [self.iconArray addObject:@"ic_model"];
    [self.titleArray addObject:kDeviceOTA];
    [self.iconArray addObject:@"ic_update"];
    [self.titleArray addObject:kPublication];
    [self.iconArray addObject:@"ic_publish"];
    //Private Beacon
    [self.titleArray addObject:kPrivateBeacon];
    [self.iconArray addObject:@"ic_private_beacon"];
#endif
    //发送自定义vendor指令，v4.1.0.0后的版本不再需要。
//    if (self.model.isSensor) {
//        [self.titleArray addObject:kLPN];
//        [self.iconArray addObject:@"ic_battery-20-bluetooth"];
//    }
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
    NSString *title = self.titleArray[indexPath.row];
    if ([title isEqualToString:kPublication]) {
        //Publication
        EntryCell *cell = (EntryCell *)[tableView dequeueReusableCellWithIdentifier:NSStringFromClass(EntryCell.class) forIndexPath:indexPath];
        cell.iconImageView.image = [UIImage imageNamed:self.iconArray[indexPath.row]];
        UInt16 elementAddress = [self.model.publishAddress.firstObject intValue];
        UInt16 option = self.model.publishModelID;
        ModelIDModel *modelIDModel = [SigDataSource.share getModelIDModel:@(option)];
        cell.nameLabel.text = [NSString stringWithFormat:@"%@(ele:0x%04X, model:%@)",kPublication , elementAddress, modelIDModel.modelName];
        cell.chooseButton.selected = self.model.hasOpenPublish;
        __weak typeof(self) weakSelf = self;
        [cell.chooseButton addAction:^(UIButton *button) {
            button.selected = !button.selected;
            /* 周期，20秒上报一次(periodSteps:kPublishIntervalOfDemo,:Range：0x01-0x3F; periodResolution:1) */
            /* sensor上报周期，0x00，即变化才上报.*/
            UInt8 periodSteps = weakSelf.model.isSensor ? 0 : SigDataSource.share.defaultPublishPeriodModel.numberOfSteps;
            SigStepResolution periodResolution = weakSelf.model.isSensor ? SigStepResolution_hundredsOfMilliseconds : [LibTools getSigStepResolutionWithSigPeriodModel:SigDataSource.share.defaultPublishPeriodModel];
            BOOL result = [DemoCommand editPublishListWithPublishAddress:button.isSelected ? kMeshAddress_allNodes : kMeshAddress_unassignedAddress nodeAddress:weakSelf.model.address elementAddress:elementAddress modelIdentifier:option companyIdentifier:0 periodSteps:periodSteps periodResolution:periodResolution retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigModelPublicationStatus * _Nonnull responseMessage) {
                TelinkLogDebug(@"editPublishList callback");
                if (responseMessage.status == SigConfigMessageStatus_success && responseMessage.elementAddress == elementAddress) {
                    if (responseMessage.publish.publicationAddress.address == kMeshAddress_allNodes) {
                        [weakSelf.model openPublish];
#ifdef kIsTelinkCloudSigMeshLib
                        [AppDataSource.share setPublishWithNodeModel:weakSelf.model resultBlock:^(NSError * _Nullable error) {
                            if (error) {
                                TelinkLogError(@"error=%@", error);
                            }
                        }];
#endif
                        [SigPublishManager.share startCheckOfflineTimerWithAddress:@(weakSelf.model.address)];
                    } else {
                        [weakSelf.model closePublish];
#ifdef kIsTelinkCloudSigMeshLib
                        [AppDataSource.share deletePublishWithNodeModel:weakSelf.model resultBlock:^(NSError * _Nullable error) {
                            if (error) {
                                TelinkLogError(@"error=%@", error);
                            }
                        }];
#endif
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
        return cell;
    }
    SettingTitleItemCell *cell = (SettingTitleItemCell *)[tableView dequeueReusableCellWithIdentifier:NSStringFromClass(SettingTitleItemCell.class) forIndexPath:indexPath];
    cell.nameLabel.text = title;
    cell.iconImageView.image = [UIImage imageNamed:self.iconArray[indexPath.row]];
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    NSString *title = self.titleArray[indexPath.row];
    if ([title isEqualToString:kDeviceRename]) {
        [self clickRenameButton];
    } else if ([title isEqualToString:kDeviceConfig]) {
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
    } else if ([title isEqualToString:kPrivateBeacon]) {
        [self pushToPrivateBeaconVC];
    } else if ([title isEqualToString:kLPN]) {
        [self pushToSensorVC];
    } else if ([title isEqualToString:kPTStest]) {
        [self pushToPTSVC];
    }
}

- (void)clickRenameButton {
    __weak typeof(self) weakSelf = self;
    UIAlertController *alertVc = [UIAlertController alertControllerWithTitle:kDefaultAlertTitle message:@"Please input new node name!" preferredStyle: UIAlertControllerStyleAlert];
    [alertVc addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
        textField.placeholder = @"new name";
    }];
    UIAlertAction *action1 = [UIAlertAction actionWithTitle:kDefaultAlertOK style:UIAlertActionStyleDestructive handler:^(UIAlertAction * _Nonnull action) {
        NSString *nodeName = [[alertVc textFields] objectAtIndex:0].text;
        nodeName = nodeName.removeHeadAndTailSpacePro;
        TelinkLogDebug(@"new nodeName is %@", nodeName);
        if (nodeName == nil || nodeName.length == 0) {
            [weakSelf showTips:@"Node name can not be empty!"];
            return;
        }
        if (nodeName.length > 10) {
            [weakSelf showTips:@"The maximum length of the node name is 10!"];
            return;
        }
        weakSelf.model.name = nodeName;
        [SigDataSource.share saveLocationData];
        [weakSelf showTips:@"Rename success!"];
    }];
    UIAlertAction *action2 = [UIAlertAction actionWithTitle:kDefaultAlertCancel style:UIAlertActionStyleCancel handler:nil];
    [alertVc addAction:action2];
    [alertVc addAction:action1];
    [self presentViewController:alertVc animated:YES completion:nil];
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
#ifdef kIsTelinkCloudSigMeshLib
    SchedulerListViewController *vc = (SchedulerListViewController *)[UIStoryboard initVC:NSStringFromClass(SchedulerListViewController.class) storyboard:@"Cloud"];
#else
    SchedulerListViewController *vc = (SchedulerListViewController *)[UIStoryboard initVC:NSStringFromClass(SchedulerListViewController.class) storyboard:@"Setting"];
#endif
    vc.model = self.model;
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)pushToSubscriptionVC{
    DeviceSubscriptionListViewController *vc = (DeviceSubscriptionListViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_DeviceSubscriptionListViewControllerID storyboard:@"DeviceSetting"];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)pushToDeviceOTA{
#ifdef kIsTelinkCloudSigMeshLib
    OTAVC *vc = (OTAVC *)[UIStoryboard initVC:NSStringFromClass(OTAVC.class) storyboard:@"Cloud"];
#else
    SingleOTAViewController *vc = (SingleOTAViewController *)[UIStoryboard initVC:NSStringFromClass(SingleOTAViewController.class)];
#endif
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

- (void)pushToPrivateBeaconVC {
    PrivateBeaconVC *vc = [[PrivateBeaconVC alloc] init];
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
    TelinkLogDebug(@"");
}

@end
