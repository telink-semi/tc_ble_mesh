/********************************************************************************************************
* @file     SigRemoteAddManager.m
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
//  SigRemoteAddManager.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2020/3/26.
//  Copyright © 2020 Telink. All rights reserved.
//

#import "SigRemoteAddManager.h"
#import "SigProvisioningData.h"

typedef void(^RemotePDUResultCallBack)(BOOL isSuccess);

@interface SigRemoteAddManager ()<SigMessageDelegate>
@property (nonatomic,assign) BOOL isRemoteScaning;
@property (nonatomic,assign) UInt8 currentMaxScannedItems;
@property (nonatomic,weak) id oldDelegateForDeveloper;
@property (nonatomic,strong) NSMutableArray <SigRemoteScanRspModel *>*remoteScanRspModels;
@property (nonatomic,copy) remoteProvisioningScanReportCallBack reportBlock;
@property (nonatomic,copy) resultBlock scanResultBlock;

@property (nonatomic,assign) UInt16 unicastAddress;
@property (nonatomic,strong) NSData *staticOobData;
@property (nonatomic,strong) SigScanRspModel *unprovisionedDevice;
@property (nonatomic,copy) addDevice_prvisionSuccessCallBack provisionSuccessBlock;
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

@end

@implementation SigRemoteAddManager

+ (SigRemoteAddManager *)share {
    static SigRemoteAddManager *shareManager = nil;
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        shareManager = [[SigRemoteAddManager alloc] init];
    });
    return shareManager;
}

#pragma mark - remote scan

- (void)startRemoteProvisionScanWithReportCallback:(remoteProvisioningScanReportCallBack)reportCallback resultCallback:(resultBlock)resultCallback {
    if (self.isRemoteScaning) {
        NSString *errstr = @"SigRemoteAddManager is remoteScaning, please call `stopRemoteProvisionScan`";
        TeLogError(@"%@",errstr);
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
    self.isRemoteScaning = YES;
    [self startSingleRemoteProvisionScan];
}

- (void)stopRemoteProvisionScan {
    self.isRemoteScaning = NO;
}

- (void)endRemoteProvisionScan {
    self.isRemoteScaning = NO;
//    if (self.messageHandle) {
//        [self.messageHandle cancel];
//    }
    //因为上面for循环发送了多次，在这里清理block。
    [SigMeshLib.share cleanAllCommandsAndRetry];
    if (self.currentMaxScannedItems >= 0x04 && self.currentMaxScannedItems <= 0xFF) {
        if (self.scanResultBlock) {
            self.scanResultBlock(YES, nil);
        }
    } else {
        NSString *errstr = @"Reomte scan fail: current connected node is no support remote provision scan.";
        TeLogError(@"%@",errstr);
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
/// @param provisionAddress address of new device.
/// @param reportNodeAddress address of node that report this uuid
/// @param reportNodeUUID identify node that need to provision.
/// @param networkKey networkKey
/// @param netkeyIndex netkeyIndex
/// @param provisionType ProvisionTpye_NoOOB means oob data is 16 bytes zero data, ProvisionTpye_StaticOOB means oob data is get from HTTP API.
/// @param staticOOBData oob data get from HTTP API when provisionType is ProvisionTpye_StaticOOB.
/// @param provisionSuccess callback when provision success.
/// @param fail callback when provision fail.
- (void)remoteProvisionWithNextProvisionAddress:(UInt16)provisionAddress reportNodeAddress:(UInt16)reportNodeAddress reportNodeUUID:(NSData *)reportNodeUUID networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex provisionType:(ProvisionTpye)provisionType staticOOBData:(nullable NSData *)staticOOBData provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail {
    self.reportNodeAddress = reportNodeAddress;
    self.reportNodeUUID = reportNodeUUID;
    self.oldDelegateForDeveloper = SigMeshLib.share.delegateForDeveloper;
    SigMeshLib.share.delegateForDeveloper = self;
    self.staticOobData = staticOOBData;
    self.unicastAddress = provisionAddress;
    self.provisionSuccessBlock = provisionSuccess;
    self.failBlock = fail;
    self.unprovisionedDevice = [SigDataSource.share getScanRspModelWithUUID:[SigBearer.share getCurrentPeripheral].identifier.UUIDString];
    SigNetkeyModel *provisionNet = nil;
    NSArray *netKeys = [NSArray arrayWithArray:SigDataSource.share.netKeys];
    for (SigNetkeyModel *net in netKeys) {
        if ([networkKey isEqualToData:[LibTools nsstringToHex:net.key]] && netkeyIndex == net.index) {
            provisionNet = net;
            break;
        }
    }
    if (provisionNet == nil) {
        TeLogError(@"error network key.");
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
    TeLogInfo(@"start provision.");
    [SigBluetooth.share setBluetoothDisconnectCallback:^(CBPeripheral * _Nonnull peripheral, NSError * _Nonnull error) {
        [SigMeshLib.share cleanAllCommandsAndRetry];
        if ([peripheral.identifier.UUIDString isEqualToString:SigBearer.share.getCurrentPeripheral.identifier.UUIDString]) {
            if (weakSelf.isProvisionning) {
                TeLog(@"disconnect in provisioning，provision fail.");
                if (fail) {
                    weakSelf.isProvisionning = NO;
                    NSError *err = [NSError errorWithDomain:@"disconnect in provisioning，provision fail." code:-1 userInfo:nil];
                    fail(err);
                }
            }
        }
    }];
    
    self.messageHandle = [SDKLibCommand remoteProvisioningLinkOpenWithDestination:reportNodeAddress UUID:reportNodeUUID retryCount:kRemoteProgressRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigRemoteProvisioningLinkStatus * _Nonnull responseMessage) {
        TeLogInfo(@"linkOpen responseMessage=%@,parameters=%@,source=0x%x,destination=0x%x",responseMessage,responseMessage.parameters,source,destination);
//        if (responseMessage.status == SigRemoteProvisioningStatus_success) {
//            [weakSelf getCapabilitiesWithTimeout:kRemoteProgressTimeout callback:^(SigProvisioningResponse * _Nullable response) {
//                [weakSelf getCapabilitiesResultWithResponse:response];
//            }];
//        } else {
//            [self provisionFail];
//            TeLogDebug(@"sentProvisionConfirmationPdu error, parameters= %@",responseMessage.parameters);
//        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        if (error) {
            [self provisionFail];
            TeLogDebug(@"sentProvisionConfirmationPdu error = %@",error.domain);
        }
    }];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(remoteProvisionTimeout) object:nil];
        [self performSelector:@selector(remoteProvisionTimeout) withObject:nil afterDelay:kRemoteProgressTimeout];
    });
}

- (void)remoteProvisionTimeout {
    [self provisionFail];
}

// 1.remoteProvisioningScanCapabilitiesGet
- (void)getRemoteProvisioningScanCapabilities {
    TeLogVerbose(@"getRemoteProvisioningScanCapabilities address=0x%x",SigDataSource.share.getCurrentConnectedNode.address);
    __weak typeof(self) weakSelf = self;
    self.messageHandle = [SDKLibCommand remoteProvisioningScanCapabilitiesGetWithDestination:SigDataSource.share.getCurrentConnectedNode.address retryCount:kRemoteProgressRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigRemoteProvisioningScanCapabilitiesStatus * _Nonnull responseMessage) {
//        if (responseMessage.activeScan) {
            weakSelf.currentMaxScannedItems = responseMessage.maxScannedItems;
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(endSingleRemoteProvisionScan) object:nil];
            [weakSelf performSelector:@selector(startRemoteProvisioningScan) withObject:nil afterDelay:0.5];
        });
//        } else {
//            TeLogInfo(@"nodeAddress 0x%x no support remote provision scan, try next address.");
//            [weakSelf endSingleRemoteProvisionScan];
//        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        if (error) {
            TeLogInfo(@"nodeAddress 0x%x get remote provision scan Capabilities timeout.",SigDataSource.share.unicastAddressOfConnected);
            [weakSelf endSingleRemoteProvisionScan];
        }
    }];
}

// 2.remoteProvisioningScanStart
- (void)startRemoteProvisioningScan {
//    self.messageHandle = [SDKLibCommand remoteProvisioningScanStartWithDestination:kMeshAddress_allNodes scannedItemsLimit:kScannedItemsLimit timeout:kScannedItemsTimeout UUID:nil retryCount:0 responseMaxCount:0 successCallback:^(UInt16 source, UInt16 destination, SigRemoteProvisioningScanStatus * _Nonnull responseMessage) {
//        TeLogInfo(@"source=0x%x,destination=0x%x,opCode=0x%x,parameters=%@",source,destination,responseMessage.opCode,[LibTools convertDataToHexStr:responseMessage.parameters]);
//    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
//        TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
//    }];
    
    //since v3.2.2, remote provision with deviceKey.
    __weak typeof(self) weakSelf = self;
    NSOperationQueue *oprationQueue = [[NSOperationQueue alloc] init];
    [oprationQueue addOperationWithBlock:^{
        //这个block语句块在子线程中执行
        for (SigNodeModel *node in SigDataSource.share.curNodes) {
            if (!weakSelf.isRemoteScaning) {
                return;
            }
            weakSelf.semaphore = dispatch_semaphore_create(0);
            weakSelf.messageHandle = [SDKLibCommand remoteProvisioningScanStartWithDestination:node.address scannedItemsLimit:kScannedItemsLimit timeout:kScannedItemsTimeout UUID:nil retryCount:0 responseMaxCount:0 successCallback:^(UInt16 source, UInt16 destination, SigRemoteProvisioningScanStatus * _Nonnull responseMessage) {
                TeLogInfo(@"source=0x%x,destination=0x%x,opCode=0x%x,parameters=%@",source,destination,responseMessage.opCode,[LibTools convertDataToHexStr:responseMessage.parameters]);
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                [NSThread sleepForTimeInterval:0.1];
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

- (void)setState:(ProvisionigState)state{
    _state = state;
    if (state == ProvisionigState_fail) {
        [self reset];
        SigMeshLib.share.delegateForDeveloper = self.oldDelegateForDeveloper; 
        if (self.failBlock) {
            NSError *err = [NSError errorWithDomain:@"provision fail." code:-1 userInfo:nil];
            self.failBlock(err);
        }
    }
}

- (BOOL)isDeviceSupported{
    if (self.provisioningCapabilities.pduType != SigProvisioningPduType_capabilities || self.provisioningCapabilities.numberOfElements == 0) {
        TeLogError(@"Capabilities is error.");
        return NO;
    }
    return self.provisioningCapabilities.algorithms.fipsP256EllipticCurve == 1;
}

/// This method sends the provisioning request to the device over the Bearer specified in the init. Additionally, it adds the request payload to given inputs. Inputs are required in device authorization.
///
/// - parameter request: The request to be sent.
/// - parameter inputs:  The Provisioning Inputs.
- (void)sendPdu:(SigProvisioningPdu *)pdu andAccumulateToData:(SigProvisioningData *)data outboundPDUNumber:(UInt8)outboundPDUNumber complete:(RemotePDUResultCallBack)complete {
    NSData *pduData = pdu.pduData;
    // The first byte is the type. We only accumulate payload.
    [data accumulatePduData:[pduData subdataWithRange:NSMakeRange(1, pduData.length - 1)]];
    [self sendRemoteProvisionPDUWithOutboundPDUNumber:outboundPDUNumber provisioningPDU:pduData retryCount:kRemoteProgressRetryCount complete:complete];
}

- (void)sendRemoteProvisionPDUWithOutboundPDUNumber:(UInt8)outboundPDUNumber provisioningPDU:(NSData *)provisioningPDU retryCount:(NSInteger)retryCount complete:(RemotePDUResultCallBack)complete {
    self.messageHandle = [SDKLibCommand remoteProvisioningPDUSendWithDestination:self.reportNodeAddress OutboundPDUNumber:outboundPDUNumber provisioningPDU:provisioningPDU retryCount:retryCount responseMaxCount:0 resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        if (error) {
            TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        }
    }];
}

- (void)provisionSuccess{
    [self cancelLastMessageHandle];
    
    UInt16 address = self.provisioningData.unicastAddress;
    NSString *identify = nil;
    identify = [LibTools convertDataToHexStr:_reportNodeUUID];
    UInt8 ele_count = self.provisioningCapabilities.numberOfElements;
    [SigDataSource.share saveLocationProvisionAddress:address+ele_count-1];
    NSData *devKeyData = self.provisioningData.deviceKey;
    TeLogInfo(@"deviceKey=%@",devKeyData);
    
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

    [SigDataSource.share addAndSaveNodeToMeshNetworkWithDeviceModel:model];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self];
    });
    [SigBluetooth.share setBluetoothDisconnectCallback:nil];
}

- (void)provisionFail {
    [self.messageHandle cancel];
    self.state = ProvisionigState_fail;
    self.provisionResponseBlock = nil;
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self];
    });
}

/// This method should be called when the OOB value has been received and Auth Value has been calculated. It computes and sends the Provisioner Confirmation to the device.
///
/// - parameter value: The 16 byte long Auth Value.
- (void)authValueReceivedData:(NSData *)data {
    SigAuthenticationModel *auth = nil;
    self.authenticationModel = auth;
    [self.provisioningData provisionerDidObtainAuthValue:data];
    SigProvisioningPdu *pdu = [[SigProvisioningPdu alloc] initProvisioningConfirmationPduWithConfirmation:self.provisioningData.provisionerConfirmation];
    self.outboundPDUNumber = 3;
    [self sendRemoteProvisionPDUWithOutboundPDUNumber:self.outboundPDUNumber provisioningPDU:pdu.pduData retryCount:kRemoteProgressRetryCount complete:nil];
}

/// Resets the provisioning properties and state.
- (void)reset {
    self.authenticationMethod = 0;
    memset(&_provisioningCapabilities, 0, sizeof(_provisioningCapabilities));
    SigProvisioningData *tem = nil;
    self.provisioningData = tem;
    self.state = ProvisionigState_ready;
    [SigBearer.share setBearerProvisioned:YES];
}

- (void)cancelLastMessageHandle {
    if (self.messageHandle) {
        [self.messageHandle cancel];
    }
}

#pragma mark step1:getCapabilities
- (void)getCapabilitiesWithTimeout:(NSTimeInterval)timeout callback:(prvisionResponseCallBack)block {
    TeLogInfo(@"\n\n==========provision:step1\n\n");
    self.provisionResponseBlock = block;
    [self cancelLastMessageHandle];
    [self identifyWithAttentionTimer:0];
}

/**
 This method get the Capabilities of the device.

 @param attentionTimer This value determines for how long (in seconds) the device shall remain attracting human's attention by blinking, flashing, buzzing, etc. The value 0 disables Attention Timer.
 */
