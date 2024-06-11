/********************************************************************************************************
 * @file     SigMeshLib.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/8/15
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

#import "SigMeshLib.h"
#import "SDKLibCommand.h"
#import "SigLowerTransportLayer.h"

@interface SigMeshLib ()<SigMessageDelegate>
/// The Network Layer handler.
@property (nonatomic,strong) SigNetworkManager *networkManager;
@end

@implementation SigMeshLib

/// Singleton instance
static SigMeshLib *shareLib = nil;

/**
 *  @brief  Singleton method
 *
 *  @return the default singleton instance. You are not allowed to create your own instances of this class.
 */
+ (instancetype)share {
    /// Note: The dispatch_once function can ensure that a certain piece
    /// of code is only executed once in the entire application life cycle!
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        /// Initialize the Singleton configure parameters.
        shareLib = [[SigMeshLib alloc] init];
        shareLib.isReceiveSegmentPDUing = NO;
        shareLib.sourceOfReceiveSegmentPDU = 0;
        shareLib.commands = [NSMutableArray array];
        shareLib.dataSource = SigDataSource.share;
        shareLib.sendBeaconType = AppSendBeaconType_auto;
        [shareLib config];
        [shareLib initDelegate];
    });
    return shareLib;
}

/// Initialize SigMessageDelegate object.
- (void)initDelegate {
    _delegate = shareLib;
}

/// Initialize SigMeshLib object.
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        [self config];
    }
    return self;
}

/// Get Network Manager.
- (SigNetworkManager *)networkManager {
    return SigNetworkManager.share;
}

/// config SigMeshLib default parameters.
- (void)config {
    _defaultTtl = 10;
    _incompleteMessageTimeout = 15.0;
    _receiveSegmentMessageTimeout = 15.0;
    _acknowledgmentTimerInterval = 0.150;
    _transmissionTimerInterval = 0.200;
    _retransmissionLimit = 20;
    _networkTransmitCount = 0b101;
    _networkTransmitIntervalSteps = 0b00010;
    [self updateTheValueOfMaxNetworkTransmitInterval];
    _queue = dispatch_queue_create("SigMeshLib.queue(消息收发队列)", DISPATCH_QUEUE_SERIAL);
    _delegateQueue = dispatch_queue_create("SigMeshLib.delegateQueue", DISPATCH_QUEUE_SERIAL);
}

/// Set Network Transmit Interval Steps.
/// It will calculate _maxNetworkTransmitInterval again.
- (void)setNetworkTransmitIntervalSteps:(UInt8)networkTransmitIntervalSteps {
    if (networkTransmitIntervalSteps > 0b11111) {
        TelinkLogDebug(@"networkTransmitIntervalSteps range:0~0b11111! Set to default value 0b00010.");
        networkTransmitIntervalSteps = 0b00010;
    }
    _networkTransmitIntervalSteps = networkTransmitIntervalSteps;
    [self updateTheValueOfMaxNetworkTransmitInterval];
}

/// Set Network Transmit Count.
/// It will calculate _maxNetworkTransmitInterval again.
- (void)setNetworkTransmitCount:(UInt8)networkTransmitCount {
    if (networkTransmitCount > 0b111) {
        TelinkLogDebug(@"networkTransmitCount range:0~0b111! Set to default value 0b101.");
        _networkTransmitCount = 0b101;
    }
    _networkTransmitCount = networkTransmitCount;
    [self updateTheValueOfMaxNetworkTransmitInterval];
}

/// 4.2.19 NetworkTransmit
/// The Network Transmit state is a composite state that controls the number and timing of the transmissions of Network PDU originating from a node.
/// The state includes a Network Transmit Count field and a Network Transmit Interval Steps field.
/// There is a single instance of this state for the node.
/// maxNetworkTransmitInterval = (networkTransmitIntervalSteps +1+1)*10*(networkTransmitCount+1)
/// default is (0b00010+1+1)*10*(0b101 + 1)=240ms.
/// @note   - seeAlso: Mesh_v1.0.pdf  (page.150),
/// 4.2.19.1 Network Transmit Count.
- (void)updateTheValueOfMaxNetworkTransmitInterval {
    _maxNetworkTransmitInterval = (_networkTransmitIntervalSteps + 1 + 1) * 10 / 1000.0 * (_networkTransmitCount + 1);//单位ms
}

#pragma mark - Receive Mesh Messages

/// This method should be called whenever a PDU has been received from the mesh network using SigBearer. When a complete Mesh Message is received and reassembled, the delegate's `didReceiveMessage:sentFromSource:toDestination:` will be called.
/// @param data The PDU received.
/// @param type The PDU type.
- (void)bearerDidDeliverData:(NSData *)data type:(SigPduType)type {
    if (self.networkManager == nil) {
        TelinkLogDebug(@"self.networkManager == nil");
        return;
    }
    __weak typeof(self) weakSelf = self;
    dispatch_async(_queue, ^{
        [weakSelf.networkManager handleIncomingPdu:data ofType:type];
    });
}

/// This method should be called whenever a PDU has been decoded from the mesh network using SigNetworkLayer.
/// @param networkPdu The network pdu in (Mesh_v1.0.pdf 3.4.4 Network PDU).
- (void)receiveNetworkPdu:(SigNetworkPdu *)networkPdu {
    // 作用：接收到segment pdu时，如果存在应用层的重试，则在该地方修正一下重试定时器的时间。
    // 注意：当前直接callback解密后的networkPdu，方便后期新增一些优化的逻辑代码
    // Function: When receiving segment pdu, if there is a retry at the application layer, correct the retry timer time there.
    // Note: Currently, the decrypted networkPdu is directly called back to facilitate adding some optimized logic code later.
    if (networkPdu.isSegmented) {
        if (_receiveSegmentTimer == nil) {
//            TelinkLogDebug(@"==========RxBusy标志开始");
            __weak typeof(self) weakSelf = self;
            _receiveSegmentTimer = [BackgroundTimer scheduledTimerWithTimeInterval:_receiveSegmentMessageTimeout repeats:NO block:^(BackgroundTimer * _Nonnull t) {
                [weakSelf cleanReceiveSegmentBusyStatus];
            }];
        }
        self.isReceiveSegmentPDUing = networkPdu.isSegmented;
        self.sourceOfReceiveSegmentPDU = networkPdu.source;
        NSLock *lock = [[NSLock alloc] init];
        [lock lock];
        if (self.commands && self.commands.count) {
            SDKLibCommand *command = self.commands.firstObject;
            [self retrySendSDKLibCommand:command];
        }
        [lock unlock];
    }
}

