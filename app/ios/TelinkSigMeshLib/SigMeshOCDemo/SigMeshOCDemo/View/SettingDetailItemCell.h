/********************************************************************************************************
 * @file     SettingDetailItemCell.h
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/10/10
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

NS_ASSUME_NONNULL_BEGIN

@interface SettingDetailItemCell : BaseCell
/// Background layer for setting rounded corners.
@property (weak, nonatomic) IBOutlet UIView *bgView;
/// Image layer used to set icon image.
@property (weak, nonatomic) IBOutlet UIImageView *iconImageView;
/// Text layer used to set name.
@property (weak, nonatomic) IBOutlet UILabel *nameLabel;
@property (weak, nonatomic) IBOutlet UILabel *detailLabel;
/// Image layer used to set next image.
@property (weak, nonatomic) IBOutlet UIImageView *nextImageView;
@end

NS_ASSUME_NONNULL_END
