//
//  SigSegmentedAccessMessage.h
//  TelinkSigMeshLib
//
//  Created by Liangjiazhi on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigSegmentedMessage.h"

NS_ASSUME_NONNULL_BEGIN

@class SigUpperTransportPdu;

@interface SigSegmentedAccessMessage : SigSegmentedMessage
/// The Application Key identifier.
/// This field is set to `nil` if the message is signed with a
/// Device Key instead.
@property (nonatomic,assign) UInt8 aid;
/// The size of Transport MIC: 4 or 8 bytes.
@property (nonatomic,assign) UInt8 transportMicSize;
/// The sequence number used to encode this message.
@property (nonatomic,assign) UInt32 sequence;

@property (nonatomic,assign) UInt8 opCode;

/// Creates a Segment of an Access Message from a Network PDU that contains
/// a segmented access message. If the PDU is invalid, the
/// init returns `nil`.
///
/// - parameter networkPdu: The received Network PDU with segmented
///                         Upper Transport message.
- (instancetype)initFromSegmentedPdu:(SigNetworkPdu *)networkPdu;

/// Creates a Segment of an Access Message object from the Upper Transport PDU
/// with given segment offset.
///
/// - parameter pdu: The segmented Upper Transport PDU.
/// - parameter networkKey: The Network Key to encrypt the PCU with.
/// - parameter offset: The segment offset.
- (instancetype)initFromUpperTransportPdu:(SigUpperTransportPdu *)pdu usingNetworkKey:(SigNetkeyModel *)networkKey offset:(UInt8)offset;

- (NSData *)transportPdu;

@end

NS_ASSUME_NONNULL_END
