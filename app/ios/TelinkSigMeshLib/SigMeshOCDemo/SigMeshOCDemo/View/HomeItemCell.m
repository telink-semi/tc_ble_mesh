/********************************************************************************************************
 * @file     HomeItemCell.m 
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
//  HomeItemCell.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "HomeItemCell.h"

@implementation HomeItemCell

- (void)updateContent:(SigNodeModel *)model{
    UInt16 deviceAddress = 0;
    NSString *iconName = @"";
    
    SigNodeModel *btModel = (SigNodeModel *)model;
    switch (btModel.state) {
        case DeviceStateOutOfLine:
            iconName = @"dengo";
            break;
        case DeviceStateOff:
            iconName = @"dengn";
            break;
        case DeviceStateOn:
            //亮度不为100则图标显示开淡色，已经废弃。
//            if (btModel.trueBrightness == 100) {
                iconName = @"dengs";
//            }else{
//                iconName = @"dengs_half";
//            }
            break;
        default:
            break;
    }
    if (!btModel.isKeyBindSuccess) {
        iconName = @"dengo";
    }
    //传感器sensor特殊处理
    if (btModel.isSensor) {
        iconName = @"ic_battery-20-bluetooth";
    }

    self.icon.image = [UIImage imageNamed:iconName];
    deviceAddress = btModel.address;
    NSString *tempAddress = [NSString stringWithFormat:@"%02X",deviceAddress];
    if (deviceAddress > 0xff) {
        tempAddress = [NSString stringWithFormat:@"%04X",deviceAddress];
    }
    NSString *tempType = @"";
    if ([LibTools uint16From16String:btModel.cid] == kCompanyID) {
        tempType = [NSString stringWithFormat:@"Pid-%02X",[LibTools uint16From16String:btModel.pid]];
    }else{
        tempType = [NSString stringWithFormat:@"Cid-%04X",[LibTools uint16From16String:btModel.cid]];
    }
    
    //原做法：显示短地址+亮度+色温
//    NSString *tempBrightness = [NSString stringWithFormat:@"%d",btModel.trueBrightness];
//    NSString *tempTemperatrure = [NSString stringWithFormat:@"%d",btModel.trueTemperature];
//    self.address.text = btModel.state == DeviceStateOn ? [NSString stringWithFormat:@"%@ : %@ : %@",tempAddress,tempBrightness,tempTemperatrure]:[NSString stringWithFormat:@"%@ : 0 : 0",tempAddress];
    //新做法：显示短地址+PID
    self.address.text = [NSString stringWithFormat:@"%@(%@)",tempAddress,tempType];
    
    if (!btModel.isKeyBindSuccess) {
        self.address.text = [NSString stringWithFormat:@"%@(unbound)",tempAddress];
    }
    self.address.textColor = [UIColor grayColor];
    //原做法：直连设备显示蓝色
//    if ([model.peripheralUUID isEqualToString:SigBearer.share.getCurrentPeripheral.identifier.UUIDString] && SigBearer.share.getCurrentPeripheral.state == CBPeripheralStateConnected) {
//        self.address.textColor = kDefultColor;
//    }
    //新做法：直连设备显示蓝色
    if (model.address == SigDataSource.share.unicastAddressOfConnected && SigBearer.share.isOpen) {
        self.address.textColor = kDefultColor;
    }
}

@end
