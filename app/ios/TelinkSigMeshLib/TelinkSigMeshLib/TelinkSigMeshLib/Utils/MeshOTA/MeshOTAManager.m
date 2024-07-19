/********************************************************************************************************
 * @file     MeshOTAManager.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/4/24
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "MeshOTAManager.h"
#import "ConnectTools.h"
#import "SDKLibCommand+firmwareUpdate.h"

//每一个步骤的超时时间，默认是60.0s
#define kTimeOutOfEveryStep (dispatch_semaphore_wait(weakSelf.semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 60.0)))
#define kRetryCountInBLOBChunkTransfer    (3)
#define kMeshOTAGroupAddress    (0xc000)
#define kUnSegmentPacketInterval   (0.180)
#define kFirmwareDistributionReceiversGetInterval   (10.0)
#define kCreateSimulatedMessageInterval   (0.010)

#define kPid    @"pid"
#define kVid    @"vid"

@interface MeshOTAManager()<SigBearerDataDelegate,SigMessageDelegate>
@property (nonatomic, copy) FinishBlock finishBlock;
@property (nonatomic, copy) ErrorBlock errorBlock;
@property (nonatomic, assign) UInt8 blockSizeLog;//记录本次meshOTA的block大小，size为2的kBlockSizeLog次幂，默认是2^12=4096字节
@property (nonatomic, assign) UInt16 chunkSize;//记录本次meshOTA的chunk大小
@property (nonatomic, strong) NSError *failError;

// test model
@property (nonatomic, assign) BOOL testFinish;

@property (nonatomic, assign) NSInteger LPNReachablleTimerCounter;
@property (nonatomic, strong) BackgroundTimer *LPNReachablleTimer;
@property (nonatomic, strong) SigBLOBPartialBlockReport *BLOBPartialBlockReport;

// distributor
@property (nonatomic, assign) SigFirmwareUpdateProgress firmwareUpdateProgress;//记录Firmware Update的进度
@property (nonatomic, strong) NSData *firmwareID;
@property (nonatomic, copy) ProgressBlock gattDistributionProgressBlock;//用于Initiator发送到Distributor过程中，上报Distributor的进度状态。
@property (nonatomic, copy) ProgressReceiversListBlock advDistributionProgressBlock;//用于Distributor广播到updating node(s)过程中，上报updating node(s)的状态。
@property (nonatomic, strong) SigFirmwareDistributionReceiversList *firmwareDistributionReceiversList;
@property (nonatomic, strong) NSMutableData *firmwareDataOnDistributor;//手机作为Distributor时，存储手机端Initiator发送给手机端Distributor的firmwareData
// distributor

@property (nonatomic, strong) NSMutableArray <NSNumber *>*successAddressArray;//Mesh OTA成功的短地址列表
@property (nonatomic, strong) NSMutableArray <NSNumber *>*failAddressArray;//Mesh OTA失败的短地址列表（包括中途失败）

// 直连节点作为distributor时，发送ReceiversGet使用的firstIndex参数。初始化为0。
@property (nonatomic, assign) NSInteger firstIndexOfFirmwareDistributionReceiversGet;

@property (nonatomic, assign) UInt8 allBlockCount;//记录block总个数
@property (nonatomic, assign) UInt8 blockIndex;//记录当前block的index
@property (nonatomic, strong) NSData *currentBlockData;//记录当前block的data
@property (nonatomic, assign) NSInteger chunksCountofCurrentBlock;//记录当前block的chunk总个数
@property (nonatomic, assign) NSInteger chunkIndex;//记录当前chunk的index
@property (nonatomic, assign) NSInteger successActionInCurrentProgress;//记录当前阶段成功的设备个数
@property (nonatomic, strong) NSMutableDictionary <NSNumber *, NSArray *>*losePacketsDict;//step10阶段传输失败的包。
@property (nonatomic, strong) NSMutableDictionary <NSNumber *, NSDictionary *>*oFirmwareInformation;//记录MeshOTA前设备的版本号,[@(UInt16):@{@"cid":@(UInt16),@"pid":@(UInt16),@"vid":@(UInt16)}]
@property (nonatomic, strong) NSMutableDictionary <NSNumber *, NSDictionary *>*nFirmwareInformation;//记录MeshOTA后设备的版本号,[@(UInt16):@{@"cid":@(UInt16),@"pid":@(UInt16),@"vid":@(UInt16)}]
@property (nonatomic, strong) NSMutableDictionary <NSNumber *, SigBLOBInformationStatus *>*oBLOBInformations;//记录MeshOTA前设备的BLOBInformation
@property (nonatomic, assign) UInt8 retryCountInBLOBChunkTransfer;//记录step10:BLOBChunkTransfer阶段已经重试的次数
@property (nonatomic, retain) dispatch_semaphore_t semaphore;
@property (nonatomic, strong) SigMessageHandle *messageHandle;

@property (nonatomic, strong) NSThread *meshOTAThread;
@property (nonatomic, weak) id <SigBearerDataDelegate>oldBearerDataDelegate;
@property (nonatomic, weak) id <SigMessageDelegate>oldDelegateForDeveloper;

@property (nonatomic, strong) SigNodeModel *connectedNodeModel;//记录本次meshOTA的指令节点

@property (nonatomic, assign) NSInteger firmwareDistributionGetCount;//记录当前Distributor还剩余DistributionGet指令的次数，默认10秒一次，查6次。

@end

@implementation MeshOTAManager

/**
 *  @brief  Singleton method
 *
 *  @return the default singleton instance. You are not allowed to create your own instances of this class.
 */
+ (instancetype)share {
    /// Singleton instance
    static MeshOTAManager *meshOTAManager = nil;
    /// Note: The dispatch_once function can ensure that a certain piece
    /// of code is only executed once in the entire application life cycle!
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        /// Initialize the Singleton configure parameters.
        meshOTAManager = [[MeshOTAManager alloc] init];
        [meshOTAManager initData];
    });
    return meshOTAManager;
}

/// Initialize the parameters of MeshOTAManager.
-(void)initData {
    _allAddressArray = [NSMutableArray array];
    _successAddressArray = [NSMutableArray array];
    _failAddressArray = [NSMutableArray array];
    _additionalInformationDictionary = [NSMutableDictionary dictionary];
    
    // config parameters for meshOTA R04
    _transferModeOfDistributor = SigTransferModeState_pushBLOBTransferMode;
    _transferModeOfUpdateNodes = SigTransferModeState_pushBLOBTransferMode;
    _updatePolicy = SigUpdatePolicyType_verifyAndApply;
    _distributionFirmwareImageIndex = 0;
    UInt16 gAddress = kMeshOTAGroupAddress;
    _distributionMulticastAddress = [NSData dataWithBytes:&gAddress length:2];
    UInt64 tem64 = 0;
    _incomingFirmwareMetadata = [NSData dataWithBytes:&tem64 length:8];
    _updateFirmwareImageIndex = 0;
    _updateTTL = 0xFF;
    _uploadTTL = 0xFF;
    _distributionTTL = 0xFF;
    _updateTimeoutBase = 0;
    _uploadTimeoutBase = 0;
    _distributionTimeoutBase = 0;
    _updateBLOBID = 0x8877665544332211;
    _updateBLOBIDForDistributor = 0x8977665544332211;
    _MTUSize = 380;
    _phoneIsDistributor = NO;
    _needCheckVersionAfterApply = NO;

    _meshOTAThread = [[NSThread alloc] initWithTarget:self selector:@selector(startThread) object:nil];
    _meshOTAThread.name = @"MeshOTA Thread";
    _meshOTAThread.threadPriority = 0.9;
    [_meshOTAThread start];
}

#pragma mark - Private
- (void)startThread {
    [NSTimer scheduledTimerWithTimeInterval:[[NSDate distantFuture] timeIntervalSinceNow] target:self selector:@selector(nullFunc) userInfo:nil repeats:NO];
    while (1) {
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
    }
}

/// null function
- (void)nullFunc {}

// 7.4.1.3 Pull BLOB State Machine
/// The Maximum BLOB Poll Interval is designated “T_MBPI” and shall be set to 30 seconds. （返回LPN节点轮询一次friend节点的时间间隔。）
- (NSTimeInterval)getT_MBPI {
    return 30;
}

- (NSTimeInterval)getSecondOfClientTimeoutBase:(UInt16)clientTimeoutBase ttl:(UInt16)ttl {
    NSTimeInterval clientTimeout = 10.0 * (clientTimeoutBase + 2) + (0.1 * ttl);
    return clientTimeout;
}

- (NSInteger)getCounterOfBLOBPartialBlockReport {
    NSInteger counter = (NSInteger)ceil([self getSecondOfClientTimeoutBase:self.distributionTimeoutBase ttl:self.distributionTTL] / [self getTimerValueOfBLOBPartialBlockReport] / 2);
    return counter;
}

- (NSTimeInterval)getTimerValueOfBLOBPartialBlockReport {
    NSTimeInterval timerValue = 2 * [self getT_MBPI] + 7;
    return timerValue;
}

- (void)resetMeshOTAData{
    self.otaData = nil;
    self.failError = nil;
    self.finishBlock = nil;
    self.errorBlock = nil;
    [self stopLPNReachablleTimer];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self];
    });
}

- (void)cancelSignal {
    dispatch_semaphore_signal(self.semaphore);
}

/// Check whether the SDK is currently in the meshOTA process.
- (BOOL)isMeshOTAing{
    NSDictionary *dict = [[NSUserDefaults standardUserDefaults] objectForKey:kSaveMeshOTADictKey];
    if (dict != nil && [dict[@"isMeshOTAing"] boolValue]) {
        return YES;
    } else {
        return NO;
    }
}

- (void)saveIsMeshOTAing:(BOOL)isMeshOTAing {
    NSDictionary *dict = @{@"isMeshOTAing":@(isMeshOTAing)};
    [[NSUserDefaults standardUserDefaults] setObject:dict forKey:kSaveMeshOTADictKey];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (void)callBackMeshOTASuccessAddress:(UInt16)address {
    if (![self.successAddressArray containsObject:@(address)] && [self.allAddressArray containsObject:@(address)]) {
        [self.successAddressArray addObject:@(address)];
    }
}

- (void)showMeshOTAResult {
    [self recoveryExtendBearerMode];
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
        if (self.phoneIsDistributor && self.advDistributionProgressBlock) {
            TelinkLogInfo(@"create SigFirmwareDistributionReceiversList by app.");
            NSMutableArray *list = [NSMutableArray array];
            NSArray *allAddressArray = [NSArray arrayWithArray:self.allAddressArray];
            for (NSNumber *nodeAddress in allAddressArray) {
                if (![self.successAddressArray containsObject:nodeAddress]) {
                    continue;
                }
                //待验证实际升级时下面几个参数是否正确。
                SigUpdatingNodeEntryModel *model = [[SigUpdatingNodeEntryModel alloc] initWithAddress:nodeAddress.intValue retrievedUpdatePhase:SigFirmwareUpdatePhaseType_applySuccess updateStatus:SigFirmwareUpdateServerAndClientModelStatusType_success transferStatus:SigBLOBTransferStatusType_success transferProgress:50 updateFirmwareImageIndex:0];
                [list addObject:model];
            }
            UInt16 firstIndex = 0;
            UInt16 receiversListCount = list.count;
            SigFirmwareDistributionReceiversList *responseMessage = [[SigFirmwareDistributionReceiversList alloc] initWithReceiversListCount:receiversListCount firstIndex:firstIndex receiversList:list];
            self.firmwareDistributionReceiversList = responseMessage;
            [self callbackAdvDistributionProgressBlock];
        }
        self.finishBlock(self.successAddressArray, failArray);
    }
    //OTA完成，初始化参数
    [self resetMeshOTAData];
    [SigMeshLib.share cleanAllCommandsAndRetry];
}

-(void)dealloc {
    NSLog(@"%s",__func__);
}

- (NSString *)getProgressNameWithFirmwareUpdateProgress:(SigFirmwareUpdateProgress)firmwareUpdateProgress {
    NSString *tem = @"";
    switch (firmwareUpdateProgress) {
        case SigFirmwareUpdateProgressIdle:
            tem = @"idle";
            break;
        case SigFirmwareUpdateProgressCheckLastFirmwareUpdateStatue:
            tem = @"checkLastFirmwareUpdateStatue(Initiator->Distributor)";
            break;
        case SigFirmwareUpdateProgressFirmwareDistributionCapabilitiesGet:
            tem = @"firmwareDistributionCapabilitiesGet(Initiator->Distributor)";
            break;
        case SigFirmwareUpdateProgressFirmwareUpdateInformationGet:
            tem = @"firmwareUpdateInformationGet(Initiator->updating node(s))";
            break;
        case SigFirmwareUpdateProgressFirmwareUpdateFirmwareMetadataCheck:
            tem = @"firmwareUpdateFirmwareMetadataCheck(Initiator->updating node(s))";
            break;
        case SigFirmwareUpdateProgressSubscriptionAdd:
            tem = @"configModelSubscriptionAdd(Initiator->updating node(s))";
            break;
        case SigFirmwareUpdateProgressFirmwareDistributionReceiversAdd:
            tem = @"firmwareDistributionReceiversAdd(Initiator->Distributor)";
            break;
        case SigFirmwareUpdateProgressFirmwareDistributionUploadStart:
            tem = @"firmwareDistributionUploadStart(Initiator->Distributor)";
            break;
        case SigFirmwareUpdateProgressInitiatorToDistributorBLOBTransferGet:
            tem = @"BLOBTransferGet(initiator->Distributor)";
            break;
        case SigFirmwareUpdateProgressInitiatorToDistributorBLOBInformationGet:
            tem = @"BLOBInformationGet(initiator->Distributor)";
            break;
        case SigFirmwareUpdateProgressInitiatorToDistributorBLOBTransferStart:
            tem = @"BLOBTransferStart(initiator->Distributor)";
            break;
        case SigFirmwareUpdateProgressInitiatorToDistributorBLOBBlockStart:
            tem = @"BLOBBlockStart(initiator->Distributor)";
            break;
        case SigFirmwareUpdateProgressInitiatorToDistributorBLOBChunkTransfer:
            tem = @"BLOBChunkTransfer(initiator->Distributor)";
            break;
        case SigFirmwareUpdateProgressInitiatorToDistributorBLOBBlockGet:
            tem = @"BLOBBlockGet(initiator->Distributor)";
            break;
        case SigFirmwareUpdateProgressFirmwareDistributionStart:
            tem = @"firmwareDistributionStart(Initiator->Distributor)";
            break;
        case SigFirmwareUpdateProgressFirmwareUpdateStart:
            tem = @"firmwareUpdateStart(Distributor->updating node(s))";
            break;
        case SigFirmwareUpdateProgressDistributorToUpdatingNodesBLOBTransferGet:
            tem = @"BLOBTransferGet(Distributor->updating node(s))";
            break;
        case SigFirmwareUpdateProgressDistributorToUpdatingNodesBLOBInformationGet:
            tem = @"BLOBInformationGet(Distributor->updating node(s))";
            break;
        case SigFirmwareUpdateProgressDistributorToUpdatingNodesBLOBTransferStart:
            tem = @"BLOBTransferStart(Distributor->updating node(s))";
            break;
        case SigFirmwareUpdateProgressDistributorToUpdatingNodesBLOBBlockStart:
            tem = @"BLOBBlockStart(Distributor->updating node(s))";
            break;
        case SigFirmwareUpdateProgressDistributorToUpdatingNodesBLOBChunkTransfer:
            tem = @"BLOBChunkTransfer(Distributor->updating node(s))";
            break;
        case SigFirmwareUpdateProgressDistributorToUpdatingNodesBLOBBlockGet:
            tem = @"BLOBBlockGet(Distributor->updating node(s))";
            break;
        case SigFirmwareUpdateProgressFirmwareDistributionReceiversGet:
            tem = @"firmwareDistributionReceiversGet(Initiator->Distributor)";
            break;
        case SigFirmwareUpdateProgressFirmwareDistributionApply:
            tem = @"firmwareDistributionApply(Initiator->Distributor)";
            break;
        case SigFirmwareUpdateProgressFirmwareUpdateGet:
            tem = @"firmwareUpdateGet(Distributor->updating node(s))";
            break;
        case SigFirmwareUpdateProgressFirmwareUpdateApply:
            tem = @"firmwareUpdateApply(Distributor->updating node(s))";
            break;
        case SigFirmwareUpdateProgressFirmwareDistributionGet:
            tem = @"firmwareDistributionGet(Initiator->Distributor)";
            break;
        case SigFirmwareUpdateInformationGetCheckVersion:
            tem = @"firmwareUpdateInformationGetCheckVersion(Initiator->updating node(s))";
            break;
        case SigFirmwareUpdateProgressFirmwareDistributionCancel:
            tem = @"firmwareDistributionCancel(Initiator->Distributor)";
            break;

        default:
            break;
    }
    return tem;
}

- (void)createErrorWithString:(NSString *)errorString {
    self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"%@, %@, extend=%@, connected address=0x%04X", [self getProgressNameWithFirmwareUpdateProgress:self.firmwareUpdateProgress], errorString, SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength == kUnsegmentedMessageLowerTransportPDUMaxLength?@"NO":@"YES", SigDataSource.share.unicastAddressOfConnected] code:-self.firmwareUpdateProgress userInfo:nil];
}

#pragma  mark - SigBearerDataDelegate

/// Callback called when the Bearer is no longer open.
/// @param bearer The Bearer.
/// @param error The reason of closing the Bearer, or `nil` if closing was intended.
- (void)bearer:(SigBearer *)bearer didCloseWithError:(NSError *)error {
    TelinkLogVerbose(@"");
    if (self.isMeshOTAing) {
        if (self.semaphore) {
            dispatch_semaphore_signal(self.semaphore);
        }
        [self performSelector:@selector(cancelAllAfterDelay) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];

        if (self.firmwareUpdateProgress != SigFirmwareUpdateProgressCheckLastFirmwareUpdateStatue && ((self.phoneIsDistributor && self.firmwareUpdateProgress < SigFirmwareUpdateProgressFirmwareDistributionApply) || (!self.phoneIsDistributor && self.firmwareUpdateProgress < SigFirmwareUpdateProgressFirmwareDistributionStart))) {
            [self createErrorWithString:@"Node is disconnected, MeshOTA fail"];
            [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
        } else {
            if ((self.firmwareUpdateProgress >= SigFirmwareUpdateProgressDistributorToUpdatingNodesBLOBTransferGet && self.firmwareUpdateProgress != SigFirmwareUpdateProgressFirmwareDistributionApply) || self.firmwareUpdateProgress == SigFirmwareUpdateProgressCheckLastFirmwareUpdateStatue || self.firmwareUpdateProgress == SigFirmwareUpdateProgressInitiatorToDistributorBLOBChunkTransfer) {
                //处理逻辑：当设备端做为Distributor且处于Distributor的处理阶段时，即使APP断开与Distributor的连接也不会进入失败流程，而是重连原来的直连节点
                SigNodeModel *node = [SigDataSource.share getNodeWithAddress:self.distributorAddress];
                NSMutableArray *nodeList = [NSMutableArray array];
                if (self.phoneIsDistributor) {
                    if (self.firmwareUpdateProgress == SigFirmwareUpdateInformationGetCheckVersion) {
                        //firmwareUpdateInformationGet，该消息在modelID：kSigModel_FirmwareUpdateServer_ID里面。
                        UInt16 modelIdentifier = kSigModel_FirmwareUpdateServer_ID;
                        NSArray *curNodes = [NSArray arrayWithArray:SigDataSource.share.curNodes];
                        for (SigNodeModel *model in curNodes) {
                                NSArray *addressArray = [model getAddressesWithModelID:@(modelIdentifier)];
                            if (addressArray && addressArray.count > 0) {
                                [nodeList addObject:model];
                            }
                        }

                    } else {
                        if (![nodeList containsObject:self.connectedNodeModel]) {
                            [nodeList addObject:self.connectedNodeModel];
                        }
                    }
                } else {
                    [nodeList addObject:node];
                }
                if (nodeList.count) {
                    //无限扫描连接distributor，直到连接成功。（可能会修改）
                    __weak typeof(self) weakSelf = self;
                    if (self.peripheralStateChangeBlock) {
                        self.peripheralStateChangeBlock(bearer.getCurrentPeripheral);
                    }
                    [ConnectTools.share startConnectToolsWithNodeList:nodeList timeout:0xFFFFFFFF Complete:^(BOOL successful) {
                        TelinkLogVerbose(@"successful=%d",successful);
                        if (successful) {
                            if (weakSelf.peripheralStateChangeBlock) {
                                weakSelf.peripheralStateChangeBlock(bearer.getCurrentPeripheral);
                            }
                            dispatch_async(dispatch_get_main_queue(), ^{
                                [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(connectMeshOTAByFirmwareUpdateProgress) object:nil];
                                dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(3.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                                    [weakSelf performSelector:@selector(connectMeshOTAByFirmwareUpdateProgress) onThread:weakSelf.meshOTAThread withObject:nil waitUntilDone:NO];
                                });
                            });
                        }
                    }];
                } else {
                    [self firmwareUpdateFailAction];
                }
            } else {
                [self firmwareUpdateFailAction];
            }
        }
    }
}

#pragma mark - SigMessageDelegate

