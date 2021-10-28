/********************************************************************************************************
 * @file     SigUpperTransportLayer.h
 *
 * @brief    for TLSR chips
 *
 * @author       Telink, 梁家誌
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
@property (nonatomic,strong) SigUpperTransportPdu *upperTransportPdu;

- (instancetype)initWithNetworkManager:(SigNetworkManager *)networkManager;

/// Handles received Lower Transport PDU.
/// Depending on the PDU type, the message will be either propagated to Access Layer, or handled internally.
/// @param lowerTransportPdu The Lower Trasport PDU received.
- (void)handleLowerTransportPdu:(SigLowerTransportPdu *)lowerTransportPdu;

/// Encrypts the Access PDU using given key set and sends it down to Lower Transport Layer.
/// @param accessPdu The Access PDU to be sent.
/// @param initialTtl The initial TTL (Time To Live) value of the message. If `nil`, the default Node TTL will be used.
/// @param keySet The set of keys to encrypt the message with.
/// @param command The command of the message.
- (void)sendAccessPdu:(SigAccessPdu *)accessPdu withTtl:(UInt8)initialTtl usingKeySet:(SigKeySet *)keySet command:(SDKLibCommand *)command;

/// Cancels sending all segmented messages matching given handle.
/// Unsegmented messages are sent almost instantaneously and cannot be cancelled.
/// @param handle The message handle.
- (void)cancelHandleSigMessageHandle:(SigMessageHandle *)handle;

/// A callback called by the lower transport layer when the segmented PDU has been sent to the given destination.
/// This method removes the sent PDU from the queue and initiates sending a next one, had it been enqueued.
/// @param destination The destination address.
- (void)lowerTransportLayerDidSendSegmentedUpperTransportPduToDestination:(UInt16)destination;

- (void)handleHearbeat:(SigHearbeatMessage *)hearbeat;

@end

NS_ASSUME_NONNULL_END
