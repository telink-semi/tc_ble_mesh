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
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2018/4/24.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "MeshOTAManager.h"

//每一个步骤的超时时间，默认是60.0s
#define kTimeOutOfEveryStep (dispatch_semaphore_wait(weakSelf.semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 60.0)))
//block大小，为10的kBlockSizeLog次幂，默认是10^12=4096字节
#define kBlockSizeLog   (0x0C)
#define kChunkSize      (0x0100)
#define kRetryCountInBLOBChunkTransfer    (3)
#define kMeshOTAGroupAddress    (0xc000)

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

//========== R04 ==========//
@property (nonatomic, assign) UInt16 distributionAppKeyIndex;
@property (nonatomic, assign) SigTransferModeState distributionTransferMode;
@property (nonatomic, assign) BOOL updatePolicy;
/// Multicast address used in a firmware image distribution. Size is 16 bits or 128 bits.
@property (nonatomic,strong) NSData *distributionMulticastAddress;
@property (nonatomic, assign) UInt16 distributionFirmwareImageIndex;
@property (nonatomic, assign) UInt8 incomingFirmwareMetadataLength;
@property (nonatomic, strong) NSData *incomingFirmwareMetadata;
@property (nonatomic, assign) UInt16 index;
@property (nonatomic, assign) UInt8 updateTTL;
@property (nonatomic, assign) UInt16 updateTimeoutBase;
@property (nonatomic, assign) UInt64 updateBLOBID;
@property (nonatomic, assign) UInt16 MTUSize;
@property (nonatomic, assign) UInt16 timeout;
// test model
@property (nonatomic, assign) BOOL testFinish;
//========== R04 ==========//

@property (nonatomic, strong) NSMutableArray <NSNumber *>*allAddressArray;//Mesh OTA的所有短地址列表
@property (nonatomic, strong) NSMutableArray <NSNumber *>*successAddressArray;//Mesh OTA成功的短地址列表
@property (nonatomic, strong) NSMutableArray <NSNumber *>*failAddressArray;//Mesh OTA失败的短地址列表（包括中途失败）

@property (nonatomic, assign) SigMeshOTAProgress meshOTAProgress;//记录meshOTA当前的进度
@property (nonatomic, assign) UInt8 allBlockCount;//记录block总个数
@property (nonatomic, assign) UInt8 blockIndex;//记录当前block的index
@property (nonatomic, strong) NSData *currentBlockData;//记录当前block的data
@property (nonatomic, assign) UInt8 chunksCountofCurrentBlock;//记录当前block的chunk总个数
@property (nonatomic, assign) UInt8 chunkIndex;//记录当前chunk的index
@property (nonatomic, assign) UInt8 successActionInCurrentProgress;//记录当前阶段成功的设备个数
@property (nonatomic, strong) NSMutableDictionary <NSNumber *, NSArray *>*losePacketsDict;//step10阶段传输失败的包。
@property (nonatomic, strong) NSMutableDictionary <NSNumber *, NSDictionary *>*oFirmwareInformations;//记录MeshOTA前设备的版本号,[@(UInt16):@{@"cid":@(UInt16),@"pid":@(UInt16),@"vid":@(UInt16)}]
@property (nonatomic, strong) NSMutableDictionary <NSNumber *, NSNumber *>*nFirmwareInformations;//记录MeshOTA后设备的版本号,[@(UInt16):@{@"cid":@(UInt16),@"pid":@(UInt16),@"vid":@(UInt16)}]
@property (nonatomic, strong) NSMutableDictionary <NSNumber *, SigBLOBInformationStatus *>*oBLOBInformations;//记录MeshOTA前设备的BLOBInformation
@property (nonatomic, assign) UInt8 retryCountInBLOBChunkTransfer;//记录step10:BLOBChunkTransfer阶段已经重试的次数
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
    _failAddressArray = [NSMutableArray array];
    UInt32 firmwareID = 0xff000021;
    _firmwareIDData = [NSData dataWithBytes:&firmwareID length:4];
    _objectID = 0x8877665544332211;
    _companyID = 0x0211;
    
    // config parameters for meshOTA R04
    _distributionAppKeyIndex = SigDataSource.share.curAppkeyModel.index;
    _distributionTransferMode = SigTransferModeState_pushBLOBTransferMode;
    _updatePolicy = NO;
    _distributionFirmwareImageIndex = 0;
    UInt16 gAddress = kMeshOTAGroupAddress;
    _distributionMulticastAddress = [NSData dataWithBytes:&gAddress length:2];
    _incomingFirmwareMetadataLength = 4;
    UInt32 tem32 = 0;
    _incomingFirmwareMetadata = [NSData dataWithBytes:&tem32 length:4];
    _index = 0;
    _updateTTL = 0xFF;
    _updateTimeoutBase = 0;
    _updateBLOBID = 0x8877665544332211;
    _MTUSize = 380;
    _timeout = 0;
    
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
    if (self.semaphore) {
        dispatch_semaphore_signal(self.semaphore);
    }
    [self.meshOTAThread cancel];
    [SigMeshLib.share cancelSigMessageHandle:self.messageHandle];

//    if (SigBearer.share.isOpen) {
//        self.messageHandle = [SDKLibCommand firmwareDistributionCancelWithDestination:SigDataSource.share.getCurrentConnectedNode.address retryCount:0 responseMaxCount:0 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareDistributionStatus * _Nonnull responseMessage) {
//            TeLogDebug(@"firmwareDistributionCancel=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
//        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
//            TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
//        }];
//    }
    
    [SigBearer.share stopMeshConnectWithComplete:^(BOOL successful) {
        TeLogInfo(@"断开连接:%@",successful?@"成功":@"失败");
    }];
}

