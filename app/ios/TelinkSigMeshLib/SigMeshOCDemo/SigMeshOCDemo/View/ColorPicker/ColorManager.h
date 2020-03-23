/********************************************************************************************************
 * @file     ColorManager.h
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
//  ColorManager.h
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2019/4/3.
//  Copyright © 2019年 Telink. All rights reserved.
//

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
