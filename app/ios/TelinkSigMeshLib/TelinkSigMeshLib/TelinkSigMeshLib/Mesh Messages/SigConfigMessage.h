/********************************************************************************************************
 * @file     SigConfigMessage.h
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

NS_ASSUME_NONNULL_BEGIN

@class SigPublish,SigModelIDModel,SigNodeFeatures,SigConfigModelPublicationVirtualAddressSet;

@interface SigConfigMessage : SigStaticMeshMessage

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

/// Encodes given list of Key Indexes into a Data.
/// As each Key Index is 12 bits long, a pair of them can fit 3 bytes.
/// This method ensures that they are packed in compliance to the Bluetooth Mesh specification.
/// @param limit Maximum number of Key Indexes to encode.
/// @param indexes An array of 12-bit Key Indexes.
/// @returns Key Indexes encoded to a Data.
- (NSData *)encodeLimit:(int)limit indexes:(NSArray <NSNumber *>*)indexes;

/// Encodes given list of Key Indexes into a Data.
/// As each Key Index is 12 bits long, a pair of them can fit 3 bytes.
/// This method ensures that they are packed in compliance to the Bluetooth Mesh specification.
/// Maximum number of Key Indexes to decode is 10000.
/// @param indexes An array of 12-bit Key Indexes.
/// @returns Key Indexes encoded to a Data.
- (NSData *)encodeIndexes:(NSArray <NSNumber *>*)indexes;

/// Decodes number of Key Indexes from the given Data from the given offset.
/// This will decode as many Indexes as possible, until the end of data is reached.
/// @param limit Maximum number of Key Indexes to decode.
/// @param data The data from where the indexes should be read.
/// @param offset The offset from where to read the indexes.
/// @returns Decoded Key Indexes.
+ (NSArray <NSNumber *>*)decodeLimit:(int)limit indexesFromData:(NSData *)data atOffset:(int)offset;

/// Decodes number of Key Indexes from the given Data from the given offset.
/// This will decode as many Indexes as possible, until the end of data is reached.
/// Maximum number of Key Indexes to decode is 10000.
/// @param data The data from where the indexes should be read.
/// @param offset The offset from where to read the indexes.
/// @returns Decoded Key Indexes.
+ (NSArray <NSNumber *>*)decodeIndexesFromData:(NSData *)data atOffset:(int)offset;

/// By default, all Config Messages will be sent as segmented to make them more reliable.
//- (BOOL)isSegmented;

@end


@interface SigConfigStatusMessage : SigConfigMessage
/// Operation status.
@property (nonatomic,assign) SigConfigMessageStatus status;
/// Returns whether the operation was successful or not.
@property (nonatomic,assign) BOOL isSuccess;
/// The status as String.
@property (nonatomic,strong,readonly) NSString *message;
@end

@interface SigAcknowledgedConfigMessage : SigConfigMessage
@end

@interface SigConfigNetKeyMessage : SigConfigMessage
/// The Network Key Index.
@property (nonatomic,assign) UInt16 networkKeyIndex;

/// Encodes Network Key Index in 2 bytes using Little Endian.
/// @returns Key Index encoded in 2 bytes.
- (NSData *)encodeNetKeyIndex;

/// Decodes the Network Key Index from 2 bytes at given offset.
/// There are no any checks whether the data at the given offset are valid, or even if the offset is not outside of the data range.
/// @param data The data from where the indexes should be read.
/// @param offset The offset from where to read the indexes.
/// @returns Decoded Key Index.
+ (UInt16)decodeNetKeyIndexFromData:(NSData *)data atOffset:(int)offset;

@end


@interface SigConfigAppKeyMessage : SigConfigMessage
/// Application Key Index.
@property (nonatomic,assign) UInt16 applicationKeyIndex;
@end


@interface SigConfigNetAndAppKeyMessage : SigConfigMessage
/// The Network Key Index.
@property (nonatomic,assign) UInt16 networkKeyIndex;
/// Application Key Index.
@property (nonatomic,assign) UInt16 applicationKeyIndex;

- (instancetype)initWithNetworkKeyIndex:(UInt16)networkKeyIndex applicationKeyIndex:(UInt16)applicationKeyIndex;

/// Encodes Network Key Index and Application Key Index in 3 bytes using Little Endian.
/// @returns Key Indexes encoded in 3 bytes.
- (NSData *)encodeNetAndAppKeyIndex;

/// Decodes the Network Key Index and Application Key Index from 3 bytes at given offset.
/// There are no any checks whether the data at the given offset are valid, or even if the offset is not outside of the data range.
/// @param data The data from where the indexes should be read.
/// @param offset The offset from where to read the indexes.
/// @returns Decoded Key Indexes.
+ (nullable SigConfigNetAndAppKeyMessage *)decodeNetAndAppKeyIndexFromData:(NSData *)data atOffset:(int)offset;

@end


@interface SigConfigElementMessage : SigConfigMessage
/// The Unicast Address of the Model's parent Element.
@property (nonatomic,assign) UInt16 elementAddress;
@end


@interface SigConfigModelMessage : SigConfigElementMessage
/// The 16-bit Model identifier.
@property (nonatomic,assign) UInt16 modelIdentifier;
/// The 32-bit Model identifier.
@property (nonatomic,assign) UInt32 modelId;
@end


@interface SigConfigAnyModelMessage : SigConfigModelMessage
/// The Company identified, as defined in Assigned Numbers, or `nil`, if the Model is defined in Bluetooth Mesh Model Specification.
///
/// - seeAlso: https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers/
@property (nonatomic,assign) UInt16 companyIdentifier;
@end


@interface SigConfigVendorModelMessage : SigConfigModelMessage
/// The Company identified, as defined in Assigned Numbers.
///
/// - seeAlso: https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers/
@property (nonatomic,assign) UInt16 companyIdentifier;
@end


@interface SigConfigAddressMessage : SigConfigMessage
/// Value of the Address.
@property (nonatomic,assign,readonly) UInt16 address;
@end


@interface SigConfigVirtualLabelMessage : SigConfigMessage
/// Value of the 128-bt Virtual Label UUID.
@property (nonatomic,strong,readonly) CBUUID *virtualLabel;
@end


@interface SigConfigModelAppList : SigConfigStatusMessage
/// The 16-bit Model identifier.
@property (nonatomic,assign) UInt16 modelIdentifier;
/// The 32-bit Model identifier.
@property (nonatomic,assign) UInt32 modelId;
/// Application Key Indexes bound to the Model.
@property (nonatomic,strong) NSMutableArray <NSNumber *>*applicationKeyIndexes;//[KeyIndex]
@end


@interface SigConfigModelSubscriptionList : SigConfigStatusMessage
/// The 16-bit Model identifier.
@property (nonatomic,assign) UInt16 modelIdentifier;
/// The 32-bit Model identifier.
@property (nonatomic,assign) UInt32 modelId;
/// A list of Addresses.
@property (nonatomic,strong) NSMutableArray <NSNumber *>*addresses;//[Address]
@end


@interface SigCompositionDataPage : NSObject
/// Page number of the Composition Data to get.
@property (nonatomic,assign) UInt8 page;
/// Composition Data parameters as Data.
@property (nonatomic,strong) NSData *parameters;
@end

@interface SigPage0 : SigCompositionDataPage
/// The 16-bit Company Identifier (CID) assigned by the Bluetooth SIG.
/// The value of this property is obtained from node composition data.
@property (nonatomic,assign) UInt16 companyIdentifier;
/// The 16-bit vendor-assigned Product Identifier (PID).
/// The value of this property is obtained from node composition data.
@property (nonatomic,assign) UInt16 productIdentifier;
/// The 16-bit vendor-assigned Version Identifier (VID).
/// The value of this property is obtained from node composition data.
@property (nonatomic,assign) UInt16 versionIdentifier;
/// The minimum number of Replay Protection List (RPL) entries for this node.
/// The value of this property is obtained from node composition data.
@property (nonatomic,assign) UInt16 minimumNumberOfReplayProtectionList;
/// Node's features. See `NodeFeatures` for details.
@property (nonatomic,strong) SigNodeFeatures *features;
/// An array of node's elements.
@property (nonatomic,strong) NSMutableArray <SigElementModel *>*elements;

- (BOOL)isSegmented;

/// This initializer constructs the Page 0 of Composition Data from the given Node.
/// @param node  The Node to construct the Page 0 from.
- (instancetype)initWithNode:(SigNodeModel *)node;

/// This initializer should construct the message based on the received parameters.
/// @param parameters  The Access Layer parameters.
- (instancetype)initWithParameters:(NSData *)parameters;

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
@interface SigConfigAppKeyAdd : SigConfigNetAndAppKeyMessage
/// The 128-bit Application Key data.
/// The NetKeyIndexAndAppKeyIndex field contains two indexes that shall identify
/// the global NetKey Index of the NetKey and the global AppKey Index of the AppKey.
/// These two indexes shall be encoded as defined in Section 4.3.1.1
/// using NetKey Index as first key index and AppKey Index as second key index.
/// The AppKey field shall contain the AppKey value identified by the AppKeyIndex.
@property (nonatomic,strong) NSData *key;

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
- (instancetype)initWithApplicationKey:(SigAppkeyModel *)applicationKey;

/**
 * @brief   Initialize SigConfigAppKeyAdd object.
 * @param   parameters    the hex data of SigConfigAppKeyAdd.
 * @return  return `nil` when initialize SigConfigAppKeyAdd object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

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
@interface SigConfigAppKeyUpdate : SigConfigNetAndAppKeyMessage
/// The 128-bit Application Key data.
/// The NetKeyIndexAndAppKeyIndex field contains two indexes that shall identify
/// the global NetKey Index of the NetKey and the global AppKey Index of the AppKey.
/// These two indexes shall be encoded as defined in Section 4.3.1.1
/// using NetKey Index as first key index and AppKey Index as second key index.
/// The AppKeyIndex shall be bound to the NetKeyIndex.
/// The AppKey field shall contain the new value of the AppKey, identified by the AppKeyIndex.
@property (nonatomic,strong) NSData *key;

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
- (instancetype)initWithApplicationKey:(SigAppkeyModel *)applicationKey;

/**
 * @brief   Initialize SigConfigAppKeyUpdate object.
 * @param   parameters    the hex data of SigConfigAppKeyUpdate.
 * @return  return `nil` when initialize SigConfigAppKeyUpdate object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.5 Config Composition Data Status, opcode:0x02


/// The Config Composition Data Status is an unacknowledged message
/// used to report a single page of the Composition Data (see Section 4.2.1).
/// @note   This message uses a single octet opcode to maximize the size of a payload.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.155), 4.3.2.5 Config Composition Data Status.
@interface SigConfigCompositionDataStatus : SigConfigMessage
/// The Composition Data page.
@property (nonatomic,strong) SigCompositionDataPage *page;

///**
// * @brief   Initialize SigConfigCompositionDataStatus object.
// * @param   page    The Composition Data page.
// * @return  return `nil` when initialize SigConfigCompositionDataStatus object fail.
// */
//- (instancetype)initWithReportPage:(SigCompositionDataPage *)page;

/**
 * @brief   Initialize SigConfigCompositionDataStatus object.
 * @param   parameters    the hex data of SigConfigCompositionDataStatus.
 * @return  return `nil` when initialize SigConfigCompositionDataStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

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
@interface SigConfigModelPublicationSet : SigConfigAnyModelMessage
/// The config parameter of publication.
@property (nonatomic,strong) SigPublish *publish;

/**
 * @brief   Initialize SigConfigModelPublicationSet object.
 * @param   publish    The config parameter of publication.
 * @param   elementAddress    The ElementAddress field is the unicast address of the element, all other address types are Prohibited.
 * @param   modelIdentifier    The model ID.
 * @param   companyIdentifier    The company ID.
 * @return  return `nil` when initialize SigConfigModelPublicationSet object fail.
 */
- (instancetype)initWithPublish:(SigPublish *)publish toElementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier;

/**
 * @brief   Initialize SigConfigModelPublicationSet object.
 * @param   publish    The config parameter of publication.
 * @param   elementAddress    The ElementAddress field is the unicast address of the element, all other address types are Prohibited.
 * @param   model    The SigModelIDModel object.
 * @return  return `nil` when initialize SigConfigModelPublicationSet object fail.
 */
- (instancetype)initWithPublish:(SigPublish *)publish toElementAddress:(UInt16)elementAddress model:(SigModelIDModel *)model;

/**
 * @brief   Initialize SigConfigModelPublicationSet object.
 * @param   parameters    the hex data of SigConfigModelPublicationSet.
 * @return  return `nil` when initialize SigConfigModelPublicationSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.39 Config AppKey Delete, opcode:0x8000


/// The Config AppKey Delete is an acknowledged message used
/// to delete an AppKey from the AppKey List on a node.
/// @note   The response to a Config AppKey Delete message is a Config AppKey Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.170), 4.3.2.39 Config AppKey Delete.
@interface SigConfigAppKeyDelete : SigConfigNetAndAppKeyMessage

/**
 * @brief   Initialize SigConfigAppKeyDelete object.
 * @param   applicationKey    the application key need to delete.
 * @return  return `nil` when initialize SigConfigAppKeyDelete object fail.
 */
- (instancetype)initWithApplicationKey:(SigAppkeyModel *)applicationKey;

/**
 * @brief   Initialize SigConfigAppKeyDelete object.
 * @param   parameters    the hex data of SigConfigAppKeyDelete.
 * @return  return `nil` when initialize SigConfigAppKeyDelete object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.41 Config AppKey Get, opcode:0x8001


/// The AppKey Get is an acknowledged message used
/// to report all AppKeys bound to the NetKey.
/// @note   The response to a Config AppKey Get message is a Config AppKey List message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.171), 4.3.2.41 Config AppKey Get.
@interface SigConfigAppKeyGet : SigConfigNetKeyMessage

/**
 * @brief   Initialize SigConfigAppKeyGet object.
 * @param   networkKeyIndex    the index of network key.
 * @return  return `nil` when initialize SigConfigAppKeyGet object fail.
 */
- (instancetype)initWithNetworkKeyIndex:(UInt16)networkKeyIndex;

