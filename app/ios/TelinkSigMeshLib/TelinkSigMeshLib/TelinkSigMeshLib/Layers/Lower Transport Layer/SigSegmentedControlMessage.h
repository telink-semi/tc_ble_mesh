/********************************************************************************************************
* @file     SigSegmentedControlMessage.h
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
//  SigSegmentedControlMessage.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigSegmentedMessage.h"

NS_ASSUME_NONNULL_BEGIN

@interface SigSegmentedControlMessage : SigSegmentedMessage

/// Message Op Code.
@property (nonatomic,assign) UInt8 opCode;

/// Creates a Segment of an Control Message from a Network PDU that contains
/// a segmented control message. If the PDU is invalid, the
/// init returns `nil`.
///
/// - parameter networkPdu: The received Network PDU with segmented
///                         Upper Transport message.
- (instancetype)initFromSegmentedPdu:(SigNetworkPdu *)networkPdu;

- (NSData *)transportPdu;

@end

NS_ASSUME_NONNULL_END
