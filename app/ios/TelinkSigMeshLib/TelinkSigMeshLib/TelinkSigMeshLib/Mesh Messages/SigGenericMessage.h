/********************************************************************************************************
* @file     SigGenericMessage.h
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
//  SigGenericMessage.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/11/12.
//  Copyright © 2019 Telink. All rights reserved.
//

NS_ASSUME_NONNULL_BEGIN

@class SigGenericPowerRangeSet,SigGenericPowerRangeSetUnacknowledged,SigTimeModel,SchedulerModel,SigSensorDescriptorModel;

@interface SigGenericMessage : SigStaticMeshMessage

/// tid和continueTransaction为指定类型的SigGenericMessage子类中才有实际意义的（返回该指令是否需要TID）
- (BOOL)isTransactionMessage;

/// Transaction identifier. If not set, this field will automatically be set when the message is being sent or received.
@property (nonatomic,assign) UInt8 tid;
/// Whether the message should start a new transaction.
///
/// The messages within a transaction carry the cumulative values of a field. In case one or more messages within a transaction are not received by the Server (e.g., as a result of radio collisions), the next received message will make up for the lost messages, carrying cumulative values of the field.
///
/// A new transaction is started when this field is set to `true`, or when the last message of the transaction was sent 6 or more seconds earlier.
///
/// This defaults to `false`, which means that each new message will receive a new transaction identifier (if not set explicitly).
@property (nonatomic,assign) BOOL continueTransaction;

@end


@interface SigAcknowledgedGenericMessage : SigGenericMessage
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark - Generic
#pragma mark opcode:0x8201

/// 3.2.1.1 Generic OnOff Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.42)
@interface SigGenericOnOffGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8202

/// 3.2.1.2 Generic OnOff Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.43)
@interface SigGenericOnOffSet : SigAcknowledgedGenericMessage
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
/// The new state of Generic OnOff Server.
@property (nonatomic,assign) BOOL isOn;
- (NSData *)parameters;

/// Creates the Generic OnOff Set message.
///
/// @param isOn The target value of the Generic OnOff state.
- (instancetype)initWithIsOn:(BOOL)isOn;
/// Creates the Generic OnOff Set message.
/// @param isOn The target value of the Generic OnOff state.
/// @param transitionTime The time that an element will take to transition to the target state from the present state.
/// @param delay Message execution delay in 5 millisecond steps.
- (instancetype)initWithIsOn:(BOOL)isOn transitionTime:(SigTransitionTime *)transitionTime dalay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8203

/// 3.2.1.3 Generic OnOff Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.43)
@interface SigGenericOnOffSetUnacknowledged : SigGenericMessage
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
/// The new state of Generic OnOff Server.
@property (nonatomic,assign) BOOL isOn;
- (NSData *)parameters;
/// Creates the Generic OnOff Set Unacknowledged message.
/// @param isOn  The target value of the Generic OnOff state.
- (instancetype)initWithIsOn:(BOOL)isOn;
/// Creates the Generic OnOff Set Unacknowledged message.
/// @param isOn  The target value of the Generic OnOff state.
/// @param transitionTime  The time that an element will take to transition to the target state from the present state.
/// @param delay  Message execution delay in 5 millisecond steps.
- (instancetype)initWithIsOn:(BOOL)isOn transitionTime:(SigTransitionTime *)transitionTime dalay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8204

/// 3.2.1.4 Generic OnOff Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.44)
@interface SigGenericOnOffStatus : SigGenericMessage
/// The Remaining Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *remainingTime;
/// The present state of Generic OnOff Server.
@property (nonatomic,assign) BOOL isOn;
/// The target state of Generic OnOff Server.
@property (nonatomic,assign) BOOL targetState;
- (NSData *)parameters;
/// Creates the Generic OnOff Status message.
/// @param isOn  The current value of the Generic OnOff state.
- (instancetype)initWithIsOn:(BOOL)isOn;
/// Creates the Generic OnOff Status message.
/// @param isOn  The current value of the Generic OnOff state.
/// @param targetState  The target value of the Generic OnOff state.
/// @param remainingTime  The time that an element will take to transition to the target state from the present state.
- (instancetype)initWithIsOn:(BOOL)isOn targetState:(BOOL)targetState remainingTime:(SigTransitionTime *)remainingTime;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8205

/// 3.2.2.1 Generic Level Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.44)
@interface SigGenericLevelGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8206

/// 3.2.2.2 Generic Level Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.44)
@interface SigGenericLevelSet : SigAcknowledgedGenericMessage
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
/// The target value of the Generic Level state.
@property (nonatomic,assign) UInt16 level;
- (NSData *)parameters;

/// Creates the Generic Level Set message.
///
/// @param level  The target value of the Generic Level state.
- (instancetype)initWithLevel:(UInt16)level;
/// Creates the Generic OnOff Set message.
/// @param level The target value of the Generic Level state.
/// @param transitionTime The time that an element will take to transition to the target state from the present state.
/// @param delay Message execution delay in 5 millisecond steps.
- (instancetype)initWithLevel:(UInt16)level transitionTime:(SigTransitionTime *)transitionTime dalay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8207

/// 3.2.2.3 Generic Level Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.45)
@interface SigGenericLevelSetUnacknowledged : SigGenericMessage
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
/// The target value of the Generic Level state.
@property (nonatomic,assign) UInt16 level;
- (NSData *)parameters;
/// Creates the Generic Level Set Unacknowledged message.
/// @param level  The target value of the Generic Level state.
- (instancetype)initWithLevel:(UInt16)level;
/// Creates the Generic Level Set Unacknowledged message.
/// @param level  The target value of the Generic Level state.
/// @param transitionTime  The time that an element will take to transition to the target state from the present state.
/// @param delay  Message execution delay in 5 millisecond steps.
- (instancetype)initWithLevel:(UInt16)level transitionTime:(SigTransitionTime *)transitionTime dalay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8208

/// 3.2.2.8 Generic Level Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.48)
@interface SigGenericLevelStatus : SigGenericMessage
/// The Remaining Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *remainingTime;
/// The target value of the Generic Level state.
@property (nonatomic,assign) UInt16 level;
/// The target value of the Generic Level state.
@property (nonatomic,assign) UInt16 targetLevel;
- (NSData *)parameters;
/// Creates the Generic Level Status message.
/// @param level  The current value of the Generic Level state.
- (instancetype)initWithLevel:(UInt16)level;
/// Creates the Generic Level Status message.
/// @param level  The current value of the Generic Level state.
/// @param targetLevel  The target value of the Generic Level state.
/// @param remainingTime  The time that an element will take to transition to the target state from the present state.
- (instancetype)initWithLevel:(UInt16)level targetLevel:(BOOL)targetLevel remainingTime:(SigTransitionTime *)remainingTime;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8209

/// 3.2.2.4 Generic Delta Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.46)
@interface SigGenericDeltaSet : SigAcknowledgedGenericMessage
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
/// The Delta change of the Generic Level state.
@property (nonatomic,assign) UInt32 delta;
- (NSData *)parameters;

/// Creates the Generic Level Set message.
/// @param delta  The Delta change of the Generic Level state.
- (instancetype)initWithDelta:(UInt32)delta;
/// Creates the Generic Level Set message.
/// @param delta  The Delta change of the Generic Level state.
/// @param transitionTime The time that an element will take to transition to the target state from the present state.
/// @param delay Message execution delay in 5 millisecond steps.
- (instancetype)initWithDelta:(UInt32)delta transitionTime:(SigTransitionTime *)transitionTime dalay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x820A

/// 3.2.2.5 Generic Delta Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.46)
@interface SigGenericDeltaSetUnacknowledged : SigGenericMessage
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
/// The Delta change of the Generic Level state.
@property (nonatomic,assign) UInt32 delta;
- (NSData *)parameters;
/// Creates the Generic Level Set Unacknowledged message.
/// @param delta  The Delta change of the Generic Level state.
- (instancetype)initWithDelta:(UInt32)delta;
/// Creates the Generic Level Set Unacknowledged message.
/// @param delta  The Delta change of the Generic Level state.
/// @param transitionTime The time that an element will take to transition to the target state from the present state.
/// @param delay Message execution delay in 5 millisecond steps.
- (instancetype)initWithDelta:(UInt32)delta transitionTime:(SigTransitionTime *)transitionTime dalay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x820B

/// 3.2.2.6 Generic Move Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.47)
@interface SigGenericMoveSet : SigAcknowledgedGenericMessage
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
/// The Delta Level step to calculate Move speed for the Generic Level state.
@property (nonatomic,assign) UInt16 deltaLevel;
- (NSData *)parameters;

/// Creates the Generic Level Set message.
/// @param deltaLevel  The Delta Level step to calculate Move speed for the Generic Level state.
- (instancetype)initWithDeltaLevel:(UInt16)deltaLevel;

/// Creates the Generic Level Set message.
/// @param deltaLevel  The Delta Level step to calculate Move speed for the Generic Level state.
/// @param transitionTime The time that an element will take to transition to the target state from the present state.
/// @param delay Message execution delay in 5 millisecond steps.
- (instancetype)initWithDeltaLevel:(UInt16)deltaLevel transitionTime:(SigTransitionTime *)transitionTime dalay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x820C

/// 3.2.2.7 Generic Move Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.47)
@interface SigGenericMoveSetUnacknowledged : SigGenericMessage
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
/// The Delta Level step to calculate Move speed for the Generic Level state.
@property (nonatomic,assign) UInt16 deltaLevel;
- (NSData *)parameters;

/// Creates the Generic Level Set message.
/// @param deltaLevel  The Delta Level step to calculate Move speed for the Generic Level state.
- (instancetype)initWithDeltaLevel:(UInt16)deltaLevel;

/// Creates the Generic Level Set message.
/// @param deltaLevel  The Delta Level step to calculate Move speed for the Generic Level state.
/// @param transitionTime The time that an element will take to transition to the target state from the present state.
/// @param delay Message execution delay in 5 millisecond steps.
- (instancetype)initWithDeltaLevel:(UInt16)deltaLevel transitionTime:(SigTransitionTime *)transitionTime dalay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark opcode:0x820D

/// 3.2.3.1 Generic Default Transition Time Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.48)
@interface SigGenericDefaultTransitionTimeGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x820E

/// 3.2.3.2 Generic Default Transition Time Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.49)
@interface SigGenericDefaultTransitionTimeSet : SigAcknowledgedGenericMessage
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
- (NSData *)parameters;

/// Creates the Generic Default Transition Time Set message.
/// @param transitionTime  The default time that an element will take to transition to the target state from the present state.
- (instancetype)initWithTransitionTime:(SigTransitionTime *)transitionTime;

- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x820F

/// 3.2.3.3 Generic Default Transition Time Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.49)
@interface SigGenericDefaultTransitionTimeSetUnacknowledged : SigGenericMessage
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
- (NSData *)parameters;

/// Creates the Generic Default Transition Time Set message.
/// @param transitionTime  The default time that an element will take to transition to the target state from the present state.
- (instancetype)initWithTransitionTime:(SigTransitionTime *)transitionTime;

- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark opcode:0x8210

/// 3.2.3.4 Generic Default Transition Time Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.49)
@interface SigGenericDefaultTransitionTimeStatus : SigGenericMessage
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
- (NSData *)parameters;

/// Creates the Generic Default Transition Time Set message.
/// @param transitionTime  The default time that an element will take to transition to the target state from the present state.
- (instancetype)initWithTransitionTime:(SigTransitionTime *)transitionTime;

- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark opcode:0x8211

/// 3.2.4.1 Generic OnPowerUp Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.49)
@interface SigGenericOnPowerUpGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8212

/// 3.2.4.4 Generic OnPowerUp Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.50)
@interface SigGenericOnPowerUpStatus : SigGenericMessage
/// The value of the Generic OnPowerUp state.
@property (nonatomic,assign) SigOnPowerUp state;
- (NSData *)parameters;
/// Creates the Generic On Power Up Status message.
/// @param state  The value of the Generic OnPowerUp state.
- (instancetype)initWithState:(SigOnPowerUp)state;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8213

/// 3.2.4.2 Generic OnPowerUp Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.50)
@interface SigGenericOnPowerUpSet : SigAcknowledgedGenericMessage
/// The value of the Generic OnPowerUp state.
@property (nonatomic,assign) SigOnPowerUp state;
- (NSData *)parameters;
/// Creates the Generic On Power Up Status message.
/// @param state  The value of the Generic OnPowerUp state.
- (instancetype)initWithState:(SigOnPowerUp)state;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8214

/// 3.2.4.3 Generic OnPowerUp Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.50)
@interface SigGenericOnPowerUpSetUnacknowledged : SigGenericMessage
/// The value of the Generic OnPowerUp state.
@property (nonatomic,assign) SigOnPowerUp state;
- (NSData *)parameters;
/// Creates the Generic On Power Up Status message.
/// @param state  The value of the Generic OnPowerUp state.
- (instancetype)initWithState:(SigOnPowerUp)state;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8215

/// 3.2.5.1 Generic Power Level Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.50)
@interface SigGenericPowerLevelGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8216

/// 3.2.5.2 Generic Power Level Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.51)
@interface SigGenericPowerLevelSet : SigAcknowledgedGenericMessage
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
/// The target value of the Generic Power Actual state.
@property (nonatomic,assign) UInt16 power;

- (NSData *)parameters;

/// Creates the Generic Power Level Set message.
///
/// The Generic Power Actual state determines the linear percentage of the maximum power level of an element, representing a range from 0 percent through 100 percent. The value is derived using the following formula:
///
/// Represented power level [%] = 100 [%] * Generic Power Actual / 65535
///
/// @param power  The target value of the Generic Power Actual state.
- (instancetype)initWithPower:(UInt16)power;

/// Creates the Generic Power Level Set message.
///
/// The Generic Power Actual state determines the linear percentage of the maximum power level of an element, representing a range from 0 percent through 100 percent. The value is derived using the following formula:
///
/// Represented power level [%] = 100 [%] * Generic Power Actual / 65535
///
/// @param power  The target value of the Generic Power Actual state.
/// @param transitionTime The time that an element will take to transition to the target state from the present state.
/// @param delay Message execution delay in 5 millisecond steps.
- (instancetype)initWithPower:(UInt16)power transitionTime:(nullable SigTransitionTime *)transitionTime dalay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8217

/// 3.2.5.3 Generic Power Level Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.51)
@interface SigGenericPowerLevelSetUnacknowledged : SigGenericMessage
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
/// The target value of the Generic Power Actual state.
@property (nonatomic,assign) UInt16 power;

- (NSData *)parameters;

/// Creates the Generic Power Level Set message.
///
/// The Generic Power Actual state determines the linear percentage of the maximum power level of an element, representing a range from 0 percent through 100 percent. The value is derived using the following formula:
///
/// Represented power level [%] = 100 [%] * Generic Power Actual / 65535
///
/// @param power  The target value of the Generic Power Actual state.
- (instancetype)initWithPower:(UInt16)power;

/// Creates the Generic Power Level Set message.
///
/// The Generic Power Actual state determines the linear percentage of the maximum power level of an element, representing a range from 0 percent through 100 percent. The value is derived using the following formula:
///
/// Represented power level [%] = 100 [%] * Generic Power Actual / 65535
///
/// @param power  The target value of the Generic Power Actual state.
/// @param transitionTime The time that an element will take to transition to the target state from the present state.
/// @param delay Message execution delay in 5 millisecond steps.
- (instancetype)initWithPower:(UInt16)power transitionTime:(SigTransitionTime *)transitionTime dalay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark opcode:0x8218

/// 3.2.5.4 Generic Power Level Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.52)
@interface SigGenericPowerLevelStatus : SigGenericMessage
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// The present value of the Generic Power Actual state.
@property (nonatomic,assign) UInt16 power;
/// The target value of the Generic Power Actual state.
@property (nonatomic,assign) UInt16 targetPower;

- (NSData *)parameters;

/// Creates the Generic Power Level Status message.
///
/// @param power  The present value of the Generic Power Actual state.
- (instancetype)initWithPower:(UInt16)power;

/// Creates the Generic Power Level Status message.
///
/// @param power  The present value of the Generic Power Actual state.
/// @param targetPower The target value of the Generic Power Actual state.
/// @param transitionTime The time that an element will take to transition to the target state from the present state.
- (instancetype)initWithPower:(UInt16)power targetPower:(UInt16)targetPower transitionTime:(SigTransitionTime *)transitionTime;
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark opcode:0x8219

/// 3.2.5.5 Generic Power Last Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.52)
@interface SigGenericPowerLastGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x821A

/// 3.2.5.6 Generic Power Last Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.52)
@interface SigGenericPowerLastStatus : SigGenericMessage
/// The value of the Generic Power Last state.
@property (nonatomic,assign) UInt16 power;
- (NSData *)parameters;
/// Creates the Generic Power Last Status message.
/// @param power  The value of the Generic Power Last state.
- (instancetype)initWithPower:(UInt16)power;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x821B

/// 3.2.5.7 Generic Power Default Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.53)
@interface SigGenericPowerDefaultGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x821C

/// 3.2.5.10 Generic Power Default Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.53)
@interface SigGenericPowerDefaultStatus : SigGenericMessage
/// The value of the Generic Power Default state.
@property (nonatomic,assign) UInt16 power;
- (NSData *)parameters;
/// Creates the Generic Power Default Status message.
/// @param power  The value of the Generic Power Default state.
- (instancetype)initWithPower:(UInt16)power;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x821D

/// 3.2.5.11 Generic Power Range Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.53)
@interface SigGenericPowerRangeGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x821E

/// 3.2.5.14 Generic Power Range Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.54)
@interface SigGenericPowerRangeStatus : SigGenericMessage
/// Status Code for the requesting message.
@property (nonatomic,assign) SigGenericMessageStatus status;
/// The value of the Generic Power Range Min field of the Generic Power Range state.
@property (nonatomic,assign) UInt16 rangeMin;
/// The value of the Generic Power Range Max field of the Generic Power Range state.
@property (nonatomic,assign) UInt16 rangeMax;
- (NSData *)parameters;
/// Creates the Generic Power Range Status message.
/// @param rangeMin  The value of the Generic Power Range Min field of the Generic Power Range state.
/// @param rangeMax  The value of the Generic Power Range Max field of the Generic Power Range state.
- (instancetype)initWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax;
/// Creates the Generic Power Range Status message.
/// @param status  Status Code for the requesting message.
/// @param request  The request received.
- (instancetype)initWithStatus:(SigGenericMessageStatus)status forSigGenericPowerRangeSetRequest:(SigGenericPowerRangeSet *)request;
/// Creates the Generic Power Range Status message.
/// @param status  Status Code for the requesting message.
/// @param request  The request received.
- (instancetype)initWithStatus:(SigGenericMessageStatus)status forSigGenericPowerRangeSetUnacknowledgedRequest:(SigGenericPowerRangeSetUnacknowledged *)request;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x821F

/// 3.2.5.8 Generic Power Default Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.53)
@interface SigGenericPowerDefaultSet : SigAcknowledgedGenericMessage
/// The value of the Generic Power Default state.
@property (nonatomic,assign) UInt16 power;
- (NSData *)parameters;
/// Creates the Generic Power Default Set message.
/// @param power  The value of the Generic Power Default state.
- (instancetype)initWithPower:(UInt16)power;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8220

/// 3.2.5.9 Generic Power Default Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.53)
@interface SigGenericPowerDefaultSetUnacknowledged : SigGenericMessage
/// The value of the Generic Power Default state.
@property (nonatomic,assign) UInt16 power;
- (NSData *)parameters;
/// Creates the Generic Power Default Set message.
/// @param power  The value of the Generic Power Default state.
- (instancetype)initWithPower:(UInt16)power;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8221

/// 3.2.5.12 Generic Power Range Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.54)
@interface SigGenericPowerRangeSet : SigAcknowledgedGenericMessage
/// The value of the Generic Power Range Min field of the Generic Power Range state.
@property (nonatomic,assign) UInt16 rangeMin;
/// The value of the Generic Power Range Max field of the Generic Power Range state.
@property (nonatomic,assign) UInt16 rangeMax;
- (NSData *)parameters;
/// Creates the Generic Power Range Set message.
/// @param rangeMin  The value of the Generic Power Range Min field of the Generic Power Range state.
/// @param rangeMax  The value of the Generic Power Range Max field of the Generic Power Range state.
- (instancetype)initWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8222

/// 3.2.5.13 Generic Power Range Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.54)
@interface SigGenericPowerRangeSetUnacknowledged : SigGenericMessage
/// The value of the Generic Power Range Min field of the Generic Power Range state.
@property (nonatomic,assign) UInt16 rangeMin;
/// The value of the Generic Power Range Max field of the Generic Power Range state.
@property (nonatomic,assign) UInt16 rangeMax;
- (NSData *)parameters;
/// Creates the Generic Power Range Set message.
/// @param rangeMin  The value of the Generic Power Range Min field of the Generic Power Range state.
/// @param rangeMax  The value of the Generic Power Range Max field of the Generic Power Range state.
- (instancetype)initWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8223

/// 3.2.6.1 Generic Battery Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.55)
@interface SigGenericBatteryGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8224

/// 3.2.6.2 Generic Battery Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.55)
@interface SigGenericBatteryStatus : SigGenericMessage
/// Battery level state in percentage. Only values 0...100 and 0xFF are allowed.
///
/// Vaklue 0xFF means that the battery state is unknown.
@property (nonatomic,assign) UInt8 batteryLevel;
/// Time to discharge, in minutes. Value 0xFFFFFF means unknown time.
@property (nonatomic,assign) UInt32 timeToDischarge;
/// Time to charge, in minutes. Value 0xFFFFFF means unknown time.
@property (nonatomic,assign) UInt32 timeToCharge;
/// Flags.
@property (nonatomic,assign) UInt8 flags;
/// Whether the battery level is known.
- (BOOL)isBatteryLevelKnown;
/// Whether the time to discharge is known.
- (BOOL)isTimeToDischargeKnown;
/// Whether the time to charge is known.
- (BOOL)isTimeToChargeKnown;
/// Presence of the battery.
- (SigBatteryPresence)batteryPresence;
/// Charge level of the battery.
- (SigBatteryIndicator)batteryIndicator;
/// Whether the battery is charging.
- (SigBatteryChargingState)batteryChargingState;
/// The serviceability of the battery.
- (SigBatteryServiceability)batteryServiceability;
- (instancetype)initWithBatteryLevel:(UInt8)batteryLevel timeToDischarge:(UInt32)timeToDischarge andCharge:(UInt32)timeToCharge batteryPresence:(SigBatteryPresence)batteryPresence batteryIndicator:(SigBatteryIndicator)batteryIndicator batteryChargingState:(SigBatteryChargingState)batteryChargingState batteryServiceability:(SigBatteryServiceability)batteryServiceability;

- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark - Sensor
#pragma mark opcode:0x8230

/// 4.2.1 Sensor Descriptor Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.114)
@interface SigSensorDescriptorGet : SigAcknowledgedGenericMessage
/// If present, the Property ID field identifies a Sensor Property ID state of an element.(0x0001–0xFFFF)
@property (nonatomic,assign) UInt16 propertyID;
- (NSData *)parameters;
- (instancetype)initWithPropertyID:(UInt16)propertyID;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x51

/// 4.2.2 Sensor Descriptor Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.114)
@interface SigSensorDescriptorStatus : SigGenericMessage
/// The Descriptor field shall contain a sequence of 0 or more Sensor Descriptor states.(8*N or 2 bytes)
@property (nonatomic,strong) NSMutableArray <SigSensorDescriptorModel *>*descriptorModels;
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8231

/// 4.2.13 SensorGet
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.120)
@interface SigSensorGet : SigAcknowledgedGenericMessage
/// If present, the Property ID field identifies a Sensor Property ID state of an element.(0x0001–0xFFFF)
@property (nonatomic,assign) UInt16 propertyID;
- (NSData *)parameters;
- (instancetype)initWithPropertyID:(UInt16)propertyID;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x52

/// 4.2.14 SensorStatus
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.120)
@interface SigSensorStatus : SigGenericMessage
@property (nonatomic,strong) NSData *marshalledSensorData;
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8232

/// 4.2.3 Sensor Cadence Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.122)
@interface SigSensorColumnGet : SigAcknowledgedGenericMessage
/// If present, the Property ID field identifies a Sensor Property ID state of an element.(0x0001–0xFFFF)
@property (nonatomic,assign) UInt16 propertyID;
/// The Raw Value X field identifies a column of a sensor’s series.
@property (nonatomic,strong) NSData *rawValueX;
- (NSData *)parameters;
- (instancetype)initWithPropertyID:(UInt16)propertyID rawValueX:(NSData *)rawValueX;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x53

/// 4.2.16 SensorColumnStatus
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.122)
@interface SigSensorColumnStatus : SigGenericMessage
@property (nonatomic,strong) NSData *columnData;
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8233

/// 4.2.17 SensorSeriesGet
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.123)
@interface SigSensorSeriesGet : SigAcknowledgedGenericMessage
@property (nonatomic,strong) NSData *seriesData;
/// If present, the Property ID field identifies a Sensor Property ID state of an element.(0x0001–0xFFFF)
@property (nonatomic,assign) UInt16 propertyID;
/// Raw value identifying a starting column.
@property (nonatomic,strong) NSData *rawValueX1Data;
/// Raw value identifying an ending column.
@property (nonatomic,strong) NSData *rawValueX2Data;
- (NSData *)parameters;
- (instancetype)initWithPropertyID:(UInt16)propertyID rawValueX1Data:(NSData *)rawValueX1Data rawValueX2Data:(NSData *)rawValueX2Data;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x54

/// 4.2.18 SensorSeriesStatus
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.123)
@interface SigSensorSeriesStatus : SigGenericMessage
@property (nonatomic,strong) NSData *seriesData;
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8234

/// 4.2.3 Sensor Cadence Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.115)
@interface SigSensorCadenceGet : SigAcknowledgedGenericMessage
/// If present, the Property ID field identifies a Sensor Property ID state of an element.(0x0001–0xFFFF)
@property (nonatomic,assign) UInt16 propertyID;
- (NSData *)parameters;
- (instancetype)initWithPropertyID:(UInt16)propertyID;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x55

/// 4.2.4 Sensor Cadence Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.115)
@interface SigSensorCadenceSet : SigAcknowledgedGenericMessage
@property (nonatomic,strong) NSData *cadenceData;
/// If present, the Property ID field identifies a Sensor Property ID state of an element.(0x0001–0xFFFF)
@property (nonatomic,assign) UInt16 propertyID;
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x56

/// 4.2.5 Sensor Cadence Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.116)
@interface SigSensorCadenceSetUnacknowledged : SigGenericMessage
@property (nonatomic,strong) NSData *cadenceData;
/// If present, the Property ID field identifies a Sensor Property ID state of an element.(0x0001–0xFFFF)
@property (nonatomic,assign) UInt16 propertyID;
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x57

/// 4.2.6 Sensor Cadence Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.117)
@interface SigSensorCadenceStatus : SigGenericMessage
@property (nonatomic,strong) NSData *cadenceData;
/// If present, the Property ID field identifies a Sensor Property ID state of an element.(0x0001–0xFFFF)
@property (nonatomic,assign) UInt16 propertyID;
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8234

/// 4.2.7 Sensor Settings Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.117)
@interface SigSensorSettingsGet : SigAcknowledgedGenericMessage
/// If present, the Property ID field identifies a Sensor Property ID state of an element.(0x0001–0xFFFF)
@property (nonatomic,assign) UInt16 propertyID;
- (NSData *)parameters;
- (instancetype)initWithPropertyID:(UInt16)propertyID;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8234

/// 4.2.8 Sensor Settings Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.118)
@interface SigSensorSettingsStatus : SigGenericMessage
@property (nonatomic,strong) NSData *settingsData;
/// If present, the Property ID field identifies a Sensor Property ID state of an element.(0x0001–0xFFFF)
@property (nonatomic,assign) UInt16 propertyID;
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8236

/// 4.2.9 Sensor Setting Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.118)
@interface SigSensorSettingGet : SigAcknowledgedGenericMessage
/// Property ID identifying a sensor..(0x0001–0xFFFF)
@property (nonatomic,assign) UInt16 propertyID;
/// Setting Property ID identifying a setting within a sensor..(0x0001–0xFFFF)
@property (nonatomic,assign) UInt16 settingPropertyID;
- (NSData *)parameters;
- (instancetype)initWithPropertyID:(UInt16)propertyID settingPropertyID:(UInt16)settingPropertyID;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x59

/// 4.2.10 SensorSettingSet
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.118)
@interface SigSensorSettingSet : SigAcknowledgedGenericMessage
/// Property ID identifying a sensor..(0x0001–0xFFFF)
@property (nonatomic,assign) UInt16 propertyID;
/// Setting Property ID identifying a setting within a sensor..(0x0001–0xFFFF)
@property (nonatomic,assign) UInt16 settingPropertyID;
/// Raw value for the setting.
@property (nonatomic,strong) NSData *settingRaw;
- (NSData *)parameters;
- (instancetype)initWithPropertyID:(UInt16)propertyID settingPropertyID:(UInt16)settingPropertyID settingRaw:(NSData *)settingRaw;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x5A

/// 4.2.11 SensorSettingSetUnacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.119)
@interface SigSensorSettingSetUnacknowledged : SigGenericMessage
/// Property ID identifying a sensor..(0x0001–0xFFFF)
@property (nonatomic,assign) UInt16 propertyID;
/// Setting Property ID identifying a setting within a sensor..(0x0001–0xFFFF)
@property (nonatomic,assign) UInt16 settingPropertyID;
/// Raw value for the setting.
@property (nonatomic,strong) NSData *settingRaw;
- (NSData *)parameters;
- (instancetype)initWithPropertyID:(UInt16)propertyID settingPropertyID:(UInt16)settingPropertyID settingRaw:(NSData *)settingRaw;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x5B

/// 4.2.12 SensorSettingStatus
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.119)
@interface SigSensorSettingStatus : SigGenericMessage
/// Property ID identifying a sensor..(0x0001–0xFFFF)
@property (nonatomic,assign) UInt16 propertyID;
/// Setting Property ID identifying a setting within a sensor..(0x0001–0xFFFF)
@property (nonatomic,assign) UInt16 settingPropertyID;
/// Read / Write access rights for the setting.
@property (nonatomic,assign) SigSensorSettingAccessType settingAccess;
/// Raw value for the setting.
@property (nonatomic,strong) NSData *settingRaw;
- (NSData *)parameters;
- (instancetype)initWithPropertyID:(UInt16)propertyID settingPropertyID:(UInt16)settingPropertyID settingAccess:(SigSensorSettingAccessType)settingAccess settingRaw:(NSData *)settingRaw;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark - Time
#pragma mark opcode:0x8237

/// 5.2.1.1 Time Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.142)
@interface SigTimeGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x5C

/// 5.2.1.2 Time Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.143)
@interface SigTimeSet : SigAcknowledgedGenericMessage
@property (nonatomic,strong) SigTimeModel *timeModel;
- (NSData *)parameters;
- (instancetype)initWithTimeModel:(SigTimeModel *)timeModel;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x5D

/// 5.2.1.2 Time Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.143)
@interface SigTimeStatus : SigGenericMessage
@property (nonatomic,strong) SigTimeModel *timeModel;
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8238

/// 5.2.1.10 Time Role Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.145)
@interface SigTimeRoleGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8239

/// 5.2.1.11 Time Role Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.146)
@interface SigTimeRoleSet : SigAcknowledgedGenericMessage
/// Time Role is an enumeration state that defines the role of a node in propagation of time information in a mesh network.
@property (nonatomic,assign) SigTimeRole timeRole;
- (NSData *)parameters;
- (instancetype)initWithTimeRole:(SigTimeRole)timeRole;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x823A

/// 5.2.1.12 Time Role Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.146)
@interface SigTimeRoleStatus : SigGenericMessage
/// Time Role is an enumeration state that defines the role of a node in propagation of time information in a mesh network.
@property (nonatomic,assign) SigTimeRole timeRole;
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x823B

/// 5.2.1.4 Time Zone Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.144)
@interface SigTimeZoneGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x823C

/// 5.2.1.5 Time Zone Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.144)
@interface SigTimeZoneSet : SigAcknowledgedGenericMessage
/// Upcoming local time zone offset.
@property (nonatomic, assign) UInt8 timeZoneOffsetNew;
/// 40 bits, TAI seconds time of the upcoming Time Zone Offset change.
@property (nonatomic, assign) UInt64 TAIOfZoneChange;
- (NSData *)parameters;
- (instancetype)initWithTimeZoneOffsetNew:(UInt8)timeZoneOffsetNew TAIOfZoneChange:(UInt64)TAIOfZoneChange;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x823D

/// 5.2.1.6 Time Zone Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.144)
@interface SigTimeZoneStatus : SigGenericMessage
/// Current local time zone offset.
@property (nonatomic, assign) UInt8 timeZoneOffsetCurrent;
/// Upcoming local time zone offset.
@property (nonatomic, assign) UInt8 timeZoneOffsetNew;
/// 40 bits, TAI seconds time of the upcoming Time Zone Offset change.
@property (nonatomic, assign) UInt64 TAIOfZoneChange;
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x823E

/// 5.2.1.7 TAI-UTC Delta Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.145)
@interface SigTAI_UTC_DeltaGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x823F

/// 5.2.1.8 TAI-UTC Delta Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.145)
@interface SigTAI_UTC_DeltaSet : SigAcknowledgedGenericMessage
/// 15 bits, Upcoming difference between TAI and UTC in seconds.
@property (nonatomic, assign) UInt16 TAI_UTC_DeltaNew;
/// 1 bit, Always 0b0. Other values are Prohibited.
@property (nonatomic, assign) UInt8 padding;
/// 40 bits, TAI seconds time of the upcoming TAI-UTC Delta change.
@property (nonatomic, assign) UInt64 TAIOfDeltaChange;
- (NSData *)parameters;
- (instancetype)initWithTAI_UTC_DeltaNew:(UInt16)TAI_UTC_DeltaNew padding:(UInt8)padding TAIOfDeltaChange:(UInt64)TAIOfDeltaChange;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8240

/// 5.2.1.9 TAI-UTC Delta Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.145)
@interface SigTAI_UTC_DeltaStatus : SigGenericMessage
/// 15 bits, Current difference between TAI and UTC in seconds.
@property (nonatomic, assign) UInt16 TAI_UTC_DeltaCurrent;
/// 1 bit, Always 0b0. Other values are Prohibited.
@property (nonatomic, assign) UInt8 paddingCurrent;
/// 15 bits, Upcoming difference between TAI and UTC in seconds.
@property (nonatomic, assign) UInt16 TAI_UTC_DeltaNew;
/// 1 bit, Always 0b0. Other values are Prohibited.
@property (nonatomic, assign) UInt8 paddingNew;
/// 40 bits, TAI seconds time of the upcoming TAI-UTC Delta change.
@property (nonatomic, assign) UInt64 TAIOfDeltaChange;
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark - Scene
#pragma mark opcode:0x8241

/// 5.2.2.5 Scene Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.148)
@interface SigSceneGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8242

/// 5.2.2.3 Scene Recall
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.147)
@interface SigSceneRecall : SigAcknowledgedGenericMessage
/// The number of the scene to be recalled.
@property (nonatomic, assign) UInt16 sceneNumber;
/// the Transition Time field identifies the time that an element will take to transition from the present states to the target states defined by the recalled Scene.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic, assign) UInt8 delay;
- (NSData *)parameters;
- (instancetype)initWithSceneNumber:(UInt16)sceneNumber transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8243

/// 5.2.2.4 Scene Recall Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.147)
@interface SigSceneRecallUnacknowledged : SigGenericMessage
/// The number of the scene to be recalled.
@property (nonatomic, assign) UInt16 sceneNumber;
/// the Transition Time field identifies the time that an element will take to transition from the present states to the target states defined by the recalled Scene.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic, assign) UInt8 delay;
- (NSData *)parameters;
- (instancetype)initWithSceneNumber:(UInt16)sceneNumber transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x5E

/// 5.2.2.6 Scene Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.148)
@interface SigSceneStatus : SigGenericMessage
/// The Status Code field identifies the status code for the last operation.
@property (nonatomic, assign) SigSceneResponseStatus statusCode;
/// The Current Scene field identifies the Scene Number of the current Scene. If no scene is active, the Current Scene field value is 0.
@property (nonatomic, assign) UInt16 currentScene;
/// When an element is in the process of changing the Scene state, the Target Scene field identifies the target Scene Number of the target Scene state the element is to reach.
@property (nonatomic, assign) UInt16 targetScene;
/// the Remaining Time field indicates the time it will take the element to complete the transition to the target Scene state of the element.
@property (nonatomic,strong) SigTransitionTime *remainingTime;
- (NSData *)parameters;
- (instancetype)initWithStatusCode:(SigSceneResponseStatus)statusCode currentScene:(UInt16)currentScene targetScene:(UInt16)targetScene remainingTime:(SigTransitionTime *)remainingTime;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8244

/// 5.2.2.7 Scene Register Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.148)
@interface SigSceneRegisterGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8245

/// 5.2.2.8 Scene Register Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.149)
@interface SigSceneRegisterStatus : SigGenericMessage
/// The Status Code field identifies the status code for the last operation.
@property (nonatomic, assign) SigSceneResponseStatus statusCode;
/// Scene Number of a current scene.
@property (nonatomic, assign) UInt16 currentScene;
/// A list of scenes stored within an element.
@property (nonatomic, strong) NSMutableArray <NSNumber *>*scenes;//[(UInt16)sceneNumber]
- (NSData *)parameters;
- (instancetype)initWithStatusCode:(SigSceneResponseStatus)statusCode currentScene:(UInt16)currentScene targetScene:(UInt16)targetScene scenes:(NSMutableArray <NSNumber *>*)scenes;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8246

/// 5.2.2.1 Scene Store
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.146)
@interface SigSceneStore : SigAcknowledgedGenericMessage
/// The number of the scene to be stored.
@property (nonatomic, assign) UInt16 sceneNumber;
- (NSData *)parameters;
- (instancetype)initWithSceneNumber:(UInt16)sceneNumber;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8247

/// 5.2.2.2 Scene Store Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.146)
@interface SigSceneStoreUnacknowledged : SigGenericMessage
/// The number of the scene to be stored.
@property (nonatomic, assign) UInt16 sceneNumber;
- (NSData *)parameters;
- (instancetype)initWithSceneNumber:(UInt16)sceneNumber;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x829E

/// 5.2.2.9 Scene Delete
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.149)
@interface SigSceneDelete : SigAcknowledgedGenericMessage
/// The number of the scene to be deleted.
@property (nonatomic, assign) UInt16 sceneNumber;
- (NSData *)parameters;
- (instancetype)initWithSceneNumber:(UInt16)sceneNumber;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x829F

/// 5.2.2.10 Scene Delete Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.149)
@interface SigSceneDeleteUnacknowledged : SigGenericMessage
/// The number of the scene to be deleted.
@property (nonatomic, assign) UInt16 sceneNumber;
- (NSData *)parameters;
- (instancetype)initWithSceneNumber:(UInt16)sceneNumber;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark - Scheduler
#pragma mark opcode:0x8248

/// 5.2.3.3 Scheduler Action Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.150)
@interface SigSchedulerActionGet : SigAcknowledgedGenericMessage
/// Index of the Schedule Register entry to get.The valid values for the Index field are 0x00–0x0F. Values 0x10–0xFF are Prohibited.
@property (nonatomic, assign) UInt8 index;
- (NSData *)parameters;
- (instancetype)initWithIndex:(UInt8)index;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x5F

/// 5.2.3.6 Scheduler Action Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.151)
@interface SigSchedulerActionStatus : SigGenericMessage
/// 80 bits, The Schedule Register bit field shall be set to the value of the entry of the Schedule Register that is indicated by the Index field.
@property (nonatomic, strong) SchedulerModel *schedulerModel;
- (NSData *)parameters;
- (instancetype)initWithSchedulerModel:(SchedulerModel *)schedulerModel;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8249

/// 5.2.3.1 Scheduler Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.150)
@interface SigSchedulerGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x824A

/// 5.2.3.2 Scheduler Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.151)
@interface SigSchedulerStatus : SigGenericMessage
/// 16 bits, Bit field indicating defined Actions in the Schedule Register.
@property (nonatomic, strong) NSMutableArray <NSNumber *>*schedulers;//[schedulerID]
- (NSData *)parameters;
- (instancetype)initWithSchedulers:(NSMutableArray <NSNumber *>*)schedulers;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x60

/// 5.2.3.4 Scheduler Action Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.150)
@interface SigSchedulerActionSet : SigAcknowledgedGenericMessage
/// 80 bits, The Schedule Register bit field shall be set to the value of the entry of the Schedule Register that is indicated by the Index field.
@property (nonatomic, strong) SchedulerModel *schedulerModel;
- (NSData *)parameters;
- (instancetype)initWithSchedulerModel:(SchedulerModel *)schedulerModel;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x61

/// 5.2.3.5 Scheduler Action Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.151)
@interface SigSchedulerActionSetUnacknowledged : SigGenericMessage
/// 80 bits, The Schedule Register bit field shall be set to the value of the entry of the Schedule Register that is indicated by the Index field.
@property (nonatomic, strong) SchedulerModel *schedulerModel;
- (NSData *)parameters;
- (instancetype)initWithSchedulerModel:(SchedulerModel *)schedulerModel;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark - Light Lightness
#pragma mark opcode:0x824B

/// 6.3.1.1 Light Lightness Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.199)
@interface SigLightLightnessGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x824C

/// 6.3.1.2 Light Lightness Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.200)
@interface SigLightLightnessSet : SigAcknowledgedGenericMessage
/// The target value of the Light Lightness Actual state.
@property (nonatomic, assign) UInt16 lightness;
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
- (NSData *)parameters;
- (instancetype)initWithLightness:(UInt16)lightness transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x824D

/// 6.3.1.3 Light Lightness Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.200)
@interface SigLightLightnessSetUnacknowledged : SigGenericMessage
/// The target value of the Light Lightness Actual state.
@property (nonatomic, assign) UInt16 lightness;
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
- (NSData *)parameters;
- (instancetype)initWithLightness:(UInt16)lightness transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x824E

/// 6.3.1.4 Light Lightness Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.201)
@interface SigLightLightnessStatus : SigGenericMessage
/// The Remaining Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *remainingTime;
/// The target value of the Light Lightness Actual state.
@property (nonatomic, assign) UInt16 presentLightness;
/// The target value of the Light Lightness Actual state.
@property (nonatomic, assign) UInt16 targetLightness;
- (NSData *)parameters;
- (instancetype)initWithPresentLightness:(UInt16)presentLightness targetLightness:(UInt16)targetLightness remainingTime:(SigTransitionTime *)remainingTime;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x824F

/// 6.3.1.5 Light Lightness Linear Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.201)
@interface SigLightLightnessLinearGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8250

/// 6.3.1.6 Light Lightness Linear Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.201)
@interface SigLightLightnessLinearSet : SigAcknowledgedGenericMessage
/// The target value of the Light Lightness Actual state.
@property (nonatomic, assign) UInt16 lightness;
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
- (NSData *)parameters;
- (instancetype)initWithLightness:(UInt16)lightness transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8251

/// 6.3.1.7 Light Lightness Linear Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.202)
@interface SigLightLightnessLinearSetUnacknowledged : SigGenericMessage
/// The target value of the Light Lightness Actual state.
@property (nonatomic, assign) UInt16 lightness;
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
- (NSData *)parameters;
- (instancetype)initWithLightness:(UInt16)lightness transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8252

/// 6.3.1.8 Light Lightness Linear Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.203)
@interface SigLightLightnessLinearStatus : SigGenericMessage
/// The present value of the Light Lightness Linear state.
@property (nonatomic, assign) UInt16 presentLightness;
/// The target value of the Light Lightness Linear state.
@property (nonatomic, assign) UInt16 targetLightness;
/// The Remaining Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *remainingTime;
- (NSData *)parameters;
- (instancetype)initWithPresentLightness:(UInt16)presentLightness targetLightness:(UInt16)targetLightness remainingTime:(SigTransitionTime *)remainingTime;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8253

/// 6.3.1.9 Light Lightness Last Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.203)
@interface SigLightLightnessLastGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8254

/// 6.3.1.10 Light Lightness Last Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.203)
@interface SigLightLightnessLastStatus : SigGenericMessage
/// The value of the Light Lightness Last.
@property (nonatomic, assign) UInt16 lightness;
- (NSData *)parameters;
- (instancetype)initWithLightness:(UInt16)lightness;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8255

/// 6.3.1.11 Light Lightness Default Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.204)
@interface SigLightLightnessDefaultGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8256

/// 6.3.1.14 Light Lightness Default Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.204)
@interface SigLightLightnessDefaultStatus : SigGenericMessage
/// The value of the Light Lightness Last.
@property (nonatomic, assign) UInt16 lightness;
- (NSData *)parameters;
- (instancetype)initWithLightness:(UInt16)lightness;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8257

/// 6.3.1.15 Light Lightness Range Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.205)
@interface SigLightLightnessRangeGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8258

/// 6.3.1.18 Light Lightness Range Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.206)
@interface SigLightLightnessRangeStatus : SigGenericMessage
/// The Status Code field identifies the status code for the last operation.
@property (nonatomic, assign) SigGenericMessageStatus statusCode;
/// The value of the Generic Power Range Min field of the Generic Power Range state.
@property (nonatomic,assign) UInt16 rangeMin;
/// The value of the Generic Power Range Max field of the Generic Power Range state.
@property (nonatomic,assign) UInt16 rangeMax;
- (NSData *)parameters;
- (instancetype)initWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8259

/// 6.3.1.12 Light Lightness Default Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.204)
@interface SigLightLightnessDefaultSet : SigAcknowledgedGenericMessage
/// The value of the Light Lightness Default state.
@property (nonatomic, assign) UInt16 lightness;
- (NSData *)parameters;
- (instancetype)initWithLightness:(UInt16)lightness;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x825A

/// 6.3.1.13 Light Lightness Default Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.204)
@interface SigLightLightnessDefaultSetUnacknowledged : SigGenericMessage
/// The value of the Light Lightness Default state.
@property (nonatomic, assign) UInt16 lightness;
- (NSData *)parameters;
- (instancetype)initWithLightness:(UInt16)lightness;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x825B

/// 6.3.1.16 Light Lightness Range Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.205)
@interface SigLightLightnessRangeSet : SigAcknowledgedGenericMessage
/// The value of the Lightness Range Min field of the Light Lightness Range state.
@property (nonatomic,assign) UInt16 rangeMin;
/// The value of the Lightness Range Max field of the Light Lightness Range state.
@property (nonatomic,assign) UInt16 rangeMax;
- (NSData *)parameters;
/// Creates the Light Lightness Range Set message.
/// @param rangeMin  The value of the Lightness Range Min field of the Light Lightness Range state.
/// @param rangeMax  The value of the Lightness Range Max field of the Light Lightness Range state.
- (instancetype)initWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x825C

/// 6.3.1.17 Light Lightness Range Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.205)
@interface SigLightLightnessRangeSetUnacknowledged : SigGenericMessage
/// The value of the Lightness Range Min field of the Light Lightness Range state.
@property (nonatomic,assign) UInt16 rangeMin;
/// The value of the Lightness Range Max field of the Light Lightness Range state.
@property (nonatomic,assign) UInt16 rangeMax;
- (NSData *)parameters;
/// Creates the Light Lightness Range Set message.
/// @param rangeMin  The value of the Lightness Range Min field of the Light Lightness Range state.
/// @param rangeMax  The value of the Lightness Range Max field of the Light Lightness Range state.
- (instancetype)initWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark - Light CTL
#pragma mark opcode:0x825D

/// 6.3.2.1 Light CTL Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.206)
@interface SigLightCTLGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x825E

/// 6.3.2.2 Light CTL Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.206)
@interface SigLightCTLSet : SigAcknowledgedGenericMessage
/// The target value of the Light CTL Lightness state.
@property (nonatomic,assign) UInt16 CTLLightness;
/// The target value of the Light CTL Temperature state.
@property (nonatomic,assign) UInt16 CTLTemperature;
/// The target value of the Light CTL Delta UV state.
@property (nonatomic,assign) UInt16 CTLDeltaUV;
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
- (NSData *)parameters;
- (instancetype)initWithCTLLightness:(UInt16)CTLLightness CTLTemperature:(UInt16)CTLTemperature CTLDeltaUV:(UInt16)CTLDeltaUV transitionTime:(SigTransitionTime *)transitionTime delay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x825F

/// 6.3.2.3 Light CTL Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.206)
@interface SigLightCTLSetUnacknowledged : SigGenericMessage
/// The target value of the Light CTL Lightness state.
@property (nonatomic,assign) UInt16 CTLLightness;
/// The target value of the Light CTL Temperature state.
@property (nonatomic,assign) UInt16 CTLTemperature;
/// The target value of the Light CTL Delta UV state.
@property (nonatomic,assign) UInt16 CTLDeltaUV;
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
- (NSData *)parameters;
- (instancetype)initWithCTLLightness:(UInt16)CTLLightness CTLTemperature:(UInt16)CTLTemperature CTLDeltaUV:(UInt16)CTLDeltaUV transitionTime:(SigTransitionTime *)transitionTime delay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8260

/// 6.3.2.4 Light CTL Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.208)
@interface SigLightCTLStatus : SigGenericMessage
/// The Remaining Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *remainingTime;
/// The present value of the Light CTL Lightness state.
@property (nonatomic,assign) UInt16 presentCTLLightness;
/// The present value of the Light CTL Temperature state.
@property (nonatomic,assign) UInt16 presentCTLTemperature;
/// The target value of the Light CTL Lightness state.
@property (nonatomic,assign) UInt16 targetCTLLightness;
/// The target value of the Light CTL Temperature state.
@property (nonatomic,assign) UInt16 targetCTLTemperature;
- (NSData *)parameters;
- (instancetype)initWithPresentCTLLightness:(UInt16)presentCTLLightness presentCTLTemperature:(UInt16)presentCTLTemperature targetCTLLightness:(UInt16)targetCTLLightness targetCTLTemperature:(UInt16)targetCTLTemperature remainingTime:(SigTransitionTime *)remainingTime;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8261

/// 6.3.2.5 Light CTL Temperature Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.208)
@interface SigLightCTLTemperatureGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8262

/// 6.3.2.9 Light CTL Temperature Range Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.210)
@interface SigLightCTLTemperatureRangeGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8263

/// 6.3.2.12 Light CTL Temperature Range Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.211)
@interface SigLightCTLTemperatureRangeStatus : SigGenericMessage
/// Status Code for the requesting message..
@property (nonatomic, assign) SigGenericMessageStatus statusCode;
/// The value of the Temperature Range Min field of the Light CTL Temperature Range state.
@property (nonatomic,assign) UInt16 rangeMin;
/// The value of the Temperature Range Max field of the Light CTL Temperature Range state.
@property (nonatomic,assign) UInt16 rangeMax;
- (NSData *)parameters;
- (instancetype)initWithStatusCode:(SigGenericMessageStatus)statusCode rangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8264

/// 6.3.2.6 Light CTL Temperature Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.208)
@interface SigLightCTLTemperatureSet : SigAcknowledgedGenericMessage
/// The target value of the Light CTL Temperature state.
@property (nonatomic,assign) UInt16 CTLTemperature;
/// The target value of the Light CTL Delta UV state.
@property (nonatomic,assign) UInt16 CTLDeltaUV;
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
- (NSData *)parameters;
- (instancetype)initWithCTLTemperature:(UInt16)CTLTemperature CTLDeltaUV:(UInt16)CTLDeltaUV transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8265

/// 6.3.2.7 Light CTL Temperature Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.209)
@interface SigLightCTLTemperatureSetUnacknowledged : SigGenericMessage
/// The target value of the Light CTL Temperature state.
@property (nonatomic,assign) UInt16 CTLTemperature;
/// The target value of the Light CTL Delta UV state.
@property (nonatomic,assign) UInt16 CTLDeltaUV;
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
- (NSData *)parameters;
- (instancetype)initWithCTLTemperature:(UInt16)CTLTemperature CTLDeltaUV:(UInt16)CTLDeltaUV transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8266

/// 6.3.2.8 Light CTL Temperature Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.210)
@interface SigLightCTLTemperatureStatus : SigGenericMessage
/// The Remaining Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *remainingTime;
/// The present value of the Light CTL Temperature state.
@property (nonatomic,assign) UInt16 presentCTLTemperature;
/// The present value of the Light CTL Delta UV state.
@property (nonatomic,assign) UInt16 presentCTLDeltaUV;
/// The target value of the Light CTL Temperature state.
@property (nonatomic,assign) UInt16 targetCTLTemperature;
/// The target value of the Light CTL Delta UV state.
@property (nonatomic,assign) UInt16 targetCTLDeltaUV;
- (NSData *)parameters;
- (instancetype)initWithPresentCTLTemperature:(UInt16)presentCTLTemperature presentCTLDeltaUV:(UInt16)presentCTLDeltaUV targetCTLTemperature:(UInt16)targetCTLTemperature targetCTLDeltaUV:(UInt16)targetCTLDeltaUV remainingTime:(SigTransitionTime *)remainingTime;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8267

/// 6.3.2.13 Light CTL Default Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.212)
@interface SigLightCTLDefaultGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8268

/// 6.3.2.16 Light CTL Default Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.213)
@interface SigLightCTLDefaultStatus : SigGenericMessage
/// The value of the Light Lightness Default state.
@property (nonatomic,assign) UInt16 lightness;
/// The value of the Light CTL Temperature Default state.
@property (nonatomic,assign) UInt16 temperature;
/// The value of the Light CTL Delta UV Default state.
@property (nonatomic,assign) UInt16 deltaUV;
- (NSData *)parameters;
- (instancetype)initWithLightness:(UInt16)lightness temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8269

/// 6.3.2.14 Light CTL Default Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.212)
@interface SigLightCTLDefaultSet : SigAcknowledgedGenericMessage
/// The value of the Light Lightness Default state.
@property (nonatomic,assign) UInt16 lightness;
/// The value of the Light CTL Temperature Default state.
@property (nonatomic,assign) UInt16 temperature;
/// The value of the Light CTL Delta UV Default state.
@property (nonatomic,assign) UInt16 deltaUV;
- (NSData *)parameters;
- (instancetype)initWithLightness:(UInt16)lightness temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x826A

/// 6.3.2.15 Light CTL Default Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.212)
@interface SigLightCTLDefaultSetUnacknowledged : SigGenericMessage
/// The value of the Light Lightness Default state.
@property (nonatomic,assign) UInt16 lightness;
/// The value of the Light CTL Temperature Default state.
@property (nonatomic,assign) UInt16 temperature;
/// The value of the Light CTL Delta UV Default state.
@property (nonatomic,assign) UInt16 deltaUV;
- (NSData *)parameters;
- (instancetype)initWithLightness:(UInt16)lightness temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x826B

/// 6.3.2.10 Light CTL Temperature Range Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.210)
@interface SigLightCTLTemperatureRangeSet : SigAcknowledgedGenericMessage
/// The value of the Temperature Range Min field of the Light CTL Temperature Range state.
@property (nonatomic,assign) UInt16 rangeMin;
/// The value of the Temperature Range Max field of the Light CTL Temperature Range state.
@property (nonatomic,assign) UInt16 rangeMax;
- (NSData *)parameters;
- (instancetype)initWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x826C

/// 6.3.2.11 Light CTL Temperature Range Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.211)
@interface SigLightCTLTemperatureRangeSetUnacknowledged : SigGenericMessage
/// The value of the Temperature Range Min field of the Light CTL Temperature Range state.
@property (nonatomic,assign) UInt16 rangeMin;
/// The value of the Temperature Range Max field of the Light CTL Temperature Range state.
@property (nonatomic,assign) UInt16 rangeMax;
- (NSData *)parameters;
- (instancetype)initWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark - Light HSL
#pragma mark opcode:0x826D

/// 6.3.3.1 Light HSL Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.213)
@interface SigLightHSLGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x826E

/// 6.3.3.7 Light HSL Hue Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.216)
@interface SigLightHSLHueGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x826F

/// 6.3.3.8 Light HSL Hue Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.216)
@interface SigLightHSLHueSet : SigAcknowledgedGenericMessage
/// The target value of the Light HSL Hue state.
@property (nonatomic,assign) UInt16 hue;
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
- (NSData *)parameters;
- (instancetype)initWithHue:(UInt16)hue transitionTime:(SigTransitionTime *)transitionTime delay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8270

/// 6.3.3.9 Light HSL Hue Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.216)
@interface SigLightHSLHueSetUnacknowledged : SigGenericMessage
/// The target value of the Light HSL Hue state.
@property (nonatomic,assign) UInt16 hue;
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
- (NSData *)parameters;
- (instancetype)initWithHue:(UInt16)hue transitionTime:(SigTransitionTime *)transitionTime delay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8271

/// 6.3.3.10 Light HSL Hue Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.217)
@interface SigLightHSLHueStatus : SigGenericMessage
/// The present value of the Light HSL Hue state.
@property (nonatomic,assign) UInt16 presentHue;
/// The target value of the Light HSL Hue state.
@property (nonatomic,assign) UInt16 targetHue;
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
- (NSData *)parameters;
- (instancetype)initWithPresentHue:(UInt16)presentHue targetHue:(UInt16)targetHue transitionTime:(SigTransitionTime *)transitionTime;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8272

/// 6.3.3.11 Light HSL Saturation Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.217)
@interface SigLightHSLSaturationGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8273

/// 6.3.3.12 Light HSL Saturation Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.218)
@interface SigLightHSLSaturationSet : SigAcknowledgedGenericMessage
/// The target value of the Light HSL Saturation state.
@property (nonatomic,assign) UInt16 saturation;
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
- (NSData *)parameters;
- (instancetype)initWithSaturation:(UInt16)saturation transitionTime:(SigTransitionTime *)transitionTime delay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8274

/// 6.3.3.13 Light HSL Saturation Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.218)
@interface SigLightHSLSaturationSetUnacknowledged : SigGenericMessage
/// The target value of the Light HSL Saturation state.
@property (nonatomic,assign) UInt16 saturation;
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
- (NSData *)parameters;
- (instancetype)initWithSaturation:(UInt16)saturation transitionTime:(SigTransitionTime *)transitionTime delay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8275

/// 6.3.3.14 Light HSL Saturation Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.219)
@interface SigLightHSLSaturationStatus : SigGenericMessage
/// The present value of the Light HSL Saturation state.
@property (nonatomic,assign) UInt16 presentSaturation;
/// The target value of the Light HSL Saturation state.
@property (nonatomic,assign) UInt16 targetSaturation;
/// The Remaining Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *remainingTime;
- (NSData *)parameters;
- (instancetype)initWithPresentSaturation:(UInt16)presentSaturation targetSaturation:(UInt16)targetSaturation remainingTime:(SigTransitionTime *)remainingTime;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8276

/// 6.3.3.1 Light HSL Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.213)
@interface SigLightHSLSet : SigAcknowledgedGenericMessage
/// The target value of the Light HSL Lightness state.
@property (nonatomic,assign) UInt16 HSLLightness;
/// The target value of the Light HSL Hue state.
@property (nonatomic,assign) UInt16 HSLHue;
/// The target value of the Light HSL Saturation state.
@property (nonatomic,assign) UInt16 HSLSaturation;
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
- (NSData *)parameters;
- (instancetype)initWithHSLLightness:(UInt16)HSLLightness HSLHue:(UInt16)HSLHue HSLSaturation:(UInt16)HSLSaturation transitionTime:(SigTransitionTime *)transitionTime delay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8277

/// 6.3.3.3 Light HSL Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.214)
@interface SigLightHSLSetUnacknowledged : SigGenericMessage
/// The target value of the Light HSL Lightness state.
@property (nonatomic,assign) UInt16 HSLLightness;
/// The target value of the Light HSL Hue state.
@property (nonatomic,assign) UInt16 HSLHue;
/// The target value of the Light HSL Saturation state.
@property (nonatomic,assign) UInt16 HSLSaturation;
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
- (NSData *)parameters;
- (instancetype)initWithHSLLightness:(UInt16)HSLLightness HSLHue:(UInt16)HSLHue HSLSaturation:(UInt16)HSLSaturation transitionTime:(SigTransitionTime *)transitionTime delay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8278

/// 6.3.3.4 Light HSL Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.215)
@interface SigLightHSLStatus : SigGenericMessage
/// The Remaining Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *remainingTime;
/// The present value of the Light HSL Lightness state.
@property (nonatomic,assign) UInt16 HSLLightness;
/// The present value of the Light HSL Hue state.
@property (nonatomic,assign) UInt16 HSLHue;
/// The present value of the Light HSL Saturation state.
@property (nonatomic,assign) UInt16 HSLSaturation;
- (NSData *)parameters;
- (instancetype)initWithHSLLightness:(UInt16)HSLLightness HSLHue:(UInt16)HSLHue HSLSaturation:(UInt16)HSLSaturation remainingTime:(SigTransitionTime *)remainingTime;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8279

/// 6.3.3.5 Light HSL Target Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.215)
@interface SigLightHSLTargetGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x827A

/// 6.3.3.6 Light HSL Target Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.215)
@interface SigLightHSLTargetStatus : SigGenericMessage
/// The target value of the Light HSL Lightness state.
@property (nonatomic,assign) UInt16 HSLLightnessTarget;
/// The target value of the Light HSL Hue state.
@property (nonatomic,assign) UInt16 HSLHueTarget;
/// The target Light HSL Saturation state.
@property (nonatomic,assign) UInt16 HSLSaturationTarget;
/// The Remaining Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *remainingTime;
- (NSData *)parameters;
- (instancetype)initWithHSLLightnessTarget:(UInt16)HSLLightnessTarget HSLHueTarget:(UInt16)HSLHueTarget HSLSaturationTarget:(UInt16)HSLSaturationTarget remainingTime:(SigTransitionTime *)remainingTime;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x827B

/// 6.3.3.15 Light HSL Default Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.219)
@interface SigLightHSLDefaultGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x827C

/// 6.3.3.18 Light HSL Default Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.220)
@interface SigLightHSLDefaultStatus : SigGenericMessage
/// The value of the Light Lightness Default state.
@property (nonatomic,assign) UInt16 lightness;
/// The value of the Light HSL Hue Default state.
@property (nonatomic,assign) UInt16 hue;
/// The value of the Light HSL Saturation Default state.
@property (nonatomic,assign) UInt16 saturation;
/// The Remaining Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *remainingTime;
- (NSData *)parameters;
- (instancetype)initWithLightness:(UInt16)lightness hue:(UInt16)hue saturation:(UInt16)saturation remainingTime:(SigTransitionTime *)remainingTime;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x827D

/// 6.3.3.19 Light HSL Range Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.221)
@interface SigLightHSLRangeGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x827E

/// 6.3.3.22 Light HSL Range Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.222)
@interface SigLightHSLRangeStatus : SigGenericMessage
/// Status Code for the requesting message..
@property (nonatomic, assign) SigGenericMessageStatus statusCode;
/// The value of the Hue Range Min field of the Light HSL Hue Range state.
@property (nonatomic,assign) UInt16 hueRangeMin;
/// The value of the Hue Range Max field of the Light HSL Hue Range state.
@property (nonatomic,assign) UInt16 hueRangeMax;
/// The value of the Saturation Range Min field of the Light HSL Saturation Range state.
@property (nonatomic,assign) UInt16 saturationRangeMin;
/// The value of the Saturation Range Max field of the Light HSL Saturation Range state.
@property (nonatomic,assign) UInt16 saturationRangeMax;
- (NSData *)parameters;
- (instancetype)initWithStatusCode:(SigGenericMessageStatus)statusCode hueRangeMin:(UInt16)hueRangeMin hueRangeMax:(UInt16)hueRangeMax saturationRangeMin:(UInt16)saturationRangeMin saturationRangeMax:(UInt16)saturationRangeMax;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x827F

/// 6.3.3.16 Light HSL Default Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.219)
@interface SigLightHSLDefaultSet : SigAcknowledgedGenericMessage
/// The value of the Light Lightness Default state.
@property (nonatomic,assign) UInt16 lightness;
/// The value of the Light HSL Hue Default state.
@property (nonatomic,assign) UInt16 hue;
/// The value of the Light HSL Saturation Default state.
@property (nonatomic,assign) UInt16 saturation;
- (NSData *)parameters;
- (instancetype)initWithLightness:(UInt16)lightness hue:(UInt16)hue saturation:(UInt16)saturation;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8280

/// 6.3.3.17 Light HSL Default Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.220)
@interface SigLightHSLDefaultSetUnacknowledged : SigGenericMessage
/// The value of the Light Lightness Default state.
@property (nonatomic,assign) UInt16 lightness;
/// The value of the Light HSL Hue Default state.
@property (nonatomic,assign) UInt16 hue;
/// The value of the Light HSL Saturation Default state.
@property (nonatomic,assign) UInt16 saturation;
- (NSData *)parameters;
- (instancetype)initWithLightness:(UInt16)lightness hue:(UInt16)hue saturation:(UInt16)saturation;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8281

/// 6.3.3.20 Light HSL Range Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.221)
@interface SigLightHSLRangeSet : SigAcknowledgedGenericMessage
/// The value of the Hue Range Min field of the Light HSL Hue Range state.
@property (nonatomic,assign) UInt16 hueRangeMin;
/// The value of the Hue Range Max field of the Light HSL Hue Range state.
@property (nonatomic,assign) UInt16 hueRangeMax;
/// The value of the Saturation Range Min field of the Light HSL Saturation Range state.
@property (nonatomic,assign) UInt16 saturationRangeMin;
/// The value of the Saturation Range Max field of the Light HSL Saturation Range state.
@property (nonatomic,assign) UInt16 saturationRangeMax;
- (NSData *)parameters;
- (instancetype)initWithHueRangeMin:(UInt16)hueRangeMin hueRangeMax:(UInt16)hueRangeMax saturationRangeMin:(UInt16)saturationRangeMin saturationRangeMax:(UInt16)saturationRangeMax;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x82

/// 6.3.3.21 Light HSL Range Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.221)
@interface SigLightHSLRangeSetUnacknowledged : SigGenericMessage
/// The value of the Hue Range Min field of the Light HSL Hue Range state.
@property (nonatomic,assign) UInt16 hueRangeMin;
/// The value of the Hue Range Max field of the Light HSL Hue Range state.
@property (nonatomic,assign) UInt16 hueRangeMax;
/// The value of the Saturation Range Min field of the Light HSL Saturation Range state.
@property (nonatomic,assign) UInt16 saturationRangeMin;
/// The value of the Saturation Range Max field of the Light HSL Saturation Range state.
@property (nonatomic,assign) UInt16 saturationRangeMax;
- (NSData *)parameters;
- (instancetype)initWithHueRangeMin:(UInt16)hueRangeMin hueRangeMax:(UInt16)hueRangeMax saturationRangeMin:(UInt16)saturationRangeMin saturationRangeMax:(UInt16)saturationRangeMax;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark - Light xyL
#pragma mark opcode:0x8283

/// 6.3.4.1 Light xyL Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.223)
@interface SigLightXyLGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8284

/// 6.3.4.2 Light xyL Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.223)
@interface SigLightXyLSet : SigAcknowledgedGenericMessage
/// The target value of the Light xyL Lightness state.
@property (nonatomic,assign) UInt16 xyLLightness;
/// The target value of the Light xyL x state.
@property (nonatomic,assign) UInt16 xyLX;
/// The target value of the Light xyL y state.
@property (nonatomic,assign) UInt16 xyLY;
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
- (NSData *)parameters;
- (instancetype)initWithXyLLightness:(UInt16)xyLLightness xyLX:(UInt16)xyLX xyLY:(UInt16)xyLY transitionTime:(SigTransitionTime *)transitionTime delay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8285

/// 6.3.4.3 Light xyL Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.224)
@interface SigLightXyLSetUnacknowledged : SigGenericMessage
/// The target value of the Light xyL Lightness state.
@property (nonatomic,assign) UInt16 xyLLightness;
/// The target value of the Light xyL x state.
@property (nonatomic,assign) UInt16 xyLX;
/// The target value of the Light xyL y state.
@property (nonatomic,assign) UInt16 xyLY;
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
- (NSData *)parameters;
- (instancetype)initWithXyLLightness:(UInt16)xyLLightness xyLX:(UInt16)xyLX xyLY:(UInt16)xyLY transitionTime:(SigTransitionTime *)transitionTime delay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8286

/// 6.3.4.4 Light xyL Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.224)
@interface SigLightXyLStatus : SigGenericMessage
/// The present value of the Light xyL Lightness state.
@property (nonatomic,assign) UInt16 xyLLightness;
/// The present value of the Light xyL x state.
@property (nonatomic,assign) UInt16 xyLX;
/// The present value of the Light xyL y state.
@property (nonatomic,assign) UInt16 xyLY;
/// If present, the Remaining Time field identifies the time it will take the element to complete the transition to the target state of the element.
@property (nonatomic,strong) SigTransitionTime *remainingTime;
- (NSData *)parameters;
- (instancetype)initWithXyLLightness:(UInt16)xyLLightness xyLX:(UInt16)xyLX xyLY:(UInt16)xyLY remainingTime:(SigTransitionTime *)remainingTime;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8287

/// 6.3.4.5 Light xyL Target Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.225)
@interface SigLightXyLTargetGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8288

/// 6.3.4.6 Light xyL Target Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.225)
@interface SigLightXyLTargetStatus : SigGenericMessage
/// The target value of the Light xyL Lightness state.
@property (nonatomic,assign) UInt16 targetXyLLightness;
/// The target value of the Light xyL x state.
@property (nonatomic,assign) UInt16 targetXyLX;
/// The target value of the Light xyL y state.
@property (nonatomic,assign) UInt16 targetXyLY;
/// If present, the Remaining Time field identifies the time it will take the element to complete the transition to the target state of the element.
@property (nonatomic,strong) SigTransitionTime *remainingTime;
- (NSData *)parameters;
- (instancetype)initWithTargetXyLLightness:(UInt16)targetXyLLightness targetXyLX:(UInt16)targetXyLX targetXyLY:(UInt16)targetXyLY remainingTime:(SigTransitionTime *)remainingTime;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8289

/// 6.3.4.7 Light xyL Default Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.225)
@interface SigLightXyLDefaultGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x828A

/// 6.3.4.10 Light xyL Default Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.226)
@interface SigLightXyLDefaultStatus : SigGenericMessage
/// The value of the Light xyL Lightness state.
@property (nonatomic,assign) UInt16 lightness;
/// The value of the Light xyL x state.
@property (nonatomic,assign) UInt16 xyLX;
/// The value of the Light xyL y state.
@property (nonatomic,assign) UInt16 xyLY;
- (NSData *)parameters;
- (instancetype)initWithXyLLightness:(UInt16)lightness xyLX:(UInt16)xyLX xyLY:(UInt16)xyLY;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x828B

/// 6.3.4.11 Light xyL Range Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.227)
@interface SigLightXyLRangeGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x828C

/// 6.3.4.14 Light xyL Range Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.228)
@interface SigLightXyLRangeStatus : SigGenericMessage
/// Status Code for the requesting message..
@property (nonatomic, assign) SigGenericMessageStatus statusCode;
/// The value of the xyL x Range Min field of the Light xyL x Range state.
@property (nonatomic,assign) UInt16 xyLXRangeMin;
/// The value of the xyL x Range Max field of the Light xyL x Range state.
@property (nonatomic,assign) UInt16 xyLXRangeMax;
/// The value of the xyL y Range Min field of the Light xyL y Range state.
@property (nonatomic,assign) UInt16 xyLYRangeMin;
/// The value of the xyL y Range Max field of the Light xyL y Range state.
@property (nonatomic,assign) UInt16 xyLYRangeMax;
- (NSData *)parameters;
- (instancetype)initWithStatusCode:(SigGenericMessageStatus)statusCode xyLXRangeMin:(UInt16)xyLXRangeMin xyLXRangeMax:(UInt16)xyLXRangeMax xyLYRangeMin:(UInt16)xyLYRangeMin xyLYRangeMax:(UInt16)xyLYRangeMax;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x828D

/// 6.3.4.8 Light xyL Default Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.226)
@interface SigLightXyLDefaultSet : SigAcknowledgedGenericMessage
/// The value of the Light Lightness Default state.
@property (nonatomic,assign) UInt16 lightness;
/// The value of the Light xyL x Default state.
@property (nonatomic,assign) UInt16 xyLX;
/// The value of the Light xyL y Default state.
@property (nonatomic,assign) UInt16 xyLY;
- (NSData *)parameters;
- (instancetype)initWithLightness:(UInt16)lightness xyLX:(UInt16)xyLX xyLY:(UInt16)xyLY;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x828E

/// 6.3.4.9 Light xyL Default Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.226)
@interface SigLightXyLDefaultSetUnacknowledged : SigGenericMessage
/// The value of the Light Lightness Default state.
@property (nonatomic,assign) UInt16 lightness;
/// The value of the Light xyL x Default state.
@property (nonatomic,assign) UInt16 xyLX;
/// The value of the Light xyL y Default state.
@property (nonatomic,assign) UInt16 xyLY;
- (NSData *)parameters;
- (instancetype)initWithLightness:(UInt16)lightness xyLX:(UInt16)xyLX xyLY:(UInt16)xyLY;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x828F

/// 6.3.4.12 Light xyL Range Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.227)
@interface SigLightXyLRangeSet : SigAcknowledgedGenericMessage
/// The value of the xyL x Range Min field of the Light xyL x Range state.
@property (nonatomic,assign) UInt16 xyLXRangeMin;
/// The value of the xyL x Range Max field of the Light xyL x Range state.
@property (nonatomic,assign) UInt16 xyLXRangeMax;
/// The value of the xyL y Range Min field of the Light xyL y Range state.
@property (nonatomic,assign) UInt16 xyLYRangeMin;
/// The value of the xyL y Range Max field of the Light xyL y Range state.
@property (nonatomic,assign) UInt16 xyLYRangeMax;
- (NSData *)parameters;
- (instancetype)initWithXyLXRangeMin:(UInt16)xyLXRangeMin xyLXRangeMax:(UInt16)xyLXRangeMax xyLYRangeMin:(UInt16)xyLYRangeMin xyLYRangeMax:(UInt16)xyLYRangeMax;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8290

/// 6.3.4.13 Light xyL Range Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.227)
@interface SigLightXyLRangeSetUnacknowledged : SigGenericMessage
/// The value of the xyL x Range Min field of the Light xyL x Range state.
@property (nonatomic,assign) UInt16 xyLXRangeMin;
/// The value of the xyL x Range Max field of the Light xyL x Range state.
@property (nonatomic,assign) UInt16 xyLXRangeMax;
/// The value of the xyL y Range Min field of the Light xyL y Range state.
@property (nonatomic,assign) UInt16 xyLYRangeMin;
/// The value of the xyL y Range Max field of the Light xyL y Range state.
@property (nonatomic,assign) UInt16 xyLYRangeMax;
- (NSData *)parameters;
- (instancetype)initWithXyLXRangeMin:(UInt16)xyLXRangeMin xyLXRangeMax:(UInt16)xyLXRangeMax xyLYRangeMin:(UInt16)xyLYRangeMin xyLYRangeMax:(UInt16)xyLYRangeMax;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark - Light Control
#pragma mark opcode:0x8291

/// 6.3.5.1.1 Light LC Mode Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.229)
@interface SigLightLCModeGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8292

/// 6.3.5.1.2 Light LC Mode Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.229)
@interface SigLightLCModeSet : SigAcknowledgedGenericMessage
/// The target value of the Light LC Mode state.
@property (nonatomic,assign) UInt8 mode;
- (NSData *)parameters;
- (instancetype)initWithMode:(UInt8)mode;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8293

/// 6.3.5.1.3 Light LC Mode Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.229)
@interface SigLightLCModeSetUnacknowledged : SigGenericMessage
/// The target value of the Light LC Mode state.
@property (nonatomic,assign) UInt8 mode;
- (NSData *)parameters;
- (instancetype)initWithMode:(UInt8)mode;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8294

/// 6.3.5.1.4 Light LC Mode Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.229)
@interface SigLightLCModeStatus : SigGenericMessage
/// The target value of the Light LC Mode state.
@property (nonatomic,assign) UInt8 mode;
- (NSData *)parameters;
- (instancetype)initWithMode:(UInt8)mode;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8295

/// 6.3.5.2.1 Light LC OM Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.230)
@interface SigLightLCOMGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8296

/// 6.3.5.2.2 Light LC OM Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.229)
@interface SigLightLCOMSet : SigAcknowledgedGenericMessage
/// The target value of the Light LC Mode state.
@property (nonatomic,assign) UInt8 mode;
- (NSData *)parameters;
- (instancetype)initWithMode:(UInt8)mode;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x8297

/// 6.3.5.2.3 Light LC OM Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.229)
@interface SigLightLCOMSetUnacknowledged : SigGenericMessage
/// The target value of the Light LC Mode state.
@property (nonatomic,assign) UInt8 mode;
- (NSData *)parameters;
- (instancetype)initWithMode:(UInt8)mode;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8298

/// 6.3.5.2.4 Light LC OM Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.229)
@interface SigLightLCOMStatus : SigGenericMessage
/// The target value of the Light LC Mode state.
@property (nonatomic,assign) UInt8 mode;
- (NSData *)parameters;
- (instancetype)initWithMode:(UInt8)mode;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x8299

/// 6.3.5.3.1 Light LC Light OnOff Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.231)
@interface SigLightLCLightOnOffGet : SigAcknowledgedGenericMessage
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x829A

/// 6.3.5.3.2 Light LC Light OnOff Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.231)
@interface SigLightLCLightOnOffSet : SigAcknowledgedGenericMessage
/// The target value of the Light LC Light OnOff state.
@property (nonatomic,assign) BOOL lightOnOff;
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
- (NSData *)parameters;
- (instancetype)initWithLightOnOff:(BOOL)lightOnOff transitionTime:(SigTransitionTime *)transitionTime dalay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x829B

/// 6.3.5.3.3 Light LC Light OnOff Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.231)
@interface SigLightLCLightOnOffSetUnacknowledged : SigGenericMessage
/// The target value of the Light LC Light OnOff state.
@property (nonatomic,assign) BOOL lightOnOff;
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
@property (nonatomic,assign) UInt8 delay;
- (NSData *)parameters;
- (instancetype)initWithLightOnOff:(BOOL)lightOnOff transitionTime:(SigTransitionTime *)transitionTime dalay:(UInt8)delay;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x829C

/// 6.3.5.3.4 Light LC Light OnOff Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.232)
@interface SigLightLCLightOnOffStatus : SigGenericMessage
/// The present value of the Light LC Light OnOff state.
@property (nonatomic,assign) BOOL presentLightOnOff;
/// The target value of the Light LC Light OnOff state.
@property (nonatomic,assign) BOOL targetLightOnOff;
/// The Remaining Time field identifies the time that an element will take to transition to the target state from the present state.
@property (nonatomic,strong) SigTransitionTime *remainingTime;
- (NSData *)parameters;
- (instancetype)initWithPresentLightOnOff:(BOOL)presentLightOnOff targetLightOnOff:(BOOL)targetLightOnOff remainingTime:(SigTransitionTime *)remainingTime;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x829D

/// 6.3.6.1 Light LC Property Get
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.232)
@interface SigLightLCPropertyGet : SigAcknowledgedGenericMessage
/// Property ID identifying a Light LC Property.
@property (nonatomic,assign) UInt16 lightLCPropertyID;
- (NSData *)parameters;
- (instancetype)initWithLightLCPropertyID:(UInt16)lightLCPropertyID;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x62

/// 6.3.6.2 Light LC Property Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.233)
@interface SigLightLCPropertySet : SigAcknowledgedGenericMessage
/// Property ID identifying a Light LC Property.
@property (nonatomic,assign) UInt16 lightLCPropertyID;
/// Raw value for the Light LC Property.
@property (nonatomic,strong) NSData *lightLCPropertyValue;
- (NSData *)parameters;
- (instancetype)initWithLightLCPropertyID:(UInt16)lightLCPropertyID lightLCPropertyValue:(NSData *)lightLCPropertyValue;
- (instancetype)initWithParameters:(NSData *)parameters;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0x63

/// 6.3.6.3 Light LC Property Set Unacknowledged
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.233)
@interface SigLightLCPropertySetUnacknowledged : SigGenericMessage
/// Property ID identifying a Light LC Property.
@property (nonatomic,assign) UInt16 lightLCPropertyID;
/// Raw value for the Light LC Property.
@property (nonatomic,strong) NSData *lightLCPropertyValue;
- (NSData *)parameters;
- (instancetype)initWithLightLCPropertyID:(UInt16)lightLCPropertyID lightLCPropertyValue:(NSData *)lightLCPropertyValue;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x64

/// 6.3.6.4 Light LC Property Status
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.233)
@interface SigLightLCPropertyStatus : SigGenericMessage
/// Property ID identifying a Light LC Property.
@property (nonatomic,assign) UInt16 lightLCPropertyID;
/// Raw value for the Light LC Property.
@property (nonatomic,strong) NSData *lightLCPropertyValue;
- (NSData *)parameters;
- (instancetype)initWithLightLCPropertyID:(UInt16)lightLCPropertyID lightLCPropertyValue:(NSData *)lightLCPropertyValue;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0xB601

/// 8.4.1.1 Firmware Update Information Get
/// - seeAlso: MshMDL_DFU_MBT_CR_R04_LbL25.pdf  (page.81)
@interface SigFirmwareUpdateInformationGet : SigAcknowledgedGenericMessage
/// Index of the first requested entry from the Firmware Information List state.
@property (nonatomic,assign) UInt8 firstIndex;
/// Maximum number of entries that the server includes in a Firmware Update Information Status message.
@property (nonatomic,assign) UInt8 entriesLimit;
- (NSData *)parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
- (instancetype)initWithFirstIndex:(UInt8)firstIndex entriesLimit:(UInt8)entriesLimit;
/// The Type of the response message.
- (Class)responseType;
/// The Op Code of the response message.
- (UInt32)responseOpCode;
@end


#pragma mark opcode:0xB602

/// 3.2.2 Firmware Information Status
/// - seeAlso: Mesh_Firmware_update_20180228_d05r05.pdf  (page.21)
@interface SigFirmwareUpdateInformationStatus : SigGenericMessage
/// The number of entries in the Firmware Information List state of the server.
@property (nonatomic,assign) UInt8 firmwareInformationListCount;
/// Index of the first requested entry from the Firmware Information List state.
@property (nonatomic,assign) UInt8 firstIndex;
/// List of entries
@property (nonatomic,strong) NSMutableArray <SigFirmwareInformationEntryModel *>*firmwareInformationList;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0xB60A

/// 8.4.2.8 Firmware Distribution Get
/// - seeAlso: MshMDL_DFU_MBT_CR_R04_LbL35_JR_PW.pdf  (page.88)
@interface SigFirmwareDistributionGet : SigAcknowledgedGenericMessage
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0xB60B

/// 8.4.2.9 Firmware Distribution Start
/// - seeAlso: MshMDL_DFU_MBT_CR_R04_LbL35_JR_PW.pdf  (page.88)
@interface SigFirmwareDistributionStart : SigAcknowledgedGenericMessage
/// Index of the application key used in a firmware image distribution.
@property (nonatomic,assign) UInt16 distributionAppKeyIndex;
/// Time To Live value used in a firmware image distribution.
@property (nonatomic,assign) UInt8 distributionTTL;
/// Used to compute the timeout of the firmware image distribution.
@property (nonatomic,assign) UInt16 distributionTimeoutBase;
/// Mode of the transfer, szie is 2 bits.
@property (nonatomic,assign) SigTransferModeState distributionTransferMode;
/// Firmware update policy, szie is 1 bits.
@property (nonatomic,assign) BOOL updatePolicy;
/// Reserved for Future Use. Size is 5 bits.
@property (nonatomic,assign) UInt8 RFU;
/// Index of the firmware image in the Firmware Images List state to use during firmware image distribution.
@property (nonatomic,assign) UInt16 distributionFirmwareImageIndex;
/// Multicast address used in a firmware image distribution. Size is 16 bits or 128 bits.
@property (nonatomic,strong) NSData *distributionMulticastAddress;

- (instancetype)initWithDistributionAppKeyIndex:(UInt16)distributionAppKeyIndex distributionTTL:(UInt8)distributionTTL distributionTimeoutBase:(UInt16)distributionTimeoutBase distributionTransferMode:(SigTransferModeState)distributionTransferMode updatePolicy:(BOOL)updatePolicy RFU:(UInt8)RFU distributionFirmwareImageIndex:(UInt16)distributionFirmwareImageIndex distributionMulticastAddress:(NSData *)distributionMulticastAddress;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0xB60C

/// 8.4.2.10 Firmware Distribution Cancel
/// - seeAlso: MshMDL_DFU_MBT_CR_R04_LbL35_JR_PW.pdf  (page.89)
@interface SigFirmwareDistributionCancel : SigAcknowledgedGenericMessage
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0xB60D

/// 8.4.2.11 Firmware Distribution Apply
/// - seeAlso: MshMDL_DFU_MBT_CR_R04_LbL35_JR_PW.pdf  (page.89)
@interface SigFirmwareDistributionApply : SigAcknowledgedGenericMessage
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0xB60E

/// 8.4.2.12 Firmware Distribution Status
/// - seeAlso: MshMDL_DFU_MBT_CR_R04_LbL35_JR_PW.pdf  (page.89)
@interface SigFirmwareDistributionStatus : SigGenericMessage
/// Status Code for the requesting message.
@property (nonatomic,assign) SigFirmwareDistributionServerAndClientModelStatusType status;
/// Phase of the firmware image distribution.
@property (nonatomic,assign) SigDistributionPhaseState distributionPhase;
/// Multicast address used in firmware image distribution (Optional).
@property (nonatomic,assign) UInt16 distributionMulticastAddress;
/// Index of an application key used in a firmware image distribution (C.1). (C.1: If the Distribution Multicast Address field is present, the Distribution AppKey Index field, Distribution TTL field, Distribution Timeout Base field, Distribution Transfer Mode field, Update Policy field, RFU field, and the Distribution Firmware Image Index field shall also be present; otherwise, the Distribution AppKey Index field, Distribution TTL field, Distribution Timeout Base field, Distribution Transfer Mode field, Update Policy field, RFU field and the Distribution Firmware Image Index field shall not be present.)
@property (nonatomic,assign) UInt16 distributionAppKeyIndex;
/// Time To Live value used in a firmware image distribution (C.1).
@property (nonatomic,assign) UInt8 distributionTTL;
/// Used to compute the timeout of the firmware image distribution (C.1).
@property (nonatomic,assign) UInt16 distributionTimeoutBase;
/// Mode of the transfer, szie is 2 bits (C.1).
@property (nonatomic,assign) SigTransferModeState distributionTransferMode;
/// Firmware update policy, szie is 1 bits (C.1).
@property (nonatomic,assign) BOOL updatePolicy;
/// Reserved for Future Use. Size is 5 bits (C.1).
@property (nonatomic,assign) UInt8 RFU;
/// Index of the firmware image in the Firmware Images List state to use during firmware image distribution (C.1).
@property (nonatomic,assign) UInt16 distributionFirmwareImageIndex;

- (instancetype)initWithParameters:(NSData *)parameters;
@end


//#pragma mark opcode:0xB60D
//
///// 3.2.7 Firmware Distribution Details Get
///// - seeAlso: Mesh_Firmware_update_20180228_d05r05.pdf  (page.24)
//@interface SigFirmwareDistributionDetailsGet : SigAcknowledgedGenericMessage
///// Status code
//@property (nonatomic,assign) SigFirmwareDistributionStatusType status;
///// Company identifier
//@property (nonatomic,assign) UInt16 companyID;
///// Unique firmware identifier
//@property (nonatomic,strong) NSData *firmwareID;
//- (instancetype)initWithStatus:(SigFirmwareDistributionStatusType)status companyID:(UInt16)companyID firmwareID:(NSData *)firmwareID;
//- (instancetype)initWithParameters:(NSData *)parameters;
//@end


#pragma mark opcode:0xB60E

/// 3.2.8 Firmware Distribution Details List
/// - seeAlso: Mesh_Firmware_update_20180228_d05r05.pdf  (page.25)
@interface SigFirmwareDistributionDetailsList : SigGenericMessage
/// Firmware Distribution Details List
@property (nonatomic,strong) NSMutableArray <SigNodeUpdateStatusModel *>*detailsList;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0xB605

/// 8.4.1.5 Firmware Update Get
/// - seeAlso: MshMDL_DFU_MBT_CR_R04_LbL25.pdf  (page.83)
@interface SigFirmwareUpdateGet : SigAcknowledgedGenericMessage
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0xB603

/// 8.4.1.3 Firmware Update Firmware Metadata Check
/// - seeAlso: MshMDL_DFU_MBT_CR_R04_LbL25.pdf  (page.82)
@interface SigFirmwareUpdateFirmwareMetadataCheck : SigAcknowledgedGenericMessage
/// Index of the firmware image in the Firmware Information List state to check.
@property (nonatomic,assign) UInt8 updateFirmwareImageIndex;
/// Vendor-specific metadata (Optional). Size is 1 to 255.
@property (nonatomic,strong) NSData *incomingFirmwareMetadata;
- (instancetype)initWithUpdateFirmwareImageIndex:(UInt8)updateFirmwareImageIndex incomingFirmwareMetadata:(nullable NSData *)incomingFirmwareMetadata;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0xB604

/// 8.4.1.4 Firmware Update Firmware Metadata Status
/// - seeAlso: MshMDL_DFU_MBT_CR_R04_LbL25.pdf  (page.82)
@interface SigFirmwareUpdateFirmwareMetadataStatus : SigGenericMessage
/// Status Code from the firmware metadata check, size is 3 bits.
@property (nonatomic,assign) SigFirmwareUpdateServerAndClientModelStatusType status;
/// The Firmware Update Additional Information state from the Firmware Update Server (see Section 8.3.1.3), szie is 5 bits.
@property (nonatomic,assign) SigFirmwareUpdateAdditionalInformationStatusType additionalInformation;
/// Index of the firmware image in the Firmware Information List state that was checked.
@property (nonatomic,assign) UInt8 updateFirmwareImageIndex;
- (instancetype)initWithParameters:(NSData *)parameters;
@end




#pragma mark opcode:0xB606

/// 8.4.1.6 Firmware Update Start
/// - seeAlso: MshMDL_DFU_MBT_CR_R04_LbL25.pdf  (page.83)
@interface SigFirmwareUpdateStart : SigAcknowledgedGenericMessage
/// Time To Live value to use during firmware image transfer.
@property (nonatomic,assign) UInt8 updateTTL;
/// Used to compute the timeout of the firmware image transfer.
@property (nonatomic,assign) UInt16 updateTimeoutBase;
/// BLOB identifier for the firmware image.
@property (nonatomic,assign) UInt64 updateBLOBID;
/// Index of the firmware image in the Firmware Information List state to be updated.
@property (nonatomic,assign) UInt8 updateFirmwareImageIndex;
/// Vendor-specific firmware metadata (Optional). Size is 1 to 255.
@property (nonatomic,strong) NSData *incomingFirmwareMetadata;
- (instancetype)initWithUpdateTTL:(UInt8)updateTTL updateTimeoutBase:(UInt16)updateTimeoutBase updateBLOBID:(UInt64)updateBLOBID updateFirmwareImageIndex:(UInt8)updateFirmwareImageIndex incomingFirmwareMetadata:(nullable NSData *)incomingFirmwareMetadata;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0xB606

/// 8.4.1.7 Firmware Update Cancel
/// - seeAlso: MshMDL_DFU_MBT_CR_R04_LbL25.pdf  (page.84)
@interface SigFirmwareUpdateCancel : SigAcknowledgedGenericMessage
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0xB608

/// 8.4.1.8 Firmware Update Apply
/// - seeAlso: MshMDL_DFU_MBT_CR_R04_LbL25.pdf  (page.84)
@interface SigFirmwareUpdateApply : SigAcknowledgedGenericMessage
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0xB609

/// 8.4.1.9 Firmware Update Status
/// - seeAlso: MshMDL_DFU_MBT_CR_R04_LbL25.pdf  (page.84)
@interface SigFirmwareUpdateStatus : SigGenericMessage
/// Status Code for the requesting message, szie is 3 bits.
@property (nonatomic,assign) SigFirmwareUpdateServerAndClientModelStatusType status;
/// Reserved for Future Use. Size is 2 bits.
@property (nonatomic,assign) UInt8 RFU1;
/// The Update Phase state of the Firmware Update Server, szie is 3 bits.
@property (nonatomic,assign) SigFirmwareUpdatePhaseType updatePhase;
/// Time To Live value to use during firmware image transfer (Optional), size is 8 bits.
@property (nonatomic,assign) UInt8 updateTTL;
/// The Firmware Update Additional Information state from the Firmware Update Server (see Section 8.3.1.3) (C.1), szie is 5 bits. (C.1: If the Update TTL field is present, the Additional Information field, RFU2 field, Update Timeout Base field, Update BLOB ID field, and Firmware Image Index field shall be present; otherwise, the Additional Information field, RFU2 field, Update Timeout Base field, BLOB ID field, and Firmware Image Index field shall not be present.)
@property (nonatomic,assign) SigFirmwareUpdateAdditionalInformationStatusType additionalInformation;
/// Reserved for Future Use (C.1), szie is 3 bits.
@property (nonatomic,assign) UInt8 RFU2;
/// Used to compute the timeout of the firmware image transfer (C.1).
@property (nonatomic,assign) UInt16 updateTimeoutBase;
/// BLOB identifier for the firmware image (C.1).
@property (nonatomic,assign) UInt64 updateBLOBID;
/// The index of the firmware image in the Firmware Information List state being updated (C.1).
@property (nonatomic,assign) UInt8 updateFirmwareImageIndex;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0xB701

/// 7.3.1.1 BLOB Transfer Get
/// - seeAlso: MshMDL_DFU_MBT_CR_R04_LbL35_JR_PW.pdf  (page.26)
@interface SigBLOBTransferGet : SigAcknowledgedGenericMessage
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0xB702

/// 7.3.1.2 BLOB Transfer Start
/// - seeAlso: MshMDL_DFU_MBT_CR_R04_LbL25.pdf  (page.27)
@interface SigBLOBTransferStart : SigAcknowledgedGenericMessage
/// Reserved for Future Use, szie is 6 bits.
@property (nonatomic,assign) UInt8 RFU;
/// BLOB transfer mode, szie is 2 bits.
@property (nonatomic,assign) SigTransferModeState transferMode;
/// Unique BLOB identifier.
@property (nonatomic,assign) UInt64 BLOBID;
/// BLOB size in bytes
@property (nonatomic,assign) UInt32 BLOBSize;
/// Size of the block during this transfer.
@property (nonatomic,assign) UInt8 BLOBBlockSizeLog;
/// Maximum payload size supported by the client.
@property (nonatomic,assign) UInt16 MTUSize;
- (instancetype)initWithTransferMode:(SigTransferModeState)transferMode BLOBID:(UInt64)BLOBID BLOBSize:(UInt32)BLOBSize BLOBBlockSizeLog:(UInt8)BLOBBlockSizeLog MTUSize:(UInt16)MTUSize;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0xB703

/// 7.3.1.3 BLOB Transfer Cancel
/// - seeAlso: MshMDL_DFU_MBT_CR_R04_LbL35_JR_PW.pdf  (page.27)
@interface SigObjectTransferCancel : SigAcknowledgedGenericMessage
/// BLOB identifier.
@property (nonatomic,assign) UInt64 BLOBID;
- (instancetype)initWithBLOBID:(UInt64)BLOBID;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0xB704

/// 7.3.1.4 BLOB Transfer Status
/// - seeAlso: MshMDL_DFU_MBT_CR_R04_LbL25.pdf  (page.27)
@interface SigBLOBTransferStatus : SigGenericMessage
/// Status Code for the requesting message, szie is 4 bits.
@property (nonatomic,assign) SigBLOBTransferStatusType status;
/// Reserved for Future Use, szie is 2 bits.
@property (nonatomic,assign) UInt8 RFU;
/// BLOB transfer mode, szie is 2 bits.
@property (nonatomic,assign) SigTransferModeState transferMode;
/// Transfer phase, size is 8 bits.
@property (nonatomic,assign) SigTransferPhaseState transferPhase;
/// BLOB identifier (Optional)
@property (nonatomic,assign) UInt64 BLOBID;
/// BLOB size in octets (C.1). (C.1: If the BLOB ID field is present, then the BLOB Size field may be present; otherwise, the BLOB Size field shall not be present.)
@property (nonatomic,assign) UInt32 BLOBSize;
/// Indicates the block size (C.2). (C.2: If the BLOB Size field is present, then the Block Size Log, Transfer MTU Size, and Blocks Not Received fields shall be present; otherwise, these fields shall not be present.)
@property (nonatomic,assign) UInt8 blockSizeLog;
/// MTU size in octets (C.2).
@property (nonatomic,assign) UInt16 transferMTUSize;
/// Bit field indicating blocks that were not received (C.2), size is variable.
@property (nonatomic,strong) NSData *blocksNotReceived;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0xB705

/// 3.1.3.1.6 BLOB Block Start
/// - seeAlso: MshMDL_BLOB_CR_Vienna_IOP.pdf  (page.19)
@interface SigBLOBBlockStart : SigAcknowledgedGenericMessage
/// Block number
@property (nonatomic,assign) UInt16 blockNumber;
/// Chunk size bytes for this block
@property (nonatomic,assign) UInt16 chunkSize;
- (instancetype)initWithBlockNumber:(UInt16)blockNumber chunkSize:(UInt16)chunkSize;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0xB706

/// 3.3.6 Object Block Transfer Status
/// - seeAlso: Mesh_Firmware_update_20180228_d05r05.pdf  (page.32)
@interface SigObjectBlockTransferStatus : SigGenericMessage
/// Status of operation
@property (nonatomic,assign) SigObjectBlockTransferStatusType status;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x7D

/// 3.1.3.1.8 BLOB Chunk Transfer
/// - seeAlso: MshMDL_BLOB_CR_Vienna_IOP.pdf  (page.20)
@interface SigBLOBChunkTransfer : SigGenericMessage
/// Chunk number
@property (nonatomic,assign) UInt16 chunkNumber;
/// Part of the BLOB data, szie is 1 to Chunk Size.
@property (nonatomic,strong) NSData *chunkData;
@property (nonatomic,assign) BOOL sendBySegmentPdu;

- (instancetype)initWithChunkNumber:(UInt16)chunkNumber chunkData:(NSData *)chunkData sendBySegmentPdu:(BOOL)sendBySegmentPdu;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0xB707

/// 3.1.3.1.5 BLOB Block Get
/// - seeAlso: MshMDL_BLOB_CR_Vienna_IOP.pdf  (page.19)
@interface SigBLOBBlockGet : SigAcknowledgedGenericMessage
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0x7E

/// 3.1.3.1.7 BLOB Block Status
/// - seeAlso: MshMDL_BLOB_CR_Vienna_IOP.pdf  (page.19)
@interface SigBLOBBlockStatus : SigGenericMessage
/// Status code of the block transfer, size is 4 bits.
@property (nonatomic,assign) SigBLOBBlockStatusType status;
/// Reserved for Future Use, size is 2 bits.
@property (nonatomic,assign) UInt8 RFU;
/// Indicates the format used to report missing chunks, size is 2 bits.
@property (nonatomic,assign) SigBLOBBlockFormatType format;
/// Transfer phase.
@property (nonatomic,assign) SigTransferPhaseState transferPhase;
/// Block’s number in a set of blocks.
@property (nonatomic,assign) UInt16 blockNumber;
/// Chunk Size (in octets) for the current block.
@property (nonatomic,assign) UInt16 chunkSize;
/// Bit field of missing chunks for this block (C.1). (C.1: If the Format field is set to Some Chunks Missing, the Missing Chunks field shall be present; otherwise, it shall not be present.)
@property (nonatomic,strong) NSMutableArray <NSNumber *>*missingChunksList;//[@(node.address)]
/// List of chunks requested by the server (C.2). (C.2 If the Format field is set to Encoded Missing Chunks, the Encoded Missing Chunks field shall be present; otherwise, it shall not be present.)
@property (nonatomic,strong) NSMutableArray <NSNumber *>*encodedMissingChunksList;//[@(node.address)]
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0xB70A

/// 3.1.3.1.9 BLOB Information Get
/// - seeAlso: MshMDL_BLOB_CR_Vienna_IOP.pdf  (page.21)
@interface SigBLOBInformationGet : SigAcknowledgedGenericMessage
- (instancetype)initWithParameters:(NSData *)parameters;
@end


#pragma mark opcode:0xB70B

/// 3.1.3.1.10 BLOB Information Status
/// - seeAlso: MshMDL_BLOB_CR_Vienna_IOP.pdf  (page.21)
@interface SigBLOBInformationStatus : SigGenericMessage
/// Minimum block size: 2 ^ Min Block Size Log
@property (nonatomic,assign) UInt8 minBlockSizeLog;
/// Maximum block size: 2 ^ Max Block Size Log
@property (nonatomic,assign) UInt8 maxBlockSizeLog;
/// Supported maximum number of chunks in block
@property (nonatomic,assign) UInt16 maxChunksNumber;
/// Maximum size of chunk supported by the server
@property (nonatomic,assign) UInt16 maxChunkSize;
/// Maximum BLOB size supported by the server
@property (nonatomic,assign) UInt32 maxBLOBSize;
/// Maximum payload size supported by the server
@property (nonatomic,assign) UInt16 MTUSize;
/// BLOB transfer modes supported by the server
@property (nonatomic,assign) UInt8 supportedTransferMode;
- (instancetype)initWithParameters:(NSData *)parameters;
@end


@interface SigTelinkOnlineStatusMessage : SigMeshMessage
@property (nonatomic,assign) UInt16 address;
@property (nonatomic,assign) DeviceState state;
@property (nonatomic,assign) UInt8 brightness;
@property (nonatomic,assign) UInt8 temperature;
- (instancetype)initWithAddress:(UInt16)address state:(DeviceState)state brightness:(UInt8)brightness temperature:(UInt8)temperature;
@end


NS_ASSUME_NONNULL_END
