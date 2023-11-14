/********************************************************************************************************
 * @file     SigNetworkManager.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/8/15
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/

#import "SigNetworkManager.h"
#import "SigDataSource.h"
#import "SigMeshLib.h"
#import "SigNetworkLayer.h"
#import "SigLowerTransportLayer.h"
#import "SigUpperTransportLayer.h"
#import "SigAccessLayer.h"
#import "SigBearer.h"

@implementation SigNetworkManager

/**
 *  @brief  Singleton method
 *
 *  @return the default singleton instance. You are not allowed to create your own instances of this class.
 */
+ (instancetype)share {
    /// Singleton instance
    static SigNetworkManager *shareManager = nil;
    /// Note: The dispatch_once function can ensure that a certain piece
    /// of code is only executed once in the entire application life cycle!
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        /// Initialize the Singleton configure parameters.
        shareManager = [[SigNetworkManager alloc] init];
        shareManager.manager = [SigMeshLib share];
        shareManager.networkLayer = [[SigNetworkLayer alloc] initWithNetworkManager:shareManager];
        shareManager.lowerTransportLayer = [[SigLowerTransportLayer alloc] initWithNetworkManager:shareManager];
        shareManager.upperTransportLayer = [[SigUpperTransportLayer alloc] initWithNetworkManager:shareManager];
        shareManager.accessLayer = [[SigAccessLayer alloc] initWithNetworkManager:shareManager];
    });
    return shareManager;
}

- (UInt8)defaultTtl {
    return MAX(MIN(_manager.defaultTtl, 127), 2);
}

- (NSTimeInterval)incompleteMessageTimeout {
    return MAX(_manager.incompleteMessageTimeout, 10.0);
}

- (NSTimeInterval)acknowledgmentTimerInterval:(UInt8)ttl {
    return MAX(_manager.acknowledgmentTimerInterval, 0.150) + (double)(ttl) * 0.050;
}

- (NSTimeInterval)transmissionTimerInterval:(UInt8)ttl {
    return MAX(_manager.transmissionTimerInterval, 0.200) + (double)(ttl) * 0.050;
}

- (int)retransmissionLimit {
    return MAX(_manager.retransmissionLimit, 2);
}

#pragma mark - Receiving messages

/// This method handles the received PDU of given type.
///
/// @param pdu The data received.
/// @param type The PDU type.
- (void)handleIncomingPdu:(NSData *)pdu ofType:(SigPduType)type {
    [self.networkLayer handleIncomingPdu:pdu ofType:type];
}

#pragma mark - Sending messages

/// Encrypts the message with the Application Key and a Network Key bound to it, and sends to the given destination address.
/// This method does not send nor return PDUs to be sent. Instead, for each created segment it calls transmitter's `send(:ofType)`,
/// which should send the PDU over the air. This is in order to support retransmittion in case a packet was lost and needs to be sent
/// again after block acknowledgment was received.
///
/// @param message The message to be sent.
/// @param element The source Element.
/// @param destination The destination address.
/// @param initialTtl The initial TTL (Time To Live) value of the message. If `nil`, the default Node TTL will be used.
/// @param applicationKey The Application Key to sign the message.
/// @param command The command of the message.
- (void)sendMeshMessage:(SigMeshMessage *)message fromElement:(SigElementModel *)element toDestination:(SigMeshAddress *)destination withTtl:(UInt8)initialTtl usingApplicationKey:(SigAppkeyModel *)applicationKey command:(SDKLibCommand *)command {
    [_accessLayer sendMessage:message fromElement:element toDestination:destination withTtl:initialTtl usingApplicationKey:applicationKey command:command];
}

/// Encrypts the message with the Device Key and the first Network Key known to the target device,
/// and sends to the given destination address.
/// The `ConfigNetKeyDelete` will be signed with a different Network Key that is being removed.
///
/// @param configMessage The message to be sent.
/// @param destination The destination address.
/// @param initialTtl The initial TTL (Time To Live) value of the message. If `nil`, the default Node TTL will be used.
/// @param command The command of the message.
- (void)sendConfigMessage:(SigConfigMessage *)configMessage toDestination:(UInt16)destination withTtl:(UInt8)initialTtl command:(SDKLibCommand *)command {
    [_accessLayer sendSigConfigMessage:configMessage toDestination:destination withTtl:initialTtl command:command];
}

