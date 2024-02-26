/********************************************************************************************************
 * @file     AddTableHeaderCell.h
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/11/22
 *
 * @par     Copyright (c) 2023, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "BaseCell.h"

NS_ASSUME_NONNULL_BEGIN

@interface AddTableHeaderCell : BaseCell
@property (weak, nonatomic) IBOutlet UIButton *selectOriginButton;
@property (weak, nonatomic) IBOutlet UIView *originDeviceBgView;
@property (weak, nonatomic) IBOutlet UIImageView *originDeviceImageView;
@property (weak, nonatomic) IBOutlet UILabel *originDeviceNameLabel;

@property (weak, nonatomic) IBOutlet UIButton *selectTargetButton;
@property (weak, nonatomic) IBOutlet UIView *targetDeviceBgView;
@property (weak, nonatomic) IBOutlet UIImageView *targetDeviceImageView;
@property (weak, nonatomic) IBOutlet UILabel *targetDeviceNameLabel;

@property (weak, nonatomic) IBOutlet UIButton *selectRouteButton;

@end

NS_ASSUME_NONNULL_END
