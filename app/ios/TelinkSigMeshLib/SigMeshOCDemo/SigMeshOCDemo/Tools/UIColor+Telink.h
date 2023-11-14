/********************************************************************************************************
 * @file     UIColor+Telink.h
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2022/8/4
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

NS_ASSUME_NONNULL_BEGIN

/// 支持0xFF0000/#FF0000/FF0000这三种格式
#define HEXA(COLOR,A) ({ \
    char *color = #COLOR;\
    NSString *colorString = [NSString stringWithUTF8String:color]; \
    colorString = [colorString stringByReplacingOccurrencesOfString:@"#" withString:@""]; \
    colorString = [colorString stringByReplacingOccurrencesOfString:@"0x" withString:@""]; \
    unsigned int red,green,blue; \
    NSRange range; \
    range.length = 2; \
    range.location = 0; \
    [[NSScanner scannerWithString:[colorString substringWithRange:range]] scanHexInt:&red]; \
    range.location = 2; \
    [[NSScanner scannerWithString:[colorString substringWithRange:range]] scanHexInt:&green]; \
    range.location = 4; \
    [[NSScanner scannerWithString:[colorString substringWithRange:range]] scanHexInt:&blue]; \
    [UIColor colorWithRed:red/255.0f green:green/255.0f blue:blue/255.0f alpha:A]; \
})

#define HEX(COLOR) HEXA(COLOR,1.0)

@interface UIColor (Telink)

+ (UIColor *)dynamicColorWithLight:(UIColor *)light dark:(UIColor *)dark;

+ (UIColor *)telinkBlue;

+ (UIColor *)telinkButtonBlue;

+ (UIColor *)telinkButtonUnableBlue;

+ (UIColor *)telinkButtonRed;

+ (UIColor *)telinkTitleBlack;

+ (UIColor *)telinkBackgroundWhite;

+ (UIColor *)telinkBorderColor;

@end

NS_ASSUME_NONNULL_END
