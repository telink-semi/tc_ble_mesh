/********************************************************************************************************
 * @file     SigMeshMessage.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/8/15
 *
 * @par     Copyright (c) [2021], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/

#import "SigMeshMessage.h"

@implementation SigBaseMeshMessage
@end


/// The base class of every mesh message. Mesh messages can be sent and
/// and received from the mesh network. For messages with the opcode known
/// during compilation a `StaticMeshMessage` protocol should be preferred.
///
/// Parameters `security` and `isSegmented` are checked and should be set
/// only for outgoing messages.
@implementation SigMeshMessage

/// This initializer should construct the message based on the received
/// parameters.
///
/// - parameter parameters: The Access Layer parameters.
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
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
//    return SigMeshMessageSecurityLow;
    return SigMeshLib.share.dataSource.security;
}

- (BOOL)isSegmented {
    return [self accessPdu].length > SigMeshLib.share.dataSource.defaultUnsegmentedMessageLowerTransportPDUMaxLength;
}

/// The Access Layer PDU data that will be sent.
- (NSData *)accessPdu {
    // Op Code 0b01111111 is invalid. We will ignore this case here
    // now and send as single byte OpCode.
    SigOpCodeType type = [SigHelper.share getOpCodeTypeWithUInt32Opcode:_opCode];
    UInt8 tem = 0;
//    if (_opCode < 0x80) {
    if (type == SigOpCodeType_sig1) {
        tem = _opCode & 0xFF;
        NSMutableData *temData = [NSMutableData dataWithBytes:&tem length:1];
        [temData appendData:self.parameters];
        return temData;
    }
//    if (_opCode < 0x4000 || (_opCode & 0xFFFC00) == 0x8000) {
    if (type == SigOpCodeType_sig2) {
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
        TelinkLogInfo(@"Low (32-bit TransMIC)");
    } else {
        TelinkLogInfo(@"High (64-bit TransMIC)");
    }
}

/// Whether the message is an acknowledged message, or not.
- (BOOL)isAcknowledged {
    return [self isMemberOfClass:SigAcknowledgedMeshMessage.class];
}

@end


/// A mesh message containing the operation status.
@implementation SigStatusMessage
@end


/// A message with Transaction Identifier.
///
/// The Transaction Identifier will automatically be set and incremented
/// each time a message is sent. The counter is reuesed for all types that
/// extend this protocol.
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


/// The base class for acknowledged messages.
///
/// An acknowledged message is transmitted and acknowledged by each
/// receiving element by responding to that message. The response is
/// typically a status message. If a response is not received within
/// an arbitrary time period, the message will be retransmitted
/// automatically until the timeout occurs.
@implementation SigAcknowledgedMeshMessage
@end


@implementation SigStaticMeshMessage
@end


@implementation SigUnknownMessage
/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = 0;
    }
    return self;
}

/**
 * @brief   Initialize SigUnknownMessage object.
 * @param   parameters    the hex data of SigUnknownMessage.
 * @return  return `nil` when initialize SigUnknownMessage object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    if (self = [super initWithParameters:parameters]) {
        self.opCode = 0;
    }
    return self;
}
@end


@implementation SigIniMeshMessage
@end

