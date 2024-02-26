/********************************************************************************************************
 * @file     HomeItemCell.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/7/31
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

/// Update content with model.
/// - Parameter model: model of cell.
- (void)updateContent:(SigNodeModel *)model{
    UInt16 deviceAddress = model.address;
    self.icon.image = [DemoTool getNodeStateImageWithUnicastAddress:model.address];
    NSString *tempAddress = [NSString stringWithFormat:@"%02X",deviceAddress];
    if (deviceAddress > 0xff) {
        tempAddress = [NSString stringWithFormat:@"%04X",deviceAddress];
    }
    NSString *tempType = @"";
    if (model.isSensor) {
        if (model.sensorDataArray.count > 0) {
            SigSensorDataModel *sensorData = model.sensorDataArray.firstObject;
            tempType = [NSString stringWithFormat:@"%04X-%@", sensorData.propertyID, [LibTools convertDataToHexStr:sensorData.rawValueData]];
        } else {
            tempType = @"Sensor-NULL";
        }
    } else {
        if ([LibTools uint16From16String:model.cid] == kCompanyID) {
            tempType = [NSString stringWithFormat:@"Pid-%02X",[LibTools uint16From16String:model.pid]];
        }else{
            tempType = [NSString stringWithFormat:@"Cid-%04X",[LibTools uint16From16String:model.cid]];
        }
    }
    //显示短地址+PID
    self.address.text = [NSString stringWithFormat:@"%@(%@)",tempAddress,tempType];

    if (!model.isKeyBindSuccess) {
        self.address.text = [NSString stringWithFormat:@"%@(unbound)",tempAddress];
    }
    self.address.textColor = [UIColor grayColor];
    //直连设备显示蓝色
    if (model.address == SigDataSource.share.unicastAddressOfConnected && SigBearer.share.isOpen) {
        self.address.textColor = HEX(#4A87EE);
    }
}

@end
