/********************************************************************************************************
 * @file     SceneListViewController.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/9/25
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

#import "SceneListViewController.h"
#import "SceneItemCell.h"
#import "SceneDetailViewController.h"
#import "UIViewController+Message.h"
#import "SceneElementView.h"
#import "NSString+extension.h"
#import "UIButton+extension.h"

@interface SceneListViewController ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <SigSceneModel *>*source;

@end

@implementation SceneListViewController

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    SceneItemCell *cell = (SceneItemCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_SceneItemCellID forIndexPath:indexPath];
    SigSceneModel *model = self.source[indexPath.row];
    [cell updateContent:model];
    __weak typeof(self) weakSelf = self;
    //set recall scene block
    [cell setClickRecallBlock:^{
        [DemoCommand recallSceneWithAddress:kMeshAddress_allNodes sceneId:[LibTools uint16From16String:model.number] responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigSceneStatus * _Nonnull responseMessage) {
            TelinkLogDebug(@"recall scene:%hu,status:%d",responseMessage.targetScene,responseMessage.statusCode);
        } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {

        }];
    }];
    //set edit scene block
    [cell setClickEditBlock:^{
        SceneDetailViewController *vc = (SceneDetailViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_SceneDetailViewControllerID storyboard:@"Setting"];
        vc.model = model;
        [weakSelf.navigationController pushViewController:vc animated:YES];
    }];

    for (UIView *view in cell.contentView.subviews) {
        if ([view isMemberOfClass:[SceneElementView class]]) {
            [view removeFromSuperview];
        }
    }
    if (model.actionList.count > 0) {
        for (int i=0; i<model.actionList.count; i++) {
            ActionModel *action = model.actionList[i];
            SigNodeModel *node = [SigDataSource.share getNodeWithAddress:action.address];
            SceneElementView *view = [[NSBundle mainBundle] loadNibNamed:@"SceneElementView" owner:self options:nil].firstObject;
            view.frame = CGRectMake(40, 89-20+44.0*i, [UIScreen mainScreen].bounds.size.width-80, 44.0);
            view.nameLabel.text = [NSString stringWithFormat:@"name: %@\nelement address: 0x%04X", node.name, action.address];
            [cell.contentView addSubview:view];
            [view.playButton addAction:^(UIButton *button) {
                [DemoCommand recallSceneWithAddress:action.address sceneId:[LibTools uint16From16String:model.number] responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigSceneStatus * _Nonnull responseMessage) {
                    TelinkLogDebug(@"recall scene:%hu,status:%d",responseMessage.targetScene,responseMessage.statusCode);
                } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {

                }];
            }];
        }
    }
    cell.selectionStyle = UITableViewCellSelectionStyleNone;
    return cell;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.source.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 89 + 44.0*self.source[indexPath.row].actionList.count;
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
    [self refreshTableViewUI];
}

- (void)refreshTableViewUI {
    self.source = [[NSMutableArray alloc] initWithArray:SigDataSource.share.scenes];
    [self.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
}

- (void)normalSetting{
    [super normalSetting];
    self.title = @"Scene List";
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_SceneItemCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_SceneItemCellID];
    //init rightBarButtonItem
    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemAdd target:self action:@selector(clickAdd)];
    self.navigationItem.rightBarButtonItem = rightItem;
    //longpress to delete scene
    UILongPressGestureRecognizer *gesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(cellDidPress:)];
    [self.view addGestureRecognizer:gesture];

}

- (void)clickAdd {
    __weak typeof(self) weakSelf = self;
    UIAlertController *alertVc = [UIAlertController alertControllerWithTitle:@"Create Scene" message:@"please input content" preferredStyle: UIAlertControllerStyleAlert];
    [alertVc addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
        textField.placeholder = @"new scene name";
    }];
    UIAlertAction *action1 = [UIAlertAction actionWithTitle:@"CONFIRM" style:UIAlertActionStyleDestructive handler:^(UIAlertAction * _Nonnull action) {
        NSString *sceneName = [[alertVc textFields] objectAtIndex:0].text;
        sceneName = sceneName.removeHeadAndTailSpacePro;
        TelinkLogInfo(@"new scene is %@", sceneName);
        if (sceneName == nil || sceneName.length == 0) {
            [weakSelf showTips:@"Scene name can not be empty!"];
            return;
        }
        SigSceneModel *model = [[SigSceneModel alloc] init];
        model.number = [NSString stringWithFormat:@"%04X",[[SigDataSource share] getNewSceneAddress]];
        model.name = sceneName;
        [SigDataSource.share.scenes addObject:model];
        [SigDataSource.share saveLocationData];
        [weakSelf performSelectorOnMainThread:@selector(refreshTableViewUI) withObject:nil waitUntilDone:YES];
    }];
    UIAlertAction *action2 = [UIAlertAction actionWithTitle:@"CANCEL" style:UIAlertActionStyleCancel handler:nil];
    [alertVc addAction:action2];
    [alertVc addAction:action1];
    [self presentViewController:alertVc animated:YES completion:nil];
}

#pragma  mark LongPressGesture
- (void)cellDidPress:(UILongPressGestureRecognizer *)sender{
    if (sender.state == UIGestureRecognizerStateBegan) {
        NSIndexPath *indexPath = [self.tableView indexPathForRowAtPoint:[sender locationInView:self.tableView]];
        if (indexPath != nil) {
            SigSceneModel *model = [self.source[indexPath.item] copy];
            TelinkLogDebug(@"%@",indexPath);
            NSString *msg = [NSString stringWithFormat:@"Are you sure delete scene ID:0x%@?",model.number];
            if (model.actionList) {
                BOOL hasOutLine = NO;
                NSArray *actionList = [NSArray arrayWithArray:model.actionList];
                for (ActionModel *action in actionList) {
                    SigNodeModel *device = [SigDataSource.share getNodeWithAddress:action.address];
                    if (device.state == DeviceStateOutOfLine) {
                        hasOutLine = YES;
                        break;
                    }
                }
                if (hasOutLine) {
                    msg = [NSString stringWithFormat:@"There are some nodes offline, are you sure delete scene ID:0x%@?",model.number];
                }
            }
            __weak typeof(self) weakSelf = self;
            [self showAlertSureAndCancelWithTitle:@"Warning" message:msg sure:^(UIAlertAction *action) {
                [weakSelf deleteSceneAction:model];
            } cancel:^(UIAlertAction *action) {

            }];
        }
    }
}

- (void)deleteSceneAction:(SigSceneModel *)scene{
    [ShowTipsHandle.share show:Tip_DeleteScene];

    NSMutableArray *array = [[NSMutableArray alloc] init];
    for(ActionModel *action in scene.actionList){
        SigNodeModel *device = [[SigDataSource share] getNodeWithAddress:action.address];
        if(device.state != DeviceStateOutOfLine){
            [array addObject:action];
        }
    }
    if (array.count > 0) {
        //send delete packet
        NSMutableArray *delArray = [[NSMutableArray alloc] initWithArray:array];
        NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
        __weak typeof(self) weakSelf = self;
        [operationQueue addOperationWithBlock:^{
            while (delArray.count > 0) {
                dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
                ActionModel *curAction = delArray.firstObject;
                [DemoCommand delSceneWithAddress:curAction.address sceneId:[LibTools uint16From16String:scene.number] responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigSceneRegisterStatus * _Nonnull responseMessage) {
                    TelinkLogDebug(@"responseMessage.statusCode=%d",responseMessage.statusCode);
                    [delArray removeObject:curAction];
                    dispatch_semaphore_signal(semaphore);
                } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {

                }];
                dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 4.0));
            }
            [weakSelf showDeleteSceneSuccess:scene];
        }];

    }else{
        //needn't send packet, just need to change Datasource.
        [self showDeleteSceneSuccess:scene];
    }
}

- (void)showDeleteSceneSuccess:(SigSceneModel *)scene{
    TelinkLogDebug(@"delect success");
    [[SigDataSource share] deleteSceneModelWithModel:scene];
    [self refreshTableViewUI];
    dispatch_async(dispatch_get_main_queue(), ^{
        [ShowTipsHandle.share show:Tip_DeleteSceneSuccess];
        [ShowTipsHandle.share delayHidden:0.5];
    });
}

@end
