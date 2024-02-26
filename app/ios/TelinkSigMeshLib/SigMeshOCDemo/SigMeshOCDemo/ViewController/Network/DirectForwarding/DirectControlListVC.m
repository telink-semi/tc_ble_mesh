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
@end

@implementation DirectControlListVC

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    DirectToggleCell *cell = (DirectToggleCell *)[tableView dequeueReusableCellWithIdentifier:NSStringFromClass(DirectToggleCell.class) forIndexPath:indexPath];
    cell.accessoryType = UITableViewCellAccessoryNone;
    SigNodeModel *model = self.source[indexPath.row];
    cell.model = model;
    __weak typeof(self) weakSelf = self;
    [cell.forwardingButton addAction:^(UIButton *button) {
        [weakSelf setDirectedForwarding:model.directControlStatus.directedForwardingState == DirectedForwardingState_disable ? DirectedForwarding_enable : DirectedForwarding_disable node:model];
    }];
    [cell.relayButton addAction:^(UIButton *button) {
        [weakSelf setDirectedRelay:model.directControlStatus.directedRelayState == DirectedRelayState_disable ? DirectedRelay_enable : DirectedRelay_disable node:model];
    }];
    [cell.proxyButton addAction:^(UIButton *button) {
        [weakSelf setDirectedProxy:model.directControlStatus.directedProxyState == DirectedProxyState_disable ? DirectedProxy_enable : DirectedProxy_disable node:model];
    }];
    [cell.friendButton addAction:^(UIButton *button) {
        [weakSelf setDirectedFriend:model.directControlStatus.directedFriendState == DirectedFriendState_disable ? DirectedFriend_enable : DirectedFriend_disable node:model];
    }];
    return cell;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.source.count;
}

- (void)setDirectedForwarding:(DirectedForwarding)directedForwarding node:(SigNodeModel *)node {
    [self directControlSetWithDirectedForwarding:directedForwarding directedRelay:directedForwarding == DirectedForwarding_disable ? DirectedRelay_disable : (DirectedRelay)node.directControlStatus.directedRelayState directedProxy:directedForwarding == DirectedForwarding_disable ? DirectedProxy_disable : (DirectedProxy)node.directControlStatus.directedProxyState directedFriend:DirectedFriend_ignore node:node];
}

- (void)setDirectedRelay:(DirectedRelay)directedRelay node:(SigNodeModel *)node {
    if (node.directControlStatus.directedForwardingState == DirectedForwardingState_disable) {
        [self showTips:@"(relay)check direct forwarding first"];
        return;
    }
    [self directControlSetWithDirectedForwarding:(DirectedForwarding)node.directControlStatus.directedForwardingState directedRelay:directedRelay directedProxy:(DirectedProxy)node.directControlStatus.directedProxyState directedFriend:DirectedFriend_ignore node:node];
}

- (void)setDirectedProxy:(DirectedProxy)directedProxy node:(SigNodeModel *)node {
    if (node.directControlStatus.directedForwardingState == DirectedForwardingState_disable) {
        [self showTips:@"(proxy)check direct forwarding first"];
        return;
    }
    [self directControlSetWithDirectedForwarding:(DirectedForwarding)node.directControlStatus.directedForwardingState directedRelay:(DirectedRelay)node.directControlStatus.directedRelayState directedProxy:directedProxy directedFriend:DirectedFriend_ignore node:node];
}

- (void)setDirectedFriend:(DirectedFriend)directedFriend node:(SigNodeModel *)node {
    if (node.directControlStatus.directedForwardingState == DirectedForwardingState_disable) {
        [self showTips:@"(friend)check direct forwarding first"];
        return;
    }
    [self directControlSetWithDirectedForwarding:(DirectedForwarding)node.directControlStatus.directedForwardingState directedRelay:(DirectedRelay)node.directControlStatus.directedRelayState directedProxy:(DirectedProxy)node.directControlStatus.directedProxyState directedFriend:directedFriend node:node];
}

- (void)directControlSetWithDirectedForwarding:(DirectedForwarding)directedForwarding directedRelay:(DirectedRelay)directedRelay directedProxy:(DirectedProxy)directedProxy directedFriend:(DirectedFriend)directedFriend node:(SigNodeModel *)node {
    if (node.state == DeviceStateOutOfLine) {
        [self showTips:Tip_DeviceOutline];
        return;
    }
    [ShowTipsHandle.share show:Tip_SetDirectControl];
    __weak typeof(self) weakSelf = self;
    [SDKLibCommand directControlSetWithNetKeyIndex:SigDataSource.share.curNetkeyModel.index directedForwarding:directedForwarding directedRelay:directedRelay directedProxy:directedProxy directedProxyUseDirectedDefault:directedProxy == DirectedProxy_enable ? DirectedProxyUseDirectedDefault_enable : DirectedProxyUseDirectedDefault_ignore directedFriend:directedFriend destination:node.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigDirectControlStatus * _Nonnull responseMessage) {
        TelinkLogDebug(@"directControlSet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
        if (source == node.address) {
            dispatch_async(dispatch_get_main_queue(), ^{
                if (responseMessage.status == SigConfigMessageStatus_success) {
                    [ShowTipsHandle.share show:Tip_SetDirectControlSuccess];
                    node.directControlStatus = responseMessage;
                    [SigDataSource.share saveLocationData];
                } else {
                    [ShowTipsHandle.share show:[NSString stringWithFormat:@"directControlSet fail, status code = %d", responseMessage.status]];
                }
                [ShowTipsHandle.share delayHidden:0.1];
            });
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TelinkLogInfo(@"isResponseAll=%d,error=%@", isResponseAll, error);
        dispatch_async(dispatch_get_main_queue(), ^{
            if (error) {
                [ShowTipsHandle.share show:error.domain];
                [ShowTipsHandle.share delayHidden:0.1];
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
}

- (void)updateData{
    self.source = [NSArray arrayWithArray:SigDataSource.share.curNodes];
    [self.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
}

@end
