/********************************************************************************************************
 * @file     ShareInVC.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/1/24
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

#import "ShareInVC.h"
#import "FileChooseVC.h"
#import "UIViewController+Message.h"
#import "ScanCodeVC.h"
#import "Reachability.h"
#import "ShareTipsVC.h"

@interface ShareInVC ()
@property (weak, nonatomic) IBOutlet UIButton *selectCDTPButton;
@property (weak, nonatomic) IBOutlet UIButton *selectQRCodeAndBLETransferButton;
@property (weak, nonatomic) IBOutlet UIButton *selectQRCodeAndCloudButton;
@property (weak, nonatomic) IBOutlet UIButton *selectJsonFileButton;
@property (weak, nonatomic) IBOutlet UIButton *importButton;
@property (strong, nonatomic) ScanCodeVC *scanCodeVC;

@end

@implementation ShareInVC

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    self.selectCDTPButton.selected = YES;
    self.selectQRCodeAndBLETransferButton.selected = NO;
    self.selectQRCodeAndCloudButton.selected = NO;
    self.selectJsonFileButton.selected = NO;
    self.importButton.backgroundColor = UIColor.telinkButtonBlue;
}

- (IBAction)clickSelectCDTPButton:(UIButton *)sender {
    self.selectCDTPButton.selected = YES;
    self.selectQRCodeAndBLETransferButton.selected = NO;
    self.selectQRCodeAndCloudButton.selected = NO;
    self.selectJsonFileButton.selected = NO;
}

- (IBAction)clickSelectQRCodeAndBLETransferButton:(UIButton *)sender {
    self.selectCDTPButton.selected = NO;
    self.selectQRCodeAndBLETransferButton.selected = YES;
    self.selectQRCodeAndCloudButton.selected = NO;
    self.selectJsonFileButton.selected = NO;
}

- (IBAction)clickSelectQRCodeAndCloudButton:(UIButton *)sender {
    self.selectCDTPButton.selected = NO;
    self.selectQRCodeAndBLETransferButton.selected = NO;
    self.selectQRCodeAndCloudButton.selected = YES;
    self.selectJsonFileButton.selected = NO;
}

- (IBAction)clickSelectJsonFileButton:(UIButton *)sender {
    self.selectCDTPButton.selected = NO;
    self.selectQRCodeAndBLETransferButton.selected = NO;
    self.selectQRCodeAndCloudButton.selected = NO;
    self.selectJsonFileButton.selected = YES;
}

- (IBAction)clickCDTPButton:(UIButton *)sender {
    ShareTipsVC *vc = (ShareTipsVC *)[UIStoryboard initVC:ViewControllerIdentifiers_ShareTipsVCID storyboard:@"Setting"];
    vc.title = TipsTitle_QRCodeAndBLETransferJSON;
    vc.tipsMessage = TipsMessage_QRCodeAndBLETransferJSON;
    [self.navigationController pushViewController:vc animated:YES];
}

- (IBAction)clickBLETransferJsonButton:(UIButton *)sender {
    ShareTipsVC *vc = (ShareTipsVC *)[UIStoryboard initVC:ViewControllerIdentifiers_ShareTipsVCID storyboard:@"Setting"];
    vc.title = TipsTitle_QRCodeAndBLETransferJSON;
    vc.tipsMessage = TipsMessage_QRCodeAndBLETransferJSON;
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
            TeLogDebug(@"SigBearer close %@",(successful?@"successful":@"fail"));
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
            NSString *tipString = [NSString stringWithFormat:@"import %@ success!",name];
            TeLogDebug(@"%@",tipString);
            [weakSelf showTips:tipString];
        } else {
            NSString *tipString = [NSString stringWithFormat:@"import %@ fail!",name];
            [weakSelf showTips:tipString];
            TeLogDebug(@"%@",tipString);
            return;
        }
        [SigDataSource.share removeMeshUUIDFromVisitorListCache:dict[@"meshUUID"]];
        [weakSelf importMeshWithDictionary:dict];
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
        TeLogDebug(@"SigBearer close %@",(successful?@"successful":@"fail"));
        [TelinkHttpManager.share downloadJsonDictionaryWithUUID:uuid didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
            dispatch_async(dispatch_get_main_queue(), ^{
                if (err) {
                    NSString *errstr = [NSString stringWithFormat:@"%@",err];
                    TeLogInfo(@"%@",errstr);
                    [weakSelf showTips:errstr];
                    [[NSNotificationCenter defaultCenter] postNotificationName:@"BackToMain" object:nil];
                } else {
                    TeLogInfo(@"result=%@",result);
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
    NSString *title = @"download success";
    if (self.selectQRCodeAndBLETransferButton.isSelected) {
        title = @"transfer success";
    }
    __weak typeof(self) weakSelf = self;
    UIAlertController *actionSheet = [UIAlertController alertControllerWithTitle:title message:@"APP will replace locat mesh data." preferredStyle:UIAlertControllerStyleAlert];
    UIAlertAction *alertT = [UIAlertAction actionWithTitle:@"replace mesh" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        NSLog(@"replace mesh");
        [weakSelf replaceMesh:jsonDict uuid:uuid];
        if (weakSelf.selectQRCodeAndBLETransferButton.isSelected) {
            //通过BLE Transfer的方式分享才需要定义访客角色，已经废弃。当前demo代码不会进入该流程。
            [SigDataSource.share addMeshUUIDToVisitorListCache:uuid];
        } else {
            [SigDataSource.share removeMeshUUIDFromVisitorListCache:jsonDict[@"meshUUID"]];
        }
        [[NSNotificationCenter defaultCenter] postNotificationName:@"BackToMain" object:nil];
    }];
    UIAlertAction *alertF = [UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
        NSLog(@"Cancel");
        [[NSNotificationCenter defaultCenter] postNotificationName:@"BackToMain" object:nil];
    }];
    [actionSheet addAction:alertT];
    [actionSheet addAction:alertF];
    [self presentViewController:actionSheet animated:YES completion:nil];
}

- (void)replaceMesh:(NSDictionary *)dict uuid:(NSString *)uuid {
    BOOL result = dict != nil;
    if (result) {
        NSString *tipString = [NSString stringWithFormat:@"import uuid:%@ success!",uuid];
        [self showTips:tipString];
        TeLogDebug(@"%@",tipString);
    } else {
        NSString *tipString = [NSString stringWithFormat:@"import uuid:%@ fail!",uuid];
        [self showTips:tipString];
        TeLogDebug(@"%@",tipString);
        return;
    }
    
    [self importMeshWithDictionary:dict];
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

-(void)dealloc{
    TeLogDebug(@"");
}

@end
