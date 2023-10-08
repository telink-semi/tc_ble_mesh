/********************************************************************************************************
 * @file     DeviceElementItemView.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/7/20
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

#import "DeviceElementItemView.h"

@implementation DeviceElementItemView

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        [self setup];
    }
    return self;
}

- (instancetype)initWithCoder:(NSCoder *)aDecoder {
    if (self = [super initWithCoder:aDecoder]) {
        [self setup];
    }
    return self;
}

- (void)setup {
//    [[NSBundle mainBundle] loadNibNamed:@"DeviceElementItemView" owner:self options:nil];
//    [self addSubview:self.stateView];
//    self.stateView.frame = self.frame;
}

- (void)updateContent:(ActionModel *)model{
    NSString *iconName = @"";
    NSString *state = @"";
    switch (model.state) {
        case DeviceStateOutOfLine:
            iconName = @"dengo";
            state = @"OFFLINE";
            break;
        case DeviceStateOff:
            iconName = @"dengn";
            state = @"OFF";
            break;
        case DeviceStateOn:
            iconName = @"dengs";
            state = @"ON";
            break;
        default:
            break;
    }
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:model.address];
    if (node) {
        //传感器sensor特殊处理
        if (node.isSensor) {
            iconName = @"ic_battery-20-bluetooth";
        }
        //遥控器remote特殊处理
        if (node.isRemote) {
            iconName = @"ic_rmt";
        }
    }
    self.stateImageView.image = [UIImage imageNamed:iconName];
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
    self.stateLabel.text = [NSString stringWithFormat:@"adr:0x%X\non/off:%@",model.address,state];
    if (node && node.sceneAddress.count == 0) {
        self.stateLabel.text = [NSString stringWithFormat:@"adr:0x%X\n%@",model.address,@"Not support scene register."];
    }
//    self.stateLabel.text = [NSString stringWithFormat:@"adr:0x%X\non/off:%@ lum:%d temp:%d",model.address,state,model.state == DeviceStateOn ? model.trueBrightness : 0 ,model.state == DeviceStateOn ? model.trueTemperature : 0];
}

@end
