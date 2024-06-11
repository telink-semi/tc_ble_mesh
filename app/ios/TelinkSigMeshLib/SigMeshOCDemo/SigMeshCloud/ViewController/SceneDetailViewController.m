/********************************************************************************************************
 * @file     SceneDetailViewController.m 
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/9/26
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

#import "SceneDetailViewController.h"
#import "ActionViewController.h"
#import "CloudActionCell.h"
#import "UIButton+extension.h"

@interface SceneDetailViewController ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
//@property (weak, nonatomic) IBOutlet UIButton *allButton;
//@property (nonatomic, strong) NSMutableArray <ActionModel *>*selectActions;
//@property (nonatomic, strong) NSMutableArray <ActionModel *>*allActions;

@property (nonatomic, strong) NSMutableArray <CloudNodeModel *>*allNodeList;
@property (nonatomic, strong) NSMutableArray <CloudNodeModel *>*selectNodeList;

@end

@implementation SceneDetailViewController

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    CloudActionCell *cell = (CloudActionCell *)[tableView dequeueReusableCellWithIdentifier:NSStringFromClass(CloudActionCell.class) forIndexPath:indexPath];
    CloudNodeModel *model = _allNodeList[indexPath.row];
    cell.model = model;
    cell.actionLabel.text = [_selectNodeList containsObject:model] ? @"REMOVE" : @"ADD";
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    CloudNodeModel *model = _allNodeList[indexPath.row];
    __weak typeof(self) weakSelf = self;
    if ([_selectNodeList containsObject:model]) {
        // remove
        [DemoCommand delSceneWithAddress:model.address sceneId:self.cloudScene.sceneId responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigSceneRegisterStatus * _Nonnull responseMessage) {
            TelinkLogDebug(@"delSceneWithAddress ResponseModel=%@",responseMessage);
            if (responseMessage.statusCode == SigSceneResponseStatus_success) {
                //success
                [AppDataSource.share removeNodeFromSceneWithNodeId:model.nodeId sceneId:weakSelf.cloudScene.cloudSceneId resultBlock:^(NSError * _Nullable error) {
                    if (error) {
                        [weakSelf showTips:[NSString stringWithFormat:@"%@", error.localizedDescription]];
                    } else {
                        [weakSelf.selectNodeList removeObject:model];
                        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                    }
                }];
            } else if (responseMessage.statusCode == SigSceneResponseStatus_sceneRegisterFull) {
                [weakSelf showTips:@"Scene Register Full"];
            } else if (responseMessage.statusCode == SigSceneResponseStatus_sceneNotFound) {
                [weakSelf showTips:@"Scene Not Found"];
            }
        } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {

        }];
    } else {
        // add
        [DemoCommand saveSceneWithAddress:model.address sceneId:self.cloudScene.sceneId responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigSceneRegisterStatus * _Nonnull responseMessage) {
            TelinkLogDebug(@"saveSceneWithAddress ResponseModel=%@",responseMessage.parameters);
            if (responseMessage.statusCode == SigSceneResponseStatus_success) {
                //success
                [AppDataSource.share addNodeToSceneWithNodeId:model.nodeId sceneId:weakSelf.cloudScene.cloudSceneId resultBlock:^(NSError * _Nullable error) {
                    if (error) {
                        [weakSelf showTips:[NSString stringWithFormat:@"%@", error.localizedDescription]];
                    } else {
                        [weakSelf.selectNodeList addObject:model];
                        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                    }
                }];
            } else if (responseMessage.statusCode == SigSceneResponseStatus_sceneRegisterFull) {
                [weakSelf showTips:@"Scene Register Full"];
            } else if (responseMessage.statusCode == SigSceneResponseStatus_sceneNotFound) {
                [weakSelf showTips:@"Scene Not Found"];
            }
        } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {

        }];
    }
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.allNodeList.count;
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)normalSetting{
    [super normalSetting];
    self.title = @"Scene Setting";
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(CloudActionCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(CloudActionCell.class)];
    self.allNodeList = [NSMutableArray arrayWithArray:AppDataSource.share.curMeshNetworkDetailModel.nodeList];
    self.selectNodeList = [NSMutableArray array];
    //init rightBarButtonItems
    UIBarButtonItem *rightItem1 = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh target:self action:@selector(clickRefresh)];
    UIBarButtonItem *rightItem2 = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemTrash target:self action:@selector(clickTrash)];
    self.navigationItem.rightBarButtonItems = @[rightItem1, rightItem2];
    [self clickRefresh];
}

- (void)clickRefresh {
    __weak typeof(self) weakSelf = self;
    [TelinkHttpTool getInnerNodesRequestWithSceneId:self.cloudScene.cloudSceneId didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"getInnerNodes error = %@", err);
            [weakSelf showTips:[NSString stringWithFormat:@"getInnerNodes error = %@", err.localizedDescription]];
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"getInnerNodes successful! dic=%@", dic);
                NSArray *array = dic[@"data"];
                NSMutableArray *mArray = [NSMutableArray array];
                if (array.count > 0) {
                    for (NSDictionary *dict in array) {
                        CloudNodeModel *model = [[CloudNodeModel alloc] init];
                        [model setDictionaryToCloudNodeModel:dict];
                        [mArray addObject:model];
                    }
                }
                weakSelf.selectNodeList = mArray;
                [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
            } else {
                TelinkLogInfo(@"getInnerNodes result = %@", dic);
                [weakSelf showTips:[NSString stringWithFormat:@"getInnerNodes errorCode = %d, message = %@", code, dic[@"message"]]];
            }
        }
    }];
}

- (void)clickTrash {
    __weak typeof(self) weakSelf = self;
    NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
    [operationQueue addOperationWithBlock:^{
        //这个block语句块在子线程中执行
        NSArray *array = [NSArray arrayWithArray:weakSelf.selectNodeList];
        for (CloudNodeModel *node in array) {
            dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
            [DemoCommand delSceneWithAddress:node.address sceneId:weakSelf.cloudScene.sceneId responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigSceneRegisterStatus * _Nonnull responseMessage) {
                TelinkLogDebug(@"delSceneWithAddress ResponseModel=%@",responseMessage);
            } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
                dispatch_semaphore_signal(semaphore);
            }];
            dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 4.0));
        }
        [AppDataSource.share deleteSceneWithSceneId:self.cloudScene.cloudSceneId resultBlock:^(NSError * _Nullable error) {
            if (error) {
                [weakSelf showTips:[NSString stringWithFormat:@"deleteScene error = %@", error.localizedDescription]];
            } else {
                [weakSelf performSelectorOnMainThread:@selector(pop) withObject:nil waitUntilDone:YES];
            }
        }];
    }];
}


- (void)pop{
    [self.navigationController popViewControllerAnimated:YES];
}

//- (void)clickSave{
//    //Attention: App needn't send packet when node hasn't scene's modelID. App needn't send packet when node hadn't keybound.
//    NSMutableArray *temArray = [NSMutableArray array];
//    NSArray *selectActions = [NSArray arrayWithArray:self.selectActions];
//    for (ActionModel *action in selectActions) {
//        SigNodeModel *device = [[SigDataSource share] getNodeWithAddress:action.address];
//        if (device.isKeyBindSuccess && device.sceneAddress.count > 0) {
//            [temArray addObject:action];
//        }
//    }
//    self.selectActions = temArray;
//
//    //Attention: Offline node needn't send packet. Offline node keep old status.
//    NSMutableArray *curArray = [[NSMutableArray alloc] init];
//    //Attention: node of choose action those has new add or change will send packet.
////    NSMutableArray *saveArray = [[NSMutableArray alloc] init];
////    for(ActionModel *action in self.selectActions){
////        SigNodeModel *device = [[SigDataSource share] getNodeWithAddress:action.address];
////        if(device && device.state != DeviceStateOutOfLine){
////            if ([self.model.actionList containsObject:action]) {
////                //address of change action't node
////                NSArray *actionList = [NSArray arrayWithArray:self.model.actionList];
////                for (ActionModel *oldAction in actionList) {
////                    if (oldAction.address == action.address) {
////                        if (![action isSameActionWithAction:oldAction]) {
////                            [saveArray addObject:action];
////                        }
////                        [curArray addObject:action];
////                        break;
////                    }
////                }
////            } else {
////                //address of new add action't node
////                [saveArray addObject:action];
////                [curArray addObject:action];
////            }
////        }
////    }
//    //选中设备都保存到curArray，选中的在线设备保存到saveArray
//    [curArray addObjectsFromArray:self.selectActions];
//    NSMutableArray *saveArray = [[NSMutableArray alloc] init];
//    for(ActionModel *action in self.selectActions){
//        SigNodeModel *device = [[SigDataSource share] getNodeWithAddress:action.address];
//        if(device && device.state != DeviceStateOutOfLine){
//            [saveArray addObject:action];
//        }
//    }
//    
//    //cancel choose, should send a delete packet.
//    NSMutableArray *delArray = [[NSMutableArray alloc] init];
//    for(ActionModel *action in self.model.actionList){
//        SigNodeModel *device = [[SigDataSource share] getNodeWithAddress:action.address];
//        if(![self.selectActions containsObject:action]){
//            if(device.state != DeviceStateOutOfLine){
//                [delArray addObject:action];
//            }
//        }
//    }
//    
//    if ((saveArray.count == 0 && delArray.count == 0)||self.selectActions.count == 0) {
//        [self showTips:Tip_SelectScene];
//        return;
//    }
//    
//    [ShowTipsHandle.share show:Tip_SaveScene];
//
//    __weak typeof(self) weakSelf = self;
//    NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
//    [operationQueue addOperationWithBlock:^{
//        __block BOOL hasFail = NO;
//        while (saveArray.count > 0) {
//            dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
//            ActionModel *curAction = saveArray.firstObject;
////            [DemoCommand getSceneRegisterStatusWithAddress:curAction.address responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigSceneRegisterStatus * _Nonnull responseMessage) {
////                TelinkLogDebug(@"getSceneRegisterStatusWithAddress ResponseModel=%@",responseMessage.parameters);
////            } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
////                if (error == nil) {
////
////                }
////            }];
//            __block SigSceneRegisterStatus *response = nil;
//            [DemoCommand saveSceneWithAddress:curAction.address sceneId:[LibTools uint16From16String:weakSelf.model.number] responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigSceneRegisterStatus * _Nonnull responseMessage) {
//                TelinkLogDebug(@"saveSceneWithAddress ResponseModel=%@",responseMessage.parameters);
//                response = responseMessage;
//            } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
//                if (error == nil && response && response.statusCode == SigSceneResponseStatus_success) {
//                } else {
//                    hasFail = YES;
//                    if (error) {
//                        [weakSelf showTips:[NSString stringWithFormat:@"%@", error.localizedDescription]];
//                    } else {
//                        if (response && response.statusCode == SigSceneResponseStatus_sceneRegisterFull) {
//                            [weakSelf showTips:@"Scene Register Full"];
//                        } else if (response && response.statusCode == SigSceneResponseStatus_sceneNotFound) {
//                            [weakSelf showTips:@"Scene Not Found"];
//                        } else {
//                            [weakSelf showTips:[NSString stringWithFormat:@"SigSceneRegisterStatus error code=0x%X", response.statusCode]];
//                        }
//                    }
//                }
//                [saveArray removeObject:curAction];
//                dispatch_semaphore_signal(semaphore);
//            }];
//            dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 4.0));
//            if (hasFail) {
//                break;
//            }
//        }
//        if (hasFail) {
//            dispatch_async(dispatch_get_main_queue(), ^{
//                [ShowTipsHandle.share show:Tip_SaveSceneFail];
//                [ShowTipsHandle.share delayHidden:0.5];
//            });
//            return;
//        }
//        TelinkLogDebug(@"add finish");
//        while (delArray.count > 0) {
//            dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
//            ActionModel *curAction = delArray.firstObject;
//            __block SigSceneRegisterStatus *response = nil;
//            [DemoCommand delSceneWithAddress:curAction.address sceneId:[LibTools uint16From16String:weakSelf.model.number] responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigSceneRegisterStatus * _Nonnull responseMessage) {
//                TelinkLogDebug(@"delSceneWithAddress ResponseModel=%@",responseMessage);
//                response = responseMessage;
//            } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
//                if (error == nil && response && response.statusCode == SigSceneResponseStatus_success) {
//                } else {
//                    hasFail = YES;
//                    if (error) {
//                        [weakSelf showTips:[NSString stringWithFormat:@"%@", error.localizedDescription]];
//                    } else {
//                        if (response && response.statusCode == SigSceneResponseStatus_sceneRegisterFull) {
//                            [weakSelf showTips:@"Scene Register Full"];
//                        } else if (response && response.statusCode == SigSceneResponseStatus_sceneNotFound) {
//                            [weakSelf showTips:@"Scene Not Found"];
//                        } else {
//                            [weakSelf showTips:[NSString stringWithFormat:@"SigSceneRegisterStatus error code=0x%X", response.statusCode]];
//                        }
//                    }
//                }
//                [delArray removeObject:curAction];
//                dispatch_semaphore_signal(semaphore);
//            }];
//            dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 4.0));
//            if (hasFail) {
//                break;
//            }
//        }
//        if (hasFail) {
//            dispatch_async(dispatch_get_main_queue(), ^{
//                [ShowTipsHandle.share show:Tip_SaveSceneFail];
//                [ShowTipsHandle.share delayHidden:0.5];
//            });
//            return;
//        }
//        TelinkLogDebug(@"del finish");
//        TelinkLogDebug(@"save success");
//        weakSelf.model.actionList = curArray;
//        [[SigDataSource share] saveSceneModelWithModel:weakSelf.model];
//        dispatch_async(dispatch_get_main_queue(), ^{
//            [ShowTipsHandle.share show:Tip_SaveSceneSuccess];
//            [ShowTipsHandle.share delayHidden:0.5];
//            [weakSelf performSelector:@selector(pop) withObject:nil afterDelay:0.5];
//        });
//    }];
//}
//
//- (IBAction)clickAllButton:(UIButton *)sender {
//    sender.selected = !sender.isSelected;
//    NSArray *allActions = [NSArray arrayWithArray:self.allActions];
//    for (ActionModel *action in allActions) {
//        SigNodeModel *node = [SigDataSource.share getNodeWithAddress:action.address];
//        if (action.state != DeviceStateOutOfLine && node && node.sceneAddress.count > 0) {
//            if (sender.isSelected) {
//                if (![self.selectActions containsObject:action]) {
//                    [self.selectActions addObject:action];
//                }
//            } else {
//                [self.selectActions removeObject:action];
//            }
//        }
//    }
//    [self reloadView];
//}
//
//- (void)reloadAllButton{
//    NSInteger hasOnlineCount = 0;
//    NSArray *allActions = [NSArray arrayWithArray:self.allActions];
//    for (ActionModel *action in allActions) {
//        if (action.state != DeviceStateOutOfLine) {
//            hasOnlineCount ++;
//        }
//    }
//    if (hasOnlineCount > 0) {
//        self.allButton.selected = self.selectActions.count == hasOnlineCount;
//    }
//}
//
//- (void)showTips:(NSString *)message{
//    UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"Warn" message:message preferredStyle:UIAlertControllerStyleAlert];
//    UIAlertAction *revoke = [UIAlertAction actionWithTitle:kDefaultAlertOK style:UIAlertActionStyleCancel handler:nil];
//    [alert addAction:revoke];
//    [self presentViewController:alert animated:YES completion:nil];
//}

-(void)dealloc{
    TelinkLogDebug(@"%s",__func__);
}

@end
