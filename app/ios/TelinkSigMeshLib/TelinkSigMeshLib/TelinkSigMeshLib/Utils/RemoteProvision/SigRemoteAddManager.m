/********************************************************************************************************
 * @file     SigRemoteAddManager.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2020/3/26
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

#import "SigRemoteAddManager.h"
#import "SigProvisioningData.h"
#import "SigECCEncryptHelper.h"
#import "SDKLibCommand+remoteProvision.h"

typedef void(^RemotePDUResultCallBack)(BOOL isSuccess);

@interface SigRemoteAddManager ()<SigMessageDelegate>
@property (nonatomic,assign) BOOL isRemoteScanning;
@property (nonatomic,assign) UInt8 currentMaxScannedItems;
@property (nonatomic,weak) id oldDelegateForDeveloper;
@property (nonatomic,strong) NSMutableArray <SigRemoteScanRspModel *>*remoteScanRspModels;
@property (nonatomic,copy) remoteProvisioningScanReportCallBack reportBlock;
@property (nonatomic,copy) resultBlock scanResultBlock;

@property (nonatomic,assign) UInt16 unicastAddress;
@property (nonatomic,strong) NSData *staticOobData;
@property (nonatomic,strong) SigScanRspModel *unprovisionedDevice;
@property (nonatomic,copy) addDevice_provisionSuccessCallBack provisionSuccessBlock;
@property (nonatomic,copy,nullable) addDevice_capabilitiesWithReturnCallBack capabilitiesResponseBlock;
@property (nonatomic,copy) ErrorBlock failBlock;
@property (nonatomic,assign) BOOL isProvisionning;
@property (nonatomic,assign) BOOL isLinkOpenReportWaiting;

// for remote provision
@property (nonatomic,assign) UInt16 reportNodeAddress;
@property (nonatomic,strong) NSData *reportNodeUUID;
@property (nonatomic,assign) NSInteger outboundPDUNumber;
@property (nonatomic,assign) NSInteger inboundPDUNumber;
@property (nonatomic,copy,nullable) prvisionResponseCallBack provisionResponseBlock;
@property (nonatomic,strong) SigMessageHandle *messageHandle;
@property (nonatomic, retain) dispatch_semaphore_t semaphore;

//缓存旧的block，防止添加设备后不断开的情况下，SigBearer下的断开block不运行。
@property (nonatomic,copy) bleDisconnectCallback oldBluetoothDisconnectCallback;

@end

@implementation SigRemoteAddManager

/**
 *  @brief  Singleton method
 *
 *  @return the default singleton instance. You are not allowed to create your own instances of this class.
 */
+ (instancetype)share {
    /// Singleton instance
    static SigRemoteAddManager *shareManager = nil;
    /// Note: The dispatch_once function can ensure that a certain piece
    /// of code is only executed once in the entire application life cycle!
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        /// Initialize the Singleton configure parameters.
        shareManager = [[SigRemoteAddManager alloc] init];
        shareManager.attentionDuration = 0;
    });
    return shareManager;
}

#pragma mark - remote scan

- (void)startRemoteProvisionScanWithReportCallback:(remoteProvisioningScanReportCallBack)reportCallback resultCallback:(resultBlock)resultCallback {
    if (self.isRemoteScanning) {
        NSString *errstr = @"SigRemoteAddManager is remoteScanning, please call `stopRemoteProvisionScan`";
        TelinkLogError(@"%@",errstr);
        NSError *err = [NSError errorWithDomain:errstr code:-1 userInfo:nil];
        if (resultCallback) {
            resultCallback(NO, err);
        }
        return;
    }
    self.reportBlock = reportCallback;
    self.scanResultBlock = resultCallback;
    self.oldDelegateForDeveloper = SigMeshLib.share.delegateForDeveloper;
    SigMeshLib.share.delegateForDeveloper = self;
    self.remoteScanRspModels = [NSMutableArray array];
    self.isRemoteScanning = YES;
    [self startSingleRemoteProvisionScan];
}

- (void)stopRemoteProvisionScan {
    self.isRemoteScanning = NO;
}

- (void)endRemoteProvisionScan {
    self.isRemoteScanning = NO;
    //因为上面for循环发送了多次，在这里清理block。
    [SigMeshLib.share cleanAllCommandsAndRetry];
    if (self.currentMaxScannedItems >= 0x04 && self.currentMaxScannedItems <= 0xFF) {
        if (self.scanResultBlock) {
            self.scanResultBlock(YES, nil);
        }
    } else {
        NSString *errstr = @"Reomte scan fail: current connected node is no support remote provision scan.";
        TelinkLogError(@"%@",errstr);
        NSError *err = [NSError errorWithDomain:errstr code:-1 userInfo:nil];
        if (self.scanResultBlock) {
            self.scanResultBlock(NO, err);
        }
    }
}

- (void)startSingleRemoteProvisionScan {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(endSingleRemoteProvisionScan) object:nil];
        [self performSelector:@selector(endSingleRemoteProvisionScan) withObject:nil afterDelay:kScanCapabilitiesTimeout];
    });
    [self getRemoteProvisioningScanCapabilities];
}

- (void)endSingleRemoteProvisionScan {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(endSingleRemoteProvisionScan) object:nil];
    });
    [self endRemoteProvisionScan];
}

#pragma mark - remote provision

