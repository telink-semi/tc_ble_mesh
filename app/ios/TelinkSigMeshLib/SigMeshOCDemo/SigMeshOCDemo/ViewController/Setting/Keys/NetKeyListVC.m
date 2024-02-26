/********************************************************************************************************
 * @file     NetKeyListVC.m
 *
 * @brief    Show all NetKey of Mesh network.
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

#import "NetKeyListVC.h"
#import "UIButton+extension.h"
#import "UIViewController+Message.h"
#import "NetKeyAddVC.h"
#import "NetKeyCell.h"

@interface NetKeyListVC ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <SigNetkeyModel *>*sourceArray;

@end

@implementation NetKeyListVC

- (void)viewDidLoad {
    [super viewDidLoad];

    self.title = @"NetKey List";
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(NetKeyCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(NetKeyCell.class)];
#ifndef kIsTelinkCloudSigMeshLib
    //init rightBarButtonItem
    UIBarButtonItem *rightItem1 = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemAdd target:self action:@selector(clickAdd:)];
    self.navigationItem.rightBarButtonItem = rightItem1;
#else
    self.tableView.allowsSelection = NO;
#endif
    //longpress to delete scene
    UILongPressGestureRecognizer *gesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(cellDidPress:)];
    [self.view addGestureRecognizer:gesture];
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    self.sourceArray = [NSMutableArray arrayWithArray:self.network.netKeys];
}

- (void)clickAdd:(UIButton *)button{
    [self addNetKeyByManualInput];
}

- (void)addNetKeyByManualInput {
    NetKeyAddVC *vc = [[NetKeyAddVC alloc] init];
    vc.isAdd = YES;

    NSString *timestamp = [LibTools getNowTimeStringOfJson];
    SigNetkeyModel *netkey = [[SigNetkeyModel alloc] init];
    netkey.index = self.network.netKeys.count;
    netkey.phase = 0;
    netkey.timestamp = timestamp;
    netkey.oldKey = nil;
    netkey.key = [LibTools convertDataToHexStr:[LibTools createNetworkKey]];
    netkey.name = [NSString stringWithFormat:@"netkey%ld",(long)netkey.index];
    netkey.minSecurity = @"secure";
    vc.netKeyModel = netkey;
    vc.network = self.network;
    __weak typeof(self) weakSelf = self;
    [vc setBackNetKeyModel:^(SigNetkeyModel * _Nonnull netKeyModel) {
        if (![weakSelf.network.netKeys containsObject:netKeyModel]) {
            [weakSelf.network.netKeys addObject:netKeyModel];
            if (weakSelf.backNetwork) {
                weakSelf.backNetwork(weakSelf.network);
            }
        }
        if (![weakSelf.sourceArray containsObject:netKeyModel]) {
            [weakSelf.sourceArray addObject:netKeyModel];
            [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
        }
    }];
    [self.navigationController pushViewController:vc animated:YES];
}

#pragma  mark LongPressGesture
- (void)cellDidPress:(UILongPressGestureRecognizer *)sender{
    if (sender.state == UIGestureRecognizerStateBegan) {
        NSIndexPath *indexPath = [self.tableView indexPathForRowAtPoint:[sender locationInView:self.tableView]];
        if (indexPath != nil) {
            if (self.network.netKeys.count == 1) {
                [self showAlertSureWithTitle:@"Hits" message:@"The mesh network needs at least one netkey!" sure:nil];
                return;
            }

            SigNetkeyModel *model = self.sourceArray[indexPath.row];
            BOOL hadBound = NO;
            NSArray *temNodes = [NSArray arrayWithArray:self.network.curNodes];
            for (SigNodeModel *node in temNodes) {
                if (node.netKeys && node.netKeys.count > 0) {
                    for (SigNodeKeyModel *nodeKey in node.netKeys) {
                        if (nodeKey.index == model.index) {
                            hadBound = YES;
                            break;
                        }
                    }
                    if (hadBound) {
                        break;
                    }
                }
            }
            TelinkLogDebug(@"%@",indexPath);
            if (hadBound) {
                [self showAlertSureWithTitle:@"Hits" message:@"Some nodes have already bound this netkey, you can`t delete it!" sure:nil];
                return;
            }

            NSString *msg = [NSString stringWithFormat:@"Are you sure delete netKey, index:0x%04lX key:%@",(long)model.index,model.key];
            __weak typeof(self) weakSelf = self;
            [self showAlertSureAndCancelWithTitle:@"Hits" message:msg sure:^(UIAlertAction *action) {
                [weakSelf.network.netKeys removeObject:model];
                if (weakSelf.backNetwork) {
                    weakSelf.backNetwork(weakSelf.network);
                }
                [weakSelf.sourceArray removeObject:model];
                [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
            } cancel:^(UIAlertAction *action) {

            }];
        }
    }
}

#pragma mark - UITableViewDataSource
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.sourceArray.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    NetKeyCell *cell = [tableView dequeueReusableCellWithIdentifier:NSStringFromClass(NetKeyCell.class) forIndexPath:indexPath];
    SigNetkeyModel *model = self.sourceArray[indexPath.row];
    [cell setModel:model];
#ifdef kIsTelinkCloudSigMeshLib
    cell.editButton.hidden = YES;
#endif
    __weak typeof(self) weakSelf = self;
    [cell.editButton addAction:^(UIButton *button) {
        NetKeyAddVC *vc = [[NetKeyAddVC alloc] init];
        vc.isAdd = NO;
        vc.network = weakSelf.network;
        [vc setNetKeyModel:model];
        [vc setBackNetKeyModel:^(SigNetkeyModel * _Nonnull netKeyModel) {
            [weakSelf.network.netKeys replaceObjectAtIndex:indexPath.row withObject:netKeyModel];
            if (weakSelf.backNetwork) {
                weakSelf.backNetwork(weakSelf.network);
            }
            [weakSelf.sourceArray replaceObjectAtIndex:indexPath.row withObject:netKeyModel];
            [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
        }];
        [weakSelf.navigationController pushViewController:vc animated:YES];
    }];
    return cell;
}

#ifndef kIsTelinkCloudSigMeshLib
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [tableView cellForRowAtIndexPath:indexPath];
    cell.selected = NO;
    //当前使用的netkey不存储本地
    if (![self.network.meshUUID isEqualToString:SigDataSource.share.meshUUID]) {
        return;
    }
    SigNetkeyModel *model = self.sourceArray[indexPath.row];
    if (model != self.network.curNetkeyModel) {
        NSString *msg = [NSString stringWithFormat:@"Are you sure change current netKey to index:0x%04lX key:%@",(long)model.index,model.key];
        __weak typeof(self) weakSelf = self;
        [self showAlertSureAndCancelWithTitle:@"Hits" message:msg sure:^(UIAlertAction *action) {
            SigDataSource.share.curNetkeyModel = model;
            [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
        } cancel:^(UIAlertAction *action) {

        }];
    }
}
#endif

@end
