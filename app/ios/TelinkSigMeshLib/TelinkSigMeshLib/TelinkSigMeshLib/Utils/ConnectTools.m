/********************************************************************************************************
 * @file     ConnectTools.m
 *
 * @brief    A concise description.
 *
 * @author       梁家誌
 * @date         2021/4/19
 *
 * @par      Copyright © 2021 Telink Semiconductor (Shanghai) Co., Ltd. All rights reserved.
 *
 *           The information contained herein is confidential property of Telink
 *           Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *           of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *           Co., Ltd. and the licensee or the terms described here-in. This heading
 *           MUST NOT be removed from this file.
 *
 *           Licensee shall not delete, modify or alter (or permit any third party to delete, modify, or  
 *           alter) any information contained herein in whole or in part except as expressly authorized  
 *           by Telink semiconductor (shanghai) Co., Ltd. Otherwise, licensee shall be solely responsible  
 *           for any claim to the extent arising out of or relating to such deletion(s), modification(s)  
 *           or alteration(s).
 *
 *           Licensees are granted free, non-transferable use of the information in this
 *           file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/

#import "ConnectTools.h"

#define kScanTimeout    (5.0)

typedef enum : NSUInteger {
    ConnectToolsProgress_scan = 0,
    ConnectToolsProgress_connect,
    ConnectToolsProgress_setFilter,
    ConnectToolsProgress_setNodeIdentity,
} ConnectToolsProgress;

@interface ConnectTools ()
@property (nonatomic, strong) CBPeripheral *peripheral;
@property (nonatomic, assign) NSInteger maxRssi;
@property (nonatomic, strong) NSMutableArray <SigNodeModel *>*connectNodeList;
@property (nonatomic, copy) startMeshConnectResultBlock startMeshConnectCallback;
@property (nonatomic, copy) stopMeshConnectResultBlock stopMeshConnectCallback;
@property (nonatomic, assign) BOOL isEnd;
@property (nonatomic, assign) BOOL isFirstScan;//第一次扫描，1秒内扫描不到对应设备则开始连接并setFilter。后面则扫描5秒钟。

@end

@implementation ConnectTools

+ (ConnectTools *)share{
    static ConnectTools *shareT = nil;
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        shareT = [[ConnectTools alloc] init];
        shareT.peripheral = nil;
        shareT.maxRssi = -127;
    });
    return shareT;
}

/// demo 自定义连接工具类，用于开始连接指定的节点（逻辑：扫描5秒->扫描到则连接setFilter返回成功，扫描不到则连接已经扫描到的任意设备->setFilter->是则返回成功，不是则setNodeIdentity(多个设备则调用多次)->重复扫描5秒流程。）
/// @param nodeList 需要连接的节点，需要是SigDataSource里面的节点。
/// @param timeout 超时时间
/// @param complete 连接结果回调
- (void)startConnectToolsWithNodeList:(NSArray <SigNodeModel *>*)nodeList timeout:(NSInteger)timeout Complete:(nullable startMeshConnectResultBlock)complete {
    TeLogInfo(@"");
    if (nodeList == nil || nodeList.count == 0) {
        nodeList = SigDataSource.share.curNodes;
        TeLogWarn(@"nodeList is nil, set nodeList with SigDataSource.share.curNodes!!!");
    }
    self.startMeshConnectCallback = complete;
    self.connectNodeList = [NSMutableArray arrayWithArray:nodeList];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self];
        [self performSelector:@selector(connectToolsTimeout) withObject:nil afterDelay:timeout];
    });
    self.isEnd = NO;
    self.isFirstScan = NO;
    TeLogInfo(@"isAutoReconnect=%d,unicastAddressOfConnected=%d",SigBearer.share.isAutoReconnect,SigDataSource.share.unicastAddressOfConnected);
    for (SigNodeModel *node in nodeList) {
        TeLogInfo(@"try to connect node:0x%X",node.address);
    }
    if (SigBearer.share.isOpen) {
        BOOL isConnected = NO;
        NSArray *array = [NSArray arrayWithArray:nodeList];
        for (SigNodeModel *node in array) {
            if (node.address == SigDataSource.share.unicastAddressOfConnected) {
                isConnected = YES;
                break;
            }
        }
        if (isConnected) {
            [self successAction];
        } else {
            [self setNodeIdentity];
        }
    } else {
        self.isFirstScan = YES;
        [self scanNode];
    }
}

- (void)scanNode {
    TeLogInfo(@"");
    __weak typeof(self) weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(connectPeripheral) object:nil];
        [self performSelector:@selector(connectPeripheral) withObject:nil afterDelay:kScanTimeout];
    });
    self.peripheral = nil;
    self.maxRssi = -127;
    if (self.isEnd) {
        return;
    }
    [SDKLibCommand scanProvisionedDevicesWithResult:^(CBPeripheral * _Nonnull peripheral, NSDictionary<NSString *,id> * _Nonnull advertisementData, NSNumber * _Nonnull RSSI, BOOL unprovisioned) {
        TeLogInfo(@"");
        if (!unprovisioned && !weakSelf.isEnd) {
            if (RSSI.intValue > weakSelf.maxRssi) {
                self.peripheral = peripheral;
            }
            if (weakSelf.isFirstScan) {
                weakSelf.isFirstScan = NO;
                dispatch_async(dispatch_get_main_queue(), ^{
                    [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(firstScanFinishAction) object:nil];
                    [weakSelf performSelector:@selector(firstScanFinishAction) withObject:nil afterDelay:1.0];
                });
            }
            SigScanRspModel *rspModel = [SigMeshLib.share.dataSource getScanRspModelWithUUID:peripheral.identifier.UUIDString];
            if (rspModel.nodeIdentityData && rspModel.nodeIdentityData.length == 16) {
                SigEncryptedModel *encryptedModel = nil;
                NSArray *temArray = [NSMutableArray arrayWithArray:weakSelf.connectNodeList];
                for (SigNodeModel *node in temArray) {
                    encryptedModel = [SigMeshLib.share.dataSource getSigEncryptedModelWithAddress:node.address];
                    if (encryptedModel != nil) {
                        break;
                    }
                }
                if (encryptedModel && encryptedModel.identityData && encryptedModel.identityData.length == 16 && [encryptedModel.identityData isEqualToData:rspModel.nodeIdentityData]) {
                    TeLogInfo(@"start connect address:0x%X macAddress:%@",rspModel.address,rspModel.macAddress);
                    weakSelf.peripheral = peripheral;
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(firstScanFinishAction) object:nil];
                    });
                    [SDKLibCommand stopScan];
                    [weakSelf connectPeripheral];
                }
            }
        }
    }];
}

- (void)firstScanFinishAction {
    TeLogInfo(@"");
    [SDKLibCommand stopScan];
    [self connectPeripheral];
}

- (void)connectPeripheral {
    TeLogInfo(@"");
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(connectPeripheral) object:nil];
    });
    if (self.isEnd) {
        return;
    }
    if (self.peripheral) {
        __weak typeof(self) weakSelf = self;
        [SigBearer.share changePeripheral:self.peripheral result:^(BOOL successful) {
            if (weakSelf.isEnd) {
                return;
            }
            if (successful) {
                [SigBearer.share openWithResult:^(BOOL successful) {
                    if (weakSelf.isEnd) {
                        return;
                    }
                    if (successful) {
                        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.5 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                            [SDKLibCommand setFilterForProvisioner:SigDataSource.share.curProvisionerModel successCallback:^(UInt16 source, UInt16 destination, SigFilterStatus * _Nonnull responseMessage) {
                                
                            } finishCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                                if (error) {
                                    TeLogVerbose(@"setFilter失败");
                                    [weakSelf scanNode];
                                } else {
                                    UInt16 unicastAddressOfConnected = SigDataSource.share.unicastAddressOfConnected;
                                    TeLogVerbose(@"setFilter成功，unicastAddressOfConnected=0x%X",unicastAddressOfConnected);
                                    BOOL success = NO;
                                    for (SigNodeModel *node in weakSelf.connectNodeList) {
                                        if (node.address == unicastAddressOfConnected) {
                                            success = YES;
                                            break;
                                        }
                                    }
                                    if (success) {
                                        TeLogVerbose(@"连接到正确的节点");
                                        [weakSelf successAction];
                                    } else {
                                        TeLogVerbose(@"未连接到正确的节点，开始setNodeIdentity");
                                        [weakSelf setNodeIdentity];
                                    }
                                }
                            }];
                        });
                    } else {
                        TeLogVerbose(@"连接失败或者读服务失败");
                        [weakSelf scanNode];
                    }
                }];
            } else {
                TeLogVerbose(@"断开连接失败");
                [weakSelf scanNode];
            }
        }];
    } else {
        TeLogVerbose(@"扫描超时");
        [self scanNode];
    }
}

- (void)setNodeIdentity {
    TeLogInfo(@"");
    if (self.isEnd) {
        return;
    }
    if (self.connectNodeList.count > 0) {
        NSMutableArray *array = [[NSMutableArray alloc] initWithArray:self.connectNodeList];
        NSOperationQueue *oprationQueue = [[NSOperationQueue alloc] init];
        __weak typeof(self) weakSelf = self;
        [oprationQueue addOperationWithBlock:^{
            while (array.count > 0) {
                if (weakSelf.isEnd) {
                    return;
                }
                dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
                SigNodeModel *node = array.firstObject;
                TeLogVerbose(@"NodeIdentitySet:0x%X",node.address);
                [SDKLibCommand configNodeIdentitySetWithDestination:node.address netKeyIndex:SigDataSource.share.curNetkeyModel.index identity:SigNodeIdentityState_enabled retryCount:SigMeshLib.share.dataSource.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigNodeIdentityStatus * _Nonnull responseMessage) {
                    TeLogInfo(@"configNodeIdentitySetWithDestination=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    [array removeObject:node];
                    TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    dispatch_semaphore_signal(semaphore);
                }];
                dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 4.0));
            }
            if (weakSelf.isEnd) {
                return;
            }
            TeLogVerbose(@"NodeIdentitySet完成");
            [SDKLibCommand stopMeshConnectWithComplete:^(BOOL successful) {
                TeLogVerbose(@"NodeIdentitySet完成后，断开连接完成");
                if (weakSelf.isEnd) {
                    return;
                }
                TeLogVerbose(@"断开连接完成，开始下一轮设备扫描");
                [weakSelf scanNode];
            }];
        }];
        
    }
}

/// demo 自定义连接工具类，用于停止连接指定的节点流程并断开当前的连接。
- (void)stopConnectToolsWithComplete:(nullable stopMeshConnectResultBlock)complete {
    self.stopMeshConnectCallback = complete;
    [self endAction];
}

/// demo 自定义连接工具类，用于停止连接指定的节点流程保持当前的连接。
- (void)endConnectTools {
    TeLogInfo(@"");
    self.isEnd = YES;
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self];
    });
}

- (void)successAction {
    TeLogInfo(@"");
    self.isEnd = YES;
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self];
    });
//    SigBearer.share.isAutoReconnect = YES;//断开后会自动重连，但重连的是所有设备而不是当前类里面的这些特定设备。
    [self startMeshConnectSuccess];
}

- (void)endAction {
    TeLogInfo(@"");
    self.isEnd = YES;
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self];
    });
    [SDKLibCommand stopMeshConnectWithComplete:self.stopMeshConnectCallback];
    if (self.startMeshConnectCallback) {
        self.startMeshConnectCallback(NO);
    }
}

- (void)connectToolsTimeout {
    TeLogInfo(@"");
    [self endAction];
}

- (void)startMeshConnectSuccess {
    if (SigMeshLib.share.dataSource.hasNodeExistTimeModelID && SigMeshLib.share.dataSource.needPublishTimeModel) {
        [SDKLibCommand statusNowTime];
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.3 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            if ([SigBearer.share.dataDelegate respondsToSelector:@selector(bearerDidOpen:)]) {
                [SigBearer.share.dataDelegate bearerDidOpen:SigBearer.share];
            }
            if (self.startMeshConnectCallback) {
                self.startMeshConnectCallback(YES);
            }
        });
    }else{
        if ([SigBearer.share.dataDelegate respondsToSelector:@selector(bearerDidOpen:)]) {
            [SigBearer.share.dataDelegate bearerDidOpen:SigBearer.share];
        }
        if (self.startMeshConnectCallback) {
            self.startMeshConnectCallback(YES);
        }
    }
}

@end
