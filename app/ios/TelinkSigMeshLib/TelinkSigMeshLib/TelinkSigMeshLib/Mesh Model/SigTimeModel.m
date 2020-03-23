//
//  SigTimeModel.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/11/20.
//  Copyright © 2019 梁家誌. All rights reserved.
//

#import "SigTimeModel.h"

@implementation SigTimeModel

- (instancetype)initWithTAISeconds:(UInt64)TAISeconds subSeconds:(UInt8)subSeconds uncertainty:(UInt8)uncertainty timeAuthority:(UInt8)timeAuthority TAI_UTC_Delta:(UInt16)TAI_UTC_Delta timeZoneOffset:(UInt8)timeZoneOffset {
    if (self = [super init]) {
        _TAISeconds = TAISeconds;
        _subSeconds = subSeconds;
        _uncertainty = uncertainty;
        _timeAuthority = timeAuthority;
        _TAI_UTC_Delta = TAI_UTC_Delta;
        _timeZoneOffset = timeZoneOffset;
    }
    return self;
}

- (instancetype)initWithParameters:(NSData *)parameters {
    if (self = [super init]) {
        UInt64 tem64 = 0;
        UInt16 tem16 = 0;
        UInt8 tem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        if (parameters.length >= 5) {
            memcpy(&tem64, dataByte, 5);
            _TAISeconds = tem64;
        }
        if (parameters.length >= 6) {
            memcpy(&tem8, dataByte+5, 1);
            _subSeconds = tem8;
        }
        if (parameters.length >= 7) {
            memcpy(&tem8, dataByte+6, 1);
            _uncertainty = tem8;
        }
        if (parameters.length >= 9) {
            memcpy(&tem16, dataByte+7, 2);
            _timeAuthority = (tem16 >> 15) & 0b1;
            _TAI_UTC_Delta = tem16 & 0x7FFF;
        }
        if (parameters.length >= 10) {
            memcpy(&tem8, dataByte+9, 1);
            _timeZoneOffset = tem8;
        }
    }
    return self;
}

- (NSData *)getTimeParameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = 0;
    UInt16 tem16 = 0;
    UInt64 tem64 = _TAISeconds & 0xFFFFFFFF;
    NSData *data = [NSData dataWithBytes:&tem64 length:8];
    [mData appendData:[data subdataWithRange:NSMakeRange(0, 5)]];
    tem8 = _subSeconds;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _uncertainty;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem16 = ((UInt16)_timeAuthority << 15) | _subSeconds;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem8 = _timeZoneOffset;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

@end