- (void)cancelSignal {
    dispatch_semaphore_signal(self.semaphore);
}

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
            NSArray *allAddressArray = [NSArray arrayWithArray:self.allAddressArray];
            for (NSNumber *tem in allAddressArray) {
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
    [self.failAddressArray removeAllObjects];
    [self.allAddressArray addObjectsFromArray:deviceAddresses];
    TeLogInfo(@"self.allAddressArray=%@",self.allAddressArray);
    self.testFinish = otaData.length > 1024*10;
    self.oldBearerDataDelegate = SigBearer.share.dataDelegate;
    SigBearer.share.dataDelegate = self;
    _companyID = cid;

    NSMutableArray *rssiArray = [NSMutableArray array];
    __weak typeof(self) weakSelf = self;
    [SDKLibCommand genericOnOffGetWithDestination:kMeshAddress_allNodes retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:SigDataSource.share.getOnlineDevicesNumber successCallback:^(UInt16 source, UInt16 destination, SigGenericOnOffStatus * _Nonnull responseMessage) {
        if (![rssiArray containsObject:@(source)] && [weakSelf.allAddressArray containsObject:@(source)]) {
            [rssiArray addObject:@(source)];
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        //1.安装回包顺序，越先返回状态的地址越后操作，未返回状态的地址再最前面。
        for (int i=0; i<rssiArray.count; i++) {
            NSNumber *rssiSource = rssiArray[rssiArray.count - 1 - i];
            [self.allAddressArray removeObject:rssiSource];
            [self.allAddressArray addObject:rssiSource];
        }
        //2.直连地址应该处于数组最后的位置。
        if ([self.allAddressArray containsObject:@(SigDataSource.share.unicastAddressOfConnected)]) {
            [self.allAddressArray removeObject:@(SigDataSource.share.unicastAddressOfConnected)];
            [self.allAddressArray addObject:@(SigDataSource.share.unicastAddressOfConnected)];
        }
        TeLogInfo(@"self.allAddressArray=%@",self.allAddressArray);
        
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
        [self performSelector:@selector(firmwareDistributionStart) onThread:self.meshOTAThread withObject:nil waitUntilDone:NO];

    }];
}

#pragma mark step1:firmwareDistributionStart
- (void)firmwareDistributionStart {
    self.meshOTAProgress = SigMeshOTAProgressFirmwareDistributionStart;
    TeLogInfo(@"\n\n==========meshOTA:step%d\n\n",self.meshOTAProgress);

    if (self.progressBlock) {
        self.progressBlock(0);
    }
//    __block BOOL hasFail = NO;
//    self.successActionInCurrentProgress = 0;
//    __weak typeof(self) weakSelf = self;
//
//    //做法1
//    self.semaphore = dispatch_semaphore_create(0);
//    [SDKLibCommand firmwareDistributionStartWithDestination:SigDataSource.share.curLocationNodeModel.address distributionAppKeyIndex:self.distributionAppKeyIndex distributionTTL:self.updateTTL distributionTimeoutBase:self.updateTimeoutBase distributionTransferMode:self.distributionTransferMode updatePolicy:self.updatePolicy RFU:0 distributionFirmwareImageIndex:self.distributionFirmwareImageIndex distributionMulticastAddress:self.distributionMulticastAddress retryCount:0 responseMaxCount:0 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareDistributionStatus * _Nonnull responseMessage) {
//        TeLogDebug(@"firmwareDistributionStart=%@,source=0x%x,destination=0x%x",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
//    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
//        TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
//        dispatch_semaphore_signal(weakSelf.semaphore);
//    }];
//    //Most provide 3 seconds to firmwareUpdateInformationGet every node.
//    dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
//    if (hasFail) {
//        [self firmwareDistributionStartFailAction];
//    } else {
        [self firmwareDistributionStartSuccessAction];
//    }
}

- (void)firmwareDistributionStartSuccessAction {
    [self startConfigModelSubscriptionAdd];
}

- (void)firmwareDistributionStartFailAction {
    [self meshOTAFailAction];
}

#pragma mark step2:configModelSubscriptionAdd
- (void)startConfigModelSubscriptionAdd {
    self.meshOTAProgress = SigMeshOTAProgressSubscriptionAdd;
    TeLogInfo(@"\n\n==========meshOTA:step%d\n\n",self.meshOTAProgress);

    __block BOOL hasSuccess = NO;
    __weak typeof(self) weakSelf = self;
    NSArray *allAddressArray = [NSArray arrayWithArray:self.allAddressArray];
    for (NSNumber *nodeAddress in allAddressArray) {
        if (![self isMeshOTAing]) {
            return;
        }
        if ([self.failAddressArray containsObject:nodeAddress]) {
            continue;
        }
        SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:nodeAddress.intValue];
        if (node.state != DeviceStateOutOfLine) {
            UInt16 modelIdentifier = SIG_MD_BLOB_TRANSFER_S;
            UInt16 companyIdentifier = 0;
            UInt16 groupAddress = kMeshOTAGroupAddress;
            NSArray *addressArray = [node getAddressesWithModelID:@(modelIdentifier)];
            if (addressArray && addressArray.count > 0) {
                UInt16 eleAddress = [addressArray.firstObject intValue];
                self.semaphore = dispatch_semaphore_create(0);
                self.messageHandle = [SDKLibCommand configModelSubscriptionAddWithDestination:node.address toGroupAddress:groupAddress elementAddress:eleAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigModelSubscriptionStatus * _Nonnull responseMessage) {
                    if (responseMessage.elementAddress == eleAddress && responseMessage.address == groupAddress) {
                        TeLogDebug(@"configModelSubscriptionAdd=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                        if (responseMessage.companyIdentifier == companyIdentifier && responseMessage.modelIdentifier == modelIdentifier) {
                            if (responseMessage.status == SigConfigMessageStatus_success) {
                                hasSuccess = YES;
                            } else {
                                weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in configModelSubscriptionAdd,SigConfigModelSubscriptionStatus.status=0x%x",responseMessage.status] code:-weakSelf.meshOTAProgress userInfo:nil];
                                TeLogError(@"fail in configModelSubscriptionAdd,SigConfigModelSubscriptionStatus.status=0x%x",responseMessage.status);
                                if (![weakSelf.failAddressArray containsObject:nodeAddress]) {
                                    [weakSelf.failAddressArray addObject:nodeAddress];
                                }
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
                if (![self.failAddressArray containsObject:nodeAddress]) {
                    [self.failAddressArray addObject:nodeAddress];
                }
           }
        } else {
            self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in configModelSubscriptionAdd, node is outOfLine"] code:-weakSelf.meshOTAProgress userInfo:nil];
            if (![self.failAddressArray containsObject:nodeAddress]) {
                [self.failAddressArray addObject:nodeAddress];
            }
        }
    }
    if (hasSuccess) {
        [self configModelSubscriptionAddSuccessAction];
    } else {
        [self configModelSubscriptionAddFailAction];
    }
}

- (void)configModelSubscriptionAddSuccessAction {
    [self firmwareUpdateInformationGet];
}

- (void)configModelSubscriptionAddFailAction {
    [self meshOTAFailAction];
}

#pragma mark step3:firmwareUpdateInformationGet
- (void)firmwareUpdateInformationGet {
    self.meshOTAProgress = SigMeshOTAProgressFirmwareUpdateInformationGet;
    TeLogInfo(@"\n\n==========meshOTA:step%d\n\n",self.meshOTAProgress);

    __block BOOL hasSuccess = NO;
    self.oFirmwareInformations = [NSMutableDictionary dictionary];
    __weak typeof(self) weakSelf = self;
    NSArray *allAddressArray = [NSArray arrayWithArray:self.allAddressArray];
    for (NSNumber *nodeAddress in allAddressArray) {
        if (![self isMeshOTAing]) {
            return;
        }
        if ([self.failAddressArray containsObject:nodeAddress]) {
            continue;
        }
        SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:nodeAddress.intValue];
        if (node.state != DeviceStateOutOfLine) {
            self.semaphore = dispatch_semaphore_create(0);
            self.messageHandle = [SDKLibCommand firmwareUpdateInformationGetWithDestination:node.address firstIndex:0 entriesLimit:1 retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareUpdateInformationStatus * _Nonnull responseMessage) {
                if (source == nodeAddress.intValue && responseMessage.firmwareInformationListCount > 0) {
                    /*
                     responseMessage.firmwareInformationList.firstObject.currentFirmwareID.length = 4: 2 bytes pid(设备类型) + 2 bytes vid(版本id).
                     */
                    if (responseMessage.firmwareInformationList.count > 0) {
                        NSData *currentFirmwareID = responseMessage.firmwareInformationList.firstObject.currentFirmwareID;
                        UInt16 pid = 0,vid = 0;
                        Byte *pu = (Byte *)[currentFirmwareID bytes];
                        if (currentFirmwareID.length >= 2) memcpy(&pid, pu, 2);
                        if (currentFirmwareID.length >= 4) memcpy(&vid, pu + 2, 2);
                        TeLogDebug(@"firmwareUpdateInformationGet=%@,pid=%d,vid=%d",[LibTools convertDataToHexStr:currentFirmwareID],pid,vid);
                        weakSelf.oFirmwareInformations[@(source)] = @{kPid:@(pid),kVid:@(vid)};
                        hasSuccess = YES;
                    }
                } else {
                    if (![weakSelf.failAddressArray containsObject:nodeAddress]) {
                        [weakSelf.failAddressArray addObject:nodeAddress];
                    }
                }
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                [NSThread sleepForTimeInterval:0.2];
                dispatch_semaphore_signal(weakSelf.semaphore);
            }];
            //Most provide 3 seconds to firmwareUpdateInformationGet every node.
            dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
        } else {
            self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateInformationGet, node is outOfLine"] code:-weakSelf.meshOTAProgress userInfo:nil];
            if (![self.failAddressArray containsObject:nodeAddress]) {
                [self.failAddressArray addObject:nodeAddress];
            }
        }
    }
    
    if (hasSuccess) {
        [self firmwareUpdateInformationGetSuccessAction];
    } else {
        [self firmwareUpdateInformationGetFailAction];
    }
}

- (void)firmwareUpdateInformationGetSuccessAction {
    [self firmwareUpdateFirmwareMetadataCheck];
}

- (void)firmwareUpdateInformationGetFailAction {
    [self meshOTAFailAction];
}

#pragma mark step4:firmwareUpdateFirmwareMetadataCheck
- (void)firmwareUpdateFirmwareMetadataCheck {
    self.meshOTAProgress = SigMeshOTAProgressFirmwareUpdateFirmwareMetadataCheck;
    TeLogInfo(@"\n\n==========meshOTA:step%d\n\n",self.meshOTAProgress);

    __block BOOL hasSuccess = NO;
    __weak typeof(self) weakSelf = self;
    NSArray *allAddressArray = [NSArray arrayWithArray:self.allAddressArray];
    for (NSNumber *nodeAddress in allAddressArray) {
        if (![self isMeshOTAing]) {
            return;
        }
        if ([self.failAddressArray containsObject:nodeAddress]) {
            continue;
        }
        SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:nodeAddress.intValue];
        if (node.state != DeviceStateOutOfLine) {
            self.semaphore = dispatch_semaphore_create(0);
            TeLogInfo(@"firmwareUpdateFirmwareMetadataCheckWithDestination=0x%x",node.address);
            self.messageHandle = [SDKLibCommand firmwareUpdateFirmwareMetadataCheckWithDestination:node.address updateFirmwareImageIndex:self.index incomingFirmwareMetadata:self.incomingFirmwareMetadata retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareUpdateFirmwareMetadataStatus * _Nonnull responseMessage) {
                TeLogDebug(@"firmwareUpdateFirmwareMetadataCheck=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                if (source == nodeAddress.intValue) {
                    if (responseMessage.status == SigFirmwareUpdateServerAndClientModelStatusType_success) {
//                            if (responseMessage.additionalInformation == SigFirmwareUpdateAdditionalInformationStatusType_noChangeCompositionData) {
                            hasSuccess = YES;
//                            } else {
//                                hasFail = YES;
//                                weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateFirmwareMetadataCheck,SigFirmwareUpdateStatus.phase=0x%x",responseMessage.phase] code:-weakSelf.meshOTAProgress userInfo:nil];
//                            }
                    } else {
                        weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateFirmwareMetadataCheck,SigFirmwareUpdateStatus.status=0x%x",responseMessage.status] code:-weakSelf.meshOTAProgress userInfo:nil];
                        if (![weakSelf.failAddressArray containsObject:nodeAddress]) {
                            [weakSelf.failAddressArray addObject:nodeAddress];
                        }
                    }
                } else {
                    if (![weakSelf.failAddressArray containsObject:nodeAddress]) {
                        [weakSelf.failAddressArray addObject:nodeAddress];
                    }
                }
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                [NSThread sleepForTimeInterval:0.2];
                dispatch_semaphore_signal(weakSelf.semaphore);
            }];
            //Most provide 3 seconds to firmwareUpdateFirmwareMetadataCheck every node.
            dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
        } else {
            self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateFirmwareMetadataCheck, node is outOfLine"] code:-weakSelf.meshOTAProgress userInfo:nil];
            if (![self.failAddressArray containsObject:nodeAddress]) {
                [self.failAddressArray addObject:nodeAddress];
            }
        }
    }
    if (hasSuccess) {
        [self firmwareUpdateFirmwareMetadataCheckSuccessAction];
    } else {
        [self firmwareUpdateFirmwareMetadataCheckFailAction];
    }
}

