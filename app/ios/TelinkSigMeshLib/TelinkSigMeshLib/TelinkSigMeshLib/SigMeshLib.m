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

@interface SigMeshLib ()<SigMeshNetworkDelegate>
@end

@implementation SigMeshLib

+ (SigMeshLib *)share {
    static SigMeshLib *shareLib = nil;
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        shareLib = [[SigMeshLib alloc] init];
        shareLib.commands = [NSMutableArray array];
        shareLib.dataSource = SigDataSource.share;
        [shareLib config];
        shareLib.delegate = shareLib;
    });
    return shareLib;
}

- (void)setNetworkManager:(SigNetworkManager *)networkManager {
    _networkManager = networkManager;
}

- (BOOL)isNetworkCreated{
    return self.dataSource != nil;
}

- (BOOL)isBusy {
    return self.commands.count > 0;
}

- (instancetype)init{
    if (self = [super init]) {
        [self config];
    }
    return self;
}

- (instancetype)initWithStorage:(SigStorage *)storage{
    if (self = [super init]) {
        [self config];
        self.storage = storage;
        //    self.meshData = MeshData()
    }
    return self;
}

- (instancetype)initWithFileName:(NSString *)fileName{
    if (self = [super init]) {
        [self config];
        //    self.init(using: LocalStorage(fileName: fileName))
    }
    return self;
}

