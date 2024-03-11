/********************************************************************************************************
 * @file     SingleDeviceViewController.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/10/10
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#define kControlTitle   @"CONTROL"
#define kGroupTitle   @"GROUP"
#define kSettingsTitle   @"SETTINGS"

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

- (void)configUI {
    self.title = @"Device Setting";
    WMZPageParam *param = PageParam();
    NSArray *titleArray = @[kControlTitle, kGroupTitle, kSettingsTitle];
    // 遥控器不需要分组界面
    if (self.model.isRemote == YES) {
        titleArray = @[kControlTitle, kSettingsTitle];
    }
    __weak typeof(self) weakSelf = self;
    param.wTitleArrSet(titleArray)
    .wViewControllerSet(^UIViewController *(NSInteger index) {
        NSString *title = titleArray[index];
        if ([title isEqualToString:kControlTitle]) {
            // control
            if (weakSelf.model.isRemote) {
                DeviceRemoteVC *wordVc1 = (DeviceRemoteVC *)[UIStoryboard initVC:ViewControllerIdentifiers_DeviceRemoteVCID storyboard:@"DeviceSetting"];
                wordVc1.model = weakSelf.model;
                return wordVc1;
            } else {
                DeviceControlViewController *wordVc1 = (DeviceControlViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_DeviceControlViewControllerID storyboard:@"DeviceSetting"];
                wordVc1.model = weakSelf.model;
                return wordVc1;
            }
        } else if ([title isEqualToString:kGroupTitle]) {
            // group
            if (weakSelf.model.isRemote == NO) {
                DeviceGroupViewController *wordVc2 = (DeviceGroupViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_DeviceGroupViewControllerID storyboard:@"DeviceSetting"];
                wordVc2.model = weakSelf.model;
                return wordVc2;
            }
        }
        // settings
        DeviceSettingViewController *wordVc3 = (DeviceSettingViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_DeviceSettingViewControllerID storyboard:@"DeviceSetting"];
        wordVc3.model = weakSelf.model;
        return wordVc3;
     })
    .wMenuPositionSet(PageMenuPositionCenter)
    .wMenuAnimalTitleGradientSet(NO)
    .wMenuTitleColorSet([UIColor dynamicColorWithLight:[UIColor lightGrayColor] dark:[UIColor darkGrayColor]])
    .wMenuTitleSelectColorSet([UIColor blackColor])
    .wMenuTitleWidthSet(SCREENWIDTH/titleArray.count)
    .wScrollCanTransferSet(NO)
    .wMenuTitleSelectUIFontSet(param.wMenuTitleUIFont)
    .wMenuIndicatorColorSet(UIColor.telinkBlue)
    .wMenuIndicatorWidthSet(SCREENWIDTH/titleArray.count)
    .wCustomTabbarYSet(^CGFloat(CGFloat nowY) {
        return 0;
    })
    .wMenuAnimalSet(PageTitleMenuPDD);
    self.param = param;

    //设置返回按钮文字为空
    self.navigationItem.backBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@"" style:UIBarButtonItemStylePlain target:self action:nil];
}

-(void)dealloc{
    TelinkLogDebug(@"");
}

@end
