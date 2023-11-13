/********************************************************************************************************
 * @file     SigConfigMessage.m
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

#import "SigConfigMessage.h"
#import "CBUUID+Hex.h"

@implementation SigConfigMessage

/// The Type of the response message.
- (Class)responseType {
    return [SigBaseMeshMessage class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return 0;
}

/// Encodes given list of Key Indexes into a Data. As each Key Index is 12 bits long, a pair of them can fit 3 bytes. This method ensures that they are packed in compliance to the Bluetooth Mesh specification.
/// @param limit Maximum number of Key Indexes to encode.
/// @param indexes An array of 12-bit Key Indexes.
/// @returns Key Indexes encoded to a Data.
- (NSData *)encodeLimit:(int)limit indexes:(NSArray <NSNumber *>*)indexes {
    if (limit == 0 || indexes == nil || indexes.count == 0) {
        return [NSData data];
    }
    if (limit == 1 || indexes.count == 1) {
        // Encode a single Key Index into 2 bytes.
        NSNumber *indexNumber = indexes.firstObject;
        UInt16 index = indexNumber.intValue;
        UInt16 tem = CFSwapInt16HostToLittle(index);
        NSData *temData = [NSData dataWithBytes:&tem length:2];
        return temData;
    }else{
        // Encode a pair of Key Indexes into 3 bytes.
        NSNumber *firstIndexNumber = indexes.firstObject;
        NSNumber *secondIndexNumber = indexes[1];
        UInt32 index1 = firstIndexNumber.intValue;
        UInt32 index2 = secondIndexNumber.intValue;
        UInt32 pair = index1 << 12 | index2;
        NSData *temData = [NSData dataWithBytes:&pair length:4];
        NSMutableData *mData = [NSMutableData dataWithData:[temData subdataWithRange:NSMakeRange(0, temData.length-1)]];
        if (indexes.count > 2) {
            temData = [self encodeLimit:limit-2 indexes:[indexes subarrayWithRange:NSMakeRange(2, indexes.count-2)]];
        } else {
            temData = [self encodeLimit:limit-2 indexes:[NSArray array]];
        }
        [mData appendData:temData];
        return mData;
    }
}

/// Encodes given list of Key Indexes into a Data. As each Key Index is 12 bits long, a pair of them can fit 3 bytes. This method ensures that they are packed in compliance to the Bluetooth Mesh specification. Maximum number of Key Indexes to decode is 10000.
/// @param indexes An array of 12-bit Key Indexes.
/// @returns Key Indexes encoded to a Data.
- (NSData *)encodeIndexes:(NSArray <NSNumber *>*)indexes {
    return [self encodeLimit:10000 indexes:indexes];
}

/// Decodes number of Key Indexes from the given Data from the given offset. This will decode as many Indexes as possible, until the end of data is reached.
/// @param limit Maximum number of Key Indexes to decode.
/// @param data The data from where the indexes should be read.
/// @param offset The offset from where to read the indexes.
/// @returns Decoded Key Indexes.
+ (NSArray <NSNumber *>*)decodeLimit:(int)limit indexesFromData:(NSData *)data atOffset:(int)offset {
    NSInteger size = data.length - offset;
    if (limit == 0 || size < 2) {
//        TelinkLogDebug(@"limit is 0, or data.length is short.");
        return [NSArray array];
    }
    if (limit == 1 || size == 2) {
        // Decode a single Key Index from 2 bytes.
        UInt16 index = 0;
        Byte *dataByte = (Byte *)data.bytes;
        memcpy(&index, dataByte+offset, 2);
        return [NSArray arrayWithObject:@(index)];
    } else {
        // Decode a pair of Key Indexes from 3 bytes.
        UInt16 first = 0,second=0;
        UInt8 tem1=0,tem2=0,tem3=0;
        Byte *dataByte = (Byte *)data.bytes;
        memcpy(&tem1, dataByte+offset, 1);
        memcpy(&tem2, dataByte+offset+1, 1);
        memcpy(&tem3, dataByte+offset+2, 1);
        first = tem3 << 4 | tem2 >> 4;
        second = (tem2 & 0x0F) << 8 | tem1;
        NSMutableArray *rArray = [NSMutableArray arrayWithArray:@[@(first),@(second)]];
        [rArray addObjectsFromArray:[self decodeLimit:limit-2 indexesFromData:data atOffset:offset+3]];
        return rArray;
    }
}

/// Decodes number of Key Indexes from the given Data from the given offset. This will decode as many Indexes as possible, until the end of data is reached. Maximum number of Key Indexes to decode is 10000.
/// @param data The data from where the indexes should be read.
/// @param offset The offset from where to read the indexes.
/// @returns Decoded Key Indexes.
+ (NSArray <NSNumber *>*)decodeIndexesFromData:(NSData *)data atOffset:(int)offset {
    return [self decodeLimit:10000 indexesFromData:data atOffset:offset];
}

//- (BOOL)isSegmented {
//    return YES;
//}

@end


@implementation SigConfigStatusMessage
- (BOOL)isSuccess {
    return _status == SigConfigMessageStatus_success;
}
- (NSString *)message {
    UInt8 tem = 0;
    tem = (UInt8)_status;
    switch (tem) {
        case SigConfigMessageStatus_success:
            return @"Success";
            break;
        case SigConfigMessageStatus_invalidAddress:
            return @"Invalid Address";
            break;
        case SigConfigMessageStatus_invalidModel:
            return @"Invalid Model";
            break;
        case SigConfigMessageStatus_invalidAppKeyIndex:
            return @"Invalid Application Key Index";
            break;
        case SigConfigMessageStatus_invalidNetKeyIndex:
            return @"Invalid Network Key Index";
            break;
        case SigConfigMessageStatus_insufficientResources:
            return @"Insufficient resources";
            break;
        case SigConfigMessageStatus_keyIndexAlreadyStored:
            return @"Key Index already stored";
            break;
        case SigConfigMessageStatus_invalidPublishParameters:
            return @"Invalid publish parameters";
            break;
        case SigConfigMessageStatus_notASubscribeModel:
            return @"Not a Subscribe Model";
            break;
        case SigConfigMessageStatus_storageFailure:
            return @"Storage failure";
            break;
        case SigConfigMessageStatus_featureNotSupported:
            return @"Feature not supported";
            break;
        case SigConfigMessageStatus_cannotUpdate:
            return @"Cannot update";
            break;
        case SigConfigMessageStatus_cannotRemove:
            return @"Cannot remove";
            break;
        case SigConfigMessageStatus_cannotBind:
            return @"Cannot bind";
            break;
        case SigConfigMessageStatus_temporarilyUnableToChangeState:
            return @"Temporarily unable to change state";
            break;
        case SigConfigMessageStatus_cannotSet:
            return @"Cannot set";
            break;
        case SigConfigMessageStatus_unspecifiedError:
            return @"Unspecified error";
            break;
        case SigConfigMessageStatus_invalidBinding:
            return @"Invalid binding";
            break;
        default:
            return @"unknown status";
            break;
    }
}
@end


@implementation SigAcknowledgedConfigMessage
@end


@implementation SigConfigNetKeyMessage

/// Encodes Network Key Index in 2 bytes using Little Endian.
/// @returns Key Index encoded in 2 bytes.
- (NSData *)encodeNetKeyIndex {
    return [self encodeIndexes:@[@(_networkKeyIndex)]];
}

/// Decodes the Network Key Index from 2 bytes at given offset.
/// There are no any checks whether the data at the given offset are valid, or even if the offset is not outside of the data range.
/// @param data The data from where the indexes should be read.
/// @param offset The offset from where to read the indexes.
/// @returns Decoded Key Index.
+ (UInt16)decodeNetKeyIndexFromData:(NSData *)data atOffset:(int)offset {
    return (UInt16)[self decodeLimit:1 indexesFromData:data atOffset:offset].firstObject.integerValue;
}

@end


@implementation SigConfigAppKeyMessage
@end


@implementation SigConfigNetAndAppKeyMessage

- (instancetype)initWithNetworkKeyIndex:(UInt16)networkKeyIndex applicationKeyIndex:(UInt16)applicationKeyIndex {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _networkKeyIndex = networkKeyIndex;
        _applicationKeyIndex = applicationKeyIndex;
    }
    return self;
}

/// Encodes Network Key Index and Application Key Index in 3 bytes using Little Endian.
/// @returns Key Indexes encoded in 3 bytes.
- (NSData *)encodeNetAndAppKeyIndex {
    return [self encodeIndexes:@[@(_applicationKeyIndex),@(_networkKeyIndex)]];
}

/// Decodes the Network Key Index and Application Key Index from 3 bytes at given offset.
/// There are no any checks whether the data at the given offset are valid, or even if the offset is not outside of the data range.
/// @param data The data from where the indexes should be read.
/// @param offset The offset from where to read the indexes.
/// @returns Decoded Key Indexes.
+ (nullable SigConfigNetAndAppKeyMessage *)decodeNetAndAppKeyIndexFromData:(NSData *)data atOffset:(int)offset {
    NSArray *array = [self decodeIndexesFromData:data atOffset:offset];
    if (array && array.count >= 2) {
        UInt16 tem1 = (UInt16)[array[0] integerValue];
        UInt16 tem2 = (UInt16)[array[1] integerValue];
        SigConfigNetAndAppKeyMessage *msg = [[SigConfigNetAndAppKeyMessage alloc] initWithNetworkKeyIndex:tem2 applicationKeyIndex:tem1];
        return msg;
    }else{
        return nil;
    }
}

@end


@implementation SigConfigElementMessage
@end


@implementation SigConfigModelMessage

- (UInt32)modelId {
    return (UInt32)_modelIdentifier;
}

@end


@implementation SigConfigAnyModelMessage

/// Returns `true` for Models with identifiers assigned by Bluetooth SIG,
/// `false` otherwise.
- (BOOL)isBluetoothSIGAssigned {
    return _companyIdentifier == 0;
}

- (UInt32)modelId {
    if (_companyIdentifier != 0) {
        return ((UInt32)_companyIdentifier << 16) | (UInt32)self.modelIdentifier;
    } else {
        return (UInt32)self.modelIdentifier;
    }
}

@end


@implementation SigConfigVendorModelMessage

- (UInt32)modelId {
    return ((UInt32)_companyIdentifier << 16) | (UInt32)self.modelIdentifier;
}

@end


@implementation SigConfigAddressMessage
@end


@implementation SigConfigVirtualLabelMessage
@end


@implementation SigConfigModelAppList
@end


@implementation SigConfigModelSubscriptionList
@end


@implementation SigCompositionDataPage
@end

@implementation SigPage0

/// Message parameters as Data.
- (NSData *)parameters {
    UInt8 tem = self.page;
    NSMutableData *mData = [NSMutableData dataWithBytes:&tem length:1];
    UInt16 tem2 = self.companyIdentifier;
    [mData appendData:[NSData dataWithBytes:&tem2 length:2]];
    tem2 = self.productIdentifier;
    [mData appendData:[NSData dataWithBytes:&tem2 length:2]];
    tem2 = self.versionIdentifier;
    [mData appendData:[NSData dataWithBytes:&tem2 length:2]];
    tem2 = self.minimumNumberOfReplayProtectionList;
    [mData appendData:[NSData dataWithBytes:&tem2 length:2]];
    tem2 = self.features.rawValue;
    [mData appendData:[NSData dataWithBytes:&tem2 length:2]];
    NSArray *elements = [NSArray arrayWithArray:self.elements];
    for (SigElementModel *elementModel in elements) {
        [mData appendData:elementModel.getElementData];
    }
    return mData;
}

- (BOOL)isSegmented {
    return YES;
}

/// This initializer constructs the Page 0 of Composition Data from the given Node.
/// @param node  The Node to construct the Page 0 from.
- (instancetype)initWithNode:(SigNodeModel *)node {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.page = 0;
        _companyIdentifier = [LibTools uint16From16String:node.cid];
        _productIdentifier = [LibTools uint16From16String:node.pid];
        _versionIdentifier = [LibTools uint16From16String:node.vid];
        _minimumNumberOfReplayProtectionList = [LibTools uint16From16String:node.crpl];
        if (node.features) {
            _features = node.features;
        } else {
            _features = [[SigNodeFeatures alloc] init];
        }
        _elements = [NSMutableArray arrayWithArray:node.elements];
    }
    return self;
}

/// This initializer should construct the message based on the received parameters.
/// @param parameters  The Access Layer parameters.
- (instancetype)initWithParameters:(NSData *)parameters {
    if (parameters && parameters.length > 0) {
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        self.page = tem;
        if (parameters.length < 11) {
            return nil;
        }
    }
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        UInt16 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte+1, 2);
        _companyIdentifier = tem;
        memcpy(&tem, dataByte+3, 2);
        _productIdentifier = tem;
        memcpy(&tem, dataByte+5, 2);
        _versionIdentifier = tem;
        memcpy(&tem, dataByte+7, 2);
        _minimumNumberOfReplayProtectionList = tem;
        memcpy(&tem, dataByte+9, 2);
        _features = [[SigNodeFeatures alloc] initWithRawValue:tem];
        
        NSMutableArray *readElements = [NSMutableArray array];
        int offset = 11;
        while (offset < parameters.length) {
            SigElementModel *element = [[SigElementModel alloc] initWithCompositionData:parameters offset:&offset];
            element.index = (UInt8)readElements.count;
            [readElements addObject:element];
        }
        _elements = readElements;
    }
    return self;
}
@end


#pragma mark - 4.3.2 Configuration messages


#pragma mark 4.3.2.37 Config AppKey Add, opcode:0x00


/// The Config AppKey Add is an acknowledged message used to
/// add an AppKey to the AppKey List on a node and bind it to the NetKey identified by NetKeyIndex.
/// The added AppKey can be used by the node only as a pair with the specified NetKey.
/// The AppKey is used to authenticate and decrypt messages it receives,
/// as well as authenticate and encrypt messages it sends.
/// @note   The response to a Config AppKey Add message is a Config AppKey Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.169), 4.3.2.37 Config AppKey Add.
@implementation SigConfigAppKeyAdd

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configAppKeyAdd;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigAppKeyAdd object.
 * @param   applicationKey    The 128-bit Application Key data.
 * The NetKeyIndexAndAppKeyIndex field contains two indexes that shall identify
 * the global NetKey Index of the NetKey and the global AppKey Index of the AppKey.
 * These two indexes shall be encoded as defined in Section 4.3.1.1
 * using NetKey Index as first key index and AppKey Index as second key index.
 * The AppKey field shall contain the AppKey value identified by the AppKeyIndex.
 * @return  return `nil` when initialize SigConfigAppKeyAdd object fail.
 */
- (instancetype)initWithApplicationKey:(SigAppkeyModel *)applicationKey {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configAppKeyAdd;
        self.applicationKeyIndex = applicationKey.index;
        self.networkKeyIndex = applicationKey.boundNetKey;
        _key = [LibTools nsstringToHex:applicationKey.key];
    }
    return self;
}

/**
 * @brief   Initialize SigConfigAppKeyAdd object.
 * @param   parameters    the hex data of SigConfigAppKeyAdd.
 * @return  return `nil` when initialize SigConfigAppKeyAdd object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configAppKeyAdd;
        if (parameters == nil || parameters.length != 19) {
            return nil;
        }
        SigConfigNetAndAppKeyMessage *message = [SigConfigAppKeyAdd decodeNetAndAppKeyIndexFromData:parameters atOffset:0];
        self.networkKeyIndex = message.networkKeyIndex;
        self.applicationKeyIndex = message.applicationKeyIndex;
        _key = [parameters subdataWithRange:NSMakeRange(3, 16)];
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData dataWithData:[self encodeNetAndAppKeyIndex]];
    [mData appendData:_key];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigAppKeyStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.38 Config AppKey Update, opcode:0x01


/// The Config AppKey Update is an acknowledged message used
/// to update an AppKey value on the AppKey List on a node.
/// The updated AppKey is used by the node to authenticate and decrypt messages it receives,
/// as well as authenticate and encrypt messages it sends,
/// as defined by the Key Refresh procedure (see Section 3.10.4).
/// The response to an Config AppKey Update message is an Config AppKey Status message.
/// as well as authenticate and encrypt messages it sends.
/// @note   The response to a Config AppKey Update message is a Config AppKey Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.169), 4.3.2.38 Config AppKey Update.
@implementation SigConfigAppKeyUpdate

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configAppKeyUpdate;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigAppKeyUpdate object.
 * @param   applicationKey    The 128-bit Application Key data.
 * The NetKeyIndexAndAppKeyIndex field contains two indexes that shall identify
 * the global NetKey Index of the NetKey and the global AppKey Index of the AppKey.
 * These two indexes shall be encoded as defined in Section 4.3.1.1
 * using NetKey Index as first key index and AppKey Index as second key index.
 * The AppKey field shall contain the AppKey value identified by the AppKeyIndex.
 * @return  return `nil` when initialize SigConfigAppKeyUpdate object fail.
 */
- (instancetype)initWithApplicationKey:(SigAppkeyModel *)applicationKey {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configAppKeyUpdate;
        self.applicationKeyIndex = applicationKey.index;
        self.networkKeyIndex = applicationKey.boundNetKey;
        _key = [LibTools nsstringToHex:applicationKey.key];
    }
    return self;
}

/**
 * @brief   Initialize SigConfigAppKeyUpdate object.
 * @param   parameters    the hex data of SigConfigAppKeyUpdate.
 * @return  return `nil` when initialize SigConfigAppKeyUpdate object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configAppKeyUpdate;
        if (parameters == nil || parameters.length != 19) {
            return nil;
        }
        SigConfigNetAndAppKeyMessage *message = [SigConfigAppKeyAdd decodeNetAndAppKeyIndexFromData:parameters atOffset:0];
        self.networkKeyIndex = message.networkKeyIndex;
        self.applicationKeyIndex = message.applicationKeyIndex;
        _key = [parameters subdataWithRange:NSMakeRange(3, 16)];
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData dataWithData:[self encodeNetAndAppKeyIndex]];
    [mData appendData:_key];
    return mData;
}

/// The Op Code of the response message.
- (Class)responseType {
    return [SigConfigAppKeyStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.5 Config Composition Data Status, opcode:0x02


/// The Config Composition Data Status is an unacknowledged message
/// used to report a single page of the Composition Data (see Section 4.2.1).
/// @note   This message uses a single octet opcode to maximize the size of a payload.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.155), 4.3.2.5 Config Composition Data Status.
@implementation SigConfigCompositionDataStatus
/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configCompositionDataStatus;
    }
    return self;
}

///**
// * @brief   Initialize SigConfigCompositionDataStatus object.
// * @param   page    The Composition Data page.
// * @return  return `nil` when initialize SigConfigCompositionDataStatus object fail.
// */
//- (instancetype)initWithReportPage:(SigCompositionDataPage *)page {
//    if (self = [super init]) {
//        self.opCode = SigOpCode_configCompositionDataStatus;
//        _page = page;
//    }
//    return self;
//}

/**
 * @brief   Initialize SigConfigCompositionDataStatus object.
 * @param   parameters    the hex data of SigConfigCompositionDataStatus.
 * @return  return `nil` when initialize SigConfigCompositionDataStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configCompositionDataStatus;
        if (parameters == nil || parameters.length == 0) {
            return nil;
        } else {
            UInt8 tem = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem, dataByte, 1);
            if (tem != 0) {
                TelinkLogError(@"Other Pages are not supoprted.");
            }
//            if (tem == 0) {
                SigPage0 *page0 = [[SigPage0 alloc] initWithParameters:parameters];//001102010033306900070000001101000002000300040000FE01FE00FF01FF001002100410061007100013011303130413110200000000020002100613
                if (page0) {
                    _page = page0;
                } else {
                    TelinkLogError(@"init page0 fail.");
                    return nil;
                }
//            } else {
//                TelinkLogError(@"Other Pages are not supoprted.");
//                return nil;
//            }
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return _page.parameters;
}

@end


#pragma mark 4.3.2.16 Config Model Publication Set, opcode:0x03


/// The Config Model Publication Set is an acknowledged message
/// used to set the Model Publication state (see Section 4.2.2)
/// of an outgoing message that originates from a model.
/// @note   The response to a Config Model Publication Set message
/// is a Config Model Publication Status message.
/// @note   The Config Model Publication Set message uses a single octet opcode
/// to maximize the size of a payload.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.158), 4.3.2.16 Config Model Publication Set.
@implementation SigConfigModelPublicationSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelPublicationSet;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigModelPublicationSet object.
 * @param   publish    The config parameter of publication.
 * @param   elementAddress    The ElementAddress field is the unicast address of the element, all other address types are Prohibited.
 * @param   modelIdentifier    The model ID.
 * @param   companyIdentifier    The company ID.
 * @return  return `nil` when initialize SigConfigModelPublicationSet object fail.
 */
- (instancetype)initWithPublish:(SigPublish *)publish toElementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelPublicationSet;
        if ([SigHelper.share isVirtualAddress:[LibTools uint16From16String:publish.address]]) {
            // ConfigModelPublicationVirtualAddressSet should be used instead.
            return nil;
        }
        _publish = publish;
        self.elementAddress = elementAddress;
        self.modelIdentifier = modelIdentifier;
        self.companyIdentifier = companyIdentifier;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigModelPublicationSet object.
 * @param   publish    The config parameter of publication.
 * @param   elementAddress    The ElementAddress field is the unicast address of the element, all other address types are Prohibited.
 * @param   model    The SigModelIDModel object.
 * @return  return `nil` when initialize SigConfigModelPublicationSet object fail.
 */
- (instancetype)initWithPublish:(SigPublish *)publish toElementAddress:(UInt16)elementAddress model:(SigModelIDModel *)model {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelPublicationSet;
        if ([SigHelper.share isVirtualAddress:[LibTools uint16From16String:publish.address]]) {
            // ConfigModelPublicationVirtualAddressSet should be used instead.
            return nil;
        }
        _publish = publish;
        self.elementAddress = elementAddress;
        self.modelIdentifier = model.getIntModelIdentifier;
        self.companyIdentifier = model.getIntCompanyIdentifier;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigModelPublicationSet object.
 * @param   parameters    the hex data of SigConfigModelPublicationSet.
 * @return  return `nil` when initialize SigConfigModelPublicationSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelPublicationSet;
        if (parameters == nil || (parameters.length != 11 && parameters.length != 13)) {
            return nil;
        }
        UInt16 tem1=0,tem2=0,tem3=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem1, dataByte, 2);
        memcpy(&tem2, dataByte+2, 2);
        memcpy(&tem3, dataByte+4, 2);
        self.elementAddress = tem1;
