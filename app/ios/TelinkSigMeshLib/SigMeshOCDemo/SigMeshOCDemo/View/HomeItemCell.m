/********************************************************************************************************
 * @file     HomeItemCell.m 
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/7/31
 *
 * @par     Copyright (c) [2021], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/

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
    //遥控器remote特殊处理
    if (btModel.isRemote) {
        iconName = @"ic_rmt";
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
