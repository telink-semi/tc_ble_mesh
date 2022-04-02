/********************************************************************************************************
 * @file     ModelIDCell.m 
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

#import "ModelIDCell.h"

@interface ModelIDCell()
@property (weak, nonatomic) IBOutlet UILabel *modelIDLabel;
@property (weak, nonatomic) IBOutlet UILabel *modelGroupLabel;
@property (weak, nonatomic) IBOutlet UILabel *modelNameLabel;
@property (strong, nonatomic) ModelIDModel *model;
@end

@implementation ModelIDCell

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
}

- (void)updateContent:(ModelIDModel *)model{
    self.model = model;
    self.modelGroupLabel.text = [NSString stringWithFormat:@"Group：%@",model.modelGroup];
    self.modelNameLabel.text = [NSString stringWithFormat:@"Name：%@",model.modelName];
    self.modelIDLabel.text = [NSString stringWithFormat:@"0X%X",(int)model.sigModelID];
}

@end
