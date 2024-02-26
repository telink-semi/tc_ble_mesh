/********************************************************************************************************
 * @file     NetworkVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/5/25
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

#import "NetworkVC.h"
#import "SettingTitleItemCell.h"
#import "ShareQRCodeVC.h"
#import "ApplicantsVC.h"
#import "MeshOTAVC.h"
#import "MeshInfoVC.h"

#define kShareString    @"Share"
#define kApplicantsString    @"Applicants"
#define kScenesString    @"Scenes"
#define kMeshInfoString    @"Mesh Info"
#define kMeshOTAString    @"Mesh OTA"

@interface NetworkVC ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <NSString *>*source;
@property (nonatomic, strong) NSMutableArray <NSString *>*iconSource;
@property (nonatomic, strong) NSMutableArray <NSString *>*vcIdentifiers;
@end

@implementation NetworkVC

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    SettingTitleItemCell *cell = (SettingTitleItemCell *)[tableView dequeueReusableCellWithIdentifier:NSStringFromClass(SettingTitleItemCell.class) forIndexPath:indexPath];
    cell.accessoryType = UITableViewCellAccessoryNone;
    cell.nameLabel.text = self.source[indexPath.row];
    cell.iconImageView.image = [UIImage imageNamed:self.iconSource[indexPath.row]];
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    UITableViewCell *cell = [tableView cellForRowAtIndexPath:indexPath];
    cell.selected = NO;
    NSString *titleString = self.source[indexPath.row];
    NSString *sb = @"Setting";
    if ([titleString isEqualToString:kMeshOTAString]) {
        [self pushToMeshOTAVC];
        return;
    } else if ([titleString isEqualToString:kShareString]) {
        [self pushToShareVC];
        return;
    } else if ([titleString isEqualToString:kApplicantsString]) {
        [self pushToApplicantsVC];
        return;
    } else if ([titleString isEqualToString:kMeshInfoString]) {
        [self pushToMeshInfoVC];
        return;
    } else if ([titleString isEqualToString:kScenesString]) {
        sb = @"Cloud";
    }
    UIViewController *vc = [UIStoryboard initVC:self.vcIdentifiers[indexPath.row] storyboard:sb];
    [self.navigationController pushViewController:vc animated:YES];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.source.count;
}

- (void)pushToMeshOTAVC {
    MeshOTAVC *vc = [[MeshOTAVC alloc] init];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)pushToShareVC {
    ShareQRCodeVC *vc = (ShareQRCodeVC *)[UIStoryboard initVC:NSStringFromClass(ShareQRCodeVC.class) storyboard:@"Cloud"];
    vc.networkName = AppDataSource.share.curMeshNetworkDetailModel.name;
    vc.networkId = AppDataSource.share.curNetworkId;
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)pushToApplicantsVC {
    ApplicantsVC *vc = (ApplicantsVC *)[UIStoryboard initVC:NSStringFromClass(ApplicantsVC.class) storyboard:@"Cloud"];
    vc.networkId = AppDataSource.share.curNetworkId;
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)pushToMeshInfoVC {
    MeshInfoVC *vc = (MeshInfoVC *)[UIStoryboard initVC:NSStringFromClass(MeshInfoVC.class) storyboard:@"Setting"];
    vc.network = SigDataSource.share;
    [self.navigationController pushViewController:vc animated:YES];
}

-(void)dealloc {
    TelinkLogInfo(@"%s",__func__);
}

//将tabBar.hidden移到viewDidAppear，解决下一界面的手势返回动作取消时导致界面下方出现白条的问题。
- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    self.tabBarController.tabBar.hidden = NO;
}

- (void)normalSetting{
    [super normalSetting];
    self.title = @"Network";
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(SettingTitleItemCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(SettingTitleItemCell.class)];
    self.source = [NSMutableArray array];
    self.iconSource = [NSMutableArray array];
    self.vcIdentifiers = [NSMutableArray array];
    
    [self.source addObject:kMeshInfoString];
    [self.iconSource addObject:@"setting_network"];
    [self.vcIdentifiers addObject:ViewControllerIdentifiers_MeshInfoViewControllerID];
    if (AppDataSource.share.curMeshNetworkDetailModel && AppDataSource.share.curMeshNetworkDetailModel.role == 0) {
        [self.source addObject:kShareString];
        [self.iconSource addObject:@"setting_share"];
        [self.vcIdentifiers addObject:@""];
        [self.source addObject:kApplicantsString];
        [self.iconSource addObject:@"setting_member"];
        [self.vcIdentifiers addObject:@""];
    }
    [self.source addObject:kScenesString];
    [self.iconSource addObject:@"scene"];
    [self.vcIdentifiers addObject:ViewControllerIdentifiers_SceneListViewControllerID];
    [self.source addObject:kMeshOTAString];
    [self.iconSource addObject:@"ic_meshota"];
    [self.vcIdentifiers addObject:@""];
    //init rightBarButtonItem
    UIBarButtonItem *rightItem1 = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh target:self action:@selector(clickRefreshButton)];
    self.navigationItem.rightBarButtonItem = rightItem1;
}


- (void)clickRefreshButton {
    __weak typeof(self) weakSelf = self;
    [AppDataSource.share getDetailWithNetworkId:AppDataSource.share.curNetworkId type:AppDataSource.share.curMeshNetworkDetailModel.createUser.intValue == AppDataSource.share.userInfo.userId ? 0 : 1 resultBlock:^(NSError * _Nullable error) {
        if (error) {
            [weakSelf showTips:error.localizedDescription];
        } else {
            [weakSelf.navigationController popViewControllerAnimated:YES];
        }
    }];
}

@end
