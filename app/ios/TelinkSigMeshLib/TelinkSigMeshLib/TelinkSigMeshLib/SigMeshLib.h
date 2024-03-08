/********************************************************************************************************
 * @file     SigMeshLib.h
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

#import <Foundation/Foundation.h>
#import "TPeripheralManager.h"

NS_ASSUME_NONNULL_BEGIN

@class SigMessageHandle, SigMeshAddress, SigProxyConfigurationMessage, SDKLibCommand, SigSecureNetworkBeacon, SigNetworkPdu, SigMeshPrivateBeacon, BackgroundTimer;

@protocol SigMessageDelegate <NSObject>
@optional

/// A callback called whenever a Mesh Message has been received from the mesh network.
/// @param message The received message.
/// @param source The Unicast Address of the Element from which the message was sent.
/// @param destination The address to which the message was sent.
- (void)didReceiveMessage:(SigMeshMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination;

/// A callback called when an unsegmented message was sent to the SigBearer, or when all
/// segments of a segmented message targeting a Unicast Address were acknowledged by
/// the target Node.
/// @param message The message that has been sent.
/// @param localElement The local Element used as a source of this message.
/// @param destination The address to which the message was sent.
- (void)didSendMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toDestination:(UInt16)destination;

/// A callback called when a message failed to be sent to the target Node, or the response for
/// an acknowledged message hasn't been received before the time run out.
/// For unsegmented unacknowledged messages this callback will be invoked when the
/// SigBearer was closed.
/// For segmented unacknowledged messages targeting a Unicast Address, besides that,
/// it may also be called when sending timed out before all of the segments were acknowledged
/// by the target Node, or when the target Node is busy and not able to proceed the message
/// at the moment.
/// For acknowledged messages the callback will be called when the response has not been
/// received before the time set by `incompleteMessageTimeout` run out. The message
/// might have been retransmitted multiple times and might have been received by the target Node.
/// For acknowledged messages sent to a Group or Virtual Address this will be called when the
/// response has not been received from any Node.
/// @param message The message that has failed to be delivered.
/// @param localElement The local Element used as a source of this message.
/// @param destination The address to which the message was sent.
/// @param error The error that occurred.
- (void)failedToSendMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toDestination:(UInt16)destination error:(NSError *)error;

/// A callback called whenever a SigProxyConfiguration Message has been received from the mesh network.
/// @param message The received message.
/// @param source The Unicast Address of the Element from which the message was sent.
/// @param destination The address to which the message was sent.
- (void)didReceiveSigProxyConfigurationMessage:(SigProxyConfigurationMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination;

/// A callback called whenever a SigSecureNetworkBeacon Message has been received from the mesh network.
/// @param message The received message.
- (void)didReceiveSigSecureNetworkBeaconMessage:(SigSecureNetworkBeacon *)message;

/// A callback called whenever a SigMeshPrivateBeacon Message has been received from the mesh network.
/// @param message The received message.
- (void)didReceiveSigMeshPrivateBeaconMessage:(SigMeshPrivateBeacon *)message;

@end


@interface SigMeshLib : NSObject
/// command list cache.
@property (nonatomic,strong) NSMutableArray <SDKLibCommand *>*commands;
/// A queue to handle incoming and outgoing messages.
@property (nonatomic,strong) dispatch_queue_t queue;
/// A queue to call delegate methods on.
@property (nonatomic,strong) dispatch_queue_t delegateQueue;
/// Mesh Network data.
@property (nonatomic,strong) SigDataSource *dataSource;
/// The sender object should send PDUs created by the manager using any Bearer.
@property (nonatomic,strong) SigBearer *bearer;
/// The delegate will receive callbacks whenever a complete Mesh Message has been received and reassembled.
@property (nonatomic, weak, readonly) id <SigMessageDelegate>delegate;
@property (nonatomic, weak, readwrite) id <SigMessageDelegate>delegateForDeveloper;

#pragma mark - Network Manager properties

/// The Default TTL will be used for sending messages,
/// if the value has not been set in the Provisioner's Node.
/// By default it is set to 10, which is a reasonable value.
/// The TTL shall be in range 2...127.
@property (nonatomic,assign) UInt8 defaultTtl;//10

/// The timeout after which an incomplete segmented message will be abandoned.
/// The timer is restarted each time a segment of this message is received.（segment包重试时间）
///
/// The incomplete timeout should be set to at least 10 seconds.
@property (nonatomic,assign) NSTimeInterval incompleteMessageTimeout;//15.0
@property (nonatomic,assign) NSTimeInterval receiveSegmentMessageTimeout;//15.0

/// The amount of time after which the lower transport layer sends a Segment Acknowledgment message
/// after receiving a segment of a multi-segment message where the destination is a Unicast Address of
/// the Provisioner's Node.
///
/// The acknowledgment timer shall be set to a minimum of 150 + 50 * TTL milliseconds. The TTL dependent
/// part is added automatically, and this value shall specify only the constant part.
@property (nonatomic,assign) NSTimeInterval acknowledgmentTimerInterval;//0.150

/// The time within which a Segment Acknowledgment message is expected to be received after a segment of
/// a segmented message has been sent. When the timer is fired, the non-acknowledged segments are repeated,
/// at most `retransmissionLimit` times.（segment包发送完一轮后等待一个The transmission timer长度的
/// 时间，未收到segment发送完成则开始下一轮segment发送。）
///
/// The transmission timer shall be set to a minimum of 200 + 50 * TTL milliseconds. The TTL dependent part is
/// added automatically, and this value shall specify only the constant part.
/// （The transmission timer = transmissionTimerInterval + 50 * TTL milliseconds，transmissionTimerInterval默认值为200毫秒。）
///
/// If the SigBearer.share.isProvisioned is NO, it is recommended to set the transmission interval longer than the connection interval,
/// so that the acknowledgment had a chance to be received.
@property (nonatomic,assign) NSTimeInterval transmissionTimerInterval;//0.200

/// Number of times a non-acknowledged segment will be re-send before the message will be cancelled.（segment包默认重复次数）
///
/// The limit may be decreased with increasing of `transmissionTimerInterval` as the target Node has
/// more time to reply with the Segment Acknowledgment message.
@property (nonatomic,assign) int retransmissionLimit;//5

/// The Network Transmit Count field is a 3-bit value that controls the number of
/// message transmissions of the Network PDU originating from the node.
/// The number of transmissions is the Transmit Count + 1.
/// @note   For example a value of 0b000 represents a single transmission
/// and a value of 0b111 represents 8 transmissions.
/// @note   - seeAlso: Mesh_v1.0.pdf  (page.150),
/// 4.2.19.1 Network Transmit Count.
@property (nonatomic,assign) UInt8 networkTransmitCount;//default is 0b101=5.

/// The Network Transmit Interval Steps field is a 5-bit value representing the
/// number of 10 millisecond steps that controls the interval between message
/// transmissions of Network PDUs originating from the node.
/// @note   The transmission interval is calculated using the formula:
/// transmission interval = (Network Retransmit Interval Steps + 1) * 10
/// @note   Each transmission should be perturbed by a random value
/// between 0 to 10 milliseconds between each transmission.
/// @note   For example, a value of 0b10000 represents a transmission interval
/// between 170 and 180 milliseconds between each transmission.
/// @note   - seeAlso: Mesh_v1.0.pdf  (page.151),
/// 4.2.19.2 Network Transmit Interval Steps.
@property (nonatomic,assign) UInt8 networkTransmitIntervalSteps;//default is 0b00010=2.
/// maxNetworkTransmitInterval = (networkTransmitIntervalSteps +1+1)*10*(networkTransmitCount+1)
/// default is (0b00010+1+1)*10*(0b101 + 1)=240ms.
@property (nonatomic,assign,readonly) double maxNetworkTransmitInterval;
/// When the APP receives the segment data packet reported by the device, the timer for sending the
/// ACK data packet before receiving the complete partial packet is reset. The timer is reset when the
/// segment belonging to the current message is received.
@property (nonatomic,strong,nullable) BackgroundTimer *receiveSegmentTimer;
/// Identifies whether the SDK is currently receiving segment type data packets.
@property (nonatomic,assign) BOOL isReceiveSegmentPDUing;
/// Source Address of segment PDU.
@property (nonatomic,assign) UInt16 sourceOfReceiveSegmentPDU;
/// The secureNetworkBeacon need to send after SDK setFilter success.
@property (nonatomic,strong,nullable) SigSecureNetworkBeacon *secureNetworkBeacon;
/// The meshPrivateBeacon need to send after SDK setFilter success.
@property (nonatomic,strong,nullable) SigMeshPrivateBeacon *meshPrivateBeacon;
/// secureNetworkBeacon or meshPrivateBeacon.
@property (nonatomic,assign) AppSendBeaconType sendBeaconType;

+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));


/**
 *  @brief  Singleton method
 *
 *  @return the default singleton instance. You are not allowed to create your own instances of this class.
 */
