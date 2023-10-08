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
#import "UIViewController+Message.h"

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

- (void)showTips:(NSString *)message {
    [self showTips:message sure:nil];
}

- (void)showTips:(NSString *)message sure:(void (^) (UIAlertAction *action))sure {
    __weak typeof(self) weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        [weakSelf showAlertSureWithTitle:@"Hits" message:message sure:sure];
    });
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
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    //这里是弃用的属性
    //解决iOS9下tableview头尾存在一大块空白视图的bug
        self.automaticallyAdjustsScrollViewInsets=YES;
#pragma clang diagnostic pop
    //设置导航栏不透明
    self.navigationController.navigationBar.translucent = NO;
    //设置导航条背景颜色和Title颜色
    UIImage *bgImage = [UIImage createImageWithColor:UIColor.telinkBlue];
    if (@available(iOS 13.0, *)) {
        UINavigationBarAppearance *appearance = [[UINavigationBarAppearance alloc] init];
        [appearance configureWithOpaqueBackground];
        appearance.backgroundImage = bgImage;
        appearance.titleTextAttributes = @{NSForegroundColorAttributeName:[UIColor whiteColor]};
        self.navigationController.navigationBar.standardAppearance = appearance;
        self.navigationController.navigationBar.scrollEdgeAppearance = appearance;
    }else{
        self.navigationController.navigationBar.titleTextAttributes = @{NSForegroundColorAttributeName:[UIColor whiteColor]};
    }
    [self.navigationController.navigationBar setBackgroundImage:bgImage forBarMetrics:UIBarMetricsDefault];
    //设置TabBar的颜色
    for (UITabBarItem *tabBarItem in self.tabBarController.tabBar.items) {
        [tabBarItem setTitleTextAttributes:@{NSForegroundColorAttributeName:HEX(#7D7D7D)} forState:UIControlStateNormal];
        [tabBarItem setTitleTextAttributes:@{NSForegroundColorAttributeName:HEX(#4A87EE)} forState:UIControlStateSelected];
    }
    //设置返回按钮文字为空
    self.navigationItem.backBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@"" style:UIBarButtonItemStylePlain target:self action:nil];
    
    // 配置iOS13工具条
    if (@available(iOS 13.0, *)) {
        UITabBarAppearance *appearance = self.tabBarController.tabBar.standardAppearance.copy;
        appearance.backgroundImage = [UIImage createImageWithColor:[UIColor colorNamed:@"telinkTabBarBackgroundColor"]];
        appearance.backgroundColor = [UIColor colorNamed:@"telinkTabBarBackgroundColor"];
        appearance.shadowImage = [UIImage createImageWithColor:[UIColor colorNamed:@"telinkTabBarshadowImageColor"]];
        appearance.shadowColor = [UIColor colorNamed:@"telinkTabBarshadowImageColor"];
        self.tabBarController.tabBar.standardAppearance = appearance;
    }

    [self configTabBarForiOS15];
    [self configNavigationBarForiOS15];
}

// 配置iOS15工具条
- (void)configTabBarForiOS15 {
    if (@available(iOS 15.0, *)) {
        UITabBarAppearance *bar = [[UITabBarAppearance alloc] init];
        bar.backgroundColor = [UIColor colorNamed:@"telinkTabBarBackgroundColor"];
        bar.shadowImage = [UIImage createImageWithColor:[UIColor colorNamed:@"telinkTabBarshadowImageColor"]];
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
        app.backgroundColor = UIColor.telinkBlue;
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
        self.navigationController.navigationBar.barTintColor = UIColor.telinkBlue;
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