- (void)identifyWithAttentionTimer:(UInt8)attentionTimer {
    
    // Has the provisioning been restarted?
    if (self.state == ProvisionigState_fail) {
        [self reset];
    }
    
    // Is the Provisioner Manager in the right state?
    if (self.state != ProvisionigState_ready) {
        TeLogError(@"current node isn't in ready.");
        return;
    }

    // Initialize provisioning data.
    self.provisioningData = [[SigProvisioningData alloc] init];

    self.state = ProvisionigState_requestingCapabilities;
    
    SigProvisioningPdu *pdu = [[SigProvisioningPdu alloc] initProvisioningInvitePduWithAttentionTimer:attentionTimer];
    self.outboundPDUNumber = 0;
    [self sendPdu:pdu andAccumulateToData:self.provisioningData outboundPDUNumber:self.outboundPDUNumber complete:nil];
}

#pragma mark step2:start
- (void)sentStartNoOobProvisionPduWithTimeout:(NSTimeInterval)timeout callback:(prvisionResponseCallBack)block {
    TeLogInfo(@"\n\n==========provision:step2(noOob)\n\n");
    // Is the Provisioner Manager in the right state?
    if (self.state != ProvisionigState_capabilitiesReceived) {
        TeLogError(@"current state is wrong.");
        return;
    }
    
    // Ensure the Network Key is set.
    if (self.networkKey == nil) {
        TeLogError(@"current networkKey isn't specified.");
        return;
    }
    
    // Is the Bearer open?
    if (!SigBearer.share.isOpen) {
        TeLogError(@"current node's bearer isn't open.");
        return;
    }
    
    self.provisionResponseBlock = block;
    [self cancelLastMessageHandle];

    [self.provisioningData generateProvisionerRandomAndProvisionerPublicKey];
    
    // Send Provisioning Start request.
    self.state = ProvisionigState_provisioning;
    [self.provisioningData prepareWithNetwork:SigDataSource.share networkKey:self.networkKey unicastAddress:self.unicastAddress];
    PublicKey *publicKey = [[PublicKey alloc] initWithPublicKeyType:PublicKeyType_noOobPublicKey];
    AuthenticationMethod authenticationMethod = AuthenticationMethod_noOob;
    self.authenticationMethod = AuthenticationMethod_noOob;

    SigProvisioningPdu *startPdu = [[SigProvisioningPdu alloc] initProvisioningstartPduWithAlgorithm:Algorithm_fipsP256EllipticCurve publicKeyType:publicKey.publicKeyType authenticationMethod:authenticationMethod authenticationAction:0 authenticationSize:0];
    self.outboundPDUNumber = 1;
    [self sendPdu:startPdu andAccumulateToData:self.provisioningData outboundPDUNumber:self.outboundPDUNumber complete:nil];

}