/// Replies to the received message, which was sent with the given key set, with the given message.
/// The message will be sent from the local Primary Element.
///
/// @param origin The destination address of the message that the reply is for.
/// @param message The response message to be sent.
/// @param destination The destination address. This must be a Unicast Address.
/// @param keySet The keySet that should be used to encrypt the message.
/// @param command The command of the message.
- (void)replyToMessageSentToOrigin:(UInt16)origin withMessage:(SigMeshMessage *)message toDestination:(UInt16)destination usingKeySet:(SigKeySet *)keySet command:(SDKLibCommand *)command {
    SigElementModel *primaryElement = _manager.dataSource.curLocationNodeModel.elements.firstObject;
    if (primaryElement) {
        [_accessLayer replyToMessageSentToOrigin:origin withMeshMessage:message fromElement:primaryElement toDestination:destination usingKeySet:keySet command:command];
    }
}

/// Replies to the received message, which was sent with the given key set, with the given message.
/// @param origin The destination address of the message that the reply is for.
/// @param message The response message to be sent.
/// @param element The source Element.
/// @param destination The destination address. This must be a Unicast Address.
/// @param keySet The keySet that should be used to encrypt the message.
/// @param command The command of the message.
- (void)replyToMessageSentToOrigin:(UInt16)origin withMessage:(SigMeshMessage *)message fromElement:(SigElementModel *)element toDestination:(UInt16)destination usingKeySet:(SigKeySet *)keySet command:(SDKLibCommand *)command {
    [_accessLayer replyToMessageSentToOrigin:origin withMeshMessage:message fromElement:element toDestination:destination usingKeySet:keySet command:command];
}

/// Sends the Proxy Configuration message to the connected Proxy Node.
/// @param message The message to be sent.
- (void)sendSigProxyConfigurationMessage:(SigProxyConfigurationMessage *)message {
    [_networkLayer sendSigProxyConfigurationMessage:message];
}

/// Cancels sending the message with the given handler.
/// @param handler The message identifier.
- (void)cancelSigMessageHandle:(SigMessageHandle *)handler {
    [_accessLayer cancelSigMessageHandle:handler];
}

#pragma mark - Callbacks

/// Notifies the delegate about a new mesh message from the given source.
/// @param message The mesh message that was received.
/// @param source The source Unicast Address.
/// @param destination The destination address of the message received.
- (void)notifyAboutNewMessage:(SigMeshMessage *)message fromSource:(UInt16)source toDestination:(UInt16)destination {
    __weak typeof(self) weakSelf = self;
    dispatch_async(self.manager.delegateQueue, ^{
        if ([weakSelf.manager.delegate respondsToSelector:@selector(didReceiveMessage:sentFromSource:toDestination:)]) {
            [weakSelf.manager.delegate didReceiveMessage:message sentFromSource:source toDestination:destination];
        }
        if ([weakSelf.manager.delegateForDeveloper respondsToSelector:@selector(didReceiveMessage:sentFromSource:toDestination:)]) {
            [weakSelf.manager.delegateForDeveloper didReceiveMessage:message sentFromSource:source toDestination:destination];
        }
    });
}

/// Notifies the delegate about delivering the mesh message to the given destination address.
/// @param message The mesh message that was sent.
/// @param localElement The local element used to send the message.
/// @param destination The destination address.
- (void)notifyAboutDeliveringMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toDestination:(UInt16)destination {
    __weak typeof(self) weakSelf = self;
    dispatch_async(self.manager.delegateQueue, ^{
        if ([weakSelf.manager.delegate respondsToSelector:@selector(didSendMessage:fromLocalElement:toDestination:)]) {
            [weakSelf.manager.delegate didSendMessage:message fromLocalElement:localElement toDestination:destination];
        }
        if ([weakSelf.manager.delegateForDeveloper respondsToSelector:@selector(didSendMessage:fromLocalElement:toDestination:)]) {
            [weakSelf.manager.delegateForDeveloper didSendMessage:message fromLocalElement:localElement toDestination:destination];
        }
    });
}

/// Notifies the delegate about an error during sending the mesh message to the given destination address.
/// @param error The error that occurred.
/// @param message The mesh message that failed to be sent.
/// @param localElement The local element used to send the message.
/// @param destination The destination address.
- (void)notifyAboutError:(NSError *)error duringSendingMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toDestination:(UInt16)destination {
    __weak typeof(self) weakSelf = self;
    dispatch_async(self.manager.delegateQueue, ^{
        if ([weakSelf.manager.delegate respondsToSelector:@selector(failedToSendMessage:fromLocalElement:toDestination:error:)]) {
            [weakSelf.manager.delegate failedToSendMessage:message fromLocalElement:localElement toDestination:destination error:error];
        }
        if ([weakSelf.manager.delegateForDeveloper respondsToSelector:@selector(failedToSendMessage:fromLocalElement:toDestination:error:)]) {
            [weakSelf.manager.delegateForDeveloper failedToSendMessage:message fromLocalElement:localElement toDestination:destination error:error];
        }
    });
}

@end
