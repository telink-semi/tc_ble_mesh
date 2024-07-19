/********************************************************************************************************
 * @file     AppKeyListVC.m
 *
 * @brief    Show all AppKey of Mesh network.
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

#import "AppKeyListVC.h"
#import "UIButton+extension.h"
#import "UIViewController+Message.h"
#import "AppKeyAddVC.h"
#import "AppKeyCell.h"

@interface AppKeyListVC ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <SigAppkeyModel *>*sourceArray;

@end

@implementation AppKeyListVC

- (void)viewDidLoad {
    [super viewDidLoad];

    self.title = @"AppKey List";
    self.sourceArray = [NSMutableArray arrayWithArray:self.network.appKeys];
    UIView *footerView = [[UIView alloc] initWithFrame:CGRectZero];
    self.tableView.tableFooterView = footerView;
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(AppKeyCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(AppKeyCell.class)];
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

- (void)clickAdd:(UIButton *)button{
    [self addAppKeyByManualInput];
}

- (void)addAppKeyByManualInput {
    AppKeyAddVC *vc = [[AppKeyAddVC alloc] init];
    vc.isAdd = YES;
    SigAppkeyModel *appkey = [[SigAppkeyModel alloc] init];
    appkey.oldKey = @"00000000000000000000000000000000";
    appkey.key = [LibTools convertDataToHexStr:[LibTools initAppKey]];
    appkey.index = self.network.appKeys.count;
    appkey.name = [NSString stringWithFormat:@"appkey%ld",(long)appkey.index];
    appkey.boundNetKey = self.network.curNetkeyModel.index;
    vc.appKeyModel = appkey;
    vc.network = self.network;
    __weak typeof(self) weakSelf = self;
    [vc setBackAppKeyModel:^(SigAppkeyModel * _Nonnull appKeyModel) {
        if (![weakSelf.network.appKeys containsObject:appKeyModel]) {
            [weakSelf.network.appKeys addObject:appKeyModel];
            if (weakSelf.backNetwork) {
                weakSelf.backNetwork(weakSelf.network);
            }
        }
        if (![weakSelf.sourceArray containsObject:appKeyModel]) {
            [weakSelf.sourceArray addObject:appKeyModel];
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
            if (self.network.appKeys.count == 1) {
                [self showTips:@"The mesh network needs at least one appkey!"];
                return;
            }

            SigAppkeyModel *model = self.sourceArray[indexPath.row];
            BOOL hadBound = NO;
            NSArray *temNodes = [NSArray arrayWithArray:self.network.curNodes];
            for (SigNodeModel *node in temNodes) {
                if (node.appKeys && node.appKeys.count > 0) {
                    for (SigNodeKeyModel *nodeKey in node.appKeys) {
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
                [self showTips:@"Some nodes have already bound this appkey, you can`t delete it!"];
                return;
            }
            __weak typeof(self) weakSelf = self;
            [self showAlertTitle:kDefaultAlertTitle message:[NSString stringWithFormat:@"Are you sure delete appKey, index:0x%04lX key:%@",(long)model.index,model.key] sure:^(UIAlertAction *action) {
                [weakSelf.network.appKeys removeObject:model];
                if (weakSelf.backNetwork) {
                    weakSelf.backNetwork(weakSelf.network);
                }
                [weakSelf.sourceArray removeObject:model];
                [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
            }];
        }
    }
}

#pragma mark - UITableViewDataSource
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.sourceArray.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    AppKeyCell *cell = [tableView dequeueReusableCellWithIdentifier:NSStringFromClass(AppKeyCell.class) forIndexPath:indexPath];
    SigAppkeyModel *model = self.sourceArray[indexPath.row];
    [cell setModel:model];
#ifdef kIsTelinkCloudSigMeshLib
    cell.editButton.hidden = YES;
#endif
    __weak typeof(self) weakSelf = self;
    [cell.editButton addAction:^(UIButton *button) {
        AppKeyAddVC *vc = [[AppKeyAddVC alloc] init];
        vc.isAdd = NO;
        [vc setAppKeyModel:model];
        vc.network = weakSelf.network;
        [vc setBackAppKeyModel:^(SigAppkeyModel * _Nonnull appKeyModel) {
            [weakSelf.network.appKeys replaceObjectAtIndex:indexPath.row withObject:appKeyModel];
            if (weakSelf.backNetwork) {
                weakSelf.backNetwork(weakSelf.network);
            }
            [weakSelf.sourceArray replaceObjectAtIndex:indexPath.row withObject:appKeyModel];
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
    //当前使用的appkey不存储本地
    if (![self.network.meshUUID isEqualToString:SigDataSource.share.meshUUID]) {
        return;
    }
    SigAppkeyModel *model = self.sourceArray[indexPath.row];
    if (model != self.network.curAppkeyModel) {
        __weak typeof(self) weakSelf = self;
        [self showAlertSureAndCancelWithTitle:kDefaultAlertTitle message:[NSString stringWithFormat:@"Are you sure change current appKey to index:0x%04lX key:%@",(long)model.index,model.key] sure:^(UIAlertAction *action) {
            SigNodeModel *node = [SigDataSource.share curLocationNodeModel];
            bool needReConnectMesh = YES;
            if (node && node.appKeys.count > 0) {
                NSArray *array = [NSArray arrayWithArray:node.appKeys];
                for (SigNodeKeyModel *key in array) {
                    if (key.index == model.index) {
                        needReConnectMesh = NO;
                        break;
                    }
                }
            }
            if (needReConnectMesh) {
                [SDKLibCommand stopMeshConnectWithComplete:^(BOOL successful) {
                    TelinkLogInfo(@"change appkey: stop mesh=%d", successful);
                    [SDKLibCommand startMeshConnectWithComplete:^(BOOL successful) {
                        TelinkLogInfo(@"change appkey: start mesh=%d", successful);
                    }];
                }];
            }
            SigDataSource.share.curAppkeyModel = model;
            [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
        } cancel:nil];
    }
}
#endif

@end