/// A callback called whenever a Mesh Message has been received from the mesh network.
/// @param message The received message.
/// @param source The Unicast Address of the Element from which the message was sent.
/// @param destination The address to which the message was sent.
- (void)didReceiveMessage:(SigMeshMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination {
    if ([message isKindOfClass:[SigBLOBPartialBlockReport class]]) {
        TelinkLogVerbose(@"MeshOTAManager Receive:%@,source=%d,destination=%d",[LibTools convertDataToHexStr:message.parameters],source,destination);
        self.BLOBPartialBlockReport = (SigBLOBPartialBlockReport *)message;
        if (self.isMeshOTAing && self.transferModeOfUpdateNodes == SigTransferModeState_pullBLOBTransferMode && self.firmwareUpdateProgress >= SigFirmwareUpdateProgressDistributorToUpdatingNodesBLOBBlockStart && self.firmwareUpdateProgress <= SigFirmwareUpdateProgressDistributorToUpdatingNodesBLOBBlockGet) {
            if (self.BLOBPartialBlockReport.encodedMissingChunks) {
//                TelinkLogError(@"=====chunk，接收到地址%d需要发送的chunk=%@",source,self.BLOBPartialBlockReport.encodedMissingChunks);
                self.losePacketsDict[@(source)] = self.BLOBPartialBlockReport.encodedMissingChunks;
                [self stopLPNReachablleTimer];
                [self handleLPNReportAction];
            } else if (self.chunkIndex != 0) {
                //原因：有时候，发送start时就返回一个空的BLOBPartialBlockReport，导致APP走到查询流程。
                    //当前block发送完成，检查是否漏包，不漏则blockIndex加一进行下一个block的发送。
                [self performSelector:@selector(distributorToUpdatingNodesBLOBBlockGet) onThread:self.meshOTAThread withObject:nil waitUntilDone:NO];
            }
        }
    }
}

- (void)handleLPNReportAction {
    [self performSelector:@selector(sendBLOBChunkTransferByLosePacketsDict) onThread:self.meshOTAThread withObject:nil waitUntilDone:NO];
}

/// 当前只能是pull模式才会调用这个接口，pull模式现在只支持一次升级一个节点，BLOBChunkTransfer使用的是节点的单播地址。
- (void)sendBLOBChunkTransferByLosePacketsDict {
    TelinkLogVerbose(@"");
    if (![self isMeshOTAing]) {
        return;
    }

    if (self.losePacketsDict.count == 0) {
        return;
    }
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressDistributorToUpdatingNodesBLOBChunkTransfer;
    TelinkLogInfo(@"\n\n==========firmware update:step%d.2\n\n",self.firmwareUpdateProgress);

    UInt16 destination = [self.losePacketsDict.allKeys.firstObject intValue];
    for (NSNumber *missingChunkIndex in self.losePacketsDict[@(destination)]) {
        self.chunkIndex = missingChunkIndex.intValue;
        NSData *chunkData = nil;
        if (self.chunkIndex == self.chunksCountofCurrentBlock - 1) {
            //end chunk of current block
            chunkData = [self.currentBlockData subdataWithRange:NSMakeRange(self.chunkSize * self.chunkIndex, self.currentBlockData.length - self.chunkSize * self.chunkIndex)];
        } else {
            chunkData = [self.currentBlockData subdataWithRange:NSMakeRange(self.chunkSize * self.chunkIndex, self.chunkSize)];
        }
        __weak typeof(self) weakSelf = self;
        TelinkLogInfo(@"all Block count=%d,current block index=%d,all chunk count=%d,current chunk index=%d ",self.allBlockCount,self.blockIndex,self.chunksCountofCurrentBlock,self.chunkIndex);
        //该处为新meshOTA逻辑，需要模拟Distributor广播固件到updating nodes，并模拟adv回包，即self.firmwareDistributionReceiversList.
        [self createSigFirmwareDistributionReceiversListWithCurrentChunkData:chunkData otaData:self.firmwareDataOnDistributor];
        [self callbackAdvDistributionProgressBlock];
        self.semaphore = dispatch_semaphore_create(0);
        TelinkLogVerbose(@"send chunk index=%d,self.chunksCountofCurrentBlock=%d",self.chunkIndex,self.chunksCountofCurrentBlock);

        //v3.3.0开始新增优化逻辑：当只有一个节点且为直连节点时，不再通过组地址进行OTA数据发送，只对直连节点进行OTA数据发送即可。
//        TelinkLogError(@"=====chunk，开始给地址%d发送chunk%d,block%d",destination,self.chunkIndex,self.blockIndex);

        self.messageHandle = [SDKLibCommand BLOBChunkTransferWithDestination:destination chunkNumber:self.chunkIndex chunkData:chunkData sendBySegmentPdu:NO retryCount:0 responseMaxCount:0 resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
            TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
            if (error) {
                weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in BLOBChunkTransfer, error=%@.",error.domain] code:-weakSelf.firmwareUpdateProgress userInfo:nil];
                dispatch_semaphore_signal(weakSelf.semaphore);
            } else {
                weakSelf.successActionInCurrentProgress ++;
                int64_t delta = (int64_t)(kUnSegmentPacketInterval * NSEC_PER_SEC);
                if (weakSelf.allAddressArray.count == 1) {
                    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:weakSelf.allAddressArray.firstObject.intValue];
                    if (node && node.isLPN && node.address == SigDataSource.share.unicastAddressOfConnected && self.phoneIsDistributor) {
                        //直连LPN升级的情况，不需要延时
                        delta = 0;
                    }
                }
                dispatch_after(dispatch_time(DISPATCH_TIME_NOW, delta), dispatch_get_main_queue(), ^{
                    dispatch_semaphore_signal(weakSelf.semaphore);
                });
            }
        }];
        //Most provide 24*60*60 seconds for BLOBChunkTransfer(Distributor->updating node(s)) in every chunk.
        dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
//        TelinkLogError(@"=====chunk，完成给地址%d发送chunk%d,block%d",destination,self.chunkIndex,self.blockIndex);
    }
//    TelinkLogError(@"=====chunk，当前一轮chunk发送完成。");
    [self startLPNReachablleTimer];
}

- (void)startLPNReachablleTimer {
    [self stopLPNReachablleTimer];
    self.LPNReachablleTimerCounter = 0;
    TelinkLogVerbose(@"self.LPNReachablleTimerCounter = %d",self.LPNReachablleTimerCounter);
    __weak typeof(self) weakSelf = self;
    self.LPNReachablleTimer = [BackgroundTimer scheduledTimerWithTimeInterval:[self getSecondOfClientTimeoutBase:self.distributionTimeoutBase ttl:self.distributionTTL] repeats:YES block:^(BackgroundTimer * _Nonnull t) {
        TelinkLogVerbose(@"self.LPNReachablleTimerCounter = %d",weakSelf.LPNReachablleTimerCounter);
        if (weakSelf.LPNReachablleTimerCounter > [weakSelf getCounterOfBLOBPartialBlockReport]) {
            //总超时已经到，meshOTA失败
            [weakSelf createErrorWithString:@"LPN reachable timeout"];
            [weakSelf stopLPNReachablleTimer];
            [weakSelf performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
        } else {
            //达到一次超时，重发一次
            if (weakSelf.BLOBPartialBlockReport) {
                //chunk过程中重试
                weakSelf.LPNReachablleTimerCounter ++;
                [weakSelf sendBLOBChunkTransferByLosePacketsDict];
            } else {
                //BLOB Block start过程中重试，该位置不需要重试
            }
        }
    }];
}

- (void)stopLPNReachablleTimer {
    if (self.LPNReachablleTimer) {
        [self.LPNReachablleTimer invalidate];
        self.LPNReachablleTimer = nil;
    }
}

- (void)cancelAllAfterDelay {
    //meshOTA逻辑里面的查询版本号先取消掉
    TelinkLogDebug(@"取消10秒计时firmwareUpdateFirmwareDistributionGetSuccessAction");
    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(firmwareUpdateFirmwareDistributionGet) object:nil];
    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(firmwareUpdateFirmwareDistributionGetSuccessAction) object:nil];
    //meshOTA逻辑里面的查询进度先取消掉
    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(fiemwareUpdateFirmwareDistributionReceiversGet) object:nil];
}

#pragma mark - Firmware update API

/*新增逻辑：升级单个直连节点时，虽然设备上报的chunkSize=8，APP依然使用chunkSize=208来发送chunk包，这样可以加快升级速率。代码逻辑是：判断升级单个直连节点时，修改缓存的内容SigDataSource.share.telinkExtendBearerMode=SigTelinkExtendBearerMode_extendGATTOnly，升级完成后，无论是升级成功还是失败，都将这个值修改回本地存储的值。*/

/// 开始MeshOTA
/// @param deviceAddresses 需要升级的设备地址数组
/// @param otaData 需要升级的设备firmware数据
/// @param incomingFirmwareMetadata meshOTA校验数据
/// @param gattDistributionProgressBlock initiator->Distributor阶段升级的进度回调
/// @param advDistributionProgressBlock Distributor->updating node(s)阶段升级的进度回调
/// @param finishBlock 升级完成的回调
/// @param errorBlock 升级失败的回调
- (void)startFirmwareUpdateWithDeviceAddresses:(NSArray <NSNumber *>*)deviceAddresses otaData:(NSData *)otaData incomingFirmwareMetadata:(NSData *)incomingFirmwareMetadata gattDistributionProgressHandle:(ProgressBlock)gattDistributionProgressBlock advDistributionProgressHandle:(ProgressReceiversListBlock)advDistributionProgressBlock finishHandle:(FinishBlock)finishBlock errorHandle:(ErrorBlock)errorBlock {
    TelinkLogVerbose(@"");

    [SigMeshLib.share cleanAllCommandsAndRetry];
    if (_meshOTAThread.isCancelled && !_meshOTAThread.isExecuting) {
        [_meshOTAThread start];
    }
    self.failError = nil;
    self.distributorAddress = SigMeshLib.share.dataSource.unicastAddressOfConnected;
    self.connectedNodeModel = SigMeshLib.share.dataSource.getCurrentConnectedNode;
    self.otaData = otaData;
    self.gattDistributionProgressBlock = gattDistributionProgressBlock;
    self.advDistributionProgressBlock = advDistributionProgressBlock;
    self.finishBlock = finishBlock;
    self.errorBlock = errorBlock;
    self.firstIndexOfFirmwareDistributionReceiversGet = 0;
    [self saveIsMeshOTAing:YES];
    [self.allAddressArray removeAllObjects];
    [self.successAddressArray removeAllObjects];
    [self.failAddressArray removeAllObjects];
    [self.additionalInformationDictionary removeAllObjects];
    [self.allAddressArray addObjectsFromArray:deviceAddresses];
    self.testFinish = otaData.length > 1024*3;
    self.incomingFirmwareMetadata = incomingFirmwareMetadata;
    UInt16 gAddress = kMeshOTAGroupAddress;
    _distributionMulticastAddress = [NSData dataWithBytes:&gAddress length:2];
    if (SigBearer.share.dataDelegate) {
        self.oldBearerDataDelegate = SigBearer.share.dataDelegate;
    }
    if (SigMeshLib.share.delegateForDeveloper) {
        self.oldDelegateForDeveloper = SigMeshLib.share.delegateForDeveloper;
    }
    SigMeshLib.share.delegateForDeveloper = self;

    SigBearer.share.dataDelegate = self;
    [SDKLibCommand setBluetoothCentralUpdateStateCallback:nil];
    SigBearer.share.isAutoReconnect = NO;
    NSMutableArray *rssiArray = [NSMutableArray array];
    __weak typeof(self) weakSelf = self;
    //该逻辑有两个目的：1.防止近的设备先apply而远的设备后apply导致部分设备apply失败。 2.直连节点必须最后apply，，否则在直连节点后面apply的设备先进行重启导致其他未apply的设备接受不到apply指令了。
    if (deviceAddresses.count == 1) {
        SigNodeModel *node = [SigDataSource.share getNodeWithAddress:deviceAddresses.firstObject.intValue];
        if (node && node.address == SigDataSource.share.unicastAddressOfConnected) {
            //新增逻辑：升级单个直连节点时，虽然设备上报的chunkSize=8，APP依然使用chunkSize=208来发送chunk包，这样可以加快升级速率。
            SigDataSource.share.telinkExtendBearerMode = SigTelinkExtendBearerMode_extendGATTOnly;
            SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength = kDLEUnsegmentLength;
        }
        if (node && node.isLPN) {//LPN的时候才使用0，非LPN的时候还是使用0xC000这个group地址。LPN只能单独升级，且不能用组播地址进行升级。- seeAlso: MshMDL_DFU_MBT_CR_R04_LbL35_JR_PW.pdf  (page.89)
            UInt16 gAddress = 0;
            _distributionMulticastAddress = [NSData dataWithBytes:&gAddress length:2];
        }
        //新增逻辑：防止上一次meshOTA未完成，设备还处于非idle状态，在该处先查询设备的firmwareUpdate状态，如果phase是非idle状态，则需要先updateCancel，再进行meshOTA流程。
        [self checkLastFirmwareUpdateStatueWithCompleteHandle:^(BOOL isSuccess) {
            [weakSelf connectMeshOTAByFirmwareUpdateProgress];
        }];
    } else {
        UInt16 responseMaxCount = SigMeshLib.share.dataSource.getOnlineDevicesNumber;
        if (responseMaxCount == 0) {
            responseMaxCount = SigDataSource.share.curNodes.count;
        }
        [SDKLibCommand genericOnOffGetWithDestination:kMeshAddress_allNodes retryCount:SigMeshLib.share.dataSource.defaultRetryCount responseMaxCount:responseMaxCount successCallback:^(UInt16 source, UInt16 destination, SigGenericOnOffStatus * _Nonnull responseMessage) {
            if (![rssiArray containsObject:@(source)] && [weakSelf.allAddressArray containsObject:@(source)]) {
                [rssiArray addObject:@(source)];
            }
        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
            TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
            //1.按照回包顺序，越先返回状态的地址越后操作，未返回状态的地址再最前面。
            for (int i=0; i<rssiArray.count; i++) {
                NSNumber *rssiSource = rssiArray[rssiArray.count - 1 - i];
                [weakSelf.allAddressArray removeObject:rssiSource];
                [weakSelf.allAddressArray addObject:rssiSource];
            }
            //2.直连地址应该处于数组最后的位置。
            if ([weakSelf.allAddressArray containsObject:@(SigMeshLib.share.dataSource.unicastAddressOfConnected)]) {
                [weakSelf.allAddressArray removeObject:@(SigMeshLib.share.dataSource.unicastAddressOfConnected)];
                [weakSelf.allAddressArray addObject:@(SigMeshLib.share.dataSource.unicastAddressOfConnected)];
            }
            //新增逻辑：防止上一次meshOTA未完成，设备还处于非idle状态，在该处先查询设备的firmwareUpdate状态，如果phase是非idle状态，则需要先updateCancel，再进行meshOTA流程。
            [weakSelf checkLastFirmwareUpdateStatueWithCompleteHandle:^(BOOL isSuccess) {
                [weakSelf connectMeshOTAByFirmwareUpdateProgress];
            }];
        }];
    }
}

/// 继续MeshOTA，仅用于直连节点作为Distributor时，Distributor处于广播firmware到updating node(s)阶段才可用，其它情况不可继续上一次未完成的meshOTA。
/// @param deviceAddresses 需要升级的设备地址数组
/// @param advDistributionProgressBlock Distributor->updating node(s)阶段升级的进度回调
/// @param finishBlock 升级完成的回调
/// @param errorBlock 升级失败的回调
- (void)continueFirmwareUpdateWithDeviceAddresses:(NSArray <NSNumber *>*)deviceAddresses advDistributionProgressHandle:(ProgressReceiversListBlock)advDistributionProgressBlock finishHandle:(FinishBlock)finishBlock errorHandle:(ErrorBlock)errorBlock {
    TelinkLogVerbose(@"");

    if (_meshOTAThread.isCancelled && !_meshOTAThread.isExecuting) {
        [_meshOTAThread start];
    }
    self.failError = nil;
//    self.distributorAddress = SigMeshLib.share.dataSource.unicastAddressOfConnected;
    self.connectedNodeModel = SigMeshLib.share.dataSource.getCurrentConnectedNode;
    self.advDistributionProgressBlock = advDistributionProgressBlock;
    self.finishBlock = finishBlock;
    self.errorBlock = errorBlock;
    self.firstIndexOfFirmwareDistributionReceiversGet = 0;
    [self saveIsMeshOTAing:YES];
    [self.allAddressArray removeAllObjects];
    [self.successAddressArray removeAllObjects];
    [self.failAddressArray removeAllObjects];
    [self.allAddressArray addObjectsFromArray:deviceAddresses];
    TelinkLogInfo(@"meshOTA地址列表=%@",self.allAddressArray);
    if (SigBearer.share.dataDelegate) {
        self.oldBearerDataDelegate = SigBearer.share.dataDelegate;
    }
    if (SigMeshLib.share.delegateForDeveloper) {
        self.oldDelegateForDeveloper = SigMeshLib.share.delegateForDeveloper;
    }
    SigMeshLib.share.delegateForDeveloper = self;

    SigBearer.share.dataDelegate = self;
    [SDKLibCommand setBluetoothCentralUpdateStateCallback:nil];
    SigBearer.share.isAutoReconnect = NO;
    self.phoneIsDistributor = NO;

    NSMutableArray *rssiArray = [NSMutableArray array];
    __weak typeof(self) weakSelf = self;
    //该逻辑有两个目的：1.防止近的设备先apply而远的设备后apply导致部分设备apply失败。 2.直连节点必须最后apply，，否则在直连节点后面apply的设备先进行重启导致其他未apply的设备接受不到apply指令了。
    if (deviceAddresses.count > 1) {
        UInt16 responseMaxCount = SigMeshLib.share.dataSource.getOnlineDevicesNumber;
        if (responseMaxCount == 0) {
            responseMaxCount = SigDataSource.share.curNodes.count;
        }
        [SDKLibCommand genericOnOffGetWithDestination:kMeshAddress_allNodes retryCount:SigMeshLib.share.dataSource.defaultRetryCount responseMaxCount:responseMaxCount successCallback:^(UInt16 source, UInt16 destination, SigGenericOnOffStatus * _Nonnull responseMessage) {
            if (![rssiArray containsObject:@(source)] && [weakSelf.allAddressArray containsObject:@(source)]) {
                [rssiArray addObject:@(source)];
            }
        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
            TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
            //1.按照回包顺序，越先返回状态的地址越后操作，未返回状态的地址再最前面。
            for (int i=0; i<rssiArray.count; i++) {
                NSNumber *rssiSource = rssiArray[rssiArray.count - 1 - i];
                [weakSelf.allAddressArray removeObject:rssiSource];
                [weakSelf.allAddressArray addObject:rssiSource];
            }
            //2.直连地址应该处于数组最后的位置。
            if ([weakSelf.allAddressArray containsObject:@(SigMeshLib.share.dataSource.unicastAddressOfConnected)]) {
                [weakSelf.allAddressArray removeObject:@(SigMeshLib.share.dataSource.unicastAddressOfConnected)];
                [weakSelf.allAddressArray addObject:@(SigMeshLib.share.dataSource.unicastAddressOfConnected)];
            }
            [weakSelf performSelector:@selector(fiemwareUpdateFirmwareDistributionReceiversGet) onThread:weakSelf.meshOTAThread withObject:nil waitUntilDone:NO];
        }];
    } else {
        [self performSelector:@selector(fiemwareUpdateFirmwareDistributionReceiversGet) onThread:self.meshOTAThread withObject:nil waitUntilDone:NO];
    }
}

/// Stop Firmware Update
/// @param completeBlock The handle of stop Firmware Update finish.
- (void)stopFirmwareUpdateWithCompleteHandle:(CompleteBlock)completeBlock {
    TelinkLogVerbose(@"");
    if (self.oldBearerDataDelegate) {
        SigBearer.share.dataDelegate = self.oldBearerDataDelegate;
    }
    if (self.oldDelegateForDeveloper) {
        SigMeshLib.share.delegateForDeveloper = self.oldDelegateForDeveloper;
    }

    [self showMeshOTAResult];
    [self recoveryExtendBearerMode];
    SigBearer.share.isAutoReconnect = YES;
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self];
    });
    [self resetInitiatorMeshOTAData];

    [self saveIsMeshOTAing:NO];
    if (self.semaphore) {
        dispatch_semaphore_signal(self.semaphore);
    }
    [self.meshOTAThread cancel];
    [SigMeshLib.share cancelSigMessageHandle:self.messageHandle];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(fiemwareUpdateFirmwareDistributionReceiversGet) object:nil];
    });
}

- (void)connectMeshOTAByFirmwareUpdateProgress {
    TelinkLogInfo(@"");
    switch (self.firmwareUpdateProgress) {
        case SigFirmwareUpdateProgressCheckLastFirmwareUpdateStatue:
        {
            if (self.phoneIsDistributor) {
                //如果外部配置设置phone作为distributor，则直接跳过DistributionCapabilitiesGet流程。
                TelinkLogVerbose(@"外部配置设置phone作为distributor，则直接跳过DistributionCapabilitiesGet流程!");
                self.phoneIsDistributor = YES;
                self.distributorAddress = SigMeshLib.share.dataSource.curLocationNodeModel.address;
                [self performSelector:@selector(firmwareUpdateFirmwareDistributionCapabilitiesGetSuccessAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:NO];
            } else {
                [self performSelector:@selector(firmwareUpdateFirmwareDistributionCapabilitiesGet) onThread:self.meshOTAThread withObject:nil waitUntilDone:NO];
            }
        }
            break;
        case SigFirmwareUpdateProgressDistributorToUpdatingNodesBLOBTransferGet:
        case SigFirmwareUpdateProgressDistributorToUpdatingNodesBLOBTransferStart:
        case SigFirmwareUpdateProgressDistributorToUpdatingNodesBLOBBlockStart:
        case SigFirmwareUpdateProgressDistributorToUpdatingNodesBLOBBlockGet:
        case SigFirmwareUpdateProgressFirmwareDistributionReceiversGet:
        {
            [self performSelector:@selector(fiemwareUpdateFirmwareDistributionReceiversGet) onThread:self.meshOTAThread withObject:nil waitUntilDone:NO];
        }
            break;
        case SigFirmwareUpdateProgressDistributorToUpdatingNodesBLOBChunkTransfer:
        {
            if (self.phoneIsDistributor) {
                [self performSelector:@selector(distributorToUpdatingNodesBLOBChunkTransfer) onThread:self.meshOTAThread withObject:nil waitUntilDone:NO];
            } else {
                [self performSelector:@selector(fiemwareUpdateFirmwareDistributionReceiversGet) onThread:self.meshOTAThread withObject:nil waitUntilDone:NO];
            }
        }
            break;
        case SigFirmwareUpdateProgressInitiatorToDistributorBLOBBlockStart:
        {
                [self performSelector:@selector(initiatorToDistributorBLOBBlockStart) onThread:self.meshOTAThread withObject:nil waitUntilDone:NO];
        }
            break;
        case SigFirmwareUpdateProgressInitiatorToDistributorBLOBChunkTransfer:
        {
            [self performSelector:@selector(initiatorToDistributorBLOBChunkTransfer) onThread:self.meshOTAThread withObject:nil waitUntilDone:NO];
        }
            break;
        case SigFirmwareUpdateProgressInitiatorToDistributorBLOBBlockGet:
        {
            [self performSelector:@selector(initiatorToDistributorBLOBBlockGet) onThread:self.meshOTAThread withObject:nil waitUntilDone:NO];
        }
            break;
        case SigFirmwareUpdateProgressFirmwareDistributionGet:
        {
            [self performSelector:@selector(firmwareUpdateFirmwareDistributionGet) onThread:self.meshOTAThread withObject:nil waitUntilDone:NO];
        }
        case SigFirmwareUpdateInformationGetCheckVersion:
        {
            [self performSelector:@selector(firmwareUpdateInformationGetCheckVersion) onThread:self.meshOTAThread withObject:nil waitUntilDone:NO];
        }
            break;
        default:
            break;
    }
}

#pragma mark - Firmware update step1:checkLastFirmwareUpdateStatue(Initiator->Distributor,发送到直连节点)
- (void)checkLastFirmwareUpdateStatueWithCompleteHandle:(CompleteBlock)completeBlock {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressCheckLastFirmwareUpdateStatue;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);

    __weak typeof(self) weakSelf = self;
    NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
    [operationQueue addOperationWithBlock:^{
        weakSelf.semaphore = dispatch_semaphore_create(0);
        [SDKLibCommand configNetworkTransmitGetWithDestination:SigMeshLib.share.dataSource.unicastAddressOfConnected retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigNetworkTransmitStatus * _Nonnull responseMessage) {
            TelinkLogDebug(@"configNetworkTransmitGet(Initiator->Distributor)=%@,source=0x%x,destination=0x%x,count=0x%x,steps=0x%x",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination,responseMessage.count,responseMessage.steps);
            SigMeshLib.share.networkTransmitCount = responseMessage.count;
            SigMeshLib.share.networkTransmitIntervalSteps = responseMessage.steps;
        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
            TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
            dispatch_semaphore_signal(weakSelf.semaphore);
        }];
        dispatch_semaphore_wait(weakSelf.semaphore, dispatch_semaphore_wait(weakSelf.semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 10.0)));

        NSArray *allAddressArray = [NSArray arrayWithArray:self.allAddressArray];
        NSMutableArray *cancelArray = [NSMutableArray array];
        for (NSNumber *nodeAddress in allAddressArray) {
            if (![weakSelf isMeshOTAing]) {
                return;
            }
            if ([weakSelf.failAddressArray containsObject:nodeAddress]) {
                continue;
            }
            SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:nodeAddress.intValue];
            if (node.state != DeviceStateOutOfLine || node.features.lowPowerFeature != SigNodeFeaturesState_notSupported) {
                weakSelf.semaphore = dispatch_semaphore_create(0);
                weakSelf.messageHandle = [SDKLibCommand firmwareUpdateGetWithDestination:node.address retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareUpdateStatus * _Nonnull responseMessage) {
                    TelinkLogDebug(@"firmwareUpdateGet(Initiator->Distributor)=%@,source=0x%x,destination=0x%x",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                    if (source == nodeAddress.intValue && responseMessage.status == SigFirmwareUpdateServerAndClientModelStatusType_success && responseMessage.updatePhase != SigFirmwareUpdatePhaseType_idle) {
                        [cancelArray addObject:nodeAddress];
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    dispatch_semaphore_signal(weakSelf.semaphore);
                }];
                //Most provide 3 seconds to firmwareUpdateGet every node.
                dispatch_semaphore_wait(weakSelf.semaphore, kTimeOutOfEveryStep);
            }
        }
        //下一版本再讨论完善这个Cancel流程
//        __block BOOL needCancelDistribution = NO;
//        weakSelf.semaphore = dispatch_semaphore_create(0);
//        weakSelf.messageHandle = [SDKLibCommand firmwareDistributionGetWithDestination:SigDataSource.share.unicastAddressOfConnected retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareDistributionStatus * _Nonnull responseMessage) {
//            TelinkLogDebug(@"firmwareDistributionGet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
//            if (responseMessage.distributionPhase != SigDistributionPhaseState_idle) {
//                needCancelDistribution = YES;
//            }
//        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
//            TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
//            dispatch_semaphore_signal(weakSelf.semaphore);
//        }];
//        dispatch_semaphore_wait(weakSelf.semaphore, kTimeOutOfEveryStep);
        BOOL reBoot = NO;
        for (NSNumber *nodeAddress in cancelArray) {
            UInt16 address = nodeAddress.intValue;
            if (address == SigDataSource.share.unicastAddressOfConnected) {
                reBoot = YES;
            }
            weakSelf.semaphore = dispatch_semaphore_create(0);
            TelinkLogInfo(@"firmwareUpdateCancel=0x%x",address);
            weakSelf.messageHandle = [SDKLibCommand firmwareUpdateCancelWithDestination:address retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareUpdateStatus * _Nonnull responseMessage) {
                TelinkLogDebug(@"firmwareUpdateCancel(Distributor->updating node(s))=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                dispatch_semaphore_signal(weakSelf.semaphore);
            }];
            //Most provide 3 seconds to firmwareUpdateCancel(Distributor->updating node(s)) every node.
            dispatch_semaphore_wait(weakSelf.semaphore, kTimeOutOfEveryStep);
        }
        //下一版本再讨论完善这个Cancel流程
//        if (needCancelDistribution) {
//            reBoot = YES;
//            weakSelf.semaphore = dispatch_semaphore_create(0);
//            TelinkLogInfo(@"firmwareDistributionCancel=0x%x",SigDataSource.share.unicastAddressOfConnected);
//            weakSelf.messageHandle = [SDKLibCommand firmwareDistributionCancelWithDestination:SigDataSource.share.unicastAddressOfConnected retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareDistributionStatus * _Nonnull responseMessage) {
//                TelinkLogDebug(@"firmwareDistributionCancel=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
//            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
//                TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
//                dispatch_semaphore_signal(weakSelf.semaphore);
//            }];
//            dispatch_semaphore_wait(weakSelf.semaphore, kTimeOutOfEveryStep);
//        }
//        if (cancelArray.count || needCancelDistribution) {
        if (cancelArray.count) {
            if (reBoot) {
                //设备端需要10秒钟的时间进行重启，重启后在断开连接处进行重连操作。

            } else {
                //设备端需要10秒钟的时间进行重启，但直连节点未断开，10秒钟后继续下一步。
                dispatch_async(dispatch_get_main_queue(), ^{
                    [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(connectMeshOTAByFirmwareUpdateProgress) object:nil];
                    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(10.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                        [weakSelf performSelector:@selector(connectMeshOTAByFirmwareUpdateProgress) onThread:weakSelf.meshOTAThread withObject:nil waitUntilDone:NO];
                    });
                });
            }
        } else {
            if (completeBlock) {
                completeBlock(YES);
            }
        }
    }];
}

