/********************************************************************************************************
 * @file     ShareInVC.m
 *
 * @brief    for TLSR chips
 *
 * @author     telink
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
//  ShareInVC.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2019/1/24.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import "ShareInVC.h"
#import "FileChooseVC.h"
#import "UIViewController+Message.h"
#import "ScanCodeVC.h"

@interface ShareInVC ()
@property (weak, nonatomic) IBOutlet UIButton *selectJsonButton;
@property (weak, nonatomic) IBOutlet UIButton *selectQRCodeButton;
@property (strong, nonatomic) ScanCodeVC *scanCodeVC;

@end

@implementation ShareInVC

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    self.selectJsonButton.selected = YES;
    self.selectQRCodeButton.selected = NO;
}

- (IBAction)clickSelectJsonFile:(UIButton *)sender {
    self.selectJsonButton.selected = YES;
    self.selectQRCodeButton.selected = NO;

}

- (IBAction)clickSelectQRCode:(UIButton *)sender {
    self.selectJsonButton.selected = NO;
    self.selectQRCodeButton.selected = YES;

}

- (IBAction)clickImportButton:(UIButton *)sender {
    if (self.selectJsonButton.selected) {
        [self importMeshByJsonFile];
    } else if (self.selectQRCodeButton.selected) {
        [self importMeshByQRCode];
    }

}

- (void)importMeshByJsonFile {
    FileChooseVC *vc = (FileChooseVC *)[UIStoryboard initVC:ViewControllerIdentifiers_FileChooseViewControllerID storybroad:@"Setting"];
    __weak typeof(self) weakSelf = self;
    [vc setBackJsonData:^(NSData * _Nonnull jsonData, NSString * _Nonnull jsonName) {
        [SigBearer.share stopMeshConnectWithComplete:^(BOOL successful) {
            TeLogDebug(@"SigBearer close %@",(successful?@"successful":@"fail"));
            [weakSelf loadJsonData:jsonData jaonName:jsonName];
        }];
    }];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)importMeshByQRCode {
    [self.navigationController pushViewController:self.scanCodeVC animated:YES];
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

        [weakSelf importMeshWithDictionary:dict];
    }];
}

- (void)showTips:(NSString *)message{
    __weak typeof(self) weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        [weakSelf showAlertSureWithTitle:@"Hits" message:message sure:^(UIAlertAction *action) {
            
        }];
    });
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
    [SigBearer.share stopMeshConnectWithComplete:^(BOOL successful) {
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
    NSString *oldMeshUUID = SigDataSource.share.meshUUID;
    [SigDataSource.share setDictionaryToDataSource:dict];

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
    
    [SigDataSource.share checkExistLocationProvisioner];
    if (hasPhoneUUID) {
        // v3.1.0, 存在
        BOOL isSameMesh = [SigDataSource.share.meshUUID isEqualToString:oldMeshUUID];
        if (isSameMesh) {
            // v3.1.0, 存在，且为相同mesh网络，覆盖JSON，且使用本地的sno和ProvisionerAddress
            needChangeProvisionerAddress = NO;
            reStartSequenceNumber = NO;
        } else {
            // v3.1.0, 存在，但为不同mesh网络，获取provision，修改为新的ProvisionerAddress，sno从0开始
            needChangeProvisionerAddress = YES;
            reStartSequenceNumber = YES;
        }
    } else {
        // v3.1.0, 不存在，覆盖并新建provisioner
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
        //修改已经使用的设备地址
        [SigDataSource.share saveLocationProvisionAddress:newProvisionAddress];
    } else {
        //修改已经使用的设备地址
        [SigDataSource.share saveLocationProvisionAddress:maxAddr];
    }
    TeLogDebug(@"下一次添加设备可以使用的地址address=0x%x",SigDataSource.share.provisionAddress);
    
//    SigDataSource.share.curNetkeyModel = nil;
//    SigDataSource.share.curAppkeyModel = nil;
    [SigDataSource.share saveLocationData];
    [SigDataSource.share.scanList removeAllObjects];
}

-(void)dealloc{
    TeLogDebug(@"");
}

@end
