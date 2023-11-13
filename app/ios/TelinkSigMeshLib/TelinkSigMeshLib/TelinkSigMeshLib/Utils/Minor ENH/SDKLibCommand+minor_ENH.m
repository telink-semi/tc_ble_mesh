/********************************************************************************************************
 * @file     SDKLibCommand+minor_ENH.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2022/4/12
 *
 * @par     Copyright (c) [2022], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "SDKLibCommand+minor_ENH.h"
#import "SDKLibCommand+directForwarding.h"

@implementation SigMetadataEntriesModel

/**
 * @brief   Initialize SigMetadataEntriesModel object.
 * @param   parameters    the hex data of SigMetadataEntriesModel.
 * @return  return `nil` when initialize SigMetadataEntriesModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters == nil || parameters.length < 4) {
            return nil;
        }
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _metadataLength = tem16;
        memcpy(&tem16, dataByte+2, 2);
        _metadataID = tem16;
        if (_metadataLength > 0 && parameters.length-4 >= _metadataLength) {
            _metadata = [parameters subdataWithRange:NSMakeRange(4, _metadataLength)];
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _metadataLength;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _metadataID;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    if (_metadata) {
        [mData appendData:_metadata];
    }
    return mData;
}

/// Get description string.
- (NSString *)getDescription {
    return [NSString stringWithFormat:@"MetadataLength:%d\nMetadataID:0x%04X\nMetadata:0x%@", _metadataLength, _metadataID, [LibTools convertDataToHexStr:_metadata]];
}

@end


@implementation SigSIGMetadataItemsModel

/**
 * @brief   Initialize SigSIGMetadataItemsModel object.
 * @param   parameters    the hex data of SigSIGMetadataItemsModel.
 * @return  return `nil` when initialize SigSIGMetadataItemsModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters == nil || parameters.length < 3) {
            return nil;
        }
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _modelID = tem16;
        UInt8 tem8 = 0;
        memcpy(&tem8, dataByte+2, 1);
        _metadataEntriesNumber = tem8;
        NSMutableArray *mArray = [NSMutableArray array];
        NSInteger loc = 3;
        while (mArray.count < _metadataEntriesNumber) {
            if (loc >= parameters.length) {
                break;
            } else {
                SigMetadataEntriesModel *entry = [[SigMetadataEntriesModel alloc] initWithParameters:[parameters subdataWithRange:NSMakeRange(loc, parameters.length-loc)]];
                if (entry) {
                    [mArray addObject:entry];
                    loc += entry.parameters.length;
                } else {
                    break;
                }
            }
        }
        _metadataEntries = mArray;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _modelID;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = _metadataEntriesNumber;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_metadataEntries) {
        NSArray *array = [NSArray arrayWithArray:_metadataEntries];
        for (SigMetadataEntriesModel *entry in array) {
            [mData appendData:entry.parameters];
        }
    }
    return mData;
}

/// Get description string.
- (NSString *)getDescription {
    NSString *tem = [NSString stringWithFormat:@"ModelID:0x%04X\nMetadataEntriesNumber:%d", _modelID, _metadataEntriesNumber];
    if (_metadataEntries) {
        tem = [NSString stringWithFormat:@"%@\nMetadataEntries:[\n", tem];
        NSArray *array = [NSArray arrayWithArray:_metadataEntries];
        for (int i=0; i<array.count; i++) {
            SigMetadataEntriesModel *entry = array[i];
            tem = [NSString stringWithFormat:@"[%d]%@\n]", i, entry.getDescription];
        }
        tem = [NSString stringWithFormat:@"%@\n]", tem];
    }
    return tem;
}

@end


@implementation SigVendorMetadataItemsModel

/**
 * @brief   Initialize SigVendorMetadataItemsModel object.
 * @param   parameters    the hex data of SigVendorMetadataItemsModel.
 * @return  return `nil` when initialize SigVendorMetadataItemsModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters == nil || parameters.length < 5) {
            return nil;
        }
        UInt32 tem32 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem32, dataByte, 4);
        _modelID = tem32;
        UInt8 tem8 = 0;
        memcpy(&tem8, dataByte+4, 1);
        _metadataEntriesNumber = tem8;
        NSMutableArray *mArray = [NSMutableArray array];
        NSInteger loc = 5;
        while (mArray.count < _metadataEntriesNumber) {
            if (loc >= parameters.length) {
                break;
            } else {
                SigMetadataEntriesModel *entry = [[SigMetadataEntriesModel alloc] initWithParameters:[parameters subdataWithRange:NSMakeRange(loc, parameters.length-loc)]];
                if (entry) {
                    [mArray addObject:entry];
                    loc += entry.parameters.length;
                } else {
                    break;
                }
            }
        }
        _metadataEntries = mArray;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt32 tem32 = _modelID;
    NSData *data = [NSData dataWithBytes:&tem32 length:4];
    [mData appendData:data];
    UInt8 tem8 = _metadataEntriesNumber;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_metadataEntries) {
        NSArray *array = [NSArray arrayWithArray:_metadataEntries];
        for (SigMetadataEntriesModel *entry in array) {
            [mData appendData:entry.parameters];
        }
    }
    return mData;
}

/// Get description string.
- (NSString *)getDescription {
    NSString *tem = [NSString stringWithFormat:@"ModelID:0x%08X\nMetadataEntriesNumber:%d", _modelID, _metadataEntriesNumber];
    if (_metadataEntries) {
        tem = [NSString stringWithFormat:@"%@\nMetadataEntries:[\n", tem];
        NSArray *array = [NSArray arrayWithArray:_metadataEntries];
        for (int i=0; i<array.count; i++) {
            SigMetadataEntriesModel *entry = array[i];
            tem = [NSString stringWithFormat:@"[%d]%@\n]", i, entry.getDescription];
        }
        tem = [NSString stringWithFormat:@"%@\n]", tem];
    }
    return tem;
}

@end


@implementation SigMetadataElementsModel

/**
 * @brief   Initialize SigMetadataElementsModel object.
 * @param   parameters    the hex data of SigMetadataElementsModel.
 * @return  return `nil` when initialize SigMetadataElementsModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters == nil || parameters.length < 2) {
            return nil;
        }
        UInt8 tem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _itemsNumberSIGModels = tem8;
        memcpy(&tem8, dataByte+1, 1);
        _itemsNumberVendorModels = tem8;
        NSMutableArray *mArray = [NSMutableArray array];
        NSMutableArray *mArray2 = [NSMutableArray array];
        NSInteger loc = 2;
        while (mArray.count < _itemsNumberSIGModels) {
            if (loc >= parameters.length) {
                break;
            } else {
                SigSIGMetadataItemsModel *item = [[SigSIGMetadataItemsModel alloc] initWithParameters:[parameters subdataWithRange:NSMakeRange(loc, parameters.length-loc)]];
                if (item) {
                    [mArray addObject:item];
                    loc += item.parameters.length;
                } else {
                    break;
                }
            }
        }
        _SIGMetadataItems = mArray;
        while (mArray2.count < _itemsNumberVendorModels) {
            if (loc >= parameters.length) {
                break;
            } else {
                SigVendorMetadataItemsModel *item = [[SigVendorMetadataItemsModel alloc] initWithParameters:[parameters subdataWithRange:NSMakeRange(loc, parameters.length-loc)]];
                if (item) {
                    [mArray2 addObject:item];
                    loc += item.parameters.length;
                } else {
                    break;
                }
            }
        }
        _vendorMetadataItems = mArray2;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _itemsNumberSIGModels;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _itemsNumberVendorModels;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_SIGMetadataItems) {
        NSArray *array = [NSArray arrayWithArray:_SIGMetadataItems];
        for (SigSIGMetadataItemsModel *item in array) {
            [mData appendData:item.parameters];
        }
    }
    if (_vendorMetadataItems) {
        NSArray *array = [NSArray arrayWithArray:_vendorMetadataItems];
        for (SigVendorMetadataItemsModel *item in array) {
            [mData appendData:item.parameters];
        }
    }
    return mData;
}

/// Get description string.
- (NSString *)getDescription {
    NSString *tem = [NSString stringWithFormat:@"ItemsNumberSIGModels:%d\nItemsNumberVendorModels:%d", _itemsNumberSIGModels, _itemsNumberVendorModels];
    if (_SIGMetadataItems) {
        tem = [NSString stringWithFormat:@"%@\nSIGMetadataItems:[\n", tem];
        NSArray *array = [NSArray arrayWithArray:_SIGMetadataItems];
        for (int i=0; i<array.count; i++) {
            SigSIGMetadataItemsModel *item = array[i];
            tem = [NSString stringWithFormat:@"[%d]%@\n]", i, item.getDescription];
        }
        tem = [NSString stringWithFormat:@"%@\n]", tem];
    }
    if (_vendorMetadataItems) {
        tem = [NSString stringWithFormat:@"%@\n_VendorMetadataItems:[\n", tem];
        NSArray *array = [NSArray arrayWithArray:_vendorMetadataItems];
        for (int i=0; i<array.count; i++) {
            SigVendorMetadataItemsModel *item = array[i];
            tem = [NSString stringWithFormat:@"[%d]%@\n]", i, item.getDescription];
        }
        tem = [NSString stringWithFormat:@"%@\n]", tem];
    }
    return tem;
}

@end


@implementation SigModelsMetadataPage0Model

/**
 * @brief   Initialize SigModelsMetadataPage0Model object.
 * @param   parameters    the hex data of SigModelsMetadataPage0Model.
 * @return  return `nil` when initialize SigModelsMetadataPage0Model object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters == nil || parameters.length < 2) {
            return nil;
        }
        NSMutableArray *mArray = [NSMutableArray array];
        NSInteger loc = 0;
        while (loc < parameters.length) {
            SigMetadataElementsModel *element = [[SigMetadataElementsModel alloc] initWithParameters:[parameters subdataWithRange:NSMakeRange(loc, parameters.length-loc)]];
            if (element) {
                [mArray addObject:element];
                loc += element.parameters.length;
            } else {
                break;
            }
        }
        _metadataElements = mArray;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    if (_metadataElements) {
        NSArray *array = [NSArray arrayWithArray:_metadataElements];
        for (SigMetadataElementsModel *element in array) {
            [mData appendData:element.parameters];
        }
    }
    return mData;
}

/// Get description string.
- (NSString *)getDescription {
    NSString *tem = @"Models Metadata Page 0:";
    if (_metadataElements) {
        tem = [NSString stringWithFormat:@"%@\nMetadataElements:[\n", tem];
        NSArray *array = [NSArray arrayWithArray:_metadataElements];
        for (int i=0; i<array.count; i++) {
            SigMetadataElementsModel *element = array[i];
            tem = [NSString stringWithFormat:@"[%d]%@\n]", i, element.getDescription];
        }
        tem = [NSString stringWithFormat:@"%@\n]", tem];
    }
    return tem;
}

@end


#pragma mark - 4.3.6 On-Demand Private GATT Proxy messages


#pragma mark 4.3.6.1 ON_DEMAND_PRIVATE_PROXY_GET, opcode:0xB800


/// An ON_DEMAND_PRIVATE_PROXY_GET message is an acknowledged message
/// used to get the current On-Demand Private GATT Proxy state of a node.
/// @note   The response to an ON_DEMAND_PRIVATE_PROXY_GET message is an
/// ON_DEMAND_PRIVATE_PROXY_GET_STATUS message.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.394), 4.3.6.1 ON_DEMAND_PRIVATE_PROXY_GET.
@implementation SigOnDemandPrivateProxyGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_OnDemandPrivateProxyGet;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigOnDemandPrivateProxyStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.6.2 ON_DEMAND_PRIVATE_PROXY_SET, opcode:0xB801


/// An ON_DEMAND_PRIVATE_PROXY_SET message is an acknowledged message
/// used to set the On- Demand Private GATT Proxy state of a node.
/// @note   The response to an ON_DEMAND_PRIVATE_PROXY_SET message is an
/// ON_DEMAND_PRIVATE_PROXY_STATUS message.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.394), 4.3.6.2 ON_DEMAND_PRIVATE_PROXY_SET.
@implementation SigOnDemandPrivateProxySet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_OnDemandPrivateProxySet;
    }
    return self;
}

/**
 * @brief   Initialize SigOnDemandPrivateProxySet object.
 * @param   onDemandPrivateGATTProxy    New On-Demand Private GATT Proxy state.
 * The On-Demand_Private_GATT_Proxy field contains the new On-Demand Private
 * GATT Proxy state of the node.
 * @return  return `nil` when initialize SigOnDemandPrivateProxySet object fail.
 */
