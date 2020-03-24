/********************************************************************************************************
 * @file     ShareVC.m
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
//  ShareVC.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2019/1/24.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import "ShareVC.h"
#import "ShareOutVC.h"
#import "ShareInVC.h"

@interface ShareVC ()

@end

@implementation ShareVC

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self configUI];
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)configUI{
    self.title = @"Share by iTunes";
    
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

-(void)dealloc{
    TeLogDebug(@"");
}


@end
