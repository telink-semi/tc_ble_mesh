/********************************************************************************************************
 * @file     BaseViewController.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/7/31
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

#import "BaseViewController.h"
#import "ShowTipsView.h"
#import "NetworkListVC.h"
#import "UIImage+Extension.h"
#import "UIViewController+Message.h"
#import "LoginVC.h"

@implementation BaseViewController

#pragma mark Private

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
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
        app.titleTextAttributes = @{NSFontAttributeName : [UIFont boldSystemFontOfSize:17.0f], NSForegroundColorAttributeName : [UIColor whiteColor]};
        // 当可滚动内容的边缘与导航栏的边缘对齐时，导航栏的外观设置。
        self.navigationController.navigationBar.scrollEdgeAppearance = app;
        // 标准高度导航条的外观设置(常规设置)
        self.navigationController.navigationBar.standardAppearance = app;
        // 应用于导航栏背景的色调。
        self.navigationController.navigationBar.barTintColor = UIColor.telinkBlue;
        // 应用于导航栏按钮项的着色颜色。
        self.navigationController.navigationBar.tintColor = [UIColor whiteColor];
    }
}

#pragma mark Public

- (void)normalSetting {
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
    } else {
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

/// Set block
- (void)blockState{}

/// Clean block
- (void)nilBlock{}

/// Show alert with message
/// - Parameters:
///   - title: title string
///   - tips: message string
- (void)showTitle:(NSString *)title tips:(NSString *)tips {
    [self showAlertSureWithTitle:title message:tips sure:nil];
}

/// Show alert with message
/// - Parameter message: message
- (void)showTips:(NSString *)message {
    [self showAlertSureWithTitle:kDefaultAlertTitle message:message sure:nil];
}

/// Show alert with message and sure handle block
/// - Parameters:
///   - message: message
///   - sure: sure handle block
- (void)showTips:(NSString *)message sure:(void (^) (UIAlertAction *action))sure {
    [self showAlertSureWithTitle:kDefaultAlertTitle message:message sure:sure];
}

/// Show alert with message and sure handle block
/// - Parameters:
///   - message: message
///   - sure: sure handle block
- (void)showAlertTitle:(NSString *)title message:(NSString *)message sure:(void (^) (UIAlertAction *action))sure {
    [self showAlertSureWithTitle:title message:message sure:sure];
}

/// 设置导航栏标题和副标题，为了同时显示Title和Mesh名称在导航栏而新增的方法。
/// - Parameters:
///   - title: 导航栏标题
///   - subTitle: 导航栏副标题
- (void)setTitle:(NSString *)title subTitle:(NSString *)subTitle {
    //创建一个Label
    UILabel *titleView = [[UILabel alloc] init];
    //显示宽高
    titleView.frame = CGRectMake(0, 0, self.view.frame.size.width - 100, 44);
    //设置文字居中显示
    titleView.textAlignment = NSTextAlignmentCenter;
    //设置titleLabel自动换行
    titleView.numberOfLines = 0;
    //设置文本颜色
    titleView.textColor = [UIColor whiteColor];
    //获取标题的字符串
    NSString *str = [NSString stringWithFormat:@"%@\n%@", title, subTitle];
    //创建一个带有属性的字符串比如说颜色，字体等文字的属性
    NSMutableAttributedString *attrStr = [[NSMutableAttributedString alloc] initWithString:str];
    //设置title的字体大小
    [attrStr addAttribute:NSFontAttributeName value:[UIFont boldSystemFontOfSize:16] range:[str rangeOfString:title]];
    //设置subTitle的字体大小
    [attrStr addAttribute:NSFontAttributeName value:[UIFont systemFontOfSize:13] range:[str rangeOfString:subTitle]];
    //设置有属性的text
    titleView.attributedText = attrStr;
    //设置导航栏的titleView
    self.navigationItem.titleView = titleView;
}

/// Pop To ViewController With Class
/// - Parameter vcClass: Class of ViewController
- (BOOL)popToViewControllerWithClass:(Class)vcClass {
    BOOL has = NO;
    for (UIViewController *controller in self.navigationController.viewControllers) {
        if ([controller isKindOfClass:vcClass]) {
            has = YES;
            [self.navigationController popToViewController:controller animated:YES];
            break;
        }
    }
    return has;
}

#pragma mark 离线版本 SigMeshOCDemo 使用的接口

/// 导入mesh后，将Mesh添加到Mesh列表或者更新Mesh信息。
/// - Parameter dict: 新导入的Mesh数据
- (void)addOrUpdateMeshDictionaryToMeshList:(NSDictionary *)dict {
    NSArray *meshList = [[NSUserDefaults standardUserDefaults] valueForKey:kCacheMeshListKey];
    NSMutableArray *mArray = [NSMutableArray arrayWithArray:meshList];
    BOOL update = NO;
    for (int i=0; i<meshList.count; i++) {
        NSDictionary *d = [LibTools getDictionaryWithJSONData:meshList[i]];
        if ([[d[@"meshUUID"] uppercaseString] isEqualToString:[dict[@"meshUUID"] uppercaseString]]) {
            [mArray replaceObjectAtIndex:i withObject:[LibTools getJSONDataWithDictionary:dict]];
            update = YES;
            break;
        }
    }
    if (update == NO) {
        [mArray addObject:[LibTools getJSONDataWithDictionary:dict]];
    }
    [[NSUserDefaults standardUserDefaults] setValue:mArray forKey:kCacheMeshListKey];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

/// 接收到分享来的Mesh数据的统一处理接口。（文件分享、二维码分享、CDTP分享都使用这个方法）
/// - Parameter dict: 新导入的Mesh数据
- (void)handleMeshDictionaryFromShareImport:(NSDictionary *)dict {
    __weak typeof(self) weakSelf = self;
    //提示是否导入Mesh
    [self showAlertSureAndCancelWithTitle:kDefaultAlertTitle message:@"Mesh JSON receive complete, import data?" sure:^(UIAlertAction *action) {
        [weakSelf addOrUpdateMeshDictionaryToMeshList:dict];
        NSNumber *importCompleteAction = [[NSUserDefaults standardUserDefaults] valueForKey:kImportCompleteAction];
        if (importCompleteAction.intValue == ImportSwitchMode_manual) {
            //弹框提示用户选择是否切换Mesh
            [weakSelf showAlertSureAndCancelWithTitle:kDefaultAlertTitle message:@"Share import success, switch to the new mesh?" sure:^(UIAlertAction *action) {
                [weakSelf switchMeshActionWithMeshDictionary:dict];
            } cancel:nil];
        } else {
            //自动切换Mesh
            [weakSelf switchMeshActionWithMeshDictionary:dict];
        }
    } cancel:nil];
}

/// 切换到其中一份Mesh数据
/// - Parameter dict: 将要使用的Mesh数据
- (void)switchMeshActionWithMeshDictionary:(NSDictionary *)dict {
    [SigDataSource.share switchToNewMeshDictionary:dict];
    [SDKLibCommand stopMeshConnectWithComplete:nil];
    [[NSUserDefaults standardUserDefaults] setValue:SigDataSource.share.meshUUID forKey:kCacheCurrentMeshUUIDKey];
    [[NSUserDefaults standardUserDefaults] setValue:@(0) forKey:kDistributorAddress];
    [[NSUserDefaults standardUserDefaults] removeObjectForKey:kDistributorPolicy];
    [[NSUserDefaults standardUserDefaults] removeObjectForKey:kUpdateNodeAddresses];
    [[NSUserDefaults standardUserDefaults] removeObjectForKey:kDistributorBinString];
    [[NSUserDefaults standardUserDefaults] synchronize];
    dispatch_async(dispatch_get_main_queue(), ^{
        BOOL has = [self popToViewControllerWithClass:[NetworkListVC class]];
        if (!has) {
            [self.navigationController popViewControllerAnimated:YES];
        }
    });
}

#ifdef kIsTelinkCloudSigMeshLib

#pragma mark 云端版本 SigMeshCloud 使用的接口

/// Add a circular button in the bottom right corner.
/// - Parameters:
///   - imageString: string of image
///   - action: action of UIControlEventTouchUpInside
- (void)addCustomCircularButtonWithImageString:(NSString *_Nonnull)imageString action:(ButtonBlock _Nonnull )action {
    /// 除了第一个分页，其它分页的UINavigationController为nil。
    UINavigationController *navigationController = [[UINavigationController alloc] init];
    /// 获取导航栏+状态栏的高度
    CGFloat height = navigationController.navigationBar.frame.size.height + [[UIApplication sharedApplication] statusBarFrame].size.height;
    _circularButton = [[CustomCircularButton alloc] initWithFrame:CGRectMake(self.view.bounds.size.width-30-60, self.view.bounds.size.height-30-60-height-44, 60, 60)];
    [_circularButton.customButton setBackgroundImage:[UIImage imageNamed:imageString] forState:UIControlStateNormal];
    [_circularButton.customButton addAction:action];
    [self.view addSubview:_circularButton];
}

/// Add no data background view.
- (void)addNoDataUI {
    UIView *backgroundView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 60, 90)];
    [self.view addSubview:backgroundView];
    [self.view sendSubviewToBack:backgroundView];
    backgroundView.center = CGPointMake(self.view.center.x, self.view.center.y*0.8);
    UIImageView *imageView = [[UIImageView alloc] initWithFrame:CGRectMake(0, 0, 60, 60)];
    [imageView setImage:[UIImage imageNamed:@"ic_exclamationPoint_blue"]];
    [backgroundView addSubview:imageView];
    UILabel *label = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, self.view.frame.size.width, 20)];
    label.font = [UIFont boldSystemFontOfSize:18];
    label.textColor = [UIColor telinkLabelBlue];
    label.text = @"No Data";
    label.textAlignment = NSTextAlignmentCenter;
    [backgroundView addSubview:label];
    label.center = CGPointMake(imageView.center.x, imageView.center.y+50);
}

/// Push to LoginVC with animated
/// - Parameter animated: animated
- (void)pushToLogInVCWithAnimated:(BOOL)animated {
    LoginVC *vc = (LoginVC *)[UIStoryboard initVC:@"LoginVC" storyboard:@"Cloud"];
    [self.navigationController pushViewController:vc animated:animated];
}

/// Push to NetworkListVC
/// - Parameter hidden: Identify whether to hide the return button
- (void)pushToNetworkListVCWithHiddenBackButton:(BOOL)hidden {
    UIViewController *vc = [UIStoryboard initVC:@"MeshNetworkListVC" storyboard:@"Cloud"];
    vc.navigationItem.hidesBackButton = hidden;
    [self.navigationController pushViewController:vc animated:YES];
}

/// Set BaseTabbarController to RootViewController
- (void)setBaseTabbarControllerToRootViewController {
    dispatch_async(dispatch_get_main_queue(), ^{
        //注释的是重新登录后停留UserVC的写法
//        BOOL result = [self popToViewControllerWithClass:UserVC.class];
//        if (!result) {
        //下面是重新登录后显示首页HomeVC的写法
            UITabBarController *tabBarController = (UITabBarController *)[UIStoryboard initVC:@"BaseTabBarController" storyboard:@"CloudMain"];
            UIWindow *keyWindow = [self getKeyWindow];
            keyWindow.rootViewController = tabBarController;
            [keyWindow makeKeyAndVisible];
//        }
    });
}

/// Save networkId in local.
/// - Parameters:
///   - networkId: network ID in cloud
///   - type: network type
- (void)saveLocalWithCurrentNetworkId:(NSInteger)networkId type:(NSInteger)type {
    [NSUserDefaults.standardUserDefaults setObject:@{@"userId":@(AppDataSource.share.userInfo.userId), @"networkId":@(networkId), @"type":@(type)} forKey:@"kLocalNetworkId"];
    [NSUserDefaults.standardUserDefaults synchronize];
}

/// Clean networkId in local.
- (void)cleanLocalNetworkId {
    [NSUserDefaults.standardUserDefaults removeObjectForKey:@"kLocalNetworkId"];
    [NSUserDefaults.standardUserDefaults synchronize];
}

/// Get networkId in local.
- (NSNumber * _Nullable)getLocalNetworkId {
    NSNumber *number = nil;
    NSDictionary *dict = [[NSUserDefaults standardUserDefaults] valueForKey:@"kLocalNetworkId"];
    if (dict != nil) {
        if ([dict[@"userId"] intValue] == AppDataSource.share.userInfo.userId) {
            number = dict[@"networkId"];
        }
    }
    return number;
}

/// Get network type in local.
- (NSNumber * _Nullable)getLocalNetworkType {
    NSNumber *number = nil;
    NSDictionary *dict = [[NSUserDefaults standardUserDefaults] valueForKey:@"kLocalNetworkId"];
    if (dict != nil) {
        if ([dict[@"userId"] intValue] == AppDataSource.share.userInfo.userId) {
            number = dict[@"type"];
        }
    }
    return number;
}

#pragma mark apply address handle

- (UInt16)applyNodeAddressWithTCVendorID:(NSInteger)tcVendorID pid:(NSInteger)pid elementCount:(NSInteger)elementCount {
    __block UInt16 tem = 0;
    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
    [AppDataSource.share applyNodeAddressWithTCVendorID:tcVendorID pid:pid elementCount:elementCount resultBlock:^(UInt16 address, NSError * _Nullable error) {
        if (error == nil) {
            tem = address;
        }
        dispatch_semaphore_signal(semaphore);
    }];
    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
    return tem;
}

#pragma mark provision success handle

- (void)addNodeAfterProvisionSuccessWithNodeAddress:(UInt16)nodeAddress {
    [AppDataSource.share addNodeAfterProvisionSuccessWithNodeAddress:nodeAddress resultBlock:^(NSError * _Nullable error) {
        TelinkLogInfo(@"error=%@", error);
    }];
}

#pragma mark keyBind success handle

- (void)updateKeyBindSuccessToCloudWithUnicastAddress:(UInt16)unicastAddress {
    [AppDataSource.share updateNodeBindStateWithBindState:1 address:unicastAddress];
}

#pragma mark release address handle

- (void)releaseNodeAddressWithNodeAddress:(UInt16)nodeAddress elementCount:(NSInteger)elementCount tcVendorId:(NSInteger)tcVendorId {
    if (nodeAddress != 0) {
        [AppDataSource.share releaseNodeAddressWithNodeAddress:nodeAddress elementCount:elementCount tcVendorId:tcVendorId resultBlock:^(NSError * _Nullable error) {
            TelinkLogInfo(@"error=%@", error);
        }];
    }
}

/// Get KeyWindow
- (UIWindow *)getKeyWindow {
    UIWindow *keyWindow = nil;
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= 130000 // 编译时检查SDK版本（兼容不同版本的Xcode，防止编译报错）
    if (@available(iOS 13.0, *)) { // 运行时检查系统版本（兼容不同版本的系统，防止运行报错）
        NSSet<UIScene *> *connectedScenes = [UIApplication sharedApplication].connectedScenes;
        for (UIScene *scene in connectedScenes) {
            if (scene.activationState == UISceneActivationStateForegroundActive && [scene isKindOfClass:[UIWindowScene class]]) {
                UIWindowScene *windowScene = (UIWindowScene *)scene;
                for (UIWindow *window in windowScene.windows) {
                    if (window.isKeyWindow) {
                        keyWindow = window;
                        break;
                    }
                }
            }
        }
    }
#endif
        
    if (!keyWindow) {
        keyWindow = [UIApplication sharedApplication].windows.firstObject;
        if (!keyWindow.isKeyWindow) {
#if __IPHONE_OS_VERSION_MIN_REQUIRED < 130000
            UIWindow *window = [UIApplication sharedApplication].keyWindow;
            if (CGRectEqualToRect(window.bounds, UIScreen.mainScreen.bounds)) {
                keyWindow = window;
            }
#endif
        }
    }
    
    return keyWindow;
}

#endif

@end