- (void)config{
    _defaultTtl = 5;
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

//- (void)sendMessage:(SigConfigMessage *)message toDestinationAddress:(UInt16)destination{
//    if (_networkManager == nil) {
//        TeLogError(@"_networkManager = nil.");
//        return;
//    }
//    [_networkManager sendConfigMessage:message toDestination:destination withTtl:0];
//}

//- (void)sendMessage:(SigConfigMessage *)message toNode:(SigNodeModel *)model{
//    [self sendMessage:message toDestinationAddress:model.address];
//}

//- (void)sendMessage:(SigConfigMessage *)message toElement:(SigElementModel *)element{
//    SigNodeModel *node = element.parentNode;
//    if (node == nil) {
//        TeLogError(@"Error: Element does not belong to a Node.");
//        return;
//    }
//    [self sendMessage:message toNode:node];
//}

//- (void)sendConfigMessage:(SigConfigMessage *)message toModel:(SigModelIDModel *)model{
//    SigElementModel *element = model.parentElement;
//    if (element == nil) {
//        TeLogError(@"Error: Model does not belong to a Element.");
//        return;
//    }
//    [self sendMessage:message toElement:element];
//}

#pragma mark - Send / Receive Mesh Messages

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
#warning 2019年12月05日09:35:54，待完善(需添加更新状态带数据源)
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

//- (SigMessageHandle *)publishSigMeshMessage:(SigMeshMessage *)message fromModel:(SigModelIDModel *)model withTtl:(UInt8)initialTtl command:(SDKLibCommand *)command {
//    if (model.publish && model.parentElement && SigDataSource.share.appKeys[model.publish.index]) {
//        SigMeshAddress *publicationAddress = [[SigMeshAddress alloc] initWithHex:model.publish.address];
//        [self sendMeshMessage:message fromLocalElement:model.parentElement toDestination:publicationAddress withTtl:initialTtl usingApplicationKey:SigDataSource.share.appKeys[model.publish.index] command:command];
//    }
//    return nil;
//}
//- (SigMessageHandle *)publishSigMeshMessage:(SigMeshMessage *)message fromModel:(SigModelIDModel *)model command:(SDKLibCommand *)command {
//    UInt8 ttl = model.parentElement.parentNode.defaultTTL;
//    if (![SigHelper.share isValidTTL:ttl]) {
//        ttl = _networkManager.defaultTtl;
//    }
//    return [self publishSigMeshMessage:message fromModel:model withTtl:ttl command:command];
//}

- (SigMessageHandle *)sendMeshMessage:(SigMeshMessage *)message fromLocalElement:(nullable SigElementModel *)localElement toDestination:(SigMeshAddress *)destination usingApplicationKey:(SigAppkeyModel *)applicationKey command:(SDKLibCommand *)command {
    UInt8 ttl = localElement.parentNode.defaultTTL;
    if (![SigHelper.share isValidTTL:ttl]) {
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
    if (![SigHelper.share isValidTTL:initialTtl]) {
        TeLogError(@"TTL value %d is invalid.",initialTtl);
        return nil;
    }
    __weak typeof(self) weakSelf = self;
    dispatch_async(_queue, ^{
        if ([command.curMeshMessage isKindOfClass:[SigMeshMessage class]] && [SigHelper.share isAcknowledgedMessage:(SigMeshMessage *)command.curMeshMessage] && command.responseMaxCount == 0) {
            command.responseMaxCount = 1;
        }
        if (command.responseMaxCount == 0) {
            command.retryCount = 0;
        }
        [weakSelf addCommandToCacheListWithCommand:command];
        [weakSelf.networkManager sendMeshMessage:message fromElement:source toDestination:destination withTtl:initialTtl usingApplicationKey:applicationKey];
        if (command.retryCount) {
            BackgroundTimer *timer = [BackgroundTimer scheduledTimerWithTimeInterval:command.timeout repeats:YES block:^(BackgroundTimer * _Nonnull t) {
                if (command.hadRetryCount < command.retryCount) {
                    command.hadRetryCount ++;
                    TeLogDebug(@"command.curMeshMessage=%@,retry count=%d",command.curMeshMessage,command.hadRetryCount);
                    [weakSelf.networkManager sendMeshMessage:message fromElement:source toDestination:destination withTtl:initialTtl usingApplicationKey:applicationKey];
                } else {
                    [weakSelf commandResponseFinishWithCommand:command];
                }
            }];
            command.retryTimer = timer;
        }
    });
    return [[SigMessageHandle alloc] initForMessage:message sentFromSource:source.unicastAddress toDestination:destination.address usingManager:self];
}

- (void)retrySendMessageWithTimer:(NSTimer *)timer {
    
}

//- (SigMessageHandle *)sendMeshMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toGroup:(SigGroupModel *)group withTtl:(UInt8)initialTtl usingApplicationKey:(SigAppkeyModel *)applicationKey command:(SDKLibCommand *)command {
//    SigMeshAddress *destinationAddress = [[SigMeshAddress alloc] initWithAddress:group.intAddress];
//    return [self sendMeshMessage:message fromLocalElement:localElement toDestination:destinationAddress withTtl:initialTtl usingApplicationKey:applicationKey command:command];
//}

//- (SigMessageHandle *)sendMeshMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toModel:(SigModelIDModel *)model withTtl:(UInt8)initialTtl command:(SDKLibCommand *)command{
//    if (!model.parentElement) {
//        TeLogError(@"Element does not belong to a Node.");
//        return nil;
//    }
//    if (model.bind.firstObject == nil || SigDataSource.share == nil || SigDataSource.share.appKeys[model.bind.firstObject.intValue] == nil) {
//        TeLogError(@"Model is not bound to any Application Key.");
//        return nil;
//    }
//    SigMeshAddress *destinationAddress = [[SigMeshAddress alloc] initWithAddress:localElement.unicastAddress];
//    return [self sendMeshMessage:message fromLocalElement:localElement toDestination:destinationAddress withTtl:initialTtl usingApplicationKey:SigDataSource.share.appKeys[model.bind.firstObject.intValue] command:command];
//}

//- (SigMessageHandle *)sendMeshMessage:(SigMeshMessage *)message fromLocalModel:(SigModelIDModel *)localModel toModel:(SigModelIDModel *)model withTtl:(UInt8)initialTtl command:(SDKLibCommand *)command{
//    if (SigDataSource.share == nil) {
//        TeLogError(@"Mesh Network not created");
//        return nil;
//    }
//    if (!model.parentElement) {
//        TeLogError(@"Element does not belong to a Node.");
//        return nil;
//    }
//    if (!localModel.parentElement) {
//        TeLogError(@"Source Model does not belong to an Element.");
//        return nil;
//    }
//    SigElementModel *element = model.parentElement;
//    SigElementModel *localElement = localModel.parentElement;
//    UInt8 commonKeyIndex = 0;
//    BOOL has = NO;
//    for (NSNumber *index in model.bind) {
//        if ([localModel.bind containsObject:index]) {
//            commonKeyIndex = index.intValue;
//            has = YES;
//            break;
//        }
//    }
//    if (NO) {
//        TeLogError(@"Models are not bound to any common Application Key.");
//        return nil;
//    }
//    if (SigDataSource.share == nil || SigDataSource.share.appKeys[commonKeyIndex] == nil) {
//        TeLogError(@"Model is not bound to any Application Key.");
//        return nil;
//    }
//    SigAppkeyModel *applicationKey = SigDataSource.share.appKeys[commonKeyIndex];
//    SigMeshAddress *destinationAddress = [[SigMeshAddress alloc] initWithAddress:element.unicastAddress];
//    return [self sendMeshMessage:message fromLocalElement:localElement toDestination:destinationAddress withTtl:initialTtl usingApplicationKey:applicationKey command:command];
//}

- (SigMessageHandle *)sendConfigMessage:(SigConfigMessage *)message toDestination:(UInt16)destination command:(SDKLibCommand *)command {
    UInt8 ttl = self.dataSource.curLocationNodeModel.defaultTTL;
    if (![SigHelper.share isValidTTL:ttl]) {
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
//    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:destination];
//    if (!node) {
//        TeLogError(@"Unknown destination Node.");
//        return nil;
//    }
//    if (node.netKeys.firstObject == nil) {
//        TeLogError(@"The target Node does not have Network Key.");
//        return nil;
//    }
//    if ([message isMemberOfClass:[SigConfigNetKeyDelete class]]) {
//        if (node.netKeys.count <= 1) {
//            TeLogError(@"Cannot remove last Network Key.");
//            return nil;
//        }
//    }
    if (![SigHelper.share isValidTTL:initialTtl]) {
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
                    [weakSelf commandResponseFinishWithCommand:command];
                }
            }];
            command.retryTimer = timer;
        }
        [weakSelf.networkManager sendConfigMessage:message toDestination:destination withTtl:initialTtl];
    });
    return [[SigMessageHandle alloc] initForMessage:message sentFromSource:source toDestination:destination usingManager:self];
}

