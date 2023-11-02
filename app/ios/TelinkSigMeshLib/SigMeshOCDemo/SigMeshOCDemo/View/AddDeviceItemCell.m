/********************************************************************************************************
 * @file     AddDeviceItemCell.m 
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/10/17
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

#import "AddDeviceItemCell.h"

@implementation AddDeviceItemCell{
    __weak IBOutlet UIView *_bgView;
    __weak IBOutlet UIImageView *_icon;
    __weak IBOutlet UIImageView *_icon_cert;
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
    NSString *addressString = @"";
    if (model.state == AddDeviceModelStateScanned || model.state == AddDeviceModelStateConnecting || model.scanRspModel.address == 0) {
        addressString = @"[Unallocated]";
    } else {
        addressString = [NSString stringWithFormat:@"0x%04X", model.scanRspModel.address];
    }    
    _nameLabel.text = [NSString stringWithFormat:@"adr:%@\nmac:%@",addressString,[LibTools getMacStringWithMac:model.scanRspModel.macAddress]];
    _nameLabel.font = [UIFont systemFontOfSize:10.0];
    _icon_cert.hidden = !model.scanRspModel.advOobInformation.supportForCertificateBasedProvisioning;
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
        case AddDeviceModelStateScanned:
            _stateLabel.text = @"SCANED";
            break;
        case AddDeviceModelStateConnecting:
            _stateLabel.text = @"CONNECTING";
            break;
        case AddDeviceModelStateProvisioning:
            _stateLabel.text = @"PROVISIONING";
            break;

        default:
            break;
    }
}

@end
