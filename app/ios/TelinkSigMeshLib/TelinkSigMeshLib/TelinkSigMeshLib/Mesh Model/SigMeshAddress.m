/********************************************************************************************************
* @file     SigMeshAddress.m
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
//  SigMeshAddress.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/10/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigMeshAddress.h"
#import "CBUUID+Hex.h"
#import "OpenSSLHelper.h"

@implementation SigMeshAddress

- (instancetype)initWithHex:(NSString *)hex {
    if (hex.length == 4) {
        return [self initWithAddress:[LibTools uint16From16String:hex]];
    }else{
        CBUUID *virtualLabel = [[CBUUID alloc] initWithHex:hex];
        if (virtualLabel) {
            return [self initWithVirtualLabel:virtualLabel];
        }
    }
    return nil;
}

/// Creates a Mesh Address. For virtual addresses use
/// `init(_ virtualAddress:UUID)` instead.
///
/// This method will be used for Virtual Address
/// if the Virtual Label is not known, that is in
/// `ConfigModelPublicationStatus`.
-(instancetype)initWithAddress:(UInt16)address {
    if (self = [super init]) {
        _address = address;
        _virtualLabel = nil;
    }
    return self;
}

/// Creates a Mesh Address based on the virtual label.
- (instancetype)initWithVirtualLabel:(CBUUID *)virtualLabel {
    if (self = [super init]) {
        _virtualLabel = virtualLabel;
        
        // Calculate the 16-bit virtual address based on the 128-bit label.
        NSData *salt = [OpenSSLHelper.share calculateSalt:[@"vtad" dataUsingEncoding:kCFStringEncodingASCII]];
        NSData *hash = [OpenSSLHelper.share calculateCMAC:[LibTools nsstringToHex:_virtualLabel.UUIDString] andKey:salt];
        UInt16 address = CFSwapInt16HostToBig([LibTools uint16FromBytes:[hash subdataWithRange:NSMakeRange(14, 2)]]);
        address |= 0x8000;
        address &= 0xBFFF;
        _address = address;
    }
    return self;
}

- (NSString *)getHex {
    if (_virtualLabel != nil) {
        return _virtualLabel.getHex;
    }
    return [NSString stringWithFormat:@"%04d",_address];
}

- (BOOL)isEqual:(id)object {
    if ([object isKindOfClass:[SigMeshAddress class]]) {
        return _address == ((SigMeshAddress *)object).address;
    } else {
        return NO;
    }
}

@end
