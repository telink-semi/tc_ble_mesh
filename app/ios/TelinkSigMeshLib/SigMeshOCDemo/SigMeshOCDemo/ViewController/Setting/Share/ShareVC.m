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
#import "ShareTipsVC.h"
#import "Reachability.h"

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
