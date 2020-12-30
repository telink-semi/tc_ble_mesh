/********************************************************************************************************
 * @file     ActionItemCell.m 
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
//  ActionItemCell.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/10/11.
//  Copyright © 2018年 Telink. All rights reserved.
//

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
