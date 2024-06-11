/********************************************************************************************************
 * @file     SDKLibCommand+directForwarding.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2022/3/9
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "SDKLibCommand+directForwarding.h"

#pragma mark 3.4.2.2.1 Unicast address range format


/// A range of unicast addresses may be used in a message, for example, to communicate all the unicast addresses
/// used by a node. In this case, the unicast address range format can be used to efficiently pack the range of unicast
/// addresses. In the unicast address range format, the 15 least significant bits of the starting unicast address of the
/// range and the number of addresses in the range are formatted as in Table 3.6.
/// @note   The sum of the RangeStart field value and the RangeLength field value shall not exceed 0x8000.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.54),
/// 3.4.2.2.1 Unicast address range format.
@implementation SigUnicastAddressRangeFormatModel

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = (_lengthPresent ? 1 : 0)  | ((_rangeStart & 0x7FFF) << 1);
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    if (_lengthPresent) {
        UInt8 tem8 = _rangeLength;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    return mData;
}

/**
 * @brief   Initialize SigUnicastAddressRangeFormatModel object.
 * @param   parameters    The hex data of SigUnicastAddressRangeFormatModel object.
 * @return  return `nil` when initialize SigUnicastAddressRangeFormatModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters == nil || parameters.length == 1) {
            return nil;
        }
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _lengthPresent = tem16 & 1;
        _rangeStart = (tem16 >> 1) & 0x7FFF;
        if (parameters.length > 2 && _lengthPresent) {
            UInt8 tem8 = 0;
            memcpy(&tem8, dataByte+2, 1);
            _rangeLength = tem8;
        }
    }
    return self;
}

/**
 * @brief   Initialize SigUnicastAddressRangeFormatModel object.
 * @param   unicastAddressRangeFormat    A range of unicast addresses may be used in a message, for example,
 * to communicate all the unicast addresses used by a node. In this case, the unicast address range format can be used to
 * efficiently pack the range of unicast addresses. In the unicast address range format, the 15 least significant bits of the
 * starting unicast address of the range and the number of addresses in the range are formatted as in Table 3.6.
 * @return  return `nil` when initialize SigUnicastAddressRangeFormatModel object fail.
 */
- (instancetype)initWithOldSigUnicastAddressRangeFormatModel:(SigUnicastAddressRangeFormatModel *)unicastAddressRangeFormat {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _lengthPresent = unicastAddressRangeFormat.lengthPresent;
        _rangeStart = unicastAddressRangeFormat.rangeStart;
        if (unicastAddressRangeFormat.lengthPresent) {
            _rangeLength = unicastAddressRangeFormat.rangeLength;
        }
    }
    return self;
}

/**
 * @brief   Initialize SigUnicastAddressRangeFormatModel object.
 * @param   lengthPresent    Indicates the presence or absence of the RangeLength field.
 * The LengthPresent field indicates whether the address range contains a single address or multiple addresses. The LengthPresent
 * field value shall be set to 1 when the number of addresses in the range of unicast addresses is greater than 1; otherwise, the
 * LengthPresent field value shall be set to 0.
 * @param   rangeStart    15 least significant bits of the starting unicast address.
 * The RangeStart field shall contain the 15 least significant bits of the starting unicast address of the range of unicast addresses.
 * The RangeStart field value shall not be 0.
 * @param   rangeLength    Number of addresses in the range (0x02 – 0xFF) (C.1)(C.1: If the LengthPresent field value is 1, the
 * RangeLength field shall be present; otherwise, the RangeLength field shall not be present.)
 * If present, the RangeLength field shall contain the number of addresses in the range of unicast addresses. The valid values for the
 * RangeLength field are 0x02 to 0xFF. The values of 0x00 and 0x01 are prohibited.
 * @return  return `nil` when initialize SigUnicastAddressRangeFormatModel object fail.
 */
- (instancetype)initWithLengthPresent:(BOOL)lengthPresent rangeStart:(UInt16)rangeStart rangeLength:(UInt8)rangeLength {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _lengthPresent = lengthPresent;
        _rangeStart = rangeStart;
        if (lengthPresent) {
            _rangeLength = rangeLength;
        }
    }
    return self;
}

/// Get description string.
- (NSString *)getDescription {
    NSString *tem = [NSString stringWithFormat:@"lengthPresent:%@\nrangeStart:0x%X", _lengthPresent ? @"True" : @"False", _rangeStart];
    if (_lengthPresent) {
        tem = [NSString stringWithFormat:@"%@\nrangeLength:0x%X", tem, _lengthPresent];
    }
    return tem;
}

@end


/// Table 4.185: Fixed path entry format
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.347)
@implementation SigFixedPathEntryFormatModel

