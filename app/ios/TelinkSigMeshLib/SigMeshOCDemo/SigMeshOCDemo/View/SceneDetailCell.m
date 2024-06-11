/********************************************************************************************************
 * @file     SceneDetailCell.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2024/3/25
 *
 * @par     Copyright (c) 2024, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "SceneDetailCell.h"

@implementation SceneDetailCell

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
}

/// Update content with model.
/// - Parameter model: model of cell.
- (void)updateContent:(SigNodeModel *)model{
    if (model.sceneAddress.count > 0) {
        _nameLabel.text = [NSString stringWithFormat:@"name: %@\naddress: 0x%04X", model.name, model.address];
    } else {
        _nameLabel.text = [NSString stringWithFormat:@"name: %@(scene not support)\naddress: 0x%04X", model.name, model.address];
    }
    [self configurationCornerWithBgView:_bgView];
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

@end
