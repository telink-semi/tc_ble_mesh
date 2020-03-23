//
//  SigModelDelegate.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/10/29.
//  Copyright © 2019 梁家誌. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol SigModelDelegate <NSObject>

/// A map of mesh message types that the associated Model may receive
/// and handle. It should not contain types of messages that this
/// Model only sends. Items of this map are used to instantiate a
/// message when an Access PDU with given opcode is received.
///
/// The key in the map should be the opcode and the value
/// the message type supported by the handler.
@property (nonatomic,strong) NSMutableDictionary <NSNumber *,NSNumber *>*messageTypes;//[UInt32 : MeshMessage.Type]

/// This method should handle the received Acknowledged Message.
///
/// - parameters:
///   - model: The Model associated with this Model Delegate.
///   - request: The Acknowledged Message received.
///   - source:  The source Unicast Address.
///   - destination: The destination address of the request.
/// - returns: The response message to be sent to the sender.
- (SigMeshMessage *)getModel:(SigModelIDModel *)model didReceiveAcknowledgedMessage:(SigAcknowledgedMeshMessage *)request fromSource:(UInt16)source sentToDestination:(SigMeshAddress *)destination;

/// This method should handle the received Unacknowledged Message.
///
/// - parameters:
///   - model: The Model associated with this Model Delegate.
///   - message: The Unacknowledged Message received.
///   - source: The source Unicast Address.
///   - destination: The destination address of the request.
- (void)model:(SigModelIDModel *)model didReceiveUnacknowledgedMessage:(SigAcknowledgedMeshMessage *)request fromSource:(UInt16)source sentToDestination:(SigMeshAddress *)destination;

/// This method should handle the received response to the
/// previously sent request.
///
/// - parameters:
///   - model: The Model associated with this Model Delegate.
///   - response: The response received.
///   - request: The Acknowledged Message sent.
///   - source: The Unicast Address of the Element that sent the
///             response.
- (void)model:(SigModelIDModel *)model didReceiveResponse:(SigMeshMessage *)response toAcknowledgedMessage:(SigAcknowledgedMeshMessage *)request fromSource:(UInt16)source;

/// This method tries to decode the Access PDU into a Message.
///
/// The Model Handler must support the opcode and specify to
/// which type should the message be decoded.
///
/// - parameter accessPdu: The Access PDU received.
/// - returns: The decoded message, or `nil`, if the message
///            is not supported or invalid.
- (SigMeshMessage *)decodeSigAccessPdu:(SigAccessPdu *)accessPdu;

/// This method handles the decoded message and passes it to
/// the proper handle method, depending on its type or whether
/// it is a response to a previously sent request.
///
/// - parameters:
///   - model:   The local Model that received the message.
///   - message: The decoded message.
///   - source:  The Unicast Address of the Element that the message
///              originates from.
///   - destination: The destination address of the request.
///   - request: The request message sent previously that this message
///              replies to, or `nil`, if this is not a response.
/// - returns: The response message, if the received message is an
///            Acknowledged Mesh Message that needs to be replied.
- (SigMeshMessage *)getModel:(SigModelIDModel *)model didReceiveMeshMessage:(SigMeshMessage *)message fromSource:(UInt16)source sentToDestination:(SigMeshAddress *)destination asResponseTo:(SigAcknowledgedMeshMessage *)request;

@end

NS_ASSUME_NONNULL_END
