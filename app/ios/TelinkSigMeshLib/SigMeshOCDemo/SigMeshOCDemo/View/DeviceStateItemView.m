/********************************************************************************************************
 * @file     DeviceStateItemView.m 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
//
//  DeviceStateItemView.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/10/11.
//  Copyright © 2018年 Telink. All rights reserved.
//

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
    }
    self.stateImageView.image = [UIImage imageNamed:iconName];

    //离线与关闭，亮度色温显示0
    self.stateLabel.text = [NSString stringWithFormat:@"adr:0x%X\non/off:%@",model.address,state];
    if (node && node.sceneAddress.count == 0) {
        self.stateLabel.text = [NSString stringWithFormat:@"adr:0x%X\n%@",model.address,@"Not support scene register."];
    }
//    self.stateLabel.text = [NSString stringWithFormat:@"adr:0x%X\non/off:%@ lum:%d temp:%d",model.address,state,model.state == DeviceStateOn ? model.trueBrightness : 0 ,model.state == DeviceStateOn ? model.trueTemperature : 0];
}

@end
