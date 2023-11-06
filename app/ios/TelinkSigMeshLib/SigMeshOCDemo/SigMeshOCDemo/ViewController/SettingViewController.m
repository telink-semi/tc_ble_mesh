/********************************************************************************************************
 * @file     SettingViewController.m 
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/7/31
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

#import "SettingViewController.h"
#import "SettingItemCell.h"
#import "MeshOTAVC.h"
#import "ResponseTestVC.h"
#import <StoreKit/StoreKit.h>
#import "UIViewController+Message.h"

@interface SettingViewController()<UITableViewDataSource,UITableViewDelegate,SKStoreProductViewControllerDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <NSString *>*source;
@property (nonatomic, strong) NSMutableArray <NSString *>*iconSource;
@property (nonatomic, strong) NSMutableArray <NSString *>*vcIdentifiers;
@property (weak, nonatomic) IBOutlet UILabel *versionLabel;
@end

@implementation SettingViewController

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    SettingItemCell *cell = (SettingItemCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_SettingItemCellID forIndexPath:indexPath];
    cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    cell.nameLabel.text = self.source[indexPath.row];
    cell.iconImageView.image = [UIImage imageNamed:self.iconSource[indexPath.row]];
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    NSString *titleString = self.source[indexPath.row];
    if ([titleString isEqualToString:@"How to import bin file?"]) {
        [self pushToTipsVC];
        return;
    }
    if ([titleString isEqualToString:@"Get More Telink Apps"]) {
        [self pushToTelinkApps];
        return;
    }
    NSString *sb = @"Setting";
    UIViewController *vc = nil;
    if ([titleString isEqualToString:@"Log"]) {
        sb = @"Main";
    }

    if ([titleString isEqualToString:@"Mesh OTA"]) {
        vc = [[MeshOTAVC alloc] init];
    } else if ([titleString isEqualToString:@"Test"]) {
        [self clickTest];
        return;
    } else {
        vc = [UIStoryboard initVC:self.vcIdentifiers[indexPath.row] storyboard:sb];
    }
    [self.navigationController pushViewController:vc animated:YES];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.source.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 51.0;
}

- (void)clickTest {
    __weak typeof(self) weakSelf = self;
    UIAlertController *actionSheet = [UIAlertController alertControllerWithTitle:@"Select Test Actions" message:nil preferredStyle:UIAlertControllerStyleAlert];
    UIAlertAction *alertT1 = [UIAlertAction actionWithTitle:@"Response Test" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        ResponseTestVC *vc = [[ResponseTestVC alloc] init];
        vc.isResponseTest = YES;
        [weakSelf.navigationController pushViewController:vc animated:YES];
    }];
    [actionSheet addAction:alertT1];
    UIAlertAction *alertT2 = [UIAlertAction actionWithTitle:@"Interval Test" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        ResponseTestVC *vc = [[ResponseTestVC alloc] init];
        vc.isResponseTest = NO;
        [weakSelf.navigationController pushViewController:vc animated:YES];
    }];
    [actionSheet addAction:alertT2];
    UIAlertAction *alertF = [UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
        NSLog(@"Cancel");
    }];
    [actionSheet addAction:alertF];
    [self presentViewController:actionSheet animated:YES completion:nil];
}

- (void)pushToTipsVC {
    UIViewController *vc = [UIStoryboard initVC:@"TipsVC" storyboard:@"Setting"];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)pushToTelinkApps {
    //实现代理SKStoreProductViewControllerDelegate
    SKStoreProductViewController *storeProductViewContorller = [[SKStoreProductViewController alloc] init];
    storeProductViewContorller.delegate = self;
    //加载一个新的视图展示
    [storeProductViewContorller loadProductWithParameters: @{SKStoreProductParameterITunesItemIdentifier : @"1637594591"} completionBlock:^(BOOL result, NSError *error) {
        //回调
        if(error){
//            NSLog(@"错误%@",error);
//            [self showTips:@"Push to `Telink Apps` of App Store fail, please check the network!"];
            if (error.localizedDescription) {
                [self showTips:error.localizedDescription];
            } else {
                [self showTips:[NSString stringWithFormat:@"%@", error]];
            }
        }else{
            //AS应用界面
            if (@available(iOS 10.0, *)) {
            } else {
                [[UINavigationBar appearance] setTintColor:[UIColor blueColor]];
            }
            [self presentViewController:storeProductViewContorller animated:YES completion:nil];
        }
    }];
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = NO;
    [self initData];
    [self.tableView reloadData];
}

- (void)initData {
    self.source = [NSMutableArray array];
    self.iconSource = [NSMutableArray array];
    self.vcIdentifiers = [NSMutableArray array];
    if (SigDataSource.share.curMeshIsVisitor == NO) {
        if (kShowScenes) {
            [self.source addObject:@"Scenes"];
            [self.iconSource addObject:@"scene"];
            [self.vcIdentifiers addObject:ViewControllerIdentifiers_SceneListViewControllerID];
        }
    }
    if (kshowShare) {
        [self.source addObject:@"Share"];
        [self.iconSource addObject:@"ic_model"];
        [self.vcIdentifiers addObject:ViewControllerIdentifiers_ShareViewControllerID];
    }
    if (SigDataSource.share.curMeshIsVisitor == NO) {
        [self.source addObject:@"Mesh OTA"];
        [self.iconSource addObject:@"ic_mesh_ota"];
        [self.vcIdentifiers addObject:@"no found"];
    }
    if (kshowLog) {
        [self.source addObject:@"Log"];
        [self.iconSource addObject:@"ic_model"];
        [self.vcIdentifiers addObject:ViewControllerIdentifiers_LogViewControllerID];
    }
    if (kshowMeshInfo) {
        [self.source addObject:@"Mesh Info"];
        [self.iconSource addObject:@"ic_model"];
        [self.vcIdentifiers addObject:ViewControllerIdentifiers_MeshInfoViewControllerID];
    }
    if (kshowMeshSettings) {
        [self.source addObject:@"Settings"];
        [self.iconSource addObject:@"ic_model"];
        [self.vcIdentifiers addObject:ViewControllerIdentifiers_SettingsVCID];
    }
    if (SigDataSource.share.curMeshIsVisitor == NO) {
        if (kshowTest) {
            [self.source addObject:@"Test"];
            [self.iconSource addObject:@"ic_model"];
            [self.vcIdentifiers addObject:ViewControllerIdentifiers_TestVCID];
        }
        [self.source addObject:@"Direct Control List"];
        [self.iconSource addObject:@"ic_model"];
        [self.vcIdentifiers addObject:ViewControllerIdentifiers_DirectControlListVCID];
        [self.source addObject:@"Forwarding Table List"];
        [self.iconSource addObject:@"ic_model"];
        [self.vcIdentifiers addObject:ViewControllerIdentifiers_ForwardingTableVCID];
    }
    
    //v3.3.3.6新增 How to import bin file?
    [self.source addObject:@"How to import bin file?"];
    [self.iconSource addObject:@"ic_model"];
    [self.vcIdentifiers addObject:@""];
    //v3.3.3.6新增 Get More Telink Apps
    [self.source addObject:@"Get More Telink Apps"];
    [self.iconSource addObject:@"ic_model"];
    [self.vcIdentifiers addObject:@""];
}

- (void)normalSetting{
    [super normalSetting];
    self.title = @"Setting";
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    
//    NSString *app_Version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
    NSString *app_Version = kTelinkSigMeshLibVersion;
    
//#ifdef DEBUG
//    NSString *appBundleVersion = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleVersion"];
//    self.versionLabel.text = [NSString stringWithFormat:@"%@ Bulid:%@",app_Version,appBundleVersion];
//#else
    self.versionLabel.text = [NSString stringWithFormat:@"%@",app_Version];
//#endif

}

#pragma mark - SKStoreProductViewControllerDelegate

//取消按钮监听
- (void)productViewControllerDidFinish:(SKStoreProductViewController *)viewController {
    if (@available(iOS 10.0, *)) {
    } else {
        [[UINavigationBar appearance] setTintColor:[UIColor whiteColor]];
    }
    [self dismissViewControllerAnimated:YES completion:nil];
}

@end
