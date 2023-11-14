/********************************************************************************************************
 * @file     DeviceSubscriptionListViewController.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/10/11
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

#import "DeviceSubscriptionListViewController.h"
#import "ModelIDCell.h"

@interface DeviceSubscriptionListViewController ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <ModelIDModel *>*source;
@end

@implementation DeviceSubscriptionListViewController

- (void)normalSetting{
    [super normalSetting];
    self.title = @"Model Setting";
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    self.source = [[NSMutableArray alloc] init];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_ModelIDCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_ModelIDCellID];
    [self reloadSelectModelID];
}

- (void)reloadSelectModelID{
    NSArray *options = SigDataSource.share.defaultGroupSubscriptionModels;
    [self.source removeAllObjects];
    for (NSNumber *modelID in options) {
        ModelIDModel *model = [SigDataSource.share getModelIDModel:modelID];
        if (model) {
            [self.source addObject:model];
        }
    }
    [self.tableView reloadData];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    ModelIDCell *cell = (ModelIDCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_ModelIDCellID forIndexPath:indexPath];
    [cell updateContent:self.source[indexPath.row]];
    return cell;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.source.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 44.0;
}

@end
