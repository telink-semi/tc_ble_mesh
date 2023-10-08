/********************************************************************************************************
 * @file     SettingsVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2021/3/8
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

#import "SettingsVC.h"
#import "OOBListVC.h"
#import "AppKeyListVC.h"
#import "NetKeyListVC.h"
#import "InfoSwitchCell.h"
#import "InfoNextCell.h"
#import "ExtendBearerModeCell.h"
#import "CertificateListVC.h"
#import "UIViewController+Message.h"
#import "ProxyFilterVC.h"
#import "ShareTipsVC.h"
#import "UIButton+extension.h"

@interface SettingsVC ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIButton *resetMeshButton;
@property (nonatomic, strong) NSMutableArray <NSString *>*source;
@property (assign, nonatomic) UInt32 ivIndex;
@end

@implementation SettingsVC

- (void)viewDidLoad {
    [super viewDidLoad];
    self.resetMeshButton.backgroundColor = UIColor.telinkButtonRed;
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_InfoSwitchCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_InfoSwitchCellID];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_InfoNextCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_InfoNextCellID];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_ExtendBearerModeCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_ExtendBearerModeCellID];

    self.title = @"Settings";
    _ivIndex = [LibTools uint32From16String:SigDataSource.share.ivIndex];
    [self refreshSourceAndUI];
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)refreshSourceAndUI {
    NSMutableArray *array = [NSMutableArray array];
    [array addObject:kAutoProvisionTitle];
    [array addObject:kRemoteProvisionTitle];
    [array addObject:kFastProvisionTitle];
    [array addObject:kDefaultBoundTitle];
    [array addObject:kOnlineStatusTitle];
    [array addObject:kSubscribeLevelServerModelIDTitle];
    [array addObject:kUseNoOOBAutomaticallyTitle];
    [array addObject:kDirectedSecurityTitle];
    [array addObject:@"OOB Database"];
    [array addObject:@"Proxy Filter"];
    [array addObject:@"Root Certificate"];
    [array addObject:@"Enable DLE Mode Extend Bearer"];
    _source = array;
    [self.tableView reloadData];
}

- (void)pushToTipsVCWithTitle:(NSString *)title message:(NSString *)message {
    ShareTipsVC *vc = (ShareTipsVC *)[UIStoryboard initVC:ViewControllerIdentifiers_ShareTipsVCID storyboard:@"Setting"];
    vc.title = title;
    vc.tipsMessage = message;
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)clickAutoProvisionSwitch:(UISwitch *)sender {
    NSNumber *type = [NSNumber numberWithBool:sender.on];
    [[NSUserDefaults standardUserDefaults] setValue:type forKey:kAutoProvision];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (void)clickRemoteSwitch:(UISwitch *)sender {
    NSNumber *type = [NSNumber numberWithBool:sender.on];
    [[NSUserDefaults standardUserDefaults] setValue:type forKey:kRemoteAddType];
    [[NSUserDefaults standardUserDefaults] synchronize];
    NSNumber *fastProvision = [[NSUserDefaults standardUserDefaults] valueForKey:kFastAddType];
    if (sender.on && fastProvision.boolValue) {
        [[NSUserDefaults standardUserDefaults] setValue:[NSNumber numberWithBool:NO] forKey:kFastAddType];
        [[NSUserDefaults standardUserDefaults] synchronize];
        [self refreshSourceAndUI];
    }
}

- (void)clickSwitch:(UISwitch *)sender {
    NSNumber *type = [NSNumber numberWithInteger:sender.on == YES ? KeyBindType_Fast : KeyBindType_Normal];
    [[NSUserDefaults standardUserDefaults] setValue:type forKey:kKeyBindType];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (void)clickFastProvisionSwitch:(UISwitch *)sender {
    NSNumber *type = [NSNumber numberWithBool:sender.on];
    [[NSUserDefaults standardUserDefaults] setValue:type forKey:kFastAddType];
    [[NSUserDefaults standardUserDefaults] synchronize];
    NSNumber *remote = [[NSUserDefaults standardUserDefaults] valueForKey:kRemoteAddType];
    if (sender.on && remote.boolValue) {
        [[NSUserDefaults standardUserDefaults] setValue:[NSNumber numberWithBool:NO] forKey:kRemoteAddType];
        [[NSUserDefaults standardUserDefaults] synchronize];
        [self refreshSourceAndUI];
    }
}

- (void)clickLevelSwitch:(UISwitch *)sender {
    if (sender.on) {
        if (![SigDataSource.share.defaultGroupSubscriptionModels containsObject:@(kSigModel_GenericLevelServer_ID)]) {
            [SigDataSource.share.defaultGroupSubscriptionModels addObject:@(kSigModel_GenericLevelServer_ID)];
        }
    } else {
        if ([SigDataSource.share.defaultGroupSubscriptionModels containsObject:@(kSigModel_GenericLevelServer_ID)]) {
            [SigDataSource.share.defaultGroupSubscriptionModels removeObject:@(kSigModel_GenericLevelServer_ID)];
        }
    }
    NSNumber *type = [NSNumber numberWithBool:sender.on];
    [[NSUserDefaults standardUserDefaults] setValue:type forKey:kSubscribeLevelServiceModelID];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (void)clickOnlineStatusSwitch:(UISwitch *)sender {
    NSNumber *type = [NSNumber numberWithBool:sender.on];
    [[NSUserDefaults standardUserDefaults] setValue:type forKey:kGetOnlineStatusType];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (void)clickAddStaticOOBDeviceByNoOOBEnableSwitch:(UISwitch *)sender {
    SigDataSource.share.addStaticOOBDeviceByNoOOBEnable = sender.on;
    NSNumber *type = [NSNumber numberWithBool:sender.on];
    [[NSUserDefaults standardUserDefaults] setValue:type forKey:kAddStaticOOBDeviceByNoOOBEnable];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (void)clickOOBButton {
    OOBListVC *vc = [[OOBListVC alloc] init];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)clickProxyFilterButton {
    ProxyFilterVC *vc = [[ProxyFilterVC alloc] init];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)clickDirectedSecuritySwitch:(UISwitch *)sender {
    SigDataSource.share.sendByDirectedSecurity = sender.on;
    NSNumber *type = [NSNumber numberWithBool:sender.on];
    [[NSUserDefaults standardUserDefaults] setValue:type forKey:kDirectedSecurityEnable];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (IBAction)clickResetMesh:(UIButton *)sender {
    __weak typeof(self) weakSelf = self;
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Warning" message:@"Wipe all mesh info?" preferredStyle:UIAlertControllerStyleAlert];
    [alertController addAction:[UIAlertAction actionWithTitle:@"Confirm" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        TeLogDebug(@"点击确认");
        //清除SigDataSource.share里面的所有参数（包括scanList、sequenceNumber、sequenceNumberOnDelegate），并随机生成新的默认参数。
        [SigDataSource.share resetMesh];
        //清除可能存在的未完成的MeshOTA相关记录
        [[NSUserDefaults standardUserDefaults] setValue:@(0) forKey:kDistributorAddress];
        [[NSUserDefaults standardUserDefaults] removeObjectForKey:kDistributorPolicy];
        [[NSUserDefaults standardUserDefaults] removeObjectForKey:kUpdateNodeAddresses];
        [[NSUserDefaults standardUserDefaults] synchronize];
        [weakSelf showTips:@"Reset mesh success!"];
    }]];
    [alertController addAction:[UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
        TeLogDebug(@"点击取消");
        
    }]];
    [self presentViewController:alertController animated:YES completion:nil];
}

- (void)clickRootCertificateButton {
    CertificateListVC *vc = [[CertificateListVC alloc] init];
    [self.navigationController pushViewController:vc animated:YES];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    __weak typeof(self) weakSelf = self;
    if (indexPath.row <= 7) {
        NSArray *titles = @[kAutoProvisionTitle, kRemoteProvisionTitle, kFastProvisionTitle, kDefaultBoundTitle, kOnlineStatusTitle, kSubscribeLevelServerModelIDTitle, kUseNoOOBAutomaticallyTitle, kDirectedSecurityTitle];
        NSArray *messages = @[kAutoProvisionMessage, kRemoteProvisionMessage, kFastProvisionMessage, kDefaultBoundMessage, kOnlineStatusMessage, kSubscribeLevelServerModelIDMessage, kUseNoOOBAutomaticallyMessage, kDirectedSecurityMessage];
        InfoSwitchCell *cell = (InfoSwitchCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_InfoSwitchCellID forIndexPath:indexPath];
        cell.accessoryType = UITableViewCellAccessoryNone;
        cell.showLabel.text = _source[indexPath.row];
        [cell.tipsButton addAction:^(UIButton *button) {
            [weakSelf pushToTipsVCWithTitle:titles[indexPath.row] message:messages[indexPath.row]];
        }];
        BOOL on = NO;
        if (indexPath.row == 0) {
            on = [[[NSUserDefaults standardUserDefaults] valueForKey:kAutoProvision] boolValue];
            [cell.showSwitch addTarget:self action:@selector(clickAutoProvisionSwitch:) forControlEvents:UIControlEventValueChanged];
        } else if (indexPath.row == 1) {
            on = [[[NSUserDefaults standardUserDefaults] valueForKey:kRemoteAddType] boolValue];
            [cell.showSwitch addTarget:self action:@selector(clickRemoteSwitch:) forControlEvents:UIControlEventValueChanged];
        } else if (indexPath.row == 2) {
            on = [[[NSUserDefaults standardUserDefaults] valueForKey:kFastAddType] boolValue];
            [cell.showSwitch addTarget:self action:@selector(clickFastProvisionSwitch:) forControlEvents:UIControlEventValueChanged];
        } else if (indexPath.row == 3) {
            on = [[[NSUserDefaults standardUserDefaults] valueForKey:kKeyBindType] boolValue];
            [cell.showSwitch addTarget:self action:@selector(clickSwitch:) forControlEvents:UIControlEventValueChanged];
        } else if (indexPath.row == 4) {
            on = [[[NSUserDefaults standardUserDefaults] valueForKey:kGetOnlineStatusType] boolValue];
            [cell.showSwitch addTarget:self action:@selector(clickOnlineStatusSwitch:) forControlEvents:UIControlEventValueChanged];
        } else if (indexPath.row == 5) {
            on = [[[NSUserDefaults standardUserDefaults] valueForKey:kSubscribeLevelServiceModelID] boolValue];
            [cell.showSwitch addTarget:self action:@selector(clickLevelSwitch:) forControlEvents:UIControlEventValueChanged];
        } else if (indexPath.row == 6) {
            on = [[[NSUserDefaults standardUserDefaults] valueForKey:kAddStaticOOBDeviceByNoOOBEnable] boolValue];
            [cell.showSwitch addTarget:self action:@selector(clickAddStaticOOBDeviceByNoOOBEnableSwitch:) forControlEvents:UIControlEventValueChanged];
        } else if (indexPath.row == 7) {
            on = [[[NSUserDefaults standardUserDefaults] valueForKey:kDirectedSecurityEnable] boolValue];
            [cell.showSwitch addTarget:self action:@selector(clickDirectedSecuritySwitch:) forControlEvents:UIControlEventValueChanged];
        }
        cell.showSwitch.on = on;
        return cell;
    } else if (indexPath.row >= 8 && indexPath.row <= 10) {
        InfoNextCell *cell = (InfoNextCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_InfoNextCellID forIndexPath:indexPath];
        cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
        cell.showLabel.text = _source[indexPath.row];
        return cell;
    } else if (indexPath.row == 11) {
        ExtendBearerModeCell *cell = (ExtendBearerModeCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_ExtendBearerModeCellID forIndexPath:indexPath];
        cell.accessoryType = UITableViewCellAccessoryNone;
        NSNumber *extendBearerMode = [[NSUserDefaults standardUserDefaults] valueForKey:kExtendBearerMode];
        [cell setExtendBearerMode:extendBearerMode.intValue];
        [cell setBlock:^(SigTelinkExtendBearerMode extendBearerMode) {
            if (extendBearerMode == SigTelinkExtendBearerMode_noExtend) {
                SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength = kUnsegmentedMessageLowerTransportPDUMaxLength;
            } else {
                SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength = kDLEUnsegmentLength;
            }
            NSNumber *mode = [NSNumber numberWithInt:extendBearerMode];
            [[NSUserDefaults standardUserDefaults] setValue:mode forKey:kExtendBearerMode];
            [[NSUserDefaults standardUserDefaults] synchronize];
        }];
        return cell;
    }
    return [[UITableViewCell alloc] init];
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.row == 8) {
        [self clickOOBButton];
    } else if (indexPath.row == 9) {
        [self clickProxyFilterButton];
    } else if (indexPath.row == 10) {
        [self clickRootCertificateButton];
    }
    UITableViewCell *cell = [tableView cellForRowAtIndexPath:indexPath];
    cell.selected = NO;
    [tableView reloadData];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.source.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.row == 11) {
        return 149.0;
    } else {
        return 51.0;
    }
}

@end
