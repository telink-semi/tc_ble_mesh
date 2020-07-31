/********************************************************************************************************
* @file     SigMessageHandle.h
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
//  SigMessageHandle.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/10/25.
//  Copyright © 2019 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/// The mesh message handle is returned upon sending a mesh message
/// and allows the message to be cancelled.
///
/// Only segmented or acknowledged messages may be cancelled.
/// Unsegmented unacknowledged messages are sent almost instantaneously
/// (depending on the connection interval and message size)
/// and therefore cannot be cancelled.
///
/// The handle contains information about the message that was sent:
/// its opcode, source and destination addresses.
@interface SigMessageHandle : NSObject
@property (nonatomic,assign) UInt32 opCode;
@property (nonatomic,assign) UInt16 source;
@property (nonatomic,assign) UInt16 destination;
@property (nonatomic,strong) SigMeshLib *manager;

- (instancetype)initForMessage:(SigMeshMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination usingManager:(SigMeshLib *)manager;

/// Cancels sending the message.
///
/// Only segmented or acknowledged messages may be cancelled.
/// Unsegmented unacknowledged messages are sent almost instantaneously
/// (depending on the connection interval and message size)
/// and therefore cannot be cancelled.
- (void)cancel;

@end

NS_ASSUME_NONNULL_END
