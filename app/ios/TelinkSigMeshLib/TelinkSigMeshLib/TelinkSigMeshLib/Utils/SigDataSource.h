/********************************************************************************************************
 * @file     SigDataSource.h
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

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class SigNetkeyModel,SigProvisionerModel,SigAppkeyModel,SigSceneModel,SigGroupModel,SigNodeModel, SigExclusionListObjectModel, SigIvIndex,SigBaseMeshMessage, SigForwardingTableModel,SigDataSource;

@protocol SigDataSourceDelegate <NSObject>
@optional

/**
 * @brief   Callback called when the sequenceNumber or ivIndex change.
 * @param   sequenceNumber sequenceNumber of current provisioner.
 * @param   ivIndex ivIndex of current mesh network.
 */
- (void)onSequenceNumberUpdate:(UInt32)sequenceNumber ivIndexUpdate:(UInt32)ivIndex;

/**
 * @brief   Callback called when the unicastRange of provisioner had been changed.
 * APP need update the json to cloud at this time!
 * @param   unicastRange Randge model had beed change.
 * @param   provisioner provisioner of unicastRange.
 * @note    The address of the last node may be out of range.
 */
- (void)onUpdateAllocatedUnicastRange:(SigRangeModel *)unicastRange ofProvisioner:(SigProvisionerModel *)provisioner;

/**
 * @brief   Callback called when the mesh network JOSN data had been changed.
 * APP need update the json to cloud at this time!
 * @param   network new mesh network JSON data.
 * @note    When the SDK calls `saveLocationData`, it will also notify the APP through this callback method.
 */
- (void)onMeshNetworkUpdated:(SigDataSource *)network;

@end


/// This specification defines a representation of the Mesh Configuration Database in
/// JavaScript Object Notation (JSON), as defined by [2], and follows the format defined
/// by the JSON Schema Draft 4 [3]. Guidelines for creating the Mesh Configuration
/// Database are provided in Appendix A.
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.7),
/// 2 Mesh Configuration Database format.
@interface SigDataSource : NSObject

/*
 2.1 Mesh object
 A Mesh Configuration Database shall contain a single JSON object that
 describes the mesh network, called the Mesh Object. The Mesh Object
 shall only contain information about a single mesh network and associated
 subnets. The Mesh Object documents the current state of the mesh network
 known by the device that generates and maintains this database.
 */

/// The $schema property contains a URL (see Section 4) that points to the version of the JSON
/// Schema specification that is used to generate the Mesh Configuration Database schema,
/// which corresponds to JSON Schema Draft 4 [3].
@property (nonatomic, copy) NSString *schema;

/// The id property contains a URL (see Section 4) that points to the version of the Mesh
/// Configuration Database schema that defines the format for this Mesh Object.
@property (nonatomic, copy) NSString *jsonFormatID;

/// The version property contains a string that represents a version number of the Mesh
/// Configuration Database that this Mesh Object supports. The version property for the Mesh
/// Configuration Database that is defined by this specification is “1.0.1”.
@property (nonatomic, copy) NSString *version;

/// The meshUUID property is defined as a string that represents the 128-bit Universally
/// Unique Identifier (UUID), which allows differentiation among multiple mesh networks.
/// This string shall follow the UUID string representation format as defined by [7].
/// @note   UUID format in JSON："standardUUID": {
/// "type": "string",
/// "name": "UUID",
/// "pattern": "^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$"}
@property (nonatomic, copy) NSString *meshUUID;

/// The meshName property contains a UTF-8 string, which should be a human-readable
/// name for the mesh network. This is useful when a Mesh Manager tracks multiple mesh
/// networks to enable a user to identify different mesh networks.
@property (nonatomic, copy) NSString *meshName;

/// The timestamp property contains a string that represents the last time the Mesh Object
/// has been modified. The timestamp is based on Coordinated Universal Time (UTC) and
/// follows the “date-time” format as defined by JSON Schema Draft 4 [3], which is based
/// on the Internet date/time format described in Section 5.6 of RFC 3339 [6]:
/// YYYY-MM-DDThh:mm:ssZ or YYYY-MM-DDThh:mm:ss+/- timeoffset
/// where “YYYY” denotes a year; “MM” denotes a two-digit month (01 to 12); “DD” denotes
/// a two-digit day of the month (01 to 31); “hh” denotes a two-digit hour (00 to 23); “mm”
/// denotes a two-digit minute (00 to 59);'ss" denotes a two-digit second (00 to 59); “Z” denotes
/// the UTC time zone; and “timeoffset” denotes the offset between local time and UTC in the
/// format of +hh:mm or -hh:mm.
@property (nonatomic, copy) NSString *timestamp;

/// The partial property indicates if this Mesh Configuration Database is part of a larger database.
@property (nonatomic, assign) bool partial;

/// The provisioners property contains an array of provisioner objects (see Section 2.1.1) that
/// includes information about known Provisioners and ranges of addresses that have been
/// allocated to these Provisioners.
@property (nonatomic, strong) NSMutableArray<SigProvisionerModel *> *provisioners;

/// The netKeys property contains an array of network key objects (see Section 2.1.2) that
/// includes information about network keys used in the mesh network.
@property (nonatomic, strong) NSMutableArray<SigNetkeyModel *> *netKeys;

/// The appKeys property contains an array of application key objects (see Section 2.1.3) that
/// includes information about application keys used in the mesh network.
@property (nonatomic, strong) NSMutableArray<SigAppkeyModel *> *appKeys;

