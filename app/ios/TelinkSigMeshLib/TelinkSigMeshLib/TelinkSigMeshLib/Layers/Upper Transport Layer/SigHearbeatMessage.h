/********************************************************************************************************
 * @file     SigHearbeatMessage.h
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
//  SigHearbeatMessage.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class SigControlMessage;

@interface SigHearbeatMessage : NSObject
/// Source Address.
@property (nonatomic,assign) UInt16 source;
/// Destination Address.
@property (nonatomic,assign) UInt16 destination;
/// Message Op Code.
@property (nonatomic,assign) UInt8 opCode;
/// Initial TTL used when sending the message.
@property (nonatomic,assign) UInt8 initTtl;
/// Currently active features of the node.
@property (nonatomic,assign) SigFeatures features;

- (instancetype)initFromControlMessage:(SigControlMessage *)message;

/// Creates a Heartbeat message.
///
/// - parameter ttl:         Initial TTL used when sending the message.
/// - parameter features:    Currently active features of the node.
/// - parameter source:      The source address.
/// - parameter destination: The destination address.
- (instancetype)initWithInitialTtl:(UInt8)ttl features:(SigFeatures)features fromSource:(UInt16)source targettingDestination:(UInt16)destination;

@end

NS_ASSUME_NONNULL_END
