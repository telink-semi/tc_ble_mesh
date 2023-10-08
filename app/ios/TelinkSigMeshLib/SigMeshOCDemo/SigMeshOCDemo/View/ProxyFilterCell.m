/********************************************************************************************************
 * @file     ProxyFilterCell.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/4/12
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

#import "ProxyFilterCell.h"

@implementation ProxyFilterCell

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

- (void)setFilterType:(SigProxyFilerType)filterType {
    _whiteListButton.selected = filterType == SigProxyFilerType_whitelist ? YES : NO;
    _blackListButton.selected = filterType == SigProxyFilerType_blacklist ? YES : NO;
}

- (IBAction)clickWhiteList:(UIButton *)sender {
    _whiteListButton.selected = YES;
    _blackListButton.selected = NO;
    if (self.block) {
        self.block(SigProxyFilerType_whitelist);
    }
}

- (IBAction)clickExtendGattOnly:(UIButton *)sender {
    _whiteListButton.selected = NO;
    _blackListButton.selected = YES;
    if (self.block) {
        self.block(SigProxyFilerType_blacklist);
    }
}

@end
