/********************************************************************************************************
 * @file     ShowQRCodeViewController.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/11/19
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

#import "ShowQRCodeViewController.h"
#import "UIImage+Extension.h"
#import "zipAndUnzip+zipString.h"
#import "zipAndUnzip.h"

@interface ShowQRCodeViewController ()
@property (strong, nonatomic) IBOutlet UIImageView *QRCodeImage;
@property (strong, nonatomic) IBOutlet UILabel *countDownLabel;
@property (strong, nonatomic) NSDate *endDate;
@property (strong, nonatomic) NSTimer *timer;
@end

@implementation ShowQRCodeViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    if (self.uuidString.length == 36) {
        //QRCode + Cloud
        self.title = @"QRCode + Cloud";
        self.endDate = [NSDate dateWithTimeInterval:60 * 5 sinceDate:[NSDate date]];
        self.timer = [NSTimer timerWithTimeInterval:1.0 target:self selector:@selector(refreshCountDownLabel) userInfo:nil repeats:YES];
        [[NSRunLoop currentRunLoop] addTimer:self.timer forMode:NSRunLoopCommonModes];
        TeLogInfo(@"计时开始");
        [self.timer fire];
    } else {
        self.countDownLabel.hidden = YES;
        //QRCode + BLE Transfer
        self.title = @"QRCode + BLE Transfer";
    }
    self.QRCodeImage.image = [UIImage createQRImageWithString:self.uuidString rate:3];
}

- (void)backToMain{
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.navigationController popViewControllerAnimated:YES];
    });
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear: animated];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(backToMain) name:@"BackToMain" object:nil];
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    if (self.timer) {
        [self.timer invalidate];
        self.timer = nil;
    }
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"BackToMain" object:nil];
}

- (void)refreshCountDownLabel {
//    TeLogInfo(@"计时一次");
    NSTimeInterval time = [_endDate timeIntervalSinceDate:[NSDate date]];
    int timeInt = ceil(time);
    self.countDownLabel.text = [NSString stringWithFormat:@"QR_Code available in %d seconds.",timeInt];
    if (timeInt <= 0) {
        [self.timer invalidate];
        self.timer = nil;
        TeLogInfo(@"计时结束");
        __weak typeof(self) weakSelf = self;
        [self showTips:@"QR_Code is invalid!" sure:^(UIAlertAction *action) {
            [weakSelf.navigationController popViewControllerAnimated:YES];
        }];
    }
}

-(void)dealloc{
    TeLogDebug(@"%s",__func__);
}

@end