/// founcation4: remote provision (SDK need connected provisioned node.)
/// @param reportNodeAddress address of node that report this uuid
/// @param reportNodeUUID identify node that need to provision.
/// @param networkKey networkKey
/// @param netkeyIndex netkeyIndex
/// @param provisionType ProvisionType_NoOOB means oob data is 16 bytes zero data, ProvisionType_StaticOOB means oob data is get from HTTP API.
/// @param staticOOBData oob data get from HTTP API when provisionType is ProvisionType_StaticOOB.
/// @param capabilitiesResponse callback when capabilities response.
/// @param provisionSuccess callback when provision success.
/// @param fail callback when provision fail.
- (void)remoteProvisionWithNReportNodeAddress:(UInt16)reportNodeAddress reportNodeUUID:(NSData *)reportNodeUUID networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex provisionType:(ProvisionType)provisionType staticOOBData:(nullable NSData *)staticOOBData capabilitiesResponse:(nullable addDevice_capabilitiesWithReturnCallBack)capabilitiesResponse provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail {
    //since v3.3.3 每次provision前都初始化一次ECC算法的公私钥。
    [SigECCEncryptHelper.share eccInit];

    self.reportNodeAddress = reportNodeAddress;
    self.reportNodeUUID = reportNodeUUID;
    self.oldDelegateForDeveloper = SigMeshLib.share.delegateForDeveloper;
    SigMeshLib.share.delegateForDeveloper = self;
    self.staticOobData = staticOOBData;
    self.capabilitiesResponseBlock = capabilitiesResponse;
    self.provisionSuccessBlock = provisionSuccess;
    self.failBlock = fail;
    self.unprovisionedDevice = [SigMeshLib.share.dataSource getScanRspModelWithUUID:[SigBearer.share getCurrentPeripheral].identifier.UUIDString];
    SigNetkeyModel *provisionNet = nil;
    NSArray *netKeys = [NSArray arrayWithArray:SigMeshLib.share.dataSource.netKeys];
    for (SigNetkeyModel *net in netKeys) {
        if (([networkKey isEqualToData:[LibTools nsstringToHex:net.key]] || (net.phase == distributingKeys && [networkKey isEqualToData:[LibTools nsstringToHex:net.oldKey]])) && netkeyIndex == net.index) {
            provisionNet = net;
            break;
        }
    }
    if (provisionNet == nil) {
        TelinkLogError(@"error network key.");
        return;
    }
    [self reset];
    [SigBearer.share setBearerProvisioned:YES];//remote provision走proxy通道。
    self.networkKey = provisionNet;
    self.isProvisionning = YES;
    _outboundPDUNumber = -1;
    _inboundPDUNumber = -1;
    self.isLinkOpenReportWaiting = YES;
    __weak typeof(self) weakSelf = self;
    TelinkLogInfo(@"start provision.");
    self.oldBluetoothDisconnectCallback = SigBluetooth.share.bluetoothDisconnectCallback;
    [SigBluetooth.share setBluetoothDisconnectCallback:^(CBPeripheral * _Nonnull peripheral, NSError * _Nonnull error) {
        [SigMeshLib.share cleanAllCommandsAndRetry];
        if ([peripheral.identifier.UUIDString isEqualToString:SigBearer.share.getCurrentPeripheral.identifier.UUIDString]) {
            if (weakSelf.isProvisionning) {
                TelinkLogInfo(@"disconnect in provisioning，provision fail.");
                if (fail) {
                    weakSelf.isProvisionning = NO;
                    NSError *err = [NSError errorWithDomain:@"disconnect in provisioning，provision fail." code:-1 userInfo:nil];
                    fail(err);
                }
            }
        }
    }];

    self.messageHandle = [SDKLibCommand remoteProvisioningLinkOpenWithDestination:reportNodeAddress UUID:reportNodeUUID retryCount:kRemoteProgressRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigRemoteProvisioningLinkStatus * _Nonnull responseMessage) {
        TelinkLogInfo(@"linkOpen responseMessage=%@,parameters=%@,source=0x%x,destination=0x%x",responseMessage,responseMessage.parameters,source,destination);
//        if (responseMessage.status == SigRemoteProvisioningStatus_success) {
//            [weakSelf getCapabilitiesWithTimeout:kRemoteProgressTimeout callback:^(SigProvisioningResponse * _Nullable response) {
//                [weakSelf getCapabilitiesResultWithResponse:response];
//            }];
//        } else {
//            [self provisionFail];
//            TelinkLogDebug(@"sentProvisionConfirmationPdu error, parameters= %@",responseMessage.parameters);
//        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        if (error) {
            [weakSelf provisionFail];
            TelinkLogDebug(@"sentProvisionConfirmationPdu error = %@",error.domain);
        }
    }];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(remoteProvisionTimeout) object:nil];
        [self performSelector:@selector(remoteProvisionTimeout) withObject:nil afterDelay:kRemoteProgressTimeout];
    });
}

/// founcation4: remote provision (SDK need connected provisioned node.)
/// @param provisionAddress address of new device.
/// @param reportNodeAddress address of node that report this uuid
/// @param reportNodeUUID identify node that need to provision.
/// @param networkKey networkKey
/// @param netkeyIndex netkeyIndex
/// @param provisionType ProvisionType_NoOOB means oob data is 16 bytes zero data, ProvisionType_StaticOOB means oob data is get from HTTP API.
/// @param staticOOBData oob data get from HTTP API when provisionType is ProvisionType_StaticOOB.
/// @param provisionSuccess callback when provision success.
/// @param fail callback when provision fail.
- (void)remoteProvisionWithNextProvisionAddress:(UInt16)provisionAddress reportNodeAddress:(UInt16)reportNodeAddress reportNodeUUID:(NSData *)reportNodeUUID networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex provisionType:(ProvisionType)provisionType staticOOBData:(nullable NSData *)staticOOBData provisionSuccess:(addDevice_provisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail {
    self.unicastAddress = provisionAddress;
    [self remoteProvisionWithNReportNodeAddress:reportNodeAddress reportNodeUUID:reportNodeUUID networkKey:networkKey netkeyIndex:netkeyIndex provisionType:provisionType staticOOBData:staticOOBData capabilitiesResponse:nil provisionSuccess:provisionSuccess fail:fail];
}

- (void)remoteProvisionTimeout {
    [self provisionFail];
}

// 1.remoteProvisioningScanCapabilitiesGet
- (void)getRemoteProvisioningScanCapabilities {
    TelinkLogVerbose(@"getRemoteProvisioningScanCapabilities address=0x%x",SigMeshLib.share.dataSource.getCurrentConnectedNode.address);
    __weak typeof(self) weakSelf = self;
    self.messageHandle = [SDKLibCommand remoteProvisioningScanCapabilitiesGetWithDestination:SigMeshLib.share.dataSource.getCurrentConnectedNode.address retryCount:kRemoteProgressRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigRemoteProvisioningScanCapabilitiesStatus * _Nonnull responseMessage) {
//        if (responseMessage.activeScan) {
            weakSelf.currentMaxScannedItems = responseMessage.maxScannedItems;
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(endSingleRemoteProvisionScan) object:nil];
            [weakSelf performSelector:@selector(startRemoteProvisioningScan) withObject:nil afterDelay:0.5];
        });
