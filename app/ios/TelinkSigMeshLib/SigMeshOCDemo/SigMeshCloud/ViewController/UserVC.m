/********************************************************************************************************
 * @file     UserVC.m
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

#import "UserVC.h"
#import <StoreKit/StoreKit.h>
#import "SettingTitleItemCell.h"
#import "NSString+extension.h"
#import "SettingsViewController.h"

#define kChangePasswordString    @"Change password"
#define kManageNetworksString    @"Manage networks"
#define kSettingsString    @"Settings"
#define kLogoutString    @"Logout"
#define kHowToImportBinFileString    @"How to import bin file?"
#define kHowToGetMoreTelinkAppsString    @"Get More Telink Apps"

@interface UserVC ()<UITableViewDataSource,UITableViewDelegate,SKStoreProductViewControllerDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <NSString *>*source;
@property (nonatomic, strong) NSMutableArray <NSString *>*iconSource;
@property (weak, nonatomic) IBOutlet UILabel *versionLabel;
@property (weak, nonatomic) IBOutlet UIImageView *iconImageView;
@property (weak, nonatomic) IBOutlet UIButton *editButton;
@property (weak, nonatomic) IBOutlet UILabel *nickNameLabel;
@property (weak, nonatomic) IBOutlet UILabel *companyLabel;
@end

@implementation UserVC

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    SettingTitleItemCell *cell = (SettingTitleItemCell *)[tableView dequeueReusableCellWithIdentifier:NSStringFromClass(SettingTitleItemCell.class) forIndexPath:indexPath];
    cell.accessoryType = UITableViewCellAccessoryNone;
    NSString *title = self.source[indexPath.row];
    cell.nameLabel.text = title;
    cell.iconImageView.image = [UIImage imageNamed:self.iconSource[indexPath.row]];
    if ([title isEqualToString:kLogoutString]) {
        cell.nameLabel.textColor = HEX(#c64b30);
        cell.nextImageView.hidden = YES;
    }
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    UITableViewCell *cell = [tableView cellForRowAtIndexPath:indexPath];
    cell.selected = NO;
    NSString *titleString = self.source[indexPath.row];
    if ([titleString isEqualToString:kChangePasswordString]) {
        [self pushToChangePasswordVC];
    } else if ([titleString isEqualToString:kManageNetworksString]) {
        [self pushToNetworkListVCWithHiddenBackButton:NO];
    } else if ([titleString isEqualToString:kSettingsString]) {
        [self pushToSettingsVC];
    } else if ([titleString isEqualToString:kLogoutString]) {
        [self logoutAction];
    } else if ([titleString isEqualToString:kHowToImportBinFileString]) {
        [self pushToTipsVC];
    } else if ([titleString isEqualToString:kHowToGetMoreTelinkAppsString]) {
        [self pushToTelinkApps];
    }
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.source.count;
}

- (void)pushToSettingsVC {
    UIViewController *vc = [UIStoryboard initVC:NSStringFromClass(SettingsViewController.class) storyboard:@"Setting"];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)pushToChangePasswordVC {
    UIViewController *vc = [UIStoryboard initVC:@"UpdatePasswordVC" storyboard:@"Cloud"];
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
    self.title = @"User";
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(SettingTitleItemCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(SettingTitleItemCell.class)];
    self.source = [NSMutableArray array];
    self.iconSource = [NSMutableArray array];
    
    [self.source addObject:kChangePasswordString];
    [self.iconSource addObject:@"setting_changePassword"];
    [self.source addObject:kManageNetworksString];
    [self.iconSource addObject:@"setting_manage"];
    if (AppDataSource.share.curMeshNetworkDetailModel && AppDataSource.share.curMeshNetworkDetailModel.role == 0) {
        [self.source addObject:kSettingsString];
        [self.iconSource addObject:@"ic_setting"];
    }
    [self.source addObject:kLogoutString];
    [self.iconSource addObject:@"logout"];
    [self.source addObject:kHowToImportBinFileString];
    [self.iconSource addObject:@"setting_file"];
    [self.source addObject:kHowToGetMoreTelinkAppsString];
    [self.iconSource addObject:@"setting_more"];

    NSString *app_Version = kTelinkSigMeshLibVersion;
    self.versionLabel.text = [NSString stringWithFormat:@"%@",app_Version];

    [self refreshUserUI];
}

- (IBAction)clickEditButton:(UIButton *)sender {
    __weak typeof(self) weakSelf = self;
    UIAlertController *alertVc = [UIAlertController alertControllerWithTitle:kDefaultAlertTitle message:@"Please input new nickname!" preferredStyle:
UIAlertControllerStyleAlert];
    [alertVc addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
        textField.placeholder = @"new nickname";
    }];
    UIAlertAction *action1 = [UIAlertAction actionWithTitle:kDefaultAlertOK style:UIAlertActionStyleDestructive handler:^(UIAlertAction * _Nonnull action) {
        NSString *nickname = [[alertVc textFields] objectAtIndex:0].text;
        nickname = nickname.removeHeadAndTailSpacePro;
        NSLog(@"new nickname is %@", nickname);
        if (nickname == nil || nickname.length == 0) {
            [weakSelf showTips:@"Nickname can not be empty!"];
            return;
        }
        if ([nickname isEqualToString:AppDataSource.share.userInfo.nickname]) {
            [weakSelf showTips:@"Nickname is not change!"];
            return;
        }
        [weakSelf changeNicknameAction:nickname];
    }];
    UIAlertAction *action2 = [UIAlertAction actionWithTitle:kDefaultAlertCancel style:UIAlertActionStyleCancel handler:nil];
    [alertVc addAction:action2];
    [alertVc addAction:action1];
    [self presentViewController:alertVc animated:YES completion:nil];
}

- (void)logoutAction {
    __weak typeof(self) weakSelf = self;
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:kDefaultAlertTitle message:@"Are you sure to logout?" preferredStyle:UIAlertControllerStyleAlert];
    [alertController addAction:[UIAlertAction actionWithTitle:kDefaultAlertOK style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        TelinkLogDebug(@"点击确认");
        [TelinkHttpTool logoutRequestWithDidLoadData:^(id  _Nullable result, NSError * _Nullable err) {
            if (err) {
                TelinkLogInfo(@"logout error = %@", err);
                [weakSelf showTips:[NSString stringWithFormat:@"logout error = %@", err.localizedDescription]];
            } else {
                NSDictionary *dic = (NSDictionary *)result;
                int code = [dic[@"code"] intValue];
                if (code == 200) {
                    TelinkLogInfo(@"logout successful.");
                    [weakSelf cleanLocalNetworkId];
                    [AppDataSource.share logoutSuccessAction];
                    [SDKLibCommand stopMeshConnectWithComplete:nil];
                    //清除SigDataSource.share里面的所有参数（包括scanList、sequenceNumber、sequenceNumberOnDelegate），并随机生成新的默认参数。
                    [SigDataSource.share resetMesh];
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [weakSelf pushToLogInVCWithAnimated:YES];
                    });
                } else {
                    TelinkLogInfo(@"logout result = %@", dic);
                    [weakSelf showTips:[NSString stringWithFormat:@"logout errorCode = %d, message = %@", code, dic[@"message"]]];
                }
            }
        }];
    }]];
    [alertController addAction:[UIAlertAction actionWithTitle:kDefaultAlertCancel style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
        TelinkLogDebug(@"点击取消");
        
    }]];
    [self presentViewController:alertController animated:YES completion:nil];
}

- (void)changeNicknameAction:(NSString *)nickname {
    __weak typeof(self) weakSelf = self;
    [TelinkHttpTool updateInfoRequestWithNickname:nickname sign:nil phone:nil company:nil didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"updateInfo-nickname error = %@", err);
            [weakSelf showTips:[NSString stringWithFormat:@"updateInfo-nickname error = %@", err.localizedDescription]];
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                //更新nickname到AppDataSource，更新UI。
                [weakSelf showTips:[NSString stringWithFormat:@"update nickname:%@ successful.", nickname]];
                AppDataSource.share.userInfo.nickname = nickname;
                [weakSelf performSelectorOnMainThread:@selector(refreshUserUI) withObject:nil waitUntilDone:YES];
            } else {
                TelinkLogInfo(@"updateInfo-nickname result = %@", dic);
                [weakSelf showTips:[NSString stringWithFormat:@"updateInfo-nickname errorCode = %d, message = %@", code, dic[@"message"]]];
            }
        }
    }];
}

- (void)refreshUserUI {
    if (AppDataSource.share.userInfo) {
        self.nickNameLabel.text = AppDataSource.share.userInfo.nickname;
        self.companyLabel.text = AppDataSource.share.userInfo.company;
    }
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
