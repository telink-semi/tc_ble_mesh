/********************************************************************************************************
 * @file     SigAccessPdu.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/9/16
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

#import "SigAccessPdu.h"
#import "SigUpperTransportPdu.h"

@implementation SigAccessPdu

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _isAccessMessage = SigLowerTransportPduType_accessMessage;
    }
    return self;
}

/// Initialize SigAccessPdu object.
/// @param pdu The SigUpperTransportPdu object.
/// @returns return `nil` when initialize SigAccessPdu object fail.
- (instancetype)initFromUpperTransportPdu:(SigUpperTransportPdu *)pdu {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _isAccessMessage = SigLowerTransportPduType_accessMessage;
        _message = nil;
        _localElement = nil;
        _userInitiated = NO;
        _source = pdu.source;
        _destination = [[SigMeshAddress alloc] initWithAddress:pdu.destination];
        _accessPdu = pdu.accessPdu;

        SigOpCodeAndParametersModel *model = [[SigOpCodeAndParametersModel alloc] initWithOpCodeAndParameters:pdu.accessPdu];
        if (model == nil) {
            return nil;
        }
        _opCode = model.opCode;
        _parameters = model.parameters;
    }
    return self;
}

/// Initialize SigAccessPdu object.
/// @param message The Mesh Message that is being sent, or `nil`, when the message
/// was received.
/// @param localElement The local Element that is sending the message, or `nil` when the
/// message was received.
/// @param destination Destination Address.
/// @param userInitiated Whether sending this message has been initiated by the user.
/// @returns return `nil` when initialize SigAccessPdu object fail.
- (instancetype)initFromMeshMessage:(SigMeshMessage *)message sentFromLocalElement:(SigElementModel *)localElement toDestination:(SigMeshAddress *)destination userInitiated:(BOOL)userInitiated {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
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

        NSMutableData *mData = [NSMutableData dataWithData:[SigHelper.share getOpCodeDataWithUInt32Opcode:_opCode]];
        [mData appendData:_parameters];
        _accessPdu = mData;
    }
    return self;
}

- (NSString *)description {
    return [NSString stringWithFormat:@"Access PDU, source:(0x%04X)->destination: (0x%04X) Op Code: (0x%X), accessPdu=%@ len=%lu", _source, _destination.address, (unsigned int)_opCode,[LibTools convertDataToHexStr:_accessPdu], (unsigned long)_accessPdu.length];
}

@end
