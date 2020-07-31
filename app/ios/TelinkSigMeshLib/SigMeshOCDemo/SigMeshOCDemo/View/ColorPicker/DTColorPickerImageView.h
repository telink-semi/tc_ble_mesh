/********************************************************************************************************
 * @file     DTColorPickerImageView.h 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
//
//  DTColorPickerImageView.h
//  ColorPicker
//
//  Created by 梁家誌 on 2015/3/19.
//  Copyright (c) 2015年 Telink. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef void(^DTColorPickerHandler)(UIColor *__nonnull color);

@protocol DTColorPickerImageViewDelegate;

NS_ASSUME_NONNULL_BEGIN
@interface DTColorPickerImageView : UIImageView

@property (assign, nullable) IBOutlet id<DTColorPickerImageViewDelegate> delegate;

+ (instancetype)colorPickerWithFrame:(CGRect)frame;
+ (instancetype)colorPickerWithImage:(nullable UIImage *)image;

// When handler and delegate not nil, will handler respondes first.
- (void)handlesDidPickColor:(DTColorPickerHandler)handler;

@end

@protocol DTColorPickerImageViewDelegate <NSObject>

@optional
- (void)imageView:(DTColorPickerImageView *)imageView didPickColorWithColor:(UIColor *)color;
- (void)beganTouchImageView:(DTColorPickerImageView *)imageView;
- (void)endTouchImageView:(DTColorPickerImageView *)imageView;

@end
NS_ASSUME_NONNULL_END
