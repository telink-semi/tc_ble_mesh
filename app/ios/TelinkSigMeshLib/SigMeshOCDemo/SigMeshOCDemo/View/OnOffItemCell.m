/********************************************************************************************************
 * @file     OnOffItemCell.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/11/30
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

#import "OnOffItemCell.h"

@implementation OnOffItemCell

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
    
    self.onoffView.layer.cornerRadius = 5;
    self.onoffView.layer.borderWidth = 1;
    self.onoffView.layer.borderColor = [UIColor lightGrayColor].CGColor;

}

- (IBAction)changeOnoff:(UISwitch *)sender {
    if (self.clickSwitchBlock) {
        self.clickSwitchBlock(sender);
    }
}

@end