- (void)sentStartStaticOobProvisionPduAndPublicKeyPduWithStaticOobData:(NSData *)oobData timeout:(NSTimeInterval)timeout callback:(prvisionResponseCallBack)block {
    TeLogInfo(@"\n\n==========provision:step2(staticOob)\n\n");
    // Is the Provisioner Manager in the right state?
    if (self.state != ProvisionigState_capabilitiesReceived) {
        TeLogError(@"current state is wrong.");
        return;
    }

    // Ensure the Network Key is set.
    if (self.networkKey == nil) {
        TeLogError(@"current networkKey isn't specified.");
        return;
    }

    // Is the Bearer open?
    if (!SigBearer.share.isOpen) {
        TeLogError(@"current node's bearer isn't open.");
        return;
    }

    self.provisionResponseBlock = block;
    [self cancelLastMessageHandle];
    
    [self.provisioningData generateProvisionerRandomAndProvisionerPublicKey];
    [self.provisioningData provisionerDidObtainAuthValue:oobData];

    // Send Provisioning Start request.
    self.state = ProvisionigState_provisioning;
    [self.provisioningData prepareWithNetwork:SigDataSource.share networkKey:self.networkKey unicastAddress:self.unicastAddress];
    PublicKey *publicKey = [[PublicKey alloc] initWithPublicKeyType:PublicKeyType_noOobPublicKey];
    AuthenticationMethod authenticationMethod = AuthenticationMethod_staticOob;
    self.authenticationMethod = AuthenticationMethod_staticOob;

    SigProvisioningPdu *startPdu = [[SigProvisioningPdu alloc] initProvisioningstartPduWithAlgorithm:Algorithm_fipsP256EllipticCurve publicKeyType:publicKey.publicKeyType authenticationMethod:authenticationMethod authenticationAction:0 authenticationSize:0];
    self.outboundPDUNumber = 1;
    [self sendPdu:startPdu andAccumulateToData:self.provisioningData outboundPDUNumber:self.outboundPDUNumber complete:nil];
}

