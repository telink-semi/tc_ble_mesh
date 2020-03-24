/********************************************************************************************************
 * @file     MeshOTAManager.m
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
//  MeshOTAManager.m
//  TelinkBlueDemo
//
//  Created by Arvin on 2018/4/24.
//  Copyright © 2018年 Green. All rights reserved.
//

#import "MeshOTAManager.h"

//每一个步骤的超时时间，默认是20.0s
#define kTimeOutOfEveryStep (dispatch_semaphore_wait(weakSelf.semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 20.0)))
//block大小，为10的kBlockSizeLog次幂，默认是10^12=4096字节
#define kBlockSizeLog   (0x0C)
#define kChunkSize      (0x0100)
#define kRetryCountInObjectChunkTransfer    (3)
#define kMeshOTAGroupAddress    (0xc000)

#define kCid    @"cid"
#define kPid    @"pid"
#define kVid    @"vid"

@interface MeshOTAManager()<SigBearerDataDelegate>
@property (nonatomic, copy) ProgressBlock progressBlock;
@property (nonatomic, copy) FinishBlock finishBlock;
@property (nonatomic, copy) ErrorBlock errorBlock;
@property (nonatomic, strong) NSData *otaData;
@property (nonatomic, assign) UInt8 blockSizeLog;//记录本次meshOTA的block大小
@property (nonatomic, strong) NSData *firmwareIDData;
@property (nonatomic, assign) UInt64 objectID;
@property (nonatomic, assign) UInt16 companyID;
@property (nonatomic, strong) NSError *failError;

@property (nonatomic, strong) NSMutableArray <NSNumber *>*allAddressArray;//Mesh OTA的所有短地址列表
@property (nonatomic, strong) NSMutableArray <NSNumber *>*successAddressArray;//Mesh OTA成功的短地址列表

@property (nonatomic, assign) SigMeshOTAProgress meshOTAProgress;//记录meshOTA当前的进度
@property (nonatomic, assign) UInt8 allBlockCount;//记录block总个数
@property (nonatomic, assign) UInt8 blockIndex;//记录当前block的index
@property (nonatomic, strong) NSData *currentBlockData;//记录当前block的data
@property (nonatomic, assign) UInt8 chunksCountofCurrentBlock;//记录当前block的chunk总个数
@property (nonatomic, assign) UInt8 chunkIndex;//记录当前chunk的index
@property (nonatomic, assign) UInt8 successActionInCurrentProgress;//记录当前阶段成功的设备个数
@property (nonatomic, strong) NSMutableDictionary <NSNumber *, NSArray *>*losePacketsDict;//step8阶段传输失败的包。
@property (nonatomic, strong) NSMutableDictionary <NSNumber *, NSDictionary *>*oFirmwareInformations;//记录MeshOTA前设备的版本号,[@(UInt16):@{@"cid":@(UInt16),@"pid":@(UInt16),@"vid":@(UInt16)}]
@property (nonatomic, strong) NSMutableDictionary <NSNumber *, NSNumber *>*nFirmwareInformations;//记录MeshOTA后设备的版本号,[@(UInt16):@{@"cid":@(UInt16),@"pid":@(UInt16),@"vid":@(UInt16)}]
@property (nonatomic, strong) NSMutableDictionary <NSNumber *, SigObjectInformationStatus *>*oObjectInformations;//记录MeshOTA前设备的ObjectInformation
@property (nonatomic, assign) UInt8 retryCountInObjectChunkTransfer;//记录step8:ObjectChunkTransfer阶段已经重试的次数
@property (nonatomic, retain) dispatch_semaphore_t semaphore;
@property (nonatomic, strong) SigMessageHandle *messageHandle;

@property (nonatomic, strong) NSThread *meshOTAThread;
@property (nonatomic, weak) id <SigBearerDataDelegate>oldBearerDataDelegate;

@end

@implementation MeshOTAManager

+ (MeshOTAManager*)share{
    static MeshOTAManager *meshOTAManager = nil;
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        meshOTAManager = [[MeshOTAManager alloc] init];
        [meshOTAManager initData];
    });
    return meshOTAManager;
}

-(void)initData {
    _allAddressArray = [NSMutableArray array];
    _successAddressArray = [NSMutableArray array];
    UInt32 firmwareID = 0xff000021;
    _firmwareIDData = [NSData dataWithBytes:&firmwareID length:4];
    _objectID = 0x8877665544332211;
    _companyID = 0x0211;
    
    _meshOTAThread = [[NSThread alloc] initWithTarget:self selector:@selector(startThread) object:nil];
    _meshOTAThread.name = @"meshOTAThread";
    [_meshOTAThread start];
}

#pragma mark - Private
- (void)startThread{
    [NSTimer scheduledTimerWithTimeInterval:[[NSDate distantFuture] timeIntervalSinceNow] target:self selector:@selector(nullFunc) userInfo:nil repeats:NO];
    while (1) {
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
    }
}

- (void)nullFunc{}

- (void)resetMeshOTAData{
    self.otaData = nil;
    self.failError = nil;
    self.progressBlock = nil;
    self.finishBlock = nil;
    self.errorBlock = nil;
}

- (void)stopMeshOTA{
    SigBearer.share.dataDelegate = self.oldBearerDataDelegate;

    [self resetMeshOTAData];
    
    [self saveIsMeshOTAing:NO];
    dispatch_semaphore_signal(self.semaphore);
    [self.meshOTAThread cancel];
    [SigMeshLib.share cancelSigMessageHandle:self.messageHandle];

    [SDKLibCommand firmwareDistributionStopWithDestination:SigDataSource.share.getCurrentConnectedNode.address companyID:self.companyID firmwareID:self.firmwareIDData resMax:0 retryCount:0 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareDistributionStatus * _Nonnull responseMessage) {
        TeLogDebug(@"firmwareDistributionStop=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
    }];
    
    [SigBearer.share stopMeshConnectWithComplete:^(BOOL successful) {
        TeLogInfo(@"断开连接:%@",successful?@"成功":@"失败");
    }];
}

- (void)cancelSignal {
    dispatch_semaphore_signal(self.semaphore);
}

///查询当前是否处在meshOTA
- (BOOL)isMeshOTAing{
    NSDictionary *dict = [[NSUserDefaults standardUserDefaults] objectForKey:kSaveMeshOTADictKey];
    if (dict != nil && [dict[@"isMeshOTAing"] boolValue]) {
        return YES;
    } else {
        return NO;
    }
}

- (void)saveIsMeshOTAing:(BOOL)isMeshOTAing{
    NSDictionary *dict = @{@"isMeshOTAing":@(isMeshOTAing)};
    [[NSUserDefaults standardUserDefaults] setObject:dict forKey:kSaveMeshOTADictKey];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (void)callBackMeshOTASuccessAddress:(UInt16)address{
    if (![self.successAddressArray containsObject:@(address)] && [self.allAddressArray containsObject:@(address)]) {
        [self.successAddressArray addObject:@(address)];
    }
}

- (void)showMeshOTAResult{
    [self saveIsMeshOTAing:NO];
    if (self.finishBlock) {
        NSMutableArray *failArray = [NSMutableArray array];
        if (self.allAddressArray.count != self.successAddressArray.count) {
            for (NSNumber *tem in self.allAddressArray) {
                if (![self.successAddressArray containsObject:tem]) {
                    [failArray addObject:tem];
                }
            }
        }
        self.finishBlock(self.successAddressArray, failArray);
    }
    //OTA完成，初始化参数
    [self resetMeshOTAData];
}

- (void)startMeshOTAWithLocationAddress:(int)locationAddress cid:(int)cid deviceAddresses:(NSArray <NSNumber *>*)deviceAddresses otaData:(NSData *)otaData progressHandle:(ProgressBlock)progressBlock finishHandle:(FinishBlock)finishBlock errorHandle:(ErrorBlock)errorBlock{
    self.otaData = otaData;
    self.progressBlock = progressBlock;
    self.finishBlock = finishBlock;
    self.errorBlock = errorBlock;
    [self saveIsMeshOTAing:YES];
    [self.allAddressArray removeAllObjects];
    [self.successAddressArray removeAllObjects];
    [self.allAddressArray addObjectsFromArray:deviceAddresses];
    
    self.oldBearerDataDelegate = SigBearer.share.dataDelegate;
    SigBearer.share.dataDelegate = self;

    _companyID = cid;
    
    //1.
//    __weak typeof(self) weakSelf = self;
//    NSOperationQueue *oprationQueue = [[NSOperationQueue alloc] init];
//    [oprationQueue addOperationWithBlock:^{
//        //这个block语句块在子线程中执行
//        NSLog(@"oprationQueue");
//        [weakSelf startConfigModelSubscriptionAdd];
//    }];
    
    //2.
//    [self startConfigModelSubscriptionAdd];
    
    //3.
    [self performSelector:@selector(startConfigModelSubscriptionAdd) onThread:self.meshOTAThread withObject:nil waitUntilDone:NO];
}

#pragma mark step1:configModelSubscriptionAdd
- (void)startConfigModelSubscriptionAdd {
    TeLogInfo(@"SigDataSource.share.getCurrentConnectedNode.address=0x%x",SigDataSource.share.getCurrentConnectedNode.address);
    TeLogInfo(@"\n\n\n\n==========step1\n\n\n\n");
    if (self.progressBlock) {
        self.progressBlock(0);
    }
    self.meshOTAProgress = SigMeshOTAProgressSubscriptionAdd;
    BOOL hasFail = NO;
    self.successActionInCurrentProgress = 0;
    __weak typeof(self) weakSelf = self;
    for (NSNumber *nodeAddress in self.allAddressArray) {
        if (![self isMeshOTAing]) {
            return;
        }
        if ([self.allAddressArray indexOfObject:nodeAddress] == self.successActionInCurrentProgress) {
            SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:nodeAddress.intValue];
            if (node.state != DeviceStateOutOfLine) {
                UInt16 modelID = 0xFF00;
                UInt16 groupAddress = kMeshOTAGroupAddress;
                NSArray *addressArray = [node getAddressesWithModelID:@(modelID)];
                if (addressArray && addressArray.count > 0) {
                    UInt16 eleAddress = [addressArray.firstObject intValue];
                    self.semaphore = dispatch_semaphore_create(0);
                    self.messageHandle = [SDKLibCommand configModelSubscriptionAddWithGroupAddress:groupAddress toNodeAddress:node.address elementAddress:eleAddress modelIdentifier:modelID companyIdentifier:0 successCallback:^(UInt16 source, UInt16 destination, SigConfigModelSubscriptionStatus * _Nonnull responseMessage) {
                        if (responseMessage.elementAddress == eleAddress && responseMessage.address == groupAddress) {
                            TeLogDebug(@"configModelSubscriptionAdd=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                            UInt32 modelId = 0;
                            if (responseMessage.companyIdentifier == 0) {
                                modelId = responseMessage.modelIdentifier;
                            } else {
                                modelId = responseMessage.modelIdentifier | (responseMessage.companyIdentifier << 16);
                            }
                            if (modelId == modelID) {
                                if (responseMessage.status == SigConfigMessageStatus_success) {
                                    weakSelf.successActionInCurrentProgress ++;
                                } else {
                                    weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in configModelSubscriptionAdd,SigConfigModelSubscriptionStatus.status=0x%x",responseMessage.status] code:-weakSelf.meshOTAProgress userInfo:nil];
                                    TeLogError(@"fail in configModelSubscriptionAdd,SigConfigModelSubscriptionStatus.status=0x%x",responseMessage.status);
                                }
                            }
                        }
                    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                        TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                        [NSThread sleepForTimeInterval:0.2];
                        dispatch_semaphore_signal(weakSelf.semaphore);
                    }];
                    //Most provide 3 seconds to configModelSubscriptionAdd every node.
                    dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
                } else {
                    self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in configModelSubscriptionAdd, there is no modelID = 0xFF00"] code:-weakSelf.meshOTAProgress userInfo:nil];
                    hasFail = YES;
                   break;
               }
            } else {
                self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in configModelSubscriptionAdd, node is outOfLine"] code:-weakSelf.meshOTAProgress userInfo:nil];
                hasFail = YES;
                break;
            }
        } else {
            if (!self.failError) {
                self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in configModelSubscriptionAdd, SubscriptionAdd is timeout."] code:-weakSelf.meshOTAProgress userInfo:nil];
            }
            hasFail = YES;
            break;
        }
    }
    if (hasFail) {
        [self configModelSubscriptionAddFailAction];
    } else {
        [self configModelSubscriptionAddSuccessAction];
    }
}

- (void)configModelSubscriptionAddSuccessAction {
    [self firmwareInformationGet];
}

- (void)configModelSubscriptionAddFailAction {
    [self meshOTAFailAction];
}

#pragma mark step2:firmwareInformationGet
- (void)firmwareInformationGet {
    TeLogInfo(@"\n\n\n\n==========step2\n\n\n\n");
    self.meshOTAProgress = SigMeshOTAProgressFirmwareInformationGet;
    __block BOOL hasFail = NO;
    self.successActionInCurrentProgress = 0;
    self.oFirmwareInformations = [NSMutableDictionary dictionary];
    __weak typeof(self) weakSelf = self;
    for (NSNumber *nodeAddress in self.allAddressArray) {
        if (![self isMeshOTAing]) {
            return;
        }
        if ([self.allAddressArray indexOfObject:nodeAddress] == self.successActionInCurrentProgress) {
            SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:nodeAddress.intValue];
            if (node.state != DeviceStateOutOfLine) {
                self.semaphore = dispatch_semaphore_create(0);
                self.messageHandle = [SDKLibCommand firmwareInformationGetWithDestination:node.address resMax:1 retryCount:2 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareInformationStatus * _Nonnull responseMessage) {
                    if (source == nodeAddress.intValue) {
                        /*
                         responseMessage.parameters.length = 6: 2 bytes cid(vendor id) + 2 bytes pid(设备类型) + 2 bytes vid(版本id).
                         */
                        UInt16 cid = 0,pid = 0,vid = 0;
                        Byte *pu = (Byte *)[responseMessage.parameters bytes];
                        if (responseMessage.parameters.length >= 2) memcpy(&cid, pu, 2);
                        if (responseMessage.parameters.length >= 4) memcpy(&pid, pu + 2, 2);
                        if (responseMessage.parameters.length >= 6) memcpy(&vid, pu + 4, 2);
                        TeLogDebug(@"firmwareInformationGet=%@,cid=%d,pid=%d,vid=%d",[LibTools convertDataToHexStr:responseMessage.parameters],cid,pid,vid);
                        weakSelf.oFirmwareInformations[@(source)] = @{kCid:@(cid),kPid:@(pid),kVid:@(vid)};
                        weakSelf.successActionInCurrentProgress ++;
                    } else {
                        hasFail = YES;
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    [NSThread sleepForTimeInterval:0.2];
                        dispatch_semaphore_signal(weakSelf.semaphore);
                }];
                //Most provide 3 seconds to firmwareInformationGet every node.
                dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
            } else {
                self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareInformationGet, node is outOfLine"] code:-weakSelf.meshOTAProgress userInfo:nil];
                hasFail = YES;
                break;
            }
        } else {
            if (!self.failError) {
                self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareInformationGet, firmwareInformationGet is timeout."] code:-weakSelf.meshOTAProgress userInfo:nil];
            }
            hasFail = YES;
            break;
        }
    }
    if (hasFail) {
        [self firmwareInformationGetFailAction];
    } else {
        [self firmwareInformationGetSuccessAction];
    }
}