/**
 * @brief   Initialize SigConfigAppKeyGet object.
 * @param   parameters    the hex data of SigConfigAppKeyGet.
 * @return  return `nil` when initialize SigConfigAppKeyGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.42 Config AppKey List, opcode:0x8002


/// The Config AppKey List is an unacknowledged message
/// reporting all AppKeys that are bound to the NetKey.
/// @note   The response to a Config AppKey Get message is a Config AppKey List message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.171), 4.3.2.42 Config AppKey List.
@interface SigConfigAppKeyList : SigConfigNetKeyMessage
/// Status Code for the requesting message.
/// The Status field shall identify the Status Code for the last operation on the AppKey of the NetKey.
/// The allowed values for Status codes and their meanings are documented in Section 4.3.5.
@property (nonatomic,assign,readonly) SigConfigMessageStatus status;
/// Returns whether the operation was successful or not.
@property (nonatomic,assign,readonly) BOOL isSuccess;
/// The status as String.
@property (nonatomic,strong,readonly) NSString *message;

/// A list of AppKey indexes that are bound to the NetKey identified by NetKeyIndex.
/// The AppKeyIndexes field shall contain all AppKey indexes that are bound to the NetKey.
/// The AppKey indexes shall be encoded as defined in Section 4.3.1.1.
@property (nonatomic,strong) NSMutableArray <NSNumber *>*applicationKeyIndexes;//[KeyIndex]

//- (instancetype)initWithNetWorkKey:(SigNetkeyModel *)networkKey applicationKeys:(NSArray <SigAppkeyModel *>*)applicationKeys status:(SigConfigMessageStatus)status;
//
//- (instancetype)initWithStatus:(SigConfigMessageStatus)status forMessage:(SigConfigAppKeyGet *)message;

/**
 * @brief   Initialize SigConfigAppKeyList object.
 * @param   parameters    the hex data of SigConfigAppKeyList.
 * @return  return `nil` when initialize SigConfigAppKeyList object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.2.40 Config AppKey Status, opcode:0x8003


/// The Config AppKey Status is an unacknowledged message used to
/// report a status for the requesting message,
/// based on the NetKey Index identifying the NetKey on the NetKey List
/// and on the AppKey Index identifying the AppKey on the AppKey List.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.170), 4.3.2.40 Config AppKey Status.
@interface SigConfigAppKeyStatus : SigConfigNetAndAppKeyMessage
/// Status Code for the requesting message
/// The Status field shall identify the Status Code for the last operation on the AppKey List.
/// The allowed values for Status codes and their meanings are documented in Section 4.3.5.
/// The Status Code shall be Success if the received request was redundant (add of an identical existing key,
/// update of an identical updated key, or delete of a non-existent key), with no further action taken.
@property (nonatomic,assign) SigConfigMessageStatus status;

//- (instancetype)initWithApplicationKey:(SigAppkeyModel *)applicationKey;
//
//- (instancetype)initWithStatus:(SigConfigMessageStatus)status forMessage:(SigConfigNetAndAppKeyMessage *)message;

/**
 * @brief   Initialize SigConfigAppKeyStatus object.
 * @param   parameters    the hex data of SigConfigAppKeyStatus.
 * @return  return `nil` when initialize SigConfigAppKeyStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.2.4 Config Composition Data Get, opcode:0x8008


/// The Config Composition Data Get is an acknowledged message used
/// to read one page of the Composition Data (see Section 4.2.1).
/// @note   The response to a Config Composition Data Get message
/// is a Config Composition Data Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.154), 4.3.2.4 Config Composition Data Get.
@interface SigConfigCompositionDataGet : SigConfigMessage
/// Page number of the Composition Data.
/// The Page field shall identify the Composition Data Page number that is being read.
@property (nonatomic,assign) UInt8 page;

- (instancetype)initWithPage:(UInt8)page;

/**
 * @brief   Initialize SigConfigCompositionDataGet object.
 * @param   parameters    the hex data of SigConfigCompositionDataGet.
 * @return  return `nil` when initialize SigConfigCompositionDataGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.1 Config Beacon Get, opcode:0x8009


/// The Config Beacon Get is an acknowledged message used to get
/// the current Secure Network Beacon state of a node (see Section 4.2.10).
/// @note   The response to a Config Beacon Get message is a Config Beacon Status message.
/// @note   There are no Parameters for this message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.154), 4.3.2.1 Config Beacon Get.
@interface SigConfigBeaconGet : SigConfigMessage

/**
 * @brief   Initialize SigConfigBeaconGet object.
 * @param   parameters    the hex data of SigConfigBeaconGet.
 * @return  return `nil` when initialize SigConfigBeaconGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.2 Config Beacon Set, opcode:0x800A


/// The Config Beacon Set is an acknowledged message used to set
/// the Secure Network Beacon state of a node (see Section 4.2.10).
/// @note   The response to a Config Beacon Set message is a Config Beacon Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.154), 4.3.2.2 Config Beacon Set.
@interface SigConfigBeaconSet : SigConfigMessage
/// New Secure Network Beacon state.
/// The Beacon field shall provide the new Secure Network Beacon state of the node (see Section 4.2.10).
@property (nonatomic,assign) BOOL state;

/// Configures the Secure Network Beacon behavior on the Node.
/// @param enable `True` to enable Secure Network Beacon feature,  `false` to disable.
- (instancetype)initWithEnable:(BOOL)enable;

/**
 * @brief   Initialize SigConfigBeaconSet object.
 * @param   parameters    the hex data of SigConfigBeaconSet.
 * @return  return `nil` when initialize SigConfigBeaconSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.3 Config Beacon Status, opcode:0x800B


/// The Config Beacon Status is an unacknowledged message used to report
/// the current Secure Network Beacon state of a node (see Section 4.2.10).
/// @note   - seeAlso: Mesh_v1.0.pdf (page.154), 4.3.2.3 Config Beacon Status.
@interface SigConfigBeaconStatus : SigConfigMessage
/// Secure Network Beacon state.
/// The Beacon field shall provide the current Secure Network Beacon state of the node (see Section 4.2.10).
@property (nonatomic,assign) BOOL isEnabled;

/// Configures the Secure Network Beacon behavior on the Node.
/// @param enable `True` to enable Secure Network Beacon feature, `false` to disable.
- (instancetype)initWithEnable:(BOOL)enable;

/**
 * @brief   Initialize SigConfigBeaconStatus object.
 * @param   parameters    the hex data of SigConfigBeaconStatus.
 * @return  return `nil` when initialize SigConfigBeaconStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.2.6 Config Default TTL Get, opcode:0x800C


/// The Config Default TTL Get is an acknowledged message used to get
/// the current Default TTL state of a node.
/// @note   The response to a Config Default TTL Get message is a
/// Config Default TTL Status message.
/// @note   There are no Parameters for this message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.155), 4.3.2.6 Config Default TTL Get.
@interface SigConfigDefaultTtlGet : SigConfigMessage

/**
 * @brief   Initialize SigConfigDefaultTtlGet object.
 * @param   parameters    the hex data of SigConfigDefaultTtlGet.
 * @return  return `nil` when initialize SigConfigDefaultTtlGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.7 Config Default TTL Set, opcode:0x800D


/// The Config Default TTL Set is an acknowledged message used to set
/// the Default TTL state of a node (see Section 4.2.7).
/// @note   The response to a Config Default TTL Set message is a
/// Config Default TTL Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.155), 4.3.2.7 Config Default TTL Set.
@interface SigConfigDefaultTtlSet : SigConfigMessage
/// The Time To Live (TTL) value. Valid value is in range 0x00, 0x02...0x7F.
/// New Default TTL value.
/// The TTL field shall identify a new Default TTL for the node (see Section 4.2.7).
@property (nonatomic,assign) UInt8 ttl;

- (instancetype)initWithTtl:(UInt8)ttl;

/**
 * @brief   Initialize SigConfigDefaultTtlSet object.
 * @param   parameters    the hex data of SigConfigDefaultTtlSet.
 * @return  return `nil` when initialize SigConfigDefaultTtlSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.8 Config Default TTL Status, opcode:0x800E


/// The Config Default TTL Status is an unacknowledged message used to report
/// the current Default TTL state of a node (see Section 4.2.7).
/// @note   - seeAlso: Mesh_v1.0.pdf (page.155), 4.3.2.8 Config Default TTL Status.
@interface SigConfigDefaultTtlStatus : SigConfigMessage
/// The Time To Live (TTL) value. Valid value is in range 1...127.
/// Default TTL.
/// The TTL field shall identify the Default TTL for the node, as defined in Default TTL (see Section 4.2.7).
@property (nonatomic,assign) UInt8 ttl;

//- (instancetype)initWithTtl:(UInt8)ttl;

/**
 * @brief   Initialize SigConfigDefaultTtlStatus object.
 * @param   parameters    the hex data of SigConfigDefaultTtlStatus.
 * @return  return `nil` when initialize SigConfigDefaultTtlStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.2.55 Config Friend Get, opcode:0x800F


/// The Config Friend Get is an acknowledged message used to get
/// the current Friend state of a node (see Section 4.2.13).
/// @note   The response to a Config Friend Get message is a Config Friend Status message.
/// @note   There are no Parameters for this message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.176), 4.3.2.55 Config Friend Get.
@interface SigConfigFriendGet : SigConfigMessage

/**
 * @brief   Initialize SigConfigFriendGet object.
 * @param   parameters    the hex data of SigConfigFriendGet.
 * @return  return `nil` when initialize SigConfigFriendGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.56 Config Friend Set, opcode:0x8010


/// The Config Friend Set is an acknowledged message used to set
/// the Friend state of a node (see Section 4.2.13).
/// @note   The response to a Config Friend Set message is a Config Friend Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.176), 4.3.2.56 Config Friend Set.
@interface SigConfigFriendSet : SigConfigMessage
/// New Friend state.
/// The Friend field shall provide the new Friend state of the node (see Section 4.2.13).
@property (nonatomic,assign) SigNodeFeaturesState state;

- (instancetype)initWithEnable:(BOOL)enable;

/**
 * @brief   Initialize SigConfigFriendSet object.
 * @param   parameters    the hex data of SigConfigFriendSet.
 * @return  return `nil` when initialize SigConfigFriendSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.57 Config Friend Status, opcode:0x8011


/// The Config Friend Status is an unacknowledged message used to report
/// the current Friend state of a node (see Section 4.2.13).
/// @note   - seeAlso: Mesh_v1.0.pdf (page.177), 4.3.2.57 Config Friend Status.
@interface SigConfigFriendStatus : SigConfigMessage
/// Friend state.
/// The Friend field shall provide the current Friend state of the node (see Section 4.2.13).
@property (nonatomic,assign) SigNodeFeaturesState state;

///// Creates the Config Friend Status message.
/////
///// - parameter state: The Friend state of the Node.
//- (instancetype)initWithState:(SigNodeFeaturesState)state;
//
//- (instancetype)initWithNode:(SigNodeModel *)node;

/**
 * @brief   Initialize SigConfigFriendStatus object.
 * @param   parameters    the hex data of SigConfigFriendStatus.
 * @return  return `nil` when initialize SigConfigFriendStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.2.9 Config GATT Proxy Get, opcode:0x8012


/// The Config GATT Proxy Get is an acknowledged message used to get
/// the current GATT Proxy state of a node (see Section 4.2.11).
/// @note   The response to a Config GATT Proxy Get message is a Config GATT Proxy Status message.
/// @note   There are no Parameters for this message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.156), 4.3.2.9 Config GATT Proxy Get.
@interface SigConfigGATTProxyGet : SigConfigMessage

/**
 * @brief   Initialize SigConfigGATTProxyGet object.
 * @param   parameters    the hex data of SigConfigGATTProxyGet.
 * @return  return `nil` when initialize SigConfigGATTProxyGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.10 Config GATT Proxy Set, opcode:0x8013


/// The Config GATT Proxy Set is an acknowledged message used to set
/// the GATT Proxy state of a node (see Section 4.2.11).
/// @note   The response to a Config GATT Proxy Set message is a Config GATT Proxy Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.156), 4.3.2.10 Config GATT Proxy Set.
@interface SigConfigGATTProxySet : SigConfigMessage
/// New GATT Proxy state.
/// The GATTProxy field shall provide the new GATT Proxy state of the node (see Section 4.2.11).
@property (nonatomic,assign) SigNodeFeaturesState state;

/// The interval between retransmissions, in seconds.
/// Configures the GATT Proxy on the Node.
///
/// When disabled, the Node will no longer be able to work as a GATT Proxy
/// until enabled again.
///
/// - parameter enable: `True` to enable GATT Proxy feature, `false` to disable.
- (instancetype)initWithEnable:(BOOL)enable;

/**
 * @brief   Initialize SigConfigGATTProxySet object.
 * @param   parameters    the hex data of SigConfigGATTProxySet.
 * @return  return `nil` when initialize SigConfigGATTProxySet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.11 Config GATT Proxy Status, opcode:0x8014


/// The Config GATT Proxy Status is an unacknowledged message used to report
/// the current GATT Proxy state of a node (see Section 4.2.11).
/// @note   - seeAlso: Mesh_v1.0.pdf (page.156), 4.3.2.11 Config GATT Proxy Status.
@interface SigConfigGATTProxyStatus : SigConfigMessage
/// GATT Proxy state.
/// The GATTProxy field shall provide the current GATT Proxy state of the node (see Section 4.2.11).
@property (nonatomic,assign) SigNodeGATTProxyState state;

///// Creates the Config GATT Proxy Status message.
/////
///// - parameter state: The GATT Proxy state of the Node.
//- (instancetype)initWithState:(SigNodeFeaturesState)state;
//
//- (instancetype)initWithNode:(SigNodeModel *)node;

/**
 * @brief   Initialize SigConfigGATTProxyStatus object.
 * @param   parameters    the hex data of SigConfigGATTProxyStatus.
 * @return  return `nil` when initialize SigConfigGATTProxyStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.2.58 Config Key Refresh Phase Get, opcode:0x8015


/// The Config Key Refresh Phase Get is an acknowledged message used to get
/// the current Key Refresh Phase state of the identified network key.
/// @note   The response to a Config Key Refresh Phase Get message is a Config Key Refresh Phase Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.177), 4.3.2.58 Config Key Refresh Phase Get.
@interface SigConfigKeyRefreshPhaseGet : SigConfigMessage
/// NetKey Index.
/// The NetKeyIndex field shall identify the global NetKey Index of the NetKey.
/// The NetKeyIndex field shall be encoded as defined in Section 4.3.1.1.
@property (nonatomic,assign) UInt16 netKeyIndex;

/**
 * @brief   Initialize SigConfigKeyRefreshPhaseGet object.
 * @param   parameters    the hex data of SigConfigKeyRefreshPhaseGet.
 * @return  return `nil` when initialize SigConfigKeyRefreshPhaseGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithNetKeyIndex:(UInt16)netKeyIndex;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.59 Config Key Refresh Phase Set, opcode:0x8016


/// The Config Key Refresh Phase Set is an acknowledged message used to set
/// the Key Refresh Phase state of the identified network key (see Section 4.2.14).
/// @note   The response to a Config Key Refresh Phase Get message is a Config Key Refresh Phase Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.177), 4.3.2.59 Config Key Refresh Phase Set.
@interface SigConfigKeyRefreshPhaseSet : SigConfigMessage
/// NetKey Index.
/// The NetKeyIndex field shall identify the global NetKey Index of the NetKey.
/// The NetKeyIndex field shall be encoded as defined in Section 4.3.1.1.
@property (nonatomic,assign) UInt16 netKeyIndex;
/// New Key Refresh Phase Transition.
/// The Transition field shall identify the Key Refresh Phase Transitions (see Section 4.2.14, Table 4.18)
/// allowed for each given starting state. All other transition values are Prohibited.
@property (nonatomic,assign) SigControllableKeyRefreshTransitionValues transition;

/**
 * @brief   Initialize SigConfigKeyRefreshPhaseSet object.
 * @param   parameters    the hex data of SigConfigKeyRefreshPhaseSet.
 * @return  return `nil` when initialize SigConfigKeyRefreshPhaseSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithNetKeyIndex:(UInt16)netKeyIndex transition:(SigControllableKeyRefreshTransitionValues)transition;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.60 Config Key Refresh Phase Status, opcode:0x8017


/// The Config Key Refresh Phase Status is an unacknowledged message used to report
/// the current Key Refresh Phase state of the identified network key (see Section 4.2.14).
/// @note   - seeAlso: Mesh_v1.0.pdf (page.177), 4.3.2.60 Config Key Refresh Phase Status.
@interface SigConfigKeyRefreshPhaseStatus : SigConfigMessage
/// Status Code for the requesting message.
/// The Status field shall identify the Status Code for the requesting message.
/// The allowed values for Status codes and their meanings are documented in Section 4.3.5.
/// The Status Code shall be Success if the received request was redundant (the requested
/// phase transition has already occurred), with no further action taken.
@property (nonatomic,assign) SigConfigMessageStatus status;
/// NetKey Index.
/// The NetKeyIndex field shall identify the global NetKey Index of the NetKey.
/// The NetKeyIndex field shall be encoded as defined in Section 4.3.1.1.
@property (nonatomic,assign) UInt16 netKeyIndex;
/// Key Refresh Phase State.
/// The Phase field shall identify the Key Refresh Phase state for the node,
/// as defined in Key Refresh Phase (see Section 4.2.14).
@property (nonatomic, assign) KeyRefreshPhase phase;

/**
 * @brief   Initialize SigConfigKeyRefreshPhaseStatus object.
 * @param   parameters    the hex data of SigConfigKeyRefreshPhaseStatus.
 * @return  return `nil` when initialize SigConfigKeyRefreshPhaseStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.2.15 Config Model Publication Get, opcode:0x8018


/// The Config Model Publication Get is an acknowledged message used to get
/// the publish address and parameters of an outgoing message that originates from a model.
/// @note   The response to a Config Model Publication Get message is a Config Model Publication Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.157), 4.3.2.15 Config Model Publication Get.
@interface SigConfigModelPublicationGet : SigConfigAnyModelMessage

- (instancetype)initWithElementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier;

/**
 * @brief   Initialize SigConfigModelPublicationGet object.
 * @param   parameters    the hex data of SigConfigModelPublicationGet.
 * @return  return `nil` when initialize SigConfigModelPublicationGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.18 Config Model Publication Status, opcode:0x8019


/// The Config Model Publication Status is an unacknowledged message used to report
/// the model Publication state (see Section 4.2.2) of an outgoing message that is published by the model.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.160), 4.3.2.18 Config Model Publication Status.
@interface SigConfigModelPublicationStatus : SigConfigAnyModelMessage
@property (nonatomic,strong) SigPublish *publish;
/// Status Code for the requesting message
/// The Status field shall identify the Status Code for the last operation on Config Model Publication parameters.
/// The allowed values for Status codes and their meanings are documented in Section 4.3.5.
@property (nonatomic,assign,readonly) SigConfigMessageStatus status;

//- (instancetype)initResponseToSigConfigAnyModelMessage:(SigConfigAnyModelMessage *)request;
//
- (instancetype)initResponseToSigConfigAnyModelMessage:(SigConfigAnyModelMessage *)request withPublish:(SigPublish *)publish;
//
//- (instancetype)initResponseToSigConfigAnyModelMessage:(SigConfigAnyModelMessage *)request withStatus:(SigConfigMessageStatus)status;
//
//- (instancetype)initWithConfirmSigConfigModelPublicationSet:(SigConfigModelPublicationSet *)request;
//
//- (instancetype)initWithConfirmSigConfigModelPublicationVirtualAddressSet:(SigConfigModelPublicationVirtualAddressSet *)request;

/**
 * @brief   Initialize SigConfigModelPublicationStatus object.
 * @param   parameters    the hex data of SigConfigModelPublicationStatus.
 * @return  return `nil` when initialize SigConfigModelPublicationStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.2.17 Config Model Publication Virtual Address Set, opcode:0x801A


/// The Config Model Publication Virtual Address Set is an acknowledged message used to set
/// the model Publication state (see Section 4.2.2) of an outgoing message that originates from a model.
/// @note   The response to a Config Model Publication Virtual Address Set message is a
/// Config Model Publication Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.159), 4.3.2.17 Config Model Publication Virtual Address Set.
@interface SigConfigModelPublicationVirtualAddressSet : SigConfigAnyModelMessage
@property (nonatomic,strong) SigPublish *publish;

- (instancetype)initWithPublish:(SigPublish *)publish toElementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier;

- (instancetype)initWithPublish:(SigPublish *)publish toElementAddress:(UInt16)elementAddress model:(SigModelIDModel *)model;

/**
 * @brief   Initialize SigConfigModelPublicationVirtualAddressSet object.
 * @param   parameters    the hex data of SigConfigModelPublicationVirtualAddressSet.
 * @return  return `nil` when initialize SigConfigModelPublicationVirtualAddressSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.19 Config Model Subscription Add, opcode:0x801B


/// The Config Model Subscription Add is an acknowledged message used to add
/// an address to a Subscription List of a model (see Section 4.2.3).
/// @note   The response to a Config Model Subscription Add message is a
/// Config Model Subscription Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.161), 4.3.2.19 Config Model Subscription Add.
@interface SigConfigModelSubscriptionAdd : SigConfigAnyModelMessage
/// Value of the address.
/// The Address field shall contain the new address to be added to the Subscription List.
/// The value of the Address field shall not be an unassigned address, unicast address,
/// all-nodes address or virtual address.
@property (nonatomic,assign) UInt16 address;

- (instancetype)initWithGroupAddress:(UInt16)groupAddress toElementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier;

- (instancetype)initWithGroup:(SigGroupModel *)group toElementAddress:(UInt16)elementAddress model:(SigModelIDModel *)model;

/**
 * @brief   Initialize SigConfigModelSubscriptionAdd object.
 * @param   parameters    the hex data of SigConfigModelSubscriptionAdd.
 * @return  return `nil` when initialize SigConfigModelSubscriptionAdd object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.21 Config Model Subscription Delete, opcode:0x801C


/// The Config Model Subscription Delete is an acknowledged message used to delete
/// a subscription address from the Subscription List of a model (see Section 4.2.3).
/// @note   The response to a Config Model Subscription Delete message is a
/// Config Model Subscription Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.162), 4.3.2.21 Config Model Subscription Delete.
@interface SigConfigModelSubscriptionDelete : SigConfigAnyModelMessage
/// Value of the Address.
/// The Address field shall identify the address to be removed from the Subscription List.
/// The value of the Address field shall not be an unassigned address, unicast address,
/// all-nodes address or virtual address.
@property (nonatomic,assign) UInt16 address;

- (instancetype)initWithGroupAddress:(UInt16)groupAddress elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier;

- (instancetype)initWithGroup:(SigGroupModel *)group fromModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress;

/**
 * @brief   Initialize SigConfigModelSubscriptionDelete object.
 * @param   parameters    the hex data of SigConfigModelSubscriptionDelete.
 * @return  return `nil` when initialize SigConfigModelSubscriptionDelete object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.25 Config Model Subscription Delete All, opcode:0x801D


/// The Config Model Subscription Delete All is an acknowledged message used to discard
/// the Subscription List of a model (see Section 4.2.3).
/// @note   The response to a Config Model Subscription Delete All message is a
/// Config Model Subscription Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.164), 4.3.2.25 Config Model Subscription Delete All.
@interface SigConfigModelSubscriptionDeleteAll : SigConfigAnyModelMessage

- (instancetype)initWithElementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier;

- (instancetype)initFromModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress;

/**
 * @brief   Initialize SigConfigModelSubscriptionDeleteAll object.
 * @param   parameters    the hex data of SigConfigModelSubscriptionDeleteAll.
 * @return  return `nil` when initialize SigConfigModelSubscriptionDeleteAll object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.23 Config Model Subscription Overwrite, opcode:0x801E


/// The Config Model Subscription Overwrite is an acknowledged message used to
/// discard the Subscription List and add an address to the cleared Subscription
/// List of a model (see Section 4.2.3).
/// @note   The response to a Config Model Subscription Overwrite message is a
/// Config Model Subscription Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.163), 4.3.2.23 Config Model Subscription Overwrite.
@interface SigConfigModelSubscriptionOverwrite : SigConfigAnyModelMessage
/// Value of the Address.
/// The Address field shall contain the new address to be added to the Subscription List.
/// The value of the Address field shall not be an unassigned address, unicast address,
/// all-nodes address or virtual address.
@property (nonatomic,assign) UInt16 address;

- (instancetype)initWithGroupAddress:(UInt16)groupAddress elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier;

- (instancetype)initWithGroup:(SigGroupModel *)group toModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress;

/**
 * @brief   Initialize SigConfigModelSubscriptionOverwrite object.
 * @param   parameters    the hex data of SigConfigModelSubscriptionOverwrite.
 * @return  return `nil` when initialize SigConfigModelSubscriptionOverwrite object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.26 Config Model Subscription Status, opcode:0x801F


/// The Config Model Subscription Status is an unacknowledged message used to report
/// a status of the operation on the Subscription List (see Section 4.2.3).
/// @note   - seeAlso: Mesh_v1.0.pdf (page.165), 4.3.2.26 Config Model Subscription Status.
@interface SigConfigModelSubscriptionStatus : SigConfigAnyModelMessage
/// Status Code for the requesting message.
/// The Status field shall identify the Status Code for the last operation on the Subscription List.
/// The allowed values for Status codes and their meanings are documented in Section 4.3.5.
@property (nonatomic,assign) SigConfigMessageStatus status;
/// Returns whether the operation was successful or not.
@property (nonatomic,assign,readonly) BOOL isSuccess;
/// The status as String.
@property (nonatomic,strong,readonly) NSString *message;
/// Value of the address.
/// The value of the Address field shall contain the address that was used to modify the
/// Subscription List or the unassigned address. When referencing the Label UUID, the
/// virtual address shall be used. The value of the Address field shall not be a unicast
/// address or the all-nodes address.
@property (nonatomic,assign) UInt16 address;

//- (instancetype)initResponseToSigConfigModelPublicationStatus:(SigConfigModelPublicationStatus *)request withStatus:(SigConfigMessageStatus *)status;
//
//- (instancetype)initResponseToSigConfigAnyModelMessage:(SigConfigAnyModelMessage *)request withStatus:(SigConfigMessageStatus)status;
//
//- (instancetype)initResponseToSigConfigModelSubscriptionDeleteAll:(SigConfigModelSubscriptionDeleteAll *)request withStatus:(SigConfigMessageStatus)status;
//
//- (instancetype)initWithConfirmAddingGroup:(SigGroupModel *)group toModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress withStatus:(SigConfigMessageStatus)status;
//
//- (instancetype)initWithConfirmDeletingAddress:(UInt16)address fromModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress withStatus:(SigConfigMessageStatus)status;
//
//- (instancetype)initWithConfirmDeletingAllFromModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress;

/**
 * @brief   Initialize SigConfigModelSubscriptionStatus object.
 * @param   parameters    the hex data of SigConfigModelSubscriptionStatus.
 * @return  return `nil` when initialize SigConfigModelSubscriptionStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.2.20 Config Model Subscription Virtual Address Add, opcode:0x8020


/// The Config Model Subscription Virtual Address Add is an acknowledged message used to add
/// an address to a Subscription List of a model (see Section 4.2.3).
/// @note   The response to a Config Model Subscription Virtual Address Add message is a
/// Config Model Subscription Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.162), 4.3.2.20 Config Model Subscription Virtual Address Add.
@interface SigConfigModelSubscriptionVirtualAddressAdd : SigConfigAnyModelMessage
/// Value of the 128-bt Virtual Label UUID.
/// The Label field shall contain the Label UUID to be added to the Subscription List.
@property (nonatomic,strong) CBUUID *virtualLabel;

- (instancetype)initWithVirtualLabel:(CBUUID *)virtualLabel elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier;

- (instancetype)initWithGroup:(SigGroupModel *)group toModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress;

/**
 * @brief   Initialize SigConfigModelSubscriptionVirtualAddressAdd object.
 * @param   parameters    the hex data of SigConfigModelSubscriptionVirtualAddressAdd.
 * @return  return `nil` when initialize SigConfigModelSubscriptionVirtualAddressAdd object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.22 Config Model Subscription Virtual Address Delete, opcode:0x8021


/// The Config Model Subscription Virtual Address Delete is an acknowledged message used to delete
/// a subscription address from the Subscription List of a model (see Section 4.2.3).
/// @note   The response to a Config Model Subscription Virtual Address Delete message is a
/// Config Model Subscription Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.162), 4.3.2.22 Config Model Subscription Virtual Address Delete.
@interface SigConfigModelSubscriptionVirtualAddressDelete : SigConfigAnyModelMessage
/// Value of the 128-bt Virtual Label UUID.
/// The Address field shall contain the Label UUID used to identify the Address to be removed from the Subscription List.
@property (nonatomic,strong,readonly) CBUUID *virtualLabel;

- (instancetype)initWithVirtualLabel:(CBUUID *)virtualLabel elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier;

- (instancetype)initWithGroup:(SigGroupModel *)group fromModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress;

/**
 * @brief   Initialize SigConfigModelSubscriptionVirtualAddressDelete object.
 * @param   parameters    the hex data of SigConfigModelSubscriptionVirtualAddressDelete.
 * @return  return `nil` when initialize SigConfigModelSubscriptionVirtualAddressDelete object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.24 Config Model Subscription Virtual Address Overwrite, opcode:0x8022


/// The Config Model Subscription Virtual Address Overwrite is an acknowledged message used to discard
/// the Subscription List and add an address to the cleared Subscription List of a model (see Section 4.2.3).
/// @note   The response to a Config Model Subscription Virtual Address Overwrite message is a
/// Config Model Subscription Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.164), 4.3.2.24 Config Model Subscription Virtual Address Overwrite.
@interface SigConfigModelSubscriptionVirtualAddressOverwrite : SigConfigAnyModelMessage
/// Value of the 128-bt Virtual Label UUID.
/// The Address field shall contain the Label UUID used as the new Address to be added to the Subscription List.
@property (nonatomic,strong,readonly) CBUUID *virtualLabel;

- (instancetype)initWithVirtualLabel:(CBUUID *)virtualLabel elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier;

- (instancetype)initWithGroup:(SigGroupModel *)group fromModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress;

/**
 * @brief   Initialize SigConfigModelSubscriptionVirtualAddressOverwrite object.
 * @param   parameters    the hex data of SigConfigModelSubscriptionVirtualAddressOverwrite.
 * @return  return `nil` when initialize SigConfigModelSubscriptionVirtualAddressOverwrite object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.69 Config Network Transmit Get, opcode:0x8023


/// The Config Network Transmit Get is an acknowledged message used to get
/// the current Network Transmit state of a node (see Section 4.2.19).
/// @note   The response to a Config Network Transmit Get message is a
/// Config Network Transmit Status message.
/// @note   There are no Parameters for this message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.182), 4.3.2.69 Config Network Transmit Get.
@interface SigConfigNetworkTransmitGet : SigConfigMessage

/**
 * @brief   Initialize SigConfigNetworkTransmitGet object.
 * @param   parameters    the hex data of SigConfigNetworkTransmitGet.
 * @return  return `nil` when initialize SigConfigNetworkTransmitGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.70 Config Network Transmit Set, opcode:0x8024


/// The Config Network Transmit Set is an acknowledged message used to set
/// the Network Transmit state of a node (see Section 4.2.19).
/// @note   The response to a Config Network Transmit Set message is a
/// Config Network Transmit Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.182), 4.3.2.70 Config Network Transmit Set.
@interface SigConfigNetworkTransmitSet : SigConfigMessage
/// Number of message transmissions of Network PDU originating from the
/// Node. Possible values are 0...7, which correspond to 1-8 transmissions
/// in total.
@property (nonatomic,assign) UInt8 count;
/// Number of 10-millisecond steps between transmissions, decremented by 1.
/// Possible values are 0...31, which corresponds to 10-320 milliseconds
/// intervals.
@property (nonatomic,assign) UInt8 steps;

/// The interval between retransmissions, in seconds.
//- (NSTimeInterval)interval;
/// Sets the Network Transmit property of the Node.
///
/// - parameter count: Number of message transmissions of Network PDU
///                    originating from the Node. Possible values are 0...7,
///                    which correspond to 1-8 transmissions in total.
/// - parameter steps: Number of 10-millisecond steps between transmissions,
///                    decremented by 1. Possible values are 0...31, which
///                    corresponds to 10-320 milliseconds intervals.
- (instancetype)initWithCount:(UInt8)count steps:(UInt8)steps;

/**
 * @brief   Initialize SigConfigNetworkTransmitSet object.
 * @param   parameters    the hex data of SigConfigNetworkTransmitSet.
 * @return  return `nil` when initialize SigConfigNetworkTransmitSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.71 Config Network Transmit Status, opcode:0x8025


/// The Config Network Transmit Status is an unacknowledged message used to report
/// the current Network Transmit state of a node (see Section 4.2.19).
/// @note   - seeAlso: Mesh_v1.0.pdf (page.183), 4.3.2.71 Config Network Transmit Status.
@interface SigConfigNetworkTransmitStatus : SigConfigMessage
/// Number of message transmissions of Network PDU originating from the
/// Node. Possible values are 0...7, which correspond to 1-8 transmissions
/// in total.
@property (nonatomic,assign) UInt8 count;
/// Number of 10-millisecond steps between transmissions, decremented by 1.
/// Possible values are 0...31, which corresponds to 10-320 milliseconds
/// intervals.
@property (nonatomic,assign) UInt8 steps;

/// The interval between retransmissions, in seconds.
- (NSTimeInterval)interval;

/// Sets the Network Transmit property of the Node.
///
/// - parameter count: Number of message transmissions of Network PDU
///                    originating from the Node. Possible values are 0...7,
///                    which correspond to 1-8 transmissions in total.
/// - parameter steps: Number of 10-millisecond steps between transmissions,
///                    decremented by 1. Possible values are 0...31, which
///                    corresponds to 10-320 milliseconds intervals.
- (instancetype)initWithCount:(UInt8)count steps:(UInt8)steps;

- (instancetype)initWithNode:(SigNodeModel *)node;

/**
 * @brief   Initialize SigConfigNetworkTransmitStatus object.
 * @param   parameters    the hex data of SigConfigNetworkTransmitStatus.
 * @return  return `nil` when initialize SigConfigNetworkTransmitStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.2.12 Config Relay Get, opcode:0x8026


/// The Config Relay Get is an acknowledged message used to get the current
/// Relay (see Section 4.2.8) and Relay Retransmit (see Section 4.2.20) states of a node.
/// @note   The response to a Config Relay Get message is a
/// Config Relay Status message.
/// @note   There are no Parameters for this message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.156), 4.3.2.12 Config Relay Get.
@interface SigConfigRelayGet : SigConfigMessage

/**
 * @brief   Initialize SigConfigRelayGet object.
 * @param   parameters    the hex data of SigConfigRelayGet.
 * @return  return `nil` when initialize SigConfigRelayGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.13 Config Relay Set, opcode:0x8027


/// The Config Relay Set is an acknowledged message used to set
/// the Relay (see Section 4.2.8) and Relay Retransmit (see Section 4.2.20) states of a node.
/// @note   The response to a Config Relay Set message is a Config Relay Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.156), 4.3.2.13 Config Relay Set.
@interface SigConfigRelaySet : SigConfigMessage
/// The new Relay state for the Node.
@property (nonatomic,assign) SigNodeRelayState state;
/// Number of retransmissions on advertising bearer for each Network PDU
/// relayed by the Node. Possible values are 0...7, which correspond to
/// 1-8 transmissions in total.
@property (nonatomic,assign) UInt8 count;
/// Number of 10-millisecond steps between retransmissions, decremented by 1.
/// Possible values are 0...31, which corresponds to 10-320 milliseconds
/// intervals.
@property (nonatomic,assign) UInt8 steps;

/// The interval between retransmissions, in seconds.
- (NSTimeInterval)interval;

- (instancetype)initWithCount:(UInt8)count steps:(UInt8)steps;

/**
 * @brief   Initialize SigConfigRelaySet object.
 * @param   parameters    the hex data of SigConfigRelaySet.
 * @return  return `nil` when initialize SigConfigRelaySet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.14 Config Relay Status, opcode:0x8028


/// The Config Relay Status is an unacknowledged message used to report
/// the current Relay (see Section 4.2.8) and Relay Retransmit (see Section 4.2.20) states of a node.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.157), 4.3.2.14 Config Relay Status.
@interface SigConfigRelayStatus : SigConfigMessage
/// The new Relay state for the Node.
@property (nonatomic,assign) SigNodeFeaturesState state;
/// Number of retransmissions on advertising bearer for each Network PDU
/// relayed by the Node. Possible values are 0...7, which correspond to
/// 1-8 transmissions in total.
@property (nonatomic,assign) UInt8 count;
/// Number of 10-millisecond steps between retransmissions, decremented by 1.
/// Possible values are 0...31, which corresponds to 10-320 milliseconds
/// intervals.
@property (nonatomic,assign) UInt8 steps;

/// The interval between retransmissions, in seconds.
- (NSTimeInterval)interval;

/**
 * @brief   Initialize SigConfigRelayStatus object.
 * @param   parameters    the hex data of SigConfigRelayStatus.
 * @return  return `nil` when initialize SigConfigRelayStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.2.27 Config SIG Model Subscription Get, opcode:0x8029


/// The Config SIG Model Subscription Get is an acknowledged message used to get
/// the list of subscription addresses of a model within the element. This message is only for SIG Models.
/// @note   The response to a Config SIG Model Subscription Get message is a
/// Config SIG Model Subscription List message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.165), 4.3.2.27 Config SIG Model Subscription Get.
@interface SigConfigSIGModelSubscriptionGet : SigConfigModelMessage

- (instancetype)initWithElementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier;

- (instancetype)initOfModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress;

/**
 * @brief   Initialize SigConfigSIGModelSubscriptionGet object.
 * @param   parameters    the hex data of SigConfigSIGModelSubscriptionGet.
 * @return  return `nil` when initialize SigConfigSIGModelSubscriptionGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.28 Config SIG Model Subscription List, opcode:0x802A


/// The Config SIG Model Subscription Get is an acknowledged message used to get
/// the list of subscription addresses of a model within the element. This message is only for SIG Models.
/// @note   The response to a Config SIG Model Subscription Get message is a
/// Config SIG Model Subscription List message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.165), 4.3.2.28 Config SIG Model Subscription List.
@interface SigConfigSIGModelSubscriptionList : SigConfigModelSubscriptionList
/// Address of the element.
/// The ElementAddress field is the unicast address of the element, all other address types are Prohibited.
@property (nonatomic,assign) UInt16 elementAddress;

//- (instancetype)initForModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress addresses:(NSArray <NSNumber *>*)addresses withStatus:(SigConfigMessageStatus)status;

/**
 * @brief   Initialize SigConfigSIGModelSubscriptionList object.
 * @param   parameters    the hex data of SigConfigSIGModelSubscriptionList.
 * @return  return `nil` when initialize SigConfigSIGModelSubscriptionList object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.2.29 Config Vendor Model Subscription Get, opcode:0x802B


/// The Config Vendor Model Subscription Get is an acknowledged message used to get
/// the list of subscription addresses of a model within the element. This message is only for Vendor Models.
/// @note   The response to a Config Vendor Model Subscription Get message is a
/// Config Vendor Model Subscription List message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.166), 4.3.2.29 Config Vendor Model Subscription Get.
@interface SigConfigVendorModelSubscriptionGet : SigConfigVendorModelMessage

- (instancetype)initWithElementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier;

- (instancetype)initOfModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress;

/**
 * @brief   Initialize SigConfigVendorModelSubscriptionGet object.
 * @param   parameters    the hex data of SigConfigVendorModelSubscriptionGet.
 * @return  return `nil` when initialize SigConfigVendorModelSubscriptionGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.30 Config Vendor Model Subscription List, opcode:0x802C


/// The Config Vendor Model Subscription List is an unacknowledged message used to report
/// all addresses from the Subscription List of the model (see Section 4.2.3). This message is only for Vendor Models.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.166), 4.3.2.30 Config Vendor Model Subscription List.
@interface SigConfigVendorModelSubscriptionList : SigConfigVendorModelMessage
/// Status Code for the requesting message.
/// The Status field shall identify the Status Code for the last operation on the Subscription List.
/// The allowed values for Status codes and their meanings are documented in Section 4.3.5.
@property (nonatomic,assign) SigConfigMessageStatus status;
/// A block of all addresses from the Subscription List.
/// The Addresses field shall identify all addresses from the Subscription List of an element.
/// When using a Label UUID, the status message shall provide the value of the virtual address
/// as defined in Section 3.4.2.3. The empty Subscription List results in Address field of zero length.
@property (nonatomic,strong) NSMutableArray <NSNumber *>*addresses;//[Address]

//- (instancetype)initForModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress addresses:(NSArray <NSNumber *>*)addresses withStatus:(SigConfigMessageStatus)status;

/**
 * @brief   Initialize SigConfigVendorModelSubscriptionList object.
 * @param   parameters    the hex data of SigConfigVendorModelSubscriptionList.
 * @return  return `nil` when initialize SigConfigVendorModelSubscriptionList object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

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
@interface SigConfigLowPowerNodePollTimeoutGet : SigConfigMessage
/// The unicast address of the Low Power node.
/// The LPNAddress field shall contain the primary unicast address of the Low Power node within a Friend node.
@property (nonatomic,assign) UInt16 LPNAddress;

/**
 * @brief   Initialize SigConfigLowPowerNodePollTimeoutGet object.
 * @param   parameters    the hex data of SigConfigLowPowerNodePollTimeoutGet.
 * @return  return `nil` when initialize SigConfigLowPowerNodePollTimeoutGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithLPNAddress:(UInt16)LPNAddress;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.68 Config Low Power Node PollTimeout Status, opcode:0x802E


/// The Config Low Power Node PollTimeout Status is an unacknowledged message used to report
/// the current value of the PollTimeout timer of the Low Power node within a Friend node.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.181), 4.3.2.68 Config Low Power Node PollTimeout Status.
@interface SigConfigLowPowerNodePollTimeoutStatus : SigConfigMessage
/// The unicast address of the Low Power node.
/// The LPNAddress field shall contain the primary unicast address of the Low Power node.
@property (nonatomic,assign) UInt16 LPNAddress;
/// The current value of the PollTimeout timer of the Low Power node. Size is 3 bytes.
/// The PollTimeout field shall contain the current value of the PollTimeout timer of the
/// Low Power node within a Friend node, or 0x000000 if the node is not a Friend node
/// for the Low Power node identified by LPNAddress.
@property (nonatomic,assign) UInt32 pollTimeout;

/**
 * @brief   Initialize SigConfigLowPowerNodePollTimeoutStatus object.
 * @param   parameters    the hex data of SigConfigLowPowerNodePollTimeoutStatus.
 * @return  return `nil` when initialize SigConfigLowPowerNodePollTimeoutStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.2.61 Config Heartbeat Publication Get, opcode:0x8038


/// The Config Heartbeat Publication Get is an acknowledged message used to get
/// the current Heartbeat Publication state of an element (see Section 4.2.17).
/// @note   The response to a Config Heartbeat Publication Get message is a
/// Config Heartbeat Publication Status message.
/// @note   The message has no parameters.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.178), 4.3.2.61 Config Heartbeat Publication Get.
@interface SigConfigHeartbeatPublicationGet : SigConfigMessage

/**
 * @brief   Initialize SigConfigHeartbeatPublicationGet object.
 * @param   parameters    the hex data of SigConfigHeartbeatPublicationGet.
 * @return  return `nil` when initialize SigConfigHeartbeatPublicationGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.62 Config Heartbeat Publication Set, opcode:0x8039


/// The Config Heartbeat Publication Set is an acknowledged message used to set
/// the current Heartbeat Publication state of an element (see Section 4.2.17).
/// @note   The response to a Config Heartbeat Publication Set message is a
/// Config Heartbeat Publication Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.179), 4.3.2.62 Config Heartbeat Publication Set.
@interface SigConfigHeartbeatPublicationSet : SigConfigMessage
/// Destination address for Heartbeat messages.
/// The Destination field shall identify the Heartbeat Publication Destination state (see Section 4.2.17.1).
@property (nonatomic,assign) UInt16 destination;
/*
 Table 4.24: Heartbeat Publication Count Log values
 
 Value          Description
 0x00           Heartbeat messages are not being sent periodically
 0x01–0x11      Number of Heartbeat messages, 2^(n-1), that remain to be sent
 0x12-0xFE      Prohibited
 0xFF           Heartbeat messages are being sent indefinitely
 */
