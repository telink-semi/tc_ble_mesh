//
//  SigSegmentedControlMessage.h
//  TelinkSigMeshLib
//
//  Created by Liangjiazhi on 2019/9/16.
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