- (void)firmwareInformationGetSuccessAction {
    [self objectInformationGet];
}

- (void)firmwareInformationGetFailAction {
    [self meshOTAFailAction];
}

#pragma mark step3:objectInformationGet
- (void)objectInformationGet {
    TeLogInfo(@"\n\n\n\n==========step3\n\n\n\n");
    self.meshOTAProgress = SigMeshOTAProgressFirmwareInformationGet;
    __block BOOL hasFail = NO;
    self.successActionInCurrentProgress = 0;
    self.oObjectInformations = [NSMutableDictionary dictionary];
    __weak typeof(self) weakSelf = self;
    for (NSNumber *nodeAddress in self.allAddressArray) {
        if (![self isMeshOTAing]) {
            return;
        }
        if ([self.allAddressArray indexOfObject:nodeAddress] == self.successActionInCurrentProgress) {
            SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:nodeAddress.intValue];
            if (node.state != DeviceStateOutOfLine) {
                self.semaphore = dispatch_semaphore_create(0);
                self.messageHandle = [SDKLibCommand objectInformationGetWithDestination:node.address resMax:1 retryCount:2 successCallback:^(UInt16 source, UInt16 destination, SigObjectInformationStatus * _Nonnull responseMessage) {
                    if (source == nodeAddress.intValue) {
                        weakSelf.oObjectInformations[@(source)] = responseMessage;
                        TeLogDebug(@"objectInformationGet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                        weakSelf.successActionInCurrentProgress ++;
                    } else {
                        hasFail = YES;
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    [NSThread sleepForTimeInterval:0.2];
                        dispatch_semaphore_signal(weakSelf.semaphore);
                }];
                //Most provide 3 seconds to firmwareInformationGet every node.
                dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
            } else {
                self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in objectInformationGet, node is outOfLine"] code:-weakSelf.meshOTAProgress userInfo:nil];
                hasFail = YES;
                break;
            }
        } else {
            if (!self.failError) {
                self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in objectInformationGet, objectInformationGet is timeout."] code:-weakSelf.meshOTAProgress userInfo:nil];
            }
            hasFail = YES;
            break;
        }
    }
    if (hasFail) {
        [self objectInformationGetFailAction];
    } else {
        if (self.oObjectInformations && self.oObjectInformations.count > 0) {
            UInt8 minBlockSizeLog = self.oObjectInformations.allValues.firstObject.minBlockSizeLog;
            UInt8 maxBlockSizeLog = self.oObjectInformations.allValues.firstObject.maxBlockSizeLog;
            UInt16 maxChunksNumber = self.oObjectInformations.allValues.firstObject.maxChunksNumber;
            for (SigObjectInformationStatus *message in self.oObjectInformations.allValues) {
                if (minBlockSizeLog < message.minBlockSizeLog) {
                    minBlockSizeLog = message.minBlockSizeLog;
                }
                if (maxBlockSizeLog > message.maxBlockSizeLog) {
                    maxBlockSizeLog = message.maxBlockSizeLog;
                }
                if (maxChunksNumber > message.maxChunksNumber) {
                    maxChunksNumber = message.maxChunksNumber;
                }
            }
            
            BOOL isAccord = YES;
            if (minBlockSizeLog <= kBlockSizeLog && maxBlockSizeLog >= kBlockSizeLog) {
                UInt16 blockSize = pow(2, kBlockSizeLog);
                if (ceil(blockSize / 4096.0) <= maxChunksNumber) {
                    self.blockSizeLog = kBlockSizeLog;
                } else {
                    isAccord = NO;
                }
            } else {
                isAccord = NO;
            }
            
            if (isAccord) {
                //默认支持
                [self objectInformationGetSuccessAction];
            } else {
                //默认值不支持，则重新计算blockSizeLog
                if (minBlockSizeLog > maxBlockSizeLog) {
                    //无所有设备都支持的blockSizeLog值，meshOTA失败
                    self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in objectInformationGet, blockSizeLog is error."] code:-weakSelf.meshOTAProgress userInfo:nil];
                    [self objectInformationGetFailAction];
                } else {
                    //先判断minBlockSizeLog是否可用
                    UInt16 blockSize = pow(2, minBlockSizeLog);
                    UInt16 minChunksSize = ceil(blockSize / maxChunksNumber);
                    if (minChunksSize <= 0x0100) {
                        isAccord = YES;
                        self.blockSizeLog = minBlockSizeLog;
                    } else {
                        isAccord = NO;
                    }

                    //minBlockSizeLog不可用，再判断maxBlockSizeLog是否可用
                    if (!isAccord) {
                        blockSize = pow(2, maxBlockSizeLog);
                        minChunksSize = ceil(blockSize / maxChunksNumber);
                        if (minChunksSize <= 0x0100) {
                            isAccord = YES;
                            self.blockSizeLog = maxBlockSizeLog;
                        } else {
                            isAccord = NO;
                        }
                    }
                    
                    if (isAccord) {
                        //重新计算blockSizeLog成功
                        [self objectInformationGetSuccessAction];
                    } else {
                        //重新计算blockSizeLog失败
                        self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in objectInformationGet, blockSizeLog is error."] code:-weakSelf.meshOTAProgress userInfo:nil];
                        [self objectInformationGetFailAction];
                    }
                }
            }
        } else {
            self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in objectInformationGet, objectInformation is empty."] code:-weakSelf.meshOTAProgress userInfo:nil];
            [self objectInformationGetFailAction];
        }
    }
}

- (void)objectInformationGetSuccessAction {
    [self firmwareUpdatePrepare];
}

- (void)objectInformationGetFailAction {
    [self meshOTAFailAction];
}

#pragma mark step4:firmwareUpdatePrepare
- (void)firmwareUpdatePrepare {
    TeLogInfo(@"\n\n\n\n==========step4\n\n\n\n");
    self.meshOTAProgress = SigMeshOTAProgressFirmwareUpdatePrepare;
    __block BOOL hasFail = NO;
    self.successActionInCurrentProgress = 0;
    __weak typeof(self) weakSelf = self;
    for (NSNumber *nodeAddress in self.allAddressArray) {
        if (![self isMeshOTAing]) {
            return;
        }
        if ([self.allAddressArray indexOfObject:nodeAddress] == self.successActionInCurrentProgress) {
            SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:nodeAddress.intValue];
            if (node.state != DeviceStateOutOfLine) {
                self.semaphore = dispatch_semaphore_create(0);
                TeLogInfo(@"firmwareUpdatePrepareWithDestination=0x%x",node.address);
                self.messageHandle = [SDKLibCommand firmwareUpdatePrepareWithDestination:node.address companyID:weakSelf.companyID firmwareID:weakSelf.firmwareIDData objectID:weakSelf.objectID vendorValidationData:nil resMax:1 retryCount:2 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareUpdateStatus * _Nonnull responseMessage) {
                    if (source == nodeAddress.intValue) {
                        TeLogDebug(@"firmwareUpdatePrepare=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                        if (responseMessage.status == SigFirmwareUpdateStatusType_success) {
                            if (responseMessage.phase == SigFirmwareUpdatePhaseType_prepare || responseMessage.phase == SigFirmwareUpdatePhaseType_inProgress) {
                                if (!responseMessage.additionalInformation.ProvisioningNeeded) {
                                    weakSelf.successActionInCurrentProgress ++;
                                } else {
                                    hasFail = YES;
                                    weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdatePrepare,responseMessage.additionalInformation.ProvisioningNeeded=0x%x",responseMessage.additionalInformation.ProvisioningNeeded] code:-weakSelf.meshOTAProgress userInfo:nil];
                                }
                            } else {
                                hasFail = YES;
                                weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdatePrepare,SigFirmwareUpdateStatus.phase=0x%x",responseMessage.phase] code:-weakSelf.meshOTAProgress userInfo:nil];
                            }
                        } else {
                            hasFail = YES;
                            weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdatePrepare,SigFirmwareUpdateStatus.status=0x%x",responseMessage.status] code:-weakSelf.meshOTAProgress userInfo:nil];
                        }
                    } else {
                        hasFail = YES;
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    [NSThread sleepForTimeInterval:0.2];
                        dispatch_semaphore_signal(weakSelf.semaphore);
                }];
                //Most provide 3 seconds to firmwareUpdatePrepare every node.
                dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
            } else {
                self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdatePrepare, node is outOfLine"] code:-weakSelf.meshOTAProgress userInfo:nil];
                hasFail = YES;
                break;
            }
        } else {
            if (!self.failError) {
                self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdatePrepare, firmwareUpdatePrepare is timeout."] code:-weakSelf.meshOTAProgress userInfo:nil];
            }
            hasFail = YES;
            break;
        }
    }
    if (hasFail) {
        [self firmwareUpdatePrepareFailAction];
    } else {
        [self firmwareUpdatePrepareSuccessAction];
    }
}

