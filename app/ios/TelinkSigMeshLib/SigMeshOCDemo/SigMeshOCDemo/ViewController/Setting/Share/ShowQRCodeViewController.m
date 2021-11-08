/********************************************************************************************************
 * @file     ShowQRCodeViewController.m
 *
 * @brief    for TLSR chips
 *
 * @author       Telink, 梁家誌
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *             The information contained herein is confidential and proprietary property of Telink
 *              Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *             of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *             Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 *              Licensees are granted free, non-transferable use of the information in this
 *             file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/
//
//  ScanQRCodeViewController.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/11/19.
//  Copyright © 2018年 Telink. All rights reserved.
//

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
    
    self.title = @"Share";
    self.QRCodeImage.image = [UIImage createQRImageWithString:self.uuidString rate:3];
    self.endDate = [NSDate dateWithTimeInterval:60 * 5 sinceDate:[NSDate date]];
    self.timer = [NSTimer timerWithTimeInterval:1.0 target:self selector:@selector(refreshCountDownLabel) userInfo:nil repeats:YES];
    [[NSRunLoop currentRunLoop] addTimer:self.timer forMode:NSRunLoopCommonModes];
    TeLogInfo(@"计时开始");
    [self.timer fire];
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    if (self.timer) {
        [self.timer invalidate];
        self.timer = nil;
    }
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
        [self showTips:@"QR_Code is invalid!"];
    }
}

- (void)showTips:(NSString *)tips{
    dispatch_async(dispatch_get_main_queue(), ^{
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Hits" message:tips preferredStyle:UIAlertControllerStyleAlert];
        [alertController addAction:[UIAlertAction actionWithTitle:@"Sure" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            TeLogDebug(@"点击确认");
            [self.navigationController popViewControllerAnimated:YES];
        }]];
        [self presentViewController:alertController animated:YES completion:nil];
    });
}

-(void)dealloc{
    TeLogDebug(@"%s",__func__);
}

@end
