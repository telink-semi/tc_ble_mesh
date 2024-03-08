/********************************************************************************************************
 * @file     TelinkHttpTool.h
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/1/10
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
#import "AppDataSource.h"

NS_ASSUME_NONNULL_BEGIN

typedef void (^MyBlock) (id _Nullable result, NSError * _Nullable err);

@interface TelinkHttpTool : NSObject

/// 发送一个GET请求
/// - Parameters:
///   - url: 请求路径
///   - params: 请求参数
///   - success: 请求成功后的回调（请将请求成功后想做的事情写到这个block中）
///   - failure: 请求失败后的回调（请将请求失败后想做的事情写到这个block中）
+ (void)getRequestWithUrl:(NSString *)url params:( NSDictionary *)params success:(void(^)(id json))success failure:(void(^)(NSError *error))failure;

/// 发送一个POST请求
/// - Parameters:
///   - url: 请求路径
///   - params: 请求参数
///   - success: 请求成功后的回调（请将请求成功后想做的事情写到这个block中）
///   - failure: 请求失败后的回调（请将请求失败后想做的事情写到这个block中）
+ (void)postRequestWithUrl:(NSString *)url params:( NSDictionary *)params success:(void(^)(id json))success failure:(void(^)(NSError *error))failure;

#pragma mark - 用户管理 : User Controller 相关

/// 1.checkNameAvailable 检查用户名是否可用
/// @param name user name
/// @param block result callback
+ (void)checkNameAvailableRequestWithName:(NSString *)name didLoadData:(MyBlock)block;

/// 2.getVerificationCode 获取验证码
/// @param mailAddress user mail
/// @param usage 用途, 1-注册 2-重置密码
/// @param block result callback
+ (void)getVerificationCodeRequestWithMailAddress:(NSString *)mailAddress usage:(TelinkUserUsage)usage didLoadData:(MyBlock)block;

/// 3.register 注册
/// @param mailAddress user mail
/// @param name user name
/// @param verificationCode verification Code
/// @param password password
/// @param block result callback
+ (void)registerRequestWithMailAddress:(NSString *)mailAddress name:(NSString *)name verificationCode:(NSString *)verificationCode password:(NSString *)password didLoadData:(MyBlock)block;

/// 4.login 登录
/// @param username user mail or user name
/// @param password password
/// @param block result callback
+ (void)loginRequestWithUsername:(NSString *)username password:(NSString *)password didLoadData:(MyBlock)block;

/// 5.logout 退出登录
/// @param block result callback
+ (void)logoutRequestWithDidLoadData:(MyBlock)block;

/// 6.resetPassword 通过验证码来重置密码
/// @param mailAddress user mail
/// @param verificationCode verification Code
/// @param newPassword user new password
/// @param block result callback
+ (void)resetPasswordRequestWithMailAddress:(NSString *)mailAddress verificationCode:(NSString *)verificationCode newPassword:(NSString *)newPassword didLoadData:(MyBlock)block;

/// 7.updateInfo 更新用户信息
/// @param nickname user nickname
/// @param sign sign
/// @param phone user phone
/// @param company user company
/// @param block result callback
+ (void)updateInfoRequestWithNickname:(NSString * _Nullable )nickname sign:(NSString * _Nullable )sign phone:(NSString * _Nullable )phone company:(NSString * _Nullable )company didLoadData:(MyBlock)block;

/// 8.updatePassword 通过旧密码来更新密码
/// @param oldPassword user old password
/// @param newPassword user new password
/// @param block result callback
+ (void)updatePasswordRequestWithOldPassword:(NSString *)oldPassword newPassword:(NSString *)newPassword didLoadData:(MyBlock)block;

#pragma mark - 网络管理 : Mesh Network Controller 相关

/// 1.applyProvisioner 申请provisioner
/// @param networkId 网络id
/// @param clientId 客户端设备ID
/// @param block result callback
+ (void)applyProvisionerRequestWithNetworkId:(NSInteger)networkId clientId:(NSString *)clientId didLoadData:(MyBlock)block;

/// 2.create 创建网络信息
/// @param name 网络名称
/// @param clientId 客户端设备ID
/// @param block result callback
+ (void)createMeshNetworkRequestWithName:(NSString *)name clientId:(NSString *)clientId didLoadData:(MyBlock)block;

/// 3.createShare 创建分享链接
/// @param networkId 网络id
/// @param validPeriod 有效时间-单位分钟
/// @param maxCount 最大次数
/// @param block result callback
+ (void)createShareLinkRequestWithNetworkId:(NSInteger)networkId validPeriod:(NSInteger)validPeriod maxCount:(NSInteger)maxCount didLoadData:(MyBlock)block;

/// 4.deleteNetwork 删除网络信息
/// @param networkId 网络id
/// @param block result callback
+ (void)deleteNetworkRequestWithNetworkId:(NSInteger)networkId didLoadData:(MyBlock)block;

/// 5.deleteAllNetwork 删除用户下所有网络信息
/// @param block result callback
+ (void)deleteAllNetworkRequestWithDidLoadData:(MyBlock)block;

/// 6.deleteNetworkByNetworkId 删除用户下指定ID的网络信息
/// @param networkId 网络id
/// @param block result callback
+ (void)deleteNetworkByNetworkIdRequestWithNetworkId:(NSInteger)networkId didLoadData:(MyBlock)block;

/// 7.deleteShareLink 删除分享链接
/// @param shareLinkId 分享链接id
/// @param block result callback
+ (void)deleteNetworkByNetworkIdRequestWithShareLinkId:(NSInteger)shareLinkId didLoadData:(MyBlock)block;

/// 8.getCreatedNetworkList 获取用户创建的网络列表
/// @param block result callback
+ (void)getCreatedNetworkListRequestWithDidLoadData:(MyBlock)block;

/// 9.getNetworkDetail 获取用户下指定ID的网络信息
/// @param networkId 网络id
/// @param clientId 客户端设备ID
/// @param type 类型(默认为0)： 0-创建的网络， 1-分享加入的网络
/// @param block result callback
+ (void)getNetworkDetailRequestWithNetworkId:(NSInteger)networkId clientId:(NSString *)clientId type:(NSInteger)type didLoadData:(MyBlock)block;

/// 10.getJoinedList 获取用户创建的分享状态信息， 即用户申请加入过哪些网络
/// @param state 分享状态： 申请中， 正常， 被禁用
/// @param block result callback
+ (void)getJoinedListRequestWithState:(ResponseShareState)state didLoadData:(MyBlock)block;

/// 11.getShareInfoList 获取该网络下的分享信息， 即分享给谁
/// @param networkId 网络id
/// @param state 分享状态： 申请中， 正常， 被禁用
/// @param block result callback
+ (void)getShareInfoListRequestWithNetworkId:(NSInteger)networkId state:(ResponseShareState)state didLoadData:(MyBlock)block;

/// 12.getShareLinkList 创建分享链接
/// @param networkId 网络id
/// @param expired 是否已过期
/// @param block result callback
+ (void)getShareLinkListRequestWithNetworkId:(NSInteger)networkId expired:(BOOL)expired didLoadData:(MyBlock)block;

/// 13.joinNetworkFromLink 通过share link加入网络， 传入shareLinkId
/// @param shareLinkId 网络分享链接ID
/// @param block result callback
+ (void)joinNetworkFromLinkRequestWithShareLinkId:(NSInteger)shareLinkId didLoadData:(MyBlock)block;

/// 14.leaveNetwork 用户主动退出之前加入的网络
/// @param shareInfoId 分享状态id
/// @param block result callback
+ (void)leaveNetworkRequestWithShareInfoId:(NSInteger)shareInfoId didLoadData:(MyBlock)block;

/// 15.updateIvIndex 更新网络的ivIndex信息
/// @param networkId 网络id
/// @param ivIndex 新的ivIndex
/// @param block result callback
+ (void)updateIvIndexRequestWithNetworkId:(NSInteger)networkId ivIndex:(NSInteger)ivIndex didLoadData:(MyBlock)block;

/// 16.updateNetworkName 更新网络名称
/// @param networkId 网络id
/// @param name 网络名称
/// @param block result callback
+ (void)updateNetworkNameRequestWithNetworkId:(NSInteger)networkId name:(NSString *)name didLoadData:(MyBlock)block;

/// 17.updateSequenceNumber 更新provisioner的sequence number信息
/// @param provisionerId provisioner ID
/// @param sequenceNumber 新的sequenceNumber
/// @param block result callback
+ (void)updateSequenceNumberRequestWithProvisionerId:(NSString *)provisionerId sequenceNumber:(NSInteger)sequenceNumber didLoadData:(MyBlock)block;

/// 18.updateShareInfoState 网络创建者更新网络分享状态, 允许网络分享(1)， 不允许(禁用)网络分享（2）, 修改权限等。。
/// @param shareInfoId 网络分享id
/// @param state 更新状态
/// @param block result callback
+ (void)updateShareInfoStateRequestWithShareInfoId:(NSInteger)shareInfoId state:(ResponseShareState)state didLoadData:(MyBlock)block;

#pragma mark - mesh节点管理 : Mesh Node Controller 相关

/// 1.addGroup 添加设备分组
/// @note 在添加分组后调用
/// @param nodeId node id
/// @param groupId group id
/// @param provisionerId provisioner ID
/// @param block result callback
+ (void)addGroupRequestWithNodeId:(NSInteger)nodeId groupId:(NSInteger)groupId provisionerId:(NSInteger)provisionerId didLoadData:(MyBlock)block;

/// 2.addNode 添加设备
/// @param tcVendorId 从蓝牙广播中获取的厂商自定义ID
/// @param pid 从蓝牙广播中获取的pid, 与cps data中一致
/// @param networkId 网络id 
/// @param deviceUUID deviceUUID 
/// @param name 设备名称 
/// @param address address 
/// @param provisionerId provisionerId 
/// @param deviceKey deviceKey 
/// @param block result callback
+ (void)addNodeRequestWithTcVendorId:(NSInteger)tcVendorId pid:(NSInteger)pid networkId:(NSInteger)networkId deviceUUID:(NSString *)deviceUUID name:(NSString *)name address:(NSInteger)address provisionerId:(NSInteger)provisionerId deviceKey:(NSString *)deviceKey didLoadData:(MyBlock)block;

/// 3.applyNodeAddress 申请节点地址
/// @note 在扫描到设备时， 传入设备ID， 从云端申请地址
/// @param tcVendorId 从蓝牙广播中获取的厂商自定义ID
/// @param pid 从蓝牙广播中获取的pid, 与cps data中一致
/// @param networkId 网络id 
/// @param provisionerId provisionerId 
/// @param elementCount 节点元素个数， 从capability中获取 
/// @param block result callback
+ (void)applyNodeAddressRequestWithTCVendorId:(NSInteger)tcVendorId pid:(NSInteger)pid networkId:(NSInteger)networkId provisionerId:(NSInteger)provisionerId elementCount:(NSInteger)elementCount didLoadData:(MyBlock)block;

/// 4.deleteNode 剔除节点
/// @note 在踢灯完成后调用
/// @param nodeId node id
/// @param block result callback
+ (void)deleteNodeRequestWithNodeId:(NSInteger)nodeId didLoadData:(MyBlock)block;

/// 5.deleteGroup 删除设备分组
/// @note 在删除分组后调用
/// @param nodeId node id
/// @param groupId group id
/// @param provisionerId provisioner ID
/// @param block result callback
+ (void)deleteNodeRequestWithNodeId:(NSInteger)nodeId groupId:(NSInteger)groupId provisionerId:(NSInteger)provisionerId didLoadData:(MyBlock)block;

/// 6.downloadBin 获取该设备下的所有分组信息
/// @param path path
/// @param block result callback
+ (void)downloadBinRequestWithPath:(NSString *)path didLoadData:(MyBlock)block;

/// 7.getGroups 获取该设备下的所有分组信息
/// @note 传入设备ID
/// @param nodeId node id
/// @param block result callback
+ (void)getGroupsOfNodeRequestWithNodeId:(NSInteger)nodeId didLoadData:(MyBlock)block;

/// 8.getLatestVersionInfo 获取最新的固件信息， app需要检查vid是否更大
/// @note 传入产品ID
/// @param productId 产品ID
/// @param block result callback
+ (void)getLatestVersionInfoRequestWithProductId:(NSInteger)productId didLoadData:(MyBlock)block;

/// 9.releaseNodeAddress 释放节点地址
/// @note 组网失败时调用， 传入设备ID， 从云端释放该地址
/// @param networkId 网络ID
/// @param provisionerId provisioner Id
/// @param elementCount elementCount
/// @param tcVendorId 从蓝牙广播中获取的厂商自定义ID
/// @param nodeAddress 已申请到的address
/// @param block result callback
+ (void)releaseNodeAddressRequestWithNetworkId:(NSInteger)networkId provisionerId:(NSInteger)provisionerId elementCount:(NSInteger)elementCount tcVendorId:(NSInteger)tcVendorId nodeAddress:(NSInteger)nodeAddress didLoadData:(MyBlock)block;

/// 10.updateNodeBindState 更新设备key bind状态
/// @note 在key bind成功后调用
/// @param nodeId node id
/// @param vid versionId 从composition data中获取的vid
/// @param bindState bind State
/// @param block result callback
+ (void)updateNodeBindStateRequestWithNodeId:(NSInteger)nodeId vid:(NSInteger)vid bindState:(NSInteger)bindState didLoadData:(MyBlock)block;

/// 11.updateNodeVersion 更新设备版本信息
/// @note 在OTA完成后，或者在获取到cps后发现不匹配时调用
/// @param nodeId node id
/// @param vid versionId 从composition data中获取的vid
/// @param block result callback
+ (void)updateNodeVersionRequestWithNodeId:(NSInteger)nodeId vid:(NSInteger)vid didLoadData:(MyBlock)block;

/// 12.uploadRecord 更新设备的操作日志
/// @note 在OTA完成后，或者在获取到cps后发现不匹配时调用
/// @param nodeId node id
/// @param status 状态
/// @param block result callback
+ (void)uploadRecordRequestWithNodeId:(NSInteger)nodeId status:(NSString *)status didLoadData:(MyBlock)block;

/// 13.getPublish 获取设备publish信息
/// @param nodeId 设备ID
/// @param block result callback
+ (void)getPublishRequestWithNodeId:(NSInteger)nodeId didLoadData:(MyBlock)block;

/// 14.setPublish 设置设备publish信息，目前仅能配置一个，如果当前已存在，则会替换
/// @param nodeId 设备ID
/// @param elementAddress 元素地址
/// @param publishAddress publish地址
/// @param appKeyIndex appKeyIndex
/// @param period period
/// @param modelId modelId
/// @param isSigModel 1 为 true, 0 为 false， 可选 默认为1
/// @param credentialFlag 默认为0x00
/// @param ttl 可选，默认为 0xFF
/// @param retransmitCount 可选 默认为 0x05
/// @param retransmitIntervalSteps 可选 默认为 0x00
/// @param block result callback
+ (void)setPublishRequestWithNodeId:(NSInteger)nodeId elementAddress:(NSInteger)elementAddress publishAddress:(NSInteger)publishAddress appKeyIndex:(NSInteger)appKeyIndex period:(NSInteger)period modelId:(NSInteger)modelId isSigModel:(NSInteger)isSigModel credentialFlag:(NSInteger)credentialFlag ttl:(NSInteger)ttl retransmitCount:(NSInteger)retransmitCount retransmitIntervalSteps:(NSInteger)retransmitIntervalSteps didLoadData:(MyBlock)block;

/// 15.updatePublish 更新设备publish信息，除publish ID外，其它均为可选，只需传入需要修改的参数。
/// @param publishId publish Id
/// @param elementAddress 元素地址
/// @param publishAddress publish地址
/// @param appKeyIndex appKeyIndex
/// @param period period
/// @param modelId modelId
/// @param isSigModel 1 为 true, 0 为 false， 可选 默认为1
/// @param credentialFlag 默认为0x00
/// @param ttl 可选，默认为 0xFF
/// @param retransmitCount 可选 默认为 0x05
/// @param retransmitIntervalSteps 可选 默认为 0x00
/// @param block result callback
+ (void)updatePublishRequestWithPublishId:(NSInteger)publishId elementAddress:(NSInteger)elementAddress publishAddress:(NSInteger)publishAddress appKeyIndex:(NSInteger)appKeyIndex period:(NSInteger)period modelId:(NSInteger)modelId isSigModel:(NSInteger)isSigModel credentialFlag:(NSInteger)credentialFlag ttl:(NSInteger)ttl retransmitCount:(NSInteger)retransmitCount retransmitIntervalSteps:(NSInteger)retransmitIntervalSteps didLoadData:(MyBlock)block;

/// 16.deletePublish 删除指定publish信息
/// @param publishId publish ID
/// @param block result callback
+ (void)deletePublishRequestWithPublishId:(NSInteger)publishId didLoadData:(MyBlock)block;

/// 17.getConfigs 返回所有的config消息，在修改设备配置前，需要先调用该接口以获取所有状态
/// @param nodeId 设备ID
/// @param block result callback
+ (void)getConfigsRequestWithNodeId:(NSInteger)nodeId didLoadData:(MyBlock)block;

/// 18.updateConfigs 更新所有config信息, 可用于设置多个config信息, 只需要上传需要修改的信息即可
/// @param nodeConfigs node configs
/// @param block result callback
+ (void)updateConfigsRequestWithNodeConfigs:(CloudNodeConfigsModel *)nodeConfigs didLoadData:(MyBlock)block;

#pragma mark - Mesh场景管理 : Mesh Scene Controller 相关

/// 1.addNodeToScene 将设备添加到场景
/// @note 在app发送scene store 成功后调用
/// @param nodeId node id
/// @param sceneId scene id
/// @param block result callback
+ (void)addNodeToSceneRequestWithNodeId:(NSInteger)nodeId sceneId:(NSInteger)sceneId didLoadData:(MyBlock)block;

/// 2.createScene 添加场景
/// @note 用户可以在app上添加场景
/// @param networkId 网络ID
/// @param name 场景名称
/// @param info 描述信息
/// @param block result callback
+ (void)createSceneRequestWithNetworkId:(NSInteger)networkId name:(NSString *)name info:(NSString * _Nullable)info didLoadData:(MyBlock)block;

/// 3.deleteScene 删除场景
/// @note 用户可以在app上删除场景
/// @param sceneId scene id
/// @param block result callback
+ (void)deleteSceneRequestWithSceneId:(NSInteger)sceneId didLoadData:(MyBlock)block;

/// 4.getInnerNodes 获取场景内的所有设备信息
/// @note 在app发送scene store成功后调用
/// @param sceneId scene id
/// @param block result callback
+ (void)getInnerNodesRequestWithSceneId:(NSInteger)sceneId didLoadData:(MyBlock)block;

/// 5.getSceneList 通过网络ID获取所有的场景信息
/// @note 进入scene 列表页面时调用
/// @param networkId 网络ID
/// @param block result callback
+ (void)getSceneListRequestWithNetworkId:(NSInteger)networkId didLoadData:(MyBlock)block;

/// 6.removeNodeFromScene 将设备移出场景
/// @note 在app发送scene delete 成功后调用
/// @param nodeId node id
/// @param sceneId scene id
/// @param block result callback
+ (void)removeNodeFromSceneRequestWithNodeId:(NSInteger)nodeId sceneId:(NSInteger)sceneId didLoadData:(MyBlock)block;

#pragma mark - Mesh闹钟管理 : Mesh Scheduler Controller 相关

/// 1.createScheduler 添加闹钟
/// @note 用户在app上添加闹钟， 一个节点最多只能16个（0-15）, index值由云端分配
/// @param nodeId 节点ID
/// @param name 闹钟名称
/// @param info 描述信息
/// @param block result callback
+ (void)createSchedulerRequestWithNodeId:(NSInteger)nodeId name:(NSString *)name info:(NSString * _Nullable)info didLoadData:(MyBlock)block;

/// 2.deleteScheduler 删除闹钟
/// @note 用户在app上删除闹钟
/// @param schedulerId 闹钟ID
/// @param block result callback
+ (void)deleteSchedulerRequestWithSchedulerId:(NSInteger)schedulerId didLoadData:(MyBlock)block;

/// 3.getSchedulerList 通过设备ID获取所有的闹钟信息
/// @note 在app发送scene store成功后调用
/// @param nodeId 节点ID
/// @param block result callback
+ (void)getSchedulerListRequestWithNodeId:(NSInteger)nodeId didLoadData:(MyBlock)block;

/// 4.saveScheduler 保存闹钟信息
/// @note 在配置闹钟完成后调用, 如果设置的action是scene, 则需要传入sceneId
/// @param schedulerId 闹钟ID
/// @param params scheduler参数信息(hex string)
/// @param meshSceneId 场景ID
/// @param block result callback
+ (void)saveSchedulerRequestWithSchedulerId:(NSInteger)schedulerId params:(NSString *)params meshSceneId:(NSInteger)meshSceneId didLoadData:(MyBlock)block;

/// 5.updateScheduler 更新闹钟的名称和描述信息
/// @note 在用户修改闹钟的名称和描述信息时调用, 参数可以为空， 但不能同时都为空
/// @param schedulerId 闹钟ID
/// @param name 闹钟名称
/// @param info 描述信息
/// @param block result callback
+ (void)updateSchedulerRequestWithSchedulerId:(NSInteger)schedulerId name:(NSString *)name info:(NSString *)info didLoadData:(MyBlock)block;

@end

NS_ASSUME_NONNULL_END
