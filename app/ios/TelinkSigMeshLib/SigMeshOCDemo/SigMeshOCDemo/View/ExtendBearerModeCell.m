/********************************************************************************************************
 * @file     ExtendBearerModeCell.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2021/6/28
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

#import "ExtendBearerModeCell.h"

@implementation ExtendBearerModeCell

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

- (void)setExtendBearerMode:(SigTelinkExtendBearerMode)extendBearerMode {
    _noExtendButton.selected = extendBearerMode == SigTelinkExtendBearerMode_noExtend ? YES : NO;
    _extendGattOnlyButton.selected = extendBearerMode == SigTelinkExtendBearerMode_extendGATTOnly ? YES : NO;
    _extendGattAndAdvButton.selected = extendBearerMode == SigTelinkExtendBearerMode_extendGATTAndAdv ? YES : NO;
}

- (IBAction)clickNoExtend:(UIButton *)sender {
    _noExtendButton.selected = YES;
    _extendGattOnlyButton.selected = NO;
    _extendGattAndAdvButton.selected = NO;
    if (self.block) {
        self.block(SigTelinkExtendBearerMode_noExtend);
    }
}

- (IBAction)clickExtendGattOnly:(UIButton *)sender {
    _noExtendButton.selected = NO;
    _extendGattOnlyButton.selected = YES;
    _extendGattAndAdvButton.selected = NO;
    if (self.block) {
        self.block(SigTelinkExtendBearerMode_extendGATTOnly);
    }
}

- (IBAction)clickExtendGattAndAdv:(UIButton *)sender {
    _noExtendButton.selected = NO;
    _extendGattOnlyButton.selected = NO;
    _extendGattAndAdvButton.selected = YES;
    if (self.block) {
        self.block(SigTelinkExtendBearerMode_extendGATTAndAdv);
    }
}

@end