/// The nodes property contains an array of node objects (see Section 2.1.4) that includes
/// information about mesh nodes in the mesh network.
@property (nonatomic, strong) NSMutableArray<SigNodeModel *> *nodes;

/// The groups property contains an array of group objects (see Section 2.1.5) that includes
/// information about groups configured in the mesh network.
@property (nonatomic, strong) NSMutableArray<SigGroupModel *> *groups;

/// The scenes property contains an array of scene objects (see Section 2.1.6) that includes
/// information about scenes configured in the mesh network.
@property (nonatomic, strong) NSMutableArray<SigSceneModel *> *scenes;

/// The networkExclusions property contains the array of exclusionList objects
/// (see section 2.1.7).(Optional)
@property (nonatomic, strong) NSMutableArray<SigExclusionListObjectModel *> *networkExclusions;

/// The proxy for callbacks when Mesh data has been updated.
/// @note   1.Callback called when the sequenceNumber or ivIndex change.
/// @note   2.Callback called when the unicastRange of provisioner had been changed.
@property (nonatomic, weak) id <SigDataSourceDelegate>delegate;


@property (nonatomic,strong) NSMutableArray <SigEncryptedModel *>*encryptedArray;

/* default config value */
@property (nonatomic, strong) SigNetkeyModel *defaultNetKeyA;
@property (nonatomic, strong) SigAppkeyModel *defaultAppKeyA;
@property (nonatomic, strong) SigIvIndex *defaultIvIndexA;
@property (nonatomic, strong) SigNetkeyModel *curNetkeyModel;
@property (nonatomic, strong) SigAppkeyModel *curAppkeyModel;
/* cache value */
@property (nonatomic, strong) NSMutableArray<SigScanRspModel *> *scanList;
/// nodes should show in HomeViewController
@property (nonatomic,strong, nullable) NSMutableArray <SigNodeModel *>*curNodes;
/// modelID of subscription group
@property (nonatomic, strong) NSMutableArray <NSNumber *>*defaultGroupSubscriptionModels;
/// default nodeInfo for fast bind.
@property (nonatomic, strong) NSMutableArray <DeviceTypeModel *>*defaultNodeInfos;
/// get from source address of `setFilterForProvisioner:`
@property (nonatomic, assign) UInt16 unicastAddressOfConnected;
@property (nonatomic, assign) BOOL needPublishTimeModel;
@property (nonatomic, strong) NSMutableArray <SigOOBModel *>*OOBList;
/// `YES` means SDK will add staticOOB devcie that never input staticOOB data by noOOB provision.
/// `NO` means SDK will not add staticOOB devcie that never input staticOOB data.
@property (nonatomic, assign) BOOL addStaticOOBDeviceByNoOOBEnable;
/// default retry count of every command. default is 2.
@property (nonatomic, assign) UInt8 defaultRetryCount;
/// 默认一个provisioner分配的设备地址区间，默认值为kAllocatedUnicastRangeHighAddress（0x400）.
@property (nonatomic, assign) UInt16 defaultAllocatedUnicastRangeHighAddress;
/// 默认sequenceNumber的步长，默认值为kSequenceNumberIncrement（128）.
@property (nonatomic, assign) UInt8 defaultSequenceNumberIncrement;
/// 默认一个unsegmented Access PDU的最大长度，大于该长度则需要进行segment分包，默认值为kUnsegmentedMessageLowerTransportPDUMaxLength（15，如onoff：2bytes opcode + 9bytes data(1byte onoff+1byte TID+7bytes other data) + 4bytes MIC）。默认一个segmented Access PDU的最大长度为kUnsegmentedMessageLowerTransportPDUMaxLength-3。
@property (nonatomic, assign) UInt16 defaultUnsegmentedMessageLowerTransportPDUMaxLength;
/// telink私有定义的Extend Bearer Mode，SDK默认是使用0，特殊用户需要用到2。
@property (nonatomic, assign) SigTelinkExtendBearerMode telinkExtendBearerMode;
/// 默认publish的周期，默认值为kPublishInterval（20），SigStepResolution_seconds.
@property (nonatomic, strong) SigPeriodModel *defaultPublishPeriodModel;
/// 0 means have response message, other means haven't response message.
//@property (nonatomic,assign) UInt32 responseOpCode;

/// Returns whether the message should be sent or has been sent using
/// 32-bit or 64-bit TransMIC value. By default `.low` is returned.
///
/// Only Segmented Access Messages can use 64-bit MIC. If the payload
/// is shorter than 11 bytes, make sure you return `true` from
/// `isSegmented`, otherwise this field will be ignored.
@property (nonatomic,assign) SigMeshMessageSecurity security;

/// sig mesh协议v1.1之后，SDK进行provision操作使用算法的配置项，默认为SigFipsP256EllipticCurve_auto，自动适配provision算法。
@property (nonatomic, assign) SigFipsP256EllipticCurve fipsP256EllipticCurve;

/// 非LPN节点的默认可靠发包间隔，默认值为1.28。
@property (nonatomic, assign) float defaultReliableIntervalOfNotLPN;
/// LPN节点的默认可靠发包间隔，默认值为2.56。
@property (nonatomic, assign) float defaultReliableIntervalOfLPN;

