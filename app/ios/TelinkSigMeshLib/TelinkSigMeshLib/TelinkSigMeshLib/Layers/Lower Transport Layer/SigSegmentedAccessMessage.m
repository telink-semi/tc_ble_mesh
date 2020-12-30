/********************************************************************************************************
* @file     SigSegmentedAccessMessage.m
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
//  SigSegmentedAccessMessage.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigSegmentedAccessMessage.h"
#import "SigUpperTransportPdu.h"

@implementation SigSegmentedAccessMessage

- (instancetype)init {
    if (self = [super init]) {
        self.type = SigLowerTransportPduType_accessMessage;
    }
    return self;
}

- (instancetype)initFromSegmentedPdu:(SigNetworkPdu *)networkPdu {
    if (self = [super init]) {
        self.type = SigLowerTransportPduType_accessMessage;
        NSData *data = networkPdu.transportPdu;
        Byte *dataByte = (Byte *)data.bytes;
        UInt8 tem = 0;
        memcpy(&tem, dataByte, 1);
        if (data.length < 5 || (tem & 0x80) == 0) {
            TeLogError(@"initFromUnsegmentedPdu fail.");
            return nil;
        }
        BOOL akf = (tem & 0b01000000) != 0;
        self.AKF = akf;
        if (akf) {
            _aid = tem & 0x3F;
        } else {
            _aid = 0;
        }
        UInt8 tem1 = 0,tem2=0,tem3=0;
        memcpy(&tem1, dataByte+1, 1);
        memcpy(&tem2, dataByte+2, 1);
        memcpy(&tem3, dataByte+3, 1);
        UInt8 szmic = tem1 >> 7;
        _transportMicSize = szmic == 0 ? 4 : 8;
        self.sequenceZero = (UInt16)(((tem1 & 0x7F) << 6) | (UInt16)(tem2 >> 2));
        self.segmentOffset = ((tem2 & 0x03) << 3) | ((tem3 & 0xE0) >> 5);
        self.lastSegmentNumber = tem3 & 0x1F;
        if (self.segmentOffset > self.lastSegmentNumber) {
            TeLogError(@"initFromUnsegmentedPdu fail.");
            return nil;
        }
        self.upperTransportPdu = [data subdataWithRange:NSMakeRange(4, data.length-4)];
        _sequence = (networkPdu.sequence & 0xFFE000) | (UInt32)self.sequenceZero;
        self.source = networkPdu.source;
        self.destination = networkPdu.destination;
        self.networkKey = networkPdu.networkKey;
        self.message = nil;
        self.localElement = nil;
        self.userInitiated = NO;
    }
    return self;
}

- (instancetype)initFromUpperTransportPdu:(SigUpperTransportPdu *)pdu usingNetworkKey:(SigNetkeyModel *)networkKey ivIndex:(SigIvIndex *)ivIndex offset:(UInt8)offset {
    if (self = [super init]) {
        self.type = SigLowerTransportPduType_accessMessage;
        self.message = pdu.message;
        self.localElement = pdu.localElement;
        self.AKF = pdu.AKF;
        _aid = pdu.aid;
        self.source = pdu.source;
        self.destination = pdu.destination;
        self.networkKey = networkKey;
        self.ivIndex = ivIndex;
        _transportMicSize = pdu.transportMicSize;
        _sequence = pdu.sequence;
        self.sequenceZero = (UInt16)(pdu.sequence & 0x1FFF);
        self.segmentOffset = offset;
        int lowerBound = (int)(offset * (SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength - 3));
        int upperBound = (int)MIN(pdu.transportPdu.length, (int)(offset + 1) * (SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength - 3));
        NSData *segment = [pdu.transportPdu subdataWithRange:NSMakeRange(lowerBound, upperBound-lowerBound)];
        self.lastSegmentNumber = (UInt8)((pdu.transportPdu.length + ((SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength - 3) - 1)) / (SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength - 3)) - 1;
        self.upperTransportPdu = segment;
        self.userInitiated = pdu.userInitiated;
    }
    return self;

}

/// Creates a Segment of an Access Message object from the Upper Transport PDU
/// with given segment offset.
///
/// - parameter pdu: The segmented Upper Transport PDU.
/// - parameter networkKey: The Network Key to encrypt the PCU with.
/// - parameter offset: The segment offset.
- (instancetype)initFromUpperTransportPdu:(SigUpperTransportPdu *)pdu usingNetworkKey:(SigNetkeyModel *)networkKey offset:(UInt8)offset {
    if (self = [super init]) {
        self.type = SigLowerTransportPduType_accessMessage;
        self.message = pdu.message;
        self.localElement = pdu.localElement;
        self.AKF = pdu.AKF;
        _aid = pdu.aid;
        self.source = pdu.source;
        self.destination = pdu.destination;
        self.networkKey = networkKey;
        _transportMicSize = pdu.transportMicSize;
        _sequence = pdu.sequence;
        self.sequenceZero = (UInt16)(pdu.sequence & 0x1FFF);
        self.segmentOffset = offset;
        int lowerBound = (int)(offset * (SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength - 3));
        int upperBound = (int)MIN(pdu.transportPdu.length, (int)(offset + 1) * (SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength - 3));
        NSData *segment = [pdu.transportPdu subdataWithRange:NSMakeRange(lowerBound, upperBound-lowerBound)];
        self.lastSegmentNumber = (UInt8)((pdu.transportPdu.length + ((SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength - 3) - 1)) / (SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength - 3)) - 1;
        self.upperTransportPdu = segment;
    }
    return self;

}

- (NSData *)transportPdu {
    UInt8 octet0 = 0x80;// SEG = 1
    if (self.AKF) {
        octet0 |= 0b01000000;// AKF = 1
    }
    octet0 |= _aid;
    UInt8 octet1 = ((_transportMicSize << 4) & 0x80) | (UInt8)(self.sequenceZero >> 6);
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

- (NSString *)description {
    return[NSString stringWithFormat:@"<%p> - SigSegmentedAccessMessage, aid:(0x%X) transportMicSize:(0x%X) sequence:(0x%X),sequenceZero:(0x%X), opCode:(0x%X)", self, _aid,_transportMicSize,(unsigned int)_sequence,self.sequenceZero,_opCode];
}

@end
