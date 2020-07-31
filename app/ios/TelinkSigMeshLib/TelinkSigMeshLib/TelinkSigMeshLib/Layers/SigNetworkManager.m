/********************************************************************************************************
 * @file     SigNetworkManager.m
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
//  SigNetworkManager.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/8/15.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import "SigNetworkManager.h"
#import "SigDataSource.h"
#import "SigMeshLib.h"
#import "SigNetworkLayer.h"
#import "SigLowerTransportLayer.h"
#import "SigUpperTransportLayer.h"
#import "SigAccessLayer.h"
#import "SigBearer.h"

@implementation SigNetworkManager

+ (SigNetworkManager *)share {
    static SigNetworkManager *shareManager = nil;
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        shareManager = [[SigNetworkManager alloc] init];
        shareManager.manager = [SigMeshLib share];
        shareManager.networkLayer = [[SigNetworkLayer alloc] initWithNetworkManager:shareManager];
        shareManager.lowerTransportLayer = [[SigLowerTransportLayer alloc] initWithNetworkManager:shareManager];
        shareManager.upperTransportLayer = [[SigUpperTransportLayer alloc] initWithNetworkManager:shareManager];
        shareManager.accessLayer = [[SigAccessLayer alloc] initWithNetworkManager:shareManager];
    });
    return shareManager;
}

- (UInt8)defaultTtl {
    return MAX(MIN(_manager.defaultTtl, 127), 2);
}

- (NSTimeInterval)incompleteMessageTimeout {
    return MAX(_manager.incompleteMessageTimeout, 10.0);
}

//- (NSTimeInterval)acknowledgmentMessageTimeout {
//    return MAX(_manager.acknowledgmentMessageTimeout, 30.0);
//}

//- (NSTimeInterval)acknowledgmentMessageInterval:(UInt8)ttl segmentCount:(int)segmentCount {
//    return MAX(_manager.acknowledgmentMessageInterval, 2.0) + (double)(ttl) * 0.050 + (double)(segmentCount) * 0.050;
//}

- (NSTimeInterval)acknowledgmentTimerInterval:(UInt8)ttl {
    return MAX(_manager.acknowledgmentTimerInterval, 0.150) + (double)(ttl) * 0.050;
}

- (NSTimeInterval)transmissionTimerInteral:(UInt8)ttl {
    return MAX(_manager.transmissionTimerInteral, 0.200) + (double)(ttl) * 0.050;
}

- (int)retransmissionLimit {
    return MAX(_manager.retransmissionLimit, 2);
}

#pragma mark - Receiving messages

- (void)handleIncomingPdu:(NSData *)pdu ofType:(SigPduType)type {
    [self.networkLayer handleIncomingPdu:pdu ofType:type];
}

#pragma mark - Sending messages

- (void)sendMeshMessage:(SigMeshMessage *)message fromElement:(SigElementModel *)element toDestination:(SigMeshAddress *)destination withTtl:(UInt8)initialTtl usingApplicationKey:(SigAppkeyModel *)applicationKey command:(SDKLibCommand *)command {
    [_accessLayer sendMessage:message fromElement:element toDestination:destination withTtl:initialTtl usingApplicationKey:applicationKey command:command];
}

- (void)sendMeshMessage:(SigMeshMessage *)message fromElement:(SigElementModel *)element toDestination:(SigMeshAddress *)destination withTtl:(UInt8)initialTtl usingApplicationKey:(SigAppkeyModel *)applicationKey {
    [_accessLayer sendMessage:message fromElement:element toDestination:destination withTtl:initialTtl usingApplicationKey:applicationKey];
}

- (void)sendConfigMessage:(SigConfigMessage *)configMessage toDestination:(UInt16)destination withTtl:(UInt8)initialTtl {
    [_accessLayer sendSigConfigMessage:configMessage toDestination:destination withTtl:initialTtl];
}

- (void)replyToMessageSentToOrigin:(UInt16)origin withMessage:(SigMeshMessage *)message toDestination:(UInt16)destination usingKeySet:(SigKeySet *)keySet {
    SigElementModel *primaryElement = _manager.dataSource.curLocationNodeModel.elements.firstObject;
    if (primaryElement) {
        [_accessLayer replyToMessageSentToOrigin:origin withMeshMessage:message fromElement:primaryElement toDestination:destination usingKeySet:keySet];
    }
}

- (void)replyToMessageSentToOrigin:(UInt16)origin withMessage:(SigMeshMessage *)message fromElement:(SigElementModel *)element toDestination:(UInt16)destination usingKeySet:(SigKeySet *)keySet {
    [_accessLayer replyToMessageSentToOrigin:origin withMeshMessage:message fromElement:element toDestination:destination usingKeySet:keySet];
}

- (void)sendSigProxyConfigurationMessage:(SigProxyConfigurationMessage *)message {
    [_networkLayer sendSigProxyConfigurationMessage:message];
}

- (void)cancelSigMessageHandle:(SigMessageHandle *)handler {
    [_accessLayer cancelSigMessageHandle:handler];
}

#pragma mark - Callbacks

- (void)notifyAboutNewMessage:(SigMeshMessage *)message fromSource:(UInt16)source toDestination:(UInt16)destination {
    __weak typeof(self) weakSelf = self;
    dispatch_async(self.manager.delegateQueue, ^{
        if ([weakSelf.manager.delegate respondsToSelector:@selector(didReceiveMessage:sentFromSource:toDestination:)]) {
            [weakSelf.manager.delegate didReceiveMessage:message sentFromSource:source toDestination:destination];
        }
        if ([weakSelf.manager.delegateForDeveloper respondsToSelector:@selector(didReceiveMessage:sentFromSource:toDestination:)]) {
            [weakSelf.manager.delegateForDeveloper didReceiveMessage:message sentFromSource:source toDestination:destination];
        }
    });
}

- (void)notifyAboutDeliveringMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toDestination:(UInt16)destination {
    __weak typeof(self) weakSelf = self;
    dispatch_async(self.manager.delegateQueue, ^{
        if ([weakSelf.manager.delegate respondsToSelector:@selector(didSendMessage:fromLocalElement:toDestination:)]) {
            [weakSelf.manager.delegate didSendMessage:message fromLocalElement:localElement toDestination:destination];
        }
        if ([weakSelf.manager.delegateForDeveloper respondsToSelector:@selector(didSendMessage:fromLocalElement:toDestination:)]) {
            [weakSelf.manager.delegateForDeveloper didSendMessage:message fromLocalElement:localElement toDestination:destination];
        }
    });
}

- (void)notifyAboutError:(NSError *)error duringSendingMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toDestination:(UInt16)destination {
    __weak typeof(self) weakSelf = self;
    dispatch_async(self.manager.delegateQueue, ^{
        if ([weakSelf.manager.delegate respondsToSelector:@selector(failedToSendMessage:fromLocalElement:toDestination:error:)]) {
            [weakSelf.manager.delegate failedToSendMessage:message fromLocalElement:localElement toDestination:destination error:error];
        }
        if ([weakSelf.manager.delegateForDeveloper respondsToSelector:@selector(failedToSendMessage:fromLocalElement:toDestination:error:)]) {
            [weakSelf.manager.delegateForDeveloper failedToSendMessage:message fromLocalElement:localElement toDestination:destination error:error];
        }
    });
}

#pragma mark - new api

//- (void)setFilter:(SigNetkeyModel *)netkeyModel {
//    TeLogDebug(@"netkeyModel=%@",netkeyModel);
//    [self.networkLayer updateProxyFilterUsingNetworkKey:netkeyModel];
//}

@end
