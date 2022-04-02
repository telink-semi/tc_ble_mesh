/********************************************************************************************************
 * @file     ShareVC.m
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

#import "ShareVC.h"
#import "ShareOutVC.h"
#import "ShareInVC.h"
#import "ShowQRCodeViewController.h"
#import "ScanCodeVC.h"
#import "ShareTipsVC.h"
#import "Reachability.h"

@interface ShareVC ()
@property (strong, nonatomic) ScanCodeVC *scanCodeVC;

@end

@implementation ShareVC

- (void)viewDidLoad {
    [super viewDidLoad];
    [self startCheckNetwork];
    [self configUI];
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)configUI{
    self.title = @"Share Mesh";
    
    [self setUpAllViewController];
    
    __weak typeof(self) weakSelf = self;
    double h = kGetRectNavAndStatusHight;
    [self setUpContentViewFrame:^(UIView *contentView) {
        contentView.frame = CGRectMake(0, 0, weakSelf.view.frame.size.width, weakSelf.view.frame.size.height-h);
    }];
    
    [self setUpTitleEffect:^(UIColor *__autoreleasing *titleScrollViewColor, UIColor *__autoreleasing *norColor, UIColor *__autoreleasing *selColor, UIFont *__autoreleasing *titleFont, CGFloat *titleHeight, CGFloat *titleWidth) {
        *norColor = [UIColor lightGrayColor];
        *selColor = [UIColor blackColor];
        *titleWidth = [UIScreen mainScreen].bounds.size.width / 2;
    }];
    
    // 标题渐变
    // *推荐方式(设置标题渐变)
    [self setUpTitleGradient:^(YZTitleColorGradientStyle *titleColorGradientStyle, UIColor *__autoreleasing *norColor, UIColor *__autoreleasing *selColor) {
        
    }];
    
    [self setUpUnderLineEffect:^(BOOL *isUnderLineDelayScroll, CGFloat *underLineH, UIColor *__autoreleasing *underLineColor,BOOL *isUnderLineEqualTitleWidth) {
        *underLineColor = kDefultColor;
        *isUnderLineEqualTitleWidth = YES;
    }];
    
    //设置返回按钮文字为空
    self.navigationItem.backBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@"" style:UIBarButtonItemStylePlain target:self action:nil];
//    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemAdd target:self action:@selector(clickTopRight:)];
    UIButton *but = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 60, 30)];
    [but setTitle:@"Tips" forState:UIControlStateNormal];
    [but addTarget:self action:@selector(clickTopRight:) forControlEvents:UIControlEventTouchUpInside];
    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithCustomView:but];
    self.navigationItem.rightBarButtonItem = rightItem;
}

// 添加所有子控制器
- (void)setUpAllViewController
{
    // ShareOutVC
    ShareOutVC *wordVc1 = (ShareOutVC *)[UIStoryboard initVC:ViewControllerIdentifiers_ShareOutViewControllerID storybroad:@"Setting"];
    wordVc1.title = @"EXPORT";
    [self addChildViewController:wordVc1];
    
    // ShareInVC
    ShareInVC *wordVc2 = (ShareInVC *)[UIStoryboard initVC:ViewControllerIdentifiers_ShareInViewControllerID storybroad:@"Setting"];
    wordVc2.title = @"IMPORT";
    [self addChildViewController:wordVc2];
}

- (void)clickTopRight:(UIButton *)button {
    ShareTipsVC *vc = (ShareTipsVC *)[UIStoryboard initVC:ViewControllerIdentifiers_ShareTipsVCID storybroad:@"Setting"];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)clickCamera{
    [self.navigationController pushViewController:self.scanCodeVC animated:YES];
}

- (void)clickExportByQRCode {
    __weak typeof(self) weakSelf = self;
    //设置有效时间5分钟
    [TelinkHttpManager.share uploadJsonDictionary:[SigDataSource.share getFormatDictionaryFromDataSource] timeout:60 * 5 didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        dispatch_async(dispatch_get_main_queue(), ^{
            if (err) {
                NSString *errstr = [NSString stringWithFormat:@"%@",err];
                TeLogInfo(@"%@",errstr);
                [weakSelf showTips:errstr];
            } else {
                TeLogInfo(@"result=%@",result);
                NSDictionary *dic = (NSDictionary *)result;
                BOOL isSuccess = [dic[@"isSuccess"] boolValue];
                if (isSuccess) {
                    [weakSelf pushToShowQRCodeVCWithUUID:dic[@"data"]];
                }else{
                    [weakSelf showTips:dic[@"msg"]];
                }
            }
        });
    }];
}

- (void)pushToShowQRCodeVCWithUUID:(NSString *)uuid {
    ShowQRCodeViewController *vc = (ShowQRCodeViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_ShowQRCodeViewControllerID storybroad:@"Setting"];
    vc.uuidString = uuid;
    [self.navigationController pushViewController:vc animated:YES];
}

- (ScanCodeVC *)scanCodeVC {
    if (!_scanCodeVC) {
        _scanCodeVC = [ScanCodeVC scanCodeVC];
        __weak typeof(self) weakSelf = self;
        [_scanCodeVC scanDataViewControllerBackBlock:^(id content) {
            //AnalysisShareDataVC
            NSString *uuidString = (NSString *)content;
            if (uuidString.length && [LibTools validateUUID:uuidString]) {
                [weakSelf getTelinkJsonWithUUID:uuidString];
            }else{
                //hasn't data
                [[NSNotificationCenter defaultCenter] postNotificationName:@"BackToMain" object:nil];
                [weakSelf showTips:@"QRCode is error."];
                return;
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
    __weak typeof(self) weakSelf = self;
    UIAlertController *actionSheet = [UIAlertController alertControllerWithTitle:@"download success" message:@"APP will replace locat mesh data." preferredStyle:UIAlertControllerStyleAlert];
    UIAlertAction *alertT = [UIAlertAction actionWithTitle:@"replace mesh" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        NSLog(@"replace mesh");
        [weakSelf replaceMesh:jsonDict uuid:uuid];
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
    NSString *oldMeshUUID = SigDataSource.share.meshUUID;
    [SigDataSource.share setDictionaryToDataSource:dict];
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

    BOOL needChangeProvisionerAddress = NO;//修改手机本地节点的地址
    BOOL reStartSequenceNumber = NO;//修改手机本地节点使用的发包序列号sno
    BOOL hasPhoneUUID = NO;
    NSString *curPhoneUUID = [SigDataSource.share getCurrentProvisionerUUID];
    NSArray *provisioners = [NSArray arrayWithArray:SigDataSource.share.provisioners];
    for (SigProvisionerModel *provision in provisioners) {
        if ([provision.UUID isEqualToString:curPhoneUUID]) {
            hasPhoneUUID = YES;
            break;
        }
    }
    if (hasPhoneUUID) {
        // v3.1.0 存在
        BOOL isSameMesh = [SigDataSource.share.meshUUID isEqualToString:oldMeshUUID];
        if (isSameMesh) {
            // v3.1.0 存在，且为相同mesh网络，覆盖JSON，且使用本地的sno和ProvisionerAddress
            needChangeProvisionerAddress = NO;
            reStartSequenceNumber = NO;
        } else {
            // v3.1.0 存在，但为不同mesh网络，获取provision，修改为新的ProvisionerAddress，sno从0开始
            needChangeProvisionerAddress = YES;
            reStartSequenceNumber = YES;
        }
    } else {
        // v3.1.0 不存在，覆盖并新建provisioner
        needChangeProvisionerAddress = NO;
        reStartSequenceNumber = YES;
    }
    
    //重新计算sno
    if (reStartSequenceNumber) {
        [[NSUserDefaults standardUserDefaults] removeObjectForKey:kCurrentMeshProvisionAddress_key];
        [SigDataSource.share setLocationSno:0];
    }
    
    UInt16 maxAddr = SigDataSource.share.curProvisionerModel.allocatedUnicastRange.firstObject.lowIntAddress;
    NSArray *nodes = [NSArray arrayWithArray:SigDataSource.share.nodes];
    for (SigNodeModel *node in nodes) {
        NSInteger curMax = node.address + node.elements.count - 1;
        if (curMax > maxAddr) {
            maxAddr = curMax;
        }
    }
    if (needChangeProvisionerAddress) {
        //修改手机的本地节点的地址
        UInt16 newProvisionAddress = maxAddr + 1;
        [SigDataSource.share changeLocationProvisionerNodeAddressToAddress:newProvisionAddress];
        TeLogDebug(@"已经使用了address=0x%x作为本地地址",newProvisionAddress);
        //修改下一次添加设备使用的地址
        [SigDataSource.share saveLocationProvisionAddress:maxAddr + 1];
    } else {
        //修改下一次添加设备使用的地址
        [SigDataSource.share saveLocationProvisionAddress:maxAddr];
    }
    TeLogDebug(@"下一次添加设备可以使用的地址address=0x%x",SigDataSource.share.provisionAddress);
    
//    SigDataSource.share.curNetkeyModel = nil;
//    SigDataSource.share.curAppkeyModel = nil;
    [SigDataSource.share checkExistLocationProvisioner];
    [SigDataSource.share saveLocationData];
    [SigDataSource.share.scanList removeAllObjects];
}

- (void)showTips:(NSString *)tips{
    if (!tips) {
        return;
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Hits" message:tips preferredStyle:UIAlertControllerStyleAlert];
        [alertController addAction:[UIAlertAction actionWithTitle:@"Sure" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            TeLogDebug(@"点击确认");
        }]];
        [self presentViewController:alertController animated:YES completion:nil];
    });
}

- (void)startCheckNetwork {
    NSString *remoteHostName = @"www.apple.com";
    Reachability *hostReachability = [Reachability reachabilityWithHostName:remoteHostName];
    if (hostReachability.currentReachabilityStatus == NotReachable) {
        TeLogDebug(@"有网络");
    } else {
        TeLogDebug(@"无网络");
    }
}

-(void)dealloc {
    TeLogDebug(@"");
}

@end
