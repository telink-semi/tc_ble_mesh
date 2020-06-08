/********************************************************************************************************
* @file     SigControlMessage.h
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
//  SigControlMessage.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigLowerTransportPdu.h"

NS_ASSUME_NONNULL_BEGIN

@class SigSegmentedAccessMessage,SigNetworkPdu;

@interface SigControlMessage : SigLowerTransportPdu

/// Message Op Code.
@property (nonatomic,assign) UInt8 opCode;

/// Creates an Control Message from a Network PDU that contains
/// an unsegmented control message. If the PDU is invalid, the
/// init returns `nil`.
///
/// - parameter networkPdu: The received Network PDU with unsegmented
///                         Upper Transport message.
- (instancetype)initFromNetworkPdu:(SigNetworkPdu *)networkPdu;

/// Creates an Control Message object from the given list of segments.
///
/// - parameter segments: List of ordered segments.
- (instancetype)initFromSegments:(NSArray <SigSegmentedAccessMessage *>*)segments;

/// Creates a Control Message from the given Proxy Configuration
/// message. The source should be set to the local Node address.
/// The given Network Key should be known to the Proxy Node.
///
/// - parameter message:    The message to be sent.
/// - parameter source:     The address of the local Node.
/// - parameter networkKey: The Network Key to signe the message with.
///                         The key should be known to the connected
///                         Proxy Node.
- (instancetype)initFromProxyConfigurationMessage:(SigProxyConfigurationMessage *)message sentFromSource:(UInt16)source usingNetworkKey:(SigNetkeyModel *)networkKey;

- (NSData *)transportPdu;

@end

NS_ASSUME_NONNULL_END
