/********************************************************************************************************
 * @file     NetworkListVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/10/11
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

#import "NetworkListVC.h"
#import "NetworkCell.h"
#import "UIButton+extension.h"
#import "CustomShareAlertView.h"
#import "MeshInfoVC.h"
#import "ShareOutVC.h"
#import "ShareInVC.h"
#import "UIViewController+Message.h"
#import "NSString+extension.h"

@interface NetworkListVC ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIView *importView;
@property (nonatomic, strong) NSMutableArray <SigDataSource *>*source;
@end

@implementation NetworkListVC

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    NetworkCell *cell = (NetworkCell *)[tableView dequeueReusableCellWithIdentifier:NSStringFromClass(NetworkCell.class) forIndexPath:indexPath];
    SigDataSource *model = self.source[indexPath.row];
    cell.nameLabel.text = model.meshName;
    cell.uuidLabel.text = model.meshUUID;
    cell.createTimeLabel.text = model.timestamp;
    [cell setUIWithSelected:[model.meshUUID isEqualToString:SigDataSource.share.meshUUID] bgView:cell.bgView];
    __weak typeof(self) weakSelf = self;
    [cell.moreButton addAction:^(UIButton *button) {
        [weakSelf clickMoreButtonWithNetwork:model];
    }];
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    UITableViewCell *cell = [tableView cellForRowAtIndexPath:indexPath];
    cell.selected = NO;
    [tableView reloadData];
    [self clickMoreButtonWithNetwork:self.source[indexPath.row]];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.source.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 160;
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
    [self reloadDataAndUI];
}

- (void)reloadDataAndUI {
    NSArray *meshList = [[NSUserDefaults standardUserDefaults] valueForKey:kCacheMeshListKey];
    self.source = [[NSMutableArray alloc] init];
    for (NSData *data in meshList) {
        SigDataSource *ds = [[SigDataSource alloc] init];
        [ds setDictionaryToDataSource:[LibTools getDictionaryWithJSONData:data]];
        if ([ds.meshUUID isEqualToString:SigDataSource.share.meshUUID]) {
            [self.source addObject:SigDataSource.share];
        } else {
            [self.source addObject:ds];
        }
    }
    [self.tableView reloadData];
}

- (void)normalSetting{
    [super normalSetting];
    //cornerRadius
    self.importView.layer.cornerRadius = self.importView.bounds.size.width/2;
    //masksToBounds
    self.importView.layer.masksToBounds = YES;

    self.title = @"Network List";
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    self.tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(NetworkCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(NetworkCell.class)];
    //init rightBarButtonItem
    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemAdd target:self action:@selector(clickAddMeshNetwork)];
    self.navigationItem.rightBarButtonItem = rightItem;
}

- (IBAction)clickRemoveAllButton:(UIButton *)sender {
    __weak typeof(self) weakSelf = self;
    [self showAlertSureAndCancelWithTitle:@"Warning" message:@"remove all and create a new mesh?" sure:^(UIAlertAction *action) {
        [SigDataSource.share cleanAllLocalPrivateBeaconState];
        [weakSelf.source removeAllObjects];
        SigDataSource *ds = [[SigDataSource alloc] initDefaultMesh];
        [self.source addObject:ds];
        [[NSUserDefaults standardUserDefaults] setValue:ds.meshUUID forKey:kCacheCurrentMeshUUIDKey];
        [weakSelf saveMeshList];
        [weakSelf.tableView reloadData];
        [weakSelf switchMeshActionWithMeshDictionary:[ds getDictionaryFromDataSource]];
    } cancel:^(UIAlertAction *action) {

    }];
}

- (IBAction)clickImportMeshButton:(UIButton *)sender {
    ShareInVC *vc = (ShareInVC *)[UIStoryboard initVC:ViewControllerIdentifiers_ShareInViewControllerID storyboard:@"Setting"];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)clickMoreButtonWithNetwork:(SigDataSource *)network {
    ShareAlertItemModel *item1 = [[ShareAlertItemModel alloc] init];
    item1.itemType = ShareItemType_icon_text_next;
    item1.iconImage = [UIImage imageNamed:@"ic_detail"];
    item1.textString = @"Show Detail";
    item1.textColor = HEX(#4A87EE);
    ShareAlertItemModel *item2 = [[ShareAlertItemModel alloc] init];
    item2.itemType = ShareItemType_icon_text_next;
    item2.iconImage = [UIImage imageNamed:@"ic_share"];
    item2.textString = @"Share Export";
    item2.textColor = HEX(#4A87EE);
    ShareAlertItemModel *item3 = [[ShareAlertItemModel alloc] init];
    item3.itemType = ShareItemType_icon_text_next;
    item3.iconImage = [UIImage imageNamed:@"ic_switch"];
    item3.textString = @"Switch To This Network";
    item3.textColor = HEX(#4A87EE);
    ShareAlertItemModel *item4 = [[ShareAlertItemModel alloc] init];
    item4.itemType = ShareItemType_button;
    item4.textString = @"DELETE";

    __weak typeof(self) weakSelf = self;
    CustomShareAlertView *customAlertView = [[CustomShareAlertView alloc] initWithTitle:[NSString stringWithFormat:@"select action for: %@", network.meshName] itemArray:@[item1, item2, item3, item4] alertResult:^(NSInteger selectIndex) {
        if (selectIndex == 0) {
            [weakSelf clickShowDetailButtonWithNetwork:network];
        } else if (selectIndex == 1) {
            [weakSelf clickShareExportButtonWithNetwork:network];
        } else if (selectIndex == 2) {
            [weakSelf clickSwitchToThisNetworkButtonWithNetwork:network];
        } else if (selectIndex == 3) {
            [weakSelf clickDeleteNetwork:network];
        }
    }];
    [customAlertView showCustomAlertView];
}

- (void)clickShowDetailButtonWithNetwork:(SigDataSource *)network {
    MeshInfoVC *vc = (MeshInfoVC *)[UIStoryboard initVC:ViewControllerIdentifiers_MeshInfoViewControllerID storyboard:@"Setting"];
    vc.network = network;
    __weak typeof(self) weakSelf = self;
    [vc setBackNetwork:^(SigDataSource * _Nonnull nNetwork) {
        for (int i=0; i < self.source.count; i++) {
            SigDataSource *n = self.source[i];
            if ([n.meshUUID isEqualToString:network.meshUUID]) {
                [weakSelf.source replaceObjectAtIndex:i withObject:nNetwork];
                [weakSelf saveMeshList];
                [weakSelf.tableView reloadData];
                break;
            }
        }
    }];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)clickShareExportButtonWithNetwork:(SigDataSource *)network {
    ShareOutVC *vc = (ShareOutVC *)[UIStoryboard initVC:ViewControllerIdentifiers_ShareOutViewControllerID storyboard:@"Setting"];
    vc.network = network;
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)clickSwitchToThisNetworkButtonWithNetwork:(SigDataSource *)network {
    [self switchMeshActionWithMeshDictionary:network.getDictionaryFromDataSource];
    [self reloadDataAndUI];
}

- (void)clickDeleteNetwork:(SigDataSource *)network {
    if ([network.meshUUID isEqualToString:SigDataSource.share.meshUUID]) {
        [self showTips:@"can not delete current mesh network"];
        return;
    }
    NSString *msg = [NSString stringWithFormat:@"Are you sure delete mesh network, name:%@ uuid:%@",network.meshName,network.meshUUID];
    __weak typeof(self) weakSelf = self;
    [self showAlertSureAndCancelWithTitle:@"Warning" message:msg sure:^(UIAlertAction *action) {
        [SigDataSource.share cleanLocalPrivateBeaconStateWithMeshUUID:network.meshUUID];
        [weakSelf.source removeObject:network];
        [weakSelf saveMeshList];
        [weakSelf.tableView reloadData];
    } cancel:^(UIAlertAction *action) {

    }];
}

- (void)clickAddMeshNetwork {
    __weak typeof(self) weakSelf = self;
    UIAlertController *alertVc = [UIAlertController alertControllerWithTitle:@"Tips" message:@"Please input new mesh network name!" preferredStyle: UIAlertControllerStyleAlert];
    [alertVc addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
        textField.placeholder = @"new network name";
    }];
    UIAlertAction *action1 = [UIAlertAction actionWithTitle:@"Done" style:UIAlertActionStyleDestructive handler:^(UIAlertAction * _Nonnull action) {
        NSString *networkName = [[alertVc textFields] objectAtIndex:0].text;
        networkName = networkName.removeHeadAndTailSpacePro;
        NSLog(@"new networkName is %@", networkName);
        if (networkName == nil || networkName.length == 0) {
            [weakSelf showTips:@"Network name can not be empty!"];
            return;
        }
        [weakSelf addMeshNetworkActionWithMeshNetworkName:networkName];
    }];
    UIAlertAction *action2 = [UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:nil];
    [alertVc addAction:action2];
    [alertVc addAction:action1];
    [self presentViewController:alertVc animated:YES completion:nil];
}

- (void)addMeshNetworkActionWithMeshNetworkName:(NSString *)meshNetworkName {
    SigDataSource *ds = [[SigDataSource alloc] initDefaultMesh];
    ds.meshName = meshNetworkName;
    [self.source addObject:ds];
    [self saveMeshList];
    [self.tableView reloadData];
    [self showTips:[NSString stringWithFormat:@"addMeshNetwork:%@ successful.", meshNetworkName]];
}

- (void)saveMeshList {
    NSMutableArray *meshList = [NSMutableArray array];
    for (SigDataSource *n in self.source) {
        NSDictionary *meshDict = [n getDictionaryFromDataSource];
        NSData *tempData = [LibTools getJSONDataWithDictionary:meshDict];
        [meshList addObject:tempData];
    }
    [[NSUserDefaults standardUserDefaults] setValue:meshList forKey:kCacheMeshListKey];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

@end