/// Number of Heartbeat messages to be sent.
/// The CountLog field shall identify the Heartbeat Publication Count Log state (see Section 4.2.17.2).
@property (nonatomic,assign) UInt8 countLog;
/*
 Table 4.25: Heartbeat Publication Period Log values
 
 Value          Description
 0x00           Heartbeat messages are not being sent periodically
 0x01–0x11      Period in 2^(n-1) seconds for sending periodical Heartbeat messages
 0x12-0xFF      Prohibited
 */
/// Period for sending Heartbeat messages.
/// The PeriodLog field shall identify the Heartbeat Publication Period Log state (see Section 4.2.17.3).
@property (nonatomic,assign) UInt8 periodLog;
/*
 Table 4.26: Heartbeat Publication TTL values
 
 Value          Description
 0x00–0x7F      The Heartbeat Publication TTL state
 0x80-0xFF      Prohibited
 */
/// Number of Heartbeat messages to be sent.
/// The TTL field shall identify the Heartbeat Publication TTL state (see Section 4.2.17.4).
@property (nonatomic,assign) UInt8 ttl;
/// Bit field indicating features that trigger Heartbeat messages when changed.
/// The Features field shall identify the Heartbeat Publication Features state (see Section 4.2.17.5).
@property (nonatomic,assign) SigFeatures features;
/// Index of the NetKey.
/// The NetKeyIndex field shall identify the global NetKey Index of the NetKey.
/// The NetKeyIndex field shall be encoded as defined in Section 4.3.1.1.
@property (nonatomic,assign) UInt16 netKeyIndex;

