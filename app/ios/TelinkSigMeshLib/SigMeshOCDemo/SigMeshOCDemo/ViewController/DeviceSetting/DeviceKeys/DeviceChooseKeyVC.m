/********************************************************************************************************
 * @file     DeviceChooseKeyVC.m
 *
 * @brief    Show all AppKey or NetKey which can add to the node.
 *
 * @author   Telink, 梁家誌
 * @date     2020/9/17
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "DeviceChooseKeyVC.h"
#import "UIViewController+Message.h"
#import "NetKeyCell.h"
#import "AppKeyCell.h"

@interface DeviceChooseKeyVC ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <SigNetkeyModel *>*netkeyArray;
@property (nonatomic, strong) NSMutableArray <SigAppkeyModel *>*appkeyArray;

@end

@implementation DeviceChooseKeyVC

- (void)viewDidLoad {
    [super viewDidLoad];

    if (self.backNetKeyModel) {
        [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(NetKeyCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(NetKeyCell.class)];
        self.title = @"Choose add NetKey";
        self.netkeyArray = [NSMutableArray array];
        for (SigNetkeyModel *netKey in SigDataSource.share.netKeys) {
            BOOL hadBound = NO;
            for (SigNodeKeyModel *nodeKey in self.model.netKeys) {
                if (nodeKey.index == netKey.index) {
                    hadBound = YES;
                    break;
                }
            }
            if (!hadBound) {
                [self.netkeyArray addObject:netKey];
            }
        }
    }
    if (self.backAppKeyModel) {
        [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(AppKeyCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(AppKeyCell.class)];
        self.title = @"Choose add AppKey";
        self.appkeyArray = [NSMutableArray array];
        for (SigAppkeyModel *appKey in SigDataSource.share.appKeys) {
            for (SigNodeKeyModel *net in self.model.netKeys) {
                if (net.index == appKey.boundNetKey) {
                    BOOL hadBound = NO;
                    for (SigNodeKeyModel *nodeKey in self.model.appKeys) {
                        if (nodeKey.index == appKey.index) {
                            hadBound = YES;
                            break;
                        }
                    }
                    if (!hadBound) {
                        [self.appkeyArray addObject:appKey];
                    }
                    break;
                }
            }
        }
    }
}

#pragma mark - UITableViewDataSource
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    if (self.backNetKeyModel) {
        return self.netkeyArray.count;
    }
    if (self.backAppKeyModel) {
        return self.appkeyArray.count;
    }
    return 0;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    if (self.backNetKeyModel) {
        NetKeyCell *cell = [tableView dequeueReusableCellWithIdentifier:NSStringFromClass(NetKeyCell.class) forIndexPath:indexPath];
        [cell setModel:self.netkeyArray[indexPath.row]];
        cell.editButton.hidden = YES;
        return cell;
    }
    if (self.backAppKeyModel) {
        AppKeyCell *cell = [tableView dequeueReusableCellWithIdentifier:NSStringFromClass(AppKeyCell.class) forIndexPath:indexPath];
        [cell setModel:self.appkeyArray[indexPath.row]];
        cell.editButton.hidden = YES;
        return cell;
    }
    return [[UITableViewCell alloc] init];
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [tableView cellForRowAtIndexPath:indexPath];
    cell.selected = NO;
    if (SigBearer.share.isOpen) {
        NSString *msg = @"";
        __weak typeof(self) weakSelf = self;
        if (self.backNetKeyModel) {
            SigNetkeyModel *model = self.netkeyArray[indexPath.row];
            msg = [NSString stringWithFormat:@"Are you sure add this netKey to node. index:0x%04lX key:%@",(long)model.index,model.key];
            [self showAlertSureAndCancelWithTitle:@"Hits" message:msg sure:^(UIAlertAction *action) {
                if (weakSelf.backNetKeyModel) {
                    weakSelf.backNetKeyModel(model);
                }
                [weakSelf.navigationController popViewControllerAnimated:YES];
            } cancel:^(UIAlertAction *action) {

            }];
        }
        if (self.backAppKeyModel) {
            SigAppkeyModel *model = self.appkeyArray[indexPath.row];
            msg = [NSString stringWithFormat:@"Are you sure add this appKey to node. index:0x%04lX key:%@",(long)model.index,model.key];
            [self showAlertSureAndCancelWithTitle:@"Hits" message:msg sure:^(UIAlertAction *action) {
                if (weakSelf.backAppKeyModel) {
                    weakSelf.backAppKeyModel(model);
                }
                [weakSelf.navigationController popViewControllerAnimated:YES];
            } cancel:^(UIAlertAction *action) {

            }];
        }
    }
}

@end
