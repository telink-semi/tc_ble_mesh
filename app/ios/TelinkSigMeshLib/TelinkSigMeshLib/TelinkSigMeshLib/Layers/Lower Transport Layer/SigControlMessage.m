/********************************************************************************************************
 * @file     SigControlMessage.m
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

#import "SigControlMessage.h"
#import "SigSegmentedAccessMessage.h"

@implementation SigControlMessage

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.type = SigLowerTransportPduType_transportControlMessage;
    }
    return self;
}

/// Creates an Control Message from a Network PDU that contains
/// an unsegmented control message. If the PDU is invalid, the
/// init returns `nil`.
///
/// - parameter networkPdu: The received Network PDU with unsegmented
///                         Upper Transport message.
- (instancetype)initFromNetworkPdu:(SigNetworkPdu *)networkPdu {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.type = SigLowerTransportPduType_transportControlMessage;
        NSData *data = networkPdu.transportPdu;
        Byte *dataByte = (Byte *)data.bytes;
        UInt8 tem = 0;
        memcpy(&tem, dataByte, 1);
        if (data.length < 1 || (tem & 0x80) != 0) {
            TeLogError(@"initFromUnsegmentedPdu fail.");
            return nil;
        }
        _opCode = tem & 0x7F;
        self.upperTransportPdu = [data subdataWithRange:NSMakeRange(1, data.length-1)];
        self.source = networkPdu.source;
        self.destination = networkPdu.destination;
        self.networkKey = networkPdu.networkKey;
    }
    return self;
}

/// Creates an Control Message object from the given list of segments.
///
/// - parameter segments: List of ordered segments.
- (instancetype)initFromSegments:(NSArray <SigSegmentedAccessMessage *>*)segments {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.type = SigLowerTransportPduType_transportControlMessage;
        // Assuming all segments have the same AID, source and destination addresses and TransMIC.
        SigSegmentedAccessMessage *segment = segments.firstObject;
        _opCode = segment.opCode;
        self.source = segment.source;
        self.destination = segment.destination;
        self.networkKey = segment.networkKey;
        
        // Segments are already sorted by `segmentOffset`.
        NSMutableData *mData = [NSMutableData data];
        for (SigSegmentedAccessMessage *msg in segments) {
            [mData appendData:msg.upperTransportPdu];
        }
        self.upperTransportPdu = mData;
    }
    return self;
}

/// Creates a Control Message from the given Proxy Configuration
/// message. The source should be set to the local Node address.
/// The given Network Key should be known to the Proxy Node.
///
/// - parameter message:    The message to be sent.
/// - parameter source:     The address of the local Node.
/// - parameter networkKey: The Network Key to signe the message with.
///                         The key should be known to the connected
///                         Proxy Node.
- (instancetype)initFromProxyConfigurationMessage:(SigProxyConfigurationMessage *)message sentFromSource:(UInt16)source usingNetworkKey:(SigNetkeyModel *)networkKey {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.type = SigLowerTransportPduType_transportControlMessage;
        _opCode = message.opCode;
        self.source = source;
        self.destination = MeshAddress_unassignedAddress;
        self.networkKey = networkKey;
        self.upperTransportPdu = message.parameters;
    }
    return self;
}

- (NSData *)transportPdu {
    UInt8 tem = _opCode;
    NSMutableData *mData = [NSMutableData dataWithBytes:&tem length:1];
    [mData appendData:self.upperTransportPdu];
    return mData;
}

@end
