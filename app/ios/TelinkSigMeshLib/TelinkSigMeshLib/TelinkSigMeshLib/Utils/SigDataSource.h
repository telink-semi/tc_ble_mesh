/********************************************************************************************************
 * @file     SigDataSource.h
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
//  SigDataSource.h
//  SigMeshLib
//
//  Created by Liangjiazhi on 2019/8/15.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Model.h"
#import "SigEnumeration.h"

@class SigDataSource,SigNetkeyModel,SigNetkeyDerivaties,SigProvisionerModel,SigRangeModel,SigSceneRangeModel,SigAppkeyModel,SigSceneModel,SigGroupModel,SigNodeModel,SigRelayretransmitModel,SigNetworktransmitModel,SigFeatureModel,SigNodeKeyModel,SigElementModel,SigModelIDModel,SigPublishModel,SigRetransmitModel, Groups, OpenSSLHelper,SigIvIndex,SigModelDelegate,SigMeshAddress,SigConfigNetworkTransmitSet,SigConfigNetworkTransmitStatus,SigPage0,SigNodeFeatures,SigBaseMeshMessage;

struct ProvisionInfo {
    Byte prov_newkey[16];
    Byte prov_key_index[2];
    Byte flag;
    Byte prov_iv_index[4];
    Byte prov_unicast_address[2];
};

/// 唯一标识符为identityData，且只存储本地json存在的identityData不为空的SigEncryptedModel。设备断电后会改变identityData，出现相同的address的SigEncryptedModel时，需要replace旧的。
@interface SigEncryptedModel : NSObject
@property (nonatomic, strong) NSData *identityData;
@property (nonatomic, strong) NSData *hashData;
@property (nonatomic, strong) NSData *randomData;
@property (nonatomic, strong) NSString *peripheralUUID;
@property (nonatomic, strong) NSData *encryptedData;
@property (nonatomic, assign) UInt16 address;
@end

@interface SigDataSource : NSObject

@property (nonatomic, strong) NSMutableArray<SigProvisionerModel *> *provisioners;

@property (nonatomic, strong) NSMutableArray<SigNodeModel *> *nodes;

@property (nonatomic, strong) NSMutableArray<SigGroupModel *> *groups;

@property (nonatomic, strong) NSMutableArray<SigSceneModel *> *scenes;

@property (nonatomic, strong) NSMutableArray<SigNetkeyModel *> *netKeys;

@property (nonatomic, strong) NSMutableArray<SigAppkeyModel *> *appKeys;

@property (nonatomic, copy) NSString *meshUUID;

@property (nonatomic, copy) NSString *$schema;

@property (nonatomic, copy) NSString *version;

@property (nonatomic, copy) NSString *meshName;
//The timestamp property contains a hexadecimal string that contains an integer representing the last time the Provisioner database has been modified. 
@property (nonatomic, copy) NSString *timestamp;

@property (nonatomic, copy) NSString *ivIndex;

@property (assign, nonatomic) struct ProvisionInfo provsionInfo;

@property (nonatomic,strong) NSMutableArray <SigEncryptedModel *>*encryptedArray;

/* config value */
//@property (nonatomic,assign) UInt8 defaultFirmwareIDLength;//default is 4.
@property (nonatomic, strong) SigNetkeyModel *netKeyA;
@property (nonatomic, strong) SigAppkeyModel *appKeyA;
@property (nonatomic, strong) SigIvIndex *ivIndexA;


+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));


+ (SigDataSource *)share;

- (NSDictionary *)getDictionaryFromDataSource;
- (void)setDictionaryToDataSource:(NSDictionary *)dictionary;

@property (nonatomic, strong) NSMutableArray<SigScanRspModel *> *scanList;
@property (nonatomic,assign) UInt16 provisionAddress;
///TimeInterval from call provision to keyBind_callback
@property (nonatomic, assign) NSTimeInterval time;

