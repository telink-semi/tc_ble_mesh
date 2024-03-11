/********************************************************************************************************
 * @file     BaseViewController.h
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

#import <UIKit/UIKit.h>
#ifdef kIsTelinkCloudSigMeshLib
#import "UIButton+extension.h"
#import "CustomCircularButton.h"
#endif

@interface BaseViewController : UIViewController

#pragma mark Public

/// Default UI setting in BaseViewController
- (void)normalSetting;

/// Set block
- (void)blockState;

/// Clean block
- (void)nilBlock;

/// Show alert with message
/// - Parameter message: message
- (void)showTips:(NSString *)message;

/// Show alert with message and sure handle block
/// - Parameters:
///   - message: message
///   - sure: sure handle block
- (void)showTips:(NSString *)message sure:(void (^) (UIAlertAction *action))sure;

/// 设置导航栏标题和副标题，为了同时显示Title和Mesh名称在导航栏而新增的方法。
/// - Parameters:
///   - title: 导航栏标题
///   - subTitle: 导航栏副标题
- (void)setTitle:(NSString *)title subTitle:(NSString *)subTitle;

/// Pop To ViewController With Class
/// - Parameter vcClass: Class of ViewController
- (BOOL)popToViewControllerWithClass:(Class)vcClass;

#pragma mark 离线版本 SigMeshOCDemo 使用的接口

/// 导入mesh后，将Mesh添加到Mesh列表或者更新Mesh信息。
/// - Parameter dict: 新导入的Mesh数据
- (void)addOrUpdateMeshDictionaryToMeshList:(NSDictionary *)dict;

/// 接收到分享来的Mesh数据的统一处理接口。（文件分享、二维码分享、CDTP分享都使用这个方法）
/// - Parameter dict: 新导入的Mesh数据
- (void)handleMeshDictionaryFromShareImport:(NSDictionary *)dict;

/// 切换到其中一份Mesh数据
/// - Parameter dict: 将要使用的Mesh数据
- (void)switchMeshActionWithMeshDictionary:(NSDictionary *)dict;

#ifdef kIsTelinkCloudSigMeshLib

#pragma mark 云端版本 SigMeshCloud 使用的接口

/// UI右下角悬浮的圆形紫色自定义按钮，当前有添加网络和扫描二维码两个地方需要用到。
@property (nonatomic, strong) CustomCircularButton *circularButton;

/// Add a circular button in the bottom right corner.
/// - Parameters:
///   - imageString: string of image
///   - action: action of UIControlEventTouchUpInside
- (void)addCustomCircularButtonWithImageString:(NSString *_Nonnull)imageString action:(ButtonBlock _Nonnull )action;

/// Add no data background view.
- (void)addNoDataUI;

/// Push to LoginVC with animated
/// - Parameter animated: animated
- (void)pushToLogInVCWithAnimated:(BOOL)animated;

/// Push to NetworkListVC
/// - Parameter hidden: Identify whether to hide the return button
- (void)pushToNetworkListVCWithHiddenBackButton:(BOOL)hidden;

/// Set BaseTabbarController to RootViewController
- (void)setBaseTabbarControllerToRootViewController;

/// Save networkId in local.
/// - Parameters:
///   - networkId: network ID in cloud
///   - type: network type
- (void)saveLocalWithCurrentNetworkId:(NSInteger)networkId type:(NSInteger)type;

/// Clean networkId in local.
- (void)cleanLocalNetworkId;

/// Get networkId in local.
- (NSNumber * _Nullable)getLocalNetworkId;

/// Get network type in local.
- (NSNumber * _Nullable)getLocalNetworkType;

#pragma mark apply address handle

- (UInt16)applyNodeAddressWithTCVendorID:(NSInteger)tcVendorID pid:(NSInteger)pid elementCount:(NSInteger)elementCount;

#pragma mark provision success handle

- (void)addNodeAfterProvisionSuccessWithNodeAddress:(UInt16)nodeAddress;

#pragma mark keyBind success handle

- (void)updateKeyBindSuccessToCloudWithUnicastAddress:(UInt16)unicastAddress;

#pragma mark release address handle

- (void)releaseNodeAddressWithNodeAddress:(UInt16)nodeAddress elementCount:(NSInteger)elementCount tcVendorId:(NSInteger)tcVendorId;

#endif

@end