/**
 * @brief   Initialize SigConfigHeartbeatPublicationSet object.
 * @param   parameters    the hex data of SigConfigHeartbeatPublicationSet.
 * @return  return `nil` when initialize SigConfigHeartbeatPublicationSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithDestination:(UInt16)destination countLog:(UInt8)countLog periodLog:(UInt8)periodLog ttl:(UInt8)ttl features:(SigFeatures)features netKeyIndex:(UInt16)netKeyIndex;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.63 Config Heartbeat Publication Status, opcode:0x6


/// The Config Heartbeat Publication Status is an unacknowledged message used to report
/// the Heartbeat Publication state of a node (see Section 4.2.17).
/// @note   - seeAlso: Mesh_v1.0.pdf (page.179), 4.3.2.63 Config Heartbeat Publication Status.
@interface SigConfigHeartbeatPublicationStatus : SigConfigMessage
/// Status Code for the requesting message.
/// The Status field shall identify the Status Code for the requesting message.
/// The allowed values for Status codes and their meanings are documented in Section 4.3.5.
@property (nonatomic,assign) SigConfigMessageStatus status;
/// Destination address for Heartbeat messages.
/// The Destination field shall identify the Heartbeat Publication Destination state (see Section 4.2.17.1).
@property (nonatomic,assign) UInt16 destination;
/*
 Table 4.24: Heartbeat Publication Count Log values
 
 Value          Description
 0x00           Heartbeat messages are not being sent periodically
 0x01–0x11      Number of Heartbeat messages, 2^(n-1), that remain to be sent
 0x12-0xFE      Prohibited
 0xFF           Heartbeat messages are being sent indefinitely
 */