/**
 * @brief   Initialize SigFixedPathEntryFormatModel object.
 * @param   parameters    the hex data of SigFixedPathEntryFormatModel.
 * @return  return `nil` when initialize SigFixedPathEntryFormatModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.

    }
    return self;
}

/// Get dictionary from SigFixedPathEntryFormatModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigFixedPathEntryFormatModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"forwardingTableEntryHeader"] = @(_forwardingTableEntryHeader.value);
    if (_pathOriginUnicastAddrRange && _pathOriginUnicastAddrRange.parameters) {
        dict[@"pathOriginUnicastAddrRange"] = _pathOriginUnicastAddrRange.parameters;
    }
    dict[@"dependentOriginListSize"] = @(_dependentOriginListSize);
    dict[@"bearerTowardPathOrigin"] = @(_bearerTowardPathOrigin);
    if (_pathTargetUnicastAddrRange && _pathTargetUnicastAddrRange.parameters) {
        dict[@"pathTargetUnicastAddrRange"] = _pathTargetUnicastAddrRange.parameters;
    }
    dict[@"multicastDestination"] = @(_multicastDestination);
    dict[@"dependentTargetListSize"] = @(_dependentTargetListSize);
    dict[@"bearerTowardPathTarget"] = @(_bearerTowardPathTarget);
    return dict;
}

/// Set dictionary to SigFixedPathEntryFormatModel object.
/// @param dictionary SigFixedPathEntryFormatModel dictionary object.
- (void)setDictionaryToSigFixedPathEntryFormatModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"forwardingTableEntryHeader"]) {
        struct ForwardingTableEntryHeader tem = {};
        tem.value = [dictionary[@"forwardingTableEntryHeader"] intValue];
        _forwardingTableEntryHeader = tem;
    }
    if ([allKeys containsObject:@"pathOriginUnicastAddrRange"]) {
        SigUnicastAddressRangeFormatModel *model = [[SigUnicastAddressRangeFormatModel alloc] initWithParameters:dictionary[@"pathOriginUnicastAddrRange"]];
        _pathOriginUnicastAddrRange = model;
    }
    if ([allKeys containsObject:@"dependentOriginListSize"]) {
        _dependentOriginListSize = [dictionary[@"dependentOriginListSize"] intValue];
    }
    if ([allKeys containsObject:@"bearerTowardPathOrigin"]) {
        _bearerTowardPathOrigin = [dictionary[@"bearerTowardPathOrigin"] intValue];
    }
    if ([allKeys containsObject:@"pathTargetUnicastAddrRange"]) {
        SigUnicastAddressRangeFormatModel *model = [[SigUnicastAddressRangeFormatModel alloc] initWithParameters:dictionary[@"pathTargetUnicastAddrRange"]];
        _pathTargetUnicastAddrRange = model;
    }
    if ([allKeys containsObject:@"multicastDestination"]) {
        _multicastDestination = [dictionary[@"multicastDestination"] intValue];
    }
    if ([allKeys containsObject:@"dependentTargetListSize"]) {
        _dependentTargetListSize = [dictionary[@"dependentTargetListSize"] intValue];
    }
    if ([allKeys containsObject:@"bearerTowardPathTarget"]) {
        _bearerTowardPathTarget = [dictionary[@"bearerTowardPathTarget"] intValue];
    }
}

@end


/// Table 4.186: Non-fixed path entry format
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.348)
@implementation SigNonFixedPathEntryFormatModel

- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.

    }
    return self;
}

@end


/// 4.2.29.2 Forwarding Table Entries
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.268)
@implementation SigForwardingTableEntryModel
@end


/// 4.2.29 Forwarding Table
/// - seeAlso: MshPRFd1.1r14_clean.pdf  (page.268)
@implementation SigForwardingTableModel

/// 计算属性，转发表的起始地址
/// Computed attribute, source address of forwarding table
- (UInt16)tableSource {
    UInt16 tem = 0;
    if (_pathOriginUnicastAddrRange) {
        tem = _pathOriginUnicastAddrRange.rangeStart;
    }
    return tem;
}

/// 计算属性，转发表的目的地址
/// Computed attribute, destination address of forwarding table
- (UInt16)tableDestination {
    UInt16 tem = 0;
    if (_unicastDestinationFlag) {
        if (_pathTargetUnicastAddrRange) {
            tem = _pathTargetUnicastAddrRange.rangeStart;
        }
    } else {
        tem = _multicastDestination;
    }
    return tem;
}

/**
 * @brief   Initialize SigForwardingTableModel object.
 * @param   netKeyIndex    NetKey Index of the NetKey used in the subnet.
 * @param   unicastDestinationFlag    Indicates whether or not the destination of the path is a unicast address.
 * @param   backwardPathValidatedFlag    Indicates whether or not the backward path has been validated.
 * @param   pathOriginUnicastAddrRange    Unicast address range of the Path Origin.
 * @param   pathTargetUnicastAddrRange    Unicast address range of the Path Target (C.1)(C.1: If the value of the Unicast_Destination_Flag field is 1,
 * then the Path_Target_Unicast_Addr_Range field shall be present; otherwise,
 * the Path_Target_Unicast_Addr_Range field shall not be present.)
 * @param   multicastDestination    Multicast destination address (C.2)(C.2: If the value of the Unicast_Destination_Flag field is 0,
 * then the Multicast_Destination field shall be present; otherwise,
 * the Multicast_Destination field shall not be present.)
 * @param   bearerTowardPathOrigin    Index of the bearer toward the Path Origin.
 * @param   bearerTowardPathTarget    Index of the bearer toward the Path Target.
 * @return  return `nil` when initialize SigForwardingTableModel object fail.
 */