//- (SigMessageHandle *)sendConfigMessage:(SigConfigMessage *)message toNode:(SigNodeModel *)node command:(SDKLibCommand *)command {
//    UInt8 ttl = self.dataSource.curLocationNodeModel.defaultTTL;
//    if (![SigHelper.share isValidTTL:ttl]) {
//        ttl = _networkManager.defaultTtl;
//    }
//    return [self sendConfigMessage:message toDestination:node.address withTtl:ttl command:command];
//}

//- (SigMessageHandle *)sendConfigMessage:(SigConfigMessage *)message toNode:(SigNodeModel *)node withTtl:(UInt8)initialTtl command:(SDKLibCommand *)command {
//    return [self sendConfigMessage:message toDestination:node.address withTtl:initialTtl command:command];
//}

//- (SigMessageHandle *)sendConfigMessage:(SigConfigMessage *)message toElement:(SigElementModel *)element withTtl:(UInt8)initialTtl command:(SDKLibCommand *)command {
//    SigNodeModel *node = element.parentNode;
//    if (node == nil) {
//        TeLogError(@"Error: Element does not belong to a Node.");
//        return nil;
//    }
//    return [self sendConfigMessage:message toNode:node withTtl:initialTtl command:command];
//}

//- (SigMessageHandle *)sendConfigMessage:(SigConfigMessage *)message toModel:(SigModelIDModel *)model withTtl:(UInt8)initialTtl command:(SDKLibCommand *)command {
//    SigElementModel *element = model.parentElement;
//    if (!element) {
//        TeLogError(@"Model does not belong to an Element.");
//        return nil;
//    }
//    return [self sendConfigMessage:message toElement:element withTtl:initialTtl command:command];
//}

