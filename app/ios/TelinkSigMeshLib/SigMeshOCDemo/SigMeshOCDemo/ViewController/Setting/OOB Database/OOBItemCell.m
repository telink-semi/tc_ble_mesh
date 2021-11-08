/********************************************************************************************************
* @file     OOBItemCell.m
*
* @brief    for TLSR chips
*
* @author       Telink, 梁家誌
* @date     Sep. 30, 2010
*
* @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
*           All rights reserved.
*
*             The information contained herein is confidential and proprietary property of Telink
*              Semiconductor (Shanghai) Co., Ltd. and is available under the terms
*             of Commercial License Agreement between Telink Semiconductor (Shanghai)
*             Co., Ltd. and the licensee in separate contract or the terms described here-in.
*           This heading MUST NOT be removed from this file.
*
*              Licensees are granted free, non-transferable use of the information in this
*             file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
*
*******************************************************************************************************/
//
//  OOBItemCell.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2020/5/12.
//  Copyright © 2020 Telink. All rights reserved.
//

#import "OOBItemCell.h"

@implementation OOBItemCell

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
}

- (void)setModel:(SigOOBModel *)model {
    _model = model;
    _UUIDLabel.text = [NSString stringWithFormat:@"UUID:%@",model.UUIDString];
    _OOBLabel.text = [NSString stringWithFormat:@"OOB :%@",model.OOBString];
    _sourceLabel.text = [NSString stringWithFormat:@"(%@ - %@)",model.lastEditTimeString,model.sourceType == OOBSourceTpyeManualInput ? @"manual input" : @"from file"];
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

@end
