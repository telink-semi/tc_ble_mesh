/********************************************************************************************************
* @file     SigControlMessage.m
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
//  SigControlMessage.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigControlMessage.h"
#import "SigSegmentedAccessMessage.h"

@implementation SigControlMessage

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
        if (data.length < 1 || (tem & 0x80) != 0) {
            TeLogError(@"initFromUnsegmentedPdu fail.");
            return nil;
        }
        _opCode = tem & 0x7F;
        self.upperTransportPdu = [data subdataWithRange:NSMakeRange(1, data.length-1)];
        self.source = networkPdu.source;
        self.destination = networkPdu.destination;
        self.networkKey = networkPdu.networkKey;
    }
    return self;
}

- (instancetype)initFromSegments:(NSArray <SigSegmentedAccessMessage *>*)segments {
    if (self = [super init]) {
        self.type = SigLowerTransportPduType_controlMessage;
        // Assuming all segments have the same AID, source and destination addresses and TransMIC.
        SigSegmentedAccessMessage *segment = segments.firstObject;
        _opCode = segment.opCode;
        self.source = segment.source;
        self.destination = segment.destination;
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

- (instancetype)initFromProxyConfigurationMessage:(SigProxyConfigurationMessage *)message sentFromSource:(UInt16)source usingNetworkKey:(SigNetkeyModel *)networkKey {
    if (self = [super init]) {
        self.type = SigLowerTransportPduType_controlMessage;
        _opCode = message.opCode;
        self.source = source;
        self.destination = MeshAddress_unassignedAddress;
        self.networkKey = networkKey;
        self.upperTransportPdu = message.parameters;
    }
    return self;
}

- (NSData *)transportPdu {
    UInt8 tem = _opCode;
    NSMutableData *mData = [NSMutableData dataWithBytes:&tem length:1];
    [mData appendData:self.upperTransportPdu];
    return mData;
}

@end
