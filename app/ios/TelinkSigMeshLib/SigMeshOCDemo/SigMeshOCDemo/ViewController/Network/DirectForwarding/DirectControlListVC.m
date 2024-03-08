/********************************************************************************************************
 * @file     DirectControlListVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2022/3/18
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "DirectControlListVC.h"
#import "DirectToggleCell.h"
#import "UIButton+extension.h"

@interface DirectControlListVC ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSArray <SigNodeModel *>*source;
@property (nonatomic, strong) SigNodeModel *allNode;
@end

@implementation DirectControlListVC

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    DirectToggleCell *cell = (DirectToggleCell *)[tableView dequeueReusableCellWithIdentifier:NSStringFromClass(DirectToggleCell.class) forIndexPath:indexPath];
    cell.accessoryType = UITableViewCellAccessoryNone;
    SigNodeModel *model = self.source[indexPath.row];
    cell.model = model;
    NSArray *nodeList = @[model];
    if (model.address == 0xFFFF) {
        nodeList = [NSArray arrayWithArray:SigDataSource.share.curNodes];
        BOOL hasOnline = NO;
        for (SigNodeModel *node in self.source) {
            if (node.state != DeviceStateOutOfLine) {
                hasOnline = YES;
                break;
            }
        }
        cell.iconImageView.image = [UIImage imageNamed:hasOnline ? @"dengs" : @"dengo"];
        cell.nameLabel.text = @"adr-All Nodes Setting";
    }
    __weak typeof(self) weakSelf = self;
    [cell.forwardingButton addAction:^(UIButton *button) {
        [weakSelf setDirectedForwarding:model.directControlStatus.directedForwardingState == DirectedForwardingState_disable ? DirectedForwarding_enable : DirectedForwarding_disable nodeList:nodeList];
    }];
    [cell.relayButton addAction:^(UIButton *button) {
        [weakSelf setDirectedRelay:model.directControlStatus.directedRelayState == DirectedRelayState_disable ? DirectedRelay_enable : DirectedRelay_disable nodeList:nodeList];
    }];
    [cell.proxyButton addAction:^(UIButton *button) {
        [weakSelf setDirectedProxy:model.directControlStatus.directedProxyState == DirectedProxyState_disable ? DirectedProxy_enable : DirectedProxy_disable nodeList:nodeList];
    }];
    [cell.friendButton addAction:^(UIButton *button) {
        [weakSelf setDirectedFriend:model.directControlStatus.directedFriendState == DirectedFriendState_disable ? DirectedFriend_enable : DirectedFriend_disable nodeList:nodeList];
    }];
    return cell;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.source.count;
}

- (void)setDirectedForwarding:(DirectedForwarding)directedForwarding nodeList:(NSArray <SigNodeModel *>*)nodeList {
    __weak typeof(self) weakSelf = self;
    NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
    [operationQueue addOperationWithBlock:^{
        //这个block语句块在子线程中执行
        for (int i=0; i<nodeList.count; i++) {
            SigNodeModel *node = nodeList[i];
            dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
            [weakSelf directControlSetWithDirectedForwarding:directedForwarding directedRelay:directedForwarding == DirectedForwarding_disable ? DirectedRelay_disable : (DirectedRelay)node.directControlStatus.directedRelayState directedProxy:directedForwarding == DirectedForwarding_disable ? DirectedProxy_disable : (DirectedProxy)node.directControlStatus.directedProxyState directedFriend:DirectedFriend_ignore node:node resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                dispatch_semaphore_signal(semaphore);
            }];
            dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
        }
        if (nodeList.count > 1) {
            weakSelf.allNode.directControlStatus.directedForwardingState = (DirectedForwardingState)directedForwarding;
        }
        [weakSelf finishAction];
    }];
}

- (void)setDirectedRelay:(DirectedRelay)directedRelay nodeList:(NSArray <SigNodeModel *>*)nodeList {
    __weak typeof(self) weakSelf = self;
    NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
    [operationQueue addOperationWithBlock:^{
        //这个block语句块在子线程中执行
        for (int i=0; i<nodeList.count; i++) {
            SigNodeModel *node = nodeList[i];
            if (node.directControlStatus.directedForwardingState == DirectedForwardingState_disable) {
                [weakSelf showTips:@"(relay)check direct forwarding first"];
                break;
            }
            dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
            [weakSelf directControlSetWithDirectedForwarding:(DirectedForwarding)node.directControlStatus.directedForwardingState directedRelay:directedRelay directedProxy:(DirectedProxy)node.directControlStatus.directedProxyState directedFriend:DirectedFriend_ignore node:node resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                dispatch_semaphore_signal(semaphore);
            }];
            dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
        }
        if (nodeList.count > 1) {
            weakSelf.allNode.directControlStatus.directedRelayState = (DirectedRelayState)directedRelay;
        }
        [weakSelf finishAction];
    }];
}

- (void)setDirectedProxy:(DirectedProxy)directedProxy nodeList:(NSArray <SigNodeModel *>*)nodeList {
    __weak typeof(self) weakSelf = self;
    NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
    [operationQueue addOperationWithBlock:^{
        //这个block语句块在子线程中执行
        for (int i=0; i<nodeList.count; i++) {
            SigNodeModel *node = nodeList[i];
            if (node.directControlStatus.directedForwardingState == DirectedForwardingState_disable) {
                [self showTips:@"(proxy)check direct forwarding first"];
                break;
            }
            dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
            [weakSelf directControlSetWithDirectedForwarding:(DirectedForwarding)node.directControlStatus.directedForwardingState directedRelay:(DirectedRelay)node.directControlStatus.directedRelayState directedProxy:directedProxy directedFriend:DirectedFriend_ignore node:node resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                dispatch_semaphore_signal(semaphore);
            }];
            dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
        }
        if (nodeList.count > 1) {
            weakSelf.allNode.directControlStatus.directedProxyState = (DirectedProxyState)directedProxy;
        }
        [weakSelf finishAction];
    }];
}

- (void)setDirectedFriend:(DirectedFriend)directedFriend nodeList:(NSArray <SigNodeModel *>*)nodeList {
    __weak typeof(self) weakSelf = self;
    NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
    [operationQueue addOperationWithBlock:^{
        //这个block语句块在子线程中执行
        for (int i=0; i<nodeList.count; i++) {
            SigNodeModel *node = nodeList[i];
            if (node.directControlStatus.directedForwardingState == DirectedForwardingState_disable) {
                [self showTips:@"(friend)check direct forwarding first"];
                break;
            }
            dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
            [weakSelf directControlSetWithDirectedForwarding:(DirectedForwarding)node.directControlStatus.directedForwardingState directedRelay:(DirectedRelay)node.directControlStatus.directedRelayState directedProxy:(DirectedProxy)node.directControlStatus.directedProxyState directedFriend:directedFriend node:node resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                dispatch_semaphore_signal(semaphore);
            }];
            dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
        }
        if (nodeList.count > 1) {
            weakSelf.allNode.directControlStatus.directedFriendState = (DirectedFriendState)directedFriend;
        }
        [weakSelf finishAction];
    }];
}

- (void)finishAction {
    [self updateData];
    dispatch_async(dispatch_get_main_queue(), ^{
        [ShowTipsHandle.share delayHidden:0.1];
    });
}

- (void)directControlSetWithDirectedForwarding:(DirectedForwarding)directedForwarding directedRelay:(DirectedRelay)directedRelay directedProxy:(DirectedProxy)directedProxy directedFriend:(DirectedFriend)directedFriend node:(SigNodeModel *)node resultCallback:(resultBlock)resultCallback {
    if (node.state == DeviceStateOutOfLine) {
        [self showTips:[NSString stringWithFormat:@"%@ address:0x%04X", Tip_DeviceOutline, node.address]];
        if (resultCallback) {
            NSError *error = [NSError errorWithDomain:[NSString stringWithFormat:@"The address=0x04%X node is outline.", node.address] code:-1 userInfo:nil];
            resultCallback(NO, error);
        }
        return;
    }
    [ShowTipsHandle.share show:[NSString stringWithFormat:@"%@ address:0x%04X", Tip_SetDirectControl, node.address]];
    __weak typeof(self) weakSelf = self;
    [SDKLibCommand directControlSetWithNetKeyIndex:SigDataSource.share.curNetkeyModel.index directedForwarding:directedForwarding directedRelay:directedRelay directedProxy:directedProxy directedProxyUseDirectedDefault:directedProxy == DirectedProxy_enable ? DirectedProxyUseDirectedDefault_enable : DirectedProxyUseDirectedDefault_ignore directedFriend:directedFriend destination:node.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigDirectControlStatus * _Nonnull responseMessage) {
        TelinkLogDebug(@"directControlSet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
        if (source == node.address) {
            dispatch_async(dispatch_get_main_queue(), ^{
                if (responseMessage.status == SigConfigMessageStatus_success) {
                    [ShowTipsHandle.share show:[NSString stringWithFormat:@"%@ address:0x%04X", Tip_SetDirectControlSuccess, node.address]];
                    node.directControlStatus = responseMessage;
                    [SigDataSource.share saveLocationData];
                } else {
                    [ShowTipsHandle.share show:[NSString stringWithFormat:@"directControlSet fail, status code = %d", responseMessage.status]];
                }
            });
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TelinkLogInfo(@"isResponseAll=%d,error=%@", isResponseAll, error);
        if (resultCallback) {
            resultCallback(isResponseAll, error);
        }
        dispatch_async(dispatch_get_main_queue(), ^{
            if (error) {
                [ShowTipsHandle.share show:[NSString stringWithFormat:@"%@ address:0x%04X", error.domain, node.address]];
            }
            [weakSelf updateData];
        });
    }];
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
    [self updateData];
}

- (void)normalSetting{
    [super normalSetting];
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(DirectToggleCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(DirectToggleCell.class)];
    self.source = [[NSMutableArray alloc] init];
    self.title = @"Direct Toggle List";
    self.allNode = [[SigNodeModel alloc] init];
    SigDirectControlStatus *status = [[SigDirectControlStatus alloc] init];
    self.allNode.directControlStatus = status;
    self.allNode.unicastAddress = @"FFFF";
}

- (void)updateData{
    NSMutableArray *mArray = [NSMutableArray arrayWithObject:self.allNode];
    [mArray addObjectsFromArray:SigDataSource.share.curNodes];
    self.source = mArray;
    [self.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
}

@end