- (instancetype)initWithNetKeyIndex:(UInt16)netKeyIndex unicastDestinationFlag:(BOOL)unicastDestinationFlag backwardPathValidatedFlag:(BOOL)backwardPathValidatedFlag pathOriginUnicastAddrRange:(SigUnicastAddressRangeFormatModel * __nullable)pathOriginUnicastAddrRange pathTargetUnicastAddrRange:(SigUnicastAddressRangeFormatModel * __nullable)pathTargetUnicastAddrRange multicastDestination:(UInt16)multicastDestination bearerTowardPathOrigin:(UInt16)bearerTowardPathOrigin bearerTowardPathTarget:(UInt16)bearerTowardPathTarget {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _netKeyIndex = netKeyIndex;
        _unicastDestinationFlag = unicastDestinationFlag;
        _backwardPathValidatedFlag = backwardPathValidatedFlag;
        _pathOriginUnicastAddrRange = pathOriginUnicastAddrRange;
        _pathTargetUnicastAddrRange = pathTargetUnicastAddrRange;
        _multicastDestination = multicastDestination;
        _bearerTowardPathOrigin = bearerTowardPathOrigin;
        _bearerTowardPathTarget = bearerTowardPathTarget;
        _entryNodeAddress = [NSMutableArray array];
    }
    return self;
}

/**
 * @brief   Initialize SigForwardingTableModel object.
 * @param   parameters    the hex data of SigForwardingTableModel.
 * @return  return `nil` when initialize SigForwardingTableModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters == nil || parameters.length < 10) {
            return nil;
        }
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _netKeyIndex = tem16;
        _unicastDestinationFlag = (tem16 >> 1) & 1;
        _backwardPathValidatedFlag = tem16 & 1;
        UInt8 index = 2;
        SigUnicastAddressRangeFormatModel *model1 = [[SigUnicastAddressRangeFormatModel alloc] initWithParameters:[parameters subdataWithRange:NSMakeRange(index, parameters.length - index)]];
        _pathOriginUnicastAddrRange = model1;
        index += model1.parameters.length;
        if (_unicastDestinationFlag == YES) {
            SigUnicastAddressRangeFormatModel *model2 = [[SigUnicastAddressRangeFormatModel alloc] initWithParameters:[parameters subdataWithRange:NSMakeRange(index, parameters.length - index)]];
            _pathTargetUnicastAddrRange = model2;
            index += model2.parameters.length;
        } else {
            memcpy(&tem16, dataByte + index, 2);
            _multicastDestination = tem16;
            index += 2;
        }
        memcpy(&tem16, dataByte + index, 2);
        _bearerTowardPathOrigin = tem16;
        index += 2;
        memcpy(&tem16, dataByte + index, 2);
        _bearerTowardPathTarget = tem16;
        _entryNodeAddress = [NSMutableArray array];
    }
    return self;
}

/**
 * @brief   Initialize SigForwardingTableModel object.
 * @param   forwardingTable    the old SigForwardingTableModel object.
 * @return  return `nil` when initialize SigForwardingTableModel object fail.
 */
- (instancetype)initWithOldSigForwardingTableModel:(SigForwardingTableModel *)forwardingTable {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _netKeyIndex = forwardingTable.netKeyIndex;
        _unicastDestinationFlag = forwardingTable.unicastDestinationFlag;
        _backwardPathValidatedFlag = forwardingTable.backwardPathValidatedFlag;
        _pathOriginUnicastAddrRange = [[SigUnicastAddressRangeFormatModel alloc] initWithOldSigUnicastAddressRangeFormatModel:forwardingTable.pathOriginUnicastAddrRange];
        _pathTargetUnicastAddrRange = [[SigUnicastAddressRangeFormatModel alloc] initWithOldSigUnicastAddressRangeFormatModel:forwardingTable.pathTargetUnicastAddrRange];
        _multicastDestination = forwardingTable.multicastDestination;
        _bearerTowardPathOrigin = forwardingTable.bearerTowardPathOrigin;
        _bearerTowardPathTarget = forwardingTable.bearerTowardPathTarget;
        _entryNodeAddress = [NSMutableArray arrayWithArray:forwardingTable.entryNodeAddress];
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem = _netKeyIndex;
    tem = tem | ((_unicastDestinationFlag ? 1 : 0) << 14);
    tem = tem | ((_backwardPathValidatedFlag ? 1 : 0) << 15);
    NSData *data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    [mData appendData:_pathOriginUnicastAddrRange.parameters];
    if (_unicastDestinationFlag) {
        [mData appendData:_pathTargetUnicastAddrRange.parameters];
    } else {
        tem = _multicastDestination;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    }
    tem = _bearerTowardPathOrigin;
    data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    tem = _bearerTowardPathTarget;
    data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    return mData;
}