/// Clean busy status of receive segment.
- (void)cleanReceiveSegmentBusyStatus {
//    TelinkLogDebug(@"");
    if (self.isReceiveSegmentPDUing) {
//        TelinkLogDebug(@"==========RxBusy标志清除");
        self.isReceiveSegmentPDUing = NO;
        self.sourceOfReceiveSegmentPDU = 0;
        if (_receiveSegmentTimer) {
            [_receiveSegmentTimer invalidate];
            _receiveSegmentTimer = nil;
        }
        [self.networkManager.lowerTransportLayer.incompleteSegments removeAllObjects];
        [self.networkManager.lowerTransportLayer.acknowledgmentTimers removeAllObjects];
    }
}

/// This method should be called whenever a PDU has been received from the kOnlineStatusCharacteristicsID.
/// @param address address of node
/// @param state state of node
/// @param bright100 bright of node
/// @param temperature100 temperature of node
- (void)updateOnlineStatusWithDeviceAddress:(UInt16)address deviceState:(DeviceState)state bright100:(UInt8)bright100 temperature100:(UInt8)temperature100{
    SigTelinkOnlineStatusMessage *message = [[SigTelinkOnlineStatusMessage alloc] initWithAddress:address state:state brightness:bright100 temperature:temperature100];
    SDKLibCommand *command = [self getCommandWithReceiveMessage:message fromSource:address];
    BOOL shouldCallback = NO;
    if (![command.responseSourceArray containsObject:@(address)]) {
        shouldCallback = YES;
        [command.responseSourceArray addObject:@(address)];
    }
    if (command.responseSourceArray.count >= command.responseMaxCount) {
        [self commandResponseFinishWithCommand:command];
    }
    __weak typeof(self) weakSelf = self;
    //all response message callback in this code.
    if (shouldCallback && command && command.responseAllMessageCallBack) {
        dispatch_async(dispatch_get_main_queue(), ^{
            command.responseAllMessageCallBack(address, weakSelf.dataSource.curLocationNodeModel.address, message);
        });
    }
    //已经废弃：不用通过SigPublishManager的callback上报状态，直接通过didReceiveMessage上报所有状态。
//    if (SigPublishManager.share.discoverOutlineNodeCallback) {
//        dispatch_async(dispatch_get_main_queue(), ^{
//            SigPublishManager.share.discoverOutlineNodeCallback(@(address));
//        });
//    }
    if ([self.delegateForDeveloper respondsToSelector:@selector(didReceiveMessage:sentFromSource:toDestination:)]) {
        [self.delegateForDeveloper didReceiveMessage:message sentFromSource:address toDestination:weakSelf.dataSource.curLocationNodeModel.address];
    }
}

#pragma mark - Send Mesh Messages

/// Sends Configuration Message to the Node with given destination Address.
/// The `destination` must be a Unicast Address, otherwise the method does nothing.
///
/// A `SigMessageDelegate` method will be called when the message has been sent, delivered, or fail to be sent.
///
/// @param message The message to be sent.
/// @param destination The destination Unicast Address.
/// @param initialTtl The initial TTL (Time To Live) value of the message. initialTtl is Relayed TTL.
/// @param command The command save message and callback.
/// @returns Message handle that can be used to cancel sending.
- (nullable SigMessageHandle *)sendConfigMessage:(SigConfigMessage *)message toDestination:(UInt16)destination withTtl:(UInt8)initialTtl command:(SDKLibCommand *)command {
#ifndef TESTMODE
    if (!SigBearer.share.isOpen) {
        TelinkLogError(@"Send fail! Mesh Network is disconnected!");
        // handle for disconnected
        if (command.resultCallback) {
            //handle resultCallback
            command.resultCallback(NO, [NSError errorWithDomain:@"Send fail! Mesh Network is disconnected!" code:-1 userInfo:nil]);
        }
        return nil;
    }
#endif
    if (self.dataSource == nil) {
        TelinkLogError(@"Send fail! Mesh Network not created!");
        // handle for dataSource
        if (command.resultCallback) {
            //handle resultCallback
            command.resultCallback(NO, [NSError errorWithDomain:@"Send fail! Mesh Network not created!" code:-1 userInfo:nil]);
        }
        return nil;
    }
    if (self.dataSource.curLocationNodeModel == nil || self.dataSource.curLocationNodeModel.address == 0) {
        TelinkLogError(@"Send fail! Local Provisioner has no Unicast Address assigned!");
        // handle for curLocationNodeModel
        if (command.resultCallback) {
            //handle resultCallback
            command.resultCallback(NO, [NSError errorWithDomain:@"Send fail! Local Provisioner has no Unicast Address assigned!" code:-1 userInfo:nil]);
        }
        return nil;
    }
    if (![SigHelper.share isUnicastAddress:destination]) {
        TelinkLogError(@"Send fail! Address: 0x%x is not a Unicast Address.",destination);
        // handle for destination
        if (command.resultCallback) {
            //handle resultCallback
            command.resultCallback(NO, [NSError errorWithDomain:[NSString stringWithFormat:@"Send fail! Address: 0x%x is not a Unicast Address.", destination] code:-1 userInfo:nil]);
        }
        return nil;
    }
    if (![SigHelper.share isRelayedTTL:initialTtl]) {
        TelinkLogError(@"Send fail! TTL value %d is invalid.",initialTtl);
        // handle for ttl
        if (command.resultCallback) {
            //handle resultCallback
            command.resultCallback(NO, [NSError errorWithDomain:[NSString stringWithFormat:@"Send fail! TTL value %d is invalid.", initialTtl] code:-1 userInfo:nil]);
        }
        return nil;
    }

    [self handleResponseMaxCommands];
    command.source = self.dataSource.curLocationNodeModel.elements.firstObject;
    command.destination = [[SigMeshAddress alloc] initWithAddress:destination];
    command.initialTtl = initialTtl;
    command.commandType = SigCommandType_configMessage;
    [self addCommandToCacheListWithCommand:command];
    SigMessageHandle *messageHandle = [[SigMessageHandle alloc] initForSDKLibCommand:command usingManager:self];
    command.messageHandle = messageHandle;
    NSLock *lock = [[NSLock alloc] init];
    [lock lock];
    if (self.commands && self.commands.count == 1) {
        __weak typeof(self) weakSelf = self;
        dispatch_async(_queue, ^{
            [weakSelf.networkManager sendConfigMessage:message toDestination:destination withTtl:initialTtl command:command];
        });
    } else {
        TelinkLogInfo(@"The current command has been added to the queue, and there are %d %@ ahead. Please wait until the previous command processing is completed.",self.commands.count-1,self.commands.count-1>1?@"commands":@"command");
    }
    [lock unlock];
    return messageHandle;
}

