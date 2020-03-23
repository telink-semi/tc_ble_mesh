//
//  SigSegmentedControlMessage.m
//  TelinkSigMeshLib
//
//  Created by Liangjiazhi on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigSegmentedControlMessage.h"

@implementation SigSegmentedControlMessage

- (instancetype)init {
    if (self = [super init]) {
        self.type = SigLowerTransportPduType_controlMessage;
    }
    return self;
}

- (instancetype)initFromSegmentedPdu:(SigNetworkPdu *)networkPdu {
    if (self = [super init]) {
        self.type = SigLowerTransportPduType_controlMessage;
        NSData *data = networkPdu.transportPdu;
        Byte *dataByte = (Byte *)data.bytes;
        UInt8 tem = 0;
        memcpy(&tem, dataByte, 1);
        if (data.length < 5 || (tem & 0x80) == 0) {
            TeLogError(@"initFromUnsegmentedPdu fail.");
            return nil;
        }
        _opCode = tem & 0x7F;
        if (_opCode == 0) {
            TeLogError(@"initFromUnsegmentedPdu fail.");
            return nil;
        }
        UInt8 tem1 = 0,tem2=0,tem3=0;
        memcpy(&tem1, dataByte+1, 1);
        memcpy(&tem2, dataByte+2, 1);
        memcpy(&tem3, dataByte+3, 1);
        self.sequenceZero = (UInt16)(((tem1 & 0x7F) << 6) | (UInt16)tem2 >> 2);
        self.segmentOffset = ((tem2 & 0x03) << 3) | ((tem3 & 0xE0) >> 5);
        self.lastSegmentNumber = tem3 & 0x1F;
        if (self.segmentOffset > self.lastSegmentNumber) {
            TeLogError(@"initFromUnsegmentedPdu fail.");
            return nil;
        }
        self.upperTransportPdu = [data subdataWithRange:NSMakeRange(4, data.length-4)];
        self.source = networkPdu.source;
        self.destination = networkPdu.destination;
        self.networkKey = networkPdu.networkKey;
    }
    return self;
}

- (NSData *)transportPdu {
    UInt8 octet0 = 0x80 | (_opCode & 0x7F);// SEG = 1
    UInt8 octet1 = (UInt8)(self.sequenceZero >> 5);
    UInt8 octet2 = (UInt8)((self.sequenceZero & 0x3F) << 2) | (self.segmentOffset >> 3);
    UInt8 octet3 = ((self.segmentOffset & 0x07) << 5) | (self.lastSegmentNumber & 0x1F);
    NSMutableData *mData = [NSMutableData data];
    [mData appendData:[NSData dataWithBytes:&octet0 length:1]];
    [mData appendData:[NSData dataWithBytes:&octet1 length:1]];
    [mData appendData:[NSData dataWithBytes:&octet2 length:1]];
    [mData appendData:[NSData dataWithBytes:&octet3 length:1]];
    [mData appendData:self.upperTransportPdu];
    return mData;
}

@end
