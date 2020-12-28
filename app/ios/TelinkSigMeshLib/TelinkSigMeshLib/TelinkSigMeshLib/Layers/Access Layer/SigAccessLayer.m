/********************************************************************************************************
 * @file     SigAccessLayer.m
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
//  SigAccessLayer.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigAccessLayer.h"
#import "SigAccessPdu.h"
#import "SigUpperTransportLayer.h"
#import "SigLowerTransportLayer.h"

/// The transaction object is used for Transaction Messages,
/// for example `GenericLevelSet`.
@interface SigTransaction : NSObject
/// Last used Transaction Identifier.
@property (nonatomic,assign) UInt8 lastTid;
/// The timestamp of the last transaction message sent.
@property (nonatomic,strong) NSDate *timestamp;
@end
@implementation SigTransaction
/// Returns the last used TID.
- (instancetype)init{
    if (self = [super init]) {
        _lastTid = arc4random()%(0xff+1);
    }
    return self;
}
- (UInt8)currentTid {
    _timestamp = [NSDate date];
    return _lastTid;
}
/// Returns the next TID.
- (UInt8)nextTid {
    if (_lastTid < 255) {
        _lastTid = _lastTid + 1;
    } else {
        _lastTid = 0;
    }
    _timestamp = [NSDate date];
    return _lastTid;
}
/// Whether the transaction can be continued.
- (BOOL)isActive {
    // A transaction may last up to 6 seconds.
    return _timestamp.timeIntervalSinceNow > -6.0;
}
@end

@interface SigAcknowledgmentContext : NSObject
@property (nonatomic,strong) SigAcknowledgedMeshMessage *request;
@property (nonatomic,assign) UInt16 source;
@property (nonatomic,assign) UInt16 destination;
@property (nonatomic,strong) BackgroundTimer *timeoutTimer;
@property (nonatomic,strong,nullable) BackgroundTimer *retryTimer;
@end
@implementation SigAcknowledgmentContext
- (instancetype)initForRequest:(SigAcknowledgedMeshMessage *)request sentFromSource:(UInt16)source toDestination:(UInt16)destination repeatAfterDelay:(NSTimeInterval)delay repeatBlock:(void (^ _Nonnull)(void))repeatBlock timeout:(NSTimeInterval)timeout timeoutBlock:(void (^ _Nonnull)(void))timeoutBlock {
    if (self = [super init]) {
        _request = request;
        _source = source;
        _destination = destination;
        __weak typeof(self) weakSelf = self;
        _timeoutTimer = [BackgroundTimer scheduledTimerWithTimeInterval:timeout repeats:NO block:^(BackgroundTimer * _Nonnull t) {
            [weakSelf invalidate];
            if (timeoutBlock) {
                timeoutBlock();
            }
        }];
        [self initializeRetryTimerWithDelay:delay callback:repeatBlock];
    }
    return self;
}

/// Invalidates the timers.
- (void)invalidate {
//    if (_timeoutTimer) {
//        [_timeoutTimer invalidate];
//    }
    if (_retryTimer) {
        [_retryTimer invalidate];
        _retryTimer = nil;
    }
}

- (void)initializeRetryTimerWithDelay:(NSTimeInterval)delay callback:(void (^ _Nonnull)(void))callback {
    __weak typeof(self) weakSelf = self;
    if (_retryTimer) {
        [_retryTimer invalidate];
        _retryTimer = nil;
    }
    _retryTimer = [BackgroundTimer scheduledTimerWithTimeInterval:delay repeats:NO block:^(BackgroundTimer * _Nonnull t) {
        if (weakSelf.retryTimer) {
            if (callback) {
                callback();
            }
            [weakSelf initializeRetryTimerWithDelay:t.interval * 2 callback:callback];
        } else {
            return;
        }
    }];
}

@end

@interface SigAccessLayer ()
@property (nonatomic,strong) SigNetworkManager *networkManager;
/// A map of current transactions.
///
/// The key is a value combined from the source and destination addresses.
@property (nonatomic,strong) NSMutableDictionary <NSNumber *,SigTransaction *>*transactions;
/// This array contains information about the expected acknowledgments
/// for acknowledged mesh messages that have been sent, and for which
/// the response has not been received yet.
@property (nonatomic,strong) NSMutableArray <SigAcknowledgmentContext *>*reliableMessageContexts;
@end

@implementation SigAccessLayer

- (instancetype)init {
    if (self = [super init]) {
        _transactions = [NSMutableDictionary dictionary];
        _reliableMessageContexts = [NSMutableArray array];
    }
    return self;
}

- (instancetype)initWithNetworkManager:(SigNetworkManager *)networkManager {
    if (self = [super init]) {
        _networkManager = networkManager;
        _transactions = [NSMutableDictionary dictionary];
        _reliableMessageContexts = [NSMutableArray array];
    }
    return self;
}

- (void)dealloc {
    TeLogWarn(@"_reliableMessageContexts=%@",_reliableMessageContexts);
    [_transactions removeAllObjects];
    NSArray *reliableMessageContexts = [NSArray arrayWithArray:_reliableMessageContexts];
    for (SigAcknowledgmentContext *model in reliableMessageContexts) {
        [model invalidate];
    }
    [_reliableMessageContexts removeAllObjects];
}

- (void)handleUpperTransportPdu:(SigUpperTransportPdu *)upperTransportPdu sentWithSigKeySet:(SigKeySet *)keySet {
    SigAccessPdu *accessPdu = [[SigAccessPdu alloc] initFromUpperTransportPdu:upperTransportPdu];
    if (accessPdu == nil) {
        TeLogError(@"handleUpperTransportPdu fail.");
        return;
    }
    // If a response to a sent request has been received, cancel the context.
    SigAcknowledgedMeshMessage *request = nil;
    NSInteger index = 0;
    BOOL exist = NO;
    for (int i=0; i<_reliableMessageContexts.count; i++) {
        SigAcknowledgmentContext *model = _reliableMessageContexts[i];
        if (model.source == upperTransportPdu.destination && model.request.responseOpCode == accessPdu.opCode) {
            index = i;
            exist = YES;
            break;
        }
    }
    if ([SigHelper.share isUnicastAddress:upperTransportPdu.destination] && exist) {
        SigAcknowledgmentContext *context = _reliableMessageContexts[index];
        request = context.request;
        [context invalidate];
        [_reliableMessageContexts removeObjectAtIndex:index];
    }
    TeLogInfo(@"receieved:%@",accessPdu);
    [self handleAccessPdu:accessPdu sendWithSigKeySet:keySet asResponseToRequest:request];
}

- (void)sendMessage:(SigMeshMessage *)message fromElement:(SigElementModel *)element toDestination:(SigMeshAddress *)destination withTtl:(UInt8)initialTtl usingApplicationKey:(SigAppkeyModel *)applicationKey command:(SDKLibCommand *)command {
    // Should the TID be updated?
    SigMeshMessage *m = message;
    if ([message isKindOfClass:[SigGenericMessage class]]) {
        SigGenericMessage *genericMessage = (SigGenericMessage *)message;
        if (command.needTid && command.tid != 0) {
            genericMessage.tid = command.tid;
        }
        if ([genericMessage isTransactionMessage] && genericMessage.tid == 0) {
            // Ensure there is a transaction for our destination.
            UInt32 k = [self getKeyForElement:element andDestination:destination];
            _transactions[@(k)] = _transactions[@(k)] == nil ? [[SigTransaction alloc] init] : _transactions[@(k)];
            // Should the last transaction be continued?
            if (genericMessage.continueTransaction && [_transactions[@(k)] isActive]) {
                genericMessage.tid = [_transactions[@(k)] currentTid];
            } else {
                // If not, start a new transaction by setting a new TID value.
                genericMessage.tid = [_transactions[@(k)] nextTid];
            }
            m = genericMessage;
        }
//        TeLogVerbose(@"sending message TID=0x%x",genericMessage.tid);
    }
    SigAccessPdu *pdu = [[SigAccessPdu alloc] initFromMeshMessage:m sentFromLocalElement:element toDestination:destination userInitiated:YES];
    SigAccessKeySet *keySet = [[SigAccessKeySet alloc] initWithApplicationKey:applicationKey];
    TeLogInfo(@"Sending message:%@->%@",message.class,pdu);
    
    // Set timers for the acknowledged messages.
//    #warning 2019年11月10日23:44:04，类型可能不太匹配，待完善
//    if ([message isKindOfClass:[SigAcknowledgedMeshMessage class]]) {
//        [self createReliableContextForSigAccessPdu:pdu sentFromElement:element withTtl:initialTtl usingKeySet:keySet];
//    }else{
//        TeLogDebug(@"非SigAcknowledgedMeshMessage子类。");
//    }
//    if ([SigHelper.share getResponseOpcodeWithSendOpcode:message.opCode] != 0 || [message isKindOfClass:[SigAcknowledgedMeshMessage class]]) {
    
    //==========telink retry by retry count in SigMeshLib==========//
//    if ([SigHelper.share isAcknowledgedMessage:message]) {
//        [self createReliableContextForSigAccessPdu:pdu sentFromElement:element withTtl:initialTtl usingKeySet:keySet];
//    }
    //==========telink retry by retry count in SigMeshLib==========//
    [_networkManager.upperTransportLayer sendAccessPdu:pdu withTtl:initialTtl usingKeySet:keySet command:command];
}

- (void)sendMessage:(SigMeshMessage *)message fromElement:(SigElementModel *)element toDestination:(SigMeshAddress *)destination withTtl:(UInt8)initialTtl usingApplicationKey:(SigAppkeyModel *)applicationKey {
    // Should the TID be updated?
    SigMeshMessage *m = message;
    if ([message isKindOfClass:[SigGenericMessage class]]) {
        SigGenericMessage *genericMessage = (SigGenericMessage *)message;
        if ([genericMessage isTransactionMessage] && genericMessage.tid == 0) {
            // Ensure there is a transaction for our destination.
            UInt32 k = [self getKeyForElement:element andDestination:destination];
            _transactions[@(k)] = _transactions[@(k)] == nil ? [[SigTransaction alloc] init] : _transactions[@(k)];
            // Should the last transaction be continued?
            if (genericMessage.continueTransaction && [_transactions[@(k)] isActive]) {
                genericMessage.tid = [_transactions[@(k)] currentTid];
            } else {
                // If not, start a new transaction by setting a new TID value.
                genericMessage.tid = [_transactions[@(k)] nextTid];
            }
            m = genericMessage;
        }
//        TeLogVerbose(@"sending message TID=0x%x",genericMessage.tid);
    }
    TeLogVerbose(@"Sending %@ from: %@, to: 0x%x",m,element,destination.address);
    SigAccessPdu *pdu = [[SigAccessPdu alloc] initFromMeshMessage:m sentFromLocalElement:element toDestination:destination userInitiated:YES];
    SigAccessKeySet *keySet = [[SigAccessKeySet alloc] initWithApplicationKey:applicationKey];
    TeLogInfo(@"Sending %@",pdu);
    
    // Set timers for the acknowledged messages.
//    #warning 2019年11月10日23:44:04，类型可能不太匹配，待完善
//    if ([message isKindOfClass:[SigAcknowledgedMeshMessage class]]) {
//        [self createReliableContextForSigAccessPdu:pdu sentFromElement:element withTtl:initialTtl usingKeySet:keySet];
//    }else{
//        TeLogDebug(@"非SigAcknowledgedMeshMessage子类。");
//    }
//    if ([SigHelper.share getResponseOpcodeWithSendOpcode:message.opCode] != 0 || [message isKindOfClass:[SigAcknowledgedMeshMessage class]]) {
    
    //==========telink retry by retry count in SigMeshLib==========//
//    if ([SigHelper.share isAcknowledgedMessage:message]) {
//        [self createReliableContextForSigAccessPdu:pdu sentFromElement:element withTtl:initialTtl usingKeySet:keySet];
//    }
    //==========telink retry by retry count in SigMeshLib==========//
    [_networkManager.upperTransportLayer sendAccessPdu:pdu withTtl:initialTtl usingKeySet:keySet];
}

- (void)sendSigConfigMessage:(SigConfigMessage *)message toDestination:(UInt16)destination withTtl:(UInt16)initialTtl {
    SigElementModel *element = SigDataSource.share.curLocationNodeModel.elements.firstObject;
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:destination];
    SigNetkeyModel *networkKey = node.getNetworkKeys.firstObject;
    if (networkKey == nil) {
        networkKey = SigDataSource.share.curNetkeyModel;
    }
    // ConfigNetKeyDelete must not be signed using the key that is being deleted.
    if ([message isMemberOfClass:[SigConfigNetKeyDelete class]]) {
        SigConfigNetKeyDelete *netKeyDelete = (SigConfigNetKeyDelete *)message;
        if (netKeyDelete.networkKeyIndex == networkKey.index) {
            networkKey = node.getNetworkKeys.lastObject;
        }
    }
    SigMeshAddress *meshAddress = [[SigMeshAddress alloc] initWithAddress:destination];
    SigAccessPdu *pdu = [[SigAccessPdu alloc] initFromMeshMessage:message sentFromLocalElement:element toDestination:meshAddress userInitiated:YES];
    TeLogInfo(@"Sending %@ %@",message,pdu);
    SigDeviceKeySet *keySet = [[SigDeviceKeySet alloc] initWithNetworkKey:networkKey node:node];
    
    // Set timers for the acknowledged messages.
    //==========telink retry by retry count in SigMeshLib==========//
//    if ([SigHelper.share isAcknowledgedMessage:message]) {
//        [self createReliableContextForSigAccessPdu:pdu sentFromElement:element withTtl:initialTtl usingKeySet:keySet];
//    } else {
//        TeLogDebug(@"unAcknowledged Message.");
//    }
    //==========telink retry by retry count in SigMeshLib==========//

//#warning 2019年11月10日23:44:04，类型可能不太匹配，待完善（SigConfigCompositionDataGet已经匹配）
//    if ([message isKindOfClass:[SigConfigMessage class]]) {
////        if ([message isMemberOfClass:[SigAcknowledgedConfigMessage class]]) {
//        [self createReliableContextForSigAccessPdu:pdu sentFromElement:element withTtl:initialTtl usingKeySet:keySet];
//    }else{
//        TeLogDebug(@"非SigConfigMessage子类。");
//    }
    [_networkManager.upperTransportLayer sendAccessPdu:pdu withTtl:initialTtl usingKeySet:keySet];
}

- (void)replyToMessageSentToOrigin:(UInt16)origin withMeshMessage:(SigMeshMessage *)message fromElement:(SigElementModel *)element toDestination:(UInt16)destination usingKeySet:(SigKeySet *)keySet {
    TeLogInfo(@"Replying with %@ from: %@, to: 0x%x",message,element,destination);
    SigMeshAddress *meshAddress = [[SigMeshAddress alloc] initWithAddress:destination];
    SigAccessPdu *pdu = [[SigAccessPdu alloc] initFromMeshMessage:message sentFromLocalElement:element toDestination:meshAddress userInitiated:NO];
    
    // If the message is sent in response to a received message that was sent to
    // a Unicast Address, the node should transmit the response message with a random
    // delay between 20 and 50 milliseconds. If the message is sent in response to a
    // received message that was sent to a group address or a virtual address, the node
    // should transmit the response message with a random delay between 20 and 500
    // milliseconds. This reduces the probability of multiple nodes responding to this
    // message at exactly the same time, and therefore increases the probability of
    // message delivery rather than message collisions.
    float delay = [SigHelper.share isUnicastAddress:origin] ? [SigHelper.share getRandomfromA:0.020 toB:0.050] : [SigHelper.share getRandomfromA:0.020 toB:0.500];
    __weak typeof(self) weakSelf = self;
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delay * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        TeLogInfo(@"Sending %@",pdu);
        UInt8 ttl = element.getParentNode.defaultTTL;
        if (![SigHelper.share isRelayedTTL:ttl]) {
            ttl = weakSelf.networkManager.defaultTtl;
        }
        [weakSelf.networkManager.upperTransportLayer sendAccessPdu:pdu withTtl:ttl usingKeySet:keySet];
    });
//    [BackgroundTimer scheduledTimerWithTimeInterval:delay repeats:NO block:^(BackgroundTimer * _Nonnull t) {
//        TeLogInfo(@"Sending %@",pdu);
//        [weakSelf.networkManager.upperTransportLayer sendAccessPdu:pdu withTtl:nil usingKeySet:keySet];
//    }];
}

- (void)cancelSigMessageHandle:(SigMessageHandle *)handle {
    TeLogInfo(@"Cancelling messages with op code:0x%x, sent from:0x%x to:0x%x",(unsigned int)handle.opCode,handle.source,handle.destination);
    NSArray *reliableMessageContexts = [NSArray arrayWithArray:_reliableMessageContexts];
    for (SigAcknowledgmentContext *model in reliableMessageContexts) {
        if (model.request.opCode == handle.opCode && model.source == handle.source &&
        model.destination == handle.destination) {
            [model invalidate];
            [_reliableMessageContexts removeObject:model];
            break;
        }
    }
    [_networkManager.upperTransportLayer cancelHandleSigMessageHandle:handle];
    [_networkManager.lowerTransportLayer cancelTXSendingSegmentedWithDestination:handle.destination];
}

- (void)handleAccessPdu:(SigAccessPdu *)accessPdu sendWithSigKeySet:(SigKeySet *)keySet asResponseToRequest:(SigAcknowledgedMeshMessage *)request {
    SigNodeModel *localNode = SigDataSource.share.curLocationNodeModel;
    if (localNode == nil) {
        TeLogError(@"localNode error.");
        return;
    }
    
    SigMeshMessage *receiveMessage = [self decodeSigAccessPdu:accessPdu];
    if (receiveMessage == nil) {
        SigUnknownMessage *unknownMessage = [[SigUnknownMessage alloc] initWithParameters:accessPdu.parameters];
        unknownMessage.opCode = accessPdu.opCode;
        receiveMessage = unknownMessage;
    }
    [_networkManager notifyAboutNewMessage:receiveMessage fromSource:accessPdu.source toDestination:accessPdu.destination.address];
}

- (UInt32)getKeyForElement:(SigElementModel *)element andDestination:(SigMeshAddress *)destination {
    return (UInt32)((element.unicastAddress) << 16) | (UInt32)(destination.address);
}

- (void)removeAllTimeoutTimerInreliableMessageContexts {
    TeLogInfo(@"============9.3.AccessError.timeout");
    NSArray *reliableMessageContexts = [NSArray arrayWithArray:_reliableMessageContexts];
    for (SigAcknowledgmentContext *context in reliableMessageContexts) {
        if (context.timeoutTimer == nil) {
            [_reliableMessageContexts removeObject:context];
        }
    }
}



//- (void)handleUpperTransportPdu:(SigUpperTransportPdu *)upperTransportPdu {
//    SigAccessPdu *accessPdu = [[SigAccessPdu alloc] initFromUpperTransportPdu:upperTransportPdu];
//    if (accessPdu == nil) {
//        TeLogError(@"handleUpperTransportPdu fail.");
//        return;
//    }
//    [self handleAccessPdu:accessPdu];
//}

//- (void)sendMessage:(SigMeshMessage *)message toDestination:(UInt16)address applicationKey:(SigAppkeyModel *)applicationKey {
//    SigMeshMessage *m = message;
//    SigTransactionMessage *tranactionMessage = (SigTransactionMessage *)message;
//    if ([tranactionMessage isMemberOfClass:[SigTransactionMessage class]] && tranactionMessage.tid == 0) {
//        tranactionMessage.tid = _tid;
//        // Increase the TID to the next value modulo 255.
//        if (_tid < 255) {
//            _tid ++;
//        } else {
//            _tid = 0;
//        }
//        m = tranactionMessage;
//    }
//    TeLogInfo(@"Sending data:%@ to:ox%x",message.parameters,address);
//    [_networkManager.upperTransportLayer sendMeshMessage:m toDestination:address usingApplicationKey:applicationKey];
//}

//- (void)sendMessage:(SigConfigMessage *)message toDestination:(UInt16)destination {
//    if (![SigHelper.share isUnicastAddress:destination]) {
//        TeLogError(@"Address: 0x%x is not a Unicast Address.",destination);
//        return;
//    }
//    if ([_networkManager.foundationLayer handleConfigMessage:message toDestination:destination]) {
//        TeLogInfo(@"Sending data:%@ to ox%x",message.parameters,destination);
//        [_networkManager.upperTransportLayer sendConfigMessage:message toDestination:destination];
//    }
//}

- (Class)getMeshMessageWithOpCode:(SigOpCode)opCode {
    Class messageType = nil;
    TeLogVerbose(@"解析opCode=0x%08x",(unsigned int)opCode);
    if ((opCode & 0xC00000) == 0xC00000) {
        // Vendor Messages
//        MessageType = networkManager.meshNetworkManager.vendorTypes[opCode] ?? UnknownMessage.self
    } else {
        switch (opCode) {
                // Composition Data
            case SigOpCode_configCompositionDataGet:
                messageType = [SigConfigCompositionDataGet class];
                break;
            case SigOpCode_configCompositionDataStatus:
                messageType = [SigConfigCompositionDataStatus class];
                break;
                // Secure Network Beacon configuration
            case SigOpCode_configBeaconGet:
                messageType = [SigConfigBeaconGet class];
                break;
            case SigOpCode_configBeaconSet:
                messageType = [SigConfigBeaconSet class];
                break;
            case SigOpCode_configBeaconStatus:
                messageType = [SigConfigBeaconStatus class];
                break;
                // Relay configuration
            case SigOpCode_configRelayGet:
                messageType = [SigConfigRelayGet class];
                break;
            case SigOpCode_configRelaySet:
                messageType = [SigConfigRelaySet class];
                break;
            case SigOpCode_configRelayStatus:
                messageType = [SigConfigRelayStatus class];
                break;
                // GATT Proxy configuration
            case SigOpCode_configGATTProxyGet:
                messageType = [SigConfigGATTProxyGet class];
                break;
            case SigOpCode_configGATTProxySet:
                messageType = [SigConfigGATTProxySet class];
                break;
            case SigOpCode_configGATTProxyStatus:
                messageType = [SigConfigGATTProxyStatus class];
                break;
                // Key Refresh Phase
            case SigOpCode_configKeyRefreshPhaseGet:
                messageType = [SigConfigKeyRefreshPhaseGet class];
                break;
            case SigOpCode_configKeyRefreshPhaseSet:
                messageType = [SigConfigKeyRefreshPhaseSet class];
                break;
            case SigOpCode_configKeyRefreshPhaseStatus:
                messageType = [SigConfigKeyRefreshPhaseStatus class];
                break;
                // Friend configuration
            case SigOpCode_configFriendGet:
                messageType = [SigConfigFriendGet class];
                break;
            case SigOpCode_configFriendSet:
                messageType = [SigConfigFriendSet class];
                break;
            case SigOpCode_configFriendStatus:
                messageType = [SigConfigFriendStatus class];
                break;
                // Network Transmit configuration
            case SigOpCode_configNetworkTransmitGet:
                messageType = [SigConfigNetworkTransmitGet class];
                break;
            case SigOpCode_configNetworkTransmitSet:
                messageType = [SigConfigNetworkTransmitSet class];
                break;
            case SigOpCode_configNetworkTransmitStatus:
                messageType = [SigConfigNetworkTransmitStatus class];
                break;
                // Network Keys Management
            case SigOpCode_configNetKeyAdd:
                messageType = [SigConfigNetKeyAdd class];
                break;
            case SigOpCode_configNetKeyDelete:
                messageType = [SigConfigNetKeyDelete class];
                break;
            case SigOpCode_configNetKeyUpdate:
                messageType = [SigConfigNetKeyUpdate class];
                break;
            case SigOpCode_configNetKeyStatus:
                messageType = [SigConfigNetKeyStatus class];
                break;
            case SigOpCode_configNetKeyGet:
                messageType = [SigConfigNetKeyGet class];
                break;
            case SigOpCode_configNetKeyList:
                messageType = [SigConfigNetKeyList class];
                break;
                // App Keys Management
            case SigOpCode_configAppKeyAdd:
                messageType = [SigConfigAppKeyAdd class];
                break;
            case SigOpCode_configAppKeyDelete:
                messageType = [SigConfigAppKeyDelete class];
                break;
            case SigOpCode_configAppKeyUpdate:
                messageType = [SigConfigAppKeyUpdate class];
                break;
            case SigOpCode_configAppKeyStatus:
                messageType = [SigConfigAppKeyStatus class];
                break;
            case SigOpCode_configAppKeyGet:
                messageType = [SigConfigAppKeyGet class];
                break;
            case SigOpCode_configAppKeyList:
                messageType = [SigConfigAppKeyList class];
                break;
                // Model Bindings
            case SigOpCode_configModelAppBind:
                messageType = [SigConfigModelAppBind class];
                break;
            case SigOpCode_configModelAppUnbind:
                messageType = [SigConfigModelAppUnbind class];
                break;
            case SigOpCode_configModelAppStatus:
                messageType = [SigConfigModelAppStatus class];
                break;
            case SigOpCode_configSIGModelAppGet:
                messageType = [SigConfigSIGModelAppGet class];
                break;
            case SigOpCode_configSIGModelAppList:
                messageType = [SigConfigAppKeyGet class];
                break;
            case SigOpCode_configVendorModelAppGet:
                messageType = [SigConfigVendorModelAppGet class];
                break;
            case SigOpCode_configVendorModelAppList:
                messageType = [SigConfigVendorModelAppList class];
                break;
                // Publications
            case SigOpCode_configModelPublicationGet:
                messageType = [SigConfigModelPublicationGet class];
                break;
            case SigOpCode_configModelPublicationSet:
                messageType = [SigConfigModelPublicationSet class];
                break;
            case SigOpCode_configModelPublicationVirtualAddressSet:
                messageType = [SigConfigModelPublicationVirtualAddressSet class];
                break;
            case SigOpCode_configModelPublicationStatus:
                messageType = [SigConfigModelPublicationStatus class];
                break;
                // Subscriptions
            case SigOpCode_configModelSubscriptionAdd:
                messageType = [SigConfigModelSubscriptionAdd class];
                break;
            case SigOpCode_configModelSubscriptionDelete:
                messageType = [SigConfigModelSubscriptionDelete class];
                break;
            case SigOpCode_configModelSubscriptionDeleteAll:
                messageType = [SigConfigModelSubscriptionDeleteAll class];
                break;
            case SigOpCode_configModelSubscriptionOverwrite:
                messageType = [SigConfigModelSubscriptionOverwrite class];
                break;
            case SigOpCode_configModelSubscriptionStatus:
                messageType = [SigConfigModelSubscriptionStatus class];
                break;
            case SigOpCode_configModelSubscriptionVirtualAddressAdd:
                messageType = [SigConfigModelSubscriptionVirtualAddressAdd class];
                break;
            case SigOpCode_configModelSubscriptionVirtualAddressDelete:
                messageType = [SigConfigModelSubscriptionVirtualAddressDelete class];
                break;
            case SigOpCode_configModelSubscriptionVirtualAddressOverwrite:
                messageType = [SigConfigModelSubscriptionVirtualAddressOverwrite class];
                break;
            case SigOpCode_configSIGModelSubscriptionGet:
                messageType = [SigConfigSIGModelSubscriptionGet class];
                break;
            case SigOpCode_configSIGModelSubscriptionList:
                messageType = [SigConfigSIGModelSubscriptionList class];
                break;
            case SigOpCode_configVendorModelSubscriptionGet:
                messageType = [SigConfigVendorModelSubscriptionGet class];
                break;
            case SigOpCode_configVendorModelSubscriptionList:
                messageType = [SigConfigVendorModelSubscriptionList class];
                break;
                
                // Low Power Node Poll Timeout
            case SigOpCode_configLowPowerNodePollTimeoutGet:
                messageType = [SigConfigLowPowerNodePollTimeoutGet class];
                break;
            case SigOpCode_configLowPowerNodePollTimeoutStatus:
                messageType = [SigConfigLowPowerNodePollTimeoutStatus class];
                break;

                // Heartbeat Publication
            case SigOpCode_configHeartbeatPublicationGet:
                messageType = [SigConfigHeartbeatPublicationGet class];
                break;
            case SigOpCode_configHeartbeatPublicationSet:
                messageType = [SigConfigHeartbeatPublicationSet class];
                break;
            case SigOpCode_configHeartbeatPublicationStatus:
                messageType = [SigConfigHeartbeatPublicationStatus class];
                break;
                // node identity
            case SigOpCode_configHeartbeatSubscriptionGet:
                messageType = [SigConfigHeartbeatSubscriptionGet class];
                break;
            case SigOpCode_configHeartbeatSubscriptionSet:
                messageType = [SigConfigHeartbeatSubscriptionSet class];
                break;
            case SigOpCode_configHeartbeatSubscriptionStatus:
                messageType = [SigConfigHeartbeatSubscriptionStatus class];
                break;

                // node identity
            case SigOpCode_configNodeIdentityGet:
                messageType = [SigConfigNodeIdentityGet class];
                break;
            case SigOpCode_configNodeIdentitySet:
                messageType = [SigConfigNodeIdentitySet class];
                break;
            case SigOpCode_configNodeIdentityStatus:
                messageType = [SigConfigNodeIdentityStatus class];
                break;

                // Resetting Node
            case SigOpCode_configNodeReset:
                messageType = [SigConfigNodeReset class];
                break;
            case SigOpCode_configNodeResetStatus:
                messageType = [SigConfigNodeResetStatus class];
                break;
                // Default TTL
            case SigOpCode_configDefaultTtlGet:
                messageType = [SigConfigDefaultTtlGet class];
                break;
            case SigOpCode_configDefaultTtlSet:
                messageType = [SigConfigDefaultTtlSet class];
                break;
            case SigOpCode_configDefaultTtlStatus:
                messageType = [SigConfigDefaultTtlStatus class];
                break;
                // remote provision
            case SigOpCode_remoteProvisioningScanCapabilitiesGet:
                messageType = [SigRemoteProvisioningScanCapabilitiesGet class];
                break;
            case SigOpCode_remoteProvisioningScanCapabilitiesStatus:
                messageType = [SigRemoteProvisioningScanCapabilitiesStatus class];
                break;
            case SigOpCode_remoteProvisioningScanGet:
                messageType = [SigRemoteProvisioningScanGet class];
                break;
            case SigOpCode_remoteProvisioningScanStart:
                messageType = [SigRemoteProvisioningScanStart class];
                break;
            case SigOpCode_remoteProvisioningScanStop:
                messageType = [SigRemoteProvisioningScanStop class];
                break;
            case SigOpCode_remoteProvisioningScanStatus:
                messageType = [SigRemoteProvisioningScanStatus class];
                break;
            case SigOpCode_remoteProvisioningScanReport:
                messageType = [SigRemoteProvisioningScanReport class];
                break;
            case SigOpCode_remoteProvisioningExtendedScanStart:
                messageType = [SigRemoteProvisioningExtendedScanStart class];
                break;
            case SigOpCode_remoteProvisioningExtendedScanReport:
                messageType = [SigRemoteProvisioningExtendedScanReport class];
                break;
            case SigOpCode_remoteProvisioningLinkGet:
                messageType = [SigRemoteProvisioningLinkGet class];
                break;
            case SigOpCode_remoteProvisioningLinkOpen:
                messageType = [SigRemoteProvisioningLinkOpen class];
                break;
            case SigOpCode_remoteProvisioningLinkClose:
                messageType = [SigRemoteProvisioningLinkClose class];
                break;
            case SigOpCode_remoteProvisioningLinkStatus:
                messageType = [SigRemoteProvisioningLinkStatus class];
                break;
            case SigOpCode_remoteProvisioningLinkReport:
                messageType = [SigRemoteProvisioningLinkReport class];
                break;
            case SigOpCode_remoteProvisioningPDUSend:
                messageType = [SigRemoteProvisioningPDUSend class];
                break;
            case SigOpCode_remoteProvisioningPDUOutboundReport:
                messageType = [SigRemoteProvisioningPDUOutboundReport class];
                break;
            case SigOpCode_remoteProvisioningPDUReport:
                messageType = [SigRemoteProvisioningPDUReport class];
                break;
                // Generics
            case SigOpCode_genericOnOffGet:
                messageType = [SigGenericOnOffGet class];
                break;
            case SigOpCode_genericOnOffSet:
                messageType = [SigGenericOnOffSet class];
                break;
            case SigOpCode_genericOnOffSetUnacknowledged:
                messageType = [SigGenericOnOffSetUnacknowledged class];
                break;
            case SigOpCode_genericOnOffStatus:
                messageType = [SigGenericOnOffStatus class];
                break;
            case SigOpCode_genericLevelGet:
                messageType = [SigGenericLevelGet class];
                break;
            case SigOpCode_genericLevelSet:
                messageType = [SigGenericLevelSet class];
                break;
            case SigOpCode_genericLevelSetUnacknowledged:
                messageType = [SigGenericLevelSetUnacknowledged class];
                break;
            case SigOpCode_genericLevelStatus:
                messageType = [SigGenericLevelStatus class];
                break;
            case SigOpCode_genericDeltaSet:
                messageType = [SigGenericDeltaSet class];
                break;
            case SigOpCode_genericDeltaSetUnacknowledged:
                messageType = [SigGenericDeltaSetUnacknowledged class];
                break;
            case SigOpCode_genericMoveSet:
                messageType = [SigGenericMoveSet class];
                break;
            case SigOpCode_genericMoveSetUnacknowledged:
                messageType = [SigGenericMoveSetUnacknowledged class];
                break;
            case SigOpCode_genericDefaultTransitionTimeGet:
                messageType = [SigGenericDefaultTransitionTimeGet class];
                break;
            case SigOpCode_genericDefaultTransitionTimeSet:
                messageType = [SigGenericDefaultTransitionTimeSet class];
                break;
            case SigOpCode_genericDefaultTransitionTimeSetUnacknowledged:
                messageType = [SigGenericDefaultTransitionTimeSetUnacknowledged class];
                break;
            case SigOpCode_genericDefaultTransitionTimeStatus:
                messageType = [SigGenericDefaultTransitionTimeStatus class];
                break;
            case SigOpCode_genericOnPowerUpGet:
                messageType = [SigGenericOnPowerUpGet class];
                break;
            case SigOpCode_genericOnPowerUpSet:
                messageType = [SigGenericOnPowerUpSet class];
                break;
            case SigOpCode_genericOnPowerUpSetUnacknowledged:
                messageType = [SigGenericOnPowerUpSetUnacknowledged class];
                break;
            case SigOpCode_genericOnPowerUpStatus:
                messageType = [SigGenericOnPowerUpStatus class];
                break;
            case SigOpCode_genericPowerLevelGet:
                messageType = [SigGenericPowerLevelGet class];
                break;
            case SigOpCode_genericPowerLevelSet:
                messageType = [SigGenericPowerLevelSet class];
                break;
            case SigOpCode_genericPowerLevelSetUnacknowledged:
                messageType = [SigGenericPowerLevelSetUnacknowledged class];
                break;
            case SigOpCode_genericPowerLevelStatus:
                messageType = [SigGenericPowerLevelStatus class];
                break;
            case SigOpCode_genericPowerLastGet:
                messageType = [SigGenericPowerLastGet class];
                break;
            case SigOpCode_genericPowerLastStatus:
                messageType = [SigGenericPowerLastStatus class];
                break;
            case SigOpCode_genericPowerDefaultGet:
                messageType = [SigGenericPowerDefaultGet class];
                break;
            case SigOpCode_genericPowerDefaultStatus:
                messageType = [SigGenericPowerDefaultStatus class];
                break;
            case SigOpCode_genericPowerRangeGet:
                messageType = [SigGenericPowerRangeGet class];
                break;
            case SigOpCode_genericPowerRangeStatus:
                messageType = [SigGenericPowerRangeStatus class];
                break;
            case SigOpCode_genericPowerDefaultSet:
                messageType = [SigGenericPowerDefaultSet class];
                break;
            case SigOpCode_genericPowerDefaultSetUnacknowledged:
                messageType = [SigGenericPowerDefaultSetUnacknowledged class];
                break;
            case SigOpCode_genericPowerRangeSet:
                messageType = [SigGenericPowerRangeSet class];
                break;
            case SigOpCode_genericPowerRangeSetUnacknowledged:
                messageType = [SigGenericPowerRangeSetUnacknowledged class];
                break;
            case SigOpCode_genericBatteryGet:
                messageType = [SigGenericBatteryGet class];
                break;
            case SigOpCode_genericBatteryStatus:
                messageType = [SigGenericBatteryStatus class];
                break;
                //Sensor
            case SigOpCode_sensorDescriptorGet:
                messageType = [SigSensorDescriptorGet class];
                break;
            case SigOpCode_sensorDescriptorStatus:
                messageType = [SigSensorDescriptorStatus class];
                break;
            case SigOpCode_sensorGet:
                messageType = [SigSensorGet class];
                break;
            case SigOpCode_sensorStatus:
                messageType = [SigSensorStatus class];
                break;
            case SigOpCode_sensorColumnGet:
                messageType = [SigSensorColumnGet class];
                break;
            case SigOpCode_sensorColumnStatus:
                messageType = [SigSensorColumnStatus class];
                break;
            case SigOpCode_sensorSeriesGet:
                messageType = [SigSensorSeriesGet class];
                break;
            case SigOpCode_sensorSeriesStatus:
                messageType = [SigSensorSeriesStatus class];
                break;
                
                //Sensor Setup
            case SigOpCode_sensorCadenceGet:
                messageType = [SigSensorCadenceGet class];
                break;
            case SigOpCode_sensorCadenceSet:
                messageType = [SigSensorCadenceSet class];
                break;
            case SigOpCode_sensorCadenceSetUnacknowledged:
                messageType = [SigSensorCadenceSetUnacknowledged class];
                break;
            case SigOpCode_sensorCadenceStatus:
                messageType = [SigSensorCadenceStatus class];
                break;
            case SigOpCode_sensorSettingsGet:
                messageType = [SigSensorSettingsGet class];
                break;
            case SigOpCode_sensorSettingsStatus:
                messageType = [SigSensorSettingsStatus class];
                break;
            case SigOpCode_sensorSettingGet:
                messageType = [SigSensorSettingGet class];
                break;
            case SigOpCode_sensorSettingSet:
                messageType = [SigSensorSettingSet class];
                break;
            case SigOpCode_sensorSettingSetUnacknowledged:
                messageType = [SigSensorSettingSetUnacknowledged class];
                break;
            case SigOpCode_sensorSettingStatus:
                messageType = [SigSensorSettingStatus class];
                break;
            //Time
            case SigOpCode_timeGet:
                messageType = [SigTimeGet class];
                break;
            case SigOpCode_timeSet:
                messageType = [SigTimeSet class];
                break;
            case SigOpCode_timeStatus:
                messageType = [SigTimeStatus class];
                break;
            case SigOpCode_timeRoleGet:
                messageType = [SigTimeRoleGet class];
                break;
            case SigOpCode_timeRoleSet:
                messageType = [SigTimeRoleSet class];
                break;
            case SigOpCode_timeRoleStatus:
                messageType = [SigTimeRoleStatus class];
                break;
            case SigOpCode_timeZoneGet:
                messageType = [SigTimeZoneGet class];
                break;
            case SigOpCode_timeZoneSet:
                messageType = [SigTimeZoneSet class];
                break;
            case SigOpCode_timeZoneStatus:
                messageType = [SigTimeZoneStatus class];
                break;
            case SigOpCode_TAI_UTC_DeltaGet:
                messageType = [SigTAI_UTC_DeltaGet class];
                break;
            case SigOpCode_TAI_UTC_DeltaSet:
                messageType = [SigTAI_UTC_DeltaSet class];
                break;
            case SigOpCode_TAI_UTC_DeltaStatus:
                messageType = [SigTAI_UTC_DeltaStatus class];
                break;
            //Scene
            case SigOpCode_sceneGet:
                messageType = [SigSceneGet class];
                break;
            case SigOpCode_sceneRecall:
                messageType = [SigSceneRecall class];
                break;
            case SigOpCode_sceneRecallUnacknowledged:
                messageType = [SigSceneRecallUnacknowledged class];
                break;
            case SigOpCode_sceneStatus:
                messageType = [SigSceneStatus class];
                break;
            case SigOpCode_sceneRegisterGet:
                messageType = [SigSceneRegisterGet class];
                break;
            case SigOpCode_sceneRegisterStatus:
                messageType = [SigSceneRegisterStatus class];
                break;
            //Scene Setup
            case SigOpCode_sceneStore:
                messageType = [SigSceneStore class];
                break;
            case SigOpCode_sceneStoreUnacknowledged:
                messageType = [SigSceneStore class];
                break;
            case SigOpCode_sceneDelete:
                messageType = [SigSceneDelete class];
                break;
            case SigOpCode_sceneDeleteUnacknowledged:
                messageType = [SigSceneDeleteUnacknowledged class];
                break;
            //Scheduler
            case SigOpCode_schedulerActionGet:
                messageType = [SigSchedulerActionGet class];
                break;
            case SigOpCode_schedulerActionStatus:
                messageType = [SigSchedulerActionStatus class];
                break;
            case SigOpCode_schedulerGet:
                messageType = [SigSchedulerGet class];
                break;
            case SigOpCode_schedulerStatus:
                messageType = [SigSchedulerStatus class];
                break;
            //Scheduler Setup
            case SigOpCode_schedulerActionSet:
                messageType = [SigSchedulerActionSet class];
                break;
            case SigOpCode_schedulerActionSetUnacknowledged:
                messageType = [SigSchedulerActionSetUnacknowledged class];
                break;
            //Light Lightness
            case SigOpCode_lightLightnessGet:
                messageType = [SigLightLightnessGet class];
                break;
            case SigOpCode_lightLightnessSet:
                messageType = [SigLightLightnessSet class];
                break;
            case SigOpCode_lightLightnessSetUnacknowledged:
                messageType = [SigLightLightnessSetUnacknowledged class];
                break;
            case SigOpCode_lightLightnessStatus:
                messageType = [SigLightLightnessStatus class];
                break;
            case SigOpCode_lightLightnessLinearGet:
                messageType = [SigLightLightnessLinearGet class];
                break;
            case SigOpCode_lightLightnessLinearSet:
                messageType = [SigLightLightnessLinearSet class];
                break;
            case SigOpCode_lightLightnessLinearSetUnacknowledged:
                messageType = [SigLightLightnessLinearSetUnacknowledged class];
                break;
            case SigOpCode_lightLightnessLinearStatus:
                messageType = [SigLightLightnessLinearStatus class];
                break;
            case SigOpCode_lightLightnessLastGet:
                messageType = [SigLightLightnessLastGet class];
                break;
            case SigOpCode_lightLightnessLastStatus:
                messageType = [SigLightLightnessLastStatus class];
                break;
            case SigOpCode_lightLightnessDefaultGet:
                messageType = [SigLightLightnessDefaultGet class];
                break;
            case SigOpCode_lightLightnessDefaultStatus:
                messageType = [SigLightLightnessDefaultStatus class];
                break;
            case SigOpCode_lightLightnessRangeGet:
                messageType = [SigLightLightnessRangeGet class];
                break;
            case SigOpCode_lightLightnessRangeStatus:
                messageType = [SigLightLightnessRangeStatus class];
                break;
            //Light Lightness Setup
            case SigOpCode_lightLightnessDefaultSet:
                messageType = [SigLightLightnessDefaultSet class];
                break;
            case SigOpCode_lightLightnessDefaultSetUnacknowledged:
                messageType = [SigLightLightnessDefaultSetUnacknowledged class];
                break;
            case SigOpCode_lightLightnessRangeSet:
                messageType = [SigLightLightnessRangeSet class];
                break;
            case SigOpCode_lightLightnessRangeSetUnacknowledged:
                messageType = [SigLightLightnessRangeSetUnacknowledged class];
                break;
            //Light CTL
            case SigOpCode_lightCTLGet:
                messageType = [SigLightCTLGet class];
                break;
            case SigOpCode_lightCTLSet:
                messageType = [SigLightCTLSet class];
                break;
            case SigOpCode_lightCTLSetUnacknowledged:
                messageType = [SigLightCTLSetUnacknowledged class];
                break;
            case SigOpCode_lightCTLStatus:
                messageType = [SigLightCTLStatus class];
                break;
            case SigOpCode_lightCTLTemperatureGet:
                messageType = [SigLightCTLTemperatureGet class];
                break;
            case SigOpCode_lightCTLTemperatureRangeGet:
                messageType = [SigLightCTLTemperatureRangeGet class];
                break;
            case SigOpCode_lightCTLTemperatureRangeStatus:
                messageType = [SigLightCTLTemperatureRangeStatus class];
                break;
            case SigOpCode_lightCTLTemperatureSet:
                messageType = [SigLightCTLTemperatureSet class];
                break;
            case SigOpCode_lightCTLTemperatureSetUnacknowledged:
                messageType = [SigLightCTLTemperatureSetUnacknowledged class];
                break;
            case SigOpCode_lightCTLTemperatureStatus:
                messageType = [SigLightCTLTemperatureStatus class];
                break;
            case SigOpCode_lightCTLDefaultGet:
                messageType = [SigLightCTLDefaultGet class];
                break;
            case SigOpCode_lightCTLDefaultStatus:
                messageType = [SigLightCTLDefaultStatus class];
                break;
            //Light CTL Setup
            case SigOpCode_lightCTLDefaultSet:
                messageType = [SigLightCTLDefaultSet class];
                break;
            case SigOpCode_lightCTLDefaultSetUnacknowledged:
                messageType = [SigLightCTLDefaultSetUnacknowledged class];
                break;
            case SigOpCode_lightCTLTemperatureRangeSet:
                messageType = [SigLightCTLTemperatureRangeSet class];
                break;
            case SigOpCode_lightCTLTemperatureRangeSetUnacknowledged:
                messageType = [SigLightCTLTemperatureRangeSetUnacknowledged class];
                break;
            //Light HSL
            case SigOpCode_lightHSLGet:
                messageType = [SigLightHSLGet class];
                break;
            case SigOpCode_lightHSLHueGet:
                messageType = [SigLightHSLHueGet class];
                break;
            case SigOpCode_lightHSLHueSet:
                messageType = [SigLightHSLHueSet class];
                break;
            case SigOpCode_lightHSLHueSetUnacknowledged:
                messageType = [SigLightHSLHueSetUnacknowledged class];
                break;
            case SigOpCode_lightHSLHueStatus:
                messageType = [SigLightHSLHueStatus class];
                break;
            case SigOpCode_lightHSLSaturationGet:
                messageType = [SigLightHSLSaturationGet class];
                break;
            case SigOpCode_lightHSLSaturationSet:
                messageType = [SigLightHSLSaturationSet class];
                break;
            case SigOpCode_lightHSLSaturationSetUnacknowledged:
                messageType = [SigLightHSLSaturationSetUnacknowledged class];
                break;
            case SigOpCode_lightHSLSaturationStatus:
                messageType = [SigLightHSLSaturationStatus class];
                break;
            case SigOpCode_lightHSLSet:
                messageType = [SigLightHSLSet class];
                break;
            case SigOpCode_lightHSLSetUnacknowledged:
                messageType = [SigLightHSLSetUnacknowledged class];
                break;
            case SigOpCode_lightHSLStatus:
                messageType = [SigLightHSLStatus class];
                break;
            case SigOpCode_lightHSLTargetGet:
                messageType = [SigLightHSLTargetGet class];
                break;
            case SigOpCode_lightHSLTargetStatus:
                messageType = [SigLightHSLTargetStatus class];
                break;
            case SigOpCode_lightHSLDefaultGet:
                messageType = [SigLightHSLDefaultGet class];
                break;
            case SigOpCode_lightHSLDefaultStatus:
                messageType = [SigLightHSLDefaultStatus class];
                break;
            case SigOpCode_lightHSLRangeGet:
                messageType = [SigLightHSLRangeGet class];
                break;
            case SigOpCode_lightHSLRangeStatus:
                messageType = [SigLightHSLRangeStatus class];
                break;
            //Light HSL Setup
            case SigOpCode_lightHSLDefaultSet:
                messageType = [SigLightHSLDefaultSet class];
                break;
            case SigOpCode_lightHSLDefaultSetUnacknowledged:
                messageType = [SigLightHSLDefaultSetUnacknowledged class];
                break;
            case SigOpCode_lightHSLRangeSet:
                messageType = [SigLightHSLRangeSet class];
                break;
            case SigOpCode_lightHSLRangeSetUnacknowledged:
                messageType = [SigLightHSLRangeSetUnacknowledged class];
                break;
            //Light xyL
            case SigOpCode_lightXyLGet:
                messageType = [SigLightXyLGet class];
                break;
            case SigOpCode_lightXyLSet:
                messageType = [SigLightXyLSet class];
                break;
            case SigOpCode_lightXyLSetUnacknowledged:
                messageType = [SigLightXyLSetUnacknowledged class];
                break;
            case SigOpCode_lightXyLStatus:
                messageType = [SigLightXyLStatus class];
                break;
            case SigOpCode_lightXyLTargetGet:
                messageType = [SigLightXyLTargetGet class];
                break;
            case SigOpCode_lightXyLTargetStatus:
                messageType = [SigLightXyLTargetStatus class];
                break;
            case SigOpCode_lightXyLDefaultGet:
                messageType = [SigLightXyLDefaultGet class];
                break;
            case SigOpCode_lightXyLDefaultStatus:
                messageType = [SigLightXyLDefaultStatus class];
                break;
            case SigOpCode_lightXyLRangeGet:
                messageType = [SigLightXyLRangeGet class];
                break;
            case SigOpCode_lightXyLRangeStatus:
                messageType = [SigLightXyLRangeStatus class];
                break;
                //Light xyL Setup
            case SigOpCode_lightXyLDefaultSet:
                messageType = [SigLightXyLDefaultSet class];
                break;
            case SigOpCode_lightXyLDefaultSetUnacknowledged:
                messageType = [SigLightXyLDefaultSetUnacknowledged class];
                break;
            case SigOpCode_lightXyLRangeSet:
                messageType = [SigLightXyLRangeSet class];
                break;
            case SigOpCode_lightXyLRangeSetUnacknowledged:
                messageType = [SigLightXyLRangeSetUnacknowledged class];
                break;
                //Light Control
            case SigOpCode_LightLCModeGet:
                messageType = [SigLightLCModeGet class];
                break;
            case SigOpCode_LightLCModeSet:
                messageType = [SigLightLCModeSet class];
                break;
            case SigOpCode_LightLCModeSetUnacknowledged:
                messageType = [SigLightLCModeSetUnacknowledged class];
                break;
            case SigOpCode_LightLCModeStatus:
                messageType = [SigLightLCModeStatus class];
                break;
            case SigOpCode_LightLCOMGet:
                messageType = [SigLightLCOMGet class];
                break;
            case SigOpCode_LightLCOMSet:
                messageType = [SigLightLCOMSet class];
                break;
            case SigOpCode_LightLCOMSetUnacknowledged:
                messageType = [SigLightLCOMSetUnacknowledged class];
                break;
            case SigOpCode_LightLCOMStatus:
                messageType = [SigLightLCOMStatus class];
                break;
            case SigOpCode_LightLCLightOnOffGet:
                messageType = [SigLightLCLightOnOffGet class];
                break;
            case SigOpCode_LightLCLightOnOffSet:
                messageType = [SigLightLCLightOnOffSet class];
                break;
            case SigOpCode_LightLCLightOnOffSetUnacknowledged:
                messageType = [SigLightLCLightOnOffSetUnacknowledged class];
                break;
            case SigOpCode_LightLCLightOnOffStatus:
                messageType = [SigLightLCLightOnOffStatus class];
                break;
            case SigOpCode_LightLCPropertyGet:
                messageType = [SigLightLCPropertyGet class];
                break;
            case SigOpCode_LightLCPropertySet:
                messageType = [SigLightLCPropertySet class];
                break;
            case SigOpCode_LightLCPropertySetUnacknowledged:
                messageType = [SigLightLCPropertySetUnacknowledged class];
                break;
            case SigOpCode_LightLCPropertyStatus:
                messageType = [SigLightLCPropertyStatus class];
                break;
                
                // Firmware Update Messages
            case SigOpCode_FirmwareUpdateInformationGet:
                messageType = [SigFirmwareUpdateInformationGet class];
                break;
            case SigOpCode_FirmwareUpdateInformationStatus:
                messageType = [SigFirmwareUpdateInformationStatus class];
                break;
            case SigOpCode_FirmwareUpdateFirmwareMetadataCheck:
                messageType = [SigFirmwareUpdateFirmwareMetadataCheck class];
                break;
            case SigOpCode_FirmwareUpdateFirmwareMetadataStatus:
                messageType = [SigFirmwareUpdateFirmwareMetadataStatus class];
                break;
            case SigOpCode_FirmwareUpdateGet:
                messageType = [SigFirmwareUpdateGet class];
                break;
            case SigOpCode_FirmwareUpdateStart:
                messageType = [SigFirmwareUpdateStart class];
                break;
            case SigOpCode_FirmwareUpdateCancel:
                messageType = [SigFirmwareUpdateCancel class];
                break;
            case SigOpCode_FirmwareUpdateApply:
                messageType = [SigFirmwareUpdateApply class];
                break;
            case SigOpCode_FirmwareUpdateStatus:
                messageType = [SigFirmwareUpdateStatus class];
                break;
            case SigOpCode_FirmwareDistributionGet:
                messageType = [SigFirmwareDistributionGet class];
                break;
            case SigOpCode_FirmwareDistributionStart:
                messageType = [SigFirmwareDistributionStart class];
                break;
            case SigOpCode_FirmwareDistributionCancel:
                messageType = [SigFirmwareDistributionCancel class];
                break;
            case SigOpCode_FirmwareDistributionApply:
                messageType = [SigFirmwareDistributionApply class];
                break;
            case SigOpCode_FirmwareDistributionStatus:
                messageType = [SigFirmwareDistributionStatus class];
                break;
//            case SigOpCode_FirmwareDistributionNodesGet:
//                messageType = [SigFirmwareDistributionDetailsGet class];
//                break;
            case SigOpCode_FirmwareDistributionNodesList:
                messageType = [SigFirmwareDistributionDetailsList class];
                break;

                // BLOB Transfer Messages
            case SigOpCode_BLOBTransferGet:
                messageType = [SigBLOBTransferGet class];
                break;
            case SigOpCode_BLOBTransferStart:
                messageType = [SigBLOBTransferStart class];
                break;
            case SigOpCode_BLOBTransferCancel:
                messageType = [SigObjectTransferCancel class];
                break;
            case SigOpCode_BLOBTransferStatus:
                messageType = [SigBLOBTransferStatus class];
                break;
            case SigOpCode_BLOBBlockStart:
                messageType = [SigBLOBBlockStart class];
                break;
            case SigOpCode_ObjectBlockTransferStatus:
                messageType = [SigObjectBlockTransferStatus class];
                break;
            case SigOpCode_BLOBChunkTransfer:
                messageType = [SigBLOBChunkTransfer class];
                break;
            case SigOpCode_BLOBBlockGet:
                messageType = [SigBLOBBlockGet class];
                break;
            case SigOpCode_BLOBBlockStatus:
                messageType = [SigBLOBBlockStatus class];
                break;
            case SigOpCode_BLOBInformationGet:
                messageType = [SigBLOBInformationGet class];
                break;
            case SigOpCode_BLOBInformationStatus:
                messageType = [SigBLOBInformationStatus class];
                break;
            default:
                break;
        }
    }
    return messageType;
}

//- (void)handleAccessPdu:(SigAccessPdu *)accessPdu {
//    Class MessageType = [self getMeshMessageWithOpCode:accessPdu.opCode];
//    if (MessageType != nil) {
//        SigMeshMessage *msg = [[MessageType alloc] initWithParameters:accessPdu.parameters];
//        if (msg) {
////            if var unknownMessage = message as? UnknownMessage {
////                unknownMessage.opCode = accessPdu.opCode
////                message = unknownMessage
////            }
//
//            SigConfigMessage *configMessage = (SigConfigMessage *)msg;
//            if ([configMessage isKindOfClass:[SigConfigMessage class]]) {
//                [_networkManager.foundationLayer handleConfigMessage:configMessage fromSourceAddress:accessPdu.source];
//            }
//            [_networkManager notifyAboutNewMessage:msg fromSource:accessPdu.source];
//        }
//    }
//}

/// This method tries to decode the Access PDU into a Message.
///
/// The Model Handler must support the opcode and specify to
/// which type should the message be decoded.
///
/// - parameter accessPdu: The Access PDU received.
/// - returns: The decoded message, or `nil`, if the message
///            is not supported or invalid.
- (SigMeshMessage *)decodeSigAccessPdu:(SigAccessPdu *)accessPdu {
//    if (self.modelDelegate && [self getMeshMessageWithOpCode:accessPdu.opCode]) {
        Class MessageType = [self getMeshMessageWithOpCode:accessPdu.opCode];
        if (MessageType != nil) {
            SigMeshMessage *msg = [[MessageType alloc] initWithParameters:accessPdu.parameters];
            return msg;
        }
//    }
    return nil;
}

/// This method handles the decoded message and passes it to
/// the proper handle method, depending on its type or whether
/// it is a response to a previously sent request.
///
/// - parameters:
///   - model:   The local Model that received the message.
///   - message: The decoded message.
///   - source:  The Unicast Address of the Element that the message
///              originates from.
///   - destination: The destination address of the request.
///   - request: The request message sent previously that this message
///              replies to, or `nil`, if this is not a response.
/// - returns: The response message, if the received message is an
///            Acknowledged Mesh Message that needs to be replied.
//- (SigMeshMessage *)getModel:(SigModelIDModel *)model didReceiveMeshMessage:(SigMeshMessage *)message fromSource:(UInt16)source sentToDestination:(SigMeshAddress *)destination asResponseTo:(SigAcknowledgedMeshMessage *)request {
//    if (request) {
//        [self model:model didReceiveResponse:message toAcknowledgedMessage:request fromSource:source];
//        return nil;
//    }else if([request isKindOfClass:[SigAcknowledgedMeshMessage class]]) {
//        return [self getModel:model didReceiveAcknowledgedMessage:request fromSource:source sentToDestination:destination];
//    }else{
//        [self model:model didReceiveUnacknowledgedMessage:message fromSource:source sentToDestination:destination];
//        return nil;
//    }
//}

@end
