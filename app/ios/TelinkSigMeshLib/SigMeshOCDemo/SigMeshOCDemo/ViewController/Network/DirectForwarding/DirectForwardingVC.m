/********************************************************************************************************
 * @file     DirectForwardingVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/10/10
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

#import "DirectForwardingVC.h"
#import "AddForwardingTableVC.h"
#import "UIViewController+Message.h"
#import "TableCell.h"

@interface DirectForwardingVC ()<UITableViewDelegate,UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (strong, nonatomic) IBOutlet UIButton *addForwardingTableButton;
@property (nonatomic, strong) NSMutableArray <SigForwardingTableModel *>*forwardingTableModelList;
@property (nonatomic, strong) SigMessageHandle *messageHandle;
@end

@implementation DirectForwardingVC

- (IBAction)clickDirectToggles:(UIButton *)sender {
    UIViewController *vc = [UIStoryboard initVC:ViewControllerIdentifiers_DirectControlListVCID storyboard:@"Setting"];
    [self.navigationController pushViewController:vc animated:YES];
}

- (IBAction)addForwardingTable:(UIButton *)sender {
    TelinkLogDebug(@"");
    if (SigBearer.share.isOpen) {
        [self pushToAddForwardingTableVC];
    } else {
        [self showTips:@"The mesh is offline, app can not add forwarding table."];
    }
}

- (void)pushToAddForwardingTableVC {
    AddForwardingTableVC *vc = (AddForwardingTableVC *)[UIStoryboard initVC:ViewControllerIdentifiers_AddForwardingTableVCID storyboard:@"Setting"];
    vc.isNew = YES;
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)removeForwardingTable:(SigForwardingTableModel *)forwardingTable {
    TelinkLogDebug(@"");
    if (SigMeshLib.share.isBusyNow) {
        TelinkLogInfo(@"send request for ForwardingTableDelete, but busy now.");
        [self showTips:@"app is busy now, try again later."];
    } else {
        if (SigBearer.share.isOpen) {
            [ShowTipsHandle.share show:@"delete forwarding table..."];
            UInt16 pathOrigin = forwardingTable.pathOriginUnicastAddrRange.rangeStart;
            UInt16 pathDestination = forwardingTable.pathTargetUnicastAddrRange.rangeStart;
            if (forwardingTable.unicastDestinationFlag == NO) {
                pathDestination = forwardingTable.multicastDestination;
            }
            __weak typeof(self) weakSelf = self;
            NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
            [operationQueue addOperationWithBlock:^{
                //这个block语句块在子线程中执行
                NSMutableArray *array = [NSMutableArray arrayWithArray:forwardingTable.entryNodeAddress];
                //补起点
                if (![array containsObject:@(forwardingTable.tableSource)]) {
                    [array addObject:@(forwardingTable.tableSource)];
                }
                //补终点
                if (![array containsObject:@(forwardingTable.tableDestination)]) {
                    [array addObject:@(forwardingTable.tableDestination)];
                }

                for (NSNumber *addressNumber in array) {
                    UInt16 address = addressNumber.intValue;
                    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
                    TelinkLogInfo(@"send request for ForwardingTableDelete, address:%d", address);
                    [SDKLibCommand forwardingTableDeleteWithNetKeyIndex:forwardingTable.netKeyIndex pathOrigin:pathOrigin pathDestination:pathDestination destination:address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigForwardingTableStatus * _Nonnull responseMessage) {
                        TelinkLogDebug(@"forwardingTableDelete=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                        TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                        dispatch_semaphore_signal(semaphore);
                    }];
                    dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 4.0));
                }
                [weakSelf removeForwardingTableFromDataSource:forwardingTable];
            }];
        } else {
            [self showTips:@"The mesh is offline, app can not remove forwarding table."];
        }
    }
}

- (void)removeForwardingTableFromDataSource:(SigForwardingTableModel *)forwardingTable {
    NSArray *array = [NSArray arrayWithArray:SigDataSource.share.forwardingTableModelList];
    for (SigForwardingTableModel *model in array) {
        if ([model isEqual:forwardingTable]) {
            [SigDataSource.share.forwardingTableModelList removeObject:model];
            [self.forwardingTableModelList removeObject:model];
            [SigDataSource.share saveLocationData];
            break;
        }
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.tableView reloadData];
        [ShowTipsHandle.share show:@"delete forwarding table from node success!"];
        [ShowTipsHandle.share delayHidden:0.5];
    });
}

#pragma mark - Life method
- (void)normalSetting{
    [super normalSetting];
    self.title = @"Direct Forwarding";
    self.addForwardingTableButton.backgroundColor = UIColor.telinkButtonBlue;
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(TableCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(TableCell.class)];
    UILongPressGestureRecognizer *gesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(cellDidPress:)];
    [self.view addGestureRecognizer:gesture];
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
    self.forwardingTableModelList = [NSMutableArray arrayWithArray:SigDataSource.share.forwardingTableModelList];
    [self.tableView reloadData];
}

-(void)dealloc{
    if (_messageHandle) {
        [_messageHandle cancel];
    }
    NSLog(@"%s",__func__);
}

#pragma mark - UITableView
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.forwardingTableModelList.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    TableCell *cell = (TableCell *)[tableView dequeueReusableCellWithIdentifier:NSStringFromClass(TableCell.class) forIndexPath:indexPath];
    SigForwardingTableModel *model = self.forwardingTableModelList[indexPath.row];
    cell.model = model;
    return cell;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {
    SigForwardingTableModel *model = self.forwardingTableModelList[indexPath.row];
    return [TableCell getHeightOfModel:model];
}

//安卓没有编辑功能，暂时屏蔽
//- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
//    SigForwardingTableModel *model = self.forwardingTableModelList[indexPath.row];
//    AddForwardingTableVC *vc = (AddForwardingTableVC *)[UIStoryboard initVC:ViewControllerIdentifiers_AddForwardingTableVCID storyboard:@"Setting"];
//    vc.isNew = NO;
//    vc.oldTable = model;
//    [self.navigationController pushViewController:vc animated:YES];
//}

#pragma  mark LongPressGesture
- (void)cellDidPress:(UILongPressGestureRecognizer *)sender{
    if (sender.state == UIGestureRecognizerStateBegan) {
        NSIndexPath *indexPath = [self.tableView indexPathForRowAtPoint:[sender locationInView:self.tableView]];
        if (indexPath != nil) {
            if (!SigBearer.share.isOpen) {
                [self showAlertSureWithTitle:@"Hits" message:@"The mesh network is not online!" sure:nil];
                return;
            }

            SigForwardingTableModel *forwardingTableModel = self.forwardingTableModelList[indexPath.row];
            NSString *msg = [NSString stringWithFormat:@"Confirm to delete table?"];
            __weak typeof(self) weakSelf = self;
            [self showAlertSureAndCancelWithTitle:@"Warning" message:msg sure:^(UIAlertAction *action) {
                [weakSelf removeForwardingTable:forwardingTableModel];
            } cancel:^(UIAlertAction *action) {

            }];
        }
    }
}

@end