/// Sends Configuration Message to the Node with given destination Address.
/// The `destination` must be a Unicast Address, otherwise the method does nothing.
///
/// A `SigMessageDelegate` method will be called when the message has been sent, delivered, or fail to be sent.
///
/// @param message The message to be sent.
/// @param destination The destination Unicast Address.
/// @param command The command save message and callback.
/// @returns Message handle that can be used to cancel sending.
- (nullable SigMessageHandle *)sendConfigMessage:(SigConfigMessage *)message toDestination:(UInt16)destination command:(SDKLibCommand *)command {
    UInt8 ttl = self.dataSource.curLocationNodeModel.defaultTTL;
    if (![SigHelper.share isRelayedTTL:ttl]) {
        ttl = self.networkManager.defaultTtl;
    }
    return [self sendConfigMessage:message toDestination:destination withTtl:ttl command:command];
}

/// Encrypts the message with the Application Key and a Network Key bound to it, and sends to the given destination address.
///
/// A `SigMessageDelegate` method will be called when the message has been sent, delivered, or fail to be sent.
///
/// @param message The message to be sent.
/// @param localElement The source Element. If `nil`, the primary Element will be used.
/// The Element must belong to the local Provisioner's Node.
/// @param destination The destination address.
/// @param initialTtl The initial TTL (Time To Live) value of the message. initialTtl is Relayed TTL.
/// @param applicationKey The Application Key to sign the message.
/// @param command The command save message and callback.
/// @returns Message handle that can be used to cancel sending.
- (nullable SigMessageHandle *)sendMeshMessage:(SigMeshMessage *)message fromLocalElement:(nullable SigElementModel *)localElement toDestination:(SigMeshAddress *)destination withTtl:(UInt8)initialTtl usingApplicationKey:(SigAppkeyModel *)applicationKey command:(SDKLibCommand *)command {
#ifndef TESTMODE
    if (!SigBearer.share.isOpen) {
        TelinkLogError(@"Send fail! Mesh Network is disconnected!");
        // handle for disconnected
        if (command.resultCallback) {
            //handle resultCallback
            command.resultCallback(NO, [NSError errorWithDomain:@"Send fail! Mesh Network is disconnected!" code:-1 userInfo:nil]);
        }
        return nil;
    }
#endif

    if (self.networkManager == nil || self.dataSource == nil) {
        TelinkLogError(@"Send fail! Mesh Network not created");
        // handle for dataSource
        if (command.resultCallback) {
            //handle resultCallback
            command.resultCallback(NO, [NSError errorWithDomain:@"Send fail! Mesh Network not created!" code:-1 userInfo:nil]);
        }
        return nil;
    }
    if (self.dataSource.curLocationNodeModel == nil || self.dataSource.curLocationNodeModel.elements.firstObject == nil) {
        TelinkLogError(@"Send fail! Local Provisioner has no Unicast Address assigned.");
        // handle for curLocationNodeModel
        if (command.resultCallback) {
            //handle resultCallback
            command.resultCallback(NO, [NSError errorWithDomain:@"Send fail! Local Provisioner has no Unicast Address assigned!" code:-1 userInfo:nil]);
        }
        return nil;
    }
    SigNodeModel *localNode = self.dataSource.curLocationNodeModel;
    SigElementModel *source = localNode.elements.firstObject;
    if (source.getParentNode != localNode) {
        TelinkLogError(@"Send fail! The Element does not belong to the local Node.");
        // handle for curLocationNodeModel
        if (command.resultCallback) {
            //handle resultCallback
            command.resultCallback(NO, [NSError errorWithDomain:@"Send fail! The Element does not belong to the local Node." code:-1 userInfo:nil]);
        }
        return nil;
    }
    if (![SigHelper.share isRelayedTTL:initialTtl]) {
        TelinkLogError(@"Send fail! TTL value %d is invalid.",initialTtl);
        // handle for ttl
        if (command.resultCallback) {
            //handle resultCallback
            command.resultCallback(NO, [NSError errorWithDomain:[NSString stringWithFormat:@"Send fail! TTL value %d is invalid.", initialTtl] code:-1 userInfo:nil]);
        }
        return nil;
    }

    [self handleResponseMaxCommands];
    command.source = source;
    command.destination = destination;
    command.initialTtl = initialTtl;
    command.curAppkey = applicationKey;
    command.commandType = SigCommandType_meshMessage;
    [self addCommandToCacheListWithCommand:command];
    SigMessageHandle *messageHandle = [[SigMessageHandle alloc] initForSDKLibCommand:command usingManager:self];
    command.messageHandle = messageHandle;
    NSLock *lock = [[NSLock alloc] init];
    [lock lock];
    if (self.commands && self.commands.count == 1) {
        __weak typeof(self) weakSelf = self;
        dispatch_async(_queue, ^{
            [weakSelf.networkManager sendMeshMessage:message fromElement:source toDestination:destination withTtl:initialTtl usingApplicationKey:applicationKey command:command];
        });
    } else {
        TelinkLogInfo(@"The current command has been added to the queue, and there are %d %@ ahead. Please wait until the previous command processing is completed.",self.commands.count-1,self.commands.count-1>1?@"commands":@"command");
    }
    [lock unlock];
    return messageHandle;
}

/// Encrypts the message with the Application Key and a Network Key bound to it, and sends to the given destination address.
///
/// A `SigMessageDelegate` method will be called when the message has been sent, delivered, or fail to be sent.
///
/// @param message The message to be sent.
/// @param localElement The source Element. If `nil`, the primary Element will be used.
/// The Element must belong to the local Provisioner's Node.
/// @param destination The destination address.
/// @param applicationKey The Application Key to sign the message.
/// @param command The command save message and callback.
/// @returns Message handle that can be used to cancel sending.
- (nullable SigMessageHandle *)sendMeshMessage:(SigMeshMessage *)message fromLocalElement:(nullable SigElementModel *)localElement toDestination:(SigMeshAddress *)destination usingApplicationKey:(SigAppkeyModel *)applicationKey command:(SDKLibCommand *)command {
    UInt8 ttl = localElement.getParentNode.defaultTTL;
    if (![SigHelper.share isRelayedTTL:ttl]) {
        ttl = self.networkManager.defaultTtl;
    }
    return [self sendMeshMessage:message fromLocalElement:localElement toDestination:destination withTtl:ttl usingApplicationKey:applicationKey command:command];
}

