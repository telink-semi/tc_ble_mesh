/********************************************************************************************************
 * @file     EditModelIDsViewController.m 
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/7/31
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

#import "EditModelIDsViewController.h"
#import "ChooseModelIDCell.h"

@interface EditModelIDsViewController()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <ModelIDModel *>*source;
@property (nonatomic, strong) NSMutableArray <NSNumber *>*allNumbers;
@property (nonatomic, strong) NSMutableArray <NSNumber *>*selectNumbers;
@end

@implementation EditModelIDsViewController

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    ChooseModelIDCell *cell = (ChooseModelIDCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_ChooseModelIDCellID forIndexPath:indexPath];
    ModelIDModel *model = self.source[indexPath.row];
    [cell updateContent:model];
    cell.selectButton.selected = [self.selectNumbers containsObject:@(model.sigModelID)];
    return cell;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.source.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 44.0;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    ModelIDModel *model = self.source[indexPath.row];
    if ([self.selectNumbers containsObject:@(model.sigModelID)]) {
        [self.selectNumbers removeObjectAtIndex:[self.selectNumbers indexOfObject:@(model.sigModelID)]];
    } else {
        [self.selectNumbers addObject:@(model.sigModelID)];
    }
    [self.tableView reloadData];
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)normalSetting{
    [super normalSetting];
    self.title = @"Edit";
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_ChooseModelIDCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_ChooseModelIDCellID];
    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithTitle:@"Save" style:UIBarButtonItemStylePlain target:self action:@selector(clickSave)];
    self.navigationItem.rightBarButtonItem = rightItem;
    
    ModelIDs *modelIDs = [[ModelIDs alloc] init];
    self.source = [[NSMutableArray alloc] initWithArray:modelIDs.modelIDs];
    self.selectNumbers = [[NSMutableArray alloc] initWithArray:SigDataSource.share.keyBindModelIDs];
    self.allNumbers = [[NSMutableArray alloc] init];
    NSArray *source = [NSArray arrayWithArray:self.source];
    for (ModelIDModel *model in source) {
        [self.allNumbers addObject:@(model.sigModelID)];
    }
}

- (void)clickSave{
    SigDataSource.share.keyBindModelIDs = self.selectNumbers;
    [SigDataSource.share saveLocationData];
    [ShowTipsHandle.share hidden];
    [self.navigationController popViewControllerAnimated:YES];
}

@end
