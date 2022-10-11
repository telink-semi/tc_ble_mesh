/********************************************************************************************************
 * @file     SceneListViewController.m 
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/9/25
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

#import "SceneListViewController.h"
#import "SceneItemCell.h"
#import "SceneDetailViewController.h"
#import "DeviceStateItemView.h"
#import "UIViewController+Message.h"

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
        [DemoCommand recallSceneWithAddress:kMeshAddress_allNodes sceneId:[LibTools uint16From16String:model.number] responseMaxCount:(int)model.actionList.count ack:YES successCallback:^(UInt16 source, UInt16 destination, SigSceneStatus * _Nonnull responseMessage) {
            TeLogDebug(@"recall scene:%hu,status:%d",responseMessage.targetScene,responseMessage.statusCode);
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
        if ([view isMemberOfClass:[DeviceStateItemView class]]) {
            [view removeFromSuperview];
        }
    }
    if (model.actionList.count > 0) {
        for (int i=0; i<model.actionList.count; i++) {
            DeviceStateItemView *view = [[DeviceStateItemView alloc] init];
            view.frame = CGRectMake(20, 44+45.0*i, [UIScreen mainScreen].bounds.size.width-20, 45.0);
            [view updateContent:model.actionList[i]];
            [cell.contentView addSubview:view];
        }
    }
    return cell;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.source.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 44.0 + 45.0*self.source[indexPath.row].actionList.count;
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
    
    self.source = [[NSMutableArray alloc] initWithArray:SigDataSource.share.scenes];
    [self.tableView reloadData];
}

- (void)normalSetting{
    [super normalSetting];
    self.title = @"Scenes";
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_SceneItemCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_SceneItemCellID];
    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemAdd target:self action:@selector(clickAdd)];
    self.navigationItem.rightBarButtonItem = rightItem;
    //longpress to delete scene
    UILongPressGestureRecognizer *gesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(cellDidPress:)];
    [self.view addGestureRecognizer:gesture];
    
}

- (void)clickAdd{
    SigSceneModel *model = [[SigSceneModel alloc] init];
    model.number = [NSString stringWithFormat:@"%04X",[[SigDataSource share] getNewSceneAddress]];
    model.name = [NSString stringWithFormat:@"scene:0x%lX",(long)model.number];
    SceneDetailViewController *vc = (SceneDetailViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_SceneDetailViewControllerID storyboard:@"Setting"];
    vc.model = model;
    [self.navigationController pushViewController:vc animated:YES];
}

#pragma  mark LongPressGesture
- (void)cellDidPress:(UILongPressGestureRecognizer *)sender{
    if (sender.state == UIGestureRecognizerStateBegan) {
        NSIndexPath *indexPath = [self.tableView indexPathForRowAtPoint:[sender locationInView:self.tableView]];
        if (indexPath != nil) {
            SigSceneModel *model = [self.source[indexPath.item] copy];
            TeLogDebug(@"%@",indexPath);
            NSString *msg = [NSString stringWithFormat:@"Are you sure delete scene:0x%@",model.number];
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
                    msg = [NSString stringWithFormat:@"There are some nodes offline, are you sure delete scene:0x%@",model.number];
                }
            }
            __weak typeof(self) weakSelf = self;
            [self showAlertSureAndCancelWithTitle:@"Hits" message:msg sure:^(UIAlertAction *action) {
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
                    TeLogDebug(@"responseMessage.statusCode=%d",responseMessage.statusCode);
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
    TeLogDebug(@"delect success");
    [[SigDataSource share] deleteSceneModelWithModel:scene];
    self.source = [[NSMutableArray alloc] initWithArray:SigDataSource.share.scenes];
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.tableView reloadData];
        [ShowTipsHandle.share show:Tip_DeleteSceneSuccess];
        [ShowTipsHandle.share delayHidden:0.5];
    });
}

@end