@property (nonatomic, strong) SigAppkeyModel *curAppkeyModel;
@property (nonatomic, strong) SigNetkeyModel *curNetkeyModel;
@property (nonatomic, strong) SigProvisionerModel *curProvisionerModel;
@property (nonatomic, strong) SigNodeModel *curLocationNodeModel;
@property (nonatomic, strong) SigElementModel *curLocationElementModel;
///nodes should show in HomeViewController
@property (nonatomic,strong) NSMutableArray <SigNodeModel *>*curNodes;

//path of mesh.json in location
@property (nonatomic,strong) NSString *path;
//There is the modelID that show in ModelIDListViewController, it is using when app use whiteList at keybind.
@property (nonatomic,strong) NSMutableArray <NSNumber *>*keyBindModelIDs;
@property (nonatomic, strong) NSData *curNetKey;
@property (nonatomic, strong) NSData *curAppKey;
//flag for app is or not write JSON data to test.bin of lib on open app.
@property (nonatomic, assign) BOOL hasWriteDataSourceToLib;
@property (nonatomic, strong) NSMutableArray <NSNumber *>*defaultGroupSubscriptionModels;//modelID of subscription group
@property (nonatomic, strong) NSMutableArray <DeviceTypeModel *>*defaultNodeInfos;// default nodeInfo for fast bind.
@property (nonatomic, assign) UInt16 unicastAddressOfConnected;//get from source address of `setFilterForProvisioner:`

/// Init SDK location Data(include create mesh.json, check provisioner, provisionLocation)
- (void)configData;

/// write SigDataSource To test.bin of lib
- (void)writeDataSourceToLib;

/// check SigDataSource.provisioners, this api will auto create a provisioner when SigDataSource.provisioners hasn't provisioner corresponding to app's UUID.
- (void)checkExistLocationProvisioner;

- (void)changeLocationProvisionerNodeAddressToAddress:(UInt16)address;

- (void)saveDeviceWithDeviceModel:(SigNodeModel *)model;

- (void)removeModelWithDeviceAddress:(UInt16)deviceAddress;

- (void)editGroupIDsOfDevice:(BOOL)add device_address:(NSNumber *)device_address group_address:(NSNumber *)group_address;


- (void)setAllDevicesOutline;

//- (BOOL)updateResponseModelWithResponse:(ResponseModel *)response;

- (void)saveLocationData;

- (NSInteger)getOnlineDevicesNumber;

- (UInt16)provisionAddress;

- (void)saveLocationProvisionAddress:(NSInteger)address;

- (BOOL)hasNodeExistTimeModelID;

- (SigNodeModel *)getNodeWithUUID:(NSString *)uuid;

- (SigNodeModel *)getDeviceWithMacAddress:(NSString *)macAddress;

- (SigNodeModel *)getNodeWithAddress:(UInt16)address;

- (SigNodeModel *)getCurrentConnectedNode;

- (ModelIDModel *)getModelIDModel:(NSNumber *)modelID;

- (UInt16)getNewSceneAddress;

- (void)saveSceneModelWithModel:(SigSceneModel *)model;

- (void)delectSceneModelWithModel:(SigSceneModel *)model;

- (SigScanRspModel *)getScanRspModelWithUUID:(NSString *)uuid;
- (SigScanRspModel *)getScanRspModelWithMac:(NSString *)mac;
- (SigScanRspModel *)getScanRspModelWithAddress:(UInt16)address;
- (void)deleteScanRspModelWithAddress:(UInt16)address;
- (SigEncryptedModel *)getSigEncryptedModelWithAddress:(UInt16)address;
- (void)updateScanRspModelToDataSource:(SigScanRspModel *)model;
///Special handling: determine model whether exist current meshNetwork
- (BOOL)existScanRspModelOfCurrentMeshNetwork:(SigScanRspModel *)model;
///Special handling: determine peripheralUUIDString whether exist current meshNetwork
- (BOOL)existPeripheralUUIDString:(NSString *)peripheralUUIDString;

- (NSData *)getIvIndexData;
- (void)updateIvIndexData:(NSData *)ivIndex;

