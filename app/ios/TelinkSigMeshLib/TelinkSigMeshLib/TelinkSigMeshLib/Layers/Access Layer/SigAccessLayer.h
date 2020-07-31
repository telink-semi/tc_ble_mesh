/********************************************************************************************************
 * @file     SigAccessLayer.h
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
//  SigAccessLayer.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class SigNetworkManager,SigUpperTransportPdu,SigAccessPdu;

@interface SigAccessLayer : NSObject

- (instancetype)initWithNetworkManager:(SigNetworkManager *)networkManager;

/// This method handles the Upper Transport PDU and reads the Opcode.
/// If the Opcode is supported, a message object is created and sent
/// to the delegate. Otherwise, a generic MeshMessage object is created
/// for the app to handle.
///
/// - parameters:
///   - upperTransportPdu: The decoded Upper Transport PDU.
///   - keySet: The keySet that the message was encrypted with.
- (void)handleUpperTransportPdu:(SigUpperTransportPdu *)upperTransportPdu sentWithSigKeySet:(SigKeySet *)keySet;

- (void)sendMessage:(SigMeshMessage *)message fromElement:(SigElementModel *)element toDestination:(SigMeshAddress *)destination withTtl:(UInt8)initialTtl usingApplicationKey:(SigAppkeyModel *)applicationKey command:(SDKLibCommand *)command;

/// Sends the MeshMessage to the destination. The message is encrypted
/// using given Application Key and a Network Key bound to it.
///
/// Before sending, this method updates the transaction identifier (TID)
/// for message extending `TransactionMessage`.
///
/// - parameters:
///   - message:        The Mesh Message to send.
///   - element:        The source Element.
///   - destination:    The destination Address. This can be any
///                     valid mesh Address.
///   - initialTtl:     The initial TTL (Time To Live) value of the message.
///                     If `nil`, the default Node TTL will be used.
///   - applicationKey: The Application Key to sign the message with.
- (void)sendMessage:(SigMeshMessage *)message fromElement:(SigElementModel *)element toDestination:(SigMeshAddress *)destination withTtl:(UInt8)initialTtl usingApplicationKey:(SigAppkeyModel *)applicationKey;

/// Sends the ConfigMessage to the destination. The message is encrypted
/// using the Device Key which belongs to the target Node, and first
/// Network Key known to this Node.
///
/// - parameters:
///   - message:     The Mesh Config Message to send.
///   - destination: The destination address. This must be a Unicast Address.
///   - initialTtl:  The initial TTL (Time To Live) value of the message.
///                  If `nil`, the default Node TTL will be used.
- (void)sendSigConfigMessage:(SigConfigMessage *)message toDestination:(UInt16)destination withTtl:(UInt16)initialTtl;

/// Replies to the received message, which was sent with the given key set,
/// with the given message.
///
/// - parameters:
///   - origin:      The destination address of the message that the reply is for.
///   - message:     The response message to be sent.
///   - element:     The source Element.
///   - destination: The destination address. This must be a Unicast Address.
///   - keySet:      The set of keys that the message was encrypted with.
- (void)replyToMessageSentToOrigin:(UInt16)origin withMeshMessage:(SigMeshMessage *)message fromElement:(SigElementModel *)element toDestination:(UInt16)destination usingKeySet:(SigKeySet *)keySet;

/// Cancels sending the message with the given handle.
///
/// - parameter handle: The message handle.
- (void)cancelSigMessageHandle:(SigMessageHandle *)handle;

/// This method delivers the received PDU to all Models that support
/// it and are subscribed to the message destination address.
///
/// In general, each Access PDU should be consumed only by one Model
/// in an Element. For example, Generic OnOff Client may send Generic
/// OnOff Set message to the corresponding Server, which can decode it,
/// change its state and reply with Generic OnOff Status message, that
/// will be consumed by the Client.
///
/// However, nothing stop the developers to reuse the same opcode in
/// multiple Models. For example, there may be a Log Model on an Element,
/// which accepts all opcodes supported by other Models on this Element,
/// and logs the received data. The Log Models, instead of decoding the
/// received Access PDU to Generic OnOff Set message, it may decode it as
/// some "Message X" type.
///
/// This method will make sure that each Model will receive a message
/// decoded to the type specified in `messageTypes` in its `ModelDelegate`,
/// but the manager's delegate will be notified with the first message only.
///
/// - parameters:
///   - accessPdu: The Access PDU received.
///   - keySet:    The set of keys that the message was encrypted with.
///   - request:   The previosly sent request message, that the received
///                message responds to, or `nil`, if no request has
///                been sent.
- (void)handleAccessPdu:(SigAccessPdu *)accessPdu sendWithSigKeySet:(SigKeySet *)keySet asResponseToRequest:(SigAcknowledgedMeshMessage *)request;

@end

NS_ASSUME_NONNULL_END
