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
    if (self.selectAddresses.count == 0) {
        [self showTips:@"Pls select at least ONE device"];
        return;
    }
    if (self.backAddressCallback) {
        self.backAddressCallback(self.selectAddresses);
        [self.navigationController popViewControllerAnimated:YES];
    }
}

#pragma mark - Life method
- (void)normalSetting{
    [super normalSetting];
    if (self.entryType == EntryType_tableEntry) {
        self.title = @"Select Route";
    } else if (self.entryType == EntryType_originRange) {
        self.title = @"Select Origin";
    } else {
        self.title = @"Select Target";
    }
    self.addEntryButton.backgroundColor = UIColor.telinkButtonBlue;
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_EntryCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_EntryCellID];
    self.allAddresses = [NSMutableArray array];
    self.selectAddresses = [NSMutableArray array];
    if (self.oldAddresses) {
        [self.selectAddresses addObjectsFromArray:self.oldAddresses];
    }

    NSMutableArray *allNodes = [NSMutableArray array];
    NSArray *arr = [NSArray arrayWithArray:SigDataSource.share.curNodes];
    for (SigNodeModel *node in arr) {
        [allNodes addObject:@(node.address)];
    }
    [self.allAddresses addObjectsFromArray:allNodes];
}

-(void)dealloc{
    TelinkLogDebug(@"%s",__func__);
}

#pragma mark - UITableView
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.allAddresses.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    EntryCell *cell = (EntryCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_EntryCellID forIndexPath:indexPath];
    UInt16 address = [self.allAddresses[indexPath.row] intValue];
    cell.chooseButton.selected = [self.selectAddresses containsObject:@(address)];
    SigNodeModel *model = [SigDataSource.share getNodeWithAddress:address];
    cell.model = model;
    __weak typeof(self) weakSelf = self;
    [cell.chooseButton addAction:^(UIButton *button) {
        if (model.state == DeviceStateOutOfLine) {
            [weakSelf showTips:Tip_DeviceOutline];
            return;
        }
        if (cell.chooseButton.selected) {
            [weakSelf.selectAddresses removeObject:@(address)];
        } else {
            if (weakSelf.entryType != EntryType_tableEntry) {
                [weakSelf.selectAddresses removeAllObjects];
            }
            [weakSelf.selectAddresses addObject:@(address)];
        }
        [weakSelf reloadUI];
    }];
    return cell;
}

- (void)reloadUI {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
    });
}

@end
