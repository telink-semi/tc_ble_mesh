//
//  SigLowerTransportLayer.m
//  TelinkSigMeshLib
//
//  Created by Liangjiazhi on 2019/9/16.
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
#import "SigPdu.h"

@interface SigLowerTransportLayer ()
@property (nonatomic,strong) NSUserDefaults *defaults;
@property (nonatomic,strong) dispatch_queue_t mutex;

@end

@implementation SigLowerTransportLayer

- (instancetype)initWithNetworkManager:(SigNetworkManager *)networkManager {
    if (self = [super init]) {
        _networkManager = networkManager;
        _meshNetwork = networkManager.meshNetwork;
        _defaults = [NSUserDefaults standardUserDefaults];
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
    // Some validation, just to be sure. This should pass for sure.
    if (networkPdu.transportPdu.length == 0) {
        TeLogError(@"networkPdu.transportPdu.length == 0");
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
        if (segmented) {
            if (networkPdu.type == SigLowerTransportPduType_accessMessage) {
                SigSegmentedAccessMessage *segment = [[SigSegmentedAccessMessage alloc] initFromSegmentedPdu:networkPdu];
                if (segmented) {
                    SigLowerTransportPdu *pdu = [weakSelf assembleSegmentedMessage:segment createdFrom:networkPdu];
                    if (pdu) {
                        [weakSelf.networkManager.upperTransportLayer handleLowerTransportPdu:pdu];
                    } else {
//                        TeLogError(@"============1.3.pdu = nil.");
                    }
                }else{
                    TeLogError(@"segmented = nil.");
                }
            }else if (networkPdu.type == SigLowerTransportPduType_controlMessage) {
                SigSegmentedControlMessage *segment = [[SigSegmentedControlMessage alloc] initFromSegmentedPdu:networkPdu];
                if (segmented) {
                    TeLogInfo(@"%@ receieved (decrypted using key: %@)",segment,segment.networkKey);
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
                    TeLogInfo(@"%@ receieved (decrypted using key: %@)",accessMessage,accessMessage.networkKey);
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
                        TeLogInfo(@"SigSegmentAcknowledgmentMessage receieved =%@ (decrypted using key: %@)",ack,ack.networkKey);
                        [weakSelf handleSegmentAcknowledgmentMessage:ack];
                    } else {
                        TeLogError(@"ack = nil.");
                    }
                } else {
                    SigControlMessage *controlMessage = [[SigControlMessage alloc] initFromNetworkPdu:networkPdu];
                    if (controlMessage) {
                        TeLogInfo(@"%@ receieved (decrypted using key: %@)",controlMessage,controlMessage.networkKey);
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

- (void)sendUnsegmentedUpperTransportPdu:(SigUpperTransportPdu *)pdu withTtl:(UInt8)initialTtl usingNetworkKey:(SigNetkeyModel *)networkKey {
    SigNodeModel *provisionerNode = _meshNetwork.curLocationNodeModel;
    if (provisionerNode == nil) {
        TeLogError(@"_meshNetwork.curLocationNodeModel = nil.");
        return;
    }
    UInt8 ttl = initialTtl;
    if (![SigHelper.share isValidTTL:ttl]) {
        ttl = provisionerNode.defaultTTL;
        if (![SigHelper.share isValidTTL:ttl]) {
            ttl = _networkManager.defaultTtl;
        }
    }
    SigAccessMessage *message = [[SigAccessMessage alloc] initFromUnsegmentedUpperTransportPdu:pdu usingNetworkKey:networkKey];
    [_networkManager.networkLayer sendLowerTransportPdu:message ofType:SigPduType_networkPdu withTtl:ttl];
    [_networkManager notifyAboutDeliveringMessage:pdu.message fromLocalElement:pdu.localElement toDestination:pdu.destination];
}

- (void)sendSegmentedUpperTransportPdu:(SigUpperTransportPdu *)pdu withTtl:(UInt8)initialTtl usingNetworkKey:(SigNetkeyModel *)networkKey {
    SigNodeModel *provisionerNode = _meshNetwork.curLocationNodeModel;
    if (provisionerNode == nil) {
        TeLogError(@"_meshNetwork.curLocationNodeModel = nil.");
        return;
    }
    /// Last 13 bits of the sequence number are known as seqZero.
    UInt16 sequenceZero = (UInt16)(pdu.sequence & 0x1FFF);
    /// Number of segments to be sent.
    NSInteger count = (pdu.transportPdu.length + 11) / 12;
    // Create all segments to be sent.
    NSMutableArray *outgoingSegments = [NSMutableArray array];
    for (int i=0; i<count; i++) {
        SigSegmentedAccessMessage *msg = [[SigSegmentedAccessMessage alloc] initFromUpperTransportPdu:pdu usingNetworkKey:networkKey offset:(UInt8)i];
        [outgoingSegments addObject:msg];
    }
    UInt8 ttl = initialTtl;
    if (![SigHelper.share isValidTTL:ttl] || ttl == 0) {
        ttl = provisionerNode.defaultTTL;
        if (![SigHelper.share isValidTTL:ttl]) {
            ttl = _networkManager.defaultTtl;
        }
    }
    _segmentTtl[@(sequenceZero)] = @(ttl);
    _outgoingSegments[@(sequenceZero)] = outgoingSegments;
    [self sendSegmentsForSequenceZero:sequenceZero limit:_networkManager.retransmissionLimit];
}

- (void)cancelSendingSegmentedUpperTransportPdu:(SigUpperTransportPdu *)pdu {
    /// Last 13 bits of the sequence number are known as seqZero.
    UInt16 sequenceZero = (UInt16)(pdu.sequence & 0x1FFF);
    TeLogInfo(@"Cancelling sending segments with seqZero:%d",sequenceZero);
    [_outgoingSegments removeObjectForKey:@(sequenceZero)];
    [_segmentTtl removeObjectForKey:@(sequenceZero)];
    BackgroundTimer *timer = _segmentTransmissionTimers[@(sequenceZero)];
    [timer invalidate];
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
    BOOL newSource = [_defaults objectForKey:[SigHelper.share getNodeAddressString:networkPdu.source]]==nil;//source为node的address
//    TeLogDebug(@"============newSource=%d,networkPdu.source=%d",newSource,networkPdu.source);
    if (!newSource) {
        NSInteger lastSequence = [_defaults integerForKey:[SigHelper.share getNodeAddressString:networkPdu.source]];
        UInt64 localSeqAuth = ((UInt64)networkPdu.networkKey.ivIndex.index) << 24 | (UInt64)lastSequence;
        UInt64 receivedSeqAuth = ((UInt64)networkPdu.networkKey.ivIndex.index) << 24 | (UInt64)sequence;

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
            TeLogInfo(@"============")
        }else{
            TeLogError(@"Discarding packet (seqAuth:%llu, expected >%llu)",receivedSeqAuth,localSeqAuth);
            return NO;
        }
    }
    // SeqAuth is valid, save the new sequence authentication value.
    //2019年11月01日16:34:37，更新本地节点的sno
    [SigDataSource.share updateCurrentProvisionerIntSequenceNumber:sequence];
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
//        TeLogInfo(@"================1.4.lastAck=%@,lastAck.sequenceZero=0x%x",lastAck,lastAck.sequenceZero);
        SigNodeModel *provisionerNode = _meshNetwork.curLocationNodeModel;
        if (provisionerNode) {
            TeLogInfo(@"Message already acknowledged, sending ACK again.");
            UInt8 ttl = 0;
            if (networkPdu.ttl > 0) {
                ttl = provisionerNode.defaultTTL;
                if (![SigHelper.share isValidTTL:ttl]) {
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
//        TeLogInfo(@"================1.5. %@ received",message);
        // A single segment message may immediately be acknowledged.
        SigNodeModel *provisionerNode = _meshNetwork.curLocationNodeModel;
        if (provisionerNode == nil) {
            TeLogError(@"_meshNetwork.curLocationNodeModel = nil.");
            return nil;
        }
        if (networkPdu.destination == provisionerNode.address) {
            UInt8 ttl = 0;
            if (networkPdu.ttl > 0) {
                ttl = provisionerNode.defaultTTL;
                if (![SigHelper.share isValidTTL:ttl]) {
                    ttl = _networkManager.defaultTtl;
                }
            }
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
            SigNodeModel *provisionerNode = _meshNetwork.curLocationNodeModel;
            if (provisionerNode == nil) {
                TeLogError(@"_meshNetwork.curLocationNodeModel = nil.");
                return nil;
            }
//            TeLogInfo(@"================1.7.all segments were received,networkPdu.destination=0x%x,provisionerNode.address=0x%x,message.upperTransportPdu=%@,message.upperTransportPdu.length=0x%lx",networkPdu.destination,provisionerNode.address,message.upperTransportPdu,message.upperTransportPdu.length);
            if (networkPdu.destination == provisionerNode.address) {
                // ...invalidate timers...
                BackgroundTimer *timer1 = [_incompleteTimers objectForKey:@(key)];
                [timer1 invalidate];
                [_incompleteTimers removeObjectForKey:@(key)];
                BackgroundTimer *timer2 = [_acknowledgmentTimers objectForKey:@(key)];
                [timer2 invalidate];
                [_acknowledgmentTimers removeObjectForKey:@(key)];

                // ...and send the ACK that all segments were received.
                UInt8 ttl = 0;
                if (networkPdu.ttl > 0) {
                    ttl = provisionerNode.defaultTTL;
                    if (![SigHelper.share isValidTTL:ttl]) {
                        ttl = _networkManager.defaultTtl;
                    }
                }
                [self sendAckForSegments:allSegments withTtl:ttl];
            }
//            TeLogInfo(@"================1.8.all segments were received,networkPdu.destination=0x%x,provisionerNode.address=0x%x,message.upperTransportPdu=%@,message.upperTransportPdu.length=0x%lx",networkPdu.destination,provisionerNode.address,message.upperTransportPdu,(unsigned long)message.upperTransportPdu.length);
            return message;
        } else {
//            TeLogInfo(@"================1.7.wait segment.");
            // The Provisioner shall send block acknowledgment only if the message was
            // send directly to it's Unicast Address.
            SigNodeModel *provisionerNode = _meshNetwork.curLocationNodeModel;
            if (provisionerNode == nil) {
                TeLogError(@"_meshNetwork.curLocationNodeModel = nil.");
                return nil;
            }
            if (networkPdu.destination != provisionerNode.address) {
                TeLogDebug(@"networkPdu.destination != provisionerNode.address");
                return nil;
            }
            // If the Lower Transport Layer receives any segment while the incomplete
            // timer is active, the timer shall be restarted.
            BackgroundTimer *timer1 = [_incompleteTimers objectForKey:@(key)];
            [timer1 invalidate];
            __weak typeof(self) weakSelf = self;
            BackgroundTimer *timer2 = [BackgroundTimer scheduledTimerWithTimeInterval:_networkManager.incompleteMessageTimeout repeats:NO block:^(BackgroundTimer * _Nonnull t) {
                TeLogDebug(@"Incomplete message timeout: cancelling message (src: 0x%x, seqZero: 0x%x)",(UInt16)(key >> 16),(UInt16)(key & 0x1FFF));
                BackgroundTimer *timer1 = [weakSelf.incompleteTimers objectForKey:@(key)];
                [timer1 invalidate];
                [weakSelf.incompleteTimers removeObjectForKey:@(key)];
                BackgroundTimer *timer2 = [weakSelf.acknowledgmentTimers objectForKey:@(key)];
                [timer2 invalidate];
                [weakSelf.acknowledgmentTimers removeObjectForKey:@(key)];
                [weakSelf.incompleteSegments removeObjectForKey:@(key)];
            }];
            _incompleteTimers[@(key)] = timer2;
            // If the Lower Transport Layer receives any segment while the acknowlegment
            // timer is inactive, it shall restart the timer. Active timer should not be restarted.
            if (_acknowledgmentTimers[@(key)] == nil) {
                UInt8 ttl = provisionerNode.defaultTTL;
                if (![SigHelper.share isValidTTL:ttl]) {
                    ttl = _networkManager.defaultTtl;
                }

                BackgroundTimer *timer3 = [BackgroundTimer scheduledTimerWithTimeInterval:[_networkManager acknowledgmentTimerInterval:ttl] repeats:NO block:^(BackgroundTimer * _Nonnull t) {
                    if (weakSelf.incompleteSegments[@(key)] != nil) {
                        NSMutableArray *segments = weakSelf.incompleteSegments[@(key)];
                        UInt8 ttl2 = networkPdu.ttl > 0 ? ttl : 0;
                        [weakSelf sendAckForSegments:segments withTtl:ttl2];
                    }
                    BackgroundTimer *timer4 = [weakSelf.acknowledgmentTimers objectForKey:@(key)];
                    [timer4 invalidate];
                    [weakSelf.acknowledgmentTimers removeObjectForKey:@(key)];
                }];
                _acknowledgmentTimers[@(key)] = timer3;
            }
            return nil;
        }
    }
}

/// This method handles the Segment Acknowledgment Message.
///
/// - parameter ack: The Segment Acknowledgment Message received.
- (void)handleSegmentAcknowledgmentMessage:(SigSegmentAcknowledgmentMessage *)ack {
    // 先判断发送的Segment字典内是否有对应这个ack.sequenceZero的SegmentedMessage，且这个SegmentedMessage未被设备返回ack
    SigSegmentedMessage *segment = [self firstNotAcknowledgedFrom:_outgoingSegments[@(ack.sequenceZero)]];
    if (segment == nil) {
        TeLogError(@"segment == nil");
        return;
    }
    // 取消这个SegmentedMessage的Transmission定时器
    BackgroundTimer *timer = _segmentTransmissionTimers[@(ack.sequenceZero)];
    [timer invalidate];
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
        [_outgoingSegments removeObjectForKey:@(ack.sequenceZero)];
        [_networkManager notifyAboutDeliveringMessage:segment.message fromLocalElement:segment.localElement toDestination:segment.destination];
        [_networkManager.upperTransportLayer lowerTransportLayerDidSendSegmentedUpperTransportPduToDestination:segment.destination];
    }else{
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
    TeLogInfo(@"sending ACK=%@ ,from ack.source :0x%x, to destination :0x%x, ack.sequenceZero=0x%x",ack,ack.source,ack.destination,ack.sequenceZero);
    [self.networkManager.networkLayer sendLowerTransportPdu:ack ofType:SigPduType_networkPdu withTtl:ttl];
}

/// Sends all non-`nil` segments from `outgoingSegments` map from the given
/// `sequenceZero` key.
///
/// - parameter sequenceZero: The key to get segments from the map.
- (void)sendSegmentsForSequenceZero:(UInt16)sequenceZero limit:(int)limit {
    NSArray *array = _outgoingSegments[@(sequenceZero)];
    NSInteger count = array.count;
    UInt8 ttl = (UInt8)[_segmentTtl[@(sequenceZero)] intValue];
    SigNodeModel *provisionerNode = _meshNetwork.curLocationNodeModel;
    if (count == 0 || provisionerNode == nil) {
        return;
    }
    /// Segment Acknowledgment Message is expected when the message is targetting
    /// a Unicast Address.
    BOOL ackExpected = NO;

    UInt16 destination = 0;
    
    // Send all the segments that have not been acknowledged yet.
    for (int i=0; i<count; i++) {
        SigSegmentedMessage *segment = array[i];
        if (![segment isEqual:[NSNull null]]) {
            if (destination == 0) {
                destination = segment.destination;
            }
            ackExpected = [SigHelper.share isUnicastAddress:segment.destination];
            [_networkManager.networkLayer sendLowerTransportPdu:segment ofType:SigPduType_networkPdu withTtl:ttl];
            //==========test==========//
            //因为非直连设备地址的segment包需要在mesh网络m内部进行转发，且设备不一定存在ack返回。（objectChunkTransfer）
            if (segment.destination != SigMeshLib.share.dataSource.getCurrentConnectedNode.address) {
                [NSThread sleepForTimeInterval:SigMeshLib.share.networkTransmitInterval];
            }
            //==========test==========//
        }
    }
    TeLogInfo(@"==========发送seg count=%d结束",count);
    __weak typeof(self) weakSelf = self;

    //==========telink need this==========//
    if (!ackExpected && destination) {
        TeLogWarn(@"lowerTransportLayerDidSendSegmentedUpperTransportPduToDestination");
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
    [timer invalidate];
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
    [timer invalidate];
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
