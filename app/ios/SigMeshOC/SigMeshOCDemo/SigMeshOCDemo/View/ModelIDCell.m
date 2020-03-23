/********************************************************************************************************
 * @file     ModelIDCell.m 
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
//  ModelIDCell.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "ModelIDCell.h"

@interface ModelIDCell()
@property (weak, nonatomic) IBOutlet UILabel *modelIDLabel;
@property (weak, nonatomic) IBOutlet UILabel *modelGroupLabel;
@property (weak, nonatomic) IBOutlet UILabel *modelNameLabel;
@property (strong, nonatomic) ModelIDModel *model;
@end

@implementation ModelIDCell

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
}

- (void)updateContent:(ModelIDModel *)model{
    self.model = model;
    self.modelGroupLabel.text = [NSString stringWithFormat:@"Group：%@",model.modelGroup];
    self.modelNameLabel.text = [NSString stringWithFormat:@"Name：%@",model.modelName];
    self.modelIDLabel.text = [NSString stringWithFormat:@"0X%X",(int)model.sigModelID];
}

@end
