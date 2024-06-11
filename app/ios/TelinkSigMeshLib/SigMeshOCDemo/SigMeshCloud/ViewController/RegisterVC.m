/********************************************************************************************************
 * @file     RegisterVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/1/9
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

#import "RegisterVC.h"
#import "NSString+extension.h"

//验证码获取间隔，当前默认间隔60秒。
#define kDefaultVerificationCodeGetInterval (60)

@interface RegisterVC ()
@property (weak, nonatomic) IBOutlet UITextField *nameTF;
@property (weak, nonatomic) IBOutlet UITextField *emailTF;
@property (weak, nonatomic) IBOutlet UITextField *passwordTF;
@property (weak, nonatomic) IBOutlet UITextField *verificationCodeTF;
@property (weak, nonatomic) IBOutlet UIButton *verificationCodeGetButton;
@property (weak, nonatomic) IBOutlet UIButton *registerButton;

@property (strong, nonatomic) NSDate *endDate;
@property (strong, nonatomic) NSTimer *timer;

@end

@implementation RegisterVC

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    [self configEyeButton];
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

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    if (self.timer) {
        [self.timer invalidate];
        self.timer = nil;
    }
}

- (void)clickEyeButton:(UIButton *)button {
    button.selected = !button.selected;
    self.passwordTF.secureTextEntry = !self.passwordTF.secureTextEntry;
}

- (IBAction)clickCloseButton:(UIButton *)sender {
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (IBAction)clickGetVerificationCodeButton:(UIButton *)sender {
    NSString *emailString = self.emailTF.text;
    if (![NSString validateEmail:emailString]) {
        TelinkLogInfo(@"Incorrect mailbox format.");
        [self showTips:@"Incorrect mailbox format."];
        return;
    }
//    TelinkLogInfo(@"计时开始");
    [self setVerificationCodeGetButtonEnable:NO];
    self.endDate = [NSDate dateWithTimeInterval:kDefaultVerificationCodeGetInterval sinceDate:[NSDate date]];
    self.timer = [NSTimer timerWithTimeInterval:1.0 target:self selector:@selector(refreshCountDownText) userInfo:nil repeats:YES];
    [[NSRunLoop currentRunLoop] addTimer:self.timer forMode:NSRunLoopCommonModes];
    [self.timer fire];
    __weak typeof(self) weakSelf = self;
    [TelinkHttpTool getVerificationCodeRequestWithMailAddress:emailString usage:TelinkUserUsage_register didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"getVerificationCode error = %@", err);
            [weakSelf showTips:[NSString stringWithFormat:@"getVerificationCode error = %@", err.localizedDescription]];
            [weakSelf countDownFinishAction];
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"%@", dic[@"message"]);
            } else {
                TelinkLogInfo(@"getVerificationCode result = %@", dic);
                [weakSelf showTips:[NSString stringWithFormat:@"getVerificationCode errorCode = %d, message = %@", code, dic[@"message"]]];
                [weakSelf countDownFinishAction];
            }
        }
    }];
}

- (IBAction)clickRegisterButton:(UIButton *)sender {
    self.nameTF.text = self.nameTF.text.removeAllSpaceAndNewlines;
    self.emailTF.text = self.emailTF.text.removeAllSpaceAndNewlines;
    self.verificationCodeTF.text = self.verificationCodeTF.text.removeAllSpaceAndNewlines;
    if (self.nameTF.text.length == 0) {
        TelinkLogInfo(@"Name is empty.");
        [self showTips:@"Name is empty."];
        return;
    }
    if (self.emailTF.text.length == 0) {
        TelinkLogInfo(@"Mail is empty.");
        [self showTips:@"Mail is empty."];
        return;
    }
    if (self.passwordTF.text.length == 0) {
        TelinkLogInfo(@"Password is empty.");
        [self showTips:@"Password is empty."];
        return;
    }
    NSString *emailString = self.emailTF.text;
    if (![NSString validateEmail:emailString]) {
        TelinkLogInfo(@"Incorrect mailbox format.");
        [self showTips:@"Incorrect mailbox format."];
        return;
    }
    if (self.verificationCodeTF.text.length != 6) {
        TelinkLogInfo(@"Please enter the correct verification code.");
        [self showTips:@"Please enter the correct verification code."];
        return;
    }
    NSString *name = self.nameTF.text;
    NSString *password = self.passwordTF.text;
    NSString *verificationCode = self.verificationCodeTF.text;
    
    __weak typeof(self) weakSelf = self;
    __block BOOL nameAvailable = NO;
    NSOperationQueue *oprationQueue = [[NSOperationQueue alloc] init];
    [oprationQueue addOperationWithBlock:^{
        dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
        [TelinkHttpTool checkNameAvailableRequestWithName:name didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
            if (err) {
                TelinkLogInfo(@"checkNameAvailable error = %@", err);
                [weakSelf showTips:[NSString stringWithFormat:@"checkNameAvailable error = %@", err.localizedDescription]];
            } else {
                NSDictionary *dic = (NSDictionary *)result;
                int code = [dic[@"code"] intValue];
                if (code == 200) {
                    nameAvailable = YES;
                } else {
                    TelinkLogInfo(@"checkNameAvailable result = %@", dic);
                    [weakSelf showTips:[NSString stringWithFormat:@"checkNameAvailable errorCode = %d, message = %@", code, dic[@"message"]]];
                }
            }
            dispatch_semaphore_signal(semaphore);
        }];
        dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
        if (nameAvailable == NO) {
            return;
        }
        semaphore = dispatch_semaphore_create(0);
        [TelinkHttpTool registerRequestWithMailAddress:emailString name:name verificationCode:verificationCode password:password.sha256String didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
            if (err) {
                TelinkLogInfo(@"register error = %@", err);
                [weakSelf showTips:[NSString stringWithFormat:@"register error = %@", err.localizedDescription]];
            } else {
                NSDictionary *dic = (NSDictionary *)result;
                int code = [dic[@"code"] intValue];
                if (code == 200) {
                    TelinkLogInfo(@"register successful!");
                    [weakSelf showAlertTitle:@"Register successful" message:[NSString stringWithFormat:@"You can log in with mailAddress:%@ or name:%@", emailString, name] sure:^(UIAlertAction *action) {
                        [weakSelf dismissViewControllerAnimated:YES completion:nil];
                    }];
                } else {
                    TelinkLogInfo(@"register result = %@", dic);
                    [weakSelf showTips:[NSString stringWithFormat:@"register errorCode = %d, message = %@", code, dic[@"message"]]];
                }
            }
            dispatch_semaphore_signal(semaphore);

        }];
        dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
    }];
}

- (void)refreshCountDownText {
//    TelinkLogInfo(@"计时一次");
    NSTimeInterval time = [_endDate timeIntervalSinceDate:[NSDate date]];
    int timeInt = ceil(time);
    [self.verificationCodeGetButton setTitle:[NSString stringWithFormat:@"%ds",timeInt] forState:UIControlStateNormal];
    if (timeInt <= 0) {
//        TelinkLogInfo(@"计时结束");
        [self countDownFinishAction];
    }
}

- (void)countDownFinishAction {
    [self.timer invalidate];
    self.timer = nil;
    dispatch_async(dispatch_get_main_queue(), ^{
        [self setVerificationCodeGetButtonEnable:YES];
        [self.verificationCodeGetButton setTitle:@"Get" forState:UIControlStateNormal];
    });
}

- (void)setVerificationCodeGetButtonEnable:(BOOL)enable {
    self.verificationCodeGetButton.enabled = enable;
}

@end
