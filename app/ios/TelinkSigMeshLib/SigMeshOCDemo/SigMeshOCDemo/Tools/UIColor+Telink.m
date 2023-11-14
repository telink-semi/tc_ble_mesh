/********************************************************************************************************
 * @file     UIColor+Telink.m
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

#import "UIColor+Telink.h"

@implementation UIColor (Telink)

+ (UIColor *)dynamicColorWithLight:(UIColor *)light dark:(UIColor *)dark {
    if (@available(iOS 13.0, *)) {
        return [UIColor colorWithDynamicProvider:^UIColor * _Nonnull(UITraitCollection * _Nonnull traitCollection) {
                if (traitCollection.userInterfaceStyle == UIUserInterfaceStyleDark) {
                    //深色模式下的颜色
                    return dark;
                }else {
                    //浅色模式下的颜色
                    return light;
                }
            }];
    } else {
        return light;
    }
}

+ (UIColor *)telinkBlue {
    if (@available(iOS 11.0, *)) {
        return [UIColor colorNamed:@"telinkBlue"];
    } else {
        return HEX(#4A87EE);
    }
}

+ (UIColor *)telinkButtonBlue {
    if (@available(iOS 11.0, *)) {
        return [UIColor colorNamed:@"telinkButtonBlue"];
    } else {
        return HEX(#4A87EE);
    }
}

+ (UIColor *)telinkButtonUnableBlue {
    if (@available(iOS 11.0, *)) {
        return [UIColor colorNamed:@"telinkButtonUnableBlue"];
    } else {
        return HEXA(#4A87EE,0.5);
    }
}

+ (UIColor *)telinkButtonRed {
    if (@available(iOS 11.0, *)) {
        return [UIColor colorNamed:@"telinkButtonRed"];
    } else {
        return HEX(#F00002);
    }
}

+ (UIColor *)telinkTitleBlack {
    if (@available(iOS 11.0, *)) {
        return [UIColor colorNamed:@"telinkTitleBlack"];
    } else {
        return HEX(#000000);
    }
}

+ (UIColor *)telinkBackgroundWhite {
    if (@available(iOS 11.0, *)) {
        return [UIColor colorNamed:@"telinkBackgroundWhite"];
    } else {
        return HEX(#FFFFFF);
    }
}

+ (UIColor *)telinkBorderColor {
    if (@available(iOS 11.0, *)) {
        return [UIColor colorNamed:@"telinkBorderColor"];
    } else {
        return HEX(#CBCBCB);
    }
}

@end
