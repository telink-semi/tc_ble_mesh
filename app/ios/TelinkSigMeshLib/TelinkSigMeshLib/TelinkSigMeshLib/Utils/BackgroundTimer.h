/********************************************************************************************************
* @file     BackgroundTimer.h
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
//  BackgroundTimer.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/11/27.
//  Copyright © 2019 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface BackgroundTimer : NSObject
@property (nonatomic,assign,readonly) NSTimeInterval interval;

/// Chedules a timer that can be started from a background DispatchQueue.
+ (BackgroundTimer * _Nonnull)scheduledTimerWithTimeInterval:(NSTimeInterval)interval repeats:(BOOL)repeats block:(void (^ _Nonnull)(BackgroundTimer * _Nonnull t))block;

/// Asynchronously cancels the dispatch source, preventing any further invocation of its event handler block.
- (void)invalidate;

@end

NS_ASSUME_NONNULL_END
