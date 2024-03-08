/********************************************************************************************************
 * @file     OTAVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/5/30
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

#import "OTAVC.h"
#import <AFNetworking/AFNetworking.h>
#import "OTAFileSource.h"

@interface OTAVC ()
@property (weak, nonatomic) IBOutlet UILabel *nodeInfoLabel;
@property (weak, nonatomic) IBOutlet UITextView *logTV;
@property (weak, nonatomic) IBOutlet UIButton *otaButton;
@property (weak, nonatomic) IBOutlet UIProgressView *otaProgressView;
@property (weak, nonatomic) IBOutlet UILabel *otaProgressLabel;

@property (nonatomic,strong) CloudNodeModel *cloudNodeModel;
@property (nonatomic, strong) UIColor *normalColor;
@property (nonatomic, strong) UIColor *unableColor;
@property (nonatomic, strong) CloudVersionInfoModel *lastVersionInfoModel;
@property (nonatomic,strong) NSData *binData;

@end

@implementation OTAVC

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    self.title = @"OTA";
    self.otaButton.backgroundColor = UIColor.telinkButtonBlue;
    self.otaProgressView.progress = 0;
    self.normalColor = UIColor.telinkButtonBlue;
    self.unableColor = [UIColor colorWithRed:185.0/255.0 green:185.0/255.0 blue:185.0/255.0 alpha:1.0];
    self.logTV.text = @"";
    self.logTV.editable = NO;
    self.cloudNodeModel = [AppDataSource.share getCloudNodeModelWithNodeAddress:self.model.address];
    [self checkVersion];
    [self updateUI];
}

- (void)updateUI {
    self.nodeInfoLabel.text = [NSString stringWithFormat:@"Node Info\naddress: 0x%04X\nUUID: %@\nversion: pid-0x%04X vid-0x%04X", self.model.address, self.model.UUID, CFSwapInt16HostToBig([LibTools uint16From16String:self.model.pid]), CFSwapInt16HostToBig([LibTools uint16From16String:self.model.vid])];
}

- (IBAction)clickOtaButton:(UIButton *)sender {
    self.otaProgressView.progress = 0;
    [self otaAction];
}

/*
 updateNodeBindState successful! dic={
     code = 200;
     data =     {
         binFilePath = "m-2/p-1/v-4/16850720992578258_mesh_telink_platform_20230526_V3.3.3.7.bin";
         compositionData = 1102010033376900070000000C01000002000300001002100410061007100013011303130413110200000000020002100613;
         createTime = 1685072099250;
         elementCount = 2;
         id = 4;
         info = "0526\U4e0a\U4f20-1";
         name = "3337\U7248\U672c";
         productId = 1;
         state = 0;
         updateTime = "<null>";
         vid = 13111;
     };
     message = "get version info success";
     totalCount = 0;
 }
 */
- (void)checkVersion {
    __weak typeof(self) weakSelf = self;
    [weakSelf addMessageToUI:@"get version - start"];
    [TelinkHttpTool getLatestVersionInfoRequestWithProductId:self.cloudNodeModel.productId didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            [weakSelf addMessageToUI:err.localizedDescription];
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                [weakSelf addMessageToUI:@"get version - success"];
                TelinkLogInfo(@"getLatestVersionInfo successful! dic=%@", dic);
                NSDictionary *dict = dic[@"data"];
                if (dict.count > 0) {
                    CloudVersionInfoModel *version = [[CloudVersionInfoModel alloc] init];
                    [version setDictionaryToCloudVersionInfoModel:dict];
                    weakSelf.lastVersionInfoModel = version;
                    [weakSelf addMessageToUI:[NSString stringWithFormat:@"version compare - local=%lX cloud=%lX", (long)weakSelf.cloudNodeModel.versionInfo.vid, (long)weakSelf.lastVersionInfoModel.vid]];
                    if (weakSelf.lastVersionInfoModel.vid >= weakSelf.cloudNodeModel.versionInfo.vid) {
                        [weakSelf addMessageToUI:[NSString stringWithFormat:@"Find the new version available - 0x%lX - %@", (long)weakSelf.lastVersionInfoModel.vid, weakSelf.lastVersionInfoModel.name]];
                        [weakSelf checkBinFile:weakSelf.lastVersionInfoModel.binFilePath];
                    } else {
                        [weakSelf addMessageToUI:@"The device is the latest version"];
                        [weakSelf setOtaEnable:NO];
                    }
                }
            } else {
                NSString *msg = [NSString stringWithFormat:@"getLatestVersionInfo errorCode = %d, message = %@", code, dic[@"message"]];
                [weakSelf addMessageToUI:msg];
            }
        }
    }];
}