//        } else {
//            TelinkLogInfo(@"nodeAddress 0x%x no support remote provision scan, try next address.");
//            [weakSelf endSingleRemoteProvisionScan];
//        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        if (error) {
            TelinkLogInfo(@"nodeAddress 0x%x get remote provision scan Capabilities timeout.",SigMeshLib.share.dataSource.unicastAddressOfConnected);
            [weakSelf endSingleRemoteProvisionScan];
        }
    }];
}

// 2.remoteProvisioningScanStart
- (void)startRemoteProvisioningScan {
    //since v3.2.2, remote provision with deviceKey.
    __weak typeof(self) weakSelf = self;
    NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
    [operationQueue addOperationWithBlock:^{
        //这个block语句块在子线程中执行
        for (SigNodeModel *node in SigMeshLib.share.dataSource.curNodes) {
            if (!weakSelf.isRemoteScanning) {
                return;
            }
            weakSelf.semaphore = dispatch_semaphore_create(0);
            weakSelf.messageHandle = [SDKLibCommand remoteProvisioningScanStartWithDestination:node.address scannedItemsLimit:kScannedItemsLimit timeout:kScannedItemsTimeout UUID:nil retryCount:0 responseMaxCount:0 successCallback:^(UInt16 source, UInt16 destination, SigRemoteProvisioningScanStatus * _Nonnull responseMessage) {
                TelinkLogInfo(@"source=0x%x,destination=0x%x,opCode=0x%x,parameters=%@",source,destination,responseMessage.opCode,[LibTools convertDataToHexStr:responseMessage.parameters]);
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                [NSThread sleepForTimeInterval:0.2];
                dispatch_semaphore_signal(weakSelf.semaphore);
            }];
            //Most provide 3 seconds to send remoteProvisioningScan every node.
            dispatch_semaphore_wait(self.semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * kSendOneNodeScanTimeout));
        }
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(endSingleRemoteProvisionScan) object:nil];
            [weakSelf performSelector:@selector(endSingleRemoteProvisionScan) withObject:nil afterDelay:kScannedItemsTimeout*2];
        });
    }];
}

- (void)setState:(ProvisioningState)state{
    _state = state;
    if (state == ProvisioningState_fail) {
        [self reset];
        SigMeshLib.share.delegateForDeveloper = self.oldDelegateForDeveloper;
        if (self.failBlock) {
            NSError *err = [NSError errorWithDomain:@"provision fail." code:-1 userInfo:nil];
            self.failBlock(err);
        }
    }
}

- (BOOL)isDeviceSupported{
    if (self.provisioningCapabilities.provisionType != SigProvisioningPduType_capabilities || self.provisioningCapabilities.numberOfElements == 0) {
        TelinkLogError(@"Capabilities is error.");
        return NO;
    }
    return self.provisioningCapabilities.algorithms.fipsP256EllipticCurve == 1;
}

- (void)sendRemoteProvisionPDUWithOutboundPDUNumber:(UInt8)outboundPDUNumber provisioningPDU:(NSData *)provisioningPDU retryCount:(NSInteger)retryCount complete:(RemotePDUResultCallBack)complete {
    self.messageHandle = [SDKLibCommand remoteProvisioningPDUSendWithDestination:self.reportNodeAddress outboundPDUNumber:outboundPDUNumber provisioningPDU:provisioningPDU retryCount:retryCount responseMaxCount:0 resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        if (error) {
            TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        }
    }];
}

- (void)provisionSuccess{
    [self cancelLastMessageHandle];

    UInt16 address = self.provisioningData.unicastAddress;
    NSString *identify = nil;
    identify = [LibTools convertDataToHexStr:_reportNodeUUID];
    UInt8 ele_count = self.provisioningCapabilities.numberOfElements;
    NSData *devKeyData = self.provisioningData.deviceKey;
    TelinkLogInfo(@"deviceKey=%@",devKeyData);

    SigNodeModel *model = [[SigNodeModel alloc] init];
    [model setAddress:address];
    model.deviceKey = [LibTools convertDataToHexStr:devKeyData];
    model.peripheralUUID = nil;
    model.UUID = identify;
    //Attention: There isn't scanModel at remote add, so develop need add macAddress in provisionSuccessCallback.
    model.macAddress = [LibTools convertDataToHexStr:[LibTools turnOverData:[_reportNodeUUID subdataWithRange:NSMakeRange(_reportNodeUUID.length - 6, 6)]]];
    SigNodeKeyModel *nodeNetkey = [[SigNodeKeyModel alloc] init];
    nodeNetkey.index = self.networkKey.index;
    if (![model.netKeys containsObject:nodeNetkey]) {
        [model.netKeys addObject:nodeNetkey];
    }

    SigPage0 *compositionData = [[SigPage0 alloc] init];
    NSMutableArray *elements = [NSMutableArray array];
    if (ele_count) {
        for (int i=0; i<ele_count; i++) {
            SigElementModel *ele = [[SigElementModel alloc] init];
            [elements addObject:ele];
        }
    }
    compositionData.elements = elements;
    model.compositionData = compositionData;

    [SigMeshLib.share.dataSource addAndSaveNodeToMeshNetworkWithDeviceModel:model];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self];
    });
}

