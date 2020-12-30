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
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/8/15.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

@class SigNetkeyModel,SigProvisionerModel,SigAppkeyModel,SigSceneModel,SigGroupModel,SigNodeModel, SigIvIndex,SigBaseMeshMessage;

@protocol SigDataSourceDelegate <NSObject>
@optional

/// Callback called when the sequenceNumber or ivIndex change.
/// @param sequenceNumber sequenceNumber of current provisioner.
/// @param ivIndex ivIndex of current mesh network.
- (void)onSequenceNumberUpdate:(UInt32)sequenceNumber ivIndexUpdate:(UInt32)ivIndex;

@end

@interface SigDataSource : NSObject

@property (nonatomic, weak) id <SigDataSourceDelegate>delegate;

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
/// The timestamp property contains a hexadecimal string that contains an integer representing the last time the Provisioner database has been modified. 
@property (nonatomic, copy) NSString *timestamp;

@property (nonatomic, copy) NSString *ivIndex;

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
@property (nonatomic,strong) NSMutableArray <SigNodeModel *>*curNodes;
/// There is the modelID that show in ModelIDListViewController, it is using when app use whiteList at keybind.
@property (nonatomic,strong) NSMutableArray <NSNumber *>*keyBindModelIDs;
/// modelID of subscription group
@property (nonatomic, strong) NSMutableArray <NSNumber *>*defaultGroupSubscriptionModels;
/// default nodeInfo for fast bind.
@property (nonatomic, strong) NSMutableArray <DeviceTypeModel *>*defaultNodeInfos;
/// get from source address of `setFilterForProvisioner:`
@property (nonatomic, assign) UInt16 unicastAddressOfConnected;
@property (nonatomic, assign) BOOL needPublishTimeModel;
@property (nonatomic, strong) NSMutableArray <SigOOBModel *>*OOBList;
/// `YES` means SDK will add staticOOB devcie that never input staticOOB data by noOOB provision. `NO` means SDK will not add staticOOB devcie that never input staticOOB data.
@property (nonatomic, assign) BOOL addStaticOOBDevcieByNoOOBEnable;
/// default retry count of every command. default is 2.
@property (nonatomic, assign) UInt8 defaultRetryCount;
/// 默认一个provisioner分配的设备地址区间，默认值为kAllocatedUnicastRangeHighAddress（0x400）.
@property (nonatomic, assign) UInt16 defaultAllocatedUnicastRangeHighAddress;
/// 默认sequenceNumber的步长，默认值为kSnoIncrement（128）.
@property (nonatomic, assign) UInt8 defaultSnoIncrement;
/// 默认一个unsegmented Access PDU的最大长度，大于该长度则需要进行segment分包，默认值为kUnsegmentedMessageLowerTransportPDUMaxLength（15，如onoff：2bytes opcode + 9bytes data(1byte onoff+1byte TID+7bytes other data) + 4bytes MIC）。默认一个segmented Access PDU的最大长度为kUnsegmentedMessageLowerTransportPDUMaxLength-3。
@property (nonatomic, assign) UInt16 defaultUnsegmentedMessageLowerTransportPDUMaxLength;


//取消该限制：因为客户可以init该类型，用于创建一个中间的mesh数据，用于比较前后的mesh信息。
//+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
//- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));


+ (SigDataSource *)share;

- (NSDictionary *)getDictionaryFromDataSource;
- (void)setDictionaryToDataSource:(NSDictionary *)dictionary;
- (NSDictionary *)getFormatDictionaryFromDataSource;

- (UInt16)provisionAddress;
- (SigProvisionerModel *)curProvisionerModel;
- (NSData *)curNetKey;
- (NSData *)curAppKey;
- (SigNodeModel *)curLocationNodeModel;
- (NSInteger)getOnlineDevicesNumber;
- (BOOL)hasNodeExistTimeModelID;
///Special handling: store the uuid of current provisioner.
- (void)saveCurrentProvisionerUUID:(NSString *)uuid;
///Special handling: get the uuid of current provisioner.
- (NSString *)getCurrentProvisionerUUID;

