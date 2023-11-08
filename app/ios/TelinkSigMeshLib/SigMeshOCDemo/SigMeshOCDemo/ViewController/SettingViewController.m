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
#import "SettingDetailItemCell.h"
#import "MeshOTAVC.h"
#import "ResponseTestVC.h"
#import <StoreKit/StoreKit.h>
#import "UIViewController+Message.h"
#import "OOBListVC.h"
#import "CertificateListVC.h"

// define title string
#define kManagerNetworkTitle @"Manage Network"
#define kOOBDatabaseTitle @"OOB Database"
#define kRootCertTitle @"Root Cert"
#define kSettingsTitle @"Settings"
#define kTestTitle @"Test"
#define kHowToImportBinFileTitle    @"How to import bin file?"
#define kGetMoreTelinkAppsTitle    @"Get More Telink Apps"

@interface SettingViewController()<UITableViewDataSource,UITableViewDelegate,SKStoreProductViewControllerDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <NSString *>*titleSource;
@property (nonatomic, strong) NSMutableArray <NSString *>*detailSource;
@property (nonatomic, strong) NSMutableArray <NSString *>*iconSource;
@property (nonatomic, strong) NSMutableArray <NSString *>*vcIdentifiers;
@property (weak, nonatomic) IBOutlet UILabel *versionLabel;
@end

@implementation SettingViewController

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    SettingDetailItemCell *cell = (SettingDetailItemCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_SettingDetailItemCellID forIndexPath:indexPath];
    cell.accessoryType = UITableViewCellAccessoryNone;
    cell.nameLabel.text = self.titleSource[indexPath.row];
    cell.detailLabel.text = self.detailSource[indexPath.row];
    cell.iconImageView.image = [UIImage imageNamed:self.iconSource[indexPath.row]];
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    UITableViewCell *cell = [tableView cellForRowAtIndexPath:indexPath];
    cell.selected = NO;
    NSString *titleString = self.titleSource[indexPath.row];
    if ([titleString isEqualToString:kHowToImportBinFileTitle]) {
        [self pushToTipsVC];
    } else if ([titleString isEqualToString:kGetMoreTelinkAppsTitle]) {
        [self pushToTelinkApps];
    } else if ([titleString isEqualToString:kTestTitle]) {
        [self clickTest];
    } else if ([titleString isEqualToString:kOOBDatabaseTitle]) {
        [self pushToOOBVC];
    } else if ([titleString isEqualToString:kRootCertTitle]) {
        [self pushRootCertificateVC];
    } else {
        UIViewController *vc = [UIStoryboard initVC:self.vcIdentifiers[indexPath.row] storyboard:@"Setting"];
        [self.navigationController pushViewController:vc animated:YES];
    }
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.titleSource.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 70.0;
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

- (void)pushToOOBVC {
    OOBListVC *vc = [[OOBListVC alloc] init];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)pushRootCertificateVC {
    CertificateListVC *vc = [[CertificateListVC alloc] init];
    [self.navigationController pushViewController:vc animated:YES];
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
    self.titleSource = [NSMutableArray array];
    self.detailSource = [NSMutableArray array];
    self.iconSource = [NSMutableArray array];
    self.vcIdentifiers = [NSMutableArray array];
    
    [self.titleSource addObject:kManagerNetworkTitle];
    [self.detailSource addObject:@"Switch, Create/Delete, Import/Export..."];
    [self.iconSource addObject:@"setting_manage"];
    [self.vcIdentifiers addObject:ViewControllerIdentifiers_NetworkListVCID];
    [self.titleSource addObject:kOOBDatabaseTitle];
    [self.detailSource addObject:@"used in static-oob provisioning"];
    [self.iconSource addObject:@"setting_OOBDatabase"];
    [self.vcIdentifiers addObject:@""];
    [self.titleSource addObject:kRootCertTitle];
    [self.detailSource addObject:@"used in certificate-based provisioning"];
    [self.iconSource addObject:@"setting_certificate"];
    [self.vcIdentifiers addObject:@""];
    [self.titleSource addObject:kSettingsTitle];
    [self.detailSource addObject:@"other settings..."];
    [self.iconSource addObject:@"ic_setting"];
    [self.vcIdentifiers addObject:ViewControllerIdentifiers_SettingsViewControllerID];

    if (kshowTest) {
        [self.titleSource addObject:kTestTitle];
        [self.detailSource addObject:@"stress testing sends and receives Mesh packets."];
        [self.iconSource addObject:@"ic_model"];
        [self.vcIdentifiers addObject:@""];
    }
    
    //v3.3.3.6新增 How to import bin file?
    [self.titleSource addObject:@"How to import bin file?"];
    [self.detailSource addObject:@"prompt users on how to import Bin files into the app."];
    [self.iconSource addObject:@"ic_model"];
    [self.vcIdentifiers addObject:@""];
    //v3.3.3.6新增 Get More Telink Apps
    [self.titleSource addObject:@"Get More Telink Apps"];
    [self.detailSource addObject:@"prompt users on how to obtain more Telink tool apps."];
    [self.iconSource addObject:@"setting_modeAPP"];
    [self.vcIdentifiers addObject:@""];
}

- (void)normalSetting{
    [super normalSetting];
    self.title = @"Setting";
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    self.tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_SettingDetailItemCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_SettingDetailItemCellID];

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
