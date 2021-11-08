/********************************************************************************************************
 * @file     BaseTableView.m
 *
 * @brief    for TLSR chips
 *
 * @author       Telink, 梁家誌
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *             The information contained herein is confidential and proprietary property of Telink
 *              Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *             of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *             Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 *              Licensees are granted free, non-transferable use of the information in this
 *             file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/
//
//  BaseTableView.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2020/12/18.
//  Copyright © 2020 Telink. All rights reserved.
//

#import "BaseTableView.h"

@implementation BaseTableView

// MARK: - UITouch delegate methods

-(BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch {
    if ([self.baseTableViewDelegate respondsToSelector:@selector(baseTableView:gestureRecognizer:shouldReceiveTouch:)]) {
        return [self.baseTableViewDelegate baseTableView:self gestureRecognizer:gestureRecognizer shouldReceiveTouch:touch];
    }
//    if ([touch.view isDescendantOfView:self]) {
//        return NO;
//    }
    return YES;
}

@end