- (void)checkBinFile:(NSString *)path {
    NSArray *array = [path componentsSeparatedByString:@"\\"]; //从字符/中分隔成多个元素的数组
    NSString *file = [array lastObject];
    if ([self isFileExist:file]) {
        [self addMessageToUI:[NSString stringWithFormat:@"bin file exists - %@", file]];
        //获取Documents 下的文件路径
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
        NSString *path = [paths objectAtIndex:0];
        NSString *pathString = [path stringByAppendingFormat:@"/%@",file];
        NSLog(@"path:%@", pathString);
        [self loadBinFile:pathString];
    } else {
        //重新下载
        [self downloadFile:path];
    }
}

/// 第二步、判断沙盒中是否存在此文件
- (BOOL)isFileExist:(NSString *)fileName {
    //获取Documents 下的文件路径
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *path = [paths objectAtIndex:0];
    NSString *filePath = [path stringByAppendingPathComponent:fileName];
    NSFileManager *fileManager = [NSFileManager defaultManager];
    BOOL result = [fileManager fileExistsAtPath:filePath];
    return result;
}

/// 第三步、下载PDF
- (void)downloadFile:(NSString *)downLoadUrl {
    __weak typeof(self)weakSelf = self;
    [TelinkHttpTool downloadBinRequestWithPath:downLoadUrl didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            [weakSelf addMessageToUI:err.localizedDescription];
        } else {
            [weakSelf addMessageToUI:@"download bin file - success"];
            NSURL *filePath = (NSURL *)result;
            [weakSelf loadBinFile:[filePath path]];
        }
    }];
}

/// 第四步、已经下载了的文件用webview显示
-(void)loadBinFile:(NSString *)filePath {
    NSData *data = [[NSFileHandle fileHandleForReadingAtPath:filePath] readDataToEndOfFile];
    if (data && data.length > 0) {
        self.binData = data;
        [self addMessageToUI:[NSString stringWithFormat:@"bin version: pid-0x%04X vid-0x%04X", CFSwapInt16HostToBig([OTAFileSource.share getPidWithOTAData:data]), CFSwapInt16HostToBig([OTAFileSource.share getVidWithOTAData:data])]];
    }
}

- (void)addMessageToUI:(NSString *)message {
    TelinkLogInfo(@"%@", message);
    if (self.logTV.text.length > 0) {
        self.logTV.text = [NSString stringWithFormat:@"%@\n%@ => %@", self.logTV.text, [LibTools getNowTimeStringInFormatHHmmss], message];
    } else {
        self.logTV.text = [NSString stringWithFormat:@"%@ => %@", [LibTools getNowTimeStringInFormatHHmmss], message];
    }
}

- (void)setOtaEnable:(BOOL)enable {
    self.otaButton.backgroundColor = enable ? self.normalColor : self.unableColor;
    self.otaButton.enabled = enable;
}

- (void)otaAction {
    TelinkLogVerbose(@"clickStartOTA");
    self.otaButton.backgroundColor = self.unableColor;
    [self addMessageToUI:@"start OTA"];
    __weak typeof(self) weakSelf = self;
    BOOL result = [OTAManager.share startOTAWithOtaData:self.binData models:@[self.model] singleSuccessAction:^(SigNodeModel *device) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [weakSelf OTASuccessAction];
        });
    } singleFailAction:^(SigNodeModel *device) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [weakSelf addMessageToUI:@"OTA fail"];
        });
    } singleProgressAction:^(float progress) {
        dispatch_async(dispatch_get_main_queue(), ^{
            weakSelf.otaProgressView.progress = progress/100.0;
            weakSelf.otaProgressLabel.text = [NSString stringWithFormat:@"%.0f%%", progress];
        });
    } finishAction:^(NSArray<SigNodeModel *> *successModels, NSArray<SigNodeModel *> *fileModels) {
        TelinkLogDebug(@"");
    }];
    TelinkLogDebug(@"result = %d",result);
}

- (void)OTASuccessAction {
    [self addMessageToUI:@"OTA success"];
    __weak typeof(self) weakSelf = self;
    [TelinkHttpTool updateNodeVersionRequestWithNodeId:self.cloudNodeModel.nodeId vid:self.lastVersionInfoModel.vid didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            [weakSelf addMessageToUI:err.localizedDescription];
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                [weakSelf addMessageToUI:@"update node version - success"];
                TelinkLogInfo(@"update node version successful! dic=%@", dic);
                weakSelf.cloudNodeModel.versionInfo.vid = weakSelf.lastVersionInfoModel.vid;
                UInt8 page0 = 0;
                NSMutableData *mData = [NSMutableData dataWithBytes:&page0 length:1];
                [mData appendData:[LibTools nsstringToHex:weakSelf.lastVersionInfoModel.compositionData]];
                SigPage0 *page = [[SigPage0 alloc] initWithParameters:mData];
                [weakSelf.model setCompositionData:page];
                [weakSelf updateUI];
            } else {
                NSString *msg = [NSString stringWithFormat:@"update node version errorCode = %d, message = %@", code, dic[@"message"]];
                [weakSelf addMessageToUI:msg];
            }
        }
    }];
}

@end
