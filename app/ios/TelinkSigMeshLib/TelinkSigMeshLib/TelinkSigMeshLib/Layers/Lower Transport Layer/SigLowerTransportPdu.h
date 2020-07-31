/********************************************************************************************************
* @file     SigLowerTransportPdu.h
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
//  SigLowerTransportPdu.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface SigLowerTransportPdu : NSObject
/// Source Address.
@property (nonatomic,assign) UInt16 source;
/// Destination Address.
@property (nonatomic,assign) UInt16 destination;
/// The Network Key used to decode/encode the PDU.
@property (nonatomic,strong) SigNetkeyModel *networkKey;
@property (nonatomic,strong) SigIvIndex *ivIndex;
/// Message type.
@property (nonatomic,assign) SigLowerTransportPduType type;
/// The raw data of Lower Transport Layer PDU.
@property (nonatomic,strong) NSData *transportPdu;
/// The raw data of Upper Transport Layer PDU.
@property (nonatomic,strong) NSData *upperTransportPdu;

@end

NS_ASSUME_NONNULL_END
