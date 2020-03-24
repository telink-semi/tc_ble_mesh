//
//  SigLowerTransportLayer.h
//  TelinkSigMeshLib
//
//  Created by Liangjiazhi on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class SigSegmentedMessage,SigUpperTransportPdu,SigNetkeyModel,SigSegmentAcknowledgmentMessage;

@interface SigLowerTransportLayer : NSObject

@property (nonatomic,strong) SigNetworkManager *networkManager;
@property (nonatomic,strong) SigDataSource *meshNetwork;
/// The map of incomplete received segments. Every time a Segmented Message is received
/// it is added to the map to an ordered array. When all segments are received
/// they are sent for processing to higher layer.
///
/// The key consists of 16 bits of source address in 2 most significant bytes
/// and `sequenceZero` field in 13 least significant bits.
/// See `UInt32(keyFor:segment)` below.
@property (nonatomic,strong) NSMutableDictionary <NSNumber *,NSMutableArray <SigSegmentedMessage *>*>*incompleteSegments;/*{NSNumber:NSMutableArray <SigSegmentedMessage *>*}*/
/// This map contains Segment Acknowlegment Messages of completed messages.
/// It is used when a complete Segmented Message has been received and the
/// ACK has been sent but failed to reach the source Node.
/// The Node would then resend all non-acknowledged segments and expect a new ACK.
/// Without this map, this layer would have to complete again all segments in
/// order to send the ACK. By checking if a segment comes from an already
/// acknowledged message, it can immediatelly send the ACK again.
///
/// An item is removed when a next message has been received from the same Node.
@property (nonatomic,strong) NSMutableDictionary <NSNumber *,SigSegmentAcknowledgmentMessage *>*acknowledgments;/*{NSNumber:SigSegmentAcknowledgmentMessage}*/
/// The map of active timers. Every message has `defaultIncompleteTimerInterval`
/// seconds to be completed (timer resets when next segment was received).
/// After that time the segments are discarded.
///
/// The key consists of 16 bits of source address in 2 most significant bytes
/// and `sequenceZero` field in 13 least significant bits.
/// See `UInt32(keyFor:sequenceZero)` below.
@property (nonatomic,strong) NSMutableDictionary <NSNumber *,BackgroundTimer *>*incompleteTimers;/*{NSNumber:NSTimer}*/
/// The map of acknowledgment timers. After receiving a segment targetting
/// any of the Unicast Addresses of one of the Elements of the local Node, a
/// timer is started that will send the Segment Acknowledgment Message for
/// segments received until than. The timer is invalidated when the message
/// has been completed.
///
/// The key consists of 16 bits of source address in 2 most significant bytes
/// and `sequenceZero` field in 13 least significant bits.
/// See `UInt32(keyFor:sequenceZero)` below.
@property (nonatomic,strong) NSMutableDictionary <NSNumber *,BackgroundTimer *>*acknowledgmentTimers;/*{NSNumber:NSTimer}*/
/// The map of outgoing segmented messages.
///
/// The key is the `sequenceZero` of the message.
@property (nonatomic,strong) NSMutableDictionary <NSNumber *,NSMutableArray <SigSegmentedMessage *>*>*outgoingSegments;/*{NSNumber:SigSegmentedMessage}*/
/// The map of segment transmission timers. A segment transmission timer
/// for a Segmented Message with `sequenceZero` is started whenever such
/// message is sent to a Unicast Address. After the timer expires, the
/// layer will resend all non-confirmed segments and reset the timer.
///
/// The key is the `sequenceZero` of the message.
@property (nonatomic,strong) NSMutableDictionary <NSNumber *,BackgroundTimer *>*segmentTransmissionTimers;/*{NSNumber:NSTimer}*/

/// The initial TTL values.
///
/// The key is the `sequenceZero` of the message.
@property (nonatomic,strong) NSMutableDictionary <NSNumber *,NSNumber *>*segmentTtl;/*{NSNumber(uint16):NSNumber(uint8)}*/

- (instancetype)initWithNetworkManager:(SigNetworkManager *)networkManager;