/// get dictionary from SigForwardingTableModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigForwardingTableModel {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"netKeyIndex"] = @(_netKeyIndex);
    dict[@"unicastDestinationFlag"] = [NSNumber numberWithBool:_unicastDestinationFlag];
    dict[@"backwardPathValidatedFlag"] = [NSNumber numberWithBool:_backwardPathValidatedFlag];
    if (_pathOriginUnicastAddrRange && _pathOriginUnicastAddrRange.parameters) {
        dict[@"pathOriginUnicastAddrRange"] = [LibTools convertDataToHexStr:_pathOriginUnicastAddrRange.parameters];
    }
    if (_pathTargetUnicastAddrRange && _pathTargetUnicastAddrRange.parameters) {
        dict[@"pathTargetUnicastAddrRange"] = [LibTools convertDataToHexStr:_pathTargetUnicastAddrRange.parameters];
    }
    dict[@"multicastDestination"] = @(_multicastDestination);
    dict[@"bearerTowardPathOrigin"] = @(_bearerTowardPathOrigin);
    dict[@"bearerTowardPathTarget"] = @(_bearerTowardPathTarget);
    dict[@"entryNodeAddress"] = _entryNodeAddress;
    return dict;
}

/// Set dictionary to SigForwardingTableModel object.
/// @param dictionary SigForwardingTableModel dictionary object.
- (void)setDictionaryToSigForwardingTableModel:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"netKeyIndex"]) {
        _netKeyIndex = [dictionary[@"netKeyIndex"] intValue];
    }
    if ([allKeys containsObject:@"unicastDestinationFlag"]) {
        _unicastDestinationFlag = [dictionary[@"unicastDestinationFlag"] boolValue];
    }
    if ([allKeys containsObject:@"backwardPathValidatedFlag"]) {
        _backwardPathValidatedFlag = [dictionary[@"backwardPathValidatedFlag"] boolValue];
    }
    if ([allKeys containsObject:@"pathOriginUnicastAddrRange"]) {
        SigUnicastAddressRangeFormatModel *model = [[SigUnicastAddressRangeFormatModel alloc] initWithParameters:[LibTools nsstringToHex:dictionary[@"pathOriginUnicastAddrRange"]]];
        _pathOriginUnicastAddrRange = model;
    }
    if ([allKeys containsObject:@"pathTargetUnicastAddrRange"]) {
        SigUnicastAddressRangeFormatModel *model = [[SigUnicastAddressRangeFormatModel alloc] initWithParameters:[LibTools nsstringToHex:dictionary[@"pathTargetUnicastAddrRange"]]];
        _pathTargetUnicastAddrRange = model;
    }
    if ([allKeys containsObject:@"multicastDestination"]) {
        _multicastDestination = [dictionary[@"multicastDestination"] intValue];
    }
    if ([allKeys containsObject:@"bearerTowardPathOrigin"]) {
        _bearerTowardPathOrigin = [dictionary[@"bearerTowardPathOrigin"] intValue];
    }
    if ([allKeys containsObject:@"bearerTowardPathTarget"]) {
        _bearerTowardPathTarget = [dictionary[@"bearerTowardPathTarget"] intValue];
    }
    if ([allKeys containsObject:@"entryNodeAddress"]) {
        _entryNodeAddress = dictionary[@"entryNodeAddress"];
    }
}

/// Determine if the data of two SigForwardingTableModel is the same
- (BOOL)isEqual:(id)object {
    if ([object isKindOfClass:[SigForwardingTableModel class]]) {
        //tableSource and tableDestination are the unique identifier of SigForwardingTableModel.
        SigForwardingTableModel *table = (SigForwardingTableModel *)object;
        return self.tableSource == table.tableSource && self.tableDestination == table.tableDestination;
    } else {
        //Two SigForwardingTableModel object is different.
        return NO;
    }
}

@end


#pragma mark - 4.3.5 Directed Forwarding Configuration messages


#pragma mark 4.3.5.3 DIRECTED_CONTROL_GET, opcode:0xBF30


/// A DIRECTED_CONTROL_GET message is an acknowledged message used to get
/// the current Directed Control state of a node (see Section 4.2.26).
/// @note   The response to a DIRECTED_CONTROL_GET message is a
/// DIRECTED_CONTROL_STATUS message.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.353),
/// 4.3.5.3 DIRECTED_CONTROL_GET.
@implementation SigDirectControlGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_DirectControlGet;
    }
    return self;
}

/**
 * @brief   Initialize SigDirectControlGet object.
 * @param   netKeyIndex    NetKey Index of the NetKey used in the subnet.
 * The NetKeyIndex field is the global NetKey Index of the NetKey of the subnet that is
 * associated with the Directed Control state and is encoded as defined in Section 4.3.1.1.
 * @return  return `nil` when initialize SigDirectControlGet object fail.
 */
