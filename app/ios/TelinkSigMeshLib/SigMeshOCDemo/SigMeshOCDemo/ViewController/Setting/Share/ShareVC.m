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
        *norColor = [UIColor dynamicColorWithLight:[UIColor lightGrayColor] dark:[UIColor darkGrayColor]];
        *selColor = [UIColor blackColor];
        *titleScrollViewColor = [UIColor dynamicColorWithLight:[UIColor whiteColor] dark:[UIColor grayColor]];
        *titleWidth = [UIScreen mainScreen].bounds.size.width / 2;
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
    ShareOutVC *wordVc1 = (ShareOutVC *)[UIStoryboard initVC:ViewControllerIdentifiers_ShareOutViewControllerID storyboard:@"Setting"];
    wordVc1.title = @"EXPORT";
    [self addChildViewController:wordVc1];
    
    // ShareInVC
    ShareInVC *wordVc2 = (ShareInVC *)[UIStoryboard initVC:ViewControllerIdentifiers_ShareInViewControllerID storyboard:@"Setting"];
    wordVc2.title = @"IMPORT";
    [self addChildViewController:wordVc2];
}

- (void)clickTopRight:(UIButton *)button {
    ShareTipsVC *vc = (ShareTipsVC *)[UIStoryboard initVC:ViewControllerIdentifiers_ShareTipsVCID storyboard:@"Setting"];
    vc.title = @"Share Tips";
    vc.tipsMessage = @"Export JSON:\n\n1. Click EXPORT button to create a new json file.\n2. Iphone connect to computer that install iTunes.\n3. Click on the iTunes phone icon in the upper left corner of iTunes into the iphone interface.\n4. Select \"file sharing\" in the left of the iTunes, then find and click on the demo APP in the application of \"TelinkSigMesh\", wait for iTunes load file.\n5. After file is loaded, found file \"mesh-.json\"in the \"TelinkSigMesh\".You just must to drag the file to your computer.\n\n\nImport JSON:\n\n1. Iphone connect to computer that install iTunes.\n2. Click on the iTunes phone icon in the upper left corner of iTunes into the iphone interface.\n3. Select \"file sharing\" in the left of the iTunes, then find and click on the demo APP in the application of \"TelinkSigMesh\", wait for iTunes load file.\n4. After file is loaded, drag the files on the computer \"mesh.json\" into the right side of the \"TelinkSigMesh\", replace the old file and reopen the APP, the APP will load json data file automatically.\n5. Click IMPORT button to choose new json file and load it.\n\n\n\n导出JSON数据操作，步骤如下：\n\n1. 点击APP的EXPORT按钮，生成新的json文件。\n2. 将手机连接到安装了iTunes的电脑上。\n3. 点击iTunes左上角的手机图标进入iTunes设备详情界面。\n4. 选择iTunes左侧的“文件共享”，然后在应用中找到并点击demo APP “TelinkSigMesh”，等待iTunes加载文件。\n5. 文件加载完成后，在“TelinkSigMesh”的文稿中找到mesh的分享数据文件“mesh-.json”，把该文件拖到电脑即可。\n\n\n导入JSON数据操作，步骤如下：\n\n1. 将手机连接到安装了iTunes的电脑上。\n2. 点击iTunes左上角的手机图标进入iTunes设备详情界面。\n3. 选择iTunes左侧的“文件共享”，然后在应用中找到并点击demo APP “TelinkSigMesh”，等待iTunes加载文件。\n4. 文件加载完成后，将电脑上的json文件拖入右侧的“TelinkSigMesh”的文稿中。\n5. APP点击IMPORT按钮选择刚刚的JSON文件进行加载。";
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)startCheckNetwork {
    NSString *remoteHostName = @"www.apple.com";
    Reachability *hostReachability = [Reachability reachabilityWithHostName:remoteHostName];
    if (hostReachability.currentReachabilityStatus == NotReachable) {
        TeLogDebug(@"无网络");
    } else {
        TeLogDebug(@"有网络");
    }
}

-(void)dealloc {
    TeLogDebug(@"");
}

@end
