/********************************************************************************************************
 * @file     ColorPickerView.h
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2020/11/27
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
