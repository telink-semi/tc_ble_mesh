/********************************************************************************************************
 * @file     ModelIDListViewController.m 
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/9/25
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

#import "ModelIDListViewController.h"
#import "ModelIDCell.h"
#import "EditModelIDsViewController.h"

@interface ModelIDListViewController ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <ModelIDModel *>*source;
@end

@implementation ModelIDListViewController

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

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
    [self reloadSelectModelID];
}

- (void)normalSetting{
    [super normalSetting];
    self.title = @"ModelIDs";
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    self.source = [[NSMutableArray alloc] init];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_ModelIDCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_ModelIDCellID];
    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"tabbar_setting"] style:UIBarButtonItemStylePlain target:self action:@selector(clickEdit)];

    self.navigationItem.rightBarButtonItem = rightItem;
}

- (void)clickEdit{
    EditModelIDsViewController *vc = (EditModelIDsViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_EditModelIDsViewControllerID storybroad:@"Setting"];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)reloadSelectModelID{
    [self.source removeAllObjects];
    NSArray *keyBindModelIDs = [NSArray arrayWithArray:SigDataSource.share.keyBindModelIDs];
    for (NSNumber *modelID in keyBindModelIDs) {
        ModelIDModel *model = [SigDataSource.share getModelIDModel:modelID];
        if (model) {
            [self.source addObject:model];
        }
    }
    [self.tableView reloadData];
}

@end