#pragma mark step2.5:Publickey
- (void)sentProvisionPublickeyPduWithTimeout:(NSTimeInterval)timeout callback:(prvisionResponseCallBack)block {
    TeLogInfo(@"\n\n==========provision:step2.5(noOob)\n\n");

    self.provisionResponseBlock = block;
    [self cancelLastMessageHandle];
    // Send the Public Key of the Provisioner.
    SigProvisioningPdu *publicPdu = [[SigProvisioningPdu alloc] initProvisioningPublicKeyPduWithPublicKey:self.provisioningData.provisionerPublicKey];
    self.outboundPDUNumber = 2;
    [self sendPdu:publicPdu andAccumulateToData:self.provisioningData outboundPDUNumber:self.outboundPDUNumber complete:nil];
}

#pragma mark step3:Confirmation
- (void)sentProvisionConfirmationPduWithTimeout:(NSTimeInterval)timeout callback:(prvisionResponseCallBack)block {
    TeLogInfo(@"\n\n==========provision:step3\n\n");
    self.provisionResponseBlock = block;
    NSData *authValue = nil;
    if (self.staticOobData) {
        //当前设置为static oob provision
        authValue = self.staticOobData;
    } else {
        //当前设置为no oob provision
        UInt8 value[16] = {};
        memset(&value, 0, 16);
        authValue = [NSData dataWithBytes:&value length:16];
    }
    [self cancelLastMessageHandle];

    [self authValueReceivedData:authValue];
}

