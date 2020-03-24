/********************************************************************************************************
 * @file     NSString+calculate.m
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
//  NSString+calculate.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2019/3/20.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import "NSString+calculate.h"

@implementation NSString (calculate)

- (NSDecimalNumber *)decimalWrapper{
    return [NSDecimalNumber decimalNumberWithString:self];
}

- (NSDecimal )decimalStruct{
    return self.decimalWrapper.decimalValue;
}

/// >
- (CompareBlock)g{
    return ^BOOL (NSString *other) {
        return [self.decimalWrapper compare:other.decimalWrapper] == NSOrderedDescending;
    };
}

/// >=
- (CompareBlock)ge{
    return ^BOOL (NSString *other) {
        NSComparisonResult rel = [self.decimalWrapper compare:other.decimalWrapper];
        return rel == NSOrderedDescending || rel == NSOrderedSame;
    };
}

/// <
- (CompareBlock)l{
    return ^BOOL (NSString *other) {
        return [self.decimalWrapper compare:other.decimalWrapper] == NSOrderedAscending;
    };
}

/// <=
- (CompareBlock)le{
    return ^BOOL (NSString *other) {
        NSComparisonResult rel = [self.decimalWrapper compare:other.decimalWrapper];
        return rel == NSOrderedAscending || rel == NSOrderedSame;
    };
}

/// ==
- (CompareBlock)e{
    return ^BOOL (NSString *other) {
        return [self.decimalWrapper compare:other.decimalWrapper] == NSOrderedSame;
    };
}

/// !=
- (CompareBlock)ne{
    return ^BOOL (NSString *other) {
        return [self.decimalWrapper compare:other.decimalWrapper] != NSOrderedSame;
    };
}


/// +
- (CalBlock)add{
    return ^NSString *(NSString *other) {
        NSString *rel = nil;
        @try {
            rel = [self.decimalWrapper decimalNumberByAdding:other.decimalWrapper].stringValue;
            return rel;
        } @catch (NSException *exception) {
            return @"0";
        }
    };
}

/// -
- (CalBlock)sub{
    return ^NSString *(NSString *other) {
        NSString *rel = nil;
        @try {
            rel = [self.decimalWrapper decimalNumberBySubtracting:other.decimalWrapper].stringValue;
            return rel;
        } @catch (NSException *exception) {
            return @"0";
        }
    };
}

/// *
- (CalBlock)mul{
    return ^NSString *(NSString *other) {
        NSString *rel = nil;
        @try {
            rel = [self.decimalWrapper decimalNumberByMultiplyingBy:other.decimalWrapper].stringValue;
            return rel;
        } @catch (NSException *exception) {
            return @"0";
        }
    };
}

/// /
- (CalBlock)div{
    return ^NSString *(NSString *other) {
        NSString *rel = nil;
        @try {
            rel = [self.decimalWrapper decimalNumberByDividingBy:other.decimalWrapper].stringValue;
            return rel;
        } @catch (NSException *exception) {
            return @"0";
        }
    };
}


/// +    内部自己捕获异常
- (CalRoundBlock)add_r{
    return ^NSString *(NSString *other, NSInteger scale) {
        NSString *rel = nil;
        @try {
            rel = [self.decimalWrapper decimalNumberByAdding:other.decimalWrapper].stringValue;
            return rel.roundToPlain(scale);
        } @catch (NSException *exception) {
            return @"0";
        }
    };
}

/// -    内部自己捕获异常
- (CalRoundBlock)sub_r{
    return ^NSString *(NSString *other, NSInteger scale) {
        NSString *rel = nil;
        @try {
            rel = [self.decimalWrapper decimalNumberBySubtracting:other.decimalWrapper].stringValue;
            return rel.roundToPlain(scale);
        } @catch (NSException *exception) {
            return @"0";
        }
    };
}

/// *    内部自己捕获异常
- (CalRoundBlock)mul_r{
    return ^NSString *(NSString *other, NSInteger scale) {
        NSString *rel = nil;
        @try {
            rel = [self.decimalWrapper decimalNumberByMultiplyingBy:other.decimalWrapper].stringValue;
            return rel.roundToPlain(scale);
        } @catch (NSException *exception) {
            return @"0";
        }
    };
}

/// /    内部自己捕获异常
- (CalRoundBlock)div_r{
    return ^NSString *(NSString *other, NSInteger scale) {
        NSString *rel = nil;
        @try {
            rel = [self.decimalWrapper decimalNumberByDividingBy:other.decimalWrapper].stringValue;
            return rel.roundToPlain(scale);
        } @catch (NSException *exception) {
            return @"0";
        }
    };

}


/// +    异常则抛出
- (CalBlock)add_exc{
    return ^NSString *(NSString *other) {
        NSString *rel = nil;
        @try {
            rel = [self.decimalWrapper decimalNumberByAdding:other.decimalWrapper].stringValue;
            return rel;
        } @catch (NSException *exception) {
            @throw exception;
        }
    };
}

/// -    异常则抛出
- (CalBlock)sub_exc{
    return ^NSString *(NSString *other) {
        NSString *rel = nil;
        @try {
            rel = [self.decimalWrapper decimalNumberBySubtracting:other.decimalWrapper].stringValue;
            return rel;
        } @catch (NSException *exception) {
            @throw exception;
        }
    };
}

/// *    异常则抛出
- (CalBlock)mul_exc{
    return ^NSString *(NSString *other) {
        NSString *rel = nil;
        @try {
            rel = [self.decimalWrapper decimalNumberByMultiplyingBy:other.decimalWrapper].stringValue;
            return rel;
        } @catch (NSException *exception) {
            @throw exception;
        }
    };
}

/// /    异常则抛出
- (CalBlock)div_exc{
    return ^NSString *(NSString *other) {
        NSString *rel = nil;
        @try {
            rel = [self.decimalWrapper decimalNumberByDividingBy:other.decimalWrapper].stringValue;
            return rel;
        } @catch (NSException *exception) {
            @throw exception;
        }
    };
}


///四舍五入
- (RoundBlock)roundToPlain{
    return ^NSString *(NSInteger scale) {
        NSDecimal result;
        NSDecimal origin = self.decimalStruct;
        NSDecimalRound(&result, &origin, scale, NSRoundPlain);
        return [NSDecimalNumber decimalNumberWithDecimal:result].stringValue;
    };
}

///四舍五入六去偶
- (RoundBlock)roundToBankers{
    return ^NSString *(NSInteger scale) {
        NSDecimal result;
        NSDecimal origin = self.decimalStruct;
        NSDecimalRound(&result, &origin, scale, NSRoundBankers);
        return [NSDecimalNumber decimalNumberWithDecimal:result].stringValue;
    };
}

///向上舍入
- (RoundBlock)roundToUp{
    return ^NSString *(NSInteger scale) {
        NSDecimal result;
        NSDecimal origin = self.decimalStruct;
        NSDecimalRound(&result, &origin, scale, NSRoundUp);
        return [NSDecimalNumber decimalNumberWithDecimal:result].stringValue;
    };
}

///向下舍入
- (RoundBlock)roundToDown{
    return ^NSString *(NSInteger scale) {
        NSDecimal result;
        NSDecimal origin = self.decimalStruct;
        NSDecimalRound(&result, &origin, scale, NSRoundDown);
        return [NSDecimalNumber decimalNumberWithDecimal:result].stringValue;
    };
}

@end