//        UInt16 address = tem2;
        UInt16 index = tem3 & 0x0FFF;
        UInt8 tem = 0;
        memcpy(&tem, dataByte+5, 1);
        int flag = (int)((tem & 0x10) >> 4);
        memcpy(&tem, dataByte+6, 1);
        UInt8 ttl = tem;
        memcpy(&tem, dataByte+7, 1);
        UInt8 periodSteps = tem & 0x3F;
        SigStepResolution periodResolution = tem >> 6;
        memcpy(&tem, dataByte+8, 1);
        UInt8 count = tem & 0x07;
        UInt8 interval = tem >> 3;
        SigRetransmit *retransmit = [[SigRetransmit alloc] initWithPublishRetransmitCount:count intervalSteps:interval];
        self.publish = [[SigPublish alloc] initWithDestination:tem2 withKeyIndex:index friendshipCredentialsFlag:flag ttl:ttl periodSteps:periodSteps periodResolution:periodResolution retransmit:retransmit];
        if (parameters.length == 13) {
            memcpy(&tem3, dataByte+11, 2);
            self.modelIdentifier = tem3;
            memcpy(&tem3, dataByte+9, 2);
            self.companyIdentifier = tem3;
        }else{
            memcpy(&tem3, dataByte+9, 2);
            self.modelIdentifier = tem3;
            self.companyIdentifier = 0;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem = self.elementAddress;
    NSData *data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    tem = [LibTools uint16From16String:_publish.address];
    data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    UInt8 tem8 = 0;
    tem8 = _publish.index & 0xFF;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = (UInt8)(_publish.index >> 8) | (UInt8)(_publish.credentials << 4);
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _publish.ttl;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = (_publish.periodSteps & 0x3F) | (_publish.periodResolution << 6);
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = (_publish.retransmit.count & 0x07) | (_publish.retransmit.steps << 3);
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (self.companyIdentifier) {
        tem = self.companyIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
        tem = self.modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    } else {
        tem = self.modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigModelPublicationStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.39 Config AppKey Delete, opcode:0x8000


/// The Config AppKey Delete is an acknowledged message used
/// to delete an AppKey from the AppKey List on a node.
/// @note   The response to a Config AppKey Delete message is a Config AppKey Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.170), 4.3.2.39 Config AppKey Delete.
@implementation SigConfigAppKeyDelete

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configAppKeyDelete;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigAppKeyDelete object.
 * @param   applicationKey    the application key need to delete.
 * @return  return `nil` when initialize SigConfigAppKeyDelete object fail.
 */
- (instancetype)initWithApplicationKey:(SigAppkeyModel *)applicationKey {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configAppKeyDelete;
        self.applicationKeyIndex = applicationKey.index;
        self.networkKeyIndex = applicationKey.boundNetKey;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigAppKeyDelete object.
 * @param   parameters    the hex data of SigConfigAppKeyDelete.
 * @return  return `nil` when initialize SigConfigAppKeyDelete object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configAppKeyDelete;
        if (parameters == nil || parameters.length != 3) {
            return nil;
        }
        SigConfigNetAndAppKeyMessage *message = [SigConfigAppKeyAdd decodeNetAndAppKeyIndexFromData:parameters atOffset:0];
        self.networkKeyIndex = message.networkKeyIndex;
        self.applicationKeyIndex = message.applicationKeyIndex;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData dataWithData:[self encodeNetAndAppKeyIndex]];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigAppKeyStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.41 Config AppKey Get, opcode:0x8001


/// The AppKey Get is an acknowledged message used
/// to report all AppKeys bound to the NetKey.
/// @note   The response to a Config AppKey Get message is a Config AppKey List message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.171), 4.3.2.41 Config AppKey Get.
@implementation SigConfigAppKeyGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configAppKeyGet;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigAppKeyGet object.
 * @param   networkKeyIndex    the index of network key.
 * @return  return `nil` when initialize SigConfigAppKeyGet object fail.
 */
- (instancetype)initWithNetworkKeyIndex:(UInt16)networkKeyIndex {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configAppKeyGet;
        self.networkKeyIndex = networkKeyIndex;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigAppKeyGet object.
 * @param   parameters    the hex data of SigConfigAppKeyGet.
 * @return  return `nil` when initialize SigConfigAppKeyGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configAppKeyGet;
        if (parameters == nil || parameters.length != 2) {
            return nil;
        }
        self.networkKeyIndex = [SigConfigNetKeyMessage decodeNetKeyIndexFromData:parameters atOffset:0];
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData dataWithData:[self encodeNetKeyIndex]];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigAppKeyList class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.42 Config AppKey List, opcode:0x8002


/// The Config AppKey List is an unacknowledged message
/// reporting all AppKeys that are bound to the NetKey.
/// @note   The response to a Config AppKey Get message is a Config AppKey List message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.171), 4.3.2.42 Config AppKey List.
@implementation SigConfigAppKeyList

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configAppKeyList;
    }
    return self;
}

//- (instancetype)initWithNetWorkKey:(SigNetkeyModel *)networkKey applicationKeys:(NSArray <SigAppkeyModel *>*)applicationKeys status:(SigConfigMessageStatus)status {
//    if (self = [super init]) {
//        self.opCode = SigOpCode_configAppKeyList;
//        self.networkKeyIndex = networkKey.index;
//        _applicationKeyIndexes = [NSMutableArray array];
//        for (SigAppkeyModel *model in applicationKeys) {
//            [_applicationKeyIndexes addObject:@(model.index)];
//        }
//        _status = status;
//    }
//    return self;
//}
//
//- (instancetype)initWithStatus:(SigConfigMessageStatus)status forMessage:(SigConfigAppKeyGet *)message {
//    if (self = [super init]) {
//        self.opCode = SigOpCode_configAppKeyList;
//        self.networkKeyIndex = message.networkKeyIndex;
//        _applicationKeyIndexes = [NSMutableArray array];
//        _status = status;
//    }
//    return self;
//}