#pragma mark step4:Random
- (void)sentProvisionRandomPduWithTimeout:(NSTimeInterval)timeout callback:(prvisionResponseCallBack)block {
    TeLogInfo(@"\n\n==========provision:step4\n\n");
    self.provisionResponseBlock = block;
    [self cancelLastMessageHandle];

    SigProvisioningPdu *pdu = [[SigProvisioningPdu alloc] initProvisioningRandomPduWithRandom:self.provisioningData.provisionerRandom];
    self.outboundPDUNumber = 4;
    [self sendRemoteProvisionPDUWithOutboundPDUNumber:self.outboundPDUNumber provisioningPDU:pdu.pduData retryCount:kRemoteProgressRetryCount complete:nil];
}

#pragma mark step5:EncryptedData
- (void)sentProvisionEncryptedDataWithMicPduWithTimeout:(NSTimeInterval)timeout callback:(prvisionResponseCallBack)block {
    TeLogInfo(@"\n\n==========provision:step5\n\n");
    self.provisionResponseBlock = block;
    [self cancelLastMessageHandle];

    SigProvisioningPdu *pdu = [[SigProvisioningPdu alloc] initProvisioningEncryptedDataWithMicPduWithEncryptedData:self.provisioningData.encryptedProvisioningDataWithMic];
    self.outboundPDUNumber = 5;
    [self sendRemoteProvisionPDUWithOutboundPDUNumber:self.outboundPDUNumber provisioningPDU:pdu.pduData retryCount:kRemoteProgressRetryCount complete:nil];
}

