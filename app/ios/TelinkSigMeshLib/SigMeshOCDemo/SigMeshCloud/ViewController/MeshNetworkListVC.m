/********************************************************************************************************
 * @file     MeshNetworkListVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/2/7
 *
 * @par     Copyright (c) 2023, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "MeshNetworkListVC.h"
#import "CreateNetworkListVC.h"
#import "JoinedNetworkListVC.h"

#define kCreatedTitle   @"CREATED"
#define kJoinedTitle   @"JOINED"

@interface MeshNetworkListVC ()
@property (nonatomic, strong) CreateNetworkListVC *createNetworkListVC;
@property (nonatomic, strong) JoinedNetworkListVC *joinedNetworkListVC;
@end

@implementation MeshNetworkListVC

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    [self configUI];
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
    [self.navigationController setNavigationBarHidden:NO animated:animated];
}

- (void)configUI{
    self.title = @"Network List";
    WMZPageParam *param = PageParam();
    NSArray *titleArray = @[kCreatedTitle, kJoinedTitle];
    __weak typeof(self) weakSelf = self;
    param.wTitleArrSet(titleArray)
    .wViewControllerSet(^UIViewController *(NSInteger index) {
        NSString *title = titleArray[index];
        if ([title isEqualToString:kCreatedTitle]) {
            // create
            weakSelf.createNetworkListVC = (CreateNetworkListVC *)[UIStoryboard initVC:NSStringFromClass(CreateNetworkListVC.class) storyboard:@"Cloud"];
            weakSelf.createNetworkListVC.supperVC = weakSelf;
            weakSelf.createNetworkListVC.title = title;
            return weakSelf.createNetworkListVC;
        }
        // joined
        weakSelf.joinedNetworkListVC = (JoinedNetworkListVC *)[UIStoryboard initVC:NSStringFromClass(JoinedNetworkListVC.class) storyboard:@"Cloud"];
        weakSelf.joinedNetworkListVC.supperVC = weakSelf;
        weakSelf.joinedNetworkListVC.title = title;
        return weakSelf.joinedNetworkListVC;
     })
    //控制器开始切换
    .wEventBeganTransferControllerSet(^(UIViewController *oldVC, UIViewController *newVC, NSInteger oldIndex, NSInteger newIndex) {
        if (newIndex == 0) {
            if (weakSelf.createNetworkListVC.selectBlock) {
                weakSelf.createNetworkListVC.selectBlock();
            }
        } else {
            if (weakSelf.joinedNetworkListVC.selectBlock) {
                weakSelf.joinedNetworkListVC.selectBlock();
            }
        }
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
