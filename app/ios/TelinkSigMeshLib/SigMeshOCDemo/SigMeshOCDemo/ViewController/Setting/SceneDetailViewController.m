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
#import "ActionItemCell.h"

@interface SceneDetailViewController ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIButton *allButton;
@property (nonatomic, strong) NSMutableArray <ActionModel *>*selectActions;
@property (nonatomic, strong) NSMutableArray <ActionModel *>*allActions;

@end

@implementation SceneDetailViewController

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    ActionItemCell *cell = (ActionItemCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_ActionItemCellID forIndexPath:indexPath];
    ActionModel *model = self.allActions[indexPath.row];
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:model.address];
    [cell updateContent:model];
    cell.selectButton.selected = [self.selectActions containsObject:model];
    __weak typeof(self) weakSelf = self;
    [cell setClickSelectBlock:^{
        if (model.state != DeviceStateOutOfLine && node && node.sceneAddress.count > 0) {
            if ([weakSelf.selectActions containsObject:model]) {
                [weakSelf.selectActions removeObject:model];
            } else {
                [weakSelf.selectActions addObject:model];
            }
            [weakSelf reloadView];
        }
    }];
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    //为了跟安卓同步，暂时屏蔽情景详情界面的实时修改设备状态功能
    //    ActionViewController *vc = (ActionViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_ActionViewControllerID storybroad:@"Setting"];
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
//    TeLogDebug(@"getCompositionData 0x%02x",model.address);
//    [SDKLibCommand configCompositionDataGetWithDestination:model.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigCompositionDataStatus * _Nonnull responseMessage) {
//        TeLogInfo(@"opCode=0x%x,parameters=%@",responseMessage.opCode,[LibTools convertDataToHexStr:responseMessage.parameters]);
//    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
//        TeLogDebug(@"finish.");
//    }];
    //===========test==========//
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.allActions.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 45.0;
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)normalSetting{
    [super normalSetting];
    self.title = @"Scene Setting";
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_ActionItemCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_ActionItemCellID];
    
    self.selectActions = [NSMutableArray arrayWithArray:self.model.actionList];
    self.allActions = [NSMutableArray array];
    NSArray *curNodes = [NSArray arrayWithArray:SigDataSource.share.curNodes];
    for (SigNodeModel *device in curNodes) {
        [self.allActions addObject:[[ActionModel alloc] initWithNode:device]];
    }
    
    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"ic_check"] style:UIBarButtonItemStylePlain target:self action:@selector(clickSave)];
    self.navigationItem.rightBarButtonItem = rightItem;
    [self reloadAllButton];
}

- (void)reloadView{
    [self.allActions sortUsingComparator:^NSComparisonResult(id  _Nonnull obj1, id  _Nonnull obj2) {
        return [(ActionModel *)obj1 address] > [(ActionModel *)obj2 address];
    }];
    [self.tableView reloadData];
    [self reloadAllButton];
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
    NSMutableArray *saveArray = [[NSMutableArray alloc] init];
    for(ActionModel *action in self.selectActions){
        SigNodeModel *device = [[SigDataSource share] getNodeWithAddress:action.address];
        if(device && device.state != DeviceStateOutOfLine){
            if ([self.model.actionList containsObject:action]) {
                //address of change action't node
                NSArray *actionList = [NSArray arrayWithArray:self.model.actionList];
                for (ActionModel *oldAction in actionList) {
                    if (oldAction.address == action.address) {
                        if (![action isSameActionWithAction:oldAction]) {
                            [saveArray addObject:action];
                        }
                        [curArray addObject:action];
                        break;
                    }
                }
            } else {
                //address of new add action't node
                [saveArray addObject:action];
                [curArray addObject:action];
            }
        }
    }
    
    //cancel choose, should send a delete packet.
    NSMutableArray *delArray = [[NSMutableArray alloc] init];
    for(ActionModel *action in self.model.actionList){
        SigNodeModel *device = [[SigDataSource share] getNodeWithAddress:action.address];
        if(![self.selectActions containsObject:action]){
            if(device.state != DeviceStateOutOfLine){
                [delArray addObject:action];
            }else{
                [curArray addObject:action];
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
        while (saveArray.count > 0) {
            dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
            ActionModel *curAction = saveArray.firstObject;
            [DemoCommand getSceneRegisterStatusWithAddress:curAction.address responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigSceneRegisterStatus * _Nonnull responseMessage) {
                TeLogDebug(@"getSceneRegisterStatusWithAddress ResponseModel=%@",responseMessage.parameters);
            } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
                if (error == nil) {
                    [DemoCommand saveSceneWithAddress:curAction.address sceneId:[LibTools uint16From16String:weakSelf.model.number] responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigSceneRegisterStatus * _Nonnull responseMessage) {
                        TeLogDebug(@"saveSceneWithAddress ResponseModel=%@",responseMessage.parameters);
                    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
                        if (error == nil) {
                            [saveArray removeObject:curAction];
                            dispatch_semaphore_signal(semaphore);
                        }
                    }];
                }
            }];
            dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 4.0));
        }
        TeLogDebug(@"add finish");
        while (delArray.count > 0) {
            dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
            ActionModel *curAction = delArray.firstObject;
            [DemoCommand delSceneWithAddress:curAction.address sceneId:[LibTools uint16From16String:weakSelf.model.number] responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigSceneRegisterStatus * _Nonnull responseMessage) {
                TeLogDebug(@"delSceneWithAddress ResponseModel=%@",responseMessage);
            } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
                if (error == nil) {
                    [delArray removeObject:curAction];
                    dispatch_semaphore_signal(semaphore);
                }
            }];
            dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 4.0));
        }
        TeLogDebug(@"del finish");
        TeLogDebug(@"save success");
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
    NSArray *allActions = [NSArray arrayWithArray:self.allActions];
    for (ActionModel *action in allActions) {
        SigNodeModel *node = [SigDataSource.share getNodeWithAddress:action.address];
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
    [self reloadView];
}

- (void)reloadAllButton{
    NSInteger hasOnlineCount = 0;
    NSArray *allActions = [NSArray arrayWithArray:self.allActions];
    for (ActionModel *action in allActions) {
        if (action.state != DeviceStateOutOfLine) {
            hasOnlineCount ++;
        }
    }
    if (hasOnlineCount > 0) {
        self.allButton.selected = self.selectActions.count == hasOnlineCount;
    }
}

- (void)showTips:(NSString *)message{
    UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"Warn" message:message preferredStyle:UIAlertControllerStyleAlert];
    UIAlertAction *revoke = [UIAlertAction actionWithTitle:@"Done" style:UIAlertActionStyleCancel handler:nil];
    [alert addAction:revoke];
    [self presentViewController:alert animated:YES completion:nil];
}

-(void)dealloc{
    TeLogDebug(@"%s",__func__);
}

@end