- (void)firmwareUpdatePrepareSuccessAction {
    [self firmwareUpdateStart];
}

- (void)firmwareUpdatePrepareFailAction {
    [self meshOTAFailAction];
}

#pragma mark step5:firmwareUpdateStart
- (void)firmwareUpdateStart {
    TeLogInfo(@"\n\n\n\n==========step5\n\n\n\n");
    self.meshOTAProgress = SigMeshOTAProgressFirmwareUpdateStart;
    __block BOOL hasFail = NO;
    self.successActionInCurrentProgress = 0;
    __weak typeof(self) weakSelf = self;
    for (NSNumber *nodeAddress in self.allAddressArray) {
        if (![self isMeshOTAing]) {
            return;
        }
        if ([self.allAddressArray indexOfObject:nodeAddress] == self.successActionInCurrentProgress) {
            SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:nodeAddress.intValue];
            if (node.state != DeviceStateOutOfLine) {
                self.semaphore = dispatch_semaphore_create(0);
                TeLogInfo(@"firmwareUpdateStartWithDestination=0x%x",node.address);
                self.messageHandle = [SDKLibCommand firmwareUpdateStartWithDestination:node.address updatePolicy:SigUpdatePolicyType_none companyID:self.companyID firmwareID:self.firmwareIDData resMax:1 retryCount:2 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareUpdateStatus * _Nonnull responseMessage) {
                    if (source == nodeAddress.intValue) {
                        TeLogDebug(@"firmwareUpdateStart=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                        if (responseMessage.status == SigFirmwareUpdateStatusType_success) {
                            if (responseMessage.phase == SigFirmwareUpdatePhaseType_inProgress) {
                                if (!responseMessage.additionalInformation.ProvisioningNeeded) {
                                    weakSelf.successActionInCurrentProgress ++;
                                } else {
                                    hasFail = YES;
                                    weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateStart,responseMessage.additionalInformation.ProvisioningNeeded=0x%x",responseMessage.additionalInformation.ProvisioningNeeded] code:-weakSelf.meshOTAProgress userInfo:nil];
                                }
                            } else {
                                hasFail = YES;
                                weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateStart,SigFirmwareUpdateStatus.phase=0x%x",responseMessage.phase] code:-weakSelf.meshOTAProgress userInfo:nil];
                            }
                        } else {
                            hasFail = YES;
                            weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateStart,SigFirmwareUpdateStatus.status=0x%x",responseMessage.status] code:-weakSelf.meshOTAProgress userInfo:nil];
                        }
                    } else {
                        hasFail = YES;
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    [NSThread sleepForTimeInterval:0.2];
                        dispatch_semaphore_signal(weakSelf.semaphore);
                }];
                //Most provide 3 seconds to firmwareUpdateStart every node.
                dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
            } else {
                self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateStart, node is outOfLine"] code:-weakSelf.meshOTAProgress userInfo:nil];
                hasFail = YES;
                break;
            }
        } else {
            if (!self.failError) {
                self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateStart, firmwareUpdateStart is timeout."] code:-weakSelf.meshOTAProgress userInfo:nil];
            }
            hasFail = YES;
            break;
        }
    }
    if (hasFail) {
        [self firmwareUpdateStartFailAction];
    } else {
        [self firmwareUpdateStartSuccessAction];
    }
}