- (void)firmwareUpdateFirmwareMetadataCheckSuccessAction {
    [self firmwareUpdateStart];
}

- (void)firmwareUpdateFirmwareMetadataCheckFailAction {
    [self meshOTAFailAction];
}

#pragma mark step5:firmwareUpdateStart
- (void)firmwareUpdateStart {
    self.meshOTAProgress = SigMeshOTAProgressFirmwareUpdateStart;
    TeLogInfo(@"\n\n==========meshOTA:step%d\n\n",self.meshOTAProgress);

    __block BOOL hasSuccess = NO;
    __weak typeof(self) weakSelf = self;
    NSArray *allAddressArray = [NSArray arrayWithArray:self.allAddressArray];
    for (NSNumber *nodeAddress in allAddressArray) {
        if (![self isMeshOTAing]) {
            return;
        }
        if ([self.failAddressArray containsObject:nodeAddress]) {
            continue;
        }
        SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:nodeAddress.intValue];
        if (node.state != DeviceStateOutOfLine) {
            self.semaphore = dispatch_semaphore_create(0);
            TeLogInfo(@"firmwareUpdateStartWithDestination=0x%x",node.address);
            self.messageHandle = [SDKLibCommand firmwareUpdateStartWithDestination:node.address updateTTL:self.updateTTL updateTimeoutBase:self.updateTimeoutBase updateBLOBID:self.updateBLOBID updateFirmwareImageIndex:self.index incomingFirmwareMetadata:self.incomingFirmwareMetadata retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareUpdateStatus * _Nonnull responseMessage) {
                TeLogDebug(@"firmwareUpdateStart=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                if (source == nodeAddress.intValue) {
                    if (responseMessage.status == SigFirmwareUpdateStatusType_success) {
//                            if (responseMessage.phase == SigFirmwareUpdatePhaseType_inProgress) {
//                                if (!responseMessage.additionalInformation.ProvisioningNeeded) {
                                hasSuccess = YES;
//                                } else {
//                                    hasFail = YES;
//                                    weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateStart,responseMessage.additionalInformation.ProvisioningNeeded=0x%x",responseMessage.additionalInformation.ProvisioningNeeded] code:-weakSelf.meshOTAProgress userInfo:nil];
//                                }
//                            } else {
//                                hasFail = YES;
//                                weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateStart,SigFirmwareUpdateStatus.phase=0x%x",responseMessage.phase] code:-weakSelf.meshOTAProgress userInfo:nil];
//                            }
                    } else {
                        weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateStart,SigFirmwareUpdateStatus.status=0x%x",responseMessage.status] code:-weakSelf.meshOTAProgress userInfo:nil];
                        if (![weakSelf.failAddressArray containsObject:nodeAddress]) {
                            [weakSelf.failAddressArray addObject:nodeAddress];
                        }
                    }
                } else {
                    if (![weakSelf.failAddressArray containsObject:nodeAddress]) {
                        [weakSelf.failAddressArray addObject:nodeAddress];
                    }
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
            if (![self.failAddressArray containsObject:nodeAddress]) {
                [self.failAddressArray addObject:nodeAddress];
            }
        }
    }
    if (hasSuccess) {
        [self firmwareUpdateStartSuccessAction];
    } else {
        [self firmwareUpdateStartFailAction];
    }
}

- (void)firmwareUpdateStartSuccessAction {
    [self BLOBTransferGet];
}

- (void)firmwareUpdateStartFailAction {
    [self meshOTAFailAction];
}

#pragma mark step6:BLOBTransferGet
- (void)BLOBTransferGet {
    self.meshOTAProgress = SigMeshOTAProgressBLOBTransferGet;
    TeLogInfo(@"\n\n==========meshOTA:step%d\n\n",self.meshOTAProgress);

    __block BOOL hasSuccess = NO;
    __weak typeof(self) weakSelf = self;
    NSArray *allAddressArray = [NSArray arrayWithArray:self.allAddressArray];
    for (NSNumber *nodeAddress in allAddressArray) {
        if (![self isMeshOTAing]) {
            return;
        }
        if ([self.failAddressArray containsObject:nodeAddress]) {
            continue;
        }
        SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:nodeAddress.intValue];
        if (node.state != DeviceStateOutOfLine) {
            self.semaphore = dispatch_semaphore_create(0);
            self.messageHandle = [SDKLibCommand BLOBTransferGetWithDestination:node.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigBLOBTransferStatus * _Nonnull responseMessage) {
                TeLogDebug(@"BLOBTransferGet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                if (source == nodeAddress.intValue) {
//                        if (responseMessage.status == SigBLOBTransferStatusType_busy) {
                        hasSuccess = YES;
//                        } else {
//                           hasFail = YES;
//                           weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in BLOBTransferGet,SigBLOBTransferStatus.status=0x%x",responseMessage.status] code:-weakSelf.meshOTAProgress userInfo:nil];
//                        }
                } else {
                    if (![weakSelf.failAddressArray containsObject:nodeAddress]) {
                        [weakSelf.failAddressArray addObject:nodeAddress];
                    }
                }
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                [NSThread sleepForTimeInterval:0.2];
                dispatch_semaphore_signal(weakSelf.semaphore);
            }];
            //Most provide 3 seconds to BLOBTransferGet every node.
            dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
        } else {
            self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in BLOBTransferGet, node is outOfLine"] code:-weakSelf.meshOTAProgress userInfo:nil];
            if (![self.failAddressArray containsObject:nodeAddress]) {
                [self.failAddressArray addObject:nodeAddress];
            }
        }
    }
    if (hasSuccess) {
        [self BLOBTransferGetSuccessAction];
    } else {
        [self BLOBTransferGetFailAction];
    }
}

