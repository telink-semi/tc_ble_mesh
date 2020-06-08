/********************************************************************************************************
 * @file     GroupCell.m 
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
//  GroupCell.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "GroupCell.h"

@interface GroupCell()
@property (weak, nonatomic) IBOutlet UILabel *groupID;
@property (weak, nonatomic) IBOutlet UISwitch *sw;
@property (assign, nonatomic) UInt16 groupAddress;
@property (strong, nonatomic) SigGroupModel *model;
@end

@implementation GroupCell

- (void)awakeFromNib {
    [super awakeFromNib];

    self.groupAddress = 0;
}

- (void)updateContent:(SigGroupModel *)model{
    self.model = model;
    self.groupID.text = model.name;
    self.groupAddress = model.intAddress;
    self.sw.on = model.isOn;
}

- (IBAction)changeStatus:(UISwitch *)sender {
    BOOL on = self.sw.isOn;
    if (!SigBearer.share.isOpen) {
        return;
    }
    __weak typeof(self) weakSelf = self;
    [DemoCommand switchOnOffWithIsOn:on address:self.groupAddress responseMaxCount:(int)self.model.groupOnlineDevices.count ack:YES successCallback:^(UInt16 source, UInt16 destination, SigGenericOnOffStatus * _Nonnull responseMessage) {
        weakSelf.sw.on = on;
    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
        
    }];
}

@end