- (int)getCurrentProvisionerIntSequenceNumber;
- (void)updateCurrentProvisionerIntSequenceNumber:(int)sequenceNumber;

- (SigNetkeyModel *)getNetkeyModelWithNetworkId:(NSData *)networkId;
- (SigAppkeyModel *)getAppkeyModelWithAppkeyIndex:(NSInteger)appkeyIndex;
- (SigNetkeyModel *)getNetkeyModelWithNetkeyIndex:(NSInteger)index;

- (SigGroupModel *)getGroupModelWithGroupAddress:(UInt16)groupAddress;

///Special handling: get the uuid of current provisioner.
- (NSString *)getCurrentProvisionerUUID;
- (UInt32)getLocationSno;
- (void)setLocationSno:(UInt32)sno;

- (void)updateNodeStatusWithBaseMeshMessage:(SigBaseMeshMessage *)responseMessage source:(UInt16)source;

- (DeviceTypeModel *)getNodeInfoWithCID:(UInt16)CID PID:(UInt16)PID;

@end


@interface SigNetkeyModel : NSObject

@property (nonatomic, copy) NSString *oldKey;

@property (nonatomic, assign) UInt16 index;

@property (nonatomic, assign) KeyRefreshPhase phase;

@property (nonatomic, copy) NSString *timestamp;

@property (nonatomic, copy) NSString *key;

@property (nonatomic, copy) NSString *name;

@property (nonatomic, copy) NSString *minSecurity;

@property (nonatomic, strong) SigNetkeyDerivaties *keys;
@property (nonatomic, strong) SigNetkeyDerivaties *oldKeys;

/// Network identifier.
@property (nonatomic, assign) UInt8 nid;
/// Network identifier derived from the old key.
@property (nonatomic, assign) UInt8 oldNid;
/// The IV Index for this subnetwork.
@property (nonatomic, strong) SigIvIndex *ivIndex;
/// The Network ID derived from this Network Key. This identifier
/// is public information.
@property (nonatomic, strong) NSData *networkId;
/// The Network ID derived from the old Network Key. This identifier
/// is public information. It is set when `oldKey` is set.
@property (nonatomic, strong) NSData *oldNetworkId;

- (SigNetkeyDerivaties *)transmitKeys;

/// Returns whether the Network Key is the Primary Network Key.
/// The Primary key is the one which Key Index is equal to 0.
///
/// A Primary Network Key may not be removed from the mesh network.
- (BOOL)isPrimary;

- (NSDictionary *)getDictionaryOfSigNetkeyModel;
- (void)setDictionaryToSigNetkeyModel:(NSDictionary *)dictionary;

@end

@interface SigNetkeyDerivaties : NSObject

@property (nonatomic, strong) NSData *identityKey;

@property (nonatomic, strong) NSData *beaconKey;

@property (nonatomic, strong) NSData *encryptionKey;

@property (nonatomic, strong) NSData *privacyKey;

- (SigNetkeyDerivaties *)initWithNetkeyData:(NSData *)key helper:(OpenSSLHelper *)helper;

@end

@interface SigProvisionerModel : NSObject

@property (nonatomic, strong) NSMutableArray <SigRangeModel *>*allocatedGroupRange;

@property (nonatomic, strong) NSMutableArray <SigRangeModel *>*allocatedUnicastRange;

@property (nonatomic, copy) NSString *UUID;

@property (nonatomic, copy) NSString *provisionerName;

@property (nonatomic, strong) NSMutableArray <SigSceneRangeModel *>*allocatedSceneRange;


/**
 create new provisioner by count of exist provisioners.

 @param count count of exist provisioners
 @param provisionerUUID new provisioner's uuid
 @return SigProvisionerModel model
 */
-(instancetype)initWithExistProvisionerCount:(UInt16)count andProvisionerUUID:(NSString *)provisionerUUID;

- (SigNodeModel *)node;

- (NSDictionary *)getDictionaryOfSigProvisionerModel;
- (void)setDictionaryToSigProvisionerModel:(NSDictionary *)dictionary;

