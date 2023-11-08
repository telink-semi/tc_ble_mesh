/********************************************************************************************************
 * @file     CDTPExportVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/7/11
 *
 * @par     Copyright (c) [2023], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "CDTPExportVC.h"

@interface CDTPExportVC ()<CDTPServiceDelegate>
@property (nonatomic, strong) CDTPServiceModel *service;
@property (weak, nonatomic) IBOutlet UILabel *tipsLabel;

@end

@implementation CDTPExportVC

- (void)normalSetting {
    [super normalSetting];
    
    self.title = @"CDTP Export";
    self.service = [[CDTPServiceModel alloc] initWithShareMeshDictionary:self.meshDictionary];
    self.service.delegate = self;
    [self.service startAdvertising];
    self.tipsLabel.text = @"start export mesh object";
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    [self.service endShare];
}

/// A callback called whenever the CDTP Client Read some object data from CDTP Service.
/// @param progress The progress of read object. Range of progress is 0.0~1.0, 1.0 mean success.
- (void)onServiceReadProgress:(float)progress {
    TeLogInfo(@"progress=%f", progress);
    self.tipsLabel.text = [NSString stringWithFormat:@"Read Progress: %0.f%%", progress * 100];
    if (progress == 1.0) {
        __weak typeof(self) weakSelf = self;
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(3.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            [weakSelf showTips:@"Share success!" sure:^(UIAlertAction *action) {
                [weakSelf.navigationController popViewControllerAnimated:YES];
            }];
        });
    }
}

/// This method is called back when an exception is triggered when the CDTP Client write Object data to the CDTP Service.
/// @param error Error of Write progress.
//- (void)onServiceWriteError:(NSError *)error {
//    TeLogInfo(@"error=%@", error);
//    self.tipsLabel.text = [NSString stringWithFormat:@"Write Error: %@", error.localizedDescription];
//    __weak typeof(self) weakSelf = self;
//    [self showTips:[NSString stringWithFormat:@"Write Error: %@", error.localizedDescription] sure:^(UIAlertAction *action) {
//        [weakSelf.navigationController popViewControllerAnimated:YES];
//    }];
//}

/// This method is called back when an exception is triggered when the CDTP Client read Object data from the CDTP Service.
/// @param error Error of Read progress.
- (void)onServiceReadError:(NSError *)error {
    TeLogInfo(@"error=%@", error);
    self.tipsLabel.text = [NSString stringWithFormat:@"Read Error: %@", error.localizedDescription];
    __weak typeof(self) weakSelf = self;
    [self showTips:[NSString stringWithFormat:@"Read Error: %@", error.localizedDescription] sure:^(UIAlertAction *action) {
        [weakSelf.navigationController popViewControllerAnimated:YES];
    }];
}

-(void)dealloc {
    TeLogDebug(@"");
}

@end
