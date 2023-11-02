/********************************************************************************************************
 * @file     DeviceGroupViewController.m 
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/10/10
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

#import "DeviceGroupViewController.h"
#import "DeviceGroupListCell.h"

@interface DeviceGroupViewController ()<UITableViewDelegate,UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic,strong) NSMutableArray <SigGroupModel *>*source;
@end

@implementation DeviceGroupViewController

#pragma mark - Life method
- (void)normalSetting{
    [super normalSetting];
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    self.source = [NSMutableArray arrayWithArray:SigDataSource.share.getAllShowGroupList];
}

#pragma mark - UITableView
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.source.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    DeviceGroupListCell *cell = (DeviceGroupListCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_DeviceGroupListCellID forIndexPath:indexPath];
    SigGroupModel *gModel = self.source[indexPath.row];
    [cell contentWithGroupAddress:@(gModel.intAddress) groupName:gModel.name model:self.model];
    return cell;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 51.0;
}

-(void)dealloc{
    TeLogDebug(@"");
}

@end
