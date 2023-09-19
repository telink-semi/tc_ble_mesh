/********************************************************************************************************
 * @file     ActionItemCell.m 
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/10/11
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

#import "ActionItemCell.h"
#import "DeviceStateItemView.h"

@interface ActionItemCell ()
@property (weak, nonatomic) IBOutlet DeviceStateItemView *stateView;

@end

@implementation ActionItemCell

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
}

- (void)updateContent:(ActionModel *)model{
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:model.address];
    if (model.state != DeviceStateOutOfLine && node && node.sceneAddress.count > 0) {
        [self.selectButton setImage:[UIImage imageNamed:@"unxuan"] forState:UIControlStateNormal];
    } else {
        [self.selectButton setImage:[UIImage imageNamed:@"bukexuan"] forState:UIControlStateNormal];
    }

    [self.stateView updateContent:model];
}

- (IBAction)clickSelectButton:(UIButton *)sender {
    if (self.clickSelectBlock) {
        self.clickSelectBlock();
    }
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:NO animated:animated];
    // Configure the view for the selected state
}


@end
