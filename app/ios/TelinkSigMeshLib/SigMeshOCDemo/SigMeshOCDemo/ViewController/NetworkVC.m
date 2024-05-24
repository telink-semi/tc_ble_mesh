/********************************************************************************************************
 * @file     NetworkVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/10/10
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
#import "SettingDetailItemCell.h"
#import "SettingTitleItemCell.h"
#import "MeshOTAVC.h"
#import "ResponseTestVC.h"
#import "UIViewController+Message.h"
#import "MeshInfoVC.h"
#import "ProxyFilterVC.h"
#import "ActivityIndicatorCell.h"

#define kMeshInfo   @"Mesh Info"
#define kSolicitationPDU   @"Solicitation PDU"

@interface NetworkVC ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <NSString *>*source;
@property (nonatomic, strong) NSMutableArray <NSString *>*iconSource;
@property (nonatomic, strong) NSMutableArray <NSString *>*vcIdentifiers;
//the end time of broadcast Solicitation PDU.
@property (strong, nonatomic) NSDate *endDate;
@end

@implementation NetworkVC

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    NSString *title = self.source[indexPath.row];
    if ([title isEqualToString:kSolicitationPDU]) {
        //Solicitation PDU
        ActivityIndicatorCell *cell = (ActivityIndicatorCell *)[tableView dequeueReusableCellWithIdentifier:NSStringFromClass(ActivityIndicatorCell.class) forIndexPath:indexPath];
        cell.nameLabel.text = title;
        cell.iconImageView.image = [UIImage imageNamed:self.iconSource[indexPath.row]];
        NSComparisonResult result = [[NSDate date] compare:self.endDate];
        if (result == NSOrderedAscending) {
            [cell.broadcastActivityView startAnimating];
        } else {
            [cell.broadcastActivityView stopAnimating];
        }
        return cell;
    } else if ([title isEqualToString:kMeshInfo]) {
        SettingDetailItemCell *cell = (SettingDetailItemCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_SettingDetailItemCellID forIndexPath:indexPath];
        cell.accessoryType = UITableViewCellAccessoryNone;
        cell.nameLabel.text = self.source[indexPath.row];
        cell.detailLabel.text = SigDataSource.share.meshName;
        cell.iconImageView.image = [UIImage imageNamed:self.iconSource[indexPath.row]];
        return cell;
    }
    SettingTitleItemCell *cell = (SettingTitleItemCell *)[tableView dequeueReusableCellWithIdentifier:NSStringFromClass(SettingTitleItemCell.class) forIndexPath:indexPath];
    cell.accessoryType = UITableViewCellAccessoryNone;
    cell.nameLabel.text = self.source[indexPath.row];
    cell.iconImageView.image = [UIImage imageNamed:self.iconSource[indexPath.row]];
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    NSString *titleString = self.source[indexPath.row];
    NSString *sb = @"Setting";
    UIViewController *vc = nil;
    if ([titleString isEqualToString:kSolicitationPDU]) {
        NSComparisonResult result = [[NSDate date] compare:self.endDate];
        [self setSolicitationPDUEnable:result != NSOrderedAscending];
    } else if ([titleString isEqualToString:@"Mesh OTA"]) {
        vc = [[MeshOTAVC alloc] init];
    } else if ([titleString isEqualToString:@"Proxy Filter"]) {
        vc = [[ProxyFilterVC alloc] init];
    } else {
        vc = [UIStoryboard initVC:self.vcIdentifiers[indexPath.row] storyboard:sb];
        if ([titleString isEqualToString:kMeshInfo]) {
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

    [self.source addObject:kMeshInfo];
    [self.iconSource addObject:@"ic_network"];
    [self.vcIdentifiers addObject:ViewControllerIdentifiers_MeshInfoViewControllerID];
    [self.source addObject:@"Scenes"];
    [self.iconSource addObject:@"ic_scene"];
    [self.vcIdentifiers addObject:ViewControllerIdentifiers_SceneListViewControllerID];
    [self.source addObject:@"Direct Forwarding"];
    [self.iconSource addObject:@"ic_directForwarding"];
    [self.vcIdentifiers addObject:ViewControllerIdentifiers_DirectForwardingVCID];
    [self.source addObject:@"Mesh OTA"];
    [self.iconSource addObject:@"ic_meshota"];
    [self.vcIdentifiers addObject:@""];
//    [self.source addObject:@"Proxy Filter"];
//    [self.iconSource addObject:@"ic_proxyFilter"];
//    [self.vcIdentifiers addObject:@""];
    //Solicitation PDU
    [self.source addObject:kSolicitationPDU];
    [self.iconSource addObject:@"ic_publish"];
    [self.vcIdentifiers addObject:@""];
}

- (void)normalSetting{
    [super normalSetting];
    self.title = @"Network";
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    self.tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_SettingDetailItemCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_SettingDetailItemCellID];
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(SettingTitleItemCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(SettingTitleItemCell.class)];
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(ActivityIndicatorCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(ActivityIndicatorCell.class)];
}

- (void)setSolicitationPDUEnable:(BOOL)enable {
    if (enable) {
        self.endDate = [NSDate dateWithTimeInterval:10 sinceDate:[NSDate date]];
        __weak typeof(self) weakSelf = self;
        [SigMeshLib.share advertisingSolicitationPDUWithSource:SigDataSource.share.curLocationNodeModel.address destination:MeshAddress_allProxies advertisingInterval:10 result:^(BOOL isSuccess) {
            weakSelf.endDate = [NSDate dateWithTimeIntervalSince1970:0];
            [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
        }];
    } else {
        self.endDate = [NSDate dateWithTimeIntervalSince1970:0];
        [TPeripheralManager.share stopAdvertising];
    }
    [self.tableView reloadData];
}

@end