@end

@interface SigRangeModel : NSObject

@property (nonatomic, copy) NSString *lowAddress;

@property (nonatomic, copy) NSString *highAddress;

- (NSInteger)lowIntAddress;
- (NSInteger)hightIntAddress;

- (NSDictionary *)getDictionaryOfSigRangeModel;
- (void)setDictionaryToSigRangeModel:(NSDictionary *)dictionary;

@end

@interface SigSceneRangeModel : NSObject

@property (nonatomic, assign) NSInteger lowAddress;

@property (nonatomic, assign) NSInteger highAddress;

- (NSDictionary *)getDictionaryOfSigSceneRangeModel;
- (void)setDictionaryToSigSceneRangeModel:(NSDictionary *)dictionary;

@end

@interface SigAppkeyModel : NSObject

@property (nonatomic, copy) NSString *oldKey;

@property (nonatomic, copy) NSString *key;

@property (nonatomic, copy) NSString *name;

@property (nonatomic, assign) NSInteger boundNetKey;

@property (nonatomic, assign) NSInteger index;

@property (nonatomic, assign) UInt8 aid;
@property (nonatomic, assign) UInt8 oldAid;

- (SigNetkeyModel *)getCurrentBoundNetKey;
- (NSData *)getDataKey;
- (NSData *)getDataOldKey;

- (NSDictionary *)getDictionaryOfSigAppkeyModel;
- (void)setDictionaryToSigAppkeyModel:(NSDictionary *)dictionary;

@end

@interface SigSceneModel : NSObject<NSCopying>

@property (nonatomic, copy) NSString *name;

@property (nonatomic, assign) NSInteger number;

@property (nonatomic, strong) NSMutableArray <NSString *>*addresses;

//暂时添加并保存json
@property (nonatomic, strong) NSMutableArray <ActionModel *>*actionList;

- (NSDictionary *)getDictionaryOfSigSceneModel;
- (void)setDictionaryToSigSceneModel:(NSDictionary *)dictionary;

@end

@interface SigGroupModel : NSObject

@property (nonatomic, copy) NSString *name;

@property (nonatomic, copy) NSString *address;

@property (nonatomic, copy) NSString *parentAddress;

@property (nonatomic, copy) SigMeshAddress *meshAddress;

- (UInt16)intAddress;

- (NSDictionary *)getDictionaryOfSigGroupModel;
- (void)setDictionaryToSigGroupModel:(NSDictionary *)dictionary;

@end

///Attention: Boolean type should use bool not BOOL.
@interface SigNodeModel : NSObject<NSCopying>

@property (nonatomic, strong) SigNodeFeatures *features;

@property (nonatomic, copy) NSString *unicastAddress;

@property (nonatomic, assign) bool secureNetworkBeacon;

@property (nonatomic, strong) SigRelayretransmitModel *relayRetransmit;

@property (nonatomic, strong) SigNetworktransmitModel *networkTransmit;

@property (nonatomic, assign) bool configComplete;

@property (nonatomic, copy) NSString *vid;

@property (nonatomic, copy) NSString *cid;

@property (nonatomic, assign) bool blacklisted;

@property (nonatomic, copy) NSString *UUID;

@property (nonatomic, copy) NSString *security;

@property (nonatomic, copy) NSString *crpl;

@property (nonatomic, assign) NSInteger defaultTTL;

@property (nonatomic, copy) NSString *pid;

@property (nonatomic, copy) NSString *name;

@property (nonatomic, copy) NSString *deviceKey;

@property (nonatomic, copy) NSString *macAddress;//new add the mac to json, get mac from scanResponse's Manufacturer Data.

@property (nonatomic, strong) NSMutableArray<SigElementModel *> *elements;
@property (nonatomic, strong) NSMutableArray<SigNodeKeyModel *> *netKeys;
@property (nonatomic, strong) NSMutableArray<SigNodeKeyModel *> *appKeys;//node isn't unbound when appkeys is empty.

