/********************************************************************************************************
* @file     CBUUID+Hex.m
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
//  CBUUID+Hex.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/10/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "CBUUID+Hex.h"

//#import <AppKit/AppKit.h>


@implementation CBUUID (Hex)

/// Creates the UUID from a 32-character hexadecimal string.
- (instancetype)initWithHex:(NSString *)hex {
    if(self = [super init]){
        if (hex.length != 32) {
            return nil;
        }
        NSString *uuidString = [NSString stringWithFormat:@"%@_%@_%@_%@_%@",[hex substringWithRange:NSMakeRange(0, 8)],[hex substringWithRange:NSMakeRange(8, 4)],[hex substringWithRange:NSMakeRange(12, 4)],[hex substringWithRange:NSMakeRange(16, 4)],[hex substringWithRange:NSMakeRange(20, 12)]];
        CBUUID *uuid = [CBUUID UUIDWithString:uuidString];
        if(uuid == nil){
            return nil;
        }
        return uuid;
    }
    return nil;
}

/// Returns the uuidString without dashes.
- (NSString *)getHex {
    return [self.UUIDString stringByReplacingOccurrencesOfString:@"-" withString:@""];
}

/// The UUID as Data.
- (NSData *)getData {
    return [LibTools nsstringToHex:self.UUIDString];
}

@end
