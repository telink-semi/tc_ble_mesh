/********************************************************************************************************
 * @file     BaseTableView.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2020/12/18
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
