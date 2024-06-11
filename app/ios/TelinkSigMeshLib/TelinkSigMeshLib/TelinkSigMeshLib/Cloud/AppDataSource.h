/********************************************************************************************************
 * @file     AppDataSource.h
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
#import "TelinkCloudModel.h"

NS_ASSUME_NONNULL_BEGIN

#define kUserName   @"kUserName"
#define kPassword   @"kPassword"
#define kUserInfo   @"kUserInfo"
#define kSelectNetworkUuid   @"kSelectNetworkUuid"

typedef void(^resultHandle)(NSError  * _Nullable error);
typedef void(^applyAddressResultHandle)(UInt16 address, NSError  * _Nullable error);

@interface AppDataSource : NSObject
@property (nonatomic, strong) UserInfoModel *userInfo;
@property (nonatomic, assign) NSInteger curNetworkId;//获取MeshNetworkDetail的唯一ID，0表示未登录未选中Mesh。
@property (nonatomic, strong) CloudNetworkDetailModel *curMeshNetworkDetailModel;
@property (nonatomic, strong) CloudProvisionerModel *curProvisionerModel;
/// 当前手机的设备ID，第一次打开APP的时候生成一次。 为SigDataSource.share.getCurrentProvisionerUUID
@property (nonatomic, strong) NSString *curClientId;
@property (nonatomic, strong) NSMutableArray<AppMeshNetworkModel *> *createdNetwordList;
@property (nonatomic, strong) NSMutableArray<AppJoinedNetworkModel *> *sharedNetwordList;

+ (AppDataSource *)share;

#pragma mark - 登入登出的处理

- (BOOL)isLoginSuccess;
- (BOOL)shouldAutoLogin;
- (void)loginWithUsername:(NSString *)username password:(NSString *)password resultBlock:(resultHandle)resultBlock;
- (void)autoLoginActionWithResultBlock:(resultHandle)resultBlock;
- (void)logoutSuccessAction;

#pragma mark - Mesh网络数据的HTTP处理

- (void)createMeshNetworkWithName:(NSString *)name resultBlock:(resultHandle)resultBlock;
- (void)getCreatedListAndJoinedListWithResultBlock:(resultHandle)resultBlock;
- (void)getCreatedNetworkListWithResultBlock:(resultHandle _Nullable)resultBlock;
- (void)getJoinedNetworkListWithResultBlock:(resultHandle _Nullable)resultBlock;
- (void)getDetailWithNetworkId:(NSInteger)networkId type:(NSInteger)type resultBlock:(resultHandle)resultBlock;
- (void)addNodeAfterProvisionSuccessWithNodeAddress:(UInt16)nodeAddress resultBlock:(resultHandle)resultBlock;
- (void)updateNodeBindStateWithBindState:(NSInteger)bindState address:(UInt16)address;
- (void)deleteNodeWithAddress:(UInt16)address resultBlock:(resultHandle)resultBlock;
- (void)addNodeToGroupWithNodeAddress:(UInt16)nodeAddress groupAddress:(UInt16)groupAddress resultBlock:(resultHandle)resultBlock;
- (void)deleteNodeFromGroupWithNodeAddress:(UInt16)nodeAddress groupAddress:(UInt16)groupAddress resultBlock:(resultHandle)resultBlock;
- (void)updateSequenceNumber:(UInt32)sequenceNumber resultBlock:(resultHandle)resultBlock;
- (void)updateIvIndex:(UInt32)ivIndex resultBlock:(resultHandle)resultBlock;
- (void)createSchedulerWithNodeId:(NSInteger)nodeId name:(NSString *)name info:(NSString * _Nullable)info resultBlock:(resultHandle)resultBlock;
- (void)getSchedulerListRequestWithNodeId:(NSInteger)nodeId resultBlock:(resultHandle)resultBlock;
- (void)deleteSchedulerWithSchedulerId:(NSInteger)schedulerId cloudNode:(CloudNodeModel *)cloudNode resultBlock:(resultHandle)resultBlock;
- (void)saveSchedulerWithCloudSchedulerModel:(CloudSchedulerModel *)cloudSchedulerModel params:(NSString *)params meshSceneId:(NSInteger)meshSceneId resultBlock:(resultHandle)resultBlock;
- (void)getSceneListWithNetworkId:(NSInteger)networkId resultBlock:(resultHandle)resultBlock;
- (void)createSceneWithNetworkId:(NSInteger)networkId name:(NSString *)name info:(NSString * _Nullable)info resultBlock:(resultHandle)resultBlock;
- (void)addNodeToSceneWithNodeId:(NSInteger)nodeId sceneId:(NSInteger)sceneId resultBlock:(resultHandle)resultBlock;
- (void)removeNodeFromSceneWithNodeId:(NSInteger)nodeId sceneId:(NSInteger)sceneId resultBlock:(resultHandle)resultBlock;
- (void)deleteSceneWithSceneId:(NSInteger)sceneId resultBlock:(resultHandle)resultBlock;

- (void)applyNodeAddressWithTCVendorID:(NSInteger)tcVendorID pid:(NSInteger)pid elementCount:(NSInteger)elementCount resultBlock:(applyAddressResultHandle)resultBlock;
- (void)releaseNodeAddressWithNodeAddress:(UInt16)nodeAddress elementCount:(NSInteger)elementCount tcVendorId:(NSInteger)tcVendorId resultBlock:(resultHandle)resultBlock;
- (void)setPublishWithNodeModel:(SigNodeModel *)nodeModel resultBlock:(resultHandle)resultBlock;
- (void)deletePublishWithNodeModel:(SigNodeModel *)nodeModel resultBlock:(resultHandle)resultBlock;
- (void)getNodeConfigsWithNodeModel:(SigNodeModel *)nodeModel resultBlock:(resultHandle)resultBlock;
- (void)updateNodeConfigsWithNodeConfigs:(CloudNodeConfigsModel *)nodeConfigs resultBlock:(resultHandle _Nullable)resultBlock;

#pragma mark - AppDataSource的数据转换到SigDataSource里面

- (CloudNodeModel *)getCloudNodeModelWithNodeAddress:(NSInteger)address;
- (CloudNodeModel *)getCloudNodeModelWithNodeId:(NSInteger)nodeId;
- (CloudGroupModel *)getCloudGroupModelWithGroupAddress:(NSInteger)address;
- (void)importCloudNetworkDetailModel:(CloudNetworkDetailModel *)model toSigDataSource:(SigDataSource *)dataSource;

- (void)saveToLocation;

- (AppMeshNetworkModel *)getAppMeshNetworkModelWithUuid:(NSString *)uuid;

@end

NS_ASSUME_NONNULL_END