/// certificate-base provision 的根证书。 默认为APP端写死的root.der，开发者也可以自行修改该证书。
@property (nonatomic, strong) NSData *defaultRootCertificateData;

/// 缓存mesh里面的Direct Forwarding Table列表
@property (nonatomic,strong) NSMutableArray <SigForwardingTableModel *>*forwardingTableModelList;
/// v3.3.3.6及之后的版本添加，YES则对于支持Aggregator的节点，keybind会自动使用Aggregator进行发送。NO则默认都不使用Aggregator进行keybind。
@property (nonatomic, assign) BOOL aggregatorEnable;

/// cache属性，不导出分享的JSON，也不缓存于本地的JSON，只单独缓存于kLocationIvIndexAndSequenceNumberDictionary_key这个字典里面。
@property (nonatomic, copy) NSString *ivIndex;
/// v3.3.3.6及之后的版本添加，cache属性，不导出分享的JSON，也不缓存于本地的JSON，只单独缓存于kLocationIvIndexAndSequenceNumberDictionary_key这个字典里面。
@property (nonatomic, copy) NSString *sequenceNumber;
//@property (nonatomic,assign) UInt32 ivIndexUInt32;
@property (nonatomic,assign) UInt32 sequenceNumberUInt32;

/// v3.3.3.6及之后的版本添加，
@property (nonatomic, assign) BOOL sendByDirectedSecurity;
@property (nonatomic, strong) SigProxyFilterModel *filterModel;
//@property (nonatomic, strong) NSMutableArray <SigNodeSeqZeroModel *>*nodeSequenceNumberCacheList;
@property (nonatomic, strong) NSMutableArray <SigNodeSequenceNumberCacheModel *>*nodeSequenceNumberCacheList;


//取消该限制：客户可以初始化该类型的对象，创建一个中间的mesh数据，用于比较前后的mesh信息。
//+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
//- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));

/**
 *  @brief  Singleton method
 *
 *  @return the default singleton instance.
 */
+ (instancetype)share;

/**
 * @brief   Set mesh dictionary to SDK DataSource.
 * @param   dictionary    mesh dictionary.
 */
- (void)setDictionaryToDataSource:(NSDictionary *)dictionary;

/**
 * @brief   Get dictionary for save local.
 * @return  mesh dictionary
 * @note    Developer need use this API to get mesh data save locally.
 */
- (NSDictionary *)getDictionaryFromDataSource;

/**
 * @brief   Get formatDictionary for Export mesh.
 * @return  mesh formatDictionary
 * @note    Developer need use this API to export mesh data.
 */
- (NSDictionary *)getFormatDictionaryFromDataSource;

/**
 * @brief   Get the unicastAddress for provision in add device process.
 * @return  unicastAddress for provision
 * @note    The address of the last node may be out of range..
 */
- (UInt16)provisionAddress DEPRECATED_MSG_ATTRIBUTE("Use 'getProvisionAddressWithElementCount:' instead");

/**
 * @brief   Get the unicastAddress for provision in add device process.
 * @param   elementCount    the element count of node.
 * @return  unicastAddress for provision
 * @note    If the allocatedUnicastRange of provision has been exhausted, SDK will add a new allocatedUnicastRange. 0 means all allocatedUnicastRange is exhausted.
 */
- (UInt16)getProvisionAddressWithElementCount:(UInt8)elementCount;

/**
 * @brief   Get current netKey hex data of current mesh.
 * @return  netKey hex data
 */
- (NSData *)curNetKey;

/**
 * @brief   Get current appKey hex data of current mesh.
 * @return  appKey hex data
 */
- (NSData *)curAppKey;

/**
 * @brief   Get local provisioner of current mesh.
 * @return  local provisioner
 */
- (SigProvisionerModel *)curProvisionerModel;

/**
 * @brief   Get local provisioner node of current mesh.
 * @return  local provisioner node
 */
- (SigNodeModel *)curLocationNodeModel;

/**
 * @brief   Get online node count of current mesh.
 * @return  online node count
 */
- (NSInteger)getOnlineDevicesNumber;

/**
 * @brief   Determine whether there is a node containing the time modelID.
 * @return  `YES` means exist, `NO` means not exist.
 */
- (BOOL)hasNodeExistTimeModelID;

/**
 * @brief   Get the ivIndex of current mesh.
 * @return  ivIndex
 */
- (UInt32)getIvIndexUInt32;

/**
 * @brief   Set the ivIndex to current mesh.
 * @param   ivIndexUInt32    the ivIndex of mesh.
 * @note    Developer can set ivIndex by this API if ivIndex is save in other place. Range is 0~0xFFFFFFFF, SDK will trigger ivUpdate process when sequenceNumber is greater than 0xC00000.
 */
- (void)setIvIndexUInt32:(UInt32)ivIndexUInt32;

/**
 * @brief   Get the SequenceNumber of current mesh.
 * @return  SequenceNumber
 */
- (UInt32)getSequenceNumberUInt32;

/**
 * @brief   Set the sequenceNumber to current mesh.
 * @param   sequenceNumberUInt32    the sequenceNumber of mesh.
 * @note    Developer can set sequenceNumber by this API if sequenceNumber is save in other place. Range is 0~0xFFFFFF, SDK will trigger ivUpdate process when sequenceNumber is greater than 0xC00000.
 */
- (void)setSequenceNumberUInt32:(UInt32)sequenceNumberUInt32;

