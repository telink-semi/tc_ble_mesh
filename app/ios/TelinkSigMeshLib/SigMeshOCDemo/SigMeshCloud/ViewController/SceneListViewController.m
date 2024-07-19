/********************************************************************************************************
 * @file     SceneListViewController.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/5/15
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

#import "SceneListViewController.h"
#import "CloudSceneCell.h"
#import "SceneDetailViewController.h"
#import "DeviceStateItemView.h"
#import "UIViewController+Message.h"
#import "UIButton+extension.h"
#import "NSString+extension.h"

@interface SceneListViewController ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <CloudSceneModel *>*sceneList;

@end

@implementation SceneListViewController

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    CloudSceneCell *cell = (CloudSceneCell *)[tableView dequeueReusableCellWithIdentifier:NSStringFromClass(CloudSceneCell.class) forIndexPath:indexPath];
    CloudSceneModel *model = self.sceneList[indexPath.row];
    cell.model = model;
    __weak typeof(self) weakSelf = self;
    //set recall scene block
    [cell.recallButton addAction:^(UIButton *button) {
        __block BOOL result = NO;
        [DemoCommand recallSceneWithAddress:kMeshAddress_allNodes sceneId:model.sceneId responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigSceneStatus * _Nonnull responseMessage) {
            TelinkLogDebug(@"recall scene:%hu,status:%d",responseMessage.targetScene,responseMessage.statusCode);
            if (responseMessage.statusCode == SigSceneResponseStatus_success) {
                result = YES;
            }
        } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
            if (result) {
                [weakSelf showTips:@"recall scene success"];
            }
        }];
    }];
    //set edit scene block
    [cell.editButton addAction:^(UIButton *button) {
        SceneDetailViewController *vc = (SceneDetailViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_SceneDetailViewControllerID storyboard:@"Cloud"];
        vc.cloudScene = model;
        [weakSelf.navigationController pushViewController:vc animated:YES];
    }];
    return cell;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.sceneList.count;
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
    [self clickRefresh];
}

- (void)normalSetting{
    [super normalSetting];
    self.title = @"Scene List";
    [self addNoDataUI];
    __weak typeof(self) weakSelf = self;
    [self addCustomCircularButtonWithImageString:@"ic_add_white" action:^(UIButton *button) {
        [weakSelf clickAdd];
    }];
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(CloudSceneCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(CloudSceneCell.class)];
    //init rightBarButtonItem
    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh target:self action:@selector(clickRefresh)];
    self.navigationItem.rightBarButtonItem = rightItem;
}

- (void)clickRefresh {
    __weak typeof(self) weakSelf = self;
    [AppDataSource.share getSceneListWithNetworkId:AppDataSource.share.curNetworkId resultBlock:^(NSError * _Nullable error) {
        if (error) {
            [weakSelf showTips:[NSString stringWithFormat:@"getSceneList error = %@", error.localizedDescription]];
        } else {
            [weakSelf refreshUI];
        }
    }];
}

- (void)clickAdd {
    __weak typeof(self) weakSelf = self;
    UIAlertController *alertVc = [UIAlertController alertControllerWithTitle:@"Please input scene name" message:@"please input content" preferredStyle: UIAlertControllerStyleAlert];
    [alertVc addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
        textField.placeholder = @"new scene name";
    }];
    UIAlertAction *action1 = [UIAlertAction actionWithTitle:kDefaultAlertOK style:UIAlertActionStyleDestructive handler:^(UIAlertAction * _Nonnull action) {
        NSString *sceneName = [[alertVc textFields] objectAtIndex:0].text;
        sceneName = sceneName.removeHeadAndTailSpacePro;
        TelinkLogInfo(@"new scene is %@", sceneName);
        if (sceneName == nil || sceneName.length == 0) {
            [weakSelf showTips:@"Scene name can not be empty!"];
            return;
        }
        [weakSelf addSceneActionWithName:sceneName];
    }];
    UIAlertAction *action2 = [UIAlertAction actionWithTitle:kDefaultAlertOK style:UIAlertActionStyleCancel handler:nil];
    [alertVc addAction:action2];
    [alertVc addAction:action1];
    [self presentViewController:alertVc animated:YES completion:nil];
}

- (void)addSceneActionWithName:(NSString *)name {
    __weak typeof(self) weakSelf = self;
    [AppDataSource.share createSceneWithNetworkId:AppDataSource.share.curNetworkId name:name info:nil resultBlock:^(NSError * _Nullable error) {
        if (error) {
            [weakSelf showTips:[NSString stringWithFormat:@"%@", error.localizedDescription]];
        } else {
            [weakSelf clickRefresh];
        }

    }];
}


#pragma  mark LongPressGesture
//- (void)cellDidPress:(UILongPressGestureRecognizer *)sender{
//    if (sender.state == UIGestureRecognizerStateBegan) {
//        NSIndexPath *indexPath = [self.tableView indexPathForRowAtPoint:[sender locationInView:self.tableView]];
//        if (indexPath != nil) {
//            SigSceneModel *model = [self.source[indexPath.item] copy];
//            TelinkLogDebug(@"%@",indexPath);
//            NSString *msg = [NSString stringWithFormat:@"Are you sure delete scene:0x%@",model.number];
//            if (model.actionList) {
//                BOOL hasOutLine = NO;
//                NSArray *actionList = [NSArray arrayWithArray:model.actionList];
//                for (ActionModel *action in actionList) {
//                    SigNodeModel *device = [SigDataSource.share getNodeWithAddress:action.address];
//                    if (device.state == DeviceStateOutOfLine) {
//                        hasOutLine = YES;
//                        break;
//                    }
//                }
//                if (hasOutLine) {
//                    msg = [NSString stringWithFormat:@"There are some nodes offline, are you sure delete scene:0x%@",model.number];
//                }
//            }
//            __weak typeof(self) weakSelf = self;
//            [self showAlertSureAndCancelWithTitle:kDefaultAlertTitle message:msg sure:^(UIAlertAction *action) {
//                [weakSelf deleteSceneAction:model];
//            } cancel:^(UIAlertAction *action) {
//                
//            }];
//        }
//    }
//}

//- (void)deleteSceneAction:(SigSceneModel *)scene{
//    [ShowTipsHandle.share show:Tip_DeleteScene];
//
//    NSMutableArray *array = [[NSMutableArray alloc] init];
//    for(ActionModel *action in scene.actionList){
//        SigNodeModel *device = [[SigDataSource share] getNodeWithAddress:action.address];
//        if(device.state != DeviceStateOutOfLine){
//            [array addObject:action];
//        }
//    }
//    if (array.count > 0) {
//        //send delete packet
//        NSMutableArray *delArray = [[NSMutableArray alloc] initWithArray:array];
//        NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
//        __weak typeof(self) weakSelf = self;
//        [operationQueue addOperationWithBlock:^{
//            while (delArray.count > 0) {
//                dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
//                ActionModel *curAction = delArray.firstObject;
//                [DemoCommand delSceneWithAddress:curAction.address sceneId:[LibTools uint16From16String:scene.number] responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigSceneRegisterStatus * _Nonnull responseMessage) {
//                    TelinkLogDebug(@"responseMessage.statusCode=%d",responseMessage.statusCode);
//                    [delArray removeObject:curAction];
//                    dispatch_semaphore_signal(semaphore);
//                } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
//                    
//                }];
//                dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 4.0));
//            }
//            [weakSelf showDeleteSceneSuccess:scene];
//        }];
//        
//    }else{
//        //needn't send packet, just need to change Datasource.
//        [self showDeleteSceneSuccess:scene];
//    }
//}

//- (void)showDeleteSceneSuccess:(SigSceneModel *)scene{
//    TelinkLogDebug(@"delect success");
//    [[SigDataSource share] deleteSceneModelWithModel:scene];
//    self.source = [[NSMutableArray alloc] initWithArray:SigDataSource.share.scenes];
//    dispatch_async(dispatch_get_main_queue(), ^{
//        [self.tableView reloadData];
//        [ShowTipsHandle.share show:Tip_DeleteSceneSuccess];
//        [ShowTipsHandle.share delayHidden:0.5];
//    });
//}

- (void)refreshUI {
    self.sceneList = [NSMutableArray arrayWithArray:AppDataSource.share.curMeshNetworkDetailModel.sceneList];
    [self.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
    self.tableView.hidden = self.sceneList.count == 0;
}

@end