- (void)provisionFail {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self];
    });
    SigBluetooth.share.bluetoothDisconnectCallback = self.oldBluetoothDisconnectCallback;
    [self.messageHandle cancel];
    self.state = ProvisioningState_fail;
    self.provisionResponseBlock = nil;
}

/// This method should be called when the OOB value has been received and Auth Value has been calculated. It computes and sends the Provisioner Confirmation to the device.
/// @param data The 16 byte long Auth Value.
- (void)authValueReceivedData:(NSData *)data {
    SigAuthenticationModel *auth = nil;
    self.authenticationModel = auth;
    [self.provisioningData provisionerDidObtainAuthValue:data];
    NSData *provisionerConfirmationData = [self.provisioningData provisionerConfirmation];
    SigProvisioningConfirmationPdu *pdu = [[SigProvisioningConfirmationPdu alloc] initWithConfirmation:provisionerConfirmationData];
//    TelinkLogInfo(@"app端的Confirmation=%@",[LibTools convertDataToHexStr:provisionerConfirmationData]);
    self.outboundPDUNumber = 3;
    [self sendRemoteProvisionPDUWithOutboundPDUNumber:self.outboundPDUNumber provisioningPDU:pdu.pduData retryCount:kRemoteProgressRetryCount complete:nil];
}

/// Resets the provisioning properties and state.
- (void)reset {
    self.authenticationMethod = 0;
    memset(&_provisioningCapabilities, 0, sizeof(_provisioningCapabilities));
    self.provisioningData = nil;
    self.state = ProvisioningState_ready;
    self.isLinkOpenReportWaiting = NO;
    [SigBearer.share setBearerProvisioned:YES];
}

- (void)cancelLastMessageHandle {
    if (self.messageHandle) {
        [self.messageHandle cancel];
    }
}

#pragma mark step1:getCapabilities
- (void)getCapabilitiesWithTimeout:(NSTimeInterval)timeout callback:(prvisionResponseCallBack)block {
    TelinkLogInfo(@"\n\n==========remote provision:step1\n\n");
    self.provisionResponseBlock = block;
    [self cancelLastMessageHandle];
    [self identifyWithAttentionDuration:self.attentionDuration];
}

/// This method get the Capabilities of the device.
/// @param attentionDuration 0x00, Off; 0x01–0xFF, On, remaining time in seconds.
- (void)identifyWithAttentionDuration:(UInt8)attentionDuration {

    // Has the provisioning been restarted?
    if (self.state == ProvisioningState_fail) {
        [self reset];
    }

    // Is the Provisioner Manager in the right state?
    if (self.state != ProvisioningState_ready) {
        TelinkLogError(@"current node isn't in ready.");
        return;
    }

    // Initialize provisioning data.
    self.provisioningData = [[SigProvisioningData alloc] init];

    self.state = ProvisioningState_requestingCapabilities;

    SigProvisioningInvitePdu *pdu = [[SigProvisioningInvitePdu alloc] initWithAttentionDuration:attentionDuration];
    self.outboundPDUNumber = 0;
    self.provisioningData.provisioningInvitePDUValue = [pdu.pduData subdataWithRange:NSMakeRange(1, pdu.pduData.length-1)];
    [self sendRemoteProvisionPDUWithOutboundPDUNumber:self.outboundPDUNumber provisioningPDU:pdu.pduData retryCount:kRemoteProgressRetryCount complete:nil];
}

#pragma mark step2:start
- (void)sentStartNoOobProvisionPduWithTimeout:(NSTimeInterval)timeout callback:(prvisionResponseCallBack)block {
    TelinkLogInfo(@"\n\n==========remote provision:step2(noOob)\n\n");
    // Is the Provisioner Manager in the right state?
    if (self.state != ProvisioningState_capabilitiesReceived) {
        TelinkLogError(@"current state is wrong.");
        return;
    }

    // Ensure the Network Key is set.
    if (self.networkKey == nil) {
        TelinkLogError(@"current networkKey isn't specified.");
        return;
    }

    // Is the Bearer open?
    if (!SigBearer.share.isOpen) {
        TelinkLogError(@"current node's bearer isn't open.");
        return;
    }

    self.provisionResponseBlock = block;
    [self cancelLastMessageHandle];

    self.provisioningData.algorithm = [self getCurrentProvisionAlgorithm];
    [self.provisioningData generateProvisionerRandomAndProvisionerPublicKey];

    // Send Provisioning Start request.
    self.state = ProvisioningState_provisioning;
    [self.provisioningData prepareWithNetwork:SigMeshLib.share.dataSource networkKey:self.networkKey unicastAddress:self.unicastAddress];
    PublicKey *publicKey = [[PublicKey alloc] initWithPublicKeyType:PublicKeyType_noOobPublicKey];
    AuthenticationMethod authenticationMethod = AuthenticationMethod_noOob;
    self.authenticationMethod = AuthenticationMethod_noOob;

    SigProvisioningStartPdu *startPdu = [[SigProvisioningStartPdu alloc] initWithAlgorithm:[self getCurrentProvisionAlgorithm] publicKeyType:publicKey.publicKeyType authenticationMethod:authenticationMethod authenticationAction:0 authenticationSize:0];
    self.outboundPDUNumber = 1;
    self.provisioningData.provisioningStartPDUValue = [startPdu.pduData subdataWithRange:NSMakeRange(1, startPdu.pduData.length-1)];
    [self sendRemoteProvisionPDUWithOutboundPDUNumber:self.outboundPDUNumber provisioningPDU:startPdu.pduData retryCount:kRemoteProgressRetryCount complete:nil];
}

