/********************************************************************************************************
 * @file     BaseCell.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/11/16
 *
 * @par     Copyright (c) 2023, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "BaseCell.h"

@implementation BaseCell

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

/// 给bgView新增圆角的UI
/// - Parameter bgView: background view
- (void)configurationCornerWithBgView:(UIView *)bgView {
    //cornerRadius
    bgView.layer.cornerRadius = 7;
    //borderWidth
    bgView.layer.borderWidth = 1;
    //borderColor
    bgView.layer.borderColor = [UIColor telinkBorderColor].CGColor;
    //masksToBounds
    bgView.layer.masksToBounds = YES;
    //backgroundColor
    self.backgroundColor = [UIColor clearColor];
    bgView.backgroundColor = [UIColor telinkBackgroundWhite];
}

//根据是否选中更新UI
- (void)setUIWithSelected:(BOOL)isSelected bgView:(UIView *)bgView {
    bgView.layer.borderColor = isSelected ? [UIColor telinkButtonBlue].CGColor : [UIColor telinkBorderColor].CGColor;
}

@end
