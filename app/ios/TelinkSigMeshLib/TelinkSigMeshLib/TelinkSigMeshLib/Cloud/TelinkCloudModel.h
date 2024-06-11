/********************************************************************************************************
 * @file     TelinkCloudModel.h
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/12/4
 *
 * @par     Copyright (c) 2023, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

typedef enum : NSUInteger {
    /// 申请中
    ResponseShareStateApplying = 0,
    /// 访客
    ResponseShareStateVisitor = 1,
    /// 成员
    ResponseShareStateMember = 2,
    /// 禁止
    ResponseShareStateForbidden = 3,
    /// 所有
    ResponseShareStateAll = 0xFF,
} ResponseShareState;

typedef enum : NSUInteger {
    TelinkUserUsage_register = 1,
    TelinkUserUsage_resetPassword = 2,
} TelinkUserUsage;


@class CloudSchedulerModel,CloudSceneModel;


@interface TelinkCloudModel : NSObject

@end


/// user info of cloud.
@interface UserInfoModel : TelinkCloudModel
/// company
@property (nonatomic, copy) NSString *company;
/// email
@property (nonatomic, copy) NSString *email;
/// last login time
@property (nonatomic, copy) NSString *lastLoginTime;
/// name
@property (nonatomic, copy) NSString *name;
/// nickname
@property (nonatomic, copy) NSString *nickname;
/// password
@property (nonatomic, copy) NSString *password;
/// phone
@property (nonatomic, copy) NSString *phone;
/// sign
@property (nonatomic, copy) NSString *sign;
/// updateTime
@property (nonatomic, copy) NSString *updateTime;
/// createTime
@property (nonatomic, assign) long createTime;
/// userId
@property (nonatomic, assign) NSInteger userId;

/// get dictionary from UserInfoModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfUserInfoModel;

/// Set dictionary to UserInfoModel object.
/// @param dictionary UserInfoModel dictionary object.
- (void)setDictionaryToUserInfoModel:(NSDictionary *)dictionary;

@end


/// Mesh info of created network list in the cloud.
@interface AppMeshNetworkModel : TelinkCloudModel
/// createTime
@property (nonatomic, assign) long createTime;
/// createUserId
@property (nonatomic, assign) NSInteger createUserId;
/// networkId
/// 对应createMeshNetwork返回的data/id
@property (nonatomic, assign) NSInteger networkId;
/// ivIndex
@property (nonatomic, assign) NSInteger ivIndex;
/// nodeAddressIndex
@property (nonatomic, assign) NSInteger nodeAddressIndex;
/// provisionerAddressIndex
@property (nonatomic, assign) NSInteger provisionerAddressIndex;
/// sharePermission
@property (nonatomic, assign) NSInteger sharePermission;
/// name
@property (nonatomic, copy) NSString *name;
/// networkSchema
@property (nonatomic, copy) NSString *networkSchema;
/// uuid
/// eg:"9bb6cb80-ee15-4b56-ad49-09c6c11cde62"
@property (nonatomic, copy) NSString *uuid;
/// version
@property (nonatomic, copy) NSString *version;
/// createUserName
@property (nonatomic, copy) NSString *createUserName;

/// get dictionary from AppMeshNetworkModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfAppMeshNetworkModel;

/// Set dictionary to AppMeshNetworkModel object.
/// @param dictionary AppMeshNetworkModel dictionary object.
- (void)setDictionaryToAppMeshNetworkModel:(NSDictionary *)dictionary;

@end


/// Mesh info of joined network list in the cloud.
@interface AppJoinedNetworkModel : TelinkCloudModel
/// createTime
@property (nonatomic, assign) long createTime;
/// updateTime
@property (nonatomic, assign) long updateTime;
/// applyUserId
@property (nonatomic, assign) NSInteger applyUserId;
/// joinNetworkId
@property (nonatomic, assign) NSInteger joinNetworkId;
/// networkId
@property (nonatomic, assign) NSInteger networkId;
/// state
@property (nonatomic, assign) ResponseShareState state;
/// applyUserName
@property (nonatomic, copy) NSString *applyUserName;
/// networkCreator
@property (nonatomic, copy) NSString *networkCreator;
/// networkName
@property (nonatomic, copy) NSString *networkName;

/// get dictionary from AppJoinedNetworkModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfAppJoinedNetworkModel;

/// Set dictionary to AppJoinedNetworkModel object.
/// @param dictionary AppJoinedNetworkModel dictionary object.
- (void)setDictionaryToAppJoinedNetworkModel:(NSDictionary *)dictionary;

@end


/// Application key info  in the cloud.
@interface CloudApplicationKeyModel : TelinkCloudModel
/// createTime
@property (nonatomic, assign) long createTime;
/// applicationKeyId
@property (nonatomic, assign) NSInteger applicationKeyId;
/// networkId
@property (nonatomic, assign) NSInteger networkId;
/// networkKeyId
@property (nonatomic, assign) NSInteger networkKeyId;
/// index
@property (nonatomic, assign) NSInteger index;
/// modelDescription
@property (nonatomic, copy) NSString *modelDescription;
/// key
@property (nonatomic, copy) NSString *key;
/// name
@property (nonatomic, copy) NSString *name;
/// oldKey
@property (nonatomic, copy) NSString *oldKey;
/// updateTime
@property (nonatomic, copy) NSString *updateTime;

/// get dictionary from CloudApplicationKeyModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfCloudApplicationKeyModel;

/// Set dictionary to CloudApplicationKeyModel object.
/// @param dictionary CloudApplicationKeyModel dictionary object.
- (void)setDictionaryToCloudApplicationKeyModel:(NSDictionary *)dictionary;

@end


/// Group info  in the cloud.
@interface CloudGroupModel : TelinkCloudModel
/// createTime
@property (nonatomic, assign) long createTime;
/// address
@property (nonatomic, assign) NSInteger address;
/// createProvisionerId
@property (nonatomic, assign) NSInteger createProvisionerId;
/// createUserId
@property (nonatomic, assign) NSInteger createUserId;
/// groupId
@property (nonatomic, assign) NSInteger groupId;
/// networkId
@property (nonatomic, assign) NSInteger networkId;
/// name
@property (nonatomic, copy) NSString *name;
/// updateTime
@property (nonatomic, copy) NSString *updateTime;

/// get dictionary from CloudGroupModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfCloudGroupModel;

/// Set dictionary to CloudGroupModel object.
/// @param dictionary CloudGroupModel dictionary object.
- (void)setDictionaryToCloudGroupModel:(NSDictionary *)dictionary;

@end


/// Network key info  in the cloud.
@interface CloudNetworkKeyModel : TelinkCloudModel
/// createTime
@property (nonatomic, assign) long createTime;
/// networkKeyId
@property (nonatomic, assign) NSInteger networkKeyId;
/// index
@property (nonatomic, assign) NSInteger index;
/// networkId
@property (nonatomic, assign) NSInteger networkId;
/// phase
@property (nonatomic, assign) NSInteger phase;
/// name
@property (nonatomic, copy) NSString *name;
/// key
@property (nonatomic, copy) NSString *key;
/// oldKey
@property (nonatomic, copy) NSString *oldKey;
/// modelDescription
@property (nonatomic, copy) NSString *modelDescription;
/// updateTime
@property (nonatomic, copy) NSString *updateTime;

/// get dictionary from CloudNetworkKeyModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfCloudNetworkKeyModel;

/// Set dictionary to CloudNetworkKeyModel object.
/// @param dictionary CloudNetworkKeyModel dictionary object.
- (void)setDictionaryToCloudNetworkKeyModel:(NSDictionary *)dictionary;

@end


/// Product info  in the cloud.
@interface CloudProductInfoModel : TelinkCloudModel
/// createTime
@property (nonatomic, assign) long createTime;
/// productInfoId
@property (nonatomic, assign) NSInteger productInfoId;
/// manufactureId
@property (nonatomic, assign) NSInteger manufactureId;
/// pid
@property (nonatomic, assign) NSInteger pid;
/// name
@property (nonatomic, copy) NSString *name;
/// updateTime
@property (nonatomic, copy) NSString *updateTime;
/// info
@property (nonatomic, copy) NSString *info;
/// manufactureName
@property (nonatomic, copy) NSString *manufactureName;

/// get dictionary from CloudProductInfoModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfCloudProductInfoModel;

/// Set dictionary to CloudProductInfoModel object.
/// @param dictionary CloudProductInfoModel dictionary object.
- (void)setDictionaryToCloudProductInfoModel:(NSDictionary *)dictionary;

@end


/// Publish info  in the cloud.
@interface CloudPublishInfoModel : TelinkCloudModel
/// createTime
@property (nonatomic, assign) long createTime;
/// appKeyIndex
@property (nonatomic, assign) NSInteger appKeyIndex;
/// credentialFlag
@property (nonatomic, assign) NSInteger credentialFlag;
/// elementAddress
@property (nonatomic, assign) NSInteger elementAddress;
/// publishId
@property (nonatomic, assign) NSInteger publishId;//id
/// isSigModel
@property (nonatomic, assign) NSInteger isSigModel;
/// modelId
@property (nonatomic, assign) NSInteger modelId;
/// nodeId
@property (nonatomic, assign) NSInteger nodeId;
/// period
@property (nonatomic, assign) NSInteger period;
/// publishAddress
@property (nonatomic, assign) NSInteger publishAddress;
/// retransmitCount
@property (nonatomic, assign) NSInteger retransmitCount;
/// retransmitIntervalSteps
@property (nonatomic, assign) NSInteger retransmitIntervalSteps;
/// ttl
@property (nonatomic, assign) NSInteger ttl;

/// get dictionary from CloudPublishInfoModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfCloudPublishInfoModel;

/// Set dictionary to CloudPublishInfoModel object.
/// @param dictionary CloudPublishInfoModel dictionary object.
- (void)setDictionaryToCloudPublishInfoModel:(NSDictionary *)dictionary;

@end


/// Node configs in the cloud.
@interface CloudNodeConfigsModel : TelinkCloudModel
/// 设备配置ID
@property (nonatomic, assign) NSInteger configId;
/// defaultTtl
@property (nonatomic, strong) NSNumber *defaultTtl;
/// relay
@property (nonatomic, strong) NSNumber *relay;
/// relayRetransmit
@property (nonatomic, strong) NSNumber *relayRetransmit;
/// secureNetworkBeacon
@property (nonatomic, strong) NSNumber *secureNetworkBeacon;
/// gattProxy
@property (nonatomic, strong) NSNumber *gattProxy;
/// privateBeacon
@property (nonatomic, strong) NSNumber *privateBeacon;
/// privateGattProxy
@property (nonatomic, strong) NSNumber *privateGattProxy;
/// friendConfig, key is friend
@property (nonatomic, strong) NSNumber *friendConfig;
/// networkRetransmit
@property (nonatomic, strong) NSNumber *networkRetransmit;
/// onDemandPrivateGattProxy
@property (nonatomic, strong) NSNumber *onDemandPrivateGattProxy;
/// directForwarding
@property (nonatomic, strong) NSNumber *directForwarding;
/// directRelay
@property (nonatomic, strong) NSNumber *directRelay;
/// directProxy
@property (nonatomic, strong) NSNumber *directProxy;
/// directFriend
@property (nonatomic, strong) NSNumber *directFriend;

/// get dictionary from CloudNodeConfigsModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfCloudNodeConfigsModel;

/// Set dictionary to CloudNodeConfigsModel object.
/// @param dictionary CloudNodeConfigsModel dictionary object.
- (void)setDictionaryToCloudNodeConfigsModel:(NSDictionary *)dictionary;

@end


/// Version info  in the cloud.
@interface CloudVersionInfoModel : TelinkCloudModel
/// createTime
@property (nonatomic, assign) long createTime;
/// elementCount
@property (nonatomic, assign) NSInteger elementCount;
/// versionInfoId
@property (nonatomic, assign) NSInteger versionInfoId;
/// productId
@property (nonatomic, assign) NSInteger productId;
/// state
@property (nonatomic, assign) NSInteger state;
/// vid
@property (nonatomic, assign) NSInteger vid;
/// compositionData
@property (nonatomic, copy) NSString *compositionData;
/// binFilePath
@property (nonatomic, copy) NSString *binFilePath;
/// info
@property (nonatomic, copy) NSString *info;
/// name
@property (nonatomic, copy) NSString *name;
/// updateTime
@property (nonatomic, copy) NSString *updateTime;

/// get dictionary from CloudVersionInfoModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfCloudVersionInfoModel;

/// Set dictionary to CloudVersionInfoModel object.
/// @param dictionary CloudVersionInfoModel dictionary object.
- (void)setDictionaryToCloudVersionInfoModel:(NSDictionary *)dictionary;

@end


/// Node info  in the cloud.
@interface CloudNodeModel : TelinkCloudModel
/// createTime
@property (nonatomic, assign) long createTime;
/// address
@property (nonatomic, assign) NSInteger address;
/// bindState
@property (nonatomic, assign) NSInteger bindState;
/// createUserId
@property (nonatomic, assign) NSInteger createUserId;
/// nodeId
@property (nonatomic, assign) NSInteger nodeId;
/// meshProvisionerId
@property (nonatomic, assign) NSInteger meshProvisionerId;
/// networkId
@property (nonatomic, assign) NSInteger networkId;
/// productId
@property (nonatomic, assign) NSInteger productId;
/// state
@property (nonatomic, assign) NSInteger state;
/// name
@property (nonatomic, copy) NSString *name;
/// nodeDescription
@property (nonatomic, copy) NSString *nodeDescription;
/// deviceKey
@property (nonatomic, copy) NSString *deviceKey;
/// deviceUuid
@property (nonatomic, copy) NSString *deviceUuid;
/// updateTime
@property (nonatomic, copy) NSString *updateTime;
/// versionId
@property (nonatomic, copy) NSString *versionId;
/// networkName
@property (nonatomic, copy) NSString *networkName;
/// productName
@property (nonatomic, copy) NSString *productName;
/// subList
@property (nonatomic, strong) NSMutableArray<CloudGroupModel *> *subList;
/// productInfo
@property (nonatomic, strong) CloudProductInfoModel *productInfo;
/// versionInfo
@property (nonatomic, strong) CloudVersionInfoModel *versionInfo;
/// publishInfo
@property (nonatomic, strong, nullable) CloudPublishInfoModel *publishInfo;
/// configsInfo
@property (nonatomic, strong, nullable) CloudNodeConfigsModel *configsInfo;
/// schedulerList
@property (nonatomic, strong) NSMutableArray<CloudSchedulerModel *> *schedulerList;

/// get dictionary from CloudNodeModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfCloudNodeModel;

/// Set dictionary to CloudNodeModel object.
/// @param dictionary CloudNodeModel dictionary object.
- (void)setDictionaryToCloudNodeModel:(NSDictionary *)dictionary;

/// delete scheduler with schedulerId.
/// @param schedulerId cloud schedulerId.
- (void)deleteCloudSchedulerModelWithSchedulerId:(NSInteger)schedulerId;

@end


/// Provisioner info  in the cloud.
@interface CloudProvisionerModel : TelinkCloudModel
/// createTime
@property (nonatomic, assign) long createTime;
/// address
@property (nonatomic, assign) NSInteger address;
/// provisionerId
@property (nonatomic, assign) NSInteger provisionerId;
/// networkId
@property (nonatomic, assign) NSInteger networkId;
/// sequenceNumber
@property (nonatomic, assign) NSInteger sequenceNumber;
/// userId
@property (nonatomic, assign) NSInteger userId;
/// name
@property (nonatomic, copy) NSString *name;
/// clientId
@property (nonatomic, copy) NSString *clientId;

/// get dictionary from CloudProvisionerModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfCloudProvisionerModel;

/// Set dictionary to CloudProvisionerModel object.
/// @param dictionary CloudProvisionerModel dictionary object.
- (void)setDictionaryToCloudProvisionerModel:(NSDictionary *)dictionary;

@end


/// Network detail info  in the cloud.
@interface CloudNetworkDetailModel : TelinkCloudModel
/// createTime
@property (nonatomic, assign) long createTime;
/// networkId
@property (nonatomic, assign) NSInteger networkId;
/// ivIndex
@property (nonatomic, assign) NSInteger ivIndex;
/// role
@property (nonatomic, assign) NSInteger role;
/// name
@property (nonatomic, copy) NSString *name;
/// createUser
@property (nonatomic, strong) NSString *createUser;
/// provisioner
@property (nonatomic, strong) CloudProvisionerModel *provisioner;
/// networkKeyList
@property (nonatomic, strong) NSMutableArray<CloudNetworkKeyModel *> *networkKeyList;
/// applicationKeyList
@property (nonatomic, strong) NSMutableArray<CloudApplicationKeyModel *> *applicationKeyList;
/// groupList
@property (nonatomic, strong) NSMutableArray<CloudGroupModel *> *groupList;
/// nodeList
@property (nonatomic, strong) NSMutableArray<CloudNodeModel *> *nodeList;
/// sceneList
@property (nonatomic, strong) NSMutableArray<CloudSceneModel *> *sceneList;

/// get dictionary from CloudNetworkDetailModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfCloudNetworkDetailModel;

/// Set dictionary to CloudNetworkDetailModel object.
/// @param dictionary CloudNetworkDetailModel dictionary object.
- (void)setDictionaryToCloudNetworkDetailModel:(NSDictionary *)dictionary;

@end


/// Scheduler info  in the cloud.
@interface CloudSchedulerModel : TelinkCloudModel
/// createTime
@property (nonatomic, assign) long createTime;
/// schedulerId
/// key is id
@property (nonatomic, assign) NSInteger schedulerId;
/// meshNodeId
@property (nonatomic, assign) NSInteger meshNodeId;
/// meshSceneId
@property (nonatomic, assign) NSInteger meshSceneId;
/// schedulerIndex
@property (nonatomic, assign) NSInteger schedulerIndex;
/// name
@property (nonatomic, copy) NSString *name;
/// params
@property (nonatomic, copy) NSString *params;
/// info
@property (nonatomic, copy) NSString *info;

/// get dictionary from CloudSchedulerModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfCloudSchedulerModel;

/// Set dictionary to CloudSchedulerModel object.
/// @param dictionary CloudSchedulerModel dictionary object.
- (void)setDictionaryToCloudSchedulerModel:(NSDictionary *)dictionary;

@end


/// Scene info  in the cloud.
@interface CloudSceneModel : TelinkCloudModel
/// createTime
@property (nonatomic, assign) long createTime;
/// cloudSceneId
/// key is id
@property (nonatomic, assign) NSInteger cloudSceneId;
/// meshNetworkId
@property (nonatomic, assign) NSInteger meshNetworkId;
/// sceneId
@property (nonatomic, assign) NSInteger sceneId;
/// name
@property (nonatomic, copy) NSString *name;
/// info
@property (nonatomic, copy) NSString *info;

/// get dictionary from CloudSceneModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfCloudSceneModel;

/// Set dictionary to CloudSceneModel object.
/// @param dictionary CloudSceneModel dictionary object.
- (void)setDictionaryToCloudSceneModel:(NSDictionary *)dictionary;

@end

NS_ASSUME_NONNULL_END
