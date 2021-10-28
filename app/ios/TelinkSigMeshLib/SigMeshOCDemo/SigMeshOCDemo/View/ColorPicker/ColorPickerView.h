/********************************************************************************************************
 * @file     ColorPickerView.h
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
//  ColorPickerView.h
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2020/11/27.
//  Copyright © 2020 Telink. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

typedef void(^ColorPickerHandler)(UIColor *__nonnull color);

@protocol ColorPickerViewDelegate;

@interface ColorPickerView : UIControl
@property (nonatomic, assign) CGFloat hue;//[0.0,1.0)
@property (nonatomic, assign) CGFloat saturation;//[0.0,1.0]
@property (nonatomic, assign) CGFloat brightness;//[0.0,1.0]

@property (assign, nullable) IBOutlet id<ColorPickerViewDelegate> delegate;

// When handler and delegate not nil, will handler respondes first.
- (void)handlesDidPickColor:(ColorPickerHandler)handler;

@end


@protocol ColorPickerViewDelegate <NSObject>

@optional
- (void)colorPickerView:(ColorPickerView *)colorPickerView didPickColorWithColor:(UIColor *)color hue:(CGFloat)hue saturation:(CGFloat)saturation brightness:(CGFloat)brightness;
- (void)beganTouchColorPickerView:(ColorPickerView *)colorPickerView;
- (void)endTouchColorPickerView:(ColorPickerView *)colorPickerView;

@end

NS_ASSUME_NONNULL_END