/// Sends the Proxy Configuration Message to the connected Proxy Node.
/// @param message The Proxy Configuration message to be sent.
/// @param command The command save message and callback.
- (nullable SigMessageHandle *)sendSigProxyConfigurationMessage:(SigProxyConfigurationMessage *)message command:(SDKLibCommand *)command {
#ifndef TESTMODE
    if (!SigBearer.share.isOpen) {
        TelinkLogError(@"Send fail! Mesh Network is disconnected!");
        // handle for disconnected
        if (command.resultCallback) {
            //handle resultCallback
            command.resultCallback(NO, [NSError errorWithDomain:@"Send fail! Mesh Network is disconnected!" code:-1 userInfo:nil]);
        }
        return nil;
    }
#endif
    if (self.dataSource == nil) {
        TelinkLogError(@"Send fail! Mesh Network not created");
        // handle for dataSource
        if (command.resultCallback) {
            //handle resultCallback
            command.resultCallback(NO, [NSError errorWithDomain:@"Send fail! Mesh Network not created!" code:-1 userInfo:nil]);
        }
        return nil;
    }

    [self handleResponseMaxCommands];
    command.commandType = SigCommandType_proxyConfigurationMessage;
    [self addCommandToCacheListWithCommand:command];
    SigMessageHandle *messageHandle = [[SigMessageHandle alloc] initForSDKLibCommand:command usingManager:self];
    command.messageHandle = messageHandle;
    NSLock *lock = [[NSLock alloc] init];
    [lock lock];
    if (self.commands && self.commands.count == 1) {
        __weak typeof(self) weakSelf = self;
        dispatch_async(_queue, ^{
            [weakSelf.networkManager sendSigProxyConfigurationMessage:message];
        });
    } else {
        TelinkLogInfo(@"The current command has been added to the queue, and there are %d %@ ahead. Please wait until the previous command processing is completed.",self.commands.count-1,self.commands.count-1>1?@"commands":@"command");
    }
    [lock unlock];
    return messageHandle;
}

/// Advertising Solicitation PDU With Mesh Message.
/// @param source The source Unicast Address.
/// @param destination The destination address of the message received.
/// @param interval advertising interval
/// @param result advertising result
- (void)advertisingSolicitationPDUWithSource:(UInt16)source destination:(UInt16)destination advertisingInterval:(NSTimeInterval)interval result:(advertisingResult)result {
    SigNetkeyModel *networkKey = SigDataSource.share.curNetkeyModel;
    // Get the current sequence number for local Provisioner's source address.
    UInt32 sequence = (UInt32)[SigMeshLib.share.dataSource getLocalSolicitationSequenceNumber];
    // As the sequence number was just used, it has to be incremented.
    [SigMeshLib.share.dataSource saveLocalSolicitationSequenceNumber:sequence+1];
    SigIvIndex *index = [[SigIvIndex alloc] initWithIndex:0 updateActive:NO];
//    SigNetworkPdu *networkPdu = [[SigNetworkPdu alloc] initWithEncodeSolicitationPDU:message.accessPdu networkKey:networkKey source:source destination:destination withSequence:sequence ivIndex:index];
    SigNetworkPdu *networkPdu = [[SigNetworkPdu alloc] initWithEncodeSolicitationPDU:[NSData data] networkKey:networkKey source:source destination:destination withSequence:sequence ivIndex:index];
    NSMutableData *mData = [NSMutableData data];
    UInt8 serviceDataAdvertisingFlag = 0x16;//AD type
    [mData appendBytes:&serviceDataAdvertisingFlag length:1];
    UInt16 serviceUUID = 0x1859;//uuid
    [mData appendBytes:&serviceUUID length:2];
    //Proxy Solicitation with Replay Protection type
    UInt8 identificationType = 0;
    [mData appendBytes:&identificationType length:1];
    //Format determined by Identification Type field
    NSData *identificationParameters = networkPdu.pduData;
    [mData appendData:identificationParameters];
    [TPeripheralManager.share advertisingManufacturerData:mData advertisingInterval:interval result:result];
}

- (void)stopAdvertising {
    
}

/// Sends the telink's onlineStatus command.
/// @param message The onlineStatus message to be sent.
/// @param command The command save message and callback.
/// @returns return `nil` when send message successful.
- (nullable NSError *)sendTelinkApiGetOnlineStatueFromUUIDWithMessage:(SigMeshMessage *)message command:(SDKLibCommand *)command {
#ifndef TESTMODE
    if (!SigBearer.share.isOpen) {
        TelinkLogError(@"Send fail! Mesh Network is disconnected!");
        return [NSError errorWithDomain:kSigMeshLibCommandMeshDisconnectedErrorMessage code:kSigMeshLibCommandMeshDisconnectedErrorCode userInfo:nil];
    }
#endif
    if (self.dataSource == nil) {
        TelinkLogError(@"Send fail! Mesh Network not created");
        return [NSError errorWithDomain:kSigMeshLibNoCreateMeshNetworkErrorMessage code:kSigMeshLibNoCreateMeshNetworkErrorCode userInfo:nil];
    }

    [self handleResponseMaxCommands];
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
                    TelinkLogDebug(@"command.curMeshMessage=%@,retry count=%d",command.curMeshMessage,command.hadRetryCount);
                    [SigBearer.share.getCurrentPeripheral writeValue:data forCharacteristic:onlineStatusCharacteristic type:CBCharacteristicWriteWithResponse];
                } else {
                    [weakSelf commandTimeoutWithCommand:command];
                }
            }];
            command.retryTimer = timer;
        }
        return nil;
    }else{
        return [NSError errorWithDomain:kSigMeshLibNoFoundOnlineStatusCharacteristicErrorMessage code:kSigMeshLibNoFoundOnlineStatusCharacteristicErrorCode userInfo:nil];
    }
}