/**
 * @brief   Initialize SigConfigAppKeyList object.
 * @param   parameters    the hex data of SigConfigAppKeyList.
 * @return  return `nil` when initialize SigConfigAppKeyList object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configAppKeyList;
        if (parameters == nil || parameters.length < 3) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _status = tem;
        self.networkKeyIndex = [SigConfigNetKeyMessage decodeNetKeyIndexFromData:parameters atOffset:1];
        self.applicationKeyIndexes = [NSMutableArray arrayWithArray:[SigConfigNetKeyMessage decodeIndexesFromData:parameters atOffset:3]];
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _status;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    data = [self encodeNetKeyIndex];
    [mData appendData:data];
    data = [self encodeIndexes:_applicationKeyIndexes];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 4.3.2.40 Config AppKey Status, opcode:0x8003


/// The Config AppKey Status is an unacknowledged message used to
/// report a status for the requesting message,
/// based on the NetKey Index identifying the NetKey on the NetKey List
/// and on the AppKey Index identifying the AppKey on the AppKey List.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.170), 4.3.2.40 Config AppKey Status.
@implementation SigConfigAppKeyStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configAppKeyStatus;
    }
    return self;
}

//- (instancetype)initWithApplicationKey:(SigAppkeyModel *)applicationKey {
//    if (self = [super init]) {
//        self.opCode = SigOpCode_configAppKeyStatus;
//        self.applicationKeyIndex = applicationKey.index;
//        self.networkKeyIndex = applicationKey.boundNetKey;
//        _status = SigConfigMessageStatus_success;
//    }
//    return self;
//}
//
//- (instancetype)initWithStatus:(SigConfigMessageStatus)status forMessage:(SigConfigNetAndAppKeyMessage *)message {
//    if (self = [super init]) {
//        self.opCode = SigOpCode_configAppKeyStatus;
//        self.applicationKeyIndex = message.applicationKeyIndex;
//        self.networkKeyIndex = message.networkKeyIndex;
//        _status = status;
//    }
//    return self;
//}

/**
 * @brief   Initialize SigConfigAppKeyStatus object.
 * @param   parameters    the hex data of SigConfigAppKeyStatus.
 * @return  return `nil` when initialize SigConfigAppKeyStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configAppKeyStatus;
        if (parameters == nil || parameters.length != 4) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _status = tem;
        SigConfigNetAndAppKeyMessage *message = [SigConfigAppKeyAdd decodeNetAndAppKeyIndexFromData:parameters atOffset:1];
        self.networkKeyIndex = message.networkKeyIndex;
        self.applicationKeyIndex = message.applicationKeyIndex;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _status;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    data = [self encodeNetAndAppKeyIndex];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 4.3.2.4 Config Composition Data Get, opcode:0x8008


/// The Config Composition Data Get is an acknowledged message used
/// to read one page of the Composition Data (see Section 4.2.1).
/// @note   The response to a Config Composition Data Get message
/// is a Config Composition Data Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.154), 4.3.2.4 Config Composition Data Get.
@implementation SigConfigCompositionDataGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configCompositionDataGet;
        _page = 0;
    }
    return self;
}

- (instancetype)initWithPage:(UInt8)page {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configCompositionDataGet;
        _page = page;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigCompositionDataGet object.
 * @param   parameters    the hex data of SigConfigCompositionDataGet.
 * @return  return `nil` when initialize SigConfigCompositionDataGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configCompositionDataGet;
        _page = 0;
        if (parameters.length == 1) {
            UInt8 tem = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem, dataByte, 1);
            _page = tem;
        } else {
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    UInt8 tem = _page;
    NSData *temData = [NSData dataWithBytes:&tem length:1];
    return temData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigCompositionDataStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}


@end


#pragma mark 4.3.2.1 Config Beacon Get, opcode:0x8009


/// The Config Beacon Get is an acknowledged message used to get
/// the current Secure Network Beacon state of a node (see Section 4.2.10).
/// @note   The response to a Config Beacon Get message is a Config Beacon Status message.
/// @note   There are no Parameters for this message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.154), 4.3.2.1 Config Beacon Get.
@implementation SigConfigBeaconGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configBeaconGet;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigBeaconGet object.
 * @param   parameters    the hex data of SigConfigBeaconGet.
 * @return  return `nil` when initialize SigConfigBeaconGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configBeaconGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigBeaconStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.2 Config Beacon Set, opcode:0x800A


/// The Config Beacon Set is an acknowledged message used to set
/// the Secure Network Beacon state of a node (see Section 4.2.10).
/// @note   The response to a Config Beacon Set message is a Config Beacon Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.154), 4.3.2.2 Config Beacon Set.
@implementation SigConfigBeaconSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configBeaconSet;
    }
    return self;
}

/// Configures the Secure Network Beacon behavior on the Node.
/// @param enable `True` to enable Secure Network Beacon feature,  `false` to disable.
- (instancetype)initWithEnable:(BOOL)enable {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configBeaconSet;
        _state = enable;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigBeaconSet object.
 * @param   parameters    the hex data of SigConfigBeaconSet.
 * @return  return `nil` when initialize SigConfigBeaconSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configBeaconSet;
        if (parameters == nil || parameters.length != 1) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        if (tem > 1) {
            return nil;
        }
        _state = tem == 0x01;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    UInt8 tem = [self state]?0x01:0x00;
    return [NSData dataWithBytes:&tem length:1];
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigBeaconStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.3 Config Beacon Status, opcode:0x800B


/// The Config Beacon Status is an unacknowledged message used to report
/// the current Secure Network Beacon state of a node (see Section 4.2.10).
/// @note   - seeAlso: Mesh_v1.0.pdf (page.154), 4.3.2.3 Config Beacon Status.
@implementation SigConfigBeaconStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configBeaconStatus;
    }
    return self;
}

/// Configures the Secure Network Beacon behavior on the Node.
/// @param enable `True` to enable Secure Network Beacon feature, `false` to disable.
- (instancetype)initWithEnable:(BOOL)enable {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configBeaconStatus;
        _isEnabled = enable;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigBeaconStatus object.
 * @param   parameters    the hex data of SigConfigBeaconStatus.
 * @return  return `nil` when initialize SigConfigBeaconStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configBeaconStatus;
        if (parameters == nil || parameters.length == 0) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        if (tem > 1) {
            return nil;
        }
        _isEnabled = tem == 0x01;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    UInt8 tem = [self isEnabled]?0x01:0x00;
    return [NSData dataWithBytes:&tem length:1];
}

@end


#pragma mark 4.3.2.6 Config Default TTL Get, opcode:0x800C


/// The Config Default TTL Get is an acknowledged message used to get
/// the current Default TTL state of a node.
/// @note   The response to a Config Default TTL Get message is a
/// Config Default TTL Status message.
/// @note   There are no Parameters for this message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.155), 4.3.2.6 Config Default TTL Get.
@implementation SigConfigDefaultTtlGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configDefaultTtlGet;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigDefaultTtlGet object.
 * @param   parameters    the hex data of SigConfigDefaultTtlGet.
 * @return  return `nil` when initialize SigConfigDefaultTtlGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configDefaultTtlGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigDefaultTtlStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.7 Config Default TTL Set, opcode:0x800D


/// The Config Default TTL Set is an acknowledged message used to set
/// the Default TTL state of a node (see Section 4.2.7).
/// @note   The response to a Config Default TTL Set message is a
/// Config Default TTL Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.155), 4.3.2.7 Config Default TTL Set.
@implementation SigConfigDefaultTtlSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configDefaultTtlSet;
    }
    return self;
}

- (instancetype)initWithTtl:(UInt8)ttl {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configDefaultTtlSet;
        _ttl = ttl;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigDefaultTtlSet object.
 * @param   parameters    the hex data of SigConfigDefaultTtlSet.
 * @return  return `nil` when initialize SigConfigDefaultTtlSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configDefaultTtlSet;
        if (parameters == nil || parameters.length != 1) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _ttl = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem = _ttl;
    NSData *temData = [NSData dataWithBytes:&tem length:1];
    [mData appendData:temData];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigDefaultTtlStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.8 Config Default TTL Status, opcode:0x800E


/// The Config Default TTL Status is an unacknowledged message used to report
/// the current Default TTL state of a node (see Section 4.2.7).
/// @note   - seeAlso: Mesh_v1.0.pdf (page.155), 4.3.2.8 Config Default TTL Status.
@implementation SigConfigDefaultTtlStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configDefaultTtlStatus;
    }
    return self;
}

//- (instancetype)initWithTtl:(UInt8)ttl {
//    if (self = [super init]) {
//        self.opCode = SigOpCode_configDefaultTtlStatus;
//        _ttl = ttl;
//    }
//    return self;
//}

/**
 * @brief   Initialize SigConfigDefaultTtlStatus object.
 * @param   parameters    the hex data of SigConfigDefaultTtlStatus.
 * @return  return `nil` when initialize SigConfigDefaultTtlStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configDefaultTtlStatus;
        if (parameters == nil || parameters.length != 1) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _ttl = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem = _ttl;
    NSData *temData = [NSData dataWithBytes:&tem length:1];
    [mData appendData:temData];
    return mData;
}

@end


#pragma mark 4.3.2.55 Config Friend Get, opcode:0x800F


/// The Config Friend Get is an acknowledged message used to get
/// the current Friend state of a node (see Section 4.2.13).
/// @note   The response to a Config Friend Get message is a Config Friend Status message.
/// @note   There are no Parameters for this message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.176), 4.3.2.55 Config Friend Get.
@implementation SigConfigFriendGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configFriendGet;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigFriendGet object.
 * @param   parameters    the hex data of SigConfigFriendGet.
 * @return  return `nil` when initialize SigConfigFriendGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configFriendGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigFriendStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.56 Config Friend Set, opcode:0x8010


/// The Config Friend Set is an acknowledged message used to set
/// the Friend state of a node (see Section 4.2.13).
/// @note   The response to a Config Friend Set message is a Config Friend Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.176), 4.3.2.56 Config Friend Set.
@implementation SigConfigFriendSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configFriendSet;
    }
    return self;
}

- (instancetype)initWithEnable:(BOOL)enable {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configFriendSet;
        self.state = enable ? SigNodeFeaturesState_enabled : SigNodeFeaturesState_notEnabled;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigFriendSet object.
 * @param   parameters    the hex data of SigConfigFriendSet.
 * @return  return `nil` when initialize SigConfigFriendSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configFriendSet;
        if (parameters == nil || parameters.length != 1) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _state = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    UInt8 tem = _state;
    return [NSData dataWithBytes:&tem length:1];
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigFriendStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.57 Config Friend Status, opcode:0x8011


/// The Config Friend Status is an unacknowledged message used to report
/// the current Friend state of a node (see Section 4.2.13).
/// @note   - seeAlso: Mesh_v1.0.pdf (page.177), 4.3.2.57 Config Friend Status.
@implementation SigConfigFriendStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configFriendStatus;
    }
    return self;
}

///// Creates the Config Friend Status message.
/////
///// - parameter state: The Friend state of the Node.
//- (instancetype)initWithState:(SigNodeFeaturesState)state {
//    if (self = [super init]) {
//        self.opCode = SigOpCode_configFriendStatus;
//        self.state = state;
//    }
//    return self;
//}
//
//- (instancetype)initWithNode:(SigNodeModel *)node {
//    if (self = [super init]) {
//        self.opCode = SigOpCode_configFriendStatus;
//        self.state = node.features.proxyFeature <= 2 ? node.features.proxyFeature : SigNodeFeaturesState_notSupported;
//    }
//    return self;
//}

/**
 * @brief   Initialize SigConfigFriendStatus object.
 * @param   parameters    the hex data of SigConfigFriendStatus.
 * @return  return `nil` when initialize SigConfigFriendStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configFriendStatus;
        if (parameters == nil || parameters.length != 1) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _state = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    UInt8 tem = _state;
    return [NSData dataWithBytes:&tem length:1];
}

@end


#pragma mark 4.3.2.9 Config GATT Proxy Get, opcode:0x8012


/// The Config GATT Proxy Get is an acknowledged message used to get
/// the current GATT Proxy state of a node (see Section 4.2.11).
/// @note   The response to a Config GATT Proxy Get message is a Config GATT Proxy Status message.
/// @note   There are no Parameters for this message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.156), 4.3.2.9 Config GATT Proxy Get.
@implementation SigConfigGATTProxyGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configGATTProxyGet;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigGATTProxyGet object.
 * @param   parameters    the hex data of SigConfigGATTProxyGet.
 * @return  return `nil` when initialize SigConfigGATTProxyGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configGATTProxyGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigGATTProxyStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.10 Config GATT Proxy Set, opcode:0x8013


/// The Config GATT Proxy Set is an acknowledged message used to set
/// the GATT Proxy state of a node (see Section 4.2.11).
/// @note   The response to a Config GATT Proxy Set message is a Config GATT Proxy Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.156), 4.3.2.10 Config GATT Proxy Set.
@implementation SigConfigGATTProxySet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configGATTProxySet;
    }
    return self;
}

/// The interval between retransmissions, in seconds.
/// Configures the GATT Proxy on the Node.
///
/// When disabled, the Node will no longer be able to work as a GATT Proxy
/// until enabled again.
///
/// - parameter enable: `True` to enable GATT Proxy feature, `false` to disable.
- (instancetype)initWithEnable:(BOOL)enable {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configGATTProxySet;
        self.state = enable ? SigNodeFeaturesState_enabled : SigNodeFeaturesState_notEnabled;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigGATTProxySet object.
 * @param   parameters    the hex data of SigConfigGATTProxySet.
 * @return  return `nil` when initialize SigConfigGATTProxySet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configGATTProxySet;
        if (parameters == nil || parameters.length != 1) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _state = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    UInt8 tem = _state;
    return [NSData dataWithBytes:&tem length:1];
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigGATTProxyStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.11 Config GATT Proxy Status, opcode:0x8014


/// The Config GATT Proxy Status is an unacknowledged message used to report
/// the current GATT Proxy state of a node (see Section 4.2.11).
/// @note   - seeAlso: Mesh_v1.0.pdf (page.156), 4.3.2.11 Config GATT Proxy Status.
@implementation SigConfigGATTProxyStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configGATTProxyStatus;
    }
    return self;
}

///// Creates the Config GATT Proxy Status message.
/////
///// - parameter state: The GATT Proxy state of the Node.
//- (instancetype)initWithState:(SigNodeFeaturesState)state {
//    if (self = [super init]) {
//        self.opCode = SigOpCode_configGATTProxyStatus;
//        self.state = state;
//    }
//    return self;
//}
//
//- (instancetype)initWithNode:(SigNodeModel *)node {
//    if (self = [super init]) {
//        self.opCode = SigOpCode_configGATTProxyStatus;
//        self.state = node.features.proxyFeature <= 2 ? node.features.proxyFeature : SigNodeFeaturesState_notSupported;
//    }
//    return self;
//}

/**
 * @brief   Initialize SigConfigGATTProxyStatus object.
 * @param   parameters    the hex data of SigConfigGATTProxyStatus.
 * @return  return `nil` when initialize SigConfigGATTProxyStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configGATTProxyStatus;
        if (parameters == nil || parameters.length != 1) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _state = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    UInt8 tem = _state;
    return [NSData dataWithBytes:&tem length:1];
}

@end


#pragma mark 4.3.2.58 Config Key Refresh Phase Get, opcode:0x8015


/// The Config Key Refresh Phase Get is an acknowledged message used to get
/// the current Key Refresh Phase state of the identified network key.
/// @note   The response to a Config Key Refresh Phase Get message is a Config Key Refresh Phase Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.177), 4.3.2.58 Config Key Refresh Phase Get.
@implementation SigConfigKeyRefreshPhaseGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configKeyRefreshPhaseGet;
    }
    return self;
}

- (instancetype)initWithNetKeyIndex:(UInt16)netKeyIndex {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configKeyRefreshPhaseGet;
        _netKeyIndex = netKeyIndex;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigKeyRefreshPhaseGet object.
 * @param   parameters    the hex data of SigConfigKeyRefreshPhaseGet.
 * @return  return `nil` when initialize SigConfigKeyRefreshPhaseGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configKeyRefreshPhaseGet;
        if (parameters == nil || parameters.length != 2) {
            return nil;
        }
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _netKeyIndex = (tem16 >> 4) & 0xFFF;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _netKeyIndex << 4;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigKeyRefreshPhaseStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.59 Config Key Refresh Phase Set, opcode:0x8016


/// The Config Key Refresh Phase Set is an acknowledged message used to set
/// the Key Refresh Phase state of the identified network key (see Section 4.2.14).
/// @note   The response to a Config Key Refresh Phase Get message is a Config Key Refresh Phase Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.177), 4.3.2.59 Config Key Refresh Phase Set.
@implementation SigConfigKeyRefreshPhaseSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configKeyRefreshPhaseSet;
    }
    return self;
}

- (instancetype)initWithNetKeyIndex:(UInt16)netKeyIndex transition:(SigControllableKeyRefreshTransitionValues)transition {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configKeyRefreshPhaseSet;
        _netKeyIndex = netKeyIndex;
        _transition = transition;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigKeyRefreshPhaseSet object.
 * @param   parameters    the hex data of SigConfigKeyRefreshPhaseSet.
 * @return  return `nil` when initialize SigConfigKeyRefreshPhaseSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configKeyRefreshPhaseSet;
        if (parameters == nil || parameters.length != 3) {
            return nil;
        }
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _netKeyIndex = (tem16 >> 4) & 0xFFF;
        UInt8 tem8 = 0;
        memcpy(&tem8, dataByte+2, 1);
        _transition = tem8;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _netKeyIndex << 4;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = _transition;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigKeyRefreshPhaseStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.60 Config Key Refresh Phase Status, opcode:0x8017


/// The Config Key Refresh Phase Status is an unacknowledged message used to report
/// the current Key Refresh Phase state of the identified network key (see Section 4.2.14).
/// @note   - seeAlso: Mesh_v1.0.pdf (page.177), 4.3.2.60 Config Key Refresh Phase Status.
@implementation SigConfigKeyRefreshPhaseStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configKeyRefreshPhaseStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigKeyRefreshPhaseStatus object.
 * @param   parameters    the hex data of SigConfigKeyRefreshPhaseStatus.
 * @return  return `nil` when initialize SigConfigKeyRefreshPhaseStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configKeyRefreshPhaseStatus;
        if (parameters == nil || parameters.length != 4) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _status = tem;
        UInt16 tem16 = 0;
        memcpy(&tem16, dataByte+1, 2);
        _netKeyIndex = (tem16 >> 4) & 0xFFF;
        memcpy(&tem, dataByte+3, 1);
        _phase = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _status;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    UInt16 tem16 = _netKeyIndex << 4;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem8 = _phase;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 4.3.2.15 Config Model Publication Get, opcode:0x8018


/// The Config Model Publication Get is an acknowledged message used to get
/// the publish address and parameters of an outgoing message that originates from a model.
/// @note   The response to a Config Model Publication Get message is a Config Model Publication Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.157), 4.3.2.15 Config Model Publication Get.
@implementation SigConfigModelPublicationGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelPublicationGet;
    }
    return self;
}

- (instancetype)initWithElementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelPublicationGet;
        self.elementAddress = elementAddress;
        self.modelIdentifier = modelIdentifier;
        self.companyIdentifier = companyIdentifier;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigModelPublicationGet object.
 * @param   parameters    the hex data of SigConfigModelPublicationGet.
 * @return  return `nil` when initialize SigConfigModelPublicationGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelPublicationGet;
        if (parameters == nil || (parameters.length != 4 && parameters.length != 6)) {
            return nil;
        }
        UInt16 tem1=0,tem2=0,tem3=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem1, dataByte, 2);
        memcpy(&tem2, dataByte+2, 2);
        self.elementAddress = tem1;
        if (parameters.length == 6) {
            memcpy(&tem3, dataByte+4, 2);
            self.modelIdentifier = tem3;
            self.companyIdentifier = tem2;
        }else{
            self.modelIdentifier = tem2;
            self.companyIdentifier = 0;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem = self.elementAddress;
    NSData *data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    if (self.companyIdentifier) {
        tem = self.companyIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    }
    tem = self.modelIdentifier;
    data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigModelPublicationStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.18 Config Model Publication Status, opcode:0x8019


/// The Config Model Publication Status is an unacknowledged message used to report
/// the model Publication state (see Section 4.2.2) of an outgoing message that is published by the model.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.160), 4.3.2.18 Config Model Publication Status.
@implementation SigConfigModelPublicationStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelPublicationStatus;
    }
    return self;
}

//- (instancetype)initResponseToSigConfigAnyModelMessage:(SigConfigAnyModelMessage *)request {
//    return [self initResponseToSigConfigAnyModelMessage:request withPublish:[[SigPublish class] init]];
//}
//
- (instancetype)initResponseToSigConfigAnyModelMessage:(SigConfigAnyModelMessage *)request withPublish:(SigPublish *)publish {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelPublicationStatus;
        _publish = publish;
        self.elementAddress = request.elementAddress;
        self.modelIdentifier = request.modelIdentifier;
        self.companyIdentifier = request.companyIdentifier;
        _status = SigConfigMessageStatus_success;
    }
    return self;
}
//
//- (instancetype)initResponseToSigConfigAnyModelMessage:(SigConfigAnyModelMessage *)request withStatus:(SigConfigMessageStatus)status {
//    if (self = [super init]) {
//        self.opCode = SigOpCode_configModelPublicationStatus;
//        _publish = [[SigPublish class] init];
//        self.elementAddress = request.elementAddress;
//        self.modelIdentifier = request.modelIdentifier;
//        self.companyIdentifier = request.companyIdentifier;
//        _status = status;
//    }
//    return self;
//}
//
//- (instancetype)initWithConfirmSigConfigModelPublicationSet:(SigConfigModelPublicationSet *)request {
//    return [self initResponseToSigConfigAnyModelMessage:request withPublish:request.publish];
//}
//
//- (instancetype)initWithConfirmSigConfigModelPublicationVirtualAddressSet:(SigConfigModelPublicationVirtualAddressSet *)request {
//    return [self initResponseToSigConfigAnyModelMessage:request withPublish:request.publish];
//}

/**
 * @brief   Initialize SigConfigModelPublicationStatus object.
 * @param   parameters    the hex data of SigConfigModelPublicationStatus.
 * @return  return `nil` when initialize SigConfigModelPublicationStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelPublicationStatus;
        if (parameters == nil || (parameters.length != 12 && parameters.length != 14)) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _status = tem;
        
        UInt16 tem1=0,tem2=0,tem3=0;
        memcpy(&tem1, dataByte+1, 2);
        memcpy(&tem2, dataByte+3, 2);
        memcpy(&tem3, dataByte+5, 2);
        self.elementAddress = tem1;
        UInt16 address = tem2;
        UInt16 index = tem3 & 0x0FFF;
        memcpy(&tem, dataByte+6, 1);
        int flag = (int)((tem & 0x10) >> 4);
        memcpy(&tem, dataByte+7, 1);
        UInt8 ttl = tem;
        memcpy(&tem, dataByte+8, 1);
        UInt8 periodSteps = tem & 0x3F;
        SigStepResolution periodResolution = tem >> 6;
        memcpy(&tem, dataByte+9, 1);
        UInt8 count = tem & 0x07;
        UInt8 interval = tem >> 3;
        SigRetransmit *retransmit = [[SigRetransmit alloc] initWithPublishRetransmitCount:count intervalSteps:interval];
        self.publish = [[SigPublish alloc] initWithDestination:address withKeyIndex:index friendshipCredentialsFlag:flag ttl:ttl periodSteps:periodSteps periodResolution:periodResolution retransmit:retransmit];
        
        if (parameters.length == 14) {
            memcpy(&tem3, dataByte+12, 2);
            self.modelIdentifier = tem3;
            memcpy(&tem3, dataByte+10, 2);
            self.companyIdentifier = tem3;
        }else{
            memcpy(&tem3, dataByte+10, 2);
            self.modelIdentifier = tem3;
            self.companyIdentifier = 0;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _status;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    UInt16 tem = self.elementAddress;
    data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    tem = [LibTools uint16From16String:_publish.address];
    data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    tem8 = _publish.index & 0xFF;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = (UInt8)(_publish.index >> 8) | (UInt8)(_publish.credentials << 4);
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _publish.ttl;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = (_publish.periodSteps & 0x3F) | (_publish.periodResolution >> 6);
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = (_publish.retransmit.count & 0x07) | (_publish.retransmit.steps << 3);
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (self.companyIdentifier) {
        tem = self.companyIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
        tem = self.modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    } else {
        tem = self.modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    }
    return mData;
}

@end


#pragma mark 4.3.2.17 Config Model Publication Virtual Address Set, opcode:0x801A


/// The Config Model Publication Virtual Address Set is an acknowledged message used to set
/// the model Publication state (see Section 4.2.2) of an outgoing message that originates from a model.
/// @note   The response to a Config Model Publication Virtual Address Set message is a
/// Config Model Publication Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.159), 4.3.2.17 Config Model Publication Virtual Address Set.
@implementation SigConfigModelPublicationVirtualAddressSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelPublicationVirtualAddressSet;
    }
    return self;
}

- (instancetype)initWithPublish:(SigPublish *)publish toElementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelPublicationVirtualAddressSet;
        if ([SigHelper.share isVirtualAddress:[LibTools uint16From16String:_publish.address]]) {
            // ConfigModelPublicationVirtualAddressSet should be used instead.
            return nil;
        }
        _publish = publish;
        self.elementAddress = elementAddress;
        self.modelIdentifier = modelIdentifier;
        self.companyIdentifier = companyIdentifier;
    }
    return self;
}

- (instancetype)initWithPublish:(SigPublish *)publish toElementAddress:(UInt16)elementAddress model:(SigModelIDModel *)model {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelPublicationVirtualAddressSet;
        if ([SigHelper.share isVirtualAddress:[LibTools uint16From16String:_publish.address]]) {
            // ConfigModelPublicationVirtualAddressSet should be used instead.
            return nil;
        }
        _publish = publish;
        self.elementAddress = elementAddress;
        self.modelIdentifier = model.getIntModelIdentifier;
        self.companyIdentifier = model.getIntCompanyIdentifier;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigModelPublicationVirtualAddressSet object.
 * @param   parameters    the hex data of SigConfigModelPublicationVirtualAddressSet.
 * @return  return `nil` when initialize SigConfigModelPublicationVirtualAddressSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelPublicationVirtualAddressSet;
        if (parameters == nil || (parameters.length != 25 && parameters.length != 27)) {
            return nil;
        }
        UInt16 tem1=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem1, dataByte, 2);
        self.elementAddress = tem1;
        NSString *label = [LibTools convertDataToHexStr:[parameters subdataWithRange:NSMakeRange(2, 16)]];
        memcpy(&tem1, dataByte+18, 2);
        UInt16 index = tem1 & 0x0FFF;
        UInt8 tem = 0;
        memcpy(&tem, dataByte+19, 1);
        int flag = (int)((tem & 0x10) >> 4);
        memcpy(&tem, dataByte+20, 1);
        UInt8 ttl = tem;
        memcpy(&tem, dataByte+21, 1);
        UInt8 periodSteps = tem & 0x3F;
        SigStepResolution periodResolution = tem >> 6;
        memcpy(&tem, dataByte+22, 1);
        UInt8 count = tem & 0x07;
        UInt8 interval = tem >> 3;
        SigRetransmit *retransmit = [[SigRetransmit alloc] initWithPublishRetransmitCount:count intervalSteps:interval];
        self.publish = [[SigPublish alloc] initWithStringDestination:label withKeyIndex:index friendshipCredentialsFlag:flag ttl:ttl periodSteps:periodSteps periodResolution:periodResolution retransmit:retransmit];

        
        if (parameters.length == 27) {
            memcpy(&tem1, dataByte+25, 2);
            self.modelIdentifier = tem1;
            memcpy(&tem1, dataByte+23, 2);
            self.companyIdentifier = tem1;
        }else{
            memcpy(&tem1, dataByte+23, 2);
            self.modelIdentifier = tem1;
            self.companyIdentifier = 0;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = 0;
    UInt16 tem = self.elementAddress;
    NSData *data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    [mData appendData:_publish.publicationAddress.virtualLabel.getData];
    tem8 = _publish.index & 0xFF;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = (UInt8)(_publish.index >> 8) | (UInt8)(_publish.credentials << 4);
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _publish.ttl;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = (_publish.periodSteps & 0x3F) | (_publish.periodResolution << 6);
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = (_publish.retransmit.count & 0x07) | (_publish.retransmit.steps << 3);
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (self.companyIdentifier) {
        tem = self.companyIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
        tem = self.modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    } else {
        tem = self.modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigModelPublicationStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.19 Config Model Subscription Add, opcode:0x801B


/// The Config Model Subscription Add is an acknowledged message used to add
/// an address to a Subscription List of a model (see Section 4.2.3).
/// @note   The response to a Config Model Subscription Add message is a
/// Config Model Subscription Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.161), 4.3.2.19 Config Model Subscription Add.
@implementation SigConfigModelSubscriptionAdd

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionAdd;
    }
    return self;
}

- (instancetype)initWithGroupAddress:(UInt16)groupAddress toElementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionAdd;
        if (![SigHelper.share isGroupAddress:groupAddress]) {
            // ConfigModelSubscriptionVirtualAddressAdd should be used instead.
            return nil;
        }
        _address = groupAddress;
        self.elementAddress = elementAddress;
        self.modelIdentifier = modelIdentifier;
        self.companyIdentifier = companyIdentifier;
    }
    return self;
}

- (instancetype)initWithGroup:(SigGroupModel *)group toElementAddress:(UInt16)elementAddress model:(SigModelIDModel *)model {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionAdd;
        if (![SigHelper.share isGroupAddress:group.intAddress]) {
            // ConfigModelSubscriptionVirtualAddressAdd should be used instead.
            return nil;
        }
        _address = [LibTools uint16From16String:group.address];
        self.elementAddress = elementAddress;
        self.modelIdentifier = model.getIntModelIdentifier;
        self.companyIdentifier = model.getIntCompanyIdentifier;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigModelSubscriptionAdd object.
 * @param   parameters    the hex data of SigConfigModelSubscriptionAdd.
 * @return  return `nil` when initialize SigConfigModelSubscriptionAdd object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionAdd;
        if (parameters == nil || (parameters.length != 6 && parameters.length != 8)) {
            return nil;
        }
        UInt16 tem1=0,tem2=0,tem3=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem1, dataByte, 2);
        memcpy(&tem2, dataByte+2, 2);
        self.elementAddress = tem1;
        _address = tem2;
        if (parameters.length == 8) {
            memcpy(&tem3, dataByte+4, 2);
            self.companyIdentifier = tem3;
            memcpy(&tem3, dataByte+6, 2);
            self.modelIdentifier = tem3;
        } else {
            memcpy(&tem3, dataByte+4, 2);
            self.modelIdentifier = tem3;
            self.companyIdentifier = 0;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem = self.elementAddress;
    NSData *data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    tem = _address;
    data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    if (self.companyIdentifier) {
        tem = self.companyIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
        tem = self.modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    } else {
        tem = self.modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigModelSubscriptionStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.21 Config Model Subscription Delete, opcode:0x801C


/// The Config Model Subscription Delete is an acknowledged message used to delete
/// a subscription address from the Subscription List of a model (see Section 4.2.3).
/// @note   The response to a Config Model Subscription Delete message is a
/// Config Model Subscription Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.162), 4.3.2.21 Config Model Subscription Delete.
@implementation SigConfigModelSubscriptionDelete

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionDelete;
    }
    return self;
}

- (instancetype)initWithGroupAddress:(UInt16)groupAddress elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionDelete;
        if (![SigHelper.share isGroupAddress:groupAddress]) {
            // ConfigModelSubscriptionVirtualAddressAdd should be used instead.
            return nil;
        }
        _address = groupAddress;
        self.elementAddress = elementAddress;
        self.modelIdentifier = modelIdentifier;
        self.companyIdentifier = companyIdentifier;
    }
    return self;
}

- (instancetype)initWithGroup:(SigGroupModel *)group fromModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionDelete;
        if (![SigHelper.share isGroupAddress:group.intAddress]) {
            // ConfigModelSubscriptionVirtualAddressAdd should be used instead.
            return nil;
        }
        _address = [LibTools uint16From16String:group.address];
        self.elementAddress = elementAddress;
        self.modelIdentifier = model.getIntModelIdentifier;
        self.companyIdentifier = model.getIntCompanyIdentifier;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigModelSubscriptionDelete object.
 * @param   parameters    the hex data of SigConfigModelSubscriptionDelete.
 * @return  return `nil` when initialize SigConfigModelSubscriptionDelete object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionDelete;
        if (parameters == nil || (parameters.length != 6 && parameters.length != 8)) {
            return nil;
        }
        UInt16 tem1=0,tem2=0,tem3=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem1, dataByte, 2);
        memcpy(&tem2, dataByte+2, 2);
        self.elementAddress = tem1;
        _address = tem2;
        if (parameters.length == 8) {
            memcpy(&tem3, dataByte+4, 2);
            self.companyIdentifier = tem3;
            memcpy(&tem3, dataByte+6, 2);
            self.modelIdentifier = tem3;
        } else {
            memcpy(&tem3, dataByte+4, 2);
            self.modelIdentifier = tem3;
            self.companyIdentifier = 0;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem = self.elementAddress;
    NSData *data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    tem = _address;
    data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    if (self.companyIdentifier) {
        tem = self.companyIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
        tem = self.modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    } else {
        tem = self.modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigModelSubscriptionStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.25 Config Model Subscription Delete All, opcode:0x801D


/// The Config Model Subscription Delete All is an acknowledged message used to discard
/// the Subscription List of a model (see Section 4.2.3).
/// @note   The response to a Config Model Subscription Delete All message is a
/// Config Model Subscription Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.164), 4.3.2.25 Config Model Subscription Delete All.
@implementation SigConfigModelSubscriptionDeleteAll

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionDeleteAll;
    }
    return self;
}

- (instancetype)initWithElementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionDeleteAll;
        self.elementAddress = elementAddress;
        self.modelIdentifier = modelIdentifier;
        self.companyIdentifier = companyIdentifier;
    }
    return self;
}

- (instancetype)initFromModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionDeleteAll;
        self.elementAddress = elementAddress;
        self.modelIdentifier = model.getIntModelIdentifier;
        self.companyIdentifier = model.getIntCompanyIdentifier;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigModelSubscriptionDeleteAll object.
 * @param   parameters    the hex data of SigConfigModelSubscriptionDeleteAll.
 * @return  return `nil` when initialize SigConfigModelSubscriptionDeleteAll object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionDeleteAll;
        if (parameters == nil || (parameters.length != 4 && parameters.length != 6)) {
            return nil;
        }
        UInt16 tem1=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem1, dataByte, 2);
        self.elementAddress = tem1;
        if (parameters.length == 6) {
            memcpy(&tem1, dataByte+2, 2);
            self.companyIdentifier = tem1;
            memcpy(&tem1, dataByte+4, 2);
            self.modelIdentifier = tem1;
        } else {
            memcpy(&tem1, dataByte+2, 2);
            self.modelIdentifier = tem1;
            self.companyIdentifier = 0;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem = self.elementAddress;
    NSData *data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    if (self.companyIdentifier) {
        tem = self.companyIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
        tem = self.modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    } else {
        tem = self.modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigModelSubscriptionStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.23 Config Model Subscription Overwrite, opcode:0x801E


/// The Config Model Subscription Overwrite is an acknowledged message used to
/// discard the Subscription List and add an address to the cleared Subscription
/// List of a model (see Section 4.2.3).
/// @note   The response to a Config Model Subscription Overwrite message is a
/// Config Model Subscription Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.163), 4.3.2.23 Config Model Subscription Overwrite.
@implementation SigConfigModelSubscriptionOverwrite

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionOverwrite;
    }
    return self;
}

- (instancetype)initWithGroupAddress:(UInt16)groupAddress elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionOverwrite;
        if (![SigHelper.share isGroupAddress:groupAddress]) {
            // ConfigModelSubscriptionVirtualAddressAdd should be used instead.
            return nil;
        }
        _address = groupAddress;
        self.elementAddress = elementAddress;
        self.modelIdentifier = modelIdentifier;
        self.companyIdentifier = companyIdentifier;
    }
    return self;
}

- (instancetype)initWithGroup:(SigGroupModel *)group toModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionOverwrite;
        if (![SigHelper.share isGroupAddress:group.intAddress]) {
            // ConfigModelSubscriptionVirtualAddressAdd should be used instead.
            return nil;
        }
        _address = [LibTools uint16From16String:group.address];
        self.elementAddress = elementAddress;
        self.modelIdentifier = model.getIntModelIdentifier;
        self.companyIdentifier = model.getIntCompanyIdentifier;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigModelSubscriptionOverwrite object.
 * @param   parameters    the hex data of SigConfigModelSubscriptionOverwrite.
 * @return  return `nil` when initialize SigConfigModelSubscriptionOverwrite object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionOverwrite;
        if (parameters == nil || (parameters.length != 6 && parameters.length != 8)) {
            return nil;
        }
        UInt16 tem1=0,tem2=0,tem3=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem1, dataByte, 2);
        memcpy(&tem2, dataByte+2, 2);
        self.elementAddress = tem1;
        _address = tem2;
        if (parameters.length == 8) {
            memcpy(&tem3, dataByte+4, 2);
            self.companyIdentifier = tem3;
            memcpy(&tem3, dataByte+6, 2);
            self.modelIdentifier = tem3;
        } else {
            memcpy(&tem3, dataByte+4, 2);
            self.modelIdentifier = tem3;
            self.companyIdentifier = 0;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem = self.elementAddress;
    NSData *data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    tem = _address;
    data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    if (self.companyIdentifier) {
        tem = self.companyIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
        tem = self.modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    } else {
        tem = self.modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigModelSubscriptionStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.26 Config Model Subscription Status, opcode:0x801F


/// The Config Model Subscription Status is an unacknowledged message used to report
/// a status of the operation on the Subscription List (see Section 4.2.3).
/// @note   - seeAlso: Mesh_v1.0.pdf (page.165), 4.3.2.26 Config Model Subscription Status.
@implementation SigConfigModelSubscriptionStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionStatus;
    }
    return self;
}

//- (instancetype)initResponseToSigConfigModelPublicationStatus:(SigConfigModelPublicationStatus *)request withStatus:(SigConfigMessageStatus *)status {
//    if (self = [super init]) {
//        self.opCode = SigOpCode_configModelSubscriptionStatus;
//        //SigConfigModelSubscriptionAdd|SigConfigModelSubscriptionDelete|SigConfigModelSubscriptionOverwrite|SigConfigModelSubscriptionStatus
//        if ([request isMemberOfClass:[SigConfigModelSubscriptionAdd class]]) {
//            _address = ((SigConfigModelSubscriptionAdd *)request).address;
//        }else if ([request isMemberOfClass:[SigConfigModelSubscriptionDelete class]]) {
//            _address = ((SigConfigModelSubscriptionDelete *)request).address;
//        }else if ([request isMemberOfClass:[SigConfigModelSubscriptionOverwrite class]]) {
//            _address = ((SigConfigModelSubscriptionOverwrite *)request).address;
//        }else if ([request isMemberOfClass:[SigConfigModelSubscriptionStatus class]]) {
//            _address = ((SigConfigModelSubscriptionStatus *)request).address;
//        }else{
//            TelinkLogError(@"Unknown request class.");
//            return nil;
//        }
//        self.elementAddress = request.elementAddress;
//        self.modelIdentifier = request.modelIdentifier;
//        self.companyIdentifier = request.companyIdentifier;
//        _status = request.status;
//        }
//    return self;
//}
//
//- (instancetype)initResponseToSigConfigAnyModelMessage:(SigConfigAnyModelMessage *)request withStatus:(SigConfigMessageStatus)status {
//    if (self = [super init]) {
//        self.opCode = SigOpCode_configModelSubscriptionStatus;
//        //SigConfigModelSubscriptionVirtualAddressAdd|SigConfigModelSubscriptionVirtualAddressDelete|SigConfigModelSubscriptionVirtualAddressOverwrite
//        if ([request isMemberOfClass:[SigConfigModelSubscriptionVirtualAddressAdd class]]) {
//            _address = [[SigMeshAddress alloc] initWithVirtualLabel:((SigConfigModelSubscriptionVirtualAddressAdd *)request).virtualLabel].address;
//        }else if ([request isMemberOfClass:[SigConfigModelSubscriptionVirtualAddressDelete class]]) {
//            _address = [[SigMeshAddress alloc] initWithVirtualLabel:((SigConfigModelSubscriptionVirtualAddressDelete *)request).virtualLabel].address;
//        }else if ([request isMemberOfClass:[SigConfigModelSubscriptionVirtualAddressDelete class]]) {
//            _address = [[SigMeshAddress alloc] initWithVirtualLabel:((SigConfigModelSubscriptionVirtualAddressDelete *)request).virtualLabel].address;
//        }else{
//            TelinkLogError(@"Unknown request class.");
//            return nil;
//        }
//        self.elementAddress = request.elementAddress;
//        self.modelIdentifier = request.modelIdentifier;
//        self.companyIdentifier = request.companyIdentifier;
//        _status = status;
//        }
//    return self;
//}
//
//- (instancetype)initResponseToSigConfigModelSubscriptionDeleteAll:(SigConfigModelSubscriptionDeleteAll *)request withStatus:(SigConfigMessageStatus)status {
//    if (self = [super init]) {
//        self.opCode = SigOpCode_configModelSubscriptionStatus;
//        _address = MeshAddress_unassignedAddress;
//        self.elementAddress = request.elementAddress;
//        self.modelIdentifier = request.modelIdentifier;
//        self.companyIdentifier = request.companyIdentifier;
//        _status = status;
//    }
//    return self;
//}
//
//- (instancetype)initWithConfirmAddingGroup:(SigGroupModel *)group toModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress withStatus:(SigConfigMessageStatus)status {
//    if (self = [super init]) {
//        self.opCode = SigOpCode_configModelSubscriptionStatus;
//        _status = status;
//        _address = [LibTools uint16From16String:group.address];
//        self.elementAddress = elementAddress;
//        self.modelIdentifier = model.getIntModelIdentifier;
//        self.companyIdentifier = model.getIntCompanyIdentifier;
//    }
//    return self;
//}
//
//- (instancetype)initWithConfirmDeletingAddress:(UInt16)address fromModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress withStatus:(SigConfigMessageStatus)status {
//    if (self = [super init]) {
//        self.opCode = SigOpCode_configModelSubscriptionStatus;
//        _status = status;
//        _address = address;
//        self.elementAddress = elementAddress;
//        self.modelIdentifier = model.getIntModelIdentifier;
//        self.companyIdentifier = model.getIntCompanyIdentifier;
//    }
//    return self;
//}
//
//- (instancetype)initWithConfirmDeletingAllFromModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress {
//    if (self = [super init]) {
//        self.opCode = SigOpCode_configModelSubscriptionStatus;
//        _status = SigConfigMessageStatus_success;
//        _address = MeshAddress_unassignedAddress;
//        self.elementAddress = elementAddress;
//        self.modelIdentifier = model.getIntModelIdentifier;
//        self.companyIdentifier = model.getIntCompanyIdentifier;
//    }
//    return self;
//}

/**
 * @brief   Initialize SigConfigModelSubscriptionStatus object.
 * @param   parameters    the hex data of SigConfigModelSubscriptionStatus.
 * @return  return `nil` when initialize SigConfigModelSubscriptionStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionStatus;
        if (parameters == nil || (parameters.length != 7 && parameters.length != 9)) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _status = tem;
        
        UInt16 tem1=0,tem2=0;
        memcpy(&tem1, dataByte+1, 2);
        memcpy(&tem2, dataByte+3, 2);
        self.elementAddress = tem1;
        _address = tem2;
        if (parameters.length == 9) {
            memcpy(&tem1, dataByte+5, 2);
            self.companyIdentifier = tem1;
            memcpy(&tem1, dataByte+7, 2);
            self.modelIdentifier = tem1;
        } else {
            self.companyIdentifier = 0;
            memcpy(&tem1, dataByte+5, 2);
            self.modelIdentifier = tem1;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 temState = _status;
    NSData *data = [NSData dataWithBytes:&temState length:1];
    [mData appendData:data];
    UInt16 tem = self.elementAddress;
    data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    tem = _address;
    data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    if (self.companyIdentifier) {
        tem = self.companyIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
        tem = self.modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    } else {
        tem = self.modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    }
    return mData;
}

@end


#pragma mark 4.3.2.20 Config Model Subscription Virtual Address Add, opcode:0x8020


/// The Config Model Subscription Virtual Address Add is an acknowledged message used to add
/// an address to a Subscription List of a model (see Section 4.2.3).
/// @note   The response to a Config Model Subscription Virtual Address Add message is a
/// Config Model Subscription Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.162), 4.3.2.20 Config Model Subscription Virtual Address Add.
@implementation SigConfigModelSubscriptionVirtualAddressAdd

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionVirtualAddressAdd;
    }
    return self;
}

- (instancetype)initWithVirtualLabel:(CBUUID *)virtualLabel elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionVirtualAddressAdd;
        if (!virtualLabel) {
            // ConfigModelSubscriptionAdd should be used instead.
            return nil;
        }
        _virtualLabel = virtualLabel;
        self.elementAddress = elementAddress;
        self.modelIdentifier = modelIdentifier;
        self.companyIdentifier = companyIdentifier;
    }
    return self;
}

- (instancetype)initWithGroup:(SigGroupModel *)group toModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionVirtualAddressAdd;
        if (!group.meshAddress.virtualLabel) {
            // ConfigModelSubscriptionAdd should be used instead.
            return nil;
        }
        _virtualLabel = group.meshAddress.virtualLabel;
        self.elementAddress = elementAddress;
        self.modelIdentifier = model.getIntModelIdentifier;
        self.companyIdentifier = model.getIntCompanyIdentifier;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigModelSubscriptionVirtualAddressAdd object.
 * @param   parameters    the hex data of SigConfigModelSubscriptionVirtualAddressAdd.
 * @return  return `nil` when initialize SigConfigModelSubscriptionVirtualAddressAdd object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionVirtualAddressAdd;
        if (parameters == nil || (parameters.length != 20 && parameters.length != 24)) {
            return nil;
        }
        UInt16 tem1=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem1, dataByte, 2);
        self.elementAddress = tem1;
        _virtualLabel = [CBUUID UUIDWithData:[parameters subdataWithRange:NSMakeRange(2, 16)]];
        if (parameters.length == 24) {
            memcpy(&tem1, dataByte+18, 2);
            self.companyIdentifier = tem1;
            memcpy(&tem1, dataByte+20, 2);
            self.modelIdentifier = tem1;
        } else {
            memcpy(&tem1, dataByte+18, 2);
            self.modelIdentifier = tem1;
            self.companyIdentifier = 0;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem = self.elementAddress;
    NSData *data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    data = _virtualLabel.data;
    [mData appendData:data];
    if (self.companyIdentifier) {
        tem = self.companyIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
        tem = self.modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    } else {
        tem = self.modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigModelSubscriptionStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.22 Config Model Subscription Virtual Address Delete, opcode:0x8021


/// The Config Model Subscription Virtual Address Delete is an acknowledged message used to delete
/// a subscription address from the Subscription List of a model (see Section 4.2.3).
/// @note   The response to a Config Model Subscription Virtual Address Delete message is a
/// Config Model Subscription Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.162), 4.3.2.22 Config Model Subscription Virtual Address Delete.
@implementation SigConfigModelSubscriptionVirtualAddressDelete

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionVirtualAddressDelete;
    }
    return self;
}

- (instancetype)initWithVirtualLabel:(CBUUID *)virtualLabel elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionVirtualAddressDelete;
        if (!virtualLabel) {
            // ConfigModelSubscriptionAdd should be used instead.
            return nil;
        }
        _virtualLabel = virtualLabel;
        self.elementAddress = elementAddress;
        self.modelIdentifier = modelIdentifier;
        self.companyIdentifier = companyIdentifier;
    }
    return self;
}

- (instancetype)initWithGroup:(SigGroupModel *)group fromModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionVirtualAddressDelete;
        if (!group.meshAddress.virtualLabel) {
            // ConfigModelSubscriptionAdd should be used instead.
            return nil;
        }
        _virtualLabel = group.meshAddress.virtualLabel;
        self.elementAddress = elementAddress;
        self.modelIdentifier = model.getIntModelIdentifier;
        self.companyIdentifier = model.getIntCompanyIdentifier;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigModelSubscriptionVirtualAddressDelete object.
 * @param   parameters    the hex data of SigConfigModelSubscriptionVirtualAddressDelete.
 * @return  return `nil` when initialize SigConfigModelSubscriptionVirtualAddressDelete object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionVirtualAddressDelete;
        if (parameters == nil || (parameters.length != 20 && parameters.length != 24)) {
            return nil;
        }
        UInt16 tem1=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem1, dataByte, 2);
        self.elementAddress = tem1;
        _virtualLabel = [CBUUID UUIDWithData:[parameters subdataWithRange:NSMakeRange(2, 16)]];
        if (parameters.length == 24) {
            memcpy(&tem1, dataByte+18, 2);
            self.companyIdentifier = tem1;
            memcpy(&tem1, dataByte+20, 2);
            self.modelIdentifier = tem1;
        } else {
            memcpy(&tem1, dataByte+18, 2);
            self.modelIdentifier = tem1;
            self.companyIdentifier = 0;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem = self.elementAddress;
    NSData *data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    data = _virtualLabel.data;
    [mData appendData:data];
    if (self.companyIdentifier) {
        tem = self.companyIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
        tem = self.modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    } else {
        tem = self.modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigModelSubscriptionStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.24 Config Model Subscription Virtual Address Overwrite, opcode:0x8022


/// The Config Model Subscription Virtual Address Overwrite is an acknowledged message used to discard
/// the Subscription List and add an address to the cleared Subscription List of a model (see Section 4.2.3).
/// @note   The response to a Config Model Subscription Virtual Address Overwrite message is a
/// Config Model Subscription Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.164), 4.3.2.24 Config Model Subscription Virtual Address Overwrite.
@implementation SigConfigModelSubscriptionVirtualAddressOverwrite

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionVirtualAddressOverwrite;
    }
    return self;
}

- (instancetype)initWithVirtualLabel:(CBUUID *)virtualLabel elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionVirtualAddressOverwrite;
        if (!virtualLabel) {
            // ConfigModelSubscriptionAdd should be used instead.
            return nil;
        }
        _virtualLabel = virtualLabel;
        self.elementAddress = elementAddress;
        self.modelIdentifier = modelIdentifier;
        self.companyIdentifier = companyIdentifier;
    }
    return self;
}

- (instancetype)initWithGroup:(SigGroupModel *)group fromModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionVirtualAddressOverwrite;
        if (!group.meshAddress.virtualLabel) {
            // ConfigModelSubscriptionAdd should be used instead.
            return nil;
        }
        _virtualLabel = group.meshAddress.virtualLabel;
        self.elementAddress = elementAddress;
        self.modelIdentifier = model.getIntModelIdentifier;
        self.companyIdentifier = model.getIntCompanyIdentifier;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigModelSubscriptionVirtualAddressOverwrite object.
 * @param   parameters    the hex data of SigConfigModelSubscriptionVirtualAddressOverwrite.
 * @return  return `nil` when initialize SigConfigModelSubscriptionVirtualAddressOverwrite object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelSubscriptionVirtualAddressOverwrite;
        if (parameters == nil || (parameters.length != 20 && parameters.length != 24)) {
            return nil;
        }
        UInt16 tem1=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem1, dataByte, 2);
        self.elementAddress = tem1;
        _virtualLabel = [CBUUID UUIDWithData:[parameters subdataWithRange:NSMakeRange(2, 16)]];
        if (parameters.length == 24) {
            memcpy(&tem1, dataByte+18, 2);
            self.companyIdentifier = tem1;
            memcpy(&tem1, dataByte+20, 2);
            self.modelIdentifier = tem1;
        } else {
            memcpy(&tem1, dataByte+18, 2);
            self.modelIdentifier = tem1;
            self.companyIdentifier = 0;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem = self.elementAddress;
    NSData *data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    data = _virtualLabel.data;
    [mData appendData:data];
    if (self.companyIdentifier) {
        tem = self.companyIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
        tem = self.modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    } else {
        tem = self.modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigModelSubscriptionStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.69 Config Network Transmit Get, opcode:0x8023


/// The Config Network Transmit Get is an acknowledged message used to get
/// the current Network Transmit state of a node (see Section 4.2.19).
/// @note   The response to a Config Network Transmit Get message is a
/// Config Network Transmit Status message.
/// @note   There are no Parameters for this message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.182), 4.3.2.69 Config Network Transmit Get.
@implementation SigConfigNetworkTransmitGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetworkTransmitGet;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigNetworkTransmitGet object.
 * @param   parameters    the hex data of SigConfigNetworkTransmitGet.
 * @return  return `nil` when initialize SigConfigNetworkTransmitGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetworkTransmitGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigNetworkTransmitStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.70 Config Network Transmit Set, opcode:0x8024


/// The Config Network Transmit Set is an acknowledged message used to set
/// the Network Transmit state of a node (see Section 4.2.19).
/// @note   The response to a Config Network Transmit Set message is a
/// Config Network Transmit Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.182), 4.3.2.70 Config Network Transmit Set.
@implementation SigConfigNetworkTransmitSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetworkTransmitSet;
        _count = 0;
        _steps = 0;
    }
    return self;
}

/// Sets the Network Transmit property of the Node.
///
/// - parameter count: Number of message transmissions of Network PDU
///                    originating from the Node. Possible values are 0...7,
///                    which correspond to 1-8 transmissions in total.
/// - parameter steps: Number of 10-millisecond steps between transmissions,
///                    decremented by 1. Possible values are 0...31, which
///                    corresponds to 10-320 milliseconds intervals.
- (instancetype)initWithCount:(UInt8)count steps:(UInt8)steps {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetworkTransmitSet;
        _count = MIN(7, count);
        _steps = MIN(63, steps);
    }
    return self;
}

/**
 * @brief   Initialize SigConfigNetworkTransmitSet object.
 * @param   parameters    the hex data of SigConfigNetworkTransmitSet.
 * @return  return `nil` when initialize SigConfigNetworkTransmitSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetworkTransmitSet;
        if (parameters == nil || parameters.length != 1) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        
        _count = tem & 0x07;
        _steps = tem >> 3;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    UInt8 tem = (_count & 0x07) | _steps << 3;
    return [NSData dataWithBytes:&tem length:1];
}

//- (NSTimeInterval)interval {
//    return (NSTimeInterval)(_steps+1)/100;
//}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigNetworkTransmitStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.71 Config Network Transmit Status, opcode:0x8025


/// The Config Network Transmit Status is an unacknowledged message used to report
/// the current Network Transmit state of a node (see Section 4.2.19).
/// @note   - seeAlso: Mesh_v1.0.pdf (page.183), 4.3.2.71 Config Network Transmit Status.
@implementation SigConfigNetworkTransmitStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetworkTransmitStatus;
        _count = 0;
        _steps = 0;
    }
    return self;
}

/// Sets the Network Transmit property of the Node.
///
/// - parameter count: Number of message transmissions of Network PDU
///                    originating from the Node. Possible values are 0...7,
///                    which correspond to 1-8 transmissions in total.
/// - parameter steps: Number of 10-millisecond steps between transmissions,
///                    decremented by 1. Possible values are 0...31, which
///                    corresponds to 10-320 milliseconds intervals.
- (instancetype)initWithCount:(UInt8)count steps:(UInt8)steps {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetworkTransmitSet;
        _count = MIN(7, count);
        _steps = MIN(63, steps);
    }
    return self;
}

/**
 * @brief   Initialize SigConfigNetworkTransmitStatus object.
 * @param   parameters    the hex data of SigConfigNetworkTransmitStatus.
 * @return  return `nil` when initialize SigConfigNetworkTransmitStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetworkTransmitStatus;
        if (parameters == nil || parameters.length != 1) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        
        _count = tem & 0x07;
        _steps = tem >> 3;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    UInt8 tem = (_count & 0x07) | _steps << 3;
    return [NSData dataWithBytes:&tem length:1];
}

- (NSTimeInterval)interval {
    return (NSTimeInterval)(_steps+1)/100;
}

- (instancetype)initWithNode:(SigNodeModel *)node {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetworkTransmitStatus;
        SigNetworktransmitModel *transmit = node.networkTransmit;
        if (!transmit) {
            _count = 0;
            _steps = 0;
        }else{
            _count = transmit.networkTransmitCount;
            _steps = transmit.networkTransmitIntervalSteps;
        }
    }
    return self;
}

@end


#pragma mark 4.3.2.12 Config Relay Get, opcode:0x8026


/// The Config Relay Get is an acknowledged message used to get the current
/// Relay (see Section 4.2.8) and Relay Retransmit (see Section 4.2.20) states of a node.
/// @note   The response to a Config Relay Get message is a
/// Config Relay Status message.
/// @note   There are no Parameters for this message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.156), 4.3.2.12 Config Relay Get.
@implementation SigConfigRelayGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configRelayGet;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigRelayGet object.
 * @param   parameters    the hex data of SigConfigRelayGet.
 * @return  return `nil` when initialize SigConfigRelayGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configRelayGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigRelayStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.13 Config Relay Set, opcode:0x8027


/// The Config Relay Set is an acknowledged message used to set
/// the Relay (see Section 4.2.8) and Relay Retransmit (see Section 4.2.20) states of a node.
/// @note   The response to a Config Relay Set message is a Config Relay Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.156), 4.3.2.13 Config Relay Set.
@implementation SigConfigRelaySet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configRelaySet;
        _state = SigNodeRelayState_notEnabled;
        _count = 0;
        _steps = 0;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigRelaySet object.
 * @param   parameters    the hex data of SigConfigRelaySet.
 * @return  return `nil` when initialize SigConfigRelaySet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configRelaySet;
        if (parameters == nil || parameters.length != 2) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte+1, 1);
        _count = tem & 0x07;
        _steps = tem >> 3;
        memcpy(&tem, dataByte, 1);
        _state = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    UInt8 tem[2] = {};
    tem[0] = _state;
    tem[1] = (_count & 0x07) | _steps << 3;
    return [NSData dataWithBytes:tem length:2];
}

- (NSTimeInterval)interval {
    return (NSTimeInterval)(_steps+1)/100;
}

- (instancetype)initWithCount:(UInt8)count steps:(UInt8)steps {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configRelaySet;
        _state = SigNodeRelayState_enabled;
        _count = MIN(7, count);
        _steps = MIN(63, steps);
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigRelayStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.14 Config Relay Status, opcode:0x8028


/// The Config Relay Status is an unacknowledged message used to report
/// the current Relay (see Section 4.2.8) and Relay Retransmit (see Section 4.2.20) states of a node.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.157), 4.3.2.14 Config Relay Status.
@implementation SigConfigRelayStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configRelayStatus;
        _count = 0;
        _steps = 0;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigRelayStatus object.
 * @param   parameters    the hex data of SigConfigRelayStatus.
 * @return  return `nil` when initialize SigConfigRelayStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configRelayStatus;
        if (parameters == nil || parameters.length != 2) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte+1, 1);
        _count = tem & 0x07;
        _steps = tem >> 3;
        memcpy(&tem, dataByte, 1);
        _state = tem;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    UInt8 tem[2] = {};
    tem[0] = _state;
    tem[1] = (_count & 0x07) | _steps << 3;
    return [NSData dataWithBytes:tem length:2];
}

- (NSTimeInterval)interval {
    return (NSTimeInterval)(_steps+1)/100;
}

- (instancetype)initWithState:(SigNodeFeaturesState)state count:(UInt8)count steps:(UInt8)steps {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _state = state;
        _count = MIN(7, count);
        _steps = MIN(63, steps);
    }
    return self;
}

@end


#pragma mark 4.3.2.27 Config SIG Model Subscription Get, opcode:0x8029


/// The Config SIG Model Subscription Get is an acknowledged message used to get
/// the list of subscription addresses of a model within the element. This message is only for SIG Models.
/// @note   The response to a Config SIG Model Subscription Get message is a
/// Config SIG Model Subscription List message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.165), 4.3.2.27 Config SIG Model Subscription Get.
@implementation SigConfigSIGModelSubscriptionGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configSIGModelSubscriptionGet;
    }
    return self;
}

- (instancetype)initWithElementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configSIGModelSubscriptionGet;
        if (companyIdentifier != 0) {
            // Use ConfigVendorModelSubscriptionGet instead.
            return nil;
        }
        self.elementAddress = elementAddress;
        self.modelIdentifier = modelIdentifier;
    }
    return self;
}

- (instancetype)initOfModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configSIGModelSubscriptionGet;
        if (model.getIntCompanyIdentifier != 0) {
            // Use ConfigVendorModelSubscriptionGet instead.
            return nil;
        }
        self.elementAddress = elementAddress;
        self.modelIdentifier = model.getIntModelIdentifier;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigSIGModelSubscriptionGet object.
 * @param   parameters    the hex data of SigConfigSIGModelSubscriptionGet.
 * @return  return `nil` when initialize SigConfigSIGModelSubscriptionGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configSIGModelSubscriptionGet;
        if (parameters == nil || parameters.length != 4) {
            return nil;
        }
        UInt16 tem1=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem1, dataByte, 2);
        self.elementAddress = tem1;
        memcpy(&tem1, dataByte+2, 2);
        self.modelIdentifier = tem1;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem = self.elementAddress;
    NSData *data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    tem = self.modelIdentifier;
    data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigSIGModelSubscriptionList class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.28 Config SIG Model Subscription List, opcode:0x802A


/// The Config SIG Model Subscription Get is an acknowledged message used to get
/// the list of subscription addresses of a model within the element. This message is only for SIG Models.
/// @note   The response to a Config SIG Model Subscription Get message is a
/// Config SIG Model Subscription List message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.165), 4.3.2.28 Config SIG Model Subscription List.
@implementation SigConfigSIGModelSubscriptionList

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configSIGModelSubscriptionList;
    }
    return self;
}

//- (instancetype)initForModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress addresses:(NSArray <NSNumber *>*)addresses withStatus:(SigConfigMessageStatus)status {
//    if (self = [super init]) {
//        self.opCode = SigOpCode_configSIGModelSubscriptionList;
//        if (model.getIntCompanyIdentifier != 0) {
//            // Use ConfigVendorModelSubscriptionList instead.
//            return nil;
//        }
//        self.elementAddress = elementAddress;
//        self.modelIdentifier = model.getIntModelIdentifier;
//        self.addresses = [NSMutableArray arrayWithArray:addresses];
//        self.status = status;
//    }
//    return self;
//}

/**
 * @brief   Initialize SigConfigSIGModelSubscriptionList object.
 * @param   parameters    the hex data of SigConfigSIGModelSubscriptionList.
 * @return  return `nil` when initialize SigConfigSIGModelSubscriptionList object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configSIGModelSubscriptionList;
        if (parameters == nil || parameters.length < 5) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        self.status = tem;
        UInt16 tem1=0;
        memcpy(&tem1, dataByte+1, 2);
        self.elementAddress = tem1;
        memcpy(&tem1, dataByte+3, 2);
        self.modelIdentifier = tem1;
        // Read list of addresses.
        NSMutableArray *array = [NSMutableArray array];
        
        for (int i=5; (i+1)<parameters.length; i += 2) {
            memcpy(&tem1, dataByte+i, 2);
            [array addObject:@(tem1)];
        }
        self.addresses = array;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 temStatus = self.status;
    NSData *statusData = [NSData dataWithBytes:&temStatus length:1];
    [mData appendData:statusData];
    UInt16 tem = self.elementAddress;
    NSData *data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    tem = self.modelIdentifier;
    data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    NSArray *addresses = [NSArray arrayWithArray:self.addresses];
    for (NSNumber *addressNumber in addresses) {
        UInt16 address = addressNumber.intValue;
        data = [NSData dataWithBytes:&address length:2];
        [mData appendData:data];
    }
    return mData;
}

@end


#pragma mark 4.3.2.29 Config Vendor Model Subscription Get, opcode:0x802B


/// The Config Vendor Model Subscription Get is an acknowledged message used to get
/// the list of subscription addresses of a model within the element. This message is only for Vendor Models.
/// @note   The response to a Config Vendor Model Subscription Get message is a
/// Config Vendor Model Subscription List message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.166), 4.3.2.29 Config Vendor Model Subscription Get.
@implementation SigConfigVendorModelSubscriptionGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configVendorModelSubscriptionGet;
    }
    return self;
}

- (instancetype)initWithElementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configVendorModelSubscriptionGet;
        if (companyIdentifier == 0) {
            // Use ConfigSIGModelSubscriptionGet instead.
            return nil;
        }
        self.elementAddress = elementAddress;
        self.modelIdentifier = modelIdentifier;
        self.companyIdentifier = companyIdentifier;
    }
    return self;
}

- (instancetype)initOfModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configVendorModelSubscriptionGet;
        if (model.getIntCompanyIdentifier == 0) {
            // Use ConfigSIGModelSubscriptionGet instead.
            return nil;
        }
        self.elementAddress = elementAddress;
        self.modelIdentifier = model.getIntModelIdentifier;
        self.companyIdentifier = model.getIntCompanyIdentifier;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigVendorModelSubscriptionGet object.
 * @param   parameters    the hex data of SigConfigVendorModelSubscriptionGet.
 * @return  return `nil` when initialize SigConfigVendorModelSubscriptionGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configVendorModelSubscriptionGet;
        if (parameters == nil || parameters.length != 6) {
            return nil;
        }
        UInt16 tem1=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem1, dataByte, 2);
        self.elementAddress = tem1;
        memcpy(&tem1, dataByte+2, 2);
        self.companyIdentifier = tem1;
        memcpy(&tem1, dataByte+4, 2);
        self.modelIdentifier = tem1;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem = self.elementAddress;
    NSData *data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    tem = self.companyIdentifier;
    data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    tem = self.modelIdentifier;
    data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigVendorModelSubscriptionList class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.30 Config Vendor Model Subscription List, opcode:0x802C


/// The Config Vendor Model Subscription List is an unacknowledged message used to report
/// all addresses from the Subscription List of the model (see Section 4.2.3). This message is only for Vendor Models.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.166), 4.3.2.30 Config Vendor Model Subscription List.
@implementation SigConfigVendorModelSubscriptionList

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configVendorModelSubscriptionList;
    }
    return self;
}

//- (instancetype)initForModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress addresses:(NSArray <NSNumber *>*)addresses withStatus:(SigConfigMessageStatus)status {
//    if (self = [super init]) {
//        self.opCode = SigOpCode_configVendorModelSubscriptionList;
//        if (model.getIntCompanyIdentifier == 0) {
//            // Use ConfigSIGModelSubscriptionList instead.
//            return nil;
//        }
//        self.elementAddress = elementAddress;
//        self.modelIdentifier = model.getIntModelIdentifier;
//        self.companyIdentifier = model.getIntCompanyIdentifier;
//        self.addresses = [NSMutableArray arrayWithArray:addresses];
//        self.status = status;
//    }
//    return self;
//}

/**
 * @brief   Initialize SigConfigVendorModelSubscriptionList object.
 * @param   parameters    the hex data of SigConfigVendorModelSubscriptionList.
 * @return  return `nil` when initialize SigConfigVendorModelSubscriptionList object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configVendorModelSubscriptionList;
        if (parameters == nil || parameters.length < 7) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        self.status = tem;
        UInt16 tem1=0;
        memcpy(&tem1, dataByte+1, 2);
        self.elementAddress = tem1;
        memcpy(&tem1, dataByte+3, 2);
        self.modelIdentifier = tem1;
        memcpy(&tem1, dataByte+5, 2);
        self.companyIdentifier = tem1;
        // Read list of addresses.
        NSMutableArray *array = [NSMutableArray array];
        
        for (int i=7; (i+1)<parameters.length; i += 2) {
            memcpy(&tem1, dataByte+i, 2);
            [array addObject:@(tem1)];
        }
        self.addresses = array;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 temStatus = self.status;
    NSData *statusData = [NSData dataWithBytes:&temStatus length:1];
    [mData appendData:statusData];
    UInt16 tem = self.elementAddress;
    NSData *data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    tem = self.companyIdentifier;
    data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    tem = self.modelIdentifier;
    data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    NSArray *addresses = [NSArray arrayWithArray:self.addresses];
    for (NSNumber *addressNumber in addresses) {
        UInt16 address = addressNumber.intValue;
        data = [NSData dataWithBytes:&address length:2];
        [mData appendData:data];
    }
    return mData;
}

@end


#pragma mark 4.3.2.67 Config Low Power Node PollTimeout Get, opcode:0x802D


/// The Config Low Power Node PollTimeout Get is an acknowledged message used to get
/// the current value of PollTimeout timer of the Low Power node within a Friend node (see Section 3.6.6.1).
/// The message is sent to a Friend node that has claimed to be handling messages by sending
/// ACKs On Behalf Of (OBO) the indicated Low Power node. This message should only be sent
/// to a node that has the Friend feature supported and enabled.
/// @note   The response to a Config Low Power Node PollTimeout Get message is a
/// Config Low Power Node PollTimeout Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.181), 4.3.2.67 Config Low Power Node PollTimeout Get.
@implementation SigConfigLowPowerNodePollTimeoutGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configLowPowerNodePollTimeoutGet;
    }
    return self;
}

- (instancetype)initWithLPNAddress:(UInt16)LPNAddress {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configKeyRefreshPhaseGet;
        _LPNAddress = LPNAddress;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigLowPowerNodePollTimeoutGet object.
 * @param   parameters    the hex data of SigConfigLowPowerNodePollTimeoutGet.
 * @return  return `nil` when initialize SigConfigLowPowerNodePollTimeoutGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configKeyRefreshPhaseGet;
        if (parameters == nil || parameters.length != 2) {
            return nil;
        }
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _LPNAddress = tem16;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _LPNAddress;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigLowPowerNodePollTimeoutStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.68 Config Low Power Node PollTimeout Status, opcode:0x802E


/// The Config Low Power Node PollTimeout Status is an unacknowledged message used to report
/// the current value of the PollTimeout timer of the Low Power node within a Friend node.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.181), 4.3.2.68 Config Low Power Node PollTimeout Status.
@implementation SigConfigLowPowerNodePollTimeoutStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configLowPowerNodePollTimeoutStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigLowPowerNodePollTimeoutStatus object.
 * @param   parameters    the hex data of SigConfigLowPowerNodePollTimeoutStatus.
 * @return  return `nil` when initialize SigConfigLowPowerNodePollTimeoutStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configLowPowerNodePollTimeoutStatus;
        if (parameters == nil || parameters.length != 5) {
            return nil;
        }
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _LPNAddress = tem16;
        UInt32 tem1 = 0,tem2 = 0,tem3 = 0;
        memcpy(&tem1, dataByte+2, 1);
        memcpy(&tem2, dataByte+3, 1);
        memcpy(&tem3, dataByte+4, 1);
        _pollTimeout = tem1 << 16 | tem2 << 8 | tem3;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _LPNAddress;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt32 tem32 = CFSwapInt32HostToBig(_pollTimeout);
    NSData *temData = [NSData dataWithBytes:&tem32 length:4];
    [mData appendData:[temData subdataWithRange:NSMakeRange(1, 3)]];
    return mData;
}

@end


#pragma mark 4.3.2.61 Config Heartbeat Publication Get, opcode:0x8038


/// The Config Heartbeat Publication Get is an acknowledged message used to get
/// the current Heartbeat Publication state of an element (see Section 4.2.17).
/// @note   The response to a Config Heartbeat Publication Get message is a
/// Config Heartbeat Publication Status message.
/// @note   The message has no parameters.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.178), 4.3.2.61 Config Heartbeat Publication Get.
@implementation SigConfigHeartbeatPublicationGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configHeartbeatPublicationGet;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigHeartbeatPublicationGet object.
 * @param   parameters    the hex data of SigConfigHeartbeatPublicationGet.
 * @return  return `nil` when initialize SigConfigHeartbeatPublicationGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configHeartbeatPublicationGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigHeartbeatPublicationStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.62 Config Heartbeat Publication Set, opcode:0x8039


/// The Config Heartbeat Publication Set is an acknowledged message used to set
/// the current Heartbeat Publication state of an element (see Section 4.2.17).
/// @note   The response to a Config Heartbeat Publication Set message is a
/// Config Heartbeat Publication Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.179), 4.3.2.62 Config Heartbeat Publication Set.
@implementation SigConfigHeartbeatPublicationSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configHeartbeatPublicationSet;
    }
    return self;
}

- (instancetype)initWithDestination:(UInt16)destination countLog:(UInt8)countLog periodLog:(UInt8)periodLog ttl:(UInt8)ttl features:(SigFeatures)features netKeyIndex:(UInt16)netKeyIndex {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configHeartbeatPublicationSet;
        _destination = destination;
        _countLog = countLog;
        _periodLog = periodLog;
        _ttl = ttl;
        _features = features;
        _netKeyIndex = netKeyIndex;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigHeartbeatPublicationSet object.
 * @param   parameters    the hex data of SigConfigHeartbeatPublicationSet.
 * @return  return `nil` when initialize SigConfigHeartbeatPublicationSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configHeartbeatPublicationSet;
        if (parameters == nil || parameters.length != 9) {
            return nil;
        }
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _destination = tem16;
        UInt8 tem8 = 0;
        memcpy(&tem8, dataByte+2, 1);
        _countLog = tem8;
        memcpy(&tem8, dataByte+3, 1);
        _periodLog = tem8;
        memcpy(&tem8, dataByte+4, 1);
        _ttl = tem8;
        memcpy(&tem16, dataByte+5, 2);
        _features.value = tem16;
        memcpy(&tem16, dataByte+7, 2);
        _netKeyIndex = (tem16 >> 4) & 0xFFF;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _destination;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = _countLog;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _periodLog;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _ttl;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem16 = _features.value;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _netKeyIndex << 4;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigHeartbeatPublicationStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.63 Config Heartbeat Publication Status, opcode:0x6


/// The Config Heartbeat Publication Status is an unacknowledged message used to report
/// the Heartbeat Publication state of a node (see Section 4.2.17).
/// @note   - seeAlso: Mesh_v1.0.pdf (page.179), 4.3.2.63 Config Heartbeat Publication Status.
@implementation SigConfigHeartbeatPublicationStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configHeartbeatPublicationStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigHeartbeatPublicationStatus object.
 * @param   parameters    the hex data of SigConfigHeartbeatPublicationStatus.
 * @return  return `nil` when initialize SigConfigHeartbeatPublicationStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configHeartbeatPublicationStatus;
        if (parameters == nil || parameters.length != 10) {
            return nil;
        }
        UInt8 tem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _status = tem8;
        UInt16 tem16 = 0;
        memcpy(&tem16, dataByte+1, 2);
        _destination = tem16;
        memcpy(&tem8, dataByte+3, 1);
        _countLog = tem8;
        memcpy(&tem8, dataByte+4, 1);
        _periodLog = tem8;
        memcpy(&tem8, dataByte+5, 1);
        _ttl = tem8;
        memcpy(&tem16, dataByte+6, 2);
        _features.value = tem16;
        memcpy(&tem16, dataByte+8, 2);
        _netKeyIndex = (tem16 >> 4) & 0xFFF;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _status;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    UInt16 tem16 = _destination;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem8 = _countLog;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _periodLog;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _ttl;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem16 = _features.value;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _netKeyIndex << 4;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 4.3.2.64 Config Heartbeat Subscription Get, opcode:0x803A


/// The Config Heartbeat Subscription Get is an acknowledged message used to get
/// the current Heartbeat Subscription state of an element (see Section 4.2.18).
/// @note   The response to a Config Heartbeat Subscription Get message is a
/// Config Heartbeat Subscription Status message.
/// @note   The message has no parameters.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.180), 4.3.2.64 Config Heartbeat Subscription Get.
@implementation SigConfigHeartbeatSubscriptionGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configHeartbeatSubscriptionGet;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigHeartbeatSubscriptionGet object.
 * @param   parameters    the hex data of SigConfigHeartbeatSubscriptionGet.
 * @return  return `nil` when initialize SigConfigHeartbeatSubscriptionGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configHeartbeatSubscriptionGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigHeartbeatSubscriptionStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.65 Config Heartbeat Subscription Set, opcode:0x803B


/// The Config Heartbeat Subscription Set is an acknowledged message used to set
/// the current Heartbeat Subscription state of an element (see Section 4.2.18).
/// @note   The response to a Config Heartbeat Subscription Set message is a
/// Config Heartbeat Subscription Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.180), 4.3.2.65 Config Heartbeat Subscription Set.
@implementation SigConfigHeartbeatSubscriptionSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configHeartbeatSubscriptionSet;
    }
    return self;
}

- (instancetype)initWithSource:(UInt16)source destination:(UInt16)destination periodLog:(UInt8)periodLog {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configHeartbeatSubscriptionSet;
        _source = source;
        _destination = destination;
        _periodLog = periodLog;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigHeartbeatSubscriptionSet object.
 * @param   parameters    the hex data of SigConfigHeartbeatSubscriptionSet.
 * @return  return `nil` when initialize SigConfigHeartbeatSubscriptionSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configHeartbeatSubscriptionSet;
        if (parameters == nil || parameters.length != 5) {
            return nil;
        }
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _source = tem16;
        memcpy(&tem16, dataByte+2, 2);
        _destination = tem16;
        UInt8 tem8 = 0;
        memcpy(&tem8, dataByte+4, 1);
        _periodLog = tem8;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _source;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _destination;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = _periodLog;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigHeartbeatSubscriptionStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.66 Config Heartbeat Subscription Status, opcode:0x803C


/// The Config Heartbeat Subscription Status is an unacknowledged message used to report
/// the Heartbeat Subscription state of a node (see Section 4.2.18).
/// @note   - seeAlso: Mesh_v1.0.pdf (page.181), 4.3.2.66 Config Heartbeat Subscription Status.
@implementation SigConfigHeartbeatSubscriptionStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configHeartbeatSubscriptionStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigHeartbeatSubscriptionStatus object.
 * @param   parameters    the hex data of SigConfigHeartbeatSubscriptionStatus.
 * @return  return `nil` when initialize SigConfigHeartbeatSubscriptionStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configHeartbeatSubscriptionStatus;
        if (parameters == nil || parameters.length != 9) {
            return nil;
        }
        UInt8 tem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _status = tem8;
        UInt16 tem16 = 0;
        memcpy(&tem16, dataByte+1, 2);
        _source = tem16;
        memcpy(&tem16, dataByte+3, 2);
        _destination = tem16;
        memcpy(&tem8, dataByte+5, 1);
        _periodLog = tem8;
        memcpy(&tem8, dataByte+6, 1);
        _countLog = tem8;
        memcpy(&tem8, dataByte+7, 1);
        _minHops = tem8;
        memcpy(&tem8, dataByte+8, 1);
        _maxHops = tem8;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _status;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    UInt16 tem16 = _source;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = _destination;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem8 = _periodLog;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _countLog;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _minHops;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _maxHops;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 4.3.2.46 Config Model App Bind, opcode:0x803D


/// The Config Model App Bind is an acknowledged message used to
/// bind an AppKey to a model.
/// @note   The response to a Config Model App Bind message is a
/// Config Model App Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.173), 4.3.2.46 Config Model App Bind.
@implementation SigConfigModelAppBind

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelAppBind;
    }
    return self;
}

- (instancetype)initWithApplicationKeyIndex:(UInt16)applicationKeyIndex elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelAppBind;
        self.applicationKeyIndex = applicationKeyIndex;
        _elementAddress = elementAddress;
        _modelIdentifier = modelIdentifier;
        _companyIdentifier = companyIdentifier;
        _isVendorModelID = companyIdentifier != 0;
    }
    return self;
}

- (instancetype)initWithApplicationKeyIndex:(UInt16)applicationKeyIndex elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier isVendorModelID:(BOOL)isVendorModelID {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelAppBind;
        self.applicationKeyIndex = applicationKeyIndex;
        _elementAddress = elementAddress;
        _modelIdentifier = modelIdentifier;
        _companyIdentifier = companyIdentifier;
        _isVendorModelID = isVendorModelID;
    }
    return self;
}

- (instancetype)initWithApplicationKey:(SigAppkeyModel *)applicationKey toModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelAppBind;
        self.applicationKeyIndex = applicationKey.index;
        _elementAddress = elementAddress;
        _modelIdentifier = model.getIntModelIdentifier;
        _companyIdentifier = model.getIntCompanyIdentifier;
        _isVendorModelID = model.isVendorModelID;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigModelAppBind object.
 * @param   parameters    the hex data of SigConfigModelAppBind.
 * @return  return `nil` when initialize SigConfigModelAppBind object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelAppBind;
        if (parameters == nil || (parameters.length != 6 && parameters.length != 8)) {
            return nil;
        }
        UInt16 tem1=0,tem2=0,tem3=0,tem4=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem1, dataByte, 2);
        memcpy(&tem2, dataByte+2, 2);
        _elementAddress = tem1;
        self.applicationKeyIndex = tem2;
        if (parameters.length == 8) {
            memcpy(&tem3, dataByte+4, 2);
            memcpy(&tem4, dataByte+6, 2);
            _companyIdentifier = tem3;
            _modelIdentifier = tem4;
            _isVendorModelID = YES;
        }else{
            memcpy(&tem3, dataByte+4, 2);
            _companyIdentifier = 0;
            _modelIdentifier = tem3;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem = _elementAddress;
    NSData *data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    tem = self.applicationKeyIndex;
    data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    if (_isVendorModelID) {
        tem = _companyIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
        tem = _modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    } else {
        tem = _modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigModelAppStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

- (BOOL)isSegmented {
    return NO;
}

@end


#pragma mark 4.3.2.48 Config Model App Status, opcode:0x803E


/// The Config Model App Status is an unacknowledged message used to report
/// a status for the requesting message, based on the element address,
/// the AppKeyIndex identifying the AppKey on the AppKey List, and the ModelIdentifier.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.173), 4.3.2.48 Config Model App Status.
@implementation SigConfigModelAppStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelAppStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigModelAppStatus object.
 * @param   parameters    the hex data of SigConfigModelAppStatus.
 * @return  return `nil` when initialize SigConfigModelAppStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelAppStatus;
        if (parameters == nil || (parameters.length != 7 && parameters.length != 9)) {
            return nil;
        }
        UInt8 status=0;
        UInt16 tem1=0,tem2=0,tem3=0,tem4=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&status, dataByte, 1);
        memcpy(&tem1, dataByte+1, 2);
        memcpy(&tem2, dataByte+3, 2);
        _status = status;
        _elementAddress = tem1;
        self.applicationKeyIndex = tem2;
        if (parameters.length == 9) {
            _isVendorModelID = YES;
            memcpy(&tem3, dataByte+5, 2);
            memcpy(&tem4, dataByte+7, 2);
            _companyIdentifier = tem3;
            _modelIdentifier = tem4;
        }else{
            _isVendorModelID = NO;
            memcpy(&tem3, dataByte+5, 2);
            _companyIdentifier = 0;
            _modelIdentifier = tem3;
        }
    }
    return self;
}

- (instancetype)initWithStatus:(SigConfigMessageStatus)status applicationKey:(SigAppkeyModel *)applicationKey toModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelAppBind;
        self.applicationKeyIndex = applicationKey.index;
        _elementAddress = elementAddress;
        _modelIdentifier = model.getIntModelIdentifier;
        _companyIdentifier = model.getIntCompanyIdentifier;
        _isVendorModelID = model.isVendorModelID;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _status;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    UInt16 tem = _elementAddress;
    data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    tem = self.applicationKeyIndex;
    data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    if (_isVendorModelID) {
        tem = _companyIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
        tem = _modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    } else {
        tem = _modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    }
    return mData;
}

@end


#pragma mark 4.3.2.47 Config Model App Unbind, opcode:0x803F


/// The Config Model App Unbind is an acknowledged message used to remove
/// the binding between an AppKey and a model.
/// @note   The response to a Config Model App Unbind message is a
/// Config Model App Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.173), 4.3.2.47 Config Model App Unbind.
@implementation SigConfigModelAppUnbind

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelAppUnbind;
    }
    return self;
}

- (instancetype)initWithApplicationKeyIndex:(UInt16)applicationKeyIndex elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelAppUnbind;
        self.applicationKeyIndex = applicationKeyIndex;
        _elementAddress = elementAddress;
        _modelIdentifier = modelIdentifier;
        _companyIdentifier = companyIdentifier;
        _isVendorModelID = companyIdentifier != 0;
    }
    return self;
}

- (instancetype)initWithApplicationKeyIndex:(UInt16)applicationKeyIndex elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier isVendorModelID:(BOOL)isVendorModelID {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelAppUnbind;
        self.applicationKeyIndex = applicationKeyIndex;
        _elementAddress = elementAddress;
        _modelIdentifier = modelIdentifier;
        _companyIdentifier = companyIdentifier;
        _isVendorModelID = isVendorModelID;
    }
    return self;
}

- (instancetype)initWithApplicationKey:(SigAppkeyModel *)applicationKey toModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelAppUnbind;
        self.applicationKeyIndex = applicationKey.index;
        _elementAddress = elementAddress;
        _modelIdentifier = model.getIntModelIdentifier;
        _companyIdentifier = model.getIntCompanyIdentifier;
        _isVendorModelID = model.isVendorModelID;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigModelAppUnbind object.
 * @param   parameters    the hex data of SigConfigModelAppUnbind.
 * @return  return `nil` when initialize SigConfigModelAppUnbind object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configModelAppUnbind;
        if (parameters == nil || (parameters.length != 6 && parameters.length != 8)) {
            return nil;
        }
        UInt16 tem1=0,tem2=0,tem3=0,tem4=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem1, dataByte, 2);
        memcpy(&tem2, dataByte+2, 2);
        _elementAddress = tem1;
        self.applicationKeyIndex = tem2;
        if (parameters.length == 8) {
            _isVendorModelID = YES;
            memcpy(&tem3, dataByte+4, 2);
            memcpy(&tem4, dataByte+6, 2);
            _companyIdentifier = tem3;
            _modelIdentifier = tem4;
        }else{
            _isVendorModelID = NO;
            memcpy(&tem3, dataByte+4, 2);
            _companyIdentifier = 0;
            _modelIdentifier = tem3;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem = _elementAddress;
    NSData *data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    tem = self.applicationKeyIndex;
    data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    if (_isVendorModelID) {
        tem = _companyIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
        tem = _modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    } else {
        tem = _modelIdentifier;
        data = [NSData dataWithBytes:&tem length:2];
        [mData appendData:data];
    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigModelAppStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.31 Config NetKey Add, opcode:0x8040


/// The Config NetKey Add is an acknowledged message used to add
/// a NetKey to a NetKey List (see Section 4.2.4) on a node. The added
/// NetKey is then used by the node to authenticate and decrypt messages
/// it receives, as well as authenticate and encrypt messages it sends.
/// @note   The response to a Config NetKey Add message is a
/// Config NetKey Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.167), 4.3.2.31 Config NetKey Add.
@implementation SigConfigNetKeyAdd

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetKeyAdd;
    }
    return self;
}

- (instancetype)initWithNetworkKeyIndex:(UInt16)networkKeyIndex networkKeyData:(NSData *)networkKeyData {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetKeyAdd;
        self.networkKeyIndex = networkKeyIndex;
        _key = networkKeyData;
    }
    return self;
}

- (instancetype)initWithNetworkKey:(SigNetkeyModel *)networkKey {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetKeyAdd;
        self.networkKeyIndex = networkKey.index;
        _key = [LibTools nsstringToHex:networkKey.key];
    }
    return self;
}

/**
 * @brief   Initialize SigConfigNetKeyAdd object.
 * @param   parameters    the hex data of SigConfigNetKeyAdd.
 * @return  return `nil` when initialize SigConfigNetKeyAdd object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetKeyAdd;
        if (parameters == nil || parameters.length != 18) {
            return nil;
        }
        self.networkKeyIndex = [SigConfigNetKeyMessage decodeNetKeyIndexFromData:parameters atOffset:0];
        _key = [parameters subdataWithRange:NSMakeRange(2, 16)];
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    NSData *data = [self encodeNetKeyIndex];
    [mData appendData:data];
    [mData appendData:_key];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigNetKeyStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.33 Config NetKey Delete, opcode:0x8041


/// The Config NetKey Delete is an acknowledged message used to delete
/// a NetKey on a NetKey List from a node.
/// @note   The response to a Config NetKey Delete message is a
/// Config NetKey Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.168), 4.3.2.33 Config NetKey Delete.
@implementation SigConfigNetKeyDelete

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetKeyDelete;
    }
    return self;
}

- (instancetype)initWithNetworkKeyIndex:(UInt16)networkKeyIndex {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetKeyDelete;
        self.networkKeyIndex = networkKeyIndex;
    }
    return self;
}

- (instancetype)initWithNetworkKey:(SigNetkeyModel *)networkKey {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetKeyDelete;
        self.networkKeyIndex = networkKey.index;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigNetKeyDelete object.
 * @param   parameters    the hex data of SigConfigNetKeyDelete.
 * @return  return `nil` when initialize SigConfigNetKeyDelete object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetKeyDelete;
        if (parameters == nil || parameters.length != 18) {
            return nil;
        }
        self.networkKeyIndex = [SigConfigNetKeyMessage decodeNetKeyIndexFromData:parameters atOffset:0];
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData dataWithData:[self encodeNetKeyIndex]];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigNetKeyStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.35 Config NetKey Get, opcode:0x8042


/// The Config NetKey Get is an acknowledged message used to report
/// all NetKeys known to the node.
/// @note   The response to a Config NetKey Get message is a
/// Config NetKey List message.
/// @note   There are no Parameters for this message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.169), 4.3.2.35 Config NetKey Get.
@implementation SigConfigNetKeyGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetKeyGet;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigNetKeyGet object.
 * @param   parameters    the hex data of SigConfigNetKeyGet.
 * @return  return `nil` when initialize SigConfigNetKeyGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetKeyGet;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigNetKeyList class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.36 Config NetKey List, opcode:0x8043


/// The Config NetKey List is an unacknowledged message reporting
/// all NetKeys known to the node.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.169), 4.3.2.36 Config NetKey List.
@implementation SigConfigNetKeyList

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetKeyList;
    }
    return self;
}

//- (instancetype)initWithNetworkKeys:(NSArray <SigNetkeyModel *>*)networkKeys {
//    if (self = [super init]) {
//        self.opCode = SigOpCode_configNetKeyList;
//        _networkKeyIndexes = [NSMutableArray array];
//        for (SigNetkeyModel *model in networkKeys) {
//            [_networkKeyIndexes addObject:@(model.index)];
//        }
//    }
//    return self;
//}

/**
 * @brief   Initialize SigConfigNetKeyList object.
 * @param   parameters    the hex data of SigConfigNetKeyList.
 * @return  return `nil` when initialize SigConfigNetKeyList object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetKeyList;
        if (parameters == nil || parameters.length == 0) {
            return nil;
        }
        _networkKeyIndexes = [NSMutableArray arrayWithArray:[SigConfigNetKeyMessage decodeIndexesFromData:parameters atOffset:0]];
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return [self encodeIndexes:_networkKeyIndexes];
}

@end


#pragma mark 4.3.2.34 Config NetKey Status, opcode:0x8044


/// The Config NetKey Status is an unacknowledged message used to report
/// the status of the operation on the NetKey List.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.168), 4.3.2.34 Config NetKey Status.
@implementation SigConfigNetKeyStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetKeyStatus;
    }
    return self;
}

//- (instancetype)initWithNetworkKey:(SigNetkeyModel *)networkKey {
//    if (self = [super init]) {
//        self.opCode = SigOpCode_configNetKeyStatus;
//        self.networkKeyIndex = networkKey.index;
//        _status = SigConfigMessageStatus_success;
//
//    }
//    return self;
//}
//
//- (instancetype)initWithStatus:(SigConfigMessageStatus)status forMessage:(SigConfigNetKeyMessage *)message {
//    if (self = [super init]) {
//        self.opCode = SigOpCode_configNetKeyStatus;
//        self.networkKeyIndex = message.networkKeyIndex;
//        _status = status;
//    }
//    return self;
//}

/**
 * @brief   Initialize SigConfigNetKeyStatus object.
 * @param   parameters    the hex data of SigConfigNetKeyStatus.
 * @return  return `nil` when initialize SigConfigNetKeyStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetKeyStatus;
        if (parameters == nil || parameters.length != 3) {
            return nil;
        }
        UInt8 tem = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _status = tem;
        self.networkKeyIndex = [SigConfigNetKeyMessage decodeNetKeyIndexFromData:parameters atOffset:1];
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _status;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    data = [self encodeNetKeyIndex];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 4.3.2.32 Config NetKey Update, opcode:0x8045


/// The Config NetKey Update is an acknowledged message used to update
/// a NetKey on a node. The updated NetKey is then used by the node to
/// authenticate and decrypt messages it receives, as well as authenticate and
/// encrypt messages it sends, as defined by the Key Refresh procedure (see Section 3.10.4).
/// @note   The response to a Config NetKey Update message is a
/// Config NetKey Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.167), 4.3.2.32 Config NetKey Update.
@implementation SigConfigNetKeyUpdate

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetKeyUpdate;
    }
    return self;
}

- (instancetype)initWithNetworkKeyIndex:(UInt16)networkKeyIndex networkKeyData:(NSData *)networkKeyData {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetKeyUpdate;
        self.networkKeyIndex = networkKeyIndex;
        _key = networkKeyData;
    }
    return self;
}

- (instancetype)initWithNetworkKey:(SigNetkeyModel *)networkKey {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetKeyUpdate;
        self.networkKeyIndex = networkKey.index;
        _key = [LibTools nsstringToHex:networkKey.key];
    }
    return self;
}

/**
 * @brief   Initialize SigConfigNetKeyUpdate object.
 * @param   parameters    the hex data of SigConfigNetKeyUpdate.
 * @return  return `nil` when initialize SigConfigNetKeyUpdate object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNetKeyUpdate;
        if (parameters == nil || parameters.length != 18) {
            return nil;
        }
        self.networkKeyIndex = [SigConfigNetKeyMessage decodeNetKeyIndexFromData:parameters atOffset:0];
        _key = [parameters subdataWithRange:NSMakeRange(2, 16)];
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    NSData *data = [self encodeNetKeyIndex];
    [mData appendData:data];
    [mData appendData:_key];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigNetKeyStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.43 Config Node Identity Get, opcode:0x8046


/// The Config Node Identity Get is an acknowledged message used to get
/// the current Node Identity state for a subnet (see Section 4.2.12).
/// @note   The response to a Config Node Identity Get message is a
/// Config Node Identity Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.171), 4.3.2.43 Config Node Identity Get.
@implementation SigConfigNodeIdentityGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNodeIdentityGet;
    }
    return self;
}

- (instancetype)initWithNetKeyIndex:(UInt16)netKeyIndex {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNodeIdentityGet;
        _netKeyIndex = netKeyIndex;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigNodeIdentityGet object.
 * @param   parameters    the hex data of SigConfigNodeIdentityGet.
 * @return  return `nil` when initialize SigConfigNodeIdentityGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNodeIdentityGet;
        if (parameters == nil || parameters.length != 2) {
            return nil;
        }
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _netKeyIndex = (tem16 >> 4) & 0xFFF;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _netKeyIndex << 4;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigNodeIdentityStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.44 Config Node Identity Set, opcode:0x8047


/// The Config Node Identity Set is an acknowledged message used to set
/// the current Node Identity state for a subnet (see Section 4.2.12).
/// @note   The response to a Config Node Identity Set message is a
/// Config Node Identity Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.172), 4.3.2.44 Config Node Identity Set.
@implementation SigConfigNodeIdentitySet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNodeIdentitySet;
    }
    return self;
}

- (instancetype)initWithNetKeyIndex:(UInt16)netKeyIndex identity:(SigNodeIdentityState)identity {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNodeIdentitySet;
        _netKeyIndex = netKeyIndex;
        _identity = identity;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigNodeIdentitySet object.
 * @param   parameters    the hex data of SigConfigNodeIdentitySet.
 * @return  return `nil` when initialize SigConfigNodeIdentitySet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNodeIdentitySet;
        if (parameters == nil || parameters.length != 3) {
            return nil;
        }
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _netKeyIndex = (tem16 >> 4) & 0xFFF;
        UInt8 tem8 = 0;
        memcpy(&tem8, dataByte+2, 1);
        _identity = tem8;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _netKeyIndex << 4;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = _identity;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigNodeIdentityStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.45 Config Node Identity Status, opcode:0x8048


/// The Config Node Identity Status is an unacknowledged message used to report
/// the current Node Identity state for a subnet (see Section 4.2.12).
/// @note   - seeAlso: Mesh_v1.0.pdf (page.172), 4.3.2.45 Config Node Identity Status.
@implementation SigConfigNodeIdentityStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNodeIdentityStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigNodeIdentityStatus object.
 * @param   parameters    the hex data of SigConfigNodeIdentityStatus.
 * @return  return `nil` when initialize SigConfigNodeIdentityStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNodeIdentityStatus;
        if (parameters == nil || parameters.length != 4) {
            return nil;
        }
        Byte *dataByte = (Byte *)parameters.bytes;
        UInt8 tem8 = 0;
        memcpy(&tem8, dataByte, 1);
        _status = tem8;
        UInt16 tem16 = 0;
        memcpy(&tem16, dataByte+1, 2);
        _netKeyIndex = (tem16 >> 4) & 0xFFF;
        memcpy(&tem8, dataByte+3, 1);
        _identity = tem8;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _status;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    UInt16 tem16 = _netKeyIndex << 4;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem8 = _identity;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 4.3.2.53 Config Node Reset, opcode:0x8049


/// The Config Node Reset is an acknowledged message used to reset
/// a node (other than a Provisioner) and remove it from the network.
/// @note   The response to a Config Node Reset message is a
/// Config Node Reset Status message.
/// @note   There are no Parameters for this message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.176), 4.3.2.53 Config Node Reset.
@implementation SigConfigNodeReset

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNodeReset;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigNodeReset object.
 * @param   parameters    the hex data of SigConfigNodeReset.
 * @return  return `nil` when initialize SigConfigNodeReset object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNodeReset;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigNodeResetStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.54 Config Node Reset Status, opcode:0x804A


/// The Config Node Reset Status is an unacknowledged message used to
/// acknowledge that an element has received a Config Node Reset message.
/// @note   There are no Parameters for this message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.176), 4.3.2.54 Config Node Reset Status.
@implementation SigConfigNodeResetStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNodeResetStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigConfigNodeResetStatus object.
 * @param   parameters    the hex data of SigConfigNodeResetStatus.
 * @return  return `nil` when initialize SigConfigNodeResetStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configNodeResetStatus;
        if (parameters == nil || parameters.length == 0) {
            return self;
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

@end


#pragma mark 4.3.2.49 Config SIG Model App Get, opcode:0x804B


/// The Config SIG Model App Get is an acknowledged message used to request
/// report of all AppKeys bound to the SIG Model.
/// @note   The response to a Config SIG Model App Get message is a
/// Config SIG Model App List message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.174), 4.3.2.49 Config SIG Model App Get.
@implementation SigConfigSIGModelAppGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configSIGModelAppGet;
    }
    return self;
}

- (instancetype)initWithElementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configSIGModelAppGet;
        if (companyIdentifier == 0) {
            self.elementAddress = elementAddress;
            self.modelIdentifier = modelIdentifier;
        } else {
            // Use ConfigVendorModelAppGet instead.
            return nil;
        }
    }
    return self;
}

- (instancetype)initWithModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configSIGModelAppGet;
        if (model.getIntCompanyIdentifier == 0) {
            self.elementAddress = elementAddress;
            self.modelIdentifier = model.getIntModelIdentifier;
        } else {
            // Use ConfigVendorModelAppGet instead.
            return nil;
        }
    }
    return self;
}

/**
 * @brief   Initialize SigConfigSIGModelAppGet object.
 * @param   parameters    the hex data of SigConfigSIGModelAppGet.
 * @return  return `nil` when initialize SigConfigSIGModelAppGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configSIGModelAppGet;
        if (parameters == nil || parameters.length != 4) {
            return nil;
        }
        UInt16 tem1=0,tem2=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem1, dataByte, 2);
        memcpy(&tem2, dataByte+2, 2);
        self.elementAddress = tem1;
        self.modelIdentifier = tem2;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem = self.elementAddress;
    NSData *data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    tem = self.modelIdentifier;
    data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigSIGModelAppList class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.50 Config SIG Model App List, opcode:0x804C


/// The Config SIG Model App List is an unacknowledged message used to report
/// all AppKeys bound to the SIG Model.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.174), 4.3.2.50 Config SIG Model App List.
@implementation SigConfigSIGModelAppList

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configSIGModelAppList;
    }
    return self;
}

//- (instancetype)initResponseToSigConfigSIGModelAppGet:(SigConfigSIGModelAppGet *)request withApplicationKeys:(NSArray <SigAppkeyModel *>*)applicationKeys {
//    if (self = [super init]) {
//        self.opCode = SigOpCode_configSIGModelAppList;
//        _elementAddress = request.elementAddress;
//        self.modelIdentifier = request.modelIdentifier;
//        self.applicationKeyIndexes = [NSMutableArray array];
//        for (SigAppkeyModel *model in applicationKeys) {
//            [self.applicationKeyIndexes addObject:@(model.index)];
//        }
//        self.status = SigConfigMessageStatus_success;
//    }
//    return self;
//}
//
//- (instancetype)initResponseToSigConfigSIGModelAppGet:(SigConfigSIGModelAppGet *)request withStatus:(SigConfigMessageStatus)status {
//    if (self = [super init]) {
//        self.opCode = SigOpCode_configSIGModelAppList;
//        _elementAddress = request.elementAddress;
//        self.modelIdentifier = request.modelIdentifier;
//        self.applicationKeyIndexes = [NSMutableArray array];
//        self.status = status;
//    }
//    return self;
//}

/**
 * @brief   Initialize SigConfigSIGModelAppList object.
 * @param   parameters    the hex data of SigConfigSIGModelAppList.
 * @return  return `nil` when initialize SigConfigSIGModelAppList object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configSIGModelAppList;
        if (parameters == nil || parameters.length < 5) {
            return nil;
        }
        UInt8 status=0;
        UInt16 tem1=0,tem2=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&status, dataByte, 1);
        memcpy(&tem1, dataByte+1, 2);
        memcpy(&tem2, dataByte+3, 2);
        self.status = status;
        _elementAddress = tem1;
        self.modelIdentifier = tem2;
        self.applicationKeyIndexes = [NSMutableArray arrayWithArray:[SigConfigNetKeyMessage decodeIndexesFromData:parameters atOffset:5]];
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = self.status;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    UInt16 tem16 = _elementAddress;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = self.modelIdentifier;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    data = [self encodeIndexes:self.applicationKeyIndexes];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 4.3.2.51 Config Vendor Model App Get, opcode:0x804D


/// The Config Vendor Model App Get is an acknowledged message used to request
/// report of all AppKeys bound to the model. This message is only for Vendor Models.
/// @note   The response to a Config Vendor Model App Get message is a
/// Config Vendor Model App List message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.175), 4.3.2.51 Config Vendor Model App Get.
@implementation SigConfigVendorModelAppGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configVendorModelAppGet;
    }
    return self;
}

- (instancetype)initWithElementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configVendorModelAppGet;
        if (companyIdentifier == 0) {
            self.elementAddress = elementAddress;
            self.modelIdentifier = modelIdentifier;
            self.companyIdentifier = companyIdentifier;

        } else {
            return nil;
        }
    }
    return self;
}

- (instancetype)initWithModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configVendorModelAppGet;
        if (model.getIntCompanyIdentifier == 0) {
            self.elementAddress = elementAddress;
            self.modelIdentifier = model.getIntModelIdentifier;
            self.companyIdentifier = model.getIntCompanyIdentifier;

        } else {
            return nil;
        }
    }
    return self;
}

/**
 * @brief   Initialize SigConfigVendorModelAppGet object.
 * @param   parameters    the hex data of SigConfigVendorModelAppGet.
 * @return  return `nil` when initialize SigConfigVendorModelAppGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configVendorModelAppGet;
        if (parameters == nil || parameters.length != 6) {
            return nil;
        }
        UInt16 tem1=0,tem2=0,tem3=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem1, dataByte, 2);
        memcpy(&tem2, dataByte+2, 2);
        memcpy(&tem3, dataByte+4, 2);
        self.elementAddress = tem1;
        self.modelIdentifier = tem2;
        self.companyIdentifier = tem3;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem = self.elementAddress;
    NSData *data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    tem = self.companyIdentifier;
    data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    tem = self.modelIdentifier;
    data = [NSData dataWithBytes:&tem length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigConfigVendorModelAppList class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.2.52 Config Vendor Model App List, opcode:0x804E


/// The Config Vendor Model App List is an unacknowledged message used to report
/// indexes of all AppKeys bound to the model. This message is only for Vendor Models.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.175), 4.3.2.52 Config Vendor Model App List.
@implementation SigConfigVendorModelAppList

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configVendorModelAppList;
    }
    return self;
}

//- (instancetype)initWithModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress applicationKeys:(NSArray <SigAppkeyModel *>*)applicationKeys status:(SigConfigMessageStatus)status {
//    if (self = [super init]) {
//        self.opCode = SigOpCode_configVendorModelAppList;
//        if (model.getIntCompanyIdentifier == 0) {
//            _elementAddress = elementAddress;
//            self.modelIdentifier = model.getIntModelIdentifier;
//            self.companyIdentifier = model.getIntCompanyIdentifier;
//            self.status = status;
//            self.applicationKeyIndexes = [NSMutableArray array];
//            for (SigAppkeyModel *model in applicationKeys) {
//                [self.applicationKeyIndexes addObject:@(model.index)];
//            }
//        } else {
//            return nil;
//        }
//    }
//    return self;
//}

/**
 * @brief   Initialize SigConfigVendorModelAppList object.
 * @param   parameters    the hex data of SigConfigVendorModelAppList.
 * @return  return `nil` when initialize SigConfigVendorModelAppList object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_configVendorModelAppList;
        if (parameters == nil || parameters.length < 7) {
            return nil;
        }
        UInt8 status=0;
        UInt16 tem1=0,tem2=0,tem3=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&status, dataByte, 1);
        memcpy(&tem1, dataByte+1, 2);
        memcpy(&tem2, dataByte+3, 2);
        memcpy(&tem3, dataByte+5, 2);
        self.status = status;
        _elementAddress = tem1;
        self.companyIdentifier = tem2;
        self.modelIdentifier = tem3;
        self.applicationKeyIndexes = [NSMutableArray arrayWithArray:[SigConfigNetKeyMessage decodeIndexesFromData:parameters atOffset:7]];
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = self.status;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    UInt16 tem16 = _elementAddress;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = self.companyIdentifier;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem16 = self.modelIdentifier;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    data = [self encodeIndexes:self.applicationKeyIndexes];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark - Remote Provisioning messages


#pragma mark 4.3.4.1 Remote Provisioning Scan Capabilities Get, opcode:0x804F


/// The Remote Provisioning Scan Capabilities Get message is an acknowledged message
/// used by the Remote Provisioning Client to get the value of the Remote Provisioning
/// Scan Capabilities state (see Section 4.2.22.)
/// @note   The response to a Remote Provisioning Scan Capabilities Get message is a
/// Remote Provisioning Scan Capabilities Status message (see Section 4.3.4.6).
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.18), 4.3.4.1 Remote Provisioning Scan Capabilities Get.
@implementation SigRemoteProvisioningScanCapabilitiesGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningScanCapabilitiesGet;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigRemoteProvisioningScanCapabilitiesStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

- (BOOL)isSegmented {
    return NO;
}

@end


#pragma mark 4.3.4.2 Remote Provisioning Scan Capabilities Status, opcode:0x805F


/// The Remote Provisioning Scan Capabilities Status message is an unacknowledged message
/// used by the Remote Provisioning Server to report the current value of the Remote Provisioning
/// Scan Capabilities state of a Remote Provisioning Server.
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.18), 4.3.4.2 Remote Provisioning Scan Capabilities Status.
@implementation SigRemoteProvisioningScanCapabilitiesStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningScanCapabilitiesStatus;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = self.maxScannedItems;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = self.activeScan?1:0;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

/**
 * @brief   Initialize SigRemoteProvisioningScanCapabilitiesStatus object.
 * @param   parameters    the hex data of SigRemoteProvisioningScanCapabilitiesStatus.
 * @return  return `nil` when initialize SigRemoteProvisioningScanCapabilitiesStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningScanCapabilitiesStatus;
        if (parameters == nil || parameters.length < 2) {
            return nil;
        }
        UInt8 tem=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _maxScannedItems = tem;
        memcpy(&tem, dataByte+1, 1);
        _activeScan = tem;
    }
    return self;
}

@end


#pragma mark 4.3.4.3 Remote Provisioning Scan Get, opcode:0x8051


/// The Remote Provisioning Scan Get message is an acknowledged message
/// that is used by the Remote Provisioning Client to get the various scanning
/// states of a Remote Provisioning Server model.
/// @note   The response to a Remote Provisioning Scan Get message is a
/// Remote Provisioning Scan Status message (see Section 4.3.4.6).
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.18), 4.3.4.3 Remote Provisioning Scan Get.
@implementation SigRemoteProvisioningScanGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningScanGet;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigRemoteProvisioningScanStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

- (BOOL)isSegmented {
    return NO;
}

@end


#pragma mark 4.3.4.4 Remote Provisioning Scan Start, opcode:0x8052


/// The Remote Provisioning Scan Start message is an acknowledged message
/// that is used by the Remote Provisioning Client to start the Remote Provisioning
/// Scan procedure, which finds unprovisioned devices within immediate radio
/// range of the Remote Provisioning Server (see Section 4.4.5.2).
/// @note   The response to a Remote Provisioning Scan Start message is a
/// Remote Provisioning Scan Status message (see Section 4.3.4.6).
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.18), 4.3.4.4 Remote Provisioning Scan Start.
@implementation SigRemoteProvisioningScanStart

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningScanStart;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = self.scannedItemsLimit;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = self.timeout;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (self.UUID && self.UUID.length) {
        [mData appendData:self.UUID];
    }
    return mData;
}

/**
 * @brief   Initialize SigRemoteProvisioningScanStart object.
 * @param   parameters    the hex data of SigRemoteProvisioningScanStart.
 * @return  return `nil` when initialize SigRemoteProvisioningScanStart object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningScanStart;
        if (parameters == nil || (parameters.length != 18 && parameters.length != 2)) {
            return nil;
        }
        UInt8 tem=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _scannedItemsLimit = tem;
        memcpy(&tem, dataByte+1, 1);
        _timeout = tem;
        if (parameters.length >= 18) {
            _UUID = [parameters subdataWithRange:NSMakeRange(2, 16)];
        }
    }
    return self;
}

/**
 * @brief   Initialize SigRemoteProvisioningScanStart object.
 * @param   scannedItemsLimit   Maximum number of scanned items to be reported.
 * The ScannedItemsLimit field identifies the maximum number of unprovisioned
 * devices the Remote Provisioning Server can report while executing the Remote
 * Provisioning Scan procedure. Value 0 indicates that the Remote Provisioning
 * Client does not set a limit on the number of unprovisioned devices that the
 * Remote Provisioning Server can report.
 * @param   timeout    Time limit for a scan (in seconds).
 * The Timeout field identifies the new value of the Remote Provisioning Timeout
 * state (see Section 4.2.23.2). The value of the Timeout field shall not be 0.
 * @param   UUID    Device UUID (Optional)
 * If the UUID field is present, the Remote Provisioning Client is requesting a
 * Single Device Scanning procedure, i.e., a scan for a specific device identified
 * by the value of the UUID field. If the UUID field is absent, the Remote
 * Provisioning Client is requesting a scan for all unprovisioned devices within
 * immediate radio range (a Multiple Devices Scanning).
 * @return  return `nil` when initialize SigRemoteProvisioningScanStart object fail.
 */
