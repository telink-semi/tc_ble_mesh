/********************************************************************************************************
 * @file     NSString+calculate.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/3/20
 *
 * @par     Copyright (c) [2021], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
