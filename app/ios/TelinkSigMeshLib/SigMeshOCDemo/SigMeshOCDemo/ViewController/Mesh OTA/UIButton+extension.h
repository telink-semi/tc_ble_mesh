/********************************************************************************************************
 * @file     UIButton+extension.h
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
//  UIButton+extension.h
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/4/18.
//  Copyright © 2018年 Telink. All rights reserved.
//

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
