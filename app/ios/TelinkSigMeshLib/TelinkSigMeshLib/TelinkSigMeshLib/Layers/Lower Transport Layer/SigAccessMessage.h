/********************************************************************************************************
* @file     SigAccessMessage.h
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
//  SigAccessMessage.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigLowerTransportPdu.h"

NS_ASSUME_NONNULL_BEGIN

@class SigSegmentedAccessMessage,SigUpperTransportPdu;

@interface SigAccessMessage : SigLowerTransportPdu

/// Application Key Flag
@property (nonatomic,assign) BOOL AKF;

/// 6-bit Application Key identifier. This field is set to `nil`
/// if the message is signed with a Device Key instead.
@property (nonatomic,assign) UInt8 aid;
/// The sequence number used to encode this message.
@property (nonatomic,assign) UInt32 sequence;
/// The size of Transport MIC: 4 or 8 bytes.
@property (nonatomic,assign) UInt8 transportMicSize;

/// Creates an Access Message from a Network PDU that contains
/// an unsegmented access message. If the PDU is invalid, the
/// init returns `nil`.
///
/// - parameter networkPdu: The received Network PDU with unsegmented
///                         Upper Transport message.
- (instancetype)initFromUnsegmentedPdu:(SigNetworkPdu *)networkPdu;

/// Creates an Access Message object from the given list of segments.
///
/// - parameter segments: List of ordered segments.
- (instancetype)initFromSegments:(NSArray <SigSegmentedAccessMessage *>*)segments;

/// Creates an Access Message object from the Upper Transport PDU.
///
/// - parameter pdu: The Upper Transport PDU.
/// - parameter networkKey: The Network Key to encrypt the PCU with.
- (instancetype)initFromUnsegmentedUpperTransportPdu:(SigUpperTransportPdu *)pdu usingNetworkKey:(SigNetkeyModel *)networkKey;

- (NSData *)transportPdu;

@end

NS_ASSUME_NONNULL_END
