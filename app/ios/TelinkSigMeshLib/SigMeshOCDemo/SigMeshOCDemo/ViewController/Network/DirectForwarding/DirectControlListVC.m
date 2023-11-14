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
#import "SettingItemCell.h"

@interface DirectControlListVC ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <SigNodeModel *>*source;
@end

@implementation DirectControlListVC

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    SettingItemCell *cell = (SettingItemCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_SettingItemCellID forIndexPath:indexPath];
    cell.accessoryType = UITableViewCellAccessoryNone;
    cell.stateSwitch.hidden = NO;
    SigNodeModel *node = self.source[indexPath.row];
    cell.iconImageView.image = [UIImage imageNamed:@"ic_setting"];
    cell.nameLabel.text = [NSString stringWithFormat:@"Adr:0x%X", node.address];
    cell.stateSwitch.on = node.directControlStatus.directedForwardingState;
    [cell setChangeStateBlock:^(UISwitch * _Nonnull stateSwitch) {
        [ShowTipsHandle.share show:Tip_SetDirectControl];
        NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
        [operationQueue addOperationWithBlock:^{
            //这个block语句块在子线程中执行
            NSArray *array = [NSArray arrayWithArray:node.netKeys];
            __block BOOL hadFail = NO;
            for (SigNodeKeyModel *nodeKey in array) {
                dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
                dispatch_async(dispatch_get_main_queue(), ^{
                    [SDKLibCommand directControlSetWithNetKeyIndex:nodeKey.index directedForwarding:stateSwitch.isOn ? DirectedForwarding_enable : DirectedForwarding_disable directedRelay:stateSwitch.isOn ? DirectedRelay_enable : DirectedRelay_disable directedProxy:stateSwitch.isOn ? DirectedProxy_enable : DirectedProxy_disable directedProxyUseDirectedDefault:stateSwitch.isOn ? DirectedProxyUseDirectedDefault_enable : DirectedProxyUseDirectedDefault_ignore directedFriend:DirectedFriend_ignore destination:node.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigDirectControlStatus * _Nonnull responseMessage) {
                        TelinkLogDebug(@"directControlSet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                        if (source == node.address) {
                            dispatch_async(dispatch_get_main_queue(), ^{
                                if (responseMessage.status == SigConfigMessageStatus_success) {
                                    stateSwitch.on = responseMessage.directedForwardingState == DirectedForwardingState_enable;
                                    [ShowTipsHandle.share show:Tip_SetDirectControlSuccess];
                                    node.directControlStatus = responseMessage;
                                    [SigDataSource.share saveLocationData];
                                } else {
                                    hadFail = YES;
                                    stateSwitch.on = !stateSwitch.isOn;
                                    [ShowTipsHandle.share show:[NSString stringWithFormat:@"directControlSet fail, status code = %d", responseMessage.status]];
                                }
                                [ShowTipsHandle.share delayHidden:2.0];
                            });
                        }
                    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                        TelinkLogInfo(@"isResponseAll=%d,error=%@", isResponseAll, error);
                        dispatch_async(dispatch_get_main_queue(), ^{
                            if (error) {
                                hadFail = YES;
                                stateSwitch.on = !stateSwitch.isOn;
                                [ShowTipsHandle.share show:error.domain];
                                [ShowTipsHandle.share delayHidden:2.0];
                            }
                        });
                        dispatch_semaphore_signal(semaphore);
                    }];
                });
                dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 4.0));
                if (hadFail) {
                    break;
                }
            }
        }];
    }];
    return cell;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.source.count;
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
    [self updateData];
}

- (void)normalSetting{
    [super normalSetting];
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    self.source = [[NSMutableArray alloc] init];
    self.title = @"Direct Control List";
}

- (void)updateData{
    self.source = [NSMutableArray array];
    NSArray *temArray = [NSArray arrayWithArray:SigDataSource.share.curNodes];
    for (SigNodeModel *node in temArray) {
        if (node.hasDirectForwardingFunction) {
            [self.source addObject:node];
        }
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.tableView reloadData];
    });
}

@end
