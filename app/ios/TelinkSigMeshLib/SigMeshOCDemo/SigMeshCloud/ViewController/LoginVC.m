/********************************************************************************************************
 * @file     LoginVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/1/3
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

#import "LogInVC.h"
#import "NSString+extension.h"

@interface LoginVC ()
@property (weak, nonatomic) IBOutlet UITextField *emailTF;
@property (weak, nonatomic) IBOutlet UITextField *passwordTF;
@property (weak, nonatomic) IBOutlet UIButton *loginButton;

@end

@implementation LoginVC

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    [self loadLocationData];
    [self configEyeButton];
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
    [self.navigationController setNavigationBarHidden:YES animated:animated];
}

- (void)loadLocationData {
    NSString *username = [NSUserDefaults.standardUserDefaults valueForKey:kUserName];
    NSString *password = [NSUserDefaults.standardUserDefaults valueForKey:kPassword];
    if (username != nil) {
        self.emailTF.text = username;
    }
    if (password != nil) {
        self.passwordTF.text = password;
    }
}

- (void)configEyeButton {
    self.passwordTF.secureTextEntry = YES;
    UIButton *eyeButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [eyeButton setFrame:CGRectMake(0, 0, 40, 40)];
    [eyeButton setBackgroundImage:[UIImage imageNamed:@"denglu-mimabukejian"] forState:UIControlStateNormal];
    [eyeButton setBackgroundImage:[UIImage imageNamed:@"denglu-mimakejian"] forState:UIControlStateSelected];
    [eyeButton addTarget:self action:@selector(clickEyeButton:) forControlEvents:UIControlEventTouchUpInside];
    self.passwordTF.rightView = eyeButton;
    self.passwordTF.rightViewMode = UITextFieldViewModeAlways;
}

- (void)clickEyeButton:(UIButton *)button {
    button.selected = !button.selected;
    self.passwordTF.secureTextEntry = !self.passwordTF.secureTextEntry;
}

- (IBAction)clickLoginButton:(UIButton *)sender {
    [self.view endEditing:YES];
    if (self.emailTF.text.length == 0) {
        TelinkLogInfo(@"Username is empty.");
        [self showTips:@"Username is empty."];
        return;
    }
    if (self.passwordTF.text.length == 0) {
        TelinkLogInfo(@"Password is empty.");
        [self showTips:@"Password is empty."];
        return;
    }
    NSString *username = self.emailTF.text;
    NSString *password = self.passwordTF.text;
    
    __weak typeof(self) weakSelf = self;
    [AppDataSource.share loginWithUsername:username password:password.sha256String resultBlock:^(NSError * _Nonnull error) {
        if (error) {
            TelinkLogInfo(@"login error = %@", error);
            [weakSelf showTips:[NSString stringWithFormat:@"login error = %@", error.localizedDescription]];
        } else {
            [NSUserDefaults.standardUserDefaults setValue:username forKey:kUserName];
            [NSUserDefaults.standardUserDefaults setValue:password forKey:kPassword];
            [NSUserDefaults.standardUserDefaults synchronize];
            NSNumber *localNetworkId = [weakSelf getLocalNetworkId];
            if (localNetworkId != nil) {
                //存在
                AppDataSource.share.curNetworkId = localNetworkId.intValue;
                [AppDataSource.share getDetailWithNetworkId:localNetworkId.intValue type:[weakSelf.getLocalNetworkType intValue] resultBlock:^(NSError * _Nullable error) {
                    if (error) {
                        TelinkLogInfo(@"%@", error.localizedDescription);
                        [weakSelf pushToNetworkListVCWithHiddenBackButton:YES];
                    } else {
                        [weakSelf setBaseTabbarControllerToRootViewController];
                    }
                }];
                [AppDataSource.share getCreatedNetworkListWithResultBlock:nil];
                [AppDataSource.share getJoinedNetworkListWithResultBlock:nil];
            } else {
                //不存在
                [weakSelf pushToNetworkListVCWithHiddenBackButton:YES];
            }
        }
    }];    
}

- (IBAction)clickRegisterButton:(UIButton *)sender {
    [self.view endEditing:YES];
    [self presentToRegisterVC];
}

- (IBAction)clickForgetButton:(UIButton *)sender {
    [self.view endEditing:YES];
    [self presentToForgetVC];
}

- (void)presentToRegisterVC {
    UIViewController *vc = [UIStoryboard initVC:@"RegisterVC" storyboard:@"Cloud"];
    [self presentViewController:vc animated:YES completion:nil];
}

- (void)presentToForgetVC {
    UIViewController *vc = [UIStoryboard initVC:@"ResetPasswordVC" storyboard:@"Cloud"];
    [self presentViewController:vc animated:YES completion:nil];
}

@end