- (void)sentStartStaticOobProvisionPduAndPublicKeyPduWithStaticOobData:(NSData *)oobData timeout:(NSTimeInterval)timeout callback:(prvisionResponseCallBack)block {
    TelinkLogInfo(@"\n\n==========remote provision:step2(staticOob)\n\n");
    // Is the Provisioner Manager in the right state?
    if (self.state != ProvisioningState_capabilitiesReceived) {
        TelinkLogError(@"current state is wrong.");
        return;
    }

    // Ensure the Network Key is set.
    if (self.networkKey == nil) {
        TelinkLogError(@"current networkKey isn't specified.");
        return;
    }

    // Is the Bearer open?
    if (!SigBearer.share.isOpen) {
        TelinkLogError(@"current node's bearer isn't open.");
        return;
    }

    self.provisionResponseBlock = block;
    [self cancelLastMessageHandle];

    self.provisioningData.algorithm = [self getCurrentProvisionAlgorithm];
    [self.provisioningData generateProvisionerRandomAndProvisionerPublicKey];
    [self.provisioningData provisionerDidObtainAuthValue:oobData];

    // Send Provisioning Start request.
    self.state = ProvisioningState_provisioning;
    [self.provisioningData prepareWithNetwork:SigMeshLib.share.dataSource networkKey:self.networkKey unicastAddress:self.unicastAddress];
    PublicKey *publicKey = [[PublicKey alloc] initWithPublicKeyType:PublicKeyType_noOobPublicKey];
    AuthenticationMethod authenticationMethod = AuthenticationMethod_staticOob;
    self.authenticationMethod = AuthenticationMethod_staticOob;

    self.provisioningData.algorithm = [self getCurrentProvisionAlgorithm];
    SigProvisioningStartPdu *startPdu = [[SigProvisioningStartPdu alloc] initWithAlgorithm:[self getCurrentProvisionAlgorithm] publicKeyType:publicKey.publicKeyType authenticationMethod:authenticationMethod authenticationAction:0 authenticationSize:0];
    self.outboundPDUNumber = 1;
    self.provisioningData.provisioningStartPDUValue = [startPdu.pduData subdataWithRange:NSMakeRange(1, startPdu.pduData.length-1)];
    [self sendRemoteProvisionPDUWithOutboundPDUNumber:self.outboundPDUNumber provisioningPDU:startPdu.pduData retryCount:kRemoteProgressRetryCount complete:nil];
}

#pragma mark step2.5:Publickey
- (void)sentProvisionPublickeyPduWithTimeout:(NSTimeInterval)timeout callback:(prvisionResponseCallBack)block {
    TelinkLogInfo(@"\n\n==========remote provision:step2.5(noOob)\n\n");

    self.provisionResponseBlock = block;
    [self cancelLastMessageHandle];
    // Send the Public Key of the Provisioner.
    SigProvisioningPublicKeyPdu *publicPdu = [[SigProvisioningPublicKeyPdu alloc] initWithPublicKey:self.provisioningData.provisionerPublicKey];
    self.outboundPDUNumber = 2;
    [self sendRemoteProvisionPDUWithOutboundPDUNumber:self.outboundPDUNumber provisioningPDU:publicPdu.pduData retryCount:kRemoteProgressRetryCount complete:nil];
}

#pragma mark step3:Confirmation
- (void)sentProvisionConfirmationPduWithTimeout:(NSTimeInterval)timeout callback:(prvisionResponseCallBack)block {
    TelinkLogInfo(@"\n\n==========remote provision:step3\n\n");
    self.provisionResponseBlock = block;
    NSData *authValue = nil;
    if (self.staticOobData) {
        //当前设置为static oob provision
        authValue = self.staticOobData;
    } else {
        //当前设置为no oob provision
        UInt8 value[32] = {};
        memset(&value, 0, 32);
        if ([self getCurrentProvisionAlgorithm] == Algorithm_fipsP256EllipticCurve) {
            authValue = [NSData dataWithBytes:&value length:16];
        } else if ([self getCurrentProvisionAlgorithm] == Algorithm_fipsP256EllipticCurve_HMAC_SHA256) {
            authValue = [NSData dataWithBytes:&value length:32];
        }
    }
    [self cancelLastMessageHandle];

    [self authValueReceivedData:authValue];
}

#pragma mark step4:Random
- (void)sentProvisionRandomPduWithTimeout:(NSTimeInterval)timeout callback:(prvisionResponseCallBack)block {
    TelinkLogInfo(@"\n\n==========remote provision:step4\n\n");
    self.provisionResponseBlock = block;
    [self cancelLastMessageHandle];
    SigProvisioningRandomPdu *pdu = [[SigProvisioningRandomPdu alloc] initWithRandom:self.provisioningData.provisionerRandom];
    self.outboundPDUNumber = 4;
    [self sendRemoteProvisionPDUWithOutboundPDUNumber:self.outboundPDUNumber provisioningPDU:pdu.pduData retryCount:kRemoteProgressRetryCount complete:nil];
}

#pragma mark step5:EncryptedData
- (void)sentProvisionEncryptedDataWithMicPduWithTimeout:(NSTimeInterval)timeout callback:(prvisionResponseCallBack)block {
    TelinkLogInfo(@"\n\n==========remote provision:step5\n\n");
    self.provisionResponseBlock = block;
    [self cancelLastMessageHandle];

    NSData *provisioningData = self.provisioningData.getProvisioningData;
    NSData *encryptedProvisioningDataAndMic = [self.provisioningData getEncryptedProvisioningDataAndMicWithProvisioningData:provisioningData];
    SigProvisioningDataPdu *pdu = [[SigProvisioningDataPdu alloc] initWithEncryptedProvisioningData:[encryptedProvisioningDataAndMic subdataWithRange:NSMakeRange(0, 25)] provisioningDataMIC:[encryptedProvisioningDataAndMic subdataWithRange:NSMakeRange(25, 8)]];
    self.outboundPDUNumber = 5;
    [self sendRemoteProvisionPDUWithOutboundPDUNumber:self.outboundPDUNumber provisioningPDU:pdu.pduData retryCount:kRemoteProgressRetryCount complete:nil];
}

