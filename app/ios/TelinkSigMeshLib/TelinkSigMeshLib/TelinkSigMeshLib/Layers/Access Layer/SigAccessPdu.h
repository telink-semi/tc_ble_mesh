/********************************************************************************************************
 * @file     SigAccessPdu.h
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
//  SigAccessPdu.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class SigUpperTransportPdu,SigMeshAddress;

/// 3.7.3 Access payload
/// - seeAlso: Mesh_v1.0.pdf  (page.92)
@interface SigAccessPdu : NSObject
/// Operation Code. Size is 1, 2, or 3 bytes.
@property (nonatomic,assign) UInt32 opCode;
/// Application Parameters. Size is 0 ~ 379.
@property (nonatomic,strong) NSData *parameters;
/// Number of packets for this PDU.
///
/// Number of Packets | Maximum useful access payload size (octets)
///            | 32 bit TransMIC    | 64 bit TransMIC
/// -----------------+---------------------+-------------------------
/// 1                      | 11 (unsegmented) | n/a
/// 1                      | 8 (segmented)       | 4 (segmented)
/// 2                      | 20                          | 16
/// 3                      | 32                          | 28
/// n                      | (n×12)-4                 | (n×12)-8
/// 32                    | 380                        | 376
- (int)segmentsCount;




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
@property (nonatomic,strong) SigMeshAddress *destination;
/// The Access Layer PDU data that will be sent.
@property (nonatomic,strong) NSData *accessPdu;

@property (nonatomic,assign) SigLowerTransportPduType isAccessMessage;
/// Whether the outgoind message will be sent as segmented, or not.
- (BOOL)isSegmented;

- (instancetype)initFromUpperTransportPdu:(SigUpperTransportPdu *)pdu;

- (instancetype)initFromMeshMessage:(SigMeshMessage *)message sentFromLocalElement:(SigElementModel *)localElement toDestination:(SigMeshAddress *)destination userInitiated:(BOOL)userInitiated;

@end

NS_ASSUME_NONNULL_END