/// Init SDK location Data(include create mesh.json, check provisioner, provisionLocation)
- (void)configData;

/// check SigDataSource.provisioners, this api will auto create a provisioner when SigDataSource.provisioners hasn't provisioner corresponding to app's UUID.
- (void)checkExistLocationProvisioner;

- (void)changeLocationProvisionerNodeAddressToAddress:(UInt16)address;

- (void)addAndSaveNodeToMeshNetworkWithDeviceModel:(SigNodeModel *)model;

- (void)deleteNodeFromMeshNetworkWithDeviceAddress:(UInt16)deviceAddress;

- (void)editGroupIDsOfDevice:(BOOL)add unicastAddress:(NSNumber *)unicastAddress groupAddress:(NSNumber *)groupAddress;

- (void)setAllDevicesOutline;

- (void)saveLocationData;
- (void)saveLocationProvisionAddress:(NSInteger)address;

- (void)updateNodeStatusWithBaseMeshMessage:(SigBaseMeshMessage *)responseMessage source:(UInt16)source;

- (UInt16)getNewSceneAddress;
- (void)saveSceneModelWithModel:(SigSceneModel *)model;
- (void)delectSceneModelWithModel:(SigSceneModel *)model;

- (NSData *)getIvIndexData;
- (void)updateIvIndexString:(NSString *)ivIndexString;

- (int)getCurrentProvisionerIntSequenceNumber;
- (void)updateCurrentProvisionerIntSequenceNumber:(int)sequenceNumber;
- (void)setLocationSno:(UInt32)sno;

- (SigEncryptedModel *)getSigEncryptedModelWithAddress:(UInt16)address;
///Special handling: determine model whether exist current meshNetwork
- (BOOL)existScanRspModelOfCurrentMeshNetwork:(SigScanRspModel *)model;
///Special handling: determine peripheralUUIDString whether exist current meshNetwork
- (BOOL)existPeripheralUUIDString:(NSString *)peripheralUUIDString;

///Special handling: update the uuid and MAC mapping relationship.
- (void)updateScanRspModelToDataSource:(SigScanRspModel *)model;
- (SigScanRspModel *)getScanRspModelWithUUID:(NSString *)uuid;
- (SigScanRspModel *)getScanRspModelWithMac:(NSString *)mac;
- (SigScanRspModel *)getScanRspModelWithAddress:(UInt16)address;
- (void)deleteScanRspModelWithAddress:(UInt16)address;

- (SigNetkeyModel *)getNetkeyModelWithNetworkId:(NSData *)networkId;
- (SigNetkeyModel *)getNetkeyModelWithNetkeyIndex:(NSInteger)index;

- (SigAppkeyModel *)getAppkeyModelWithAppkeyIndex:(NSInteger)appkeyIndex;

- (SigNodeModel *)getNodeWithUUID:(NSString *)uuid;
- (SigNodeModel *)getNodeWithAddress:(UInt16)address;
- (SigNodeModel *)getDeviceWithMacAddress:(NSString *)macAddress;
- (SigNodeModel *)getCurrentConnectedNode;

- (ModelIDModel *)getModelIDModel:(NSNumber *)modelID;

- (SigGroupModel *)getGroupModelWithGroupAddress:(UInt16)groupAddress;

- (DeviceTypeModel *)getNodeInfoWithCID:(UInt16)CID PID:(UInt16)PID;

#pragma mark - OOB存取相关

- (void)addAndUpdateSigOOBModel:(SigOOBModel *)oobModel;
- (void)addAndUpdateSigOOBModelList:(NSArray <SigOOBModel *>*)oobModelList;
- (void)deleteSigOOBModel:(SigOOBModel *)oobModel;
- (void)deleteAllSigOOBModel;
- (SigOOBModel *)getSigOOBModelWithUUID:(NSString *)UUIDString;

@end
