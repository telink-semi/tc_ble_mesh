/********************************************************************************************************
 * @file     SensorConfigCell.h
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2024/1/18
 *
 * @par     Copyright (c) 2024, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

@interface SensorConfigCell : BaseCell
@property (weak, nonatomic) IBOutlet UIView *bgView;
@property (weak, nonatomic) IBOutlet UILabel *propertyIdLabel;
@property (weak, nonatomic) IBOutlet UILabel *rawDataLabel;
@property (weak, nonatomic) IBOutlet UILabel *positiveToleranceLabel;
@property (weak, nonatomic) IBOutlet UILabel *negativeToleranceLabel;
@property (weak, nonatomic) IBOutlet UILabel *samplingFunctionLabel;
@property (weak, nonatomic) IBOutlet UILabel *measurementPeriodLabel;
@property (weak, nonatomic) IBOutlet UILabel *updateIntervalLabel;
@property (weak, nonatomic) IBOutlet UILabel *fastCadencePeriodDivisorLabel;
@property (weak, nonatomic) IBOutlet UILabel *statusTriggerTypeLabel;
@property (weak, nonatomic) IBOutlet UILabel *statusTriggerDeltaDownLabel;
@property (weak, nonatomic) IBOutlet UILabel *statusTriggerDeltaUpLabel;
@property (weak, nonatomic) IBOutlet UILabel *statusMinIntervalLabel;
@property (weak, nonatomic) IBOutlet UILabel *fastCadenceLowLabel;
@property (weak, nonatomic) IBOutlet UILabel *fastCadenceHighLabel;
@property (weak, nonatomic) IBOutlet UIButton *getSensorDataButton;
@property (weak, nonatomic) IBOutlet UIButton *getSensorDescriptorButton;
@property (weak, nonatomic) IBOutlet UIButton *getSensorCadenceButton;

@property (nonatomic, strong) SigSensorDataModel *sensorDataModel;
@property (nonatomic, strong) SigSensorDescriptorModel *sensorDescriptorModel;
@property (nonatomic, strong) SigSensorCadenceModel *sensorCadenceModel;

@end

NS_ASSUME_NONNULL_END
