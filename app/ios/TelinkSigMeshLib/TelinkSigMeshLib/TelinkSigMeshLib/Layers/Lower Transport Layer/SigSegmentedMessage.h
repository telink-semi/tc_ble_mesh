//
//  SigSegmentedMessage.h
//  TelinkSigMeshLib
//
//  Created by Liangjiazhi on 2019/9/16.
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
