/********************************************************************************************************
 * @file     SceneDetailViewController.m 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
//
//  SceneDetailViewController.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/9/26.
//  Copyright © 2018年 Telink. All rights reserved.
//

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
    NSOperationQueue *oprationQueue = [[NSOperationQueue alloc] init];
    [oprationQueue addOperationWithBlock:^{
        while (saveArray.count > 0) {
            dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
            ActionModel *curAction = saveArray.firstObject;
            [DemoCommand getSceneRegisterStatusWithAddress:curAction.address responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigSceneRegisterStatus * _Nonnull responseMessage) {
                TeLogDebug(@"getSceneRegisterStatusWithAddress ResponseModel=%@",responseMessage.parameters);
            } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
                if (error == nil) {
                    [DemoCommand saveSceneWithAddress:curAction.address sceneId:weakSelf.model.number responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigSceneRegisterStatus * _Nonnull responseMessage) {
                        TeLogDebug(@"saveSceneWithAddress ResponseModel=%@",responseMessage.parameters);
                    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
                        if (error == nil) {
                            [saveArray removeObject:curAction];
                            dispatch_semaphore_signal(semaphore);
                        }
                    }];
                }
            }];
            dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
        }
        TeLogDebug(@"add finish");
        while (delArray.count > 0) {
            dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
            ActionModel *curAction = delArray.firstObject;
            [DemoCommand delSceneWithAddress:curAction.address sceneId:weakSelf.model.number responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigSceneRegisterStatus * _Nonnull responseMessage) {
                TeLogDebug(@"delSceneWithAddress ResponseModel=%@",responseMessage);
            } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
                if (error == nil) {
                    [delArray removeObject:curAction];
                    dispatch_semaphore_signal(semaphore);
                }
            }];
            dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
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
        if (action.state != DeviceStateOutOfLine) {
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