- (instancetype)initWithNetKeyIndex:(UInt16)netKeyIndex {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_DirectControlGet;
        _netKeyIndex = netKeyIndex;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData dataWithData:[self encodeIndexes:@[@(_netKeyIndex)]]];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigDirectControlStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.5.4 DIRECTED_CONTROL_SET, opcode:0xBF31


/// A DIRECTED_CONTROL_SET message is an acknowledged message used to set
/// the Directed Control state of a subnet (see Section 4.2.26).
/// @note   The response to a DIRECTED_CONTROL_SET message is a
/// DIRECTED_CONTROL_STATUS message.
/// @note   If the Directed_Forwarding field is set to Disable, then the Enable value is
/// Prohibited for any of the Directed_Relay, Directed_Proxy, and Directed_Friend fields.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.353),
/// 4.3.5.4 DIRECTED_CONTROL_SET.
@implementation SigDirectControlSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_DirectControlSet;
    }
    return self;
}

/**
 * @brief   Initialize SigDirectControlSet object.
 * @param   netKeyIndex    NetKey Index of the NetKey used in the subnet.
 * The NetKeyIndex field is the global NetKey Index of the NetKey of the subnet that is
 * associated with the Directed Control state and is encoded as defined in Section 4.3.1.1.
 * @param   directedForwarding    New Directed Forwarding state.
 * The Directed_Forwarding field determines the new Directed Forwarding state for the
 * identified subnet, as defined in Section 4.2.26.1.
 * @param   directedRelay    New Directed Relay state.
 * The Directed_Relay field determines the new Directed Relay state for the identified subnet,
 * as defined in Section 4.2.26.2.
 * @param   directedProxy    New Directed Proxy state or value to ignore.
 * If the value of the Directed_Proxy field is Disable or Enable, then the Directed_Proxy field
 * determines the new Directed Proxy state for the identified subnet, as defined in Section 4.2.26.3.
 * @param   directedProxyUseDirectedDefault    New Directed Proxy Use Directed
 * Default state or value to ignore.
 * If the value of the Directed_Proxy_Use_Directed_Default field is Disable or Enable,
 * then the Directed_Proxy_Use_Directed_Default field determines the new Directed Proxy
 * Use Directed Default state for the identified subnet, as defined in Section 4.2.26.4.
 * @param   directedFriend    New Directed Friend state or value to ignore.
 * If the value of the Directed_Proxy field is either Do Not Process or Disabled, then the value
 * of the Directed_Proxy_Use_Directed_Default field shall be set to Do Not Process. If the value
 * of the Directed_Proxy field is Enable, then the value of the Directed_Proxy_Use_Directed_Default
 * field shall be set to either Disable or Enable.If the value of the Directed_Friend field is Disable or
 * Enable, then the Directed_Friend field determines the new Directed Friend state for the identified
 * subnet, as defined in Section 4.2.26.5.
 * @return  return `nil` when initialize SigDirectControlSet object fail.
 */
- (instancetype)initWithNetKeyIndex:(UInt16)netKeyIndex directedForwarding:(DirectedForwarding)directedForwarding directedRelay:(DirectedRelay)directedRelay directedProxy:(DirectedProxy)directedProxy directedProxyUseDirectedDefault:(DirectedProxyUseDirectedDefault)directedProxyUseDirectedDefault directedFriend:(DirectedFriend)directedFriend {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_DirectControlSet;
        _netKeyIndex = netKeyIndex;
        _directedForwarding = directedForwarding;
        _directedRelay = directedRelay;
        _directedProxy = directedProxy;
        _directedProxyUseDirectedDefault = directedProxyUseDirectedDefault;
        _directedFriend = directedFriend;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    NSData *data = [self encodeIndexes:@[@(_netKeyIndex)]];
    [mData appendData:data];
    UInt8 tem8 = 0;
    tem8 = _directedForwarding;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _directedRelay;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _directedProxy;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _directedProxyUseDirectedDefault;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _directedFriend;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigDirectControlStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.5.5 DIRECTED_CONTROL_STATUS, opcode:0xBF32


/// A DIRECTED_CONTROL_STATUS message is an unacknowledged message
/// used to report the current Directed Control state of a subnet (see Section 4.2.26).
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.353),
/// 4.3.5.4 DIRECTED_CONTROL_STATUS.
@implementation SigDirectControlStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_DirectControlStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigDirectControlStatus object.
 * @param   parameters    the hex data of SigDirectControlStatus.
 * @return  return `nil` when initialize SigDirectControlStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_DirectControlStatus;
        if (parameters == nil || parameters.length != 8) {
            return nil;
        }
        UInt8 tem8 = 0;
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _status = tem8;
        memcpy(&tem16, dataByte+1, 2);
        _netKeyIndex = tem16;
        memcpy(&tem8, dataByte+3, 1);
        _directedForwardingState = tem8;
        memcpy(&tem8, dataByte+4, 1);
        _directedRelayState = tem8;
        memcpy(&tem8, dataByte+5, 1);
        _directedProxyState = tem8;
        memcpy(&tem8, dataByte+6, 1);
        _directedProxyUseDirectedDefaultState = tem8;
        memcpy(&tem8, dataByte+7, 1);
        _directedFriendState = tem8;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _status;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    data = [self encodeIndexes:@[@(_netKeyIndex)]];
    [mData appendData:data];
    tem8 = _directedForwardingState;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _directedRelayState;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _directedProxyState;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _directedProxyUseDirectedDefaultState;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _directedFriendState;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

