/********************************************************************************************************
 * @file     SigUpperTransportPdu.h
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
//  SigUpperTransportPdu.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class SigAccessMessage,SigKeySet,SigAccessPdu;

@interface SigUpperTransportPdu : NSObject

/// The Mesh Message that is being sent, or `nil`, when the message
/// was received.
@property (nonatomic,strong) SigMeshMessage *message;
/// The local Element that is sending the message, or `nil` when the
/// message was received.
@property (nonatomic,strong) SigElementModel *localElement;
/// Whether sending this message has been initiated by the user.
@property (nonatomic,assign) BOOL userInitiated;
/// Source Address.
@property (nonatomic,assign) UInt16 source;
/// Destination Address.
@property (nonatomic,assign) UInt16 destination;
/// Application Key Flag
@property (nonatomic,assign) BOOL AKF;
/// 6-bit Application Key identifier. This field is set to `nil`
/// if the message is signed with a Device Key instead.
@property (nonatomic,assign) UInt8 aid;
/// The sequence number used to encode this message.
@property (nonatomic,assign) UInt32 sequence;
/// The size of Transport MIC: 4 or 8 bytes.
@property (nonatomic,assign) UInt8 transportMicSize;
/// The Access Layer data.
@property (nonatomic,strong) NSData *accessPdu;
/// The raw data of Upper Transport Layer PDU.
@property (nonatomic,strong) NSData *transportPdu;

- (instancetype)initFromLowerTransportAccessMessage:(SigAccessMessage *)accessMessage key:(NSData *)key;
- (instancetype)initFromLowerTransportAccessMessage:(SigAccessMessage *)accessMessage key:(NSData *)key forVirtualGroup:(SigGroupModel *)virtualGroup;
- (instancetype)initFromLowerTransportAccessMessage:(SigAccessMessage *)accessMessage key:(NSData *)key ivIndex:(SigIvIndex *)ivIndex forVirtualGroup:(SigGroupModel *)virtualGroup;
- (instancetype)initFromAccessPdu:(SigAccessPdu *)pdu usingKeySet:(SigKeySet *)keySet ivIndex:(SigIvIndex *)ivIndex sequence:(UInt32)sequence;
- (instancetype)initFromAccessPdu:(SigAccessPdu *)pdu usingKeySet:(SigKeySet *)keySet sequence:(UInt32)sequence;
+ (NSDictionary *)decodeAccessMessage:(SigAccessMessage *)accessMessage forMeshNetwork:(SigDataSource *)meshNetwork;//{@"SigUpperTransportPdu":SigUpperTransportPdu,@"SigKeySet":SigKeySet}

@end

NS_ASSUME_NONNULL_END