/**
 * @brief   Get the SigNodeSequenceNumberCacheModel from the match SequenceNumberCache through the unicastAddress of node.
 * @param   unicastAddress    the unicastAddress of node.
 * @return  A SigNodeSequenceNumberCacheModel that save NodeSequenceNumber infomation. nil means there are no SigNodeSequenceNumberCacheModel match this unicastAddress had been receive.
 * @note    SDK will clean all SigNodeSequenceNumberCacheModel when connect mesh success, then SDK add SigNodeSequenceNumberCacheModel by all mesh message notify on current mesh.
 */
- (SigNodeSequenceNumberCacheModel *)getSigNodeSequenceNumberCacheModelWithUnicastAddress:(UInt16)unicastAddress;

/**
 * @brief   Update ivIndex in local after SDK receivce a new vaild beacon.
 * @param   ivIndexUInt32    the new ivIndexUInt32.
 * @note    Callback this API when SDK receivce a new vaild beacon.
 */
- (void)updateIvIndexUInt32FromBeacon:(UInt32)ivIndexUInt32;

/**
 * @brief   Update sequenceNumber after SDK send a mesh message.
 * @param   sequenceNumberUInt32    the sequenceNumberUInt32 for next networkPDU.
 * @note    Callback this API when SDK send a networkPDU in SigNetworkLayer.
 */
- (void)updateSequenceNumberUInt32WhenSendMessage:(UInt32)sequenceNumberUInt32;

/**
 * @brief   Loading local mesh data
 * @note    1.create provisionerUUID if need. 2.create mesh if need. 3.check provisioner. 4.load SigScanRspModel cache list.
 */
- (void)configData;

/**
 * @brief   Check SigDataSource.provisioners.
 * @note    This api will auto create a provisioner when SigDataSource.provisioners hasn't provisioner corresponding to app's UUID.
 */
- (void)checkExistLocationProvisioner;

/**
 * @brief   Add node of provisioner to mesh network.
 * @param   provisioner    the SigProvisionerModel object.
 * @note    Callback this API when add a new provisioner to the mesh network.
 */
- (void)addLocationNodeWithProvisioner:(SigProvisionerModel *)provisioner;

/**
 * @brief   Add node to mesh network.
 * @param   model    the SigNodeModel object.
 * @note    Callback this API when provision success.
 */
- (void)addAndSaveNodeToMeshNetworkWithDeviceModel:(SigNodeModel *)model;

/**
 * @brief   Delete node from mesh network.
 * @param   deviceAddress    the unicastAddress of node.
 * @note    Callback this API when APP delete node complete. 1.SDK will set this node to blacklisted, then optimization Data Of Blacklisted.  2.Delete action with this unicasetAddress in scene. 3.Delete ScanRspModel of this unicastAddress. 4.Delete SigEncryptedModel of this unicastAddress.
 */
- (void)deleteNodeFromMeshNetworkWithDeviceAddress:(UInt16)deviceAddress;

/**
 * @brief   Get max unicast address of all provisioner.allocatedUnicastRange.
 * @return  new scene address, min address is 1.
 * @note    new scene address = exist scene address + 1, min address is 1.
 */
- (UInt16)getMaxHighAllocatedUnicastAddress;

/**
 * @brief   add or delete the groupID of node.
 * @param   add    `YES` means add groupID to node, `NO` means delete groupID from node.
 * @param   unicastAddress    the unicastAddress of node.
 * @param   groupAddress    the groupAddress of group.
 */
- (void)editGroupIDsOfDevice:(BOOL)add unicastAddress:(NSNumber *)unicastAddress groupAddress:(NSNumber *)groupAddress;

/**
 * @brief   Update the state of all nodes to DeviceStateOutOfLine.
 * @note    1.set _curNodes to nil. 2.set node of _nodes to DeviceStateOutOfLine.
 */
- (void)setAllDevicesOutline;

/// Manager mesh network, switch new mesh.
/// @param dict new Mesh Dictionary
- (void)switchToNewMeshDictionary:(NSDictionary *)dict;

/**
 * @brief   Save Current SigDataSource to local by NSUserDefaults.
 * @note    1.sort _nodes, sort _groups, sort _scene. 2.change SigDataSource to data, and save by NSUserDefaults. 3. save encrypt json data to file `TelinkSDKMeshJsonData`.
 */
- (void)saveLocationData;

/**
 * @brief   update the state of node.
 * @param   responseMessage    the state responseMessage of node.
 * @param   source    the unicastAddress of responseMessage.
 */
- (void)updateNodeStatusWithBaseMeshMessage:(SigBaseMeshMessage *)responseMessage source:(UInt16)source;

/**
 * @brief   Get the new scene address for add a new scene object.
 * @return  new scene address, min address is 1.
 * @note    new scene address = exist scene address + 1, min address is 1.
 */
- (UInt16)getNewSceneAddress;

/**
 * @brief   Add or Update the infomation of  SigSceneModel to _scenes..
 * @param   model    the SigSceneModel object.
 */
- (void)saveSceneModelWithModel:(SigSceneModel *)model;

/**
 * @brief   Delete SigSceneModel from _scenes..
 * @param   model   the SigSceneModel object.
 */
- (void)deleteSceneModelWithModel:(SigSceneModel *)model;