// 8.7.3 Initiator updating the receivers list for a firmware update(Figure 8.9, step1)

#pragma mark - Firmware update step2:firmwareDistributionCapabilitiesGet(Initiator->Distributor,发送到直连节点)
- (void)firmwareUpdateFirmwareDistributionCapabilitiesGet {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressFirmwareDistributionCapabilitiesGet;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);

    if (self.gattDistributionProgressBlock) {
        self.gattDistributionProgressBlock(0);
    }

    BOOL needCheckDistributionCapabilities = NO;
    if (self.phoneIsDistributor == NO) {
        NSArray *allNode = [NSArray arrayWithArray:self.allAddressArray];
        for (NSNumber *addressNumber in allNode) {
            if (addressNumber.intValue != SigDataSource.share.getCurrentConnectedNode.address) {
                needCheckDistributionCapabilities = YES;
                break;
            }
        }
    }
    if (needCheckDistributionCapabilities) {
        SigNodeModel *node = SigDataSource.share.getCurrentConnectedNode;
        SigModelIDModel *modelId = [node getModelIDModelWithModelID:kSigModel_FirmwareDistributionServer_ID];
        if (modelId == nil) {
            [self createErrorWithString:@"distributor have not FirmwareDistributionServer_ID"];
            [self firmwareUpdateFirmwareDistributionCapabilitiesGetFailAction];
        } else {
            __block BOOL hasFail = NO;
            self.successActionInCurrentProgress = 0;
            __weak typeof(self) weakSelf = self;
            self.semaphore = dispatch_semaphore_create(0);
            [SDKLibCommand firmwareDistributionCapabilitiesGetWithDestination:self.distributorAddress retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareDistributionCapabilitiesStatus * _Nonnull responseMessage) {
                TelinkLogDebug(@"initiator firmwareDistributionCapabilitiesGet=%@,source=0x%x,destination=0x%x",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                if (error) {
                    hasFail = YES;
                    [weakSelf createErrorWithString:[NSString stringWithFormat:@"The distributorAddress=0x%04X has not response SigFirmwareDistributionCapabilitiesStatus", weakSelf.distributorAddress]];
                }
                dispatch_semaphore_signal(weakSelf.semaphore);
            }];
            //Most provide 3 seconds to firmwareDistributionCapabilitiesGet every node.
            dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);

            //新做法：直连节点不支持distributor，提示meshOTA失败
            if (hasFail) {
                [self firmwareUpdateFirmwareDistributionCapabilitiesGetFailAction];
            } else {
                self.phoneIsDistributor = NO;
                self.distributorAddress = SigMeshLib.share.dataSource.unicastAddressOfConnected;
                self.connectedNodeModel = SigMeshLib.share.dataSource.getCurrentConnectedNode;
                [self firmwareUpdateFirmwareDistributionCapabilitiesGetSuccessAction];
            }
        }
    } else {
        //待验证
        self.distributorAddress = SigMeshLib.share.dataSource.unicastAddressOfConnected;
        self.connectedNodeModel = SigMeshLib.share.dataSource.getCurrentConnectedNode;
        [self firmwareUpdateFirmwareDistributionCapabilitiesGetSuccessAction];
    }
}

