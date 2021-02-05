/********************************************************************************************************
 * @file     MeshInfoVC.m
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
//  MeshInfoVC.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2019/1/24.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import "MeshInfoVC.h"
#import "OOBListVC.h"
#import "AppKeyListVC.h"
#import "NetKeyListVC.h"
#import "InfoSwitchCell.h"
#import "InfoNextCell.h"
#import "InfoButtonCell.h"

@interface MeshInfoVC ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <NSString *>*source;
@property (assign, nonatomic) UInt32 ivIndex;
@end

@implementation MeshInfoVC

- (void)viewDidLoad{
    [super viewDidLoad];
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_InfoSwitchCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_InfoSwitchCellID];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_InfoNextCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_InfoNextCellID];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_InfoButtonCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_InfoButtonCellID];

    self.title = @"Mesh Info";
    _ivIndex = [LibTools uint32From16String:SigDataSource.share.ivIndex];
    [self refreshSourceAndUI];
//    #ifdef kExist
//    self.RPLabelTopLayout.constant = kExistRemoteProvision == YES ? 30 : -30;
//    #endif
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
    [array addObject:[NSString stringWithFormat:@"IV Index: 0x%08X",(unsigned int)_ivIndex]];
    [array addObject:[NSString stringWithFormat:@"Sequence Number: 0x%06X",(unsigned int)SigDataSource.share.getCurrentProvisionerIntSequenceNumber]];
    [array addObject:[NSString stringWithFormat:@"Local Address: 0x%04X",(unsigned int)SigDataSource.share.curLocationNodeModel.address]];
    [array addObject:@"NetKey List"];
    [array addObject:@"AppKey List"];
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

- (void)clickDLEEnableSwitch:(UISwitch *)sender {
    SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength = sender.on ? kDLEUnsegmentLength : kUnsegmentedMessageLowerTransportPDUMaxLength;
    NSNumber *type = [NSNumber numberWithBool:sender.on];
    [[NSUserDefaults standardUserDefaults] setValue:type forKey:kDLEType];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (void)clickOOBButton {
    OOBListVC *vc = [[OOBListVC alloc] init];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)clickAppKeyListButton {
    AppKeyListVC *vc = [[AppKeyListVC alloc] init];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)clickNetKeyListButton {
    NetKeyListVC *vc = [[NetKeyListVC alloc] init];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)clickIvUpdate:(UIButton *)sender {
    if (!SigBearer.share.isOpen) {
        [self showTips:@"Mesh is disconnected."];
        return;
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        NSString *t = @"sending ivUpdate...";
        [ShowTipsHandle.share show:t];
    });

//    if (SigDataSource.share.getCurrentProvisionerIntSequenceNumber < 0xc00000) {
//        [SigDataSource.share setLocationSno:0xc00000];
//    }
    
    __weak typeof(self) weakSelf = self;
    NSOperationQueue *oprationQueue = [[NSOperationQueue alloc] init];
    [oprationQueue addOperationWithBlock:^{
        //这个block语句块在子线程中执行
        [SDKLibCommand updateIvIndexWithKeyRefreshFlag:NO ivUpdateActive:YES networkId:SigDataSource.share.curNetkeyModel.networkId ivIndex:weakSelf.ivIndex + 1 usingNetworkKey:SigDataSource.share.curNetkeyModel];
        
//        [NSThread sleepForTimeInterval:2.0];
//
        [SigDataSource.share setLocationSno:0x01];
//        [SDKLibCommand updateIvIndexWithKeyRefreshFlag:NO ivUpdateActive:NO networkId:SigDataSource.share.curNetkeyModel.networkId ivIndex:weakSelf.ivIndex + 1 usingNetworkKey:SigDataSource.share.curNetkeyModel];
//
        [SigDataSource.share updateIvIndexString:[NSString stringWithFormat:@"%08lX",(unsigned long)weakSelf.ivIndex+1]];
//
        dispatch_async(dispatch_get_main_queue(), ^{
            [ShowTipsHandle.share delayHidden:0.5];
//            SigDataSource.share.curNetkeyModel.ivIndex.index = weakSelf.ivIndex + 1;
//            SigDataSource.share.curNetkeyModel.ivIndex.updateActive = YES;
//            [weakSelf refreshSourceAndUI];
//            [weakSelf showTips:@"ivUpdate success."];
        });

    }];

}

- (void)ivUpdateTimeout {
    dispatch_async(dispatch_get_main_queue(), ^{
        [ShowTipsHandle.share hidden];
        [self showTips:@"ivUpdate fail."];
    });
}

- (void)showTips:(NSString *)tips{
    dispatch_async(dispatch_get_main_queue(), ^{
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Hits" message:tips preferredStyle:UIAlertControllerStyleAlert];
        [alertController addAction:[UIAlertAction actionWithTitle:@"Sure" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            TeLogDebug(@"点击确认");
        }]];
        [self presentViewController:alertController animated:YES completion:nil];
    });
}


- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.row <= 4 || indexPath.row == 6) {
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
        } else if (indexPath.row == 6) {
            NSNumber *DLEEnable = [[NSUserDefaults standardUserDefaults] valueForKey:kDLEType];
            cell.showSwitch.on = DLEEnable.boolValue;
            [cell.showSwitch addTarget:self action:@selector(clickDLEEnableSwitch:) forControlEvents:UIControlEventValueChanged];
        }
        return cell;
    } else if (indexPath.row == 5 || indexPath.row == 10 || indexPath.row == 11) {
        InfoNextCell *cell = (InfoNextCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_InfoNextCellID forIndexPath:indexPath];
        cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
        cell.showLabel.text = _source[indexPath.row];
        return cell;
    } else if (indexPath.row == 7 || indexPath.row == 8 || indexPath.row == 9) {
        InfoButtonCell *cell = (InfoButtonCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_InfoButtonCellID forIndexPath:indexPath];
        cell.accessoryType = UITableViewCellAccessoryNone;
        cell.showLabel.text = _source[indexPath.row];
        if (indexPath.row == 7) {
            [cell.showButton setTitle:@"+1" forState:UIControlStateNormal];
            [cell.showButton addTarget:self action:@selector(clickIvUpdate:) forControlEvents:UIControlEventTouchUpInside];
        }
        cell.showButton.hidden = YES;
        return cell;
    } else {
        InfoNextCell *cell = (InfoNextCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_InfoNextCellID forIndexPath:indexPath];
        cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
        cell.showLabel.text = _source[indexPath.row];
        return cell;
    }
    return nil;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.row == 5) {
        [self clickOOBButton];
    } else if (indexPath.row == 10) {
        [self clickNetKeyListButton];
    } else if (indexPath.row == 11) {
        [self clickAppKeyListButton];
   }
    UITableViewCell *cell = [tableView cellForRowAtIndexPath:indexPath];
    cell.selected = NO;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.source.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
#ifndef kExist
    if (indexPath.row == 0) {
        return 0.1;
    }
#endif
    return 51.0;
}

@end
