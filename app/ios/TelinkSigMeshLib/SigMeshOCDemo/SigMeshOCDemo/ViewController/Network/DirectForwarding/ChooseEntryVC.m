/********************************************************************************************************
 * @file     ChooseEntryVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2022/3/24
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "ChooseEntryVC.h"
#import "UIViewController+Message.h"
#import "EntryCell.h"
#import "UIButton+extension.h"

@interface ChooseEntryVC ()<UITableViewDelegate,UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIButton *addEntryButton;
@property (nonatomic,strong) NSMutableArray <NSNumber *>*allAddresses;
@property (nonatomic, strong) NSMutableArray <NSNumber *>*selectAddresses;
@end

@implementation ChooseEntryVC

- (IBAction)clickConfirm:(UIButton *)sender {
    TelinkLogDebug(@"");
    if (self.backAddressCallback) {
        self.backAddressCallback(self.selectAddresses);
        [self.navigationController popViewControllerAnimated:YES];
    }
}

#pragma mark - Life method
- (void)normalSetting{
    [super normalSetting];
    self.title = @"Choose Table Entry";
    self.addEntryButton.backgroundColor = UIColor.telinkButtonBlue;
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_EntryCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_EntryCellID];
    self.allAddresses = [NSMutableArray array];
    self.selectAddresses = [NSMutableArray array];
    if (self.oldAddresses) {
        [self.selectAddresses addObjectsFromArray:self.oldAddresses];
    }

    if (self.entryType == EntryType_tableEntry) {
        NSMutableArray *allNodes = [NSMutableArray array];
        NSArray *arr = [NSArray arrayWithArray:SigDataSource.share.curNodes];
        for (SigNodeModel *node in arr) {
            [allNodes addObject:@(node.address)];
        }
        [self.allAddresses addObjectsFromArray:allNodes];
    } else if (self.entryType == EntryType_originRange) {
        NSMutableArray *allNodes = [NSMutableArray array];
        NSArray *arr = [NSArray arrayWithArray:SigDataSource.share.nodes];
        for (SigNodeModel *node in arr) {
            [allNodes addObject:@(node.address)];
        }
        [self.allAddresses addObjectsFromArray:allNodes];
    } else if (self.entryType == EntryType_targetRange) {
        NSMutableArray *allNodes = [NSMutableArray array];
        NSArray *arr = [NSArray arrayWithArray:SigDataSource.share.nodes];
        for (SigNodeModel *node in arr) {
            [allNodes addObject:@(node.address)];
        }
        [self.allAddresses addObjectsFromArray:allNodes];
        NSMutableArray *allGroups = [NSMutableArray array];
        arr = [NSArray arrayWithArray:SigDataSource.share.getAllShowGroupList];
        for (SigGroupModel *group in arr) {
            [allGroups addObject:@(group.intAddress)];
        }
        [self.allAddresses addObjectsFromArray:allGroups];
    }
}

-(void)dealloc{
    TelinkLogDebug(@"%s",__func__);
}

#pragma mark - UITableView
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.allAddresses.count + (self.entryType == EntryType_tableEntry ? 1 : 0);
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    EntryCell *cell = (EntryCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_EntryCellID forIndexPath:indexPath];
    cell.selectionStyle = UITableViewCellSelectionStyleDefault;
    cell.onButton.backgroundColor = UIColor.telinkButtonBlue;
    cell.offButton.backgroundColor = UIColor.telinkButtonBlue;
    [cell.chooseButton setImage:[UIImage imageNamed:@"unxuan"] forState:UIControlStateNormal];
    [cell.chooseButton setImage:[UIImage imageNamed:@"xuan"] forState:UIControlStateSelected];
    if (self.entryType == EntryType_tableEntry && indexPath.row == 0) {
        NSArray *arr = [NSArray arrayWithArray:self.allAddresses];
        NSInteger count = 0;
        for (NSNumber *num in arr) {
            SigProvisionerModel *provisioner = [SigDataSource.share getProvisionerModelWithAddress:num.intValue];
            SigNodeModel *model = [SigDataSource.share getNodeWithAddress:num.intValue];
            if (provisioner == nil && model != nil && model.isSensor == NO && model.isRemote == NO && model.isKeyBindSuccess && model.state == DeviceStateOutOfLine) {
                continue;
            }
            count ++;
        }
        cell.onButton.hidden = cell.offButton.hidden = YES;
        cell.chooseButton.selected = count == self.selectAddresses.count;
        cell.nameLabel.text = @"choose all";
        return cell;
    }
    UInt16 address = [self.allAddresses[indexPath.row - (self.entryType == EntryType_tableEntry ? 1 : 0)] intValue];
    cell.onButton.hidden = cell.offButton.hidden = NO;
    cell.chooseButton.selected = [self.selectAddresses containsObject:@(address)];
    if (address < 0x8000) {
        SigProvisionerModel *provisioner = [SigDataSource.share getProvisionerModelWithAddress:address];
        if (provisioner) {
            cell.nameLabel.text = [NSString stringWithFormat:@"provisioner:0x%X", address];
            cell.onButton.hidden = cell.offButton.hidden = YES;
        } else {
            SigNodeModel *model = [SigDataSource.share getNodeWithAddress:address];
            if (model.isSensor) {
                cell.nameLabel.text = [NSString stringWithFormat:@"node(seneor):0x%X", address];
                cell.onButton.hidden = cell.offButton.hidden = YES;
            } else if (model.isRemote) {
                cell.nameLabel.text = [NSString stringWithFormat:@"node(remote):0x%X", address];
                cell.onButton.hidden = cell.offButton.hidden = YES;
            } else if (model.isKeyBindSuccess == NO) {
                cell.nameLabel.text = [NSString stringWithFormat:@"node(unbound):0x%X", address];
                cell.onButton.hidden = cell.offButton.hidden = YES;
            } else if (model.state == DeviceStateOutOfLine) {
                cell.nameLabel.text = [NSString stringWithFormat:@"node(off-line):0x%X", address];
                cell.onButton.hidden = cell.offButton.hidden = YES;
                [cell.chooseButton setImage:[UIImage imageNamed:@"bukexuan"] forState:UIControlStateNormal];
                cell.selectionStyle = UITableViewCellSelectionStyleNone;
            } else {
                cell.nameLabel.text = [NSString stringWithFormat:@"node:0x%X", address];
            }
        }
    } else {
        SigGroupModel *group = [SigDataSource.share getGroupModelWithGroupAddress:address];
        if (group) {
            cell.nameLabel.text = [NSString stringWithFormat:@"group:0x%X", address];
        } else {
            cell.nameLabel.text = [NSString stringWithFormat:@"unknown:0x%X", address];
        }
    }
    [cell.onButton addAction:^(UIButton *button) {
        [DemoCommand switchOnOffWithIsOn:YES address:address responseMaxCount:0 ack:NO successCallback:^(UInt16 source, UInt16 destination, SigGenericOnOffStatus * _Nonnull responseMessage) {

        } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {

        }];
    }];
    [cell.offButton addAction:^(UIButton *button) {
        [DemoCommand switchOnOffWithIsOn:NO address:address responseMaxCount:0 ack:NO successCallback:^(UInt16 source, UInt16 destination, SigGenericOnOffStatus * _Nonnull responseMessage) {

        } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {

        }];
    }];
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    EntryCell *cell = (EntryCell *)[tableView cellForRowAtIndexPath:indexPath];
    if (self.entryType == EntryType_tableEntry && indexPath.row == 0) {
        NSArray *arr = [NSArray arrayWithArray:self.allAddresses];
        for (NSNumber *num in arr) {
            SigProvisionerModel *provisioner = [SigDataSource.share getProvisionerModelWithAddress:num.intValue];
            SigNodeModel *model = [SigDataSource.share getNodeWithAddress:num.intValue];
            if (provisioner == nil && model != nil && model.isSensor == NO && model.isRemote == NO && model.isKeyBindSuccess && model.state == DeviceStateOutOfLine) {
                continue;
            }
            if (cell.chooseButton.selected) {
                [self.selectAddresses removeObject:num];
            } else {
                [self.selectAddresses addObject:num];
            }
        }
    } else {
        NSNumber *address = self.allAddresses[indexPath.row - (self.entryType == EntryType_tableEntry ? 1 : 0)];
        if (cell.chooseButton.selected) {
            [self.selectAddresses removeObject:address];
        } else {
            if (self.entryType != EntryType_tableEntry) {
                [self.selectAddresses removeAllObjects];
            }
            [self.selectAddresses addObject:address];
        }
    }
    [self reloadUI];
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {
    return 44.0f;
}

- (void)reloadUI {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
    });
}

@end