- (instancetype)initWithOnDemandPrivateGATTProxy:(UInt8)onDemandPrivateGATTProxy {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_OnDemandPrivateProxySet;
        _onDemandPrivateGATTProxy = onDemandPrivateGATTProxy;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = 0;
    tem8 = _onDemandPrivateGATTProxy;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigOnDemandPrivateProxyStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.6.3 ON_DEMAND_PRIVATE_PROXY_STATUS, opcode:0xB802


/// An ON_DEMAND_PRIVATE_PROXY_STATUS message is an unacknowledged message
/// used to report the current On-Demand Private GATT Proxy state of a node.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.395), 4.3.6.3 ON_DEMAND_PRIVATE_PROXY_STATUS.
@implementation SigOnDemandPrivateProxyStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_OnDemandPrivateProxyStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigOnDemandPrivateProxyStatus object.
 * @param   parameters    the hex data of SigOnDemandPrivateProxyStatus.
 * @return  return `nil` when initialize SigOnDemandPrivateProxyStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_OnDemandPrivateProxyStatus;
        if (parameters == nil || parameters.length == 0) {
            return nil;
        }
        UInt8 tem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _onDemandPrivateGATTProxy = tem8;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _onDemandPrivateGATTProxy;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 4.3.7 Solicitation PDU RPL Configuration messages
/*
 Solicitation PDU RPL Configuration messages are used to control the solicitation
 replay protection list of a node (see Section 6.9.2.1).
 */

#pragma mark 4.3.7.1 SOLICITATION_PDU_RPL_ITEMS_CLEAR, opcode:0xB815


/// A SOLICITATION_PDU_RPL_ITEMS_CLEAR message is an acknowledged message
/// used to remove one or more items from the solicitation replay protection list of a node.
/// @note   The response to a SOLICITATION_PDU_RPL_ITEMS_CLEAR message is a
/// SOLICITATION_PDU_RPL_ITEMS_CLEAR_STATUS message.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.395), 4.3.7.1 SOLICITATION_PDU_RPL_ITEMS_CLEAR.
@implementation SigSolicitationPduRplItemsClear

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_SolicitationPduRplItemsClear;
    }
    return self;
}

