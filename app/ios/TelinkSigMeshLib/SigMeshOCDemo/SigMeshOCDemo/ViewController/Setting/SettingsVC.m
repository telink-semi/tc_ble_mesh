/********************************************************************************************************
 * @file     SettingsVC.m
 *
 * @brief    A concise description.
 *
 * @author       梁家誌
 * @date         2021/3/8
 *
 * @par      Copyright © 2021 Telink Semiconductor (Shanghai) Co., Ltd. All rights reserved.
 *
 *           The information contained herein is confidential property of Telink
 *           Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *           of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *           Co., Ltd. and the licensee or the terms described here-in. This heading
 *           MUST NOT be removed from this file.
 *
 *           Licensee shall not delete, modify or alter (or permit any third party to delete, modify, or  
 *           alter) any information contained herein in whole or in part except as expressly authorized  
 *           by Telink semiconductor (shanghai) Co., Ltd. Otherwise, licensee shall be solely responsible  
 *           for any claim to the extent arising out of or relating to such deletion(s), modification(s)  
 *           or alteration(s).
 *
 *           Licensees are granted free, non-transferable use of the information in this
 *           file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/

#import "SettingsVC.h"
#import "OOBListVC.h"
#import "AppKeyListVC.h"
#import "NetKeyListVC.h"
#import "InfoSwitchCell.h"
#import "InfoNextCell.h"
#import "ExtendBearerModeCell.h"
#import "CertificateListVC.h"

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
    NSNumber *type = [NSNumber numberWithInteger:sender.on == YES ? KeyBindTpye_Fast : KeyBindTpye_Normal];
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

- (void)clickAddStaticOOBDevcieByNoOOBEnableSwitch:(UISwitch *)sender {
    SigDataSource.share.addStaticOOBDevcieByNoOOBEnable = sender.on;
    NSNumber *type = [NSNumber numberWithBool:sender.on];
    [[NSUserDefaults standardUserDefaults] setValue:type forKey:kAddStaticOOBDevcieByNoOOBEnable];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (void)clickOOBButton {
    OOBListVC *vc = [[OOBListVC alloc] init];
    [self.navigationController pushViewController:vc animated:YES];
}

- (IBAction)clickResetMesh:(UIButton *)sender {
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Warning" message:@"Wipe all mesh info?" preferredStyle:UIAlertControllerStyleAlert];
    [alertController addAction:[UIAlertAction actionWithTitle:@"Confirm" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        TeLogDebug(@"点击确认");
        //清除SigDataSource.share里面的所有参数（包括scanList、sequenceNumber、sequenceNumberOnDelegate），并随机生成新的默认参数。
        [SigDataSource.share resetMesh];
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
    if (indexPath.row <= 4) {
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
            NSNumber *addStaticOOBDevcieByNoOOBEnable = [[NSUserDefaults standardUserDefaults] valueForKey:kAddStaticOOBDevcieByNoOOBEnable];
            cell.showSwitch.on = addStaticOOBDevcieByNoOOBEnable.boolValue;
            [cell.showSwitch addTarget:self action:@selector(clickAddStaticOOBDevcieByNoOOBEnableSwitch:) forControlEvents:UIControlEventValueChanged];
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
    return nil;
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
    if (indexPath.row == 0 || indexPath.row == 7) {
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