/// get dictionary from SigDirectControlStatus object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigDirectControlStatus {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    dict[@"status"] = @(_status);
    dict[@"netKeyIndex"] = @(_netKeyIndex);
    dict[@"directedForwardingState"] = @(_directedForwardingState);
    dict[@"directedRelayState"] = @(_directedRelayState);
    dict[@"directedProxyState"] = @(_directedProxyState);
    dict[@"directedProxyUseDirectedDefaultState"] = @(_directedProxyUseDirectedDefaultState);
    dict[@"directedFriendState"] = @(_directedFriendState);
    return dict;
}

/// Set dictionary to SigDirectControlStatus object.
/// @param dictionary SigDirectControlStatus dictionary object.
- (void)setDictionaryToSigDirectControlStatus:(NSDictionary *)dictionary {
    if (dictionary == nil || dictionary.allKeys.count == 0) {
        return;
    }
    NSArray *allKeys = dictionary.allKeys;
    if ([allKeys containsObject:@"status"]) {
        _status = [dictionary[@"status"] intValue];
    }
    if ([allKeys containsObject:@"netKeyIndex"]) {
        _netKeyIndex = [dictionary[@"netKeyIndex"] intValue];
    }
    if ([allKeys containsObject:@"directedForwardingState"]) {
        _directedForwardingState = [dictionary[@"directedForwardingState"] intValue];
    }
    if ([allKeys containsObject:@"directedRelayState"]) {
        _directedRelayState = [dictionary[@"directedRelayState"] intValue];
    }
    if ([allKeys containsObject:@"directedProxyState"]) {
        _directedProxyState = [dictionary[@"directedProxyState"] intValue];
    }
    if ([allKeys containsObject:@"directedProxyUseDirectedDefaultState"]) {
        _directedProxyUseDirectedDefaultState = [dictionary[@"directedProxyUseDirectedDefaultState"] intValue];
    }
    if ([allKeys containsObject:@"directedFriendState"]) {
        _directedFriendState = [dictionary[@"directedFriendState"] intValue];
    }
}

@end


#pragma mark 4.3.5.12 FORWARDING_TABLE_ADD, opcode:0xBF39


/// A FORWARDING_TABLE_ADD message is an acknowledged message used to add
/// a fixed path entry to the Forwarding Table state of a node or to update an existing
/// fixed path entry (see Section 4.2.29).
/// @note   The response to a FORWARDING_TABLE_ADD message is a
/// FORWARDING_TABLE_STATUS message.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.360),
/// 4.3.5.12 FORWARDING_TABLE_ADD.
@implementation SigForwardingTableAdd

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_ForwardingTableAdd;
    }
    return self;
}

/**
 * @brief   Initialize SigForwardingTableAdd object.
 * @param   forwardingTableModel    Forwarding Table Model object.
 * @return  return `nil` when initialize SigForwardingTableAdd object fail.
 */
- (instancetype)initWithForwardingTableModel:(SigForwardingTableModel *)forwardingTableModel {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_ForwardingTableAdd;
        _forwardingTableModel = forwardingTableModel;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return _forwardingTableModel.parameters;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigForwardingTableStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.5.13 FORWARDING_TABLE_DELETE, opcode:0xBF3A


/// A FORWARDING_TABLE_DELETE message is an acknowledged message used to delete
/// all the path entries from a specific Path Origin to a specific destination from the Forwarding
/// Table state of a node (see Section 4.2.29).
/// @note   The response to a FORWARDING_TABLE_DELETE message is a
/// FORWARDING_TABLE_STATUS message.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.362),
/// 4.3.5.13 FORWARDING_TABLE_DELETE.
@implementation SigForwardingTableDelete

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_ForwardingTableDelete;
    }
    return self;
}

/**
 * @brief   Initialize SigForwardingTableDelete object.
 * @param   netKeyIndex    NetKey Index of the NetKey used in the subnet.
 * The NetKeyIndex field is the global NetKey Index of the NetKey of the subnet that is associated
 * with the Forwarding Table state and is encoded as defined in Section 4.3.1.1.
 * @param   pathOrigin    Primary element address of the Path Origin.
 * The Path_Origin and Destination fields identify respectively the Path Origin and the destination of
 * the path entries to be deleted from the Forwarding Table state. The unassigned address, group
 * addresses, and virtual addresses are Prohibited for the Path_Origin field. The unassigned address
 * and the all- directed-forwarding-nodes, all-nodes, and all-relays fixed group addresses are Prohibited
 * for the Destination field. The Path_Origin and Destination fields shall not have the same value.
 * @param   pathDestination    Destination address.
 * The Path_Origin and Destination fields shall not have the same value.
 * @return  return `nil` when initialize SigForwardingTableDelete object fail.
 */