//暂时添加到json数据中
@property (nonatomic,strong) NSMutableArray <SchedulerModel *>*schedulerList;
//@property (nonatomic, copy) NSString *sno;
//The following properties are not stored JSON
@property (nonatomic,assign) DeviceState state;
@property (nonatomic,assign) BOOL isKeyBindSuccess;
@property (nonatomic,assign) UInt16 brightness;
@property (nonatomic,assign) UInt16 temperature;
@property (nonatomic,assign) UInt16 HSL_Lightness;
@property (nonatomic,assign) UInt16 HSL_Hue;
@property (nonatomic,assign) UInt16 HSL_Saturation;
@property (nonatomic,assign) UInt8 HSL_Lightness100;
@property (nonatomic,assign) UInt8 HSL_Hue100;
@property (nonatomic,assign) UInt8 HSL_Saturation100;
@property (nonatomic,strong) NSMutableArray <NSNumber *>*keyBindModelIDs;//There are modelID that current had key bind.
@property (nonatomic,assign) VC_node_info_t nodeInfo;//That is composition data get from add device process.APP can get ele_cut in provision_end_callback, app can get detail composition data in App_key_bind_end_callback.
@property (nonatomic,strong) NSMutableArray <NSNumber *>*onoffAddresses;//element addresses of onoff
@property (nonatomic,strong) NSMutableArray <NSNumber *>*levelAddresses;//element addresses of level
@property (nonatomic,strong) NSMutableArray <NSNumber *>*temperatureAddresses;//element addresses of color temperature
@property (nonatomic,strong) NSMutableArray <NSNumber *>*HSLAddresses;//element addresses of HSL
@property (nonatomic,strong) NSMutableArray <NSNumber *>*lightnessAddresses;//element addresses of lightness
@property (nonatomic,strong) NSMutableArray <NSNumber *>*schedulerAddress;//element addresses of scheduler
@property (nonatomic,strong) NSMutableArray <NSNumber *>*sceneAddress;//element addresses of scene
@property (nonatomic,strong) NSMutableArray <NSNumber *>*publishAddress;//element addresses of publish
@property (nonatomic,assign) UInt32 publishModelID;//modelID of set publish
@property (nonatomic,strong) NSString *peripheralUUID;

///return node true brightness, range is 0~100
- (UInt8)trueBrightness;

///return node true color temperture, range is 0~100
- (UInt8)trueTemperature;

///update node status, YES means status had changed, NO means status hadn't changed.
//- (BOOL)update:(ResponseModel *)m;

///update node status from api getOnlineStatusFromUUIDWithCompletation
- (void)updateOnlineStatusWithDeviceState:(DeviceState)state bright100:(UInt8)bright100 temperature100:(UInt8)temperature100;

- (UInt16)getNewSchedulerID;

- (void)saveSchedulerModelWithModel:(SchedulerModel *)scheduler;

- (UInt8)getElementCount;

- (NSMutableArray *)getAddressesWithModelID:(NSNumber *)sigModelID;

- (instancetype)initWithNode:(SigNodeModel *)node;

- (UInt16)address;
- (void)setAddress:(UInt16)address;
//- (int)getIntSNO;
//- (void)setIntSno:(UInt32)intSno;

///get all groupIDs of node(获取该设备的所有组号)
- (NSMutableArray <NSNumber *>*)getGroupIDs;

///add new groupID to node(新增设备的组号)
- (void)addGroupID:(NSNumber *)groupID;

///delete old groupID from node(删除设备的组号)
- (void)deleteGroupID:(NSNumber *)groupID;

- (void)openPublish;

- (void)closePublish;

- (BOOL)hasPublishFunction;

- (BOOL)hasOpenPublish;

///publish是否存在周期上报功能。
- (BOOL)hasPublishPeriod;

- (BOOL)isSensor;

/// Returns list of Network Keys known to this Node.
- (NSArray <SigNetkeyModel *>*)getNetworkKeys;

