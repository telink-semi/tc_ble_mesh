/********************************************************************************************************
* @file     SigLowerTransportLayer.m
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
//  SigLowerTransportLayer.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigLowerTransportLayer.h"
#import "SigSegmentAcknowledgmentMessage.h"
#import "SigNetworkLayer.h"
#import "SigUpperTransportPdu.h"
#import "SigSegmentedAccessMessage.h"
#import "SigAccessMessage.h"
#import "SigSegmentedMessage.h"
#import "SigUpperTransportLayer.h"
#import "SigControlMessage.h"
#import "SigSegmentedControlMessage.h"
#import "SigNetworkManager.h"

@interface SigLowerTransportLayer ()
@property (nonatomic,strong) dispatch_queue_t mutex;

@end

@implementation SigLowerTransportLayer

- (instancetype)initWithNetworkManager:(SigNetworkManager *)networkManager {
    if (self = [super init]) {
        _networkManager = networkManager;
        self.incompleteSegments = [NSMutableDictionary dictionary];
        self.incompleteTimers = [NSMutableDictionary dictionary];
        self.acknowledgmentTimers = [NSMutableDictionary dictionary];
        self.outgoingSegments = [NSMutableDictionary dictionary];
        self.segmentTransmissionTimers = [NSMutableDictionary dictionary];
        self.acknowledgments = [NSMutableDictionary dictionary];
        self.segmentTtl = [NSMutableDictionary dictionary];
        _mutex = dispatch_queue_create("mutex", DISPATCH_QUEUE_SERIAL);
    }
    return self;
}

- (void)handleNetworkPdu:(SigNetworkPdu *)networkPdu {
    TeLogVerbose(@"receive:%@,%@,%d",networkPdu,networkPdu.pduData,networkPdu.isSegmented);
    @synchronized(self) {
        // Some validation, just to be sure. This should pass for sure.
        if (networkPdu.transportPdu.length <= 1) {
            return;
        }
        // Segmented messages must be validated and assembled in thread safe way.
        __weak typeof(self) weakSelf = self;
        dispatch_async(_mutex, ^{
            BOOL result = [weakSelf checkAgainstReplayAttackWithNetworkPdu:networkPdu];
            if (!result) {
                TeLogError(@"LowerTransportError.replayAttack");
                return;
            }
            // Lower Transport Messages can be Unsegmented or Segmented.
            // This information is stored in the most significant bit of the first octet.
            BOOL segmented = networkPdu.isSegmented;
    //        TeLogInfo(@"==========networkPdu.isSegmented=%d,networkPdu.type=%d",networkPdu.isSegmented,networkPdu.type);
            if (segmented) {
                if (networkPdu.type == SigLowerTransportPduType_accessMessage) {
                    SigSegmentedAccessMessage *segment = [[SigSegmentedAccessMessage alloc] initFromSegmentedPdu:networkPdu];
                    if (segmented) {
                        TeLogVerbose(@"accessMessage %@ receieved (decrypted using key: %@)",segment,segment.networkKey);
                        TeLogVerbose(@"networkPdu %@",networkPdu);
                        SigLowerTransportPdu *pdu = [weakSelf assembleSegmentedMessage:segment createdFrom:networkPdu];
                        if (pdu) {
                            [weakSelf.networkManager.upperTransportLayer handleLowerTransportPdu:pdu];
                        }
                    }else{
                        TeLogError(@"segmented = nil.");
                    }
                }else if (networkPdu.type == SigLowerTransportPduType_controlMessage) {
                    SigSegmentedControlMessage *segment = [[SigSegmentedControlMessage alloc] initFromSegmentedPdu:networkPdu];
                    if (segmented) {
                        TeLogVerbose(@"controlMessage %@ receieved (decrypted using key: %@)",segment,segment.networkKey);
                        SigLowerTransportPdu *pdu = [weakSelf assembleSegmentedMessage:segment createdFrom:networkPdu];
                        if (pdu) {
                            [weakSelf.networkManager.upperTransportLayer handleLowerTransportPdu:pdu];
                        } else {
                            TeLogError(@"pdu = nil.");
                        }
                    }else{
                        TeLogError(@"segmented = nil.");
                    }
                }else{
                    TeLogError(@"networkPdu.type no exist.");
                }
            }else{
                if (networkPdu.type == SigLowerTransportPduType_accessMessage) {
                    SigAccessMessage *accessMessage = [[SigAccessMessage alloc] initFromUnsegmentedPdu:networkPdu];
                    if (accessMessage) {
                        TeLogVerbose(@"%@ receieved (decrypted using key: %@)",accessMessage,accessMessage.networkKey);
                        // Unsegmented message is not acknowledged. Just pass it to higher layer.
                        [weakSelf.networkManager.upperTransportLayer handleLowerTransportPdu:accessMessage];
                    } else {
                        TeLogError(@"accessMessage = nil.");
                    }
                }else if (networkPdu.type == SigLowerTransportPduType_controlMessage) {
                    UInt8 tem = 0;
                    Byte *byte = (Byte *)networkPdu.transportPdu.bytes;
                    memcpy(&tem, byte, 1);
                    UInt8 opCode = tem & 0x7F;
                    if (opCode == 0x00) {
                        SigSegmentAcknowledgmentMessage *ack = [[SigSegmentAcknowledgmentMessage alloc] initFromNetworkPdu:networkPdu];
                        if (ack) {
                            TeLogVerbose(@"SigSegmentAcknowledgmentMessage receieved =%@ (decrypted using key: %@)",ack,ack.networkKey);
                            [weakSelf handleSegmentAcknowledgmentMessage:ack];
                        } else {
                            TeLogError(@"ack = nil.");
                        }
                    } else {
                        SigControlMessage *controlMessage = [[SigControlMessage alloc] initFromNetworkPdu:networkPdu];
                        if (controlMessage) {
                            TeLogVerbose(@"%@ receieved (decrypted using key: %@)",controlMessage,controlMessage.networkKey);
                            // Unsegmented message is not acknowledged. Just pass it to higher layer.
                            [weakSelf.networkManager.upperTransportLayer handleLowerTransportPdu:controlMessage];
                        } else {
                            TeLogError(@"controlMessage = nil.");
                        }
                    }
                }else{
                    TeLogError(@"networkPdu.type no exist.");
                }
            }
        });
    }
}

- (void)sendUnsegmentedUpperTransportPdu:(SigUpperTransportPdu *)pdu withTtl:(UInt8)initialTtl usingNetworkKey:(SigNetkeyModel *)networkKey ivIndex:(SigIvIndex *)ivIndex {
    SigNodeModel *provisionerNode = SigDataSource.share.curLocationNodeModel;
    if (provisionerNode == nil) {
        TeLogError(@"curLocationNodeModel = nil.");
        return;
    }
    UInt8 ttl = initialTtl;
    if (![SigHelper.share isRelayedTTL:ttl]) {
        ttl = provisionerNode.defaultTTL;
        if (![SigHelper.share isRelayedTTL:ttl]) {
            ttl = _networkManager.defaultTtl;
        }
    }
    SigAccessMessage *message = [[SigAccessMessage alloc] initFromUnsegmentedUpperTransportPdu:pdu usingNetworkKey:networkKey];
    [_networkManager.networkLayer sendLowerTransportPdu:message ofType:SigPduType_networkPdu withTtl:ttl ivIndex:ivIndex];
    [_networkManager notifyAboutDeliveringMessage:pdu.message fromLocalElement:pdu.localElement toDestination:pdu.destination];
}

- (void)sendUnsegmentedUpperTransportPdu:(SigUpperTransportPdu *)pdu withTtl:(UInt8)initialTtl usingNetworkKey:(SigNetkeyModel *)networkKey {
    SigNodeModel *provisionerNode = SigDataSource.share.curLocationNodeModel;
    if (provisionerNode == nil) {
        TeLogError(@"curLocationNodeModel = nil.");
        return;
    }
    UInt8 ttl = initialTtl;
    if (![SigHelper.share isRelayedTTL:ttl]) {
        ttl = provisionerNode.defaultTTL;
        if (![SigHelper.share isRelayedTTL:ttl]) {
            ttl = _networkManager.defaultTtl;
        }
    }
    SigAccessMessage *message = [[SigAccessMessage alloc] initFromUnsegmentedUpperTransportPdu:pdu usingNetworkKey:networkKey];
    [_networkManager.networkLayer sendLowerTransportPdu:message ofType:SigPduType_networkPdu withTtl:ttl];
    [_networkManager notifyAboutDeliveringMessage:pdu.message fromLocalElement:pdu.localElement toDestination:pdu.destination];
}

- (void)sendSegmentedUpperTransportPdu:(SigUpperTransportPdu *)pdu withTtl:(UInt8)initialTtl usingNetworkKey:(SigNetkeyModel *)networkKey ivIndex:(SigIvIndex *)ivIndex {
    SigNodeModel *provisionerNode = SigDataSource.share.curLocationNodeModel;
    if (provisionerNode == nil) {
        TeLogError(@"curLocationNodeModel = nil.");
        return;
    }
    /// Last 13 bits of the sequence number are known as seqZero.
    UInt16 sequenceZero = (UInt16)(pdu.sequence & 0x1FFF);
    //==========telink need this==========//
    [self startTXTimeoutWithAddress:pdu.destination sequenceZero:sequenceZero];
    //==========telink need this==========//
    /// Number of segments to be sent.
    NSInteger count = (pdu.transportPdu.length + (SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength - 3) - 1) / (SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength - 3);
    // Create all segments to be sent.
    NSMutableArray *outgoingSegments = [NSMutableArray array];
    for (int i=0; i<count; i++) {
        SigSegmentedAccessMessage *msg = [[SigSegmentedAccessMessage alloc] initFromUpperTransportPdu:pdu usingNetworkKey:networkKey ivIndex:ivIndex offset:(UInt8)i];
        [outgoingSegments addObject:msg];
    }
    UInt8 ttl = initialTtl;
    if (![SigHelper.share isRelayedTTL:ttl] || ttl == 0) {
        ttl = provisionerNode.defaultTTL;
        if (![SigHelper.share isRelayedTTL:ttl]) {
            ttl = _networkManager.defaultTtl;
        }
    }
    _segmentTtl[@(sequenceZero)] = @(ttl);
    _outgoingSegments[@(sequenceZero)] = outgoingSegments;
    [self sendSegmentsForSequenceZero:sequenceZero limit:_networkManager.retransmissionLimit];
}

- (void)sendSegmentedUpperTransportPdu:(SigUpperTransportPdu *)pdu withTtl:(UInt8)initialTtl usingNetworkKey:(SigNetkeyModel *)networkKey {
    SigNodeModel *provisionerNode = SigDataSource.share.curLocationNodeModel;
    if (provisionerNode == nil) {
        TeLogError(@"curLocationNodeModel = nil.");
        return;
    }
    /// Last 13 bits of the sequence number are known as seqZero.
    UInt16 sequenceZero = (UInt16)(pdu.sequence & 0x1FFF);
    //==========telink need this==========//
    [self startTXTimeoutWithAddress:pdu.destination sequenceZero:sequenceZero];
    //==========telink need this==========//
    /// Number of segments to be sent.
    NSInteger count = (pdu.transportPdu.length + ((SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength - 3) - 1)) / (SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength - 3);
    // Create all segments to be sent.
    NSMutableArray *outgoingSegments = [NSMutableArray array];
    for (int i=0; i<count; i++) {
        SigSegmentedAccessMessage *msg = [[SigSegmentedAccessMessage alloc] initFromUpperTransportPdu:pdu usingNetworkKey:networkKey offset:(UInt8)i];
        [outgoingSegments addObject:msg];
    }
    UInt8 ttl = initialTtl;
    if (![SigHelper.share isRelayedTTL:ttl] || ttl == 0) {
        ttl = provisionerNode.defaultTTL;
        if (![SigHelper.share isRelayedTTL:ttl]) {
            ttl = _networkManager.defaultTtl;
        }
    }
    _segmentTtl[@(sequenceZero)] = @(ttl);
    _outgoingSegments[@(sequenceZero)] = outgoingSegments;
    [self sendSegmentsForSequenceZero:sequenceZero limit:_networkManager.retransmissionLimit];
}

- (void)cancelTXSendingSegmentedWithDestination:(UInt16)destination {
    if (_incompleteTimers && _incompleteTimers.allKeys.count > 0) {
        NSArray *keys = _incompleteTimers.allKeys;
        for (NSNumber *n in keys) {
            UInt32 key = n.intValue;
            if (((key >> 16) & 0xFFFF) == destination) {
                BackgroundTimer *t = _incompleteTimers[n];
                [t invalidate];
                [_incompleteTimers removeObjectForKey:n];
                t = nil;
                break;
            }
        }
    }
}

- (void)cancelSendingSegmentedUpperTransportPdu:(SigUpperTransportPdu *)pdu {
    /// Last 13 bits of the sequence number are known as seqZero.
    UInt16 sequenceZero = (UInt16)(pdu.sequence & 0x1FFF);
    TeLogInfo(@"Cancelling sending segments with seqZero:0x%X",sequenceZero);
    [_outgoingSegments removeObjectForKey:@(sequenceZero)];
    [_segmentTtl removeObjectForKey:@(sequenceZero)];
    BackgroundTimer *timer = _segmentTransmissionTimers[@(sequenceZero)];
    if (timer) {
        [timer invalidate];
    }
    [_segmentTransmissionTimers removeObjectForKey:@(sequenceZero)];
    timer = nil;
}

#pragma mark - private

/// This method checks the given Network PDU against replay attacks.
///
/// Unsegmented messages are checked against their sequence number.
///
/// Segmented messages are checked against the SeqAuth value of the first
/// segment of the message. Segments may be received in random order
/// and unless the message SeqAuth is always greater, the replay attack
/// is not possible.
///
/// - parameter networkPdu: The Network PDU to validate.
- (BOOL)checkAgainstReplayAttackWithNetworkPdu:(SigNetworkPdu *)networkPdu {
//    // Don't check messages sent to another Node's Elements.
//    if (![SigHelper.share isUnicastAddress:networkPdu.destination]) {
//        TeLogError(@"Address: 0x%x is not a Unicast Address.",networkPdu.destination);
//        return YES;
//    }
    UInt32 sequence = [networkPdu messageSequence];
    UInt64 receivedSeqAuth = ((UInt64)networkPdu.networkKey.ivIndex.index) << 24 | (UInt64)sequence;
    NSDictionary *oldDic = [[NSUserDefaults standardUserDefaults] objectForKey:SigDataSource.share.meshUUID];
    NSNumber *lastSequenceAuthNumber = [oldDic objectForKey:[SigHelper.share getNodeAddressString:networkPdu.source]];
    BOOL newSource = lastSequenceAuthNumber == nil;//source为node的address

//    TeLogVerbose(@"============newSource=%d,networkPdu.source=0x%x",newSource,networkPdu.source);
    if (!newSource) {
        UInt64 localSeqAuth = (UInt64)lastSequenceAuthNumber.intValue;

        // In general, the SeqAuth of the received message must be greater
        // than SeqAuth of any previously received message from the same source.
        // However, for SAR (Segmentation and Reassembly) sessions, it is
        // the SeqAuth of the message, not segment, that is being checked.
        // If SAR is active (at least one segment for the same SeqAuth has
        // been previously received), the segments may be processed in any order.
        // The SeqAuth of this message must be greater or equal to the last one.
        BOOL reassemblyInProgress = NO;
        if (networkPdu.isSegmented) {
            UInt16 sequenceZero = (UInt16)(sequence & 0x1FFF);
            UInt32 key = (UInt32)[self getKeyForAddress:networkPdu.source sequenceZero:sequenceZero];
            SigSegmentAcknowledgmentMessage *msg = _acknowledgments[@(networkPdu.source)];
            reassemblyInProgress = _incompleteSegments[@(key)] != nil || msg.sequenceZero == sequenceZero;
        }
        if (receivedSeqAuth > localSeqAuth || (reassemblyInProgress && receivedSeqAuth == localSeqAuth)) {
//            TeLogInfo(@"============SeqAuth校验通过！")
        }else{
            TeLogError(@"Discarding packet (seqAuth:%llu, expected >%llu)",receivedSeqAuth,localSeqAuth);
            return NO;
        }
    }
    // SeqAuth is valid, save the new sequence authentication value.
    NSMutableDictionary *newDic = [NSMutableDictionary dictionaryWithDictionary:oldDic];
    [newDic setValue:@(receivedSeqAuth) forKey:[SigHelper.share getNodeAddressString:networkPdu.source]];
    [[NSUserDefaults standardUserDefaults] setValue:newDic forKey:SigDataSource.share.meshUUID];
    [[NSUserDefaults standardUserDefaults] synchronize];
    return YES;
}

/// Handles the segment created from the given network PDU.
///
/// - parameters:
///   - segment: The segment to handle.
///   - networkPdu: The Network PDU from which the segment was decoded.
/// - returns: The Lower Transport PDU had it been fully assembled,
///            `nil` otherwise.
- (SigLowerTransportPdu *)assembleSegmentedMessage:(SigSegmentedMessage *)segment createdFrom:(SigNetworkPdu *)networkPdu {
    // If the received segment comes from an already completed and
    // acknowledged message, send the same ACK immediately.
    SigSegmentAcknowledgmentMessage *lastAck = _acknowledgments[@(segment.source)];
    if (lastAck && lastAck.sequenceZero == segment.sequenceZero) {
        SigNodeModel *provisionerNode = SigDataSource.share.curLocationNodeModel;
        if (provisionerNode) {
            TeLogInfo(@"Message already acknowledged, sending ACK again.");
            UInt8 ttl = 0;
            if (networkPdu.ttl > 0) {
                ttl = provisionerNode.defaultTTL;
                if (![SigHelper.share isRelayedTTL:ttl]) {
                    ttl = _networkManager.defaultTtl;
                }
            }
            [self sendAckSigSegmentAcknowledgmentMessage:lastAck withTtl:ttl];
        } else {
            [_acknowledgments removeObjectForKey:@(segment.source)];
        }
        return nil;
    }
    // Remove the last ACK. The source Node has sent a new message, so
    // the last ACK must have been received.
    [_acknowledgments removeObjectForKey:@(segment.source)];

    // A segmented message may be composed of 1 or more segments.
    if (segment.isSingleSegment) {
        SigLowerTransportPdu *message = nil;
        if ([segment isKindOfClass:[SigSegmentedAccessMessage class]]) {
            message = [[SigAccessMessage alloc] initFromSegments:@[(SigSegmentedAccessMessage *)segment]];
        } else {
            message = [[SigControlMessage alloc] initFromSegments:@[(SigSegmentedAccessMessage *)segment]];
        }
        // A single segment message may immediately be acknowledged.
        SigNodeModel *provisionerNode = SigDataSource.share.curLocationNodeModel;
        if (provisionerNode == nil) {
            TeLogError(@"curLocationNodeModel = nil.");
            return nil;
        }
        if (networkPdu.destination == provisionerNode.address) {
            UInt8 ttl = 0;
            if (networkPdu.ttl > 0) {
                ttl = provisionerNode.defaultTTL;
                if (![SigHelper.share isRelayedTTL:ttl]) {
                    ttl = _networkManager.defaultTtl;
                }
            }
            TeLogInfo(@"response last segment, sent ack.");
            [self sendAckForSegments:@[segment] withTtl:ttl];
        }
        return message;
    }else{
        // If a message is composed of multiple segments, they all need to
        // be received before it can be processed.
        UInt32 key = (UInt32)[self getKeyForAddress:networkPdu.source sequenceZero:segment.sequenceZero];
        if (_incompleteSegments[@(key)] == nil) {
            _incompleteSegments[@(key)] = [NSMutableArray array];
            for (int i=0; i<segment.count; i++) {
                [_incompleteSegments[@(key)] addObject:(SigSegmentedMessage *)[NSNull null]];
            }
        }
        if (_incompleteSegments[@(key)].count <= segment.index) {
            // Segment is invalid. We can stop here.
            TeLogInfo(@"Invalid segment.");
            return nil;
        }
        _incompleteSegments[@(key)][segment.index] = segment;

        // If all segments were received, send ACK and send the PDU to Upper
        // Transport Layer for processing.
        if ([self segmentsArrayIsComplete:_incompleteSegments[@(key)]]) {
            NSMutableArray *allSegments = _incompleteSegments[@(key)];
            [_incompleteSegments removeObjectForKey:@(key)];
            SigLowerTransportPdu *message = nil;
            SigSegmentedMessage *seg = [self firstNotAcknowledgedFrom:allSegments];
            if ([seg isKindOfClass:[SigSegmentedAccessMessage class]]) {
                message = [[SigAccessMessage alloc] initFromSegments:allSegments];
            } else {
                message = [[SigControlMessage alloc] initFromSegments:allSegments];
            }
            TeLogInfo(@"%@ received",message);
            // If the access message was targetting directly the local Provisioner...
            SigNodeModel *provisionerNode = SigDataSource.share.curLocationNodeModel;
            if (provisionerNode == nil) {
                TeLogError(@"curLocationNodeModel = nil.");
                return nil;
            }
            if (networkPdu.destination == provisionerNode.address) {
                // ...invalidate timers...
                BackgroundTimer *timer1 = [_incompleteTimers objectForKey:@(key)];
                if (timer1) {
                    [timer1 invalidate];
                }
                [_incompleteTimers removeObjectForKey:@(key)];
                BackgroundTimer *timer2 = [_acknowledgmentTimers objectForKey:@(key)];
                if (timer2) {
                    [timer2 invalidate];
                }
                [_acknowledgmentTimers removeObjectForKey:@(key)];

                // ...and send the ACK that all segments were received.
                UInt8 ttl = 0;
                if (networkPdu.ttl > 0) {
                    ttl = provisionerNode.defaultTTL;
                    if (![SigHelper.share isRelayedTTL:ttl]) {
                        ttl = _networkManager.defaultTtl;
                    }
                }
                [self sendAckForSegments:allSegments withTtl:ttl];
            }
            return message;
        } else {
            // The Provisioner shall send block acknowledgment only if the message was
            // send directly to it's Unicast Address.
            SigNodeModel *provisionerNode = SigDataSource.share.curLocationNodeModel;
            if (provisionerNode == nil) {
                TeLogError(@"curLocationNodeModel = nil.");
                return nil;
            }
            if (networkPdu.destination != provisionerNode.address) {
                TeLogDebug(@"networkPdu.destination != provisionerNode.address");
                return nil;
            }
            __weak typeof(self) weakSelf = self;

            //==========telink not need this==========//
//            // If the Lower Transport Layer receives any segment while the incomplete
//            // timer is active, the timer shall be restarted.
//            BackgroundTimer *timer1 = [_incompleteTimers objectForKey:@(key)];
//            [timer1 invalidate];
//            __weak typeof(self) weakSelf = self;
//            BackgroundTimer *timer2 = [BackgroundTimer scheduledTimerWithTimeInterval:_networkManager.incompleteMessageTimeout repeats:NO block:^(BackgroundTimer * _Nonnull t) {
//                TeLogDebug(@"Incomplete message timeout: cancelling message (src: 0x%x, seqZero: 0x%x)",(UInt16)(key >> 16),(UInt16)(key & 0x1FFF));
//                BackgroundTimer *timer1 = [weakSelf.incompleteTimers objectForKey:@(key)];
//                [timer1 invalidate];
//                [weakSelf.incompleteTimers removeObjectForKey:@(key)];
//                BackgroundTimer *timer2 = [weakSelf.acknowledgmentTimers objectForKey:@(key)];
//                [timer2 invalidate];
//                [weakSelf.acknowledgmentTimers removeObjectForKey:@(key)];
//                [weakSelf.incompleteSegments removeObjectForKey:@(key)];
//            }];
//            _incompleteTimers[@(key)] = timer2;
            //==========telink not need this==========//

            // If the Lower Transport Layer receives any segment while the acknowlegment
            // timer is inactive, it shall restart the timer. Active timer should not be restarted.
            if (_acknowledgmentTimers[@(key)] == nil) {
                UInt8 ttl = provisionerNode.defaultTTL;
                if (![SigHelper.share isRelayedTTL:ttl]) {
                    ttl = _networkManager.defaultTtl;
                }

                BackgroundTimer *timer3 = [BackgroundTimer scheduledTimerWithTimeInterval:[_networkManager acknowledgmentTimerInterval:ttl] repeats:NO block:^(BackgroundTimer * _Nonnull t) {
                    if (weakSelf.incompleteSegments[@(key)] != nil) {
                        NSMutableArray *segments = weakSelf.incompleteSegments[@(key)];
                        UInt8 ttl2 = networkPdu.ttl > 0 ? ttl : 0;
                        [weakSelf sendAckForSegments:segments withTtl:ttl2];
                    }
                    BackgroundTimer *timer4 = [weakSelf.acknowledgmentTimers objectForKey:@(key)];
                    if (timer4) {
                        [timer4 invalidate];
                    }
                    [weakSelf.acknowledgmentTimers removeObjectForKey:@(key)];
                }];
                _acknowledgmentTimers[@(key)] = timer3;
            }
            return nil;
        }
    }
}

- (void)startTXTimeoutWithAddress:(UInt16)address sequenceZero:(UInt16)sequenceZero {
    UInt32 key = (UInt32)[self getKeyForAddress:address sequenceZero:sequenceZero];
    // If the Lower Transport Layer receives any segment while the incomplete
    // timer is active, the timer shall be restarted.
    BackgroundTimer *timer1 = [_incompleteTimers objectForKey:@(key)];
    if (timer1) {
        [timer1 invalidate];
    }
    __weak typeof(self) weakSelf = self;
    BackgroundTimer *timer2 = [BackgroundTimer scheduledTimerWithTimeInterval:_networkManager.incompleteMessageTimeout repeats:NO block:^(BackgroundTimer * _Nonnull t) {
        TeLogDebug(@"Incomplete message timeout: cancelling message (src: 0x%x, seqZero: 0x%x)",(UInt16)(key >> 16),(UInt16)(key & 0x1FFF));
        BackgroundTimer *timer1 = [weakSelf.incompleteTimers objectForKey:@(key)];
        if (timer1) {
            [timer1 invalidate];
        }
        [weakSelf.incompleteTimers removeObjectForKey:@(key)];
        BackgroundTimer *timer2 = [weakSelf.acknowledgmentTimers objectForKey:@(key)];
        if (timer2) {
            [timer2 invalidate];
        }
        [weakSelf.acknowledgmentTimers removeObjectForKey:@(key)];
        [weakSelf.incompleteSegments removeObjectForKey:@(key)];
        
        // A limit has been reached and some segments were not ACK.
        NSArray *segments = weakSelf.outgoingSegments[@(sequenceZero)];
        SigSegmentedMessage *segment = [weakSelf firstNotAcknowledgedFrom:segments];
        if (segment) {
            if (segment.userInitiated && !segment.message.isAcknowledged) {
                [weakSelf.networkManager notifyAboutError:[NSError errorWithDomain:@"LowerTransportError.timeout" code:SigLowerTransportError_timeout userInfo:nil] duringSendingMessage:segment.message fromLocalElement:segment.localElement toDestination:segment.destination];
            }
            [weakSelf.networkManager.upperTransportLayer lowerTransportLayerDidSendSegmentedUpperTransportPduToDestination:segment.destination];
        }
        [weakSelf.outgoingSegments removeObjectForKey:@(sequenceZero)];
    }];
    _incompleteTimers[@(key)] = timer2;
}

/// This method handles the Segment Acknowledgment Message.
///
/// - parameter ack: The Segment Acknowledgment Message received.
- (void)handleSegmentAcknowledgmentMessage:(SigSegmentAcknowledgmentMessage *)ack {
    // 先判断发送的Segment字典内是否有对应这个ack.sequenceZero的SegmentedMessage，且这个SegmentedMessage未被设备返回ack
    SigSegmentedMessage *segment = [self firstNotAcknowledgedFrom:_outgoingSegments[@(ack.sequenceZero)]];
    if (segment == nil) {
        TeLogError(@"No location SigSegmentedMessage of ack.sequenceZero=(0x%X),ack=%@",ack.sequenceZero,ack);
        return;
    }
    // 取消这个SegmentedMessage的Transmission定时器
    BackgroundTimer *timer = _segmentTransmissionTimers[@(ack.sequenceZero)];
    if (timer) {
        [timer invalidate];
    }
    [_segmentTransmissionTimers removeObjectForKey:@(ack.sequenceZero)];
    timer = nil;
    // Is the target Node busy?
    if (ack.isBusy) {
        [_outgoingSegments removeObjectForKey:@(ack.sequenceZero)];
        if (segment.userInitiated && !segment.message.isAcknowledged) {
            [_networkManager notifyAboutError:[NSError errorWithDomain:@"LowerTransportError.busy" code:SigLowerTransportError_busy userInfo:nil] duringSendingMessage:segment.message fromLocalElement:segment.localElement toDestination:segment.destination];
        }
        return;
    }
    // Clear all acknowledged segments.
    for (int index=0; index<_outgoingSegments[@(ack.sequenceZero)].count; index++) {
        if ([ack isSegmentReceived:index]) {
            _outgoingSegments[@(ack.sequenceZero)][index] = (SigSegmentedMessage *)[NSNull null];
        }
    }
    // If all the segments were acknowledged, notify the manager.
    if ([self segmentsArrayHasMore:_outgoingSegments[@(ack.sequenceZero)]] == NO) {
        TeLogInfo(@"node response SegmentAcknowledgmentMessage,all the segments were acknowledged. ack.sequenceZero = 0x%x, ack.blockAck=0x%x",ack.sequenceZero,ack.blockAck);
        UInt32 key = (UInt32)[self getKeyForAddress:ack.source sequenceZero:ack.sequenceZero];
        BackgroundTimer *timer1 = [self.incompleteTimers objectForKey:@(key)];
        if (timer1) {
            [timer1 invalidate];
        }
        [self.incompleteTimers removeObjectForKey:@(key)];

        [_outgoingSegments removeObjectForKey:@(ack.sequenceZero)];
        [_networkManager notifyAboutDeliveringMessage:segment.message fromLocalElement:segment.localElement toDestination:segment.destination];
        [_networkManager.upperTransportLayer lowerTransportLayerDidSendSegmentedUpperTransportPduToDestination:segment.destination];
    }else{
        TeLogInfo(@"node response SegmentAcknowledgmentMessage,send again all packets that were not acknowledged.ack.blockAck=0x%x",ack.blockAck);
        // Else, send again all packets that were not acknowledged.
        [self sendSegmentsForSequenceZero:ack.sequenceZero limit:_networkManager.retransmissionLimit];
    }
}

/// This method tries to send the Segment Acknowledgment Message to the
/// given address. It will try to send if the local Provisioner is set and
/// has the Unicast Address assigned.
///
/// If the `transporter` throws an error during sending, this error will be ignored.
///
/// - parameters:
///   - segments: The array of message segments, of which at least one
///               has to be not `nil`.
///   - ttl:      Initial Time To Live (TTL) value.
- (void)sendAckForSegments:(NSArray <SigSegmentedMessage *>*)segments withTtl:(UInt8)ttl {
    SigSegmentAcknowledgmentMessage *ack = [[SigSegmentAcknowledgmentMessage alloc] initForSegments:segments];
    ack.ivIndex = SigDataSource.share.curNetkeyModel.ivIndex;
    ack.networkKey = SigDataSource.share.curNetkeyModel;
    if ([self segmentsArrayIsComplete:segments]) {
        _acknowledgments[@(ack.destination)] = ack;
    }
    [self sendAckSigSegmentAcknowledgmentMessage:ack withTtl:ttl];
}

/// Sends the given ACK on the global background queue.
///
/// - parameters:
///   - ack: The Segment Acknowledgment Message to sent.
///   - ttl: Initial Time To Live (TTL) value.
- (void)sendAckSigSegmentAcknowledgmentMessage:(SigSegmentAcknowledgmentMessage *)ack withTtl:(UInt8)ttl {
//    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0), ^{
//        TeLogInfo(@"sending ACK=%@ ,from ack.source :0x%x, to destination :0x%x, ack.sequenceZero=0x%x",ack,ack.source,ack.destination,ack.sequenceZero);
//        [self.networkManager.networkLayer sendLowerTransportPdu:ack ofType:SigPduType_networkPdu withTtl:ttl];
//    });
    dispatch_async(SigMeshLib.share.queue, ^{
        TeLogInfo(@"sending ACK=%@ ,from ack.source :0x%x, to destination :0x%x, ack.sequenceZero=0x%x",ack,ack.source,ack.destination,ack.sequenceZero);
        [self.networkManager.networkLayer sendLowerTransportPdu:ack ofType:SigPduType_networkPdu withTtl:ttl];
    });
}

/// Sends all non-`nil` segments from `outgoingSegments` map from the given
/// `sequenceZero` key.
///
/// - parameter sequenceZero: The key to get segments from the map.
- (void)sendSegmentsForSequenceZero:(UInt16)sequenceZero limit:(int)limit {
    TeLogVerbose(@"sequenceZero=0x%x,limit=%d",sequenceZero,limit);
    NSArray *array = _outgoingSegments[@(sequenceZero)];
    NSInteger count = array.count;
    UInt8 ttl = (UInt8)[_segmentTtl[@(sequenceZero)] intValue];
    SigNodeModel *provisionerNode = SigDataSource.share.curLocationNodeModel;
    if (count == 0 || provisionerNode == nil) {
        return;
    }
    /// Segment Acknowledgment Message is expected when the message is targetting
    /// a Unicast Address.
    BOOL ackExpected = NO;

    UInt16 destination = 0;
    TeLogVerbose(@"==========发送seg=%@开始",array);

    // Send all the segments that have not been acknowledged yet.
    for (int i=0; i<count; i++) {
        SigSegmentedMessage *segment = array[i];
        if (![segment isEqual:[NSNull null]]) {
            if (destination == 0) {
                destination = segment.destination;
            }
            ackExpected = [SigHelper.share isUnicastAddress:segment.destination];
            [_networkManager.networkLayer sendLowerTransportPdu:segment ofType:SigPduType_networkPdu withTtl:ttl ivIndex:segment.ivIndex];
            //==========test==========//
            //因为非直连设备地址的segment包需要在mesh网络m内部进行转发，且设备不一定存在ack返回。（BLOBChunkTransfer）
            if (segment.destination != SigMeshLib.share.dataSource.getCurrentConnectedNode.address) {
                [NSThread sleepForTimeInterval:SigMeshLib.share.networkTransmitInterval];
            }
            //==========test==========//
        }
    }
    TeLogVerbose(@"==========发送seg count=%d结束",count);
    __weak typeof(self) weakSelf = self;

    //==========telink need this==========//
    if (!ackExpected && destination) {
        [weakSelf.networkManager.upperTransportLayer lowerTransportLayerDidSendSegmentedUpperTransportPduToDestination:destination];
    }
    //==========telink not need this==========//
    // It is recommended to send all Lower Transport PDUs that are being sent
    // to a Group or Virtual Address mutliple times, introducing small random
    // delays between repetitions. The specification does not say what small
    // random delay is, so assuming 0.5-1.5 second.
//    if (!ackExpected) {
//            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)([SigHelper.share getRandomfromA:0.500 toB:1.500] * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
//            UInt16 destination = 0;
//            for (int i=0; i<array.count; i++) {
//                SigSegmentedMessage *segment = array[i];
//                if (![segment isEqual:[NSNull null]]) {
//                    if (destination == 0) {
//                        destination = segment.destination;
//                    }
//                    [weakSelf.networkManager.networkLayer sendLowerTransportPdu:segment ofType:SigPduType_networkPdu withTtl:ttl];
//                }
//            }
//            if (destination) {
//                [weakSelf.networkManager.upperTransportLayer lowerTransportLayerDidSendSegmentedUpperTransportPduToDestination:destination];
//            }
//        });
//    }
    //==========telink not need this==========//

    BackgroundTimer *timer = [_segmentTransmissionTimers objectForKey:@(sequenceZero)];
    if (timer) {
        [timer invalidate];
    }
    [_segmentTransmissionTimers removeObjectForKey:@(sequenceZero)];
    NSArray *segments = _outgoingSegments[@(sequenceZero)];
    BOOL hasMore = [self segmentsArrayHasMore:segments];
    if (ackExpected && segments && hasMore) {
        if (limit > 0) {
            NSTimeInterval interval = [_networkManager transmissionTimerInteral:ttl];
            BackgroundTimer *timer2 = [BackgroundTimer scheduledTimerWithTimeInterval:interval repeats:NO block:^(BackgroundTimer * _Nonnull t) {
                [weakSelf sendSegmentsForSequenceZero:sequenceZero limit:limit-1];
            }];
            _segmentTransmissionTimers[@(sequenceZero)] = timer2;
        } else {
            // A limit has been reached and some segments were not ACK.
            SigSegmentedMessage *segment = [self firstNotAcknowledgedFrom:segments];
            if (segment) {
                if (segment.userInitiated && !segment.message.isAcknowledged) {
                    [_networkManager notifyAboutError:[NSError errorWithDomain:@"LowerTransportError.timeout" code:SigLowerTransportError_timeout userInfo:nil] duringSendingMessage:segment.message fromLocalElement:segment.localElement toDestination:segment.destination];
                }
                [_networkManager.upperTransportLayer lowerTransportLayerDidSendSegmentedUpperTransportPduToDestination:segment.destination];
            }
            [_outgoingSegments removeObjectForKey:@(sequenceZero)];
        }
    } else
    {
        // All segments have been successfully sent to a Group Address.
        SigSegmentedMessage *segment = [self firstNotAcknowledgedFrom:array];
        if (segment) {
            [_networkManager notifyAboutDeliveringMessage:segment.message fromLocalElement:segment.localElement toDestination:segment.destination];
        }
        [_outgoingSegments removeObjectForKey:@(sequenceZero)];
    }
}

- (void)sendSegment:(NSTimer *)timer {
    NSDictionary *userInfo = timer.userInfo;
    NSArray *segments = userInfo[@"segments"];
    NSInteger ttl = [userInfo[@"ttl"] integerValue];
    for (int i=0; i<segments.count; i++) {
        SigSegmentedMessage *segment = segments[i];
        [_networkManager.networkLayer sendLowerTransportPdu:segment ofType:SigPduType_networkPdu withTtl:ttl];
    }
    if (timer) {
        [timer invalidate];
    }
}

- (void)sendSegment2:(NSTimer *)timer {
    NSDictionary *userInfo = timer.userInfo;
    UInt16 sequenceZero = [userInfo[@"sequenceZero"] intValue];
    int limit = [userInfo[@"limit"] intValue];
    [self sendSegmentsForSequenceZero:sequenceZero limit:limit];
}

/// Returns the key used in maps in Lower Transport Layer to keep
/// segments received to or from given source address.
- (UInt32)getKeyForAddress:(UInt16)address sequenceZero:(UInt16)sequenceZero {
    return ((UInt32)address << 16) | (UInt32)(sequenceZero & 0x1FFF);
}

/// This method tries to send the Segment Acknowledgment Message to the
/// given address. It will try to send if the local Provisioner is set and
/// has the Unicast Address assigned.
///
/// If the `transporter` throws an error during sending, this error will be ignored.
///
/// - parameter segments:   The array of message segments, of which at least one
///                         has to be not `nil`.
/// - parameter networkKey: The Network Key to be used to encrypt the message on
///                         on Network Layer.
/// - parameter ttl:        Initial Time To Live (TTL) value.
- (void)sendAckForSegments:(NSArray <SigSegmentedMessage *>*)segments usingNetworkKey:(SigNetkeyModel *)networkKey withTtl:(UInt8)ttl {
    SigSegmentAcknowledgmentMessage *ack = [[SigSegmentAcknowledgmentMessage alloc] initForSegments:segments];
    if ([self segmentsArrayIsComplete:segments]) {
        _acknowledgments[@(ack.destination)] = ack;
    }
    [_networkManager.networkLayer sendLowerTransportPdu:ack ofType:SigPduType_networkPdu withTtl:ttl];
}

/// Returns whether all the segments were received.
- (BOOL)segmentsArrayIsComplete:(NSArray *)array {
    BOOL tem = NO;
    for (NSObject *obj in array) {
        if ([obj isEqual:[NSNull null]]) {
            tem = YES;
            break;
        }
    }
    return !tem;
}

/// Returns whether some segments were not yet acknowledged.
- (BOOL)segmentsArrayHasMore:(NSArray *)array {
    BOOL tem = NO;
    for (NSObject *obj in array) {
        if (![obj isEqual:[NSNull null]]) {
            tem = YES;
            break;
        }
    }
    return tem;
}

- (SigSegmentedMessage *)firstNotAcknowledgedFrom:(NSArray *)array{
    SigSegmentedMessage *segment = nil;
    for (SigSegmentedMessage *tem in array) {
        if (![tem isEqual:[NSNull null]]) {
            segment = tem;
            break;
        }
    }
    return segment;
}

//limit=10
- (void)sendSegmentsForSequenceZero:(UInt16)sequenceZero {
    [self sendSegmentsForSequenceZero:sequenceZero limit:10];
}

@end