- (instancetype)initWithScannedItemsLimit:(UInt8)scannedItemsLimit timeout:(UInt8)timeout UUID:(nullable NSData *)UUID {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningScanStart;
        _scannedItemsLimit = scannedItemsLimit;
        _timeout = timeout;
        _UUID = UUID;
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigRemoteProvisioningScanStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

- (BOOL)isSegmented {
    return NO;
}

@end


#pragma mark 4.3.4.5 Remote Provisioning Scan Stop, opcode:0x8053


/// The Remote Provisioning Scan Stop message is an acknowledged message
/// that is used by the Remote Provisioning Client to terminate the Remote
/// Provisioning Scan procedure (see Section 4.4.5.2).
/// @note   The response to a Remote Provisioning Scan Stop message is a
/// Remote Provisioning Scan Status message (see Section 4.3.4.6).
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.19), 4.3.4.5 Remote Provisioning Scan Stop.
@implementation SigRemoteProvisioningScanStop

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningScanStop;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigRemoteProvisioningScanStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

- (BOOL)isSegmented {
    return NO;
}

@end


#pragma mark 4.3.4.6 Remote Provisioning Scan Status, opcode:0x8054


/// The Remote Provisioning Scan Status message is an unacknowledged message
/// used by the Remote Provisioning Server to report the current value of the Remote
/// Provisioning Scan Parameters state and the Remote Provisioning Scan state of a
/// Remote Provisioning Server model.
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.19), 4.3.4.6 Remote Provisioning Scan Status.
@implementation SigRemoteProvisioningScanStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningScanStatus;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = self.status;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = self.RPScanningState;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = self.scannedItemsLimit;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = self.timeout;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

