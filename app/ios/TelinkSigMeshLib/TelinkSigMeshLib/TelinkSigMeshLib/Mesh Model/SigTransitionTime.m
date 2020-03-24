/********************************************************************************************************
 * @file     SigTransitionTime.m
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
//  SigTransitionTime.m
//  SigMeshLib
//
//  Created by Liangjiazhi on 2019/9/6.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigTransitionTime.h"

@implementation SigTransitionTime

- (instancetype)init {
    if (self = [super init]) {
        _steps = 0x3F;
        _stepResolution = SigStepResolution_hundredsOfMilliseconds;
    }
    return self;
}

- (instancetype)initWithRawValue:(UInt8)rawValue {
    if (self = [super init]) {
        _steps = rawValue & 0x3F;
        _stepResolution = (SigStepResolution)rawValue >> 6;
    }
    return self;
}

- (instancetype)initWithSetps:(UInt8)steps stepResolution:(SigStepResolution)stepResolution {
    if (self = [super init]) {
        _steps = MIN(steps, 0x3E);
        _stepResolution = stepResolution;
    }
    return self;
}

- (int)milliseconds {
    return [SigHelper.share getPeriodFromSteps:_steps & 0x3F];
}

- (NSTimeInterval)interval {
    return (NSTimeInterval)[self milliseconds]/1000.0;
}

- (UInt8)rawValue {
    return (_steps & 0x3F) | (_stepResolution << 6);
}

/// Returns whether the transition time is known.
- (BOOL)isKnown {
    return _steps < 0x3F;
}

@end
