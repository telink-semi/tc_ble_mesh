/********************************************************************************************************
 * @file     NetKeyCell.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/12/21
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

#import "NetKeyCell.h"

@implementation NetKeyCell

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
    [self configurationCornerWithBgView:_bgView];
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

- (void)setModel:(SigNetkeyModel *)model {
    _model = model;
    _nameLabel.text = [NSString stringWithFormat:@"%@%@", [_model.key isEqualToString:SigDataSource.share.curNetkeyModel.key] ? @"[Current] " : @"", model.name];
    _indexLabel.text = [NSString stringWithFormat:@"0x%03lX", (long)model.index];
    _keyLabel.text = model.key;
    [self setUIWithSelected:[_model.key isEqualToString:SigDataSource.share.curNetkeyModel.key] bgView:_bgView];
}

@end
