/********************************************************************************************************
 * @file     SigUpperTransportLayer.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/9/16
 *
 * @par     Copyright (c) [2021], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

/*
 The upper transport layer encrypts, decrypts, and authenticates application data and is designed
 to provide confidentiality of access messages. It also defines how transport control messages
 are used to manage the upper transport layer between nodes, including when used by the Friend feature.
 */
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
/// The upper transport layer shall not transmit a new segmented Upper Transport PDU to a given destination until the previous Upper Transport PDU to that destination has been either completed or cancelled.
///
/// This map contains queues of messages targetting each destination.
@property (nonatomic,strong) NSMutableDictionary <NSNumber *,NSMutableArray <SigUpperTransportModel *>*>*queues;

@end

@implementation SigUpperTransportLayer

/// Initialize SigUpperTransportLayer object.
/// @param networkManager The SigNetworkManager object.
/// @returns return `nil` when initialize SigUpperTransportLayer object fail.
- (instancetype)initWithNetworkManager:(SigNetworkManager *)networkManager {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _networkManager = networkManager;
        _defaults = [NSUserDefaults standardUserDefaults];
        _queues = [NSMutableDictionary dictionary];
    }
    return self;
}

/// Handles received Lower Transport PDU.
/// Depending on the PDU type, the message will be either propagated to Access Layer, or handled internally.
/// @param lowerTransportPdu The Lower Trasport PDU received.
- (void)handleLowerTransportPdu:(SigLowerTransportPdu *)lowerTransportPdu {
    switch (lowerTransportPdu.type) {
        case SigLowerTransportPduType_accessMessage:
            {
//                TeLogDebug(@"lowerTransportPdu.upperTransportPdu=%@,length=%d",[LibTools convertDataToHexStr:lowerTransportPdu.transportPdu],lowerTransportPdu.transportPdu.length);
                SigAccessMessage *accessMessage = (SigAccessMessage *)lowerTransportPdu;
                NSDictionary *dict = [SigUpperTransportPdu decodeAccessMessage:accessMessage forMeshNetwork:SigMeshLib.share.dataSource];
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
        case SigLowerTransportPduType_transportControlMessage:
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

/// Encrypts the Access PDU using given key set and sends it down to Lower Transport Layer.
/// @param accessPdu The Access PDU to be sent.
/// @param initialTtl The initial TTL (Time To Live) value of the message. If `nil`, the default Node TTL will be used.
/// @param keySet The set of keys to encrypt the message with.
/// @param command The command of the message.
- (void)sendAccessPdu:(SigAccessPdu *)accessPdu withTtl:(UInt8)initialTtl usingKeySet:(SigKeySet *)keySet command:(SDKLibCommand *)command {
    UInt32 sequence = [SigMeshLib.share.dataSource getSequenceNumberUInt32];
    SigNetkeyModel *networkKey = command.curNetkey;
    SigUpperTransportPdu *pdu = [[SigUpperTransportPdu alloc] initFromAccessPdu:accessPdu usingKeySet:keySet ivIndex:command.curIvIndex sequence:sequence];
    _networkManager.upperTransportLayer.upperTransportPdu = pdu;
//    TeLogVerbose(@"Sending %@ encrypted using key: %@,pdu.transportPdu=%@",pdu,keySet,pdu.transportPdu);
    
    //1.修正当前进行分包的pdu.unsegmentedMessageLowerTransportPDUMaxLength的值。
    //2.如果是SigMeshLib.share.dataSource.security==SigMeshMessageSecurityHigh，必得是发送segment。
    //3.如果客户指定了command.sendBySegmentPDU = YES，也强制发送segment。默认是command.sendBySegmentPDU = NO，优先发送unsegment。
    //4.如果command.sendBySegmentPDU = NO，根据pdu.unsegmentedMessageLowerTransportPDUMaxLength和类定义的消息类message.isSegmented判断是否进行segment分包。
    if (SigMeshLib.share.dataSource.telinkExtendBearerMode == SigTelinkExtendBearerMode_extendGATTOnly && accessPdu.destination.address != SigMeshLib.share.dataSource.unicastAddressOfConnected) {
        pdu.unsegmentedMessageLowerTransportPDUMaxLength = kUnsegmentedMessageLowerTransportPDUMaxLength;
    } else {
        pdu.unsegmentedMessageLowerTransportPDUMaxLength = command.unsegmentedMessageLowerTransportPDUMaxLength;
    }
    BOOL isSegmented = SigMeshLib.share.dataSource.security == SigMeshMessageSecurityHigh;
    if (!isSegmented) {
        if (command.sendBySegmentPDU) {
            isSegmented = YES;
        } else {
            isSegmented = pdu.transportPdu.length > pdu.unsegmentedMessageLowerTransportPDUMaxLength || accessPdu.message.isSegmented;
        }
    }
    
    if (isSegmented) {
        TeLogInfo(@"sending segment pdu.");
        // Enqueue the PDU. If the queue was empty, the PDU will be sent
        // immediately.
        [self enqueueSigUpperTransportPdu:pdu initialTtl:initialTtl networkKey:networkKey ivIndex:command.curIvIndex];
    } else {
        TeLogInfo(@"sending unsegment pdu.");
        [_networkManager.lowerTransportLayer sendUnsegmentedUpperTransportPdu:pdu withTtl:initialTtl usingNetworkKey:networkKey ivIndex:command.curIvIndex];
    }
}

/// Cancels sending all segmented messages matching given handle.
/// Unsegmented messages are sent almost instantaneously and cannot be cancelled.
/// @param handle The message handle.
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

/// A callback called by the lower transport layer when the segmented PDU has been sent to the given destination.
/// This method removes the sent PDU from the queue and initiates sending a next one, had it been enqueued.
/// @param destination The destination address.
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

/// Sends the next enqueded PDU.
///
/// If the queue for the given destination does not exist or is empty, this method does nothing.
///
/// @param destination The destination address.
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
