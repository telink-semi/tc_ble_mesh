/********************************************************************************************************
* @file     SigLowerTransportLayer.h
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
//  SigLowerTransportLayer.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class SigSegmentedMessage,SigUpperTransportPdu,SigNetkeyModel,SigSegmentAcknowledgmentMessage;

@interface SigLowerTransportLayer : NSObject

@property (nonatomic,strong) SigNetworkManager *networkManager;

//#prage mark out going segments

/// 缓存APP端发送到设备端的Segment数据包列表。The key is the `sequenceZero` of the message.
@property (nonatomic,strong) NSMutableDictionary <NSNumber *,NSMutableArray <SigSegmentedMessage *>*>*outgoingSegments;
/// 缓存APP端发送Segment数据包列表的定时器。The key is the `sequenceZero` of the message.（用于判断outgoingSegments的数据包是否有发送失败的，失败则再发送一次。）
@property (nonatomic,strong) NSMutableDictionary <NSNumber *,BackgroundTimer *>*segmentTransmissionTimers;
/// 缓存重发segment数据包的定时器。(设备端不返回这个Segment长包的接收情况，该定时器是segment发包和重发包的总超时定时器，该定时器执行则表示该segment包发送失败，设备端没收全APP发送的segment包。)
@property (nonatomic,strong) NSMutableDictionary <NSNumber *,BackgroundTimer *>*incompleteTimers;
/// An item is removed when a next message has been received from the same Node.
/// 缓存接收到的Segment Acknowlegment Messages。（设备端返回这个APP发送的Segment长包的接收情况，APP需要补充丢失的包或者停止重发segment数据包的定时器incompleteTimers）
@property (nonatomic,strong) NSMutableDictionary <NSNumber *,SigSegmentAcknowledgmentMessage *>*acknowledgments;

//#prage mark in going segments

/// 缓存接收到的segment数据包。（用于返回app接收到segment的情况，定时器）
@property (nonatomic,strong) NSMutableDictionary <NSNumber *,NSMutableArray <SigSegmentedMessage *>*>*incompleteSegments;
/// 缓存APP端向设备端发送Segment Acknowlegment Messages的定时器。(告诉设备端一个Segment长包中哪个包丢失了或者所有包都接收了。)
@property (nonatomic,strong) NSMutableDictionary <NSNumber *,BackgroundTimer *>*acknowledgmentTimers;



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

- (void)sendUnsegmentedUpperTransportPdu:(SigUpperTransportPdu *)pdu withTtl:(UInt8)initialTtl usingNetworkKey:(SigNetkeyModel *)networkKey ivIndex:(SigIvIndex *)ivIndex;

/// This method tries to send the Upper Transport Message.
///
/// - parameters:
///   - pdu:        The unsegmented Upper Transport PDU to be sent.
///   - initialTtl: The initial TTL (Time To Live) value of the message.
///                 If `nil`, the default Node TTL will be used.
///   - networkKey: The Network Key to be used to encrypt the message on
///                 on Network Layer.
- (void)sendUnsegmentedUpperTransportPdu:(SigUpperTransportPdu *)pdu withTtl:(UInt8)initialTtl usingNetworkKey:(SigNetkeyModel *)networkKey;

- (void)sendSegmentedUpperTransportPdu:(SigUpperTransportPdu *)pdu withTtl:(UInt8)initialTtl usingNetworkKey:(SigNetkeyModel *)networkKey ivIndex:(SigIvIndex *)ivIndex;

/// This method tries to send the Upper Transport Message.
///
/// - parameter pdu:        The segmented Upper Transport PDU to be sent.
/// - parameter initialTtl: The initial TTL (Time To Live) value of the message.
///                         If `nil`, the default Node TTL will be used.
/// - parameter networkKey: The Network Key to be used to encrypt the message on
///                         on Network Layer.
- (void)sendSegmentedUpperTransportPdu:(SigUpperTransportPdu *)pdu withTtl:(UInt8)initialTtl usingNetworkKey:(SigNetkeyModel *)networkKey;

- (void)cancelTXSendingSegmentedWithDestination:(UInt16)destination;

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
