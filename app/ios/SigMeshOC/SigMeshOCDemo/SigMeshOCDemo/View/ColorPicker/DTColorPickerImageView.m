/********************************************************************************************************
 * @file     DTColorPickerImageView.m 
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
//  DTColorPickerImageView.m
//  ColorPicker
//
//  Created by Darktt on 2015/3/19.
//  Copyright (c) 2015年 Darktt. All rights reserved.
//

#import "DTColorPickerImageView.h"
#import "UIImage+ColorPicker.h"

@interface DTColorPickerImageView()
{
    DTColorPickerHandler _handler;
}

@end

@implementation DTColorPickerImageView

+ (instancetype)colorPickerWithFrame:(CGRect)frame
{
    DTColorPickerImageView *colorPicker = [[DTColorPickerImageView alloc] initWithFrame:frame];
    
    return [colorPicker autorelease];
}

+ (instancetype)colorPickerWithImage:(UIImage *)image
{
    DTColorPickerImageView *colorPicker = [[DTColorPickerImageView alloc] initWithImage:image];
    
    return [colorPicker autorelease];
}

#pragma mark - Instance Methods

- (void)awakeFromNib
{
    [super awakeFromNib];
    
    [self setUserInteractionEnabled:YES];
}

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self == nil) return nil;
    
    [self setUserInteractionEnabled:YES];
    return self;
}

- (instancetype)initWithImage:(UIImage *)image
{
    self = [super initWithImage:image];
    if (self == nil) return nil;
    
    [self setUserInteractionEnabled:YES];
    
    return self;
}

- (void)dealloc
{
    if (_handler != nil) {
        Block_release(_handler);
    }
    
    [super dealloc];
}

- (void)handlesDidPickColor:(DTColorPickerHandler)handler
{
    if (_handler != nil) {
        Block_release(_handler);
    }
    
    _handler = Block_copy(handler);
}

#pragma mark - Touch events

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    CGPoint location = [touch locationInView:self];
    if ([self.delegate respondsToSelector:@selector(beganTouchImageView:)]) {
        [self.delegate beganTouchImageView:self];
    }
    [self pickerColorAtPoint:location];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    CGPoint location = [touch locationInView:self];
    
    // Respondes when point in self bounds.
    if (!CGRectContainsPoint(self.bounds, location)) {
        return;
    }
    
    [self pickerColorAtPoint:location];
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event{
    if ([self.delegate respondsToSelector:@selector(endTouchImageView:)]) {
        [self.delegate endTouchImageView:self];
    }
}

- (void)pickerColorAtPoint:(CGPoint)point
{
    CGPoint convertPoint = [self.image convertPoint:point fromImageView:self];
    
    UIColor *color = [self.image pickColorWithPoint:convertPoint];
    
    if (_handler != nil) {
        _handler(color);
        
        return;
    }
    
    //过滤无效的数据
    CGFloat red, green, blue;
    [color getRed:&red green:&green blue:&blue alpha:NULL];
    if (red == 0 && green == 0 && blue == 0) {
        return;
    }
    
    if ([self.delegate respondsToSelector:@selector(imageView:didPickColorWithColor:)]) {
        [self.delegate imageView:self didPickColorWithColor:color];
    }
}

@end
