/********************************************************************************************************
 * @file     ShareQRCodeVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/5/17
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

#import "ShareQRCodeVC.h"
#import "UIImage+Extension.h"

@interface ShareQRCodeVC ()
@property (strong, nonatomic) IBOutlet UIImageView *QRCodeImage;
@property (strong, nonatomic) IBOutlet UILabel *countDownLabel;
@property (strong, nonatomic) NSDate *endDate;
@property (strong, nonatomic) NSTimer *timer;

@end

@implementation ShareQRCodeVC

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    self.title = @"Share";
    //init rightBarButtonItem
    UIBarButtonItem *rightItem1 = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh target:self action:@selector(clickRefreshButton)];
    self.navigationItem.rightBarButtonItem = rightItem1;

    [self createShareLink];
}

- (void)clickRefreshButton {
    [self createShareLink];
}

- (void)createShareLink {
    //有效时间-单位分钟
    NSInteger validPeriod = 30;
    __weak typeof(self) weakSelf = self;
    [TelinkHttpTool createShareLinkRequestWithNetworkId:self.networkId validPeriod:validPeriod maxCount:1 didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"createShareLink error = %@", err);
            [weakSelf showTips:[NSString stringWithFormat:@"createShareLink error = %@", err.localizedDescription]];
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"createShareLink successful! dic=%@", dic);
                NSDictionary *dict = [NSDictionary dictionaryWithDictionary:dic[@"data"]];
                NSInteger shareId = [dict[@"id"] intValue];
                NSDictionary *shareDict = @{@"networkName":weakSelf.networkName, @"shareLinkId":@(shareId), @"userName":AppDataSource.share.userInfo.name};
                [weakSelf loadQrCodeString:[LibTools getJSONStringWithDictionary:shareDict] timeout:validPeriod];
            } else {
                TelinkLogInfo(@"createShareLink result = %@", dic);
                [weakSelf showTips:[NSString stringWithFormat:@"createShareLink errorCode = %d, message = %@", code, dic[@"message"]]];
            }
        }
    }];
}

- (void)loadQrCodeString:(NSString *)string timeout:(NSInteger)timeout {
    self.QRCodeImage.image = [UIImage createQRImageWithString:string rate:3];
    self.endDate = [NSDate dateWithTimeInterval:60 * timeout sinceDate:[NSDate date]];
    if (self.timer) {
        [self.timer invalidate];
        self.timer = nil;
    }
    self.timer = [NSTimer timerWithTimeInterval:1.0 target:self selector:@selector(refreshCountDownLabel) userInfo:nil repeats:YES];
    [[NSRunLoop currentRunLoop] addTimer:self.timer forMode:NSRunLoopCommonModes];
    TelinkLogInfo(@"计时开始");
    [self.timer fire];
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    if (self.timer) {
        [self.timer invalidate];
        self.timer = nil;
    }
}

- (void)refreshCountDownLabel {
//    TelinkLogInfo(@"计时一次");
    NSTimeInterval time = [_endDate timeIntervalSinceDate:[NSDate date]];
    int timeInt = ceil(time);
    NSString *timeString = @"";
    if (timeInt >= 60) {
        timeString = [timeString stringByAppendingFormat:@"%dmin", timeInt / 60];
    }
    if (timeInt % 60 != 0) {
        timeString = [timeString stringByAppendingFormat:@"%ds", timeInt % 60];
    }
    self.countDownLabel.text = [NSString stringWithFormat:@"Available in %@", timeString];
    if (timeInt <= 0) {
        [self.timer invalidate];
        self.timer = nil;
        TelinkLogInfo(@"计时结束");
        [self showTips:@"QR_Code is invalid!"];
    }
}

-(void)dealloc{
    TelinkLogDebug(@"%s",__func__);
}

@end
