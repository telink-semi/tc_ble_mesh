/********************************************************************************************************
 * @file     BaseViewController.m 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
//
//  BaseViewController.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "BaseViewController.h"
#import "ShowTipsView.h"
#import "UIImage+ColorPicker.h"

@implementation BaseViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [self normalSetting];
    self.ble = Bluetooth.share;
    
}

-(void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    [self blockState];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(isBusy:) name:NotifyCommandIsBusyOrNot object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(responseBack:) name:NotifyReliableReportBack object:nil];
}

- (void)viewWillDisappear:(BOOL)animated{
    [super viewWillDisappear:animated];
    [self nilBlock];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:NotifyCommandIsBusyOrNot object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:NotifyReliableReportBack object:nil];
}

- (void)responseBack:(NSNotification *)notify{
    
}

- (void)isBusy:(NSNotification *)notify{
    NSDictionary *dict = notify.userInfo;
    BOOL isBusy = [dict[CommandIsBusyKey] boolValue];
    if (isBusy) {
        [ShowTipsHandle.share show:Tip_CommandBusy];
        self.view.userInteractionEnabled = NO;
    } else {
        dispatch_async(dispatch_get_main_queue(), ^{
            if (!self.view.isUserInteractionEnabled) {
                [ShowTipsHandle.share hidden];
            }
            self.view.userInteractionEnabled = YES;
        });
    }
}

- (void)normalSetting{
    //解决iOS9下tableview头尾存在一大块空白视图的bug
    self.automaticallyAdjustsScrollViewInsets = NO;
    //导航栏背景颜色
    UINavigationBar *bar = self.navigationController.navigationBar;
    bar.translucent = NO;
    UIImage *bgImage = [UIImage imageWithFrame:CGRectMake(0, 0, [UIScreen mainScreen].bounds.size.width, 64) color:kDefultColor];
    [bar setBackgroundImage:bgImage forBarMetrics:UIBarMetricsDefault];
    //设置返回按钮文字为空
    self.navigationItem.backBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@"" style:UIBarButtonItemStylePlain target:self action:nil];
}

- (void)blockState{
    __weak typeof(self) weakSelf = self;
    self.ble.bleCentralUpdateStateCallBack = ^(CBCentralManagerState state) {
        switch (state) {
            case CBCentralManagerStatePoweredOn:
                [weakSelf.ble startScan];
                break;
            case CBCentralManagerStatePoweredOff:
//                [weakSelf.ble openBluetoothVC];
                break;
            default:
                break;
        }
    };
}

- (void)nilBlock{}

@end