- (void)getCapabilitiesResultWithResponse:(SigProvisioningPdu *)response {
    if (response.provisionType == SigProvisioningPduType_capabilities && self.outboundPDUNumber == 0 && self.inboundPDUNumber == 0) {
        SigProvisioningCapabilitiesPdu *capabilitiesPdu = (SigProvisioningCapabilitiesPdu *)response;
        TelinkLogInfo(@"%@",capabilitiesPdu.getCapabilitiesString);
        self.provisioningCapabilities = capabilitiesPdu;
        self.provisioningData.provisioningCapabilitiesPDUValue = [capabilitiesPdu.pduData subdataWithRange:NSMakeRange(1, capabilitiesPdu.pduData.length-1)];
        self.state = ProvisioningState_capabilitiesReceived;
        if (self.capabilitiesResponseBlock) {
            self.unicastAddress = self.capabilitiesResponseBlock(capabilitiesPdu);
        }
        if (self.unicastAddress == 0) {
            [self provisionFail];
        }else{
            __weak typeof(self) weakSelf = self;
            if (self.staticOobData) {
                //当前设置为static oob provision
                if (self.provisioningCapabilities.staticOobType.staticOobInformationAvailable == 1) {
                    TelinkLogVerbose(@"static oob provision, staticOobData=%@",self.staticOobData);
                    [self sentStartStaticOobProvisionPduAndPublicKeyPduWithStaticOobData:self.staticOobData timeout:kRemoteProgressTimeout callback:^(SigProvisioningPdu * _Nullable response) {
                        [weakSelf sentProvisionPublicKeyPduWithResponse:response];
                    }];
                } else {
                    //设备不支持则直接provision fail
                    TelinkLogError(@"This device is not support static oob.");
                    [self provisionFail];
                }
            }else{
                //当前设置为no oob provision
                [self sentStartNoOobProvisionPduWithTimeout:kRemoteProgressTimeout callback:^(SigProvisioningPdu * _Nullable response) {
                    [weakSelf sentProvisionPublicKeyPduWithResponse:response];
                }];
            }
        }
    }else if (!response || response.provisionType == SigProvisioningPduType_failed) {
        SigProvisioningFailedPdu *failedPdu = (SigProvisioningFailedPdu *)response;
        TelinkLogDebug(@"getCapabilities error = %lu",(unsigned long)failedPdu.errorCode);
        [self provisionFail];
    }else{
        TelinkLogDebug(@"getCapabilities:no handle this response data");
    }
}

- (void)sentProvisionPublicKeyPduWithResponse:(SigProvisioningPdu *)response {
    if (response.provisionType == SigProvisioningPduType_publicKey && self.outboundPDUNumber == 2 && self.inboundPDUNumber == 2) {
        SigProvisioningPublicKeyPdu *publicKeyPdu = (SigProvisioningPublicKeyPdu *)response;
        TelinkLogInfo(@"device public key back:%@",[LibTools convertDataToHexStr:publicKeyPdu.publicKey]);
        self.provisioningData.devicePublicKey = publicKeyPdu.publicKey;
        [self.provisioningData provisionerDidObtainWithDevicePublicKey:publicKeyPdu.publicKey];
        if (self.provisioningData.sharedSecret && self.provisioningData.sharedSecret.length > 0) {
            __weak typeof(self) weakSelf = self;
            [self sentProvisionConfirmationPduWithTimeout:kRemoteProgressTimeout callback:^(SigProvisioningPdu * _Nullable response) {
                [weakSelf sentProvisionConfirmationPduWithResponse:response];
            }];
        }else{
            TelinkLogDebug(@"calculate SharedSecret fail.");
            [self provisionFail];
        }
    }else if (!response || response.provisionType == SigProvisioningPduType_failed) {
        [self provisionFail];
        SigProvisioningFailedPdu *failedPdu = (SigProvisioningFailedPdu *)response;
        TelinkLogDebug(@"sentStartProvisionPduAndPublicKeyPdu error = %lu",(unsigned long)failedPdu.errorCode);
    }else{
        TelinkLogDebug(@"sentStartProvisionPduAndPublicKeyPdu:no handle this response data");
    }
}

- (void)sentProvisionConfirmationPduWithResponse:(SigProvisioningPdu *)response {
    if (response.provisionType == SigProvisioningPduType_confirmation && self.outboundPDUNumber == 3 && self.inboundPDUNumber == 3) {
        SigProvisioningConfirmationPdu *confirmationPdu = (SigProvisioningConfirmationPdu *)response;
        TelinkLogInfo(@"device confirmation back:%@",[LibTools convertDataToHexStr:confirmationPdu.confirmation]);
        [self.provisioningData provisionerDidObtainWithDeviceConfirmation:confirmationPdu.confirmation];
        if ([[self.provisioningData provisionerConfirmation] isEqualToData:confirmationPdu.confirmation]) {
            TelinkLogDebug(@"Confirmation of device is equal to confirmation of provisioner!");
            self.state = ProvisioningState_fail;
            return;
        }
        __weak typeof(self) weakSelf = self;
        [self sentProvisionRandomPduWithTimeout:kRemoteProgressTimeout callback:^(SigProvisioningPdu * _Nullable response) {
            [weakSelf sentProvisionRandomPduWithResponse:response];
        }];
    }else if (!response || response.provisionType == SigProvisioningPduType_failed) {
        [self provisionFail];
        SigProvisioningFailedPdu *failedPdu = (SigProvisioningFailedPdu *)response;
        TelinkLogDebug(@"sentProvisionConfirmationPdu error = %lu",(unsigned long)failedPdu.errorCode);
    }else{
        TelinkLogDebug(@"sentProvisionConfirmationPdu:no handle this response data");
    }
}

