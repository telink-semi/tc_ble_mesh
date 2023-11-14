/********************************************************************************************************
 * @file     LogViewController.m
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

#import "LogViewController.h"

@interface LogViewController()
@property (weak, nonatomic) IBOutlet UITextView *contentText;
@property (nonatomic,strong) NSOperationQueue *operation;
@end

@implementation LogViewController

- (IBAction)clear:(UIButton *)sender {
    __weak typeof(self) weakSelf = self;
    [self.operation addOperationWithBlock:^{
        [SigLogger.share clearAllLog];
        dispatch_async(dispatch_get_main_queue(), ^{
            weakSelf.contentText.text = nil;
        });
        TelinkLogDebug(@"click clear");
    }];
}

- (void)normalSetting{
    [super normalSetting];
    self.title = @"Log";
    self.operation = [[NSOperationQueue alloc] init];
    [self updateContent];
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)updateContent{
    __weak typeof(self) weakSelf = self;
    [self.operation addOperationWithBlock:^{
        //日志文件可能比较大，只显示最新的100K的log信息。
        NSString *str = [SigLogger.share getLogStringWithLength:1024 * 100];
        dispatch_async(dispatch_get_main_queue(), ^{
            weakSelf.contentText.text = str;
        });
    }];
}

- (void)dealloc {
    TelinkLogInfo(@"");
}

@end
