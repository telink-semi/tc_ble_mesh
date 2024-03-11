/********************************************************************************************************
 * @file     UIButton+extension.h
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/4/18
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

#import <UIKit/UIKit.h>

typedef void(^ButtonBlock)(UIButton * button);

@interface UIButton (extension)

/**
 *  使用block回调点击事件,默认的touchUpInside
 *
 *  @param block block返回值
 */
- (void)addAction:(ButtonBlock)block;
/**
 *    使用block回调点击事件
 *
 *  @param block   block返回值
 *  @param controlEvent UIControlEvents 自定义点击事件
 */
- (void)addAction:(ButtonBlock)block forControlEvents:(UIControlEvents)controlEvent;

/**
 *  快速创建UIButton
 *
 *  @param frame     按钮的frame
 *  @param image     背景图片
 *  @param textColor 按钮字体颜色
 *  @param title     按钮文字
 *
 *  @return instance
 */
+ (UIButton *)buttonWithFrame:(CGRect)frame image:(UIImage *)image textColor:(UIColor *)textColor title:(NSString *)title;

@end
