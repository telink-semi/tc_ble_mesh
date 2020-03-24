/********************************************************************************************************
 * @file     SingleDeviceViewController.m 
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
//  SingleDeviceViewController.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2018/10/10.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "SingleDeviceViewController.h"
#import "DeviceControlViewController.h"
#import "DeviceGroupViewController.h"
#import "DeviceSettingViewController.h"

@interface SingleDeviceViewController ()

@end

@implementation SingleDeviceViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    [self configUI];
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    // 禁用返回手势
    if ([self.navigationController respondsToSelector:@selector(interactivePopGestureRecognizer)]) {
        self.navigationController.interactivePopGestureRecognizer.enabled = NO;
    }
}

- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    // 开启返回手势
    if ([self.navigationController respondsToSelector:@selector(interactivePopGestureRecognizer)]) {
        self.navigationController.interactivePopGestureRecognizer.enabled = YES;
    }
}

- (void)configUI{
    self.title = @"Device Setting";
    
    // add sub vc
    [self setUpAllViewController];
    
    __weak typeof(self) weakSelf = self;
    double h = kGetRectNavAndStatusHight;
    [self setUpContentViewFrame:^(UIView *contentView) {
        contentView.frame = CGRectMake(0, 0, weakSelf.view.frame.size.width, weakSelf.view.frame.size.height-h);
    }];
    
    [self setUpTitleEffect:^(UIColor *__autoreleasing *titleScrollViewColor, UIColor *__autoreleasing *norColor, UIColor *__autoreleasing *selColor, UIFont *__autoreleasing *titleFont, CGFloat *titleHeight, CGFloat *titleWidth) {
        *norColor = [UIColor lightGrayColor];
        *selColor = [UIColor blackColor];
        *titleWidth = [UIScreen mainScreen].bounds.size.width / 3;
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

}

- (void)setUpAllViewController
{
    // control
    DeviceControlViewController *wordVc1 = (DeviceControlViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_DeviceControlViewControllerID storybroad:@"DeviceSetting"];
    wordVc1.title = @"CONTROL";
    wordVc1.model = self.model;
    [self addChildViewController:wordVc1];
    
    // group
    DeviceGroupViewController *wordVc2 = (DeviceGroupViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_DeviceGroupViewControllerID storybroad:@"DeviceSetting"];
    wordVc2.title = @"GROUP";
    wordVc2.model = self.model;
    [self addChildViewController:wordVc2];
    
    // settings
    DeviceSettingViewController *wordVc3 = (DeviceSettingViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_DeviceSettingViewControllerID storybroad:@"DeviceSetting"];
    wordVc3.title = @"SETTINGS";
    wordVc3.model = self.model;
    [self addChildViewController:wordVc3];
}

-(void)dealloc{
    TeLog(@"");
}

@end