//- (SigMessageHandle *)sendSigMeshMessage:(SigMeshMessage *)message fromLocalElement:(nullable SigElementModel *)localElement toModelIDModel:(SigModelIDModel *)model command:(SDKLibCommand *)command {
//    UInt8 ttl = self.dataSource.curLocationNodeModel.defaultTTL;
//    if (![SigHelper.share isValidTTL:ttl]) {
//        ttl = _networkManager.defaultTtl;
//    }
//    return [self sendSigMeshMessage:message fromLocalElement:localElement toModelIDModel:model withTtl:ttl command:command];
//}
//
//- (SigMessageHandle *)sendSigMeshMessage:(SigMeshMessage *)message fromLocalElement:(nullable SigElementModel *)localElement toModelIDModel:(SigModelIDModel *)model withTtl:(UInt8)initialTtl command:(SDKLibCommand *)command {
//    SigElementModel *element = model.parentElement;
//    if (!element) {
//        TeLogError(@"Error: Element does not belong to a Node.");
//        return nil;
//    }
//    if (model.bind == nil || model.bind.count == 0) {
//        TeLogError(@"Error: model does not bind to a appkey.");
//        return nil;
//    }
//    UInt16 firstKeyIndex = (UInt16)model.bind.firstObject.intValue;
//    if (!_dataSource) {
//        TeLogError(@"Error: _dataSource is nil.");
//        return nil;
//    }
//    SigAppkeyModel *applicationKey = _dataSource.appKeys[firstKeyIndex];
//    if (!applicationKey) {
//        TeLogError(@"Error: Model is not bound to any Application Key.");
//        return nil;
//    }
//    return [self sendMeshMessage:message fromLocalElement:localElement toDestination:[[SigMeshAddress alloc] initWithAddress:element.unicastAddress] withTtl:initialTtl usingApplicationKey:applicationKey command:command];
//}

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
                    [weakSelf commandResponseFinishWithCommand:command];
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
                    [weakSelf commandResponseFinishWithCommand:command];
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
    if (command.responseAllMessageCallBack || command.resultCallback) {
        //command存储下来，超时或者失败，或者返回response时，从该地方拿到command，获取里面的callback，执行，再删除。
        [self.commands addObject:command];
        TeLogVerbose(@"self.commands=%@",self.commands);
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(commandTimeoutWithCommand:) object:command];
            [self performSelector:@selector(commandTimeoutWithCommand:) withObject:command afterDelay:command.timeout];
        });
    }
}

- (void)commandTimeoutWithCommand:(SDKLibCommand *)command {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(commandTimeoutWithCommand:) object:command];
    });
    if (command.retryCount <= command.reSendCount) {
        //retry finish
        [self.commands removeObject:command];
        if (command.resultCallback) {
            TeLogDebug(@"timeout command:%@",command);
            NSError *error = [NSError errorWithDomain:kSigMeshLibCommandTimeoutErrorMessage code:kSigMeshLibCommandTimeoutErrorCode userInfo:nil];
            command.resultCallback(NO, error);
        }
    } else {
        //has more retry count
        
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
}

- (BOOL)isBusyNow {
    return self.commands.count > 0;
}

#pragma mark - Helper methods for Bearer support

- (void)didDeliverData:(NSData *)data ofType:(SigPduType)type bearer:(SigBearer *)bearer{
    [self bearerDidDeliverData:data type:type];
}

#pragma mark - Save / Load

- (BOOL)load{
    
    return NO;
//    if let data = storage.load() {
//        let decoder = JSONDecoder()
//        decoder.dateDecodingStrategy = .iso8601
//
//        meshData = try decoder.decode(MeshData.self, from: data)
//        guard let network = meshData.meshNetwork else {
//            return false
//        }
//        network.provisioners.forEach {
//            $0.meshNetwork = network
//        }
//        networkManager = NetworkManager(self)
//        return true
//    }
//    return false
}

- (BOOL)save{
    return NO;
//    let encoder = JSONEncoder()
//    encoder.dateEncodingStrategy = .iso8601
//
//    let data = try! encoder.encode(meshData)
//    return storage.save(data)
}