- (void)sentProvisionRandomPduWithResponse:(SigProvisioningPdu *)response {
    if (response.provisionType == SigProvisioningPduType_random && self.outboundPDUNumber == 4 && self.inboundPDUNumber == 4) {
        SigProvisioningRandomPdu *randomPdu = (SigProvisioningRandomPdu *)response;
        TelinkLogInfo(@"device random back:%@",randomPdu.random);
        [self.provisioningData provisionerDidObtainWithDeviceRandom:randomPdu.random];
        if ([self.provisioningData.provisionerRandom isEqualToData:randomPdu.random]) {
            TelinkLogDebug(@"Random of device is equal to random of provisioner!");
            self.state = ProvisioningState_fail;
            return;
        }
        if (![self.provisioningData validateConfirmation]) {
            TelinkLogDebug(@"validate Confirmation fail");
            [self provisionFail];
            return;
        }
        __weak typeof(self) weakSelf = self;
        [self sentProvisionEncryptedDataWithMicPduWithTimeout:kSentProvisionEncryptedDataWithMicTimeout callback:^(SigProvisioningPdu * _Nullable response) {
            [weakSelf sentProvisionEncryptedDataWithMicPduWithResponse:response];
        }];
    }else if (!response || response.provisionType == SigProvisioningPduType_failed) {
        [self provisionFail];
        SigProvisioningFailedPdu *failedPdu = (SigProvisioningFailedPdu *)response;
        TelinkLogDebug(@"sentProvisionRandomPdu error = %lu",(unsigned long)failedPdu.errorCode);
    }else{
        TelinkLogDebug(@"sentProvisionRandomPdu:no handle this response data");
    }
}

- (void)sentProvisionEncryptedDataWithMicPduWithResponse:(SigProvisioningPdu *)response {
    TelinkLogInfo(@"\n\n==========remote provision end.\n\n");
    TelinkLogInfo(@"device provision result back:%@",response.pduData);
    if (response.provisionType == SigProvisioningPduType_complete && self.outboundPDUNumber == 5 && self.inboundPDUNumber == 5) {
        [self provisionSuccess];
        [SigBearer.share setBearerProvisioned:YES];
        if (self.provisionSuccessBlock) {
            self.provisionSuccessBlock(self.unprovisionedDevice.uuid,self.unicastAddress);
        }
        SigMeshLib.share.delegateForDeveloper = self.oldDelegateForDeveloper;
        self.provisionResponseBlock = nil;
    }else if (!response || response.provisionType == SigProvisioningPduType_failed) {
        [self provisionFail];
        SigProvisioningFailedPdu *failedPdu = (SigProvisioningFailedPdu *)response;
        TelinkLogDebug(@"sentProvisionEncryptedDataWithMicPdu error = %lu",(unsigned long)failedPdu.errorCode);
        self.provisionResponseBlock = nil;
    }else{
        TelinkLogDebug(@"sentProvisionEncryptedDataWithMicPdu:no handle this response data");
    }
}

#pragma mark - SigMessageDelegate