- (void)firmwareUpdateStartSuccessAction {
    [self objectTransferStart];
}

- (void)firmwareUpdateStartFailAction {
    [self meshOTAFailAction];
}

#pragma mark step6:objectTransferStart
- (void)objectTransferStart {
    TeLogInfo(@"\n\n\n\n==========step6\n\n\n\n");
    self.meshOTAProgress = SigMeshOTAProgressObjectTransferStart;
    __block BOOL hasFail = NO;
    self.successActionInCurrentProgress = 0;
    __weak typeof(self) weakSelf = self;
    for (NSNumber *nodeAddress in self.allAddressArray) {
        if (![self isMeshOTAing]) {
            return;
        }
        if ([self.allAddressArray indexOfObject:nodeAddress] == self.successActionInCurrentProgress) {
            SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:nodeAddress.intValue];
            if (node.state != DeviceStateOutOfLine) {
                self.semaphore = dispatch_semaphore_create(0);
                self.messageHandle = [SDKLibCommand objectTransferStartWithDestination:node.address objectID:self.objectID objectSize:(UInt32)self.otaData.length blockSizeLog:self.blockSizeLog resMax:1 retryCount:2 successCallback:^(UInt16 source, UInt16 destination, SigObjectTransferStatus * _Nonnull responseMessage) {
                    if (source == nodeAddress.intValue) {
                        TeLogDebug(@"objectTransferStart=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                        if (responseMessage.status == SigObjectTransferStatusType_busy) {
                            weakSelf.successActionInCurrentProgress ++;
                        } else {
                           hasFail = YES;
                           weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in objectTransferStart,SigObjectTransferStatus.status=0x%x",responseMessage.status] code:-weakSelf.meshOTAProgress userInfo:nil];
                        }
                    } else {
                        hasFail = YES;
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    [NSThread sleepForTimeInterval:0.2];
                        dispatch_semaphore_signal(weakSelf.semaphore);
                }];
                //Most provide 3 seconds to objectTransferStart every node.
                dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
            } else {
                self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in objectTransferStart, node is outOfLine"] code:-weakSelf.meshOTAProgress userInfo:nil];
                hasFail = YES;
                break;
            }
        } else {
            if (!self.failError) {
                self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in objectTransferStart, objectTransferStart is timeout."] code:-weakSelf.meshOTAProgress userInfo:nil];
            }
            hasFail = YES;
            break;
        }
    }
    if (hasFail) {
        [self objectTransferStartFailAction];
    } else {
        [self objectTransferStartSuccessAction];
    }
}

