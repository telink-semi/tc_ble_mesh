/********************************************************************************************************
 * @file     SelectUpdateDeviceVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2024/1/3
 *
 * @par     Copyright (c) 2024, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "SelectUpdateDeviceVC.h"
#import "SelectDeviceCell.h"
#import "UIButton+extension.h"

@interface SelectUpdateDeviceVC ()
@property (weak, nonatomic) IBOutlet UIButton *confirmButton;
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (strong, nonatomic) NSMutableArray <NSMutableArray <SigNodeModel *>*>*allNodes;
@property (strong, nonatomic) NSMutableArray <SigNodeModel *>*selectNodes;
@end

@implementation SelectUpdateDeviceVC

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    self.title = @"Select Update Device";
    self.allNodes = [NSMutableArray array];
    self.selectNodes = [NSMutableArray array];
    NSDictionary *dict = [SigDataSource.share currentProductNodesDictionary];
    if (dict.allValues.count > 0) {
        [self.allNodes addObjectsFromArray:dict.allValues];
    }
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(SelectDeviceCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(SelectDeviceCell.class)];
    //iOS 15中 UITableView 新增了一个属性：sectionHeaderTopPadding。此属性会给每一个 section header 增加一个默认高度，当我们使用 UITableViewStylePlain 初始化UITableView 的时候，系统默认给 section header 增高了22像素。
    if(@available(iOS 15.0,*)) {
        self.tableView.sectionHeaderTopPadding = 0;
    }

}

- (IBAction)clickConfirmButton:(UIButton *)sender {
    if (self.selectNodes.count == 0) {
        [self showTips:@"Please select at least one device!"];
    }
    if (self.backSelectNodes) {
        self.backSelectNodes(self.selectNodes);
    }
    [self.navigationController popViewControllerAnimated:YES];
}

#pragma mark - UITableViewDataSource
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return self.allNodes.count;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    NSArray *nodes = self.allNodes[section];
    return nodes.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    SelectDeviceCell *cell = (SelectDeviceCell *)[tableView dequeueReusableCellWithIdentifier:NSStringFromClass(SelectDeviceCell.class) forIndexPath:indexPath];
    NSArray *nodes = self.allNodes[indexPath.section];
    SigNodeModel *node = nodes[indexPath.row];
    [cell setModel:node];
    if ([self.selectNodes containsObject:node] && node.state != DeviceStateOutOfLine) {
        cell.selectButton.selected = YES;
    } else {
        cell.selectButton.selected = NO;
    }
    __weak typeof(self) weakSelf = self;
    [cell.selectButton addAction:^(UIButton *button) {
        if (node.state != DeviceStateOutOfLine) {
            button.selected = !button.selected;
            if ([weakSelf.selectNodes containsObject:node]) {
                [weakSelf.selectNodes removeObject:node];
            } else {
                [weakSelf.selectNodes addObject:node];
            }
        }
    }];
    return cell;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section {
    NSArray *nodes = self.allNodes[section];
    SigNodeModel *node = nodes.firstObject;
#ifdef kIsTelinkCloudSigMeshLib
    CloudNodeModel *model = [AppDataSource.share getCloudNodeModelWithNodeAddress:node.address];
    return [NSString stringWithFormat:@"Product Name:%@ ID:%04lX", model.productInfo.name, (long)model.productId];
#else
    return [NSString stringWithFormat:@"Product ID:%@", node.pid];
#endif
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    SelectDeviceCell *cell = (SelectDeviceCell *)[tableView cellForRowAtIndexPath:indexPath];
    cell.selected = NO;
    if (cell.model.state != DeviceStateOutOfLine) {
        cell.selectButton.selected = !cell.selectButton.selected;
        if ([self.selectNodes containsObject:cell.model]) {
            [self.selectNodes removeObject:cell.model];
        } else {
            [self.selectNodes addObject:cell.model];
        }
    }
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
