/********************************************************************************************************
 * @file     NSString+calculate.h
 *
 * @brief    for TLSR chips
 *
 * @author       Telink, 梁家誌
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
//  NSString+calculate.h
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2019/3/20.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NSString *_Nonnull(^CalBlock)(NSString *);
typedef NSString *_Nonnull(^CalRoundBlock)(NSString *, NSInteger scale);
typedef NSString *_Nonnull(^RoundBlock)(NSInteger scale);
typedef BOOL (^CompareBlock)(NSString *);

@interface NSString (calculate)

- (NSDecimalNumber *)decimalWrapper;
- (NSDecimal )decimalStruct;

/// >
- (CompareBlock)g;
/// >=
- (CompareBlock)ge;
/// <
- (CompareBlock)l;
/// <=
- (CompareBlock)le;
/// ==
- (CompareBlock)e;
/// !=
- (CompareBlock)ne;

/// +
- (CalBlock)add;
/// -
- (CalBlock)sub;
/// *
- (CalBlock)mul;
/// /
- (CalBlock)div;

/// +    内部自己捕获异常
- (CalRoundBlock)add_r;
/// -    内部自己捕获异常
- (CalRoundBlock)sub_r;
/// *    内部自己捕获异常
- (CalRoundBlock)mul_r;
/// /    内部自己捕获异常
- (CalRoundBlock)div_r;

/// +    异常则抛出
- (CalBlock)add_exc;
/// -    异常则抛出
- (CalBlock)sub_exc;
/// *    异常则抛出
- (CalBlock)mul_exc;
/// /    异常则抛出
- (CalBlock)div_exc;

///四舍五入
- (RoundBlock)roundToPlain;
///四舍五入六去偶
- (RoundBlock)roundToBankers;
///向上舍入
- (RoundBlock)roundToUp;
///向下舍入
- (RoundBlock)roundToDown;


@end

NS_ASSUME_NONNULL_END