/// The last unicast address allocated to this node. Each node's element
/// uses its own subsequent unicast address. The first (0th) element is identified
/// by the node's unicast address. If there are no elements, the last unicast address
/// is equal to the node's unicast address.
- (UInt16)lastUnicastAddress;

/// Returns whether the address uses the given unicast address for one
/// of its elements.
///
/// - parameter addr: Address to check.
/// - returns: `True` if any of node's elements (or the node itself) was assigned
///            the given address, `false` otherwise.
- (BOOL)hasAllocatedAddr:(UInt16)addr;

- (SigModelIDModel *)getModelIDModelWithModelID:(UInt16)modelID;
- (SigModelIDModel *)getModelIDModelWithModelID:(UInt16)modelID andElementAddress:(UInt16)elementAddress;

- (NSDictionary *)getDictionaryOfSigNodeModel;
- (void)setDictionaryToSigNodeModel:(NSDictionary *)dictionary;

- (void)setAddSigAppkeyModelSuccess:(SigAppkeyModel *)appkey;
- (void)setCompositionData:(SigPage0 *)compositionData;
- (void)setBindSigNodeKeyModel:(SigNodeKeyModel *)appkey toSigModelIDModel:(SigModelIDModel *)modelID;

- (void)updateNodeStatusWithBaseMeshMessage:(SigBaseMeshMessage *)responseMessage source:(UInt16)source;

@end

@interface SigRelayretransmitModel : NSObject

@property (nonatomic, assign) NSInteger count;

@property (nonatomic, assign) NSInteger interval;
/// Number of 10-millisecond steps between transmissions.
@property (nonatomic, assign) UInt8 steps;

- (NSDictionary *)getDictionaryOfSigRelayretransmitModel;
- (void)setDictionaryToSigRelayretransmitModel:(NSDictionary *)dictionary;

@end

/// The object represents parameters of the transmissions of network
/// layer messages originating from a mesh node.
@interface SigNetworktransmitModel : NSObject
/// Number of retransmissions for relay messages.
/// The value is in range from 1 to 8.
@property (nonatomic, assign) NSInteger count;
/// The interval (in milliseconds) between retransmissions
/// (from 10 to 320 ms in 10 ms steps).
@property (nonatomic, assign) NSInteger interval;
/// Number of 10-millisecond steps between transmissions.
- (UInt8)septs;

/// The interval in as `TimeInterval` in seconds.
- (NSTimeInterval)timeInterval;

- (instancetype)initWithSigConfigNetworkTransmitSet:(SigConfigNetworkTransmitSet *)request;

- (instancetype)initWithSigConfigNetworkTransmitStatus:(SigConfigNetworkTransmitStatus *)status;

- (NSDictionary *)getDictionaryOfSigNetworktransmitModel;
- (void)setDictionaryToSigNetworktransmitModel:(NSDictionary *)dictionary;

@end

/// The features object represents the functionality of a mesh node that is determined by the set features that the node supports.
@interface SigNodeFeatures : NSObject

/// The state of Relay feature. Default is 2.
@property (nonatomic,assign) SigNodeFeaturesState relayFeature;
/// The state of Proxy feature. Default is 2.
@property (nonatomic,assign) SigNodeFeaturesState proxyFeature;
/// The state of Friend feature. Default is 2.
@property (nonatomic,assign) SigNodeFeaturesState friendFeature;
/// The state of Low Power feature. Default is 2.
@property (nonatomic,assign) SigNodeFeaturesState lowPowerFeature;

- (UInt16)rawValue;
- (instancetype)initWithRawValue:(UInt16)rawValue;
- (instancetype)initWithRelay:(SigNodeFeaturesState)relayFeature proxy:(SigNodeFeaturesState)proxyFeature friend:(SigNodeFeaturesState)friendFeature lowPower:(SigNodeFeaturesState)lowPowerFeature;

- (NSDictionary *)getDictionaryOfSigFeatureModel;
- (void)setDictionaryToSigFeatureModel:(NSDictionary *)dictionary;

@end

