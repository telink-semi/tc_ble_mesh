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
    [array addObject:@"remote provision"];
    [array addObject:@"fast provision"];
    [array addObject:@"fast bind(Default Bound)"];
    [array addObject:@"online status from uuid"];
    [array addObject:@"Add staticOOB device by noOOB provision"];
    [array addObject:@"OOB Database"];
    [array addObject:[NSString stringWithFormat:@"netKey:%@",SigDataSource.share.curNetkeyModel.key]];
    [array addObject:[NSString stringWithFormat:@"appKey:%@",SigDataSource.share.curAppkeyModel.key]];
    [array addObject:[NSString stringWithFormat:@"ivIndex: 0x%x",(unsigned int)_ivIndex]];
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

- (void)clickCopyNetKey:(UIButton *)sender {
    UIPasteboard*pasteboard = [UIPasteboard generalPasteboard];
    pasteboard.string = SigDataSource.share.curNetkeyModel.key;
    [self showTips:@"copy netKey success"];
}

- (void)clickCopyAppKey:(UIButton *)sender {
    UIPasteboard*pasteboard = [UIPasteboard generalPasteboard];
    pasteboard.string = SigDataSource.share.curAppkeyModel.key;
    [self showTips:@"copy appKey success"];
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

    if (SigDataSource.share.getCurrentProvisionerIntSequenceNumber < 0xc10000) {
        [SigDataSource.share setLocationSno:0xc10000];
    }
    
    __weak typeof(self) weakSelf = self;
    NSOperationQueue *oprationQueue = [[NSOperationQueue alloc] init];
    [oprationQueue addOperationWithBlock:^{
        //这个block语句块在子线程中执行
        NSLog(@"oprationQueue");
        [SDKLibCommand updateIvIndexWithKeyRefreshFlag:NO ivUpdateActive:YES networkId:SigDataSource.share.curNetkeyModel.networkId ivIndex:weakSelf.ivIndex + 1 usingNetworkKey:SigDataSource.share.curNetkeyModel];
        
        [NSThread sleepForTimeInterval:2.0];
        
        [SigDataSource.share setLocationSno:0x01];
        [SDKLibCommand updateIvIndexWithKeyRefreshFlag:NO ivUpdateActive:NO networkId:SigDataSource.share.curNetkeyModel.networkId ivIndex:weakSelf.ivIndex + 1 usingNetworkKey:SigDataSource.share.curNetkeyModel];

        [SigDataSource.share updateIvIndexString:[NSString stringWithFormat:@"%08X",weakSelf.ivIndex+1]];
        
        dispatch_async(dispatch_get_main_queue(), ^{
            [ShowTipsHandle.share delayHidden:0.5];
            SigDataSource.share.curNetkeyModel.ivIndex.index = weakSelf.ivIndex + 1;
            SigDataSource.share.curNetkeyModel.ivIndex.updateActive = YES;
            [weakSelf refreshSourceAndUI];
            [weakSelf showTips:@"ivUpdate success."];
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
    if (indexPath.row <= 4) {
        InfoSwitchCell *cell = (InfoSwitchCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_InfoSwitchCellID forIndexPath:indexPath];
        cell.accessoryType = UITableViewCellAccessoryNone;
        cell.showLabel.text = _source[indexPath.row];
        if (indexPath.row == 0) {
            NSNumber *remote = [[NSUserDefaults standardUserDefaults] valueForKey:kRemoteAddType];
            cell.showSwitch.on = remote.boolValue;
            [cell.showSwitch addTarget:self action:@selector(clickRemoteSwitch:) forControlEvents:UIControlEventValueChanged];
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
    } else if (indexPath.row > 5) {
        InfoButtonCell *cell = (InfoButtonCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_InfoButtonCellID forIndexPath:indexPath];
        cell.accessoryType = UITableViewCellAccessoryNone;
        cell.showLabel.text = _source[indexPath.row];
        if (indexPath.row == 6) {
            [cell.showButton setTitle:@"copy" forState:UIControlStateNormal];
            [cell.showButton addTarget:self action:@selector(clickCopyNetKey:) forControlEvents:UIControlEventTouchUpInside];
        } else if (indexPath.row == 7) {
            [cell.showButton setTitle:@"copy" forState:UIControlStateNormal];
            [cell.showButton addTarget:self action:@selector(clickCopyAppKey:) forControlEvents:UIControlEventTouchUpInside];
        } else if (indexPath.row == 8) {
            [cell.showButton setTitle:@"+1" forState:UIControlStateNormal];
            [cell.showButton addTarget:self action:@selector(clickIvUpdate:) forControlEvents:UIControlEventTouchUpInside];
        }
        return cell;
    }
    return nil;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.row == 5) {
        [self clickOOBButton];
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