- (void)getCapabilitiesResultWithResponse:(SigProvisioningResponse *)response {
    if (response.type == SigProvisioningPduType_capabilities && self.outboundPDUNumber == 0 && self.inboundPDUNumber == 0) {
        [self showCapabilitiesLog:response.capabilities];
        struct ProvisioningCapabilities capabilities = response.capabilities;
        self.provisioningCapabilities = capabilities;
        [self.provisioningData accumulatePduData:[response.responseData subdataWithRange:NSMakeRange(1, response.responseData.length - 1)]];
        self.state = ProvisionigState_capabilitiesReceived;
        if (self.unicastAddress == 0) {
            [self provisionFail];
        }else{
            __weak typeof(self) weakSelf = self;
            if (self.staticOobData) {
                //当前设置为static oob provision
                if (self.provisioningCapabilities.staticOobType.staticOobInformationAvailable == 1) {
                    TeLogVerbose(@"static oob provision, staticOobData=%@",self.staticOobData);
                    [self sentStartStaticOobProvisionPduAndPublicKeyPduWithStaticOobData:self.staticOobData timeout:kRemoteProgressTimeout callback:^(SigProvisioningResponse * _Nullable response) {
                        [weakSelf sentProvisionPublicKeyPduWithResponse:response];
                    }];
                } else {
                    //设备不支持则直接provision fail
                    TeLogError(@"This device is not support static oob.");
                    [self provisionFail];
                }
            }else{
                //当前设置为no oob provision
                [self sentStartNoOobProvisionPduWithTimeout:kRemoteProgressTimeout callback:^(SigProvisioningResponse * _Nullable response) {
                    [weakSelf sentProvisionPublicKeyPduWithResponse:response];
                }];
            }
        }
    }else if (!response || response.type == SigProvisioningPduType_failed) {
        TeLogDebug(@"getCapabilities error = %lu",(unsigned long)response.error);
        [self provisionFail];
    }else{
        TeLogDebug(@"getCapabilities:no handel this response data");
    }
}

