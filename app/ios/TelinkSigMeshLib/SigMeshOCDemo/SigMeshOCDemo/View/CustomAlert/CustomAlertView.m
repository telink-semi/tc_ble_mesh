/********************************************************************************************************
 * @file     CustomAlertView.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2021/9/16
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

#import "CustomAlertView.h"
#import "UIView+Frame.h"

@interface CustomAlertView ()
@property(nonatomic,strong) CustomAlert *customAlert;
@property(nonatomic,strong) NSString *title;
@property(nonatomic,strong) NSString *detail;
@property(nonatomic,strong) NSMutableArray <AlertItemModel *>*itemArray;
@property(nonatomic,strong) NSString *leftBtnTitle;
@property(nonatomic,strong) NSString *rightBtnTitle;
@property(nonatomic,copy) AlertResult alertResult;

@end

@implementation CustomAlertView

- (instancetype)initWithTitle:(NSString *)title detail:(NSString *)detail itemArray:(NSArray <AlertItemModel *>*)itemArray leftBtnTitle:(NSString *)leftBtnTitle rightBtnTitle:(NSString *)rightBtnTitle alertResult:(AlertResult) alertResult {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.frame = [UIScreen mainScreen].bounds;
        self.backgroundColor = [[UIColor blackColor] colorWithAlphaComponent:0.6];
        self.title = title;
        self.detail = detail;
        self.itemArray = [NSMutableArray arrayWithArray:itemArray];
        self.leftBtnTitle = leftBtnTitle;
        self.rightBtnTitle = rightBtnTitle;
        self.alertResult = alertResult;
        [self setup];
    }
    return self;

}

- (void)showCustomAlertView {
    [[UIApplication sharedApplication].keyWindow.rootViewController.view addSubview:self];
    [self creatShowAnimation];
}

- (void)setup {
    [self addSubview:self.customAlert];
    self.customAlert.title = self.title;
    self.customAlert.detail = self.detail;
    self.customAlert.itemArray = self.itemArray;
    self.customAlert.leftBtnTitle = self.leftBtnTitle;
    self.customAlert.rightBtnTitle = self.rightBtnTitle;
    self.customAlert.leftBtn.tag = 1;
    self.customAlert.rightBtn.tag = 2;
    self.customAlert.height = 145+45*self.itemArray.count;
    [self.customAlert.leftBtn addTarget:self action:@selector(btnClick:) forControlEvents:UIControlEventTouchUpInside];
    [self.customAlert.rightBtn addTarget:self action:@selector(btnClick:) forControlEvents:UIControlEventTouchUpInside];
}


- (void)btnClick:(UIButton *)button {
    if (self.alertResult) {
        [self dismiss];
        self.alertResult(self.customAlert, button.tag == 2);
    }
    [self removeFromSuperview];
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

- (CustomAlert *)customAlert {
    if (!_customAlert) {
        _customAlert =[[NSBundle mainBundle]loadNibNamed:@"CustomAlert" owner:self options:nil].firstObject;
        _customAlert.frame = CGRectMake([[UIScreen mainScreen] bounds].size.width/2 -140, ([[UIScreen mainScreen] bounds].size.height -64 - 145)/2, 280, 145);
    }
    return _customAlert;
}

@end
