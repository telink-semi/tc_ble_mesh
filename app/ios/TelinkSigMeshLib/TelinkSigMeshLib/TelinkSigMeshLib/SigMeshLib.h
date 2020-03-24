/********************************************************************************************************
 * @file     SigMeshLib.h
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
//  SigMeshLib.h
//  SigMeshLib
//
//  Created by Liangjiazhi on 2019/8/15.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class SigNetworkManager,SigStorage,SigMessageHandle,SigMeshLib,SigMeshAddress,SigProxyConfigurationMessage,SDKLibCommand;

@protocol SigMeshNetworkDelegate <NSObject>
@optional

/// A callback called whenever a Mesh Message has been received
/// from the mesh network.
///
/// The `source` is given as an Address, instead of an Element, as
/// the message may be sent by an unknown Node, or a Node which
/// Elements are unknown.
///
/// The `destination` address may be a Unicast Address of a local
/// Element, a Group or Virtual Address, but also any other address
/// if it was added to the Proxy Filter, e.g. a Unicast Address of
/// an Element on a remote Node.
///
/// - parameters:
///   - manager:     The manager which has received the message.
///   - message:     The received message.
///   - source:      The Unicast Address of the Element from which
///                  the message was sent.
///   - destination: The address to which the message was sent.
- (void)meshNetworkManager:(SigMeshLib *)manager didReceiveMessage:(SigMeshMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination;

/// A callback called when an unsegmented message was sent to the
/// `transmitter`, or when all segments of a segmented message targetting
/// a Unicast Address were acknowledged by the target Node.
///
/// - parameters:
///   - manager:      The manager used to send the message.
///   - message:      The message that has been sent.
///   - localElement: The local Element used as a source of this message.
///   - destination:  The address to which the message was sent.
- (void)meshNetworkManager:(SigMeshLib *)manager didSendMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toDestination:(UInt16)destination;

/// A callback called when a message failed to be sent to the target
/// Node, or the respnse for an acknowledged message hasn't been received
/// before the time run out.
///
/// For unsegmented unacknowledged messages this callback will be invoked when
/// the `transmitter` was set to `nil`, or has thrown an exception from
/// `send(data:ofType)`.
///
/// For segmented unacknowledged messages targetting a Unicast Address,
/// besides that, it may also be called when sending timed out before all of
/// the segments were acknowledged by the target Node, or when the target
/// Node is busy and not able to proceed the message at the moment.
///
/// For acknowledged messages the callback will be called when the response
/// has not been received before the time set by `acknowledgmentMessageTimeout`
/// run out. The message might have been retransmitted multiple times
/// and might have been received by the target Node. For acknowledged messages
/// sent to a Group or Virtual Address this will be called when the response
/// has not been received from any Node.
///
/// Possible errors are:
/// - Any error thrown by the `transmitter`.
/// - `BearerError.bearerClosed` - when the `transmitter` object was net set.
/// - `LowerTransportError.busy` - when the target Node is busy and can't
///   accept the message.
/// - `LowerTransportError.timeout` - when the segmented message targetting
///   a Unicast Address was not acknowledgned before the `retransmissionLimit`
///   was reached (for unacknowledged messages only).
/// - `AccessError.timeout` - when the response for an acknowledged message
///   has not been received before the time run out (for acknowledged messages
///   only).
///
/// - parameters:
///   - manager:      The manager used to send the message.
///   - message:      The message that has failed to be delivered.
///   - localElement: The local Element used as a source of this message.
///   - destination:  The address to which the message was sent.
///   - error:        The error that occurred.
- (void)meshNetworkManager:(SigMeshLib *)manager failedToSendMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toDestination:(UInt16)destination error:(NSError *)error;

///// Callback called when a possible change of Proxy Node have been discovered.
/////
///// This method is called in two cases: when the first Secure Network
///// beacon was received (which indicates the first successful connection
///// to a Proxy since app was started) or when the received Secure Network
///// beacon contained information about the same Network Key as one
///// received before. This happens during a reconnection to the same
///// or a different Proxy on the same subnetwork, but may also happen
///// in other sircumstances, for example when the IV Update or Key Refresh
///// Procedure is in progress, or a Network Key was removed and added again.
/////
///// This method reloads the Proxy Filter for the local Provisioner,
///// adding all the addresses the Provisioner is subscribed to, including
///// its Unicast Addresses and All Nodes address.
//- (void)newProxyDidConnect;

- (void)meshNetworkManager:(SigMeshLib *)manager didReceiveSigProxyConfigurationMessage:(SigProxyConfigurationMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination;

@end


@interface SigMeshLib : NSObject
/// Mesh Network data.
@property (nonatomic,strong) NSMutableArray <SDKLibCommand *>*commands;
/// Mesh Network data.
@property (nonatomic,strong) SigDataSource *dataSource;
/// The Network Layer handler.
@property (nonatomic,strong) SigNetworkManager *networkManager;
/// Storage to keep the app data.
@property (nonatomic,strong) SigStorage *storage;
/// A queue to handle incoming and outgoing messages.
@property (nonatomic,strong) dispatch_queue_t queue;
/// A queue to call delegate methods on.
@property (nonatomic,strong) dispatch_queue_t delegateQueue;

/// The delegate will receive callbacks whenever a complete
/// Mesh Message has been received and reassembled.
@property (nonatomic, weak) id <SigMeshNetworkDelegate>delegate;
/// The sender object should send PDUs created by the manager
/// using any Bearer.
@property (nonatomic, weak) id transmitter;

#pragma mark - Network Manager properties

/// The Default TTL will be used for sending messages, if the value has
/// not been set in the Provisioner's Node. By default it is set to 5,
/// which is a reasonable value. The TTL shall be in range 2...127.
@property (nonatomic,assign) UInt8 defaultTtl;//5

/// The timeout after which an incomplete segmented message will be
/// abandoned. The timer is restarted each time a segment of this
/// message is received.
///
/// The incomplete timeout should be set to at least 10 seconds.
@property (nonatomic,assign) NSTimeInterval incompleteMessageTimeout;//10.0

/// The amount of time after which the lower transport layer sends a
/// Segment Acknowledgment message after receiving a segment of a
/// multi-segment message where the destination is a Unicast Address
/// of the Provisioner's Node.
///
/// The acknowledgment timer shall be set to a minimum of
/// 150 + 50 * TTL milliseconds. The TTL dependent part is added
/// automatically, and this value shall specify only the constant part.
@property (nonatomic,assign) NSTimeInterval acknowledgmentTimerInterval;//0.150

/// The time within which a Segment Acknowledgment message is
/// expected to be received after a segment of a segmented message has
/// been sent. When the timer is fired, the non-acknowledged segments
/// are repeated, at most `retransmissionLimit` times.
///
/// The transmission timer shall be set to a minimum of
/// 200 + 50 * TTL milliseconds. The TTL dependent part is added
/// automatically, and this value shall specify only the constant part.
///
/// If the bearer is using GATT, it is recommended to set the transmission
/// interval longer than the connection interval, so that the acknowledgment
/// had a chance to be received.
@property (nonatomic,assign) NSTimeInterval transmissionTimerInteral;//0.200

/// Number of times a non-acknowledged segment will be re-send before
/// the message will be cancelled.
///
/// The limit may be decreased with increasing of `transmissionTimerInterval`
/// as the target Node has more time to reply with the Segment
/// Acknowledgment message.
@property (nonatomic,assign) int retransmissionLimit;//5

/// If the Element does not receive a response within a period of time known
/// as the acknowledged message timeout, then the Element may consider the
/// message has not been delivered, without sending any additional messages.
///
/// The `meshNetworkManager(_:failedToSendMessage:from:to:error)`
/// callback will be called on timeout.
///
/// The acknowledged message timeout should be set to a minimum of 30 seconds.
@property (nonatomic,assign) NSTimeInterval acknowledgmentMessageTimeout;//30.0

/// The base time after which the acknowledgmed message will be repeated.
///
/// The repeat timer will be set to the base time + 50 * TTL milliseconds +
/// 50 * segment count. The TTL and segment count dependent parts are added
/// automatically, and this value shall specify only the constant part.
@property (nonatomic,assign) NSTimeInterval acknowledgmentMessageInterval;//2.0

/// 4.2.19.2 Network Transmit Interval Steps
/// - seeAlso: Mesh_v1.0.pdf  (page.151)
@property (nonatomic,assign) UInt8 networkTransmitIntervalSteps;//defount is 0b1111=31.
@property (nonatomic,assign,readonly) double networkTransmitInterval;//defount is (0b1111+1)*10=320ms.

+ (SigMeshLib *)share;

#pragma mark - Computed properties

- (void)setNetworkManager:(SigNetworkManager *)networkManager;

/// Returns `true` if Mesh Network has been created, `false` otherwise.
- (BOOL)isNetworkCreated;

- (BOOL)isBusy;

#pragma mark - Constructors

/// Initializes the MeshNetworkManager.
/// If storage is not provided, a local file will be used instead.
///
/// - parameter storage: The storage to use to save the network configuration.
/// - seeAlso: `LocalStorage`
- (instancetype)initWithStorage:(SigStorage *)storage;

/// Initializes the MeshNetworkManager. It will use the `LocalStorage`
/// with the given file name.
///
/// - parameter fileName: File name to keep the configuration.
/// - seeAlso: `LocalStorage`
- (instancetype)initWithFileName:(NSString *)fileName;

#pragma mark - Send / Receive Mesh Messages

/// This method should be called whenever a PDU has been received
/// from the mesh network using any bearer.
/// When a complete Mesh Message is received and reassembled, the
/// delegate's `meshNetwork(:didDeliverMessage:from)` will be called.
///
/// For easier integration with Bearers use
/// `bearer(didDeliverData:ofType)` instead, and set the manager
/// as Bearer's `dataDelegate`.
///
/// - parameter data: The PDU received.
/// - parameter type: The PDU type.
- (void)bearerDidDeliverData:(NSData *)data type:(SigPduType)type;

- (void)updateOnlineStatusWithDeviceAddress:(UInt16)address deviceState:(DeviceState)state bright100:(UInt8)bright100 temperature100:(UInt8)temperature100;

// TODO: Add send to Group method.

///// Sends Configuration Message to the Node with given destination Address.
///// The `destination` must be a Unicast Address, otherwise the method
///// does nothing.
/////
///// If the `transporter` throws an error during sending, this error will be ignored.
/////
///// - parameter message:     The message to be sent.
///// - parameter destination: The destination Unicast Address.
//- (void)sendMessage:(SigConfigMessage *)message toDestinationAddress:(UInt16)destination;

///// Sends Configuration Message to the given Node.
/////
///// If the `transporter` throws an error during sending, this error will be ignored.
/////
///// - parameter message: The message to be sent.
///// - parameter node:    The destination Node.
//- (void)sendMessage:(SigConfigMessage *)message toNode:(SigNodeModel *)model;

///// Sends Configuration Message to the given Node.
/////
///// If the `transporter` throws an error during sending, this error will be ignored.
/////
///// - parameter message: The message to be sent.
///// - parameter element: The destination Element.
//- (void)sendMessage:(SigConfigMessage *)message toElement:(SigElementModel *)element;

///// Sends Configuration Message to the given Node.
/////
///// If the `transporter` throws an error during sending, this error will be ignored.
/////
///// - parameter message: The message to be sent.
///// - parameter model:   The destination Model.
//- (void)sendConfigMessage:(SigConfigMessage *)message toModel:(SigModelIDModel *)model;

#pragma mark - Save / Load

/// Loads the Mesh Network configuration from the storage.
/// If storage is not given, a local file will be used.
///
/// - returns: `True` if the network settings were loaded, `false` otherwise.
/// - throws: If loading configuration failed.
- (BOOL)load;

/// Saves the Mesh Network configuration in the storage.
/// If storage is not given, a local file will be used.
///
/// - returns: `True` if the network settings was saved, `false` otherwise.
- (BOOL)save;

#pragma mark - Export / Import

/// Returns the exported Mesh Network configuration as JSON Data.
/// The returned Data can be transferred to another application and
/// imported. The JSON is compatible with Bluetooth Mesh scheme.
///
/// - returns: The mesh network configuration as JSON Data.
- (NSData *)exportMesh;

/// Imports the Mesh Network configuration from the given Data.
/// The data must contain valid JSON with Bluetooth Mesh scheme.
///
/// - parameter data: JSON as Data.
/// - throws: This method throws an error if import or adding
///           the local Provisioner failed.
- (void)importData:(NSData *)data;


#pragma mark - Send / Receive Mesh Messages

///// This method tries to publish the given message using the
///// publication information set in the Model.
/////
///// - parameters:
/////   - message:    The message to be sent.
/////   - model:      The model from which to send the message.
/////   - initialTtl: The initial TTL (Time To Live) value of the message.
/////                 If `nil`, the default Node TTL will be used.
//- (SigMessageHandle *)publishSigMeshMessage:(SigMeshMessage *)message fromModel:(SigModelIDModel *)model withTtl:(UInt8)initialTtl command:(SDKLibCommand *)command;
//
//- (SigMessageHandle *)publishSigMeshMessage:(SigMeshMessage *)message fromModel:(SigModelIDModel *)model command:(SDKLibCommand *)command;

///// Encrypts the message with the Application Key and a Network Key
///// bound to it, and sends to the given Group.
/////
///// A `delegate` method will be called when the message has been sent,
///// or failed to be sent.
/////
///// - parameters:
/////   - message:        The message to be sent.
/////   - localElement:   The source Element. If `nil`, the primary
/////                     Element will be used. The Element must belong
/////                     to the local Provisioner's Node.
/////   - group:          The target Group.
/////   - initialTtl:     The initial TTL (Time To Live) value of the message.
/////                     If `nil`, the default Node TTL will be used.
/////   - applicationKey: The Application Key to sign the message.
///// - throws: This method throws when the mesh network has not been created,
/////           the local Node does not have configuration capabilities
/////           (no Unicast Address assigned), or the given local Element
/////           does not belong to the local Node.
///// - returns: Message handle that can be used to cancel sending.
//- (SigMessageHandle *)sendMeshMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toGroup:(SigGroupModel *)group withTtl:(UInt8)initialTtl usingApplicationKey:(SigAppkeyModel *)applicationKey command:(SDKLibCommand *)command;

///// Encrypts the message with the first Application Key bound to the given
///// Model and a Network Key bound to it, and sends it to the Node
///// to which the Model belongs to.
/////
///// A `delegate` method will be called when the message has been sent,
///// delivered, or fail to be sent.
/////
///// - parameters:
/////   - message:       The message to be sent.
/////   - localElement:  The source Element. If `nil`, the primary
/////                    Element will be used. The Element must belong
/////                    to the local Provisioner's Node.
/////   - model:         The destination Model.
/////   - initialTtl:    The initial TTL (Time To Live) value of the message.
/////                    If `nil`, the default Node TTL will be used.
/////   - applicationKey: The Application Key to sign the message.
///// - throws: This method throws when the mesh network has not been created,
/////           the target Model does not belong to any Element, or has
/////           no Application Key bound to it, or when
/////           the local Node does not have configuration capabilities
/////           (no Unicast Address assigned), or the given local Element
/////           does not belong to the local Node.
///// - returns: Message handle that can be used to cancel sending.
//- (SigMessageHandle *)sendMeshMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toModel:(SigModelIDModel *)model withTtl:(UInt8)initialTtl command:(SDKLibCommand *)command;

///// Encrypts the message with the common Application Key bound to both given
///// Models and a Network Key bound to it, and sends it to the Node
///// to which the target Model belongs to.
/////
///// A `delegate` method will be called when the message has been sent,
///// delivered, or fail to be sent.
/////
///// - parameters:
/////   - message:      The message to be sent.
/////   - localElement: The source Element. If `nil`, the primary
/////                   Element will be used. The Element must belong
/////                   to the local Provisioner's Node.
/////   - model:        The destination Model.
/////   - initialTtl:   The initial TTL (Time To Live) value of the message.
/////                   If `nil`, the default Node TTL will be used.
///// - throws: This method throws when the mesh network has not been created,
/////           the local or target Model do not belong to any Element, or have
/////           no common Application Key bound to them, or when
/////           the local Node does not have configuration capabilities
/////           (no Unicast Address assigned), or the given local Element
/////           does not belong to the local Node.
///// - returns: Message handle that can be used to cancel sending.
//- (SigMessageHandle *)sendMeshMessage:(SigMeshMessage *)message fromLocalModel:(SigModelIDModel *)localModel toModel:(SigModelIDModel *)model withTtl:(UInt8)initialTtl command:(SDKLibCommand *)command;

///// Sends Configuration Message to the given Node.
/////
///// A `delegate` method will be called when the message has been sent,
///// delivered, or fail to be sent.
/////
///// - parameters:
/////   - message: The message to be sent.
/////   - node:    The destination Node.
/////   - initialTtl: The initial TTL (Time To Live) value of the message.
/////                 If `nil`, the default Node TTL will be used.
/////   - command:    The command save message and callback.
///// - throws: This method throws when the mesh network has not been created,
/////           the local Node does not have configuration capabilities
/////           (no Unicast Address assigned), or the destination address
/////           is not a Unicast Address or it belongs to an unknown Node.
///// - returns: Message handle that can be used to cancel sending.
//- (SigMessageHandle *)sendConfigMessage:(SigConfigMessage *)message toNode:(SigNodeModel *)node withTtl:(UInt8)initialTtl command:(SDKLibCommand *)command;
//- (SigMessageHandle *)sendConfigMessage:(SigConfigMessage *)message toNode:(SigNodeModel *)node command:(SDKLibCommand *)command;

///// Sends Configuration Message to the given Node.
/////
///// A `delegate` method will be called when the message has been sent,
///// delivered, or fail to be sent.
/////
///// - parameters:
/////   - message: The message to be sent.
/////   - element: The destination Element.
/////   - initialTtl: The initial TTL (Time To Live) value of the message.
/////                 If `nil`, the default Node TTL will be used.
///// - throws: This method throws when the mesh network has not been created,
/////           the local Node does not have configuration capabilities
/////           (no Unicast Address assigned), or the target Element does not
/////           belong to any known Node.
///// - returns: Message handle that can be used to cancel sending.
//- (SigMessageHandle *)sendConfigMessage:(SigConfigMessage *)message toElement:(SigElementModel *)element withTtl:(UInt8)initialTtl command:(SDKLibCommand *)command;

///// Sends Configuration Message to the given Node.
/////
///// A `delegate` method will be called when the message has been sent,
///// delivered, or fail to be sent.
/////
///// - parameters:
/////   - message: The message to be sent.
/////   - model:   The destination Model.
/////   - initialTtl: The initial TTL (Time To Live) value of the message.
/////                 If `nil`, the default Node TTL will be used.
///// - throws: This method throws when the mesh network has not been created,
/////           the local Node does not have configuration capabilities
/////           (no Unicast Address assigned), or the target Element does
/////           not belong to any known Node.
///// - returns: Message handle that can be used to cancel sending.
//- (SigMessageHandle *)sendConfigMessage:(SigConfigMessage *)message toModel:(SigModelIDModel *)model withTtl:(UInt8)initialTtl command:(SDKLibCommand *)command;

///// Encrypts the message with the first Application Key bound to the given
///// Model and a Network Key bound to it, and sends it to the Node
///// to which the Model belongs to.
/////
///// A `delegate` method will be called when the message has been sent,
///// delivered, or fail to be sent.
/////
///// - parameters:
/////   - message:       The message to be sent.
/////   - localElement:  The source Element. If `nil`, the primary
/////                    Element will be used. The Element must belong
/////                    to the local Provisioner's Node.
/////   - model:         The destination Model.
/////   - initialTtl:    The initial TTL (Time To Live) value of the message.
/////                    If `nil`, the default Node TTL will be used.
/////   - applicationKey: The Application Key to sign the message.
///// - throws: This method throws when the mesh network has not been created,
/////           the target Model does not belong to any Element, or has
/////           no Application Key bound to it, or when
/////           the local Node does not have configuration capabilities
/////           (no Unicast Address assigned), or the given local Element
/////           does not belong to the local Node.
///// - returns: Message handle that can be used to cancel sending.
//- (SigMessageHandle *)sendSigMeshMessage:(SigMeshMessage *)message fromLocalElement:(nullable SigElementModel *)localElement toModelIDModel:(SigModelIDModel *)model withTtl:(UInt8)initialTtl command:(SDKLibCommand *)command;
//- (SigMessageHandle *)sendSigMeshMessage:(SigMeshMessage *)message fromLocalElement:(nullable SigElementModel *)localElement toModelIDModel:(SigModelIDModel *)model command:(SDKLibCommand *)command;








/// Sends Configuration Message to the Node with given destination Address.
/// The `destination` must be a Unicast Address, otherwise the method
/// does nothing.
///
/// A `delegate` method will be called when the message has been sent,
/// delivered, or fail to be sent.
///
/// - parameters:
///   - message:     The message to be sent.
///   - destination: The destination Unicast Address.
///   - initialTtl:  The initial TTL (Time To Live) value of the message.
///                  If `nil`, the default Node TTL will be used.
///   - command:    The command save message and callback.
/// - throws: This method throws when the mesh network has not been created,
///           the local Node does not have configuration capabilities
///           (no Unicast Address assigned), or the destination address
///           is not a Unicast Address or it belongs to an unknown Node.
///           Error `AccessError.cannotDelete` is sent when trying to
///           delete the last Network Key on the device.
/// - returns: Message handle that can be used to cancel sending.
- (SigMessageHandle *)sendConfigMessage:(SigConfigMessage *)message toDestination:(UInt16)destination withTtl:(UInt8)initialTtl command:(SDKLibCommand *)command;
- (SigMessageHandle *)sendConfigMessage:(SigConfigMessage *)message toDestination:(UInt16)destination command:(SDKLibCommand *)command;

/// Encrypts the message with the Application Key and a Network Key
/// bound to it, and sends to the given destination address.
///
/// This method does not send nor return PDUs to be sent. Instead,
/// for each created segment it calls transmitter's `send(:ofType)`,
/// which should send the PDU over the air. This is in order to support
/// retransmittion in case a packet was lost and needs to be sent again
/// after block acknowlegment was received.
///
/// A `delegate` method will be called when the message has been sent,
/// delivered, or failed to be sent.
///
/// - parameters:
///   - message:        The message to be sent.
///   - localElement:   The source Element. If `nil`, the primary
///                     Element will be used. The Element must belong
///                     to the local Provisioner's Node.
///   - destination:    The destination address.
///   - initialTtl:     The initial TTL (Time To Live) value of the message.
///                     If `nil`, the default Node TTL will be used.
///   - applicationKey: The Application Key to sign the message.
/// - throws: This method throws when the mesh network has not been created,
///           the local Node does not have configuration capabilities
///           (no Unicast Address assigned), or the given local Element
///           does not belong to the local Node.
/// - returns: Message handle that can be used to cancel sending.
- (SigMessageHandle *)sendMeshMessage:(SigMeshMessage *)message fromLocalElement:(nullable SigElementModel *)localElement toDestination:(SigMeshAddress *)destination withTtl:(UInt8)initialTtl usingApplicationKey:(SigAppkeyModel *)applicationKey command:(SDKLibCommand *)command;
- (SigMessageHandle *)sendMeshMessage:(SigMeshMessage *)message fromLocalElement:(nullable SigElementModel *)localElement toDestination:(SigMeshAddress *)destination usingApplicationKey:(SigAppkeyModel *)applicationKey command:(SDKLibCommand *)command;

/// Sends the Proxy Configuration Message to the connected Proxy Node.
///
/// This method will only work if the bearer uses is GATT Proxy.
/// The message will be encrypted and sent to the `transported`, which
/// should deliver the PDU to the connected Node.
///
/// - parameters:
///   - message: The Proxy Configuration message to be sent.
///   - initialTtl: The initial TTL (Time To Live) value of the message.
///                 If `nil`, the default Node TTL will be used.
/// - throws: This method throws when the mesh network has not been created.
- (void)sendSigProxyConfigurationMessage:(SigProxyConfigurationMessage *)message command:(SDKLibCommand *)command;

- (NSError *)sendTelinkApiGetOnlineStatueFromUUIDWithMessage:(SigMeshMessage *)message command:(SDKLibCommand *)command;

/// Cancels sending the message with the given identifier.
///
/// - parameter messageId: The message identifier.
- (void)cancelSigMessageHandle:(SigMessageHandle *)messageId;

- (BOOL)isBusyNow;

@end

NS_ASSUME_NONNULL_END