/// Number of Heartbeat messages to be sent.
/// The CountLog field shall identify the Heartbeat Publication Count Log state (see Section 4.2.17.2).
@property (nonatomic,assign) UInt8 countLog;
/*
 Table 4.25: Heartbeat Publication Period Log values
 
 Value          Description
 0x00           Heartbeat messages are not being sent periodically
 0x01–0x11      Period in 2^(n-1) seconds for sending periodical Heartbeat messages
 0x12-0xFF      Prohibited
 */
/// Period for sending Heartbeat messages.
/// The PeriodLog field shall identify the Heartbeat Publication Period Log state (see Section 4.2.17.3).
@property (nonatomic,assign) UInt8 periodLog;
/*
 Table 4.26: Heartbeat Publication TTL values
 
 Value          Description
 0x00–0x7F      The Heartbeat Publication TTL state
 0x80-0xFF      Prohibited
 */
/// Number of Heartbeat messages to be sent.
/// The TTL field shall identify the Heartbeat Publication TTL state (see Section 4.2.17.4).
@property (nonatomic,assign) UInt8 ttl;
/// Bit field indicating features that trigger Heartbeat messages when changed.
/// The Features field shall identify the Heartbeat Publication Features state (see Section 4.2.17.5).
@property (nonatomic,assign) SigFeatures features;
/// Index of the NetKey.
/// The NetKeyIndex field shall identify the global NetKey Index of the NetKey used to publish heartbeats.
/// The NetKeyIndex field shall be encoded as defined in Section 4.3.1.1.
@property (nonatomic,assign) UInt16 netKeyIndex;

/**
 * @brief   Initialize SigConfigHeartbeatPublicationStatus object.
 * @param   parameters    the hex data of SigConfigHeartbeatPublicationStatus.
 * @return  return `nil` when initialize SigConfigHeartbeatPublicationStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.2.64 Config Heartbeat Subscription Get, opcode:0x803A


/// The Config Heartbeat Subscription Get is an acknowledged message used to get
/// the current Heartbeat Subscription state of an element (see Section 4.2.18).
/// @note   The response to a Config Heartbeat Subscription Get message is a
/// Config Heartbeat Subscription Status message.
/// @note   The message has no parameters.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.180), 4.3.2.64 Config Heartbeat Subscription Get.
@interface SigConfigHeartbeatSubscriptionGet : SigConfigMessage

/**
 * @brief   Initialize SigConfigHeartbeatSubscriptionGet object.
 * @param   parameters    the hex data of SigConfigHeartbeatSubscriptionGet.
 * @return  return `nil` when initialize SigConfigHeartbeatSubscriptionGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.65 Config Heartbeat Subscription Set, opcode:0x803B


/// The Config Heartbeat Subscription Set is an acknowledged message used to set
/// the current Heartbeat Subscription state of an element (see Section 4.2.18).
/// @note   The response to a Config Heartbeat Subscription Set message is a
/// Config Heartbeat Subscription Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.180), 4.3.2.65 Config Heartbeat Subscription Set.
@interface SigConfigHeartbeatSubscriptionSet : SigConfigMessage
/// Source address for Heartbeat messages.
/// The Source field shall identify the Heartbeat Subscription Source state (see Section 4.2.18.1).
@property (nonatomic,assign) UInt16 source;
/// Destination address for Heartbeat messages.
/// The Destination field shall identify the Heartbeat Subscription Destination state (see Section 4.2.18.2)
@property (nonatomic,assign) UInt16 destination;
/*
 Table 4.25: Heartbeat Publication Period Log values
 
 Value          Description
 0x00           Heartbeat messages are not being sent periodically
 0x01–0x11      Period in 2^(n-1) seconds for sending periodical Heartbeat messages
 0x12-0xFF      Prohibited
 */
/// Period for sending Heartbeat messages.
/// The PeriodLog field shall identify the Heartbeat Subscription Period state (see Section 4.2.18.4).
@property (nonatomic,assign) UInt8 periodLog;

/**
 * @brief   Initialize SigConfigHeartbeatSubscriptionSet object.
 * @param   parameters    the hex data of SigConfigHeartbeatSubscriptionSet.
 * @return  return `nil` when initialize SigConfigHeartbeatSubscriptionSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithSource:(UInt16)source destination:(UInt16)destination periodLog:(UInt8)periodLog;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.66 Config Heartbeat Subscription Status, opcode:0x803C


/// The Config Heartbeat Subscription Status is an unacknowledged message used to report
/// the Heartbeat Subscription state of a node (see Section 4.2.18).
/// @note   - seeAlso: Mesh_v1.0.pdf (page.181), 4.3.2.66 Config Heartbeat Subscription Status.
@interface SigConfigHeartbeatSubscriptionStatus : SigConfigMessage
/// Status Code for the requesting message.
/// The Status field shall identify the Status Code for the requesting message.
/// The allowed values for Status codes and their meanings are documented in Section 4.3.5.
@property (nonatomic,assign) SigConfigMessageStatus status;
/// Source address for Heartbeat messages.
/// The Source field shall identify the Heartbeat Subscription Source state (see Section 4.2.18.1).
@property (nonatomic,assign) UInt16 source;
/// Destination address for Heartbeat messages.
/// The Destination field shall identify the Heartbeat Subscription Destination state (see Section 4.2.18.2).
@property (nonatomic,assign) UInt16 destination;
/*
 Table 4.25: Heartbeat Publication Period Log values
 
 Value          Description
 0x00           Heartbeat messages are not being sent periodically
 0x01–0x11      Period in 2^(n-1) seconds for sending periodical Heartbeat messages
 0x12-0xFF      Prohibited
 */
/// Period for sending Heartbeat messages.
/// The PeriodLog field shall identify the Heartbeat Subscription Period Log state (see Section 4.2.18.4).
@property (nonatomic,assign) UInt8 periodLog;
/*
 Table 4.24: Heartbeat Publication Count Log values
 
 Value          Description
 0x00           Heartbeat messages are not being sent periodically
 0x01–0x11      Number of Heartbeat messages, 2^(n-1), that remain to be sent
 0x12-0xFE      Prohibited
 0xFF           Heartbeat messages are being sent indefinitely
 */
/// Number of Heartbeat messages to be sent.
/// The CountLog field shall identify the Heartbeat Subscription Count Log state (see Section 4.2.18.3).
@property (nonatomic,assign) UInt8 countLog;
/*
 Table 4.30: Heartbeat Subscription Min TTL values
 
 Value          Description
 0x00           No Heartbeat messages have been received
 0x01–0x7F      The Heartbeat Subscription Min Hops state
 0x80-0xFF      Prohibited
 */
/// Minimum hops when receiving Heartbeat messages.
/// The MinHops field shall identify the Heartbeat Subscription Min Hops state (see Section 4.2.18.5).
@property (nonatomic,assign) UInt8 minHops;
/*
 Table 4.31: Heartbeat Subscription Max TTL values
 
 Value          Description
 0x00           No Heartbeat messages have been received
 0x01–0x7F      The Heartbeat Subscription Max Hops state
 0x80-0xFF      Prohibited
 */
/// Maximum hops when receiving Heartbeat messages.
/// The MaxHops field shall identify the Heartbeat Subscription Max Hops state (see Section 4.2.18.6).
@property (nonatomic,assign) UInt8 maxHops;