- (void)BLOBTransferGetSuccessAction {
    [self BLOBInformationGet];
}

- (void)BLOBTransferGetFailAction {
    [self meshOTAFailAction];
}

#pragma mark step7:BLOBInformationGet
- (void)BLOBInformationGet {
    self.meshOTAProgress = SigMeshOTAProgressBLOBInformationGet;
    TeLogInfo(@"\n\n==========meshOTA:step%d\n\n",self.meshOTAProgress);

    __block BOOL hasSuccess = NO;
    self.oBLOBInformations = [NSMutableDictionary dictionary];
    __weak typeof(self) weakSelf = self;
    NSArray *allAddressArray = [NSArray arrayWithArray:self.allAddressArray];
    for (NSNumber *nodeAddress in allAddressArray) {
        if (![self isMeshOTAing]) {
            return;
        }
        if ([self.failAddressArray containsObject:nodeAddress]) {
            continue;
        }
        SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:nodeAddress.intValue];
        if (node.state != DeviceStateOutOfLine) {
            self.semaphore = dispatch_semaphore_create(0);
            self.messageHandle = [SDKLibCommand BLOBInformationGetWithDestination:node.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigBLOBInformationStatus * _Nonnull responseMessage) {
                TeLogDebug(@"BLOBInformationGet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                if (source == nodeAddress.intValue) {
                    weakSelf.oBLOBInformations[@(source)] = responseMessage;
                    hasSuccess = YES;
                } else {
                    if (![weakSelf.failAddressArray containsObject:nodeAddress]) {
                        [weakSelf.failAddressArray addObject:nodeAddress];
                    }
                }
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                [NSThread sleepForTimeInterval:0.2];
                    dispatch_semaphore_signal(weakSelf.semaphore);
            }];
            //Most provide 3 seconds to firmwareUpdateInformationGet every node.
            dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
        } else {
            self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in BLOBInformationGet, node is outOfLine"] code:-weakSelf.meshOTAProgress userInfo:nil];
            if (![self.failAddressArray containsObject:nodeAddress]) {
                [self.failAddressArray addObject:nodeAddress];
            }
        }
    }
    if (hasSuccess) {
        if (self.oBLOBInformations && self.oBLOBInformations.count > 0) {
            UInt8 minBlockSizeLog = self.oBLOBInformations.allValues.firstObject.minBlockSizeLog;
            UInt8 maxBlockSizeLog = self.oBLOBInformations.allValues.firstObject.maxBlockSizeLog;
            UInt16 maxChunksNumber = self.oBLOBInformations.allValues.firstObject.maxChunksNumber;
            NSArray *allValues = [NSArray arrayWithArray:self.oBLOBInformations.allValues];
            for (SigBLOBInformationStatus *message in allValues) {
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
                [self BLOBInformationGetSuccessAction];
            } else {
                //默认值不支持，则重新计算blockSizeLog
                if (minBlockSizeLog > maxBlockSizeLog) {
                    //无所有设备都支持的blockSizeLog值，meshOTA失败
                    self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in BLOBInformationGet, blockSizeLog is error."] code:-weakSelf.meshOTAProgress userInfo:nil];
                    [self BLOBInformationGetFailAction];
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
                        [self BLOBInformationGetSuccessAction];
                    } else {
                        //重新计算blockSizeLog失败
                        self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in BLOBInformationGet, blockSizeLog is error."] code:-weakSelf.meshOTAProgress userInfo:nil];
                        [self BLOBInformationGetFailAction];
                    }
                }
            }
        } else {
            self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in BLOBInformationGet, BLOBInformation is empty."] code:-weakSelf.meshOTAProgress userInfo:nil];
            [self BLOBInformationGetFailAction];
        }
    } else {
        [self BLOBInformationGetFailAction];
    }
}

