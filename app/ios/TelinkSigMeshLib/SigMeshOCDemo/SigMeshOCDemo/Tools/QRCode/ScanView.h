/********************************************************************************************************
 * @file     ScanView.h
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/11/19
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
#import "ScanCodeVC.h"

NS_ASSUME_NONNULL_BEGIN

/*! 扫描成功发送通知（在代理实现的情况下不发送）*/
extern NSString * const SuccessScanQRCodeNotification;
/*! 通知传递数据中存储二维码信息的关键字*/
extern NSString * const ScanQRCodeMessageKey;

@interface ScanView : UIView

@property (nonatomic, copy) void(^scanDataBlock)(id content);

- (void)scanDataViewBackBlock:(void (^)(id content))block;
/*! 开始扫描*/
- (void)start;
/*! 结束扫描*/
- (void)stop;

@end

NS_ASSUME_NONNULL_END
