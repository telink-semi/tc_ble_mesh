/********************************************************************************************************
 * @file     ScanCodeVC.m
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
//  ScanCodeVC.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/11/19.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "ScanCodeVC.h"
#import "ScanView.h"
#import <AssetsLibrary/AssetsLibrary.h>
#import <Photos/Photos.h>
#import "UIViewController+Message.h"

@interface ScanCodeVC ()
@property (nonatomic, strong) ScanView *scanView;
@end

@implementation ScanCodeVC

+ (instancetype)scanCodeVC {
    return [[self alloc] init];
}

- (void)scanDataViewControllerBackBlock:(void (^)(id content))block {
    self.scanCodeVCBlock = block;
}

- (ScanView *)scanView {
    if (!_scanView) {
        __weak typeof(self) weakSelf = self;
        _scanView = [[ScanView alloc] initWithFrame:CGRectMake(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)];
        [self.scanView scanDataViewBackBlock:^(id content) {
            if (weakSelf.scanCodeVCBlock) {
                weakSelf.scanCodeVCBlock(content);
            }else{
                [[NSNotificationCenter defaultCenter] postNotificationName:SuccessScanQRCodeNotification
                                                                    object:weakSelf
                                                                  userInfo:@{ScanQRCodeMessageKey:content}];
            }
        }];
    }
    return _scanView;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    self.title = @"Scan";
    self.view.backgroundColor = [UIColor whiteColor];
    if (self.isCanUseCamera) {
        [self.view addSubview: self.scanView];
    }
}

// 后面的摄像头是否可用
- (BOOL)isCanUseCamera {
    //    iOS 判断应用是否有使用相机的权限
    NSString *mediaType = AVMediaTypeVideo;//读取媒体类型
    AVAuthorizationStatus authStatus = [AVCaptureDevice authorizationStatusForMediaType:mediaType];//读取设备授权状态
    if(authStatus == AVAuthorizationStatusRestricted || authStatus == AVAuthorizationStatusDenied){
//        NSString *errorStr = @"应用相机权限受限,请在设置中启用";
        return NO;
    }
    return YES;
}

- (void)backToMain{
    [self.navigationController popViewControllerAnimated:YES];
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear: animated];
    if (_scanView) {
        [self.scanView start];
    }
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(backToMain) name:@"BackToMain" object:nil];
    if (!self.isCanUseCamera) {
        __weak typeof(self) weakSelf = self;
        dispatch_async(dispatch_get_main_queue(), ^{
            [weakSelf showAlertSureWithTitle:@"Hits" message:@"请到设置界面打开TelinkSigMesh的相机权限。" sure:^(UIAlertAction *action) {
                NSURL *url = [NSURL URLWithString:UIApplicationOpenSettingsURLString];
                if ([[UIApplication sharedApplication] canOpenURL:url]) {
                    [[UIApplication sharedApplication] openURL:url];
                }
            }];
        });
    }
}

- (void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear: animated];
    if (_scanView) {
        [self.scanView stop];
    }
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"BackToMain" object:nil];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

- (void)dealloc
{
    TeLogVerbose(@"");
    if (_scanView) {
        [self.scanView stop];
    }
}

@end
