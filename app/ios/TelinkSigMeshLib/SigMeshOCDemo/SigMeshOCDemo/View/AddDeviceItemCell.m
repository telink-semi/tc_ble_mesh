/********************************************************************************************************
 * @file     AddDeviceItemCell.m 
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
//  AddDeviceItemCell.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/10/17.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "AddDeviceItemCell.h"

@implementation AddDeviceItemCell{
    __weak IBOutlet UIView *_bgView;
    __weak IBOutlet UIImageView *_icon;
    __weak IBOutlet UILabel *_nameLabel;
    __weak IBOutlet UILabel *_stateLabel;
}

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
    _bgView.layer.borderWidth = 1;
    _bgView.layer.borderColor = [UIColor lightGrayColor].CGColor;
    _bgView.layer.cornerRadius = 5;
}

- (void)updateContent:(AddDeviceModel *)model{
    _nameLabel.text = [NSString stringWithFormat:@"adr:0x%X\nmac:%@",model.scanRspModel.address,[LibTools getMacStringWithMac:model.scanRspModel.macAddress]];
    _nameLabel.font = [UIFont systemFontOfSize:10.0];
    switch (model.state) {
        case AddDeviceModelStateBinding:
            _stateLabel.text = @"BINDING";
            break;
        case AddDeviceModelStateProvisionFail:
            _stateLabel.text = @"Provision Fail";
            break;
        case AddDeviceModelStateBindSuccess:
            _stateLabel.text = @"BOUND";
            break;
        case AddDeviceModelStateBindFail:
            _stateLabel.text = @"UNBIND";
            break;
        case AddDeviceModelStateScaned:
            _stateLabel.text = @"SCANED";
            break;
        case AddDeviceModelStateProvisioning:
            _stateLabel.text = @"PROVISIONING";
            break;

        default:
            break;
    }
}

@end
