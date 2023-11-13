/********************************************************************************************************
 * @file     SceneDetailViewController.m 
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/9/26
 *
 * @par     Copyright (c) [2021], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "SceneDetailViewController.h"
#import "ActionViewController.h"
#import "UIButton+extension.h"
#import "DeviceElementItemView.h"

@interface SceneDetailViewController ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIButton *allButton;
@property (nonatomic, strong) NSMutableArray <SigNodeModel *>*allNodes;
@property (nonatomic, strong) NSMutableArray <ActionModel *>*selectActions;
@end

@implementation SceneDetailViewController

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    static NSString *cellID = @"cell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:cellID];
    if (!cell) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:cellID];
    }
    SigNodeModel *node = self.allNodes[indexPath.row];
    NSArray *actionList = [self getActionListOfNode:node];
    if (actionList.count > 0) {
        for (UIView *view in cell.contentView.subviews) {
            if ([view isMemberOfClass:[DeviceElementItemView class]]) {
                [view removeFromSuperview];
            }
        }
        for (int i=0; i<actionList.count; i++) {
            ActionModel *a = actionList[i];
            DeviceElementItemView *view = [[NSBundle mainBundle] loadNibNamed:@"DeviceElementItemView" owner:self options:nil].firstObject;
            view.frame = CGRectMake(0, 45.0*i, [UIScreen mainScreen].bounds.size.width, 45.0);
            [view updateContent:a];
            view.selectButton.selected = [self.selectActions containsObject:a];
            __weak typeof(self) weakSelf = self;
            [view.selectButton addAction:^(UIButton *button) {
                if (a.state != DeviceStateOutOfLine && node && node.sceneAddress.count > 0) {
                    if ([weakSelf.selectActions containsObject:a]) {
                        [weakSelf.selectActions removeObject:a];
                    } else {
                        [weakSelf.selectActions addObject:a];
                    }
                    [weakSelf reloadView];
                }
            }];
            [cell.contentView addSubview:view];
        }
    }
    
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    //为了跟安卓同步，暂时屏蔽情景详情界面的实时修改设备状态功能
    //    ActionViewController *vc = (ActionViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_ActionViewControllerID storyboard:@"Setting"];
    //    vc.model = self.allActions[indexPath.row];
    //    __weak typeof(self) weakSelf = self;
    //    [vc setBackAction:^(ActionModel *action) {
    //        [weakSelf.allActions replaceObjectAtIndex:indexPath.row withObject:action];
    //        //点击了动作的save，默认把地址勾上
    //        if (![weakSelf.selectActions containsObject:action]) {
    //            [weakSelf.selectActions addObject:action];
    //        }
    //        [weakSelf reloadView];
    //    }];
    //    [self.navigationController pushViewController:vc animated:YES];
    
    //===========test==========//
//    ActionModel *model = self.allActions[indexPath.row];
//    TelinkLogDebug(@"getCompositionData 0x%02x",model.address);
//    [SDKLibCommand configCompositionDataGetWithDestination:model.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigCompositionDataStatus * _Nonnull responseMessage) {
//        TelinkLogInfo(@"opCode=0x%x,parameters=%@",responseMessage.opCode,[LibTools convertDataToHexStr:responseMessage.parameters]);
//    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
//        TelinkLogDebug(@"finish.");
//    }];
    //===========test==========//
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.allNodes.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    SigNodeModel *node = self.allNodes[indexPath.row];
    NSArray *actionList = [self getActionListOfNode:node];
    return 45.0 * actionList.count;
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)normalSetting{
    [super normalSetting];
    self.title = @"Scene Setting";
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    
    self.selectActions = [NSMutableArray arrayWithArray:self.model.actionList];
    self.allNodes = [NSMutableArray arrayWithArray:SigDataSource.share.curNodes];
    
    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"ic_check"] style:UIBarButtonItemStylePlain target:self action:@selector(clickSave)];
    self.navigationItem.rightBarButtonItem = rightItem;
    [self reloadAllButton];
}

- (void)reloadView{
    [self.allNodes sortUsingComparator:^NSComparisonResult(id  _Nonnull obj1, id  _Nonnull obj2) {
        return [(SigNodeModel *)obj1 address] > [(SigNodeModel *)obj2 address];
    }];
    [self.tableView reloadData];
    [self reloadAllButton];
}

- (NSArray <ActionModel *>*)getActionListOfNode:(SigNodeModel *)node {
    NSMutableArray *mArray = [NSMutableArray array];
    NSArray *sceneElementAddresses = [NSArray arrayWithArray:node.sceneAddress];
    if (sceneElementAddresses.count == 0) {
        ActionModel *action = [[ActionModel alloc] initWithNode:node];
        [mArray addObject:action];
    } else {
        for (NSNumber *elementAddress in sceneElementAddresses) {
            ActionModel *action = [[ActionModel alloc] initWithNode:node elementAddress:elementAddress.intValue];
            [mArray addObject:action];
        }
    }
    return mArray;
}

- (void)clickSave{
    //Attention: App needn't send packet when node hasn't scene's modelID. App needn't send packet when node hadn't keybound.
    NSMutableArray *temArray = [NSMutableArray array];
    NSArray *selectActions = [NSArray arrayWithArray:self.selectActions];
    for (ActionModel *action in selectActions) {
        SigNodeModel *device = [[SigDataSource share] getNodeWithAddress:action.address];
        if (device.isKeyBindSuccess && device.sceneAddress.count > 0) {
            [temArray addObject:action];
        }
    }
    self.selectActions = temArray;

    //Attention: Offline node needn't send packet. Offline node keep old status.
    NSMutableArray *curArray = [[NSMutableArray alloc] init];
    //Attention: node of choose action those has new add or change will send packet.
//    NSMutableArray *saveArray = [[NSMutableArray alloc] init];
//    for(ActionModel *action in self.selectActions){
//        SigNodeModel *device = [[SigDataSource share] getNodeWithAddress:action.address];
//        if(device && device.state != DeviceStateOutOfLine){
//            if ([self.model.actionList containsObject:action]) {
//                //address of change action't node
//                NSArray *actionList = [NSArray arrayWithArray:self.model.actionList];
//                for (ActionModel *oldAction in actionList) {
//                    if (oldAction.address == action.address) {
//                        if (![action isSameActionWithAction:oldAction]) {
//                            [saveArray addObject:action];
//                        }
//                        [curArray addObject:action];
//                        break;
//                    }
//                }
//            } else {
//                //address of new add action't node
//                [saveArray addObject:action];
//                [curArray addObject:action];
//            }
//        }
//    }
    //选中设备都保存到curArray，选中的在线设备保存到saveArray
    [curArray addObjectsFromArray:self.selectActions];
    NSMutableArray *saveArray = [[NSMutableArray alloc] init];
    for(ActionModel *action in self.selectActions){
        SigNodeModel *device = [[SigDataSource share] getNodeWithAddress:action.address];
        if(device && device.state != DeviceStateOutOfLine){
            [saveArray addObject:action];
        }
    }
    
    //cancel choose, should send a delete packet.
    NSMutableArray *delArray = [[NSMutableArray alloc] init];
    for(ActionModel *action in self.model.actionList){
        SigNodeModel *device = [[SigDataSource share] getNodeWithAddress:action.address];
        if(![self.selectActions containsObject:action]){
            if(device.state != DeviceStateOutOfLine){
                [delArray addObject:action];
            }
        }
    }
    
    if ((saveArray.count == 0 && delArray.count == 0)||self.selectActions.count == 0) {
        [self showTips:Tip_SelectScene];
        return;
    }
    
    [ShowTipsHandle.share show:Tip_SaveScene];

    __weak typeof(self) weakSelf = self;
    NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
    [operationQueue addOperationWithBlock:^{
        __block BOOL hasFail = NO;
        while (saveArray.count > 0) {
            dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
            ActionModel *curAction = saveArray.firstObject;
//            [DemoCommand getSceneRegisterStatusWithAddress:curAction.address responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigSceneRegisterStatus * _Nonnull responseMessage) {
//                TelinkLogDebug(@"getSceneRegisterStatusWithAddress ResponseModel=%@",responseMessage.parameters);
//            } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
//                if (error == nil) {
//
//                }
//            }];
            __block SigSceneRegisterStatus *response = nil;
            [DemoCommand saveSceneWithAddress:curAction.address sceneId:[LibTools uint16From16String:weakSelf.model.number] responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigSceneRegisterStatus * _Nonnull responseMessage) {
                TelinkLogDebug(@"saveSceneWithAddress ResponseModel=%@",responseMessage.parameters);
                response = responseMessage;
            } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
                if (error == nil && response && response.statusCode == SigSceneResponseStatus_success) {
                } else {
                    hasFail = YES;
                    if (error) {
                        [weakSelf showTips:[NSString stringWithFormat:@"%@", error.localizedDescription]];
                    } else {
                        if (response && response.statusCode == SigSceneResponseStatus_sceneRegisterFull) {
                            [weakSelf showTips:@"Scene Register Full"];
                        } else if (response && response.statusCode == SigSceneResponseStatus_sceneNotFound) {
                            [weakSelf showTips:@"Scene Not Found"];
                        } else {
                            [weakSelf showTips:[NSString stringWithFormat:@"SigSceneRegisterStatus error code=0x%X", response.statusCode]];
                        }
                    }
                }
                [saveArray removeObject:curAction];
                dispatch_semaphore_signal(semaphore);
            }];
            dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 4.0));
            if (hasFail) {
                break;
            }
        }
        if (hasFail) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [ShowTipsHandle.share show:Tip_SaveSceneFail];
                [ShowTipsHandle.share delayHidden:0.5];
            });
            return;
        }
        TelinkLogDebug(@"add finish");
        while (delArray.count > 0) {
            dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
            ActionModel *curAction = delArray.firstObject;
            __block SigSceneRegisterStatus *response = nil;
            [DemoCommand delSceneWithAddress:curAction.address sceneId:[LibTools uint16From16String:weakSelf.model.number] responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigSceneRegisterStatus * _Nonnull responseMessage) {
                TelinkLogDebug(@"delSceneWithAddress ResponseModel=%@",responseMessage);
                response = responseMessage;
            } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
                if (error == nil && response && response.statusCode == SigSceneResponseStatus_success) {
                } else {
                    hasFail = YES;
                    if (error) {
                        [weakSelf showTips:[NSString stringWithFormat:@"%@", error.localizedDescription]];
                    } else {
                        if (response && response.statusCode == SigSceneResponseStatus_sceneRegisterFull) {
                            [weakSelf showTips:@"Scene Register Full"];
                        } else if (response && response.statusCode == SigSceneResponseStatus_sceneNotFound) {
                            [weakSelf showTips:@"Scene Not Found"];
                        } else {
                            [weakSelf showTips:[NSString stringWithFormat:@"SigSceneRegisterStatus error code=0x%X", response.statusCode]];
                        }
                    }
                }
                [delArray removeObject:curAction];
                dispatch_semaphore_signal(semaphore);
            }];
            dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 4.0));
            if (hasFail) {
                break;
            }
        }
        if (hasFail) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [ShowTipsHandle.share show:Tip_SaveSceneFail];
                [ShowTipsHandle.share delayHidden:0.5];
            });
            return;
        }
        TelinkLogDebug(@"del finish");
        TelinkLogDebug(@"save success");
        weakSelf.model.actionList = curArray;
        [[SigDataSource share] saveSceneModelWithModel:weakSelf.model];
        dispatch_async(dispatch_get_main_queue(), ^{
            [ShowTipsHandle.share show:Tip_SaveSceneSuccess];
            [ShowTipsHandle.share delayHidden:0.5];
            [weakSelf performSelector:@selector(pop) withObject:nil afterDelay:0.5];
        });
    }];
}

- (void)pop{
    [self.navigationController popViewControllerAnimated:YES];
}

- (IBAction)clickAllButton:(UIButton *)sender {
    sender.selected = !sender.isSelected;
    NSArray *nodes = [NSArray arrayWithArray:self.allNodes];
    for (SigNodeModel *node in nodes) {
        NSArray *nodeActions = [self getActionListOfNode:node];
        for (ActionModel *action in nodeActions) {
            if (action.state != DeviceStateOutOfLine && node && node.sceneAddress.count > 0) {
                if (sender.isSelected) {
                    if (![self.selectActions containsObject:action]) {
                        [self.selectActions addObject:action];
                    }
                } else {
                    [self.selectActions removeObject:action];
                }
            }
        }
    }
    [self reloadView];
}

- (void)reloadAllButton{
    NSInteger hasOnlineCount = 0;
    NSArray *nodes = [NSArray arrayWithArray:self.allNodes];
    for (SigNodeModel *node in nodes) {
        NSArray *nodeActions = [self getActionListOfNode:node];
        for (ActionModel *action in nodeActions) {
            if (action.state != DeviceStateOutOfLine && node && node.sceneAddress.count > 0) {
                hasOnlineCount ++;
            }
        }
    }    
    if (hasOnlineCount > 0) {
        self.allButton.selected = self.selectActions.count == hasOnlineCount;
    }
}

-(void)dealloc{
    TelinkLogDebug(@"%s",__func__);
}

@end
