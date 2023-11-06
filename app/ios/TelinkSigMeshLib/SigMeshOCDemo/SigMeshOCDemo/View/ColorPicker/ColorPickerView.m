/********************************************************************************************************
 * @file     ColorPickerView.m
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

#import "ColorPickerView.h"
#import <CoreGraphics/CoreGraphics.h>
#import "ColorManager.h"

@interface ColorPickerView ()
@property (nonatomic, assign) BOOL isTouched;//NO
@property (nonatomic, assign) CGImageRef wheelImage;//NO
@property (nonatomic, strong) CALayer *indicatorLayer;
@property (nonatomic,copy) ColorPickerHandler handler;
@end

@implementation ColorPickerView

+ (BOOL)requiresConstraintBasedLayout {
    return YES;
}

- (instancetype)initWithFrame:(CGRect)frame {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super initWithFrame:frame]) {
        /// Initialize self.
        [self initUI];
    }
    return self;
}

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        [self initUI];
    }
    return self;
}

- (instancetype)initWithCoder:(NSCoder *)coder {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super initWithCoder:coder]) {
        /// Initialize self.
        [self initUI];
    }
    return self;
}

- (void)initUI {
    self.isTouched = NO;
    self.hue = 0;
    self.saturation = 0;
    self.brightness = 1.0;
    self.accessibilityLabel = @"color_wheel_view";
    self.layer.delegate = self;
    self.indicatorLayer.hidden = YES;
    [self.layer addSublayer:self.indicatorLayer];
}

- (void)setHue:(CGFloat)hue {
    _hue = hue;
    [self setSelectedPoint:[self colorPickerViewSelectedPoint]];
    [self setNeedsDisplay];
}

- (void)setSaturation:(CGFloat)saturation {
    _saturation = saturation;
    [self setSelectedPoint:[self colorPickerViewSelectedPoint]];
    [self setNeedsDisplay];
}

- (void)setBrightness:(CGFloat)brightness {
    BOOL changed = brightness != _brightness;
    _brightness = brightness;
    [self setSelectedPoint:[self colorPickerViewSelectedPoint]];
    if (changed) {
        [self drawWheelImage];
    }
}

- (void)handlesDidPickColor:(ColorPickerHandler)handler {
    self.handler = handler;
}

- (CALayer *)indicatorLayer {
    if (!_indicatorLayer) {
        CGFloat dimension = 33;
        UIColor *edgeColor = [UIColor colorWithWhite:0.9 alpha:0.8];
        CALayer *indicatorLayer = [[CALayer alloc] init];
        indicatorLayer.cornerRadius = dimension / 2;
        indicatorLayer.borderColor = edgeColor.CGColor;
        indicatorLayer.borderWidth = 2;
        indicatorLayer.backgroundColor = UIColor.whiteColor.CGColor;
        indicatorLayer.bounds = CGRectMake(0, 0, dimension, dimension);
        indicatorLayer.position = CGPointMake(self.bounds.size.width / 2, self.bounds.size.height / 2);
        indicatorLayer.shadowColor = UIColor.blackColor.CGColor;
        indicatorLayer.shadowOffset = CGSizeZero;
        indicatorLayer.shadowRadius = 1;
        indicatorLayer.shadowOpacity = 0.5;
        _indicatorLayer = indicatorLayer;
    }
    return _indicatorLayer;
}

// MARK: - UITouch delegate methods

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    UITouch *touch = [touches anyObject];
    CGPoint location = [touch locationInView:self];
    self.isTouched = YES;
    [self onTouchEventWithPoint:location];
    if ([self.delegate respondsToSelector:@selector(beganTouchColorPickerView:)]) {
        [self.delegate beganTouchColorPickerView:self];
    }
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    UITouch *touch = [touches anyObject];
    CGPoint location = [touch locationInView:self];
    // Respondes when point in self bounds.
    if (!CGRectContainsPoint(self.bounds, location)) {
        return;
    }
    [self onTouchEventWithPoint:location];
    if ([self.delegate respondsToSelector:@selector(beganTouchColorPickerView:)]) {
        [self.delegate beganTouchColorPickerView:self];
    }
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    UITouch *touch = [touches anyObject];
    CGPoint location = [touch locationInView:self];
    self.isTouched = NO;
    [self onTouchEventWithPoint:location];
    if ([self.delegate respondsToSelector:@selector(endTouchColorPickerView:)]) {
        [self.delegate endTouchColorPickerView:self];
    }
}

- (void)onTouchEventWithPoint:(CGPoint)point {
    CGFloat radius = self.bounds.size.width / 2;
    double mx = radius - point.x;
    double my = radius - point.y;
    CGFloat dist = sqrt(mx * mx + my * my);
    if (dist <= radius) {
        CGFloat h = self.hue;
        CGFloat s = self.saturation;
        [self setColorWheelValueWithPosition:point hue:&h saturation:&s];
        self.hue = h;
        self.saturation = s;
        [self setSelectedPoint:point];
        if ([self.delegate respondsToSelector:@selector(colorPickerView:didPickColorWithColor:hue:saturation:brightness:)]) {
            UIColor *selectedColor = [UIColor colorWithHue:self.hue saturation:self.saturation brightness:self.brightness alpha:1];
            [self.delegate colorPickerView:self didPickColorWithColor:selectedColor hue:self.hue saturation:self.saturation brightness:self.brightness];
        }
    }
}

- (void)setSelectedPoint:(CGPoint)point {
    UIColor *selectedColor = [UIColor colorWithHue:self.hue saturation:self.saturation brightness:self.brightness alpha:1];
    [CATransaction begin];
//    [CATransaction setValue:(id)kCFBooleanTrue forKey:kCATransactionDisableActions];
    self.indicatorLayer.position = point;
    self.indicatorLayer.backgroundColor = selectedColor.CGColor;
    [CATransaction commit];
}

// MARK: - CALayerDelegate methods

- (void)displayLayer:(CALayer *)layer {
    if (self.wheelImage == nil) {
        [self drawWheelImage];
    }
}

- (void)layoutSublayersOfLayer:(CALayer *)layer {
    if (layer == self.layer) {
        [self setSelectedPoint:[self colorPickerViewSelectedPoint]];
        [self.layer setNeedsDisplay];
    }
}

// MARK: - Private methods

- (void)drawWheelImage {
    CGFloat dimension = MIN(self.frame.size.width, self.frame.size.height);
    CFMutableDataRef bitmapData = CFDataCreateMutable(NULL, 0);
    if (!bitmapData) {
        return;
    }
    CFDataSetLength(bitmapData, dimension * dimension * 4);
    [self setColorWheelBitmap:CFDataGetMutableBytePtr(bitmapData) withSize:CGSizeMake(dimension, dimension)];
    CGDataProviderRef dataProvider = CGDataProviderCreateWithCFData(bitmapData);
    if (dataProvider != nil) {
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        CGImageRef imageRef = CGImageCreate(dimension, dimension, 8, 32, dimension * 4, colorSpace, (CGBitmapInfo)kCGImageAlphaLast, dataProvider, NULL, 0, kCGRenderingIntentDefault);
        CGColorSpaceRelease(colorSpace);
        if (imageRef) {
            self.wheelImage = imageRef;
            self.layer.contents = (__bridge id _Nullable)(self.wheelImage);
            CGImageRelease(imageRef);
        }
    }
    CFRelease(bitmapData);
    CGDataProviderRelease(dataProvider);
}

- (CGPoint)colorPickerViewSelectedPoint {
    CGFloat dimension = MIN(self.frame.size.width, self.frame.size.height);
    double radius = self.saturation * dimension / 2;
    double x = dimension / 2 + radius * cos(self.hue * M_PI * 2.0);
    double y = dimension / 2 + radius * sin(self.hue * M_PI * 2.0);
    return CGPointMake(x, y);
}

- (void)setColorWheelBitmap:(UInt8 *)bitmap withSize:(CGSize)size {
    if (size.width <= 0 || size.height <= 0) {
        return;
    }
    for (int y = 0; y < size.width; y ++) {
        for (int x = 0; x < size.height; x ++) {
            CGFloat hue = 0, saturation = 0, a = 0, h = hue, s = saturation;
            [self setColorWheelValueWithPosition:CGPointMake(x, y) hue:&h saturation:&s];
            hue = h;
            saturation = s;
            RGBModel *rgb = [[RGBModel alloc] init];
            rgb.red = 1;
            rgb.green = 1;
            rgb.blud = 1;
            rgb.alpha = 0;//适配黑夜模式
            if (saturation < 1.0) {
                // Antialias the edge of the circle.
                if (saturation > 0.99) {
                    a = (1.0 - saturation) * 100;
                } else {
                    a = 1.0;
                }
                HSVModel *hsb = [[HSVModel alloc] init];
                hsb.hue = hue;
                hsb.saturation = saturation;
                hsb.value = self.brightness;
                hsb.alpha = a;
                rgb = [ColorManager getRGBWithHSVColor:hsb];
            }
            
            int i = 4 * (x + y * (int)size.width);
            bitmap[i] = (UInt8)(rgb.red * 0xFF);
            bitmap[i + 1] = (UInt8)(rgb.green * 0xFF);
            bitmap[i + 2] = (UInt8)(rgb.blud * 0xFF);
            bitmap[i + 3] = (UInt8)(rgb.alpha * 0xFF);
        }
    }
}

- (void)setColorWheelValueWithPosition:(CGPoint)position hue:(CGFloat *)hue saturation:(CGFloat *)saturation {
    int c = (int)(self.bounds.size.width / 2);
    double dx = (position.x - (CGFloat)c) / (CGFloat)c;
    double dy = (position.y - (CGFloat)c) / (CGFloat)c;
    CGFloat d = (CGFloat)sqrt(dx * dx + dy * dy);//d为position距离圆心的距离。
    *saturation = d;
    if (d == 0) {
        *hue = 0;
    } else {
        *hue = acos(dx / d) / M_PI / 2.0;
        if (dy < 0) {
            *hue = 1.0 - *hue;
        }
    }
}

@end