+ (instancetype)share;

#pragma mark - Receive Mesh Messages

/// This method should be called whenever a PDU has been received from the mesh network using SigBearer. When a complete Mesh Message is received and reassembled, the delegate's `didReceiveMessage:sentFromSource:toDestination:` will be called.
/// @param data The PDU received.
/// @param type The PDU type.
- (void)bearerDidDeliverData:(NSData *)data type:(SigPduType)type;

/// This method should be called whenever a PDU has been decoded from the mesh network using SigNetworkLayer.
/// @param networkPdu The network pdu in (Mesh_v1.0.pdf 3.4.4 Network PDU).
- (void)receiveNetworkPdu:(SigNetworkPdu *)networkPdu;

/// Clean busy status of receive segment.
- (void)cleanReceiveSegmentBusyStatus;

/// This method should be called whenever a PDU has been received from the kOnlineStatusCharacteristicsID.
/// @param address address of node
/// @param state state of node
/// @param bright100 bright of node
/// @param temperature100 temperature of node
- (void)updateOnlineStatusWithDeviceAddress:(UInt16)address deviceState:(DeviceState)state bright100:(UInt8)bright100 temperature100:(UInt8)temperature100;

#pragma mark - Send Mesh Messages

/// Sends Configuration Message to the Node with given destination Address.
/// The `destination` must be a Unicast Address, otherwise the method does nothing.
///
/// A `SigMessageDelegate` method will be called when the message has been sent, delivered, or fail to be sent.
///
/// @param message The message to be sent.
/// @param destination The destination Unicast Address.
/// @param initialTtl The initial TTL (Time To Live) value of the message. initialTtl is Relayed TTL.
/// @param command The command save message and callback.
/// @returns Message handle that can be used to cancel sending.
- (nullable SigMessageHandle *)sendConfigMessage:(SigConfigMessage *)message toDestination:(UInt16)destination withTtl:(UInt8)initialTtl command:(SDKLibCommand *)command;