- (void)objectTransferStartSuccessAction {
    self.blockIndex = 0;
    double blockSize = (double)pow(2, self.blockSizeLog);
    self.allBlockCount = ceil(self.otaData.length / blockSize);
    self.retryCountInObjectChunkTransfer = 0;
    [self objectBlockTransferStart];
}

- (void)objectTransferStartFailAction {
    [self meshOTAFailAction];
}

#pragma mark step7:objectBlockTransferStart
- (void)objectBlockTransferStart {
    TeLogInfo(@"\n\n\n\n==========step7\n\n\n\n");
    self.meshOTAProgress = SigMeshOTAProgressObjectBlockTransferStart;
    __block BOOL hasFail = NO;
    self.successActionInCurrentProgress = 0;
    __weak typeof(self) weakSelf = self;
    for (NSNumber *nodeAddress in self.allAddressArray) {
        if (![self isMeshOTAing]) {
            return;
        }
        if ([self.allAddressArray indexOfObject:nodeAddress] == self.successActionInCurrentProgress) {
            SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:nodeAddress.intValue];
            if (node.state != DeviceStateOutOfLine) {
                UInt16 blockSize = pow(2, self.blockSizeLog);
                UInt16 currentBlockSize = blockSize;
                if (self.allBlockCount - 1 <= self.blockIndex) {
                    //last block
                    currentBlockSize = self.otaData.length - blockSize * self.blockIndex;
                }
                self.currentBlockData = [self.otaData subdataWithRange:NSMakeRange(blockSize * self.blockIndex, currentBlockSize)];
                UInt32 blockChecksum = [LibTools getCRC32OfData:self.currentBlockData];
                NSData *blockChecksumValue = [NSData dataWithBytes:&blockChecksum length:4];
                TeLogInfo(@"blockChecksum=0x%x,blockChecksumValue=%@",blockChecksum,blockChecksumValue);
                self.semaphore = dispatch_semaphore_create(0);
                self.messageHandle = [SDKLibCommand objectBlockTransferStartWithDestination:node.address objectID:self.objectID blockNumber:self.blockIndex chunkSize:kChunkSize blockChecksumAlgorithm:SigBlockChecksumAlgorithmType_CRC32 blockChecksumValue:blockChecksumValue currentBlockSize:currentBlockSize resMax:1 retryCount:2 successCallback:^(UInt16 source, UInt16 destination, SigObjectBlockTransferStatus * _Nonnull responseMessage) {
                    if (source == nodeAddress.intValue) {
                        TeLogDebug(@"objectBlockTransferStart=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                        if (responseMessage.status == SigObjectBlockTransferStatusType_accepted || responseMessage.status == SigObjectBlockTransferStatusType_alreadyRX) {
                            weakSelf.successActionInCurrentProgress ++;
                        } else {
                            hasFail = YES;
                            weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in objectBlockTransferStart,SigObjectBlockTransferStatus.status=0x%x",responseMessage.status] code:-weakSelf.meshOTAProgress userInfo:nil];
                        }
                    } else {
                        hasFail = YES;
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    [NSThread sleepForTimeInterval:0.2];
                        dispatch_semaphore_signal(weakSelf.semaphore);
                }];
                //Most provide 3 seconds to objectBlockTransferStart every node.
                dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
            } else {
                self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in objectBlockTransferStart, node is outOfLine"] code:-weakSelf.meshOTAProgress userInfo:nil];
                hasFail = YES;
                break;
            }
        } else {
            if (!self.failError) {
                self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in objectBlockTransferStart, objectBlockTransferStart is timeout."] code:-weakSelf.meshOTAProgress userInfo:nil];
            }
            hasFail = YES;
            break;
        }
    }
    if (hasFail) {
        [self objectBlockTransferStartFailAction];
    } else {
        [self objectBlockTransferStartSuccessAction];
    }
}