/**
 * @brief   Initialize SigSolicitationPduRplItemsClear object.
 * @param   addressRange    Unicast address range.
 * The Address_Range field indicates the unicast address range (see Section 3.4.2.2.1)
 * of the solicitation PDU sequences to be removed from the solicitation replay protection
 * list of a node.
 * @return  return `nil` when initialize SigSolicitationPduRplItemsClear object fail.
 */
- (instancetype)initWithAddressRange:(SigUnicastAddressRangeFormatModel *)addressRange {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_SolicitationPduRplItemsClear;
        _addressRange = addressRange;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    if (_addressRange) {
        [mData appendData:_addressRange.parameters];
    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigSolicitationPduRplItemsStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.7.2 SOLICITATION_PDU_RPL_ITEMS_CLEAR_UNACKNOWLEDGED, opcode:0xB816


/// A SOLICITATION_PDU_RPL_ITEMS_CLEAR_UNACKNOWLEDGED message is an
/// unacknowledged message used to remove one or more items from the solicitation
/// replay protection list of a node.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.396),
/// 4.3.7.2 SOLICITATION_PDU_RPL_ITEMS_CLEAR_UNACKNOWLEDGED.
@implementation SigSolicitationPduRplItemsClearUnacknowledged

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_SolicitationPduRplItemsClearUnacknowledged;
    }
    return self;
}