/// This method handles the received Network PDU. If needed, it will reassembly
/// the message, send block acknowledgment to the sender, and pass the Upper
/// Transport PDU to the Upper Transport Layer.
///
/// - parameter networkPdu: The Network PDU received.
- (void)handleNetworkPdu:(SigNetworkPdu *)networkPdu;

/// This method tries to send the Upper Transport Message.
///
/// - parameters:
///   - pdu:        The unsegmented Upper Transport PDU to be sent.
///   - initialTtl: The initial TTL (Time To Live) value of the message.
///                 If `nil`, the default Node TTL will be used.
///   - networkKey: The Network Key to be used to encrypt the message on
///                 on Network Layer.
- (void)sendUnsegmentedUpperTransportPdu:(SigUpperTransportPdu *)pdu withTtl:(UInt8)initialTtl usingNetworkKey:(SigNetkeyModel *)networkKey;

/// This method tries to send the Upper Transport Message.
///
/// - parameter pdu:        The segmented Upper Transport PDU to be sent.
/// - parameter initialTtl: The initial TTL (Time To Live) value of the message.
///                         If `nil`, the default Node TTL will be used.
/// - parameter networkKey: The Network Key to be used to encrypt the message on
///                         on Network Layer.
- (void)sendSegmentedUpperTransportPdu:(SigUpperTransportPdu *)pdu withTtl:(UInt8)initialTtl usingNetworkKey:(SigNetkeyModel *)networkKey;

/// Cancels sending segmented Upper Transoprt PDU.
///
/// - parameter pdu: The Upper Transport PDU.
- (void)cancelSendingSegmentedUpperTransportPdu:(SigUpperTransportPdu *)pdu;

///// This method handles the Unprovisioned Device Beacon.
/////
///// The curernt implementation does nothing, as remote provisioning is
///// currently not supported.
/////
///// - parameter unprovisionedDeviceBeacon: The Unprovisioned Device Beacon received.
//- (void)handleUnprovisionedDeviceBeacon:(SigUnprovisionedDeviceBeacon *)unprovisionedDeviceBeacon;

///// This method handles the Secure Network Beacon.
///// It will set the proper IV Index and IV Update Active flag for the Network Key
///// that matches Network ID and change the Key Refresh Phase based on the
///// key refresh flag specified in the beacon.
/////
///// - parameter secureNetworkBeacon: The Secure Network Beacon received.
//- (void)handleSecureNetworkBeacon:(SigSecureNetworkBeacon *)secureNetworkBeacon;





///// This method tries to send the Upper Transport Message.
/////
///// - parameter pdu:         The Upper Transport PDU to be sent.
///// - parameter isSegmented: `True` if the message should be sent as segmented, `false` otherwise.
///// - parameter networkKey:  The Network Key to be used to encrypt the message on
/////                          on Network Layer.
//- (void)sendUpperTransportPdu:(SigUpperTransportPdu *)pdu asSegmentedMessage:(BOOL)isSegmented usingNetworkKey:(SigNetkeyModel *)networkKey;

///// This method tries to send the Segment Acknowledgment Message to the
///// given address. It will try to send if the local Provisioner is set and
///// has the Unicast Address assigned.
/////
///// If the `transporter` throws an error during sending, this error will be ignored.
/////
///// - parameter segments:   The array of message segments, of which at least one
/////                         has to be not `nil`.
///// - parameter networkKey: The Network Key to be used to encrypt the message on
/////                         on Network Layer.
///// - parameter ttl:        Initial Time To Live (TTL) value.
//- (void)sendAckForSegments:(NSArray <SigSegmentedMessage *>*)segments usingNetworkKey:(SigNetkeyModel *)networkKey withTtl:(UInt8)ttl;

///// Sends all non-`nil` segments from `outgoingSegments` map from the given
///// `sequenceZero` key.
/////
///// - parameter sequenceZero: The key to get segments from the map.
//- (void)sendSegmentsForSequenceZero:(UInt16)sequenceZero limit:(int)limit;
//
//- (void)sendSegmentsForSequenceZero:(UInt16)sequenceZero;//limit=10


@end

NS_ASSUME_NONNULL_END
