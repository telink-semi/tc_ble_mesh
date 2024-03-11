/********************************************************************************************************
 * @file     DeviceStateItemView.m 
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/10/11
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

#import "DeviceStateItemView.h"

@interface DeviceStateItemView ()
@property (weak, nonatomic) IBOutlet UIImageView *stateImageView;
@property (weak, nonatomic) IBOutlet UILabel *stateLabel;

@end

@implementation DeviceStateItemView

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
    [[NSBundle mainBundle] loadNibNamed:@"DeviceStateItemView" owner:self options:nil];
    [self addSubview:self.stateView];
}

- (void)updateContent:(ActionModel *)model{
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:model.address];
    self.stateImageView.image = [DemoTool getNodeStateImageWithUnicastAddress:model.address];    
    //离线与关闭，亮度色温显示0
    self.stateLabel.text = [NSString stringWithFormat:@"Name:%@ adr:0x%X\non/off:%@", node.name, model.address,[DemoTool getNodeStateStringWithUnicastAddress:model.address]];
    if (node && node.sceneAddress.count == 0) {
        self.stateLabel.text = [NSString stringWithFormat:@"Name:%@ adr:0x%X\n%@", node.name, model.address,@"Not support scene register."];
    }
}

@end
