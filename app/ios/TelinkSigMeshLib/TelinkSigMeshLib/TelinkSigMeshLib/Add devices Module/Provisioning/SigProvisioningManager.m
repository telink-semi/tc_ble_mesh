/********************************************************************************************************
 * @file     SigProvisioningManager.m
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
//  SigProvisioningManager.m
//  SigMeshLib
//
//  Created by Liangjiazhi on 2019/8/19.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import "SigProvisioningManager.h"
#import "SigBluetooth.h"

typedef void(^InboundPDUNumberPDUReportCallBack)(NSInteger inboundPDUNumber);
typedef void(^RemotePDUResultCallBack)(BOOL isSuccess);

@interface SigProvisioningManager ()<SigMessageDelegate>
@property (nonatomic, assign) UInt16 unicastAddress;
@property (nonatomic, strong) NSData *staticOobData;
@property (nonatomic, strong) SigScanRspModel *unprovisionedDevice;
@property (nonatomic,copy) addDevice_prvisionSuccessCallBack provisionSuccessBlock;
@property (nonatomic,copy) ErrorBlock failBlock;
@property (nonatomic, assign) BOOL isProvisionning;

// for remote provision
@property (nonatomic, assign) BOOL isRemoteProvision;
@property (nonatomic, assign) UInt16 reportNodeAddress;
@property (nonatomic, strong) NSData *reportNodeUUID;
@property (nonatomic, weak) id oldDelegateForDeveloper;
@property (nonatomic,copy) InboundPDUNumberPDUReportCallBack inboundPDUNumberPDUReportBlock;

@end

@implementation SigProvisioningManager

- (instancetype)init{
    if (self = [super init]) {
        _state = ProvisionigState_ready;
        _isProvisionning = NO;
    }
    return self;
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
    [self sendPdu:pdu andAccumulateToData:self.provisioningData outboundPDUNumber:0 complete:nil];
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

//#pragma mark - BearerDataDelegate

/// This method sends the provisioning request to the device
/// over the Bearer specified in the init. Additionally, it
/// adds the request payload to given inputs. Inputs are
/// required in device authorization.
///
/// - parameter request: The request to be sent.
/// - parameter inputs:  The Provisioning Inputs.
- (void)sendPdu:(SigProvisioningPdu *)pdu andAccumulateToData:(SigProvisioningData *)data outboundPDUNumber:(UInt8)outboundPDUNumber complete:(RemotePDUResultCallBack)complete {
    NSData *pduData = pdu.pduData;
    // The first byte is the type. We only accumulate payload.
    [data accumulatePduData:[pduData subdataWithRange:NSMakeRange(1, pduData.length - 1)]];
    if (!self.isRemoteProvision) {
        [self sendPdu:pdu];
        if (complete) {
            complete(YES);
        }
    } else {
        [self sendRemoteProvisionPDUWithOutboundPDUNumber:outboundPDUNumber provisioningPDU:pduData retryCount:2 complete:complete];
    }
}

- (void)sendRemoteProvisionPDUWithOutboundPDUNumber:(UInt8)outboundPDUNumber provisioningPDU:(NSData *)provisioningPDU retryCount:(NSInteger)retryCount complete:(RemotePDUResultCallBack)complete {
    [SDKLibCommand remoteProvisioningPDUSendWithOutboundPDUNumber:outboundPDUNumber provisioningPDU:provisioningPDU destination:self.reportNodeAddress resMax:0 retryCount:0 resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        
    }];
    __block NSInteger blockRetryCount = retryCount;
    if (blockRetryCount) {
        BackgroundTimer *timer = [BackgroundTimer scheduledTimerWithTimeInterval:1.0 repeats:YES block:^(BackgroundTimer * _Nonnull t) {
            if (blockRetryCount) {
                TeLogDebug(@"BackgroundTimer,retry count=%d",blockRetryCount);
                blockRetryCount --;
                [SDKLibCommand remoteProvisioningPDUSendWithOutboundPDUNumber:outboundPDUNumber provisioningPDU:provisioningPDU destination:self.reportNodeAddress resMax:0 retryCount:0 resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                }];
            }
        }];
        [self setInboundPDUNumberPDUReportBlock:^(NSInteger inboundPDUNumber) {
            [timer invalidate];
            if (complete) {
                complete(outboundPDUNumber == inboundPDUNumber);
            }
        }];
    }
}

- (void)provisionSuccess{
    UInt16 address = self.provisioningData.unicastAddress;
    NSString *identify = nil;
    if (self.isRemoteProvision) {
        identify = [LibTools convertDataToHexStr:_reportNodeUUID];
    } else {
        identify = self.unprovisionedDevice.uuid;
    }
    UInt8 ele_count = self.provisioningCapabilities.numberOfElements;
    [SigDataSource.share saveLocationProvisionAddress:address+ele_count-1];
    NSData *devKeyData = self.provisioningData.deviceKey;
    TeLogInfo(@"deviceKey=%@",devKeyData);
    
    if (!self.isRemoteProvision) {
        self.unprovisionedDevice.address = address;
        [SigDataSource.share updateScanRspModelToDataSource:self.unprovisionedDevice];
    }
    
    SigNodeModel *model = [[SigNodeModel alloc] init];
    [model setAddress:address];
    VC_node_info_t info = model.nodeInfo;
    info.element_cnt = ele_count;
    model.deviceKey = [LibTools convertDataToHexStr:devKeyData];
    model.peripheralUUID = nil;
    //Attention: There isn't scanModel at remote add, so develop need add macAddress in provisionSuccessCallback.
    if (!self.isRemoteProvision) {
        info.cps.page0_head.cid = self.unprovisionedDevice.CID;
        info.cps.page0_head.pid = self.unprovisionedDevice.PID;
        model.UUID = identify;
    }
    if (self.isRemoteProvision) {
        model.macAddress = [LibTools convertDataToHexStr:[LibTools turnOverData:[_reportNodeUUID subdataWithRange:NSMakeRange(_reportNodeUUID.length - 6, 6)]]];
    } else {
        model.macAddress = self.unprovisionedDevice.macAddress;
    }
    model.nodeInfo = info;
    [SigDataSource.share saveDeviceWithDeviceModel:model];
}

/// This method should be called when the OOB value has been received
/// and Auth Value has been calculated.
/// It computes and sends the Provisioner Confirmation to the device.
///
/// - parameter value: The 16 byte long Auth Value.
- (void)authValueReceivedData:(NSData *)data {
    SigAuthenticationModel *auth = nil;
    self.authenticationModel = auth;
    [self.provisioningData provisionerDidObtainAuthValue:data];
    SigProvisioningPdu *pdu = [[SigProvisioningPdu alloc] initProvisioningConfirmationPduWithConfirmation:self.provisioningData.provisionerConfirmation];
    if (!self.isRemoteProvision) {
        [self sendPdu:pdu];
    } else {
        [self sendRemoteProvisionPDUWithOutboundPDUNumber:3 provisioningPDU:pdu.pduData retryCount:2 complete:nil];
    }
}

- (void)sendPdu:(SigProvisioningPdu *)pdu {
    [SigBearer.share sendBlePdu:pdu ofType:SigPduType_provisioningPdu];
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

+ (SigProvisioningManager *)share {
    static SigProvisioningManager *shareManager = nil;
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        shareManager = [[SigProvisioningManager alloc] init];
        shareManager.meshNetwork = SigDataSource.share;
    });
    return shareManager;
}

- (void)provisionWithUnicastAddress:(UInt16)unicastAddress networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail {
    self.isRemoteProvision = NO;
    self.staticOobData = nil;
    self.unicastAddress = unicastAddress;
    self.provisionSuccessBlock = provisionSuccess;
    self.failBlock = fail;
    self.unprovisionedDevice = [SigDataSource.share getScanRspModelWithUUID:[SigBearer.share getCurrentPeripheral].identifier.UUIDString];
    SigNetkeyModel *provisionNet = nil;
    for (SigNetkeyModel *net in SigDataSource.share.netKeys) {
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
    [SigBearer.share setBearerProvisioned:NO];
    self.networkKey = provisionNet;
    self.isProvisionning = YES;
    __weak typeof(self) weakSelf = self;
    TeLogInfo(@"start provision.");
    [SigBluetooth.share setBluetoothDisconnectCallback:^(CBPeripheral * _Nonnull peripheral, NSError * _Nonnull error) {
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
    [self getCapabilitiesWithTimeout:kGetCapabilitiesTimeout callback:^(SigProvisioningResponse * _Nullable response) {
        [weakSelf getCapabilitiesResultWithResponse:response];
    }];
}

- (void)provisionWithUnicastAddress:(UInt16)unicastAddress networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex staticOobData:(NSData *)oobData provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail {
    self.isRemoteProvision = NO;
    self.staticOobData = oobData;
    self.unicastAddress = unicastAddress;
    self.provisionSuccessBlock = provisionSuccess;
    self.failBlock = fail;
    self.unprovisionedDevice = [SigDataSource.share getScanRspModelWithUUID:[SigBearer.share getCurrentPeripheral].identifier.UUIDString];
    SigNetkeyModel *provisionNet = nil;
    for (SigNetkeyModel *net in SigDataSource.share.netKeys) {
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
    [SigBearer.share setBearerProvisioned:NO];
    self.networkKey = provisionNet;
    self.isProvisionning = YES;
    __weak typeof(self) weakSelf = self;
    TeLogInfo(@"start provision.");
    [SigBluetooth.share setBluetoothDisconnectCallback:^(CBPeripheral * _Nonnull peripheral, NSError * _Nonnull error) {
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
    [self getCapabilitiesWithTimeout:kGetCapabilitiesTimeout callback:^(SigProvisioningResponse * _Nullable response) {
        [weakSelf getCapabilitiesResultWithResponse:response];
    }];
}

/// founcation3: provision (If CBPeripheral isn't CBPeripheralStateConnected, SDK will connect CBPeripheral in this api. )
/// @param peripheral CBPeripheral of CoreBluetooth will be provision.
/// @param unicastAddress address of new device.
/// @param networkKey networkKey
/// @param netkeyIndex netkeyIndex
/// @param provisionType ProvisionTpye_NoOOB means oob data is 16 bytes zero data, ProvisionTpye_StaticOOB means oob data is get from HTTP API.
/// @param staticOOBData oob data get from HTTP API when provisionType is ProvisionTpye_StaticOOB.
/// @param provisionSuccess callback when provision success.
/// @param fail callback when provision fail.
- (void)provisionWithPeripheral:(CBPeripheral *)peripheral unicastAddress:(UInt16)unicastAddress networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex provisionType:(ProvisionTpye)provisionType staticOOBData:(NSData * _Nullable)staticOOBData provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail {
    self.isRemoteProvision = NO;
    if (provisionType == ProvisionTpye_NoOOB || provisionType == ProvisionTpye_StaticOOB) {
        if (peripheral.state == CBPeripheralStateConnected) {
            if (provisionType == ProvisionTpye_NoOOB) {
                TeLogVerbose(@"start noOob provision.");
                [self provisionWithUnicastAddress:unicastAddress networkKey:networkKey netkeyIndex:netkeyIndex provisionSuccess:provisionSuccess fail:fail];
            } else if (provisionType == ProvisionTpye_StaticOOB) {
                TeLogVerbose(@"start staticOob provision.");
                [self provisionWithUnicastAddress:unicastAddress networkKey:networkKey netkeyIndex:netkeyIndex staticOobData:staticOOBData provisionSuccess:provisionSuccess fail:fail];
            }
        } else {
            __weak typeof(self) weakSelf = self;
            TeLogVerbose(@"start connect for provision.");
            [SigBearer.share connectAndReadServicesWithPeripheral:peripheral result:^(BOOL successful) {
                if (successful) {
                    TeLogVerbose(@"connect successful.");
                    [weakSelf provisionWithPeripheral:peripheral unicastAddress:unicastAddress networkKey:networkKey netkeyIndex:netkeyIndex provisionType:provisionType staticOOBData:staticOOBData provisionSuccess:provisionSuccess fail:fail];
                } else {
                    if (fail) {
                        NSError *err = [NSError errorWithDomain:@"Provision fail, because connect fail before provision." code:-1 userInfo:nil];
                        fail(err);
                    }
                }
            }];
        }
    } else {
        TeLogError(@"unsupport provision type.");
    }
}

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
    self.isRemoteProvision = YES;
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
    for (SigNetkeyModel *net in SigDataSource.share.netKeys) {
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
    __weak typeof(self) weakSelf = self;
    TeLogInfo(@"start provision.");
    [SigBluetooth.share setBluetoothDisconnectCallback:^(CBPeripheral * _Nonnull peripheral, NSError * _Nonnull error) {
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
    
    [SDKLibCommand remoteProvisioningLinkOpenWithUUID:reportNodeUUID destination:reportNodeAddress resMax:1 retryCount:2 successCallback:^(UInt16 source, UInt16 destination, SigRemoteProvisioningLinkStatus * _Nonnull responseMessage) {
        TeLogInfo(@"linkOpen responseMessage=%@,parameters=%@,source=0x%x,destination=0x%x",responseMessage,responseMessage.parameters,source,destination);
        if (responseMessage.status == SigRemoteProvisioningStatus_success) {
            [weakSelf getCapabilitiesWithTimeout:kGetCapabilitiesTimeout callback:^(SigProvisioningResponse * _Nullable response) {
                [weakSelf getCapabilitiesResultWithResponse:response];
            }];
        } else {
            self.state = ProvisionigState_fail;
            TeLogDebug(@"sentProvisionConfirmationPdu error, parameters= %@",responseMessage.parameters);
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        if (error) {
            self.state = ProvisionigState_fail;
            TeLogDebug(@"sentProvisionConfirmationPdu error = %@",error.domain);
        }
    }];
    
}

#pragma mark step1:getCapabilities
- (void)getCapabilitiesWithTimeout:(NSTimeInterval)timeout callback:(prvisionResponseCallBack)block {
    TeLogInfo(@"\n\n==========provision:step1\n\n");
    self.provisionResponseBlock = block;
    [self identifyWithAttentionTimer:0];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(getCapabilitiesTimeout) object:nil];
        [self performSelector:@selector(getCapabilitiesTimeout) withObject:nil afterDelay:timeout];
    });
}

#pragma mark step2:start
- (void)sentStartNoOobProvisionPduAndPublicKeyPduWithTimeout:(NSTimeInterval)timeout callback:(prvisionResponseCallBack)block {
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

    [self.provisioningData generateProvisionerRandomAndProvisionerPublicKey];
    
    // Send Provisioning Start request.
    self.state = ProvisionigState_provisioning;
    [self.provisioningData prepareWithNetwork:self.meshNetwork networkKey:self.networkKey unicastAddress:self.unicastAddress];
    PublicKey *publicKey = [[PublicKey alloc] initWithPublicKeyType:PublicKeyType_noOobPublicKey];
    AuthenticationMethod authenticationMethod = AuthenticationMethod_noOob;

    SigProvisioningPdu *startPdu = [[SigProvisioningPdu alloc] initProvisioningstartPduWithAlgorithm:Algorithm_fipsP256EllipticCurve publicKeyType:publicKey.publicKeyType authenticationMethod:authenticationMethod authenticationAction:0 authenticationSize:0];
    __weak typeof(self) weakSelf = self;
    [self sendPdu:startPdu andAccumulateToData:self.provisioningData outboundPDUNumber:1 complete:^(BOOL isSuccess) {
        if (isSuccess) {
            weakSelf.authenticationMethod = authenticationMethod;
            // Send the Public Key of the Provisioner.
            SigProvisioningPdu *publicPdu = [[SigProvisioningPdu alloc] initProvisioningPublicKeyPduWithPublicKey:weakSelf.provisioningData.provisionerPublicKey];
            [weakSelf sendPdu:publicPdu andAccumulateToData:weakSelf.provisioningData outboundPDUNumber:2 complete:nil];
            dispatch_async(dispatch_get_main_queue(), ^{
                [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(sentStartProvisionPduAndPublicKeyPduTimeout) object:nil];
                [weakSelf performSelector:@selector(sentStartProvisionPduAndPublicKeyPduTimeout) withObject:nil afterDelay:timeout];
            });
        } else {
            TeLogError(@"send outboundPDUNumber:1, no response number:1.");
            [weakSelf sentStartProvisionPduAndPublicKeyPduTimeout];
        }
    }];
    
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

    [self.provisioningData generateProvisionerRandomAndProvisionerPublicKey];
    [self.provisioningData provisionerDidObtainAuthValue:oobData];
    
    // Send Provisioning Start request.
    self.state = ProvisionigState_provisioning;
    [self.provisioningData prepareWithNetwork:self.meshNetwork networkKey:self.networkKey unicastAddress:self.unicastAddress];
    PublicKey *publicKey = [[PublicKey alloc] initWithPublicKeyType:PublicKeyType_noOobPublicKey];
    AuthenticationMethod authenticationMethod = AuthenticationMethod_staticOob;

    SigProvisioningPdu *startPdu = [[SigProvisioningPdu alloc] initProvisioningstartPduWithAlgorithm:Algorithm_fipsP256EllipticCurve publicKeyType:publicKey.publicKeyType authenticationMethod:authenticationMethod authenticationAction:0 authenticationSize:0];
    __weak typeof(self) weakSelf = self;
    [self sendPdu:startPdu andAccumulateToData:self.provisioningData outboundPDUNumber:1 complete:^(BOOL isSuccess) {
        if (isSuccess) {
            weakSelf.authenticationMethod = authenticationMethod;
            // Send the Public Key of the Provisioner.
            SigProvisioningPdu *publicPdu = [[SigProvisioningPdu alloc] initProvisioningPublicKeyPduWithPublicKey:weakSelf.provisioningData.provisionerPublicKey];
            [weakSelf sendPdu:publicPdu andAccumulateToData:weakSelf.provisioningData outboundPDUNumber:2 complete:nil];
            dispatch_async(dispatch_get_main_queue(), ^{
                [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(sentStartProvisionPduAndPublicKeyPduTimeout) object:nil];
                [weakSelf performSelector:@selector(sentStartProvisionPduAndPublicKeyPduTimeout) withObject:nil afterDelay:timeout];
            });
        } else {
            TeLogError(@"send outboundPDUNumber:1, no response number:1.");
            [weakSelf sentStartProvisionPduAndPublicKeyPduTimeout];
        }
    }];
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
    [self authValueReceivedData:authValue];
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisionConfirmationPduTimeout) object:nil];
        [self performSelector:@selector(sentProvisionConfirmationPduTimeout) withObject:nil afterDelay:timeout];
    });
}

#pragma mark step4:Random
- (void)sentProvisionRandomPduWithTimeout:(NSTimeInterval)timeout callback:(prvisionResponseCallBack)block {
    TeLogInfo(@"\n\n==========provision:step4\n\n");
    self.provisionResponseBlock = block;
    SigProvisioningPdu *pdu = [[SigProvisioningPdu alloc] initProvisioningRandomPduWithRandom:self.provisioningData.provisionerRandom];
    if (!self.isRemoteProvision) {
        [self sendPdu:pdu];
    } else {
        [self sendRemoteProvisionPDUWithOutboundPDUNumber:4 provisioningPDU:pdu.pduData retryCount:2 complete:nil];
    }

    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentStartProvisionPduAndPublicKeyPduTimeout) object:nil];
        [self performSelector:@selector(sentStartProvisionPduAndPublicKeyPduTimeout) withObject:nil afterDelay:timeout];
    });
}

#pragma mark step5:EncryptedData
- (void)sentProvisionEncryptedDataWithMicPduWithTimeout:(NSTimeInterval)timeout callback:(prvisionResponseCallBack)block {
    TeLogInfo(@"\n\n==========provision:step5\n\n");
    self.provisionResponseBlock = block;
    SigProvisioningPdu *pdu = [[SigProvisioningPdu alloc] initProvisioningEncryptedDataWithMicPduWithEncryptedData:self.provisioningData.encryptedProvisioningDataWithMic];
    if (!self.isRemoteProvision) {
        [self sendPdu:pdu];
    } else {
        [self sendRemoteProvisionPDUWithOutboundPDUNumber:5 provisioningPDU:pdu.pduData retryCount:2 complete:nil];
    }

    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisionEncryptedDataWithMicPduTimeout) object:nil];
        [self performSelector:@selector(sentProvisionEncryptedDataWithMicPduTimeout) withObject:nil afterDelay:timeout];
    });
}

- (void)getCapabilitiesResultWithResponse:(SigProvisioningResponse *)response {
    if (response.type == SigProvisioningPduType_capabilities) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(getCapabilitiesTimeout) object:nil];
        });
        [self showCapabilitiesLog:response.capabilities];
        struct ProvisioningCapabilities capabilities = response.capabilities;
        self.provisioningCapabilities = capabilities;
        [self.provisioningData accumulatePduData:[response.responseData subdataWithRange:NSMakeRange(1, response.responseData.length - 1)]];
        self.state = ProvisionigState_capabilitiesReceived;
        if (self.unicastAddress == 0) {
            self.state = ProvisionigState_fail;
        }else{
            __weak typeof(self) weakSelf = self;
            if (self.staticOobData) {
                //当前设置为static oob provision
                if (self.provisioningCapabilities.staticOobType.staticOobInformationAvailable == 1) {
                    TeLogVerbose(@"static oob provision, staticOobData=%@",self.staticOobData);
                    [self sentStartStaticOobProvisionPduAndPublicKeyPduWithStaticOobData:self.staticOobData timeout:kStartProvisionAndPublicKeyTimeout callback:^(SigProvisioningResponse * _Nullable response) {
                        [weakSelf sentStartProvisionPduAndPublicKeyPduWithResponse:response];
                    }];
                } else {
                    //设备不支持则直接provision fail
                    TeLogError(@"This device is not support static oob.");
                    self.state = ProvisionigState_fail;
                }
            }else{
                //当前设置为no oob provision
                [self sentStartNoOobProvisionPduAndPublicKeyPduWithTimeout:kStartProvisionAndPublicKeyTimeout callback:^(SigProvisioningResponse * _Nullable response) {
                    [weakSelf sentStartProvisionPduAndPublicKeyPduWithResponse:response];
                }];
            }
        }
    }else if (!response || response.type == SigProvisioningPduType_failed) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(getCapabilitiesTimeout) object:nil];
        });
        self.state = ProvisionigState_fail;
        TeLogDebug(@"getCapabilities error = %lu",(unsigned long)response.error);
    }else{
        TeLogDebug(@"getCapabilities:no handel this response data");
    }
}

- (void)getCapabilitiesTimeout {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(getCapabilitiesTimeout) object:nil];
    });
    if (self.provisionResponseBlock) {
        self.provisionResponseBlock(nil);
    }
}

- (void)sentStartProvisionPduAndPublicKeyPduWithResponse:(SigProvisioningResponse *)response {
    TeLogInfo(@"device public key back:%@",response.responseData);
    if (response.type == SigProvisioningPduType_publicKey) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentStartProvisionPduAndPublicKeyPduTimeout) object:nil];
        });
        [self.provisioningData accumulatePduData:[response.responseData subdataWithRange:NSMakeRange(1, response.responseData.length - 1)]];
        [self.provisioningData provisionerDidObtainWithDevicePublicKey:response.publicKey];
        if (self.provisioningData.sharedSecret && self.provisioningData.sharedSecret.length > 0) {
//            [self obtainAuthValue];
            __weak typeof(self) weakSelf = self;
            [self sentProvisionConfirmationPduWithTimeout:kProvisionConfirmationTimeout callback:^(SigProvisioningResponse * _Nullable response) {
                [weakSelf sentProvisionConfirmationPduWithResponse:response];
            }];
        }else{
            TeLogDebug(@"calculate SharedSecret fail.");
            self.state = ProvisionigState_fail;
        }
    }else if (!response || response.type == SigProvisioningPduType_failed) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentStartProvisionPduAndPublicKeyPduTimeout) object:nil];
        });
        self.state = ProvisionigState_fail;
        TeLogDebug(@"sentStartProvisionPduAndPublicKeyPdu error = %lu",(unsigned long)response.error);
    }else{
        TeLogDebug(@"sentStartProvisionPduAndPublicKeyPdu:no handel this response data");
    }
}

- (void)sentStartProvisionPduAndPublicKeyPduTimeout {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentStartProvisionPduAndPublicKeyPduTimeout) object:nil];
    });
    if (self.provisionResponseBlock) {
        self.provisionResponseBlock(nil);
    }
}

- (void)sentProvisionConfirmationPduWithResponse:(SigProvisioningResponse *)response {
    TeLogInfo(@"device confirmation back:%@",response.responseData);
    if (response.type == SigProvisioningPduType_confirmation) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisionConfirmationPduTimeout) object:nil];
        });
        
        [self.provisioningData provisionerDidObtainWithDeviceConfirmation:response.confirmation];
        __weak typeof(self) weakSelf = self;
        [self sentProvisionRandomPduWithTimeout:kProvisionRandomTimeout callback:^(SigProvisioningResponse * _Nullable response) {
            [weakSelf sentProvisionRandomPduWithResponse:response];
        }];
    }else if (!response || response.type == SigProvisioningPduType_failed) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisionConfirmationPduTimeout) object:nil];
        });
        self.state = ProvisionigState_fail;
        TeLogDebug(@"sentProvisionConfirmationPdu error = %lu",(unsigned long)response.error);
    }else{
        TeLogDebug(@"sentProvisionConfirmationPdu:no handel this response data");
    }
}

- (void)sentProvisionConfirmationPduTimeout {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisionConfirmationPduTimeout) object:nil];
    });
    if (self.provisionResponseBlock) {
        self.provisionResponseBlock(nil);
    }
}

- (void)sentProvisionRandomPduWithResponse:(SigProvisioningResponse *)response {
    TeLogInfo(@"device random back:%@",response.responseData);
    if (response.type == SigProvisioningPduType_random) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisionRandomPduTimeout) object:nil];
        });
        [self.provisioningData provisionerDidObtainWithDeviceRandom:response.random];
        if (![self.provisioningData validateConfirmation]) {
            TeLogDebug(@"validate Confirmation fail");
            self.state = ProvisionigState_fail;
            return;
        }
        __weak typeof(self) weakSelf = self;
        [self sentProvisionEncryptedDataWithMicPduWithTimeout:kSentProvisionEncryptedDataWithMicTimeout callback:^(SigProvisioningResponse * _Nullable response) {
            [weakSelf sentProvisionEncryptedDataWithMicPduWithResponse:response];
        }];
    }else if (!response || response.type == SigProvisioningPduType_failed) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisionRandomPduTimeout) object:nil];
        });
        self.state = ProvisionigState_fail;
        TeLogDebug(@"sentProvisionRandomPdu error = %lu",(unsigned long)response.error);
    }else{
        TeLogDebug(@"sentProvisionRandomPdu:no handel this response data");
    }
}

- (void)sentProvisionRandomPduTimeout {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisionRandomPduTimeout) object:nil];
    });
    if (self.provisionResponseBlock) {
        self.provisionResponseBlock(nil);
    }
}

- (void)sentProvisionEncryptedDataWithMicPduWithResponse:(SigProvisioningResponse *)response {
    TeLogInfo(@"\n\n==========provision end.\n\n");
    TeLogInfo(@"device provision result back:%@",response.responseData);
    if (response.type == SigProvisioningPduType_complete) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisionEncryptedDataWithMicPduTimeout) object:nil];
        });
        [self provisionSuccess];
        [SigBearer.share setBearerProvisioned:YES];
        if (self.provisionSuccessBlock) {
            self.provisionSuccessBlock(self.unprovisionedDevice.uuid,self.unicastAddress);
        }
        if (self.isRemoteProvision) {
            SigMeshLib.share.delegateForDeveloper = self.oldDelegateForDeveloper;
        }
    }else if (!response || response.type == SigProvisioningPduType_failed) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisionEncryptedDataWithMicPduTimeout) object:nil];
        });
        self.state = ProvisionigState_fail;
        TeLogDebug(@"sentProvisionRandomPdu error = %lu",(unsigned long)response.error);
    }else{
        TeLogDebug(@"sentProvisionRandomPdu:no handel this response data");
    }
    self.provisionResponseBlock = nil;
}

- (void)sentProvisionEncryptedDataWithMicPduTimeout {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sentProvisionEncryptedDataWithMicPduTimeout) object:nil];
    });
    if (self.provisionResponseBlock) {
        self.provisionResponseBlock(nil);
    }
}

- (void)showCapabilitiesLog:(struct ProvisioningCapabilities)capabilities {
    TeLogInfo(@"\n------ Capabilities ------\nNumber of elements: %d\nAlgorithms: %@\nPublic Key Type: %@\nStatic OOB Type: %@\nOutput OOB Size: %d\nOutput OOB Actions: %d\nInput OOB Size: %d\nInput OOB Actions: %d\n--------------------------",capabilities.numberOfElements,capabilities.algorithms.fipsP256EllipticCurve == 1 ?@"FIPS P-256 Elliptic Curve":@"None",capabilities.publicKeyType == PublicKeyType_noOobPublicKey ?@"No OOB Public Key":@"OOB Public Key",capabilities.staticOobType.staticOobInformationAvailable == 1 ?@"YES":@"None",capabilities.outputOobSize,capabilities.outputOobActions.value,capabilities.inputOobSize,capabilities.inputOobActions.value);
}

#pragma mark - SigMessageDelegate

- (void)didReceiveMessage:(SigMeshMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination {
    TeLogVerbose(@"didReceiveMessage=%@,message.parameters=%@,source=0x%x,destination=0x%x",message,message.parameters,source,destination);

    if ([message isKindOfClass:[SigRemoteProvisioningPDUReport class]] && self.isRemoteProvision) {
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
        if (SigProvisioningManager.share.provisionResponseBlock) {
    //                TeLogDebug(@"response=%@",response.responseData);
            SigProvisioningManager.share.provisionResponseBlock(response);
        }
    } else if ([message isKindOfClass:[SigRemoteProvisioningPDUOutboundReport class]] && self.isRemoteProvision) {
        UInt8 inboundPDUNumber = ((SigRemoteProvisioningPDUOutboundReport *)message).outboundPDUNumber;
        TeLogVerbose(@"inboundPDUNumber=%d",inboundPDUNumber);
        if (self.inboundPDUNumberPDUReportBlock) {
            self.inboundPDUNumberPDUReportBlock(inboundPDUNumber);
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
