/********************************************************************************************************
 * @file     UIButton+extension.m
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

#import "UIButton+extension.h"
#import <objc/runtime.h>

static char ActionTag;

@interface UIButton ()
/* 波纹视图 */
@property (nonatomic, strong) UIView *waveView;
@end

@implementation UIButton (extension)
static const char *waveViewId = "waveView";
static const char *showWave = "showWave";

+ (void)load {
    SwizzleMethod(self, @selector(initWithFrame:), @selector(telink_initWithFrame:));
    SwizzleMethod(self, @selector(initWithCoder:), @selector(telink_initWithCoder:));
    SwizzleMethod(self, @selector(setFrame:), @selector(telink_setFrame:));
    SwizzleMethod(self, @selector(beginTrackingWithTouch:withEvent:), @selector(telink_beginTrackingWithTouch:withEvent:));
}

void SwizzleMethod(Class c,SEL originSEL, SEL replaceSEL) {
    Method originMethod = class_getInstanceMethod(c, originSEL);
    Method replaceMethod = class_getInstanceMethod(c, replaceSEL);
    if (class_addMethod(c, originSEL, method_getImplementation(replaceMethod), method_getTypeEncoding(replaceMethod))) {
        class_replaceMethod(c, replaceSEL, method_getImplementation(originMethod), method_getTypeEncoding(originMethod));
    } else {
        method_exchangeImplementations(originMethod, replaceMethod);
    }
}

#pragma mark - Private Method
- (instancetype)telink_initWithFrame:(CGRect)frame {
    [self initWaveView];
    return [self telink_initWithFrame:frame];
}

- (instancetype)telink_initWithCoder:(NSCoder *)coder {
    [self initWaveView];
    return [self telink_initWithCoder:coder];
}

- (void)initWaveView {
    self.waveView = [[UIView alloc] init];
    self.waveView.backgroundColor = [UIColor colorWithRed:0x4A/255.0 green:0x87/255.0 blue:0xEE/255.0 alpha:0.5];
    self.waveView.alpha=0;
    self.clipsToBounds = YES;
    self.isShowWave = kDefaultShowWaveValue;
}

- (void)telink_setFrame:(CGRect)frame {
    [super setFrame:frame];
//    CGFloat max = frame.size.width > frame.size.height ? frame.size.width : frame.size.height;
//    if (max >= 100) {
//        max = 100.0;
//    }
    CGFloat max = 70;
    self.waveView.frame = CGRectMake(0, 0, 2*max, 2*max);
    self.waveView.layer.cornerRadius = max;
    self.waveView.layer.masksToBounds=true;
    self.clipsToBounds = YES;
}

- (BOOL)telink_beginTrackingWithTouch:(UITouch *)touch withEvent:(nullable UIEvent *)event {
    if (self.isShowWave) {
        CGPoint location = [touch locationInView:self];
        [self addSubview: self.waveView];
        self.waveView.center = location;
        self.waveView.transform = CGAffineTransformMakeScale(0.3, 0.3);
        [UIView animateWithDuration:0.1 animations:^{
            self.waveView.alpha = 1;
            self.alpha = 0.9;
        }];
        [UIView animateWithDuration:0.8 animations:^{
            self.waveView.transform = CGAffineTransformMakeScale(1, 1);
            self.waveView.alpha = 0;
            self.alpha = 1;
        } completion:^(BOOL finished) {
            [self.waveView removeFromSuperview];
        }];
    }
    return [super beginTrackingWithTouch:touch withEvent:event];
}

#pragma mark - setter,getter

- (void)setIsShowWave:(BOOL)isShowWave {
    self.waveView.hidden = isShowWave ? NO : YES;
    objc_setAssociatedObject(self, showWave, @(isShowWave), OBJC_ASSOCIATION_ASSIGN);
}

- (BOOL)isShowWave {
    return objc_getAssociatedObject(self, showWave);
}

- (UIView *)waveView {
    return objc_getAssociatedObject(self, waveViewId);
}

- (void)setWaveView:(UIView *)waveView {
    objc_setAssociatedObject(self, waveViewId, waveView, OBJC_ASSOCIATION_RETAIN);
}

- (void)action:(id)sender {
    ButtonBlock blockAction = (ButtonBlock)objc_getAssociatedObject(self, &ActionTag);
    if (blockAction) {
        blockAction(self);
    }
}

#pragma mark - Public Method

/**
 *  使用block回调点击事件,默认的touchUpInside
 *
 *  @param block block返回值
 */
- (void)addAction:(ButtonBlock)block {
    objc_setAssociatedObject(self, &ActionTag, block, OBJC_ASSOCIATION_COPY_NONATOMIC);
    [self addTarget:self action:@selector(action:) forControlEvents:UIControlEventTouchUpInside];
}

/**
 *    使用block回调点击事件
 *
 *  @param block   block返回值
 *  @param controlEvent UIControlEvents 自定义点击事件
 */
- (void)addAction:(ButtonBlock)block forControlEvents:(UIControlEvents)controlEvent {
    objc_setAssociatedObject(self, &ActionTag, block, OBJC_ASSOCIATION_COPY_NONATOMIC);
    [self addTarget:self action:@selector(action:) forControlEvents:controlEvent];
}

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
+ (UIButton *)buttonWithFrame:(CGRect)frame image:(UIImage *)image textColor:(UIColor *)textColor title:(NSString *)title {
    UIButton * button = [UIButton buttonWithType:UIButtonTypeCustom];
    button.frame = frame;
    [button setTitle:title forState:UIControlStateNormal];
    [button setTitleColor:textColor forState:UIControlStateNormal];
    button.imageView.contentMode=UIViewContentModeScaleAspectFit;
    [button setImage:image forState:UIControlStateNormal];
    return button;
}

@end
