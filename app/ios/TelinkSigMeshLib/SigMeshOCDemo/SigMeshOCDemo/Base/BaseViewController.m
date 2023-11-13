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
#import "NetworkListVC.h"

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

/// 设置导航栏标题和副标题，为了显示Mesh名称在导航栏而新增的方法。
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
    [attrStr addAttribute:NSFontAttributeName value:[UIFont systemFontOfSize:16] range:[str rangeOfString:title]];
    //设置subTitle的字体大小
    [attrStr addAttribute:NSFontAttributeName value:[UIFont systemFontOfSize:13] range:[str rangeOfString:subTitle]];
    //设置title的颜色
//    [attrStr addAttribute:NSForegroundColorAttributeName value:[UIColor whiteColor] range:[str rangeOfString:title]];
    //设置有属性的text
    titleView.attributedText = attrStr;
    //设置导航栏的titleView
    self.navigationItem.titleView = titleView;
}

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

/// import mesh
- (void)handleMeshDictionaryFromShareImport:(NSDictionary *)dict {
    __weak typeof(self) weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        //提示是否导入Mesh
        UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"Warning" message:@"Mesh JSON receive complete, import data?" preferredStyle:UIAlertControllerStyleAlert];
        [alert addAction:[UIAlertAction actionWithTitle:@"Confirm" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            TelinkLogDebug(@"点击确认");
            [weakSelf addOrUpdateMeshDictionaryToMeshList:dict];
            NSNumber *importCompleteAction = [[NSUserDefaults standardUserDefaults] valueForKey:kImportCompleteAction];
            if (importCompleteAction.intValue == ImportSwitchMode_manual) {
                //弹框提示用户选择是否切换Mesh
                __weak typeof(self) weakSelf = self;
                UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Warning" message:@"Share import success, switch to the new mesh?" preferredStyle:UIAlertControllerStyleAlert];
                [alertController addAction:[UIAlertAction actionWithTitle:@"Confirm" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
                    TelinkLogDebug(@"点击确认");
                    [weakSelf switchMeshActionWithMeshDictionary:dict];
                }]];
                [alertController addAction:[UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
                    TelinkLogDebug(@"点击取消");
                    
                }]];
                [weakSelf presentViewController:alertController animated:YES completion:nil];
            } else {
                //自动切换Mesh
                [weakSelf switchMeshActionWithMeshDictionary:dict];
            }
        }]];
        [alert addAction:[UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
            TelinkLogDebug(@"点击取消");
            
        }]];
        [weakSelf presentViewController:alert animated:YES completion:nil];
    });
}

/// switch mesh
- (void)switchMeshActionWithMeshDictionary:(NSDictionary *)dict {
    [SigDataSource.share switchToNewMeshDictionary:dict];
    [SDKLibCommand stopMeshConnectWithComplete:nil];
    [[NSUserDefaults standardUserDefaults] setValue:SigDataSource.share.meshUUID forKey:kCacheCurrentMeshUUIDKey];
    [[NSUserDefaults standardUserDefaults] setValue:@(0) forKey:kDistributorAddress];
    [[NSUserDefaults standardUserDefaults] removeObjectForKey:kDistributorPolicy];
    [[NSUserDefaults standardUserDefaults] removeObjectForKey:kUpdateNodeAddresses];
    [[NSUserDefaults standardUserDefaults] synchronize];
    dispatch_async(dispatch_get_main_queue(), ^{
        [self popVC:[NetworkListVC class]];
    });
}

/// pop specific VC
- (void)popVC:(Class)vcClass {
    BOOL has = NO;
    for (UIViewController *controller in self.navigationController.viewControllers) {
        if ([controller isKindOfClass:vcClass]) {
            has = YES;
            [self.navigationController popToViewController:controller animated:YES];
            break;
        }
    }
    if (!has) {
        [self.navigationController popViewControllerAnimated:YES];
    }
}

@end
