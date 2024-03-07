/********************************************************************************************************
 * @file     DeviceElementItemView.m
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

#import "DeviceElementItemView.h"

@implementation DeviceElementItemView

/// Update content with model.
/// - Parameter model: model of cell.
- (void)updateContent:(ActionModel *)model{
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:model.address];
    self.stateImageView.image = [DemoTool getNodeStateImageWithUnicastAddress:model.address];
    self.stateImageView.hidden = NO;
    if (node.sceneAddress.count > 1) {
        self.stateImageView.hidden = ![@(model.address) isEqualToNumber:node.sceneAddress.firstObject];
    }
    if (model.state != DeviceStateOutOfLine && node && node.sceneAddress.count > 0) {
        [self.selectButton setImage:[UIImage imageNamed:@"unxuan"] forState:UIControlStateNormal];
    } else {
        [self.selectButton setImage:[UIImage imageNamed:@"bukexuan"] forState:UIControlStateNormal];
    }
    //离线与关闭，亮度色温显示0
    self.stateLabel.text = [NSString stringWithFormat:@"Name:%@ adr:0x%X\non/off:%@", node.name, model.address, [DemoTool getNodeStateStringWithUnicastAddress:model.address]];
    if (node && node.sceneAddress.count == 0) {
        self.stateLabel.text = [NSString stringWithFormat:@"Name:%@ adr:0x%X\n%@", node.name, model.address,@"Not support scene register."];
    }
}

@end
