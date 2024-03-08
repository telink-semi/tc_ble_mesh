/********************************************************************************************************
 * @file     DeviceElementItemView.h
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/7/20
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

@interface DeviceElementItemView : UIView
/// Image layer used to set state image.
@property (weak, nonatomic) IBOutlet UIImageView *stateImageView;
/// Text layer used to set state.
@property (weak, nonatomic) IBOutlet UILabel *stateLabel;
/// Button layer used to set select image icon.
@property (weak, nonatomic) IBOutlet UIButton *selectButton;

/// Update content with model.
/// - Parameter model: model of cell.
- (void)updateContent:(ActionModel *)model;

@end

NS_ASSUME_NONNULL_END
