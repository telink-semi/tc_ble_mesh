/********************************************************************************************************
 * @file     SigHearbeatMessage.m
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
//  SigHearbeatMessage.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigHearbeatMessage.h"
#import "SigControlMessage.h"

@implementation SigHearbeatMessage

- (instancetype)initFromControlMessage:(SigControlMessage *)message {
    if (self = [super init]) {
        _opCode = message.opCode;
        NSData *data = message.upperTransportPdu;
        if (_opCode != 0x0A || data.length != 3) {
            TeLogError(@"SigHearbeatMessage initFromControlMessage fail.");
            return nil;
        }
        Byte *dataByte = (Byte *)data.bytes;
        UInt8 tem = 0;
        memcpy(&tem, dataByte, 1);
        _initTtl = tem & 0x7F;
        UInt8 tem1 = 0,tem2 = 0;
        memcpy(&tem1, dataByte+1, 1);
        memcpy(&tem2, dataByte+2, 1);
        _features.value = ((UInt16)tem1 << 8) | (UInt16)tem2;
        _source = message.source;
        _destination = message.destination;
    }
    return self;
}

- (instancetype)initWithInitialTtl:(UInt8)ttl features:(SigFeatures)features fromSource:(UInt16)source targettingDestination:(UInt16)destination {
    if (self = [super init]) {
        _opCode = 0x0A;
        _initTtl = ttl;
        _features = features;
        _source = source;
        _destination = destination;
    }
    return self;
}

@end
