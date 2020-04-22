/********************************************************************************************************
 * @file     SigMeshLib.m
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
//  SigMeshLib.m
//  SigMeshLib
//
//  Created by Liangjiazhi on 2019/8/15.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import "SigMeshLib.h"

@interface SigMeshLib ()<SigMessageDelegate>
@end

@implementation SigMeshLib

static SigMeshLib *shareLib = nil;

+ (SigMeshLib *)share {
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        shareLib = [[SigMeshLib alloc] init];
        shareLib.commands = [NSMutableArray array];
        shareLib.dataSource = SigDataSource.share;
        [shareLib config];
        [shareLib initDelegate];
    });
    return shareLib;
}

- (void)initDelegate {
    _delegate = shareLib;
}

- (void)setNetworkManager:(SigNetworkManager *)networkManager {
    _networkManager = networkManager;
}

- (instancetype)init{
    if (self = [super init]) {
        [self config];
    }
    return self;
}

- (void)config{
    _defaultTtl = 10;
    _incompleteMessageTimeout = 10.0;
    _acknowledgmentTimerInterval = 0.150;
    _transmissionTimerInteral = 0.200;
    _retransmissionLimit = 5;
    _acknowledgmentMessageTimeout = 30.0;
    _acknowledgmentMessageInterval = 2.0;
    _networkTransmitIntervalSteps = 0b11111;
    _networkTransmitInterval = (_networkTransmitIntervalSteps + 1) * 10 / 1000.0;//单位ms
    _queue = dispatch_queue_create("SigMeshLib.queue", DISPATCH_QUEUE_SERIAL);
    _delegateQueue = dispatch_queue_create("SigMeshLib.delegateQueue", DISPATCH_QUEUE_SERIAL);
}

- (void)setNetworkTransmitIntervalSteps:(UInt8)networkTransmitIntervalSteps {
    if (networkTransmitIntervalSteps > 0b11111) {
        TeLogError(@"networkTransmitIntervalSteps range:0~0b11111! Set to default value 0b11111.");
        networkTransmitIntervalSteps = 0b11111;
    }
    _networkTransmitIntervalSteps = networkTransmitIntervalSteps;
    _networkTransmitInterval = (_networkTransmitIntervalSteps + 1) * 10 / 1000.0;//单位ms
}

#pragma mark - Receive Mesh Messages

- (void)bearerDidDeliverData:(NSData *)data type:(SigPduType)type {
    if (_networkManager == nil) {
        TeLogDebug(@"_networkManager == nil");
        return;
    }
    __weak typeof(self) weakSelf = self;
    dispatch_async(_queue, ^{
        [weakSelf.networkManager handleIncomingPdu:data ofType:type];
    });
}

- (void)updateOnlineStatusWithDeviceAddress:(UInt16)address deviceState:(DeviceState)state bright100:(UInt8)bright100 temperature100:(UInt8)temperature100{
    SigGenericOnOffStatus *message = [[SigGenericOnOffStatus alloc] initWithIsOn:state == DeviceStateOn];
    SDKLibCommand *command = [self getCommandWithReceiveMessage:message];
    BOOL shouldCallback = NO;
    if (![command.responseSourceArray containsObject:@(address)]) {
        shouldCallback = YES;
        [command.responseSourceArray addObject:@(address)];
    }
    if (command.responseSourceArray.count >= command.responseMaxCount) {
        [self commandResponseFinishWithCommand:command];
    }
    //all response message callback in this code.
    if (shouldCallback && command && command.responseAllMessageCallBack) {
        command.responseAllMessageCallBack(address,_dataSource.curLocationNodeModel.address,message);
    }
}

#pragma mark - Send Mesh Messages

- (SigMessageHandle *)sendMeshMessage:(SigMeshMessage *)message fromLocalElement:(nullable SigElementModel *)localElement toDestination:(SigMeshAddress *)destination usingApplicationKey:(SigAppkeyModel *)applicationKey command:(SDKLibCommand *)command {
    UInt8 ttl = localElement.parentNode.defaultTTL;
    if (![SigHelper.share isRelayedTTL:ttl]) {
        ttl = _networkManager.defaultTtl;
    }
    return [self sendMeshMessage:message fromLocalElement:localElement toDestination:destination withTtl:ttl usingApplicationKey:applicationKey command:command];
}

- (SigMessageHandle *)sendMeshMessage:(SigMeshMessage *)message fromLocalElement:(nullable SigElementModel *)localElement toDestination:(SigMeshAddress *)destination withTtl:(UInt8)initialTtl usingApplicationKey:(SigAppkeyModel *)applicationKey command:(SDKLibCommand *)command {
    if (_networkManager == nil || SigDataSource.share == nil) {
        TeLogError(@"Mesh Network not created");
        return nil;
    }
    if (SigDataSource.share.curLocationNodeModel == nil || SigDataSource.share.curLocationNodeModel.elements.firstObject == nil) {
        TeLogError(@"Local Provisioner has no Unicast Address assigned.");
        return nil;
    }
    SigNodeModel *localNode = SigDataSource.share.curLocationNodeModel;
    SigElementModel *source = localNode.elements.firstObject;
    if (source.parentNode != localNode) {
        TeLogError(@"The Element does not belong to the local Node.");
        return nil;
    }
    if (![SigHelper.share isRelayedTTL:initialTtl]) {
        TeLogError(@"TTL value %d is invalid.",initialTtl);
        return nil;
    }
    __weak typeof(self) weakSelf = self;
    dispatch_async(_queue, ^{
        if ([command.curMeshMessage isKindOfClass:[SigMeshMessage class]] && [SigHelper.share isAcknowledgedMessage:(SigMeshMessage *)command.curMeshMessage] && command.responseMaxCount == 0) {
            command.responseMaxCount = 1;
        }
//        if (command.responseMaxCount == 0) {
//            command.retryCount = 0;
//        }
        [weakSelf addCommandToCacheListWithCommand:command];
        if (command.appkeyA) {
            [weakSelf.networkManager sendMeshMessage:message fromElement:source toDestination:destination withTtl:initialTtl usingApplicationKey:applicationKey command:command];
        } else {
            [weakSelf.networkManager sendMeshMessage:message fromElement:source toDestination:destination withTtl:initialTtl usingApplicationKey:applicationKey];
        }
        if (command.retryCount) {
            BackgroundTimer *timer = [BackgroundTimer scheduledTimerWithTimeInterval:command.timeout repeats:YES block:^(BackgroundTimer * _Nonnull t) {
                if (command.hadRetryCount < command.retryCount) {
                    command.hadRetryCount ++;
                    TeLogDebug(@"command.curMeshMessage=%@,retry count=%d",command.curMeshMessage,command.hadRetryCount);
                    if (command.appkeyA) {
                        [weakSelf.networkManager sendMeshMessage:message fromElement:source toDestination:destination withTtl:initialTtl usingApplicationKey:applicationKey command:command];
                    } else {
                        [weakSelf.networkManager sendMeshMessage:message fromElement:source toDestination:destination withTtl:initialTtl usingApplicationKey:applicationKey];
                    }
                } else {
                    [weakSelf commandTimeoutWithCommand:command];
                }
            }];
            command.retryTimer = timer;
        }
    });
    return [[SigMessageHandle alloc] initForMessage:message sentFromSource:source.unicastAddress toDestination:destination.address usingManager:self];
}

- (SigMessageHandle *)sendConfigMessage:(SigConfigMessage *)message toDestination:(UInt16)destination command:(SDKLibCommand *)command {
    UInt8 ttl = self.dataSource.curLocationNodeModel.defaultTTL;
    if (![SigHelper.share isRelayedTTL:ttl]) {
        ttl = _networkManager.defaultTtl;
    }
    return [self sendConfigMessage:message toDestination:destination withTtl:ttl command:command];
}

- (SigMessageHandle *)sendConfigMessage:(SigConfigMessage *)message toDestination:(UInt16)destination withTtl:(UInt8)initialTtl command:(SDKLibCommand *)command {
    if (SigDataSource.share == nil) {
        TeLogError(@"Mesh Network not created");
        return nil;
    }
    if (SigDataSource.share.curLocationNodeModel == nil || SigDataSource.share.curLocationNodeModel.address == 0) {
        TeLogError(@"Local Provisioner has no Unicast Address assigned.");
        return nil;
    }
    UInt16 source = SigDataSource.share.curLocationNodeModel.address;
    if (![SigHelper.share isUnicastAddress:destination]) {
        TeLogError(@"Address: 0x%x is not a Unicast Address.",destination);
        return nil;
    }
    if (![SigHelper.share isRelayedTTL:initialTtl]) {
        TeLogError(@"TTL value %d is invalid.",initialTtl);
        return nil;
    }
    __weak typeof(self) weakSelf = self;
    dispatch_async(_queue, ^{
        if ([command.curMeshMessage isKindOfClass:[SigMeshMessage class]] && [SigHelper.share isAcknowledgedMessage:(SigMeshMessage *)command.curMeshMessage] && command.responseMaxCount == 0) {
            command.responseMaxCount = 1;
        }
        [weakSelf addCommandToCacheListWithCommand:command];
        if (command.retryCount) {
            BackgroundTimer *timer = [BackgroundTimer scheduledTimerWithTimeInterval:command.timeout repeats:YES block:^(BackgroundTimer * _Nonnull t) {
                if (command.hadRetryCount < command.retryCount) {
                    command.hadRetryCount ++;
                    TeLogDebug(@"command.curMeshMessage=%@,retry count=%d",command.curMeshMessage,command.hadRetryCount);
                    [weakSelf.networkManager sendConfigMessage:message toDestination:destination withTtl:initialTtl];
                } else {
                    [weakSelf commandTimeoutWithCommand:command];
                }
            }];
            command.retryTimer = timer;
        }
        [weakSelf.networkManager sendConfigMessage:message toDestination:destination withTtl:initialTtl];
    });
    return [[SigMessageHandle alloc] initForMessage:message sentFromSource:source toDestination:destination usingManager:self];
}

- (void)sendSigProxyConfigurationMessage:(SigProxyConfigurationMessage *)message command:(SDKLibCommand *)command {
    if (SigDataSource.share == nil) {
        TeLogError(@"Mesh Network not created");
        return;
    }
    __weak typeof(self) weakSelf = self;
    dispatch_async(_queue, ^{
        if ([command.curMeshMessage isKindOfClass:[SigMeshMessage class]] && [SigHelper.share isAcknowledgedMessage:(SigMeshMessage *)command.curMeshMessage] && command.responseMaxCount == 0) {
            command.responseMaxCount = 1;
        }
        [weakSelf addCommandToCacheListWithCommand:command];
        if (command.retryCount) {
            BackgroundTimer *timer = [BackgroundTimer scheduledTimerWithTimeInterval:command.timeout repeats:YES block:^(BackgroundTimer * _Nonnull t) {
                if (command.hadRetryCount < command.retryCount) {
                    command.hadRetryCount ++;
                    TeLogDebug(@"command.curMeshMessage=%@,retry count=%d",command.curMeshMessage,command.hadRetryCount);
                    [weakSelf.networkManager sendSigProxyConfigurationMessage:message];
                } else {
                    [weakSelf commandTimeoutWithCommand:command];
                }
            }];
            command.retryTimer = timer;
        }
        [weakSelf.networkManager sendSigProxyConfigurationMessage:message];
    });
}

- (NSError *)sendTelinkApiGetOnlineStatueFromUUIDWithMessage:(SigMeshMessage *)message command:(SDKLibCommand *)command {
    if (SigDataSource.share == nil) {
        TeLogError(@"Mesh Network not created");
        return [NSError errorWithDomain:kSigMeshLibNoCreateMeshNetworkErrorMessage code:kSigMeshLibNoCreateMeshNetworkErrorCode userInfo:nil];
    }
    if ([command.curMeshMessage isKindOfClass:[SigMeshMessage class]] && [SigHelper.share isAcknowledgedMessage:(SigMeshMessage *)command.curMeshMessage] && command.responseMaxCount == 0) {
        command.responseMaxCount = 1;
    }
    [self addCommandToCacheListWithCommand:command];
    CBCharacteristic *onlineStatusCharacteristic = [SigBluetooth.share getCharacteristicWithUUIDString:kOnlineStatusCharacteristicsID OfPeripheral:SigBearer.share.getCurrentPeripheral];
    if (onlineStatusCharacteristic != nil) {
        uint8_t buffer[1]={1};
        NSData *data = [NSData dataWithBytes:buffer length:1];
        [SigBearer.share.getCurrentPeripheral writeValue:data forCharacteristic:onlineStatusCharacteristic type:CBCharacteristicWriteWithResponse];
        if (command.retryCount) {
            __weak typeof(self) weakSelf = self;
            BackgroundTimer *timer = [BackgroundTimer scheduledTimerWithTimeInterval:command.timeout repeats:YES block:^(BackgroundTimer * _Nonnull t) {
                if (command.hadRetryCount < command.retryCount) {
                    command.hadRetryCount ++;
                    TeLogDebug(@"command.curMeshMessage=%@,retry count=%d",command.curMeshMessage,command.hadRetryCount);
                    [SigBearer.share.getCurrentPeripheral writeValue:data forCharacteristic:onlineStatusCharacteristic type:CBCharacteristicWriteWithResponse];
                } else {
                    [weakSelf commandTimeoutWithCommand:command];
                }
            }];
            command.retryTimer = timer;
        }
        return nil;
    }else{
        return [NSError errorWithDomain:kSigMeshLibNofoundOnlineStatusCharacteristicErrorMessage code:kSigMeshLibNofoundOnlineStatusCharacteristicErrorCode userInfo:nil];
    }
}

- (void)cancelSigMessageHandle:(SigMessageHandle *)messageId {
    if (_networkManager == nil) {
        TeLogError(@"Error: Mesh Network not created.");
        return;
    }
    __weak typeof(self) weakSelf = self;
    dispatch_async(_queue, ^{
        [weakSelf.networkManager cancelSigMessageHandle:messageId];
        SDKLibCommand *command = [weakSelf getCommandWithSendMessageOpCode:messageId.opCode];
        [weakSelf commandResponseFinishWithCommand:command];
    });
}

- (void)addCommandToCacheListWithCommand:(SDKLibCommand *)command {
    if ([command.curMeshMessage isKindOfClass:[SigMeshMessage class]] && [SigHelper.share isAcknowledgedMessage:(SigMeshMessage *)command.curMeshMessage] && command.responseMaxCount == 0) {
        command.responseMaxCount = 1;
    }
    //存在response的指令出存储
    if (command.responseAllMessageCallBack || command.resultCallback || (command.retryCount > 0 && command.responseMaxCount > 0)) {
        //command存储下来，超时或者失败，或者返回response时，从该地方拿到command，获取里面的callback，执行，再删除。
        [self.commands addObject:command];
        TeLogVerbose(@"self.commands=%@",self.commands);
        if (command.retryCount == 0) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(commandTimeoutWithCommand:) object:command];
                [self performSelector:@selector(commandTimeoutWithCommand:) withObject:command afterDelay:command.timeout];
            });
        }
    }
}

- (void)commandTimeoutWithCommand:(SDKLibCommand *)command {
    [self commandResponseFinishWithCommand:command];
    if (command.resultCallback) {
        TeLogDebug(@"timeout command:%@",command);
        NSError *error = [NSError errorWithDomain:kSigMeshLibCommandTimeoutErrorMessage code:kSigMeshLibCommandTimeoutErrorCode userInfo:nil];
        command.resultCallback(NO, error);
    }
}

- (void)commandResponseFinishWithCommand:(SDKLibCommand *)command {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(commandTimeoutWithCommand:) object:command];
    });
    if (command.retryTimer) {
        [command.retryTimer invalidate];
    }
    [self.commands removeObject:command];
    if (self.commands.count == 0) {
        [self isBusyNow];
    }
}

- (void)handleResponseMaxCommands {
    NSArray *temArray = [NSArray arrayWithArray:self.commands];
    for (SDKLibCommand *com in temArray) {
        if (com.responseMaxCount == 0) {
            [self.commands removeObject:com];
        }
    }
}

- (BOOL)isBusyNow {
    [self handleResponseMaxCommands];
    BOOL busy = self.commands.count > 0;
    dispatch_async(dispatch_get_main_queue(), ^{
        [[NSNotificationCenter defaultCenter] postNotificationName:kNotifyCommandIsBusyOrNot object:nil userInfo:@{kCommandIsBusyKey : @(busy)}];
    });
    return busy;
}

#pragma mark - Helper methods for Bearer support

- (void)didDeliverData:(NSData *)data ofType:(SigPduType)type bearer:(SigBearer *)bearer{
    [self bearerDidDeliverData:data type:type];
}

#pragma mark - SigMessageDelegate

- (void)didReceiveMessage:(SigMeshMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination {
    TeLogVerbose(@"didReceiveMessage=%@,message.parameters=%@,source=0x%x,destination=0x%x",message,message.parameters,source,destination);
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:source];

    //根据设备是否打开了publish功能来判断是否给该设备添加监测离线的定时器。
    if (message.opCode == SigOpCode_lightCTLStatus || message.opCode == SigOpCode_lightHSLStatus || message.opCode == SigOpCode_lightLightnessStatus || message.opCode == SigOpCode_genericOnOffStatus) {
        if (node && node.hasOpenPublish) {
            [SigPublishManager.share startCheckOfflineTimerWithAddress:@(source)];
        }
    }
    
    SDKLibCommand *command = [self getCommandWithReceiveMessage:message];
    BOOL shouldCallback = NO;
    if (command && ![command.responseSourceArray containsObject:@(source)]) {
        shouldCallback = YES;
        //node may delete from SigDataSource, but no reset from mesh network.
        if (node) {
            [command.responseSourceArray addObject:@(source)];
        }
    }
    //update status to SigDataSource before callback.
    [SigDataSource.share updateNodeStatusWithBaseMeshMessage:message source:source];
    //非直连设备断电，再上电后设备端会主动上报0x824E。
    if (message.opCode == SigOpCode_lightLightnessStatus) {
        if (SigPublishManager.share.discoverOnlineNodeCallback) {
            SigPublishManager.share.discoverOnlineNodeCallback(@(source));
        }
    }

    //all response message callback in this code.
    if (shouldCallback && command && command.responseAllMessageCallBack) {
        dispatch_async(dispatch_get_main_queue(), ^{
            command.responseAllMessageCallBack(source,destination,message);
        });
    }
    if (command.responseMaxCount != 0) {
        if (command && command.responseSourceArray.count >= command.responseMaxCount) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [self commandResponseFinishWithCommand:command];
                if (command.resultCallback) {
                    command.resultCallback(YES, nil);
                }
            });
        }
    } else {
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(commandTimeoutWithCommand:) object:command];
        });
        if (command.retryTimer) {
            [command.retryTimer invalidate];
        }
    }

}

- (void)didSendMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toDestination:(UInt16)destination {
    TeLogVerbose(@"didSendMessage=%@,class=%@,source=0x%x,destination=0x%x",message,message.class,localElement.unicastAddress,destination);
    SDKLibCommand *command = [self getCommandWithSendMessage:message];
    BOOL shouldCallback = NO;
    if (command && (command.retryCount == 0 && command.responseMaxCount == 0)) {
        shouldCallback = YES;
    }
    if (command && destination == localElement.unicastAddress) {
        shouldCallback = YES;
    }
    if (command && shouldCallback) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [self commandResponseFinishWithCommand:command];
        });
    }

    //send finished of noAckMessage callback in this code.
    if (shouldCallback && command && command.resultCallback) {
        dispatch_async(dispatch_get_main_queue(), ^{
            command.resultCallback(YES, nil);
        });
    }
}

- (void)failedToSendMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toDestination:(UInt16)destination error:(NSError *)error {
    TeLogVerbose(@"failedToSendMessage=%@,class=%@,source=0x%x,destination=0x%x",message,message.class,localElement.unicastAddress,destination);
    SDKLibCommand *command = [self getCommandWithSendMessage:message];
    [_commands removeObject:command];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(commandTimeoutWithCommand:) object:command];
    });
    if (command.resultCallback) {
        command.resultCallback(NO, error);
    }
}

- (void)didReceiveSigProxyConfigurationMessage:(SigProxyConfigurationMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination {
    TeLogVerbose(@"didReceiveSigProxyConfigurationMessage=%@,source=0x%x,destination=0x%x",message,source,destination);
    SDKLibCommand *command = [self getCommandWithReceiveMessage:(SigMeshMessage *)message];
    [self commandResponseFinishWithCommand:command];

    //callback
    if (command && command.responseFilterStatusCallBack) {
        dispatch_async(dispatch_get_main_queue(), ^{
            command.responseFilterStatusCallBack(source,destination,(SigFilterStatus *)message);
        });
    }
    if (command && command.resultCallback) {
        dispatch_async(dispatch_get_main_queue(), ^{
            command.resultCallback(YES, nil);
        });
    }
}

#pragma mark - Private

- (SDKLibCommand *)getCommandWithReceiveMessage:(SigMeshMessage *)message {
    SDKLibCommand *tem = nil;
    if ([message isKindOfClass:[SigConfigMessage class]]) {
        for (SDKLibCommand *com in _commands) {
            if (((SigConfigMessage *)com.curMeshMessage).responseOpCode == message.opCode) {
                tem = com;
                break;
            }
        }
    } else if ([message isKindOfClass:[SigGenericMessage class]]) {
        for (SDKLibCommand *com in _commands) {
            if ([SigHelper.share isAcknowledgedMessage:(SigMeshMessage *)com.curMeshMessage] && ((SigAcknowledgedGenericMessage *)com.curMeshMessage).responseOpCode == message.opCode) {
                tem = com;
                break;
            }
        }
    } else if ([message isKindOfClass:[SigFilterStatus class]]) {
        for (SDKLibCommand *com in _commands) {
            if (((SigStaticAcknowledgedProxyConfigurationMessage *)com.curMeshMessage).responseOpCode == message.opCode) {
                tem = com;
                break;
            }
        }
    } else if ([message isKindOfClass:[SigUnknownMessage class]]) {//未定义的vendor回包
        for (SDKLibCommand *com in _commands) {
            if (((SigIniMeshMessage *)com.curMeshMessage).responseOpCode == message.opCode || ((SigIniMeshMessage *)com.curMeshMessage).responseOpCode == ((message.opCode >> 16) & 0xFF)) {
                tem = com;
                break;
            }
        }
    }
    return tem;
}

- (SDKLibCommand *)getCommandWithSendMessage:(SigMeshMessage *)message {
    SDKLibCommand *tem = nil;
    if ([message isKindOfClass:[SigConfigMessage class]]) {
        for (SDKLibCommand *com in _commands) {
            if (((SigConfigMessage *)com.curMeshMessage).opCode == message.opCode) {
                tem = com;
                break;
            }
        }
    } else if ([message isKindOfClass:[SigAcknowledgedGenericMessage class]]) {
        for (SDKLibCommand *com in _commands) {
            if (((SigAcknowledgedGenericMessage *)com.curMeshMessage).opCode == message.opCode) {
                tem = com;
                break;
            }
        }
    } else if ([message isKindOfClass:[SigIniMeshMessage class]]) {
        for (SDKLibCommand *com in _commands) {
            if (((SigIniMeshMessage *)com.curMeshMessage).opCode == message.opCode) {
                tem = com;
                break;
            }
        }
    } else if ([message isKindOfClass:[SigMeshMessage class]]) {
           for (SDKLibCommand *com in _commands) {
               if (((SigMeshMessage *)com.curMeshMessage).opCode == message.opCode) {
                   tem = com;
                   break;
               }
           }
    }
    return tem;
}

- (SDKLibCommand *)getCommandWithSendMessageOpCode:(UInt32)sendOpCode {
    SDKLibCommand *tem = nil;
    for (SDKLibCommand *com in _commands) {
        if ([com.curMeshMessage isKindOfClass:[SigMeshMessage class]]) {
            if (((SigMeshMessage *)com.curMeshMessage).opCode == sendOpCode) {
                tem = com;
                break;
            }
        }
    }
    return tem;
}

@end