/**
 * @brief   Get the SigEncryptedModel from the match advertisementDataServiceData cache through the unicastAddress of node..
 * @param   address    the unicastAddress of node.
 * @return  A SigEncryptedModel that save advertisementDataServiceData infomation. nil means there are no SigEncryptedModel match this unicastAddress had been scaned.
 * @note    SDK will cache the SigEncryptedModel when SDK scaned the nodeIdentity broadcast data of provisioned node or when SDK scaned the nodeIdentity broadcast data of provisioned node.
 */
- (SigEncryptedModel *)getSigEncryptedModelWithAddress:(UInt16)address;

/**
 * @brief   Determine whether the SigScanRspModel is in current mesh network.
 * @param   model    the ScanRspModel object.
 * @return  `YES` means model belong to the network, `NO` means model does not belong to the network.
 * @note    The SigScanRspModel has all infomation of peripheral in Apple System.
 */
- (BOOL)existScanRspModelOfCurrentMeshNetwork:(SigScanRspModel *)model;

/**
 * @brief   Determine whether the peripheralUUIDString is in current mesh network.
 * @param   peripheralUUIDString    the peripheralUUIDString in ScanRspModel object.
 * @return  `YES` means peripheralUUIDString belong to the network, `NO` means peripheralUUIDString does not belong to the network.
 * @note    The peripheralUUIDString is the peripheral identify in Apple System.
 */
- (BOOL)existPeripheralUUIDString:(NSString *)peripheralUUIDString;

/**
 * @brief   Determine whether the advertisementDataServiceData is the node in the mesh network with this networkKey. (The type of advertisementDataServiceData is nodeIdentity.)
 * @param   advertisementDataServiceData    the advertisementDataServiceData in ScanRspModel object.
 * @param   peripheralUUIDString    the peripheralUUIDString in ScanRspModel object.
 * @param   nodes    the node list of mesh network.
 * @param   networkKey    the network key of mesh network.
 * @return  `YES` means advertisementDataServiceData belong to the network, `NO` means advertisementDataServiceData does not belong to the network.
 * @note    The advertisementDataServiceData is the broadcast data of provisioned node.
 */
- (BOOL)matchNodeIdentityWithAdvertisementDataServiceData:(NSData *)advertisementDataServiceData peripheralUUIDString:(NSString *)peripheralUUIDString nodes:(NSArray <SigNodeModel *>*)nodes networkKey:(SigNetkeyModel *)networkKey;

/**
 * @brief   Determine whether the advertisementDataServiceData is the node in the mesh network with this networkKey. (The type of advertisementDataServiceData is privateNetworkIdentity.)
 * @param   advertisementDataServiceData    the advertisementDataServiceData in ScanRspModel object.
 * @param   peripheralUUIDString    the peripheralUUIDString in ScanRspModel object.
 * @param   networkKey    the network key of mesh network.
 * @return  `YES` means advertisementDataServiceData belong to the network, `NO` means advertisementDataServiceData does not belong to the network.
 * @note    The advertisementDataServiceData is the broadcast data of provisioned node.
 */
- (BOOL)matchPrivateNetworkIdentityWithAdvertisementDataServiceData:(NSData *)advertisementDataServiceData peripheralUUIDString:(NSString *)peripheralUUIDString networkKey:(SigNetkeyModel *)networkKey;

/**
 * @brief   Determine whether the advertisementDataServiceData is the node in the mesh network with this networkKey. (The type of advertisementDataServiceData is privateNodeIdentity.)
 * @param   advertisementDataServiceData    the advertisementDataServiceData in ScanRspModel object.
 * @param   peripheralUUIDString    the peripheralUUIDString in ScanRspModel object.
 * @param   nodes    the node list of mesh network.
 * @param   networkKey    the network key of mesh network.
 * @return  `YES` means advertisementDataServiceData belong to the network, `NO` means advertisementDataServiceData does not belong to the network.
 * @note    The advertisementDataServiceData is the broadcast data of provisioned node.
 */
- (BOOL)matchPrivateNodeIdentityWithAdvertisementDataServiceData:(NSData *)advertisementDataServiceData peripheralUUIDString:(NSString *)peripheralUUIDString nodes:(NSArray <SigNodeModel *>*)nodes networkKey:(SigNetkeyModel *)networkKey;

/**
 * @brief   update the ScanRspModel object infomation to dataSource.
 * @param   model    the ScanRspModel object.
 * @note    The SigScanRspModel is the node bluetooth infomation of in Apple System.
 */
- (void)updateScanRspModelToDataSource:(SigScanRspModel *)model;

/**
 * @brief   Get the ScanRspModel object through the peripheral UUIDString of node.
 * @param   peripheralUUIDString    the peripheral UUIDString of node.
 * @note    The SigScanRspModel is the node bluetooth infomation of in Apple System.
 */
- (SigScanRspModel *)getScanRspModelWithUUID:(NSString *)peripheralUUIDString;

/**
 * @brief   Get the ScanRspModel object through the macAddress of node.
 * @param   macAddress    the macAddress of node.
 * @note    The SigScanRspModel is the node bluetooth infomation of in Apple System.
 */
- (SigScanRspModel *)getScanRspModelWithMacAddress:(NSString *)macAddress;

/**
 * @brief   Get the ScanRspModel object through the unicastAddress of node.
 * @param   unicastAddress    the unicastAddress of node.
 * @note    The SigScanRspModel is the node bluetooth infomation of in Apple System.
 */
- (SigScanRspModel *)getScanRspModelWithUnicastAddress:(UInt16)unicastAddress;

