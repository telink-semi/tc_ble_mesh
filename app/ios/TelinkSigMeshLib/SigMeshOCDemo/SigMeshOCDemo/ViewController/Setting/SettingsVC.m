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

@interface SettingsVC ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <NSString *>*source;
@property (assign, nonatomic) UInt32 ivIndex;
@end

@implementation SettingsVC

- (void)viewDidLoad {
    [super viewDidLoad];
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
    [array addObject:@"Enable remote provision"];
    [array addObject:@"Enable fast provision"];
    [array addObject:@"Enable fast bind(Default Bound)"];
    [array addObject:@"Enable online status from uuid"];
    [array addObject:@"try add staticOOB device by noOOB provision"];
    [array addObject:@"OOB Database"];
    [array addObject:@"Enable DLE Mode Extend Bearer"];
    [array addObject:@"Root Certificate"];
    [array addObject:@"Directed Security"];
    _source = array;
    [self.tableView reloadData];
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
        [weakSelf showTips:@"Reset mesh success!"];
    }]];
    [alertController addAction:[UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
        TeLogDebug(@"点击取消");
        
    }]];
    [self presentViewController:alertController animated:YES completion:nil];
}

- (void)showTips:(NSString *)message{
    __weak typeof(self) weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        [weakSelf showAlertSureWithTitle:@"Hits" message:message sure:^(UIAlertAction *action) {
            
        }];
    });
}

- (void)clickRootCertificateButton {
    CertificateListVC *vc = [[CertificateListVC alloc] init];
    [self.navigationController pushViewController:vc animated:YES];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.row <= 4 || indexPath.row == 8) {
        InfoSwitchCell *cell = (InfoSwitchCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_InfoSwitchCellID forIndexPath:indexPath];
        cell.accessoryType = UITableViewCellAccessoryNone;
        cell.showLabel.text = _source[indexPath.row];
        if (indexPath.row == 0) {
            NSNumber *remote = [[NSUserDefaults standardUserDefaults] valueForKey:kRemoteAddType];
            cell.showSwitch.on = remote.boolValue;
            [cell.showSwitch addTarget:self action:@selector(clickRemoteSwitch:) forControlEvents:UIControlEventValueChanged];
#ifndef kExist
            cell.contentView.hidden = YES;
#endif
        } else if (indexPath.row == 1) {
            NSNumber *fastProvision = [[NSUserDefaults standardUserDefaults] valueForKey:kFastAddType];
            cell.showSwitch.on = fastProvision.boolValue;
            [cell.showSwitch addTarget:self action:@selector(clickFastProvisionSwitch:) forControlEvents:UIControlEventValueChanged];
        } else if (indexPath.row == 2) {
            NSNumber *type = [[NSUserDefaults standardUserDefaults] valueForKey:kKeyBindType];
            cell.showSwitch.on = type.boolValue;
            [cell.showSwitch addTarget:self action:@selector(clickSwitch:) forControlEvents:UIControlEventValueChanged];
        } else if (indexPath.row == 3) {
            NSNumber *online = [[NSUserDefaults standardUserDefaults] valueForKey:kGetOnlineStatusType];
            cell.showSwitch.on = online.boolValue;
            [cell.showSwitch addTarget:self action:@selector(clickOnlineStatusSwitch:) forControlEvents:UIControlEventValueChanged];
        } else if (indexPath.row == 4) {
            NSNumber *addStaticOOBDeviceByNoOOBEnable = [[NSUserDefaults standardUserDefaults] valueForKey:kAddStaticOOBDeviceByNoOOBEnable];
            cell.showSwitch.on = addStaticOOBDeviceByNoOOBEnable.boolValue;
            [cell.showSwitch addTarget:self action:@selector(clickAddStaticOOBDeviceByNoOOBEnableSwitch:) forControlEvents:UIControlEventValueChanged];
        } else if (indexPath.row == 8) {
            NSNumber *directedSecurityEnable = [[NSUserDefaults standardUserDefaults] valueForKey:kDirectedSecurityEnable];
            cell.showSwitch.on = directedSecurityEnable.boolValue;
            [cell.showSwitch addTarget:self action:@selector(clickDirectedSecuritySwitch:) forControlEvents:UIControlEventValueChanged];
#ifndef kExist
            cell.contentView.hidden = YES;
#endif
        }
        return cell;
    } else if (indexPath.row == 5) {
        InfoNextCell *cell = (InfoNextCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_InfoNextCellID forIndexPath:indexPath];
        cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
        cell.showLabel.text = _source[indexPath.row];
        return cell;
    } else if (indexPath.row == 6) {
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
    } else if (indexPath.row == 7) {
        InfoNextCell *cell = (InfoNextCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_InfoNextCellID forIndexPath:indexPath];
        cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
        cell.showLabel.text = _source[indexPath.row];
#ifndef kExist
            cell.contentView.hidden = YES;
#endif
        return cell;
    }
    return [[UITableViewCell alloc] init];
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.row == 5) {
        [self clickOOBButton];
    } else if (indexPath.row == 7) {
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
#ifndef kExist
    if (indexPath.row == 0 || indexPath.row == 7 || indexPath.row == 8) {
        return 0.1;
    }
#endif
    if (indexPath.row == 6) {
        return 149.0;
    } else {
        return 51.0;
    }
}

@end
