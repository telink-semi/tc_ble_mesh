/********************************************************************************************************
 * @file     NSString+calculate.h
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/3/20
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