/**
 * @brief   Initialize SigRemoteProvisioningScanStatus object.
 * @param   parameters    the hex data of SigRemoteProvisioningScanStatus.
 * @return  return `nil` when initialize SigRemoteProvisioningScanStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningScanStatus;
        if (parameters == nil || parameters.length < 4) {
            return nil;
        }
        UInt8 tem=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _status = tem;
        memcpy(&tem, dataByte+1, 1);
        _RPScanningState = tem;
        memcpy(&tem, dataByte+2, 1);
        _scannedItemsLimit = tem;
        memcpy(&tem, dataByte+3, 1);
        _timeout = tem;
    }
    return self;
}

@end


#pragma mark 4.3.4.7 Remote Provisioning Scan Report, opcode:0x8055


/// The Remote Provisioning Scan Report message is an unacknowledged message
/// used by the Remote Provisioning Server to report the scanned Device UUID of an
/// unprovisioned device. Based on the Remote Provisioning Scan Reports received
/// from multiple Remote Provisioning Servers, the Remote Provisioning Client can
/// select the most suitable Remote Provisioning Server to execute the Extended
/// Scan procedure and/or to provision the unprovisioned device.
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.20), 4.3.4.7 Remote Provisioning Scan Report.
@implementation SigRemoteProvisioningScanReport

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningScanReport;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    SInt8 stem8 = self.RSSI;
    NSData *data = [NSData dataWithBytes:&stem8 length:1];
    [mData appendData:data];
    if (self.UUID && self.UUID.length) {
        [mData appendData:self.UUID];
    }
    UInt16 tem16 = self.OOB;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

/**
 * @brief   Initialize SigRemoteProvisioningScanReport object.
 * @param   parameters    the hex data of SigRemoteProvisioningScanReport.
 * @return  return `nil` when initialize SigRemoteProvisioningScanReport object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningScanReport;
        if (parameters == nil || parameters.length <19) {
            return nil;
        }
        SInt8 stem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&stem8, dataByte, 1);
        _RSSI = stem8;
        if (parameters.length >= 17) {
            _UUID = [parameters subdataWithRange:NSMakeRange(1, 16)];
            if (parameters.length >= 19) {
                UInt16 tem16 = 0;
                memcpy(&tem16, dataByte+17, 2);
                _OOB = tem16;
            }
        }
    }
    return self;
}

- (BOOL)isSegmented {
    return NO;
}

@end


#pragma mark 4.3.4.8 Remote Provisioning Extended Scan Start, opcode:0x8056


/// The Remote Provisioning Extended Scan Start message is an unacknowledged message
/// that is used by the Remote Provisioning Client to request additional information about a
/// specific unprovisioned device or about the Remote Provisioning Server itself.
/// @note   As a result of processing a Remote Provisioning Extended Scan Start message,
/// the Remote Provisioning Server sends a Remote Provisioning Extended Scan Report
/// message (see Section 4.3.4.9).
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.20), 4.3.4.8 Remote Provisioning Extended Scan Start.
@implementation SigRemoteProvisioningExtendedScanStart

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningExtendedScanStart;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = self.ADTypeFilterCount;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (self.ADTypeFilter && self.ADTypeFilter.length) {
        [mData appendData:self.ADTypeFilter];
    }
    if (self.UUID && self.UUID.length) {
        [mData appendData:self.UUID];
        tem8 = self.timeout;
        data = [NSData dataWithBytes:&tem8 length:2];
        [mData appendData:data];
    }
    return mData;
}

/**
 * @brief   Initialize SigRemoteProvisioningExtendedScanStart object.
 * @param   parameters    the hex data of SigRemoteProvisioningExtendedScanStart.
 * @return  return `nil` when initialize SigRemoteProvisioningExtendedScanStart object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningExtendedScanStart;
        if (parameters == nil || parameters.length < 1) {
            return nil;
        }
        UInt8 tem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        self.ADTypeFilterCount = tem8;
        if (self.ADTypeFilterCount == 0) {
            if (parameters.length >= 17) {
                self.UUID = [parameters subdataWithRange:NSMakeRange(1, 16)];
                if (parameters.length >= 18) {
                    memcpy(&tem8, dataByte+17, 1);
                    self.timeout = tem8;
                }
            }
        }
    }
    return self;
}

/**
 * @brief   Initialize SigRemoteProvisioningExtendedScanStart object.
 * @param   ADTypeFilterCount    Number of AD Types in the ADTypeFilter field.
 * The ADTypeFilterCount field identifies number of AD types listed in the ADTypeFilter field.
 * @param   ADTypeFilter    List of AD Types to be reported (C.1), size:variable.
 * C.1: If ADTypeFilterCount field value is not zero, the ADTypeFilter shall be present; otherwise
 * the ADTypeFilter shall not be present
 * If present, the ADTypeFilter is a list of AD types that the client is requesting. The ADTypeFilter
 * shall not contain the Shortened Local Name AD Type, the Incomplete List of 16-bit Service
 * UUIDs AD Type, the Incomplete List of 32-bit Service UUIDs AD Type, or the Incomplete List
 * of 128-bit Service UUIDs AD Type.
 * Note: If the ADTypeFilter field contains the Complete Local Name AD Type, the client is
 * requesting either the Complete Local Name or the Shortened Local Name.
 * @param   UUID    Device UUID (Optional).
 * If present, the UUID field identifies the Device UUID of the unprovisioned device for which
 * additional information is requested (see Section 4.4.5.3). If the UUID field is absent, the
 * request retrieves information about the Remote Provisioning Server (see Section 4.4.5.5.2.1).
 * In the latter case, the Remote Provisioning Server ignores the Timeout field value.
 * @param   timeout    Time limit for a scan (in seconds) (C.2). Length of time (in seconds) to collect information
 * about the unprovisioned device: 0x01~0x05
 * C.2: If UUID field is present, the Timeout field shall also be present; otherwise Timeout
 * field shall not be present.
 * The Timeout field indicates how long the Remote Provisioning Client requests the Remote
 * Provisioning Server to collect information about the unprovisioned device identified by the
 * UUID. Table 4.11 defines the values for the Timeout field.
 * @return  return `nil` when initialize SigRemoteProvisioningExtendedScanStart object fail.
 */
