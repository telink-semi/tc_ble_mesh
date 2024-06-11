/********************************************************************************************************
 * @file     AppDataSource.m
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

#import "AppDataSource.h"
#import "TelinkHttpTool.h"
#import "SDKLibCommand+directForwarding.h"

@implementation AppDataSource

+ (AppDataSource *)share {
    static AppDataSource *shareDS = nil;
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        shareDS = [[AppDataSource alloc] init];
        shareDS.createdNetwordList = [NSMutableArray array];
        shareDS.sharedNetwordList = [NSMutableArray array];
    });
    return shareDS;
}

#pragma mark - 登入登出的处理

- (BOOL)isLoginSuccess {
    return _userInfo != nil;
}

- (BOOL)shouldAutoLogin {
    //云端APP新增判断，初步如此实现，后期讨论决定。
    NSString *password = [NSUserDefaults.standardUserDefaults valueForKey:kPassword];
    return password != nil;
}

- (void)loginWithUsername:(NSString *)username password:(NSString *)password resultBlock:(resultHandle)resultBlock {
    [TelinkHttpTool loginRequestWithUsername:username password:password didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"login error = %@", err);
            if (resultBlock) {
                resultBlock(err);
            }
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"login successful! dic=%@", dic);
                NSDictionary *dict = [NSDictionary dictionaryWithDictionary:dic[@"data"]];
                [AppDataSource.share loginSuccessActionWithUserInfoDictionary:dict];
                [NSUserDefaults.standardUserDefaults setValue:AppDataSource.share.userInfo.getDictionaryOfUserInfoModel forKey:kUserInfo];
                [NSUserDefaults.standardUserDefaults synchronize];
                if (resultBlock) {
                    resultBlock(nil);
                }
            } else {
                TelinkLogInfo(@"login result = %@", dic);
                if (resultBlock) {
                    resultBlock([NSError errorWithDomain:[NSString stringWithFormat:@"login errorCode = %d, message = %@", code, dic[@"message"]] code:-1 userInfo:nil]);
                }
            }
        }
    }];
}

- (void)autoLoginActionWithResultBlock:(resultHandle)resultBlock {
    NSString *username = [NSUserDefaults.standardUserDefaults valueForKey:kUserName];
    NSString *password = [NSUserDefaults.standardUserDefaults valueForKey:kPassword];
    if (username != nil && password != nil) {
        [self loginWithUsername:username password:password resultBlock:resultBlock];
    } else {
        if (resultBlock) {
            resultBlock([NSError errorWithDomain:@"The parameter of login is error!" code:-1 userInfo:nil]);
        }
    }
}

- (void)loginSuccessActionWithUserInfoDictionary:(NSDictionary *)dictionary {
    UserInfoModel *model = [[UserInfoModel alloc] init];
    [model setDictionaryToUserInfoModel:dictionary];
    _userInfo = model;
}

- (void)logoutSuccessAction {
    _userInfo = nil;
    _curNetworkId = 0;
    //清除缓存的password
    [[NSUserDefaults standardUserDefaults] removeObjectForKey:kPassword];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

#pragma mark - Mesh网络数据的HTTP处理

- (void)createMeshNetworkWithName:(NSString *)name resultBlock:(resultHandle)resultBlock {
    [TelinkHttpTool createMeshNetworkRequestWithName:name clientId:self.curClientId didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"createMeshNetwork error = %@", err);
            if (resultBlock) {
                resultBlock(err);
            }
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"createMeshNetwork successful! dic=%@", dic);
                NSDictionary *dictionary = dic[@"data"];
                AppMeshNetworkModel *model = [[AppMeshNetworkModel alloc] init];
                [model setDictionaryToAppMeshNetworkModel:dictionary];
                [AppDataSource.share.createdNetwordList addObject:model];
                if (resultBlock) {
                    resultBlock(nil);
                }
                
            } else {
                TelinkLogInfo(@"createMeshNetwork result = %@", dic);
                if (resultBlock) {
                    resultBlock([NSError errorWithDomain:[NSString stringWithFormat:@"createMeshNetwork errorCode = %d, message = %@", code, dic[@"message"]] code:code userInfo:nil]);
                }
            }
        }
    }];
}

- (void)getCreatedListAndJoinedListWithResultBlock:(resultHandle)resultBlock {
    __weak typeof(self) weakSelf = self;
    [self getCreatedNetworkListWithResultBlock:^(NSError * _Nullable error) {
        TelinkLogInfo(@"error=%@", error);
        if (error) {
            if (resultBlock) {
                resultBlock(nil);
            }
        } else {
            //获取分享的Mesh，获取完成则登录完成
            [weakSelf getJoinedNetworkListWithResultBlock:resultBlock];
        }
    }];
}

- (void)getCreatedNetworkListWithResultBlock:(resultHandle _Nullable)resultBlock {
    [TelinkHttpTool getCreatedNetworkListRequestWithDidLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"getCreatedNetworkList error = %@", err);
            if (resultBlock) {
                resultBlock(err);
            }
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"getCreatedNetworkList successful! dic=%@", dic);
                NSArray *array = dic[@"data"];
                [AppDataSource.share.createdNetwordList removeAllObjects];
                if (array.count > 0) {
                    //有已经创建的Mesh，解析并存储本地
                    NSArray *arr = [NSArray arrayWithArray:array];
                    for (NSDictionary *dict in arr) {
                        AppMeshNetworkModel *model = [[AppMeshNetworkModel alloc] init];
                        [model setDictionaryToAppMeshNetworkModel:dict];
                        [AppDataSource.share.createdNetwordList addObject:model];
                    }
                }
                if (resultBlock) {
                    resultBlock(nil);
                }
            } else {
                TelinkLogInfo(@"getCreatedNetworkList result = %@", dic);
                if (resultBlock) {
                    resultBlock([NSError errorWithDomain:[NSString stringWithFormat:@"getCreatedNetworkList errorCode = %d, message = %@", code, dic[@"message"]] code:-1 userInfo:nil]);
                }
            }
        }
    }];
}

- (void)getJoinedNetworkListWithResultBlock:(resultHandle _Nullable)resultBlock {
    [TelinkHttpTool getJoinedListRequestWithState:ResponseShareStateAll didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"getSharedNetworkList error = %@", err);
            if (resultBlock) {
                resultBlock(err);
            }
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"getSharedNetworkList successful! dic=%@", dic);
                NSArray *array = dic[@"data"];
                [AppDataSource.share.sharedNetwordList removeAllObjects];
                if (array.count > 0) {
                    //有分享的Mesh，解析并存储本地
                    NSArray *arr = [NSArray arrayWithArray:array];
                    for (NSDictionary *dict in arr) {
                        AppJoinedNetworkModel *model = [[AppJoinedNetworkModel alloc] init];
                        [model setDictionaryToAppJoinedNetworkModel:dict];
                        [AppDataSource.share.sharedNetwordList addObject:model];
                    }
                }
                if (resultBlock) {
                    resultBlock(nil);
                }
            } else {
                TelinkLogInfo(@"getSharedNetworkList result = %@", dic);
                if (resultBlock) {
                    resultBlock([NSError errorWithDomain:[NSString stringWithFormat:@"getSharedNetworkList errorCode = %d, message = %@", code, dic[@"message"]] code:-1 userInfo:nil]);
                }
            }
        }
    }];
}

- (void)getDetailWithNetworkId:(NSInteger)networkId type:(NSInteger)type resultBlock:(resultHandle)resultBlock {
    [TelinkHttpTool getNetworkDetailRequestWithNetworkId:networkId clientId:self.curClientId type:type didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"getNetworkDetail error = %@", err);
            if (resultBlock) {
                resultBlock(err);
            }
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"getNetworkDetail successful! dic=%@", dic);
                NSDictionary *dictionary = dic[@"data"];
                CloudNetworkDetailModel *meshData = [[CloudNetworkDetailModel alloc] init];
                [meshData setDictionaryToCloudNetworkDetailModel:dictionary];
                AppDataSource.share.curMeshNetworkDetailModel = meshData;
                [AppDataSource.share importCloudNetworkDetailModel:meshData toSigDataSource:SigDataSource.share];
                if (resultBlock) {
                    resultBlock(nil);
                }
            } else {
                TelinkLogInfo(@"getNetworkDetail result = %@", dic);
                if (resultBlock) {
                    resultBlock([NSError errorWithDomain:[NSString stringWithFormat:@"getNetworkDetail errorCode = %d, message = %@", code, dic[@"message"]] code:code userInfo:nil]);
                }
            }
        }
    }];
}

- (void)applyNodeAddressWithTCVendorID:(NSInteger)tcVendorID pid:(NSInteger)pid elementCount:(NSInteger)elementCount resultBlock:(applyAddressResultHandle)resultBlock {
    [TelinkHttpTool applyNodeAddressRequestWithTCVendorId:tcVendorID pid:pid networkId:AppDataSource.share.curNetworkId provisionerId:AppDataSource.share.curMeshNetworkDetailModel.provisioner.provisionerId elementCount:elementCount didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"applyNodeAddress error = %@", err);
            if (resultBlock) {
                resultBlock(0, err);
            }
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"applyNodeAddress successful! dic=%@", dic);
                UInt16 address = [dic[@"data"] intValue];
                if (resultBlock) {
                    resultBlock(address, err);
                }
            } else {
                TelinkLogInfo(@"applyNodeAddress result = %@", dic);
                if (resultBlock) {
                    resultBlock(0, [NSError errorWithDomain:[NSString stringWithFormat:@"applyNodeAddress errorCode = %d, message = %@", code, dic[@"message"]] code:code userInfo:nil]);
                }
            }
        }
    }];
}

- (void)releaseNodeAddressWithNodeAddress:(UInt16)nodeAddress elementCount:(NSInteger)elementCount tcVendorId:(NSInteger)tcVendorId resultBlock:(resultHandle)resultBlock {
    [TelinkHttpTool releaseNodeAddressRequestWithNetworkId:AppDataSource.share.curNetworkId provisionerId:AppDataSource.share.curMeshNetworkDetailModel.provisioner.provisionerId elementCount:elementCount tcVendorId:tcVendorId nodeAddress:nodeAddress didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"releaseNode error = %@", err);
            if (resultBlock) {
                resultBlock(err);
            }
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"releaseNode successful! dic=%@", dic);
                if (resultBlock) {
                    resultBlock(nil);
                }
            } else {
                TelinkLogInfo(@"releaseNode result = %@", dic);
                if (resultBlock) {
                    resultBlock([NSError errorWithDomain:[NSString stringWithFormat:@"releaseNode errorCode = %d, message = %@", code, dic[@"message"]] code:code userInfo:nil]);
                }
            }
        }
    }];
}

- (void)addNodeAfterProvisionSuccessWithNodeAddress:(UInt16)nodeAddress resultBlock:(resultHandle)resultBlock {
    SigScanRspModel *scanRspModel = [SigDataSource.share getScanRspModelWithUnicastAddress:nodeAddress];
    SigNodeModel *nodeModel = [SigDataSource.share getNodeWithAddress:nodeAddress];
    SigPlatformTelinkDeviceUuidModel *uuid = scanRspModel.getPlatformTelinkDeviceUuid;
    [TelinkHttpTool addNodeRequestWithTcVendorId:uuid.tcVendorID pid:uuid.pid networkId:AppDataSource.share.curNetworkId deviceUUID:scanRspModel.advUuid name:[NSString stringWithFormat:@"Name:0x%X", nodeAddress] address:nodeAddress provisionerId:AppDataSource.share.curMeshNetworkDetailModel.provisioner.provisionerId deviceKey:nodeModel.deviceKey didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"addNode error = %@", err);
            if (resultBlock) {
                resultBlock(err);
            }
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"addNode successful! dic=%@", dic);
                NSDictionary *dictionary = dic[@"data"];
                CloudNodeModel *nodeModel = [[CloudNodeModel alloc] init];
                [nodeModel setDictionaryToCloudNodeModel:dictionary];
                [AppDataSource.share.curMeshNetworkDetailModel.nodeList addObject:nodeModel];
                if (resultBlock) {
                    resultBlock(nil);
                }
            } else {
                TelinkLogInfo(@"addNode result = %@", dic);
                if (resultBlock) {
                    resultBlock([NSError errorWithDomain:[NSString stringWithFormat:@"addNode errorCode = %d, message = %@", code, dic[@"message"]] code:code userInfo:nil]);
                }
            }
        }
    }];
}

- (void)updateNodeBindStateWithBindState:(NSInteger)bindState address:(UInt16)address {
    CloudNodeModel *node = [self getCloudNodeModelWithNodeAddress:address];
    SigNodeModel *nodeModel = [SigMeshLib.share.dataSource getNodeWithAddress:address];
    [TelinkHttpTool updateNodeBindStateRequestWithNodeId:node.nodeId vid:[LibTools uint16FromBytes:[LibTools nsstringToHex:nodeModel.vid]] bindState:bindState didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"updateNodeBindState error = %@", err);
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"updateNodeBindState successful! dic=%@", dic);
                NSDictionary *nodeDict = (NSDictionary *)dic[@"data"];
                CloudNodeModel *cloudNodeModel = [[CloudNodeModel alloc] init];
                [cloudNodeModel setDictionaryToCloudNodeModel:nodeDict];
                UInt8 page0 = 0;
                NSMutableData *mData = [NSMutableData dataWithBytes:&page0 length:1];
                [mData appendData:[LibTools nsstringToHex:cloudNodeModel.versionInfo.compositionData]];
                SigPage0 *page = [[SigPage0 alloc] initWithParameters:mData];
                [nodeModel setCompositionData:page];
            } else {
                TelinkLogInfo(@"updateNodeBindState result = %@", dic);
            }
        }
    }];
}

- (void)deleteNodeWithAddress:(UInt16)address resultBlock:(resultHandle)resultBlock {
    CloudNodeModel *node = [self getCloudNodeModelWithNodeAddress:address];
    if (node) {
        [TelinkHttpTool deleteNodeRequestWithNodeId:node.nodeId didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
            if (err) {
                TelinkLogInfo(@"deleteNode error = %@", err);
                if (resultBlock) {
                    resultBlock(err);
                }
            } else {
                NSDictionary *dic = (NSDictionary *)result;
                int code = [dic[@"code"] intValue];
                if (code == 200) {
                    TelinkLogInfo(@"deleteNode successful! dic=%@", dic);
                    if (resultBlock) {
                        resultBlock(nil);
                    }
                } else {
                    TelinkLogInfo(@"deleteNode result = %@", dic);
                    if (resultBlock) {
                        resultBlock([NSError errorWithDomain:[NSString stringWithFormat:@"deleteNode errorCode = %d, message = %@", code, dic[@"message"]] code:code userInfo:nil]);
                    }
                }
            }
        }];
    }
}

- (void)addNodeToGroupWithNodeAddress:(UInt16)nodeAddress groupAddress:(UInt16)groupAddress resultBlock:(resultHandle)resultBlock {
    CloudNodeModel *node = [self getCloudNodeModelWithNodeAddress:nodeAddress];
    CloudGroupModel *group = [self getCloudGroupModelWithGroupAddress:groupAddress];
    [TelinkHttpTool addGroupRequestWithNodeId:node.nodeId groupId:group.groupId provisionerId:self.curProvisionerModel.provisionerId didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"addGroup error = %@", err);
            if (resultBlock) {
                resultBlock(err);
            }
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"addGroup successful! dic=%@", dic);
                if (resultBlock) {
                    resultBlock(nil);
                }
            } else {
                TelinkLogInfo(@"addGroup result = %@", dic);
                if (resultBlock) {
                    resultBlock([NSError errorWithDomain:[NSString stringWithFormat:@"addGroup errorCode = %d, message = %@", code, dic[@"message"]] code:code userInfo:nil]);
                }
            }
        }
    }];
}

- (void)deleteNodeFromGroupWithNodeAddress:(UInt16)nodeAddress groupAddress:(UInt16)groupAddress resultBlock:(resultHandle)resultBlock {
    CloudNodeModel *node = [self getCloudNodeModelWithNodeAddress:nodeAddress];
    CloudGroupModel *group = [self getCloudGroupModelWithGroupAddress:groupAddress];
    [TelinkHttpTool deleteNodeRequestWithNodeId:node.nodeId groupId:group.groupId provisionerId:self.curProvisionerModel.provisionerId didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"deleteNode error = %@", err);
            if (resultBlock) {
                resultBlock(err);
            }
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"deleteNode successful! dic=%@", dic);
                if (resultBlock) {
                    resultBlock(nil);
                }
            } else {
                TelinkLogInfo(@"deleteNode result = %@", dic);
                if (resultBlock) {
                    resultBlock([NSError errorWithDomain:[NSString stringWithFormat:@"deleteNode errorCode = %d, message = %@", code, dic[@"message"]] code:code userInfo:nil]);
                }
            }
        }
    }];
}

- (void)updateSequenceNumber:(UInt32)sequenceNumber resultBlock:(resultHandle)resultBlock {
    [TelinkHttpTool updateSequenceNumberRequestWithProvisionerId:[NSString stringWithFormat:@"%ld", (long)self.curProvisionerModel.provisionerId] sequenceNumber:sequenceNumber didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"updateSequenceNumber error = %@", err);
            if (resultBlock) {
                resultBlock(err);
            }
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"updateSequenceNumber successful! dic=%@", dic);
                if (resultBlock) {
                    resultBlock(nil);
                }
            } else {
                TelinkLogInfo(@"updateSequenceNumber result = %@", dic);
                if (resultBlock) {
                    resultBlock([NSError errorWithDomain:[NSString stringWithFormat:@"updateSequenceNumber errorCode = %d, message = %@", code, dic[@"message"]] code:code userInfo:nil]);
                }
            }
        }
    }];
}

- (void)updateIvIndex:(UInt32)ivIndex resultBlock:(resultHandle)resultBlock {
    [TelinkHttpTool updateIvIndexRequestWithNetworkId:self.curNetworkId ivIndex:ivIndex didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"updateIvIndex error = %@", err);
            if (resultBlock) {
                resultBlock(err);
            }
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"updateIvIndex successful! dic=%@", dic);
                if (resultBlock) {
                    resultBlock(nil);
                }
            } else {
                TelinkLogInfo(@"updateIvIndex result = %@", dic);
                if (resultBlock) {
                    resultBlock([NSError errorWithDomain:[NSString stringWithFormat:@"updateIvIndex errorCode = %d, message = %@", code, dic[@"message"]] code:code userInfo:nil]);
                }
            }
        }
    }];
}

- (void)createSchedulerWithNodeId:(NSInteger)nodeId name:(NSString *)name info:(NSString * _Nullable)info resultBlock:(resultHandle)resultBlock {
    [TelinkHttpTool createSchedulerRequestWithNodeId:nodeId name:name info:nil didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"createScheduler error = %@", err);
            if (resultBlock) {
                resultBlock(err);
            }
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"%@", dic[@"message"]);
                NSDictionary *dict = dic[@"data"];
                CloudSchedulerModel *model = [[CloudSchedulerModel alloc] init];
                [model setDictionaryToCloudSchedulerModel:dict];
                CloudNodeModel *node = [self getCloudNodeModelWithNodeId:nodeId];
                [node.schedulerList addObject:model];
                if (resultBlock) {
                    resultBlock(nil);
                }
            } else {
                TelinkLogInfo(@"createScheduler result = %@", dic);
                if (resultBlock) {
                    resultBlock([NSError errorWithDomain:[NSString stringWithFormat:@"createScheduler errorCode = %d, message = %@", code, dic[@"message"]] code:code userInfo:nil]);
                }
            }
        }
    }];

}

- (void)getSchedulerListRequestWithNodeId:(NSInteger)nodeId resultBlock:(resultHandle)resultBlock {
    [TelinkHttpTool getSchedulerListRequestWithNodeId:nodeId didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"getSchedulerList error = %@", err);
            if (resultBlock) {
                resultBlock(err);
            }
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"getSchedulerList successful! dic=%@", dic);
                NSArray *array = dic[@"data"];
                NSMutableArray *mArray = [NSMutableArray array];
                if (array.count > 0) {
                    for (NSDictionary *dict in array) {
                        CloudSchedulerModel *model = [[CloudSchedulerModel alloc] init];
                        [model setDictionaryToCloudSchedulerModel:dict];
                        [mArray addObject:model];
                    }
                }
                CloudNodeModel *node = [self getCloudNodeModelWithNodeId:nodeId];
                node.schedulerList = mArray;
                if (resultBlock) {
                    resultBlock(nil);
                }
            } else {
                TelinkLogInfo(@"getSchedulerList result = %@", dic);
                if (resultBlock) {
                    resultBlock([NSError errorWithDomain:[NSString stringWithFormat:@"getSchedulerList errorCode = %d, message = %@", code, dic[@"message"]] code:code userInfo:nil]);
                }
            }
        }
    }];
}

- (void)deleteSchedulerWithSchedulerId:(NSInteger)schedulerId cloudNode:(CloudNodeModel *)cloudNode resultBlock:(resultHandle)resultBlock {
    [TelinkHttpTool deleteSchedulerRequestWithSchedulerId:schedulerId didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"deleteScheduler error = %@", err);
            if (resultBlock) {
                resultBlock(err);
            }
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"deleteScheduler successful! dic=%@", dic);
                [cloudNode deleteCloudSchedulerModelWithSchedulerId:schedulerId];
                if (resultBlock) {
                    resultBlock(nil);
                }
            } else {
                TelinkLogInfo(@"deleteScheduler result = %@", dic);
                if (resultBlock) {
                    resultBlock([NSError errorWithDomain:[NSString stringWithFormat:@"deleteScheduler errorCode = %d, message = %@", code, dic[@"message"]] code:code userInfo:nil]);
                }
            }
        }
    }];
}

- (void)saveSchedulerWithCloudSchedulerModel:(CloudSchedulerModel *)cloudSchedulerModel params:(NSString *)params meshSceneId:(NSInteger)meshSceneId resultBlock:(resultHandle)resultBlock {
    [TelinkHttpTool saveSchedulerRequestWithSchedulerId:cloudSchedulerModel.schedulerId params:params meshSceneId:meshSceneId didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"saveScheduler error = %@", err);
            if (resultBlock) {
                resultBlock(err);
            }
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"saveScheduler successful! dic=%@", dic);
                cloudSchedulerModel.params = params;
                cloudSchedulerModel.meshSceneId = meshSceneId;
                if (resultBlock) {
                    resultBlock(nil);
                }
            } else {
                TelinkLogInfo(@"saveScheduler result = %@", dic);
                if (resultBlock) {
                    resultBlock([NSError errorWithDomain:[NSString stringWithFormat:@"saveScheduler errorCode = %d, message = %@", code, dic[@"message"]] code:code userInfo:nil]);
                }
            }
        }
    }];
}

- (void)getSceneListWithNetworkId:(NSInteger)networkId resultBlock:(resultHandle)resultBlock {
    __weak typeof(self) weakSelf = self;
    [TelinkHttpTool getSceneListRequestWithNetworkId:networkId didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"getSceneList error = %@", err);
            if (resultBlock) {
                resultBlock(err);
            }
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"getSceneList successful! dic=%@", dic);
                NSArray *array = dic[@"data"];
                NSMutableArray *mArray = [NSMutableArray array];
                if (array.count > 0) {
                    for (NSDictionary *dict in array) {
                        CloudSceneModel *model = [[CloudSceneModel alloc] init];
                        [model setDictionaryToCloudSceneModel:dict];
                        [mArray addObject:model];
                    }
                }
                weakSelf.curMeshNetworkDetailModel.sceneList = mArray;
                if (resultBlock) {
                    resultBlock(nil);
                }
            } else {
                TelinkLogInfo(@"getSceneList result = %@", dic);
                if (resultBlock) {
                    resultBlock([NSError errorWithDomain:[NSString stringWithFormat:@"getSceneList errorCode = %d, message = %@", code, dic[@"message"]] code:code userInfo:nil]);
                }
            }
        }
    }];
}

- (void)createSceneWithNetworkId:(NSInteger)networkId name:(NSString *)name info:(NSString * _Nullable)info resultBlock:(resultHandle)resultBlock {
    [TelinkHttpTool createSceneRequestWithNetworkId:networkId name:name info:info didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"createScene error = %@", err);
            if (resultBlock) {
                resultBlock(err);
            }
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"createScene successful! dic=%@", dic);
                if (resultBlock) {
                    resultBlock(nil);
                }
            } else {
                TelinkLogInfo(@"createScene result = %@", dic);
                if (resultBlock) {
                    resultBlock([NSError errorWithDomain:[NSString stringWithFormat:@"createScene errorCode = %d, message = %@", code, dic[@"message"]] code:code userInfo:nil]);
                }
            }
        }
    }];
}

- (void)addNodeToSceneWithNodeId:(NSInteger)nodeId sceneId:(NSInteger)sceneId resultBlock:(resultHandle)resultBlock {
    [TelinkHttpTool addNodeToSceneRequestWithNodeId:nodeId sceneId:sceneId didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"addNodeToScene error = %@", err);
            if (resultBlock) {
                resultBlock(err);
            }
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"addNodeToScene successful! dic=%@", dic);
                if (resultBlock) {
                    resultBlock(nil);
                }
            } else {
                TelinkLogInfo(@"addNodeToScene result = %@", dic);
                if (resultBlock) {
                    resultBlock([NSError errorWithDomain:[NSString stringWithFormat:@"addNodeToScene errorCode = %d, message = %@", code, dic[@"message"]] code:code userInfo:nil]);
                }
            }
        }
    }];
}

- (void)removeNodeFromSceneWithNodeId:(NSInteger)nodeId sceneId:(NSInteger)sceneId resultBlock:(resultHandle)resultBlock {
    [TelinkHttpTool removeNodeFromSceneRequestWithNodeId:nodeId sceneId:sceneId didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"removeNodeFromScene error = %@", err);
            if (resultBlock) {
                resultBlock(err);
            }
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"removeNodeFromScene successful! dic=%@", dic);
                if (resultBlock) {
                    resultBlock(nil);
                }
            } else {
                TelinkLogInfo(@"removeNodeFromScene result = %@", dic);
                if (resultBlock) {
                    resultBlock([NSError errorWithDomain:[NSString stringWithFormat:@"removeNodeFromScene errorCode = %d, message = %@", code, dic[@"message"]] code:code userInfo:nil]);
                }
            }
        }
    }];
}

- (void)deleteSceneWithSceneId:(NSInteger)sceneId resultBlock:(resultHandle)resultBlock {
    [TelinkHttpTool deleteSceneRequestWithSceneId:sceneId didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"deleteScene error = %@", err);
            if (resultBlock) {
                resultBlock(err);
            }
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"deleteScene successful! dic=%@", dic);
                if (resultBlock) {
                    resultBlock(nil);
                }
            } else {
                TelinkLogInfo(@"deleteScene result = %@", dic);
                if (resultBlock) {
                    resultBlock([NSError errorWithDomain:[NSString stringWithFormat:@"deleteScene errorCode = %d, message = %@", code, dic[@"message"]] code:code userInfo:nil]);
                }
            }
        }
    }];
}

- (void)setPublishWithNodeModel:(SigNodeModel *)nodeModel resultBlock:(resultHandle)resultBlock {
    CloudNodeModel *node = [self getCloudNodeModelWithNodeAddress:nodeModel.address];
    UInt16 elementAddress = [nodeModel.publishAddress.firstObject intValue];
    UInt16 modelId = nodeModel.publishModelID;
    [TelinkHttpTool setPublishRequestWithNodeId:node.nodeId elementAddress:elementAddress publishAddress:0xFFFF appKeyIndex:SigDataSource.share.curAppkeyModel.index period:[SigHelper.share getMillisecondsOfTransitionIntervalWithPeriodSteps:SigDataSource.share.defaultPublishPeriodModel.numberOfSteps periodResolution:[LibTools getSigStepResolutionWithSigPeriodModel:SigDataSource.share.defaultPublishPeriodModel]] modelId:modelId isSigModel:YES credentialFlag:0 ttl:0xFF retransmitCount:5 retransmitIntervalSteps:0 didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"setPublish error = %@", err);
            if (resultBlock) {
                resultBlock(err);
            }
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"setPublish successful! dic=%@", dic);
                CloudPublishInfoModel *publish = [[CloudPublishInfoModel alloc] init];
                [publish setDictionaryToCloudPublishInfoModel:dic[@"data"]];
                node.publishInfo = publish;
                if (resultBlock) {
                    resultBlock(nil);
                }
            } else {
                TelinkLogInfo(@"setPublish result = %@", dic);
                if (resultBlock) {
                    resultBlock([NSError errorWithDomain:[NSString stringWithFormat:@"setPublish errorCode = %d, message = %@", code, dic[@"message"]] code:code userInfo:nil]);
                }
            }
        }
    }];
}

- (void)deletePublishWithNodeModel:(SigNodeModel *)nodeModel resultBlock:(resultHandle)resultBlock {
    CloudNodeModel *node = [self getCloudNodeModelWithNodeAddress:nodeModel.address];
    [TelinkHttpTool deletePublishRequestWithPublishId:node.publishInfo.publishId didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"deletePublish error = %@", err);
            if (resultBlock) {
                resultBlock(err);
            }
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"deletePublish successful! dic=%@", dic);
                node.publishInfo = nil;
                if (resultBlock) {
                    resultBlock(nil);
                }
            } else {
                TelinkLogInfo(@"deletePublish result = %@", dic);
                if (resultBlock) {
                    resultBlock([NSError errorWithDomain:[NSString stringWithFormat:@"deletePublish errorCode = %d, message = %@", code, dic[@"message"]] code:code userInfo:nil]);
                }
            }
        }
    }];
}

- (void)getNodeConfigsWithNodeModel:(SigNodeModel *)nodeModel resultBlock:(resultHandle)resultBlock {
    CloudNodeModel *node = [self getCloudNodeModelWithNodeAddress:nodeModel.address];
    __weak typeof(self) weakSelf = self;
    [TelinkHttpTool getConfigsRequestWithNodeId:node.nodeId didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"getConfigs error = %@", err);
            if (resultBlock) {
                resultBlock(err);
            }
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"getConfigs successful! dic=%@", dic);
                CloudNodeConfigsModel *nodeConfigs = [[CloudNodeConfigsModel alloc] init];
                [nodeConfigs setDictionaryToCloudNodeConfigsModel:dic[@"data"]];
                node.configsInfo = nodeConfigs;
                [weakSelf importCloudNodeConfigsModel:nodeConfigs toSigNodeModel:nodeModel];
                if (resultBlock) {
                    resultBlock(nil);
                }
            } else {
                TelinkLogInfo(@"getConfigs result = %@", dic);
                if (resultBlock) {
                    resultBlock([NSError errorWithDomain:[NSString stringWithFormat:@"getConfigs errorCode = %d, message = %@", code, dic[@"message"]] code:code userInfo:nil]);
                }
            }
        }
    }];
}

- (void)updateNodeConfigsWithNodeConfigs:(CloudNodeConfigsModel *)nodeConfigs resultBlock:(resultHandle _Nullable)resultBlock {
    [TelinkHttpTool updateConfigsRequestWithNodeConfigs:nodeConfigs didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        if (err) {
            TelinkLogInfo(@"updateConfigs error = %@", err);
            if (resultBlock) {
                resultBlock(err);
            }
        } else {
            NSDictionary *dic = (NSDictionary *)result;
            int code = [dic[@"code"] intValue];
            if (code == 200) {
                TelinkLogInfo(@"updateConfigs successful! dic=%@", dic);
                if (resultBlock) {
                    resultBlock(nil);
                }
            } else {
                TelinkLogInfo(@"getConfigs result = %@", dic);
                if (resultBlock) {
                    resultBlock([NSError errorWithDomain:[NSString stringWithFormat:@"updateConfigs errorCode = %d, message = %@", code, dic[@"message"]] code:code userInfo:nil]);
                }
            }
        }
    }];
}

#pragma mark - AppDataSource的数据转换到SigDataSource里面

- (CloudNodeModel *)getCloudNodeModelWithNodeAddress:(NSInteger)address {
    CloudNodeModel *model = nil;
    NSArray *nodes = [NSArray arrayWithArray:self.curMeshNetworkDetailModel.nodeList];
    for (CloudNodeModel *tem in nodes) {
        if (tem.address == address) {
            model = tem;
            break;
        }
    }
    return model;
}

- (CloudNodeModel *)getCloudNodeModelWithNodeId:(NSInteger)nodeId {
    CloudNodeModel *model = nil;
    NSArray *nodes = [NSArray arrayWithArray:self.curMeshNetworkDetailModel.nodeList];
    for (CloudNodeModel *tem in nodes) {
        if (tem.nodeId == nodeId) {
            model = tem;
            break;
        }
    }
    return model;
}

- (CloudGroupModel *)getCloudGroupModelWithGroupAddress:(NSInteger)address {
    CloudGroupModel *model = nil;
    NSArray *groups = [NSArray arrayWithArray:self.curMeshNetworkDetailModel.groupList];
    for (CloudGroupModel *tem in groups) {
        if (tem.address == address) {
            model = tem;
            break;
        }
    }
    return model;
}

- (CloudNetworkKeyModel *)getCloudNetworkKeyModelWithNetworkKeyId:(NSInteger)networkKeyId fromCloudNetworkKeyModelList:(NSArray<CloudNetworkKeyModel *> *)list {
    for (CloudNetworkKeyModel *model in list) {
        if (model.networkKeyId == networkKeyId) {
            return model;
        }
    }
    return nil;
}

- (void)importCloudNetworkDetailModel:(CloudNetworkDetailModel *)model toSigDataSource:(SigDataSource *)dataSource {
    dataSource.meshName = model.name;
    dataSource.meshUUID = [NSString stringWithFormat:@"%ld", (long)model.networkId];
    //获取到iv和seq后，使用- (void)saveCurrentIvIndex:(UInt32)ivIndex sequenceNumber:(UInt32)sequenceNumber;来缓存本地，
    dataSource.ivIndexUInt32 = (UInt32)model.ivIndex;
    [dataSource setSequenceNumberUInt32:(UInt32)model.provisioner.sequenceNumber + dataSource.defaultSequenceNumberIncrement];//导入时，需要添加步长
    dataSource.curNodes = nil;
    dataSource.curNetkeyModel = nil;
    dataSource.curAppkeyModel = nil;

    NSMutableArray *netkeys = [NSMutableArray array];
    for (CloudNetworkKeyModel *net in model.networkKeyList) {
        SigNetkeyModel *netkey = [[SigNetkeyModel alloc] init];
        netkey.name = net.name;
        netkey.key = net.key;
        netkey.oldKey = net.oldKey;
        netkey.index = net.index;
        netkey.phase = net.phase;
        netkey.timestamp = net.updateTime;
        [netkeys addObject:netkey];
    }
    dataSource.netKeys = netkeys;

    NSMutableArray *appkeys = [NSMutableArray array];
    for (CloudApplicationKeyModel *app in model.applicationKeyList) {
        SigAppkeyModel *appkey = [[SigAppkeyModel alloc] init];
        appkey.name = app.name;
        appkey.key = app.key;
        appkey.oldKey = app.oldKey;
        appkey.index = app.index;
        appkey.boundNetKey = [self getCloudNetworkKeyModelWithNetworkKeyId:app.networkKeyId fromCloudNetworkKeyModelList:model.networkKeyList].index;
        [appkeys addObject:appkey];
    }
    dataSource.appKeys = appkeys;

    NSMutableArray *groups = [NSMutableArray array];
    for (CloudGroupModel *gr in model.groupList) {
        SigGroupModel *groud = [[SigGroupModel alloc] init];
        groud.name = gr.name;
        groud.address = [NSString stringWithFormat:@"%lX", (long)gr.address];
        [groups addObject:groud];
    }
    dataSource.groups = groups;

    NSMutableArray *nodes = [NSMutableArray array];
    for (CloudNodeModel *no in model.nodeList) {
        SigNodeModel *node = [[SigNodeModel alloc] init];
        node.name = no.name;
        node.address = no.address;
        node.deviceKey = no.deviceKey;
        node.UUID = no.deviceUuid;
        node.configComplete = no == 0 ? NO : YES;
        UInt8 page0 = 0;
        NSMutableData *mData = [NSMutableData dataWithBytes:&page0 length:1];
        [mData appendData:[LibTools nsstringToHex:no.versionInfo.compositionData]];
        SigPage0 *page = [[SigPage0 alloc] initWithParameters:mData];
        [node setCompositionData:page];
        if (no.bindState && model.applicationKeyList.count == 1) {
            //2023年02月10日17:26:23 云端node暂时没有AppKey的数据，APP端补全到SigDataSource
            NSMutableArray *nodeAppKeys = [NSMutableArray array];
            SigNodeKeyModel *keyModel = [[SigNodeKeyModel alloc] initWithIndex:model.applicationKeyList.firstObject.index updated:NO];
            [nodeAppKeys addObject:keyModel];
            node.appKeys = nodeAppKeys;
        }
        for (CloudGroupModel *groupModel in no.subList) {
            [node addGroupID:@(groupModel.address)];
        }
        if (no.publishInfo && no.publishInfo.publishAddress == 0xFFFF) {
            [node openPublish];
        }
        [nodes addObject:node];
    }
    if (model.provisioner) {
        NSMutableArray *provisioners = [NSMutableArray array];
        SigProvisionerModel *provisioner = [[SigProvisionerModel alloc] init];
        provisioner.UUID = SigDataSource.share.getCurrentProvisionerUUID;
        provisioner.provisionerName = model.provisioner.name;
        [provisioners addObject:provisioner];
        dataSource.provisioners = provisioners;
        
        SigNodeModel *node = [[SigNodeModel alloc] init];
        [node setAddress:model.provisioner.address];
        node.name = model.provisioner.name;
        node.UUID = SigDataSource.share.getCurrentProvisionerUUID;
        SigElementModel *ele = [[SigElementModel alloc] init];
        ele.parentNodeAddress = model.provisioner.address;
        NSMutableArray *eles = [NSMutableArray arrayWithObject:ele];
        node.elements = eles;
        [nodes addObject:node];
    }
    dataSource.nodes = nodes;
    [dataSource saveCurrentIvIndex:(UInt32)model.ivIndex sequenceNumber:(UInt32)model.provisioner.sequenceNumber + dataSource.defaultSequenceNumberIncrement];
}

- (void)importCloudNodeConfigsModel:(CloudNodeConfigsModel *)nodeConfigs toSigNodeModel:(SigNodeModel *)nodeModel {
    if (nodeConfigs.defaultTtl) {
        nodeModel.defaultTTL = [nodeConfigs.defaultTtl intValue];
    }
    if (nodeConfigs.relay) {
        nodeModel.features.relayFeature = [nodeConfigs.relay intValue];
    }
    if (nodeConfigs.relayRetransmit) {
        SigRelayRetransmitModel *model = [[SigRelayRetransmitModel alloc] init];
        model.relayRetransmitCount = [nodeConfigs.relayRetransmit intValue] & 0b111;
        model.relayRetransmitIntervalSteps = ([nodeConfigs.relayRetransmit intValue] >> 3) & 0b11111;
        nodeModel.relayRetransmit = model;
    }
    if (nodeConfigs.secureNetworkBeacon) {
        nodeModel.secureNetworkBeacon = [nodeConfigs.secureNetworkBeacon intValue];
        [SigDataSource.share setLocalConfigBeaconState:[nodeConfigs.secureNetworkBeacon intValue] unicastAddress:nodeModel.address];
    }
    if (nodeConfigs.gattProxy) {
        nodeModel.features.proxyFeature = [nodeConfigs.gattProxy intValue];
        [SigDataSource.share setLocalConfigGattProxyState:[nodeConfigs.gattProxy intValue] unicastAddress:nodeModel.address];
    }
    if (nodeConfigs.friendConfig) {
        nodeModel.features.friendFeature = [nodeConfigs.friendConfig intValue];
    }
    if (nodeConfigs.networkRetransmit) {
        SigNetworktransmitModel *model = [[SigNetworktransmitModel alloc] init];
        model.networkTransmitCount = [nodeConfigs.networkRetransmit intValue] & 0b111;
        model.networkTransmitIntervalSteps = ([nodeConfigs.networkRetransmit intValue] >> 3) & 0b11111;
        nodeModel.networkTransmit = model;
    }
    if (nodeConfigs.onDemandPrivateGattProxy) {
        nodeModel.onDemandPrivateGATTProxy = [nodeConfigs.onDemandPrivateGattProxy intValue];
    }
    if (nodeConfigs.privateGattProxy) {
        [SigDataSource.share setLocalPrivateGattProxyState:[nodeConfigs.privateGattProxy intValue] unicastAddress:nodeModel.address];
    }
    if (nodeConfigs.privateBeacon) {
        [SigDataSource.share setLocalPrivateBeaconState:[nodeConfigs.privateBeacon intValue] unicastAddress:nodeModel.address];
    }
    if (nodeConfigs.directForwarding) {
        nodeModel.directControlStatus.directedForwardingState = [nodeConfigs.directForwarding intValue];
    }
    if (nodeConfigs.directRelay) {
        nodeModel.directControlStatus.directedRelayState = [nodeConfigs.directRelay intValue];
    }
    if (nodeConfigs.directProxy) {
        nodeModel.directControlStatus.directedProxyState = [nodeConfigs.directProxy intValue];
    }
    if (nodeConfigs.directFriend) {
        nodeModel.directControlStatus.directedFriendState = [nodeConfigs.directFriend intValue];
    }
}

- (void)saveToLocation {
    
}

- (CloudProvisionerModel *)curProvisionerModel {
    return self.curMeshNetworkDetailModel.provisioner;
}

/// 当前手机的设备ID，第一次打开APP的时候生成一次。 为SigDataSource.share.getCurrentProvisionerUUID
- (NSString *)curClientId {
    return SigDataSource.share.getCurrentProvisionerUUID;
}

- (AppMeshNetworkModel *)getAppMeshNetworkModelWithUuid:(NSString *)uuid {
    NSMutableArray *array = [NSMutableArray arrayWithArray:_createdNetwordList];
    [array addObjectsFromArray:_sharedNetwordList];
    AppMeshNetworkModel *model = nil;
    for (AppMeshNetworkModel *tem in array) {
        if ([tem.uuid isEqualToString:uuid]) {
            model = tem;
            break;
        }
    }
    return model;
}


@end
