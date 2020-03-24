/********************************************************************************************************
 * @file     SigTransitionTime.h
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
//  SigTransitionTime.h
//  SigMeshLib
//
//  Created by Liangjiazhi on 2019/9/6.
//  Copyright © 2019 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface SigTransitionTime : NSObject
/// Transition Number of Steps, 6-bit value.
///
/// Value 0 indicates an immediate transition.
///
/// Value 0x3F means that the value is unknown. The state cannot be
/// set to this value, but an element may report an unknown value if
/// a transition is higher than 0x3E or not determined.
@property (nonatomic,assign) UInt8 steps;

/// The step resolution.
@property (nonatomic,assign) SigStepResolution stepResolution;

/// The transition time in milliseconds.
@property (nonatomic,assign) int milliseconds;

/// The transition time as `TimeInterval` in seconds.
@property (nonatomic,assign) NSTimeInterval interval;

@property (nonatomic,assign) UInt8 rawValue;


/// Creates the Transition Time object for an unknown time.
- (instancetype)init;

- (instancetype)initWithRawValue:(UInt8)rawValue;

/// Creates the Transition Time object.
///
/// Only values of 0x00 through 0x3E shall be used to specify the value
/// of the Transition Number of Steps field.
///
/// - parameter steps: Transition Number of Steps, valid values are in
///                    range 0...62. Value 63 means that the value is
///                    unknown and the state cannot be set to this value.
/// - parameter stepResolution: The step resolution.
- (instancetype)initWithSetps:(UInt8)steps stepResolution:(SigStepResolution)stepResolution;

@end

NS_ASSUME_NONNULL_END