- (void)sentProvisionPublicKeyPduWithResponse:(SigProvisioningResponse *)response {
    TeLogInfo(@"device public key back:%@",response.responseData);
    if (response.type == SigProvisioningPduType_publicKey && self.outboundPDUNumber == 2 && self.inboundPDUNumber == 2) {
        [self.provisioningData accumulatePduData:[response.responseData subdataWithRange:NSMakeRange(1, response.responseData.length - 1)]];
        [self.provisioningData provisionerDidObtainWithDevicePublicKey:response.publicKey];
        if (self.provisioningData.sharedSecret && self.provisioningData.sharedSecret.length > 0) {
            __weak typeof(self) weakSelf = self;
            [self sentProvisionConfirmationPduWithTimeout:kRemoteProgressTimeout callback:^(SigProvisioningResponse * _Nullable response) {
                [weakSelf sentProvisionConfirmationPduWithResponse:response];
            }];
        }else{
            TeLogDebug(@"calculate SharedSecret fail.");
            [self provisionFail];
        }
    }else if (!response || response.type == SigProvisioningPduType_failed) {
        [self provisionFail];
        TeLogDebug(@"sentStartProvisionPduAndPublicKeyPdu error = %lu",(unsigned long)response.error);
    }else{
        TeLogDebug(@"sentStartProvisionPduAndPublicKeyPdu:no handel this response data");
    }
}

- (void)sentProvisionConfirmationPduWithResponse:(SigProvisioningResponse *)response {
    TeLogInfo(@"device confirmation back:%@",response.responseData);
    if (response.type == SigProvisioningPduType_confirmation && self.outboundPDUNumber == 3 && self.inboundPDUNumber == 3) {
        [self.provisioningData provisionerDidObtainWithDeviceConfirmation:response.confirmation];
        __weak typeof(self) weakSelf = self;
        [self sentProvisionRandomPduWithTimeout:kRemoteProgressTimeout callback:^(SigProvisioningResponse * _Nullable response) {
            [weakSelf sentProvisionRandomPduWithResponse:response];
        }];
    }else if (!response || response.type == SigProvisioningPduType_failed) {
        [self provisionFail];
        TeLogDebug(@"sentProvisionConfirmationPdu error = %lu",(unsigned long)response.error);
    }else{
        TeLogDebug(@"sentProvisionConfirmationPdu:no handel this response data");
    }
}

- (void)sentProvisionRandomPduWithResponse:(SigProvisioningResponse *)response {
    TeLogInfo(@"device random back:%@",response.responseData);
    if (response.type == SigProvisioningPduType_random && self.outboundPDUNumber == 4 && self.inboundPDUNumber == 4) {
        [self.provisioningData provisionerDidObtainWithDeviceRandom:response.random];
        if (![self.provisioningData validateConfirmation]) {
            TeLogDebug(@"validate Confirmation fail");
            [self provisionFail];
            return;
        }
        __weak typeof(self) weakSelf = self;
        [self sentProvisionEncryptedDataWithMicPduWithTimeout:kSentProvisionEncryptedDataWithMicTimeout callback:^(SigProvisioningResponse * _Nullable response) {
            [weakSelf sentProvisionEncryptedDataWithMicPduWithResponse:response];
        }];
    }else if (!response || response.type == SigProvisioningPduType_failed) {
        [self provisionFail];
        TeLogDebug(@"sentProvisionRandomPdu error = %lu",(unsigned long)response.error);
    }else{
        TeLogDebug(@"sentProvisionRandomPdu:no handel this response data");
    }
}