- (instancetype)initWithNetKeyIndex:(UInt16)netKeyIndex pathOrigin:(UInt16)pathOrigin pathDestination:(UInt16)pathDestination {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_ForwardingTableDelete;
        _netKeyIndex = netKeyIndex;
        _pathOrigin = pathOrigin;
        _pathDestination = pathDestination;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    NSData *data = [self encodeIndexes:@[@(_netKeyIndex)]];
    [mData appendData:data];
    UInt16 tem16 = _pathOrigin;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _pathDestination;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigForwardingTableStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.5.14 FORWARDING_TABLE_STATUS, opcode:0xBF3B


/// A FORWARDING_TABLE_STATUS message is an unacknowledged message used to report the status
/// of the most recent operation performed on the Forwarding Table state of a node (see Section 4.2.29).
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.362),
/// 4.3.5.14 FORWARDING_TABLE_STATUS.
@implementation SigForwardingTableStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_ForwardingTableStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigForwardingTableStatus object.
 * @param   parameters    the hex data of SigForwardingTableStatus.
 * @return  return `nil` when initialize SigForwardingTableStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_ForwardingTableStatus;
        if (parameters == nil || parameters.length != 7) {
            return nil;
        }
        UInt8 tem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _status = tem8;
        UInt16 tem16 = 0;
        memcpy(&tem16, dataByte+1, 2);
        _netKeyIndex = tem16 >> 4;
        memcpy(&tem16, dataByte+3, 2);
        _pathOrigin = tem16;
        memcpy(&tem16, dataByte+3, 2);
        _pathDestination = tem16;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _status;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    data = [self encodeIndexes:@[@(_netKeyIndex)]];
    [mData appendData:data];
    UInt16 tem16 = _pathOrigin;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _pathDestination;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end



@implementation SDKLibCommand (directForwarding)


#pragma mark - 4.3.5 Directed Forwarding Configuration messages


#pragma mark 4.3.5.3 DIRECTED_CONTROL_GET, opcode:0xBF30


/**
 * @brief   DIRECTED_CONTROL_GET.
 * @param   netKeyIndex    NetKey Index of the NetKey used in the subnet.
 * The NetKeyIndex field is the global NetKey Index of the NetKey of the subnet that is
 * associated with the Directed Control state and is encoded as defined in Section 4.3.1.1.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.353),
 * 6.6.6 DIRECTED_CONTROL_GET.
 */
+ (SigMessageHandle *)directControlGetWithNetKeyIndex:(UInt16)netKeyIndex destination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseDirectControlStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigDirectControlGet *config = [[SigDirectControlGet alloc] initWithNetKeyIndex:netKeyIndex];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}


#pragma mark 4.3.5.4 DIRECTED_CONTROL_SET, opcode:0xBF31


/**
 * @brief   DIRECTED_CONTROL_GET.
 * @param   netKeyIndex    NetKey Index of the NetKey used in the subnet.
 * The NetKeyIndex field is the global NetKey Index of the NetKey of the subnet that is
 * associated with the Directed Control state and is encoded as defined in Section 4.3.1.1.
 * @param   directedForwarding    New Directed Forwarding state.
 * The Directed_Forwarding field determines the new Directed Forwarding state for the
 * identified subnet, as defined in Section 4.2.26.1.
 * @param   directedRelay    New Directed Relay state.
 * The Directed_Relay field determines the new Directed Relay state for the identified subnet,
 * as defined in Section 4.2.26.2.
 * @param   directedProxy    New Directed Proxy state or value to ignore.
 * If the value of the Directed_Proxy field is Disable or Enable, then the Directed_Proxy field
 * determines the new Directed Proxy state for the identified subnet, as defined in Section 4.2.26.3.
 * @param   directedProxyUseDirectedDefault    New Directed Proxy Use Directed Default state or value to ignore.
 * If the value of the Directed_Proxy_Use_Directed_Default field is Disable or Enable,
 * then the Directed_Proxy_Use_Directed_Default field determines the new Directed Proxy
 * Use Directed Default state for the identified subnet, as defined in Section 4.2.26.4.
 * @param   directedFriend    New Directed Friend state or value to ignore.
 * If the value of the Directed_Proxy field is either Do Not Process or Disabled, then the value
 * of the Directed_Proxy_Use_Directed_Default field shall be set to Do Not Process. If the value
 * of the Directed_Proxy field is Enable, then the value of the Directed_Proxy_Use_Directed_Default
 * field shall be set to either Disable or Enable.If the value of the Directed_Friend field is Disable or
 * Enable, then the Directed_Friend field determines the new Directed Friend state for the identified
 * subnet, as defined in Section 4.2.26.5.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.353),
 * 4.3.5.4 DIRECTED_CONTROL_SET.
 */
