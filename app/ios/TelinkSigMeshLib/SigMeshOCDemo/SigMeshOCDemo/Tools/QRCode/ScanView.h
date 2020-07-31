/********************************************************************************************************
 * @file     ScanView.h
 *
 * @brief    for TLSR chips
 *
 * @author     telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *             The information contained herein is confidential and proprietary property of Telink
 *              Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *             of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *             Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 *              Licensees are granted free, non-transferable use of the information in this
 *             file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/
//
//  ScanView.h
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/11/19.
//  Copyright © 2018年 Telink. All rights reserved.
//

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