/// Sends Configuration Message to the Node with given destination Address.
/// The `destination` must be a Unicast Address, otherwise the method does nothing.
///
/// A `SigMessageDelegate` method will be called when the message has been sent, delivered, or fail to be sent.
///
/// @param message The message to be sent.
/// @param destination The destination Unicast Address.
/// @param command The command save message and callback.
/// @returns Message handle that can be used to cancel sending.
- (nullable SigMessageHandle *)sendConfigMessage:(SigConfigMessage *)message toDestination:(UInt16)destination command:(SDKLibCommand *)command;

/// Encrypts the message with the Application Key and a Network Key bound to it, and sends to the given destination address.
///
/// A `SigMessageDelegate` method will be called when the message has been sent, delivered, or fail to be sent.
///
/// @param message The message to be sent.
/// @param localElement The source Element. If `nil`, the primary Element will be used.
/// The Element must belong to the local Provisioner's Node.
/// @param destination The destination address.
/// @param initialTtl The initial TTL (Time To Live) value of the message. initialTtl is Relayed TTL.
/// @param applicationKey The Application Key to sign the message.
/// @param command The command save message and callback.
/// @returns Message handle that can be used to cancel sending.
- (nullable SigMessageHandle *)sendMeshMessage:(SigMeshMessage *)message fromLocalElement:(nullable SigElementModel *)localElement toDestination:(SigMeshAddress *)destination withTtl:(UInt8)initialTtl usingApplicationKey:(SigAppkeyModel *)applicationKey command:(SDKLibCommand *)command;

/// Encrypts the message with the Application Key and a Network Key bound to it, and sends to the given destination address.
///
/// A `SigMessageDelegate` method will be called when the message has been sent, delivered, or fail to be sent.
///
/// @param message The message to be sent.
/// @param localElement The source Element. If `nil`, the primary Element will be used.
/// The Element must belong to the local Provisioner's Node.
/// @param destination The destination address.
/// @param applicationKey The Application Key to sign the message.
/// @param command The command save message and callback.
/// @returns Message handle that can be used to cancel sending.
- (nullable SigMessageHandle *)sendMeshMessage:(SigMeshMessage *)message fromLocalElement:(nullable SigElementModel *)localElement toDestination:(SigMeshAddress *)destination usingApplicationKey:(SigAppkeyModel *)applicationKey command:(SDKLibCommand *)command;

/// Sends the Proxy Configuration Message to the connected Proxy Node.
/// @param message The Proxy Configuration message to be sent.
/// @param command The command save message and callback.
- (nullable SigMessageHandle *)sendSigProxyConfigurationMessage:(SigProxyConfigurationMessage *)message command:(SDKLibCommand *)command;

/// Advertising Solicitation PDU With Mesh Message.
/// @param source The source Unicast Address.
/// @param destination The destination address of the message received.
/// @param interval advertising interval
/// @param result advertising result
- (void)advertisingSolicitationPDUWithSource:(UInt16)source destination:(UInt16)destination advertisingInterval:(NSTimeInterval)interval result:(advertisingResult)result;

/// Sends the telink's onlineStatus command.
/// @param message The onlineStatus message to be sent.
/// @param command The command save message and callback.
/// @returns return `nil` when send message successful.
- (nullable NSError *)sendTelinkApiGetOnlineStatueFromUUIDWithMessage:(SigMeshMessage *)message command:(SDKLibCommand *)command;

/// Cancels sending the message with the given identifier.
/// @param messageId The message identifier.
- (void)cancelSigMessageHandle:(SigMessageHandle *)messageId;

/// cancel all commands and retry of commands and retry of segment PDU.
- (void)cleanAllCommandsAndRetry;

/// cancel all commands and retry of commands and retry of segment PDU when mesh disconnected.
- (void)cleanAllCommandsAndRetryWhenMeshDisconnected;

/// Returns whether SigMeshLib is busy. YES means busy.
- (BOOL)isBusyNow;

@end

NS_ASSUME_NONNULL_END