/**
 * @brief   Initialize SigSolicitationPduRplItemsClearUnacknowledged object.
 * @param   addressRange    Unicast address range.
 * The Address_Range field indicates the unicast address range (see Section 3.4.2.2.1)
 * of the solicitation PDU sequences to be removed from the solicitation replay protection
 * list of a node.
 * @return  return `nil` when initialize SigSolicitationPduRplItemsClearUnacknowledged
 * object fail.
 */
- (instancetype)initWithAddressRange:(SigUnicastAddressRangeFormatModel *)addressRange {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_SolicitationPduRplItemsClearUnacknowledged;
        _addressRange = addressRange;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    if (_addressRange) {
        [mData appendData:_addressRange.parameters];
    }
    return mData;
}

@end


#pragma mark 4.3.7.3 SOLICITATION_PDU_RPL_ITEMS_STATUS, opcode:0xB817


/// A SOLICITATION_PDU_RPL_ITEMS_STATUS message is an unacknowledged message
/// used to confirm the removal of one or more items from the solicitation replay protection
/// list of a node.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.396),
/// 4.3.7.3 SOLICITATION_PDU_RPL_ITEMS_STATUS.
@implementation SigSolicitationPduRplItemsStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_SolicitationPduRplItemsStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigSolicitationPduRplItemsStatus object.
 * @param   parameters    the hex data of SigSolicitationPduRplItemsStatus.
 * @return  return `nil` when initialize SigSolicitationPduRplItemsStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_SolicitationPduRplItemsStatus;
        if (parameters == nil || parameters.length < 2) {
            return nil;
        }
        SigUnicastAddressRangeFormatModel *model = [[SigUnicastAddressRangeFormatModel alloc] initWithParameters:parameters];
        _addressRange = model;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    if (_addressRange) {
        [mData appendData:_addressRange.parameters];
    }
    return mData;
}

@end


#pragma mark 4.3.8 SAR Configuration messages


#pragma mark 4.3.8.1 SAR_TRANSMITTER_GET, opcode:0xB803


/// A SAR_TRANSMITTER_GET message is an acknowledged message used to
/// get the current SAR Transmitter state of a node.
/// @note   The response to a SAR_TRANSMITTER_GET message is a
/// SAR_TRANSMITTER_STATUS message.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.397),
/// 4.3.8.1 SAR_TRANSMITTER_GET.
@implementation SigSARTransmitterGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_SARTransmitterGet;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigSARTransmitterStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.8.2 SAR_TRANSMITTER_SET, opcode:0xB804


/// A SAR_TRANSMITTER_SET message is an acknowledged message used to
/// set the SAR Transmitter state of a node.
/// @note   The response to a SAR_TRANSMITTER_GET message is a
/// SAR_TRANSMITTER_STATUS message.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.397),
/// 4.3.8.2 SAR_TRANSMITTER_SET.
@implementation SigSARTransmitterSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_SARTransmitterSet;
    }
    return self;
}

