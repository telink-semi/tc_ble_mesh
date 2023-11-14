/********************************************************************************************************
 * @file     ColorManager.h
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/4/3
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

/*参考资料地址 https://blog.csdn.net/jiangxinyu/article/details/8000999 */
@interface HSVModel : NSObject
@property (nonatomic, assign) CGFloat hue;//0.0-1.0
@property (nonatomic, assign) CGFloat saturation;//0.0-1.0
@property (nonatomic, assign) CGFloat value;//0.0-1.0
@property (nonatomic, assign) CGFloat alpha;
@end

@interface HSLModel : NSObject
@property (nonatomic, assign) CGFloat hue;
@property (nonatomic, assign) CGFloat saturation;
@property (nonatomic, assign) CGFloat lightness;
@property (nonatomic, assign) CGFloat alpha;
@end

@interface RGBModel : NSObject
@property (nonatomic, assign) CGFloat red;
@property (nonatomic, assign) CGFloat green;
@property (nonatomic, assign) CGFloat blud;
@property (nonatomic, assign) CGFloat alpha;
@end


@interface ColorManager : NSObject

+ (HSVModel *)getHSVWithColor:(UIColor *)color;
+ (RGBModel *)getRGBWithHSVColor:(HSVModel *)hsv;
+ (UIColor *)getUIColorWithHSVColor:(HSVModel *)hsv;
+ (HSLModel *)getHSLWithColor:(UIColor *)color;
+ (RGBModel *)getRGBWithColor:(UIColor *)color;
+ (UIColor *)getRGBWithHSLColor:(HSLModel *)hsl;

@end

NS_ASSUME_NONNULL_END
