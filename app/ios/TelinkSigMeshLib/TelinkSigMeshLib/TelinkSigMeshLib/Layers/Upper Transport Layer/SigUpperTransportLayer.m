/********************************************************************************************************
 * @file     SigUpperTransportLayer.m
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
//  SigUpperTransportLayer.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigUpperTransportLayer.h"
#import "SigHearbeatMessage.h"
#import "SigLowerTransportPdu.h"
#import "SigAccessMessage.h"
#import "SigUpperTransportPdu.h"
#import "SigAccessLayer.h"
#import "SigControlMessage.h"
#import "SigLowerTransportLayer.h"
#import "SigMeshLib.h"
#import "SigAccessPdu.h"

@interface SigUpperTransportModel : NSObject
@property (nonatomic,strong) SigUpperTransportPdu *pdu;
@property (nonatomic,assign) UInt8 ttl;
@property (nonatomic,strong) SigNetkeyModel *networkKey;
@property (nonatomic,strong) SigIvIndex *ivIndex;
@end
@implementation SigUpperTransportModel
- (NSString *)description {
    return[NSString stringWithFormat:@"<%p> - SigUpperTransportModel, SigUpperTransportPdu:%@",self,self.pdu];
}
@end

@interface SigUpperTransportLayer ()
@property (nonatomic,strong) NSUserDefaults *defaults;
/// The upper transport layer shall not transmit a new segmented
/// Upper Transport PDU to a given destination until the previous
/// Upper Transport PDU to that destination has been either completed
/// or cancelled.
///
/// This map contains queues of messages targetting each destination.
@property (nonatomic,strong) NSMutableDictionary <NSNumber *,NSMutableArray <SigUpperTransportModel *>*>*queues;

@end

@implementation SigUpperTransportLayer

- (instancetype)initWithNetworkManager:(SigNetworkManager *)networkManager {
    if (self = [super init]) {
        _networkManager = networkManager;
        _defaults = [NSUserDefaults standardUserDefaults];
        _queues = [NSMutableDictionary dictionary];
    }
    return self;
}

- (void)handleLowerTransportPdu:(SigLowerTransportPdu *)lowerTransportPdu {
    switch (lowerTransportPdu.type) {
        case SigLowerTransportPduType_accessMessage:
            {
//                TeLogDebug(@"lowerTransportPdu.upperTransportPdu=%@,length=%d",[LibTools convertDataToHexStr:lowerTransportPdu.transportPdu],lowerTransportPdu.transportPdu.length);
                SigAccessMessage *accessMessage = (SigAccessMessage *)lowerTransportPdu;
                NSDictionary *dict = [SigUpperTransportPdu decodeAccessMessage:accessMessage forMeshNetwork:SigDataSource.share];
                if (dict && dict.allKeys.count == 2) {
                    SigUpperTransportPdu *upperTransportPdu = dict[@"SigUpperTransportPdu"];
                    SigKeySet *keySet = dict[@"SigKeySet"];
//                    TeLogInfo(@"%@ received",upperTransportPdu);
                    [_networkManager.accessLayer handleUpperTransportPdu:upperTransportPdu sentWithSigKeySet:keySet];
                }else{
                    TeLogError(@"Failed to decode PDU");
                }
            }
            break;
        case SigLowerTransportPduType_controlMessage:
        {
            SigControlMessage *controlMessage = (SigControlMessage *)lowerTransportPdu;
            switch (controlMessage.opCode) {
                case 0x0A:
                    {
                        SigHearbeatMessage *heartbeat = [[SigHearbeatMessage alloc] initFromControlMessage:controlMessage];
                        if (heartbeat) {
                            TeLogInfo(@"%@ received",heartbeat);
                            [self handleHearbeat:heartbeat];
                        }
                    }
                    break;
                    
                default:
                    TeLogInfo(@"Unsupported Control Message received (opCode: 0x%x)",controlMessage.opCode);
                    // Other Control Messages are not supported.
                    break;
            }
        }
            break;

        default:
            break;
    }
}

- (void)sendAccessPdu:(SigAccessPdu *)accessPdu withTtl:(UInt8)initialTtl usingKeySet:(SigKeySet *)keySet command:(SDKLibCommand *)command {
    UInt32 sequence = [SigDataSource.share getCurrentProvisionerIntSequenceNumber];
    SigNetkeyModel *networkKey = command.netkeyA;
    SigUpperTransportPdu *pdu = [[SigUpperTransportPdu alloc] initFromAccessPdu:accessPdu usingKeySet:keySet ivIndex:command.ivIndexA sequence:sequence];
    TeLogVerbose(@"Sending %@ encrypted using key: %@,pdu.transportPdu=%@",pdu,keySet,pdu.transportPdu);
    BOOL isSegmented = pdu.transportPdu.length > SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength || accessPdu.isSegmented;
    if (isSegmented) {
        TeLogInfo(@"sending segment pdu.");
        // Enqueue the PDU. If the queue was empty, the PDU will be sent
        // immediately.
        [self enqueueSigUpperTransportPdu:pdu initialTtl:initialTtl networkKey:networkKey ivIndex:command.ivIndexA];
    } else {
        TeLogInfo(@"sending unsegment pdu.");
        [_networkManager.lowerTransportLayer sendUnsegmentedUpperTransportPdu:pdu withTtl:initialTtl usingNetworkKey:networkKey ivIndex:command.ivIndexA];
    }
}

- (void)sendAccessPdu:(SigAccessPdu *)accessPdu withTtl:(UInt8)initialTtl usingKeySet:(SigKeySet *)keySet {
    UInt32 sequence = [SigDataSource.share getCurrentProvisionerIntSequenceNumber];
    SigNetkeyModel *networkKey = keySet.networkKey;
    SigUpperTransportPdu *pdu = [[SigUpperTransportPdu alloc] initFromAccessPdu:accessPdu usingKeySet:keySet sequence:sequence];
    TeLogVerbose(@"Sending %@ encrypted using key: %@,pdu.transportPdu=%@",pdu,keySet,pdu.transportPdu);
    BOOL isSegmented = pdu.transportPdu.length > SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength || accessPdu.isSegmented;
    if (isSegmented) {
        TeLogInfo(@"sending segment pdu.");
        // Enqueue the PDU. If the queue was empty, the PDU will be sent
        // immediately.
        [self enqueueSigUpperTransportPdu:pdu initialTtl:initialTtl networkKey:networkKey];
    } else {
        TeLogInfo(@"sending unsegment pdu.");
        [_networkManager.lowerTransportLayer sendUnsegmentedUpperTransportPdu:pdu withTtl:initialTtl usingNetworkKey:networkKey];
    }
}

/// Cancels sending all segmented messages matching given handle.
/// Unsegmented messages are sent almost instantaneously and cannot be
/// cancelled.
///
/// - parameter handle: The message handle.
- (void)cancelHandleSigMessageHandle:(SigMessageHandle *)handle {
    BOOL shouldSendNext = NO;
    // Check if the message that is currently being sent mathes the
    // handler data. If so, cancel it.
    NSMutableArray *array = _queues[@(handle.destination)];
    if (array == nil || array.count == 0) {
//        TeLogDebug(@"array == nil || array.count == 0");
        return;
    }
    SigUpperTransportModel *model = array.firstObject;
    if (model == nil) {
        TeLogDebug(@"model == nil");
        return;
    }
    if (model.pdu.message.opCode == handle.opCode && model.pdu.source == handle.source) {
        TeLogInfo(@"Cancelling sending %@",model.pdu);
        [_networkManager.lowerTransportLayer cancelSendingSegmentedUpperTransportPdu:model.pdu];
        shouldSendNext = YES;
    }
    // Remove all enqueued messages that match the handler.
    NSArray *tem1 = [NSArray arrayWithArray:_queues[@(handle.destination)]];
    NSMutableArray *tem2 = [NSMutableArray arrayWithArray:_queues[@(handle.destination)]];
    for (SigUpperTransportModel *temModel in tem1) {
        if (temModel.pdu.message.opCode == handle.opCode && temModel.pdu.source == handle.source &&
        temModel.pdu.destination == handle.destination) {
            [tem2 removeObject:temModel];
        }
    }
    _queues[@(handle.destination)] = tem2;
    // If sending a message was cancelled, try sending another one.
    if (shouldSendNext) {
        [self lowerTransportLayerDidSendSegmentedUpperTransportPduToDestination:handle.destination];
    }
}

/// A callback called by the lower transport layer when the segmented PDU
/// has been sent to the given destination.
///
/// This method removes the sent PDU from the queue and initiates sending
/// a next one, had it been enqueued.
///
/// - parameter destination: The destination address.
- (void)lowerTransportLayerDidSendSegmentedUpperTransportPduToDestination:(UInt16)destination {
    if (_queues == nil || _queues.count == 0 || _queues[@(destination)] == nil) {
        TeLogDebug(@"_queues[destination] is empty.");
        return;
    }
    NSMutableArray *tem = [NSMutableArray arrayWithArray:_queues[@(destination)]];
    if (tem.count == 0) {
        TeLogDebug(@"_queues[destination] is empty.");
        return;
    }
    
    // Remove the PDU that has just been sent.
    [tem removeObjectAtIndex:0];
    _queues[@(destination)] = tem;
    // Try to send the next one.
    [self sendNextToDestination:destination];
}

- (void)handleHearbeat:(SigHearbeatMessage *)hearbeat {
    // TODO: Implement handling Heartbeat messages

}

- (void)enqueueSigUpperTransportPdu:(SigUpperTransportPdu *)pdu initialTtl:(UInt8)initialTtl networkKey:(SigNetkeyModel *)networkKey ivIndex:(SigIvIndex *)ivIndex {
    NSMutableArray *array = _queues[@(pdu.destination)];
    if (array == nil) {
        _queues[@(pdu.destination)] = [NSMutableArray array];
        array = [NSMutableArray array];
    }
    SigUpperTransportModel *model = [[SigUpperTransportModel alloc] init];
    model.pdu = pdu;
    model.ttl = initialTtl;
    model.networkKey = networkKey;
    model.ivIndex = ivIndex;
    [array addObject:model];
    _queues[@(pdu.destination)] = array;
    if (_queues[@(pdu.destination)].count == 1) {
        [self sendNextToDestination:pdu.destination];
    }else{
        TeLogWarn(@"异常逻辑，待完善。_queues[@(pdu.destination)]=%@",_queues[@(pdu.destination)]);
    }
}

/// Enqueues the PDU to be sent using the given Network Key.
///
/// - parameters:
///   - pdu: The Upper Transport PDU to be sent.
///   - initialTtl: The initial TTL (Time To Live) value of the message.
///                 If `nil`, the default Node TTL will be used.
///   - networkKey: The Network Key to encrypt the PDU with.
- (void)enqueueSigUpperTransportPdu:(SigUpperTransportPdu *)pdu initialTtl:(UInt8)initialTtl networkKey:(SigNetkeyModel *)networkKey {
    NSMutableArray *array = _queues[@(pdu.destination)];
    if (array == nil) {
        _queues[@(pdu.destination)] = [NSMutableArray array];
        array = [NSMutableArray array];
    }
    SigUpperTransportModel *model = [[SigUpperTransportModel alloc] init];
    model.pdu = pdu;
    model.ttl = initialTtl;
    model.networkKey = networkKey;
    model.ivIndex = SigMeshLib.share.dataSource.curNetkeyModel.ivIndex;
    [array addObject:model];
    _queues[@(pdu.destination)] = array;
    if (array.count == 1) {
        [self sendNextToDestination:pdu.destination];
    }else{
        TeLogWarn(@"==========异常逻辑，待完善。_queues[@(pdu.destination)]=%@",array);
    }
}

/// Sends the next enqueded PDU.
///
/// If the queue for the given destination does not exist or is empty,
/// this method does nothing.
///
/// - parameter destination: The destination address.
- (void)sendNextToDestination:(UInt16)destination {
    NSMutableArray *array = _queues[@(destination)];
    if (array == nil || array.count == 0) {
//        TeLogDebug(@"array == nil || array.count == 0");
        return;
    }
    SigUpperTransportModel *model = array.firstObject;
    // If another PDU has been enqueued, send it.
    [_networkManager.lowerTransportLayer sendSegmentedUpperTransportPdu:model.pdu withTtl:model.ttl usingNetworkKey:model.networkKey ivIndex:model.ivIndex];
}

@end
