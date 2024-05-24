/********************************************************************************************************
 * @file     JoinedNetworkListVC.m
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

#import "JoinedNetworkListVC.h"
#import "JoinedNetworkCell.h"
#import "NSString+extension.h"
#import "UIViewController+Message.h"
#import "UIButton+extension.h"
#import "ScanCodeVC.h"
#import "CustomShareAlertView.h"

@interface JoinedNetworkListVC ()<UITableViewDelegate, UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIView *scanView;
@property (nonatomic, strong) NSMutableArray <AppJoinedNetworkModel *>*sharedList;
@property (strong, nonatomic) ScanCodeVC *scanCodeVC;
@end

@implementation JoinedNetworkListVC

- (void)viewDidLoad {
    [super viewDidLoad];
    self.scanView.layer.cornerRadius = self.scanView.bounds.size.width/2;
    self.scanView.layer.masksToBounds = YES;
    [self addNoDataUI];
    __weak typeof(self) weakSelf = self;
    [self addCustomCircularButtonWithImageString:@"scan" action:^(UIButton *button) {
        [weakSelf.navigationController pushViewController:weakSelf.scanCodeVC animated:YES];
    }];
    [self setSelectBlock:^{
        [weakSelf configRightBarButtonItem];
    }];
    self.sharedList = [NSMutableArray arrayWithArray:AppDataSource.share.sharedNetwordList];
    UIView *footerView = [[UIView alloc] initWithFrame:CGRectZero];
    self.tableView.tableFooterView = footerView;
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(JoinedNetworkCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(JoinedNetworkCell.class)];
    //longpress to delete scene
    UILongPressGestureRecognizer *gesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(cellDidPress:)];
    [self.view addGestureRecognizer:gesture];
    [self clickRefreshJoinedButton];
}

- (void)configRightBarButtonItem {
    //init rightBarButtonItem
    UIBarButtonItem *rightItem1 = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh target:self action:@selector(clickRefreshJoinedButton)];
    self.supperVC.navigationItem.rightBarButtonItem = rightItem1;
}

- (ScanCodeVC *)scanCodeVC {
    if (!_scanCodeVC) {
        _scanCodeVC = [ScanCodeVC scanCodeVC];
        __weak typeof(self) weakSelf = self;
        [_scanCodeVC scanDataViewControllerBackBlock:^(id content) {
            //AnalysisShareDataVC
            NSString *shareString = (NSString *)content;
            NSDictionary *shareDict = [LibTools getDictionaryWithJsonString:shareString];
            if (shareDict.count > 0) {
                [weakSelf joinNetworkWithShareDict:shareDict];
            }else{
                [[NSNotificationCenter defaultCenter] postNotificationName:@"BackToMain" object:nil];
                //hasn't data
                [weakSelf showTips:@"QRCode is error."];
            }
        }];
    }
    return _scanCodeVC;
}

- (void)joinNetworkWithShareDict:(NSDictionary *)shareDict {
    TelinkLogInfo(@"shareDict=%@", shareDict);
    NSString *msg = [NSString stringWithFormat:@"join network?\nname:%@\ncreator:%@", shareDict[@"networkName"], shareDict[@"userName"]];
    __weak typeof(self) weakSelf = self;
    [self showAlertSureAndCancelWithTitle:@"Warning" message:msg sure:^(UIAlertAction *action) {
        [TelinkHttpTool joinNetworkFromLinkRequestWithShareLinkId:[shareDict[@"shareLinkId"] intValue] didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
            [[NSNotificationCenter defaultCenter] postNotificationName:@"BackToMain" object:nil];
            if (err) {
                TelinkLogInfo(@"joinNetworkFromLink error = %@", err);
                [weakSelf showTips:[NSString stringWithFormat:@"joinNetworkFromLink error = %@", err.localizedDescription]];
            } else {
                NSDictionary *dic = (NSDictionary *)result;
                int code = [dic[@"code"] intValue];
                if (code == 200) {
                    TelinkLogInfo(@"joinNetworkFromLink successful! dic=%@", dic);
                    [weakSelf clickRefreshJoinedButton];
                } else {
                    TelinkLogInfo(@"joinNetworkFromLink result = %@", dic);
                    [weakSelf showTips:[NSString stringWithFormat:@"joinNetworkFromLink errorCode = %d, message = %@", code, dic[@"message"]]];
                }
            }
        }];
    } cancel:^(UIAlertAction *action) {
        [[NSNotificationCenter defaultCenter] postNotificationName:@"BackToMain" object:nil];
    }];
}

- (void)clickRefreshJoinedButton {
    __weak typeof(self) weakSelf = self;
    [AppDataSource.share getJoinedNetworkListWithResultBlock:^(NSError * _Nullable error) {
        if (error) {
            [weakSelf showTips:error.localizedDescription];
        } else {
            if (AppDataSource.share.sharedNetwordList.count == 0) {
                [weakSelf showTips:@"no joined netword."];
                weakSelf.sharedList = [NSMutableArray array];
            } else {
                weakSelf.sharedList = [NSMutableArray arrayWithArray:AppDataSource.share.sharedNetwordList];
            }
            weakSelf.tableView.hidden = AppDataSource.share.sharedNetwordList.count == 0;
            [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
        }
    }];
}

#pragma  mark LongPressGesture
- (void)cellDidPress:(UILongPressGestureRecognizer *)sender{
    if (sender.state == UIGestureRecognizerStateBegan) {
        NSIndexPath *indexPath = [self.tableView indexPathForRowAtPoint:[sender locationInView:self.tableView]];
        if (indexPath != nil) {
            AppJoinedNetworkModel *model = self.sharedList[indexPath.row];
            [self clickMoreButtonWithNetwork:model];
        }
    }
}

- (void)clickMoreButtonWithNetwork:(AppJoinedNetworkModel *)network {
    ShareAlertItemModel *item1 = [[ShareAlertItemModel alloc] init];
    item1.itemType = ShareItemType_icon_text_next;
    item1.iconImage = [UIImage imageNamed:@"ic_switch"];
    item1.textString = @"switch to this network";
    item1.textColor = HEX(#4A87EE);
    ShareAlertItemModel *item2 = [[ShareAlertItemModel alloc] init];
    item2.itemType = ShareItemType_button;
    item2.textString = @"LEAVE";
    NSMutableArray *mArray = [NSMutableArray array];
    if (network.state == ResponseShareStateVisitor || network.state == ResponseShareStateMember) {
        [mArray addObject:item1];
    }
    [mArray addObject:item2];
    
    __weak typeof(self) weakSelf = self;
    CustomShareAlertView *customAlertView = [[CustomShareAlertView alloc] initWithTitle:[NSString stringWithFormat:@"select action for: %@", network.networkName] itemArray:mArray alertResult:^(NSInteger selectIndex) {
        ShareAlertItemModel *item = mArray[selectIndex];
        if (item.itemType == ShareItemType_button) {
            [weakSelf clickLeaveButtonWithNetwork:network];
        } else {
            [weakSelf clickSwitchButtonWithNetwork:network];
        }
    }];
    [customAlertView showCustomAlertView];
}

- (void)clickLeaveButtonWithNetwork:(AppJoinedNetworkModel *)network {
    __weak typeof(self) weakSelf = self;
    [TelinkHttpTool leaveNetworkRequestWithShareInfoId:network.joinNetworkId didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"leaveNetwork error = %@", err);
            [weakSelf showTips:[NSString stringWithFormat:@"leaveNetwork error = %@", err.localizedDescription]];
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"leaveNetwork successful! dic=%@", dic);
                [weakSelf clickRefreshJoinedButton];
                [SigDataSource.share cleanLocalPrivateBeaconStateWithMeshUUID:[NSString stringWithFormat:@"%ld", (long)network.networkId]];
            } else {
                TelinkLogInfo(@"leaveNetwork result = %@", dic);
                [weakSelf showTips:[NSString stringWithFormat:@"leaveNetwork errorCode = %d, message = %@", code, dic[@"message"]]];
            }
        }
    }];
}

- (void)clickSwitchButtonWithNetwork:(AppJoinedNetworkModel *)network {
    AppDataSource.share.curNetworkId = network.networkId;
    __weak typeof(self) weakSelf = self;
    [SDKLibCommand stopMeshConnectWithComplete:nil];
    [AppDataSource.share getDetailWithNetworkId:network.networkId type:1 resultBlock:^(NSError * _Nullable error) {
        if (error) {
            [weakSelf showTips:error.localizedDescription];
        } else {
            [weakSelf saveLocalWithCurrentNetworkId:network.networkId type:1];
            [weakSelf setBaseTabbarControllerToRootViewController];
        }
    }];
}

#pragma mark - UITableViewDataSource

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.sharedList.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    JoinedNetworkCell *cell = [tableView dequeueReusableCellWithIdentifier:NSStringFromClass(JoinedNetworkCell.class) forIndexPath:indexPath];
    AppJoinedNetworkModel *model = self.sharedList[indexPath.row];
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