/**
 * @brief   Delete the ScanRspModel object through the unicastAddress of node.
 * @param   unicastAddress    the unicastAddress of node.
 * @note    The SigScanRspModel is the node bluetooth infomation of in Apple System.
 */
- (void)deleteScanRspModelWithUnicastAddress:(UInt16)unicastAddress;

/**
 * @brief   Get the netKey object through the networkId of netKey.
 * @param   networkId    the networkId of netKey.
 * @return  A SigNetkeyModel that save netKey infomation. nil means there are no netKey with this networkId in mesh network.
 */
- (SigNetkeyModel * _Nullable)getNetkeyModelWithNetworkId:(NSData *)networkId;

/**
 * @brief   Get the netKey object through the netKeyIndex of netKey.
 * @param   netKeyIndex    the netKeyIndex of netKey.
 * @return  A SigNetkeyModel that save netKey infomation. nil means there are no netKey with this netKeyIndex in mesh network.
 */
- (SigNetkeyModel * _Nullable)getNetkeyModelWithNetkeyIndex:(NSInteger)netKeyIndex;

/**
 * @brief   Get the appKey object through the appKeyIndex of appKey.
 * @param   appkeyIndex    the appKeyIndex of appKey.
 * @return  A SigAppkeyModel that save appKey infomation. nil means there are no appKey with this appKeyIndex in mesh network.
 */
- (SigAppkeyModel * _Nullable)getAppkeyModelWithAppkeyIndex:(NSInteger)appkeyIndex;

/**
 * @brief   Get the node object through the bluetooth PeripheralUUID of node.
 * @param   peripheralUUIDString    the bluetooth PeripheralUUID of node.
 * @return  A SigNodeModel that save node infomation. nil means there are no node with this unicastAddress in mesh network.
 * @note    The unicastAddress is the unique identifier of the node in the mesh network, app config the unicastAddress of node in provision progress when app add node to the mesh network. The bluetooth PeripheralUUID is the unique identifier of Apple System.
 */
- (SigNodeModel * _Nullable)getNodeWithPeripheralUUIDString:(NSString *)peripheralUUIDString;

/**
 * @brief   Get the node object through the unicastAddress of node.
 * @param   unicastAddress    the unicastAddress of node.
 * @return  A SigNodeModel that save node infomation. nil means there are no node with this unicastAddress in mesh network.
 * @note    The unicastAddress is the unique identifier of the node in the mesh network, app config the unicastAddress of node in provision progress when app add node to the mesh network.
 */
- (SigNodeModel * _Nullable)getNodeWithAddress:(UInt16)unicastAddress;

/**
 * @brief   Get current connected node object.
 * @return  A SigNodeModel that save node infomation. nil means there are no connected node in the mesh network.
 */
- (SigNodeModel * _Nullable)getCurrentConnectedNode;

/**
 * @brief   Get the provisioner object through the provisioner node address.
 * @param   address    the unicastAddress of provisioner node.
 * @return  A SigProvisionerModel that save povisioner infomation. nil means no povisioner infomation of this unicastAddress.
 */
- (SigProvisionerModel * _Nullable)getProvisionerModelWithAddress:(UInt16)address;

/**
 * @brief   Get the provisioner object through the provisionerUUID.
 * @param   provisionerUUIDString    The UUID String of provisioner.
 * @return  A SigProvisionerModel that save povisioner infomation. nil means no povisioner infomation of this UUID.
 */
- (SigProvisionerModel * _Nullable)getProvisionerModelWithProvisionerUUIDString:(NSString *)provisionerUUIDString;

/**
 * @brief   Get the modelID object through the modelID
 * @param   modelID    The id of model.
 * @return  A ModelIDModel that save model infomation. nil means no model infomation of this modelID.
 */
- (ModelIDModel * _Nullable)getModelIDModel:(NSNumber *)modelID;

/**
 * @brief   Get the group object through the group address
 * @param   groupAddress    The address of group.
 * @return  A SigGroupModel that save group infomation. nil means no group infomation of this groupAddress.
 */
- (SigGroupModel * _Nullable)getGroupModelWithGroupAddress:(UInt16)groupAddress;

/**
 * @brief   Obtain the DeviceTypeModel through the PID and CID of the node.
 * @param   CID    The company id of node.
 * @param   PID    The product id of node.
 * @return  A SigOOBModel that save oob  infomation. nil means no oob infomation of this UUID.
 * @note    This API is using to get the DeviceTypeModel through the PID and CID of the node, DeviceTypeModel has default composition data of node.
 */
- (DeviceTypeModel * _Nullable)getNodeInfoWithCID:(UInt16)CID PID:(UInt16)PID;

#pragma mark - OOB存取相关

/**
 * @brief   Add a oobModel cached by the SDK.
 * @param   oobModel    an oob object.
 * @note    This API is using to add oob infomation of node, oob infomation is use for OOB provision. If there is an old OOB object that has same UUID, the SDK will replace it.
 */
- (void)addAndUpdateSigOOBModel:(SigOOBModel *)oobModel;

/**
 * @brief   Add a list of oobModel cached by the SDK.
 * @param   oobModelList    a list of oob object.
 * @note    This API is using to add oob infomation of node, oob infomation is use for OOB provision. If there is an old OOB object that has same UUID, the SDK will replace it.
 */