- (instancetype)initWithADTypeFilterCount:(UInt8)ADTypeFilterCount ADTypeFilter:(nullable NSData *)ADTypeFilter UUID:(nullable NSData *)UUID timeout:(UInt8)timeout {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningExtendedScanStart;
        _ADTypeFilterCount = ADTypeFilterCount;
        _ADTypeFilter = ADTypeFilter;
        _UUID = UUID;
        _timeout = timeout;
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigRemoteProvisioningExtendedScanReport class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

- (BOOL)isSegmented {
    return NO;
}

@end


#pragma mark 4.3.4.9 Remote Provisioning Extended Scan Report, opcode:0x8057


/// The Remote Provisioning Extended Scan Report message is an unacknowledged message
/// used by the Remote Provisioning Server to report the advertising data requested by the client
/// in a Remote Provisioning Extended Scan Start message (see Section 4.3.4.8).
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.21), 4.3.4.9 Remote Provisioning Extended Scan Report.
@implementation SigRemoteProvisioningExtendedScanReport

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningExtendedScanReport;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = self.status;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    [mData appendData:self.UUID];
    UInt16 tem16 = self.OOBInformation;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

/**
 * @brief   Initialize SigRemoteProvisioningExtendedScanReport object.
 * @param   parameters    the hex data of SigRemoteProvisioningExtendedScanReport.
 * @return  return `nil` when initialize SigRemoteProvisioningExtendedScanReport object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningExtendedScanReport;
        if (parameters == nil || parameters.length <17) {
            return nil;
        }
        UInt8 tem8 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _status = tem8;
        if (parameters.length >= 17) {
            _UUID = [parameters subdataWithRange:NSMakeRange(1, 16)];
            if (parameters.length >= 19) {
                UInt16 tem16 = 0;
                memcpy(&tem16, dataByte+17, 2);
                _OOBInformation = tem16;
                if (parameters.length > 19) {
                    _AdvStructures = [parameters subdataWithRange:NSMakeRange(19, parameters.length-19)];
                }
            }
        }
    }
    return self;
}

@end


#pragma mark 4.3.4.10 Remote Provisioning Link Get, opcode:0x8058


/// The Remote Provisioning Link Get message is an acknowledged message used by the Remote
/// Provisioning Client to get the Remote Provisioning Link state of a Remote Provisioning Server model.
/// @note   The response to a Remote Provisioning Link Get message is a Remote Provisioning
/// Link Status message (see Section 4.3.4.13).
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.22), 4.3.4.10 Remote Provisioning Link Get.
@implementation SigRemoteProvisioningLinkGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningLinkGet;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigRemoteProvisioningLinkStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

- (BOOL)isSegmented {
    return NO;
}

@end


#pragma mark 4.3.4.11 Remote Provisioning Link Open, opcode:0x8059


/// The Remote Provisioning Link Open message is an acknowledged message used by the Remote
/// Provisioning Client to establish the provisioning bearer between a node supporting the Remote
/// Provisioning Server model and an unprovisioned device, or to open the Device Key Refresh Interface.
/// @note   The response to a Remote Provisioning Link Open message is a Remote Provisioning
/// Link Status message (see Section 4.3.4.13).
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.22), 4.3.4.11 Remote Provisioning Link Open.
@implementation SigRemoteProvisioningLinkOpen

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningLinkOpen;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    if (_UUID && _UUID.length > 0) {
        return [NSData dataWithData:_UUID];
    }
    return nil;
}

/**
 * @brief   Initialize SigRemoteProvisioningLinkOpen object.
 * @param   UUID    Device UUID (Optional).
 * If present, the UUID field identifies the Device UUID of an unprovisioned device that the
 * link will be open to. If the UUID field is absent, the Remote Provisioning Server will open
 * the Device Key Refresh Interface.
 * @return  return `nil` when initialize SigRemoteProvisioningLinkOpen object fail.
 */
