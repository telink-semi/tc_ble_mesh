/********************************************************************************************************
 * @file     SubnetBridgeListVC.m
 *
 * @brief    Show SubnetBridgeList of node.
 *
 * @author   Telink, 梁家誌
 * @date     2021/1/25
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

#import "SubnetBridgeListVC.h"
#import "UIViewController+Message.h"
#import "AddBridgeTableVC.h"

@interface SubnetBridgeListVC ()<UITableViewDelegate,UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UISwitch *enableSwitch;
@property (weak, nonatomic) IBOutlet UIButton *addBridgingTableButton;
@property (nonatomic, strong) SigMessageHandle *messageHandle;

@end

@implementation SubnetBridgeListVC

- (IBAction)addBridgingTable:(UIButton *)sender {
    TelinkLogDebug(@"");
    if (SigBearer.share.isOpen) {
        if (self.model.state != DeviceStateOutOfLine) {
            [self pushToAddBridgingTableVC];
        } else {
            [self showTips:@"The node is offline, app can not add bridge table."];
        }
    } else {
        [self showTips:@"The mesh is offline, app can not add bridge table."];
    }
}

- (IBAction)clickEnable:(UISwitch *)sender {
    TelinkLogDebug(@"");
    if (SigMeshLib.share.isBusyNow) {
        TelinkLogInfo(@"send request for subnetBridgeSet, but busy now.");
        [self showTips:@"app is busy now, try again later."];
        sender.on = !sender.on;
    } else {
        if (SigBearer.share.isOpen) {
            if (self.model.state != DeviceStateOutOfLine) {
                TelinkLogInfo(@"send request for subnetBridgeSet, address:%d",self.model.address);
                __weak typeof(self) weakSelf = self;
                _messageHandle = [SDKLibCommand subnetBridgeSetWithDestination:self.model.address subnetBridge:sender.on ? SigSubnetBridgeStateValues_enabled : SigSubnetBridgeStateValues_disabled retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigSubnetBridgeStatus * _Nonnull responseMessage) {
                    TelinkLogDebug(@"subnetBridgeSet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                    weakSelf.model.subnetBridgeEnable = sender.on;
                    [SigDataSource.share saveLocationData];
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                }];
            } else {
                sender.on = !sender.on;
                [self showTips:@"The node is offline, app can not set subnet bridge."];
            }
        } else {
            sender.on = !sender.on;
            [self showTips:@"The mesh is offline, app can not set subnet bridge."];
        }
    }
}

- (void)pushToAddBridgingTableVC {
    AddBridgeTableVC *vc = (AddBridgeTableVC *)[UIStoryboard initVC:ViewControllerIdentifiers_AddBridgeTableVCID storyboard:@"DeviceSetting"];
    vc.model = self.model;
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)removeBridgeTable:(SigSubnetBridgeModel *)subnetBridge {
    TelinkLogDebug(@"");
    if (SigMeshLib.share.isBusyNow) {
        TelinkLogInfo(@"send request for bridgeTableRemove, but busy now.");
        [self showTips:@"app is busy now, try again later."];
    } else {
        if (SigBearer.share.isOpen) {
            if (self.model.state != DeviceStateOutOfLine) {
                TelinkLogInfo(@"send request for bridgeTableRemove, address:%d",self.model.address);
                __weak typeof(self) weakSelf = self;
                _messageHandle = [SDKLibCommand bridgeTableRemoveWithDestination:self.model.address netKeyIndex1:subnetBridge.netKeyIndex1 netKeyIndex2:subnetBridge.netKeyIndex2 address1:subnetBridge.address1 address2:subnetBridge.address2 retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigBridgeTableStatus * _Nonnull responseMessage) {
                    TelinkLogDebug(@"bridgeTableRemove=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                    if (responseMessage.status == SigConfigMessageStatus_success) {
                        [weakSelf removeBridgeTableRemoveFromNode:subnetBridge];
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);

                }];
            } else {
                [self showTips:@"The node is offline, app can not remove bridge table."];
            }
        } else {
            [self showTips:@"The mesh is offline, app can not remove bridge table."];
        }
    }
}

- (void)removeBridgeTableRemoveFromNode:(SigSubnetBridgeModel *)subnetBridge {
    NSArray *array = [NSArray arrayWithArray:self.model.subnetBridgeList];
    for (SigSubnetBridgeModel *model in array) {
        if ([model isEqual:subnetBridge]) {
            [self.model.subnetBridgeList removeObject:model];
            break;
        }
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.tableView reloadData];
    });
}

#pragma mark - Life method
- (void)normalSetting{
    [super normalSetting];
    self.title = @"Subnet Bridge";
    self.addBridgingTableButton.backgroundColor = UIColor.telinkButtonBlue;
    self.enableSwitch.on  = self.model.subnetBridgeEnable;
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    //longpress to delete subnet bridge
    UILongPressGestureRecognizer *gesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(cellDidPress:)];
    [self.view addGestureRecognizer:gesture];
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
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
    return self.model.subnetBridgeList.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    static NSString *cellID = @"testCell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:cellID];
    if (!cell) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:cellID];
    }
    SigSubnetBridgeModel *model = self.model.subnetBridgeList[indexPath.row];
    cell.textLabel.numberOfLines = 0;
    cell.textLabel.font = [UIFont systemFontOfSize:13.0];
    cell.textLabel.text = model.getDescription;
    return cell;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 100.0;
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

            SigSubnetBridgeModel *subnetBridgeModel = self.model.subnetBridgeList[indexPath.row];
            NSString *msg = [NSString stringWithFormat:@"Are you sure delete this subnet "];
            __weak typeof(self) weakSelf = self;
            [self showAlertSureAndCancelWithTitle:@"Hits" message:msg sure:^(UIAlertAction *action) {
                [weakSelf deleteSubnetBridgeOfDevice:subnetBridgeModel];
            } cancel:^(UIAlertAction *action) {

            }];
        }
    }
}

- (void)deleteSubnetBridgeOfDevice:(SigSubnetBridgeModel *)subnetBridgeModel {
    [ShowTipsHandle.share show:@"delete Subnet Bridge from node..."];
    __weak typeof(self) weakSelf = self;
    [SDKLibCommand bridgeTableRemoveWithDestination:self.model.address netKeyIndex1:subnetBridgeModel.netKeyIndex1 netKeyIndex2:subnetBridgeModel.netKeyIndex2 address1:subnetBridgeModel.address1 address2:subnetBridgeModel.address2 retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigBridgeTableStatus * _Nonnull responseMessage) {
        TelinkLogInfo(@"delete Subnet Bridge responseMessage=%@,parameters=%@,source=0x%x,destination=0x%x",responseMessage,responseMessage.parameters,source,destination);
        if (responseMessage.status == SigConfigMessageStatus_success) {
            [weakSelf.model.subnetBridgeList removeObject:subnetBridgeModel];
            [SigDataSource.share saveLocationData];
            [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
            [ShowTipsHandle.share show:@"delete Subnet Bridge from node success!"];
        } else {
            [ShowTipsHandle.share show:[NSString stringWithFormat:@"delete Subnet Bridge from node fail! error status=%d",responseMessage.status]];
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        dispatch_async(dispatch_get_main_queue(), ^{
            if (error) {
                [ShowTipsHandle.share show:[NSString stringWithFormat:@"delete Subnet Bridge from node fail! error=%@",error]];
            }
            [ShowTipsHandle.share delayHidden:2.0];
        });
    }];
}

@end