- (void)BLOBInformationGetSuccessAction {
    [self BLOBTransferStart];
}

- (void)BLOBInformationGetFailAction {
    [self meshOTAFailAction];
}

#pragma mark step8:BLOBTransferStart
- (void)BLOBTransferStart {
    self.meshOTAProgress = SigMeshOTAProgressBLOBTransferStart;
    TeLogInfo(@"\n\n==========meshOTA:step%d\n\n",self.meshOTAProgress);

    __block BOOL hasSuccess = NO;
    __weak typeof(self) weakSelf = self;
    NSArray *allAddressArray = [NSArray arrayWithArray:self.allAddressArray];
    for (NSNumber *nodeAddress in allAddressArray) {
        if (![self isMeshOTAing]) {
            return;
        }
        if ([self.failAddressArray containsObject:nodeAddress]) {
            continue;
        }
            SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:nodeAddress.intValue];
            if (node.state != DeviceStateOutOfLine) {
                self.semaphore = dispatch_semaphore_create(0);
                self.messageHandle = [SDKLibCommand BLOBTransferStartWithDestination:node.address transferMode:self.distributionTransferMode BLOBID:self.updateBLOBID BLOBSize:(UInt32)self.otaData.length BLOBBlockSizeLog:self.blockSizeLog MTUSize:self.MTUSize retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigBLOBTransferStatus * _Nonnull responseMessage) {
                    TeLogDebug(@"BLOBTransferStart=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                    if (source == nodeAddress.intValue) {
//                        if (responseMessage.status == SigBLOBTransferStatusType_busy) {
                            hasSuccess = YES;
//                        } else {
//                           hasFail = YES;
//                           weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in BLOBTransferStart,SigBLOBTransferStatus.status=0x%x",responseMessage.status] code:-weakSelf.meshOTAProgress userInfo:nil];
//                        }
                    } else {
                        if (![weakSelf.failAddressArray containsObject:nodeAddress]) {
                            [weakSelf.failAddressArray addObject:nodeAddress];
                        }
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    [NSThread sleepForTimeInterval:0.2];
                    dispatch_semaphore_signal(weakSelf.semaphore);
                }];
                //Most provide 3 seconds to BLOBTransferStart every node.
                dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
            } else {
                self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in BLOBTransferStart, node is outOfLine"] code:-weakSelf.meshOTAProgress userInfo:nil];
                if (![self.failAddressArray containsObject:nodeAddress]) {
                    [self.failAddressArray addObject:nodeAddress];
                }
            }
    }
    if (hasSuccess) {
        [self BLOBTransferStartSuccessAction];
    } else {
        [self BLOBTransferStartFailAction];
    }
}

- (void)BLOBTransferStartSuccessAction {
    self.blockIndex = 0;
    double blockSize = (double)pow(2, self.blockSizeLog);
    self.allBlockCount = ceil(self.otaData.length / blockSize);
    self.retryCountInBLOBChunkTransfer = 0;
    [self BLOBBlockStart];
}

- (void)BLOBTransferStartFailAction {
    [self meshOTAFailAction];
}

#pragma mark step9:BLOBBlockStart
- (void)BLOBBlockStart {
    self.meshOTAProgress = SigMeshOTAProgressBLOBBlockStart;
    TeLogInfo(@"\n\n==========meshOTA:step%d\n\n",self.meshOTAProgress);

    __block BOOL hasSuccess = NO;
    __weak typeof(self) weakSelf = self;
    NSArray *allAddressArray = [NSArray arrayWithArray:self.allAddressArray];
    for (NSNumber *nodeAddress in allAddressArray) {
        if (![self isMeshOTAing]) {
            return;
        }
        if ([self.failAddressArray containsObject:nodeAddress]) {
            continue;
        }
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
            self.messageHandle = [SDKLibCommand BLOBBlockStartWithDestination:node.address blockNumber:self.blockIndex chunkSize:kChunkSize retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigBLOBBlockStatus * _Nonnull responseMessage) {
                TeLogDebug(@"BLOBBlockStart=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                if (source == nodeAddress.intValue) {
//                        if (responseMessage.status == SigObjectBlockTransferStatusType_accepted || responseMessage.status == SigObjectBlockTransferStatusType_alreadyRX) {
                        hasSuccess = YES;
//                        } else {
//                            hasFail = YES;
//                            weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in BLOBBlockStart,SigObjectBlockTransferStatus.status=0x%x",responseMessage.status] code:-weakSelf.meshOTAProgress userInfo:nil];
//                        }
                } else {
                    if (![weakSelf.failAddressArray containsObject:nodeAddress]) {
                        [weakSelf.failAddressArray addObject:nodeAddress];
                    }
                }
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                [NSThread sleepForTimeInterval:0.2];
                dispatch_semaphore_signal(weakSelf.semaphore);
            }];
            //Most provide 3 seconds to BLOBBlockStart every node.
            dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
        } else {
            self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in BLOBBlockStart, node is outOfLine"] code:-weakSelf.meshOTAProgress userInfo:nil];
            if (![self.failAddressArray containsObject:nodeAddress]) {
                [self.failAddressArray addObject:nodeAddress];
            }
        }
    }
    if (hasSuccess) {
        [self BLOBBlockStartSuccessAction];
    } else {
        [self BLOBBlockStartFailAction];
    }
}

