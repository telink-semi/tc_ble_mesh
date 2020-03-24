/********************************************************************************************************
 * @file     SchedulerListViewController.m 
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
//  SchedulerListViewController.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2018/9/25.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "SchedulerListViewController.h"
#import "SchedulerCell.h"
#import "SchedulerDetailViewController.h"
#import "UIViewController+Message.h"

@interface SchedulerListViewController ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <SchedulerModel *>*source;

@end

@implementation SchedulerListViewController

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    SchedulerCell *cell = (SchedulerCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_SchedulerCellID forIndexPath:indexPath];
    SchedulerModel *model = self.source[indexPath.row];
    [cell updateContent:model];
    __weak typeof(self) weakSelf = self;
    //set edit scheduler block
    [cell setClickEditBlock:^{
        SchedulerDetailViewController *vc = (SchedulerDetailViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_SchedulerDetailViewControllerID storybroad:@"Setting"];
        vc.model = model;
        vc.device = weakSelf.model;
        [weakSelf.navigationController pushViewController:vc animated:YES];
    }];
    return cell;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.source.count;
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
    
    self.source = [[NSMutableArray alloc] initWithArray:self.model.schedulerList];
    [self.tableView reloadData];
}

- (void)normalSetting{
    [super normalSetting];
    self.title = @"Scheduler List";
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_SchedulerCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_SchedulerCellID];
    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemAdd target:self action:@selector(clickAdd)];
    self.navigationItem.rightBarButtonItem = rightItem;
    //==================test================//
    //获取当前设备的闹钟ID数据，for循环根据存在闹钟数据的闹钟ID获取闹钟的详细数据。
//    __weak typeof(self) weakSelf = self;
//    NSOperationQueue *oprationQueue = [[NSOperationQueue alloc] init];
//    [oprationQueue addOperationWithBlock:^{
//        //这个block语句块在子线程中执行
//        [DemoCommand getSchedulerStatusWithAddress:weakSelf.model.address resMax:1 Completation:^(ResponseModel *m) {
//            TeLogDebug(@"getSchedulerStatusWithAddress=%@",m.rspData);
//            UInt16 status = 0;
//            Byte *pu = (Byte *)[m.rspData bytes];
//            memcpy(&status, pu + 9, 2);
//            for (int i=0; i<16; i++) {
//                if (status&(1<<i)) {
//                    TeLogDebug(@"this node has schedulerID:%d",i);
//                    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
//                    [DemoCommand getSchedulerActionWithAddress:weakSelf.model.address resMax:1 schedulerModelID:i Completation:^(ResponseModel *m) {
//                        TeLogDebug(@"getSchedulerActionWithAddress=%@",m.rspData);
//                        dispatch_semaphore_signal(semaphore);
//                    }];
//                    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
//                }
//            }
//        }];
//    }];
    //==================test================//
}

- (void)clickAdd{
    if (self.model.schedulerList.count >= 16) {
        [self showTips:@"Node's scheduler has full, you can't add any more."];
        return;
    }
    SchedulerModel *model = [[SchedulerModel alloc] init];
    model.schedulerID = [self.model getNewSchedulerID];
//    model.valid_flag_or_idx = model.schedulerID;
    SchedulerDetailViewController *vc = (SchedulerDetailViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_SchedulerDetailViewControllerID storybroad:@"Setting"];
    vc.model = model;
    vc.device = self.model;
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)showTips:(NSString *)message{
    [self showAlertSureWithTitle:@"Hits" message:message sure:^(UIAlertAction *action) {
        
    }];
}

@end
