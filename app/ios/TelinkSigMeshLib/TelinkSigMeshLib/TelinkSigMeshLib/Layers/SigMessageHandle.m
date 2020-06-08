/********************************************************************************************************
* @file     SigMessageHandle.m
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
//  SigMessageHandle.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/10/25.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigMessageHandle.h"

@implementation SigMessageHandle

- (instancetype)initForMessage:(SigMeshMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination usingManager:(SigMeshLib *)manager {
    if (self = [super init]) {
        _opCode = message.opCode;
        _source = source;
        _destination = destination;
        _manager = manager;
    }
    return self;
}

/// Cancels sending the message.
///
/// Only segmented or acknowledged messages may be cancelled.
/// Unsegmented unacknowledged messages are sent almost instantaneously
/// (depending on the connection interval and message size)
/// and therefore cannot be cancelled.
- (void)cancel {
    [_manager cancelSigMessageHandle:self];
}

@end