/// A callback called whenever a Mesh Message has been received from the mesh network.
/// @param message The received message.
/// @param source The Unicast Address of the Element from which the message was sent.
/// @param destination The address to which the message was sent.
- (void)didReceiveMessage:(SigMeshMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination {
    TelinkLogVerbose(@"didReceiveMessage=%@,message.parameters=%@,source=0x%x,destination=0x%x",message,message.parameters,source,destination);
    if ([message isKindOfClass:[SigRemoteProvisioningScanReport class]] && self.isRemoteScanning) {
        // Try parsing the response.
        SigRemoteScanRspModel *model = [[SigRemoteScanRspModel alloc] initWithParameters:message.parameters];
        if (!model) {
            TelinkLogInfo(@"parsing SigRemoteProvisioningScanReport fail.");
            return;
        }
        model.reportNodeAddress = source;
        if (![self.remoteScanRspModels containsObject:model]) {
            [self.remoteScanRspModels addObject:model];
        } else {
            NSInteger index = [self.remoteScanRspModels indexOfObject:model];
            SigRemoteScanRspModel *oldModel = [self.remoteScanRspModels objectAtIndex:index];
            if (oldModel.RSSI < model.RSSI) {
                [self.remoteScanRspModels replaceObjectAtIndex:index withObject:model];
            }
        }
        if (self.reportBlock) {
            self.reportBlock(model);
        }
    } else if ([message isKindOfClass:[SigRemoteProvisioningPDUReport class]]) {
        // Try parsing the response.
        NSData *data = [message.parameters subdataWithRange:NSMakeRange(1, message.parameters.length - 1)];
        if (data.length < 1) {
            return;
        }
        UInt8 type = 0;
        memcpy(&type, data.bytes, 1);
        Class MessageType = [SigProvisioningPdu getProvisioningPduClassWithProvisioningPduType:type];
        if (MessageType != nil) {
            SigProvisioningPdu *msg = [[MessageType alloc] initWithParameters:data];
            if (msg) {
                if (self.provisionResponseBlock) {
                    self.provisionResponseBlock(msg);
                }
            } else {
                TelinkLogDebug(@"the response is not valid.");
                return;
            }
        } else {
            TelinkLogDebug(@"parsing the response fail.");
            return;
        }
    } else if ([message isKindOfClass:[SigRemoteProvisioningPDUOutboundReport class]]) {
        UInt8 inboundPDUNumber = ((SigRemoteProvisioningPDUOutboundReport *)message).outboundPDUNumber;
        TelinkLogInfo(@"inboundPDUNumber=%d",inboundPDUNumber);
        self.inboundPDUNumber = inboundPDUNumber;
        if (self.outboundPDUNumber == 1 && self.inboundPDUNumber == 1) {
            //进行step2.5
            [self cancelLastMessageHandle];
            [self sentProvisionPublickeyPduWithTimeout:kRemoteProgressTimeout callback:self.provisionResponseBlock];
        }
    } else if ([message isKindOfClass:[SigRemoteProvisioningLinkReport class]]) {
        if (self.isLinkOpenReportWaiting) {
            self.isLinkOpenReportWaiting = NO;
            SigRemoteProvisioningLinkReport *linkReport = (SigRemoteProvisioningLinkReport *)message;
            if (linkReport.status == SigRemoteProvisioningStatus_success) {
                // means link open success
                __weak typeof(self) weakSelf = self;
                [self getCapabilitiesWithTimeout:kRemoteProgressTimeout callback:^(SigProvisioningPdu * _Nullable response) {
                    [weakSelf getCapabilitiesResultWithResponse:response];
                }];
            } else {
                // means link open fail
                [self provisionFail];
            }
        }
    }
}

/// A callback called when an unsegmented message was sent to the SigBearer, or when all
/// segments of a segmented message targeting a Unicast Address were acknowledged by
/// the target Node.
/// @param message The message that has been sent.
/// @param localElement The local Element used as a source of this message.
/// @param destination The address to which the message was sent.
- (void)didSendMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toDestination:(UInt16)destination {
//    TelinkLogVerbose(@"didSendMessage=%@,class=%@,source=0x%x,destination=0x%x",message,message.class,localElement.unicastAddress,destination);
}

/// A callback called when a message failed to be sent to the target Node, or the response for
/// an acknowledged message hasn't been received before the time run out.
/// For unsegmented unacknowledged messages this callback will be invoked when the
/// SigBearer was closed.
/// For segmented unacknowledged messages targeting a Unicast Address, besides that,
/// it may also be called when sending timed out before all of the segments were acknowledged
/// by the target Node, or when the target Node is busy and not able to proceed the message
/// at the moment.
/// For acknowledged messages the callback will be called when the response has not been
/// received before the time set by `incompleteMessageTimeout` run out. The message
/// might have been retransmitted multiple times and might have been received by the target Node.
/// For acknowledged messages sent to a Group or Virtual Address this will be called when the
/// response has not been received from any Node.
/// @param message The message that has failed to be delivered.
/// @param localElement The local Element used as a source of this message.
/// @param destination The address to which the message was sent.
/// @param error The error that occurred.
- (void)failedToSendMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toDestination:(UInt16)destination error:(NSError *)error {
//    TelinkLogVerbose(@"failedToSendMessage=%@,class=%@,source=0x%x,destination=0x%x",message,message.class,localElement.unicastAddress,destination);
}

/// A callback called whenever a SigProxyConfiguration Message has been received from the mesh network.
/// @param message The received message.
/// @param source The Unicast Address of the Element from which the message was sent.
/// @param destination The address to which the message was sent.
- (void)didReceiveSigProxyConfigurationMessage:(SigProxyConfigurationMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination {
//    TelinkLogVerbose(@"didReceiveSigProxyConfigurationMessage=%@,source=0x%x,destination=0x%x",message,source,destination);
}

/// 当SDK不支持EPA功能时，默认都使用SigFipsP256EllipticCurve_CMAC_AES128。
/// 如果static OOB长度为16，不能用SigFipsP256EllipticCurve_HMAC_SHA256而要用SigFipsP256EllipticCurve_CMAC_AES128。
/// 如果static OOB大于16，但算法为SigFipsP256EllipticCurve_CMAC_AES128，需要截取static OOB的前16字节来使用。
- (Algorithm)getCurrentProvisionAlgorithm {
    Algorithm algorithm = Algorithm_fipsP256EllipticCurve;
    if (SigDataSource.share.fipsP256EllipticCurve == SigFipsP256EllipticCurve_CMAC_AES128) {
        algorithm = Algorithm_fipsP256EllipticCurve;
    } else if (SigDataSource.share.fipsP256EllipticCurve == SigFipsP256EllipticCurve_HMAC_SHA256) {
        if ((self.staticOobData && self.staticOobData.length == 32) || self.staticOobData == nil) {
            algorithm = Algorithm_fipsP256EllipticCurve_HMAC_SHA256;
        } else {
            algorithm = Algorithm_fipsP256EllipticCurve;
        }
    } else if (SigDataSource.share.fipsP256EllipticCurve == SigFipsP256EllipticCurve_auto) {
        if (self.provisioningCapabilities.algorithms.fipsP256EllipticCurve_HMAC_SHA256 == 1) {
            if ((self.staticOobData && self.staticOobData.length == 32) || self.staticOobData == nil) {
                algorithm = Algorithm_fipsP256EllipticCurve_HMAC_SHA256;
            } else {
                algorithm = Algorithm_fipsP256EllipticCurve;
            }
        } else if (self.provisioningCapabilities.algorithms.fipsP256EllipticCurve == 1) {
            algorithm = Algorithm_fipsP256EllipticCurve;
        }
    }
    if (self.staticOobData && self.staticOobData.length > 16 && algorithm == Algorithm_fipsP256EllipticCurve) {
        self.staticOobData = [self.staticOobData subdataWithRange:NSMakeRange(0, 16)];
        TelinkLogInfo(@"Change staticOobData to 0x%@", [LibTools convertDataToHexStr:self.staticOobData]);
    }
    TelinkLogInfo(@"algorithm=%@", algorithm == Algorithm_fipsP256EllipticCurve ? @"CMAC_AES128" : @"HMAC_SHA256");
    return algorithm;
}

@end
