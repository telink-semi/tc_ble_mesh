/********************************************************************************************************
 * @file     SingleDeviceViewController.m 
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/10/10
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

#import "SingleDeviceViewController.h"
#import "DeviceControlViewController.h"
#import "DeviceGroupViewController.h"
#import "DeviceSettingViewController.h"
#import "DeviceRemoteVC.h"

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
        *norColor = [UIColor dynamicColorWithLight:[UIColor lightGrayColor] dark:[UIColor darkGrayColor]];
        *selColor = [UIColor blackColor];
        *titleScrollViewColor = [UIColor dynamicColorWithLight:[UIColor whiteColor] dark:[UIColor grayColor]];
        *titleWidth = [UIScreen mainScreen].bounds.size.width / ((weakSelf.model.isRemote || SigDataSource.share.curMeshIsVisitor) ? 2 : 3);
    }];
    
    // 标题渐变
    // *推荐方式(设置标题渐变)
    [self setUpTitleGradient:^(YZTitleColorGradientStyle *titleColorGradientStyle, UIColor *__autoreleasing *norColor, UIColor *__autoreleasing *selColor) {
        
    }];
    
    [self setUpUnderLineEffect:^(BOOL *isUnderLineDelayScroll, CGFloat *underLineH, UIColor *__autoreleasing *underLineColor,BOOL *isUnderLineEqualTitleWidth) {
        *underLineColor = UIColor.telinkButtonBlue;
        *isUnderLineEqualTitleWidth = YES;
    }];

    //设置返回按钮文字为空
    self.navigationItem.backBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@"" style:UIBarButtonItemStylePlain target:self action:nil];

}

- (void)setUpAllViewController
{
    // control
    if (self.model.isRemote) {
        DeviceRemoteVC *wordVc1 = (DeviceRemoteVC *)[UIStoryboard initVC:ViewControllerIdentifiers_DeviceRemoteVCID storyboard:@"DeviceSetting"];
        wordVc1.title = @"CONTROL";
        wordVc1.model = self.model;
        [self addChildViewController:wordVc1];
    } else {
        DeviceControlViewController *wordVc1 = (DeviceControlViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_DeviceControlViewControllerID storyboard:@"DeviceSetting"];
        wordVc1.title = @"CONTROL";
        wordVc1.model = self.model;
        [self addChildViewController:wordVc1];
    }
    
    // group
    // 遥控器不需要分组界面
    if (self.model.isRemote == NO && SigDataSource.share.curMeshIsVisitor == NO) {
        DeviceGroupViewController *wordVc2 = (DeviceGroupViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_DeviceGroupViewControllerID storyboard:@"DeviceSetting"];
        wordVc2.title = @"GROUP";
        wordVc2.model = self.model;
        [self addChildViewController:wordVc2];
    }
    
    // settings
    DeviceSettingViewController *wordVc3 = (DeviceSettingViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_DeviceSettingViewControllerID storyboard:@"DeviceSetting"];
    wordVc3.title = @"SETTINGS";
    wordVc3.model = self.model;
    [self addChildViewController:wordVc3];
}

-(void)dealloc{
    TeLogDebug(@"");
}

@end
