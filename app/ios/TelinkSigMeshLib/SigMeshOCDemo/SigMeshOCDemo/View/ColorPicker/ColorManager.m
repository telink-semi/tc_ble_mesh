/********************************************************************************************************
 * @file     ColorManager.m
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
//  ColorManager.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2019/4/3.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import "ColorManager.h"

@implementation HSVModel
- (instancetype)init{
    if (self = [super init]) {
        
    }
    return self;
}

- (void)dealloc {
}
@end


@implementation HSLModel
- (instancetype)init{
    if (self = [super init]) {
        
    }
    return self;
}

- (void)dealloc {
}
@end


@implementation RGBModel
- (instancetype)init{
    if (self = [super init]) {
        
    }
    return self;
}

- (void)dealloc {
}
@end

@implementation ColorManager

+ (HSVModel *)getHSVWithColor:(UIColor *)color{
    HSVModel *tem = [[HSVModel alloc] init];
    CGFloat h=0,s=0,b=0,a=0;
    [color getHue:&h saturation:&s brightness:&b alpha:&a];
    tem.hue = h;
    tem.saturation = s;
    tem.value = b;
    tem.alpha = a;
    return tem;
}

+ (RGBModel *)getRGBWithHSVColor:(HSVModel *)hsv {
    UIColor *color = [UIColor colorWithHue:hsv.hue saturation:hsv.saturation brightness:hsv.value alpha:hsv.alpha];
    return [ColorManager getRGBWithColor:color];
}

+ (UIColor *)getUIColorWithHSVColor:(HSVModel *)hsv {
    return [UIColor colorWithHue:hsv.hue saturation:hsv.saturation brightness:hsv.value alpha:hsv.alpha];
}

//参考转换原理https://blog.csdn.net/jiangxinyu/article/details/8000999
+ (HSLModel *)getHSLWithColor:(UIColor *)color{
    CGFloat r=0,g=0,b=0,a=0;
    [color getRed:&r green:&g blue:&b alpha:&a];
    
    CGFloat max = MAX(r, MAX(g, b));
    CGFloat min = MIN(r, MIN(g, b));
    CGFloat h,s,l = (max + min) / 2;// h ∈ [0, 360）是角度的色相角，而 s, l ∈ [0,1] 是饱和度和亮度
    
    if (max == min) {
        h = 0;
    }else if (max == r) {
        if (g >= b) {
            h = 60 * (g - b)/(max - min);
        } else {
            h = 60 * (g - b)/(max - min) + 360;
        }
    }else if (max == g) {
        h = 60 * (b - r)/(max - min) + 120;
    }else {
        h = 60 * (r - g)/(max - min) + 240;
    }
    
    l = (max + min)/2.0;
    
    if (l == 0 || max == min) {
        s = 0;
    }else if (l > 0 && l <= 1/2.0) {
        s = (max - min) / (2 * l);
    }else {
        s = (max - min) / (2 - 2 * l);
    }
    
    HSLModel *tem = [[HSLModel alloc] init];
    tem.hue = h/360.0;
    tem.saturation = s;
    tem.lightness = l;
    tem.alpha = a;
    return tem;
}

+ (UIColor *)getRGBWithHSLColor:(HSLModel *)hsl{
    CGFloat h,s,l,q,p,Tr,Tg,Tb,COLOR_R,COLOR_G,COLOR_B;
    
    h = hsl.hue;
    s = hsl.saturation;
    l = hsl.lightness;
    
    if (h<0 || h>=1 || s<0 || s>1 || l<0 || l>1) {
        TeLogDebug(@"警告：传入的HSLModel为非法值");
        return [UIColor colorWithRed:0 green:0 blue:0 alpha:1.0];
    }
    if (s == 0) {
        return [UIColor colorWithRed:l green:l blue:l alpha:1.0];
    }
    
    if (l < 0.5) {
        q = l * (1 + s);
    }else{
        q = l + s - (l * s);
    }
    p = 2 * l - q;
    Tr = h + 1/3.0;
    Tg = h;
    Tb = h - 1/3.0;
    
    if (Tr < 0) {
        Tr = Tr + 1.0;
    }else if (Tr > 1) {
        Tr = Tr - 1.0;
    }
    if (Tg < 0) {
        Tg = Tg + 1.0;
    }else if (Tg > 1) {
        Tg = Tg - 1.0;
    }
    if (Tb < 0) {
        Tb = Tb + 1.0;
    }else if (Tb > 1) {
        Tb = Tb - 1.0;
    }
    
    if (Tr < 1/6.0) {
        COLOR_R = p + ((q - p) * 6 * Tr);
    }else if (Tr >= 1/6.0 && Tr < 1/2.0) {
        COLOR_R = q;
    }else if (Tr >= 1/2.0 && Tr < 2/3.0) {
        COLOR_R = p + ((q - p) * 6 * (2/3.0 - Tr));
    }else{
        COLOR_R = p;
    }
    
    if (Tg < 1/6.0) {
        COLOR_G = p + ((q - p) * 6 * Tg);
    }else if (Tg >= 1/6.0 && Tg < 1/2.0) {
        COLOR_G = q;
    }else if (Tg >= 1/2.0 && Tg < 2/3.0) {
        COLOR_G = p + ((q - p) * 6 * (2/3.0 - Tg));
    }else{
        COLOR_G = p;
    }
    
    if (Tb < 1/6.0) {
        COLOR_B = p + ((q - p) * 6 * Tb);
    }else if (Tb >= 1/6.0 && Tb < 1/2.0) {
        COLOR_B = q;
    }else if (Tb >= 1/2.0 && Tb < 2/3.0) {
        COLOR_B = p + ((q - p) * 6 * (2/3.0 - Tb));
    }else{
        COLOR_B = p;
    }
    return [UIColor colorWithRed:COLOR_R green:COLOR_G blue:COLOR_B alpha:1.0];
}

+ (RGBModel *)getRGBWithColor:(UIColor *)color{
    RGBModel *tem = [[RGBModel alloc] init];
    CGFloat r=0,g=0,b=0,a=0;
    [color getRed:&r green:&g blue:&b alpha:&a];
    tem.red = r;
    tem.green = g;
    tem.blud = b;
    tem.alpha = a;
    return tem;
}

@end
