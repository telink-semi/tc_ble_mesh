/********************************************************************************************************
 * @file     ShowTipsView.m 
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
//  ShowTipsView.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "ShowTipsView.h"

@implementation ShowTipsView

@end


@interface ShowTipsHandle()
@property (strong, nonatomic) ShowTipsView *showTipView;
@end

@implementation ShowTipsHandle

+ (ShowTipsHandle *)share{
    static ShowTipsHandle *shareHandle = nil;
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        shareHandle = [[ShowTipsHandle alloc] init];
        [shareHandle configShowTipsView];
    });
    return shareHandle;
}

- (void)configShowTipsView{
    _showTipView = (ShowTipsView *)[[[NSBundle mainBundle] loadNibNamed:@"ShowTipsView" owner:nil options:nil] firstObject];
    _showTipView.bounds = [UIScreen mainScreen].bounds;
    _showTipView.center = [UIApplication sharedApplication].windows.firstObject.center;
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