- (void)objectBlockTransferStartSuccessAction {
    [self objectChunkTransfer];
}

- (void)objectBlockTransferStartFailAction {
    [self meshOTAFailAction];
}

#pragma mark step8:objectChunkTransfer
//做法一:默认只发送一次，设备端都可以收到。
- (void)objectChunkTransfer {
    TeLogInfo(@"\n\n\n\n==========step8\n\n\n\n");
    self.chunksCountofCurrentBlock = ceil(self.currentBlockData.length / (double)kChunkSize);
    self.meshOTAProgress = SigMeshOTAProgressObjectChunkTransfer;
    __block BOOL hasFail = NO;
    self.successActionInCurrentProgress = 0;

    for (int i = 0; i < self.chunksCountofCurrentBlock; i ++) {
        if (![self isMeshOTAing]) {
            return;
        }
        if (i == self.successActionInCurrentProgress) {
            self.chunkIndex = i;
            NSData *chunkData = nil;
            if (i == self.chunksCountofCurrentBlock - 1) {
                //end chunk of current block
                chunkData = [self.currentBlockData subdataWithRange:NSMakeRange(kChunkSize * i, self.currentBlockData.length - kChunkSize * i)];
            } else {
                chunkData = [self.currentBlockData subdataWithRange:NSMakeRange(kChunkSize * i, kChunkSize)];
            }
            __weak typeof(self) weakSelf = self;
            self.semaphore = dispatch_semaphore_create(0);
            TeLogInfo(@"all Block count=%d,current block index=%d,all chunk count=%d,current chunk index=%d ",self.allBlockCount,self.blockIndex,self.chunksCountofCurrentBlock,self.chunkIndex);
            [self showMeshOTAProgressWithCurrentChunkData:chunkData];
            self.messageHandle = [SDKLibCommand objectChunkTransferWithDestination:kMeshOTAGroupAddress chunkNumber:self.chunkIndex firmwareImageData:chunkData resMax:0 retryCount:0 resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                if (error) {
                    hasFail = YES;
                    weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in objectChunkTransfer, error=%@.",error.domain] code:-weakSelf.meshOTAProgress userInfo:nil];
                } else {
                    weakSelf.successActionInCurrentProgress ++;

                }
                [NSThread sleepForTimeInterval:0.2];
                        dispatch_semaphore_signal(weakSelf.semaphore);
            }];
            //Most provide 1000 seconds for objectChunkTransferWithDestination in every chunk.
            dispatch_semaphore_wait(self.semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 1000.0));
        } else {
            if (!self.failError) {
                self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in objectBlockTransferStart, objectBlockTransferStart is timeout."] code:-self.meshOTAProgress userInfo:nil];
            }
            hasFail = YES;
            break;
        }
    }
    if (hasFail) {
        [self objectChunkTransferFailAction];
    } else {
        [self objectChunkTransferSuccessFinishAction];
    }
}

- (void)objectChunkTransferWithLosePackets {
    TeLogInfo(@"\n\n\n\n==========step8.1\n\n\n\n");
    self.meshOTAProgress = SigMeshOTAProgressObjectChunkTransfer;
    __block BOOL hasFail = NO;
    self.successActionInCurrentProgress = 0;
    if (self.losePacketsDict && self.losePacketsDict.allKeys.count > 0) {
        for (NSNumber *addressNumber in self.losePacketsDict.allKeys) {
            UInt16 destination = (UInt16)addressNumber.intValue;
            NSArray *loaeChunkIndexs = self.losePacketsDict[addressNumber];
            for (NSNumber *chunkIndexNumber in loaeChunkIndexs) {
                UInt16 chunkIndex = (UInt16)chunkIndexNumber.intValue;
                self.chunkIndex = chunkIndex;
                NSData *chunkData = nil;
                if (chunkIndex == self.chunksCountofCurrentBlock - 1) {
                    //end chunk of current block
                    chunkData = [self.currentBlockData subdataWithRange:NSMakeRange(kChunkSize * chunkIndex, self.currentBlockData.length - kChunkSize * chunkIndex)];
                } else {
                    chunkData = [self.currentBlockData subdataWithRange:NSMakeRange(kChunkSize * chunkIndex, kChunkSize)];
                }
                __weak typeof(self) weakSelf = self;
                self.semaphore = dispatch_semaphore_create(0);
                TeLogInfo(@"all Block count=%d,current block index=%d,destination = 0x%x,all chunk count=%d,current chunk index=%d ",self.allBlockCount,self.blockIndex,destination,self.chunksCountofCurrentBlock,self.chunkIndex);
                [self showMeshOTAProgressWithCurrentChunkData:chunkData];
                self.messageHandle = [SDKLibCommand objectChunkTransferWithDestination:destination chunkNumber:self.chunkIndex firmwareImageData:chunkData resMax:0 retryCount:0 resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    if (error) {
                        hasFail = YES;
                        weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in objectChunkTransfer, error=%@.",error.domain] code:-weakSelf.meshOTAProgress userInfo:nil];
                    } else {
                        weakSelf.successActionInCurrentProgress ++;

                    }
                    [NSThread sleepForTimeInterval:0.2];
                            dispatch_semaphore_signal(weakSelf.semaphore);
                }];
                //Most provide 1000 seconds for objectChunkTransferWithDestination in every chunk.
                dispatch_semaphore_wait(self.semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 1000.0));
            }
        }
    }
    if (hasFail) {
        [self objectChunkTransferFailAction];
    } else {
        [self objectChunkTransferSuccessFinishAction];
    }
}