/// Cancels sending the message with the given identifier.
/// @param messageId The message identifier.
- (void)cancelSigMessageHandle:(SigMessageHandle *)messageId {
    if (self.networkManager == nil) {
        TelinkLogError(@"Send fail! Error: Mesh Network not created.");
        return;
    }
    SDKLibCommand *command = [self getCommandWithSendMessageOpCode:messageId.opCode];
    [self commandResponseFinishWithCommand:command];
    if (command.resultCallback) {
        command.resultCallback(NO, [NSError errorWithDomain:@"cancel message!" code:-1 userInfo:nil]);
    }
}

/// cancel all commands and retry of commands and retry of segment PDU.
- (void)cleanAllCommandsAndRetry {
    TelinkLogDebug(@"清除commands cache.");
    NSLock *lock = [[NSLock alloc] init];
    [lock lock];
    NSArray *commands = [NSArray arrayWithArray:_commands];
    for (SDKLibCommand *com in commands) {
        [com.messageHandle cancel];
        [self.commands removeObject:com];
    }
    [lock unlock];
    [self cleanReceiveSegmentBusyStatus];
}

/// cancel all commands and retry of commands and retry of segment PDU when mesh disconnected.
- (void)cleanAllCommandsAndRetryWhenMeshDisconnected {
    NSLock *lock = [[NSLock alloc] init];
    [lock lock];
    NSArray *commands = [NSArray arrayWithArray:_commands];
    for (SDKLibCommand *com in commands) {
        [com.messageHandle cancel];
        [self.commands removeObject:com];
        if (com.resultCallback) {
            NSError *error = [NSError errorWithDomain:@"Mesh is disconnected!" code:-1 userInfo:nil];
            com.resultCallback(NO, error);
        }
    }
    [lock unlock];
}

/// Returns whether SigMeshLib is busy. YES means busy.
- (BOOL)isBusyNow {
    BOOL busy = ![self hadSendCommandsFinish];
    dispatch_async(dispatch_get_main_queue(), ^{
        [[NSNotificationCenter defaultCenter] postNotificationName:kNotifyCommandIsBusyOrNot object:nil userInfo:@{kCommandIsBusyKey : @(busy)}];
    });
    return busy;
}


/// Add command to cache list `self.commands`
/// - Parameter command: The SDKLibCommand object.
- (void)addCommandToCacheListWithCommand:(SDKLibCommand *)command {
    float oldTimeout = command.timeout;
    float newTimeout = [self getReliableIntervalWithDestination:command.destination.address responseMaxCount:command.responseMaxCount];
    command.timeout = MAX(oldTimeout, newTimeout);
    //command存储下来，超时或者失败，或者返回response时，从该地方拿到command，获取里面的callback，执行，再删除。
    NSLock *lock = [[NSLock alloc] init];
    [lock lock];
    [self.commands addObject:command];
    TelinkLogInfo(@"add command:%@,source=0x%X,destination=0x%X,retryCount=%d,responseMax=%d,timeout=%f,_commands.count = %d", command.curMeshMessage, command.source.unicastAddress, command.destination.address, command.retryCount, command.responseMaxCount, command.timeout, self.commands.count);
    [lock unlock];
//    //存在response的指令需存储
//    if (command.responseAllMessageCallBack || command.resultCallback || (command.retryCount > 0 && command.responseMaxCount > 0)) {
//        float oldTimeout = command.timeout;
//        float newTimeout = [self getReliableIntervalWithDestination:command.destination.address responseMaxCount:command.responseMaxCount];
//        command.timeout = MAX(oldTimeout, newTimeout);
//        //command存储下来，超时或者失败，或者返回response时，从该地方拿到command，获取里面的callback，执行，再删除。
//        [self.commands addObject:command];
//    }
}


/// Handle command timeout action.
/// - Parameter command: The SDKLibCommand object.
- (void)commandTimeoutWithCommand:(SDKLibCommand *)command {
    [self commandResponseFinishWithCommand:command];
    TelinkLogDebug(@"timeout command:%@-%@",command.curMeshMessage,command.curMeshMessage.parameters);
    NSError *error = [NSError errorWithDomain:kSigMeshLibCommandTimeoutErrorMessage code:kSigMeshLibCommandTimeoutErrorCode userInfo:nil];
    [self handleResultCallback:command error:error];

//    if (command.resultCallback && !command.hadReceiveAllResponse) {
//        TelinkLogDebug(@"timeout command:%@-%@",command.curMeshMessage,command.curMeshMessage.parameters);
//        NSError *error = [NSError errorWithDomain:kSigMeshLibCommandTimeoutErrorMessage code:kSigMeshLibCommandTimeoutErrorCode userInfo:nil];
//        command.resultCallback(NO, error);
//    }
}

/// Handle command finish action.
/// - Parameter command: The SDKLibCommand object.
- (void)commandResponseFinishWithCommand:(SDKLibCommand *)command {
    if (command.retryTimer) {
        [command.retryTimer invalidate];
        command.retryTimer = nil;
    }
    command.retryCount = 0;
    __weak typeof(self) weakSelf = self;
    if (command.commandType == SigCommandType_meshMessage || command.commandType == SigCommandType_configMessage) {
        dispatch_async(_queue, ^{
            if (command && command.messageHandle) {
                [weakSelf.networkManager cancelSigMessageHandle:command.messageHandle];
            }
        });
    }
    NSLock *lock = [[NSLock alloc] init];
    [lock lock];
    [self.commands removeObject:command];
    [lock unlock];
    if ([self hadSendCommandsFinish]) {
        [self isBusyNow];
    }
}

/// Remote the commands that had receive all response message from the `self.commands`.
- (void)handleResponseMaxCommands {
    NSLock *lock = [[NSLock alloc] init];
    [lock lock];
    NSArray *commands = [NSArray arrayWithArray:_commands];
    for (SDKLibCommand *com in commands) {
        if (com.responseMaxCount == 0 || com.responseMaxCount == 0xFF || com.hadReceiveAllResponse) {
            [self.commands removeObject:com];
        }
    }
    [lock unlock];
}

/// Determine whether all the command in the cache list have been sent?
- (BOOL)hadSendCommandsFinish {
    NSLock *lock = [[NSLock alloc] init];
    [lock lock];
    BOOL tem = YES;
    NSArray *commands = [NSArray arrayWithArray:_commands];
    for (SDKLibCommand *com in commands) {
        if (!com.hadReceiveAllResponse) {
            tem = NO;
            break;
        }
    }
    [lock unlock];
    return tem;
}

