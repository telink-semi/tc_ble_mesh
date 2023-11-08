/********************************************************************************************************
 * @file     CustomShareAlert.h
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

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

typedef enum : NSUInteger {
    ShareItemType_icon_text_next,
    ShareItemType_icon_text,
    ShareItemType_text,
    ShareItemType_button,
} ShareItemType;

@interface ShareAlertItemModel : NSObject
@property (nonatomic,assign) ShareItemType itemType;
@property(nonatomic,strong) NSString *textString;
@property (nonatomic,strong) UIColor *textColor;
@property (nonatomic,strong) UIImage *iconImage;
//@property (nonatomic, assign) ResponseShareState state;
@end

@interface CustomShareAlert : UIView
@property (weak, nonatomic) IBOutlet UILabel *titleLabel;
@property (weak, nonatomic) IBOutlet NSLayoutConstraint *tableVewHeightConstraint;
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIButton *closeBtn;

@property(nonatomic,strong) NSString *title;
@property(nonatomic,strong) NSMutableArray <ShareAlertItemModel *>*itemArray;

@property (copy, nonatomic) void(^backClickIndexBlock)(NSInteger index);

- (CGFloat)getAlertHightWithItemArray:(NSMutableArray<ShareAlertItemModel *> *)itemArray;

@end

NS_ASSUME_NONNULL_END