- (void)BLOBBlockStartSuccessAction {
    [self BLOBChunkTransfer];
}

- (void)BLOBBlockStartFailAction {
    [self meshOTAFailAction];
}

#pragma mark step10:BLOBChunkTransfer
- (void)BLOBChunkTransfer {
    self.meshOTAProgress = SigMeshOTAProgressBLOBChunkTransfer;
    TeLogInfo(@"\n\n==========meshOTA:step%d\n\n",self.meshOTAProgress);

    self.chunksCountofCurrentBlock = ceil(self.currentBlockData.length / (double)kChunkSize);
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
            TeLogInfo(@"all Block count=%d,current block index=%d,all chunk count=%d,current chunk index=%d ",self.allBlockCount,self.blockIndex,self.chunksCountofCurrentBlock,self.chunkIndex);
            [self showMeshOTAProgressWithCurrentChunkData:chunkData];
            if (!_testFinish) {
                if (self.blockIndex == 0 && self.chunkIndex == 6) {
                    //这个包不发送，测试补包流程
                    _testFinish = YES;
                    self.successActionInCurrentProgress ++;
                    continue;
                }
            }
            self.semaphore = dispatch_semaphore_create(0);
            self.messageHandle = [SDKLibCommand BLOBChunkTransferWithDestination:kMeshOTAGroupAddress chunkNumber:self.chunkIndex chunkData:chunkData retryCount:0 responseMaxCount:0 resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                if (error) {
                    hasFail = YES;
                    weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in BLOBChunkTransfer, error=%@.",error.domain] code:-weakSelf.meshOTAProgress userInfo:nil];
                } else {
                    weakSelf.successActionInCurrentProgress ++;

                }
                [NSThread sleepForTimeInterval:0.2];
                dispatch_semaphore_signal(weakSelf.semaphore);
            }];
            //Most provide 1000 seconds for BLOBChunkTransferWithDestination in every chunk.
            dispatch_semaphore_wait(self.semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 1000.0));
        } else {
            if (!self.failError) {
                self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in BLOBBlockStart, BLOBBlockStart is timeout."] code:-self.meshOTAProgress userInfo:nil];
            }
            hasFail = YES;
            break;
        }
    }
    if (hasFail) {
        [self BLOBChunkTransferFailAction];
    } else {
        [self BLOBChunkTransferSuccessFinishAction];
    }
}

- (void)BLOBChunkTransferWithLosePackets {
    self.meshOTAProgress = SigMeshOTAProgressBLOBChunkTransfer;
    TeLogInfo(@"\n\n==========meshOTA:step%d.1\n\n",self.meshOTAProgress);
    __block BOOL hasFail = NO;
    self.successActionInCurrentProgress = 0;
    if (self.losePacketsDict && self.losePacketsDict.allKeys.count > 0) {
        NSArray *losePacketsDictAllKeys = self.losePacketsDict.allKeys;
        for (NSNumber *addressNumber in losePacketsDictAllKeys) {
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
                self.messageHandle = [SDKLibCommand BLOBChunkTransferWithDestination:destination chunkNumber:self.chunkIndex chunkData:chunkData retryCount:0 responseMaxCount:0 resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    if (error) {
                        hasFail = YES;
                        weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in BLOBChunkTransfer, error=%@.",error.domain] code:-weakSelf.meshOTAProgress userInfo:nil];
                    } else {
                        weakSelf.successActionInCurrentProgress ++;

                    }
                    [NSThread sleepForTimeInterval:0.2];
                    dispatch_semaphore_signal(weakSelf.semaphore);
                }];
                //Most provide 1000 seconds for BLOBChunkTransferWithDestination in every chunk.
                dispatch_semaphore_wait(self.semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 1000.0));
            }
        }
    }
    if (hasFail) {
        [self BLOBChunkTransferFailAction];
    } else {
        [self BLOBChunkTransferSuccessFinishAction];
    }
}


- (void)BLOBChunkTransferSuccessFinishAction {
    [self BLOBBlockGet];
}

- (void)BLOBChunkTransferFailAction {
    [self meshOTAFailAction];
}

- (void)showMeshOTAProgressWithCurrentChunkData:(NSData *)chunkData {
    if (self.progressBlock) {
        UInt16 blockSize = pow(2, self.blockSizeLog);
        double progress = (self.blockIndex * blockSize + self.chunkIndex * kChunkSize) / (double)self.otaData.length * 99;
        NSInteger intPro = (NSInteger)progress;
        TeLogDebug(@"progress=%f,intPro=%ld",progress,(long)intPro);
        self.progressBlock(intPro);
    }
}

