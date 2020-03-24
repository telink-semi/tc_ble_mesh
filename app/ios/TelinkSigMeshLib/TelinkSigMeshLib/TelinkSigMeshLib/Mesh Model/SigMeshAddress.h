/********************************************************************************************************
* @file     SigMeshAddress.h
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
//  SigMeshAddress.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/10/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface SigMeshAddress : NSObject
/// 16-bit address.
@property (nonatomic, assign) UInt16 address;
/// Virtual label UUID.
@property (nonatomic, strong) CBUUID *virtualLabel;

- (instancetype)initWithHex:(NSString *)hex;

/// Creates a Mesh Address. For virtual addresses use
/// `init(_ virtualAddress:UUID)` instead.
///
/// This method will be used for Virtual Address
/// if the Virtual Label is not known, that is in
/// `ConfigModelPublicationStatus`.
-(instancetype)initWithAddress:(UInt16)address;

/// Creates a Mesh Address based on the virtual label.
- (instancetype)initWithVirtualLabel:(CBUUID *)virtualLabel;

- (NSString *)getHex;

@end

NS_ASSUME_NONNULL_END