- (void)addAndUpdateSigOOBModelList:(NSArray <SigOOBModel *>*)oobModelList;

/**
 * @brief   Delete one oobModel cached by the SDK.
 * @param   oobModel    The oob object.
 * @note    This API is using to delete oob infomation of node, oob infomation is use for OOB provision.
 */
- (void)deleteSigOOBModel:(SigOOBModel *)oobModel;

/**
 * @brief   Remove all OOB data cached by the SDK.
 * @note    This API is using to delete oob infomation of all nodes, oob infomation is use for OOB provision.
 */
- (void)deleteAllSigOOBModel;

/**
 * @brief   Obtaining OOB information of node based on UUID of unprovision node.
 * @param   UUIDString    The UUID of unprovision node, app get UUID from scan unprovision node.
 * @return  A SigOOBModel that save oob  infomation. nil means no oob infomation of this UUID.
 * @note    This API is using to get oob infomation of node, oob infomation is use for OOB provision.
 */
- (SigOOBModel *)getSigOOBModelWithUUID:(NSString *)UUIDString;

#pragma mark - new api since v3.3.3

/**
 * @brief   Assign a new address range to the current provisioner. The SDK will automatically call this method internally, without the need for users to actively call it.
 * @return  `YES` means add success, `NO` means add fail.
 * @note    This API is using to add a new UnicastRange to  current provisioner.
 */
- (BOOL)addNewUnicastRangeToCurrentProvisioner;

/**
 * @brief   Initialize data for a mesh network. By default, all parameters are randomly generated. The data in SigDataSource.share (including scanList, sequenceNumber, and sequenceNumberOnDelegate) will not be cleared.
 * @note    This API is using to create a new mesh network.
 */
- (instancetype)initDefaultMesh;

/**
 * @brief   Clear all parameters in SigDataSource.share (including scanList, sequenceNumber, sequenceNumberOnDelegate) and randomly generate new default parameters.
 * @note    This API is using to clear all paramter of SigDataSource. The sequenceNumber of current provisioner of current mesh will save in NSUserDefaults for use next time.
 */
- (void)resetMesh;

/**
 * @brief   Update and save the vid of node to SigDataSource.
 * @param   address    the unicastAddress of node.
 * @param   vid    the versionID of node.
 * @note    This API is using to update and save the vid of node when meshOTA success.
 */
- (void)updateNodeModelVidWithAddress:(UInt16)address vid:(UInt16)vid;

#pragma mark - new api since v4.1.0.0

/**
 * @brief   Method for add extend group information.
 * @note    This API is using to fix the bug about level control of group.(When customers export Mesh, if they do not want to export extension group information, they can call this method to remove the extension group information and then export Mesh.)
 */
-(void)addExtendGroupList;

/**
 * @brief   Method for deleting extend group information.
 * @note    This API is using to fix the bug about level control of group.(When customers export Mesh, if they do not want to export extension group information, they can call this method to remove the extension group information and then export Mesh.)
 */
-(void)removeExtendGroupList;

/**
 * @brief   Create an array of all extend groups for a single base group.
 * @param   groupModel    the base SigGroupModel.
 * @return  A list of SigGroupModel that is base group.
 * @note    This API is using to fix the bug about level control of group.
 */
- (NSArray <SigGroupModel *>*)createExtendGroupListOfGroupModel:(SigGroupModel *)groupModel;

/**
 * @brief   Get all base group arrays for the current Mesh, the base group addresses ranging from 0xC000 to 0xC0FF.
 * @return  A list of SigGroupModel that is base group.
 * @note    This API is using to fix the bug about level control of group.
 */
- (NSArray <SigGroupModel *>*)getAllBaseGroupList;

/**
 * @brief   Get all extend group arrays for the current Mesh, the extend group addresses ranging from 0xD000 to 0xDFFF.
 * @return  A list of SigGroupModel that is extend group.
 * @note    This API is using to fix the bug about level control of group.
 */
- (NSArray <SigGroupModel *>*)getAllExtendGroupList;

/**
 * @brief   Get all invalid group arrays for the current Mesh, the invalid group addresses ranging from 0xE000 to 0xFEFF.
 * @return  A list of SigGroupModel that is invalid.
 * @note    This API is using to fix the bug about level control of group.
 */
- (NSArray <SigGroupModel *>*)getAllInvalidGroupList;

/**
 * @brief   Get a list of all UI displayed groups for the current Mesh, including a basic group list and an invalid group list.
 * @return  A list of SigGroupModel that show on APP.
 * @note    This API is using to fix the bug about level control of group.
 */
- (NSArray <SigGroupModel *>*)getAllShowGroupList;

/**
 * @brief   Calculate the extrnd group address based on the base group address.
 * @param   baseGroupAddress    the base group address.
 * @return  Extend group address is UInt16.
 * @note    This API is using to fix the bug about level control of group.
 */
- (UInt16)getExtendGroupAddressWithBaseGroupAddress:(UInt16)baseGroupAddress;

#pragma mark - Special handling: store the PrivateGattProxy+ConfigGattProxy+PrivateBeacon+ConfigBeacon of node in current mesh

/**
 * @brief   Get loca PrivateGattProxy state.
 * @param   unicastAddress    the unicastAddress of node.
 * @return  PrivateGattProxy state.
 * @note    This API is used to get the value of PrivateGattProxy state stored locally.
 */
- (BOOL)getLocalPrivateGattProxyStateOfUnicastAddress:(UInt16)unicastAddress;