#pragma mark step11:BLOBBlockGet
- (void)BLOBBlockGet {
    self.meshOTAProgress = SigMeshOTAProgressBLOBBlockGet;
    TeLogInfo(@"\n\n==========meshOTA:step%d\n\n",self.meshOTAProgress);

    self.losePacketsDict = [NSMutableDictionary dictionary];
    __block BOOL hasSuccess = NO;
    __block BOOL hasLost = NO;
    __weak typeof(self) weakSelf = self;
    NSArray *allAddressArray = [NSArray arrayWithArray:self.allAddressArray];
    for (NSNumber *nodeAddress in allAddressArray) {
        if (![self isMeshOTAing]) {
            return;
        }
        if ([self.failAddressArray containsObject:nodeAddress]) {
            continue;
        }
        SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:nodeAddress.intValue];
        if (node.state != DeviceStateOutOfLine) {
            self.semaphore = dispatch_semaphore_create(0);
            TeLogInfo(@"self.blockIndex=%d",self.blockIndex);
            self.messageHandle = [SDKLibCommand BLOBBlockGetWithDestination:node.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigBLOBBlockStatus * _Nonnull responseMessage) {
                TeLogDebug(@"BLOBBlockGet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                if (source == nodeAddress.intValue) {
                    /*BLOBBlockGet=80000000010600,source=2,destination=1*/
                    if (responseMessage.status == SigBLOBBlockStatusType_success) {
                        hasSuccess = YES;
                        if (responseMessage.format == SigBLOBBlockFormatType_someChunksMissing) {
                            hasLost = YES;
                            NSString *errorString = [NSString stringWithFormat:@"fail in BLOBBlockGet,SigBLOBBlockStatus.status=0x%x,format=0x%x,blockIndex=%d",responseMessage.status,responseMessage.format,weakSelf.blockIndex];
                            weakSelf.failError = [NSError errorWithDomain:errorString code:-weakSelf.meshOTAProgress userInfo:nil];
                            TeLogInfo(@"%@",errorString);
                            self.losePacketsDict[@(source)] = responseMessage.missingChunksList;
                        } else if (responseMessage.format == SigBLOBBlockFormatType_allChunksMissing) {
                            hasLost = YES;
                            NSString *errorString = [NSString stringWithFormat:@"fail in BLOBBlockGet,SigBLOBBlockStatus.status=0x%x,format=0x%x,blockIndex=%d",responseMessage.status,responseMessage.format,weakSelf.blockIndex];
                            weakSelf.failError = [NSError errorWithDomain:errorString code:-weakSelf.meshOTAProgress userInfo:nil];
                            TeLogInfo(@"%@",errorString);
                            NSMutableArray *chunkIndexs = [NSMutableArray array];
                            for (int i=0; i<self.chunksCountofCurrentBlock; i++) {
                                [chunkIndexs addObject:@(i)];
                            }
                            self.losePacketsDict[@(source)] = chunkIndexs;
                        }
                    } else {
                        NSString *errorString = [NSString stringWithFormat:@"fail in BLOBBlockGet,SigBLOBBlockStatus.status=0x%x,blockIndex=%d",responseMessage.status,weakSelf.blockIndex];
                        weakSelf.failError = [NSError errorWithDomain:errorString code:-weakSelf.meshOTAProgress userInfo:nil];
                        TeLogInfo(@"%@",errorString);
                        if (![weakSelf.failAddressArray containsObject:nodeAddress]) {
                            [weakSelf.failAddressArray addObject:nodeAddress];
                        }
                    }
                } else {
                    if (![weakSelf.failAddressArray containsObject:nodeAddress]) {
                        [weakSelf.failAddressArray addObject:nodeAddress];
                    }
                }
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                [NSThread sleepForTimeInterval:0.2];
                dispatch_semaphore_signal(weakSelf.semaphore);
            }];
            //Most provide 3 seconds to BLOBBlockStart every node.
            dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
        } else {
            self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in BLOBBlockGet, node is outOfLine"] code:-weakSelf.meshOTAProgress userInfo:nil];
            if (![self.failAddressArray containsObject:nodeAddress]) {
                [self.failAddressArray addObject:nodeAddress];
            }
        }
    }
    
    
    if (hasLost) {
        if (self.losePacketsDict && self.losePacketsDict.allKeys.count > 0) {
            //做法1：retry使用单播地址
//            [self BLOBChunkTransferWithLosePackets];
            //做法2：retry使用组播地址
            NSMutableDictionary *newDict = [NSMutableDictionary dictionary];
            NSMutableArray *newLoseChunkIndexs = [NSMutableArray array];
            NSDictionary *losePacketsDict = [NSDictionary dictionaryWithDictionary:self.losePacketsDict];
            for (NSNumber *addressNumber in losePacketsDict.allKeys) {
                NSArray *loaeChunkIndexs = losePacketsDict[addressNumber];
                for (NSNumber *chunkIndex in loaeChunkIndexs) {
                    if (![newLoseChunkIndexs containsObject:chunkIndex]) {
                        [newLoseChunkIndexs addObject:chunkIndex];
                    }
                }
            }
            if (newLoseChunkIndexs.count > 0) {
                newDict[@(kMeshOTAGroupAddress)] = newLoseChunkIndexs;
                TeLogInfo(@"newLoseChunkIndexs=%@",newLoseChunkIndexs);
                self.losePacketsDict = [NSMutableDictionary dictionaryWithDictionary:newDict];
                [self BLOBChunkTransferWithLosePackets];
            }else{
                [self BLOBBlockGetFailAction];
            }
        } else {
            [self BLOBBlockGetFailAction];
        }
    } else {
        if (hasSuccess) {
            [self BLOBBlockGetSuccessAction];
        } else {
            [self BLOBBlockGetFailAction];
        }
    }
}

- (void)BLOBBlockGetSuccessAction {
    if (self.blockIndex < self.allBlockCount - 1) {
        // send next block data
        self.blockIndex ++;
        self.retryCountInBLOBChunkTransfer = 0;
        [self BLOBBlockStart];
    } else if (self.blockIndex == self.allBlockCount - 1) {
        // all blcoks had send
        [self firmwareUpdateGet];
    } else {
        self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in BLOBBlockGet, self.blockIndex > self.allBlockCount - 1."] code:-self.meshOTAProgress userInfo:nil];
        [self meshOTAFailAction];
    }
}

- (void)BLOBBlockGetFailAction {
    if (self.retryCountInBLOBChunkTransfer < kRetryCountInBLOBChunkTransfer) {
        TeLogInfo(@"retry send block.");
        self.retryCountInBLOBChunkTransfer ++;
        [self BLOBBlockStart];
    } else {
        TeLogInfo(@"send block fail.");
        [self meshOTAFailAction];
    }
}

