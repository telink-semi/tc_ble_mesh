/********************************************************************************************************
 * @file     SigNetworkManager.h
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
//  SigNetworkManager.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/8/15.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SigMeshMessage.h"
#import "SigBearer.h"


NS_ASSUME_NONNULL_BEGIN

@class SigMeshLib,SigNodeModel,SigAppkeyModel,SigConfigMessage,SigNetworkLayer,SigLowerTransportLayer,SigUpperTransportLayer,SigAccessLayer,SigMessageHandle,SigKeySet,SigProxyConfigurationMessage;

@interface SigNetworkManager : NSObject
@property (nonatomic,strong) SigMeshLib *manager;

#pragma mark - Layers

@property (nonatomic,strong) SigNetworkLayer *networkLayer;
@property (nonatomic,strong) SigLowerTransportLayer *lowerTransportLayer;
@property (nonatomic,strong) SigUpperTransportLayer *upperTransportLayer;
@property (nonatomic,strong) SigAccessLayer *accessLayer;

#pragma mark - Computed properties

//- (SigDataSource *)meshNetwork;

- (UInt8)defaultTtl;

- (NSTimeInterval)incompleteMessageTimeout;

//- (NSTimeInterval)acknowledgmentMessageTimeout;

//- (NSTimeInterval)acknowledgmentMessageInterval:(UInt8)ttl segmentCount:(int)segmentCount;

- (NSTimeInterval)acknowledgmentTimerInterval:(UInt8)ttl;

- (NSTimeInterval)transmissionTimerInteral:(UInt8)ttl;

- (int)retransmissionLimit;


+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));


+ (SigNetworkManager *)share;

#pragma mark - Receiving messages

/// This method handles the received PDU of given type.
/// @param pdu The data received.
/// @param type The PDU type.
- (void)handleIncomingPdu:(NSData *)pdu ofType:(SigPduType)type;

#pragma mark - Sending messages

- (void)sendMeshMessage:(SigMeshMessage *)message fromElement:(SigElementModel *)element toDestination:(SigMeshAddress *)destination withTtl:(UInt8)initialTtl usingApplicationKey:(SigAppkeyModel *)applicationKey command:(SDKLibCommand *)command;

/// Encrypts the message with the Application Key and a Network Key
/// bound to it, and sends to the given destination address.
///
/// This method does not send nor return PDUs to be sent. Instead,
/// for each created segment it calls transmitter's `send(:ofType)`,
/// which should send the PDU over the air. This is in order to support
/// retransmittion in case a packet was lost and needs to be sent again
/// after block acknowlegment was received.
///
/// @param message The message to be sent.
/// @param element The source Element.
/// @param destination The destination address.
/// @param initialTtl The initial TTL (Time To Live) value of the message. If `nil`, the default Node TTL will be used.
/// @param applicationKey The Application Key to sign the message.
- (void)sendMeshMessage:(SigMeshMessage *)message fromElement:(SigElementModel *)element toDestination:(SigMeshAddress *)destination withTtl:(UInt8)initialTtl usingApplicationKey:(SigAppkeyModel *)applicationKey;

/// Encrypts the message with the Device Key and the first Network Key
/// known to the target device, and sends to the given destination address.
///
/// The `ConfigNetKeyDelete` will be signed with a different Network Key
/// that is being removed.
///
/// @param configMessage The message to be sent.
/// @param destination The destination address.
/// @param initialTtl The initial TTL (Time To Live) value of the message. If `nil`, the default Node TTL will be used.
- (void)sendConfigMessage:(SigConfigMessage *)configMessage toDestination:(UInt16)destination withTtl:(UInt8)initialTtl;

/// Replies to the received message, which was sent with the given key set,
/// with the given message. The message will be sent from the local
/// Primary Element.
///
/// - parameters:
///   - origin:      The destination address of the message that the reply is for.
///   - message:     The response message to be sent.
///   - destination: The destination address. This must be a Unicast Address.
///   - keySet:      The keySet that should be used to encrypt the message.
- (void)replyToMessageSentToOrigin:(UInt16)origin withMessage:(SigMeshMessage *)message toDestination:(UInt16)destination usingKeySet:(SigKeySet *)keySet;

/// Replies to the received message, which was sent with the given key set,
/// with the given message.
///
/// - parameters:
///   - origin:      The destination address of the message that the reply is for.
///   - message:     The response message to be sent.
///   - element:     The source Element.
///   - destination: The destination address. This must be a Unicast Address.
///   - keySet:      The keySet that should be used to encrypt the message.
- (void)replyToMessageSentToOrigin:(UInt16)origin withMessage:(SigMeshMessage *)message fromElement:(SigElementModel *)element toDestination:(UInt16)destination usingKeySet:(SigKeySet *)keySet;

/// Sends the Proxy Configuration message to the connected Proxy Node.
///
/// - parameter message: The message to be sent.
- (void)sendSigProxyConfigurationMessage:(SigProxyConfigurationMessage *)message;

/// Cancels sending the message with the given handler.
///
/// - parameter handler: The message identifier.
- (void)cancelSigMessageHandle:(SigMessageHandle *)handler;

#pragma mark - Callbacks

/// Notifies the delegate about a new mesh message from the given source.
///
/// - parameters:
///   - message: The mesh message that was received.
///   - source:  The source Unicast Address.
///   - destination: The destination address of the message received.
- (void)notifyAboutNewMessage:(SigMeshMessage *)message fromSource:(UInt16)source toDestination:(UInt16)destination;

/// Notifies the delegate about delivering the mesh message to the given
/// destination address.
///
/// - parameters:
///   - message:      The mesh message that was sent.
///   - localElement: The local element used to send the message.
///   - destination:  The destination address.
- (void)notifyAboutDeliveringMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toDestination:(UInt16)destination;

/// Notifies the delegate about an error during sending the mesh message
/// to the given destination address.
///
/// - parameters:
///   - error:   The error that occurred.
///   - message: The mesh message that failed to be sent.
///   - localElement: The local element used to send the message.
///   - destination:  The destination address.
- (void)notifyAboutError:(NSError *)error duringSendingMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toDestination:(UInt16)destination;

#pragma mark - new api

//- (void)setFilter:(SigNetkeyModel *)netkeyModel;

@end

NS_ASSUME_NONNULL_END
