/********************************************************************************************************
 * @file     ShareInVC.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/1/24
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "ShareInVC.h"
#import <Reachability/Reachability.h>
#import "FileChooseVC.h"
#import "UIViewController+Message.h"
#import "ScanCodeVC.h"
#import "ShareTipsVC.h"

@interface ShareInVC ()
@property (weak, nonatomic) IBOutlet UIButton *selectCDTPButton;
@property (weak, nonatomic) IBOutlet UIButton *selectQRCodeAndCloudButton;
@property (weak, nonatomic) IBOutlet UIButton *selectJsonFileButton;
@property (weak, nonatomic) IBOutlet UIButton *importButton;
@property (strong, nonatomic) ScanCodeVC *scanCodeVC;

@end

@implementation ShareInVC

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    self.title = @"Share Import";
    // 由于mesh v1.1并未release CDTP功能，暂时屏蔽CDTP功能。
    self.selectCDTPButton.selected = NO;
    self.selectQRCodeAndCloudButton.selected = YES;
    self.selectJsonFileButton.selected = NO;
    self.importButton.backgroundColor = UIColor.telinkButtonBlue;
}

- (IBAction)clickSelectCDTPButton:(UIButton *)sender {
    self.selectCDTPButton.selected = YES;
    self.selectQRCodeAndCloudButton.selected = NO;
    self.selectJsonFileButton.selected = NO;
}

- (IBAction)clickSelectQRCodeAndCloudButton:(UIButton *)sender {
    self.selectCDTPButton.selected = NO;
    self.selectQRCodeAndCloudButton.selected = YES;
    self.selectJsonFileButton.selected = NO;
}

- (IBAction)clickSelectJsonFileButton:(UIButton *)sender {
    self.selectCDTPButton.selected = NO;
    self.selectQRCodeAndCloudButton.selected = NO;
    self.selectJsonFileButton.selected = YES;
}

- (IBAction)clickCDTPButton:(UIButton *)sender {
    ShareTipsVC *vc = (ShareTipsVC *)[UIStoryboard initVC:ViewControllerIdentifiers_ShareTipsVCID storyboard:@"Setting"];
    vc.title = TipsTitle_CDTPImport;
    vc.tipsMessage = TipsMessage_QRCodeAndCDTPTransferJSONToOtherPhone;
    [self.navigationController pushViewController:vc animated:YES];
}

- (IBAction)clickCloudTransferJsonButton:(UIButton *)sender {
    ShareTipsVC *vc = (ShareTipsVC *)[UIStoryboard initVC:ViewControllerIdentifiers_ShareTipsVCID storyboard:@"Setting"];
    vc.title = TipsTitle_QRCodeAndCloudTransferJSON;
    vc.tipsMessage = TipsMessage_QRCodeAndCloudTransferJSON;
    [self.navigationController pushViewController:vc animated:YES];
}

- (IBAction)clickJsonFileButton:(UIButton *)sender {
    ShareTipsVC *vc = (ShareTipsVC *)[UIStoryboard initVC:ViewControllerIdentifiers_ShareTipsVCID storyboard:@"Setting"];
    vc.title = TipsTitle_JSONFile;
    vc.tipsMessage = TipsMessage_JSONFile;
    [self.navigationController pushViewController:vc animated:YES];
}

- (IBAction)clickImportButton:(UIButton *)sender {
    if (self.selectCDTPButton.selected) {
        [SDKLibCommand stopMeshConnectWithComplete:nil];
        [self importMeshByCDTP];
    } else if (self.selectQRCodeAndCloudButton.selected) {
        [SDKLibCommand stopMeshConnectWithComplete:nil];
        [self importMeshByQRCodeAndBLETransfer];
    } else if (self.selectQRCodeAndCloudButton.selected) {
        [self importMeshByQRCodeAndCloud];
    } else if (self.selectJsonFileButton.selected) {
        [self importMeshByJsonFile];
    }
}

- (void)importMeshByCDTP {
    UIViewController *vc = [UIStoryboard initVC:ViewControllerIdentifiers_CDTPServiceListVCID storyboard:@"Setting"];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)importMeshByQRCodeAndBLETransfer {
    self.scanCodeVC.title = @"QRCode + BLE Transfer";
    [self.navigationController pushViewController:self.scanCodeVC animated:YES];
}

- (void)importMeshByQRCodeAndCloud {
    NSString *remoteHostName = @"www.apple.com";
    Reachability *hostReachability = [Reachability reachabilityWithHostName:remoteHostName];
    if (hostReachability.currentReachabilityStatus == NotReachable) {
        [self showTips:@"The Internet connection appears to be offline."];
        return;
    }

    self.scanCodeVC.title = @"QRCode + Cloud";
    [self.navigationController pushViewController:self.scanCodeVC animated:YES];
}

- (void)importMeshByJsonFile {
    FileChooseVC *vc = (FileChooseVC *)[UIStoryboard initVC:ViewControllerIdentifiers_FileChooseViewControllerID storyboard:@"Setting"];
    __weak typeof(self) weakSelf = self;
    [vc setBackJsonData:^(NSData * _Nonnull jsonData, NSString * _Nonnull jsonName) {
        [SDKLibCommand stopMeshConnectWithComplete:^(BOOL successful) {
            TelinkLogDebug(@"SigBearer close %@",(successful?@"successful":@"fail"));
            [weakSelf loadJsonData:jsonData jaonName:jsonName];
        }];
    }];
    [self.navigationController pushViewController:vc animated:YES];
}

///加载json文件到本地(json data->SigDataSource。)
- (void)loadJsonData:(NSData *)data jaonName:(NSString *)name{
    NSOperationQueue *operation = [[NSOperationQueue alloc] init];
    __weak typeof(self) weakSelf = self;
    [operation addOperationWithBlock:^{
        NSString *str = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        NSDictionary *dict = [LibTools getDictionaryWithJsonString:str];
        BOOL result = dict != nil;
        if (result) {
            [weakSelf handleMeshDictionaryFromShareImport:dict];
        } else {
            NSString *tipString = [NSString stringWithFormat:@"import %@ fail!",name];
            [weakSelf showTips:tipString];
            TelinkLogDebug(@"%@",tipString);
        }
    }];
}

- (ScanCodeVC *)scanCodeVC {
    if (!_scanCodeVC) {
        _scanCodeVC = [ScanCodeVC scanCodeVC];
        __weak typeof(self) weakSelf = self;
        [_scanCodeVC scanDataViewControllerBackBlock:^(id content) {
            //AnalysisShareDataVC
            if (weakSelf.selectQRCodeAndCloudButton.isSelected) {
                NSString *uuidString = (NSString *)content;
                if (uuidString.length && [LibTools validateUUID:uuidString]) {
                    [weakSelf getTelinkJsonWithUUID:uuidString];
                }else{
                    //hasn't data
                    [[NSNotificationCenter defaultCenter] postNotificationName:@"BackToMain" object:nil];
                    [weakSelf showTips:@"QRCode is error."];
                    return;
                }
            }
        }];
    }
    return _scanCodeVC;
}

- (void)getTelinkJsonWithUUID:(NSString *)uuid {
    __weak typeof(self) weakSelf = self;
    [SDKLibCommand stopMeshConnectWithComplete:^(BOOL successful) {
        TelinkLogDebug(@"SigBearer close %@",(successful?@"successful":@"fail"));
        [TelinkHttpManager.share downloadJsonDictionaryWithUUID:uuid didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
            dispatch_async(dispatch_get_main_queue(), ^{
                if (err) {
                    NSString *errstr = [NSString stringWithFormat:@"%@",err];
                    TelinkLogInfo(@"%@",errstr);
                    [weakSelf showTips:errstr];
                    [[NSNotificationCenter defaultCenter] postNotificationName:@"BackToMain" object:nil];
                } else {
                    TelinkLogInfo(@"result=%@",result);
                    NSDictionary *dic = (NSDictionary *)result;
                    BOOL isSuccess = [dic[@"isSuccess"] boolValue];
                    if (isSuccess) {
                        [weakSelf showDownloadJsonSuccess:[LibTools getDictionaryWithJsonString:dic[@"data"]] uuid:uuid];
                    }else{
                        [weakSelf showTips:dic[@"msg"]];
                        [[NSNotificationCenter defaultCenter] postNotificationName:@"BackToMain" object:nil];
                    }
                }
            });
        }];
    }];
}

- (void)showDownloadJsonSuccess:(NSDictionary *)jsonDict uuid:(NSString *)uuid {
    [[NSNotificationCenter defaultCenter] postNotificationName:@"BackToMain" object:nil];
    [self handleMeshDictionaryFromShareImport:jsonDict];
}

-(void)dealloc{
    TelinkLogDebug(@"");
}

@end
