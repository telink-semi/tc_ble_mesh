/********************************************************************************************************
 * @file     CustomAlertView.m
 *
 * @brief    A concise description.
 *
 * @author       梁家誌
 * @date         2021/9/16
 *
 * @par      Copyright © 2021 Telink Semiconductor (Shanghai) Co., Ltd. All rights reserved.
 *
 *           The information contained herein is confidential property of Telink
 *           Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *           of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *           Co., Ltd. and the licensee or the terms described here-in. This heading
 *           MUST NOT be removed from this file.
 *
 *           Licensee shall not delete, modify or alter (or permit any third party to delete, modify, or  
 *           alter) any information contained herein in whole or in part except as expressly authorized  
 *           by Telink semiconductor (shanghai) Co., Ltd. Otherwise, licensee shall be solely responsible  
 *           for any claim to the extent arising out of or relating to such deletion(s), modification(s)  
 *           or alteration(s).
 *
 *           Licensees are granted free, non-transferable use of the information in this
 *           file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
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
    if (self = [super init]) {
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
    self.customAlert.yz_height = 145+45*self.itemArray.count;
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
