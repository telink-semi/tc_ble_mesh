/********************************************************************************************************
 * @file     SigDataHandler.h
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
//  SigDataHandler.h
//  SigMeshLib
//
//  Created by Liangjiazhi on 2019/8/19.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SigBearer.h"

NS_ASSUME_NONNULL_BEGIN

@class SigPdu;

@interface SigDataHandler : NSObject

+ (SigDataHandler *)share;

- (void)receiveBleData:(NSData *)data;

- (void)receiveOnlineStatueData:(NSData *)data;

@end

NS_ASSUME_NONNULL_END