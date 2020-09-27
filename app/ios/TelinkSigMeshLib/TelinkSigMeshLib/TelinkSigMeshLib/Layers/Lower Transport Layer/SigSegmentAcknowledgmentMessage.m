/********************************************************************************************************
* @file     SigSegmentAcknowledgmentMessage.m
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
//  SigSegmentAcknowledgmentMessage.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigSegmentAcknowledgmentMessage.h"
#import "SigSegmentedMessage.h"

@implementation SigSegmentAcknowledgmentMessage

- (instancetype)init {
    if (self = [super init]) {
        self.type = SigLowerTransportPduType_controlMessage;
    }
    return self;
}

- (instancetype)initFromNetworkPdu:(SigNetworkPdu *)networkPdu {
    if (self = [super init]) {
        self.type = SigLowerTransportPduType_controlMessage;
        NSData *data = networkPdu.transportPdu;
        Byte *dataByte = (Byte *)data.bytes;
        UInt8 tem = 0;
        memcpy(&tem, dataByte, 1);
        if (data.length != 7 || (tem & 0x80) != 0) {
            TeLogError(@"initFromUnsegmentedPdu fail.");
            return nil;
        }
        _opCode = tem & 0x7F;
        if (_opCode != 0) {
            TeLogError(@"initFromUnsegmentedPdu fail.");
            return nil;
        }
        UInt8 tem1 = 0,tem2=0;
        memcpy(&tem1, dataByte+1, 1);
        memcpy(&tem2, dataByte+2, 1);
        UInt32 tem3 = 0;
        memcpy(&tem3, dataByte+3, 4);
        _isOnBehalfOfLowPowerNode = (tem1 & 0x80) != 0;
        _sequenceZero = (UInt16)(((tem1 & 0x7F) << 6) | (UInt16)tem2 >> 2);
        _blockAck = CFSwapInt32HostToBig(tem3);
        UInt32 byte = CFSwapInt32HostToBig(tem3);
        self.upperTransportPdu = [NSData dataWithBytes:&byte length:4];
        self.source = networkPdu.source;
        self.destination = networkPdu.destination;
        self.networkKey = networkPdu.networkKey;
    }
    return self;
}

- (instancetype)initForSegments:(NSArray <SigSegmentedMessage *>*)segments {
    if (self = [super init]) {
        self.type = SigLowerTransportPduType_controlMessage;
        _opCode = 0x00;
        _isOnBehalfOfLowPowerNode = false;// Friendship is not supported.
        SigSegmentedMessage *segment = segments.firstObject;
        if (segments == nil || ![segment isKindOfClass:SigSegmentedMessage.class]) {
            for (SigSegmentedMessage *tem in segments) {
                if (tem != nil && ![tem isEqual:[NSNull null]] && [tem isKindOfClass:SigSegmentedMessage.class]) {
                    segment = tem;
                    break;
                }
            }
        }
        _sequenceZero = segment.sequenceZero;
        UInt32 ack = 0;
        for (SigSegmentedMessage *seg in segments) {
            if (seg != nil && ![seg isEqual:[NSNull null]]) {
                ack |= 1 << seg.segmentOffset;
            }
        }
        _blockAck = ack;
        UInt32 bigAck = CFSwapInt32HostToBig(ack);
        self.upperTransportPdu = [NSData dataWithBytes:&bigAck length:4];
        TeLogInfo(@"node response last segment,send response is acknowledged.ack.blockAck=0x%x,sequenceZero=0x%X,upperTransportPdu=%@",ack,_sequenceZero,self.upperTransportPdu);
        // Assuming all segments have the same source and destination addresses and network key.
        // Swaping source with destination. Destination here is guaranteed to be a Unicast Address.
        self.source = segment.destination;
        self.destination = segment.source;
        self.networkKey = segment.networkKey;
    }
    return self;
}

- (BOOL)isSegmentReceived:(int)m {
    return (_blockAck & (1<<m)) != 0;
}

- (BOOL)areAllSegmentsReceivedOfSegments:(NSArray <SigSegmentedMessage *>*)segments {
    return [self areAllSegmentsReceivedLastSegmentNumber:segments.count - 1];
}

- (BOOL)areAllSegmentsReceivedLastSegmentNumber:(UInt8)lastSegmentNumber {
    return _blockAck == ((1 << (_lastSegmentNumber + 1)) - 1);
}

- (BOOL)isBusy {
    return _blockAck == 0;
}

- (NSData *)transportPdu {
    UInt8 octet0 = (UInt8)(_opCode & 0x7F);
    UInt8 octet1 = (UInt8)(_isOnBehalfOfLowPowerNode ? 0x80 : 0x00) | (UInt8)(_sequenceZero >> 6);
    UInt8 octet2 = (UInt8)((_sequenceZero & 0x3F) << 2);
    NSMutableData *mData = [NSMutableData data];
    [mData appendData:[NSData dataWithBytes:&octet0 length:1]];
    [mData appendData:[NSData dataWithBytes:&octet1 length:1]];
    [mData appendData:[NSData dataWithBytes:&octet2 length:1]];
    [mData appendData:self.upperTransportPdu];
    return mData;
}

- (NSString *)description {
    return [NSString stringWithFormat:@"<%p> - SigSegmentAcknowledgmentMessage, source:(0x%X), destination:(0x%X) type:(%@), sequenceZero:(0x%X), opCode:(0x%X), isOnBehalfOfLowPowerNode:%d,blockAck:(0x%X),segmentOffset:(0x%X),lastSegmentNumber:(0x%X)", self, self.source,self.destination,self.type==SigLowerTransportPduType_accessMessage?@"accessMessage":@"controlMessage",_sequenceZero,_opCode,_isOnBehalfOfLowPowerNode,(unsigned int)_blockAck,_segmentOffset,_lastSegmentNumber];
}

@end
