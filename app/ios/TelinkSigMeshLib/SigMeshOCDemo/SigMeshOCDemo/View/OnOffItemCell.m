/********************************************************************************************************
 * @file     OnOffItemCell.m
 *
 * @brief    for TLSR chips
 *
 * @author     telink
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
//  OnOffItemCell.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/11/30.
//  Copyright © 2018年 Telink. All rights reserved.
//

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
    if (self.clickSwiftBlock) {
        self.clickSwiftBlock(sender);
    }
}

@end
