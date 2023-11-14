/********************************************************************************************************
 * @file     GroupCell.m
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

#import "GroupCell.h"

@interface GroupCell()
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
//    [DemoCommand switchOnOffWithIsOn:on address:self.groupAddress responseMaxCount:0 ack:NO successCallback:^(UInt16 source, UInt16 destination, SigGenericOnOffStatus * _Nonnull responseMessage) {
//        weakSelf.sw.on = on;
//    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
//
//    }];
}

@end
