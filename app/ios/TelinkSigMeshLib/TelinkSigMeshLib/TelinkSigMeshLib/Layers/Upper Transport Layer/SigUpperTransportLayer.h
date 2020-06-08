/********************************************************************************************************
 * @file     SigUpperTransportLayer.h
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
//  SigUpperTransportLayer.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class SigLowerTransportPdu,SigHearbeatMessage,SigNetworkManager,SigAccessPdu;

@interface SigUpperTransportLayer : NSObject

@property (nonatomic,strong) SigNetworkManager *networkManager;

- (instancetype)initWithNetworkManager:(SigNetworkManager *)networkManager;

/// Handles received Lower Transport PDU.
/// Depending on the PDU type, the message will be either propagated to
/// Access Layer, or handled internally.
///
/// - parameter lowetTransportPdu: The Lower Trasport PDU received.
- (void)handleLowerTransportPdu:(SigLowerTransportPdu *)lowerTransportPdu;

- (void)sendAccessPdu:(SigAccessPdu *)accessPdu withTtl:(UInt8)initialTtl usingKeySet:(SigKeySet *)keySet command:(SDKLibCommand *)command;

/// Encrypts the Access PDU using given key set and sends it down to
/// Lower Transport Layer.
///
/// - parameters:
///   - pdu: The Access PDU to be sent.
///   - initialTtl: The initial TTL (Time To Live) value of the message.
///                 If `nil`, the default Node TTL will be used.
///   - keySet: The set of keys to encrypt the message with.
- (void)sendAccessPdu:(SigAccessPdu *)accessPdu withTtl:(UInt8)initialTtl usingKeySet:(SigKeySet *)keySet;

- (void)cancelHandleSigMessageHandle:(SigMessageHandle *)handle;

/// A callback called by the lower transport layer when the segmented PDU
/// has been sent to the given destination.
///
/// This method removes the sent PDU from the queue and initiates sending
/// a next one, had it been enqueued.
///
/// - parameter destination: The destination address.
- (void)lowerTransportLayerDidSendSegmentedUpperTransportPduToDestination:(UInt16)destination;

/// Handles the Mesh Message and sends it down to Lower Transport Layer.
///
/// - parameter message: The message to be sent.
/// - parameter destination: The destination address. This can be any type of
///                          valid address.
/// - parameter applicationKey: The Application Key to sign the message with.
//- (void)sendMeshMessage:(SigMeshMessage *)message toDestination:(UInt16)destination usingApplicationKey:(SigAppkeyModel *)appkey;

/// Handles the Config Message and sends it down to Lower Transport Layer.
///
/// - parameter message: The message to be sent.
/// - parameter destination: The destination address. This must be a Unicast Address.
//- (void)sendConfigMessage:(SigConfigMessage *)message toDestination:(UInt16)destination;

- (void)handleHearbeat:(SigHearbeatMessage *)hearbeat;

@end

NS_ASSUME_NONNULL_END