/**
 * @brief   Initialize SigConfigHeartbeatSubscriptionStatus object.
 * @param   parameters    the hex data of SigConfigHeartbeatSubscriptionStatus.
 * @return  return `nil` when initialize SigConfigHeartbeatSubscriptionStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.2.46 Config Model App Bind, opcode:0x803D


/// The Config Model App Bind is an acknowledged message used to
/// bind an AppKey to a model.
/// @note   The response to a Config Model App Bind message is a
/// Config Model App Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.173), 4.3.2.46 Config Model App Bind.
@interface SigConfigModelAppBind : SigConfigAppKeyMessage
/// The Company identified, as defined in Assigned Numbers, or `nil`,
/// if the Model is defined in Bluetooth Mesh Model Specification.
///
/// - seeAlso: https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers/
@property (nonatomic,assign,readonly) UInt16 companyIdentifier;
/// The 16-bit Model identifier.
@property (nonatomic,assign) UInt16 modelIdentifier;
/// 标记是否为Vendor Model ID。
@property (nonatomic,assign) BOOL isVendorModelID;
/// The Unicast Address of the Model's parent Element.
/// The ElementAddress field is the unicast address of the element, all other address types are Prohibited.
@property (nonatomic,assign,readonly) UInt16 elementAddress;

- (instancetype)initWithApplicationKeyIndex:(UInt16)applicationKeyIndex elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier;

- (instancetype)initWithApplicationKeyIndex:(UInt16)applicationKeyIndex elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier isVendorModelID:(BOOL)isVendorModelID;

- (instancetype)initWithApplicationKey:(SigAppkeyModel *)applicationKey toModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress;

/**
 * @brief   Initialize SigConfigModelAppBind object.
 * @param   parameters    the hex data of SigConfigModelAppBind.
 * @return  return `nil` when initialize SigConfigModelAppBind object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.48 Config Model App Status, opcode:0x803E


/// The Config Model App Status is an unacknowledged message used to report
/// a status for the requesting message, based on the element address,
/// the AppKeyIndex identifying the AppKey on the AppKey List, and the ModelIdentifier.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.173), 4.3.2.48 Config Model App Status.
@interface SigConfigModelAppStatus : SigConfigAppKeyMessage
/// The Company identified, as defined in Assigned Numbers, or `nil`,
/// if the Model is defined in Bluetooth Mesh Model Specification.
///
/// - seeAlso: https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers/
@property (nonatomic,assign,readonly) UInt16 companyIdentifier;
/// The 16-bit Model identifier.
@property (nonatomic,assign) UInt16 modelIdentifier;
/// 标记是否为Vendor Model ID。
@property (nonatomic,assign) BOOL isVendorModelID;
/// The Unicast Address of the Model's parent Element.
/// The ElementAddress field is the unicast address of the element, all other address types are Prohibited.
@property (nonatomic,assign,readonly) UInt16 elementAddress;
/// The Status field shall identify the Status Code for the requesting message.
/// The allowed values for Status codes and their meanings are documented
/// in Section 4.3.5. The Status Code shall be Success if the received request
/// was redundant (bind request of existing binding, or unbind of a non-existing
/// binding), with no further action taken.
@property (nonatomic,assign,readonly) SigConfigMessageStatus status;

/**
 * @brief   Initialize SigConfigModelAppStatus object.
 * @param   parameters    the hex data of SigConfigModelAppStatus.
 * @return  return `nil` when initialize SigConfigModelAppStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithStatus:(SigConfigMessageStatus)status applicationKey:(SigAppkeyModel *)applicationKey toModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress;

@end


#pragma mark 4.3.2.47 Config Model App Unbind, opcode:0x803F


/// The Config Model App Unbind is an acknowledged message used to remove
/// the binding between an AppKey and a model.
/// @note   The response to a Config Model App Unbind message is a
/// Config Model App Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.173), 4.3.2.47 Config Model App Unbind.
@interface SigConfigModelAppUnbind : SigConfigAppKeyMessage
/// The Company identified, as defined in Assigned Numbers, or `nil`,
/// if the Model is defined in Bluetooth Mesh Model Specification.
///
/// - seeAlso: https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers/
@property (nonatomic,assign,readonly) UInt16 companyIdentifier;
/// The 16-bit Model identifier.
@property (nonatomic,assign) UInt16 modelIdentifier;
/// 标记是否为Vendor Model ID。
@property (nonatomic,assign) BOOL isVendorModelID;
/// The Unicast Address of the Model's parent Element.
/// The ElementAddress field is the unicast address of the element, all other address types are Prohibited.
@property (nonatomic,assign,readonly) UInt16 elementAddress;

- (instancetype)initWithApplicationKeyIndex:(UInt16)applicationKeyIndex elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier;

- (instancetype)initWithApplicationKeyIndex:(UInt16)applicationKeyIndex elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier isVendorModelID:(BOOL)isVendorModelID;

- (instancetype)initWithApplicationKey:(SigAppkeyModel *)applicationKey toModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress;

/**
 * @brief   Initialize SigConfigModelAppUnbind object.
 * @param   parameters    the hex data of SigConfigModelAppUnbind.
 * @return  return `nil` when initialize SigConfigModelAppUnbind object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.31 Config NetKey Add, opcode:0x8040


/// The Config NetKey Add is an acknowledged message used to add
/// a NetKey to a NetKey List (see Section 4.2.4) on a node. The added
/// NetKey is then used by the node to authenticate and decrypt messages
/// it receives, as well as authenticate and encrypt messages it sends.
/// @note   The response to a Config NetKey Add message is a
/// Config NetKey Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.167), 4.3.2.31 Config NetKey Add.
@interface SigConfigNetKeyAdd : SigConfigNetKeyMessage
/// The 128-bit Application Key data.
@property (nonatomic,strong) NSData *key;

- (instancetype)initWithNetworkKeyIndex:(UInt16)networkKeyIndex networkKeyData:(NSData *)networkKeyData;

- (instancetype)initWithNetworkKey:(SigNetkeyModel *)networkKey;

/**
 * @brief   Initialize SigConfigNetKeyAdd object.
 * @param   parameters    the hex data of SigConfigNetKeyAdd.
 * @return  return `nil` when initialize SigConfigNetKeyAdd object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.33 Config NetKey Delete, opcode:0x8041


/// The Config NetKey Delete is an acknowledged message used to delete
/// a NetKey on a NetKey List from a node.
/// @note   The response to a Config NetKey Delete message is a
/// Config NetKey Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.168), 4.3.2.33 Config NetKey Delete.
@interface SigConfigNetKeyDelete : SigConfigNetKeyMessage

- (instancetype)initWithNetworkKeyIndex:(UInt16)networkKeyIndex;

- (instancetype)initWithNetworkKey:(SigNetkeyModel *)networkKey;

/**
 * @brief   Initialize SigConfigNetKeyDelete object.
 * @param   parameters    the hex data of SigConfigNetKeyDelete.
 * @return  return `nil` when initialize SigConfigNetKeyDelete object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.35 Config NetKey Get, opcode:0x8042


/// The Config NetKey Get is an acknowledged message used to report
/// all NetKeys known to the node.
/// @note   The response to a Config NetKey Get message is a
/// Config NetKey List message.
/// @note   There are no Parameters for this message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.169), 4.3.2.35 Config NetKey Get.
@interface SigConfigNetKeyGet : SigConfigMessage

/**
 * @brief   Initialize SigConfigNetKeyGet object.
 * @param   parameters    the hex data of SigConfigNetKeyGet.
 * @return  return `nil` when initialize SigConfigNetKeyGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.36 Config NetKey List, opcode:0x8043


/// The Config NetKey List is an unacknowledged message reporting
/// all NetKeys known to the node.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.169), 4.3.2.36 Config NetKey List.
@interface SigConfigNetKeyList : SigConfigMessage
/// Network Key Indexes known to the Node.
/// The NetKeyIndexes field shall contain all NetKey Indexes that are known to the node.
/// The NetKey Indexes shall be encoded as defined in Section 4.3.1.1.
@property (nonatomic,strong) NSMutableArray <NSNumber *>*networkKeyIndexs;//[KeyIndex]

//- (instancetype)initWithNetworkKeys:(NSArray <SigNetkeyModel *>*)networkKeys;

/**
 * @brief   Initialize SigConfigNetKeyList object.
 * @param   parameters    the hex data of SigConfigNetKeyList.
 * @return  return `nil` when initialize SigConfigNetKeyList object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.2.34 Config NetKey Status, opcode:0x8044


/// The Config NetKey Status is an unacknowledged message used to report
/// the status of the operation on the NetKey List.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.168), 4.3.2.34 Config NetKey Status.
@interface SigConfigNetKeyStatus : SigConfigNetKeyMessage
/// Returns whether the operation was successful or not.
@property (nonatomic,assign,readonly) BOOL isSuccess;
/// The status as String.
@property (nonatomic,strong,readonly) NSString *message;
/// Status Code for the requesting message.
/// The Status field shall identify the Status Code for the last operation on the NetKey List.
/// The allowed values for Status codes and their meanings are documented in Section 4.3.5.
/// The Status Code shall be Success if the received request was redundant (add of an
/// identical existing key, update of an identical updated key, or delete of a non-existent key),
/// with no further action taken.
@property (nonatomic,assign) SigConfigMessageStatus status;

//- (instancetype)initWithNetworkKey:(SigNetkeyModel *)networkKey;
//
//- (instancetype)initWithStatus:(SigConfigMessageStatus)status forMessage:(SigConfigNetKeyMessage *)message;

/**
 * @brief   Initialize SigConfigNetKeyStatus object.
 * @param   parameters    the hex data of SigConfigNetKeyStatus.
 * @return  return `nil` when initialize SigConfigNetKeyStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.2.32 Config NetKey Update, opcode:0x8045


/// The Config NetKey Update is an acknowledged message used to update
/// a NetKey on a node. The updated NetKey is then used by the node to
/// authenticate and decrypt messages it receives, as well as authenticate and
/// encrypt messages it sends, as defined by the Key Refresh procedure (see Section 3.10.4).
/// @note   The response to a Config NetKey Update message is a
/// Config NetKey Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.167), 4.3.2.32 Config NetKey Update.
@interface SigConfigNetKeyUpdate : SigConfigNetKeyMessage
/// The 128-bit Application Key data.
@property (nonatomic,strong) NSData *key;

- (instancetype)initWithNetworkKeyIndex:(UInt16)networkKeyIndex networkKeyData:(NSData *)networkKeyData;

- (instancetype)initWithNetworkKey:(SigNetkeyModel *)networkKey;

/**
 * @brief   Initialize SigConfigNetKeyUpdate object.
 * @param   parameters    the hex data of SigConfigNetKeyUpdate.
 * @return  return `nil` when initialize SigConfigNetKeyUpdate object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.43 Config Node Identity Get, opcode:0x8046


/// The Config Node Identity Get is an acknowledged message used to get
/// the current Node Identity state for a subnet (see Section 4.2.12).
/// @note   The response to a Config Node Identity Get message is a
/// Config Node Identity Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.171), 4.3.2.43 Config Node Identity Get.
@interface SigConfigNodeIdentityGet : SigConfigMessage
/// Index of the NetKey.
/// The NetKeyIndex field is an index that shall identify the global NetKey Index of the NetKey.
/// The NetKeyIndex field shall be encoded as defined in Section 4.3.1.1.
@property (nonatomic,assign) UInt16 netKeyIndex;

- (instancetype)initWithNetKeyIndex:(UInt16)netKeyIndex;

/**
 * @brief   Initialize SigConfigNodeIdentityGet object.
 * @param   parameters    the hex data of SigConfigNodeIdentityGet.
 * @return  return `nil` when initialize SigConfigNodeIdentityGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.44 Config Node Identity Set, opcode:0x8047


/// The Config Node Identity Set is an acknowledged message used to set
/// the current Node Identity state for a subnet (see Section 4.2.12).
/// @note   The response to a Config Node Identity Set message is a
/// Config Node Identity Status message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.172), 4.3.2.44 Config Node Identity Set.
@interface SigConfigNodeIdentitySet : SigConfigMessage
/// Index of the NetKey.
/// The NetKeyIndex field is an index that shall identify the global NetKey
/// Index of the NetKey of the Node Identity state. The NetKeyIndex field
/// shall be encoded as defined in Section 4.3.1.1.
@property (nonatomic,assign) UInt16 netKeyIndex;
/// New Node Identity state.
/// The Identity field shall provide the new Node Identity state of the NetKey (see Section 4.2.12).
@property (nonatomic,assign) SigNodeIdentityState identity;

- (instancetype)initWithNetKeyIndex:(UInt16)netKeyIndex identity:(SigNodeIdentityState)identity;

/**
 * @brief   Initialize SigConfigNodeIdentitySet object.
 * @param   parameters    the hex data of SigConfigNodeIdentitySet.
 * @return  return `nil` when initialize SigConfigNodeIdentitySet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.45 Config Node Identity Status, opcode:0x8048


/// The Config Node Identity Status is an unacknowledged message used to report
/// the current Node Identity state for a subnet (see Section 4.2.12).
/// @note   - seeAlso: Mesh_v1.0.pdf (page.172), 4.3.2.45 Config Node Identity Status.
@interface SigConfigNodeIdentityStatus : SigConfigMessage
/// Status Code for the requesting message.
/// The Status field shall identify the Status Code for the requesting message.
/// The allowed values for Status codes and their meanings are documented in Section 4.3.5.
@property (nonatomic,assign) SigConfigMessageStatus status;
/// Index of the NetKey.
/// The NetKeyIndex field is an index that shall identify the global NetKey
/// Index of the NetKey of the Node Identity state. The NetKeyIndex field
/// shall be encoded as defined in Section 4.3.1.1.
@property (nonatomic,assign) UInt16 netKeyIndex;
/// Node Identity state.
/// The Identity field shall provide the current Node Identity state for a subnet (see Section 4.2.12).
@property (nonatomic,assign) SigNodeIdentityState identity;

/**
 * @brief   Initialize SigConfigNodeIdentityStatus object.
 * @param   parameters    the hex data of SigConfigNodeIdentityStatus.
 * @return  return `nil` when initialize SigConfigNodeIdentityStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.2.53 Config Node Reset, opcode:0x8049


/// The Config Node Reset is an acknowledged message used to reset
/// a node (other than a Provisioner) and remove it from the network.
/// @note   The response to a Config Node Reset message is a
/// Config Node Reset Status message.
/// @note   There are no Parameters for this message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.176), 4.3.2.53 Config Node Reset.
@interface SigConfigNodeReset : SigConfigMessage

/**
 * @brief   Initialize SigConfigNodeReset object.
 * @param   parameters    the hex data of SigConfigNodeReset.
 * @return  return `nil` when initialize SigConfigNodeReset object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.54 Config Node Reset Status, opcode:0x804A


/// The Config Node Reset Status is an unacknowledged message used to
/// acknowledge that an element has received a Config Node Reset message.
/// @note   There are no Parameters for this message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.176), 4.3.2.54 Config Node Reset Status.
@interface SigConfigNodeResetStatus : SigConfigMessage

/**
 * @brief   Initialize SigConfigNodeResetStatus object.
 * @param   parameters    the hex data of SigConfigNodeResetStatus.
 * @return  return `nil` when initialize SigConfigNodeResetStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.2.49 Config SIG Model App Get, opcode:0x804B


/// The Config SIG Model App Get is an acknowledged message used to request
/// report of all AppKeys bound to the SIG Model.
/// @note   The response to a Config SIG Model App Get message is a
/// Config SIG Model App List message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.174), 4.3.2.49 Config SIG Model App Get.
@interface SigConfigSIGModelAppGet : SigConfigModelMessage

- (instancetype)initWithElementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier;

- (instancetype)initWithModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress;

/**
 * @brief   Initialize SigConfigSIGModelAppGet object.
 * @param   parameters    the hex data of SigConfigSIGModelAppGet.
 * @return  return `nil` when initialize SigConfigSIGModelAppGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.50 Config SIG Model App List, opcode:0x804C


/// The Config SIG Model App List is an unacknowledged message used to report
/// all AppKeys bound to the SIG Model.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.174), 4.3.2.50 Config SIG Model App List.
@interface SigConfigSIGModelAppList : SigConfigModelAppList
/// Address of the element.
/// The ElementAddress field is the unicast address of the element, all other address types are Prohibited.
@property (nonatomic,assign,readonly) UInt16 elementAddress;

//- (instancetype)initResponseToSigConfigSIGModelAppGet:(SigConfigSIGModelAppGet *)request withApplicationKeys:(NSArray <SigAppkeyModel *>*)applicationKeys;
//
//- (instancetype)initResponseToSigConfigSIGModelAppGet:(SigConfigSIGModelAppGet *)request withStatus:(SigConfigMessageStatus)status;

/**
 * @brief   Initialize SigConfigSIGModelAppList object.
 * @param   parameters    the hex data of SigConfigSIGModelAppList.
 * @return  return `nil` when initialize SigConfigSIGModelAppList object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.2.51 Config Vendor Model App Get, opcode:0x804D


/// The Config Vendor Model App Get is an acknowledged message used to request
/// report of all AppKeys bound to the model. This message is only for Vendor Models.
/// @note   The response to a Config Vendor Model App Get message is a
/// Config Vendor Model App List message.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.175), 4.3.2.51 Config Vendor Model App Get.
@interface SigConfigVendorModelAppGet : SigConfigVendorModelMessage

- (instancetype)initWithElementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier;

- (instancetype)initWithModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress;

/**
 * @brief   Initialize SigConfigVendorModelAppGet object.
 * @param   parameters    the hex data of SigConfigVendorModelAppGet.
 * @return  return `nil` when initialize SigConfigVendorModelAppGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.2.52 Config Vendor Model App List, opcode:0x804E


/// The Config Vendor Model App List is an unacknowledged message used to report
/// indexes of all AppKeys bound to the model. This message is only for Vendor Models.
/// @note   - seeAlso: Mesh_v1.0.pdf (page.175), 4.3.2.52 Config Vendor Model App List.
@interface SigConfigVendorModelAppList : SigConfigModelAppList
/// Address of the element.
/// The ElementAddress field is the unicast address of the element,
/// all other address types are Prohibited.
@property (nonatomic,assign) UInt16 elementAddress;
@property (nonatomic,assign) UInt16 companyIdentifier;

//- (instancetype)initWithModel:(SigModelIDModel *)model elementAddress:(UInt16)elementAddress applicationKeys:(NSArray <SigAppkeyModel *>*)applicationKeys status:(SigConfigMessageStatus)status;

/**
 * @brief   Initialize SigConfigVendorModelAppList object.
 * @param   parameters    the hex data of SigConfigVendorModelAppList.
 * @return  return `nil` when initialize SigConfigVendorModelAppList object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

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
@interface SigRemoteProvisioningScanCapabilitiesGet : SigConfigMessage
@end


#pragma mark 4.3.4.2 Remote Provisioning Scan Capabilities Status, opcode:0x805F


/// The Remote Provisioning Scan Capabilities Status message is an unacknowledged message
/// used by the Remote Provisioning Server to report the current value of the Remote Provisioning
/// Scan Capabilities state of a Remote Provisioning Server.
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.18), 4.3.4.2 Remote Provisioning Scan Capabilities Status.
@interface SigRemoteProvisioningScanCapabilitiesStatus : SigConfigMessage
/// The maximum number of UUIDs that can be reported during scanning.
/// The MaxScannedItems field identifies the value of the Remote Provisioning Max Scanned Items
/// state(see Section 4.2.22.1).
@property (nonatomic,assign) UInt8 maxScannedItems;
/// Indication if active scan is supported.
/// The ActiveScan field identifies the value of the Remote Provisioning Active Scan
/// state (see Section4.2.22.2).
@property (nonatomic,assign) BOOL activeScan;

/**
 * @brief   Initialize SigRemoteProvisioningScanCapabilitiesStatus object.
 * @param   parameters    the hex data of SigRemoteProvisioningScanCapabilitiesStatus.
 * @return  return `nil` when initialize SigRemoteProvisioningScanCapabilitiesStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

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
@interface SigRemoteProvisioningScanGet : SigConfigMessage
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
@interface SigRemoteProvisioningScanStart : SigConfigMessage
/// Maximum number of scanned items to be reported.
/// The ScannedItemsLimit field identifies the maximum number of unprovisioned
/// devices the Remote Provisioning Server can report while executing the Remote
/// Provisioning Scan procedure. Value 0 indicates that the Remote Provisioning
/// Client does not set a limit on the number of unprovisioned devices that the
/// Remote Provisioning Server can report.
@property (nonatomic,assign) UInt8 scannedItemsLimit;
/// Time limit for a scan (in seconds).
/// The Timeout field identifies the new value of the Remote Provisioning Timeout
/// state (see Section 4.2.23.2). The value of the Timeout field shall not be 0.
@property (nonatomic,assign) UInt8 timeout;
/// Device UUID (Optional)
/// If the UUID field is present, the Remote Provisioning Client is requesting a
/// Single Device Scanning procedure, i.e., a scan for a specific device identified
/// by the value of the UUID field. If the UUID field is absent, the Remote
/// Provisioning Client is requesting a scan for all unprovisioned devices within
/// immediate radio range (a Multiple Devices Scanning).
@property (nonatomic,strong) NSData *UUID;

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
- (instancetype)initWithScannedItemsLimit:(UInt8)scannedItemsLimit timeout:(UInt8)timeout UUID:(nullable NSData *)UUID;

/**
 * @brief   Initialize SigRemoteProvisioningScanStart object.
 * @param   parameters    the hex data of SigRemoteProvisioningScanStart.
 * @return  return `nil` when initialize SigRemoteProvisioningScanStart object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

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
@interface SigRemoteProvisioningScanStop : SigConfigMessage
@end


#pragma mark 4.3.4.6 Remote Provisioning Scan Status, opcode:0x8054


/// The Remote Provisioning Scan Status message is an unacknowledged message
/// used by the Remote Provisioning Server to report the current value of the Remote
/// Provisioning Scan Parameters state and the Remote Provisioning Scan state of a
/// Remote Provisioning Server model.
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.19), 4.3.4.6 Remote Provisioning Scan Status.
@interface SigRemoteProvisioningScanStatus : SigConfigMessage
/// Status for the requesting message.
/// The Status field identifies the status of the most recent operation on Remote
/// Provisioning Scan state, as defined in Table 4.22.
@property (nonatomic,assign) UInt8 status;
/// The Remote Provisioning Scan state value.
/// The RPScanningState field identifies the value of the Remote Provisioning Scan
/// state (see Section 4.2.23.1).
@property (nonatomic,assign) UInt8 RPScanningState;
/// Maximum number of scanned items to be reported.
/// The ScannedItemsLimit field identifies the maximum number of unprovisioned
/// devices as requested by the Remote Provisioning Client in the Remote
/// Provisioning Scan Start message.
@property (nonatomic,assign) UInt8 scannedItemsLimit;
/// Time limit for a scan (in seconds).
/// The Timeout field identifies the current value of the Remote Provisioning
/// Timeout state (see Section 4.2.23.2).
@property (nonatomic,assign) UInt8 timeout;

/**
 * @brief   Initialize SigRemoteProvisioningScanStatus object.
 * @param   parameters    the hex data of SigRemoteProvisioningScanStatus.
 * @return  return `nil` when initialize SigRemoteProvisioningScanStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

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
@interface SigRemoteProvisioningScanReport : SigConfigMessage
/// Signed integer that is interpreted as an indication of received signal strength measured in dBm.
/// The RSSI field contains a signed 8-bit value and is interpreted as an indication of
/// received signal strength measured in dBm. The Remote Provisioning Server
/// measures the RSSI value on packets sent by the unprovisioned device. If the RSSI
/// cannot be read, the RSSI field shall be set to 127.
@property (nonatomic,assign) SInt8 RSSI;
/// Device UUID.
/// The UUID field identifies the Device UUID of the unprovisioned device.
@property (nonatomic,strong) NSData *UUID;
/// OOB information.
/// The OOB field identifies the OOB Information of the unprovisioned device (see Table 3.54).
@property (nonatomic,assign) UInt16 OOB;

/**
 * @brief   Initialize SigRemoteProvisioningScanReport object.
 * @param   parameters    the hex data of SigRemoteProvisioningScanReport.
 * @return  return `nil` when initialize SigRemoteProvisioningScanReport object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

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
@interface SigRemoteProvisioningExtendedScanStart : SigConfigMessage
/// Number of AD Types in the ADTypeFilter field.
/// The ADTypeFilterCount field identifies number of AD types listed in the ADTypeFilter field.
@property (nonatomic,assign) UInt8 ADTypeFilterCount;
/// List of AD Types to be reported (C.1), size:variable.
/// C.1: If ADTypeFilterCount field value is not zero, the ADTypeFilter shall be present; otherwise
/// the ADTypeFilter shall not be present
/// If present, the ADTypeFilter is a list of AD types that the client is requesting. The ADTypeFilter
/// shall not contain the Shortened Local Name AD Type, the Incomplete List of 16-bit Service
/// UUIDs AD Type, the Incomplete List of 32-bit Service UUIDs AD Type, or the Incomplete List
/// of 128-bit Service UUIDs AD Type.
/// Note: If the ADTypeFilter field contains the Complete Local Name AD Type, the client is
/// requesting either the Complete Local Name or the Shortened Local Name.
@property (nonatomic,strong) NSData *ADTypeFilter;
/// Device UUID (Optional).
/// If present, the UUID field identifies the Device UUID of the unprovisioned device for which
/// additional information is requested (see Section 4.4.5.3). If the UUID field is absent, the
/// request retrieves information about the Remote Provisioning Server (see Section 4.4.5.5.2.1).
/// In the latter case, the Remote Provisioning Server ignores the Timeout field value.
@property (nonatomic,strong) NSData *UUID;
/// Time limit for a scan (in seconds) (C.2). Length of time (in seconds) to collect information
/// about the unprovisioned device: 0x01~0x05
/// C.2: If UUID field is present, the Timeout field shall also be present; otherwise Timeout
/// field shall not be present.
/// The Timeout field indicates how long the Remote Provisioning Client requests the Remote
/// Provisioning Server to collect information about the unprovisioned device identified by the
/// UUID. Table 4.11 defines the values for the Timeout field.
@property (nonatomic,assign) UInt8 timeout;

/**
 * @brief   Initialize SigRemoteProvisioningExtendedScanStart object.
 * @param   parameters    the hex data of SigRemoteProvisioningExtendedScanStart.
 * @return  return `nil` when initialize SigRemoteProvisioningExtendedScanStart object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

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
- (instancetype)initWithADTypeFilterCount:(UInt8)ADTypeFilterCount ADTypeFilter:(nullable NSData *)ADTypeFilter UUID:(nullable NSData *)UUID timeout:(UInt8)timeout;

@end


#pragma mark 4.3.4.9 Remote Provisioning Extended Scan Report, opcode:0x8057


/// The Remote Provisioning Extended Scan Report message is an unacknowledged message
/// used by the Remote Provisioning Server to report the advertising data requested by the client
/// in a Remote Provisioning Extended Scan Start message (see Section 4.3.4.8).
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.21), 4.3.4.9 Remote Provisioning Extended Scan Report.
@interface SigRemoteProvisioningExtendedScanReport : SigConfigMessage
/// Status for the requesting message.
/// The Status field identifies the status of the Remote Provisioning Extended Scan Start
/// processing, as defined in Table 4.22.
@property (nonatomic,assign) SigRemoteProvisioningStatus status;
/// Device UUID (Optional).
/// The UUID field identifies the Device UUID of either the unprovisioned device
/// (see Section 3.10.3) or the Remote Provisioning Server.
@property (nonatomic,strong) NSData *UUID;
/// OOB Information (Optional).
/// The OOBInformation field contains the OOB Information of either the unprovisioned
/// device (see Section 3.9.2) or the Remote Provisioning Server.
@property (nonatomic,assign) UInt16 OOBInformation;
/// Concatenated list of AD Structures that match the AD Types requested by the client in the
/// ADTypeFilter field of the Remote Provisioning Extended Scan Start message. (C.1) size:variable
/// C.1: If OOBInformation field is present, the AdvStructures field is optional;
/// otherwise AdvStructures field shall not be present.
/// If present, the AdvStructures field contains a concatenated list of AD Structures with
/// information requested by the Remote Provisioning Client. The value has the same format
/// as advertising data or scan response data, as defined in [1] Section 11, Vol 3, Part C.
@property (nonatomic,strong) NSData *AdvStructures;

/**
 * @brief   Initialize SigRemoteProvisioningExtendedScanReport object.
 * @param   parameters    the hex data of SigRemoteProvisioningExtendedScanReport.
 * @return  return `nil` when initialize SigRemoteProvisioningExtendedScanReport object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.4.10 Remote Provisioning Link Get, opcode:0x8058


/// The Remote Provisioning Link Get message is an acknowledged message used by the Remote
/// Provisioning Client to get the Remote Provisioning Link state of a Remote Provisioning Server model.
/// @note   The response to a Remote Provisioning Link Get message is a Remote Provisioning
/// Link Status message (see Section 4.3.4.13).
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.22), 4.3.4.10 Remote Provisioning Link Get.
@interface SigRemoteProvisioningLinkGet : SigConfigMessage
@end


#pragma mark 4.3.4.11 Remote Provisioning Link Open, opcode:0x8059


/// The Remote Provisioning Link Open message is an acknowledged message used by the Remote
/// Provisioning Client to establish the provisioning bearer between a node supporting the Remote
/// Provisioning Server model and an unprovisioned device, or to open the Device Key Refresh Interface.
/// @note   The response to a Remote Provisioning Link Open message is a Remote Provisioning
/// Link Status message (see Section 4.3.4.13).
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.22), 4.3.4.11 Remote Provisioning Link Open.
@interface SigRemoteProvisioningLinkOpen : SigConfigMessage
/// Device UUID (Optional).
/// If present, the UUID field identifies the Device UUID of an unprovisioned device that the
/// link will be open to. If the UUID field is absent, the Remote Provisioning Server will open
/// the Device Key Refresh Interface.
@property (nonatomic,strong) NSData *UUID;

/**
 * @brief   Initialize SigRemoteProvisioningLinkOpen object.
 * @param   UUID    Device UUID (Optional).
 * If present, the UUID field identifies the Device UUID of an unprovisioned device that the
 * link will be open to. If the UUID field is absent, the Remote Provisioning Server will open
 * the Device Key Refresh Interface.
 * @return  return `nil` when initialize SigRemoteProvisioningLinkOpen object fail.
 */
