/********************************************************************************************************
 * @file     DeviceSelectVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2024/4/7
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

#import "DeviceSelectVC.h"
#import "SelectDeviceCell.h"
#import "UIButton+extension.h"

@interface DeviceSelectVC ()
@property (weak, nonatomic) IBOutlet UIButton *confirmButton;
@property (weak, nonatomic) IBOutlet UIButton *selectAllNormalButton;
@property (weak, nonatomic) IBOutlet UILabel *selectAllNormalLabel;
@property (weak, nonatomic) IBOutlet UITableView *normalDeviceTableView;
@property (weak, nonatomic) IBOutlet UITableView *lpnDeviceTableView;
@property (strong, nonatomic) NSMutableArray <SigNodeModel *>*allNormalNodes;
@property (strong, nonatomic) NSMutableArray <SigNodeModel *>*allLPNNodes;
@property (strong, nonatomic) NSMutableArray <SigNodeModel *>*selectNormalNodes;
@property (strong, nonatomic) NSMutableArray <SigNodeModel *>*selectLPNNodes;

@end

@implementation DeviceSelectVC

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    self.title = @"Device Select";
    self.allNormalNodes = [NSMutableArray array];
    self.allLPNNodes = [NSMutableArray array];
    self.selectNormalNodes = [NSMutableArray array];
    self.selectLPNNodes = [NSMutableArray array];
    
    NSArray *array = [NSArray arrayWithArray:SigDataSource.share.curNodes];
    for (SigNodeModel *node in array) {
        if (node.isLPN) {
            [self.allLPNNodes addObject:node];
        } else {
            [self.allNormalNodes addObject:node];
        }
    }
    if (self.allNormalNodes.count == 0) {
        self.selectAllNormalButton.hidden = self.selectAllNormalLabel.hidden = YES;
    }
    __weak typeof(self) weakSelf = self;
    [self.selectAllNormalButton addAction:^(UIButton *button) {
        [weakSelf clickSelectAllButton:button];
    }];
    [self.normalDeviceTableView registerNib:[UINib nibWithNibName:NSStringFromClass(SelectDeviceCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(SelectDeviceCell.class)];
    [self.lpnDeviceTableView registerNib:[UINib nibWithNibName:NSStringFromClass(SelectDeviceCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(SelectDeviceCell.class)];
    //iOS 15中 UITableView 新增了一个属性：sectionHeaderTopPadding。此属性会给每一个 section header 增加一个默认高度，当我们使用 UITableViewStylePlain 初始化UITableView 的时候，系统默认给 section header 增高了22像素。
    if(@available(iOS 15.0,*)) {
        self.normalDeviceTableView.sectionHeaderTopPadding = self.lpnDeviceTableView.sectionHeaderTopPadding = 0;
    }

}

- (void)clickSelectAllButton:(UIButton *)button {
    button.selected = !button.isSelected;
    if (button.isSelected) {
        NSMutableArray *mArray = [NSMutableArray array];
        NSArray *array;
        if (button == self.selectAllNormalButton) {
            array = [NSArray arrayWithArray:self.allNormalNodes];
        } else {
            array = [NSArray arrayWithArray:self.allLPNNodes];
        }
        for (SigNodeModel *node in array) {
            if (node.state != DeviceStateOutOfLine && node.hasFirmwareDistributionServerModel) {
                if (!node.isLPN && button == self.selectAllNormalButton) {
                    [mArray addObject:node];
                }
            }
        }
        button.selected = mArray.count;
        if (button == self.selectAllNormalButton) {
            self.selectNormalNodes = mArray;
            [self.normalDeviceTableView reloadData];
        } else {
            self.selectLPNNodes = mArray;
            [self.lpnDeviceTableView reloadData];
        }
    } else {
        if (button == self.selectAllNormalButton) {
            self.selectNormalNodes = [NSMutableArray array];
            [self.normalDeviceTableView reloadData];
        } else {
            self.selectLPNNodes = [NSMutableArray array];
            [self.lpnDeviceTableView reloadData];
        }
    }
}

- (IBAction)clickConfirmButton:(UIButton *)sender {
    if (self.selectNormalNodes.count + self.selectLPNNodes.count == 0) {
        [self showTips:@"Please select at least one device!"];
    }
    if (self.backSelectNodes) {
        NSMutableArray *array = [NSMutableArray arrayWithArray:self.selectNormalNodes];
        [array addObjectsFromArray:self.selectLPNNodes];
        self.backSelectNodes(array);
    }
    [self.navigationController popViewControllerAnimated:YES];
}

- (void)refreshSelectAllButton {
    int normalCount = 0;
    int lpnCount = 0;
    NSArray *array = [NSArray arrayWithArray:self.allNormalNodes];
    for (SigNodeModel *node in array) {
        if (node.state != DeviceStateOutOfLine) {
            if (node.hasFirmwareDistributionServerModel) {
                if (node.isLPN) {
                    lpnCount ++;
                } else {
                    normalCount ++;
                }
            }
        }
    }
    self.selectAllNormalButton.selected = normalCount == self.selectNormalNodes.count;
}

#pragma mark - UITableViewDataSource

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    if (tableView == self.normalDeviceTableView) {
        return self.allNormalNodes.count;
    } else {
        return self.allLPNNodes.count;
    }
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    SelectDeviceCell *cell = (SelectDeviceCell *)[tableView dequeueReusableCellWithIdentifier:NSStringFromClass(SelectDeviceCell.class) forIndexPath:indexPath];
    cell.selected = NO;
    SigNodeModel *node;
    NSMutableArray *selectArray;
    if (tableView == self.normalDeviceTableView) {
        node = self.allNormalNodes[indexPath.row];
        selectArray = self.selectNormalNodes;
    } else {
        node = self.allLPNNodes[indexPath.row];
        selectArray = self.selectLPNNodes;
    }
    [cell setModel:node];
    if (tableView == self.normalDeviceTableView) {
        if ([selectArray containsObject:node] && node.state != DeviceStateOutOfLine) {
            cell.selectButton.selected = YES;
        } else {
            cell.selectButton.selected = NO;
        }
    } else {
        if ([selectArray containsObject:node]) {
            cell.selectButton.selected = YES;
        } else {
            cell.selectButton.selected = NO;
        }
    }

    __weak typeof(self) weakSelf = self;
    [cell.selectButton addAction:^(UIButton *button) {
        if (node.state != DeviceStateOutOfLine) {
            button.selected = !button.selected;
            if ([selectArray containsObject:node]) {
                [selectArray removeObject:node];
            } else {
                [selectArray addObject:node];
            }
            [weakSelf refreshSelectAllButton];
        }
    }];
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    SelectDeviceCell *cell = (SelectDeviceCell *)[tableView cellForRowAtIndexPath:indexPath];
    cell.selected = NO;
    NSMutableArray *selectArray;
    if (tableView == self.normalDeviceTableView) {
        selectArray = self.selectNormalNodes;
    } else {
        selectArray = self.selectLPNNodes;
    }
    if (cell.model.isLPN) {
        cell.selectButton.selected = !cell.selectButton.selected;
        if ([selectArray containsObject:cell.model]) {
            [selectArray removeObject:cell.model];
        } else {
            [selectArray removeAllObjects];
            [selectArray addObject:cell.model];
        }
        self.selectLPNNodes = selectArray;
        [self.lpnDeviceTableView reloadData];
    } else {
        if (cell.model.state != DeviceStateOutOfLine) {
            cell.selectButton.selected = !cell.selectButton.selected;
            if ([selectArray containsObject:cell.model]) {
                [selectArray removeObject:cell.model];
            } else {
                [selectArray addObject:cell.model];
            }
            [self refreshSelectAllButton];
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
