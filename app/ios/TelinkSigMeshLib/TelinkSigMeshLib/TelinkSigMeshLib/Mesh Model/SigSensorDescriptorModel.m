/********************************************************************************************************
* @file     SigSensorDescriptorModel.m
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
//  SigSensorDescriptorModel.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/11/25.
//  Copyright © 2019 梁家誌. All rights reserved.
//

#import "SigSensorDescriptorModel.h"

@implementation SigSensorDescriptorModel

- (instancetype)initWithDescriptorParameters:(NSData *)parameters {
    if (self = [super init]) {
        if (parameters != nil && (parameters.length == 2 || parameters.length == 8)) {
            UInt16 tem16 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem16, dataByte, 2);
            _propertyID = tem16;
            if (parameters.length > 2) {
                memcpy(&tem16, dataByte+2, 2);
                _positiveTolerance = tem16;
                memcpy(&tem16, dataByte+4, 2);
                _negativeTolerance = tem16;
                UInt8 tem8 = 0;
                memcpy(&tem8, dataByte+5, 1);
                _samplingFunction = tem8;
                memcpy(&tem8, dataByte+6, 1);
                _measurementPeriod = tem8;
                memcpy(&tem8, dataByte+7, 1);
                _updateInterval = tem8;
            }
        }else{
            return nil;
        }
    }
    return self;
}

- (NSData *)getDescriptorParameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _propertyID;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    if (_positiveTolerance != 0) {
        tem16 = _positiveTolerance;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        tem16 = _negativeTolerance;
        data = [NSData dataWithBytes:&tem16 length:2];
        [mData appendData:data];
        UInt8 tem8 = _samplingFunction;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _measurementPeriod;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
        tem8 = _updateInterval;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    return mData;
}

@end
