/********************************************************************************************************
 * @file     SigNetworkLayer.h
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
//  SigNetworkLayer.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/9.
//  Copyright © 2019 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class SigLowerTransportPdu,SigNetworkManager,SigSegmentAcknowledgmentMessage;

@interface SigNetworkLayer : NSObject
@property (nonatomic,strong) SigNetworkManager *networkManager;
@property (nonatomic,strong) SigDataSource *meshNetwork;
//@property (nonatomic,strong) NSCache <NSData *, NSNull *>*networkMessageCache;
//@property (nonatomic,strong) NSUserDefaults *defaults;
@property (nonatomic,strong) SigIvIndex *ivIndex;
@property (nonatomic,strong) SigNetkeyModel *networkKey;
@property (nonatomic,strong,nullable) SigSegmentAcknowledgmentMessage *lastNeedSendAckMessage;

- (instancetype)initWithNetworkManager:(SigNetworkManager *)networkManager;

/// This method handles the received PDU of given type and
/// passes it to Upper Transport Layer.
///
/// - parameter pdu:  The data received.
/// - parameter type: The PDU type.
- (void)handleIncomingPdu:(NSData *)pdu ofType:(SigPduType)type;

- (void)sendLowerTransportPdu:(SigLowerTransportPdu *)pdu ofType:(SigPduType)type withTtl:(UInt8)ttl ivIndex:(SigIvIndex *)ivIndex;

/// This method tries to send the Lower Transport Message of given type to the
/// given destination address. If the local Provisioner does not exist, or
/// does not have Unicast Address assigned, this method does nothing.
///
/// - parameter pdu:  The Lower Transport PDU to be sent.
/// - parameter type: The PDU type.
/// - parameter ttl:  The initial TTL (Time To Live) value of the message.
/// - throws: This method may throw when the `transmitter` is not set, or has
///           failed to send the PDU.
- (void)sendLowerTransportPdu:(SigLowerTransportPdu *)pdu ofType:(SigPduType)type withTtl:(UInt8)ttl;

/// This method tries to send the Proxy Configuration Message.
///
/// The Proxy Filter object will be informed about the success or a failure.
///
/// - parameter message: The Proxy Confifuration message to be sent.
- (void)sendSigProxyConfigurationMessage:(SigProxyConfigurationMessage *)message;

/// Updates the information about the Network Key known to the current Proxy Server.
/// The Network Key is required to send Proxy Configuration Messages that can be
/// decoded by the connected Proxy.
///
/// If the method detects that the Proxy has just been connected, or was reconnected,
/// it will initiate the Proxy Filter with local Provisioner's Unicast Address and
/// the `Address.allNodes` group address.
///
/// - parameter networkKey: The Network Key known to the connected Proxy.
//- (void)updateProxyFilterUsingNetworkKey:(SigNetkeyModel *)networkKey;

@end

NS_ASSUME_NONNULL_END
