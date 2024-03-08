/********************************************************************************************************
 * @file     SchedulerListViewController.m 
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

#import "SchedulerListViewController.h"
#import "CloudSchedulerCell.h"
#import "SchedulerDetailViewController.h"
#import "UIViewController+Message.h"
#import "UIButton+extension.h"
#import "NSString+extension.h"

@interface SchedulerListViewController ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic,strong) CloudNodeModel *cloudNodeModel;
@property (nonatomic, strong) NSMutableArray<CloudSchedulerModel *> *schedulerList;

@end

@implementation SchedulerListViewController

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    CloudSchedulerCell *cell = (CloudSchedulerCell *)[tableView dequeueReusableCellWithIdentifier:NSStringFromClass(CloudSchedulerCell.class) forIndexPath:indexPath];
    CloudSchedulerModel *model = self.schedulerList[indexPath.row];
    cell.model = model;
    __weak typeof(self) weakSelf = self;
    //set edit scheduler block
    [cell.editButton addAction:^(UIButton *button) {
        SchedulerDetailViewController *vc = (SchedulerDetailViewController *)[UIStoryboard initVC:NSStringFromClass(SchedulerDetailViewController.class) storyboard:@"Cloud"];
        vc.cloudScheduler = model;
        SchedulerModel *scheduler = [[SchedulerModel alloc] initWithSchedulerDataAndSceneIdData:[LibTools nsstringToHex:model.params]];
        if (scheduler == nil) {
            //That means params is nil.
            scheduler = [[SchedulerModel alloc] init];
        }
        vc.model = scheduler;
        vc.device = weakSelf.model;
        [weakSelf.navigationController pushViewController:vc animated:YES];
    }];
    return cell;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.schedulerList.count;
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
    [self clickRefreshButton];
}

- (void)normalSetting{
    [super normalSetting];
    self.title = @"Scheduler List";
    [self addNoDataUI];
    self.cloudNodeModel = [AppDataSource.share getCloudNodeModelWithNodeAddress:self.model.address];
    __weak typeof(self) weakSelf = self;
    [self addCustomCircularButtonWithImageString:@"ic_add_white" action:^(UIButton *button) {
        [weakSelf clickAdd];
    }];
    //init rightBarButtonItem
    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh target:self action:@selector(clickRefreshButton)];
    self.navigationItem.rightBarButtonItem = rightItem;
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(CloudSchedulerCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(CloudSchedulerCell.class)];
}

- (void)clickRefreshButton {
    __weak typeof(self) weakSelf = self;
    [AppDataSource.share getSchedulerListRequestWithNodeId:self.cloudNodeModel.nodeId resultBlock:^(NSError * _Nullable error) {
        if (error) {
            [weakSelf showTips:[NSString stringWithFormat:@"getSchedulerList error = %@", error.localizedDescription]];
        } else {
            [weakSelf refreshUI];
        }
    }];
}

- (void)clickAdd {
    __weak typeof(self) weakSelf = self;
    UIAlertController *alertVc = [UIAlertController alertControllerWithTitle:@"Please input scheduler name" message:@"please input content" preferredStyle: UIAlertControllerStyleAlert];
    [alertVc addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
        textField.placeholder = @"new scheduler name";
    }];
    UIAlertAction *action1 = [UIAlertAction actionWithTitle:@"CONFIRM" style:UIAlertActionStyleDestructive handler:^(UIAlertAction * _Nonnull action) {
        NSString *schedulerName = [[alertVc textFields] objectAtIndex:0].text;
        schedulerName = schedulerName.removeHeadAndTailSpacePro;
        TelinkLogInfo(@"new schedulerName is %@", schedulerName);
        if (schedulerName == nil || schedulerName.length == 0) {
            [weakSelf showTips:@"Scheduler name can not be empty!"];
            return;
        }
        [weakSelf addSchedulerActionWithName:schedulerName];
    }];
    UIAlertAction *action2 = [UIAlertAction actionWithTitle:@"CANCEL" style:UIAlertActionStyleCancel handler:nil];
    [alertVc addAction:action2];
    [alertVc addAction:action1];
    [self presentViewController:alertVc animated:YES completion:nil];
}

- (void)addSchedulerActionWithName:(NSString *)name {
    __weak typeof(self) weakSelf = self;
    [AppDataSource.share createSchedulerWithNodeId:self.cloudNodeModel.nodeId name:name info:nil resultBlock:^(NSError * _Nullable error) {
        if (error) {
            [weakSelf showTips:[NSString stringWithFormat:@"%@", error.localizedDescription]];
        } else {
            [weakSelf refreshUI];
        }
    }];
}

- (void)refreshUI {
    self.schedulerList = [NSMutableArray arrayWithArray:self.cloudNodeModel.schedulerList];
    [self.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
    self.tableView.hidden = self.schedulerList.count == 0;
}

@end
