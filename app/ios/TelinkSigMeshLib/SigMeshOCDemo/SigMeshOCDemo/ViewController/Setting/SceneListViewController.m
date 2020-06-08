/********************************************************************************************************
 * @file     SceneListViewController.m 
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
//  SceneListViewController.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/9/25.
//  Copyright © 2018年 Telink. All rights reserved.
//

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
        [DemoCommand recallSceneWithAddress:kMeshAddress_allNodes sceneId:model.number responseMaxCount:(int)model.actionList.count ack:YES successCallback:^(UInt16 source, UInt16 destination, SigSceneStatus * _Nonnull responseMessage) {
            TeLogDebug(@"recall scene:%hu,status:%d",responseMessage.targetScene,responseMessage.statusCode);
        } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
            
        }];
    }];
    //set edit scene block
    [cell setClickEditBlock:^{
        SceneDetailViewController *vc = (SceneDetailViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_SceneDetailViewControllerID storybroad:@"Setting"];
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
    model.number = [[SigDataSource share] getNewSceneAddress];
    SceneDetailViewController *vc = (SceneDetailViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_SceneDetailViewControllerID storybroad:@"Setting"];
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
            NSString *msg = [NSString stringWithFormat:@"Are you sure delete scene:0x%X",(int)model.number];
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
                    msg = [NSString stringWithFormat:@"There are some nodes offline, are you sure delete scene:0x%X",(int)model.number];
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
        NSOperationQueue *oprationQueue = [[NSOperationQueue alloc] init];
        __weak typeof(self) weakSelf = self;
        [oprationQueue addOperationWithBlock:^{
            while (delArray.count > 0) {
                dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
                ActionModel *curAction = delArray.firstObject;
                [DemoCommand delSceneWithAddress:curAction.address sceneId:scene.number responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigSceneRegisterStatus * _Nonnull responseMessage) {
                    TeLogDebug(@"responseMessage.statusCode=%d",responseMessage.statusCode);
                    [delArray removeObject:curAction];
                    dispatch_semaphore_signal(semaphore);
                } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
                    
                }];
                dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
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
    [[SigDataSource share] delectSceneModelWithModel:scene];
    self.source = [[NSMutableArray alloc] initWithArray:SigDataSource.share.scenes];
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.tableView reloadData];
        [ShowTipsHandle.share show:Tip_DeleteSceneSuccess];
        [ShowTipsHandle.share delayHidden:0.5];
    });
}

@end