- (instancetype)initWithUUID:(nullable NSData *)UUID;

@end


#pragma mark 4.3.4.12 Remote Provisioning Link Close, opcode:0x805A


/// The Remote Provisioning Link Close message is an acknowledged message used by the Remote
/// Provisioning Client to close a provisioning bearer or the Device Key Refresh Interface.
/// @note   The response to a Remote Provisioning Link Close message is a Remote Provisioning
/// Link Status message (see Section 4.3.4.13).
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.23), 4.3.4.12 Remote Provisioning Link Close.
@interface SigRemoteProvisioningLinkClose : SigConfigMessage
/// Link close reason code.
/// The Reason field identifies the reason for the provisioning link close. The Reason field values for the
/// Remote Provisioning Link Close message are defined in Table 4.15.
@property (nonatomic,assign) SigRemoteProvisioningLinkCloseStatus reason;

/**
 * @brief   Initialize SigRemoteProvisioningLinkOpen object.
 * @param   reason    Link close reason code.
 * The Reason field identifies the reason for the provisioning link close. The Reason field values for the
 * Remote Provisioning Link Close message are defined in Table 4.15.
 * @return  return `nil` when initialize SigRemoteProvisioningLinkOpen object fail.
 */
- (instancetype)initWithReason:(SigRemoteProvisioningLinkCloseStatus)reason;

@end


#pragma mark 4.3.4.13 Remote Provisioning Link Status, opcode:0x805B


/// The Remote Provisioning Link Status message is an unacknowledged message used by
/// the Remote Provisioning Server to acknowledge a Remote Provisioning Link Get message,
/// a Remote Provisioning Link Open message, or a Remote Provisioning Link Close message.
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.23), 4.3.4.13 Remote Provisioning Link Status.
@interface SigRemoteProvisioningLinkStatus : SigConfigMessage
/// Status for the requesting message.
/// The Status field identifies the status of the processing of the message from the client,
/// as defined in Table 4.22.
@property (nonatomic,assign) SigRemoteProvisioningStatus status;
/// Remote Provisioning Link state
/// The RPState field identifies the Remote Provisioning Link state (see Table 4.4).
@property (nonatomic,assign) BOOL RPState;

