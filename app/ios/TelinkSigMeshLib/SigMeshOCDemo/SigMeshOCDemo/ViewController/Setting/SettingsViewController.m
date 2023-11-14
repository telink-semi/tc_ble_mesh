/********************************************************************************************************
 * @file     SettingsViewController.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/10/12
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

#import "SettingsViewController.h"
#import "SettingsItemCell.h"
#import "ShareTipsVC.h"
#import "UIButton+extension.h"

/// 1.Default Bound
#define kDefaultBound   @"Enable Private Mode(Default Bound)"
/// Tips Title of Default Bound
#define kDefaultBoundTitle  @"Private Mode"
/// Tips message of Default Bound
#define kDefaultBoundMessage  @"Enable for default bound when key-binding."
/// 2.Provision Mode
#define kProvision_Mode   @"Provision Mode:"
/// Tips Title of Provision Mode
#define kProvisionModeTitle  @"Provision Mode"
/// Tips message of Provision Mode
#define kProvisionModeMessage  @"①.normal(selectable): Enabled for select the device for provision operation;\n②.normal(auto): Enabled for scanning, provisioning and key-binding one by one automatically;\n③.remote provision: If remote provision enabled, provision process will be:\n\n1. Scan for an unprovisioned device, connect and provision;\n\n2. Keep the connection state, send scan command to the device;\n\n3. Some remote devices info will be upload;\n\n4. Check and do provision one by one.\n④.fast provision: Telink private provision profile, scan and setting device by direct connected mesh device."
/// 3.subscription level
#define kSubscriptionLevel   @"Enable subscription level service model ID"
/// Tips Title of subscription level
#define kSubscriptionLevelTitle  @"Sub Level Service"
/// Tips message of subscription level
#define kSubscriptionLevelMessage  @"The flag added in demo v4.1.0.0 and later indicates whether the Service Level ModelID (0x1003) is also subscribed when subscribing to the group address.\nIf the customer has a Mesh remote control product and the Mesh remote control product has a Level control function, this function needs to be enabled on the app side. The default value is off."
/// 4.Extend Bearer Mode
#define kExtend_Bearer_Mode   @"Extend Bearer Mode"
/// Tips Title of Extend Bearer Mode
#define kExtendBearerModeTitle  @"Extend Bearer Mode"
/// Tips message of Extend Bearer Mode
#define kExtendBearerModeMessage  @"1.No Extend: not send extend ble packet.\n2.Extend GATT Only: send extend ble packet to current connected node.\n3.Extend GATT & ADV: send extend ble packet to all node."
/// 5.Use No-OOB Automatically
#define kUseNoOOBAutomatically  @"Use No-OOB Automatically"
/// Tips Title of Use No-OOB Automatically
#define kUseNoOOBAutomaticallyTitle  @"OOB"
/// Tips message of Use No-OOB Automatically
#define kUseNoOOBAutomaticallyMessage  @"Use No-OOB if device UUID not found when device is static-OOB supported."
/// 6.Share Import Complete Action
#define kShareImport  @"Share Import Complete Action"
/// Tips Title of Share Import Complete Action
#define kShareImportTitle  @"Share Import Complete Action"
/// Tips message of Share Import Complete Action
#define kShareImportMessage  @"1.Manual Switch(Default): After successful import, a pop-up prompt will appear indicating whether to switch to the import network.\n2.Auto Switch: Automatically switch to the import network after successful import."
/// 7.Online Status
#define kOnlineStatus  @"Online Status"
/// Tips Title of Online Status
#define kOnlineStatusTitle  @"Online Status"
/// Tips message of Online Status
#define kOnlineStatusMessage  @"Telink private profile for get the status of all nodes, including on、 off and offline status."
/// 8.Directed Security
#define kDirectedSecurity  @"Directed Security"
/// Tips Title of Directed Security
#define kDirectedSecurityTitle  @"Directed Security"
/// Tips message of Directed Security
#define kDirectedSecurityMessage  @"A new configuration item is added in demo v3.3.3.6 and later to mark whether the data sent by the APP is encrypted using The directed security material. The default value is NO."



@interface SettingsViewController ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIButton *resetSettingsButton;
@property (nonatomic, strong) NSMutableArray <NSString *>*titleSource;
@property (nonatomic, strong) NSMutableArray <NSArray <NSString *>*>*itemsSource;
@property (nonatomic, strong) NSMutableArray <NSString *>*tipsTitleSource;
@property (nonatomic, strong) NSMutableArray <NSString *>*tipsMessageSource;
@end

@implementation SettingsViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.title = @"Settings";
    self.resetSettingsButton.backgroundColor = UIColor.telinkButtonRed;
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    self.tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(SettingsItemCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(SettingsItemCell.class)];
    [self refreshSourceAndUI];
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)refreshSourceAndUI {
    NSMutableArray *array = [NSMutableArray array];
    NSMutableArray *items = [NSMutableArray array];
    NSMutableArray *tipsTitles = [NSMutableArray array];
    NSMutableArray *tipsMessage = [NSMutableArray array];
    [array addObject:kDefaultBound];
    [items addObject:@[]];
    [tipsTitles addObject:kDefaultBoundTitle];
    [tipsMessage addObject:kDefaultBoundMessage];

    [array addObject:kProvision_Mode];
    [items addObject:@[@"normal(selectable)", @"normal(auto)", @"remote provision", @"fast provision"]];
    [tipsTitles addObject:kProvisionModeTitle];
    [tipsMessage addObject:kProvisionModeMessage];

    [array addObject:kSubscriptionLevel];
    [items addObject:@[]];
    [tipsTitles addObject:kSubscriptionLevelTitle];
    [tipsMessage addObject:kSubscriptionLevelMessage];

    [array addObject:kExtend_Bearer_Mode];
    [items addObject:@[@"No Extend", @"Extend GATT Only", @"Extend GATT & ADV"]];
    [tipsTitles addObject:kExtendBearerModeTitle];
    [tipsMessage addObject:kExtendBearerModeMessage];


    [array addObject:kUseNoOOBAutomatically];
    [items addObject:@[]];
    [tipsTitles addObject:kUseNoOOBAutomaticallyTitle];
    [tipsMessage addObject:kUseNoOOBAutomaticallyMessage];

    [array addObject:kShareImport];
    [items addObject:@[@"Manual Switch(Default)", @"Auto Switch"]];
    [tipsTitles addObject:kShareImportTitle];
    [tipsMessage addObject:kShareImportMessage];

    [array addObject:[NSString stringWithFormat:@"%@: %@", kOnlineStatus, [DemoCommand isPrivatelyGetOnlineStatus] == YES ? @"ENABLED" : @"DISABLED"]];
    [items addObject:@[]];
    [tipsTitles addObject:kOnlineStatusTitle];
    [tipsMessage addObject:kOnlineStatusMessage];

    [array addObject:kDirectedSecurity];
    [items addObject:@[]];
    [tipsTitles addObject:kDirectedSecurityTitle];
    [tipsMessage addObject:kDirectedSecurityMessage];

    _titleSource = array;
    _itemsSource = items;
    _tipsTitleSource = tipsTitles;
    _tipsMessageSource = tipsMessage;
    [self.tableView reloadData];
}

- (void)pushToTipsVCWithTitle:(NSString *)title message:(NSString *)message {
    ShareTipsVC *vc = (ShareTipsVC *)[UIStoryboard initVC:ViewControllerIdentifiers_ShareTipsVCID storyboard:@"Setting"];
    vc.title = title;
    vc.tipsMessage = message;
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)clickSwitch:(UISwitch *)sender {
    NSNumber *type = [NSNumber numberWithInteger:sender.on == YES ? KeyBindType_Fast : KeyBindType_Normal];
    [[NSUserDefaults standardUserDefaults] setValue:type forKey:kKeyBindType];
    [[NSUserDefaults standardUserDefaults] synchronize];
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

- (void)clickAddStaticOOBDeviceByNoOOBEnableSwitch:(UISwitch *)sender {
    SigDataSource.share.addStaticOOBDeviceByNoOOBEnable = sender.on;
    NSNumber *type = [NSNumber numberWithBool:sender.on];
    [[NSUserDefaults standardUserDefaults] setValue:type forKey:kAddStaticOOBDeviceByNoOOBEnable];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (void)clickDirectedSecuritySwitch:(UISwitch *)sender {
    SigDataSource.share.sendByDirectedSecurity = sender.on;
    NSNumber *type = [NSNumber numberWithBool:sender.on];
    [[NSUserDefaults standardUserDefaults] setValue:type forKey:kDirectedSecurityEnable];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (IBAction)clickResetSettingButton:(UIButton *)sender {
    __weak typeof(self) weakSelf = self;
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Warning" message:@"Reset all settings to default values?" preferredStyle:UIAlertControllerStyleAlert];
    [alertController addAction:[UIAlertAction actionWithTitle:@"Confirm" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        TelinkLogDebug(@"点击确认");
        [[NSUserDefaults standardUserDefaults] setValue:[NSNumber numberWithInteger:KeyBindType_Normal] forKey:kKeyBindType];
        [[NSUserDefaults standardUserDefaults] setValue:[NSNumber numberWithInteger:ProvisionMode_normalSelectable] forKey:kProvisionMode];
        if ([SigDataSource.share.defaultGroupSubscriptionModels containsObject:@(kSigModel_GenericLevelServer_ID)]) {
            [SigDataSource.share.defaultGroupSubscriptionModels removeObject:@(kSigModel_GenericLevelServer_ID)];
        }
        [[NSUserDefaults standardUserDefaults] setValue:[NSNumber numberWithBool:NO] forKey:kSubscribeLevelServiceModelID];
        [[NSUserDefaults standardUserDefaults] setValue:[NSNumber numberWithInt:SigTelinkExtendBearerMode_noExtend] forKey:kExtendBearerMode];
        [[NSUserDefaults standardUserDefaults] setValue:[NSNumber numberWithBool:YES] forKey:kAddStaticOOBDeviceByNoOOBEnable];
        [[NSUserDefaults standardUserDefaults] setValue:[NSNumber numberWithInt:ImportSwitchMode_manual] forKey:kImportCompleteAction];
        [[NSUserDefaults standardUserDefaults] setValue:[NSNumber numberWithBool:NO] forKey:kDirectedSecurityEnable];
        [[NSUserDefaults standardUserDefaults] synchronize];
        [weakSelf refreshSourceAndUI];
        [weakSelf showTips:@"Reset all settings success!"];
    }]];
    [alertController addAction:[UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
        TelinkLogDebug(@"点击取消");

    }]];
    [self presentViewController:alertController animated:YES completion:nil];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    SettingsItemCell *cell = (SettingsItemCell *)[tableView dequeueReusableCellWithIdentifier:NSStringFromClass(SettingsItemCell.class) forIndexPath:indexPath];
    [cell configUIWithTitle:self.titleSource[indexPath.row] items:self.itemsSource[indexPath.row]];
    cell.enableSwitch.hidden = NO;
    __weak typeof(self) weakSelf = self;
    [cell.tipsButton addAction:^(UIButton *button) {
        [weakSelf pushToTipsVCWithTitle:weakSelf.tipsTitleSource[indexPath.row] message:weakSelf.tipsMessageSource[indexPath.row]];
    }];
    if (indexPath.row == 0) {
        // kDefaultBound
        BOOL on = [[[NSUserDefaults standardUserDefaults] valueForKey:kKeyBindType] boolValue];
        //set state
        cell.enableSwitch.on = on;
        //add action
        [cell.enableSwitch addTarget:self action:@selector(clickSwitch:) forControlEvents:UIControlEventValueChanged];
    } else if (indexPath.row == 1) {
        // kProvision_Mode
        NSNumber *provisionMode = [[NSUserDefaults standardUserDefaults] valueForKey:kProvisionMode];
        [cell setSelectIndex:provisionMode.intValue];
        [cell setBackSelectIndexBlock:^(NSInteger selectIndex) {
            NSNumber *mode = [NSNumber numberWithInteger:selectIndex];
            [[NSUserDefaults standardUserDefaults] setValue:mode forKey:kProvisionMode];
            [[NSUserDefaults standardUserDefaults] synchronize];
        }];
    } else if (indexPath.row == 2) {
        // kSubscriptionLevel
        BOOL on = [[[NSUserDefaults standardUserDefaults] valueForKey:kSubscribeLevelServiceModelID] boolValue];
        //set state
        cell.enableSwitch.on = on;
        //add action
        [cell.enableSwitch addTarget:self action:@selector(clickLevelSwitch:) forControlEvents:UIControlEventValueChanged];
    } else if (indexPath.row == 3) {
        //kExtend_Bearer_Mode
        NSNumber *extendBearerMode = [[NSUserDefaults standardUserDefaults] valueForKey:kExtendBearerMode];
        [cell setSelectIndex:extendBearerMode.intValue];
        [cell setBackSelectIndexBlock:^(NSInteger selectIndex) {
            if (selectIndex == SigTelinkExtendBearerMode_noExtend) {
                SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength = kUnsegmentedMessageLowerTransportPDUMaxLength;
            } else {
                SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength = kDLEUnsegmentLength;
            }
            NSNumber *mode = [NSNumber numberWithInteger:selectIndex];
            [[NSUserDefaults standardUserDefaults] setValue:mode forKey:kExtendBearerMode];
            [[NSUserDefaults standardUserDefaults] synchronize];
        }];
    } else if (indexPath.row == 4) {
        // kUseNoOOBAutomatically
        BOOL on = [[[NSUserDefaults standardUserDefaults] valueForKey:kAddStaticOOBDeviceByNoOOBEnable] boolValue];
        //set state
        cell.enableSwitch.on = on;
        //add action
        [cell.enableSwitch addTarget:self action:@selector(clickAddStaticOOBDeviceByNoOOBEnableSwitch:) forControlEvents:UIControlEventValueChanged];
    } else if (indexPath.row == 5) {
        //kShareImport
        NSNumber *importCompleteAction = [[NSUserDefaults standardUserDefaults] valueForKey:kImportCompleteAction];
        [cell setSelectIndex:importCompleteAction.intValue];
        [cell setBackSelectIndexBlock:^(NSInteger selectIndex) {
            NSNumber *mode = [NSNumber numberWithInteger:selectIndex];
            [[NSUserDefaults standardUserDefaults] setValue:mode forKey:kImportCompleteAction];
            [[NSUserDefaults standardUserDefaults] synchronize];
        }];
    } else if (indexPath.row == 6) {
        // kOnlineStatus
        cell.enableSwitch.hidden = YES;
    } else if (indexPath.row == 7) {
        // kDirectedSecurity
        BOOL on = [[[NSUserDefaults standardUserDefaults] valueForKey:kDirectedSecurityEnable] boolValue];
        //set state
        cell.enableSwitch.on = on;
        //add action
        [cell.enableSwitch addTarget:self action:@selector(clickDirectedSecuritySwitch:) forControlEvents:UIControlEventValueChanged];
    }
    return cell;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.titleSource.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return [SettingsItemCell getCellHeightWithItems:self.itemsSource[indexPath.row]];
}

@end
