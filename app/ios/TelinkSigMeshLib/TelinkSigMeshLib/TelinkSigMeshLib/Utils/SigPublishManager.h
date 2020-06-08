/********************************************************************************************************
* @file     SigPublishManager.h
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
//  SigPublishManager.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/12/20.
//  Copyright © 2019 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef void(^DiscoverOutlineNodeBlock)(NSNumber *unicastAddress);
typedef void(^DiscoverOnlineNodeBlock)(NSNumber *unicastAddress);

@interface SigPublishManager : NSObject
@property (nonatomic,copy) DiscoverOutlineNodeBlock discoverOutlineNodeCallback;
@property (nonatomic,copy) DiscoverOnlineNodeBlock discoverOnlineNodeCallback;

+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));


+ (SigPublishManager *)share;

- (void)startCheckOfflineTimerWithAddress:(NSNumber *)address;

- (void)stopCheckOfflineTimerWithAddress:(NSNumber *)address;

@end

NS_ASSUME_NONNULL_END
