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
    self.service = [[CDTPServiceModel alloc] initWithShareMeshDictionary:SigDataSource.share.getFormatDictionaryFromDataSource];
    self.service.delegate = self;
    [self.service startAdvertising];
    self.tipsLabel.text = @"start export mesh object";
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    [self.service endShare];
}

- (void)importMeshWithDictionary:(NSDictionary *)dict {
    //v3.3.3.6及以后新逻辑：
    //1.当前手机的provisioner只在当前手机使用，且本地地址不变。
    //2.使用setDictionaryToDataSource接收分享后调用checkExistLocationProvisioner判断provisioner.
    //3.判断手机本地是否存在ivIndex+sequenceNumber，存在则赋值到SigDataSource且sequenceNumber+128.且立刻缓存一次本地。
    //4.不存在则需要连接获取到beacon的ivIndex。sequenceNumber从0开始。
    //5.重新计算下一次添加设备使用的unicastAddress
    [SigDataSource.share resetMesh];
    [SigDataSource.share setDictionaryToDataSource:dict];
    [SigDataSource.share checkExistLocationProvisioner];
    [SigDataSource.share saveLocationData];

    //（可选）注意：调用[SigDataSource.share resetMesh]后，filter的配置将恢复默认的白名单和本地地址+0xFFFF的配置。如果客户需要想要自定义配置，则需要再这下面进行filter相关配置。
//    NSData *filterData = [[NSUserDefaults standardUserDefaults] valueForKey:kFilter];
//    NSDictionary *filterDict = [LibTools getDictionaryWithJSONData:filterData];
//    SigProxyFilterModel *filter = [[SigProxyFilterModel alloc] init];
//    [filter setDictionaryToSigProxyFilterModel:filterDict];
}

/// A callback called whenever the CDTP Client Write some object data to CDTP Service.
/// @param progress The progress of write object. Range of progress is 0.0~1.0
/// @param objectModel The mesh network object.
- (void)onServiceWriteProgress:(float)progress objectModel:(ObjectModel *)objectModel {
    TeLogInfo(@"progress=%f", progress);
    self.tipsLabel.text = [NSString stringWithFormat:@"Write Progress: %0.f%%", progress * 100];
    if (progress == 1) {
        //回传成功
        NSData *mesh = objectModel.objectData.decompressionData;
        NSDictionary *dict = [LibTools getDictionaryWithJSONData:mesh];
        [self importMeshWithDictionary:dict];
        __weak typeof(self) weakSelf = self;
        [self showTips:@"Share Success!" sure:^(UIAlertAction *action) {
            [weakSelf.navigationController popViewControllerAnimated:YES];
        }];
    }
}

/// A callback called whenever the CDTP Client Read some object data from CDTP Service.
/// @param progress The progress of read object. Range of progress is 0.0~1.0, 1.0 mean success.
- (void)onServiceReadProgress:(float)progress {
    TeLogInfo(@"progress=%f", progress);
    self.tipsLabel.text = [NSString stringWithFormat:@"Read Progress: %0.f%%", progress * 100];
    if (progress == 1.0) {
        __weak typeof(self) weakSelf = self;
        [self showTips:@"Share success!" sure:^(UIAlertAction *action) {
            [weakSelf.navigationController popViewControllerAnimated:YES];
        }];
    }
}

/// This method is called back when an exception is triggered when the CDTP Client write Object data to the CDTP Service.
/// @param error Error of Write progress.
- (void)onServiceWriteError:(NSError *)error {
    TeLogInfo(@"error=%@", error);
    self.tipsLabel.text = [NSString stringWithFormat:@"Write Error: %@", error.localizedDescription];
    __weak typeof(self) weakSelf = self;
    [self showTips:[NSString stringWithFormat:@"Write Error: %@", error.localizedDescription] sure:^(UIAlertAction *action) {
        [weakSelf.navigationController popViewControllerAnimated:YES];
    }];
}

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
