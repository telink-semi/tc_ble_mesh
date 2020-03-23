//
//  SigTimeModel.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/11/20.
//  Copyright © 2019 梁家誌. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/// 5.2.1.2 Time Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.143)

@interface SigTimeModel : NSObject
/// 40 bits, The current TAI time in seconds.
@property (nonatomic, assign) UInt64 TAISeconds;
/// The sub-second time in units of 1/256th second.
@property (nonatomic, assign) UInt8 subSeconds;
/// The estimated uncertainty in 10 millisecond steps.
@property (nonatomic, assign) UInt8 uncertainty;
/// 1 bit, 0 = NO Time Authority, 1 = Time Authority.
@property (nonatomic, assign) UInt8 timeAuthority;
/// 15 bits, Current difference between TAI and UTC in seconds.
@property (nonatomic, assign) UInt16 TAI_UTC_Delta;
/// The local time zone offset in 15-minute increments.
@property (nonatomic, assign) UInt8 timeZoneOffset;

- (instancetype)initWithTAISeconds:(UInt64)TAISeconds subSeconds:(UInt8)subSeconds uncertainty:(UInt8)uncertainty timeAuthority:(UInt8)timeAuthority TAI_UTC_Delta:(UInt16)TAI_UTC_Delta timeZoneOffset:(UInt8)timeZoneOffset;
- (instancetype)initWithParameters:(NSData *)parameters;
- (NSData *)getTimeParameters;

@end

NS_ASSUME_NONNULL_END