- (void)sentProvisionEncryptedDataWithMicPduWithResponse:(SigProvisioningResponse *)response {
    TeLogInfo(@"\n\n==========provision end.\n\n");
    TeLogInfo(@"device provision result back:%@",response.responseData);
    if (response.type == SigProvisioningPduType_complete && self.outboundPDUNumber == 5 && self.inboundPDUNumber == 5) {
        [self provisionSuccess];
        [SigBearer.share setBearerProvisioned:YES];
        if (self.provisionSuccessBlock) {
            self.provisionSuccessBlock(self.unprovisionedDevice.uuid,self.unicastAddress);
        }
        SigMeshLib.share.delegateForDeveloper = self.oldDelegateForDeveloper;
    }else if (!response || response.type == SigProvisioningPduType_failed) {
        [self provisionFail];
        TeLogDebug(@"sentProvisionRandomPdu error = %lu",(unsigned long)response.error);
    }else{
        TeLogDebug(@"sentProvisionRandomPdu:no handel this response data");
    }
    self.provisionResponseBlock = nil;
}

- (void)showCapabilitiesLog:(struct ProvisioningCapabilities)capabilities {
    TeLogInfo(@"\n------ Capabilities ------\nNumber of elements: %d\nAlgorithms: %@\nPublic Key Type: %@\nStatic OOB Type: %@\nOutput OOB Size: %d\nOutput OOB Actions: %d\nInput OOB Size: %d\nInput OOB Actions: %d\n--------------------------",capabilities.numberOfElements,capabilities.algorithms.fipsP256EllipticCurve == 1 ?@"FIPS P-256 Elliptic Curve":@"None",capabilities.publicKeyType == PublicKeyType_noOobPublicKey ?@"No OOB Public Key":@"OOB Public Key",capabilities.staticOobType.staticOobInformationAvailable == 1 ?@"YES":@"None",capabilities.outputOobSize,capabilities.outputOobActions.value,capabilities.inputOobSize,capabilities.inputOobActions.value);
}

#pragma mark - SigMessageDelegate

- (void)didReceiveMessage:(SigMeshMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination {
    TeLogVerbose(@"didReceiveMessage=%@,message.parameters=%@,source=0x%x,destination=0x%x",message,message.parameters,source,destination);
    if ([message isKindOfClass:[SigRemoteProvisioningScanReport class]] && self.isRemoteScaning) {
        // Try parsing the response.
        SigRemoteScanRspModel *model = [[SigRemoteScanRspModel alloc] initWithParameters:message.parameters];
        if (!model) {
            TeLogInfo(@"parsing SigRemoteProvisioningScanReport fail.");
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
        SigProvisioningResponse *response = [[SigProvisioningResponse alloc] initWithData:[message.parameters subdataWithRange:NSMakeRange(1, message.parameters.length - 1)]];
        if (!response) {
            TeLogVerbose(@"parsing the response fail.");
            return;
        }
        if (!response.isValid) {
            TeLogVerbose(@"the response is not valid.");
            return;
        }
        if (self.provisionResponseBlock) {
            self.provisionResponseBlock(response);
        }
    } else if ([message isKindOfClass:[SigRemoteProvisioningPDUOutboundReport class]]) {
        UInt8 inboundPDUNumber = ((SigRemoteProvisioningPDUOutboundReport *)message).outboundPDUNumber;
        TeLogInfo(@"inboundPDUNumber=%d",inboundPDUNumber);
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
                [self getCapabilitiesWithTimeout:kRemoteProgressTimeout callback:^(SigProvisioningResponse * _Nullable response) {
                    [weakSelf getCapabilitiesResultWithResponse:response];
                }];
            } else {
                // means link open fail
                [self provisionFail];
            }
        }
    }
}

- (void)didSendMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toDestination:(UInt16)destination {
//    TeLogVerbose(@"didSendMessage=%@,class=%@,source=0x%x,destination=0x%x",message,message.class,localElement.unicastAddress,destination);
}

- (void)failedToSendMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toDestination:(UInt16)destination error:(NSError *)error {
//    TeLogVerbose(@"failedToSendMessage=%@,class=%@,source=0x%x,destination=0x%x",message,message.class,localElement.unicastAddress,destination);
}

- (void)didReceiveSigProxyConfigurationMessage:(SigProxyConfigurationMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination {
//    TeLogVerbose(@"didReceiveSigProxyConfigurationMessage=%@,source=0x%x,destination=0x%x",message,source,destination);
}

@end
