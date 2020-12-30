/********************************************************************************************************
 * @file     SigMeshMessage.m
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
//  SigMeshMessage.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/8/15.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import "SigMeshMessage.h"

@implementation SigBaseMeshMessage
@end

@implementation SigMeshMessage

- (instancetype)initWithParameters:(NSData *)parameters {
    if (self = [super init]) {
        self.parameters = parameters;
    }
    return self;
}

/// Returns the Transport MIC size in bytes: 4 for 32-bit
/// or 8 for 64-bit size.
- (UInt8)getTransportMicSizeOfMeshMessageSecurity:(SigMeshMessageSecurity)meshMessageSecurity {
    if (meshMessageSecurity == SigMeshMessageSecurityLow) {
        return 4;
    } else {
        return 8;
    }
}

- (SigMeshMessageSecurity)security {
    return SigMeshMessageSecurityLow;
}

- (BOOL)isSegmented {
    return [self accessPdu].length > SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength;
}

/// The Access Layer PDU data that will be sent.
- (NSData *)accessPdu {
    // Op Code 0b01111111 is invalid. We will ignore this case here
    // now and send as single byte OpCode.
    UInt8 tem = 0;
    if (_opCode < 0x80) {
        tem = _opCode & 0xFF;
        NSMutableData *temData = [NSMutableData dataWithBytes:&tem length:1];
        [temData appendData:self.parameters];
        return temData;
    }
    if (_opCode < 0x4000 || (_opCode & 0xFFFC00) == 0x8000) {
        tem = 0x80 | ((_opCode >> 8) & 0x3F);
        NSMutableData *temData = [NSMutableData dataWithBytes:&tem length:1];
        tem = _opCode & 0xFF;
        [temData appendData:[NSData dataWithBytes:&tem length:1]];
        [temData appendData:self.parameters];
        return temData;
    }
    tem = 0xC0 | ((_opCode >> 16) & 0x3F);
    NSMutableData *temData = [NSMutableData dataWithBytes:&tem length:1];
    tem = (_opCode >> 8) & 0xFF;
    [temData appendData:[NSData dataWithBytes:&tem length:1]];
    tem = _opCode & 0xFF;
    [temData appendData:[NSData dataWithBytes:&tem length:1]];
    [temData appendData:self.parameters];
    return temData;
}

/// Whether the message is a Vendor Message, or not.
///
/// Vendor messages use 3-byte Op Codes, where the 2 most significant
/// bits of the first octet are set to 1. The remaining bits of the
/// first octet are the operation code, while the last 2 bytes are the
/// Company Identifier (Big Endian), as registered by Bluetooth SIG.
- (BOOL)isVendorMessage {
    return (_opCode & 0xFFC00000) == 0x00C00000;
}

- (void)showMeshMessageSecurity:(SigMeshMessageSecurity)meshMessageSecurity {
    if (meshMessageSecurity == SigMeshMessageSecurityLow) {
        TeLogInfo(@"Low (32-bit TransMIC)");
    } else {
        TeLogInfo(@"High (64-bit TransMIC)");
    }
}

/// Whether the message is an acknowledged message, or not.
- (BOOL)isAcknowledged {
    return [self isMemberOfClass:SigAcknowledgedMeshMessage.class];
}

@end

@implementation SigStatusMessage
@end


@implementation SigTransactionMessage
- (void)setTid:(UInt8)tid{
    _isInitTid = YES;
    _tid = tid;
}
- (BOOL)isInitTid {
    return _tid != 0 || _isInitTid;
}
@end

@implementation SigTransitionMessage
@end

@implementation SigTransitionStatusMessage
@end

@implementation SigAcknowledgedMeshMessage
@end

@implementation SigStaticMeshMessage
@end

@implementation SigUnknownMessage
- (instancetype)init {
    if (self = [super init]) {
        self.opCode = 0;
    }
    return self;
}
- (instancetype)initWithParameters:(NSData *)parameters {
    if (self = [super initWithParameters:parameters]) {
        self.opCode = 0;
    }
    return self;
}
@end

@implementation SigIniMeshMessage
@end