@interface SigNodeKeyModel : NSObject
/// The Key index for this network key.
@property (nonatomic, assign) UInt16 index;
/// This flag contains value set to `false`, unless a Key Refresh
/// procedure is in progress and the network has been successfully
/// updated.
@property (nonatomic, assign) bool updated;
- (instancetype)initWithIndex:(UInt16)index updated:(bool)updated;

- (NSDictionary *)getDictionaryOfSigNodeKeyModel;
- (void)setDictionaryToSigNodeKeyModel:(NSDictionary *)dictionary;

@end

@interface SigElementModel : NSObject

@property (nonatomic, strong) NSMutableArray<SigModelIDModel *> *models;

@property (nonatomic, copy) NSString *name;

@property (nonatomic, copy) NSString *location;

@property (nonatomic, assign) UInt8 index;

/// Parent Node. This may be `nil` if the Element was obtained in
/// Composition Data and has not yet been added to a Node.
@property (nonatomic,strong) SigNodeModel *parentNode;

- (instancetype)initWithLocation:(SigLocation)location;

- (instancetype)initWithCompositionData:(NSData *)compositionData offset:(int *)offset;

/// Returns the Unicast Address of the Element.
/// For Elements not added to Node this returns the Element index
/// value as `Address`.
- (UInt16)unicastAddress;

- (SigLocation)getSigLocation;
- (void)setSigLocation:(SigLocation)sigLocation;

- (NSDictionary *)getDictionaryOfSigElementModel;
- (void)setDictionaryToSigElementModel:(NSDictionary *)dictionary;

- (NSData *)getElementData;

@end

@interface SigModelIDModel : NSObject
/// An array of Appliaction Key indexes to which this model is bound.
@property (nonatomic, strong) NSMutableArray <NSNumber *>*bind;//[KeyIndex]
//Attention: length=4，为SIG modelID，类型为UInt16；length=8，为vendor modelID，类型为UInt32
@property (nonatomic, copy) NSString *modelId;
/// The array of Unicast or Group Addresses (4-character hexadecimal value),
/// or Virtual Label UUIDs (32-character hexadecimal string).
@property (nonatomic, strong) NSMutableArray <NSString *>*subscribe;
/// The configuration of this Model's publication.
@property (nonatomic, strong) SigPublishModel *publish;
/// The model message handler. This is non-`nil` for supported local Models
/// and `nil` for Models of remote Nodes.
@property (nonatomic,weak) id delegate;
/// Parent Element.
@property (nonatomic,strong) SigElementModel *parentElement;

///返回整形的modelID
- (int)getIntModelID;

- (instancetype)initWithSigModelId:(UInt16)sigModelId;

- (instancetype)initWithVendorModelId:(UInt32)vendorModelId;

- (instancetype)initWithVendorModelId:(UInt32)vendorModelId withDelegate:(SigModelDelegate *)delegate;

- (instancetype)initWithSigModelId:(UInt16)sigModelId companyId:(UInt16)companyId delegate:(SigModelDelegate *)delegate;

- (instancetype)initWithSigModelId:(UInt16)sigModelId delegate:(SigModelDelegate *)delegate;

/// Bluetooth SIG or vendor-assigned model identifier.
- (UInt16)modelIdentifier;
/// The Company Identifier or `nil`, if the model is Bluetooth SIG-assigned.
- (UInt16)companyIdentifier;
/// Returns `true` for Models with identifiers assigned by Bluetooth SIG,
/// `false` otherwise.
- (BOOL)isBluetoothSIGAssigned;
/// Returns the list of known Groups that this Model is subscribed to.
/// It may be that the Model is subscribed to some other Groups, which are
/// not known to the local database, and those are not returned.
/// Use `isSubscribed(to:)` to check other Groups.
- (NSArray <SigGroupModel *>*)subscriptions;

- (BOOL)isConfigurationServer;
- (BOOL)isConfigurationClient;
- (BOOL)isHealthServer;
- (BOOL)isHealthClient;

/// Adds the given Application Key Index to the bound keys.
///
/// - paramter applicationKeyIndex: The Application Key index to bind.
- (void)bindApplicationKeyWithIndex:(UInt16)applicationKeyIndex;