- (void)firmwareUpdateFirmwareDistributionCapabilitiesGetSuccessAction {
    [self performSelector:@selector(firmwareUpdateFirmwareUpdateInformationGet) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

- (void)firmwareUpdateFirmwareDistributionCapabilitiesGetFailAction {
    [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

// 8.7.3 Initiator updating the receivers list for a firmware update(Figure 8.9, step2)

#pragma mark - Firmware update step3:firmwareUpdateInformationGet(Initiator->updating node(s))
- (void)firmwareUpdateFirmwareUpdateInformationGet {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressFirmwareUpdateInformationGet;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);

    __block BOOL hasSuccess = NO;
    self.oFirmwareInformation = [NSMutableDictionary dictionary];
    self.nFirmwareInformation = [NSMutableDictionary dictionary];
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
        if (node.state != DeviceStateOutOfLine || node.features.lowPowerFeature != SigNodeFeaturesState_notSupported) {
            self.semaphore = dispatch_semaphore_create(0);
            self.messageHandle = [SDKLibCommand firmwareUpdateInformationGetWithDestination:node.address firstIndex:0 entriesLimit:1 retryCount:SigMeshLib.share.dataSource.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareUpdateInformationStatus * _Nonnull responseMessage) {
                if (source == nodeAddress.intValue && responseMessage.firmwareInformationListCount > 0) {
                    /*
                     responseMessage.firmwareInformationList.firstObject.currentFirmwareID.length = 4: 2 bytes pid(设备类型) + 2 bytes vid(版本id).
                     */
                    if (responseMessage.firmwareInformationList.count > 0) {
                        NSData *currentFirmwareID = responseMessage.firmwareInformationList.firstObject.currentFirmwareID;
                        weakSelf.firmwareID = currentFirmwareID;
                        UInt16 pid = 0,vid = 0;
                        Byte *pu = (Byte *)[currentFirmwareID bytes];
                        if (currentFirmwareID.length >= 2) memcpy(&pid, pu, 2);
                        if (currentFirmwareID.length >= 4) memcpy(&vid, pu + 2, 2);
                        TelinkLogDebug(@"initiator firmwareUpdateInformationGet=%@,pid=%d,vid=%d",[LibTools convertDataToHexStr:currentFirmwareID],pid,vid);
                        weakSelf.oFirmwareInformation[@(source)] = @{kPid:@(pid),kVid:@(vid)};
                        if ([LibTools uint16From16String:node.vid] != vid) {
                            [SigDataSource.share updateNodeModelVidWithAddress:source vid:vid];
                        }
                        hasSuccess = YES;
                    }
                } else {
                    if (![weakSelf.failAddressArray containsObject:nodeAddress]) {
                        [weakSelf.failAddressArray addObject:nodeAddress];
                    }
                }
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                dispatch_semaphore_signal(weakSelf.semaphore);
            }];
            //Most provide 3 seconds to firmwareUpdateInformationGet every node.
            dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
        } else {
            [self createErrorWithString:[NSString stringWithFormat:@"node=0x%4X is offline", nodeAddress.intValue]];
            if (![self.failAddressArray containsObject:nodeAddress]) {
                [self.failAddressArray addObject:nodeAddress];
            }
        }
    }

    if (hasSuccess) {
        [self firmwareUpdateFirmwareUpdateInformationGetSuccessAction];
    } else {
        if (!self.failError) {
            [self createErrorWithString:@"All nodes have not response SigFirmwareUpdateInformationStatus"];
        }
        [self firmwareUpdateFirmwareUpdateInformationGetFailAction];
    }
}

- (void)firmwareUpdateFirmwareUpdateInformationGetSuccessAction {
    [self performSelector:@selector(firmwareUpdateFirmwareMetadataCheck) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

- (void)firmwareUpdateFirmwareUpdateInformationGetFailAction {
    [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

// 8.7.3 Initiator updating the receivers list for a firmware update(Figure 8.9, step3)

#pragma mark - Firmware update step4:firmwareUpdateFirmwareMetadataCheck(Initiator->updating node(s))
- (void)firmwareUpdateFirmwareMetadataCheck {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressFirmwareUpdateFirmwareMetadataCheck;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);

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
        if (node.state != DeviceStateOutOfLine || node.features.lowPowerFeature != SigNodeFeaturesState_notSupported) {
            self.semaphore = dispatch_semaphore_create(0);
            TelinkLogInfo(@"firmwareUpdateFirmwareMetadataCheckWithDestination=0x%x",node.address);
            self.messageHandle = [SDKLibCommand firmwareUpdateFirmwareMetadataCheckWithDestination:node.address updateFirmwareImageIndex:self.updateFirmwareImageIndex incomingFirmwareMetadata:self.incomingFirmwareMetadata retryCount:SigMeshLib.share.dataSource.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareUpdateFirmwareMetadataStatus * _Nonnull responseMessage) {
                TelinkLogDebug(@"firmwareUpdateFirmwareMetadataCheck=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                if (source == nodeAddress.intValue) {
                    if (responseMessage.status == SigFirmwareUpdateServerAndClientModelStatusType_success) {
                        weakSelf.additionalInformationDictionary[@(source)] = @(responseMessage.additionalInformation);
//                            if (responseMessage.additionalInformation == SigFirmwareUpdateAdditionalInformationStatusType_noChangeCompositionData) {
                            hasSuccess = YES;
//                            } else {
//                                hasFail = YES;
//                                weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateFirmwareMetadataCheck,SigFirmwareUpdateStatus.phase=0x%x",responseMessage.phase] code:-weakSelf.firmwareUpdateProgress userInfo:nil];
//                            }
                    } else {
                        [weakSelf createErrorWithString:[NSString stringWithFormat:@"SigFirmwareUpdateStatus.status=0x%x, source=0x%04X", responseMessage.status, source]];
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
                TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                dispatch_semaphore_signal(weakSelf.semaphore);
            }];
            //Most provide 3 seconds to firmwareUpdateFirmwareMetadataCheck every node.
            dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
        } else {
            [self createErrorWithString:[NSString stringWithFormat:@"node=0x%4X is offline", nodeAddress.intValue]];
            if (![self.failAddressArray containsObject:nodeAddress]) {
                [self.failAddressArray addObject:nodeAddress];
            }
        }
    }
    if (hasSuccess) {
        [self firmwareUpdateFirmwareMetadataCheckSuccessAction];
    } else {
        if (!self.failError) {
            [self createErrorWithString:@"All nodes have not response SigFirmwareUpdateFirmwareMetadataStatus"];
        }
        [self firmwareUpdateFirmwareMetadataCheckFailAction];
    }
}

- (void)firmwareUpdateFirmwareMetadataCheckSuccessAction {
    if (self.firmwareUpdateFirmwareMetadataCheckSuccessHandle) {
        self.firmwareUpdateFirmwareMetadataCheckSuccessHandle(self.additionalInformationDictionary);
    }
    [self performSelector:@selector(firmwareUpdateConfigModelSubscriptionAdd) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

- (void)firmwareUpdateFirmwareMetadataCheckFailAction {
    [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

// 8.1.3 Firmware Update process overview
/*Before initializing the Initiator, the Mesh Manager subscribes the models of the Updating nodes running Firmware X v1.0 to a multicast address.*/

#pragma mark - Firmware update step5:configModelSubscriptionAdd(Initiator->updating node(s))
- (void)firmwareUpdateConfigModelSubscriptionAdd {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressSubscriptionAdd;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);

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
        if (node.state != DeviceStateOutOfLine || node.features.lowPowerFeature != SigNodeFeaturesState_notSupported) {
            UInt16 modelIdentifier = kSigModel_BLOBTransferServer_ID;
            UInt16 companyIdentifier = 0;
            UInt16 groupAddress = kMeshOTAGroupAddress;
            NSArray *addressArray = [node getAddressesWithModelID:@(modelIdentifier)];
            if (addressArray && addressArray.count > 0) {
                UInt16 eleAddress = [addressArray.firstObject intValue];
                self.semaphore = dispatch_semaphore_create(0);
                self.messageHandle = [SDKLibCommand configModelSubscriptionAddWithDestination:node.address toGroupAddress:groupAddress elementAddress:eleAddress modelIdentifier:modelIdentifier companyIdentifier:companyIdentifier retryCount:SigMeshLib.share.dataSource.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigModelSubscriptionStatus * _Nonnull responseMessage) {
                    if (responseMessage.elementAddress == eleAddress && responseMessage.address == groupAddress) {
                        TelinkLogDebug(@"initiator configModelSubscriptionAdd=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                        if (responseMessage.companyIdentifier == companyIdentifier && responseMessage.modelIdentifier == modelIdentifier) {
                            if (responseMessage.status == SigConfigMessageStatus_success) {
                                hasSuccess = YES;
                            } else {
                                [weakSelf createErrorWithString:[NSString stringWithFormat:@"SigConfigModelSubscriptionStatus.status=0x%x, source=0x%04X", responseMessage.status, source]];
                                TelinkLogError(@"fail in configModelSubscriptionAdd,SigConfigModelSubscriptionStatus.status=0x%x",responseMessage.status);
                                if (![weakSelf.failAddressArray containsObject:nodeAddress]) {
                                    [weakSelf.failAddressArray addObject:nodeAddress];
                                }
                            }
                        }
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    dispatch_semaphore_signal(weakSelf.semaphore);
                }];
                //Most provide 3 seconds to configModelSubscriptionAdd every node.
                dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
            } else {
                [self createErrorWithString:[NSString stringWithFormat:@"node=0x%04X has not modelID = BLOBTransferServer_ID", node.address]];
                if (![self.failAddressArray containsObject:nodeAddress]) {
                    [self.failAddressArray addObject:nodeAddress];
                }
            }
        } else {
            [self createErrorWithString:[NSString stringWithFormat:@"node=0x%4X is offline", nodeAddress.intValue]];
            if (![self.failAddressArray containsObject:nodeAddress]) {
                [self.failAddressArray addObject:nodeAddress];
            }
        }
    }
    if (hasSuccess) {
        [self firmwareUpdateConfigModelSubscriptionAddSuccessAction];
    } else {
        if (!self.failError) {
            [self createErrorWithString:@"All nodes have not response SigConfigModelSubscriptionStatus"];
        }
        [self firmwareUpdateConfigModelSubscriptionAddFailAction];
    }
}

- (void)firmwareUpdateConfigModelSubscriptionAddSuccessAction {
    [self performSelector:@selector(firmwareUpdateFirmwareDistributionReceiversAdd) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

- (void)firmwareUpdateConfigModelSubscriptionAddFailAction {
    [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

// 8.7.3 Initiator updating the receivers list for a firmware update(Figure 8.9, step4)
// 8.6.2.2.4 Populate Distribution Receivers List procedure

#pragma mark - Firmware update step6:firmwareDistributionReceiversAdd(Initiator->Distributor,发送到直连节点，需模拟回包)
- (void)firmwareUpdateFirmwareDistributionReceiversAdd {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressFirmwareDistributionReceiversAdd;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);

    __weak typeof(self) weakSelf = self;
    __block BOOL hasFail = NO;

    //1.get all old ReceiversList
    self.semaphore = dispatch_semaphore_create(0);
    __block SigFirmwareDistributionReceiversList *response = nil;
    [SDKLibCommand firmwareDistributionReceiversGetWithDestination:self.distributorAddress firstIndex:0 entriesLimit:1 retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareDistributionReceiversList * _Nonnull responseMessage) {
        TelinkLogDebug(@"firmwareDistributionReceiversGet=%@,source=0x%x,destination=0x%x",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
        response = responseMessage;
        weakSelf.firmwareDistributionReceiversList = responseMessage;
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        if (error) {
            hasFail = YES;
            [weakSelf createErrorWithString:[NSString stringWithFormat:@"The distributorAddress=0x%04X has not response SigFirmwareDistributionReceiversList", weakSelf.distributorAddress]];
        }
        dispatch_semaphore_signal(weakSelf.semaphore);
    }];
    if (self.phoneIsDistributor) {
        //手机端既作为initiator，也作为Distributor。
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(kCreateSimulatedMessageInterval * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            [self createResponseSigFirmwareDistributionReceiversListOfAllAddressList];
        });
    }
    //Most provide 3 seconds to firmwareDistributionReceiversGet every node.
    dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);

    //2. delete all ReceiversList
    if (response.receiversList && response.receiversList.count > 0) {
        self.semaphore = dispatch_semaphore_create(0);
        [SDKLibCommand firmwareDistributionReceiversDeleteAllWithDestination:self.distributorAddress retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareDistributionReceiversStatus * _Nonnull responseMessage) {
            TelinkLogDebug(@"firmwareDistributionReceiversDeleteAll=%@,source=0x%x,destination=0x%x",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
            if (source != weakSelf.distributorAddress) {
                TelinkLogInfo(@"response from other node.");
            } else {
                if (responseMessage.status != SigFirmwareDistributionServerAndClientModelStatusType_success) {
                    hasFail = YES;
                    [weakSelf createErrorWithString:[NSString stringWithFormat:@"SigFirmwareDistributionReceiversStatus.status=0x%x, source=0x%04X", responseMessage.status, source]];
                }
            }
        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
            TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
            if (error) {
                hasFail = YES;
                [weakSelf createErrorWithString:[NSString stringWithFormat:@"The distributorAddress=0x%04X has not response SigFirmwareDistributionReceiversStatus", weakSelf.distributorAddress]];
            }
            dispatch_semaphore_signal(weakSelf.semaphore);
        }];
        if (self.phoneIsDistributor) {
            //手机端既作为initiator，也作为Distributor。
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(kCreateSimulatedMessageInterval * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                [self createResponseSigFirmwareDistributionReceiversStatus];
            });
        }
        //Most provide 3 seconds to firmwareDistributionReceiversDeleteAll every node.
        dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
    }
    if (hasFail) {
        [self firmwareUpdateFirmwareDistributionReceiversAddFailAction];
        return;
    }

    //3. add new ReceiversList
    __block BOOL hasSuccess = NO;
    NSArray *allAddressArray = [NSArray arrayWithArray:self.allAddressArray];
    for (NSNumber *nodeAddress in allAddressArray) {
        if (![self isMeshOTAing]) {
            return;
        }
        if ([self.failAddressArray containsObject:nodeAddress]) {
            continue;
        }
        SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:nodeAddress.intValue];
        if (node.state != DeviceStateOutOfLine || node.features.lowPowerFeature != SigNodeFeaturesState_notSupported) {
            self.semaphore = dispatch_semaphore_create(0);
            TelinkLogInfo(@"firmwareDistributionReceiversAdd=0x%x",node.address);
            SigReceiverEntryModel *model = [[SigReceiverEntryModel alloc] initWithAddress:nodeAddress.intValue updateFirmwareImageIndex:self.updateFirmwareImageIndex];
            self.messageHandle = [SDKLibCommand firmwareDistributionReceiversAddWithDestination:self.distributorAddress receiverEntriesList:@[model] retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareDistributionReceiversStatus * _Nonnull responseMessage) {
                TelinkLogDebug(@"firmwareDistributionReceiversAdd=%@,source=0x%x,destination=0x%x",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                if (source == weakSelf.distributorAddress) {
                    if (responseMessage.status == SigFirmwareDistributionServerAndClientModelStatusType_success) {
                        hasSuccess = YES;
                    } else {
                        hasFail = YES;
                        [weakSelf createErrorWithString:[NSString stringWithFormat:@"SigFirmwareDistributionReceiversStatus.status=0x%x, source=0x%04X", responseMessage.status, source]];
                        TelinkLogInfo(@"%@",[NSString stringWithFormat:@"SigFirmwareDistributionReceiversStatus.status=0x%x, source=0x%04X", responseMessage.status, source]);
                    }
                } else {
                    TelinkLogInfo(@"response from other node.");
                }
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                if (error) {
                    hasFail = YES;
                    [weakSelf createErrorWithString:[NSString stringWithFormat:@"The distributorAddress=0x%04X has not response SigFirmwareDistributionReceiversStatus", weakSelf.distributorAddress]];
                }
                dispatch_semaphore_signal(weakSelf.semaphore);
            }];
            if (self.phoneIsDistributor) {
                //手机端既作为initiator，也作为Distributor。
                dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(kCreateSimulatedMessageInterval * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                    [self createResponseSigFirmwareDistributionReceiversStatus];
                });
            }
            //Most provide 3 seconds to firmwareDistributionReceiversAdd every node.
            dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
        } else {
            [self createErrorWithString:[NSString stringWithFormat:@"node=0x%4X is offline", nodeAddress.intValue]];
            if (![self.failAddressArray containsObject:nodeAddress]) {
                [self.failAddressArray addObject:nodeAddress];
            }
        }
    }
    if (hasSuccess) {
        [self firmwareUpdateFirmwareDistributionReceiversAddSuccessAction];
    } else {
        if (!self.failError) {
            [self createErrorWithString:@"All nodes have not response SigFirmwareDistributionReceiversStatus"];
        }
        [self firmwareUpdateFirmwareDistributionReceiversAddFailAction];
    }
}

- (void)createResponseSigFirmwareDistributionReceiversListOfAllAddressList {
    TelinkLogInfo(@"create SigFirmwareDistributionReceiversList of AllAddresses by app.");
    NSMutableArray *list = [NSMutableArray array];
    NSArray *allAddressArray = [NSArray arrayWithArray:self.allAddressArray];
    UInt16 receiversListCount = allAddressArray.count;
    UInt16 firstIndex = 0;
    for (NSNumber *nodeAddress in allAddressArray) {
        SigUpdatingNodeEntryModel *model = [[SigUpdatingNodeEntryModel alloc] initWithAddress:nodeAddress.intValue retrievedUpdatePhase:SigFirmwareUpdatePhaseType_idle updateStatus:SigFirmwareUpdateServerAndClientModelStatusType_success transferStatus:SigBLOBTransferStatusType_success transferProgress:0 updateFirmwareImageIndex:0];
        [list addObject:model];
    }
    SigFirmwareDistributionReceiversList *responseMessage = [[SigFirmwareDistributionReceiversList alloc] initWithReceiversListCount:receiversListCount firstIndex:firstIndex receiversList:list];
    dispatch_async(SigMeshLib.share.delegateQueue, ^{
        if ([SigMeshLib.share.delegate respondsToSelector:@selector(didReceiveMessage:sentFromSource:toDestination:)]) {
            [SigMeshLib.share.delegate didReceiveMessage:responseMessage sentFromSource:self.distributorAddress toDestination:SigMeshLib.share.dataSource.curLocationNodeModel.address];
        }
    });
}

- (void)createResponseSigFirmwareDistributionReceiversStatus {
    TelinkLogInfo(@"create SigFirmwareDistributionReceiversStatus by app.");
    SigFirmwareDistributionServerAndClientModelStatusType status = SigFirmwareDistributionServerAndClientModelStatusType_success;
    UInt16 receiversListCount = 1;
    SigFirmwareDistributionReceiversStatus *responseMessage = [[SigFirmwareDistributionReceiversStatus alloc] initWithStatus:status receiversListCount:receiversListCount];
    dispatch_async(SigMeshLib.share.delegateQueue, ^{
        if ([SigMeshLib.share.delegate respondsToSelector:@selector(didReceiveMessage:sentFromSource:toDestination:)]) {
            [SigMeshLib.share.delegate didReceiveMessage:responseMessage sentFromSource:self.distributorAddress toDestination:SigMeshLib.share.dataSource.curLocationNodeModel.address];
        }
    });
}

- (void)firmwareUpdateFirmwareDistributionReceiversAddSuccessAction {
    [self performSelector:@selector(firmwareUpdateFirmwareDistributionUploadStart) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

- (void)firmwareUpdateFirmwareDistributionReceiversAddFailAction {
    [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

// 8.7.4 Managing firmware images on the Distributor(Figure 8.10, step1)

#pragma mark - Firmware update step7:firmwareDistributionUploadStart(Initiator->Distributor,发送到直连节点，需模拟回包)
- (void)firmwareUpdateFirmwareDistributionUploadStart {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressFirmwareDistributionUploadStart;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);

    __block BOOL hasFail = NO;
    self.successActionInCurrentProgress = 0;
    __weak typeof(self) weakSelf = self;

    self.semaphore = dispatch_semaphore_create(0);
    [SDKLibCommand firmwareDistributionUploadStartWithDestination:self.distributorAddress uploadTTL:self.uploadTTL uploadTimeoutBase:self.uploadTimeoutBase uploadBLOBID:self.updateBLOBID uploadFirmwareSize:(UInt32)self.otaData.length uploadFirmwareMetadataLength:self.incomingFirmwareMetadata.length uploadFirmwareMetadata:self.incomingFirmwareMetadata uploadFirmwareID:self.firmwareID retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareDistributionUploadStatus * _Nonnull responseMessage) {
        TelinkLogDebug(@"firmwareDistributionUploadStart=%@,source=0x%x,destination=0x%x",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
        if (source == weakSelf.distributorAddress) {
            if (responseMessage.status == SigFirmwareDistributionServerAndClientModelStatusType_success) {

            } else {
                hasFail = YES;
                [weakSelf createErrorWithString:[NSString stringWithFormat:@"SigFirmwareDistributionUploadStatus.status=0x%x, source=0x%04X", responseMessage.status, source]];
                TelinkLogInfo(@"%@",[NSString stringWithFormat:@"SigFirmwareDistributionUploadStatus.status=0x%x, source=0x%04X", responseMessage.status, source]);
            }
        } else {
            TelinkLogInfo(@"response from other node.");
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        if (error) {
            hasFail = YES;
            [weakSelf createErrorWithString:[NSString stringWithFormat:@"The distributorAddress=0x%04X has not response SigFirmwareDistributionUploadStatus", weakSelf.distributorAddress]];
        }
        dispatch_semaphore_signal(weakSelf.semaphore);
    }];
    if (self.phoneIsDistributor) {
        //手机端既作为initiator，也作为Distributor。
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(kCreateSimulatedMessageInterval * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            [self createResponseSigFirmwareDistributionUploadStatus];
        });
    }
    //Most provide 3 seconds to firmwareDistributionUploadStart every node.
    dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
    if (hasFail) {
        [self firmwareUpdateFirmwareDistributionUploadStartFailAction];
    } else {
        [self firmwareUpdateFirmwareDistributionUploadStartSuccessAction];
    }
}

- (void)createResponseSigFirmwareDistributionUploadStatus {
    TelinkLogInfo(@"create SigFirmwareDistributionUploadStatus by app.");
    SigFirmwareDistributionServerAndClientModelStatusType status = SigFirmwareDistributionServerAndClientModelStatusType_success;
    SigFirmwareUploadPhaseStateType uploadPhase = SigFirmwareUploadPhaseStateType_transferActive;
    UInt8 uploadProgress = 0;
    SigFirmwareDistributionUploadStatus *responseMessage = [[SigFirmwareDistributionUploadStatus alloc] initWithStatus:status uploadPhase:uploadPhase uploadProgress:uploadProgress uploadType:NO uploadFirmwareID:self.firmwareID];
    dispatch_async(SigMeshLib.share.delegateQueue, ^{
        if ([SigMeshLib.share.delegate respondsToSelector:@selector(didReceiveMessage:sentFromSource:toDestination:)]) {
            [SigMeshLib.share.delegate didReceiveMessage:responseMessage sentFromSource:self.distributorAddress toDestination:SigMeshLib.share.dataSource.curLocationNodeModel.address];
        }
    });
}

- (void)firmwareUpdateFirmwareDistributionUploadStartSuccessAction {
    [self performSelector:@selector(initiatorToDistributorBLOBTransferGet) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

- (void)firmwareUpdateFirmwareDistributionUploadStartFailAction {
    [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

//参考原来meshOTA的BLOB逻辑

#pragma mark - Firmware update step8:BLOBTransferGet(initiator->Distributor)
- (void)initiatorToDistributorBLOBTransferGet {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressInitiatorToDistributorBLOBTransferGet;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);

    __block BOOL hasSuccess = NO;
    __weak typeof(self) weakSelf = self;
    self.semaphore = dispatch_semaphore_create(0);
    self.messageHandle = [SDKLibCommand BLOBTransferGetWithDestination:self.distributorAddress retryCount:SigMeshLib.share.dataSource.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigBLOBTransferStatus * _Nonnull responseMessage) {
        TelinkLogDebug(@"BLOBTransferGet(initiator->Distributor)=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
        if (source == weakSelf.distributorAddress) {
//            if (responseMessage.status == SigBLOBTransferStatusType_success) {
                hasSuccess = YES;
//            } else {
//                [weakSelf createErrorWithString:[NSString stringWithFormat:@"SigBLOBTransferStatus.status=0x%x, source=0x%04X", responseMessage.status, source]];
//            }
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        if (error) {
            [weakSelf createErrorWithString:[NSString stringWithFormat:@"The distributorAddress=0x%04X has not response SigBLOBTransferStatus", weakSelf.distributorAddress]];
        }
        dispatch_semaphore_signal(weakSelf.semaphore);
    }];
    if (self.phoneIsDistributor) {
        //手机端既作为initiator，也作为Distributor。
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(kCreateSimulatedMessageInterval * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            [self createSigBLOBTransferStatus];
            self.firmwareDataOnDistributor = [NSMutableData data];
        });
    }
    //Most provide 3 seconds to BLOBTransferGet(initiator->Distributor) every node.
    dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
    if (hasSuccess) {
        [self initiatorToDistributorBLOBTransferGetSuccessAction];
    } else {
        [self initiatorToDistributorBLOBTransferGetFailAction];
    }
}

- (void)createSigBLOBTransferStatus {
    TelinkLogInfo(@"create SigBLOBTransferStatus by app.");
    SigBLOBTransferStatusType status = SigBLOBTransferStatusType_success;
    UInt8 RFU = 0;
    SigTransferModeState transferMode = SigTransferModeState_noActiveTransfer;
    SigTransferPhaseState transferPhase = SigTransferPhaseState_inactive;
    UInt32 BLOBSize = 0xFFFFFFFF;
    UInt8 blockSizeLog = 0xFF;
    UInt32 transferMTUSize = 0xFFFFFFFF;
    NSData *blocksNotReceived = [NSData data];
    SigBLOBTransferStatus *responseMessage = [[SigBLOBTransferStatus alloc] initWithStatus:status RFU:RFU transferMode:transferMode transferPhase:transferPhase BLOBID:self.updateBLOBID BLOBSize:BLOBSize blockSizeLog:blockSizeLog transferMTUSize:transferMTUSize blocksNotReceived:blocksNotReceived];
    dispatch_async(SigMeshLib.share.delegateQueue, ^{
        if ([SigMeshLib.share.delegate respondsToSelector:@selector(didReceiveMessage:sentFromSource:toDestination:)]) {
            [SigMeshLib.share.delegate didReceiveMessage:responseMessage sentFromSource:self.distributorAddress toDestination:SigMeshLib.share.dataSource.curLocationNodeModel.address];
        }
    });
}

- (void)initiatorToDistributorBLOBTransferGetSuccessAction {
    [self performSelector:@selector(initiatorToDistributorBLOBInformationGet) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

- (void)initiatorToDistributorBLOBTransferGetFailAction {
    [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

//参考原来meshOTA的BLOB逻辑

#pragma mark - Firmware update step9:BLOBInformationGet(initiator->Distributor)
- (void)initiatorToDistributorBLOBInformationGet {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressInitiatorToDistributorBLOBInformationGet;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);

    __block BOOL hasSuccess = NO;
    self.oBLOBInformations = [NSMutableDictionary dictionary];
    __weak typeof(self) weakSelf = self;
    self.semaphore = dispatch_semaphore_create(0);
    self.messageHandle = [SDKLibCommand BLOBInformationGetWithDestination:self.distributorAddress retryCount:SigMeshLib.share.dataSource.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigBLOBInformationStatus * _Nonnull responseMessage) {
        TelinkLogDebug(@"BLOBInformationGet(initiator->Distributor)=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
        if (source == weakSelf.distributorAddress && [responseMessage isMemberOfClass:[SigBLOBInformationStatus class]]) {
            weakSelf.oBLOBInformations[@(source)] = responseMessage;
            if (responseMessage.supportedTransferMode.PushBLOBTransferModeBit) {
                //注意：当前版本只考虑一个节点的pull，不考虑多个节点的pull。
                if (responseMessage.supportedTransferMode.PullBLOBTransferModeBit && weakSelf.allAddressArray.count == 1) {
                    if (weakSelf.allAddressArray.firstObject.intValue == SigDataSource.share.unicastAddressOfConnected) {
                        weakSelf.transferModeOfDistributor = SigTransferModeState_pushBLOBTransferMode;
                    } else {
                        weakSelf.transferModeOfDistributor = SigTransferModeState_pullBLOBTransferMode;
                    }
                } else {
                    weakSelf.transferModeOfDistributor = SigTransferModeState_pushBLOBTransferMode;
                }
            } else {
                weakSelf.transferModeOfDistributor = SigTransferModeState_pullBLOBTransferMode;
            }
            hasSuccess = YES;
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        if (error) {
            [weakSelf createErrorWithString:[NSString stringWithFormat:@"The distributorAddress=0x%04X has not response SigBLOBInformationStatus", weakSelf.distributorAddress]];
        }
        dispatch_semaphore_signal(weakSelf.semaphore);
    }];
    if (self.phoneIsDistributor) {
        //手机端既作为initiator，也作为Distributor。
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(kCreateSimulatedMessageInterval * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            [self createSigBLOBInformationStatus];
        });
    }
    //Most provide 3 seconds to BLOBTransferGet(initiator->Distributor) every node.
    dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
    if (hasSuccess) {
        if (self.oBLOBInformations && self.oBLOBInformations.count > 0) {
            //v3.3.0暂时逻辑，以最后一个返回的SigBLOBInformationStatus为准发送数据。
            UInt8 maxBlockSizeLog = self.oBLOBInformations.allValues.lastObject.maxBlockSizeLog;
            UInt16 maxChunksNumber = self.oBLOBInformations.allValues.lastObject.maxChunksNumber;
            //直接根据返回的BlockSizeLog和ChunksNumber算出合法的self.blockSizeLog和self.chunkSize
            UInt8 blockSizeLog = maxBlockSizeLog;
            UInt32 blockSize = pow(2, blockSizeLog);
            self.blockSizeLog = blockSizeLog;
            self.chunkSize = ceil(blockSize / (double)maxChunksNumber);
            //存在合法的blockSizeLog和chunkSize，进行下一步流程。
            [self initiatorToDistributorBLOBInformationGetSuccessAction];
        } else {
            [self createErrorWithString:[NSString stringWithFormat:@"BLOBInformation is empty of distributorAddress=0x%04X", weakSelf.distributorAddress]];
            [self initiatorToDistributorBLOBInformationGetFailAction];
        }
    } else {
        [self initiatorToDistributorBLOBInformationGetFailAction];
    }
}

- (void)createSigBLOBInformationStatus {
    TelinkLogInfo(@"create SigBLOBInformationStatus by app.");
    //1212ed04 d0000000 03007c01 01
    UInt8 minBlockSizeLog = 0x12;
    UInt8 maxBlockSizeLog = 0x12;
    UInt16 maxChunksNumber = 0x04ed;
    UInt16 maxChunkSize = 0x00d0;
    UInt32 maxBLOBSize = 0x00030000;
    UInt16 MTUSize = 0x017c;
    struct SigSupportedTransferMode supportedTransferMode;
    supportedTransferMode.PushBLOBTransferModeBit = 1;
    supportedTransferMode.PullBLOBTransferModeBit = 0;
    SigBLOBInformationStatus *responseMessage = [[SigBLOBInformationStatus alloc] initWithMinBlockSizeLog:minBlockSizeLog maxBlockSizeLog:maxBlockSizeLog maxChunksNumber:maxChunksNumber maxChunkSize:maxChunkSize maxBLOBSize:maxBLOBSize MTUSize:MTUSize supportedTransferMode:supportedTransferMode];
    dispatch_async(SigMeshLib.share.delegateQueue, ^{
        if ([SigMeshLib.share.delegate respondsToSelector:@selector(didReceiveMessage:sentFromSource:toDestination:)]) {
            [SigMeshLib.share.delegate didReceiveMessage:responseMessage sentFromSource:self.distributorAddress toDestination:SigMeshLib.share.dataSource.curLocationNodeModel.address];
        }
    });
}

- (void)initiatorToDistributorBLOBInformationGetSuccessAction {
    [self performSelector:@selector(initiatorToDistributorBLOBTransferStart) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

- (void)initiatorToDistributorBLOBInformationGetFailAction {
    [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

//参考原来meshOTA的BLOB逻辑

#pragma mark - Firmware update step10:BLOBTransferStart(initiator->Distributor)
- (void)initiatorToDistributorBLOBTransferStart {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressInitiatorToDistributorBLOBTransferStart;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);

    __block BOOL hasSuccess = NO;
    __weak typeof(self) weakSelf = self;
    self.semaphore = dispatch_semaphore_create(0);
    self.messageHandle = [SDKLibCommand BLOBTransferStartWithDestination:self.distributorAddress transferMode:self.transferModeOfDistributor BLOBID:self.updateBLOBID BLOBSize:(UInt32)self.otaData.length BLOBBlockSizeLog:self.blockSizeLog MTUSize:self.MTUSize retryCount:SigMeshLib.share.dataSource.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigBLOBTransferStatus * _Nonnull responseMessage) {
        TelinkLogDebug(@"BLOBTransferStart(initiator->Distributor)=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
        if (source == weakSelf.distributorAddress) {
//                        if (responseMessage.status == SigBLOBTransferStatusType_busy) {
                hasSuccess = YES;
//                        } else {
//                           hasFail = YES;
//                           weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in BLOBTransferStart,SigBLOBTransferStatus.status=0x%x",responseMessage.status] code:-weakSelf.firmwareUpdateProgress userInfo:nil];
//                        }
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        if (error) {
            [weakSelf createErrorWithString:[NSString stringWithFormat:@"The distributorAddress=0x%04X has not response SigBLOBTransferStatus", weakSelf.distributorAddress]];
        }
        dispatch_semaphore_signal(weakSelf.semaphore);
    }];
    if (self.phoneIsDistributor) {
        //手机端既作为initiator，也作为Distributor。
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(kCreateSimulatedMessageInterval * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            [self createSigBLOBTransferStatus];
        });
    }
    //Most provide 3 seconds to BLOBTransferGet(initiator->Distributor) every node.
    dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
    if (hasSuccess) {
        [self initiatorToDistributorBLOBTransferStartSuccessAction];
    } else {
        [self initiatorToDistributorBLOBTransferStartFailAction];
    }
}

- (void)initiatorToDistributorBLOBTransferStartSuccessAction {
    self.blockIndex = 0;
    double blockSize = (double)pow(2, self.blockSizeLog);
    self.allBlockCount = ceil(self.otaData.length / blockSize);
    self.retryCountInBLOBChunkTransfer = 0;
    [self performSelector:@selector(initiatorToDistributorBLOBBlockStart) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

- (void)initiatorToDistributorBLOBTransferStartFailAction {
    [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

//参考原来meshOTA的BLOB逻辑

#pragma mark - Firmware update step11:BLOBBlockStart(initiator->Distributor)
- (void)initiatorToDistributorBLOBBlockStart {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressInitiatorToDistributorBLOBBlockStart;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);

    __block BOOL hasSuccess = NO;
    __weak typeof(self) weakSelf = self;
    NSInteger blockSize = pow(2, self.blockSizeLog);
    NSInteger currentBlockSize = blockSize;
    if (self.allBlockCount - 1 <= self.blockIndex) {
        //last block
        currentBlockSize = self.otaData.length - blockSize * self.blockIndex;
    }
    self.currentBlockData = [self.otaData subdataWithRange:NSMakeRange(blockSize * self.blockIndex, currentBlockSize)];
    UInt32 blockChecksum = [LibTools getCRC32OfData:self.currentBlockData];
    NSData *blockChecksumValue = [NSData dataWithBytes:&blockChecksum length:4];
    TelinkLogInfo(@"blockChecksum=0x%x,blockChecksumValue=%@",blockChecksum,blockChecksumValue);

    self.semaphore = dispatch_semaphore_create(0);
    self.messageHandle = [SDKLibCommand BLOBBlockStartWithDestination:self.distributorAddress blockNumber:self.blockIndex chunkSize:self.chunkSize retryCount:SigMeshLib.share.dataSource.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigBLOBBlockStatus * _Nonnull responseMessage) {
        TelinkLogDebug(@"BLOBBlockStart(initiator->Distributor)=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
        if (source == weakSelf.distributorAddress) {
            hasSuccess = YES;
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        dispatch_semaphore_signal(weakSelf.semaphore);
    }];
    if (self.phoneIsDistributor) {
        //手机端既作为initiator，也作为Distributor。
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(kCreateSimulatedMessageInterval * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            [self createSigBLOBBlockStatus];
        });
    }
    //Most provide 3 seconds to BLOBTransferGet(initiator->Distributor) every node.
    dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
    if (hasSuccess) {
        [self initiatorToDistributorBLOBBlockStartSuccessAction];
    } else {
//        [self initiatorToDistributorBLOBBlockStartFailAction];
        //这里失败等待重试，而不是直接meshOTA失败
    }
}

- (void)createSigBLOBBlockStatus {
    TelinkLogInfo(@"create SigBLOBBlockStatus by app.");
    //400000d0 00
    SigBLOBBlockStatusType status = SigBLOBBlockStatusType_success;
    UInt8 RFU = 0;
    SigBLOBBlockFormatType format = SigBLOBBlockFormatType_noMissingChunks;
    UInt16 blockNumber = 0x0000;
    UInt16 chunkSize = 0x00d0;
    SigBLOBBlockStatus *responseMessage = [[SigBLOBBlockStatus alloc] initWithStatus:status RFU:RFU format:format blockNumber:blockNumber chunkSize:chunkSize missingChunksList:@[] encodedMissingChunksList:@[]];
    dispatch_async(SigMeshLib.share.delegateQueue, ^{
        if ([SigMeshLib.share.delegate respondsToSelector:@selector(didReceiveMessage:sentFromSource:toDestination:)]) {
            [SigMeshLib.share.delegate didReceiveMessage:responseMessage sentFromSource:self.distributorAddress toDestination:SigMeshLib.share.dataSource.curLocationNodeModel.address];
        }
    });
}

- (void)initiatorToDistributorBLOBBlockStartSuccessAction {
    self.chunkIndex = 0;
    [self performSelector:@selector(initiatorToDistributorBLOBChunkTransfer) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

- (void)initiatorToDistributorBLOBBlockStartFailAction {
    [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

//参考原来meshOTA的BLOB逻辑

#pragma mark - Firmware update step12:BLOBChunkTransfer(initiator->Distributor)
- (void)initiatorToDistributorBLOBChunkTransfer {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressInitiatorToDistributorBLOBChunkTransfer;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);

    self.chunksCountofCurrentBlock = ceil(self.currentBlockData.length / (double)self.chunkSize);
    __block BOOL hasSuccess = NO;

    for (int i = (int)self.chunkIndex; i < self.chunksCountofCurrentBlock; i ++) {
        if (![self isMeshOTAing]) {
            return;
        }
        if (!SigBearer.share.isOpen) {
            return;
        }
        self.chunkIndex = i;
        NSData *chunkData = nil;
        if (i == self.chunksCountofCurrentBlock - 1) {
            //end chunk of current block
            chunkData = [self.currentBlockData subdataWithRange:NSMakeRange(self.chunkSize * i, self.currentBlockData.length - self.chunkSize * i)];
        } else {
            chunkData = [self.currentBlockData subdataWithRange:NSMakeRange(self.chunkSize * i, self.chunkSize)];
        }
        __weak typeof(self) weakSelf = self;
        if (!self.phoneIsDistributor) {
            TelinkLogInfo(@"all Block count=%d,current block index=%d,all chunk count=%d,current chunk index=%d ",self.allBlockCount,self.blockIndex,self.chunksCountofCurrentBlock,self.chunkIndex);
        }
        [self showMeshOTAProgressWithCurrentChunkData:chunkData otaData:self.otaData progressBlock:self.gattDistributionProgressBlock];
        BOOL sendBySegmentPdu = NO;
        if (self.phoneIsDistributor) {
            //手机端既作为initiator，也作为Distributor。
            hasSuccess = YES;
            [weakSelf saveChunkDataOnDistributor:chunkData];
        } else {
            self.semaphore = dispatch_semaphore_create(0);
            self.messageHandle = [SDKLibCommand BLOBChunkTransferWithDestination:self.distributorAddress chunkNumber:self.chunkIndex chunkData:chunkData sendBySegmentPdu:sendBySegmentPdu retryCount:0 responseMaxCount:0 resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                if (error) {
                    dispatch_semaphore_signal(weakSelf.semaphore);
                } else {
                    hasSuccess = YES;
                    dispatch_semaphore_signal(weakSelf.semaphore);
                }
            }];
            //Most provide 24*60*60 seconds for BLOBChunkTransfer(initiator->Distributor) in every chunk.
            dispatch_semaphore_wait(self.semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 24*60*60.0));
        }
        if (!hasSuccess) {
            break;
        }
    }
    if (!hasSuccess) {
        if (SigBearer.share.isOpen) {
            [self createErrorWithString:[NSString stringWithFormat:@"App transfer BLOBChunk to distributorAddress=0x%04X fail", self.distributorAddress]];
            [self initiatorToDistributorBLOBChunkTransferFailAction];
        } else {
            return;
        }
    } else {
        if (self.phoneIsDistributor) {
            [self initiatorToDistributorBLOBChunkTransferSuccessFinishAction];
        } else {
            //使用DLE固件时，APP太快发送BLOBBlockGet，设备端不会回包。添加延时0.5s。
            [self performSelector:@selector(initiatorToDistributorBLOBChunkTransferSuccessFinishAction) withObject:nil afterDelay:0.5];
        }
    }
}

- (void)showMeshOTAProgressWithCurrentChunkData:(NSData *)chunkData otaData:(NSData *)otaData progressBlock:(ProgressBlock)progressBlock {
    if (progressBlock) {
        double progress = [self getProgressWithCurrentChunkData:chunkData otaData:otaData];
        NSInteger intPro = (NSInteger)progress;
//        TelinkLogDebug(@"progress=%f,intPro=%ld",progress,(long)intPro);
        progressBlock(intPro);
    }
}

- (double)getProgressWithCurrentChunkData:(NSData *)chunkData otaData:(NSData *)otaData {
    NSInteger blockSize = pow(2, self.blockSizeLog);
    //旧做法：返回0~99
//    double progress = [LibTools roundFloat:((self.blockIndex * blockSize + self.chunkIndex * self.chunkSize + (double)chunkData.length) / (double)otaData.length)] * 99;
    //新做法：返回1~100
    double progress = [LibTools roundFloat:((self.blockIndex * blockSize + self.chunkIndex * self.chunkSize + (double)chunkData.length) / (double)otaData.length)] * 100;
    return progress;
}

- (void)saveChunkDataOnDistributor:(NSData *)chunkData {
    [self.firmwareDataOnDistributor appendData:chunkData];
}

- (void)initiatorToDistributorBLOBChunkTransferWithLosePackets {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressInitiatorToDistributorBLOBChunkTransfer;
    TelinkLogInfo(@"\n\n==========firmware update:step%d.1\n\n",self.firmwareUpdateProgress);
    __block BOOL hasFail = NO;
    self.successActionInCurrentProgress = 0;
    if (self.losePacketsDict && self.losePacketsDict.allKeys.count > 0) {
        NSArray *losePacketsDictAllKeys = self.losePacketsDict.allKeys;
        for (NSNumber *addressNumber in losePacketsDictAllKeys) {
            UInt16 destination = (UInt16)addressNumber.intValue;
            NSArray *loaeChunkIndexes = self.losePacketsDict[addressNumber];
            for (NSNumber *chunkIndexNumber in loaeChunkIndexes) {
                NSInteger chunkIndex = chunkIndexNumber.intValue;
                self.chunkIndex = chunkIndex;
                NSData *chunkData = nil;
                if (chunkIndex == self.chunksCountofCurrentBlock - 1) {
                    //end chunk of current block
                    chunkData = [self.currentBlockData subdataWithRange:NSMakeRange(self.chunkSize * chunkIndex, self.currentBlockData.length - self.chunkSize * chunkIndex)];
                } else {
                    chunkData = [self.currentBlockData subdataWithRange:NSMakeRange(self.chunkSize * chunkIndex, self.chunkSize)];
                }
                __weak typeof(self) weakSelf = self;
                self.semaphore = dispatch_semaphore_create(0);
                TelinkLogInfo(@"all Block count=%d,current block index=%d,destination = 0x%x,all chunk count=%d,current chunk index=%d ",self.allBlockCount,self.blockIndex,destination,self.chunksCountofCurrentBlock,self.chunkIndex);
                BOOL sendBySegmentPdu = NO;
                self.messageHandle = [SDKLibCommand BLOBChunkTransferWithDestination:destination chunkNumber:self.chunkIndex chunkData:chunkData sendBySegmentPdu:sendBySegmentPdu retryCount:0 responseMaxCount:0 resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    if (error) {
                        hasFail = YES;
                        [self createErrorWithString:[NSString stringWithFormat:@"App transfer BLOBChunk to destination=0x%04X fail", destination]];
                        dispatch_semaphore_signal(weakSelf.semaphore);
                    } else {
                        weakSelf.successActionInCurrentProgress ++;
                            if (destination == kMeshOTAGroupAddress && !sendBySegmentPdu && (chunkData.length + 2) <= (SigMeshLib.share.dataSource.defaultUnsegmentedMessageLowerTransportPDUMaxLength - 1 - 4)) {//该指令是1个字节Opcode，4个字节MIC。剩余是AccessPDU。
                            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(kUnSegmentPacketInterval * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                                dispatch_semaphore_signal(weakSelf.semaphore);
                            });
                        } else {
                            dispatch_semaphore_signal(weakSelf.semaphore);
                        }
                    }
                }];
                //Most provide 24*60*60 seconds for BLOBChunkTransfer(initiator->Distributor) in every chunk.
                dispatch_semaphore_wait(self.semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 24*60*60.0));
            }
        }
    }
    if (hasFail) {
        [self initiatorToDistributorBLOBChunkTransferFailAction];
    } else {
        //使用DLE固件时，APP太快发送BLOBBlockGet，设备端不会回包。添加延时0.5s。
        [self performSelector:@selector(initiatorToDistributorBLOBChunkTransferSuccessFinishAction) withObject:nil afterDelay:0.5];
    }
}

- (void)initiatorToDistributorBLOBChunkTransferSuccessFinishAction {
    [self performSelector:@selector(initiatorToDistributorBLOBBlockGet) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

- (void)initiatorToDistributorBLOBChunkTransferFailAction {
    [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

//参考原来meshOTA的BLOB逻辑

#pragma mark - Firmware update step13:BLOBBlockGet(initiator->Distributor)
- (void)initiatorToDistributorBLOBBlockGet {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressInitiatorToDistributorBLOBBlockGet;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);

    self.losePacketsDict = [NSMutableDictionary dictionary];
    __block BOOL hasResponse = NO;
    __block BOOL hasSuccess = NO;
    __block BOOL hasLost = NO;
    __weak typeof(self) weakSelf = self;
    self.semaphore = dispatch_semaphore_create(0);
    self.messageHandle = [SDKLibCommand BLOBBlockGetWithDestination:self.distributorAddress retryCount:SigMeshLib.share.dataSource.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigBLOBBlockStatus * _Nonnull responseMessage) {
        TelinkLogDebug(@"BLOBBlockGet(initiator->Distributor)=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
        if (source == weakSelf.distributorAddress) {
            hasResponse = YES;
            /*BLOBBlockGet=80000000010600,source=2,destination=1*/
            if (responseMessage.status == SigBLOBBlockStatusType_success) {
                hasSuccess = YES;
                if (responseMessage.format == SigBLOBBlockFormatType_someChunksMissing && responseMessage.missingChunksList && responseMessage.missingChunksList.count > 0) {
                    hasLost = YES;
                    NSString *errorString = [NSString stringWithFormat:@"fail in BLOBBlockGet(initiator->Distributor),SigBLOBBlockStatus.status=0x%x,format=0x%x,blockIndex=%d",responseMessage.status,responseMessage.format,weakSelf.blockIndex];
                    weakSelf.failError = [NSError errorWithDomain:errorString code:-weakSelf.firmwareUpdateProgress userInfo:nil];
                    TelinkLogInfo(@"%@",errorString);
                    weakSelf.losePacketsDict[@(source)] = responseMessage.missingChunksList;
                } else if (responseMessage.format == SigBLOBBlockFormatType_encodedMissingChunks && responseMessage.encodedMissingChunksList && responseMessage.encodedMissingChunksList.count > 0) {
                    hasLost = YES;
                    NSString *errorString = [NSString stringWithFormat:@"fail in BLOBBlockGet(initiator->Distributor),SigBLOBBlockStatus.status=0x%x,format=0x%x,blockIndex=%d",responseMessage.status,responseMessage.format,weakSelf.blockIndex];
                    weakSelf.failError = [NSError errorWithDomain:errorString code:-weakSelf.firmwareUpdateProgress userInfo:nil];
                    TelinkLogInfo(@"%@",errorString);
                    weakSelf.losePacketsDict[@(source)] = responseMessage.encodedMissingChunksList;
                } else if (responseMessage.format == SigBLOBBlockFormatType_allChunksMissing) {
                    hasLost = YES;
                    NSString *errorString = [NSString stringWithFormat:@"fail in BLOBBlockGet(initiator->Distributor),SigBLOBBlockStatus.status=0x%x,format=0x%x,blockIndex=%d",responseMessage.status,responseMessage.format,weakSelf.blockIndex];
                    weakSelf.failError = [NSError errorWithDomain:errorString code:-weakSelf.firmwareUpdateProgress userInfo:nil];
                    TelinkLogInfo(@"%@",errorString);
                    NSMutableArray *chunkIndexes = [NSMutableArray array];
                    for (int i=0; i<weakSelf.chunksCountofCurrentBlock; i++) {
                        [chunkIndexes addObject:@(i)];
                    }
                    weakSelf.losePacketsDict[@(source)] = chunkIndexes;
                }
            } else {
                [weakSelf createErrorWithString:[NSString stringWithFormat:@"SigBLOBBlockStatus.status=0x%x, blockIndex=%d, source=0x%04X", responseMessage.status, weakSelf.blockIndex, source]];
                TelinkLogInfo(@"%@",[NSString stringWithFormat:@"SigBLOBBlockStatus.status=0x%x, blockIndex=%d, source=0x%04X", responseMessage.status, weakSelf.blockIndex, source]);
                if (![weakSelf.failAddressArray containsObject:@(weakSelf.distributorAddress)]) {
                    [weakSelf.failAddressArray addObject:@(weakSelf.distributorAddress)];
                }
            }
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        if (error) {
            [weakSelf createErrorWithString:[NSString stringWithFormat:@"The distributorAddress=0x%04X has not response SigBLOBBlockStatus", weakSelf.distributorAddress]];
        }
        dispatch_semaphore_signal(weakSelf.semaphore);
    }];
    if (self.phoneIsDistributor) {
        //手机端既作为initiator，也作为Distributor。
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(kCreateSimulatedMessageInterval * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            [self createSigBLOBBlockStatus];
        });
    }
    //Most provide 3 seconds to BLOBTransferGet(initiator->Distributor) every node.
    dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
    if (hasLost) {
        if (self.losePacketsDict && self.losePacketsDict.allKeys.count > 0) {
            //做法1：retry使用单播地址
//            [self BLOBChunkTransferWithLosePackets];
            //做法2：retry使用组播地址
            NSMutableDictionary *newDict = [NSMutableDictionary dictionary];
            NSMutableArray *newLoseChunkIndexes = [NSMutableArray array];
            NSDictionary *losePacketsDict = [NSDictionary dictionaryWithDictionary:self.losePacketsDict];
            for (NSNumber *addressNumber in losePacketsDict.allKeys) {
                NSArray *loaeChunkIndexes = losePacketsDict[addressNumber];
                for (NSNumber *chunkIndex in loaeChunkIndexes) {
                    if (![newLoseChunkIndexes containsObject:chunkIndex]) {
                        [newLoseChunkIndexes addObject:chunkIndex];
                    }
                }
            }
            if (newLoseChunkIndexes.count > 0) {
                if (losePacketsDict.count == 1) {
                    newDict[losePacketsDict.allKeys.firstObject] = newLoseChunkIndexes;
                } else {
                    newDict[@(kMeshOTAGroupAddress)] = newLoseChunkIndexes;
                }
                TelinkLogInfo(@"newLoseChunkIndexes=%@",newLoseChunkIndexes);
                self.losePacketsDict = [NSMutableDictionary dictionaryWithDictionary:newDict];
                [self initiatorToDistributorBLOBChunkTransferWithLosePackets];
            }else{
                [self initiatorToDistributorBLOBBlockGetFailAction];
            }
        } else {
            [self initiatorToDistributorBLOBBlockGetFailAction];
        }
    } else {
        if (hasResponse) {
            if (hasSuccess) {
                [self initiatorToDistributorBLOBBlockGetSuccessAction];
            } else {
                [self initiatorToDistributorBLOBBlockGetFailAction];
            }
        } else {
            //可能是断电或者蓝牙连接断开了，需要等待重连并重试该流程
        }
    }
}

- (void)initiatorToDistributorBLOBBlockGetSuccessAction {
    if (self.blockIndex < self.allBlockCount - 1) {
        // send next block data
        self.blockIndex ++;
        self.retryCountInBLOBChunkTransfer = 0;
        [self performSelector:@selector(initiatorToDistributorBLOBBlockStart) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
    } else if (self.blockIndex == self.allBlockCount - 1) {
        // all blocks had send
//        [self performSelector:@selector(firmwareUpdateFirmwareDistributionFirmwareGet) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
        [self performSelector:@selector(firmwareUpdateFirmwareDistributionStart) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
    } else {
        [self createErrorWithString:@"self.blockIndex > self.allBlockCount - 1."];
        [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
    }
}

- (void)initiatorToDistributorBLOBBlockGetFailAction {
    if (self.retryCountInBLOBChunkTransfer < kRetryCountInBLOBChunkTransfer) {
        TelinkLogInfo(@"retry send block.");
        self.retryCountInBLOBChunkTransfer ++;
        [self performSelector:@selector(initiatorToDistributorBLOBBlockStart) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
    } else {
        TelinkLogInfo(@"%@", [NSString stringWithFormat:@"App retry transfer BLOBChunk to distributorAddress=0x%04X three times but still failed", self.distributorAddress]);
        [self createErrorWithString:[NSString stringWithFormat:@"App retry transfer BLOBChunk to distributorAddress=0x%04X three times but still failed", self.distributorAddress]];
        [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
    }
}

// 8.7.4 Managing firmware images on the Distributor(Figure 8.10, step3)

//8.1.3.5 Starting firmware image distribution to the Updating nodes
//8.6.2.2.6 Initiate Distribution procedure(initiator发送到Distributor)
//8.5.2.2.3 Distribute Firmware procedure(Distributor广播到updating node(s))

#pragma mark - Firmware update step14:firmwareDistributionStart(Initiator->Distributor,发送到直连节点，需模拟回包)
- (void)firmwareUpdateFirmwareDistributionStart {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressFirmwareDistributionStart;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);

    __block BOOL hasFail = NO;
    self.successActionInCurrentProgress = 0;
    __weak typeof(self) weakSelf = self;

    self.semaphore = dispatch_semaphore_create(0);
    self.messageHandle = [SDKLibCommand firmwareDistributionStartWithDestination:self.distributorAddress distributionAppKeyIndex:self.distributionAppKeyIndex distributionTTL:self.distributionTTL distributionTimeoutBase:self.distributionTimeoutBase distributionTransferMode:SigTransferModeState_pushBLOBTransferMode updatePolicy:self.updatePolicy RFU:0 distributionFirmwareImageIndex:self.distributionFirmwareImageIndex distributionMulticastAddress:self.distributionMulticastAddress retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareDistributionStatus * _Nonnull responseMessage) {
        TelinkLogDebug(@"firmwareDistributionStart(Initiator->Distributor)=%@,source=0x%x,destination=0x%x",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
        if (source == weakSelf.distributorAddress) {
            if (responseMessage.status == SigFirmwareDistributionServerAndClientModelStatusType_success) {
            } else {
                hasFail = YES;
                [weakSelf createErrorWithString:[NSString stringWithFormat:@"SigFirmwareDistributionStatus.status=0x%x, source=0x%04X", responseMessage.status, source]];
                TelinkLogInfo(@"%@", [NSString stringWithFormat:@"SigFirmwareDistributionStatus.status=0x%x, source=0x%04X", responseMessage.status, source]);
            }
        } else {
            TelinkLogInfo(@"response from other node.");
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        if (error) {
            hasFail = YES;
            [weakSelf createErrorWithString:[NSString stringWithFormat:@"The distributorAddress=0x%04X has not response SigFirmwareDistributionStatus", weakSelf.distributorAddress]];
        }
        dispatch_semaphore_signal(weakSelf.semaphore);
    }];
    if (self.phoneIsDistributor) {
        //手机端既作为initiator，也作为Distributor。
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(kCreateSimulatedMessageInterval * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            [self createResponseSigFirmwareDistributionStatusWithStatus:SigFirmwareDistributionServerAndClientModelStatusType_success distributionPhase:SigDistributionPhaseState_transferActive];
        });
    }
    //Most provide 3 seconds to firmwareDistributionStart(Initiator->Distributor) every node.
    dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
    if (hasFail) {
        [self firmwareUpdateFirmwareDistributionStartFailAction];
    } else {
        [self firmwareUpdateFirmwareDistributionStartSuccessAction];
    }
}

- (void)createResponseSigFirmwareDistributionStatusWithStatus:(SigFirmwareDistributionServerAndClientModelStatusType)status distributionPhase:(SigDistributionPhaseState)distributionPhase {
    TelinkLogInfo(@"create SigFirmwareDistributionStatus by app.");
    UInt16 distributionMulticastAddress = kMeshOTAGroupAddress;
    UInt16 distributionAppKeyIndex = self.distributionAppKeyIndex;
    UInt8 distributionTTL = self.distributionTTL;
    UInt16 distributionTimeoutBase = self.distributionTimeoutBase;
    SigTransferModeState distributionTransferMode = self.transferModeOfDistributor;
    SigUpdatePolicyType updatePolicy = self.updatePolicy;
    UInt16 distributionFirmwareImageIndex = self.updateFirmwareImageIndex;
    SigFirmwareDistributionStatus *responseMessage = [[SigFirmwareDistributionStatus alloc] initWithStatus:status distributionPhase:distributionPhase distributionMulticastAddress:distributionMulticastAddress distributionAppKeyIndex:distributionAppKeyIndex distributionTTL:distributionTTL distributionTimeoutBase:distributionTimeoutBase distributionTransferMode:distributionTransferMode updatePolicy:updatePolicy RFU:0 distributionFirmwareImageIndex:distributionFirmwareImageIndex];
    dispatch_async(SigMeshLib.share.delegateQueue, ^{
        if ([SigMeshLib.share.delegate respondsToSelector:@selector(didReceiveMessage:sentFromSource:toDestination:)]) {
            [SigMeshLib.share.delegate didReceiveMessage:responseMessage sentFromSource:self.distributorAddress toDestination:SigMeshLib.share.dataSource.curLocationNodeModel.address];
        }
    });
}

- (void)firmwareUpdateFirmwareDistributionStartSuccessAction {
    if (self.phoneIsDistributor) {
        [self performSelector:@selector(firmwareUpdateFirmwareUpdateStart) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
    } else {
        TelinkLogVerbose(@"");
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(10.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            [self performSelector:@selector(fiemwareUpdateFirmwareDistributionReceiversGet) onThread:self.meshOTAThread withObject:nil waitUntilDone:NO];
        });
    }
}

- (void)firmwareUpdateFirmwareDistributionStartFailAction {
    [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

/*
 模拟Distributor进行固件广播start
 */

// 8.7.5 Starting firmware image distribution(Figure 8.11, step3)

#pragma mark - Firmware update step15:firmwareUpdateStart(Distributor->updating node(s))
- (void)firmwareUpdateFirmwareUpdateStart {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressFirmwareUpdateStart;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);

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
        if (node.state != DeviceStateOutOfLine || node.features.lowPowerFeature != SigNodeFeaturesState_notSupported) {
            self.semaphore = dispatch_semaphore_create(0);
            TelinkLogInfo(@"firmwareUpdateStartWithDestination=0x%x",node.address);
            self.messageHandle = [SDKLibCommand firmwareUpdateStartWithDestination:node.address updateTTL:self.updateTTL updateTimeoutBase:self.updateTimeoutBase updateBLOBID:self.updateBLOBIDForDistributor updateFirmwareImageIndex:self.updateFirmwareImageIndex incomingFirmwareMetadata:self.incomingFirmwareMetadata retryCount:SigMeshLib.share.dataSource.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareUpdateStatus * _Nonnull responseMessage) {
                TelinkLogDebug(@"firmwareUpdateStart(Distributor->updating node(s))=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                if (source == nodeAddress.intValue) {
                    if (responseMessage.status == SigFirmwareUpdateStatusType_success) {
//                            if (responseMessage.phase == SigFirmwareUpdatePhaseType_inProgress) {
//                                if (!responseMessage.additionalInformation.ProvisioningNeeded) {
                                hasSuccess = YES;
//                                } else {
//                                    hasFail = YES;
//                                    weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateStart,responseMessage.additionalInformation.ProvisioningNeeded=0x%x",responseMessage.additionalInformation.ProvisioningNeeded] code:-weakSelf.firmwareUpdateProgress userInfo:nil];
//                                }
//                            } else {
//                                hasFail = YES;
//                                weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateStart,SigFirmwareUpdateStatus.phase=0x%x",responseMessage.phase] code:-weakSelf.firmwareUpdateProgress userInfo:nil];
//                            }
                    } else {
                        [weakSelf createErrorWithString:[NSString stringWithFormat:@"SigFirmwareUpdateStatus.status=0x%x, source=0x%04X", responseMessage.status, source]];
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
                TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                dispatch_semaphore_signal(weakSelf.semaphore);
            }];
            //Most provide 3 seconds to firmwareUpdateStart(Distributor->updating node(s)) every node.
            dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
        } else {
            [self createErrorWithString:[NSString stringWithFormat:@"node=0x%4X is offline", nodeAddress.intValue]];
            if (![self.failAddressArray containsObject:nodeAddress]) {
                [self.failAddressArray addObject:nodeAddress];
            }
        }
    }
    if (hasSuccess) {
        [self firmwareUpdateFirmwareUpdateStartSuccessAction];
    } else {
        if (!self.failError) {
            [self createErrorWithString:@"All nodes have not response SigFirmwareUpdateStatus"];
        }
        [self firmwareUpdateFirmwareUpdateStartFailAction];
    }
}

- (void)firmwareUpdateFirmwareUpdateStartSuccessAction {
    [self performSelector:@selector(distributorToUpdatingNodesBLOBTransferGet) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

- (void)firmwareUpdateFirmwareUpdateStartFailAction {
    [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

// 8.7.5 Starting firmware image distribution(Figure 8.11, step4)

//参考原来meshOTA的BLOB逻辑

#pragma mark - Firmware update step16:BLOBTransferGet(Distributor->updating node(s))
- (void)distributorToUpdatingNodesBLOBTransferGet {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressDistributorToUpdatingNodesBLOBTransferGet;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);

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
        if (node.state != DeviceStateOutOfLine || node.features.lowPowerFeature != SigNodeFeaturesState_notSupported) {
            self.semaphore = dispatch_semaphore_create(0);
            self.messageHandle = [SDKLibCommand BLOBTransferGetWithDestination:node.address retryCount:SigMeshLib.share.dataSource.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigBLOBTransferStatus * _Nonnull responseMessage) {
                TelinkLogDebug(@"BLOBTransferGet(Distributor->updating node(s))=%@,transferMode=%d,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],responseMessage.transferMode,source,destination);
                if (source == nodeAddress.intValue) {
//                        if (responseMessage.status == SigBLOBTransferStatusType_busy) {
                        hasSuccess = YES;
//                        } else {
//                           hasFail = YES;
//                           weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in BLOBTransferGet(Distributor->updating node(s)),SigBLOBTransferStatus.status=0x%x",responseMessage.status] code:-weakSelf.firmwareUpdateProgress userInfo:nil];
//                        }
                } else {
                    if (![weakSelf.failAddressArray containsObject:nodeAddress]) {
                        [weakSelf.failAddressArray addObject:nodeAddress];
                    }
                }
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                dispatch_semaphore_signal(weakSelf.semaphore);
            }];
            //Most provide 3 seconds to BLOBTransferGet(Distributor->updating node(s)) every node.
            dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
        } else {
            [self createErrorWithString:[NSString stringWithFormat:@"node=0x%4X is offline", nodeAddress.intValue]];
            if (![self.failAddressArray containsObject:nodeAddress]) {
                [self.failAddressArray addObject:nodeAddress];
            }
        }
    }
    if (hasSuccess) {
        [self distributorToUpdatingNodesBLOBTransferGetSuccessAction];
    } else {
        if (!self.failError) {
            [self createErrorWithString:@"All nodes have not response SigBLOBTransferStatus"];
        }
        [self distributorToUpdatingNodesBLOBTransferGetFailAction];
    }
}

- (void)distributorToUpdatingNodesBLOBTransferGetSuccessAction {
    [self performSelector:@selector(distributorToUpdatingNodesBLOBInformationGet) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

- (void)distributorToUpdatingNodesBLOBTransferGetFailAction {
    [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

//参考原来meshOTA的BLOB逻辑

#pragma mark - Firmware update step17:BLOBInformationGet(Distributor->updating node(s))
- (void)distributorToUpdatingNodesBLOBInformationGet {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressDistributorToUpdatingNodesBLOBInformationGet;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);

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
        if (node.state != DeviceStateOutOfLine || node.features.lowPowerFeature != SigNodeFeaturesState_notSupported) {
            self.semaphore = dispatch_semaphore_create(0);
            self.messageHandle = [SDKLibCommand BLOBInformationGetWithDestination:node.address retryCount:SigMeshLib.share.dataSource.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigBLOBInformationStatus * _Nonnull responseMessage) {
                TelinkLogDebug(@"BLOBInformationGet(Distributor->updating node(s))=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                if (source == nodeAddress.intValue && [responseMessage isMemberOfClass:[SigBLOBInformationStatus class]]) {
                    weakSelf.oBLOBInformations[@(source)] = responseMessage;
                    TelinkLogDebug(@"supportedTransferMode.value=%d,weakSelf.transferModeOfUpdateNodes=%d",responseMessage.supportedTransferMode.value,weakSelf.transferModeOfUpdateNodes);
                    if (responseMessage.supportedTransferMode.PushBLOBTransferModeBit) {
                        //注意：当前版本只考虑一个节点的pull，不考虑多个节点的pull。（多个节点则使用最后一个节点的模式）
                        if (responseMessage.supportedTransferMode.PullBLOBTransferModeBit && weakSelf.allAddressArray.count == 1) {
                            if (weakSelf.allAddressArray.firstObject.intValue == SigDataSource.share.unicastAddressOfConnected) {
                                weakSelf.transferModeOfUpdateNodes = SigTransferModeState_pushBLOBTransferMode;
                            } else {
                                weakSelf.transferModeOfUpdateNodes = SigTransferModeState_pullBLOBTransferMode;
                            }
                        } else {
                            weakSelf.transferModeOfUpdateNodes = SigTransferModeState_pushBLOBTransferMode;
                        }
                    } else {
                        weakSelf.transferModeOfUpdateNodes = SigTransferModeState_pullBLOBTransferMode;
                    }
                    //=====验证Pull模式测试代码=====开始
//                    if (responseMessage.supportedTransferMode.PullBLOBTransferModeBit) {
//                        weakSelf.transferModeOfUpdateNodes = SigTransferModeState_pullBLOBTransferMode;
//                    }
                    //=====验证Pull模式测试代码=====结束
                    hasSuccess = YES;
                } else {
                    if (![weakSelf.failAddressArray containsObject:nodeAddress]) {
                        [weakSelf.failAddressArray addObject:nodeAddress];
                    }
                }
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    dispatch_semaphore_signal(weakSelf.semaphore);
            }];
            //Most provide 3 seconds to BLOBInformationGet(Distributor->updating node(s)) every node.
            dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
        } else {
            [self createErrorWithString:[NSString stringWithFormat:@"node=0x%4X is offline", nodeAddress.intValue]];
            if (![self.failAddressArray containsObject:nodeAddress]) {
                [self.failAddressArray addObject:nodeAddress];
            }
        }
    }
    if (hasSuccess) {
        if (self.oBLOBInformations && self.oBLOBInformations.count > 0) {
            //v3.3.0暂时逻辑，以最后一个返回的SigBLOBInformationStatus为准发送数据。
            UInt8 maxBlockSizeLog = self.oBLOBInformations.allValues.lastObject.maxBlockSizeLog;
            UInt16 maxChunksNumber = self.oBLOBInformations.allValues.lastObject.maxChunksNumber;
            //直接根据返回的BlockSizeLog和ChunksNumber算出合法的self.blockSizeLog和self.chunkSize
            UInt8 blockSizeLog = maxBlockSizeLog;
            UInt32 blockSize = pow(2, blockSizeLog);
            self.blockSizeLog = blockSizeLog;
            self.chunkSize = ceil(blockSize / (double)maxChunksNumber);

            //v3.3.3新增，对直连LPN升级的情况，push模式，可以使用self.chunkSize=8，也可以使用self.chunkSize=208，后者升级更加快。
            if (weakSelf.allAddressArray.count == 1) {
                SigNodeModel *node = [SigDataSource.share getNodeWithAddress:weakSelf.allAddressArray.firstObject.intValue];
                if (node && node.isLPN && node.address == SigDataSource.share.unicastAddressOfConnected && self.phoneIsDistributor) {
                    self.chunkSize = 208;
                }
            }

            //存在合法的blockSizeLog和chunkSize，进行下一步流程。
            [self distributorToUpdatingNodesBLOBInformationGetSuccessAction];

        } else {
            self.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in BLOBInformationGet(Distributor->updating node(s)), BLOBInformation is empty."] code:-weakSelf.firmwareUpdateProgress userInfo:nil];
            [self distributorToUpdatingNodesBLOBInformationGetFailAction];
        }
    } else {
        [self createErrorWithString:@"All nodes have not response SigBLOBInformationStatus"];
        [self distributorToUpdatingNodesBLOBInformationGetFailAction];
    }
}

- (void)distributorToUpdatingNodesBLOBInformationGetSuccessAction {
    [self performSelector:@selector(distributorToUpdatingNodesBLOBTransferStart) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

- (void)distributorToUpdatingNodesBLOBInformationGetFailAction {
    [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

//参考原来meshOTA的BLOB逻辑

#pragma mark - Firmware update step18:BLOBTransferStart(Distributor->updating node(s))
- (void)distributorToUpdatingNodesBLOBTransferStart {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressDistributorToUpdatingNodesBLOBTransferStart;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);

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
            if (node.state != DeviceStateOutOfLine || node.features.lowPowerFeature != SigNodeFeaturesState_notSupported) {
                self.semaphore = dispatch_semaphore_create(0);
                TelinkLogInfo(@"BLOBTransferStart->self.transferModeOfUpdateNodes=%d",self.transferModeOfUpdateNodes);
                self.messageHandle = [SDKLibCommand BLOBTransferStartWithDestination:node.address transferMode:self.transferModeOfUpdateNodes BLOBID:self.updateBLOBIDForDistributor BLOBSize:(UInt32)self.firmwareDataOnDistributor.length BLOBBlockSizeLog:self.blockSizeLog MTUSize:self.MTUSize retryCount:SigMeshLib.share.dataSource.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigBLOBTransferStatus * _Nonnull responseMessage) {
                    TelinkLogDebug(@"BLOBTransferStart(Distributor->updating node(s))=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                    if (source == nodeAddress.intValue) {
//                        if (responseMessage.status == SigBLOBTransferStatusType_busy) {
                            hasSuccess = YES;
//                        } else {
//                           hasFail = YES;
//                           weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in BLOBTransferStart(Distributor->updating node(s)),SigBLOBTransferStatus.status=0x%x",responseMessage.status] code:-weakSelf.firmwareUpdateProgress userInfo:nil];
//                        }
                    } else {
                        if (![weakSelf.failAddressArray containsObject:nodeAddress]) {
                            [weakSelf.failAddressArray addObject:nodeAddress];
                        }
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    dispatch_semaphore_signal(weakSelf.semaphore);
                }];
                //Most provide 3 seconds to BLOBTransferStart(Distributor->updating node(s)) every node.
                dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
            } else {
                [self createErrorWithString:[NSString stringWithFormat:@"node=0x%4X is offline", nodeAddress.intValue]];
                if (![self.failAddressArray containsObject:nodeAddress]) {
                    [self.failAddressArray addObject:nodeAddress];
                }
            }
    }
    if (hasSuccess) {
        [self distributorToUpdatingNodesBLOBTransferStartSuccessAction];
    } else {
        [self createErrorWithString:@"All nodes have not response SigBLOBTransferStatus"];
        [self distributorToUpdatingNodesBLOBTransferStartFailAction];
    }
}

- (void)distributorToUpdatingNodesBLOBTransferStartSuccessAction {
    self.blockIndex = 0;
    double blockSize = (double)pow(2, self.blockSizeLog);
    self.allBlockCount = ceil(self.firmwareDataOnDistributor.length / blockSize);
    self.retryCountInBLOBChunkTransfer = 0;
    [self performSelector:@selector(distributorToUpdatingNodesBLOBBlockStart) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

- (void)distributorToUpdatingNodesBLOBTransferStartFailAction {
    [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

//参考原来meshOTA的BLOB逻辑

#pragma mark - Firmware update step19:BLOBBlockStart(Distributor->updating node(s))
- (void)distributorToUpdatingNodesBLOBBlockStart {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressDistributorToUpdatingNodesBLOBBlockStart;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);

    self.chunkIndex = 0;
    __block BOOL hasSuccess = NO;
    __block BOOL isPullMode = NO;
    __weak typeof(self) weakSelf = self;
    NSArray *allAddressArray = [NSArray arrayWithArray:self.allAddressArray];
    for (NSNumber *nodeAddress in allAddressArray) {
        if (![self isMeshOTAing]) {
            return;
        }
        if ([self.failAddressArray containsObject:nodeAddress]) {
            continue;
        }
        if (!SigBearer.share.isOpen) {
            return;
        }
        SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:nodeAddress.intValue];
        if (node.state != DeviceStateOutOfLine || node.features.lowPowerFeature != SigNodeFeaturesState_notSupported) {
            NSInteger blockSize = pow(2, self.blockSizeLog);
            NSInteger currentBlockSize = blockSize;
            if (self.allBlockCount - 1 <= self.blockIndex) {
                //last block
                currentBlockSize = self.firmwareDataOnDistributor.length - blockSize * self.blockIndex;
            }
            self.currentBlockData = [self.firmwareDataOnDistributor subdataWithRange:NSMakeRange(blockSize * self.blockIndex, currentBlockSize)];
            self.semaphore = dispatch_semaphore_create(0);
            self.messageHandle = [SDKLibCommand BLOBBlockStartWithDestination:node.address blockNumber:self.blockIndex chunkSize:self.chunkSize retryCount:SigMeshLib.share.dataSource.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigBLOBBlockStatus * _Nonnull responseMessage) {
                TelinkLogDebug(@"BLOBBlockStart(Distributor->updating node(s))=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                if (source == nodeAddress.intValue) {
                    hasSuccess = YES;
                    if (responseMessage.status == SigBLOBBlockStatusType_success) {
                        if (responseMessage.format == SigBLOBBlockFormatType_encodedMissingChunks) {
                            if (weakSelf.isMeshOTAing) {
                                isPullMode = YES;
                                weakSelf.losePacketsDict[@(source)] = responseMessage.encodedMissingChunksList;
                                [weakSelf stopLPNReachablleTimer];
                            }
                        }
                    }
                } else {
                    if (![weakSelf.failAddressArray containsObject:nodeAddress]) {
                        [weakSelf.failAddressArray addObject:nodeAddress];
                    }
                }
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                dispatch_semaphore_signal(weakSelf.semaphore);
            }];
            //Most provide 3 seconds to BLOBBlockStart(Distributor->updating node(s)) every node.
            dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
        } else {
            [self createErrorWithString:[NSString stringWithFormat:@"node=0x%4X is offline", nodeAddress.intValue]];
            if (![self.failAddressArray containsObject:nodeAddress]) {
                [self.failAddressArray addObject:nodeAddress];
            }
        }
    }
    if (SigBearer.share.isOpen) {
        if (!isPullMode) {
            if (hasSuccess) {
                [self distributorToUpdatingNodesBLOBBlockStartSuccessAction];
            } else {
                [self createErrorWithString:@"All nodes have not response SigBLOBBlockStatus"];
                [self distributorToUpdatingNodesBLOBBlockStartFailAction];
            }
        } else {
            self.chunksCountofCurrentBlock = ceil(self.currentBlockData.length / (double)self.chunkSize);
            [weakSelf handleLPNReportAction];
        }
    } else {
        //连接断开，则等待重连比重试。
    }
}

- (void)distributorToUpdatingNodesBLOBBlockStartSuccessAction {
    if (self.transferModeOfUpdateNodes == SigTransferModeState_pushBLOBTransferMode) {
        self.chunkIndex = 0;
        [self performSelector:@selector(distributorToUpdatingNodesBLOBChunkTransfer) onThread:self.meshOTAThread withObject:nil waitUntilDone:NO];
    } else {
        //pull模式，等待Report
        self.BLOBPartialBlockReport = nil;
    }
}

- (void)distributorToUpdatingNodesBLOBBlockStartFailAction {
    [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

//参考原来meshOTA的BLOB逻辑

#pragma mark - Firmware update step20:BLOBChunkTransfer(Distributor->updating node(s))
- (void)distributorToUpdatingNodesBLOBChunkTransfer {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressDistributorToUpdatingNodesBLOBChunkTransfer;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);

    self.chunksCountofCurrentBlock = ceil(self.currentBlockData.length / (double)self.chunkSize);
    __block BOOL hasSuccess = NO;

    //v3.3.3开始新增优化逻辑：APP作为distributor，升级节点有且只有一个节点，且为直连节点时，使用直连节点地址；其它情况为组播地址。
    UInt16 destination = kMeshOTAGroupAddress;
    if (self.allAddressArray.count - self.failAddressArray.count == 1) {
        NSArray *allArray = [NSArray arrayWithArray:self.allAddressArray];
        for (NSNumber *n in allArray) {
            if (![self.failAddressArray containsObject:n]) {
                //v3.3.0开始新增优化逻辑：当只有一个节点且为直连节点时.
                if (n.intValue == SigMeshLib.share.dataSource.unicastAddressOfConnected) {
                    TelinkLogInfo(@"只存在一个设备，且是直连设备");
                    destination = n.intValue;
                } else {
                    TelinkLogInfo(@"只存在一个设备，但不是直连设备");
                    SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:n.intValue];
                    if (node && node.isLPN) {
                        destination = n.intValue;
                    }
                }
                break;;
            }
        }
    }

    for (int i = (int)self.chunkIndex; i < self.chunksCountofCurrentBlock; i ++) {
        if (![self isMeshOTAing]) {
            return;
        }
        if (!SigBearer.share.isOpen) {
            return;
        }
        self.chunkIndex = i;
        NSData *chunkData = nil;
        if (i == self.chunksCountofCurrentBlock - 1) {
            //end chunk of current block
            chunkData = [self.currentBlockData subdataWithRange:NSMakeRange(self.chunkSize * i, self.currentBlockData.length - self.chunkSize * i)];
        } else {
            chunkData = [self.currentBlockData subdataWithRange:NSMakeRange(self.chunkSize * i, self.chunkSize)];
        }
        __weak typeof(self) weakSelf = self;
        TelinkLogInfo(@"all Block count=%d,current block index=%d,all chunk count=%d,current chunk index=%d ",self.allBlockCount,self.blockIndex,self.chunksCountofCurrentBlock,self.chunkIndex);
        //该处为新meshOTA逻辑，需要模拟Distributor广播固件到updating nodes，并模拟adv回包，即self.firmwareDistributionReceiversList.
        [self createSigFirmwareDistributionReceiversListWithCurrentChunkData:chunkData otaData:self.firmwareDataOnDistributor];
        [self callbackAdvDistributionProgressBlock];
        if (!_testFinish) {
            if (self.blockIndex == 0 && self.chunkIndex == 6) {
                //这个包不发送，测试补包流程
                _testFinish = YES;
                self.successActionInCurrentProgress ++;
                continue;
            }
        }
        self.semaphore = dispatch_semaphore_create(0);

        BOOL sendBySegmentPdu = NO;
        self.messageHandle = [SDKLibCommand BLOBChunkTransferWithDestination:destination chunkNumber:self.chunkIndex chunkData:chunkData sendBySegmentPdu:sendBySegmentPdu retryCount:0 responseMaxCount:0 resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
            TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
            if (error) {
                [self createErrorWithString:[NSString stringWithFormat:@"App transfer BLOBChunk to destination=0x%04X fail", destination]];
                dispatch_semaphore_signal(weakSelf.semaphore);
            } else {
                hasSuccess = YES;
                if (destination == kMeshOTAGroupAddress && !sendBySegmentPdu && (chunkData.length + 2) <= (SigMeshLib.share.dataSource.defaultUnsegmentedMessageLowerTransportPDUMaxLength - 1 - 4)) {//该指令是1个字节Opcode，4个字节MIC。剩余是AccessPDU。
//                if (destination == kMeshOTAGroupAddress && (chunkData.length + 2) <= (SigMeshLib.share.dataSource.defaultUnsegmentedMessageLowerTransportPDUMaxLength - 1 - 4)) {//该指令是1个字节Opcode，4个字节MIC。剩余是AccessPDU。
                    float interval = kUnSegmentPacketInterval;
                    if (SigMeshLib.share.dataSource.telinkExtendBearerMode == SigTelinkExtendBearerMode_extendGATTAndAdv) {
                        interval = 0.280;
                    }
                    TelinkLogInfo(@"distributor分发地址为广播地址，延时%f秒",interval);
                    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(interval * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                        dispatch_semaphore_signal(weakSelf.semaphore);
                    });
                } else {
                    //为修复手机作为distribution升级多个节点时，发送blob完成后还有异常调用发送接口的bug。
                    if (destination != SigMeshLib.share.dataSource.unicastAddressOfConnected) {
                        SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:destination];
                        float interval = 0.0;
                        if (node && node.isLPN) {
                            //延时200ms
                            //为修复手机作为distribution升级一个非直连LPN节点时，且直连节点friend关闭时，发送blob完成后miss很多包的bug。
                            interval = 0.200;
                        } else {
                            interval = 0.120;
                        }
                        TelinkLogInfo(@"distributor分发地址为单播地址，延时%f秒",interval);
                        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(interval * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                            dispatch_semaphore_signal(weakSelf.semaphore);
                        });
                    } else {
                        //直连不延时
//                    TelinkLogInfo(@"distributor分发地址为单播地址，不延时");
                        dispatch_semaphore_signal(weakSelf.semaphore);
                    }
                }
            }
        }];
        //Most provide 24*60*60 seconds for BLOBChunkTransfer(Distributor->updating node(s)) in every chunk.
        dispatch_semaphore_wait(self.semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 24*60*60.0));
        [self.messageHandle cancel];
        self.messageHandle = nil;
        if (!hasSuccess) {
            break;
        }
    }

    if (!hasSuccess) {
        if (SigBearer.share.isOpen) {
            [self distributorToUpdatingNodesBLOBChunkTransferFailAction];
        } else {
            return;
        }
    } else {
        [self distributorToUpdatingNodesBLOBChunkTransferSuccessFinishAction];
    }
}

- (void)createSigFirmwareDistributionReceiversListWithCurrentChunkData:(NSData *)chunkData otaData:(NSData *)otaData {
    double progress = [self getProgressWithCurrentChunkData:chunkData otaData:otaData] / 2;//progress长度不足，应该返回0~50。
    NSInteger intPro = (NSInteger)progress;
    TelinkLogDebug(@"adv progress=%f,intPro=%ld",progress,(long)intPro);
    TelinkLogInfo(@"create SigFirmwareDistributionReceiversList by app.");
    NSMutableArray *list = [NSMutableArray array];
    NSArray *allAddressArray = [NSArray arrayWithArray:self.allAddressArray];
    for (NSNumber *nodeAddress in allAddressArray) {
        if ([self.failAddressArray containsObject:nodeAddress]) {
            continue;
        }
        //待验证实际升级时下面几个参数是否正确。
        SigUpdatingNodeEntryModel *model = [[SigUpdatingNodeEntryModel alloc] initWithAddress:nodeAddress.intValue retrievedUpdatePhase:SigFirmwareUpdatePhaseType_transferActive updateStatus:SigFirmwareUpdateServerAndClientModelStatusType_success transferStatus:SigBLOBTransferStatusType_success transferProgress:intPro updateFirmwareImageIndex:0];
        [list addObject:model];
    }
    UInt16 firstIndex = 0;
    UInt16 receiversListCount = list.count;
    SigFirmwareDistributionReceiversList *responseMessage = [[SigFirmwareDistributionReceiversList alloc] initWithReceiversListCount:receiversListCount firstIndex:firstIndex receiversList:list];
    self.firmwareDistributionReceiversList = responseMessage;
}

- (void)distributorToUpdatingNodesBLOBChunkTransferWithLosePackets {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressDistributorToUpdatingNodesBLOBChunkTransfer;
    TelinkLogInfo(@"\n\n==========firmware update:step%d.1\n\n",self.firmwareUpdateProgress);
    __block BOOL hasFail = NO;
    self.successActionInCurrentProgress = 0;
    if (self.losePacketsDict && self.losePacketsDict.allKeys.count > 0) {
        NSArray *losePacketsDictAllKeys = self.losePacketsDict.allKeys;
        for (NSNumber *addressNumber in losePacketsDictAllKeys) {
            UInt16 destination = (UInt16)addressNumber.intValue;
            NSArray *loaeChunkIndexes = self.losePacketsDict[addressNumber];
            for (NSNumber *chunkIndexNumber in loaeChunkIndexes) {
                NSInteger chunkIndex = chunkIndexNumber.intValue;
                self.chunkIndex = chunkIndex;
                NSData *chunkData = nil;
                if (chunkIndex == self.chunksCountofCurrentBlock - 1) {
                    //end chunk of current block
                    chunkData = [self.currentBlockData subdataWithRange:NSMakeRange(self.chunkSize * chunkIndex, self.currentBlockData.length - self.chunkSize * chunkIndex)];
                } else {
                    chunkData = [self.currentBlockData subdataWithRange:NSMakeRange(self.chunkSize * chunkIndex, self.chunkSize)];
                }
                __weak typeof(self) weakSelf = self;
                self.semaphore = dispatch_semaphore_create(0);
                TelinkLogInfo(@"all Block count=%d,current block index=%d,destination = 0x%x,all chunk count=%d,current chunk index=%d ",self.allBlockCount,self.blockIndex,destination,self.chunksCountofCurrentBlock,self.chunkIndex);

                BOOL sendBySegmentPdu = NO;
                self.messageHandle = [SDKLibCommand BLOBChunkTransferWithDestination:destination chunkNumber:self.chunkIndex chunkData:chunkData sendBySegmentPdu:sendBySegmentPdu retryCount:0 responseMaxCount:0 resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    if (error) {
                        hasFail = YES;
                        [weakSelf createErrorWithString:[NSString stringWithFormat:@"App transfer BLOBChunk to destination=0x%04X fail", destination]];
                        dispatch_semaphore_signal(weakSelf.semaphore);
                    } else {
                        weakSelf.successActionInCurrentProgress ++;
                        if (destination == kMeshOTAGroupAddress && !sendBySegmentPdu && (chunkData.length + 2) <= (SigMeshLib.share.dataSource.defaultUnsegmentedMessageLowerTransportPDUMaxLength - 1 - 4)) {//该指令是1个字节Opcode，4个字节MIC。剩余是AccessPDU。
//                        if (destination == kMeshOTAGroupAddress && (chunkData.length + 2) <= (SigMeshLib.share.dataSource.defaultUnsegmentedMessageLowerTransportPDUMaxLength - 1 - 4)) {//该指令是1个字节Opcode，4个字节MIC。剩余是AccessPDU。
                            float interval = kUnSegmentPacketInterval;
                            if (SigMeshLib.share.dataSource.telinkExtendBearerMode == SigTelinkExtendBearerMode_extendGATTAndAdv) {
                                interval = 0.280;
                            }
                            TelinkLogInfo(@"distributor分发地址为广播地址，延时%f秒",interval);
                            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(interval * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                                dispatch_semaphore_signal(weakSelf.semaphore);
                            });
                        } else {
                            //为修复手机作为distribution升级多个节点时，发送blob完成后还有异常调用发送接口的bug。
                            if (destination != SigMeshLib.share.dataSource.unicastAddressOfConnected) {
                                SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:destination];
                                float interval = 0.0;
                                if (node && node.isLPN) {
                                    //延时200ms
                                    //为修复手机作为distribution升级一个非直连LPN节点时，且直连节点friend关闭时，发送blob完成后miss很多包的bug。
                                    interval = 0.200;
                                } else {
                                    interval = 0.120;
                                }
                                TelinkLogInfo(@"distributor分发地址为单播地址，延时%f秒",interval);
                                dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(interval * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                                    dispatch_semaphore_signal(weakSelf.semaphore);
                                });
                            } else {
                                //直连不延时
//                    TelinkLogInfo(@"distributor分发地址为单播地址，不延时");
                                dispatch_semaphore_signal(weakSelf.semaphore);
                            }
                        }
                    }
                }];
                //Most provide 24*60*60 seconds for BLOBChunkTransfer(Distributor->updating node(s)) in every chunk.
                dispatch_semaphore_wait(self.semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 24*60*60.0));
            }
        }
    }
    if (hasFail) {
        [self distributorToUpdatingNodesBLOBChunkTransferFailAction];
    } else {
        [self distributorToUpdatingNodesBLOBChunkTransferSuccessFinishAction];
    }
}


- (void)distributorToUpdatingNodesBLOBChunkTransferSuccessFinishAction {
    [self performSelector:@selector(distributorToUpdatingNodesBLOBBlockGet) onThread:self.meshOTAThread withObject:nil waitUntilDone:NO];
}

- (void)distributorToUpdatingNodesBLOBChunkTransferFailAction {
    [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

//参考原来meshOTA的BLOB逻辑

#pragma mark - Firmware update step21:BLOBBlockGet(Distributor->updating node(s))
- (void)distributorToUpdatingNodesBLOBBlockGet {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressDistributorToUpdatingNodesBLOBBlockGet;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);

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
        if (!SigBearer.share.isOpen) {
            return;
        }
        SigNodeModel *node = [SigMeshLib.share.dataSource getNodeWithAddress:nodeAddress.intValue];
        if (node.state != DeviceStateOutOfLine || node.features.lowPowerFeature != SigNodeFeaturesState_notSupported) {
            self.semaphore = dispatch_semaphore_create(0);
            TelinkLogInfo(@"self.blockIndex=%d",self.blockIndex);
            self.messageHandle = [SDKLibCommand BLOBBlockGetWithDestination:node.address retryCount:SigMeshLib.share.dataSource.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigBLOBBlockStatus * _Nonnull responseMessage) {
                TelinkLogDebug(@"BLOBBlockGet(Distributor->updating node(s))=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                if (source == nodeAddress.intValue) {
                    /*BLOBBlockGet=80000000010600,source=2,destination=1*/
                    if (responseMessage.status == SigBLOBBlockStatusType_success) {
                        hasSuccess = YES;
                        if (responseMessage.format == SigBLOBBlockFormatType_someChunksMissing && responseMessage.missingChunksList && responseMessage.missingChunksList.count > 0) {
                            hasLost = YES;
                            [weakSelf createErrorWithString:[NSString stringWithFormat:@"SigBLOBBlockStatus.status=0x%x, format=0x%x,  blockIndex=%d, source=0x%04X", responseMessage.status, responseMessage.format, weakSelf.blockIndex, source]];
                            TelinkLogInfo(@"%@", [NSString stringWithFormat:@"SigBLOBBlockStatus.status=0x%x, format=0x%x,  blockIndex=%d, source=0x%04X", responseMessage.status, responseMessage.format, weakSelf.blockIndex, source]);
                            weakSelf.losePacketsDict[@(source)] = responseMessage.missingChunksList;
                        } else if (responseMessage.format == SigBLOBBlockFormatType_encodedMissingChunks && responseMessage.encodedMissingChunksList && responseMessage.encodedMissingChunksList.count > 0) {
                            hasLost = YES;
                            [weakSelf createErrorWithString:[NSString stringWithFormat:@"SigBLOBBlockStatus.status=0x%x, format=0x%x,  blockIndex=%d, source=0x%04X", responseMessage.status, responseMessage.format, weakSelf.blockIndex, source]];
                            TelinkLogInfo(@"%@", [NSString stringWithFormat:@"SigBLOBBlockStatus.status=0x%x, format=0x%x,  blockIndex=%d, source=0x%04X", responseMessage.status, responseMessage.format, weakSelf.blockIndex, source]);
                            weakSelf.losePacketsDict[@(source)] = responseMessage.encodedMissingChunksList;
                        } else if (responseMessage.format == SigBLOBBlockFormatType_allChunksMissing) {
                            hasLost = YES;
                            [weakSelf createErrorWithString:[NSString stringWithFormat:@"SigBLOBBlockStatus.status=0x%x, format=0x%x,  blockIndex=%d, source=0x%04X", responseMessage.status, responseMessage.format, weakSelf.blockIndex, source]];
                            TelinkLogInfo(@"%@", [NSString stringWithFormat:@"SigBLOBBlockStatus.status=0x%x, format=0x%x,  blockIndex=%d, source=0x%04X", responseMessage.status, responseMessage.format, weakSelf.blockIndex, source]);
                            NSMutableArray *chunkIndexes = [NSMutableArray array];
                            for (int i=0; i<weakSelf.chunksCountofCurrentBlock; i++) {
                                [chunkIndexes addObject:@(i)];
                            }
                            weakSelf.losePacketsDict[@(source)] = chunkIndexes;
                        }
                    } else {
                        [weakSelf createErrorWithString:[NSString stringWithFormat:@"SigBLOBBlockStatus.status=0x%x, blockIndex=%d, source=0x%04X", responseMessage.status, weakSelf.blockIndex, source]];
                        TelinkLogInfo(@"%@", [NSString stringWithFormat:@"SigBLOBBlockStatus.status=0x%x, blockIndex=%d, source=0x%04X", responseMessage.status, weakSelf.blockIndex, source]);
                        if (![weakSelf.failAddressArray containsObject:nodeAddress]) {
                            [weakSelf.failAddressArray addObject:nodeAddress];
                        }
                    }
                }
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                if (error) {
                    if (![weakSelf.failAddressArray containsObject:nodeAddress]) {
                        [weakSelf.failAddressArray addObject:nodeAddress];
                    }
                }
                dispatch_semaphore_signal(weakSelf.semaphore);
            }];
            //Most provide 3 seconds to BLOBBlockGet(Distributor->updating node(s)) every node.
            dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
        } else {
            [self createErrorWithString:[NSString stringWithFormat:@"node=0x%4X is offline", nodeAddress.intValue]];
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
            NSMutableArray *newLoseChunkIndexes = [NSMutableArray array];
            NSDictionary *losePacketsDict = [NSDictionary dictionaryWithDictionary:self.losePacketsDict];
            for (NSNumber *addressNumber in losePacketsDict.allKeys) {
                NSArray *loaeChunkIndexes = losePacketsDict[addressNumber];
                for (NSNumber *chunkIndex in loaeChunkIndexes) {
                    if (![newLoseChunkIndexes containsObject:chunkIndex]) {
                        [newLoseChunkIndexes addObject:chunkIndex];
                    }
                }
            }
            if (newLoseChunkIndexes.count > 0) {
                if (losePacketsDict.count == 1) {
                    newDict[losePacketsDict.allKeys.firstObject] = newLoseChunkIndexes;
                } else {
                    newDict[@(kMeshOTAGroupAddress)] = newLoseChunkIndexes;
                }
                TelinkLogInfo(@"newLoseChunkIndexes=%@",newLoseChunkIndexes);
                self.losePacketsDict = [NSMutableDictionary dictionaryWithDictionary:newDict];
                [self distributorToUpdatingNodesBLOBChunkTransferWithLosePackets];
            }else{
                [self distributorToUpdatingNodesBLOBBlockGetFailAction];
            }
        } else {
            [self distributorToUpdatingNodesBLOBBlockGetFailAction];
        }
    } else {
        if (hasSuccess) {
            [self distributorToUpdatingNodesBLOBBlockGetSuccessAction];
        } else {
            [self distributorToUpdatingNodesBLOBBlockGetFailAction];
        }
    }
}

- (void)distributorToUpdatingNodesBLOBBlockGetSuccessAction {
    if (self.blockIndex < self.allBlockCount - 1) {
        // send next block data
        self.blockIndex ++;
        self.retryCountInBLOBChunkTransfer = 0;
        [self performSelector:@selector(distributorToUpdatingNodesBLOBBlockStart) onThread:self.meshOTAThread withObject:nil waitUntilDone:NO];
    } else if (self.blockIndex == self.allBlockCount - 1) {
        // all blocks had send
        TelinkLogVerbose(@"");
        [self performSelector:@selector(fiemwareUpdateFirmwareDistributionReceiversGet) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
    } else {
        [self createErrorWithString:@"self.blockIndex > self.allBlockCount - 1."];
        [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
    }
}

- (void)distributorToUpdatingNodesBLOBBlockGetFailAction {
    if (self.retryCountInBLOBChunkTransfer < kRetryCountInBLOBChunkTransfer) {
        TelinkLogInfo(@"retry send block.");
        self.retryCountInBLOBChunkTransfer ++;
        [self performSelector:@selector(distributorToUpdatingNodesBLOBBlockStart) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
    } else {
        TelinkLogInfo(@"App retry transfer BLOBChunk to nodes three times but still failed");
        [self createErrorWithString:@"App retry transfer BLOBChunk to nodes three times but still failed"];
        [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
    }
}


/*
 模拟Distributor进行固件广播end
 */


//8.7.6 Checking distribution progress
//8.1.3.6 Checking the progress of a firmware image transfer(Distributor广播固件状态/或者校验固件状态/或者apply固件状态，initiator定时查询设备的OTA状态即可)
#pragma mark - Firmware update step22:firmwareDistributionReceiversGet(Initiator->Distributor,发送到直连节点，需模拟回包)
- (void)fiemwareUpdateFirmwareDistributionReceiversGet {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressFirmwareDistributionReceiversGet;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);

    self.successActionInCurrentProgress = 0;
    __weak typeof(self) weakSelf = self;

    __block SigFirmwareDistributionReceiversList *response = nil;
    if (SigBearer.share.isOpen) {
        self.semaphore = dispatch_semaphore_create(0);
        self.messageHandle = [SDKLibCommand firmwareDistributionReceiversGetWithDestination:self.distributorAddress firstIndex:self.firstIndexOfFirmwareDistributionReceiversGet entriesLimit:1 retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareDistributionReceiversList * _Nonnull responseMessage) {
            TelinkLogDebug(@"firmwareDistributionReceiversGet(Initiator->Distributor)=%@,source=0x%x,destination=0x%x",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
            response = responseMessage;
            weakSelf.firmwareDistributionReceiversList = responseMessage;
            [weakSelf callbackAdvDistributionProgressBlock];
        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
            TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
            [weakSelf createErrorWithString:[NSString stringWithFormat:@"The distributorAddress=0x%04X has not response SigFirmwareDistributionReceiversList", weakSelf.distributorAddress]];
            dispatch_semaphore_signal(weakSelf.semaphore);
        }];
        if (self.phoneIsDistributor) {
            //手机端既作为initiator，也作为Distributor。
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(kCreateSimulatedMessageInterval * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                [self createResponseSigFirmwareDistributionReceiversList];
            });
        }
        //Most provide 3 seconds to firmwareDistributionReceiversGet(Initiator->Distributor) every node.
        dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
    }
    if (response && response.receiversList) {
        if (response.receiversList.count > 0) {
            BOOL needReceiversGetAgain = NO;
            BOOL needCheckNextIndex = NO;
            //distributor端缓存着多个receiver的数据，但为了response返回的数据是unsegment的包，所以只查询一个index的receiver数据。
            for (SigUpdatingNodeEntryModel *model in response.receiversList) {
                //记录：直连节点返回0%-phase=4,B610010000000880010000,B61000000000080000C800
                //B610010000000280010000,B61000000000020000C800
                //记录：非直连节点0x403返回50%-phase=1,02000000038400C800
                TelinkLogVerbose(@"SigUpdatingNodeEntryModel===%@",model.getDetailString);
                if (model.retrievedUpdatePhase == SigFirmwareUpdatePhaseType_transferError || model.retrievedUpdatePhase == SigFirmwareUpdatePhaseType_verificationFailed || model.retrievedUpdatePhase == SigFirmwareUpdatePhaseType_transferCanceled || model.retrievedUpdatePhase == SigFirmwareUpdatePhaseType_applyFailed || model.retrievedUpdatePhase == SigFirmwareUpdatePhaseType_unknown) {
                    [self.failAddressArray addObject:@(model.address)];
                    needCheckNextIndex = YES;
                } else if (model.retrievedUpdatePhase == SigFirmwareUpdatePhaseType_idle || model.retrievedUpdatePhase == SigFirmwareUpdatePhaseType_applyingUpdate || model.retrievedUpdatePhase == SigFirmwareUpdatePhaseType_applySuccess) {
                    needCheckNextIndex = YES;
                } else if (model.retrievedUpdatePhase == SigFirmwareUpdatePhaseType_transferActive) {
                    needReceiversGetAgain = YES;
                } else if (model.retrievedUpdatePhase == SigFirmwareUpdatePhaseType_verificationSuccess || model.retrievedUpdatePhase == SigFirmwareUpdatePhaseType_verifyingUpdate) {
                    if (self.updatePolicy == SigUpdatePolicyType_verifyOnly || self.phoneIsDistributor) {
                        needCheckNextIndex = YES;
                    } else {
                        needReceiversGetAgain = YES;
                    }
                }
            }

            if (needCheckNextIndex) {
                if (self.firstIndexOfFirmwareDistributionReceiversGet + 1 >= self.allAddressArray.count) {
                    //已经查询完所有的地址
                    TelinkLogVerbose(@"");
                    [self firmwareDistributionReceiversGetFinishAction];
                } else {
                    TelinkLogVerbose(@"");
                    self.firstIndexOfFirmwareDistributionReceiversGet++;
                    [self fiemwareUpdateFirmwareDistributionReceiversGet];
                }
            } else if (needReceiversGetAgain) {
                TelinkLogVerbose(@"");
                dispatch_async(dispatch_get_main_queue(), ^{
                    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(fiemwareUpdateFirmwareDistributionReceiversGet) object:nil];
                });
                [self performSelector:@selector(fiemwareUpdateFirmwareDistributionReceiversGet) withObject:nil afterDelay:kFirmwareDistributionReceiversGetInterval];
            }
        } else {
            // 查询不到，说明直连节点作为distributor，且出现了断电的情况。直接进入失败流程。
            self.failAddressArray = [NSMutableArray arrayWithArray:self.allAddressArray];
            [self firmwareDistributionReceiversGetFinishAction];
        }
    } else {
        //查询不到继续下一轮查询。
        if (self.isMeshOTAing) {
            TelinkLogVerbose(@"");
            dispatch_async(dispatch_get_main_queue(), ^{
                [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(fiemwareUpdateFirmwareDistributionReceiversGet) object:nil];
            });
            [self performSelector:@selector(fiemwareUpdateFirmwareDistributionReceiversGet) withObject:nil afterDelay:kFirmwareDistributionReceiversGetInterval];
        }
    }
}

- (void)firmwareDistributionReceiversGetFinishAction {
    if (self.failAddressArray.count == self.allAddressArray.count) {
        //所有设备都已经失败
        if (!self.failError) {
            [self createErrorWithString:@"All nodes have ota fail."];
        }
        TelinkLogInfo(@"%@",@"All nodes have ota fail.");
        [self firmwareUpdateFirmwareDistributionReceiversGetFailAction];
    } else {
        //所有设备已经校验固件完成或者OTA失败，进行下一步
        NSMutableArray *temArr = [NSMutableArray arrayWithArray:self.allAddressArray];
        for (NSNumber *address in temArr) {
            if ([self.failAddressArray containsObject:address]) {
                continue;
            }
            if (![self.successAddressArray containsObject:address]) {
                TelinkLogVerbose(@"0x%X apply success!!!",address);
                [self.successAddressArray addObject:address];
            }
        }
        [self fiemwareUpdateFirmwareDistributionReceiversGetSuccessAction];
    }
}

- (void)createResponseSigFirmwareDistributionReceiversList {
    TelinkLogInfo(@"create SigFirmwareDistributionReceiversList by app.");
    UInt16 receiversListCount = 1;
    UInt16 firstIndex = self.firstIndexOfFirmwareDistributionReceiversGet;
    SigUpdatingNodeEntryModel *model = [[SigUpdatingNodeEntryModel alloc] initWithAddress:self.distributorAddress retrievedUpdatePhase:SigFirmwareUpdatePhaseType_verificationSuccess updateStatus:SigFirmwareUpdateServerAndClientModelStatusType_success transferStatus:SigBLOBTransferStatusType_success transferProgress:50 updateFirmwareImageIndex:0];
    SigFirmwareDistributionReceiversList *responseMessage = [[SigFirmwareDistributionReceiversList alloc] initWithReceiversListCount:receiversListCount firstIndex:firstIndex receiversList:@[model]];
    dispatch_async(SigMeshLib.share.delegateQueue, ^{
        if ([SigMeshLib.share.delegate respondsToSelector:@selector(didReceiveMessage:sentFromSource:toDestination:)]) {
            [SigMeshLib.share.delegate didReceiveMessage:responseMessage sentFromSource:self.distributorAddress toDestination:SigMeshLib.share.dataSource.curLocationNodeModel.address];
        }
    });
}

- (void)fiemwareUpdateFirmwareDistributionReceiversGetSuccessAction {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(fiemwareUpdateFirmwareDistributionReceiversGet) object:nil];
    });
    if (self.updatePolicy == SigUpdatePolicyType_verifyAndApply) {
        if (self.firmwareDistributionReceiversList && self.firmwareDistributionReceiversList.receiversList && self.firmwareDistributionReceiversList.receiversList.count) {
            SigUpdatingNodeEntryModel *receiver = self.firmwareDistributionReceiversList.receiversList.firstObject;
            if ((receiver && receiver.transferProgress == 50 && receiver.retrievedUpdatePhase == SigFirmwareUpdatePhaseType_idle) || receiver.retrievedUpdatePhase == SigFirmwareUpdatePhaseType_verificationSuccess || receiver.retrievedUpdatePhase == SigFirmwareUpdatePhaseType_applyingUpdate || receiver.retrievedUpdatePhase == SigFirmwareUpdatePhaseType_applySuccess) {
                if (self.phoneIsDistributor) {
                    [self performSelector:@selector(firmwareUpdateFirmwareUpdateGet) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
                } else {
//                    self.checkVersionCount = 6;
//                    [self firmwareUpdateFirmwareDistributionGetSuccessAction];
                    self.firmwareDistributionGetCount = 6;
                    [self firmwareUpdateFirmwareUpdateApplySuccessAction];
                }
                return;
            }
        }
        [self performSelector:@selector(firmwareUpdateFirmwareUpdateApply) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
    } else {
        [self performSelector:@selector(firmwareDistributionApply) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
    }
}

- (void)firmwareUpdateFirmwareDistributionReceiversGetFailAction {
    [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

// 8.1.3.7 Verification of downloaded firmware
// 8.6.1.2.7 Refresh Updating Nodes Status procedure(Distributor广播到updating node(s))

// 8.1.3.8 Applying the firmware image to the Updating nodes
// 8.1.3.8.1 Update policies(两种更新策略：Verify Only和Verify And Apply，Verify Only需要等待initiator发送distributionApply到Distributor，对应Figure 8.15和Figure 8.16)
// 8.6.1.2.5 Apply Firmware on Updating Nodes procedure(Distributor广播到updating node(s))
// 8.6.1.2.7 Refresh Updating Nodes Status procedure(Distributor广播到updating node(s))

#pragma mark - Firmware update step23:firmwareDistributionApply(Initiator->Distributor)
- (void)firmwareDistributionApply {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressFirmwareDistributionApply;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);
    if (self.phoneIsDistributor && self.updatePolicy == SigUpdatePolicyType_verifyOnly) {
        [self firmwareUpdateFirmwareUpdateGet];
        return;
    }

    __block BOOL hasFail = NO;
    self.successActionInCurrentProgress = 0;
    __weak typeof(self) weakSelf = self;

    self.semaphore = dispatch_semaphore_create(0);
    self.messageHandle = [SDKLibCommand firmwareDistributionApplyWithDestination:self.distributorAddress retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareDistributionStatus * _Nonnull responseMessage) {
        TelinkLogDebug(@"firmwareDistributionApply(Initiator->Distributor)=%@,source=0x%x,destination=0x%x",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
        if (source == weakSelf.distributorAddress) {
            if (responseMessage.status == SigFirmwareDistributionServerAndClientModelStatusType_success) {

            } else {
                if (responseMessage.status == SigFirmwareDistributionServerAndClientModelStatusType_wrongPhase && responseMessage.distributionPhase == SigDistributionPhaseState_transferActive) {
                    //可能只校验成功一部分设备，这里需要重试流程。
                    dispatch_async(dispatch_get_main_queue(), ^{
                        TelinkLogInfo(@"可能只校验成功一部分设备，这里需要10秒后重试firmwareDistributionApply流程!!!!!!");
                        [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(fiemwareUpdateFirmwareDistributionReceiversGetSuccessAction) object:nil];
                        [weakSelf performSelector:@selector(fiemwareUpdateFirmwareDistributionReceiversGetSuccessAction) withObject:nil afterDelay:10.0];
                    });
                } else {
                    hasFail = YES;
                    [weakSelf createErrorWithString:[NSString stringWithFormat:@"SigFirmwareDistributionStatus.status=0x%x, source=0x%04X", responseMessage.status, source]];
                    TelinkLogInfo(@"%@", [NSString stringWithFormat:@"SigFirmwareDistributionStatus.status=0x%x, source=0x%04X", responseMessage.status, source]);
                }
            }
        } else {
            TelinkLogInfo(@"response from other node.");
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        if (error) {
            hasFail = YES;
            [weakSelf createErrorWithString:[NSString stringWithFormat:@"The distributorAddress=0x%04X has not response SigFirmwareDistributionStatus", weakSelf.distributorAddress]];
        }
        dispatch_semaphore_signal(weakSelf.semaphore);
    }];
    if (self.phoneIsDistributor) {
        //手机端既作为initiator，也作为Distributor。
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(kCreateSimulatedMessageInterval * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            [self createResponseSigFirmwareDistributionStatusWithStatus:SigFirmwareDistributionServerAndClientModelStatusType_success distributionPhase:SigDistributionPhaseState_idle];
        });
    }
    //Most provide 3 seconds to firmwareDistributionStart(Initiator->Distributor) every node.
    dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
    if (hasFail) {
        [self firmwareDistributionApplyFailAction];
    } else {
        [self firmwareDistributionApplySuccessAction];
    }
}

- (void)firmwareDistributionApplySuccessAction {
    if (!self.phoneIsDistributor) {
        //手机不作为Distributor的话，不需要发送firmwareUpdateFirmwareUpdateGet，直接成功。
        [self firmwareUpdateFirmwareUpdateApplySuccessAction];
    } else {
        [self performSelector:@selector(firmwareUpdateFirmwareUpdateGet) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
    }
}

- (void)firmwareDistributionApplyFailAction {
    [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}


#pragma mark - Firmware update step24:firmwareUpdateGet(Distributor->updating node(s))
- (void)firmwareUpdateFirmwareUpdateGet {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressFirmwareUpdateGet;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);

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
        if (node.state != DeviceStateOutOfLine || node.features.lowPowerFeature != SigNodeFeaturesState_notSupported) {
            self.semaphore = dispatch_semaphore_create(0);
            self.messageHandle = [SDKLibCommand firmwareUpdateGetWithDestination:node.address retryCount:SigMeshLib.share.dataSource.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareUpdateStatus * _Nonnull responseMessage) {
                TelinkLogDebug(@"firmwareUpdateGet(Distributor->updating node(s))=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                if (source == nodeAddress.intValue) {
                    if (responseMessage.status == SigFirmwareUpdateStatusType_success) {
//                            if (responseMessage.phase == SigFirmwareUpdatePhaseType_DFUReady) {
//                                if (!responseMessage.additionalInformation.ProvisioningNeeded) {
                                hasSuccess = YES;
//                                } else {
//                                    hasFail = YES;
//                                    weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateGet(Distributor->updating node(s)),responseMessage.additionalInformation.ProvisioningNeeded=0x%x",responseMessage.additionalInformation.ProvisioningNeeded] code:-weakSelf.firmwareUpdateProgress userInfo:nil];
//                                }
//                            } else {
//                                hasFail = YES;
//                                weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateGet(Distributor->updating node(s)),SigFirmwareUpdateStatus.phase=0x%x",responseMessage.phase] code:-weakSelf.firmwareUpdateProgress userInfo:nil];
//                            }
                    } else {
                        [weakSelf createErrorWithString:[NSString stringWithFormat:@"SigFirmwareDistributionReceiversStatus.status=0x%x, source=0x%04X", responseMessage.status, source]];
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
                TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                dispatch_semaphore_signal(weakSelf.semaphore);
            }];
            //Most provide 3 seconds to firmwareUpdateGet(Distributor->updating node(s)) every node.
            dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
        } else {
            [self createErrorWithString:[NSString stringWithFormat:@"node=0x%4X is offline", nodeAddress.intValue]];
            if (![self.failAddressArray containsObject:nodeAddress]) {
                [self.failAddressArray addObject:nodeAddress];
            }
        }
    }
    if (hasSuccess) {
        [self firmwareUpdateFirmwareUpdateGetSuccessAction];
    } else {
        if (!self.failError) {
            [self createErrorWithString:@"All nodes have not response SigFirmwareDistributionReceiversStatus"];
        }
        [self firmwareUpdateFirmwareUpdateGetFailAction];
    }
}

- (void)firmwareUpdateFirmwareUpdateGetSuccessAction {
    [self performSelector:@selector(firmwareUpdateFirmwareUpdateApply) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

- (void)firmwareUpdateFirmwareUpdateGetFailAction {
    [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}


#pragma mark - Firmware update step25:firmwareUpdateApply(Distributor->updating node(s))
- (void)firmwareUpdateFirmwareUpdateApply {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressFirmwareUpdateApply;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);

    if (!self.phoneIsDistributor) {
        //手机不作为Distributor的话，不需要发送firmwareUpdateFirmwareUpdateApply，直接成功。
        [self firmwareUpdateFirmwareUpdateApplySuccessAction];
        return;
    }

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
        if (node.state != DeviceStateOutOfLine || node.features.lowPowerFeature != SigNodeFeaturesState_notSupported) {
            self.semaphore = dispatch_semaphore_create(0);
            self.messageHandle = [SDKLibCommand firmwareUpdateApplyWithDestination:node.address retryCount:10 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareUpdateStatus * _Nonnull responseMessage) {
                TelinkLogDebug(@"firmwareUpdateApply(Distributor->updating node(s))=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                if (source == nodeAddress.intValue) {
                    if (responseMessage.status == SigFirmwareUpdateServerAndClientModelStatusType_success) {
                        if (responseMessage.updatePhase == SigFirmwareUpdatePhaseType_applyingUpdate || responseMessage.updatePhase == SigFirmwareUpdatePhaseType_applySuccess) {
                            //if (responseMessage.updatePhase == SigFirmwareUpdatePhaseType_verificationSuccess) {
//                                if (!responseMessage.additionalInformation.ProvisioningNeeded) {
                                hasSuccess = YES;
//                                // apply 成功，则标记meshOTA成功。
//                                [weakSelf callBackMeshOTASuccessAddress:node.address];
//                                } else {
//                                    hasFail = YES;
//                                    weakSelf.failError = [NSError errorWithDomain:[NSString stringWithFormat:@"fail in firmwareUpdateApply(Distributor->updating node(s)),responseMessage.additionalInformation.ProvisioningNeeded=0x%x",responseMessage.additionalInformation.ProvisioningNeeded] code:-weakSelf.firmwareUpdateProgress userInfo:nil];
//                                }
                        } else {
                            [weakSelf createErrorWithString:[NSString stringWithFormat:@"SigFirmwareUpdateStatus.status=0x%x, updatePhase=0x%x, source=0x%04X", responseMessage.status, responseMessage.updatePhase, source]];
                            if (![weakSelf.failAddressArray containsObject:nodeAddress]) {
                                [weakSelf.failAddressArray addObject:nodeAddress];
                            }
                        }
                    } else {
                        [weakSelf createErrorWithString:[NSString stringWithFormat:@"SigFirmwareUpdateStatus.status=0x%x, updatePhase=0x%x, source=0x%04X", responseMessage.status, responseMessage.updatePhase, source]];
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
                TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    dispatch_semaphore_signal(weakSelf.semaphore);
            }];
            //Most provide 3 seconds to BLOBBlockStart every node.
            dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
        } else {
            [self createErrorWithString:[NSString stringWithFormat:@"node=0x%4X is offline", nodeAddress.intValue]];
            if (![self.failAddressArray containsObject:nodeAddress]) {
                [self.failAddressArray addObject:nodeAddress];
            }
        }
    }
    if (hasSuccess) {
        [self firmwareUpdateFirmwareUpdateApplySuccessAction];
    } else {
        if (!self.failError) {
            [self createErrorWithString:@"All nodes have not response SigFirmwareUpdateStatus"];
        }
        [self firmwareUpdateFirmwareUpdateApplyFailAction];
    }
}

- (void)firmwareUpdateFirmwareUpdateApplySuccessAction {
    [self performSelector:@selector(firmwareUpdateFirmwareDistributionGet) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

- (void)firmwareUpdateFirmwareUpdateApplyFailAction {
    [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

#pragma mark - Firmware update step26:firmwareDistributionGet(Initiator->Distributor,发送到直连节点，需模拟回包)
- (void)firmwareUpdateFirmwareDistributionGet {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressFirmwareDistributionGet;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);

    if (![self isMeshOTAing]) {
        TelinkLog(@"NO in meshOTA progress.");
        return;
    }
    if (!SigBearer.share.isOpen) {
        TelinkLog(@"mesh is not connect.");
        return;
    }
    __block BOOL needGetAgain  = YES;
    __weak typeof(self) weakSelf = self;
    self.firmwareDistributionGetCount --;
    self.semaphore = dispatch_semaphore_create(0);
    self.messageHandle = [SDKLibCommand firmwareDistributionGetWithDestination:self.distributorAddress retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareDistributionStatus * _Nonnull responseMessage) {
        TelinkLogDebug(@"firmwareDistributionGet(Initiator->Distributor)=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
//            if (responseMessage.distributionPhase == SigDistributionPhaseState_completed) {//Distributor apply完成但未重启
        if (responseMessage.distributionPhase == SigDistributionPhaseState_completed || responseMessage.distributionPhase == SigDistributionPhaseState_applyingUpdate || (responseMessage.status == SigFirmwareDistributionServerAndClientModelStatusType_success && responseMessage.distributionPhase == SigDistributionPhaseState_idle)) {//Distributor apply完成但未重启 或者 Distributor 正在apply 或者 Distributor apply完成并重启
            needGetAgain = NO;
        } else {
            [weakSelf createErrorWithString:[NSString stringWithFormat:@"SigFirmwareDistributionStatus.status=0x%x, distributionPhase=0x%x, source=0x%04X", responseMessage.status, responseMessage.distributionPhase, source]];
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        if (error) {
            [weakSelf createErrorWithString:[NSString stringWithFormat:@"The distributorAddress=0x%04X has not response SigFirmwareDistributionStatus", weakSelf.distributorAddress]];
        }
        dispatch_semaphore_signal(weakSelf.semaphore);
    }];
    if (self.phoneIsDistributor) {
        //手机端既作为initiator，也作为Distributor。
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(kCreateSimulatedMessageInterval * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            [self createResponseSigFirmwareDistributionStatusWithStatus:SigFirmwareDistributionServerAndClientModelStatusType_success distributionPhase:SigDistributionPhaseState_completed];
        });
    }
    //Most provide 3 seconds to firmwareUpdateInformationGet every node.
    dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);

    if (self.firmwareUpdateProgress == SigFirmwareUpdateProgressFirmwareDistributionGet) {
        if (needGetAgain && self.firmwareDistributionGetCount > 0) {
            //延时10秒重试
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(firmwareUpdateFirmwareDistributionGet) object:nil];
            [self performSelector:@selector(firmwareUpdateFirmwareDistributionGet) withObject:nil afterDelay:10.0];
        } else {
            if (needGetAgain) {
                //重试完成且都失败
                [self firmwareUpdateFirmwareDistributionGetFailAction];
            } else {
                //成功
                self.checkVersionCount = 6;
                [self firmwareUpdateFirmwareDistributionGetSuccessAction];
            }
        }
    }
}

- (void)firmwareUpdateFirmwareDistributionGetSuccessAction {
    TelinkLogDebug(@"立刻10秒计时firmwareUpdateFirmwareDistributionGetSuccessAction");
    if (self.needCheckVersionAfterApply) {
        if (SigBearer.share.isOpen) {
            [self performSelector:@selector(firmwareUpdateInformationGetCheckVersion) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
        } else {
            TelinkLogDebug(@"Mesh has disconnect, waiting for connection.");
        }
    } else {
        [self performSelector:@selector(firmwareUpdateFirmwareDistributionCancel) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
    }
}

- (void)firmwareUpdateFirmwareDistributionGetFailAction {
    [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

#pragma mark - Firmware update step27:firmwareUpdateInformationGetCheckVersion(Initiator->updating node(s))
- (void)firmwareUpdateInformationGetCheckVersion {
    self.firmwareUpdateProgress = SigFirmwareUpdateInformationGetCheckVersion;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);

    __block BOOL hasSuccess = NO;
    __weak typeof(self) weakSelf = self;
    NSArray *allAddressArray = [NSArray arrayWithArray:self.allAddressArray];
    NSMutableDictionary <NSNumber *,SigFirmwareUpdateInformationStatus *>*receiveStatusDict = [NSMutableDictionary dictionary];
    for (NSNumber *nodeAddress in allAddressArray) {
        if (![self isMeshOTAing]) {
            return;
        }
        if ([self.failAddressArray containsObject:nodeAddress]) {
            continue;
        }
        if ([receiveStatusDict.allKeys containsObject:nodeAddress]) {
            continue;
        }
        self.semaphore = dispatch_semaphore_create(0);
        self.messageHandle = [SDKLibCommand firmwareUpdateInformationGetWithDestination:nodeAddress.intValue firstIndex:0 entriesLimit:1 retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareUpdateInformationStatus * _Nonnull responseMessage) {
            if (responseMessage.firmwareInformationListCount > 0) {
                /*
                 responseMessage.firmwareInformationList.firstObject.currentFirmwareID.length = 4: 2 bytes pid(设备类型) + 2 bytes vid(版本id).
                 */
                if (responseMessage.firmwareInformationList.count > 0) {
                    NSData *currentFirmwareID = responseMessage.firmwareInformationList.firstObject.currentFirmwareID;
                    UInt16 pid = 0,vid = 0;
                    Byte *pu = (Byte *)[currentFirmwareID bytes];
                    if (currentFirmwareID.length >= 2) memcpy(&pid, pu, 2);
                    if (currentFirmwareID.length >= 4) memcpy(&vid, pu + 2, 2);
                    TelinkLogDebug(@"firmwareUpdateInformationGet=%@,pid=%d,vid=%d",[LibTools convertDataToHexStr:currentFirmwareID],pid,vid);
                    BOOL checkVidResult = NO;
                    if (weakSelf.otaData) {
                        //存在Bin数据则获取Bin的vid，如果当前设备的vid == Bin的vid则OTA成功
                        UInt16 binVersion = [weakSelf getVidWithOTAData:weakSelf.otaData];
                        if (binVersion == vid) {
                            checkVidResult = YES;
                        }
                    } else {
                        //不存在Bin数据，则获取设备旧的vid，如果当前设备的vid != 旧的vid则OTA成功
                        SigNodeModel *node = [SigDataSource.share getNodeWithAddress:nodeAddress.intValue];
                        UInt16 oldVid = [[weakSelf.oFirmwareInformation[@(source)] objectForKey:kVid] intValue];
                        if ([LibTools uint16From16String:node.vid] != vid || ([weakSelf.oFirmwareInformation.allKeys containsObject:@(source)] && oldVid != vid)) {
                            checkVidResult = YES;
                        }
                    }
                    if (checkVidResult) {
                        weakSelf.nFirmwareInformation[@(source)] = @{kPid:@(pid),kVid:@(vid)};
                        [SigDataSource.share updateNodeModelVidWithAddress:nodeAddress.intValue vid:vid];
                        receiveStatusDict[@(source)] = responseMessage;
                        [weakSelf callBackMeshOTASuccessAddress:source];
                        hasSuccess = YES;
                    }
                }
            }
        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
            TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
            if (error) {
                [weakSelf createErrorWithString:[NSString stringWithFormat:@"The distributorAddress=0x%04X has not response SigFirmwareUpdateInformationStatus", nodeAddress.intValue]];
            }
            dispatch_semaphore_signal(weakSelf.semaphore);
        }];
        //Most provide 3 seconds to firmwareUpdateInformationGet every node.
        dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
        if (allAddressArray.count <= self.failAddressArray.count + receiveStatusDict.allKeys.count) {
            //获取完成
            break;
        }
    }
    self.checkVersionCount -= 1;
//    if (!SigBearer.share.isOpen) {
//        //可能查询中途断开连接，导致查询逻辑异常，直接返回MeshOTA失败。所有此处添加return。
//        return;
//    }
    if ((allAddressArray.count > self.failAddressArray.count + receiveStatusDict.allKeys.count) && self.checkVersionCount > 0) {
        //未获取完成，延时10秒后再重新获取一轮，一共6轮。
        [self performSelector:@selector(cancelAllAfterDelay) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
        TelinkLogDebug(@"开始10秒计时firmwareUpdateFirmwareDistributionGetSuccessAction");
        [self performSelector:@selector(firmwareUpdateFirmwareDistributionGetSuccessAction) withObject:nil afterDelay:10.0];
        return;
    }

    if (allAddressArray.count > self.failAddressArray.count + receiveStatusDict.allKeys.count) {
        //重试多次也存在未获取到状态的节点，将其归类到失败的节点中去。
        for (NSNumber *nodeAddress in allAddressArray) {
            if (![self.failAddressArray containsObject:nodeAddress] && ![receiveStatusDict.allKeys containsObject:nodeAddress]) {
                [self.failAddressArray addObject:nodeAddress];
            }
        }
    }

    if (hasSuccess) {
        [self firmwareUpdateInformationGetCheckVersionSuccessAction];
    } else {
        [self createErrorWithString:@"No nodes updated to the new bin version"];
        [self firmwareUpdateInformationGetCheckVersionFailAction];
    }
}

- (void)firmwareUpdateInformationGetCheckVersionSuccessAction {
    [self performSelector:@selector(firmwareUpdateFirmwareDistributionCancel) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

- (void)firmwareUpdateInformationGetCheckVersionFailAction {
    self.failError = [NSError errorWithDomain:@"fail in firmwareUpdateInformationGetCheckVersion(Initiator->updating node(s))" code:-self.firmwareUpdateProgress userInfo:nil];
    [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

#pragma mark - Firmware update step28:firmwareDistributionCancel(Initiator->Distributor,发送到直连节点，需模拟回包)
- (void)firmwareUpdateFirmwareDistributionCancel {
    self.firmwareUpdateProgress = SigFirmwareUpdateProgressFirmwareDistributionCancel;
    TelinkLogInfo(@"\n\n==========firmware update:step%d\n\n",self.firmwareUpdateProgress);
    [self performSelector:@selector(cancelAllAfterDelay) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
    //当直连节点作为distributor但distributor不需要OTA时，需要给distributor发送cancel指令。
    __block BOOL hasFail = NO;
    self.successActionInCurrentProgress = 0;
    __weak typeof(self) weakSelf = self;

    self.semaphore = dispatch_semaphore_create(0);
    self.messageHandle = [SDKLibCommand firmwareDistributionCancelWithDestination:self.distributorAddress retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareDistributionStatus * _Nonnull responseMessage) {
        TelinkLogDebug(@"firmwareDistributionCancel(Initiator->Distributor)=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        if (error) {
            hasFail = YES;
            [weakSelf createErrorWithString:[NSString stringWithFormat:@"The distributorAddress=0x%04X has not response SigFirmwareDistributionStatus", weakSelf.distributorAddress]];
        }
        dispatch_semaphore_signal(weakSelf.semaphore);
    }];
    if (self.phoneIsDistributor) {
        //手机端既作为initiator，也作为Distributor。
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(kCreateSimulatedMessageInterval * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            [self createResponseSigFirmwareDistributionStatusWithStatus:SigFirmwareDistributionServerAndClientModelStatusType_success distributionPhase:SigDistributionPhaseState_idle];
        });
    }
    //Most provide 3 seconds to firmwareDistributionCancel(Initiator->Distributor) every node.
    dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
    if (hasFail) {
        [self firmwareUpdateFirmwareDistributionCancelFailAction];
    } else {
        [self firmwareUpdateFirmwareDistributionCancelSuccessAction];
    }
}

- (void)firmwareUpdateFirmwareDistributionCancelSuccessAction {
    SigBearer.share.isAutoReconnect = YES;
    [self performSelector:@selector(showMeshOTAResult) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

- (void)firmwareUpdateFirmwareDistributionCancelFailAction {
    [self performSelector:@selector(firmwareUpdateFailAction) onThread:self.meshOTAThread withObject:nil waitUntilDone:YES];
}

#pragma mark - firmware update fail
- (void)firmwareUpdateFailAction {
    if (SigBearer.share.isOpen) {
        __weak typeof(self) weakSelf = self;
        self.semaphore = dispatch_semaphore_create(0);
        self.messageHandle = [SDKLibCommand firmwareDistributionCancelWithDestination:weakSelf.distributorAddress retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareDistributionStatus * _Nonnull responseMessage) {
            TelinkLogDebug(@"initiator firmwareDistributionCancel=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);

        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
            TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
            dispatch_semaphore_signal(weakSelf.semaphore);
        }];
        dispatch_semaphore_wait(self.semaphore, kTimeOutOfEveryStep);
        [SigDataSource.share setAllDevicesOutline];
    } else {
        [ConnectTools.share stopConnectToolsWithComplete:nil];
        [SigDataSource.share setAllDevicesOutline];
    }
    if (self.errorBlock) {
        if (!self.failError) {
            self.failError = [NSError errorWithDomain:@"firmware update fail" code:-1 userInfo:nil];
        }
        self.errorBlock(self.failError);
    }
    [self stopFirmwareUpdateWithCompleteHandle:nil];
}

- (void)recoveryExtendBearerMode {
    NSNumber *extendBearerMode = [[NSUserDefaults standardUserDefaults] valueForKey:kExtendBearerMode];
    UInt8 extend = [extendBearerMode intValue];
    SigDataSource.share.telinkExtendBearerMode = extend;
    if (extend == SigTelinkExtendBearerMode_noExtend) {
        //(默认)关闭DLE功能后，SDK的Access消息是长度大于15字节才进行segment分包。
        SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength = kUnsegmentedMessageLowerTransportPDUMaxLength;
    } else {
        //(可选)打开DLE功能后，SDK的Access消息是长度大于229字节才进行segment分包。
        SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength = kDLEUnsegmentLength;
    }
}

- (void)resetInitiatorMeshOTAData {
    self.otaData = nil;
    self.failError = nil;
    self.advDistributionProgressBlock = nil;
    self.finishBlock = nil;
    self.errorBlock = nil;
    [self stopLPNReachablleTimer];
}

- (void)callbackAdvDistributionProgressBlock {
    if (self.firmwareDistributionReceiversList && self.advDistributionProgressBlock) {
        self.advDistributionProgressBlock(self.firmwareDistributionReceiversList);
    }
}

- (UInt16)getVidWithOTAData:(NSData *)data {
    UInt16 vid = 0;
    Byte *tempBytes = (Byte *)[data bytes];
    if (data && data.length >= 6) {
        memcpy(&vid, tempBytes + 0x4, 2);
    }
    return vid;
}

@end