#pragma mark step12:firmwareUpdateGet
- (void)firmwareUpdateGet {
    self.meshOTAProgress = SigMeshOTAProgressFirmwareUpdateGet;
    TeLogInfo(@"\n\n==========meshOTA:step%d\n\n",self.meshOTAProgress);
    __block BOOL hasSuccess = NO;
    __weak typeof(self) weakSelf = self;
    NSArray *allAddressArray = [NSArray arrayWithArray:self.allAddressArray];
    for (NSNumber *nodeAddress in allAddressArray) {
        if (![self isMeshOTAing]) {
            return;
        }
        if ([self.failAddressArray containsObject:nodeAddress]) {
            continue;
        }
        SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:nodeAddress.intValue];
        if (node.state != DeviceStateOutOfLine) {
            self.semaphore = dispatch_semaphore_create(0);
            self.messageHandle = [SDKLibCommand firmwareUpdateGetWithDestination:node.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareUpdateStatus * _Nonnull responseMessage) {
                TeLogDebug(@"firmwareUpdateGet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                if (source == nodeAddress.intValue) {
                    if (responseMessage.status == SigFirmwareUpdateStatusType_success) {
//                            if (responseMessage.phase == SigFirmwareUpdatePhaseType_DFUReady) {
//                                if (!responseMessage.additionalInformation.ProvisioningNeeded) {
                                hasSuccess = YES;
//                                } else {
//                                    hasFail = YES;
//                                    weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateGet,responseMessage.additionalInformation.ProvisioningNeeded=0x%x",responseMessage.additionalInformation.ProvisioningNeeded] code:-weakSelf.meshOTAProgress userInfo:nil];
//                                }
//                            } else {
//                                hasFail = YES;
//                                weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateGet,SigFirmwareUpdateStatus.phase=0x%x",responseMessage.phase] code:-weakSelf.meshOTAProgress userInfo:nil];
//                            }
                    } else {
                        weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateGet,SigFirmwareUpdateStatus.status=0x%x",responseMessage.status] code:-weakSelf.meshOTAProgress userInfo:nil];
                        if (![weakSelf.failAddressArray containsObject:nodeAddress]) {
                            [weakSelf.failAddressArray addObject:nodeAddress];
                        }
                    }
                } else {
                    if (![weakSelf.failAddressArray containsObject:nodeAddress]) {
                        [weakSelf.failAddressArray addObject:nodeAddress];
                    }
                }
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                [NSThread sleepForTimeInterval:0.2];
                dispatch_semaphore_signal(weakSelf.semaphore);
            }];
            //Most provide 3 seconds to BLOBBlockStart every node.
            dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
        } else {
            self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateGet, node is outOfLine"] code:-weakSelf.meshOTAProgress userInfo:nil];
            if (![self.failAddressArray containsObject:nodeAddress]) {
                [self.failAddressArray addObject:nodeAddress];
            }
        }
    }
    if (hasSuccess) {
        [self firmwareUpdateGetSuccessAction];
    } else {
        [self firmwareUpdateGetFailAction];
    }
}

- (void)firmwareUpdateGetSuccessAction {
    [self firmwareUpdateApply];
}

- (void)firmwareUpdateGetFailAction {
    [self meshOTAFailAction];
}

#pragma mark step13:firmwareUpdateApply
- (void)firmwareUpdateApply {
    self.meshOTAProgress = SigMeshOTAProgressFirmwareUpdateApply;
    TeLogInfo(@"\n\n==========meshOTA:step%d\n\n",self.meshOTAProgress);
    __block BOOL hasSuccess = NO;
    __weak typeof(self) weakSelf = self;
    NSArray *allAddressArray = [NSArray arrayWithArray:self.allAddressArray];
    for (NSNumber *nodeAddress in allAddressArray) {
        if (![self isMeshOTAing]) {
            return;
        }
        if ([self.failAddressArray containsObject:nodeAddress]) {
            continue;
        }
        SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:nodeAddress.intValue];
        if (node.state != DeviceStateOutOfLine) {
            self.semaphore = dispatch_semaphore_create(0);
            self.messageHandle = [SDKLibCommand firmwareUpdateApplyWithDestination:node.address retryCount:10 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareUpdateStatus * _Nonnull responseMessage) {
                TeLogDebug(@"firmwareUpdateApply=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                if (source == nodeAddress.intValue) {
                    if (responseMessage.status == SigFirmwareUpdateServerAndClientModelStatusType_success) {
                        if (responseMessage.updatePhase == SigFirmwareUpdatePhaseType_applyingUpdate) {
                            //if (responseMessage.updatePhase == SigFirmwareUpdatePhaseType_verificationSuccess) {
//                                if (!responseMessage.additionalInformation.ProvisioningNeeded) {
                                hasSuccess = YES;
                                // apply 成功，则标记meshOTA成功。
                                [weakSelf callBackMeshOTASuccessAddress:node.address];
//                                } else {
//                                    hasFail = YES;
//                                    weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateApply,responseMessage.additionalInformation.ProvisioningNeeded=0x%x",responseMessage.additionalInformation.ProvisioningNeeded] code:-weakSelf.meshOTAProgress userInfo:nil];
//                                }
                        } else {
                            weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateApply,SigFirmwareUpdateStatus.updatePhase=0x%x",responseMessage.updatePhase] code:-weakSelf.meshOTAProgress userInfo:nil];
                            if (![weakSelf.failAddressArray containsObject:nodeAddress]) {
                                [weakSelf.failAddressArray addObject:nodeAddress];
                            }
                        }
                    } else {
                        weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateApply,SigFirmwareUpdateStatus.status=0x%x",responseMessage.status] code:-weakSelf.meshOTAProgress userInfo:nil];
                        if (![weakSelf.failAddressArray containsObject:nodeAddress]) {
                            [weakSelf.failAddressArray addObject:nodeAddress];
                        }
                    }
                } else {
                    if (![weakSelf.failAddressArray containsObject:nodeAddress]) {
                        [weakSelf.failAddressArray addObject:nodeAddress];
                    }
                }
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                [NSThread sleepForTimeInterval:0.2];
                    dispatch_semaphore_signal(weakSelf.semaphore);
            }];
            //Most provide 3 seconds to BLOBBlockStart every node.
            dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
        } else {
            self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateApply, node is outOfLine"] code:-weakSelf.meshOTAProgress userInfo:nil];
            if (![self.failAddressArray containsObject:nodeAddress]) {
                [self.failAddressArray addObject:nodeAddress];
            }
        }
    }
    if (hasSuccess) {
        [self firmwareUpdateApplySuccessAction];
    } else {
        [self firmwareUpdateApplyFailAction];
    }
}

- (void)firmwareUpdateApplySuccessAction {
    if (self.progressBlock) {
        self.progressBlock(100);
    }
    [self firmwareDistributionCancel];
}

- (void)firmwareUpdateApplyFailAction {
    [self meshOTAFailAction];
}

#pragma mark step14:firmwareDistributionCancel
- (void)firmwareDistributionCancel {
    self.meshOTAProgress = SigMeshOTAProgressFirmwareDistributionCancel;
    TeLogInfo(@"\n\n==========meshOTA:step%d\n\n",self.meshOTAProgress);
//    __weak typeof(self) weakSelf = self;
//    self.messageHandle = [SDKLibCommand firmwareDistributionCancelWithDestination:SigDataSource.share.curLocationNodeModel.address retryCount:0 responseMaxCount:0 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareDistributionStatus * _Nonnull responseMessage) {
//        TeLogDebug(@"firmwareDistributionCancel=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
//        [weakSelf firmwareDistributionCancelSuccessAction];
//    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
//        TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
//    }];
    
    [self firmwareDistributionCancelSuccessAction];
}

- (void)firmwareDistributionCancelSuccessAction {
    [self showMeshOTAResult];
}

- (void)firmwareDistributionCancelFailAction {
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
