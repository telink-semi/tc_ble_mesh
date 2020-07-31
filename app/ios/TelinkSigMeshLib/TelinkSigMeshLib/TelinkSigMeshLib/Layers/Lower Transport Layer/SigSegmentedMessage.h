/********************************************************************************************************
* @file     SigSegmentedMessage.h
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
//  SigSegmentedMessage.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigLowerTransportPdu.h"

NS_ASSUME_NONNULL_BEGIN

@interface SigSegmentedMessage : SigLowerTransportPdu
/// The Mesh Message that is being sent, or `nil`, when the message
/// was received.
@property (nonatomic,strong,nullable) SigMeshMessage *message;
/// The local Element used to send the message.
@property (nonatomic,strong,nullable) SigElementModel *localElement;
/// Whether sending this message has been initiated by the user.
@property (nonatomic,assign) BOOL userInitiated;
/// 13 least significant bits of SeqAuth.
@property (nonatomic,assign) UInt16 sequenceZero;
/// This field is set to the segment number (zero-based)
/// of the segment m of this Upper Transport PDU.
@property (nonatomic,assign) UInt8 segmentOffset;
/// This field is set to the last segment number (zero-based)
/// of this Upper Transport PDU.
@property (nonatomic,assign) UInt8 lastSegmentNumber;

/// Returns whether the message is composed of only a single
/// segment. Single segment messages are used to send short,
/// acknowledged messages. The maximum size of payload of upper
/// transport control PDU is 8 bytes.
- (BOOL)isSingleSegment;

/// Returns the `segmentOffset` as `Int`.
- (int)index;

/// Returns the expected number of segments for this message.
- (int)count;

@end

NS_ASSUME_NONNULL_END
