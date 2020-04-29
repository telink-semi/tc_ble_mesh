/********************************************************************************************************
* @file     CBUUID+Hex.h
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
//  CBUUID+Hex.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/10/16.
//  Copyright © 2019 Telink. All rights reserved.
//

//#import <AppKit/AppKit.h>


#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface CBUUID (Hex)

/// Creates the UUID from a 32-character hexadecimal string.
- (instancetype)initWithHex:(NSString *)hex;

/// Returns the uuidString without dashes.
- (NSString *)getHex;

/// The UUID as Data.
- (NSData *)getData;

@end

NS_ASSUME_NONNULL_END