- (instancetype)initWithUUID:(nullable NSData *)UUID {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningLinkOpen;
        _UUID = UUID;
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigRemoteProvisioningLinkStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

- (BOOL)isSegmented {
    return NO;
}

@end


#pragma mark 4.3.4.12 Remote Provisioning Link Close, opcode:0x805A


/// The Remote Provisioning Link Close message is an acknowledged message used by the Remote
/// Provisioning Client to close a provisioning bearer or the Device Key Refresh Interface.
/// @note   The response to a Remote Provisioning Link Close message is a Remote Provisioning
/// Link Status message (see Section 4.3.4.13).
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.23), 4.3.4.12 Remote Provisioning Link Close.
@implementation SigRemoteProvisioningLinkClose

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningLinkClose;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = self.reason;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

/**
 * @brief   Initialize SigRemoteProvisioningLinkOpen object.
 * @param   reason    Link close reason code.
 * The Reason field identifies the reason for the provisioning link close. The Reason field values for the
 * Remote Provisioning Link Close message are defined in Table 4.15.
 * @return  return `nil` when initialize SigRemoteProvisioningLinkOpen object fail.
 */
- (instancetype)initWithReason:(SigRemoteProvisioningLinkCloseStatus)reason {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningLinkClose;
        _reason = reason;
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigRemoteProvisioningLinkStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

- (BOOL)isSegmented {
    return NO;
}

@end


#pragma mark 4.3.4.13 Remote Provisioning Link Status, opcode:0x805B


/// The Remote Provisioning Link Status message is an unacknowledged message used by
/// the Remote Provisioning Server to acknowledge a Remote Provisioning Link Get message,
/// a Remote Provisioning Link Open message, or a Remote Provisioning Link Close message.
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.23), 4.3.4.13 Remote Provisioning Link Status.
@implementation SigRemoteProvisioningLinkStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningLinkStatus;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _status;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _RPState;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

