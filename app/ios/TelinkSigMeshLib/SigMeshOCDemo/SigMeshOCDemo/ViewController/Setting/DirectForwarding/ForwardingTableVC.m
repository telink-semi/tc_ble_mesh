/********************************************************************************************************
 * @file     ForwardingTableVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2022/3/18
 *
 * @par     Copyright (c) [2022], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "ForwardingTableVC.h"
#import "AddForwardingTableVC.h"
#import "UIViewController+Message.h"

@interface ForwardingTableVC ()<UITableViewDelegate,UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) SigMessageHandle *messageHandle;
@end

@implementation ForwardingTableVC

- (IBAction)addForwardingTable:(UIButton *)sender {
    TeLogDebug(@"");
    if (SigBearer.share.isOpen) {
        [self pushToAddForwardingTableVC];
    } else {
        [self showTips:@"The mesh is offline, app cann`t add forwarding table."];
    }
}

- (void)pushToAddForwardingTableVC {
    AddForwardingTableVC *vc = (AddForwardingTableVC *)[UIStoryboard initVC:ViewControllerIdentifiers_AddForwardingTableVCID storyboard:@"Setting"];
    vc.isNew = YES;
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)removeForwardingTable:(SigForwardingTableModel *)forwardingTable {
    TeLogDebug(@"");
    if (SigMeshLib.share.isBusyNow) {
        TeLogInfo(@"send request for ForwardingTableDelete, but busy now.");
        [self showTips:@"app is busy now, try again later."];
    } else {
        if (SigBearer.share.isOpen) {
#ifdef kExist
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
                NSArray *array = [NSArray arrayWithArray:forwardingTable.entryNodeAddress];
                for (NSNumber *addressNumber in array) {
                    UInt16 address = addressNumber.intValue;
                    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
                    TeLogInfo(@"send request for ForwardingTableDelete, address:%d", address);
                    [SDKLibCommand forwardingTableDeleteWithNetKeyIndex:forwardingTable.netKeyIndex pathOrigin:pathOrigin pathDestination:pathDestination destination:address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigForwardingTableStatus * _Nonnull responseMessage) {
                        TeLogDebug(@"forwardingTableDelete=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                        TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                        dispatch_semaphore_signal(semaphore);
                    }];
                    dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 4.0));
                }
                [weakSelf removeForwardingTableFromDataSource:forwardingTable];
            }];
#endif
        } else {
            [self showTips:@"The mesh is offline, app cann`t remove forwarding table."];
        }
    }
}

- (void)removeForwardingTableFromDataSource:(SigForwardingTableModel *)forwardingTable {
#ifdef kExist
    NSArray *array = [NSArray arrayWithArray:SigDataSource.share.forwardingTableModelList];
    for (SigForwardingTableModel *model in array) {
        if ([model isEqual:forwardingTable]) {
            [SigDataSource.share.forwardingTableModelList removeObject:model];
            [SigDataSource.share saveLocationData];
            break;
        }
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.tableView reloadData];
        [ShowTipsHandle.share show:@"delete forwarding table from node success!"];
        [ShowTipsHandle.share delayHidden:2.0];
    });
#endif
}

#pragma mark - Life method
- (void)normalSetting{
    [super normalSetting];
    self.title = @"Forwarding Table List";
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    UILongPressGestureRecognizer *gesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(cellDidPress:)];
    [self.view addGestureRecognizer:gesture];
//    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh target:self action:@selector(clickRefreshDirectControlStatus)];
//    self.navigationItem.rightBarButtonItem = rightItem;
}

//- (void)clickRefreshDirectControlStatus {
//    [SDKLibCommand directControlGetWithNetKeyIndex:SigDataSource.share.curNetkeyModel.index destination:self.model.address retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigDirectControlStatus * _Nonnull responseMessage) {
//        TeLogInfo(@"directControlGet responseMessage=%@,parameters=%@,source=0x%x,destination=0x%x",responseMessage,responseMessage.parameters,source,destination);
//        if (responseMessage.status == SigConfigMessageStatus_success) {
//            [ShowTipsHandle.share show:@"directControlGet from node success!"];
//        } else {
//            [ShowTipsHandle.share show:[NSString stringWithFormat:@"directControlGet from node fail! error status=%d",responseMessage.status]];
//        }
//    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
//        TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
//        dispatch_async(dispatch_get_main_queue(), ^{
//            if (error) {
//                [ShowTipsHandle.share show:[NSString stringWithFormat:@"directControlGet from node fail! error=%@",error]];
//            }
//            [ShowTipsHandle.share delayHidden:2.0];
//        });
//    }];
//}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
    [self.tableView reloadData];
}

-(void)dealloc{
    if (_messageHandle) {
        [_messageHandle cancel];
    }
    NSLog(@"%s",__func__);
}

- (void)showTips:(NSString *)message{
    [self showAlertSureWithTitle:@"Hits" message:message sure:^(UIAlertAction *action) {
        
    }];
}

#pragma mark - UITableView
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return SigDataSource.share.forwardingTableModelList.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    static NSString *cellID = @"testCell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:cellID];
    if (!cell) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:cellID];
    }
    cell.textLabel.numberOfLines = 0;
    cell.textLabel.font = [UIFont systemFontOfSize:13.0];
#ifdef kExist
    SigForwardingTableModel *model = SigDataSource.share.forwardingTableModelList[indexPath.row];
    cell.textLabel.text = model.getDescription;
#endif
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    SigForwardingTableModel *model = SigDataSource.share.forwardingTableModelList[indexPath.row];
    AddForwardingTableVC *vc = (AddForwardingTableVC *)[UIStoryboard initVC:ViewControllerIdentifiers_AddForwardingTableVCID storyboard:@"Setting"];
    vc.isNew = NO;
    vc.oldTable = model;
    [self.navigationController pushViewController:vc animated:YES];
}

#pragma  mark LongPressGesture
- (void)cellDidPress:(UILongPressGestureRecognizer *)sender{
    if (sender.state == UIGestureRecognizerStateBegan) {
        NSIndexPath *indexPath = [self.tableView indexPathForRowAtPoint:[sender locationInView:self.tableView]];
        if (indexPath != nil) {
            if (!SigBearer.share.isOpen) {
                [self showAlertSureWithTitle:@"Hits" message:@"The mesh network is not online!" sure:nil];
                return;
            }

            SigForwardingTableModel *forwardingTableModel = SigDataSource.share.forwardingTableModelList[indexPath.row];
            NSString *msg = [NSString stringWithFormat:@"Are you sure delete this forwarding table "];
            __weak typeof(self) weakSelf = self;
            [self showAlertSureAndCancelWithTitle:@"Hits" message:msg sure:^(UIAlertAction *action) {
                [weakSelf removeForwardingTable:forwardingTableModel];
            } cancel:^(UIAlertAction *action) {
                
            }];
        }
    }
}

@end
