//
//  SigAccessMessage.m
//  TelinkSigMeshLib
//
//  Created by Liangjiazhi on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigAccessMessage.h"
#import "SigSegmentedAccessMessage.h"
#import "SigUpperTransportPdu.h"

@implementation SigAccessMessage

- (instancetype)init {
    if (self = [super init]) {
        self.type = SigLowerTransportPduType_accessMessage;
    }
    return self;
}

- (instancetype)initFromUnsegmentedPdu:(SigNetworkPdu *)networkPdu {
    if (self = [super init]) {
        self.type = SigLowerTransportPduType_accessMessage;
        NSData *data = networkPdu.transportPdu;
        Byte *dataByte = (Byte *)data.bytes;
        UInt8 tem = 0;
        memcpy(&tem, dataByte, 1);
        if (data.length < 6 || (tem & 0x80) != 0) {
            TeLogError(@"initFromUnsegmentedPdu fail.");
            return nil;
        }
        BOOL akf = (tem & 0b01000000) != 0;
        if (akf) {
            _aid = tem & 0x3F;
        } else {
            _aid = 0;
        }
        // For unsegmented messages, the size of the TransMIC is 32 bits.
        _transportMicSize = 4;
        _sequence = networkPdu.sequence;
        self.networkKey = networkPdu.networkKey;
        self.upperTransportPdu = [data subdataWithRange:NSMakeRange(1, data.length-1)];
        self.source = networkPdu.source;
        self.destination = networkPdu.destination;

    }
    return self;
}

- (instancetype)initFromSegments:(NSArray <SigSegmentedAccessMessage *>*)segments {
    if (self = [super init]) {
        self.type = SigLowerTransportPduType_accessMessage;
        // Assuming all segments have the same AID, source and destination addresses and TransMIC.
        SigSegmentedAccessMessage *segment = segments.firstObject;
        _aid = segment.aid;
        _transportMicSize = segment.transportMicSize;
        self.source = segment.source;
        self.destination = segment.destination;
        _sequence = segment.sequence;
        self.networkKey = segment.networkKey;
        
        // Segments are already sorted by `segmentOffset`.
        NSMutableData *mData = [NSMutableData data];
        for (SigSegmentedAccessMessage *msg in segments) {
            [mData appendData:msg.upperTransportPdu];
        }
        self.upperTransportPdu = mData;
    }
    return self;
}

- (instancetype)initFromUnsegmentedUpperTransportPdu:(SigUpperTransportPdu *)pdu usingNetworkKey:(SigNetkeyModel *)networkKey {
    if (self = [super init]) {
        self.type = SigLowerTransportPduType_accessMessage;
        
        _aid = pdu.aid;
        _transportMicSize = 4;
        _sequence = pdu.sequence;
        self.upperTransportPdu = pdu.transportPdu;
        self.source = pdu.source;
        self.destination = pdu.destination;
        self.networkKey = networkKey;
    }
    return self;
}

- (NSData *)transportPdu {
    UInt8 octet0 = 0x00;// SEG = 0
    if (_aid != 0) {
        octet0 |= 0b01000000;// AKF = 1
        octet0 |= _aid;
    }
    NSMutableData *mData = [NSMutableData dataWithBytes:&octet0 length:1];
    [mData appendData:self.upperTransportPdu];
    return mData;
}

@end