+ (SigMessageHandle *)directControlSetWithNetKeyIndex:(UInt16)netKeyIndex directedForwarding:(DirectedForwarding)directedForwarding directedRelay:(DirectedRelay)directedRelay directedProxy:(DirectedProxy)directedProxy directedProxyUseDirectedDefault:(DirectedProxyUseDirectedDefault)directedProxyUseDirectedDefault directedFriend:(DirectedFriend)directedFriend destination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseDirectControlStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigDirectControlSet *config = [[SigDirectControlSet alloc] initWithNetKeyIndex:netKeyIndex directedForwarding:directedForwarding directedRelay:directedRelay directedProxy:directedProxy directedProxyUseDirectedDefault:directedProxyUseDirectedDefault directedFriend:directedFriend];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}


#pragma mark 4.3.5.12 FORWARDING_TABLE_ADD, opcode:0xBF39


/**
 * @brief   FORWARDING_TABLE_ADD.
 * @param   netKeyIndex    NetKey Index of the NetKey used in the subnet.
 * The NetKeyIndex field is the global NetKey Index of the NetKey of the subnet that is
 * associated with the Directed Control state and is encoded as defined in Section 4.3.1.1.
 * @param   unicastDestinationFlag    Indicates whether or not the destination of the path is a unicast address.
 * @param   backwardPathValidatedFlag    Indicates whether or not the backward path has been validated.
 * @param   pathOriginUnicastAddrRange    Unicast address range of the Path Origin.
 * @param   pathTargetUnicastAddrRange     Unicast address range of the Path Target (C.1)(C.1: If the value of the Unicast_Destination_Flag field is 1,
 * then the Path_Target_Unicast_Addr_Range field shall be present; otherwise,
 * the Path_Target_Unicast_Addr_Range field shall not be present.)
 * @param   multicastDestination    Multicast destination address (C.2)(C.2: If the value of the Unicast_Destination_Flag field is 0,
 * then the Multicast_Destination field shall be present; otherwise,
 * the Multicast_Destination field shall not be present.)
 * @param   bearerTowardPathOrigin    Index of the bearer toward the Path Origin.
 * @param   bearerTowardPathTarget    Index of the bearer toward the Path Target.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.360),
 * 4.3.5.12 FORWARDING_TABLE_ADD.
 */
+ (SigMessageHandle *)forwardingTableAddWithNetKeyIndex:(UInt16)netKeyIndex unicastDestinationFlag:(BOOL)unicastDestinationFlag backwardPathValidatedFlag:(BOOL)backwardPathValidatedFlag pathOriginUnicastAddrRange:(SigUnicastAddressRangeFormatModel *)pathOriginUnicastAddrRange pathTargetUnicastAddrRange:(SigUnicastAddressRangeFormatModel *)pathTargetUnicastAddrRange multicastDestination:(UInt16)multicastDestination bearerTowardPathOrigin:(UInt16)bearerTowardPathOrigin bearerTowardPathTarget:(UInt16)bearerTowardPathTarget destination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseForwardingTableStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigForwardingTableModel *model = [[SigForwardingTableModel alloc] initWithNetKeyIndex:netKeyIndex unicastDestinationFlag:unicastDestinationFlag backwardPathValidatedFlag:backwardPathValidatedFlag pathOriginUnicastAddrRange:pathOriginUnicastAddrRange pathTargetUnicastAddrRange:pathTargetUnicastAddrRange multicastDestination:multicastDestination bearerTowardPathOrigin:bearerTowardPathOrigin bearerTowardPathTarget:bearerTowardPathTarget];
    SigForwardingTableAdd *config = [[SigForwardingTableAdd alloc] initWithForwardingTableModel:model];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}


#pragma mark 4.3.5.13 FORWARDING_TABLE_DELETE, opcode:0xBF3A


/**
 * @brief   FORWARDING_TABLE_DELETE.
 * @param   netKeyIndex    NetKey Index of the NetKey used in the subnet.
 * The NetKeyIndex field is the global NetKey Index of the NetKey of the subnet that is
 * associated with the Directed Control state and is encoded as defined in Section 4.3.1.1.
 * @param   pathOrigin    Primary element address of the Path Origin.
 * The Path_Origin and Destination fields identify respectively the Path Origin and the destination
 * of the path entries to be deleted from the Forwarding Table state. The unassigned address,
 * group addresses, and virtual addresses are Prohibited for the Path_Origin field. The unassigned
 * address and the all- directed-forwarding-nodes, all-nodes, and all-relays fixed group addresses
 * are Prohibited for the Destination field. The Path_Origin and Destination fields shall not have the
 * same value.
 * @param   pathDestination    Destination address.
 * The Path_Origin and Destination fields shall not have the same value.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.353),
 * 4.3.5.13 FORWARDING_TABLE_DELETE.
 */
+ (SigMessageHandle *)forwardingTableDeleteWithNetKeyIndex:(UInt16)netKeyIndex pathOrigin:(UInt16)pathOrigin pathDestination:(UInt16)pathDestination destination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseForwardingTableStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigForwardingTableDelete *config = [[SigForwardingTableDelete alloc] initWithNetKeyIndex:netKeyIndex pathOrigin:pathOrigin pathDestination:pathDestination];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

@end
