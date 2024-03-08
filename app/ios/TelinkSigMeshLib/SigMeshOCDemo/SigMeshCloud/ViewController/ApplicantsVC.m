/********************************************************************************************************
 * @file     ApplicantsVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/5/22
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

#import "ApplicantsVC.h"
#import "ApplicantCell.h"
#import "CustomShareAlertView.h"
#import "UIButton+extension.h"

@interface ApplicantsVC ()<UITableViewDelegate, UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <AppJoinedNetworkModel *>*applicantList;

@end

@implementation ApplicantsVC

- (void)viewDidLoad {
    [super viewDidLoad];
    self.title = @"Applicants";
    self.applicantList = [NSMutableArray array];
    UIView *footerView = [[UIView alloc] initWithFrame:CGRectZero];
    self.tableView.tableFooterView = footerView;
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(ApplicantCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(ApplicantCell.class)];
    //init rightBarButtonItem
    UIBarButtonItem *rightItem1 = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh target:self action:@selector(clickRefreshButton)];
    self.navigationItem.rightBarButtonItem = rightItem1;
    [self addNoDataUI];
    [self clickRefreshButton];
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)clickRefreshButton {
    __weak typeof(self) weakSelf = self;
    [TelinkHttpTool getShareInfoListRequestWithNetworkId:self.networkId state:ResponseShareStateAll didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"getSharedNetworkList error = %@", err);
            [weakSelf showTips:err.localizedDescription];
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                NSMutableArray *mArray = [NSMutableArray array];
                TelinkLogInfo(@"getSharedNetworkList successful! dic=%@", dic);
                NSArray *array = dic[@"data"];
                //show no data UI.
                weakSelf.tableView.hidden = array.count == 0;
                if (array.count > 0) {
                    //有分享的Mesh，解析并存储本地
                    NSArray *arr = [NSArray arrayWithArray:array];
                    for (NSDictionary *dict in arr) {
                        AppJoinedNetworkModel *model = [[AppJoinedNetworkModel alloc] init];
                        [model setDictionaryToAppJoinedNetworkModel:dict];
                        [mArray addObject:model];
                    }
                    weakSelf.applicantList = mArray;
                    [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                }
            } else {
                NSString *msg = [NSString stringWithFormat:@"getSharedNetworkList errorCode = %d, message = %@", code, dic[@"message"]];
                TelinkLogInfo(@"%@", msg);
                [weakSelf showTips:msg];
            }
        }
    }];
}

- (void)clickMoreButtonWithNetwork:(AppJoinedNetworkModel *)network {
    ShareAlertItemModel *item1 = [[ShareAlertItemModel alloc] init];
    item1.itemType = ShareItemType_text;
    item1.textString = [NSString stringWithFormat:@"State:%@", [DemoTool getStateStringWithState:network.state]];
    item1.textColor = [DemoTool getStateColorWithState:network.state];
    ShareAlertItemModel *item2 = [[ShareAlertItemModel alloc] init];
    item2.itemType = ShareItemType_icon_text;
    item2.textString = @"Modify as MEMBER";
    item2.iconImage = [DemoTool getStateImageWithState:ResponseShareStateMember];
    item2.textColor = [DemoTool getStateColorWithState:ResponseShareStateMember];
    item2.state = ResponseShareStateMember;
    ShareAlertItemModel *item3 = [[ShareAlertItemModel alloc] init];
    item3.itemType = ShareItemType_icon_text;
    item3.textString = @"Modify as VISITOR";
    item3.iconImage = [DemoTool getStateImageWithState:ResponseShareStateVisitor];
    item3.textColor = [DemoTool getStateColorWithState:ResponseShareStateVisitor];
    item3.state = ResponseShareStateVisitor;
    ShareAlertItemModel *item4 = [[ShareAlertItemModel alloc] init];
    item4.itemType = ShareItemType_icon_text;
    item4.textString = @"FORBIDDEN";
    item4.iconImage = [DemoTool getStateImageWithState:ResponseShareStateForbidden];
    item4.textColor = [DemoTool getStateColorWithState:ResponseShareStateForbidden];
    item4.state = ResponseShareStateForbidden;

    NSMutableArray *itemArray = [NSMutableArray arrayWithArray:@[item1, item2, item3, item4]];
    if (network.state == ResponseShareStateForbidden) {
        [itemArray removeObject:item4];
    } else if (network.state == ResponseShareStateMember) {
        [itemArray removeObject:item2];
    } else if (network.state == ResponseShareStateVisitor) {
        [itemArray removeObject:item3];
    }
    
    __weak typeof(self) weakSelf = self;
    CustomShareAlertView *customAlertView = [[CustomShareAlertView alloc] initWithTitle:@"Share Info Detail" itemArray:itemArray alertResult:^(NSInteger selectIndex) {
        ShareAlertItemModel *item = itemArray[selectIndex];
        if (selectIndex == 0) {
            //状态显示，无点击事件
        } else {
            [weakSelf changeShareInfoWithNetwork:network state:item.state];
        }
    }];
    [customAlertView showCustomAlertView];
}

- (void)changeShareInfoWithNetwork:(AppJoinedNetworkModel *)network state:(ResponseShareState)state {
    __weak typeof(self) weakSelf = self;
    [TelinkHttpTool updateShareInfoStateRequestWithShareInfoId:network.joinNetworkId state:state didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"updateShareInfoState error = %@", err);
            [weakSelf showTips:err.localizedDescription];
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"updateShareInfoState successful! dic=%@", dic);
                network.state = state;
                [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
            } else {
                NSString *msg = [NSString stringWithFormat:@"updateShareInfoState errorCode = %d, message = %@", code, dic[@"message"]];
                TelinkLogInfo(@"%@", msg);
                [weakSelf showTips:msg];
            }
        }
    }];
}

#pragma  mark LongPressGesture
- (void)cellDidPress:(UILongPressGestureRecognizer *)sender{
    if (sender.state == UIGestureRecognizerStateBegan) {
        NSIndexPath *indexPath = [self.tableView indexPathForRowAtPoint:[sender locationInView:self.tableView]];
        if (indexPath != nil) {
            AppJoinedNetworkModel *model = self.applicantList[indexPath.row];
            [self clickMoreButtonWithNetwork:model];
        }
    }
}

#pragma mark - UITableViewDataSource

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.applicantList.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    ApplicantCell *cell = [tableView dequeueReusableCellWithIdentifier:NSStringFromClass(ApplicantCell.class) forIndexPath:indexPath];
    AppJoinedNetworkModel *model = self.applicantList[indexPath.row];
    [cell setModel:model];
    __weak typeof(self) weakSelf = self;
    [cell.moreButton addAction:^(UIButton *button) {
        [weakSelf clickMoreButtonWithNetwork:model];
    }];
    return cell;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 130;
}

-(void)dealloc{
    NSLog(@"%s",__func__);
}

@end
