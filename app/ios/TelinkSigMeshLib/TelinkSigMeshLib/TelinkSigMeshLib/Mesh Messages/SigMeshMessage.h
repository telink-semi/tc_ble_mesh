/********************************************************************************************************
 * @file     SigMeshMessage.h
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
//  SigMeshMessage.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/8/15.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface SigBaseMeshMessage : NSObject

/// Message parameters as Data.
@property (nonatomic,strong) NSData *parameters;

@end


/// The base class of every mesh message. Mesh messages can be sent and
/// and recieved from the mesh network. For messages with the opcode known
/// during compilation a `StaticMeshMessage` protocol should be preferred.
///
/// Parameters `security` and `isSegmented` are checked and should be set
/// only for outgoing messages.
@interface SigMeshMessage : SigBaseMeshMessage

/// The message Op Code.
@property (nonatomic,assign) UInt32 opCode;

/// 0 means have response message, other means haven't response message.
//@property (nonatomic,assign) UInt32 responseOpCode;

/// Returns whether the message should be sent or has been sent using
/// 32-bit or 64-bit TransMIC value. By default `.low` is returned.
///
/// Only Segmented Access Messages can use 64-bit MIC. If the payload
/// is shorter than 11 bytes, make sure you return `true` from
/// `isSegmented`, otherwise this field will be ignored.
@property (nonatomic,assign,readonly) SigMeshMessageSecurity security;

/// Returns whether the message should be sent or was sent as
/// Segmented Access Message. By default, this parameter returns
/// `true` if payload (Op Code and parameters) size is longer than 11 bytes
/// and `false` otherwise.
///
/// To force segmentation for shorter messages return `true` despite
/// payload length. If payload size is longer than 11 bytes this
/// field is not checked as the message must be segmented.
@property (nonatomic,assign,readonly) BOOL isSegmented;

/// This initializer should construct the message based on the received
/// parameters.
///
/// - parameter parameters: The Access Layer parameters.
- (instancetype)initWithParameters:(NSData *)parameters;

- (UInt8)getTransportMicSizeOfMeshMessageSecurity:(SigMeshMessageSecurity)meshMessageSecurity;

- (NSData *)accessPdu;
- (BOOL)isVendorMessage;
- (void)showMeshMessageSecurity:(SigMeshMessageSecurity)meshMessageSecurity;
/// Whether the message is an acknowledged message, or not.
- (BOOL)isAcknowledged;
@end


/// A mesh message containing the operation status.
@interface SigStatusMessage : SigMeshMessage
/// Returns whether the operation was successful or not.
@property (nonatomic,assign,readonly) BOOL isSuccess;
/// The status as String.
@property (nonatomic,strong,readonly) NSString *message;
@end


/// A message with Transaction Identifier.
///
/// The Transaction Identifier will automatically be set and incremented
/// each time a message is sent. The counter is reuesed for all types that
/// extend this protocol.
@interface SigTransactionMessage : SigMeshMessage
/// Transaction identifier. If not set, this field will automatically
/// be set when the message is being sent or received.
@property (nonatomic,assign) UInt8 tid;
@property (nonatomic,assign) BOOL isInitTid;
@property (nonatomic,assign) BOOL continueTransaction;
@end


@interface SigTransitionMessage : SigMeshMessage
/// The Transition Time field identifies the time that an element will
/// take to transition to the target state from the present state.
@property (nonatomic,strong,readonly) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign,readonly) UInt8 delay;
@end


@interface SigTransitionStatusMessage : SigMeshMessage
/// The Remaining Time field identifies the time that an element will
/// take to transition to the target state from the present state.
@property (nonatomic,strong,readonly) SigTransitionTime *remainingTime;
@end

/// The base class for acknowledged messages.
///
/// An acknowledged message is transmitted and acknowledged by each
/// receiving element by responding to that message. The response is
/// typically a status message. If a response is not received within
/// an arbitrary time period, the message will be retransmitted
/// automatically until the timeout occurs.
@interface SigAcknowledgedMeshMessage : SigMeshMessage
@property (nonatomic,assign) UInt32 responseOpCode;
@end

@interface SigStaticMeshMessage : SigMeshMessage
@end

@interface SigUnknownMessage : SigMeshMessage

@end

@interface SigIniMeshMessage : SigMeshMessage
@property (nonatomic,assign) UInt32 responseOpCode;
@end



NS_ASSUME_NONNULL_END
