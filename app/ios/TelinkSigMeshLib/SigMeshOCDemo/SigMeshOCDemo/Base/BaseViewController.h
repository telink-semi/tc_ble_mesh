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
@class Bluetooth;

@interface BaseViewController : UIViewController

- (void)showTips:(NSString *)message;

- (void)showTips:(NSString *)message sure:(void (^) (UIAlertAction *action))sure;

- (void)isBusy:(NSNotification *)notify;

- (void)normalSetting;

- (void)blockState;

- (void)nilBlock;


/// 设置导航栏标题和副标题，为了显示Mesh名称在导航栏而新增的方法。
/// - Parameters:
///   - title: 导航栏标题
///   - subTitle: 导航栏副标题
- (void)setTitle:(NSString *)title subTitle:(NSString *)subTitle;

/// 导入mesh后，将Mesh添加到Mesh列表或者更新Mesh信息。
/// - Parameter dict: 新导入的Mesh数据
- (void)addOrUpdateMeshDictionaryToMeshList:(NSDictionary *)dict;

/// import mesh
- (void)handleMeshDictionaryFromShareImport:(NSDictionary *)dict;

/// switch mesh
- (void)switchMeshActionWithMeshDictionary:(NSDictionary *)dict;

/// pop specific VC
- (void)popVC:(Class)vcClass;

@end
