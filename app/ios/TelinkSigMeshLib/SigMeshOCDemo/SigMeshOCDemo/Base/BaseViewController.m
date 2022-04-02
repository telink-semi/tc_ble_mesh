/********************************************************************************************************
 * @file     BaseViewController.m 
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/7/31
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

#import "BaseViewController.h"
#import "ShowTipsView.h"
#import "UIImage+Extension.h"

@implementation BaseViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [self normalSetting];
    
}

-(void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    [self blockState];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(isBusy:) name:kNotifyCommandIsBusyOrNot object:nil];
}

- (void)viewWillDisappear:(BOOL)animated{
    [super viewWillDisappear:animated];
    [self nilBlock];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:kNotifyCommandIsBusyOrNot object:nil];
}

- (void)isBusy:(NSNotification *)notify{
    NSDictionary *dict = notify.userInfo;
    BOOL isBusy = [dict[kCommandIsBusyKey] boolValue];
    dispatch_async(dispatch_get_main_queue(), ^{
        if (isBusy) {
            [ShowTipsHandle.share show:Tip_CommandBusy];
            self.view.userInteractionEnabled = NO;
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(busyTimeout) object:nil];
            [self performSelector:@selector(busyTimeout) withObject:nil afterDelay:10.0];
        } else {
            if (!self.view.isUserInteractionEnabled) {
                [ShowTipsHandle.share hidden];
            }
            self.view.userInteractionEnabled = YES;
        }
    });
}

- (void)busyTimeout {
    dispatch_async(dispatch_get_main_queue(), ^{
        if (!self.view.isUserInteractionEnabled) {
            [ShowTipsHandle.share hidden];
        }
        self.view.userInteractionEnabled = YES;
    });
}

- (void)normalSetting{
    //解决iOS9下tableview头尾存在一大块空白视图的bug
    self.automaticallyAdjustsScrollViewInsets = NO;
    //导航栏背景颜色
    UINavigationBar *bar = self.navigationController.navigationBar;
    bar.translucent = NO;
    UIImage *bgImage = [UIImage createImageWithColor:kDefultColor];
    [bar setBackgroundImage:bgImage forBarMetrics:UIBarMetricsDefault];
    //设置返回按钮文字为空
    self.navigationItem.backBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@"" style:UIBarButtonItemStylePlain target:self action:nil];
    
    [self configTabBarForiOS15];
    [self configNavigationBarForiOS15];
}

// 配置iOS15工具条
- (void)configTabBarForiOS15 {
    if (@available(iOS 15.0, *)) {
        UITabBarAppearance *bar = [[UITabBarAppearance alloc] init];
        bar.backgroundColor = [UIColor colorWithRed:247/255.0 green:247/255.0 blue:247/255.0 alpha:1.0];
        bar.shadowImage = [UIImage createImageWithColor:[UIColor colorWithRed:178/255.0 green:178/255.0 blue:178/255.0 alpha:1.0]];
        self.tabBarController.tabBar.scrollEdgeAppearance = bar;
        self.tabBarController.tabBar.standardAppearance = bar;
    }
}

// 配置iOS15导航条
- (void)configNavigationBarForiOS15 {
    if (@available(iOS 15.0, *)) {
        UINavigationBarAppearance *app = [[UINavigationBarAppearance alloc] init];
        // 不透明背景色
        [app configureWithOpaqueBackground];
        // 设置背景色
        app.backgroundColor = kDefultColor;
        // 磨砂效果
        app.backgroundEffect = [UIBlurEffect effectWithStyle:UIBlurEffectStyleSystemMaterial];
        // 导航条底部分割线图片（这里设置为透明）
        UIImage *image = [UIImage createImageWithColor:[UIColor clearColor]];
        app.shadowImage = image;
        // 导航条富文本设置
//        app.titleTextAttributes = @{NSFontAttributeName : [UIFont boldSystemFontOfSize:17.0f], NSForegroundColorAttributeName : self.barTintColor};
        app.titleTextAttributes = @{NSFontAttributeName : [UIFont boldSystemFontOfSize:17.0f], NSForegroundColorAttributeName : [UIColor whiteColor]};
        // 当可滚动内容的边缘与导航栏的边缘对齐时，导航栏的外观设置。
        self.navigationController.navigationBar.scrollEdgeAppearance = app;
        // 标准高度导航条的外观设置(常规设置)
        self.navigationController.navigationBar.standardAppearance = app;
        // 应用于导航栏背景的色调。
//        self.navigationController.navigationBar.barTintColor = self.barBackgroundColor;
        self.navigationController.navigationBar.barTintColor = kDefultColor;
        // 应用于导航栏按钮项的着色颜色。
//        self.navigationController.navigationBar.tintColor = self.barTintColor;
        self.navigationController.navigationBar.tintColor = [UIColor whiteColor];
    }
}

- (void)blockState{
//    __weak typeof(self) weakSelf = self;
//    self.ble.bleCentralUpdateStateCallBack = ^(CBCentralManagerState state) {
//        switch (state) {
//            case CBCentralManagerStatePoweredOn:
//                [weakSelf.ble startScan];
//                break;
//            case CBCentralManagerStatePoweredOff:
////                [weakSelf.ble openBluetoothVC];
//                break;
//            default:
//                break;
//        }
//    };
}

- (void)nilBlock{}

@end