/**
 * @brief   Initialize SigRemoteProvisioningLinkStatus object.
 * @param   parameters    the hex data of SigRemoteProvisioningLinkStatus.
 * @return  return `nil` when initialize SigRemoteProvisioningLinkStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningLinkStatus;
        if (parameters == nil || parameters.length != 2) {
            return nil;
        }
        UInt8 tem=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _status = tem;
        memcpy(&tem, dataByte+1, 1);
        _RPState = tem;
    }
    return self;
}

@end


#pragma mark 4.3.4.14 Remote Provisioning Link Report, opcode:0x805C


/// The Remote Provisioning Link Report message is an unacknowledged message used
/// by the Remote Provisioning Server to report the state change of a provisioning bearer
/// link or the Device Key Refresh Interface.
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.24), 4.3.4.14 Remote Provisioning Link Report.
@implementation SigRemoteProvisioningLinkReport

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningLinkReport;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _status;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _RPState;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_reason < 0x03) {
        tem8 = _reason;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    return mData;
}

/**
 * @brief   Initialize SigRemoteProvisioningLinkReport object.
 * @param   parameters    the hex data of SigRemoteProvisioningLinkReport.
 * @return  return `nil` when initialize SigRemoteProvisioningLinkReport object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningLinkReport;
        if (parameters == nil || (parameters.length != 2 && parameters.length != 3)) {
            return nil;
        }
        UInt8 tem=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _status = tem;
        memcpy(&tem, dataByte+1, 1);
        _RPState = tem;
        if (parameters.length >= 3) {
            memcpy(&tem, dataByte+2, 1);
            _reason = tem;
        }
    }
    return self;
}

@end


#pragma mark 4.3.4.14 Remote Provisioning PDU Send, opcode:0x805D


/// The Remote Provisioning PDU Send message is an unacknowledged message used
/// by the Remote Provisioning Client to deliver the Provisioning PDU to an unprovisioned
/// device or to the Device Key Refresh Interface.
/// @note   The Remote Provisioning PDU Send message should be sent as a Segmented
/// Access message. Alternatively, the Remote Provisioning Client needs to keep track of
/// Remote Provisioning PDU Outbound Report messages and may need to retry sending
/// Remote Provisioning PDU Send message.
/// @note   When the Remote Provisioning server receives a Remote Provisioning PDU
/// Send message, the server attempts to deliver a Provisioning PDU. If the Provisioning
/// PDU is delivered, the Remote Provisioning Server sends a Remote Provisioning PDU
/// Outbound Report message (see Section 4.3.4.16). If the Remote Provisioning Server
/// fails to deliver the Provisioning PDU, the Remote Provisioning Link Report message
/// is sent (see Section 4.3.4.14).
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.24), 4.3.4.14 Remote Provisioning PDU Send.
@implementation SigRemoteProvisioningPDUSend

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningPDUSend;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _outboundPDUNumber;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_provisioningPDU) {
        [mData appendData:_provisioningPDU];
    }
    return mData;
}

/**
 * @brief   Initialize SigRemoteProvisioningPDUSend object.
 * @param   parameters    the hex data of SigRemoteProvisioningPDUSend.
 * @return  return `nil` when initialize SigRemoteProvisioningPDUSend object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningPDUSend;
        if (parameters == nil || parameters.length < 2) {
            return nil;
        }
        UInt8 tem=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _outboundPDUNumber = tem;
        if (parameters.length >= 2) {
            _provisioningPDU = [parameters subdataWithRange:NSMakeRange(1, parameters.length-1)];
        }
    }
    return self;
}

/**
 * @brief   Initialize SigRemoteProvisioningPDUSend object.
 * @param   outboundPDUNumber    Provisioning PDU identification number.
 * The OutboundPDUNumber field identifies the identification number of the Provisioning
 * PDU set in the ProvisioningPDU field.
 * @param   provisioningPDU    Provisioning PDU.
 * The ProvisioningPDU field identifies the Provisioning PDU that either will be sent to an
 * unprovisioned device or will be processed locally if the Device Key Refresh procedure
 * is in progress.
 * @return  return `nil` when initialize SigRemoteProvisioningPDUSend object fail.
 */
- (instancetype)initWithOutboundPDUNumber:(UInt8)outboundPDUNumber provisioningPDU:(NSData *)provisioningPDU {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningPDUSend;
        _outboundPDUNumber = outboundPDUNumber;
        _provisioningPDU = provisioningPDU;
    }
    return self;
}

- (BOOL)isSegmented {
//    return NO;
    return YES;
}

@end


#pragma mark 4.3.4.14 Remote Provisioning PDU Outbound Report, opcode:0x805E


/// The Remote Provisioning PDU Outbound Report message is an unacknowledged message
/// used by the Remote Provisioning Server to report completion of the delivery of the Provisioning
/// PDUs that the Remote Provisioning Server either sends to a device that is being provisioned or
/// processes locally during the Device Key Refresh procedure.
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.25), 4.3.4.14 Remote Provisioning PDU Outbound Report.
@implementation SigRemoteProvisioningPDUOutboundReport

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningPDUOutboundReport;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _outboundPDUNumber;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

/**
 * @brief   Initialize SigRemoteProvisioningPDUOutboundReport object.
 * @param   parameters    the hex data of SigRemoteProvisioningPDUOutboundReport.
 * @return  return `nil` when initialize SigRemoteProvisioningPDUOutboundReport object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningPDUOutboundReport;
        if (parameters == nil || parameters.length != 1) {
            return nil;
        }
        UInt8 tem=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _outboundPDUNumber = tem;
    }
    return self;
}

@end


#pragma mark 4.3.4.14 Remote Provisioning PDU Report, opcode:0x805F


/// The Remote Provisioning PDU Report message is an unacknowledged message used
/// by the Remote Provisioning Server to report the Provisioning PDU that either was received
/// from the device being provisioned or was generated locally during the Device Key
/// Refresh procedure.
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.25), 4.3.4.14 Remote Provisioning PDU Report.
@implementation SigRemoteProvisioningPDUReport

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningPDUReport;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _outboundPDUNumber;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_provisioningPDU) {
        [mData appendData:_provisioningPDU];
    }
    return mData;
}

/**
 * @brief   Initialize SigRemoteProvisioningPDUReport object.
 * @param   parameters    the hex data of SigRemoteProvisioningPDUReport.
 * @return  return `nil` when initialize SigRemoteProvisioningPDUReport object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_remoteProvisioningPDUReport;
        if (parameters == nil || parameters.length < 2) {
            return nil;
        }
        UInt8 tem=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem, dataByte, 1);
        _outboundPDUNumber = tem;
        if (parameters.length >= 2) {
            _provisioningPDU = [parameters subdataWithRange:NSMakeRange(1, parameters.length-1)];
        }
    }
    return self;
}

@end


#pragma mark 4.3.9.2 OPCODES_AGGREGATOR_SEQUENCE, opcode:0xB809


/// An OPCODES_AGGREGATOR_STATUS message is an unacknowledged message used to report
/// status for the most recent operation and response messages as a result of processing the Items
/// field in an OPCODES_AGGREGATOR_SEQUENCE message.
/// @note   - seeAlso: MshPRFd1.1r13_clean.pdf (page.389),
/// 4.3.9.2 OPCODES_AGGREGATOR_SEQUENCE.
@implementation SigOpcodesAggregatorSequence

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_OpcodesAggregatorSequence;
        _isEncryptByDeviceKey = NO;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _elementAddress;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    if (_items && _items.count) {
        NSArray *temA = [NSArray arrayWithArray:_items];
        for (SigOpcodesAggregatorItemModel *m in temA) {
            [mData appendData:m.parameters];
        }
    }
    return mData;
}

- (NSData *)opCodeAndParameters {
    NSMutableData *mData = [NSMutableData data];
    NSData *data = [SigHelper.share getOpCodeDataWithUInt32Opcode:self.opCode];
    [mData appendData:data];
    [mData appendData:self.parameters];
    return mData;
}

/**
 * @brief   Initialize SigOpcodesAggregatorSequence object.
 * @param   parameters    the hex data of SigOpcodesAggregatorSequence.
 * @return  return `nil` when initialize SigOpcodesAggregatorSequence object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_OpcodesAggregatorSequence;
        _isEncryptByDeviceKey = NO;
        if (parameters == nil || parameters.length < 2) {
            return nil;
        }
        UInt16 tem16=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _elementAddress = tem16;
        NSMutableArray *mArray = [NSMutableArray array];
        NSMutableData *mData = [NSMutableData dataWithData:[parameters subdataWithRange:NSMakeRange(2, parameters.length-2)]];
        while (mData.length > 0) {
            SigOpcodesAggregatorItemModel *model = [[SigOpcodesAggregatorItemModel alloc] initWithOpcodeAndParameters:mData];
            SigOpCodeAndParametersModel *opCodeAndParametersModel = [[SigOpCodeAndParametersModel alloc] initWithOpCodeAndParameters:model.opcodeAndParameters];
            if (opCodeAndParametersModel == nil) {
                return nil;
            }
            _isEncryptByDeviceKey = [SigHelper.share isDeviceKeyOpCode:opCodeAndParametersModel.opCode];
            if (model) {
                [mArray addObject:model];
                [mData replaceBytesInRange:NSMakeRange(0, model.parameters.length) withBytes:nil length:0];
            } else {
                break;
            }
        }
        _items = mArray;
    }
    return self;
}

- (instancetype)initWithElementAddress:(UInt16)elementAddress items:(NSArray <SigOpcodesAggregatorItemModel *>*)items {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_OpcodesAggregatorSequence;
        _isEncryptByDeviceKey = NO;
        _elementAddress = elementAddress;
        if (items && items.count > 0) {
            _items = [NSMutableArray arrayWithArray:items];
            SigOpcodesAggregatorItemModel *model = items.firstObject;
            SigOpCodeAndParametersModel *opCodeAndParametersModel = [[SigOpCodeAndParametersModel alloc] initWithOpCodeAndParameters:model.opcodeAndParameters];
            if (opCodeAndParametersModel == nil) {
                return nil;
            }
            _isEncryptByDeviceKey = [SigHelper.share isDeviceKeyOpCode:opCodeAndParametersModel.opCode];
        }
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigOpcodesAggregatorStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

//- (BOOL)isSegmented {
//    return NO;
//}

@end


#pragma mark 4.3.9.3 OPCODES_AGGREGATOR_STATUS, opcode:0xB810


/// An OPCODES_AGGREGATOR_STATUS message is an unacknowledged message
/// used to report status for the most recent operation and response messages as a result
/// of processing the Items field in an OPCODES_AGGREGATOR_SEQUENCE message.
/// @note   - seeAlso: MshPRFd1.1r13_clean.pdf (page.389),
/// 4.3.9.3 OPCODES_AGGREGATOR_STATUS.
@implementation SigOpcodesAggregatorStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_OpcodesAggregatorStatus;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _status;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    UInt16 tem16 = _elementAddress;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    if (_statusItems && _statusItems.count) {
        NSArray *temA = [NSArray arrayWithArray:_statusItems];
        for (SigOpcodesAggregatorItemModel *m in temA) {
            [mData appendData:m.parameters];
        }
    }
    return mData;
}

/**
 * @brief   Initialize SigOpcodesAggregatorStatus object.
 * @param   parameters    the hex data of SigOpcodesAggregatorStatus.
 * @return  return `nil` when initialize SigOpcodesAggregatorStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_OpcodesAggregatorStatus;
        if (parameters == nil || parameters.length < 3) {
            return nil;
        }
        UInt8 tem8=0;
        UInt16 tem16=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        memcpy(&tem16, dataByte+1, 2);
        _status = tem8;
        _elementAddress = tem16;
        NSMutableArray *mArray = [NSMutableArray array];
        NSMutableData *mData = [NSMutableData dataWithData:[parameters subdataWithRange:NSMakeRange(3, parameters.length-3)]];
        while (mData.length > 0) {
            SigOpcodesAggregatorItemModel *model = [[SigOpcodesAggregatorItemModel alloc] initWithOpcodeAndParameters:mData];
            if (model) {
                [mArray addObject:model];
                [mData replaceBytesInRange:NSMakeRange(0, model.parameters.length) withBytes:nil length:0];
            } else {
                break;
            }
        }
        _statusItems = mArray;
    }
    return self;
}

- (instancetype)initWithStatus:(SigOpcodesAggregatorMessagesStatus)status elementAddress:(UInt16)elementAddress items:(NSArray <SigOpcodesAggregatorItemModel *>*)items {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_OpcodesAggregatorStatus;
        _status = status;
        _elementAddress = elementAddress;
        _statusItems = [NSMutableArray arrayWithArray:items];
    }
    return self;
}

@end


#pragma mark - 4.3.12 Mesh Private Beacon messages


#pragma mark 4.3.12.1 PRIVATE_BEACON_GET, opcode:0xB711


/// A PRIVATE_BEACON_GET message is an acknowledged message
/// used to get the current Private Beacon state (see Section 4.2.44.1)
/// and Random Update Interval Steps state (see Section 4.2.44.2) of a node.
/// @note   The response to a PRIVATE_BEACON_GET message is a
/// PRIVATE_BEACON_STATUS message.
/// @note   - seeAlso: MshPRFd1.1r13_clean.pdf (page.401),
/// 4.3.12.1 PRIVATE_BEACON_GET.
@implementation SigPrivateBeaconGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_PrivateBeaconGet;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigPrivateBeaconStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.12.2 PRIVATE_BEACON_SET, opcode:0xB712


/// A PRIVATE_BEACON_SET message is an acknowledged message used to set
/// the Private Beacon state and the Random Update Interval Steps state of a node.
/// @note   The response to a PRIVATE_BEACON_SET message is a
/// PRIVATE_BEACON_STATUS message.
/// @note   - seeAlso: MshPRFd1.1r13_clean.pdf (page.401),
/// 4.3.12.2 PRIVATE_BEACON_SET.
@implementation SigPrivateBeaconSet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_PrivateBeaconSet;
        _needRandomUpdateIntervalSteps = NO;
    }
    return self;
}

- (instancetype)initWithPrivateBeacon:(SigPrivateBeaconState)privateBeacon randomUpdateIntervalSteps:(UInt8)randomUpdateIntervalSteps {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_PrivateBeaconSet;
        _privateBeacon = privateBeacon;
        _randomUpdateIntervalSteps = randomUpdateIntervalSteps;
        _needRandomUpdateIntervalSteps = YES;
    }
    return self;
}

- (instancetype)initWithPrivateBeacon:(SigPrivateBeaconState)privateBeacon {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_PrivateBeaconSet;
        _privateBeacon = privateBeacon;
        _needRandomUpdateIntervalSteps = NO;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _privateBeacon;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    if (_needRandomUpdateIntervalSteps) {
        tem8 = _randomUpdateIntervalSteps;
        data = [NSData dataWithBytes:&tem8 length:1];
        [mData appendData:data];
    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigPrivateBeaconStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.12.3 PRIVATE_BEACON_STATUS, opcode:0xB713


/// A PRIVATE_BEACON_STATUS message is an unacknowledged message used to report
/// the current Private Beacon state and Random Update Interval Steps state of a node.
/// @note   This message is a response to PRIVATE_BEACON_GET message or a
/// PRIVATE_BEACON_SET message.
/// @note   - seeAlso: MshPRFd1.1r13_clean.pdf (page.402),
/// 4.3.12.3 PRIVATE_BEACON_STATUS.
@implementation SigPrivateBeaconStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_PrivateBeaconStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigPrivateBeaconStatus object.
 * @param   parameters    the hex data of SigPrivateBeaconStatus.
 * @return  return `nil` when initialize SigPrivateBeaconStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_PrivateBeaconStatus;
        if (parameters == nil || parameters.length != 2) {
            return nil;
        }
        UInt8 tem8=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _privateBeacon = tem8;
        memcpy(&tem8, dataByte+1, 1);
        _randomUpdateIntervalSteps = tem8;
    }
    return self;
}

- (instancetype)initWithPrivateBeacon:(SigPrivateBeaconState)privateBeacon randomUpdateIntervalSteps:(UInt8)randomUpdateIntervalSteps {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_PrivateBeaconStatus;
        _privateBeacon = privateBeacon;
        _randomUpdateIntervalSteps = randomUpdateIntervalSteps;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _privateBeacon;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    tem8 = _randomUpdateIntervalSteps;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 4.3.12.4 PRIVATE_GATT_PROXY_GET, opcode:0xB714


/// A PRIVATE_GATT_PROXY_GET message is an acknowledged message used to
/// get the current Private GATT Proxy state of a node (see Section 4.2.45).
/// @note   The response to a PRIVATE_GATT_PROXY_GET message is a
/// PRIVATE_GATT_PROXY_STATUS message.
/// @note   - seeAlso: MshPRFd1.1r13_clean.pdf (page.402),
/// 4.3.12.4 PRIVATE_GATT_PROXY_GET.
@implementation SigPrivateGattProxyGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_PrivateGattProxyGet;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigPrivateGattProxyStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.12.5 PRIVATE_GATT_PROXY_SET, opcode:0xB715


/// A PRIVATE_GATT_PROXY_SET message is an acknowledged message
/// used to set the Private GATT Proxy state of a node (see Section 4.2.45).
/// @note   The response to a PRIVATE_GATT_PROXY_SET message is a
/// PRIVATE_GATT_PROXY_STATUS message.
/// @note   - seeAlso: MshPRFd1.1r13_clean.pdf (page.403),
/// 4.3.12.5 PRIVATE_GATT_PROXY_SET.
@implementation SigPrivateGattProxySet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_PrivateGattProxySet;
    }
    return self;
}

- (instancetype)initWithPrivateGattProxy:(SigPrivateGattProxyState)privateGattProxy {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_PrivateGattProxySet;
        _privateGattProxy = privateGattProxy;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _privateGattProxy;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigPrivateGattProxyStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.12.6 PRIVATE_GATT_PROXY_STATUS, opcode:0xB716


/// A PRIVATE_GATT_PROXY_STATUS message is an unacknowledged message used to
/// report the current Private GATT Proxy state of a node (see Section 4.2.45).
/// @note   - seeAlso: MshPRFd1.1r13_clean.pdf (page.403),
/// 4.3.12.6 PRIVATE_GATT_PROXY_STATUS.
@implementation SigPrivateGattProxyStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_PrivateGattProxyStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigPrivateGattProxyStatus object.
 * @param   parameters    the hex data of SigPrivateGattProxyStatus.
 * @return  return `nil` when initialize SigPrivateGattProxyStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_PrivateGattProxyStatus;
        if (parameters == nil || parameters.length != 1) {
            return nil;
        }
        UInt8 tem8=0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem8, dataByte, 1);
        _privateGattProxy = tem8;
    }
    return self;
}

- (instancetype)initWithPrivateGattProxy:(SigPrivateGattProxyState)privateGattProxy {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_PrivateBeaconStatus;
        _privateGattProxy = privateGattProxy;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _privateGattProxy;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

@end


#pragma mark 4.3.12.7 PRIVATE_NODE_IDENTITY_GET, opcode:0xB718


/// A PRIVATE_NODE_IDENTITY_GET message is an acknowledged message used to get
///  the current Private Node Identity state for a subnet (see Section 4.2.46).
/// @note   The response to a PRIVATE_NODE_IDENTITY_GET message is a
/// PRIVATE_NODE_IDENTITY_STATUS message.
/// @note   - seeAlso: MshPRFd1.1r13_clean.pdf (page.404),
/// 4.3.12.7 PRIVATE_NODE_IDENTITY_GET.
@implementation SigPrivateNodeIdentityGet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_PrivateNodeIdentityGet;
    }
    return self;
}

- (instancetype)initWithNetKeyIndex:(UInt16)netKeyIndex {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_PrivateNodeIdentityGet;
        _netKeyIndex = netKeyIndex;
    }
    return self;
}

/**
 * @brief   Initialize SigPrivateNodeIdentityGet object.
 * @param   parameters    the hex data of SigPrivateNodeIdentityGet.
 * @return  return `nil` when initialize SigPrivateNodeIdentityGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_PrivateNodeIdentityGet;
        if (parameters == nil || parameters.length != 2) {
            return nil;
        }
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _netKeyIndex = (tem16 >> 4) & 0xFFF;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _netKeyIndex << 4;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigPrivateNodeIdentityStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.12.8 PRIVATE_NODE_IDENTITY_SET, opcode:0xB719


/// A PRIVATE_NODE_IDENTITY_SET message is an acknowledged message used to set
/// the current Private Node Identity state for a subnet (see Section 4.2.46).
/// @note   The response to a PRIVATE_NODE_IDENTITY_SET message is a
/// PRIVATE_NODE_IDENTITY_STATUS message.
/// @note   - seeAlso: MshPRFd1.1r13_clean.pdf (page.404),
/// 4.3.12.8 PRIVATE_NODE_IDENTITY_SET.
@implementation SigPrivateNodeIdentitySet

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_PrivateNodeIdentitySet;
    }
    return self;
}

- (instancetype)initWithNetKeyIndex:(UInt16)netKeyIndex privateIdentity:(SigPrivateNodeIdentityState)privateIdentity {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_PrivateNodeIdentitySet;
        _netKeyIndex = netKeyIndex;
        _privateIdentity = privateIdentity;
    }
    return self;
}

/**
 * @brief   Initialize SigPrivateNodeIdentitySet object.
 * @param   parameters    the hex data of SigPrivateNodeIdentitySet.
 * @return  return `nil` when initialize SigPrivateNodeIdentitySet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_PrivateNodeIdentitySet;
        if (parameters == nil || parameters.length != 3) {
            return nil;
        }
        UInt16 tem16 = 0;
        Byte *dataByte = (Byte *)parameters.bytes;
        memcpy(&tem16, dataByte, 2);
        _netKeyIndex = (tem16 >> 4) & 0xFFF;
        UInt8 tem8 = 0;
        memcpy(&tem8, dataByte+2, 1);
        _privateIdentity = tem8;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt16 tem16 = _netKeyIndex << 4;
    NSData *data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    UInt8 tem8 = _privateIdentity;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [SigPrivateNodeIdentityStatus class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((SigMeshMessage *)[[self.responseType alloc] init]).opCode;
}

@end


#pragma mark 4.3.12.9 PRIVATE_NODE_IDENTITY_STATUS, opcode:0xB71A


/// A PRIVATE_NODE_IDENTITY_STATUS message is an unacknowledged message
/// used to report the current Private Node Identity state for a subnet (see Section 4.2.46).
/// @note   The response to a PRIVATE_NODE_IDENTITY_SET message is a
/// PRIVATE_NODE_IDENTITY_STATUS message.
/// @note   - seeAlso: MshPRFd1.1r13_clean.pdf (page.405),
/// 4.3.12.9 PRIVATE_NODE_IDENTITY_STATUS.
@implementation SigPrivateNodeIdentityStatus

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_PrivateNodeIdentityStatus;
    }
    return self;
}

/**
 * @brief   Initialize SigPrivateNodeIdentityStatus object.
 * @param   parameters    the hex data of SigPrivateNodeIdentityStatus.
 * @return  return `nil` when initialize SigPrivateNodeIdentityStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = SigOpCode_PrivateNodeIdentityStatus;
        if (parameters == nil || parameters.length != 4) {
            return nil;
        }
        Byte *dataByte = (Byte *)parameters.bytes;
        UInt8 tem8 = 0;
        memcpy(&tem8, dataByte, 1);
        _status = tem8;
        UInt16 tem16 = 0;
        memcpy(&tem16, dataByte+1, 2);
        _netKeyIndex = (tem16 >> 4) & 0xFFF;
        memcpy(&tem8, dataByte+3, 1);
        _privateIdentity = tem8;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _status;
    NSData *data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    UInt16 tem16 = _netKeyIndex << 4;
    data = [NSData dataWithBytes:&tem16 length:2];
    [mData appendData:data];
    tem8 = _privateIdentity;
    data = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:data];
    return mData;
}

@end
