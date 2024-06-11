/********************************************************************************************************
 * @file     UpdatePasswordVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/1/11
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

#import "UpdatePasswordVC.h"
#import "NSString+extension.h"

@interface UpdatePasswordVC ()
@property (weak, nonatomic) IBOutlet UITextField *oPasswordTF;
@property (weak, nonatomic) IBOutlet UITextField *nPasswordTF;
@property (weak, nonatomic) IBOutlet UIButton *updateButton;

@end

@implementation UpdatePasswordVC

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    self.title = @"Update Password";
    [self configEyeButton];
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)configEyeButton {
    self.oPasswordTF.secureTextEntry = YES;
    UIButton *eyeButton = [UIButton buttonWithType:UIButtonTypeCustom];
    [eyeButton setFrame:CGRectMake(0, 0, 40, 40)];
    [eyeButton setBackgroundImage:[UIImage imageNamed:@"denglu-mimabukejian"] forState:UIControlStateNormal];
    [eyeButton setBackgroundImage:[UIImage imageNamed:@"denglu-mimakejian"] forState:UIControlStateSelected];
    [eyeButton addTarget:self action:@selector(clickEyeButton:) forControlEvents:UIControlEventTouchUpInside];
    eyeButton.tag = 1;
    self.oPasswordTF.rightView = eyeButton;
    self.oPasswordTF.rightViewMode = UITextFieldViewModeAlways;
    
    self.nPasswordTF.secureTextEntry = YES;
    UIButton *eyeButton2 = [UIButton buttonWithType:UIButtonTypeCustom];
    [eyeButton2 setFrame:CGRectMake(0, 0, 40, 40)];
    [eyeButton2 setBackgroundImage:[UIImage imageNamed:@"denglu-mimabukejian"] forState:UIControlStateNormal];
    [eyeButton2 setBackgroundImage:[UIImage imageNamed:@"denglu-mimakejian"] forState:UIControlStateSelected];
    [eyeButton2 addTarget:self action:@selector(clickEyeButton:) forControlEvents:UIControlEventTouchUpInside];
    eyeButton2.tag = 2;
    self.nPasswordTF.rightView = eyeButton2;
    self.nPasswordTF.rightViewMode = UITextFieldViewModeAlways;
}

- (void)clickEyeButton:(UIButton *)button {
    button.selected = !button.selected;
    if (button.tag == 1) {
        self.oPasswordTF.secureTextEntry = !self.oPasswordTF.secureTextEntry;
    } else if (button.tag == 2) {
        self.nPasswordTF.secureTextEntry = !self.nPasswordTF.secureTextEntry;
    }
}

- (IBAction)clickCloseButton:(UIButton *)sender {
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (IBAction)clickUpdatePasswordButton:(UIButton *)sender {
    if (self.oPasswordTF.text.length == 0) {
        TelinkLogInfo(@"Old password is empty.");
        [self showTips:@"Old password is empty."];
        return;
    }
    if (self.nPasswordTF.text.length == 0) {
        TelinkLogInfo(@"New password is empty.");
        [self showTips:@"New password is empty."];
        return;
    }
    NSString *oPassword = self.oPasswordTF.text;
    NSString *nPassword = self.nPasswordTF.text;
    
    __weak typeof(self) weakSelf = self;
    [TelinkHttpTool updatePasswordRequestWithOldPassword:oPassword.sha256String newPassword:nPassword.sha256String didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"updatePassword error = %@", err);
            [weakSelf showTips:[NSString stringWithFormat:@"updatePassword error = %@", err.localizedDescription]];
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"updatePassword successful!");
                [weakSelf showAlertTitle:@"Update password successful" message:nil sure:^(UIAlertAction *action) {
                    [weakSelf.navigationController popViewControllerAnimated:YES];
                }];
            } else {
                TelinkLogInfo(@"updatePassword result = %@", dic);
                [weakSelf showTips:[NSString stringWithFormat:@"updatePassword errorCode = %d, message = %@", code, dic[@"message"]]];
            }
        }
    }];
}

@end