/// Removes the Application Key binding to with the given Key Index
/// and clears the publication, if it was set to use the same key.
///
/// - parameter applicationKeyIndex: The Application Key index to unbind.
- (void)unbindApplicationKeyWithIndex:(UInt16)applicationKeyIndex;

/// Adds the given Group to the list of subscriptions.
///
/// - parameter group: The new Group to be added.
- (void)subscribeToGroup:(SigGroupModel *)group;

/// Removes the given Group from list of subscriptions.
///
/// - parameter group: The Group to be removed.
- (void)unsubscribeFromGroup:(SigGroupModel *)group;

/// Removes the given Address from list of subscriptions.
///
/// - parameter address: The Address to be removed.
- (void)unsubscribeFromAddress:(UInt16)address;

/// Removes all subscribtions from this Model.
- (void)unsubscribeFromAll;

/// List of Application Keys bound to this Model.
///
/// The list will not contain unknown Application Keys bound
/// to this Model, possibly bound by other Provisioner.
- (NSMutableArray <SigAppkeyModel *>*)boundApplicationKeys;

/// Whether the given Application Key is bound to this Model.
///
/// - parameter applicationKey: The key to check.
/// - returns: `True` if the key is bound to this Model,
///            otherwise `false`.
- (BOOL)isBoundToApplicationKey:(SigAppkeyModel *)applicationKey;

/// Returns whether the given Model is compatible with the one.
///
/// A compatible Models create a Client-Server pair. I.e., the
/// Generic On/Off Client is compatible to Generic On/Off Server,
/// and vice versa. The rule is that the Server Model has an even
/// Model ID and the Client Model has Model ID greater by 1.
///
/// - parameter model: The Model to compare to.
/// - returns: `True`, if the Models are compatible, `false` otherwise.
- (BOOL)isCompatibleToModel:(SigModelIDModel *)model;

/// Returns whether the Model is subscribed to the given Group.
///
/// This method may also return `true` if the Group is not known
/// to the local Provisioner and is not returned using `subscriptions`
/// property.
///
/// - parameter group: The Group to check subscription to.
/// - returns: `True` if the Model is subscribed to the Group,
///            `false` otherwise.
- (BOOL)isSubscribedToGroup:(SigGroupModel *)group;

/// Returns whether the Model is subscribed to the given address.
///
/// This method may also return `true` if the address is not known
/// to the local Provisioner and is a Group with this address is
/// not returned using `subscriptions` property.
/// Moreover, if a Virtual Label of a Group is not known, but the
/// 16-bit address is known, and the given address contains the Virtual
/// Label, with the same 16-bit address, this method will return `false`,
/// as it may not guarantee that the labels are the same.
///
/// - parameter address: The address to check subscription to.
/// - returns: `True` if the Model is subscribed to a Group with given,
///            address, `false` otherwise.
- (BOOL)isSubscribedToAddress:(SigMeshAddress *)address;

- (NSDictionary *)getDictionaryOfSigModelIDModel;
- (void)setDictionaryToSigModelIDModel:(NSDictionary *)dictionary;

@end

@interface SigPublishModel : NSObject

@property (nonatomic, assign) NSInteger index;

@property (nonatomic, assign) NSInteger credentials;

@property (nonatomic, assign) NSInteger ttl;

@property (nonatomic, strong) SigRetransmitModel *retransmit;

@property (nonatomic, assign) NSInteger period;

@property (nonatomic, copy) NSString *address;

- (NSDictionary *)getDictionaryOfSigPublishModel;
- (void)setDictionaryToSigPublishModel:(NSDictionary *)dictionary;

@end

@interface SigRetransmitModel : NSObject

@property (nonatomic, assign) NSInteger count;

@property (nonatomic, assign) NSInteger interval;

- (NSDictionary *)getDictionaryOfSigRetransmitModel;
- (void)setDictionaryToSigRetransmitModel:(NSDictionary *)dictionary;

@end