/**
 * @brief   Get loca ConfigGattProxy state.
 * @param   unicastAddress    the unicastAddress of node.
 * @return  ConfigGattProxy state.
 * @note    This API is used to get the value of ConfigGattProxy state stored locally.
 */
- (BOOL)getLocalConfigGattProxyStateOfUnicastAddress:(UInt16)unicastAddress;

/**
 * @brief   Get loca PrivateBeacon state.
 * @param   unicastAddress    the unicastAddress of node.
 * @return  PrivateBeacon state.
 * @note    This API is used to get the value of PrivateBeacon state stored locally.
 */
- (BOOL)getLocalPrivateBeaconStateOfUnicastAddress:(UInt16)unicastAddress;

/**
 * @brief   Get loca ConfigBeacon state.
 * @param   unicastAddress    the unicastAddress of node.
 * @return  ConfigBeacon state.
 * @note    This API is used to get the value of ConfigBeacon state stored locally.
 */
- (BOOL)getLocalConfigBeaconStateOfUnicastAddress:(UInt16)unicastAddress;

/**
 * @brief   Set loca PrivateGattProxy state.
 * @param   state    the PrivateGattProxy state of node.
 * @param   unicastAddress    the unicastAddress of node.
 * @note    This API is used to get the value of PrivateGattProxy state stored locally.
 */
- (void)setLocalPrivateGattProxyState:(BOOL)state unicastAddress:(UInt16)unicastAddress;

/**
 * @brief   Set loca ConfigGattProxy state.
 * @param   state    the ConfigGattProxy state of node.
 * @param   unicastAddress    the unicastAddress of node.
 * @note    This API is used to get the value of ConfigGattProxy state stored locally.
 */
- (void)setLocalConfigGattProxyState:(BOOL)state unicastAddress:(UInt16)unicastAddress;

/**
 * @brief   Set loca PrivateBeacon state.
 * @param   state    the PrivateBeacon state of node.
 * @param   unicastAddress    the unicastAddress of node.
 * @note    This API is used to get the value of PrivateBeacon state stored locally.
 */
- (void)setLocalPrivateBeaconState:(BOOL)state unicastAddress:(UInt16)unicastAddress;

/**
 * @brief   Set loca ConfigBeacon state.
 * @param   state    the ConfigBeacon state of node.
 * @param   unicastAddress    the unicastAddress of node.
 * @note    This API is used to get the value of ConfigBeacon state stored locally.
 */
- (void)setLocalConfigBeaconState:(BOOL)state unicastAddress:(UInt16)unicastAddress;

#pragma mark - Special handling: store the ivIndex+sequenceNumber of current mesh

/**
 * @brief   Use the key meshUUID+provisionerUUID+unicastAddress to store ivIndex+sequenceNumber locally.
 * @param   ivIndex    the ivIndex of mesh.
 * @param   sequenceNumber    the sequenceNumber of mesh.
 */
- (void)saveCurrentIvIndex:(UInt32)ivIndex sequenceNumber:(UInt32)sequenceNumber;

/**
 * @brief   Get local ivIndex.
 * @return  the ivIndex of mesh.
 * @note    This API is used to get the value of ivIndex stored locally.
 */
- (NSString *)getLocalIvIndexString;

/**
 * @brief   Get local sequenceNumber.
 * @return  the sequenceNumber of mesh.
 * @note    This API is used to get the value of sequenceNumber stored locally.
 */
- (NSString *)getLocalSequenceNumberString;

/**
 * @brief   Get local sequenceNumber.
 * @return  the sequenceNumber of mesh.
 * @note    This API is used to get the value of sequenceNumber stored locally.
 */
- (UInt32)getLocalSequenceNumberUInt32;

/**
 * @brief   Get whether local had store IvIndexAndLocalSequenceNumber.
 * @return  `YES` means store, `NO` means no store.
 */
- (BOOL)existLocationIvIndexAndLocationSequenceNumber;

#pragma mark - provisioner UUID API

/**
 * @brief   Save the unique identifier UUID of the current phone, it also the unique identifier UUID of current provisioner.
 * @param   uuid    The unique identifier UUID of the current phone.
 * @note    The unique identifier UUID of the current phone, and it will only be regenerated after uninstalling and reinstalling.
 */
- (void)saveCurrentProvisionerUUID:(NSString *)uuid;

/**
 * @brief   Get the unique identifier UUID of the current phone, it also the unique identifier UUID of current provisioner.
 * @return  The unique identifier UUID of the current phone.
 * @note    The unique identifier UUID of the current phone, and it will only be regenerated after uninstalling and reinstalling.
 */
- (NSString *)getCurrentProvisionerUUID;

#pragma mark - deprecated API

/**
 * @brief   Get the node object through the bluetooth macAddress of node.
 * @param   macAddress    the bluetooth macAddress of node.
 * @return  A SigNodeModel that save node infomation. nil means there are no node with this macAddress in mesh network.
 * @note    The unprovision beacon UUID is the unique identifier of the node, macAddress information is no longer stored in the JSON data.
 */
- (SigNodeModel * _Nullable)getDeviceWithMacAddress:(NSString *)macAddress DEPRECATED_MSG_ATTRIBUTE("The unprovision beacon UUID is the unique identifier of the node, macAddress information is no longer stored in the JSON data.");

@end

NS_ASSUME_NONNULL_END