/// Recalculate the ReliableInterval based on the destination address and responseMaxCount.
/// - Parameters:
///   - destination: An address may be a unicast address, a virtual address, or a group address.
///   There is also a special value to represent an unassigned address that is not used in messages.
///   - responseMaxCount: The count of response message should response in this command.
- (float)getReliableIntervalWithDestination:(UInt16)destination responseMaxCount:(NSInteger)responseMaxCount {
    int multiple = 1;
    if (self.dataSource.defaultUnsegmentedMessageLowerTransportPDUMaxLength > kUnsegmentedMessageLowerTransportPDUMaxLength) {
        multiple = 2;
    }
    if (destination == kMeshAddress_allNodes) {
        unsigned long maxNum = MAX(responseMaxCount, self.dataSource.curNodes.count);
        if (maxNum <= 50) {
            return 2.0 * multiple;
        } else if (maxNum <= 100) {
            return 3.0 * multiple;
        } else if (maxNum <= 150) {
            return 4.0 * multiple;
        } else {
            return 6.0 * multiple;
        }
    } else {
        if ([SigHelper.share isUnicastAddress:destination]) {
            SigNodeModel *node = [SigDataSource.share getNodeWithAddress:destination];
            if (node.features.lowPowerFeature != SigNodeFeaturesState_notSupported) {
                //LPN节点，需要修正有效重试间隔。
                return self.dataSource.defaultReliableIntervalOfLPN;
            }
        }

        if (responseMaxCount < 10) {
            return kCMDInterval * 4 * multiple;
        } else if (responseMaxCount <= 50) {
            return 2.0 * multiple;
        } else if (responseMaxCount <= 100) {
            return 3.0 * multiple;
        } else if (responseMaxCount <= 150) {
            return 4.0 * multiple;
        } else {
            return 6.0 * multiple;
        }
    }
}

#pragma mark - Helper methods for Bearer support

/// This method should be called whenever a PDU has been received from the mesh network using SigBearer. When a complete Mesh Message is received and reassembled, the delegate's `didReceiveMessage:sentFromSource:toDestination:` will be called.
/// @param data The PDU received.
/// @param type The PDU type.
- (void)didDeliverData:(NSData *)data ofType:(SigPduType)type bearer:(SigBearer *)bearer{
    [self bearerDidDeliverData:data type:type];
}

#pragma mark - SigMessageDelegate