- (void)objectChunkTransferSuccessFinishAction {
    [self objectBlockGet];
}

- (void)objectChunkTransferFailAction {
    [self meshOTAFailAction];
}

- (void)showMeshOTAProgressWithCurrentChunkData:(NSData *)chunkData {
    if (self.progressBlock) {
        UInt16 blockSize = pow(2, self.blockSizeLog);
        double progress = (self.blockIndex * blockSize + self.chunkIndex * kChunkSize) / (double)self.otaData.length * 100;
        NSInteger intPro = (NSInteger)progress;
        TeLogDebug(@"progress=%f,intPro=%ld",progress,(long)intPro);
        self.progressBlock(intPro);
    }
}

#pragma mark step9:objectBlockGet
- (void)objectBlockGet {
    TeLogInfo(@"\n\n\n\n==========step9\n\n\n\n");
    self.losePacketsDict = [NSMutableDictionary dictionary];
    self.meshOTAProgress = SigMeshOTAProgressObjectBlockGet;
    __block BOOL hasFail = NO;
    self.successActionInCurrentProgress = 0;
    __weak typeof(self) weakSelf = self;
    for (NSNumber *nodeAddress in self.allAddressArray) {
        if (![self isMeshOTAing]) {
            return;
        }
        if ([self.allAddressArray indexOfObject:nodeAddress] == self.successActionInCurrentProgress) {
            SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:nodeAddress.intValue];
            if (node.state != DeviceStateOutOfLine) {
                self.semaphore = dispatch_semaphore_create(0);
                TeLogInfo(@"self.blockIndex=%d",self.blockIndex);
                self.messageHandle = [SDKLibCommand objectBlockGetWithDestination:node.address objectID:self.objectID blockNumber:self.blockIndex resMax:1 retryCount:2 successCallback:^(UInt16 source, UInt16 destination, SigObjectBlockStatus * _Nonnull responseMessage) {
                    if (source == nodeAddress.intValue) {
                        TeLogDebug(@"objectBlockGet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                        if (responseMessage.status == SigObjectBlockTransferStatusType_accepted) {
                            weakSelf.successActionInCurrentProgress ++;
                        } else if (responseMessage.status == SigObjectBlockTransferStatusType_alreadyRX) {
                            hasFail = YES;
                            NSString *errorString = [NSString stringWithFormat:@"fail in objectBlockGet,SigObjectBlockStatus.status=0x%x,blockIndex=%d",responseMessage.status,weakSelf.blockIndex];
                            weakSelf.failError = [NSError errorWithDomain:errorString code:-weakSelf.meshOTAProgress userInfo:nil];
                            TeLogInfo(@"%@",errorString);
                            self.losePacketsDict[@(source)] = responseMessage.missingChunksList;
                        } else {
                            hasFail = YES;
                            NSString *errorString = [NSString stringWithFormat:@"fail in objectBlockGet,SigObjectBlockStatus.status=0x%x,blockIndex=%d",responseMessage.status,weakSelf.blockIndex];
                            weakSelf.failError = [NSError errorWithDomain:errorString code:-weakSelf.meshOTAProgress userInfo:nil];
                            TeLogInfo(@"%@",errorString);
                        }
                    } else {
                        hasFail = YES;
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    [NSThread sleepForTimeInterval:0.2];
                        dispatch_semaphore_signal(weakSelf.semaphore);
                }];
                //Most provide 3 seconds to objectBlockTransferStart every node.
                dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
            } else {
                self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in objectBlockGet, node is outOfLine"] code:-weakSelf.meshOTAProgress userInfo:nil];
                hasFail = YES;
                break;
            }
        } else {
            if (!self.failError) {
                self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in objectBlockGet, objectBlockGet is timeout."] code:-self.meshOTAProgress userInfo:nil];
            }
            hasFail = YES;
            break;
        }
    }
    if (hasFail) {
        if (self.losePacketsDict && self.losePacketsDict.allKeys.count > 0) {
            [self objectChunkTransferWithLosePackets];
        } else {
            [self objectBlockGetFailAction];
        }
    } else {
        [self objectBlockGetSuccessAction];
    }
}

- (void)objectBlockGetSuccessAction {
    if (self.blockIndex < self.allBlockCount - 1) {
        // send next block data
        self.blockIndex ++;
        self.retryCountInObjectChunkTransfer = 0;
        [self objectBlockTransferStart];
    } else if (self.blockIndex == self.allBlockCount - 1) {
        // all blcoks had send
        [self firmwareUpdateGet];
    } else {
        self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in objectBlockGet, self.blockIndex > self.allBlockCount - 1."] code:-self.meshOTAProgress userInfo:nil];
        [self meshOTAFailAction];
    }
}

- (void)objectBlockGetFailAction {
    if (self.retryCountInObjectChunkTransfer < kRetryCountInObjectChunkTransfer) {
        TeLogInfo(@"retry send block.");
        self.retryCountInObjectChunkTransfer ++;
        [self objectBlockTransferStart];
    } else {
        TeLogInfo(@"send block fail.");
        [self meshOTAFailAction];
    }
}

