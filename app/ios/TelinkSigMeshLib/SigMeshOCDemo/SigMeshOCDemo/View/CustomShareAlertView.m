/********************************************************************************************************
 * @file     CustomShareAlertView.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/5/18
 *
 * @par     Copyright (c) [2023], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "CustomShareAlertView.h"
#import "UIButton+extension.h"

@interface CustomShareAlertView ()<UIGestureRecognizerDelegate>
@property(nonatomic,strong) CustomShareAlert *customAlert;
@property(nonatomic,strong) NSString *title;
@property(nonatomic,strong) NSMutableArray <ShareAlertItemModel *>*itemArray;
@property(nonatomic,copy) ShareAlertResult alertResult;

@end

@implementation CustomShareAlertView

- (instancetype)initWithTitle:(NSString *)title itemArray:(NSArray <ShareAlertItemModel *>*)itemArray alertResult:(ShareAlertResult) alertResult {
    if (self = [super init]) {
        self.frame = [UIScreen mainScreen].bounds;
        self.backgroundColor = [[UIColor blackColor] colorWithAlphaComponent:0.6];
        self.title = title;
        self.itemArray = [NSMutableArray arrayWithArray:itemArray];
        self.alertResult = alertResult;
        UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tapGestureAction:)];
        tap.delegate = self;
        [self addGestureRecognizer:tap];
        [self setup];
    }
    return self;
}

- (void)showCustomAlertView {
    [[UIApplication sharedApplication].keyWindow.rootViewController.view addSubview:self];
    [self creatShowAnimation];
}

//解决UITapGestureRecognizer手势与UITableView的点击事件的冲突
- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch {
    CGPoint point = [touch locationInView:self];
    if (CGRectContainsPoint(self.customAlert.frame, point)) {
        return NO;
    }
    return YES;
}

- (void)tapGestureAction:(UITapGestureRecognizer *)gesture {
    [self dismiss];
}

- (void)setup {
    [self addSubview:self.customAlert];
    self.customAlert.title = self.title;
    self.customAlert.itemArray = self.itemArray;
}

- (void)creatShowAnimation {
    self.customAlert.transform = CGAffineTransformMakeScale(0.90, 0.90);
    [UIView animateWithDuration:0.25 delay:0 usingSpringWithDamping:0.8 initialSpringVelocity:1 options:UIViewAnimationOptionCurveLinear animations:^{
        self.customAlert.transform = CGAffineTransformMakeScale(1.0, 1.0);
    } completion:^(BOOL finished) {
    }];
}

- (void)dismiss {
    self.customAlert.transform = CGAffineTransformMakeScale(1.0, 1.0);
    [UIView animateWithDuration:0.20 delay:0 usingSpringWithDamping:0.8 initialSpringVelocity:1 options:UIViewAnimationOptionCurveLinear animations:^{
        self.customAlert.transform = CGAffineTransformMakeScale(0.0, 0.0);
    } completion:^(BOOL finished) {
        [self removeFromSuperview];
    }];
}

- (CustomShareAlert *)customAlert {
    if (!_customAlert) {
        _customAlert =[[NSBundle mainBundle]loadNibNamed:@"CustomShareAlert" owner:self options:nil].firstObject;
        CGFloat height = [_customAlert getAlertHeightWithItemArray:_itemArray];
        _customAlert.frame = CGRectMake(0, [[UIScreen mainScreen] bounds].size.height - height, [[UIScreen mainScreen] bounds].size.width, height);
        __weak typeof(self) weakSelf = self;
        [_customAlert setBackClickIndexBlock:^(NSInteger index) {
            if (weakSelf.alertResult) {
                weakSelf.alertResult(index);
            }
            [weakSelf dismiss];
        }];
        [_customAlert.closeBtn addAction:^(UIButton *button) {
            [weakSelf dismiss];
        }];
    }
    return _customAlert;
}

@end
