/********************************************************************************************************
* @file     SigSegmentAcknowledgmentMessage.h
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
//  SigSegmentAcknowledgmentMessage.h
//  TelinkSigMeshLib
//
//  Created by Liangjiazhi on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigLowerTransportPdu.h"

NS_ASSUME_NONNULL_BEGIN

@class SigSegmentedMessage;

@interface SigSegmentAcknowledgmentMessage : SigLowerTransportPdu

/// Message Op Code.
@property (nonatomic,assign) UInt8 opCode;
/// Flag set to `true` if the message was sent by a Friend
/// on behalf of a Low Power node.
@property (nonatomic,assign) BOOL isOnBehalfOfLowPowerNode;
/// Block acknowledgment for segments, bit field.
@property (nonatomic,assign) UInt32 blockAck;
@property (nonatomic,assign) UInt16 sequenceZero;
@property (nonatomic,assign) UInt8 segmentOffset;
@property (nonatomic,assign) UInt8 lastSegmentNumber;

/// Creates the Segmented Acknowledgement Message from the given Network PDU.
/// If the PDU is not valid, it will return `nil`.
///
/// - parameter networkPdu: The Network PDU received.
- (instancetype)initFromNetworkPdu:(SigNetworkPdu *)networkPdu;

/// Creates the ACK for given array of segments. At least one of
/// segments must not be `nil`.
///
/// - parameter segments: The list of segments to be acknowledged.
- (instancetype)initForSegments:(NSArray <SigSegmentedMessage *>*)segments;

/// Returns whether the segment with given index has been received.
///
/// - parameter m: The segment number.
/// - returns: `True`, if the segment of the given number has been
///            acknowledged, `false` otherwise.
- (BOOL)isSegmentReceived:(int)m;

/// Returns whether all segments have been received.
///
/// - parameter segments: The array of segments received and expected.
/// - returns: `True` if all segments were received, `false` otherwise.
- (BOOL)areAllSegmentsReceivedOfSegments:(NSArray <SigSegmentedMessage *>*)segments;

/// Returns whether all segments have been received.
///
/// - parameter lastSegmentNumber: The number of the last expected
///             segments (segN).
/// - returns: `True` if all segments were received, `false` otherwise.
- (BOOL)areAllSegmentsReceivedLastSegmentNumber:(UInt8)lastSegmentNumber;

/// Whether the source Node is busy and the message should be cancelled, or not.
- (BOOL)isBusy;

- (NSData *)transportPdu;

@end

NS_ASSUME_NONNULL_END
