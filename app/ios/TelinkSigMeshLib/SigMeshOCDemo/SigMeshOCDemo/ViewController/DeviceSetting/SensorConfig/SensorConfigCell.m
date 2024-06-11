/********************************************************************************************************
 * @file     SensorConfigCell.m
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

#import "SensorConfigCell.h"

@implementation SensorConfigCell

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
    [self configurationCornerWithBgView:_bgView];
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

- (void)setSensorDataModel:(SigSensorDataModel *)sensorDataModel {
    _sensorDataModel = sensorDataModel;
    _propertyIdLabel.text = [NSString stringWithFormat:@"Property ID: 0x%04X（%@）", sensorDataModel.propertyID, [SigHelper.share getSensorNameWithPropertyID:sensorDataModel.propertyID]];
    _rawDataLabel.text = [NSString stringWithFormat:@"0x%@", [LibTools convertDataToHexStr:sensorDataModel.rawValueData]];
}

- (void)setSensorDescriptorModel:(SigSensorDescriptorModel *)sensorDescriptorModel {
    _sensorDescriptorModel = sensorDescriptorModel;
    _positiveToleranceLabel.text = sensorDescriptorModel == nil ? @"[NULL]" : [NSString stringWithFormat:@"%d", sensorDescriptorModel.positiveTolerance];
    _negativeToleranceLabel.text = sensorDescriptorModel == nil ? @"[NULL]" : [NSString stringWithFormat:@"%d", sensorDescriptorModel.negativeTolerance];
    _samplingFunctionLabel.text = sensorDescriptorModel == nil ? @"[NULL]" : [NSString stringWithFormat:@"%d - %@", sensorDescriptorModel.samplingFunction, [SigHelper.share getDetailOfSigSensorSamplingFunctionType:sensorDescriptorModel.samplingFunction]];
    _measurementPeriodLabel.text = sensorDescriptorModel == nil ? @"[NULL]" : [NSString stringWithFormat:@"%d", sensorDescriptorModel.measurementPeriod];
    _updateIntervalLabel.text = sensorDescriptorModel == nil ? @"[NULL]" : [NSString stringWithFormat:@"%d", sensorDescriptorModel.updateInterval];
}

- (void)setSensorCadenceModel:(SigSensorCadenceModel *)sensorCadenceModel {
    _sensorCadenceModel = sensorCadenceModel;
    _fastCadencePeriodDivisorLabel.text = sensorCadenceModel == nil ? @"[NULL]" : [NSString stringWithFormat:@"%d", sensorCadenceModel.fastCadencePeriodDivisor];
    _statusTriggerTypeLabel.text = sensorCadenceModel == nil ? @"[NULL]" : [NSString stringWithFormat:@"%d", sensorCadenceModel.statusTriggerType];
    _statusTriggerDeltaDownLabel.text = sensorCadenceModel == nil ? @"[NULL]" : [NSString stringWithFormat:@"%d", sensorCadenceModel.statusTriggerDeltaDown];
    _statusTriggerDeltaUpLabel.text = sensorCadenceModel == nil ? @"[NULL]" : [NSString stringWithFormat:@"%d", sensorCadenceModel.statusTriggerDeltaUp];
    _statusMinIntervalLabel.text = sensorCadenceModel == nil ? @"[NULL]" : [NSString stringWithFormat:@"%d", sensorCadenceModel.statusMinInterval];
    _fastCadenceLowLabel.text = sensorCadenceModel == nil ? @"[NULL]" : [NSString stringWithFormat:@"%d", sensorCadenceModel.fastCadenceLow];
    _fastCadenceHighLabel.text = sensorCadenceModel == nil ? @"[NULL]" : [NSString stringWithFormat:@"%d", sensorCadenceModel.fastCadenceHigh];
}

@end
