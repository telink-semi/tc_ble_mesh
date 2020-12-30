/********************************************************************************************************
 * @file     SigAccessPdu.m
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
//  SigAccessPdu.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigAccessPdu.h"
#import "SigUpperTransportPdu.h"

@implementation SigAccessPdu

- (BOOL)isSegmented {
    if (_message == nil) {
        return NO;
    }
    return _accessPdu.length > SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength || _message.isSegmented;
}

- (int)segmentsCount {
    if (_message == nil) {
        return 0;
    }
    if (![self isSegmented]) {
        return 1;
    }
    switch (_message.security) {
        case SigMeshMessageSecurityLow:
            return 1 + (int)((_accessPdu.length + 3) / (SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength - 3));
            break;
        case SigMeshMessageSecurityHigh:
            return 1 + (int)((_accessPdu.length + 7) / (SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength - 3));
            break;
        default:
            break;
    }
}

- (instancetype)init {
    if (self = [super init]) {
        _isAccessMessage = SigLowerTransportPduType_accessMessage;
    }
    return self;
}

- (instancetype)initFromUpperTransportPdu:(SigUpperTransportPdu *)pdu {
    if (self = [super init]) {
        _isAccessMessage = SigLowerTransportPduType_accessMessage;
        _message = nil;
        _localElement = nil;
        _userInitiated = NO;
        _source = pdu.source;
        _destination = [[SigMeshAddress alloc] initWithAddress:pdu.destination];
        _accessPdu = pdu.accessPdu;
        
        // At least 1 octet is required.
        if (pdu.accessPdu == nil || pdu.accessPdu.length == 0) {
            TeLogError(@"pdu.accessPdu has not data.");
            return nil;
        }
        UInt8 octet0 = 0;
        Byte *dataByte = (Byte *)pdu.accessPdu.bytes;
        memcpy(&octet0, dataByte, 1);

        if (octet0 == 0b01111111) {
            TeLogError(@"Opcode 0b01111111 is reseved for future use.");
            return nil;
        }

        // 1-octet Opcodes.
        if ((octet0 & 0x80) == 0) {
            _opCode = (UInt32)octet0;
            _parameters = [pdu.accessPdu subdataWithRange:NSMakeRange(1, pdu.accessPdu.length - 1)];
            return self;
        }
        // 2-octet Opcodes.
        if ((octet0 & 0x40) == 0) {
            // At least 2 octets are required.
            if (pdu.accessPdu.length < 2) {
                TeLogError(@"pdu.accessPdu is error.");
                return nil;
            }
            UInt8 octet1 = 0;
            memcpy(&octet1, dataByte+1, 1);
            _opCode = (UInt32)octet0 << 8 | (UInt32)octet1;
            _parameters = [pdu.accessPdu subdataWithRange:NSMakeRange(2, pdu.accessPdu.length - 2)];
            return self;
        }
        // 3-octet Opcodes.
        // At least 3 octets are required.
        if (pdu.accessPdu.length < 3) {
            TeLogError(@"pdu.accessPdu is error.");
            return nil;
        }
        UInt8 octet1 = 0;
        UInt8 octet2 = 0;
        memcpy(&octet1, dataByte+1, 1);
        memcpy(&octet2, dataByte+2, 1);
        _opCode = (UInt32)octet0 << 16 | (UInt32)octet1 << 8 | (UInt32)octet2;
        _parameters = [pdu.accessPdu subdataWithRange:NSMakeRange(3, pdu.accessPdu.length - 3)];
    }
    return self;
}

- (instancetype)initFromMeshMessage:(SigMeshMessage *)message sentFromLocalElement:(SigElementModel *)localElement toDestination:(SigMeshAddress *)destination userInitiated:(BOOL)userInitiated {
    if (self = [super init]) {
        _isAccessMessage = SigLowerTransportPduType_accessMessage;
        _message = message;
        _localElement = localElement;
        _userInitiated = userInitiated;
        _source = localElement.unicastAddress;
        _destination = destination;
        
        _opCode = message.opCode;
        if (message.parameters != nil) {
            _parameters = message.parameters;
        } else {
            _parameters = [NSData data];
        }
        
        // Op Code 0b01111111 is invalid. We will ignore this case here
        // for now and send as a single byte OpCode.
        // TODO: Handle 0b0111111 opcode correctly.
        UInt8 tem = 0;
        if (_opCode < 0x80) {//1-octet Opcodes
            tem = (UInt8)_opCode&0xFF;
            NSMutableData *data1 = [NSMutableData dataWithBytes:&tem length:1];
            [data1 appendData:_parameters];
            _accessPdu = data1;
        }else if (_opCode < 0x4000 || (_opCode & 0xFFFC00) == 0x8000 || (_opCode & 0xFFB600) == 0xB600) {//2-octet Opcodes
            tem = (UInt8)(0x80 | ((_opCode >> 8) & 0x3F));
            NSMutableData *data1 = [NSMutableData dataWithBytes:&tem length:1];
            tem = (UInt8)(_opCode & 0xFF);
            [data1 appendData:[NSData dataWithBytes:&tem length:1]];
            [data1 appendData:_parameters];
            _accessPdu = data1;
        }else{//3-octet Opcodes
            tem = (UInt8)(0xC0 | ((_opCode >> 16) & 0x3F));
            NSMutableData *data1 = [NSMutableData dataWithBytes:&tem length:1];
            tem = (UInt8)((_opCode >> 8) & 0xFF);
            [data1 appendData:[NSData dataWithBytes:&tem length:1]];
            tem = (UInt8)(_opCode & 0xFF);
            [data1 appendData:[NSData dataWithBytes:&tem length:1]];
            [data1 appendData:_parameters];
            _accessPdu = data1;
        }
    }
    return self;
}

- (NSString *)description {
    return [NSString stringWithFormat:@"Access PDU, source:(0x%04X)->destination: (0x%04X) Op Code: (0x%X), accessPdu=%@", _source, _destination.address, (unsigned int)_opCode,[LibTools convertDataToHexStr:_accessPdu]];
}

@end