/**
 * @brief   Initialize SigSARTransmitterSet object.
 * @param   SARTransmitter    SAR Transmitter Structure data.
 * @return  return `nil` when initialize SigSARTransmitterSet
 * object fail.
 */
- (instancetype)initWithSARTransmitter:(struct SARTransmitterStructure)SARTransmitter {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_SARTransmitterSet;
        _SARTransmitter = SARTransmitter;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt32 tem32 = CFSwapInt32HostToBig(_SARTransmitter.value);
    NSData *data = [NSData dataWithBytes:&tem32 length:4];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigSARTransmitterStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.8.3 SAR_TRANSMITTER_STATUS, opcode:0xB805


/// A SAR_TRANSMITTER_STATUS message is an unacknowledged message used
/// to report the current SAR Transmitter state of a node.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.398),
/// 4.3.8.3 SAR_TRANSMITTER_STATUS.
@implementation SigSARTransmitterStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_SARTransmitterStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigSARTransmitterStatus object.
 * @param   parameters    The hex data of SigSARTransmitterStatus object.
 * @return  return `nil` when initialize SigSARTransmitterStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_SARTransmitterStatus;
        if (parameters == nil || parameters.length < 4) {
            return nil;
        }
        UInt32 tem32 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem32, dataByte, 4);
        struct SARTransmitterStructure sarTransmitterStructure = {};
        sarTransmitterStructure.value = tem32;
        _SARTransmitter = sarTransmitterStructure;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt32 tem32 = CFSwapInt32HostToBig(_SARTransmitter.value);
    NSData *data = [NSData dataWithBytes:&tem32 length:4];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 4.3.8.4 SAR_RECEIVER_GET, opcode:0xB806


/// A SAR_RECEIVER_GET message is an acknowledged message used to get
/// the current SAR Receiver state of a node.
/// @note   The response to a SAR_RECEIVER_GET message is a
/// SAR_RECEIVER_STATUS message.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.400),
/// 4.3.8.4 SAR_RECEIVER_GET.
@implementation SigSARReceiverGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_SARReceiverGet;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigSARReceiverStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.8.5 SAR_RECEIVER_SET, opcode:0xB807


/// A SAR_RECEIVER_SET message is an acknowledged message used to set
/// the SAR Receiver state of a node.
/// @note   The response to a SAR_RECEIVER_SET message is a
/// SAR_RECEIVER_STATUS message.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.400),
/// 4.3.8.5 SAR_RECEIVER_SET.
@implementation SigSARReceiverSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_SARReceiverSet;
    }
    return self;
}

/**
 * @brief   Initialize SigSARReceiverSet object.
 * @param   SARReceiver    SAR Receiver Structure data.
 * @return  return `nil` when initialize SigSARReceiverSet
 * object fail.
 */
- (instancetype)initWithSARReceiver:(struct SARReceiverStructure)SARReceiver {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_SARReceiverSet;
        _SARReceiver = SARReceiver;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt32 tem32 = CFSwapInt32HostToBig(_SARReceiver.value);
    NSData *data = [NSData dataWithBytes:&tem32 length:3];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigSARReceiverStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.8.6 SAR_RECEIVER_STATUS, opcode:0xB808


/// A SAR_RECEIVER_STATUS message is an unacknowledged message used to report
/// the current SAR Receiver state of a node.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.401),
/// 4.3.8.6 SAR_RECEIVER_STATUS.
@implementation SigSARReceiverStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_SARReceiverStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigSARReceiverStatus object.
 * @param   parameters    The hex data of SigSARReceiverStatus object.
 * @return  return `nil` when initialize SigSARReceiverStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_SARReceiverStatus;
        if (parameters == nil || parameters.length < 3) {
            return nil;
        }
        UInt32 tem32 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem32, dataByte, 3);
        struct SARReceiverStructure SARR = {};
        SARR.value = tem32;
        _SARReceiver = SARR;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt32 tem32 = CFSwapInt32HostToBig(_SARReceiver.value);
    NSData *data = [NSData dataWithBytes:&tem32 length:3];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark - 4.3.10 Large Composition Data messages


#pragma mark 4.3.10.1 LARGE_COMPOSITION_DATA_GET, opcode:0xB811


/// A LARGE_COMPOSITION_DATA_GET message is an acknowledged message
/// used to read a portion of a page of the Composition Data (see Section 4.2.1).
/// @note   The response to a LARGE_COMPOSITION_DATA_GET message is a
/// LARGE_COMPOSITION_DATA_STATUS message.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.404),
/// 4.3.10.1 LARGE_COMPOSITION_DATA_GET.
@implementation SigLargeCompositionDataGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LargeCompositionDataGet;
    }
    return self;
}