/// A callback called whenever a Mesh Message has been received from the mesh network.
/// @param message The received message.
/// @param source The Unicast Address of the Element from which the message was sent.
/// @param destination The address to which the message was sent.
- (void)didReceiveMessage:(SigMeshMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination {
    TelinkLogInfo(@"didReceiveMessage=%@,message.parameters=%@,source=0x%x,destination=0x%x", message, message.parameters, source,destination);
    SigNodeModel *node = [self.dataSource getNodeWithAddress:source];

    //根据设备是否打开了publish功能来判断是否给该设备添加监测离线的定时器。
    if (message.opCode == SigOpCode_lightCTLStatus || message.opCode == SigOpCode_lightHSLStatus || message.opCode == SigOpCode_lightLightnessStatus || message.opCode == SigOpCode_genericOnOffStatus) {
        if (node && node.hasOpenPublish && !node.isSensor) {
            [SigPublishManager.share startCheckOfflineTimerWithAddress:@(source)];
        }
    }

    //update status to SigDataSource before callback.
    [self.dataSource updateNodeStatusWithBaseMeshMessage:message source:source];
    //非直连设备断电，再上电后设备端会主动上报0x824E。
    if (message.opCode == SigOpCode_lightLightnessStatus) {
        if (SigPublishManager.share.discoverOnlineNodeCallback) {
            SigPublishManager.share.discoverOnlineNodeCallback(@(source));
        }
    }
    SDKLibCommand *command = [self getCommandWithReceiveMessage:message fromSource:(UInt16)source];
    //filter command = nil
    if (command == nil) {
        return;
    }
    BOOL shouldCallback = NO;
    if (command && ![command.responseSourceArray containsObject:@(source)]) {
        shouldCallback = YES;
        //node may delete from SigDataSource, but no reset from mesh network.
        if (node) {
            [command.responseSourceArray addObject:@(source)];
        } else if (message.opCode > 0xFFFF) {//vendor model
            [command.responseSourceArray addObject:@(source)];
        }
    }

    //all response message callback in this code.
    if (shouldCallback && command && command.responseAllMessageCallBack) {
        dispatch_async(dispatch_get_main_queue(), ^{
            command.responseAllMessageCallBack(source, destination, message);
        });
    }
    if (command.responseMaxCount != 0) {
//        if (command && command.responseSourceArray.count >= command.responseMaxCount) {
        //优化：当实际回调的response多于传入的responseMaxCount时，使用==判断即可实现只回调一次resultCallback。
        if (command && command.responseSourceArray.count == command.responseMaxCount) {
            [self commandResponseFinishWithCommand:command];
            dispatch_async(dispatch_get_main_queue(), ^{
                [self handleResultCallback:command error:nil];
//                if (command.resultCallback) {
//                    command.resultCallback(YES, nil);
//                }
            });
        }
    } else {
        //command.responseMaxCount = 0的command已经在didSend回调处处理了resultCallback，该处无需重复处理。
        command.hadReceiveAllResponse = YES;
        if (command.retryTimer) {
            [command.retryTimer invalidate];
            command.retryTimer = nil;
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
    TelinkLogInfo(@"didSendMessage=%@,class=%@,source=0x%x,destination=0x%x", message, message.class, localElement.unicastAddress, destination);
    SDKLibCommand *command = [self getCommandWithSendMessage:message];
    //filter command = nil
    if (command == nil) {
        return;
    }
    if (command.retryCount > 0 || (command.retryCount == 0 && command.responseMaxCount > 0)) {
        // 需要重试或者等待timeout
        [self retrySendSDKLibCommand:command];
    } else {
        // 无需重试，返回发送成功。
        BOOL shouldCallback = NO;
        if (command && (command.retryCount == 0 && command.responseMaxCount == 0)) {
            shouldCallback = YES;
        }
        if (command && destination == localElement.unicastAddress) {
            shouldCallback = YES;
        }
        if (command && shouldCallback) {
//            dispatch_async(dispatch_get_main_queue(), ^{
                [self commandResponseFinishWithCommand:command];
//            });
        }

        //send finished of noAckMessage callback in this code.
        if (shouldCallback) {
            [self handleResultCallback:command error:nil];
        }
    }
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
    TelinkLogInfo(@"failedToSendMessage=%@,class=%@,source=0x%x,destination=0x%x", message, message.class, localElement.unicastAddress, destination);
    SDKLibCommand *command = [self getCommandWithSendMessage:message];
    //filter command = nil
    if (command == nil) {
        return;
    }
    if (command.retryCount > 0) {
        // 需要重试
        [self retrySendSDKLibCommand:command];
    } else {
        // 无需重试，返回发送成功。
        command.hadReceiveAllResponse = YES;
        [self handleResultCallback:command error:error];
    }
}

/// A callback called whenever a SigProxyConfiguration Message has been received from the mesh network.
/// @param message The received message.
/// @param source The Unicast Address of the Element from which the message was sent.
/// @param destination The address to which the message was sent.
- (void)didReceiveSigProxyConfigurationMessage:(SigProxyConfigurationMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination {
    TelinkLogInfo(@"didReceiveSigProxyConfigurationMessage=%@,message.parameters=%@,source=0x%x,destination=0x%x", message, message.parameters, source,destination);
    SDKLibCommand *command = [self getCommandWithReceiveMessage:(SigMeshMessage *)message fromSource:source];
    //filter command = nil
    if (command == nil) {
        return;
    }
    [self commandResponseFinishWithCommand:command];

    //callback
    if (command && command.responseAllMessageCallBack) {
        command.responseAllMessageCallBack(source, destination, (SigMeshMessage *)message);
    }
    if (command && command.resultCallback) {
        command.resultCallback(YES, nil);
    }

//    if (command && command.responseFilterStatusCallBack) {
//        command.responseFilterStatusCallBack(source,destination,(SigFilterStatus *)message);
//    }
    //没有必要再此处处理，因为理论不应该出现发送ProxyConfiguration时指令列表里面还有其它指令。
//    [self handleResultCallback:command error:nil];
}

#pragma mark - Private

/// This instruction has been processed and will be removed from the cached instruction list and the next instruction will be sent.
/// @param command The SDKLibCommand object.
/// @param error The error of this command, error != nil means this command had trigger any error.
- (void)handleResultCallback:(SDKLibCommand *)command error:(NSError *)error {
    NSLock *lock = [[NSLock alloc] init];
    [lock lock];
    [self.commands removeObject:command];
    BOOL hasNextCommand = NO;
    if (self.commands && self.commands.count > 0) {
        hasNextCommand = YES;
    }
    if (command && command.resultCallback) {
        dispatch_async(dispatch_get_main_queue(), ^{
            if (error) {
                command.resultCallback(NO, error);
            } else {
                command.resultCallback(YES, nil);
            }
        });
    }
    if (hasNextCommand) {
        SDKLibCommand *nextCommand = self.commands.firstObject;
        __weak typeof(self) weakSelf = self;
        if (nextCommand.commandType == SigCommandType_meshMessage) {
            dispatch_async(_queue, ^{
                [weakSelf.networkManager sendMeshMessage:(SigMeshMessage *)nextCommand.curMeshMessage fromElement:nextCommand.source toDestination:nextCommand.destination withTtl:nextCommand.initialTtl usingApplicationKey:nextCommand.curAppkey command:nextCommand];
            });
        } else if (nextCommand.commandType == SigCommandType_configMessage) {
            dispatch_async(_queue, ^{
                [weakSelf.networkManager sendConfigMessage:(SigConfigMessage *)nextCommand.curMeshMessage toDestination:nextCommand.destination.address withTtl:nextCommand.initialTtl command:nextCommand];
            });
        } else if (nextCommand.commandType == SigCommandType_proxyConfigurationMessage) {
            dispatch_async(_queue, ^{
                [weakSelf.networkManager sendSigProxyConfigurationMessage:(SigProxyConfigurationMessage *)nextCommand.curMeshMessage];
            });
        }
    }
    [lock unlock];
}

/// Get SDKLibCommand object by receive message and source address.
/// @param message The message received.
/// @param source source address of received message.
- (SDKLibCommand *)getCommandWithReceiveMessage:(SigMeshMessage *)message fromSource:(UInt16)source {
    NSLock *lock = [[NSLock alloc] init];
    [lock lock];
    SDKLibCommand *tem = nil;
    if ([message isKindOfClass:[SigConfigMessage class]]) {
        NSArray *commands = [NSArray arrayWithArray:_commands];
        for (SDKLibCommand *com in commands) {
            if ((((SigConfigMessage *)com.curMeshMessage).responseOpCode == message.opCode) && (![SigHelper.share isUnicastAddress:com.destination.address] || ([SigHelper.share isUnicastAddress:com.destination.address] && com.destination.address == source))) {
                tem = com;
                break;
            }
        }
    } else if ([message isKindOfClass:[SigGenericMessage class]]) {
        NSArray *commands = [NSArray arrayWithArray:_commands];
        for (SDKLibCommand *com in commands) {
            if ([SigHelper.share isAcknowledgedMessage:(SigMeshMessage *)com.curMeshMessage] && ((SigAcknowledgedGenericMessage *)com.curMeshMessage).responseOpCode == message.opCode  && (![SigHelper.share isUnicastAddress:com.destination.address] || ([SigHelper.share isUnicastAddress:com.destination.address] && com.destination.address == source))) {
                tem = com;
                break;
            }
        }
    } else if ([message isKindOfClass:[SigStaticProxyConfigurationMessage class]]) {//处理SigStaticProxyConfigurationMessage：包括SigFilterStatus和SigDirectedProxyCapabilitiesStatus
        NSArray *commands = [NSArray arrayWithArray:_commands];
        for (SDKLibCommand *com in commands) {
            if ([com.curMeshMessage isKindOfClass:[SigStaticAcknowledgedProxyConfigurationMessage class]]) {
                if (((SigStaticAcknowledgedProxyConfigurationMessage *)com.curMeshMessage).responseOpCode == message.opCode  && (![SigHelper.share isUnicastAddress:com.destination.address] || ([SigHelper.share isUnicastAddress:com.destination.address] && com.destination.address == source))) {
                    tem = com;
                    break;
                }
            }
        }
    } else if ([message isKindOfClass:[SigTelinkOnlineStatusMessage class]]){//私有定制onlineStatus回包
        NSArray *commands = [NSArray arrayWithArray:_commands];
        for (SDKLibCommand *com in commands) {
            if ([com.curMeshMessage isMemberOfClass:[SigGenericOnOffGet class]]) {
                tem = com;
                break;
            }
        }
    }
    if (tem == nil) {
        //未定义的vendor回包 或者 使用SigIniMeshMessage的方式发送sig message。
        NSArray *commands = [NSArray arrayWithArray:_commands];
        for (SDKLibCommand *com in commands) {
            //存在responseOpCode的message类型
            if ([com.curMeshMessage isKindOfClass:[SigIniMeshMessage class]] || [com.curMeshMessage isKindOfClass:[SigAcknowledgedMeshMessage class]]) {
                if ((((SigIniMeshMessage *)com.curMeshMessage).responseOpCode == message.opCode || ((SigIniMeshMessage *)com.curMeshMessage).responseOpCode == ((message.opCode >> 16) & 0xFF))  && (![SigHelper.share isUnicastAddress:com.destination.address] || ([SigHelper.share isUnicastAddress:com.destination.address] && com.destination.address == source))) {
                    tem = com;
                    break;
                }
            }
        }
    }

    [lock unlock];
    return tem;
}

/// Get SDKLibCommand object by send message.
/// @param message The message sent.
- (SDKLibCommand *)getCommandWithSendMessage:(SigMeshMessage *)message {
    NSLock *lock = [[NSLock alloc] init];
    [lock lock];
    SDKLibCommand *tem = nil;
    if ([message isKindOfClass:[SigConfigMessage class]]) {
        NSArray *commands = [NSArray arrayWithArray:_commands];
        for (SDKLibCommand *com in commands) {
            if (((SigConfigMessage *)com.curMeshMessage).opCode == message.opCode) {
                tem = com;
                break;
            }
        }
    } else if ([message isKindOfClass:[SigAcknowledgedGenericMessage class]]) {
        NSArray *commands = [NSArray arrayWithArray:_commands];
        for (SDKLibCommand *com in commands) {
            if (((SigAcknowledgedGenericMessage *)com.curMeshMessage).opCode == message.opCode) {
                tem = com;
                break;
            }
        }
    } else if ([message isKindOfClass:[SigIniMeshMessage class]]) {
        NSArray *commands = [NSArray arrayWithArray:_commands];
        for (SDKLibCommand *com in commands) {
            if (((SigIniMeshMessage *)com.curMeshMessage).opCode == message.opCode) {
                tem = com;
                break;
            }
        }
    } else if ([message isKindOfClass:[SigMeshMessage class]]) {
           NSArray *commands = [NSArray arrayWithArray:_commands];
           for (SDKLibCommand *com in commands) {
               if (((SigMeshMessage *)com.curMeshMessage).opCode == message.opCode) {
                   tem = com;
                   break;
               }
           }
    } else if ([message isKindOfClass:[SigProxyConfigurationMessage class]]) {
              NSArray *commands = [NSArray arrayWithArray:_commands];
              for (SDKLibCommand *com in commands) {
                  if (((SigProxyConfigurationMessage *)com.curMeshMessage).opCode == message.opCode) {
                      tem = com;
                      break;
                  }
              }
       }
    [lock unlock];
    return tem;
}

/// Get SDKLibCommand object by opCode of send message.
/// @param sendOpCode opCode of send message.
- (SDKLibCommand *)getCommandWithSendMessageOpCode:(UInt32)sendOpCode {
    NSLock *lock = [[NSLock alloc] init];
    [lock lock];
    SDKLibCommand *tem = nil;
    NSArray *commands = [NSArray arrayWithArray:_commands];
    for (SDKLibCommand *com in commands) {
        if ([com.curMeshMessage isKindOfClass:[SigMeshMessage class]]) {
            if (((SigMeshMessage *)com.curMeshMessage).opCode == sendOpCode) {
                tem = com;
                break;
            }
        }
    }
    [lock unlock];
    return tem;
}

/// Set a timeout BackgroundTimer or retry BackgroundTimer to this command.
/// @param command The SDKLibCommand object.
- (void)retrySendSDKLibCommand:(SDKLibCommand *)command {
    __weak typeof(self) weakSelf = self;
    if (command && command.retryTimer) {
        [command.retryTimer invalidate];
        command.retryTimer = nil;
    }
    if (command.hadRetryCount >= command.retryCount) {
        // 重试完成，一个command.timeout没有足够response则报超时。
        BackgroundTimer *timer = [BackgroundTimer scheduledTimerWithTimeInterval:command.timeout repeats:NO block:^(BackgroundTimer * _Nonnull t) {
            [weakSelf commandTimeoutWithCommand:command];
        }];
        command.retryTimer = timer;
    } else {
        // 重试未完成，继续重试。
        BackgroundTimer *timer = [BackgroundTimer scheduledTimerWithTimeInterval:command.timeout repeats:NO block:^(BackgroundTimer * _Nonnull t) {
            if (command.hadRetryCount < command.retryCount) {
                command.hadRetryCount ++;
                TelinkLogDebug(@"command.curMeshMessage=%@,retry count=%d",command.curMeshMessage,command.hadRetryCount);
                dispatch_async(weakSelf.queue, ^{
                    [weakSelf.networkManager cancelSigMessageHandle:command.messageHandle];
                    if (command.commandType == SigCommandType_meshMessage) {
                        [weakSelf.networkManager sendMeshMessage:(SigMeshMessage *)command.curMeshMessage fromElement:command.source toDestination:command.destination withTtl:command.initialTtl usingApplicationKey:command.curAppkey command:command];
                    } else if (command.commandType == SigCommandType_configMessage) {
                        [weakSelf.networkManager sendConfigMessage:(SigConfigMessage *)command.curMeshMessage toDestination:command.destination.address withTtl:command.initialTtl command:command];
                    } else if (command.commandType == SigCommandType_proxyConfigurationMessage) {
                        [weakSelf.networkManager sendSigProxyConfigurationMessage:(SigProxyConfigurationMessage *)command.curMeshMessage];
                    }
                });
            } else {
                TelinkLogError(@"retry error!");
            }
        }];
        command.retryTimer = timer;
    }
}

@end
