/********************************************************************************************************
 * @file     AppKeyCell.h
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/12/21
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

@interface AppKeyCell : BaseCell
/// Background layer for setting rounded corners.
@property (weak, nonatomic) IBOutlet UIView *bgView;
/// Text layer used to set title.
@property (weak, nonatomic) IBOutlet UILabel *nameLabel;
/// Text layer used to set index.
@property (weak, nonatomic) IBOutlet UILabel *indexLabel;
/// Text layer used to set key.
@property (weak, nonatomic) IBOutlet UILabel *keyLabel;
/// Text layer used to set bound net key.
@property (weak, nonatomic) IBOutlet UILabel *boundNetKeyLabel;
/// UIButton layer used to set edit.
@property (weak, nonatomic) IBOutlet UIButton *editButton;

@property (nonatomic, strong) SigAppkeyModel *model;

@end

NS_ASSUME_NONNULL_END
