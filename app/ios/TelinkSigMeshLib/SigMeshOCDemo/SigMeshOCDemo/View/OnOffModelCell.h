/********************************************************************************************************
 * @file     OnOffModelCell.h
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2020/11/24
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

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@class OnOffModelCell;

@protocol OnOffModelCellDelegate <NSObject>
@optional
- (void)onOffModelCell:(OnOffModelCell *)cell indexPath:(nonnull NSIndexPath *)indexPath didChangedValueWithValue:(BOOL)value;
@end


@interface OnOffModelCell : UITableViewCell
@property (strong, nonatomic) NSMutableArray <NSNumber *>*onoffAddressSource;
@property (strong, nonatomic) NSMutableArray <NSNumber *>*onoffStateSource;

@property (weak, nonatomic) id <OnOffModelCellDelegate>delegate;

- (void)refreshUI;
+ (CGFloat)getOnOffModelCellHeightOfItemCount:(NSInteger)itemCount;

@end

NS_ASSUME_NONNULL_END
