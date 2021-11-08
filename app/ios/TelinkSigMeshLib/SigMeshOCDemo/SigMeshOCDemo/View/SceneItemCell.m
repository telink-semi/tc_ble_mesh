/********************************************************************************************************
 * @file     SceneItemCell.m 
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
//  SceneItemCell.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/9/25.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "SceneItemCell.h"

@interface SceneItemCell()
@property (weak, nonatomic) IBOutlet UIImageView *iconImageView;
@property (weak, nonatomic) IBOutlet UILabel *nameLabel;

@property (strong, nonatomic) SigSceneModel *model;
@end

@implementation SceneItemCell

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
}

- (void)updateContent:(SigSceneModel *)model{
    _model = model;
    _nameLabel.text = [NSString stringWithFormat:@"sceneID:0X%@",model.number];
}

- (IBAction)clickRecallScene:(UIButton *)sender {
    if (self.clickRecallBlock) {
        self.clickRecallBlock();
    }
}

- (IBAction)clickEditScene:(UIButton *)sender {
    if (self.clickEditBlock) {
        self.clickEditBlock();
    }
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:NO animated:animated];
    // Configure the view for the selected state
}

@end