/**
 * @brief   Initialize SigRemoteProvisioningLinkStatus object.
 * @param   parameters    the hex data of SigRemoteProvisioningLinkStatus.
 * @return  return `nil` when initialize SigRemoteProvisioningLinkStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.4.14 Remote Provisioning Link Report, opcode:0x805C


/// The Remote Provisioning Link Report message is an unacknowledged message used
/// by the Remote Provisioning Server to report the state change of a provisioning bearer
/// link or the Device Key Refresh Interface.
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.24), 4.3.4.14 Remote Provisioning Link Report.
@interface SigRemoteProvisioningLinkReport : SigConfigMessage
/// Status for the requesting message.
/// The Status field identifies the provisioning bearer status as defined in Table 4.22.
@property (nonatomic,assign) SigRemoteProvisioningStatus status;
/// Remote Provisioning Link state
/// The RPState field identifies the Remote Provisioning Link state.
@property (nonatomic,assign) BOOL RPState;
/// Link close Reason code (Optional).
/// If present, the Reason field identifies the reason for the provisioning link close
/// as defined in Table 5.12. The field may be present only when Status is either
/// Link Closed by Device or Link Closed by Server and the provisioning bearer
/// provides a Reason code.
@property (nonatomic,assign) SigRemoteProvisioningLinkCloseStatus reason;

/**
 * @brief   Initialize SigRemoteProvisioningLinkReport object.
 * @param   parameters    the hex data of SigRemoteProvisioningLinkReport.
 * @return  return `nil` when initialize SigRemoteProvisioningLinkReport object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

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
@interface SigRemoteProvisioningPDUSend : SigConfigMessage
/// Provisioning PDU identification number.
/// The OutboundPDUNumber field identifies the identification number of the Provisioning
/// PDU set in the ProvisioningPDU field.
@property (nonatomic,assign) UInt8 outboundPDUNumber;
/// Provisioning PDU.
/// The ProvisioningPDU field identifies the Provisioning PDU that either will be sent to an
/// unprovisioned device or will be processed locally if the Device Key Refresh procedure
/// is in progress.
@property (nonatomic,strong) NSData *provisioningPDU;

/**
 * @brief   Initialize SigRemoteProvisioningPDUSend object.
 * @param   parameters    the hex data of SigRemoteProvisioningPDUSend.
 * @return  return `nil` when initialize SigRemoteProvisioningPDUSend object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

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
- (instancetype)initWithOutboundPDUNumber:(UInt8)outboundPDUNumber provisioningPDU:(NSData *)provisioningPDU;

@end


#pragma mark 4.3.4.14 Remote Provisioning PDU Outbound Report, opcode:0x805E


/// The Remote Provisioning PDU Outbound Report message is an unacknowledged message
/// used by the Remote Provisioning Server to report completion of the delivery of the Provisioning
/// PDUs that the Remote Provisioning Server either sends to a device that is being provisioned or
/// processes locally during the Device Key Refresh procedure.
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.25), 4.3.4.14 Remote Provisioning PDU Outbound Report.
@interface SigRemoteProvisioningPDUOutboundReport : SigConfigMessage
/// Provisioning PDU identification number.
/// The OutboundPDUNumber field contains the value of the Remote Provisioning Outbound
/// PDU Count state.
@property (nonatomic,assign) UInt8 outboundPDUNumber;

/**
 * @brief   Initialize SigRemoteProvisioningPDUOutboundReport object.
 * @param   parameters    the hex data of SigRemoteProvisioningPDUOutboundReport.
 * @return  return `nil` when initialize SigRemoteProvisioningPDUOutboundReport object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.4.14 Remote Provisioning PDU Report, opcode:0x805F


/// The Remote Provisioning PDU Report message is an unacknowledged message used
/// by the Remote Provisioning Server to report the Provisioning PDU that either was received
/// from the device being provisioned or was generated locally during the Device Key
/// Refresh procedure.
/// @note   - seeAlso: MshPRF_RPR_CR_r16_VZ2_ba3-dpc-ok2-PW_ok-PW2_RemoteProvisioner.pdf
/// (page.25), 4.3.4.14 Remote Provisioning PDU Report.
@interface SigRemoteProvisioningPDUReport : SigConfigMessage
/// Provisioning PDU identification number.
/// The InboundPDUNumber field identifies the value of the Remote Provisioning Inbound
/// PDU Count state (see Section 4.2.24.6).
@property (nonatomic,assign) UInt8 outboundPDUNumber;
/// Provisioning PDU.
/// The ProvisioningPDU field identifies the Provisioning PDU that was send by an unprovisioned
/// device or generated locally during the Device Key Refresh procedure.
@property (nonatomic,strong) NSData *provisioningPDU;

/**
 * @brief   Initialize SigRemoteProvisioningPDUReport object.
 * @param   parameters    the hex data of SigRemoteProvisioningPDUReport.
 * @return  return `nil` when initialize SigRemoteProvisioningPDUReport object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark - 4.3.9 Opcodes Aggregator messages


#pragma mark 4.3.9.2 OPCODES_AGGREGATOR_SEQUENCE, opcode:0xB809


/// An OPCODES_AGGREGATOR_STATUS message is an unacknowledged message used to report
/// status for the most recent operation and response messages as a result of processing the Items
/// field in an OPCODES_AGGREGATOR_SEQUENCE message.
/// @note   - seeAlso: MshPRFd1.1r13_clean.pdf (page.389),
/// 4.3.9.2 OPCODES_AGGREGATOR_SEQUENCE.
@interface SigOpcodesAggregatorSequence : SigConfigMessage
/// The Element_Address field is the unicast address of the element of the Opcodes Aggregator Server.
/// All other address types are Prohibited.
@property (nonatomic,assign) UInt16 elementAddress;
/// List of status items with each status item containing an unacknowledged access layer
/// message or empty item (Optional)
/// If present, the Status_Items field contains a list of unacknowledged response messages
/// corresponding to the acknowledged messages in the Items field of the
/// OPCODES_AGGREGATOR_SEQUENCE message, or an empty message when the
/// corresponding Item field contained an unacknowledged message. The format of a
/// Status_Items field entry is the same as the format of the Items field defined in Section 4.3.9.1.
@property (nonatomic,strong) NSArray <SigOpcodesAggregatorItemModel *>*items;
/// 使用SDK的方法`initWithParameters:`和`initWithElementAddress:items:`初始化的SigOpcodesAggregatorSequence，会自动根据输入的items的第一个message使用的key来赋值isEncryptByDeviceKey。
@property (nonatomic,assign) BOOL isEncryptByDeviceKey;// default is NO.

- (NSData *)opCodeAndParameters;

/**
 * @brief   Initialize SigOpcodesAggregatorSequence object.
 * @param   parameters    the hex data of SigOpcodesAggregatorSequence.
 * @return  return `nil` when initialize SigOpcodesAggregatorSequence object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithElementAddress:(UInt16)elementAddress items:(NSArray <SigOpcodesAggregatorItemModel *>*)items;

@end


#pragma mark 4.3.9.3 OPCODES_AGGREGATOR_STATUS, opcode:0xB810


/// An OPCODES_AGGREGATOR_STATUS message is an unacknowledged message
/// used to report status for the most recent operation and response messages as a result
/// of processing the Items field in an OPCODES_AGGREGATOR_SEQUENCE message.
/// @note   - seeAlso: MshPRFd1.1r13_clean.pdf (page.389),
/// 4.3.9.3 OPCODES_AGGREGATOR_STATUS.
@interface SigOpcodesAggregatorStatus : SigConfigMessage
/// The Status field indicates the status of the most recent operation.
@property (nonatomic,assign) SigOpcodesAggregatorMessagesStatus status;
/// The Element_Address field is the unicast address of the element.
/// All other address types are prohibited.
@property (nonatomic,assign) UInt16 elementAddress;
/// List of status items with each status item containing an unacknowledged access layer
/// message or empty item (Optional).
/// If present, the Status_Items field contains a list of unacknowledged response messages
/// corresponding to the acknowledged messages in the Items field of the
/// OPCODES_AGGREGATOR_SEQUENCE message, or an empty message when the
/// corresponding Item field contained an unacknowledged message. The format of a
/// Status_Items field entry is the same as the format of the Items field defined in Section 4.3.9.1.
@property (nonatomic,strong) NSArray <SigOpcodesAggregatorItemModel *>*statusItems;

/**
 * @brief   Initialize SigOpcodesAggregatorStatus object.
 * @param   parameters    the hex data of SigOpcodesAggregatorStatus.
 * @return  return `nil` when initialize SigOpcodesAggregatorStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithStatus:(SigOpcodesAggregatorMessagesStatus)status elementAddress:(UInt16)elementAddress items:(NSArray <SigOpcodesAggregatorItemModel *>*)items;

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
@interface SigPrivateBeaconGet : SigConfigMessage
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark 4.3.12.2 PRIVATE_BEACON_SET, opcode:0xB712


/// A PRIVATE_BEACON_SET message is an acknowledged message used to set
/// the Private Beacon state and the Random Update Interval Steps state of a node.
/// @note   The response to a PRIVATE_BEACON_SET message is a
/// PRIVATE_BEACON_STATUS message.
/// @note   - seeAlso: MshPRFd1.1r13_clean.pdf (page.401),
/// 4.3.12.2 PRIVATE_BEACON_SET.
@interface SigPrivateBeaconSet : SigConfigMessage
/// New Private Beacon state.
/// The Private_Beacon field shall identify the value of the new Private Beacon
/// state (see Section 4.2.44.1) of a node.
@property (nonatomic,assign) SigPrivateBeaconState privateBeacon;
/// New Random Update Interval Steps state (optional).
/// 0x00, Random field is updated for every Mesh Private beacon.
/// 0x01–0xFF, Random field is updated at an interval (in seconds) of (10 * Random Update Interval Steps).
/// If present, the Random_Update_Interval_Steps field shall identify the value of the
/// new Random Update Interval Steps state (see Section 4.2.44.2) of a node.
@property (nonatomic,assign) UInt8 randomUpdateIntervalSteps;
/// default is NO.
@property (nonatomic,assign) BOOL needRandomUpdateIntervalSteps;

- (instancetype)initWithPrivateBeacon:(SigPrivateBeaconState)privateBeacon randomUpdateIntervalSteps:(UInt8)randomUpdateIntervalSteps;
- (instancetype)initWithPrivateBeacon:(SigPrivateBeaconState)privateBeacon;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark 4.3.12.3 PRIVATE_BEACON_STATUS, opcode:0xB713


/// A PRIVATE_BEACON_STATUS message is an unacknowledged message used to report
/// the current Private Beacon state and Random Update Interval Steps state of a node.
/// @note   This message is a response to PRIVATE_BEACON_GET message or a
/// PRIVATE_BEACON_SET message.
/// @note   - seeAlso: MshPRFd1.1r13_clean.pdf (page.402),
/// 4.3.12.3 PRIVATE_BEACON_STATUS.
@interface SigPrivateBeaconStatus : SigConfigMessage
/// Current value of the Private Beacon state.
/// The Private_Beacon field shall identify the value of the current Private Beacon
/// state (see Section 4.2.44.1) of a node.
@property (nonatomic,assign) SigPrivateBeaconState privateBeacon;
/// Current value of the Random Update Interval Steps state.
/// The Random_Update_Interval_Steps field shall identify the value of the current
/// Random Update Interval Steps state (see Section 4.2.44.2) of a node.
@property (nonatomic,assign) UInt8 randomUpdateIntervalSteps;

/**
 * @brief   Initialize SigPrivateBeaconStatus object.
 * @param   parameters    the hex data of SigPrivateBeaconStatus.
 * @return  return `nil` when initialize SigPrivateBeaconStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithPrivateBeacon:(SigPrivateBeaconState)privateBeacon randomUpdateIntervalSteps:(UInt8)randomUpdateIntervalSteps;

@end


#pragma mark 4.3.12.4 PRIVATE_GATT_PROXY_GET, opcode:0xB714


/// A PRIVATE_GATT_PROXY_GET message is an acknowledged message used to
/// get the current Private GATT Proxy state of a node (see Section 4.2.45).
/// @note   The response to a PRIVATE_GATT_PROXY_GET message is a
/// PRIVATE_GATT_PROXY_STATUS message.
/// @note   - seeAlso: MshPRFd1.1r13_clean.pdf (page.402),
/// 4.3.12.4 PRIVATE_GATT_PROXY_GET.
@interface SigPrivateGattProxyGet : SigConfigMessage
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark 4.3.12.5 PRIVATE_GATT_PROXY_SET, opcode:0xB715


/// A PRIVATE_GATT_PROXY_SET message is an acknowledged message
/// used to set the Private GATT Proxy state of a node (see Section 4.2.45).
/// @note   The response to a PRIVATE_GATT_PROXY_SET message is a
/// PRIVATE_GATT_PROXY_STATUS message.
/// @note   - seeAlso: MshPRFd1.1r13_clean.pdf (page.403),
/// 4.3.12.5 PRIVATE_GATT_PROXY_SET.
@interface SigPrivateGattProxySet : SigConfigMessage
/// New Private GATT Proxy state.
/// The Private_GATT_Proxy field shall provide the new Private GATT Proxy
/// state of the node (see Section 4.2.45). The Private_GATT_Proxy field shall
/// be either Disable (0x00) or Enable (0x01), and all other values prohibited.
@property (nonatomic,assign) SigPrivateGattProxyState privateGattProxy;

- (instancetype)initWithPrivateGattProxy:(SigPrivateGattProxyState)privateGattProxy;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark 4.3.12.6 PRIVATE_GATT_PROXY_STATUS, opcode:0xB716


/// A PRIVATE_GATT_PROXY_STATUS message is an unacknowledged message used to
/// report the current Private GATT Proxy state of a node (see Section 4.2.45).
/// @note   - seeAlso: MshPRFd1.1r13_clean.pdf (page.403),
/// 4.3.12.6 PRIVATE_GATT_PROXY_STATUS.
@interface SigPrivateGattProxyStatus : SigConfigMessage
/// Private GATT Proxy state.
/// The Private_GATT_Proxy field shall provide the current Private GATT Proxy state of the
/// node (see Section 4.2.45).
@property (nonatomic,assign) SigPrivateGattProxyState privateGattProxy;

/**
 * @brief   Initialize SigPrivateGattProxyStatus object.
 * @param   parameters    the hex data of SigPrivateGattProxyStatus.
 * @return  return `nil` when initialize SigPrivateGattProxyStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithPrivateGattProxy:(SigPrivateGattProxyState)privateGattProxy;

@end


#pragma mark 4.3.12.7 PRIVATE_NODE_IDENTITY_GET, opcode:0xB718


/// A PRIVATE_NODE_IDENTITY_GET message is an acknowledged message used to get
///  the current Private Node Identity state for a subnet (see Section 4.2.46).
/// @note   The response to a PRIVATE_NODE_IDENTITY_GET message is a
/// PRIVATE_NODE_IDENTITY_STATUS message.
/// @note   - seeAlso: MshPRFd1.1r13_clean.pdf (page.404),
/// 4.3.12.7 PRIVATE_NODE_IDENTITY_GET.
@interface SigPrivateNodeIdentityGet : SigConfigMessage
/// Index of the NetKey.
/// The NetKeyIndex field is an index that shall identify the global NetKey Index of the NetKey.
/// The NetKeyIndex field shall be encoded as defined in Section 4.3.1.1.
@property (nonatomic,assign) UInt16 netKeyIndex;

- (instancetype)initWithNetKeyIndex:(UInt16)netKeyIndex;

/**
 * @brief   Initialize SigPrivateNodeIdentityGet object.
 * @param   parameters    the hex data of SigPrivateNodeIdentityGet.
 * @return  return `nil` when initialize SigPrivateNodeIdentityGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.12.8 PRIVATE_NODE_IDENTITY_SET, opcode:0xB719


/// A PRIVATE_NODE_IDENTITY_SET message is an acknowledged message used to set
/// the current Private Node Identity state for a subnet (see Section 4.2.46).
/// @note   The response to a PRIVATE_NODE_IDENTITY_SET message is a
/// PRIVATE_NODE_IDENTITY_STATUS message.
/// @note   - seeAlso: MshPRFd1.1r13_clean.pdf (page.404),
/// 4.3.12.8 PRIVATE_NODE_IDENTITY_SET.
@interface SigPrivateNodeIdentitySet : SigConfigMessage
/// Index of the NetKey.
/// The NetKeyIndex field is an index that shall identify the global NetKey Index of the NetKey
/// of the Node Identity state. The NetKeyIndex field shall be encoded as defined in Section 4.3.1.1.
@property (nonatomic,assign) UInt16 netKeyIndex;
/// New Private Node Identity state.
/// The Private_Identity field shall provide the new Private Node Identity state of the
/// NetKey (see Section 4.2.46). The Private_Identity field shall be either Disable (0x00) or
/// Enable (0x01), and all other values prohibited.
@property (nonatomic,assign) SigPrivateNodeIdentityState privateIdentity;

- (instancetype)initWithNetKeyIndex:(UInt16)netKeyIndex privateIdentity:(SigPrivateNodeIdentityState)privateIdentity;

/**
 * @brief   Initialize SigPrivateNodeIdentitySet object.
 * @param   parameters    the hex data of SigPrivateNodeIdentitySet.
 * @return  return `nil` when initialize SigPrivateNodeIdentitySet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.3.12.9 PRIVATE_NODE_IDENTITY_STATUS, opcode:0xB71A


/// A PRIVATE_NODE_IDENTITY_STATUS message is an unacknowledged message
/// used to report the current Private Node Identity state for a subnet (see Section 4.2.46).
/// @note   The response to a PRIVATE_NODE_IDENTITY_SET message is a
/// PRIVATE_NODE_IDENTITY_STATUS message.
/// @note   - seeAlso: MshPRFd1.1r13_clean.pdf (page.405),
/// 4.3.12.9 PRIVATE_NODE_IDENTITY_STATUS.
@interface SigPrivateNodeIdentityStatus : SigConfigMessage
/// Status Code for the requesting message.
/// The Status field shall identify the Status Code for the requesting message. The allowed
/// values for Status codes and their meanings are documented in Section 4.3.14.
@property (nonatomic,assign) SigConfigMessageStatus status;
/// Index of the NetKey.
/// The NetKeyIndex field is an index that shall identify the global NetKey Index of the NetKey
/// of the Node Identity state. The NetKeyIndex field shall be encoded as defined in Section 4.3.1.1.
@property (nonatomic,assign) UInt16 netKeyIndex;
/// Private Node Identity state.
/// The Private_Identity field shall provide the current Private Node Identity state for a
/// subnet (see Section 4.2.46).
@property (nonatomic,assign) SigPrivateNodeIdentityState privateIdentity;

/**
 * @brief   Initialize SigPrivateNodeIdentityStatus object.
 * @param   parameters    the hex data of SigPrivateNodeIdentityStatus.
 * @return  return `nil` when initialize SigPrivateNodeIdentityStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end

NS_ASSUME_NONNULL_END
