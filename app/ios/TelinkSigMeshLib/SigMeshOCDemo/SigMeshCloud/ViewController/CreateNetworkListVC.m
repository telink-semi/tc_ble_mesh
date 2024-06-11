/********************************************************************************************************
 * @file     CreateNetworkListVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/5/15
 *
 * @par     Copyright (c) 2023, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "CreateNetworkListVC.h"
#import "NetworkCell.h"
#import "NSString+extension.h"
#import "UIViewController+Message.h"
#import "UIButton+extension.h"
#import "ShareQRCodeVC.h"
#import "CustomShareAlertView.h"
#import "ApplicantsVC.h"
#import "UIButton+extension.h"

@interface CreateNetworkListVC ()<UITableViewDelegate, UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <AppMeshNetworkModel *>*createdList;
@end

@implementation CreateNetworkListVC

- (void)viewDidLoad {
    [super viewDidLoad];
    __weak typeof(self) weakSelf = self;
    [self addNoDataUI];
    [self addCustomCircularButtonWithImageString:@"ic_add_white" action:^(UIButton *button) {
        [weakSelf addMeshNetwork];
    }];
    [self setSelectBlock:^{
        [weakSelf configRightBarButtonItem];
    }];
    
    self.createdList = [NSMutableArray arrayWithArray:AppDataSource.share.createdNetwordList];
    UIView *footerView = [[UIView alloc] initWithFrame:CGRectZero];
    self.tableView.tableFooterView = footerView;
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(NetworkCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(NetworkCell.class)];
    //longpress to delete scene
    UILongPressGestureRecognizer *gesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(cellDidPress:)];
    [self.view addGestureRecognizer:gesture];
    [self configRightBarButtonItem];
    [self clickRefreshButton];
}

- (void)configRightBarButtonItem {
    //init rightBarButtonItem
    UIBarButtonItem *rightItem1 = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh target:self action:@selector(clickRefreshButton)];
    self.supperVC.navigationItem.rightBarButtonItem = rightItem1;
}

- (void)clickRefreshButton {
    __weak typeof(self) weakSelf = self;
    [AppDataSource.share getCreatedNetworkListWithResultBlock:^(NSError * _Nullable error) {
        if (error) {
            [weakSelf showTips:error.localizedDescription];
        } else {
            if (AppDataSource.share.createdNetwordList.count == 0) {
                weakSelf.tableView.hidden = YES;
                [weakSelf showTips:@"no create netword."];
            } else {
                weakSelf.tableView.hidden = NO;
                weakSelf.createdList = [NSMutableArray arrayWithArray:AppDataSource.share.createdNetwordList];
                [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
            }
        }
    }];
}

- (void)clickMoreButtonWithNetwork:(AppMeshNetworkModel *)network {
    ShareAlertItemModel *item1 = [[ShareAlertItemModel alloc] init];
    item1.itemType = ShareItemType_icon_text_next;
    item1.iconImage = [UIImage imageNamed:@"ic_share"];
    item1.textString = @"share";
    item1.textColor = HEX(#4A87EE);
    ShareAlertItemModel *item2 = [[ShareAlertItemModel alloc] init];
    item2.itemType = ShareItemType_icon_text_next;
    item2.iconImage = [UIImage imageNamed:@"ic_member2"];
    item2.textString = @"applicants";
    item2.textColor = HEX(#4A87EE);
    ShareAlertItemModel *item3 = [[ShareAlertItemModel alloc] init];
    item3.itemType = ShareItemType_icon_text_next;
    item3.iconImage = [UIImage imageNamed:@"ic_switch"];
    item3.textString = @"switch to this network";
    item3.textColor = HEX(#4A87EE);
    ShareAlertItemModel *item4 = [[ShareAlertItemModel alloc] init];
    item4.itemType = ShareItemType_button;
    item4.textString = @"DELETE";
    
    __weak typeof(self) weakSelf = self;
    CustomShareAlertView *customAlertView = [[CustomShareAlertView alloc] initWithTitle:[NSString stringWithFormat:@"select action for: %@", network.name] itemArray:@[item1, item2, item3, item4] alertResult:^(NSInteger selectIndex) {
        if (selectIndex == 0) {
            [weakSelf clickShareButtonWithNetwork:network];
        } else if (selectIndex == 1) {
            [weakSelf clickMemberButtonWithNetwork:network];
        } else if (selectIndex == 2) {
            [weakSelf clickSwitchButtonWithNetwork:network];
        } else if (selectIndex == 3) {
            [weakSelf deleteNetwork:network];
        }
    }];
    [customAlertView showCustomAlertView];    
}

- (void)clickShareButtonWithNetwork:(AppMeshNetworkModel *)network {
    ShareQRCodeVC *vc = (ShareQRCodeVC *)[UIStoryboard initVC:NSStringFromClass(ShareQRCodeVC.class) storyboard:@"Cloud"];
    vc.networkName = network.name;
    vc.networkId = network.networkId;
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)clickMemberButtonWithNetwork:(AppMeshNetworkModel *)network {
    ApplicantsVC *vc = (ApplicantsVC *)[UIStoryboard initVC:NSStringFromClass(ApplicantsVC.class) storyboard:@"Cloud"];
    vc.networkId = network.networkId;
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)clickSwitchButtonWithNetwork:(AppMeshNetworkModel *)network {
    AppDataSource.share.curNetworkId = network.networkId;
    __weak typeof(self) weakSelf = self;
    [SDKLibCommand stopMeshConnectWithComplete:nil];
    [AppDataSource.share getDetailWithNetworkId:network.networkId type:0 resultBlock:^(NSError * _Nullable error) {
        if (error) {
            [weakSelf showTips:error.localizedDescription];
        } else {
            [weakSelf saveLocalWithCurrentNetworkId:network.networkId type:0];
            [weakSelf setBaseTabbarControllerToRootViewController];
        }
    }];
}

- (void)addMeshNetwork {
    __weak typeof(self) weakSelf = self;
    UIAlertController *alertVc = [UIAlertController alertControllerWithTitle:kDefaultAlertTitle message:@"Please input new mesh network name!" preferredStyle: UIAlertControllerStyleAlert];
    [alertVc addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
        textField.placeholder = @"new network name";
    }];
    UIAlertAction *action1 = [UIAlertAction actionWithTitle:kDefaultAlertOK style:UIAlertActionStyleDestructive handler:^(UIAlertAction * _Nonnull action) {
        NSString *networkName = [[alertVc textFields] objectAtIndex:0].text;
        networkName = networkName.removeHeadAndTailSpacePro;
        TelinkLogInfo(@"new networkName is %@", networkName);
        if (networkName == nil || networkName.length == 0) {
            [weakSelf showTips:@"Network name can not be empty!"];
            return;
        }
        [weakSelf addMeshNetworkActionWithMeshNetworkName:networkName];
    }];
    UIAlertAction *action2 = [UIAlertAction actionWithTitle:kDefaultAlertCancel style:UIAlertActionStyleCancel handler:nil];
    [alertVc addAction:action2];
    [alertVc addAction:action1];
    [self presentViewController:alertVc animated:YES completion:nil];
}

- (void)addMeshNetworkActionWithMeshNetworkName:(NSString *)meshNetworkName {
    __weak typeof(self) weakSelf = self;
    [AppDataSource.share createMeshNetworkWithName:meshNetworkName resultBlock:^(NSError * _Nullable error) {
        if (error) {
            TelinkLogInfo(@"addMeshNetwork error = %@", error);
            [weakSelf showTips:[NSString stringWithFormat:@"addMeshNetwork error = %@", error.localizedDescription]];
        } else {
            [weakSelf showTips:[NSString stringWithFormat:@"addMeshNetwork:%@ successful.", meshNetworkName]];
            weakSelf.tableView.hidden = NO;
            weakSelf.createdList = [NSMutableArray arrayWithArray:AppDataSource.share.createdNetwordList];
            [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
        }
    }];
}

- (void)deleteNetwork:(AppMeshNetworkModel *)network {
    NSString *msg = [NSString stringWithFormat:@"Delete mesh network, name:%@ uuid:%@ ?",network.name,network.uuid];
    __weak typeof(self) weakSelf = self;
    [self showAlertSureAndCancelWithTitle:kDefaultAlertTitle message:msg sure:^(UIAlertAction *action) {
        [TelinkHttpTool deleteNetworkByNetworkIdRequestWithNetworkId:network.networkId didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
            if (err) {
                TelinkLogInfo(@"delete fail")
                [weakSelf showTips:[NSString stringWithFormat:@"Delete mesh network fail, %@", err.localizedDescription]];
            } else {
                TelinkLogInfo(@"delete success")
                [SigDataSource.share cleanLocalPrivateBeaconStateWithMeshUUID:[NSString stringWithFormat:@"%ld", (long)network.networkId]];
                [AppDataSource.share.createdNetwordList removeObject:network];
                [SigDataSource.share saveLocationData];
                [weakSelf.createdList removeObject:network];
                [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                weakSelf.tableView.hidden = weakSelf.createdList.count == 0;
            }
        }];
    } cancel:^(UIAlertAction *action) {
        
    }];
}

#pragma  mark LongPressGesture
- (void)cellDidPress:(UILongPressGestureRecognizer *)sender{
    if (sender.state == UIGestureRecognizerStateBegan) {
        NSIndexPath *indexPath = [self.tableView indexPathForRowAtPoint:[sender locationInView:self.tableView]];
        if (indexPath != nil) {
            AppMeshNetworkModel *model = self.createdList[indexPath.row];
            [self clickMoreButtonWithNetwork:model];
        }
    }
}

#pragma mark - UITableViewDataSource

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.createdList.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    NetworkCell *cell = [tableView dequeueReusableCellWithIdentifier:NSStringFromClass(NetworkCell.class) forIndexPath:indexPath];
    AppMeshNetworkModel *model = self.createdList[indexPath.row];
    [cell setModel:model];
    __weak typeof(self) weakSelf = self;
    [cell.moreButton addAction:^(UIButton *button) {
        [weakSelf clickMoreButtonWithNetwork:model];
    }];
    return cell;
}

-(void)dealloc{
    NSLog(@"%s",__func__);
}

@end