/**
 * @brief   Initialize SigLargeCompositionDataGet object.
 * @param   page    Page number of the Composition Data.
 * The Page field shall identify the Composition Data Page number that is being read.
 * @param   offset    Offset within the page.
 * The Offset field shall identify the offset within the Composition Data Page in octets.
 * @return  return `nil` when initialize SigLargeCompositionDataGet object fail.
 */
- (instancetype)initWithPage:(UInt8)page offset:(UInt16)offset {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LargeCompositionDataGet;
        _page = page;
        _offset = offset;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _page;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    UInt16 tem16 = _offset;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigLargeCompositionDataStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.10.2 LARGE_COMPOSITION_DATA_STATUS, opcode:0xB812


/// A LARGE_COMPOSITION_DATA_STATUS message is an unacknowledged message
/// used to report a portion of a page of the Composition Data (see Section 4.2.1).
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.405),
/// 4.3.10.2 LARGE_COMPOSITION_DATA_STATUS.
@implementation SigLargeCompositionDataStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LargeCompositionDataStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigLargeCompositionDataStatus object.
 * @param   parameters    The hex data of SigLargeCompositionDataStatus object.
 * @return  return `nil` when initialize SigLargeCompositionDataStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_LargeCompositionDataStatus;
        if (parameters == nil || parameters.length < 5) {
            return nil;
        }
        UInt8 tem8 = 0;
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _page = tem8;
        memcpy(&tem16, dataByte+1, 2);
        _offset = tem16;
        memcpy(&tem16, dataByte+3, 2);
        _totalSize = tem16;
        if (parameters.length > 5) {
            _data = [parameters subdataWithRange:NSMakeRange(5, parameters.length - 5)];
        } else {
            _data = [NSData data];
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _page;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    UInt16 tem16 = _offset;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _totalSize;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    if (_data) {
        [mData appendData:_data];
    }
    return mData;
}

@end


#pragma mark 4.3.10.3 MODELS_METADATA_GET, opcode:0xB813


/// A MODELS_METADATA_GET message is an acknowledged message used to
/// read a portion of a page of the Models Metadata state (see Section 4.2.50).
/// @note   The response to a MODELS_METADATA_GET message is a
/// MODELS_METADATA_STATUS message.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.405),
/// 4.3.10.3 MODELS_METADATA_GET.
@implementation SigModelsMetadataGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_ModelsMetadataGet;
    }
    return self;
}

/**
 * @brief   Initialize SigLargeCompositionDataGet object.
 * @param   metadataPage    Page number of the Models Metadata.
 * The Metadata_Page field identifies the Models Metadata Page number.
 * @param   offset    Offset within the page.
 * The Offset field identifies the offset within the Models Metadata Page in octets.
 * @return  return `nil` when initialize SigLargeCompositionDataGet object fail.
 */
- (instancetype)initWithMetadataPage:(UInt8)metadataPage offset:(UInt16)offset {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_ModelsMetadataGet;
        _metadataPage = metadataPage;
        _offset = offset;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _metadataPage;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    UInt16 tem16 = _offset;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigModelsMetadataStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.10.4 MODELS_METADATA_STATUS, opcode:0xB814


/// A MODELS_METADATA_STATUS message is an unacknowledged message used
/// to report a portion of a page of the Models Metadata state (see Section 4.2.50).
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.406),
/// 4.3.10.4 MODELS_METADATA_STATUS.
@implementation SigModelsMetadataStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_ModelsMetadataStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigModelsMetadataStatus object.
 * @param   parameters    The hex data of SigModelsMetadataStatus object.
 * @return  return `nil` when initialize SigModelsMetadataStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_ModelsMetadataStatus;
        if (parameters == nil || parameters.length < 5) {
            return nil;
        }
        UInt8 tem8 = 0;
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _metadataPage = tem8;
        memcpy(&tem16, dataByte+1, 2);
        _offset = tem16;
        memcpy(&tem16, dataByte+3, 2);
        _totalSize = tem16;
        if (parameters.length > 5) {
            _data = [parameters subdataWithRange:NSMakeRange(5, parameters.length - 5)];
        } else {
            _data = [NSData data];
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _metadataPage;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    UInt16 tem16 = _offset;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _totalSize;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    if (_data) {
        [mData appendData:_data];
    }
    return mData;
}

@end


#pragma mark - 6 Proxyprotocol - 6.6 Proxyconfigurationmessages


#pragma mark 6.6.6 DIRECTED_PROXY_CONTROL, opcode:0x05


/// A DIRECTED_PROXY_CONTROL message is sent by a Directed Proxy Client to set whether or not the Directed
/// Proxy Server uses directed forwarding for Directed Proxy Client messages for a specified range of unicast addresses.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.626),
/// 6.6.6 DIRECTED_PROXY_CONTROL.
@implementation SigDirectedProxyControl

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigProxyFilerOpcode_directedProxyControl;
        self.responseType = [[SigFilterStatus alloc] init];
    }
    return self;
}