#pragma mark step10:firmwareUpdateGet
- (void)firmwareUpdateGet {
    TeLogInfo(@"\n\n\n\n==========step10\n\n\n\n");
    self.meshOTAProgress = SigMeshOTAProgressFirmwareUpdateGet;
    __block BOOL hasFail = NO;
    self.successActionInCurrentProgress = 0;
    __weak typeof(self) weakSelf = self;
    for (NSNumber *nodeAddress in self.allAddressArray) {
        if (![self isMeshOTAing]) {
            return;
        }
        if ([self.allAddressArray indexOfObject:nodeAddress] == self.successActionInCurrentProgress) {
            SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:nodeAddress.intValue];
            if (node.state != DeviceStateOutOfLine) {
                self.semaphore = dispatch_semaphore_create(0);
                self.messageHandle = [SDKLibCommand firmwareUpdateGetWithDestination:node.address companyID:self.companyID firmwareID:self.firmwareIDData resMax:1 retryCount:2 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareUpdateStatus * _Nonnull responseMessage) {
                    if (source == nodeAddress.intValue) {
                        TeLogDebug(@"firmwareUpdateGet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                        if (responseMessage.status == SigFirmwareUpdateStatusType_success) {
                            if (responseMessage.phase == SigFirmwareUpdatePhaseType_DFUReady) {
                                if (!responseMessage.additionalInformation.ProvisioningNeeded) {
                                    weakSelf.successActionInCurrentProgress ++;
                                } else {
                                    hasFail = YES;
                                    weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateGet,responseMessage.additionalInformation.ProvisioningNeeded=0x%x",responseMessage.additionalInformation.ProvisioningNeeded] code:-weakSelf.meshOTAProgress userInfo:nil];
                                }
                            } else {
                                hasFail = YES;
                                weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateGet,SigFirmwareUpdateStatus.phase=0x%x",responseMessage.phase] code:-weakSelf.meshOTAProgress userInfo:nil];
                            }
                        } else {
                            hasFail = YES;
                            weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateGet,SigFirmwareUpdateStatus.status=0x%x",responseMessage.status] code:-weakSelf.meshOTAProgress userInfo:nil];
                        }
                    } else {
                        hasFail = YES;
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    [NSThread sleepForTimeInterval:0.2];
                        dispatch_semaphore_signal(weakSelf.semaphore);
                }];
                //Most provide 3 seconds to objectBlockTransferStart every node.
                dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
            } else {
                self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateGet, node is outOfLine"] code:-weakSelf.meshOTAProgress userInfo:nil];
                hasFail = YES;
                break;
            }
        } else {
            if (!self.failError) {
                self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateGet, firmwareUpdateGet is timeout."] code:-self.meshOTAProgress userInfo:nil];
            }
            hasFail = YES;
            break;
        }
    }
    if (hasFail) {
        [self firmwareUpdateGetFailAction];
    } else {
        [self firmwareUpdateGetSuccessAction];
    }
}

- (void)firmwareUpdateGetSuccessAction {
    [self firmwareUpdateApply];
}

- (void)firmwareUpdateGetFailAction {
    [self meshOTAFailAction];
}

#pragma mark step11:firmwareUpdateApply
- (void)firmwareUpdateApply {
    TeLogInfo(@"\n\n\n\n==========step11\n\n\n\n");
    self.meshOTAProgress = SigMeshOTAProgressFirmwareUpdateApply;
    __block BOOL hasFail = NO;
    self.successActionInCurrentProgress = 0;
    __weak typeof(self) weakSelf = self;
    for (NSNumber *nodeAddress in self.allAddressArray) {
        if (![self isMeshOTAing]) {
            return;
        }
        if ([self.allAddressArray indexOfObject:nodeAddress] == self.successActionInCurrentProgress) {
            SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:nodeAddress.intValue];
            if (node.state != DeviceStateOutOfLine) {
                self.semaphore = dispatch_semaphore_create(0);
                self.messageHandle = [SDKLibCommand firmwareUpdateApplyWithDestination:node.address companyID:self.companyID firmwareID:self.firmwareIDData resMax:1 retryCount:2 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareUpdateStatus * _Nonnull responseMessage) {
                    if (source == nodeAddress.intValue) {
                        TeLogDebug(@"firmwareUpdateApply=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                        if (responseMessage.status == SigFirmwareUpdateStatusType_success) {
                            if (responseMessage.phase == SigFirmwareUpdatePhaseType_idle) {
                                if (!responseMessage.additionalInformation.ProvisioningNeeded) {
                                    weakSelf.successActionInCurrentProgress ++;
                                    // apply 成功，则标记meshOTA成功。
                                    [weakSelf callBackMeshOTASuccessAddress:node.address];
                                } else {
                                    hasFail = YES;
                                    weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateApply,responseMessage.additionalInformation.ProvisioningNeeded=0x%x",responseMessage.additionalInformation.ProvisioningNeeded] code:-weakSelf.meshOTAProgress userInfo:nil];
                                }
                            } else {
                                hasFail = YES;
                                weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateApply,SigFirmwareUpdateStatus.phase=0x%x",responseMessage.phase] code:-weakSelf.meshOTAProgress userInfo:nil];
                            }
                        } else {
                            hasFail = YES;
                            weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateApply,SigFirmwareUpdateStatus.status=0x%x",responseMessage.status] code:-weakSelf.meshOTAProgress userInfo:nil];
                        }
                    } else {
                        hasFail = YES;
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    [NSThread sleepForTimeInterval:0.2];
                        dispatch_semaphore_signal(weakSelf.semaphore);
                }];
                //Most provide 3 seconds to objectBlockTransferStart every node.
                dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
            } else {
                self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateApply, node is outOfLine"] code:-weakSelf.meshOTAProgress userInfo:nil];
                hasFail = YES;
                break;
            }
        } else {
            if (!self.failError) {
                self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateApply, firmwareUpdateApply is timeout."] code:-self.meshOTAProgress userInfo:nil];
            }
            hasFail = YES;
            break;
        }
    }
    if (hasFail) {
        [self firmwareUpdateApplyFailAction];
    } else {
        [self firmwareUpdateApplySuccessAction];
    }
}

- (void)firmwareUpdateApplySuccessAction {
    [self firmwareDistributionStop];
}

- (void)firmwareUpdateApplyFailAction {
    [self meshOTAFailAction];
}

#pragma mark step12:firmwareDistributionStop
- (void)firmwareDistributionStop {
    TeLogInfo(@"\n\n\n\n==========step12\n\n\n\n");
    __weak typeof(self) weakSelf = self;
    self.messageHandle = [SDKLibCommand firmwareDistributionStopWithDestination:0x01 companyID:self.companyID firmwareID:self.firmwareIDData resMax:0 retryCount:0 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareDistributionStatus * _Nonnull responseMessage) {
        TeLogDebug(@"firmwareDistributionStop=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
        [weakSelf firmwareDistributionStopSuccessAction];
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
    }];
    
    [self showMeshOTAResult];
}

- (void)firmwareDistributionStopSuccessAction {
    
}

- (void)firmwareDistributionStopFailAction {
    [self meshOTAFailAction];
}

#pragma mark meshOTA fail
- (void)meshOTAFailAction {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self];
    });
    if (self.errorBlock) {
        if (!self.failError) {
            self.failError = [NSError errorWithDomain:@"Mesh OTA fail" code:-1 userInfo:nil];
        }
        self.errorBlock(self.failError);
    }
    [self stopMeshOTA];
}

-(void)dealloc{
    NSLog(@"%s",__func__);
}

#pragma  mark - SigBearerDataDelegate
- (void)bearer:(SigBearer *)bearer didCloseWithError:(NSError *)error {
    TeLogVerbose(@"");
    if (self.isMeshOTAing) {
        [self meshOTAFailAction];
    }
    
}

@end
