/********************************************************************************************************
 * @file     ShowTipsView.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/7/31
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

#import "ShowTipsView.h"

@implementation ShowTipsView

@end


@interface ShowTipsHandle()
@property (strong, nonatomic) ShowTipsView *showTipView;
@end

@implementation ShowTipsHandle

/**
 *  @brief  Singleton method
 *
 *  @return the default singleton instance. You are not allowed to create your own instances of this class.
 */
+ (instancetype)share {
    /// Singleton instance
    static ShowTipsHandle *shareHandle = nil;
    /// Note: The dispatch_once function can ensure that a certain piece
    /// of code is only executed once in the entire application life cycle!
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        /// Initialize the Singleton configure parameters.
        shareHandle = [[ShowTipsHandle alloc] init];
        [shareHandle configShowTipsView];
    });
    return shareHandle;
}

- (void)configShowTipsView{
    _showTipView = (ShowTipsView *)[[[NSBundle mainBundle] loadNibNamed:@"ShowTipsView" owner:nil options:nil] firstObject];
    _showTipView.bounds = [UIScreen mainScreen].bounds;
    _showTipView.center = [UIApplication sharedApplication].windows.firstObject.center;
    _showTipView.bgView.backgroundColor = [UIColor dynamicColorWithLight:[UIColor colorWithWhite:1.0 alpha:0.9] dark:[UIColor colorWithWhite:0.5 alpha:0.9]];
    _showTipView.tipLab.textColor = UIColor.telinkTitleBlack;
}

- (void)show:(NSString *)tip{
    __weak typeof(self) weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        if (![[UIApplication sharedApplication].windows.firstObject.subviews containsObject:weakSelf.showTipView]) {
            [[UIApplication sharedApplication].windows.firstObject addSubview:weakSelf.showTipView];
        }
        weakSelf.showTipView.tipLab.text = tip;
        [weakSelf.showTipView.activity startAnimating];
    });
}

- (void)hidden{
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.showTipView.activity stopAnimating];
        [self.showTipView removeFromSuperview];
    });
}

- (void)delayHidden:(NSTimeInterval )time{
    [self performSelector:@selector(hidden) withObject:nil afterDelay:time];
}

@end