/**
 * @brief   Initialize SigUnicastAddressRangeFormatModel object.
 * @param   useDirected    Indicates whether directed forwarding is used for messages from the
 * Directed Proxy Client.
 * The Use_Directed field determines whether or not the Directed Proxy Server uses directed forwarding
 * for Directed Proxy Client messages.
 * @param   proxyClientUnicastAddressRange    Unicast address range of the Directed Proxy
 * Client (C.1)(C.1: If the Use_Directed field is set to Enabled, the Proxy_Client_Unicast_Addr_Range field
 * shall be present; otherwise, the Proxy_Client_Unicast_Addr_Range field shall not be present.)
 * If present, the Proxy_Client_Unicast_Addr_Range field contains the unicast address range (see
 * Section 3.4.2.2.1) for which the Directed Proxy Server will use directed forwarding for all Directed
 * Proxy Client messages.
 * @return  return `nil` when initialize SigUnicastAddressRangeFormatModel object fail.
 */
- (instancetype)initWithUseDirected:(UseDirected)useDirected proxyClientUnicastAddressRange:(SigUnicastAddressRangeFormatModel *)proxyClientUnicastAddressRange {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigProxyFilerOpcode_directedProxyControl;
        self.responseType = [[SigFilterStatus alloc] init];
        _useDirected = useDirected;
        _proxyClientUnicastAddressRange = proxyClientUnicastAddressRange;
    }
    return self;
}

/**
 * @brief   Initialize SigDirectedProxyControl object.
 * @param   parameters    The hex data of SigDirectedProxyControl object.
 * @return  return `nil` when initialize SigDirectedProxyControl object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters == nil || parameters.length != 1) {
            return nil;
        }
        self.opCode = SigProxyFilerOpcode_directedProxyControl;
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _useDirected = tem;
        if (parameters.length > 1) {
            SigUnicastAddressRangeFormatModel *model = [[SigUnicastAddressRangeFormatModel alloc] initWithParameters:[parameters subdataWithRange:NSMakeRange(1, parameters.length-1)]];
            _proxyClientUnicastAddressRange = model;
        }
        self.responseType = [[SigFilterStatus alloc] init];
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem = _useDirected;
    NSData *temData = [NSData dataWithBytes:&tem length:1];
    [mData appendData:temData];
    if (_proxyClientUnicastAddressRange && _proxyClientUnicastAddressRange.parameters) {
        [mData appendData:_proxyClientUnicastAddressRange.parameters];
    }
    return mData;
}

@end


#pragma mark 6.6.5 DIRECTED_PROXY_CAPABILITIES_STATUS, opcode:0x04


/// A DIRECTED_PROXY_CAPABILITIES_STATUS message is sent by a Directed Proxy Server to report
/// current Directed Proxy capabilities in a subnet.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.625),
/// 6.6.5 DIRECTED_PROXY_CAPABILITIES_STATUS.
@implementation SigDirectedProxyCapabilitiesStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigProxyFilerOpcode_directedProxyCapabilitiesStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigUnicastAddressRangeFormatModel object.
 * @param   directedProxy    Current Directed Proxy state.
 * The Directed_Proxy field indicates the current Directed Proxy state for the identified subnet,
 * as defined in Section 4.2.26.3.
 * @param   useDirected    Indicates whether directed forwarding is
 * used for messages from the Proxy Client.
 * The Use_Directed field indicates whether or not the Directed Proxy Server uses directed forwarding
 * for retransmitting Network PDUs originated from the Proxy Client.
 * @return  return `nil` when initialize SigUnicastAddressRangeFormatModel object fail.
 */
- (instancetype)initWithDirectedProxy:(SigDirectedProxy)directedProxy useDirected:(UseDirected)useDirected {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigProxyFilerOpcode_directedProxyCapabilitiesStatus;
        _directedProxy = directedProxy;
        _useDirected = useDirected;
    }
    return self;
}

