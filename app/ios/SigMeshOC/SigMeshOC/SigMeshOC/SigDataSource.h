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
//  SigMeshOC
//
//  Created by Liangjiazhi on 2018/9/29.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Model.h"

@class SigDataSource,SigNetkeyModel,SigNetkeyDerivaties,SigProvisionerModel,SigRangeModel,SigSceneRangeModel,SigAppkeyModel,SigSceneModel,SigGroupModel,SigNodeModel,SigRelayretransmitModel,SigNetworktransmitModel,SigFeatureModel,SigNodeKeyModel,SigElementModel,SigModelIDModel,SigPublishModel,SigRetransmitModel, Groups, OpenSSLHelper;

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

@property (nonatomic, copy) NSString *timestamp;

@property (nonatomic, copy) NSString *ivIndex;

@property (assign, nonatomic) struct ProvisionInfo provsionInfo;

@property (nonatomic,strong) NSMutableArray <SigEncryptedModel *>*matchsNodeIdentityArray;
@property (nonatomic,strong) NSMutableArray <SigEncryptedModel *>*noMatchsNodeIdentityArray;

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
///nodes should show in HomeViewController
@property (nonatomic,strong) NSMutableArray <SigNodeModel *>*curNodes;

//There is the modelID that show in ModelIDListViewController, it is using when app use whiteList at keybind.
@property (nonatomic,strong) NSMutableArray <NSNumber *>*keyBindModelIDs;
@property (nonatomic, strong) NSData *curNetKey;
@property (nonatomic, strong) NSData *curAppKey;
//flag for app is or not write JSON data to test.bin of lib on open app.
@property (nonatomic, assign) BOOL hasWriteDataSourceToLib;
@property (nonatomic, strong) NSMutableArray <NSNumber *>*defaultGroupSubscriptionModels;//modelID of subscription group
@property (nonatomic, strong) NSMutableArray <DeviceTypeModel *>*defaultNodeInfos;// default nodeInfo for fast bind.

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

- (BOOL)updateResponseModelWithResponse:(ResponseModel *)response;

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
- (void)deleteSigEncryptedModelWithAddress:(UInt16)address;
- (void)updateScanRspModelToDataSource:(SigScanRspModel *)model;
///Special handling: determine model whether exist current meshNetwork
- (BOOL)existScanRspModel:(SigScanRspModel *)model;
///Special handling: determine peripheralUUIDString whether exist current meshNetwork
- (BOOL)existPeripheralUUIDString:(NSString *)peripheralUUIDString;

- (NSData *)getIvIndexData;
- (void)updateIvIndexData:(NSData *)ivIndex;

- (UInt8)getElementCountOfPid:(UInt16)pid;

- (SigGroupModel *)createNewGroupModelWithGroupName:(NSString *)groupName groupAddress:(UInt16)groupAddress;
- (void)delectGroupModelWithGroupAddress:(UInt16)groupAddress;

///Special handling: get the uuid of current provisioner.
- (NSString *)getCurrentProvisionerUUID;
- (UInt32)getLocationSno;
- (void)setLocationSno:(UInt32)sno;

- (DeviceTypeModel *)getNodeInfoWithCID:(UInt16)CID PID:(UInt16)PID;

@end

@interface SigNetkeyModel : NSObject

@property (nonatomic, copy) NSString *oldKey;

@property (nonatomic, assign) NSInteger index;

@property (nonatomic, assign) NSInteger phase;

@property (nonatomic, copy) NSString *timestamp;

@property (nonatomic, copy) NSString *key;

@property (nonatomic, copy) NSString *name;

@property (nonatomic, copy) NSString *minSecurity;

@property (nonatomic, strong) NSData *networkID;
@property (nonatomic, strong) NSData *oldNetworkID;

@property (nonatomic, strong) SigNetkeyDerivaties *keys;
@property (nonatomic, strong) SigNetkeyDerivaties *oldKeys;

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

- (UInt16)intAddress;

- (instancetype)initWithGroupName:(NSString *)groupName groupAddress:(UInt16)groupAddress;

- (NSDictionary *)getDictionaryOfSigGroupModel;
- (void)setDictionaryToSigGroupModel:(NSDictionary *)dictionary;

@end

///Attention: Boolean type should use bool not BOOL.
@interface SigNodeModel : NSObject<NSCopying>

@property (nonatomic, strong) SigFeatureModel *features;

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

//Attention:macAddress is unreliable, macAddress will remove from json since v3.0.0
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
@property (nonatomic,assign) UInt32 brightness;
@property (nonatomic,assign) UInt32 temperature;
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
- (BOOL)update:(ResponseModel *)m;

///update node status from api getOnlineStatusFromUUIDWithCompletation
- (void)updateOnlineStatusWithDeviceState:(DeviceState)state bright100:(UInt8)bright100 temperature100:(UInt8)temperature100;

- (UInt16)getNewSchedulerID;

- (void)saveSchedulerModelWithModel:(SchedulerModel *)scheduler;

- (UInt8)getElementCount;

- (NSMutableArray *)getAddressesWithModelID:(NSNumber *)sigModelID;

- (instancetype)initWithNode:(SigNodeModel *)node;

- (UInt16)address;
- (void)setAddress:(UInt16)address;

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

- (NSDictionary *)getDictionaryOfSigNodeModel;
- (void)setDictionaryToSigNodeModel:(NSDictionary *)dictionary;

@end

@interface SigRelayretransmitModel : NSObject

@property (nonatomic, assign) NSInteger count;

@property (nonatomic, assign) NSInteger interval;

- (NSDictionary *)getDictionaryOfSigRelayretransmitModel;
- (void)setDictionaryToSigRelayretransmitModel:(NSDictionary *)dictionary;

@end

@interface SigNetworktransmitModel : NSObject

@property (nonatomic, assign) NSInteger count;

@property (nonatomic, assign) NSInteger interval;

- (NSDictionary *)getDictionaryOfSigNetworktransmitModel;
- (void)setDictionaryToSigNetworktransmitModel:(NSDictionary *)dictionary;

@end

@interface SigFeatureModel : NSObject

@property (nonatomic, assign) NSInteger proxy;

@property (nonatomic, assign) NSInteger friend;

@property (nonatomic, assign) NSInteger relay;

@property (nonatomic, assign) NSInteger lowPower;

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

@property (nonatomic, assign) NSInteger index;

- (NSDictionary *)getDictionaryOfSigElementModel;
- (void)setDictionaryToSigElementModel:(NSDictionary *)dictionary;

@end

@interface SigModelIDModel : NSObject

@property (nonatomic, strong) NSMutableArray <NSNumber *>*bind;
//Attention: length=4，为SIG modelID，类型为UInt16；length=8，为vendor modelID，类型为UInt32
@property (nonatomic, copy) NSString *modelId;

@property (nonatomic, strong) NSMutableArray <NSString *>*subscribe;

@property (nonatomic, strong) SigPublishModel *publish;

///返回整形的modelID
- (int)getIntModelID;

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
