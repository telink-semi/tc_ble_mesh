/********************************************************************************************************
* @file     SigSensorDescriptorModel.h
*
* @brief    for TLSR chips
*
* @author     telink
* @date     Sep. 30, 2010
*
* @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
*           All rights reserved.
*
*             The information contained herein is confidential and proprietary property of Telink
*              Semiconductor (Shanghai) Co., Ltd. and is available under the terms
*             of Commercial License Agreement between Telink Semiconductor (Shanghai)
*             Co., Ltd. and the licensee in separate contract or the terms described here-in.
*           This heading MUST NOT be removed from this file.
*
*              Licensees are granted free, non-transferable use of the information in this
*             file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
*
*******************************************************************************************************/
//
//  SigSensorDescriptorModel.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/11/25.
//  Copyright © 2019 梁家誌. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface SigSensorDescriptorModel : NSObject

/// The Sensor Property ID field is a 2-octet value referencing a device property that describes the meaning and the format of data reported by a sensor.(0x0001–0xFFFF)
@property (nonatomic, assign) UInt16 propertyID;
/// The Sensor Positive Tolerance field is a 12-bit value representing the magnitude of a possible positive error associated with the measurements that the sensor is reporting.(0x001–0xFFF)
@property (nonatomic, assign) UInt16 positiveTolerance;
/// The Sensor Negative Tolerance field is a 12-bit value representing the magnitude of a possible negative error associated with the measurements that the sensor is reporting.(0x001–0xFFF)
@property (nonatomic, assign) UInt16 negativeTolerance;
/// This Sensor Sampling Function field specifies the averaging operation or type of sampling function applied to the measured value.(0x00–0x07)
@property (nonatomic, assign) SigSensorSamplingFunctionType samplingFunction;
/// This Sensor Measurement Period field specifies a uint8 value n that represents the averaging time span, accumulation time, or measurement period in seconds over which the measurement is taken.(0x00–0xFF)
@property (nonatomic, assign) UInt8 measurementPeriod;
/// The measurement reported by a sensor is internally refreshed at the frequency indicated in the Sensor Update Interval field.(0x00–0xFF)
@property (nonatomic, assign) UInt8 updateInterval;

- (NSData *)getDescriptorParameters;
- (instancetype)initWithDescriptorParameters:(NSData *)parameters;

@end

NS_ASSUME_NONNULL_END
