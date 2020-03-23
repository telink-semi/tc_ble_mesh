//
//  SigAccessMessage.h
//  TelinkSigMeshLib
//
//  Created by Liangjiazhi on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigLowerTransportPdu.h"

NS_ASSUME_NONNULL_BEGIN

@class SigSegmentedAccessMessage,SigUpperTransportPdu;

@interface SigAccessMessage : SigLowerTransportPdu

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