/**
 * @brief   Initialize SigDirectedProxyCapabilitiesStatus object.
 * @param   parameters    The hex data of SigDirectedProxyCapabilitiesStatus object.
 * @return  return `nil` when initialize SigDirectedProxyCapabilitiesStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters == nil || parameters.length != 2) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _directedProxy = tem;
        memcpy(&tem, dataByte+1, 1);
        _useDirected = tem;
        self.opCode = SigProxyFilerOpcode_directedProxyCapabilitiesStatus;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem = _directedProxy;
    NSData *temData = [NSData dataWithBytes:&tem length:1];
    [mData appendData:temData];
    tem = _useDirected;
    temData = [NSData dataWithBytes:&tem length:1];
    [mData appendData:temData];
    return mData;
}

@end


@implementation SDKLibCommand (minor_ENH)

#pragma mark - 4.3.6 On-Demand Private GATT Proxy messages

/// 4.3.6.1 ON_DEMAND_PRIVATE_PROXY_GET
+ (SigMessageHandle *)onDemandPrivateProxyGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseOnDemandPrivateProxyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigOnDemandPrivateProxyGet *config = [[SigOnDemandPrivateProxyGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/// 4.3.6.2 ON_DEMAND_PRIVATE_PROXY_SET
+ (SigMessageHandle *)onDemandPrivateProxySetWithOnDemandPrivateGATTProxy:(UInt8)onDemandPrivateGATTProxy destination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseOnDemandPrivateProxyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigOnDemandPrivateProxySet *config = [[SigOnDemandPrivateProxySet alloc] initWithOnDemandPrivateGATTProxy:onDemandPrivateGATTProxy];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

#pragma mark - 4.3.7 Solicitation PDU RPL Configuration messages

/// 4.3.7.1 SOLICITATION_PDU_RPL_ITEMS_CLEAR
+ (SigMessageHandle *)solicitationPduRplItemsClearWithAddressRange:(SigUnicastAddressRangeFormatModel *)addressRange destination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSolicitationPduRplItemsStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSolicitationPduRplItemsClear *config = [[SigSolicitationPduRplItemsClear alloc] initWithAddressRange:addressRange];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    return [SigMeshLib.share sendMeshMessage:config fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

/// 4.3.7.2 SOLICITATION_PDU_RPL_ITEMS_CLEAR_UNACKNOWLEDGED
+ (SigMessageHandle *)solicitationPduRplItemsClearUnacknowledgedWithAddressRange:(SigUnicastAddressRangeFormatModel *)addressRange destination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount resultCallback:(resultBlock)resultCallback {
    SigSolicitationPduRplItemsClearUnacknowledged *config = [[SigSolicitationPduRplItemsClearUnacknowledged alloc] initWithAddressRange:addressRange];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:nil resultCallback:resultCallback];
    return [SigMeshLib.share sendMeshMessage:config fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

#pragma mark - 4.3.8 SAR Configuration messages

/// 4.3.8.1 SAR_TRANSMITTER_GET
+ (SigMessageHandle *)SARTransmitterGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSARTransmitterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSARTransmitterGet *config = [[SigSARTransmitterGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/// 4.3.8.2 SAR_TRANSMITTER_SET
+ (SigMessageHandle *)SARTransmitterSetWithSARTransmitter:(struct SARTransmitterStructure)SARTransmitter destination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSARTransmitterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSARTransmitterSet *config = [[SigSARTransmitterSet alloc] initWithSARTransmitter:SARTransmitter];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/// 4.3.8.4 SAR_RECEIVER_GET
+ (SigMessageHandle *)SARReceiverGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSARReceiverStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSARReceiverGet *config = [[SigSARReceiverGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/// 4.3.8.5 SAR_RECEIVER_SET
+ (SigMessageHandle *)SARReceiverSetWithSARReceiver:(struct SARReceiverStructure)SARReceiver destination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSARReceiverStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigSARReceiverSet *config = [[SigSARReceiverSet alloc] initWithSARReceiver:SARReceiver];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

#pragma mark - 4.3.10 Large Composition Data messages

/// 4.3.10.1 LARGE_COMPOSITION_DATA_GET
+ (SigMessageHandle *)largeCompositionDataGetWithPage:(UInt8)page offset:(UInt16)offset destination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseLargeCompositionDataStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigLargeCompositionDataGet *config = [[SigLargeCompositionDataGet alloc] initWithPage:page offset:offset];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/// 4.3.10.3 MODELS_METADATA_GET
+ (SigMessageHandle *)modelsMetadataGetWithMetadataPage:(UInt8)metadataPage offset:(UInt16)offset destination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseModelsMetadataStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigModelsMetadataGet *config = [[SigModelsMetadataGet alloc] initWithMetadataPage:metadataPage offset:offset];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:config retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    return [SigMeshLib.share sendConfigMessage:config toDestination:destination command:command];
}

/// 6.6.6 DIRECTED_PROXY_CONTROL
+ (SigMessageHandle *)directedProxyControlUseDirected:(UseDirected)useDirected proxyClientUnicastAddressRange:(SigUnicastAddressRangeFormatModel *)proxyClientUnicastAddressRange successCallback:(responseDirectedProxyCapabilitiesStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigDirectedProxyControl *message = [[SigDirectedProxyControl alloc] initWithUseDirected:useDirected proxyClientUnicastAddressRange:proxyClientUnicastAddressRange];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.resultCallback = resultCallback;
    return [SigMeshLib.share sendSigProxyConfigurationMessage:message command:command];
}

@end
