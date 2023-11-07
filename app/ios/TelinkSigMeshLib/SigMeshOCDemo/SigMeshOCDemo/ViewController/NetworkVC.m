/********************************************************************************************************
 * @file     NetworkVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/10/10
 *
 * @par     Copyright (c) [2023], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#import "SettingDetailItemCell.h"
#import "SettingTitleItemCell.h"
#import "MeshOTAVC.h"
#import "ResponseTestVC.h"
#import "UIViewController+Message.h"
#import "MeshInfoVC.h"
#import "ProxyFilterVC.h"
#import "PrivateBeaconVC.h"

@interface NetworkVC ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <NSString *>*source;
@property (nonatomic, strong) NSMutableArray <NSString *>*iconSource;
@property (nonatomic, strong) NSMutableArray <NSString *>*vcIdentifiers;
@end

@implementation NetworkVC

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.row == 0) {
        SettingDetailItemCell *cell = (SettingDetailItemCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_SettingDetailItemCellID forIndexPath:indexPath];
        cell.accessoryType = UITableViewCellAccessoryNone;
        cell.nameLabel.text = self.source[indexPath.row];
        cell.detailLabel.text = SigDataSource.share.meshName;
        cell.iconImageView.image = [UIImage imageNamed:self.iconSource[indexPath.row]];
        return cell;
    }
    SettingTitleItemCell *cell = (SettingTitleItemCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_SettingTitleItemCellID forIndexPath:indexPath];
    cell.accessoryType = UITableViewCellAccessoryNone;
    cell.nameLabel.text = self.source[indexPath.row];
    cell.iconImageView.image = [UIImage imageNamed:self.iconSource[indexPath.row]];
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    NSString *titleString = self.source[indexPath.row];
    NSString *sb = @"Setting";
    UIViewController *vc = nil;
    if ([titleString isEqualToString:@"Mesh OTA"]) {
        vc = [[MeshOTAVC alloc] init];
    } else if ([titleString isEqualToString:@"Proxy Filter"]) {
        vc = [[ProxyFilterVC alloc] init];
    } else if ([titleString isEqualToString:@"Private Beacon"]) {
        vc = [[PrivateBeaconVC alloc] init];
    } else {
        vc = [UIStoryboard initVC:self.vcIdentifiers[indexPath.row] storyboard:sb];
        if ([titleString isEqualToString:@"Mesh Info"]) {
            MeshInfoVC *infoVC = (MeshInfoVC *)vc;
            infoVC.network = SigDataSource.share;
            __weak typeof(self) weakSelf = self;
            [infoVC setBackNetwork:^(SigDataSource * _Nonnull nNetwork) {
                [SigDataSource.share setDictionaryToDataSource:[nNetwork getDictionaryFromDataSource]];
                [SigDataSource.share saveLocationData];
                [weakSelf addOrUpdateMeshDictionaryToMeshList:[nNetwork getDictionaryFromDataSource]];
            }];
        }
    }
    [self.navigationController pushViewController:vc animated:YES];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.source.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 70.0;
}

//将tabBar.hidden移到viewDidAppear，解决下一界面的手势返回动作取消时导致界面下方出现白条的问题。
- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    self.tabBarController.tabBar.hidden = NO;
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
//    self.tabBarController.tabBar.hidden = NO;
    [self initData];
    [self.tableView reloadData];
}

- (void)initData {
    self.source = [NSMutableArray array];
    self.iconSource = [NSMutableArray array];
    self.vcIdentifiers = [NSMutableArray array];
    
    [self.source addObject:@"Mesh Info"];
    [self.iconSource addObject:@"ic_mesh_network"];
    [self.vcIdentifiers addObject:ViewControllerIdentifiers_MeshInfoViewControllerID];
    [self.source addObject:@"Scenes"];
    [self.iconSource addObject:@"scene"];
    [self.vcIdentifiers addObject:ViewControllerIdentifiers_SceneListViewControllerID];
    [self.source addObject:@"Direct Forwarding"];
    [self.iconSource addObject:@"ic_directForwarding"];
    [self.vcIdentifiers addObject:ViewControllerIdentifiers_DirectForwardingVCID];
    [self.source addObject:@"Mesh OTA"];
    [self.iconSource addObject:@"ic_mesh_ota"];
    [self.vcIdentifiers addObject:@""];
    [self.source addObject:@"Proxy Filter"];
    [self.iconSource addObject:@"ic_proxyFilter"];
    [self.vcIdentifiers addObject:@""];
    [self.source addObject:@"Private Beacon"];
    [self.iconSource addObject:@"ic_guangbo"];
    [self.vcIdentifiers addObject:@""];
}

- (void)normalSetting{
    [super normalSetting];
    self.title = @"Network";
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    self.tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_SettingDetailItemCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_SettingDetailItemCellID];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_SettingTitleItemCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_SettingTitleItemCellID];
}

@end
