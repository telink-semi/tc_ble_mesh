/********************************************************************************************************
 * @file     ExtendBearerModeCell.m
 *
 * @brief    A concise description.
 *
 * @author       梁家誌
 * @date         2021/6/28
 *
 * @par      Copyright © 2021 Telink Semiconductor (Shanghai) Co., Ltd. All rights reserved.
 *
 *           The information contained herein is confidential property of Telink
 *           Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *           of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *           Co., Ltd. and the licensee or the terms described here-in. This heading
 *           MUST NOT be removed from this file.
 *
 *           Licensee shall not delete, modify or alter (or permit any third party to delete, modify, or  
 *           alter) any information contained herein in whole or in part except as expressly authorized  
 *           by Telink semiconductor (shanghai) Co., Ltd. Otherwise, licensee shall be solely responsible  
 *           for any claim to the extent arising out of or relating to such deletion(s), modification(s)  
 *           or alteration(s).
 *
 *           Licensees are granted free, non-transferable use of the information in this
 *           file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/

#import "ExtendBearerModeCell.h"

@implementation ExtendBearerModeCell

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

- (void)setExtendBearerMode:(SigTelinkExtendBearerMode)extendBearerMode {
    _noExtendButton.selected = extendBearerMode == SigTelinkExtendBearerMode_noExtend ? YES : NO;
    _extendGattOnlyButton.selected = extendBearerMode == SigTelinkExtendBearerMode_extendGATTOnly ? YES : NO;
    _extendGattAndAdvButton.selected = extendBearerMode == SigTelinkExtendBearerMode_extendGATTAndAdv ? YES : NO;
}

- (IBAction)clickNoExtend:(UIButton *)sender {
    _noExtendButton.selected = YES;
    _extendGattOnlyButton.selected = NO;
    _extendGattAndAdvButton.selected = NO;
    if (self.block) {
        self.block(SigTelinkExtendBearerMode_noExtend);
    }
}

- (IBAction)clickExtendGattOnly:(UIButton *)sender {
    _noExtendButton.selected = NO;
    _extendGattOnlyButton.selected = YES;
    _extendGattAndAdvButton.selected = NO;
    if (self.block) {
        self.block(SigTelinkExtendBearerMode_extendGATTOnly);
    }
}

- (IBAction)clickExtendGattAndAdv:(UIButton *)sender {
    _noExtendButton.selected = NO;
    _extendGattOnlyButton.selected = NO;
    _extendGattAndAdvButton.selected = YES;
    if (self.block) {
        self.block(SigTelinkExtendBearerMode_extendGATTAndAdv);
    }
}

@end