#pragma mark - Export / Import

- (NSData *)exportMesh{
    return nil;
//    let encoder = JSONEncoder()
//    encoder.dateEncodingStrategy = .iso8601
//
//    return try! encoder.encode(meshData.meshNetwork)
}

- (void)importData:(NSData *)data{
//    let decoder = JSONDecoder()
//    decoder.dateDecodingStrategy = .iso8601
//
//    let meshNetwork = try decoder.decode(MeshNetwork.self, from: data)
//    meshNetwork.provisioners.forEach {
//        $0.meshNetwork = meshNetwork
//    }
//
//    meshData.meshNetwork = meshNetwork
//    networkManager = NetworkManager(self)
}

#pragma mark - SigMeshNetworkDelegate

- (void)meshNetworkManager:(SigMeshLib *)manager didReceiveMessage:(SigMeshMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination {
    TeLogInfo(@"didReceiveMessage=%@,message.parameters=%@",message,message.parameters);
    SDKLibCommand *command = [self getCommandWithReceiveMessage:message];
    BOOL shouldCallback = NO;
    if (command && ![command.responseSourceArray containsObject:@(source)]) {
        shouldCallback = YES;
        [command.responseSourceArray addObject:@(source)];
    }
    if (command && command.responseSourceArray.count >= command.responseMaxCount) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [self commandResponseFinishWithCommand:command];
        });
    }
    //update status to SigDataSource before callback.
    [SigDataSource.share updateNodeStatusWithBaseMeshMessage:message source:source];

    //all response message callback in this code.
    if (shouldCallback && command && command.responseAllMessageCallBack) {
        dispatch_async(dispatch_get_main_queue(), ^{
            command.responseAllMessageCallBack(source,destination,message);
        });
    }
    if (shouldCallback && command && command.resultCallback) {
        dispatch_async(dispatch_get_main_queue(), ^{
            command.resultCallback(YES, nil);
        });
    }
}

- (void)meshNetworkManager:(SigMeshLib *)manager didSendMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toDestination:(UInt16)destination {
    TeLogInfo(@"didSendMessage=%@,class=%@",message,message.class);
    SDKLibCommand *command = [self getCommandWithSendMessage:message];
    BOOL shouldCallback = NO;
    if (command && (command.retryCount == 0 || command.responseMaxCount == 0)) {
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

- (void)meshNetworkManager:(SigMeshLib *)manager failedToSendMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toDestination:(UInt16)destination error:(NSError *)error {
    TeLogInfo(@"failedToSendMessage=%@,class=%@",message,message.class);
    SDKLibCommand *command = [self getCommandWithSendMessage:message];
    [_commands removeObject:command];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(commandTimeoutWithCommand:) object:command];
        if (command.resultCallback) {
            command.resultCallback(NO, error);
        }
    });
}

//- (void)newProxyDidConnect {
//    [SDKLibCommand setFilterForProvisioner:self.dataSource.curProvisionerModel successCallback:^(UInt16 source, UInt16 destination, SigFilterStatus * _Nonnull responseMessage) {
//        TeLogDebug(@"setFilterForProvisioner=%@",responseMessage);
//    } failCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
//        TeLogDebug(@"setFilterForProvisioner fail,isResponseAll=%d,error=%@",isResponseAll,error);
//    }];
//}

- (void)meshNetworkManager:(SigMeshLib *)manager didReceiveSigProxyConfigurationMessage:(SigProxyConfigurationMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination {
    TeLogDebug(@"didReceiveSigProxyConfigurationMessage=%@",message);
    SDKLibCommand *command = [self getCommandWithReceiveMessage:(SigMeshMessage *)message];
    [self commandResponseFinishWithCommand:command];

    //callback
    if (command && command.responseFilterStatusCallBack) {
        dispatch_async(dispatch_get_main_queue(), ^{
            command.responseFilterStatusCallBack(source,destination,(SigFilterStatus *)message);
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
            if (((SigIniMeshMessage *)com.curMeshMessage).responseOpCode == message.opCode) {
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
