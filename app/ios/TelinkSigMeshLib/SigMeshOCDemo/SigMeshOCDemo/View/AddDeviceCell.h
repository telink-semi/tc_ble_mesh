/********************************************************************************************************
 * @file     AddDeviceCell.h
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/2/9
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

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface AddDeviceCell : UITableViewCell
/// Background layer for setting rounded corners.
@property (weak, nonatomic) IBOutlet UIView *bgView;
/// Image layer used to set icon image.
@property (weak, nonatomic) IBOutlet UIImageView *icon;
/// Image layer used to set icon certificate image.
@property (weak, nonatomic) IBOutlet UIImageView *icon_cert;
/// Text layer used to set name.
@property (weak, nonatomic) IBOutlet UILabel *nameLabel;
/// Text layer used to set state.
@property (weak, nonatomic) IBOutlet UILabel *stateLabel;
/// Button layer used to set close button title..
@property (weak, nonatomic) IBOutlet UIButton *closeButton;
/// Button layer used to set add button title..
@property (weak, nonatomic) IBOutlet UIButton *addButton;

/// Update content with model.
/// - Parameter model: model of cell.
- (void)updateContent:(AddDeviceModel *)model;

@end

NS_ASSUME_NONNULL_END
