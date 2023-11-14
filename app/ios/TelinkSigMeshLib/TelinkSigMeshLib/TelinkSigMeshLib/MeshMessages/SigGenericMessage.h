/********************************************************************************************************
 * @file     SigGenericMessage.h
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/11/12
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


#pragma mark - 3.2 Generic messages


#pragma mark - 3.2.1 Generic OnOff messages


#pragma mark 3.2.1.1 Generic OnOff Get, opcode:0x8201


/// Generic OnOff Get is an acknowledged message used to get
/// the Generic OnOff state of an element (see Section 3.1.1).
/// @note   The response to the Generic OnOff Get message
/// is a Generic OnOff Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.42),
/// 3.2.1.1 Generic OnOff Get.
@interface SigGenericOnOffGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigGenericOnOffGet object.
 * @param   parameters    the hex data of SigGenericOnOffGet.
 * @return  return `nil` when initialize SigGenericOnOffGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 3.2.1.2 Generic OnOff Set, opcode:0x8202


/// Generic OnOff Set is an acknowledged message used to set
/// the Generic OnOff state of an element (see Section 3.1.1).
/// @note   The response to the Generic OnOff Set message
/// is a Generic OnOff Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.43),
/// 3.2.1.2 Generic OnOff Set.
@interface SigGenericOnOffSet : SigAcknowledgedGenericMessage
/// The target value of the Generic OnOff state.
/// The OnOff field identifies the Generic OnOff state of the element (see Section 3.1.1).
@property (nonatomic,assign) BOOL isOn;
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Transition Time field identifies the time that an element
/// will take to transition to the target state from the present state (see
/// Section 1.4.1.1). The format of the Transition Time field matches the
/// format of the Generic Default Transition Time state as defined in
/// Section 3.1.3. Only values of 0x00 through 0x3E shall be used to
/// specify the value of the Transition Number of Steps field.
@property (nonatomic,strong,nullable) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps (C.1)
/// C.1: If the Transition Time field is present, the Delay field shall also be
/// present; otherwise these fields shall not be present.
/// The Delay field shall be present when the Transition Time field is present.
/// It identifies the message execution delay, representing a time interval
/// between receiving the message by a model and executing the
/// associated model behaviors.
@property (nonatomic,assign) UInt8 delay;

/// Creates the Generic OnOff Set message.
///
/// @param isOn The target value of the Generic OnOff state.
- (instancetype)initWithIsOn:(BOOL)isOn;

/// Creates the Generic OnOff Set message.
/// @param isOn The target value of the Generic OnOff state.
/// @param transitionTime The time that an element will take to transition to the target state from the present state.
/// @param delay Message execution delay in 5 millisecond steps.
- (instancetype)initWithIsOn:(BOOL)isOn transitionTime:(nullable SigTransitionTime *)transitionTime dalay:(UInt8)delay;

/**
 * @brief   Initialize SigGenericOnOffSet object.
 * @param   parameters    the hex data of SigGenericOnOffSet.
 * @return  return `nil` when initialize SigGenericOnOffSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 3.2.1.3 Generic OnOff Set Unacknowledged, opcode:0x8203


/// Generic OnOff Set Unacknowledged is an unacknowledged message used to set
/// the Generic OnOff state of an element (see Section 3.1.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.43),
/// 3.2.1.3 Generic OnOff Set Unacknowledged.
@interface SigGenericOnOffSetUnacknowledged : SigGenericMessage
/// The target value of the Generic OnOff state.
/// The OnOff field identifies the Generic OnOff state of the element (see Section 3.1.1).
@property (nonatomic,assign) BOOL isOn;
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Transition Time field identifies the time that an element
/// will take to transition to the target state from the present state (see
/// Section 1.4.1.1). The format of the Transition Time field matches the
/// format of the Generic Default Transition Time state as defined in
/// Section 3.1.3. Only values of 0x00 through 0x3E shall be used to
/// specify the value of the Transition Number of Steps field.
@property (nonatomic,strong,nullable) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps (C.1)
/// C.1: If the Transition Time field is present, the Delay field shall also be
/// present; otherwise these fields shall not be present.
/// The Delay field shall be present when the Transition Time field is present.
/// It identifies the message execution delay, representing a time interval
/// between receiving the message by a model and executing the
/// associated model behaviors.
@property (nonatomic,assign) UInt8 delay;

/// Creates the Generic OnOff Set Unacknowledged message.
/// @param isOn  The target value of the Generic OnOff state.
- (instancetype)initWithIsOn:(BOOL)isOn;

/// Creates the Generic OnOff Set Unacknowledged message.
/// @param isOn  The target value of the Generic OnOff state.
/// @param transitionTime  The time that an element will take to transition to the target state from the present state.
/// @param delay  Message execution delay in 5 millisecond steps.
- (instancetype)initWithIsOn:(BOOL)isOn transitionTime:(nullable SigTransitionTime *)transitionTime dalay:(UInt8)delay;

/**
 * @brief   Initialize SigGenericOnOffSetUnacknowledged object.
 * @param   parameters    the hex data of SigGenericOnOffSetUnacknowledged.
 * @return  return `nil` when initialize SigGenericOnOffSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 3.2.1.4 Generic OnOff Status, opcode:0x8204


/// Generic OnOff Status is an unacknowledged message used to report
/// the Generic OnOff state of an element (see Section 3.1.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.44),
/// 3.2.1.4 Generic OnOff Status.
@interface SigGenericOnOffStatus : SigGenericMessage
/// The present value of the Generic OnOff state.
@property (nonatomic,assign) BOOL isOn;
/// The target value of the Generic OnOff state (optional).
@property (nonatomic,assign) BOOL targetState;
/// Format as defined in Section 3.1.3. (C.1)
/// C.1: If the Target OnOff field is present, the Remaining Time field shall also be present;
/// otherwise these fields shall not be present.
/// If present, the Remaining Time field identifies the time it will take the element
/// to complete the transition to the target Generic OnOff state of the node (see
/// Section 1.4.1.1 and 3.1.1).
@property (nonatomic,strong,nullable) SigTransitionTime *remainingTime;

/// Creates the Generic OnOff Status message.
/// @param isOn  The current value of the Generic OnOff state.
- (instancetype)initWithIsOn:(BOOL)isOn;

/// Creates the Generic OnOff Status message.
/// @param isOn  The current value of the Generic OnOff state.
/// @param targetState  The target value of the Generic OnOff state.
/// @param remainingTime  The time that an element will take to transition to the target state from the present state.
- (instancetype)initWithIsOn:(BOOL)isOn targetState:(BOOL)targetState remainingTime:(nullable SigTransitionTime *)remainingTime;

/**
 * @brief   Initialize SigGenericOnOffStatus object.
 * @param   parameters    the hex data of SigGenericOnOffStatus.
 * @return  return `nil` when initialize SigGenericOnOffStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark - 3.2.2 Generic Level messages


#pragma mark 3.2.2.1 Generic Level Get, opcode:0x8205


/// Generic Level Get is an acknowledged message used to get
/// the Generic Level state of an element (see Section 3.1.2).
/// @note   The response to the Generic Level Get message is
/// a Generic Level Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.44),
/// 3.2.2.1 Generic Level Get.
@interface SigGenericLevelGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigGenericLevelGet object.
 * @param   parameters    the hex data of SigGenericLevelGet.
 * @return  return `nil` when initialize SigGenericLevelGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 3.2.2.2 Generic Level Set, opcode:0x8206


/// Generic Level Set is an acknowledged message used to set the
/// Generic Level state of an element (see Section 3.1.2) to a new absolute value.
/// @note   The response to the Generic Level Set message is a
/// Generic Level Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.44),
/// 3.2.2.2 Generic Level Set.
@interface SigGenericLevelSet : SigAcknowledgedGenericMessage
/// The target value of the Generic Level state.
/// The Level field identifies the Generic Level state of the element (see Section 3.1.2).
@property (nonatomic,assign) UInt16 level;
/// The Transition Time field identifies the time that an element will take to transition to the target state from the present state.
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Transition Time field identifies the time that an element will take to
/// transition to the target state from the present state (see Section 1.4.1.1).
/// The format of the Transition Time field matches the format of the Generic Default
/// Transition Time state as defined in Section 3.1.3. Only values of 0x00 through 0x3E
/// shall be used to specify the Transition Number of Steps.
@property (nonatomic,strong,nullable) SigTransitionTime *transitionTime;
/// Message execution delay in 5 milliseconds steps (C.1).
/// C.1: If the Transition Time field is present, the Delay field shall also be present;
/// otherwise these fields shall not be present.
/// The Delay field shall be present when the Transition Time field is present.
/// It identifies the message execution delay, representing a time interval
/// between receiving the message by a model and executing the
/// associated model behaviors.
@property (nonatomic,assign) UInt8 delay;

/// Creates the Generic Level Set message.
///
/// @param level  The target value of the Generic Level state.
- (instancetype)initWithLevel:(UInt16)level;

/// Creates the Generic Level Set message.
/// @param level The target value of the Generic Level state.
/// @param transitionTime The time that an element will take to transition to the target state from the present state.
/// @param delay Message execution delay in 5 millisecond steps.
- (instancetype)initWithLevel:(UInt16)level transitionTime:(nullable SigTransitionTime *)transitionTime dalay:(UInt8)delay;

/**
 * @brief   Initialize SigGenericLevelSet object.
 * @param   parameters    the hex data of SigGenericLevelSet.
 * @return  return `nil` when initialize SigGenericLevelSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 3.2.2.3 Generic Level Set Unacknowledged, opcode:0x8207


/// Generic Level Set Unacknowledged is an unacknowledged message used to set
/// the Generic Level state of an element (see Section 3.1.2) to a new absolute value.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.45),
/// 3.2.2.3 Generic Level Set Unacknowledged.
@interface SigGenericLevelSetUnacknowledged : SigGenericMessage
/// The target value of the Generic Level state.
/// The Level field identifies the Generic Level state of the element (see Section 3.1.2).
@property (nonatomic,assign) UInt16 level;
/// The Transition Time field identifies the time that an element will take to transition to
/// the target state from the present state.
/// Format as defined in Section 3.1.3. (Optional).
/// If present, the Transition Time field identifies the time that an element will take to
/// transition to the target state from the present state (see Section 1.4.1.1). The format
/// of the Transition Time field matches the format of the Generic Default Transition
/// Time state as defined in Section 3.1.3. Only values of 0x00 through 0x3E shall be
/// used to specify the Transition Number of Steps.
@property (nonatomic,strong,nullable) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
/// The Delay field shall be present when the Transition Time field is present. It identifies
/// the message execution delay, representing a time interval between receiving the
/// message by a model and executing the associated model behaviors.
@property (nonatomic,assign) UInt8 delay;

/// Creates the Generic Level Set Unacknowledged message.
/// @param level  The target value of the Generic Level state.
- (instancetype)initWithLevel:(UInt16)level;

/// Creates the Generic Level Set Unacknowledged message.
/// @param level  The target value of the Generic Level state.
/// @param transitionTime  The time that an element will take to transition to the target state from the present state.
/// @param delay  Message execution delay in 5 millisecond steps.
- (instancetype)initWithLevel:(UInt16)level transitionTime:(nullable SigTransitionTime *)transitionTime dalay:(UInt8)delay;

/**
 * @brief   Initialize SigGenericLevelSetUnacknowledged object.
 * @param   parameters    the hex data of SigGenericLevelSetUnacknowledged.
 * @return  return `nil` when initialize SigGenericLevelSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 3.2.2.8 Generic Level Status, opcode:0x8208


/// Generic Level Status is an unacknowledged message used to report
/// the Generic Level state of an element (see Section 3.1.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.48),
/// 3.2.2.8 Generic Level Status.
@interface SigGenericLevelStatus : SigGenericMessage
/// The present value of the Generic Level state.
/// The Present Level field identifies the present Generic Level state of
/// the element (see Section 3.1.2).
@property (nonatomic,assign) UInt16 level;
/// The target value of the Generic Level state (Optional).
/// If present, the Target Level field identifies the target Generic Level state
/// that the element is to reach (see Section 3.1.2).
@property (nonatomic,assign) UInt16 targetLevel;
/// The Remaining Time field identifies the time that an element will take to
/// transition to the target state from the present state.
/// Format as defined in Section 3.1.3 (C.1).
/// C.1: If the Target Level field is present, the Remaining Time field shall
/// also be present; otherwise these fields shall not be present.
/// If present, the Remaining Time field identifies the time that it will take
/// the element to complete the transition to the target Generic Level state
/// of the element (see Section 3.1.2).
@property (nonatomic,strong,nullable) SigTransitionTime *remainingTime;

/// Creates the Generic Level Status message.
/// @param level  The current value of the Generic Level state.
- (instancetype)initWithLevel:(UInt16)level;

/// Creates the Generic Level Status message.
/// @param level  The current value of the Generic Level state.
/// @param targetLevel  The target value of the Generic Level state.
/// @param remainingTime  The time that an element will take to transition to the target state from the present state.
- (instancetype)initWithLevel:(UInt16)level targetLevel:(BOOL)targetLevel remainingTime:(nullable SigTransitionTime *)remainingTime;

/**
 * @brief   Initialize SigGenericLevelStatus object.
 * @param   parameters    the hex data of SigGenericLevelStatus.
 * @return  return `nil` when initialize SigGenericLevelStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 3.2.2.4 Generic Delta Set, opcode:0x8209


/// Generic Delta Set is an acknowledged message used to set the
/// Generic Level state of an element (see Section 3.1.2) by a relative
/// value. The message is transactional – it supports changing the
/// state by a cumulative value with a sequence of messages that
/// are part of a transaction.
/// @note   The response to the Generic Delta Set message is
/// a Generic Level Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.46),
/// 3.2.2.4 Generic Delta Set.
@interface SigGenericDeltaSet : SigAcknowledgedGenericMessage
/// The Delta change of the Generic Level state.
/// The Delta Level field identifies the increase (when positive) or
/// decrease (if negative) of the Generic Level state of the
/// element (see Section 3.1.2).
@property (nonatomic,assign) UInt32 delta;
/// Format as defined in Section 3.1.3. (Optional).
/// The Transition Time field identifies the time that an element will
/// take to transition to the target state from the present state. The
/// format of the Transition Time field matches the format of the
/// Generic Default Transition Time state as defined in Section 3.1.3.
/// Only values of 0x00 through 0x3E shall be used to specify the
/// Transition Number of Steps.
@property (nonatomic,strong,nullable) SigTransitionTime *transitionTime;
/// Message execution delay in 5 milliseconds steps (C.1).
/// C.1: If the Transition Time field is present, the Delay field shall also
/// be present; otherwise these fields shall not be present.
/// The Delay field shall be present when the Transition Time field is
/// present. It identifies the message execution delay, representing a
/// time interval between receiving the message by a model and
/// executing the associated model behaviors.
@property (nonatomic,assign) UInt8 delay;

/// Creates the Generic Level Set message.
/// @param delta  The Delta change of the Generic Level state.
- (instancetype)initWithDelta:(UInt32)delta;

/// Creates the Generic Level Set message.
/// @param delta  The Delta change of the Generic Level state.
/// @param transitionTime The time that an element will take to transition to the target state from the present state.
/// @param delay Message execution delay in 5 millisecond steps.
- (instancetype)initWithDelta:(UInt32)delta transitionTime:(SigTransitionTime *)transitionTime dalay:(UInt8)delay;

/**
 * @brief   Initialize SigGenericDeltaSet object.
 * @param   parameters    the hex data of SigGenericDeltaSet.
 * @return  return `nil` when initialize SigGenericDeltaSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 3.2.2.5 Generic Delta Set Unacknowledged, opcode:0x820A


/// Generic Delta Set is an acknowledged message used to set the
/// Generic Level state of an element (see Section 3.1.2) by a relative
/// value. The message is transactional – it supports changing the
/// state by a cumulative value with a sequence of messages that
/// are part of a transaction.
/// @note   The response to the Generic Delta Set message is
/// a Generic Level Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.46),
/// 3.2.2.5 Generic Delta Set Unacknowledged.
@interface SigGenericDeltaSetUnacknowledged : SigGenericMessage
/// The Delta change of the Generic Level state.
/// The Delta Level field identifies the increase (when positive) or
/// decrease (if negative) of the Generic Level state of the
/// element (see Section 3.1.2).
@property (nonatomic,assign) UInt32 delta;
/// Format as defined in Section 3.1.3. (Optional).
/// The Transition Time field identifies the time that an element will
/// take to transition to the target state from the present state. The
/// format of the Transition Time field matches the format of the
/// Generic Default Transition Time state as defined in Section 3.1.3.
/// Only values of 0x00 through 0x3E shall be used to specify the
/// Transition Number of Steps.
@property (nonatomic,strong,nullable) SigTransitionTime *transitionTime;
/// Message execution delay in 5 milliseconds steps (C.1).
/// C.1: If the Transition Time field is present, the Delay field shall also
/// be present; otherwise these fields shall not be present.
/// The Delay field shall be present when the Transition Time field is
/// present. It identifies the message execution delay, representing a
/// time interval between receiving the message by a model and
/// executing the associated model behaviors.
@property (nonatomic,assign) UInt8 delay;

/// Creates the Generic Level Set Unacknowledged message.
/// @param delta  The Delta change of the Generic Level state.
- (instancetype)initWithDelta:(UInt32)delta;

/// Creates the Generic Level Set Unacknowledged message.
/// @param delta  The Delta change of the Generic Level state.
/// @param transitionTime The time that an element will take to transition to the target state from the present state.
/// @param delay Message execution delay in 5 millisecond steps.
- (instancetype)initWithDelta:(UInt32)delta transitionTime:(SigTransitionTime *)transitionTime dalay:(UInt8)delay;

/**
 * @brief   Initialize SigGenericDeltaSetUnacknowledged object.
 * @param   parameters    the hex data of SigGenericDeltaSetUnacknowledged.
 * @return  return `nil` when initialize SigGenericDeltaSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 3.2.2.6 Generic Move Set, opcode:0x820B


/// Generic Move Set is an acknowledged message used to start a process of
/// changing the Generic Level state of an element (see Section 3.1.2) with a
/// defined transition speed.
/// @note   The response to the Generic Move Set message is a
/// Generic Level Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.47),
/// 3.2.2.6 Generic Move Set.
@interface SigGenericMoveSet : SigAcknowledgedGenericMessage
/// The Delta Level step to calculate Move speed for the Generic Level state.
/// The Delta Level field shall be used to calculate the speed of the transition
/// of the Generic Level state.
@property (nonatomic,assign) UInt16 deltaLevel;
/// The Transition Time field identifies the time that an element will take to
/// transition to the target state from the present state.
/// Format as defined in Section 3.1.3 (optional).
/// If present, the Transition Time field shall be used to calculate the speed
/// of the transition of the Generic Level state. The format of the Transition
/// Time field matches the format of the Generic Default Transition Time state
/// as defined in Section 3.1.3. Only values of 0x00 through 0x3E shall be
/// used to specify the Transition Number of Steps. If the resulting Transition
/// Time is equal to 0 or is undefined, the Generic Move Set command will
/// not initiate any Generic Level state change.
@property (nonatomic,strong,nullable) SigTransitionTime *transitionTime;
/// Message execution delay in 5 milliseconds steps (C.1).
/// C.1: If the Transition Time field is present, the Delay field shall also be present;
/// otherwise these fields shall not be present.
/// The Delay field shall be present when the Transition Time field is present.
/// It identifies the message execution delay, representing a time interval
/// between receiving the message by a model and executing the associated
/// model behaviors.
@property (nonatomic,assign) UInt8 delay;

/// Creates the Generic Level Set message.
/// @param deltaLevel  The Delta Level step to calculate Move speed for the Generic Level state.
- (instancetype)initWithDeltaLevel:(UInt16)deltaLevel;

/// Creates the Generic Level Set message.
/// @param deltaLevel  The Delta Level step to calculate Move speed for the Generic Level state.
/// @param transitionTime The time that an element will take to transition to the target state from the present state.
/// @param delay Message execution delay in 5 millisecond steps.
- (instancetype)initWithDeltaLevel:(UInt16)deltaLevel transitionTime:(SigTransitionTime *)transitionTime dalay:(UInt8)delay;

/**
 * @brief   Initialize SigGenericMoveSet object.
 * @param   parameters    the hex data of SigGenericMoveSet.
 * @return  return `nil` when initialize SigGenericMoveSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 3.2.2.7 Generic Move Set Unacknowledged, opcode:0x820C


/// Generic Move Set Unacknowledged is an unacknowledged message
/// used to start a process of changing the Generic Level state of an
/// element (see Section 3.1.2) with a defined transition speed.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.47),
/// 3.2.2.7 Generic Move Set Unacknowledged.
@interface SigGenericMoveSetUnacknowledged : SigGenericMessage
/// The Delta Level step to calculate Move speed for the Generic Level state.
/// The Delta Level field shall be used to calculate the speed of the transition
/// of the Generic Level state.
@property (nonatomic,assign) UInt16 deltaLevel;
/// The Transition Time field identifies the time that an element will take to
/// transition to the target state from the present state.
/// Format as defined in Section 3.1.3 (optional).
/// If present, the Transition Time field shall be used to calculate the speed
/// of the transition of the Generic Level state. The format of the Transition
/// Time field matches the format of the Generic Default Transition Time state
/// as defined in Section 3.1.3. Only values of 0x00 through 0x3E shall be
/// used to specify the Transition Number of Steps. If the resulting Transition
/// Time is equal to 0 or is undefined, the Generic Move Set command will
/// not initiate any Generic Level state change.
@property (nonatomic,strong,nullable) SigTransitionTime *transitionTime;
/// Message execution delay in 5 milliseconds steps (C.1).
/// C.1: If the Transition Time field is present, the Delay field shall also be present;
/// otherwise these fields shall not be present.
/// The Delay field shall be present when the Transition Time field is present.
/// It identifies the message execution delay, representing a time interval
/// between receiving the message by a model and executing the associated
/// model behaviors.
@property (nonatomic,assign) UInt8 delay;

/// Creates the Generic Level Set message.
/// @param deltaLevel  The Delta Level step to calculate Move speed for the Generic Level state.
- (instancetype)initWithDeltaLevel:(UInt16)deltaLevel;

/// Creates the Generic Level Set message.
/// @param deltaLevel  The Delta Level step to calculate Move speed for the Generic Level state.
/// @param transitionTime The time that an element will take to transition to the target state from the present state.
/// @param delay Message execution delay in 5 millisecond steps.
- (instancetype)initWithDeltaLevel:(UInt16)deltaLevel transitionTime:(SigTransitionTime *)transitionTime dalay:(UInt8)delay;

/**
 * @brief   Initialize SigGenericMoveSetUnacknowledged object.
 * @param   parameters    the hex data of SigGenericMoveSetUnacknowledged.
 * @return  return `nil` when initialize SigGenericMoveSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark - 3.2.3 Generic Default Transition Time messages


#pragma mark 3.2.3.1 Generic Default Transition Time Get, opcode:0x820D


/// Generic Default Transition Time Get is an acknowledged message
/// used to get the Generic Default Transition Time state of an
/// element (see Section 3.1.3).
/// @note   The response to the Generic Default Transition Time
/// Get message is a Generic Default Transition Time Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.48),
/// 3.2.3.1 Generic Default Transition Time Get.
@interface SigGenericDefaultTransitionTimeGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigGenericDefaultTransitionTimeGet object.
 * @param   parameters    the hex data of SigGenericDefaultTransitionTimeGet.
 * @return  return `nil` when initialize SigGenericDefaultTransitionTimeGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 3.2.3.2 Generic Default Transition Time Set, opcode:0x820E


/// Generic Default Transition Time Set is an acknowledged message used to set
/// the Generic Default Transition Time state of an element (see Section 3.1.3).
/// @note   The response to the Generic Default Transition Time Set message
/// is a Generic Default Transition Time Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.49),
/// 3.2.3.2 Generic Default Transition Time Set.
@interface SigGenericDefaultTransitionTimeSet : SigAcknowledgedGenericMessage
/// The Transition Time field identifies the time that an element will take to transition
/// to the target state from the present state.
/// The Transition Time field identifies the Generic Default Transition Time state of
/// the element (see Section 3.1.3). Only values of 0x00 through 0x3E shall be used
/// to specify the Transition Number of Steps.
@property (nonatomic,strong) SigTransitionTime *transitionTime;

/// Creates the Generic Default Transition Time Set message.
/// @param transitionTime  The default time that an element will take to transition to the target state from the present state.
- (instancetype)initWithTransitionTime:(SigTransitionTime *)transitionTime;

/**
 * @brief   Initialize SigGenericDefaultTransitionTimeSet object.
 * @param   parameters    the hex data of SigGenericDefaultTransitionTimeSet.
 * @return  return `nil` when initialize SigGenericDefaultTransitionTimeSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 3.2.3.3 Generic Default Transition Time Set Unacknowledged, opcode:0x820F


/// Generic Default Transition Time Set Unacknowledged is an unacknowledged message
/// used to set the Generic Default Transition Time state of an element (see Section 3.1.3).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.49),
/// 3.2.3.3 Generic Default Transition Time Set Unacknowledged.
@interface SigGenericDefaultTransitionTimeSetUnacknowledged : SigGenericMessage
/// The Transition Time field identifies the time that an element will take to transition
/// to the target state from the present state.
/// The Transition Time field identifies the Generic Default Transition Time state of
/// the element (see Section 3.1.3).
@property (nonatomic,strong) SigTransitionTime *transitionTime;

/// Creates the Generic Default Transition Time Set message.
/// @param transitionTime  The default time that an element will take to transition to the target state from the present state.
- (instancetype)initWithTransitionTime:(SigTransitionTime *)transitionTime;

/**
 * @brief   Initialize SigGenericDefaultTransitionTimeSetUnacknowledged object.
 * @param   parameters    the hex data of SigGenericDefaultTransitionTimeSetUnacknowledged.
 * @return  return `nil` when initialize SigGenericDefaultTransitionTimeSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 3.2.3.4 Generic Default Transition Time Status, opcode:0x8210


/// Generic Default Transition Time Set Unacknowledged is an unacknowledged message
/// used to set the Generic Default Transition Time state of an element (see Section 3.1.3).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.49),
/// 3.2.3.4 Generic Default Transition Time Status.
@interface SigGenericDefaultTransitionTimeStatus : SigGenericMessage
/// The Transition Time field identifies the time that an element will take to transition to
/// the target state from the present state.
/// The Transition Time field identifies the Generic Default Transition Time state of the
/// element (see Section 3.1.3).
@property (nonatomic,strong) SigTransitionTime *transitionTime;

/// Creates the Generic Default Transition Time Set message.
/// @param transitionTime  The default time that an element will take to transition to the target state from the present state.
- (instancetype)initWithTransitionTime:(SigTransitionTime *)transitionTime;

/**
 * @brief   Initialize SigGenericDefaultTransitionTimeStatus object.
 * @param   parameters    the hex data of SigGenericDefaultTransitionTimeStatus.
 * @return  return `nil` when initialize SigGenericDefaultTransitionTimeStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark - 3.2.4 Generic OnPowerUp messages


#pragma mark 3.2.4.1 Generic OnPowerUp Get, opcode:0x8211


/// Generic OnPowerUp Get is an acknowledged message used to get
/// the Generic OnPowerUp state of an element (see Section 3.1.3).
/// @note   The response to the Generic OnPowerUp Get message
/// is a Generic OnPowerUp Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.49),
/// 3.2.4.1 Generic OnPowerUp Get.
@interface SigGenericOnPowerUpGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigGenericOnPowerUpGet object.
 * @param   parameters    the hex data of SigGenericOnPowerUpGet.
 * @return  return `nil` when initialize SigGenericOnPowerUpGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 3.2.4.4 Generic OnPowerUp Status, opcode:0x8212


/// Generic OnPowerUp Status is an unacknowledged message used to report
/// the Generic OnPowerUp state of an element (see Section 3.1.3).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.50),
/// 3.2.4.4 Generic OnPowerUp Status.
@interface SigGenericOnPowerUpStatus : SigGenericMessage
/// The value of the Generic OnPowerUp state.
/// The OnPowerUp field identifies the Generic OnPowerUp state of
/// the element (see Section 3.1.3).
@property (nonatomic,assign) SigOnPowerUp state;

/// Creates the Generic On Power Up Status message.
/// @param state  The value of the Generic OnPowerUp state.
- (instancetype)initWithState:(SigOnPowerUp)state;

/**
 * @brief   Initialize SigGenericOnPowerUpStatus object.
 * @param   parameters    the hex data of SigGenericOnPowerUpStatus.
 * @return  return `nil` when initialize SigGenericOnPowerUpStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 3.2.4.2 Generic OnPowerUp Set, opcode:0x8213


/// Generic OnPowerUp Set is an acknowledged message used to set
/// the Generic OnPowerUp state of an element (see Section 3.1.3).
/// @note   The response to the Generic OnPowerUp Set message
/// is a Generic OnPowerUp Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.50),
/// 3.2.4.2 Generic OnPowerUp Set.
@interface SigGenericOnPowerUpSet : SigAcknowledgedGenericMessage
/// The value of the Generic OnPowerUp state.
/// The OnPowerUp field identifies the Generic OnPowerUp state of the
/// element (see Section 3.1.3).
@property (nonatomic,assign) SigOnPowerUp state;

/// Creates the Generic On Power Up Status message.
/// @param state  The value of the Generic OnPowerUp state.
- (instancetype)initWithState:(SigOnPowerUp)state;

/**
 * @brief   Initialize SigGenericOnPowerUpSet object.
 * @param   parameters    the hex data of SigGenericOnPowerUpSet.
 * @return  return `nil` when initialize SigGenericOnPowerUpSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 3.2.4.3 Generic OnPowerUp Set Unacknowledged, opcode:0x8214


/// Generic OnPowerUp Set Unacknowledged is an unacknowledged message used to set
/// the Generic OnPowerUp state of an element (see Section 3.1.3).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.50),
/// 3.2.4.3 Generic OnPowerUp Set Unacknowledged.
@interface SigGenericOnPowerUpSetUnacknowledged : SigGenericMessage
/// The value of the Generic OnPowerUp state.
@property (nonatomic,assign) SigOnPowerUp state;

/// Creates the Generic On Power Up Status message.
/// @param state  The value of the Generic OnPowerUp state.
- (instancetype)initWithState:(SigOnPowerUp)state;

/**
 * @brief   Initialize SigGenericOnPowerUpSetUnacknowledged object.
 * @param   parameters    the hex data of SigGenericOnPowerUpSetUnacknowledged.
 * @return  return `nil` when initialize SigGenericOnPowerUpSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark - 3.2.5 Generic Power Level messages


#pragma mark 3.2.5.1 Generic Power Level Get, opcode:0x8215


/// Generic Power Level Get message is an acknowledged message used to get the Generic Power Actual state of an element (see Section 3.1.5.1).
/// @note   The response to the Generic Power Level Get message is a Generic Power Level Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.50),
/// 3.2.5.1 Generic Power Level Get.
@interface SigGenericPowerLevelGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigGenericPowerLevelGet object.
 * @param   parameters    the hex data of SigGenericPowerLevelGet.
 * @return  return `nil` when initialize SigGenericPowerLevelGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 3.2.5.2 Generic Power Level Set, opcode:0x8216


/// Generic Power Level Set is an acknowledged message used to set
/// the Generic Power Actual state of an element (see Section 3.1.5.1).
/// @note   The response to the Generic Power Level Set message
/// is a Generic Power Level Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.51),
/// 3.2.5.1 3.2.5.2 Generic Power Level Set.
@interface SigGenericPowerLevelSet : SigAcknowledgedGenericMessage
/// The target value of the Generic Power Actual state.
/// The Power field identifies the target Generic Power Actual state of
/// the element, as defined in Section 3.1.5.1.
@property (nonatomic,assign) UInt16 power;
/// The Transition Time field identifies the time that an element will take
/// to transition to the target state from the present state.
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Transition Time field identifies the time an element will
/// take to transition to the target state from the present state (see
/// Section 1.4.1.1). The format of the Transition Time field matches the
/// format of the Generic Default Transition Time state as defined in
/// Section 3.1.3. Only values of 0x00 through 0x3E shall be used to
/// specify the Transition Number of Steps.
@property (nonatomic,strong,nullable) SigTransitionTime *transitionTime;
/// Message execution delay in 5 milliseconds steps (C.1)
/// C.1: If the Transition Time field is present, the Delay field shall also be
/// present; otherwise these fields shall not be present.
/// The Delay field shall be present when the Transition Time field is present.
/// It identifies the message execution delay, representing a time interval
/// between receiving the message by a model and executing the associated
/// model behaviors.
@property (nonatomic,assign) UInt8 delay;

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

/**
 * @brief   Initialize SigGenericPowerLevelSet object.
 * @param   parameters    the hex data of SigGenericPowerLevelSet.
 * @return  return `nil` when initialize SigGenericPowerLevelSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 3.2.5.3 Generic Power Level Set Unacknowledged, opcode:0x8217


/// Generic Power Level Set Unacknowledged is an unacknowledged message used to
/// set the Generic Power Actual state of an element (see Section 3.1.5.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.51),
/// 3.2.5.3 Generic Power Level Set Unacknowledged.
@interface SigGenericPowerLevelSetUnacknowledged : SigGenericMessage
/// The target value of the Generic Power Actual state.
/// The Power field identifies the target Generic Power Actual state of
/// the element, as defined in Section 3.1.5.1.
@property (nonatomic,assign) UInt16 power;
/// The Transition Time field identifies the time that an element will take
/// to transition to the target state from the present state.
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Transition Time field identifies the time an element will
/// take to transition to the target state from the present state (see
/// Section 1.4.1.1). The format of the Transition Time field matches the
/// format of the Generic Default Transition Time state as defined in
/// Section 3.1.3. Only values of 0x00 through 0x3E shall be used to
/// specify the Transition Number of Steps.
@property (nonatomic,strong,nullable) SigTransitionTime *transitionTime;
/// Message execution delay in 5 milliseconds steps (C.1)
/// C.1: If the Transition Time field is present, the Delay field shall also be
/// present; otherwise these fields shall not be present.
/// The Delay field shall be present when the Transition Time field is present.
/// It identifies the message execution delay, representing a time interval
/// between receiving the message by a model and executing the associated
/// model behaviors.
@property (nonatomic,assign) UInt8 delay;

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

/**
 * @brief   Initialize SigGenericPowerLevelSetUnacknowledged object.
 * @param   parameters    the hex data of SigGenericPowerLevelSetUnacknowledged.
 * @return  return `nil` when initialize SigGenericPowerLevelSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 3.2.5.4 Generic Power Level Status, opcode:0x8218


/// Generic Power Level Status is an unacknowledged message used to report
/// the Generic Power Actual state of an element (see Section 3.1.5.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.52),
/// 3.2.5.4 Generic Power Level Status.
@interface SigGenericPowerLevelStatus : SigGenericMessage
/// The present value of the Generic Power Actual state.
/// The Present Power field identifies the Generic Power Actual state of the
/// element, as defined in Section 3.1.5.1.
@property (nonatomic,assign) UInt16 power;
/// The target value of the Generic Power Actual state (optional).
/// If present, the Target Power field identifies the target Generic Power Actual
/// state that the element is to reach (see Section 3.1.5.1).
@property (nonatomic,assign) UInt16 targetPower;
/// The Transition Time field identifies the time that an element will take to
/// transition to the target state from the present state.
/// Format as defined in Section 3.1.3 (C.1).
/// C.1: If the Target Power field is present, the Remaining Time field shall also
/// be present; otherwise these fields shall not be present.
/// If present, the Remaining Time field identifies the time it will take the element
/// to complete the transition to the target Generic Power Actual state of the
/// element (see Section 1.4.1.1 and 3.1.5.1).
@property (nonatomic,strong,nullable) SigTransitionTime *transitionTime;

/// Creates the Generic Power Level Status message.
///
/// @param power  The present value of the Generic Power Actual state.
- (instancetype)initWithPower:(UInt16)power;

/// Creates the Generic Power Level Status message.
///
/// @param power  The present value of the Generic Power Actual state.
/// @param targetPower The target value of the Generic Power Actual state.
/// @param transitionTime The time that an element will take to transition to the target state from the present state.
- (instancetype)initWithPower:(UInt16)power targetPower:(UInt16)targetPower transitionTime:(nullable SigTransitionTime *)transitionTime;

/**
 * @brief   Initialize SigGenericPowerLevelStatus object.
 * @param   parameters    the hex data of SigGenericPowerLevelStatus.
 * @return  return `nil` when initialize SigGenericPowerLevelStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 3.2.5.5 Generic Power Last Get, opcode:0x8219


/// Generic Power Last Get is an acknowledged message used to get
/// the Generic Power Last state of an element (see Section 3.1.5.1.1).
/// @note   The response to a Generic Power Last Get message is
/// a Generic Power Last Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.52),
/// 3.2.5.5 Generic Power Last Get.
@interface SigGenericPowerLastGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigGenericPowerLastGet object.
 * @param   parameters    the hex data of SigGenericPowerLastGet.
 * @return  return `nil` when initialize SigGenericPowerLastGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 3.2.5.6 Generic Power Last Status, opcode:0x821A


/// Generic Power Last Status is an unacknowledged message used to report
/// the Generic Power Last state of an element (see Section 3.1.5.1.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.52),
/// 3.2.5.6 Generic Power Last Status.
@interface SigGenericPowerLastStatus : SigGenericMessage
/// The value of the Generic Power Last state.
/// The Power field identifies the Generic Power Last state of the element,
/// as defined in Section 3.1.5.1.1.
@property (nonatomic,assign) UInt16 power;

/// Creates the Generic Power Last Status message.
/// @param power  The value of the Generic Power Last state.
- (instancetype)initWithPower:(UInt16)power;

/**
 * @brief   Initialize SigGenericPowerLastStatus object.
 * @param   parameters    the hex data of SigGenericPowerLastStatus.
 * @return  return `nil` when initialize SigGenericPowerLastStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 3.2.5.7 Generic Power Default Get, opcode:0x821B


/// Generic Power Default Get is an acknowledged message used to get
/// the Generic Power Default state of an element (see Section 3.1.5.3).
/// @note   The response to a Generic Power Default Get message is
/// a Generic Power Default Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.53),
/// 3.2.5.7 Generic Power Default Get.
@interface SigGenericPowerDefaultGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigGenericPowerDefaultGet object.
 * @param   parameters    the hex data of SigGenericPowerDefaultGet.
 * @return  return `nil` when initialize SigGenericPowerDefaultGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 3.2.5.10 Generic Power Default Status, opcode:0x821C


/// Generic Power Default Status is an unacknowledged message used to report
/// the Generic Power Default state of an element (see Section 3.1.5.3).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.53),
/// 3.2.5.10 Generic Power Default Status.
@interface SigGenericPowerDefaultStatus : SigGenericMessage
/// The value of the Generic Power Default state.
/// The Power field identifies the Generic Power Default state of the element,
/// as defined in Section 3.1.5.3.
@property (nonatomic,assign) UInt16 power;

/// Creates the Generic Power Default Status message.
/// @param power  The value of the Generic Power Default state.
- (instancetype)initWithPower:(UInt16)power;

/**
 * @brief   Initialize SigGenericPowerDefaultStatus object.
 * @param   parameters    the hex data of SigGenericPowerDefaultStatus.
 * @return  return `nil` when initialize SigGenericPowerDefaultStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 3.2.5.11 Generic Power Range Get, opcode:0x821D


/// Generic Power Range Get is an acknowledged message used to get
/// the Generic Power Range state of an element (see Section 3.1.5.4).
/// @note   The response to the Generic Power Range Get message is
/// a Generic Power Range Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.53),
/// 3.2.5.11 Generic Power Range Get.
@interface SigGenericPowerRangeGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigGenericPowerRangeGet object.
 * @param   parameters    the hex data of SigGenericPowerRangeGet.
 * @return  return `nil` when initialize SigGenericPowerRangeGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 3.2.5.14 Generic Power Range Status, opcode:0x821E


/// Generic Power Range Status is an unacknowledged message used to report
/// the Generic Power Range state of an element (see Section 3.1.5.4).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.54),
/// 3.2.5.14 Generic Power Range Status.
@interface SigGenericPowerRangeStatus : SigGenericMessage
/// Status Code for the requesting message.
/// The Status Code field identifies the Status Code for the last operation on the
/// Generic Power Range state. The allowed values for status codes and their
/// meanings are documented in Section 7.2.
@property (nonatomic,assign) SigGenericMessageStatus status;
/// The value of the Generic Power Range Min field of the Generic Power Range state.
/// The Range Min field identifies the Generic Power Range Min field of the
/// Generic Power Range state of the element (see Section 3.1.5.4).
@property (nonatomic,assign) UInt16 rangeMin;
/// The value of the Generic Power Range Max field of the Generic Power Range state.
/// The Range Max field identifies the Generic Power Range Max field of the
/// Generic Power Range state of the element (see Section 3.1.5.4).
@property (nonatomic,assign) UInt16 rangeMax;

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

/**
 * @brief   Initialize SigGenericPowerRangeStatus object.
 * @param   parameters    the hex data of SigGenericPowerRangeStatus.
 * @return  return `nil` when initialize SigGenericPowerRangeStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 3.2.5.8 Generic Power Default Set, opcode:0x821F


/// Generic Power Default Set is an acknowledged message used to set
/// the Generic Power Default state of an element (see Section 3.1.5.3).
/// @note   The response to the Generic Power Default Set message is
/// a Generic Power Default Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.53),
/// 3.2.5.8 Generic Power Default Set.
@interface SigGenericPowerDefaultSet : SigAcknowledgedGenericMessage
/// The value of the Generic Power Default state.
/// The Power field identifies the Generic Power Default state of the element,
/// as defined in Section 3.1.5.3.
@property (nonatomic,assign) UInt16 power;

/// Creates the Generic Power Default Set message.
/// @param power  The value of the Generic Power Default state.
- (instancetype)initWithPower:(UInt16)power;

/**
 * @brief   Initialize SigGenericPowerDefaultSet object.
 * @param   parameters    the hex data of SigGenericPowerDefaultSet.
 * @return  return `nil` when initialize SigGenericPowerDefaultSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 3.2.5.9 Generic Power Default Set Unacknowledged, opcode:0x8220


/// Generic Power Default Set Unacknowledged is an unacknowledged message used to set
/// the Generic Power Default state of an element (see Section 3.1.5.3).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.53),
/// 3.2.5.9 Generic Power Default Set Unacknowledged.
@interface SigGenericPowerDefaultSetUnacknowledged : SigGenericMessage
/// The value of the Generic Power Default state.
/// The Power field identifies the Generic Power Default state of the element,
/// as defined in Section 3.1.5.3.
@property (nonatomic,assign) UInt16 power;

/// Creates the Generic Power Default Set message.
/// @param power  The value of the Generic Power Default state.
- (instancetype)initWithPower:(UInt16)power;

/**
 * @brief   Initialize SigGenericPowerDefaultSetUnacknowledged object.
 * @param   parameters    the hex data of SigGenericPowerDefaultSetUnacknowledged.
 * @return  return `nil` when initialize SigGenericPowerDefaultSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 3.2.5.12 Generic Power Range Set, opcode:0x8221


/// Generic Power Range Set is an acknowledged message used to set
/// the Generic Power Range state of an element (see Section 3.1.5.4).
/// @note   The response to the Generic Power Range Set message is
/// a Generic Power Range Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.54),
/// 3.2.5.12 Generic Power Range Set.
@interface SigGenericPowerRangeSet : SigAcknowledgedGenericMessage
/// The value of the Generic Power Range Min field of the Generic Power
/// Range state.
/// The Range Min field identifies the Generic Power Range Min field of the
/// Generic Power Range state of the element (see Section 3.1.5.4).
@property (nonatomic,assign) UInt16 rangeMin;
/// The value of the Generic Power Range Max field of the Generic Power
/// Range state.
/// The Range Max field identifies the Generic Power Max field of the Generic
/// Power Range state of the element (see Section 3.1.5.4).
@property (nonatomic,assign) UInt16 rangeMax;

/// Creates the Generic Power Range Set message.
/// @param rangeMin  The value of the Generic Power Range Min field of the Generic Power Range state.
/// @param rangeMax  The value of the Generic Power Range Max field of the Generic Power Range state.
- (instancetype)initWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax;

/**
 * @brief   Initialize SigGenericPowerRangeSet object.
 * @param   parameters    the hex data of SigGenericPowerRangeSet.
 * @return  return `nil` when initialize SigGenericPowerRangeSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 3.2.5.13 Generic Power Range Set Unacknowledged, opcode:0x8222


/// Generic Power Range Set Unacknowledged is an unacknowledged message used to set
/// the Generic Power Range state of an element (see Section 3.1.5.4).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.54),
/// 3.2.5.13 Generic Power Range Set Unacknowledged.
@interface SigGenericPowerRangeSetUnacknowledged : SigGenericMessage
/// The value of the Generic Power Range Min field of the Generic Power
/// Range state.
/// The Range Min field identifies the Generic Power Range Min field of the
/// Generic Power Range state of the element (see Section 3.1.5.4).
@property (nonatomic,assign) UInt16 rangeMin;
/// The value of the Generic Power Range Max field of the Generic Power
/// Range state.
/// The Range Max field identifies the Generic Power Max field of the Generic
/// Power Range state of the element (see Section 3.1.5.4).
@property (nonatomic,assign) UInt16 rangeMax;

/// Creates the Generic Power Range Set message.
/// @param rangeMin  The value of the Generic Power Range Min field of the Generic Power Range state.
/// @param rangeMax  The value of the Generic Power Range Max field of the Generic Power Range state.
- (instancetype)initWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax;

/**
 * @brief   Initialize SigGenericPowerRangeSetUnacknowledged object.
 * @param   parameters    the hex data of SigGenericPowerRangeSetUnacknowledged.
 * @return  return `nil` when initialize SigGenericPowerRangeSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark - 3.2.6 Generic Battery messages


#pragma mark 3.2.6.1 Generic Battery Get, opcode:0x8223


/// Generic Battery Get message is an acknowledged message used to get
/// the Generic Battery state of an element (see Section 3.1.6).
/// @note   The response to the Generic Battery Get message is a Generic
/// Battery Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.55),
/// 3.2.6.1 Generic Battery Get.
@interface SigGenericBatteryGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigGenericBatteryGet object.
 * @param   parameters    the hex data of SigGenericBatteryGet.
 * @return  return `nil` when initialize SigGenericBatteryGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 3.2.6.2 Generic Battery Status, opcode:0x8224


/// Generic Battery Status is an unacknowledged message used to report
/// the Generic Battery state of an element (see Section 3.1.6).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.55),
/// 3.2.6.2 Generic Battery Status.
@interface SigGenericBatteryStatus : SigGenericMessage
/// Battery level state in percentage. Only values 0...100 and 0xFF are allowed.
///
/// Vaklue 0xFF means that the battery state is unknown.
/// The Battery Level field identifies the Generic Battery Level state of the element,
/// as defined in Section 3.1.6.1.
@property (nonatomic,assign) UInt8 batteryLevel;
/// Time to discharge, in minutes. Value 0xFFFFFF means unknown time.
/// The Time to Discharge field identifies the Generic Battery Time to Discharge
/// state of the element, as defined in Section 3.1.6.2.
@property (nonatomic,assign) UInt32 timeToDischarge;
/// Time to charge, in minutes. Value 0xFFFFFF means unknown time.
/// The Time to Charge field identifies the Generic Battery Time to Charge state of
/// the element, as defined in Section 3.1.6.3.
@property (nonatomic,assign) UInt32 timeToCharge;
/// The value of the Generic Battery Flags state.
/// The Flags field identifies the Generic Battery Flags state of the element,
/// as defined in Section 3.1.6.4.
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

/**
 * @brief   Initialize SigGenericBatteryStatus object.
 * @param   parameters    the hex data of SigGenericBatteryStatus.
 * @return  return `nil` when initialize SigGenericBatteryStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark - 4.2 Sensor messages


#pragma mark 4.2.1 Sensor Descriptor Get, opcode:0x8230


/// Sensor Descriptor Get is an acknowledged message used to get the Sensor Descriptor state of all sensors within an element (see Section 4.1.1).
/// @note   The response to a Sensor Descriptor Get message is a Sensor Descriptor Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.114),
/// 4.2.1 Sensor Descriptor Get.
@interface SigSensorDescriptorGet : SigAcknowledgedGenericMessage
/// Property ID for the sensor (Optional)
/// If present, the Property ID field identifies a Sensor Property ID state of an element.(0x0001–0xFFFF)
@property (nonatomic,assign) UInt16 propertyID;

- (instancetype)initWithPropertyID:(UInt16)propertyID;

/**
 * @brief   Initialize SigSensorDescriptorGet object.
 * @param   parameters    the hex data of SigSensorDescriptorGet.
 * @return  return `nil` when initialize SigSensorDescriptorGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.2.2 Sensor Descriptor Status, opcode:0x51


/// The Sensor Descriptor Status is an unacknowledged message used to report
/// a sequence of the Sensor Descriptor states of an element (see Section 4.1.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.114),
/// 4.2.2 Sensor Descriptor Status.
@interface SigSensorDescriptorStatus : SigGenericMessage
/// The Descriptor field shall contain a sequence of 0 or more Sensor Descriptor states.(8*N or 2 bytes)
/// Sequence of 8-octet Sensor Descriptors (Optional)
/// The message uses a single-octet Opcode to maximize the payload size.
/// The Descriptor field shall contain a sequence of 0 or more Sensor Descriptor
/// states as defined in Section 4.1.1.
/// When the message is a response to a Sensor Descriptor Get message that
/// identifies a sensor descriptor property that does not exist on the element,
/// the Descriptor field shall contain the requested Property ID value and the
/// other fields of the Sensor Descriptor state shall be omitted.
@property (nonatomic,strong) NSMutableArray <SigSensorDescriptorModel *>*descriptorModels;

/**
 * @brief   Initialize SigSensorDescriptorStatus object.
 * @param   parameters    the hex data of SigSensorDescriptorStatus.
 * @return  return `nil` when initialize SigSensorDescriptorStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.2.13 Sensor Get, opcode:0x8231


/// Sensor Get is an acknowledged message used to get the Sensor Data state (see Section 4.1.4).
/// @note   The response to the Sensor Get message is a Sensor Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.120),
/// 4.2.13 Sensor Get.
@interface SigSensorGet : SigAcknowledgedGenericMessage
/// Property for the sensor. (Optional)
/// If present, the Property ID field identifies a Sensor Property ID state of an element.(0x0001–0xFFFF)
@property (nonatomic,assign) UInt16 propertyID;

- (instancetype)initWithPropertyID:(UInt16)propertyID;

/**
 * @brief   Initialize SigSensorGet object.
 * @param   parameters    the hex data of SigSensorGet.
 * @return  return `nil` when initialize SigSensorGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.2.14 Sensor Status, opcode:0x52


/// Sensor Status is an unacknowledged message used to report
/// the Sensor Data state of an element (see Section 4.1.4).
/// @note   The message contains a Sensor Data state, defined
/// by the Sensor Descriptor state (see Section 4.1.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.120),
/// 4.2.14 Sensor Status.
@interface SigSensorStatus : SigGenericMessage
@property (nonatomic,strong) NSData *marshalledSensorData;

/**
 * @brief   Initialize SigSensorStatus object.
 * @param   parameters    the hex data of SigSensorStatus.
 * @return  return `nil` when initialize SigSensorStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.2.15 Sensor Column Get, opcode:0x8232


/// Sensor Column Get is an acknowledged message used to get
/// the Sensor Series Column state (see Section 4.1.5).
/// @note   The response to the Sensor Column Get message is
/// a Sensor Column Status message (see Section 4.2.16).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.122),
/// 4.2.15 Sensor Column Get.
@interface SigSensorColumnGet : SigAcknowledgedGenericMessage
/// Property identifying a sensor.
/// The Property ID field identifies a sensor within an element (see Section 4.1.5.1).
@property (nonatomic,assign) UInt16 propertyID;
/// Raw value identifying a column.
/// The Raw Value X field identifies a column of a sensor’s series within an element (see Section 4.1.5.2).
@property (nonatomic,strong) NSData *rawValueX;

- (instancetype)initWithPropertyID:(UInt16)propertyID rawValueX:(NSData *)rawValueX;

/**
 * @brief   Initialize SigSensorColumnGet object.
 * @param   parameters    the hex data of SigSensorColumnGet.
 * @return  return `nil` when initialize SigSensorColumnGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.2.16 SensorColumnStatus, opcode:0x53


/// Sensor Column Status is an unacknowledged message used to report
/// the Sensor Series Column state of an element (see Section 4.1.5).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.122),
/// 4.2.16 SensorColumnStatus.
@interface SigSensorColumnStatus : SigGenericMessage
/*
 The structure of the message is defined in the following table.
 Field          Size (octets)   Notes
 Property ID    2               Property identifying a sensor and the Y axis.
 Raw Value X    variable        Raw value representing the left corner of the column on the X axis.
 Column Width   variable        Raw value representing the width of the column. (Optional)
 Raw Value Y    variable        Raw value representing the height of the column on the Y axis. (C.1)

 C.1: If the Column Width field is present, the Raw Value Y field shall also be present; otherwise this field shall not be present.

 */
@property (nonatomic,strong) NSData *columnData;

/**
 * @brief   Initialize SigSensorColumnStatus object.
 * @param   parameters    the hex data of SigSensorColumnStatus.
 * @return  return `nil` when initialize SigSensorColumnStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.2.17 Sensor Series Get, opcode:0x8233


/// Sensor Series Get is an acknowledged message used to get
/// a sequence of the Sensor Series Column states (see Section 4.1.5).
/// @note   The response to the Sensor Series Get message is
/// a Sensor Series Status message (see Section 4.2.18).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.123),
/// 4.2.17 Sensor Series Get.
@interface SigSensorSeriesGet : SigAcknowledgedGenericMessage
@property (nonatomic,strong) NSData *seriesData;
/// If present, the Property ID field identifies a Sensor Property ID state of an element.(0x0001–0xFFFF)
/// Property identifying a sensor.
/// The Property ID field identifies a sensor within an element (see Section 4.1.5.1).
@property (nonatomic,assign) UInt16 propertyID;
/// Raw value identifying a starting column. (Optional)
/// The Raw Value X1 field identifies a starting column of a sensor’s series
/// within an element (see Section 4.1.5.2).
@property (nonatomic,strong) NSData *rawValueX1Data;
/// Raw value identifying an ending column. (C.1)
/// C.1: If the Raw Value X1 field is present, the Raw Value X2 field shall also be present;
/// otherwise this field shall not be present.
/// The Raw Value X2 field identifies an ending column of a sensor’s series
/// within an element (see Section 4.1.5.2).
@property (nonatomic,strong) NSData *rawValueX2Data;

- (instancetype)initWithPropertyID:(UInt16)propertyID rawValueX1Data:(NSData *)rawValueX1Data rawValueX2Data:(NSData *)rawValueX2Data;

/**
 * @brief   Initialize SigSensorSeriesGet object.
 * @param   parameters    the hex data of SigSensorSeriesGet.
 * @return  return `nil` when initialize SigSensorSeriesGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.2.18 Sensor Series Status, opcode:0x54


/// Sensor Series Status is an unacknowledged message used to report a
/// sequence of the Sensor Series Column states of an element (see Section 4.1.5).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.123),
/// 4.2.18 Sensor Series Status.
@interface SigSensorSeriesStatus : SigGenericMessage
/*
 Field              Size (octets)           Notes
 Property ID        2                       Property identifying a sensor and the Y axis.
 Raw Value X [n]    variable                Raw value representing the left corner of the nth column on the X axis.
 Column Width [n]   variable                Raw value representing the width of the nth column.
 Raw Value Y [n]    variable                Raw value representing the height of the nth column on the Y axis.
 */
/// The structure of the message is defined in the following table. The Raw Value X [n],
/// Column Width [n], and Raw Value Y [n] fields are a triplet that may be repeated
/// multiple times within the message. The Raw Value X [n] field is followed by the
/// Column Width [n] field, which is followed by the Raw Value Y [n], which is followed
/// by the Raw Value X [n+1], and so forth.
/// The message shall be sent as a response to the Sensor Series Get message (see
/// Section 4.2.17) or as an unsolicited message.
/// The Property ID field shall contain the Sensor Property ID state (see Section 4.1.5.1).
/// The Raw Value X [n] field shall contain the nth Sensor Raw Value X state (see
/// Section 4.1.5.2). The Column Width [n] field shall contain the nth Sensor Column
/// Width state (see Section 4.1.5.3). The Raw Value Y [n] field shall contain the nth
/// Sensor Raw Value Y state (see Section 4.1.5.4).
@property (nonatomic,strong) NSData *seriesData;

/**
 * @brief   Initialize SigSensorSeriesStatus object.
 * @param   parameters    the hex data of SigSensorSeriesStatus.
 * @return  return `nil` when initialize SigSensorSeriesStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.2.3 Sensor Cadence Get, opcode:0x8234


/// Sensor Cadence Get is an acknowledged message used to get
/// the Sensor Cadence state of an element (see Section 4.1.3).
/// @note   The response to the Sensor Cadence Get message is
/// a Sensor Cadence Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.115),
/// 4.2.3 Sensor Cadence Get.
@interface SigSensorCadenceGet : SigAcknowledgedGenericMessage
/// If present, the Property ID field identifies a Sensor Property ID state of an element.(0x0001–0xFFFF)
/// Property ID for the sensor.
/// The Property ID field identifies a Sensor Property ID state of an element (see Section 4.1.1.1).
@property (nonatomic,assign) UInt16 propertyID;

- (instancetype)initWithPropertyID:(UInt16)propertyID;

/**
 * @brief   Initialize SigSensorCadenceGet object.
 * @param   parameters    the hex data of SigSensorCadenceGet.
 * @return  return `nil` when initialize SigSensorCadenceGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.2.4 Sensor Cadence Set, opcode:0x55


/// Sensor Cadence Set is an acknowledged message used to set
/// the Sensor Cadence state of an element (see Section 4.1.3).
/// @note   The response to the Sensor Cadence Set message is
/// a Sensor Cadence Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.115),
/// 4.2.4 Sensor Cadence Set.
@interface SigSensorCadenceSet : SigAcknowledgedGenericMessage
/// If present, the Property ID field identifies a Sensor Property ID state of an element.(0x0001–0xFFFF)
/// Property ID for the sensor.
/// The Property ID field identifies a Sensor Property ID state of an element (see Section 4.1.1.1).
@property (nonatomic,assign) UInt16 propertyID;
/*
 Field                          Size (bits)     Notes
 Property ID                    16              Property ID for the sensor.
 Fast Cadence Period Divisor    7               Divisor for the Publish Period (see Mesh Profile specification[2]).
 Status Trigger Type            1               Defines the unit and format of the Status Trigger Delta fields.
 Status Trigger Delta Down      variable        Delta down value that triggers a status message.
 Status Trigger Delta Up        variable        Delta up value that triggers a status message.
 Status Min Interval            8               Minimum interval between two consecutive Status messages.
 Fast Cadence Low               variable        Low value for the fast cadence range.
 Fast Cadence High              variable        High value for the fast cadence range.
 */
/// The Fast Cadence Period Divisor field identifies a Fast Cadence Period
/// Divisor state of an element (see Section 4.1.3.1).
/// The Status Trigger Type field identifies a Status Trigger Type state of an
/// element (see Section 4.1.3.2).
/// The Status Trigger Delta Down field identifies a Status Trigger Delta Down
/// state of an element (see Section 4.1.3.3).
/// The Status Trigger Delta Up field identifies a Status Trigger Delta Up
/// state of an element (see Section 4.1.3.4).
/// The Status Min Interval field identifies a Status Min Interval state of an
/// element (see Section 4.1.3.5).
/// The Fast Cadence Low field identifies a Fast Cadence Low state of an
/// element (see Section 4.1.3.6).
/// The Fast Cadence High field identifies a Fast Cadence High state of an
/// element (see Section 4.1.3.7).
@property (nonatomic,strong) NSData *cadenceData;

/**
 * @brief   Initialize SigSensorCadenceSet object.
 * @param   parameters    the hex data of SigSensorCadenceSet.
 * @return  return `nil` when initialize SigSensorCadenceSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.2.5 Sensor Cadence Set Unacknowledged, opcode:0x56


/// Sensor Cadence Set is an acknowledged message used to set
/// the Sensor Cadence state of an element (see Section 4.1.3).
/// @note   The response to the Sensor Cadence Set message is
/// a Sensor Cadence Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.116),
/// 4.2.5 Sensor Cadence Set Unacknowledged.
@interface SigSensorCadenceSetUnacknowledged : SigGenericMessage
/// If present, the Property ID field identifies a Sensor Property ID state of an element.(0x0001–0xFFFF)
/// Property ID for the sensor.
/// The Property ID field identifies a Sensor Property ID state of an element (see Section 4.1.1.1).
@property (nonatomic,assign) UInt16 propertyID;
/*
 Field                          Size (bits)     Notes
 Property ID                    16              Property ID for the sensor.
 Fast Cadence Period Divisor    7               Divisor for the Publish Period (see Mesh Profile specification[2]).
 Status Trigger Type            1               Defines the unit and format of the Status Trigger Delta fields.
 Status Trigger Delta Down      variable        Delta down value that triggers a status message.
 Status Trigger Delta Up        variable        Delta up value that triggers a status message.
 Status Min Interval            8               Minimum interval between two consecutive Status messages.
 Fast Cadence Low               variable        Low value for the fast cadence range.
 Fast Cadence High              variable        High value for the fast cadence range.
 */
/// The Fast Cadence Period Divisor field identifies a Fast Cadence Period
/// Divisor state of an element (see Section 4.1.3.1).
/// The Status Trigger Type field identifies a Status Trigger Type state of an
/// element (see Section 4.1.3.2).
/// The Status Trigger Delta Down field identifies a Status Trigger Delta Down
/// state of an element (see Section 4.1.3.3).
/// The Status Trigger Delta Up field identifies a Status Trigger Delta Up
/// state of an element (see Section 4.1.3.4).
/// The Status Min Interval field identifies a Status Min Interval state of an
/// element (see Section 4.1.3.5).
/// The Fast Cadence Low field identifies a Fast Cadence Low state of an
/// element (see Section 4.1.3.6).
/// The Fast Cadence High field identifies a Fast Cadence High state of an
/// element (see Section 4.1.3.7).
@property (nonatomic,strong) NSData *cadenceData;

/**
 * @brief   Initialize SigSensorCadenceSetUnacknowledged object.
 * @param   parameters    the hex data of SigSensorCadenceSetUnacknowledged.
 * @return  return `nil` when initialize SigSensorCadenceSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.2.6 Sensor Cadence Status, opcode:0x57


/// The Sensor Cadence Status is an unacknowledged message used to report
/// the Sensor Cadence state of an element (see Section 4.1.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.117),
/// 4.2.6 Sensor Cadence Status.
@interface SigSensorCadenceStatus : SigGenericMessage
/// If present, the Property ID field identifies a Sensor Property ID state of an element.(0x0001–0xFFFF)
/// Property ID for the sensor.
/// The Property ID field identifies a Sensor Property ID state of an element (see Section 4.1.1.1).
@property (nonatomic,assign) UInt16 propertyID;
/*
 Field                          Size (bits)     Notes
 Property ID                    16              Property ID for the sensor.
 Fast Cadence Period Divisor    7               Divisor for the Publish Period (see Mesh Profile specification[2]).(Optional)
 Status Trigger Type            1               Defines the unit and format of the Status Trigger Delta fields.(C.1)
 Status Trigger Delta Down      variable        Delta down value that triggers a status message.(C.1)
 Status Trigger Delta Up        variable        Delta up value that triggers a status message.(C.1)
 Status Min Interval            8               Minimum interval between two consecutive Status messages.(C.1)
 Fast Cadence Low               variable        Low value for the fast cadence range.(C.1)
 Fast Cadence High              variable        High value for the fast cadence range.(C.1)
 */
/// C.1: If the Fast Cadence Period Divisor field is present, the Status
/// Trigger Type, Status Trigger Delta Down, Status Trigger Delta Up,
/// Status Min Interval, Fast Cadence Low, and Fast Cadence High
/// fields shall also be present; otherwise these fields shall not be present.
/// The Fast Cadence Period Divisor field identifies a Fast Cadence Period
/// Divisor state of an element (see Section 4.1.3.1).
/// The Status Trigger Type field identifies a Status Trigger Type state of an
/// element (see Section 4.1.3.2).
/// The Status Trigger Delta Down field identifies a Status Trigger Delta Down
/// state of an element (see Section 4.1.3.3).
/// The Status Trigger Delta Up field identifies a Status Trigger Delta Up
/// state of an element (see Section 4.1.3.4).
/// The Status Min Interval field identifies a Status Min Interval state of an
/// element (see Section 4.1.3.5).
/// The Fast Cadence Low field identifies a Fast Cadence Low state of an
/// element (see Section 4.1.3.6).
/// The Fast Cadence High field identifies a Fast Cadence High state of an
/// element (see Section 4.1.3.7).
@property (nonatomic,strong) NSData *cadenceData;

/**
 * @brief   Initialize SigSensorCadenceStatus object.
 * @param   parameters    the hex data of SigSensorCadenceStatus.
 * @return  return `nil` when initialize SigSensorCadenceStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.2.7 Sensor Settings Get, opcode:0x8235


/// Sensor Settings Get is an acknowledged message used to get
/// the list of Sensor Setting states of an element (see Section 4.1.2).
/// @note   The response to the Sensor Settings Get message is a
/// Sensor Settings Status message (see Section 4.2.8).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.117),
/// 4.2.7 Sensor Settings Get.
@interface SigSensorSettingsGet : SigAcknowledgedGenericMessage
/// If present, the Property ID field identifies a Sensor Property ID state of an element.(0x0001–0xFFFF)
/// Property ID identifying a sensor.
/// The Sensor Property ID field identifies a Sensor Property ID state of an element (see Section 4.1.1.1).
@property (nonatomic,assign) UInt16 propertyID;

- (instancetype)initWithPropertyID:(UInt16)propertyID;

/**
 * @brief   Initialize SigSensorSettingsGet object.
 * @param   parameters    the hex data of SigSensorSettingsGet.
 * @return  return `nil` when initialize SigSensorSettingsGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.2.8 Sensor Settings Status, opcode:0x58


/// The Sensor Settings Status is an unacknowledged message used to report
/// a list of the Sensor Setting states of an element (see Section 4.1.2).
/// @note   The message is sent as a response to the Sensor Settings Get
/// message or is sent as an unsolicited message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.118),
/// 4.2.8 Sensor Settings Status.
@interface SigSensorSettingsStatus : SigGenericMessage
/// If present, the Property ID field identifies a Sensor Property ID state of an element.(0x0001–0xFFFF)
/// Property ID identifying a sensor.
/// The Sensor Property ID field identifies a Sensor Property ID state of an element (see Section 4.1.1.1).
@property (nonatomic,assign) UInt16 propertyID;
/// Sensor Setting Property IDs
/// A sequence of N Sensor Setting Property IDs identifying Sensor Setting
/// Property IDs 2*N settings within a sensor, where N is the number of
/// property IDs included in the message. (Optional)
/// The Sensor Setting Property IDs field contains a sequence of all Sensor
/// Setting Property ID states of a sensor (see Section 4.1.2).
@property (nonatomic,strong) NSData *settingsData;

/**
 * @brief   Initialize SigSensorSettingsStatus object.
 * @param   parameters    the hex data of SigSensorSettingsStatus.
 * @return  return `nil` when initialize SigSensorSettingsStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.2.9 Sensor Setting Get, opcode:0x8236


/// Sensor Setting Get is an acknowledged message used to get
/// the Sensor Setting state of an element (see Section 4.1.2).
/// @note   The response to the Sensor Setting Get message is
/// a Sensor Setting Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.118),
/// 4.2.9 Sensor Setting Get.
@interface SigSensorSettingGet : SigAcknowledgedGenericMessage
/// Property ID identifying a sensor..(0x0001–0xFFFF)
/// The Sensor Property ID field identifies a Sensor Property ID state
/// of an element (see Section 4.1.1.1).
@property (nonatomic,assign) UInt16 propertyID;
/// Setting Property ID identifying a setting within a sensor..(0x0001–0xFFFF)
/// The Sensor Setting Property ID field identifies a Sensor Setting
/// Property ID state of a sensor (see Section 4.1.2).
@property (nonatomic,assign) UInt16 settingPropertyID;

- (instancetype)initWithPropertyID:(UInt16)propertyID settingPropertyID:(UInt16)settingPropertyID;

/**
 * @brief   Initialize SigSensorSettingGet object.
 * @param   parameters    the hex data of SigSensorSettingGet.
 * @return  return `nil` when initialize SigSensorSettingGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.2.10 Sensor Setting Set, opcode:0x59


/// Sensor Setting Set is an acknowledged message used to set
/// the Sensor Setting state of an element (see Section 4.1.2).
/// @note   The response to the Sensor Setting Set message is
/// a Sensor Setting Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.118),
/// 4.2.10 Sensor Setting Set.
@interface SigSensorSettingSet : SigAcknowledgedGenericMessage
/// Property ID identifying a sensor..(0x0001–0xFFFF)
/// The Sensor Property ID field identifies a Sensor Property ID state
/// of an element (see Section 4.1.2.1).
@property (nonatomic,assign) UInt16 propertyID;
/// Setting Property ID identifying a setting within a sensor..(0x0001–0xFFFF)
/// The Sensor Setting Property ID field identifies a Sensor Setting
/// Property ID state of a sensor (see Section 4.1.2.2).
@property (nonatomic,assign) UInt16 settingPropertyID;
/// Raw value for the setting.
/// The Sensor Setting Raw field identifies a Sensor Setting Raw
/// state of a sensor (see Section 4.1.2.4).
@property (nonatomic,strong) NSData *settingRaw;

- (instancetype)initWithPropertyID:(UInt16)propertyID settingPropertyID:(UInt16)settingPropertyID settingRaw:(NSData *)settingRaw;

/**
 * @brief   Initialize SigSensorSettingSet object.
 * @param   parameters    the hex data of SigSensorSettingSet.
 * @return  return `nil` when initialize SigSensorSettingSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 4.2.11 Sensor Setting Set Unacknowledged, opcode:0x5A


/// Sensor Setting Set Unacknowledged is an unacknowledged message used to set
/// the Sensor Setting state of an element (see Section 4.1.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.119),
/// 4.2.11 Sensor Setting Set Unacknowledged.
@interface SigSensorSettingSetUnacknowledged : SigGenericMessage
/// Property ID identifying a sensor..(0x0001–0xFFFF)
/// The Sensor Property ID field identifies a Sensor Property ID state
/// of an element (see Section 4.1.2.1).
@property (nonatomic,assign) UInt16 propertyID;
/// Setting Property ID identifying a setting within a sensor..(0x0001–0xFFFF)
/// The Sensor Setting Property ID field identifies a Sensor Setting
/// Property ID state of a sensor (see Section 4.1.2.2).
@property (nonatomic,assign) UInt16 settingPropertyID;
/// Raw value for the setting.
/// The Sensor Setting Raw field identifies a Sensor Setting Raw
/// state of a sensor (see Section 4.1.2.4).
@property (nonatomic,strong) NSData *settingRaw;

- (instancetype)initWithPropertyID:(UInt16)propertyID settingPropertyID:(UInt16)settingPropertyID settingRaw:(NSData *)settingRaw;

/**
 * @brief   Initialize SigSensorSettingSetUnacknowledged object.
 * @param   parameters    the hex data of SigSensorSettingSetUnacknowledged.
 * @return  return `nil` when initialize SigSensorSettingSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.2.12 Sensor Setting Status, opcode:0x5B


/// Sensor Setting Status is an unacknowledged message used to report
/// the Sensor Setting state of an element (see Section 4.1.2).
/// @note   The message is sent as a response to the Sensor Setting
/// Get and Sensor Setting Set messages or sent as an unsolicited message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.119),
/// 4.2.12 Sensor Setting Status.
@interface SigSensorSettingStatus : SigGenericMessage
/// Property ID identifying a sensor..(0x0001–0xFFFF)
/// The Sensor Property ID field identifies a Sensor Property ID state of an
/// element (see Section 4.1.2.1).
@property (nonatomic,assign) UInt16 propertyID;
/// Setting Property ID identifying a setting within a sensor..(0x0001–0xFFFF)
/// The Sensor Setting Property ID field identifies a Sensor Setting Property ID
/// state of a sensor (see Section 4.1.2.2).
@property (nonatomic,assign) UInt16 settingPropertyID;
/// Read / Write access rights for the setting. (Optional)
/// The Sensor Setting Access field identifies a Sensor Setting Access state
/// of a sensor (see Section 4.1.2.3).
@property (nonatomic,assign) SigSensorSettingAccessType settingAccess;
/// Raw value for the setting. (C.1)
/// C.1: If the Sensor Setting Access field is present, the Sensor Setting Raw
/// field shall also be present; otherwise this field shall not be present.
/// The Sensor Setting Raw field identifies a Sensor Setting Raw state of a
/// sensor (see Section 4.1.2.4).
@property (nonatomic,strong) NSData *settingRaw;

- (instancetype)initWithPropertyID:(UInt16)propertyID settingPropertyID:(UInt16)settingPropertyID settingAccess:(SigSensorSettingAccessType)settingAccess settingRaw:(NSData *)settingRaw;

/**
 * @brief   Initialize SigSensorSettingStatus object.
 * @param   parameters    the hex data of SigSensorSettingStatus.
 * @return  return `nil` when initialize SigSensorSettingStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark - 5.2.1 Time messages


#pragma mark 5.2.1.1 Time Get, opcode:0x8237


/// Time Get is a message used to get the Time
/// state (see Section 5.1.1) of neighbor nodes.
/// @note   The response to the Time Get
/// message is a Time Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.142),
/// 5.2.1.1 Time Get.
@interface SigTimeGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigTimeGet object.
 * @param   parameters    the hex data of SigTimeGet.
 * @return  return `nil` when initialize SigTimeGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 5.2.1.2 Time Set, opcode:0x5C


/// Time Set is an acknowledged message used to set
/// the Time state of an element (see Section 5.1.1).
/// @note   The response to the Time Set message
/// is a Time Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.143),
/// 5.2.1.2 Time Set.
@interface SigTimeSet : SigAcknowledgedGenericMessage
@property (nonatomic,strong) SigTimeModel *timeModel;

- (instancetype)initWithTimeModel:(SigTimeModel *)timeModel;

/**
 * @brief   Initialize SigTimeSet object.
 * @param   parameters    the hex data of SigTimeSet.
 * @return  return `nil` when initialize SigTimeSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 5.2.1.3 Time Status, opcode:0x5D


/// Time Status is an unacknowledged message used to report
/// the Time state of an element (see Section 5.1.1).
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.143),
/// 5.2.1.3 Time Status.
@interface SigTimeStatus : SigGenericMessage
@property (nonatomic,strong) SigTimeModel *timeModel;

/**
 * @brief   Initialize SigTimeStatus object.
 * @param   parameters    the hex data of SigTimeStatus.
 * @return  return `nil` when initialize SigTimeStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 5.2.1.10 Time Role Get, opcode:0x8238


/// Time Status is an unacknowledged message used to report
/// the Time state of an element (see Section 5.1.1).
/// @note   The response to the Time Role Get message is
/// a Time Role Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.145),
/// 5.2.1.10 Time Role Get.
@interface SigTimeRoleGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigTimeRoleGet object.
 * @param   parameters    the hex data of SigTimeRoleGet.
 * @return  return `nil` when initialize SigTimeRoleGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 5.2.1.11 Time Role Set, opcode:0x8239


/// Time Role Set is an acknowledged message used to set
/// the Time Role state of an element (see Section 5.1.2).
/// @note   The response to the Time Role Set message
/// is a Time Role Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.146),
/// 5.2.1.11 Time Role Set.
@interface SigTimeRoleSet : SigAcknowledgedGenericMessage
/// Time Role is an enumeration state that defines the role of a node in propagation of time information in a mesh network.
/// The Time Role field identifies the Time Role state (see Section 5.1.2).
@property (nonatomic,assign) SigTimeRole timeRole;

- (instancetype)initWithTimeRole:(SigTimeRole)timeRole;

/**
 * @brief   Initialize SigTimeRoleSet object.
 * @param   parameters    the hex data of SigTimeRoleSet.
 * @return  return `nil` when initialize SigTimeRoleSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 5.2.1.12 Time Role Status, opcode:0x823A


/// Time Role Status is an unacknowledged message used to report
/// the Time state of an element (see Section 5.1.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.146),
/// 5.2.1.12 Time Role Status.
@interface SigTimeRoleStatus : SigGenericMessage
/// Time Role is an enumeration state that defines the role of a node in propagation of time information in a mesh network.
/// The Time Role field identifies the Time Role state (see Section 5.1.2).
@property (nonatomic,assign) SigTimeRole timeRole;

/**
 * @brief   Initialize SigTimeRoleStatus object.
 * @param   parameters    the hex data of SigTimeRoleStatus.
 * @return  return `nil` when initialize SigTimeRoleStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 5.2.1.4 Time Zone Get, opcode:0x823B


/// Time Zone Get is an acknowledged message used to get
/// the Time Zone Offset Current state (see Section 5.1.1.5),
/// the Time Zone Offset New state (see Section 5.1.1.6), and
/// the TAI of Zone Change state (see Section 5.1.1.7).
/// @note   The response to the Time Zone Get message is
/// a Time Zone Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.144),
/// 5.2.1.4 Time Zone Get.
@interface SigTimeZoneGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigTimeZoneGet object.
 * @param   parameters    the hex data of SigTimeZoneGet.
 * @return  return `nil` when initialize SigTimeZoneGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 5.2.1.5 Time Zone Set, opcode:0x823C


/// Time Zone Set is an acknowledged message used to set
/// the Time Zone Offset New state (see Section 5.1.1.6) and
/// the TAI of Zone Change state (see Section 5.1.1.7).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.144),
/// 5.2.1.5 Time Zone Set.
@interface SigTimeZoneSet : SigAcknowledgedGenericMessage
/// Upcoming local time zone offset.
/// The Time Zone Offset New field identifies the Time Zone
/// Offset New state (see Section 5.1.1.6).
@property (nonatomic, assign) UInt8 timeZoneOffsetNew;
/// 40 bits, TAI seconds time of the upcoming Time Zone
/// Offset change.
/// The TAI of Zone Change field identifies the TAI of Zone
/// Change state (see Section 5.1.1.7).
@property (nonatomic, assign) UInt64 TAIOfZoneChange;

- (instancetype)initWithTimeZoneOffsetNew:(UInt8)timeZoneOffsetNew TAIOfZoneChange:(UInt64)TAIOfZoneChange;

/**
 * @brief   Initialize SigTimeZoneSet object.
 * @param   parameters    the hex data of SigTimeZoneSet.
 * @return  return `nil` when initialize SigTimeZoneSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 5.2.1.6 Time Zone Status, opcode:0x823D


/// Time Zone Status is an unacknowledged message used to report
/// the Time Zone Offset Current state (see Section 5.1.1.5), the Time
/// Zone Offset New state (see Section 5.1.1.6), and the TAI of Zone
/// Change state (see Section 5.1.1.7).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.144),
/// 5.2.1.6 Time Zone Status.
@interface SigTimeZoneStatus : SigGenericMessage
/// Current local time zone offset.
/// The Time Zone Offset Current field identifies the Time Zone Offset
/// Current state (see Section 5.1.1.5).
@property (nonatomic, assign) UInt8 timeZoneOffsetCurrent;
/// Upcoming local time zone offset.
/// The Time Zone Offset New field identifies the Time Zone Offset
/// New state (see Section 5.1.1.6).
@property (nonatomic, assign) UInt8 timeZoneOffsetNew;
/// 40 bits, TAI seconds time of the upcoming Time Zone Offset change.
/// The TAI of Zone Change field identifies the TAI of Zone Change
/// state (see Section 5.1.1.7).
@property (nonatomic, assign) UInt64 TAIOfZoneChange;

/**
 * @brief   Initialize SigTimeZoneStatus object.
 * @param   parameters    the hex data of SigTimeZoneStatus.
 * @return  return `nil` when initialize SigTimeZoneStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 5.2.1.7 TAI-UTC Delta Get, opcode:0x823E


/// TAI-UTC Delta Get is an acknowledged message used to get
/// the TAI-UTC Delta Current state (see Section 5.1.1.8), the
/// TAI-UTC Delta New state (see Section 5.1.1.9), and the TAI of
/// Delta Change state (see Section 5.1.1.10).
/// @note   The response to the TAI-UTC Delta Get message
/// is a TAI-UTC Delta Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.145),
/// 5.2.1.7 TAI-UTC Delta Get.
@interface SigTAI_UTC_DeltaGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigTAI_UTC_DeltaGet object.
 * @param   parameters    the hex data of SigTAI_UTC_DeltaGet.
 * @return  return `nil` when initialize SigTAI_UTC_DeltaGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 5.2.1.8 TAI-UTC Delta Set, opcode:0x823F


/// TAI-UTC Delta Set is an acknowledged message used to set
/// the TAI-UTC Delta New state (see Section 5.1.1.9) and the TAI
/// of Delta Change state (see Section 5.1.1.10).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.145),
/// 5.2.1.8 TAI-UTC Delta Set.
@interface SigTAI_UTC_DeltaSet : SigAcknowledgedGenericMessage
/// 15 bits, Upcoming difference between TAI and UTC in seconds.
/// The TAI-UTC Delta New field identifies the TAI-UTC Delta
/// New state (see Section 5.1.1.9).
@property (nonatomic, assign) UInt16 TAI_UTC_DeltaNew;
/// 1 bit, Always 0b0. Other values are Prohibited.
@property (nonatomic, assign) UInt8 padding;
/// 40 bits, TAI seconds time of the upcoming TAI-UTC Delta change.
/// The TAI of Delta Change field identifies the TAI of Delta Change
/// state (see Section 5.1.1.10).
@property (nonatomic, assign) UInt64 TAIOfDeltaChange;

- (instancetype)initWithTAI_UTC_DeltaNew:(UInt16)TAI_UTC_DeltaNew padding:(UInt8)padding TAIOfDeltaChange:(UInt64)TAIOfDeltaChange;

/**
 * @brief   Initialize SigTAI_UTC_DeltaSet object.
 * @param   parameters    the hex data of SigTAI_UTC_DeltaSet.
 * @return  return `nil` when initialize SigTAI_UTC_DeltaSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 5.2.1.9 TAI-UTC Delta Status, opcode:0x8240


/// TAI-UTC Delta Status is an unacknowledged message used to report
/// the TAI-UTC Delta Current state (see Section 5.1.1.8), the TAI-UTC
/// Delta New state (see Section 5.1.1.9), and the TAI of Delta Change
/// state (see Section 5.1.1.10).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.145),
/// 5.2.1.9 TAI-UTC Delta Status.
@interface SigTAI_UTC_DeltaStatus : SigGenericMessage
/// 15 bits, Current difference between TAI and UTC in seconds.
/// The TAI-UTC Delta Current field identifies the TAI-UTC Delta Current
/// state (see Section 5.1.1.8).
@property (nonatomic, assign) UInt16 TAI_UTC_DeltaCurrent;
/// 1 bit, Always 0b0. Other values are Prohibited.
@property (nonatomic, assign) UInt8 paddingCurrent;
/// 15 bits, Upcoming difference between TAI and UTC in seconds.
/// The TAI-UTC Delta New field identifies the TAI-UTC Delta New
/// state (see Section 5.1.1.9).
@property (nonatomic, assign) UInt16 TAI_UTC_DeltaNew;
/// 1 bit, Always 0b0. Other values are Prohibited.
@property (nonatomic, assign) UInt8 paddingNew;
/// 40 bits, TAI seconds time of the upcoming TAI-UTC Delta change.
/// The TAI Of Delta Change field identifies the TAI of Delta Change
/// state (see Section 5.1.1.10).
@property (nonatomic, assign) UInt64 TAIOfDeltaChange;

/**
 * @brief   Initialize SigTAI_UTC_DeltaStatus object.
 * @param   parameters    the hex data of SigTAI_UTC_DeltaStatus.
 * @return  return `nil` when initialize SigTAI_UTC_DeltaStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark - 5.2.2 Scene messages


#pragma mark 5.2.2.5 Scene Get, opcode:0x8241


/// Scene Get is an acknowledged message used to get
/// the current status of a currently active scene (see
/// Section 5.1.3.2) of an element.
/// @note   The response to the Scene Get message is
/// a Scene Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.148),
/// 5.2.2.5 Scene Get.
@interface SigSceneGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigSceneGet object.
 * @param   parameters    the hex data of SigSceneGet.
 * @return  return `nil` when initialize SigSceneGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 5.2.2.3 Scene Recall, opcode:0x8242


/// Scene Recall is an acknowledged message that is used to recall
/// the current state of an element from a previously stored scene.
/// @note   The response to the Scene Recall message is a Scene
/// Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.147),
/// 5.2.2.3 Scene Recall.
@interface SigSceneRecall : SigAcknowledgedGenericMessage
/// The number of the scene to be recalled.
/// The Scene Number field identifies the intended Scene.
/// The value 0x0000 is Prohibited.
@property (nonatomic, assign) UInt16 sceneNumber;
/// the Transition Time field identifies the time that an element will take to transition
/// from the present states to the target states defined by the recalled Scene.
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Transition Time field identifies the time that an element will
/// take to transition from the present states to the target states defined by
/// the recalled Scene. The format of the Transition Time field matches the
/// format of the Generic Default Transition Time state as defined in Section
/// 3.1.3. Only values of 0x00 through 0x3E shall be used to specify the
/// Transition Number of Steps.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps. (C.1)
/// C.1: If the Transition Time field is present, the Delay field shall also be present;
/// otherwise this field shall not be present.
/// The Delay field shall be present when the Transition Time field is present. It identifies
/// the message execution delay, which represents a time interval between receiving the
/// message by a model and executing the associated model behaviors.
@property (nonatomic, assign) UInt8 delay;

- (instancetype)initWithSceneNumber:(UInt16)sceneNumber transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay;

/**
 * @brief   Initialize SigSceneRecall object.
 * @param   parameters    the hex data of SigSceneRecall.
 * @return  return `nil` when initialize SigSceneRecall object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 5.2.2.4 Scene Recall Unacknowledged, opcode:0x8243


/// Scene Recall Unacknowledged is an unacknowledged message used to recall
/// the current state of an element from a previously stored Scene.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.147),
/// 5.2.2.4 Scene Recall Unacknowledged.
@interface SigSceneRecallUnacknowledged : SigGenericMessage
/// The number of the scene to be recalled.
/// The Scene Number field identifies the intended Scene.
/// The value 0x0000 is Prohibited.
@property (nonatomic, assign) UInt16 sceneNumber;
/// the Transition Time field identifies the time that an element will take to transition
/// from the present states to the target states defined by the recalled Scene.
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Transition Time field identifies the time that an element will
/// take to transition from the present states to the target states defined by
/// the recalled Scene. The format of the Transition Time field matches the
/// format of the Generic Default Transition Time state as defined in Section
/// 3.1.3. Only values of 0x00 through 0x3E shall be used to specify the
/// Transition Number of Steps.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps. (C.1)
/// C.1: If the Transition Time field is present, the Delay field shall also be present;
/// otherwise this field shall not be present.
/// The Delay field shall be present when the Transition Time field is present. It identifies
/// the message execution delay, which represents a time interval between receiving the
/// message by a model and executing the associated model behaviors.
@property (nonatomic, assign) UInt8 delay;

- (instancetype)initWithSceneNumber:(UInt16)sceneNumber transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay;

/**
 * @brief   Initialize SigSceneRecallUnacknowledged object.
 * @param   parameters    the hex data of SigSceneRecallUnacknowledged.
 * @return  return `nil` when initialize SigSceneRecallUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 5.2.2.6 Scene Status, opcode:0x5E


/// Scene Status is an unacknowledged message used to report the current
/// status of a currently active scene (see Section 5.1.3.2) of an element.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.148),
/// 5.2.2.6 Scene Status.
@interface SigSceneStatus : SigGenericMessage
/// The Status Code field identifies the status code for the last operation.
/// The allowed values for status codes and their meanings are documented
/// in Section 5.2.2.11.
@property (nonatomic, assign) SigSceneResponseStatus statusCode;
/// Scene Number of a current scene.
/// The Current Scene field identifies the Scene Number of the current Scene.
/// If no scene is active, the Current Scene field value is 0.
@property (nonatomic, assign) UInt16 currentScene;
/// Scene Number of a target scene. (Optional)
/// When an element is in the process of changing the Scene state, the Target
/// Scene field identifies the target Scene Number of the target Scene state
/// the element is to reach.
/// When an element is not in the process of changing the Scene state,
/// the Target Scene field shall be omitted.
@property (nonatomic, assign) UInt16 targetScene;
/// Format as defined in Section 3.1.3. (C.1)
/// C.1: If the Target Scene field is present, the Remaining Time field shall also
/// be present; otherwise the fields shall not be present.
/// If present, the Remaining Time field indicates the time it will take the element
/// to complete the transition to the target Scene state of the element.
@property (nonatomic,strong,nullable) SigTransitionTime *remainingTime;

- (instancetype)initWithStatusCode:(SigSceneResponseStatus)statusCode currentScene:(UInt16)currentScene targetScene:(UInt16)targetScene remainingTime:(nullable SigTransitionTime *)remainingTime;

/**
 * @brief   Initialize SigSceneStatus object.
 * @param   parameters    the hex data of SigSceneStatus.
 * @return  return `nil` when initialize SigSceneStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 5.2.2.7 Scene Register Get, opcode:0x8244


/// Scene Register Get is an acknowledged message used to get the
/// current status of the Scene Register (see Section 5.1.3.1) of an element.
/// @note   The response to the Scene Register Get message is a
/// Scene Register Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.148),
/// 5.2.2.7 Scene Register Get.
@interface SigSceneRegisterGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigSceneRegisterGet object.
 * @param   parameters    the hex data of SigSceneRegisterGet.
 * @return  return `nil` when initialize SigSceneRegisterGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 5.2.2.8 Scene Register Status, opcode:0x8245


/// Scene Register Status is an unacknowledged message that is used to report
/// the current status of the Scene Register (see Section 5.1.3.1) of an element.
/// @note   The message uses a single-octet Opcode to maximize the payload size.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.149),
/// 5.2.2.8 Scene Register Status.
@interface SigSceneRegisterStatus : SigGenericMessage
/// The Status Code field identifies the status code for the previous operation.
/// The allowed values for status codes and their meanings are documented
/// in Section 5.2.2.11.
@property (nonatomic, assign) SigSceneResponseStatus statusCode;
/// Scene Number of a current scene.
/// The Current Scene field identifies the Scene Number of the current scene.
@property (nonatomic, assign) UInt16 currentScene;
/// A list of scenes stored within an element.
/// The Scenes field identifies the Scene Register state (see Section 5.1.3.1)
/// of an element.
@property (nonatomic, strong) NSMutableArray <NSNumber *>*scenes;//[(UInt16)sceneNumber]

- (instancetype)initWithStatusCode:(SigSceneResponseStatus)statusCode currentScene:(UInt16)currentScene targetScene:(UInt16)targetScene scenes:(NSMutableArray <NSNumber *>*)scenes;

/**
 * @brief   Initialize SigSceneRegisterStatus object.
 * @param   parameters    the hex data of SigSceneRegisterStatus.
 * @return  return `nil` when initialize SigSceneRegisterStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 5.2.2.1 Scene Store, opcode:0x8246


/// Scene Store is an acknowledged message used to store the current state
/// of an element as a Scene, which can be recalled later.
/// @note   The response to the Scene Store message is a Scene Register
/// Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.146),
/// 5.2.2.1 Scene Store.
@interface SigSceneStore : SigAcknowledgedGenericMessage
/// The number of the scene to be stored.
/// The Scene Number field identifies the intended scene.
/// The value 0x0000 is Prohibited.
@property (nonatomic, assign) UInt16 sceneNumber;

- (instancetype)initWithSceneNumber:(UInt16)sceneNumber;

/**
 * @brief   Initialize SigSceneStore object.
 * @param   parameters    the hex data of SigSceneStore.
 * @return  return `nil` when initialize SigSceneStore object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 5.2.2.2 Scene Store Unacknowledged, opcode:0x8247


/// Scene Store Unacknowledged is an unacknowledged message used to store
/// the current state of an element as a Scene, which can be recalled later.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.146),
/// 5.2.2.2 Scene Store Unacknowledged.
@interface SigSceneStoreUnacknowledged : SigGenericMessage
/// The number of the scene to be stored.
/// The Scene Number field identifies the intended scene.
/// The value 0x0000 is Prohibited.
@property (nonatomic, assign) UInt16 sceneNumber;

- (instancetype)initWithSceneNumber:(UInt16)sceneNumber;

/**
 * @brief   Initialize SigSceneStoreUnacknowledged object.
 * @param   parameters    the hex data of SigSceneStoreUnacknowledged.
 * @return  return `nil` when initialize SigSceneStoreUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 5.2.2.9 Scene Delete, opcode:0x829E


/// Scene Delete is an acknowledged message used to delete a Scene from
/// the Scene Register state (see Section 5.1.3.1) of an element.
/// @note   The response to the Scene Delete message is a Scene Register
/// Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.149),
/// 5.2.2.9 Scene Delete.
@interface SigSceneDelete : SigAcknowledgedGenericMessage
/// The number of the scene to be deleted.
/// The Scene Number field identifies the Scene to be deleted.
@property (nonatomic, assign) UInt16 sceneNumber;

- (instancetype)initWithSceneNumber:(UInt16)sceneNumber;

/**
 * @brief   Initialize SigSceneDelete object.
 * @param   parameters    the hex data of SigSceneDelete.
 * @return  return `nil` when initialize SigSceneDelete object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 5.2.2.10 Scene Delete Unacknowledged, opcode:0x829F


/// Scene Delete Unacknowledged is an unacknowledged message used to delete
/// a scene from the Scene Register state (see Section 5.1.3.1) of an element.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.149),
/// 5.2.2.10 Scene Delete Unacknowledged.
@interface SigSceneDeleteUnacknowledged : SigGenericMessage
/// The number of the scene to be deleted.
/// The Scene Number field identifies the Scene to be deleted.
@property (nonatomic, assign) UInt16 sceneNumber;

- (instancetype)initWithSceneNumber:(UInt16)sceneNumber;

/**
 * @brief   Initialize SigSceneDeleteUnacknowledged object.
 * @param   parameters    the hex data of SigSceneDeleteUnacknowledged.
 * @return  return `nil` when initialize SigSceneDeleteUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark - 5.2.3 Scheduler messages


#pragma mark 5.2.3.3 Scheduler Action Get, opcode:0x8248


/// Scheduler Action Get is an acknowledged message used to report
/// the action defined by the entry of the Schedule Register state of an
/// element (see Section 5.1.4.2), identified by the Index field.
/// @note   The response to the Scheduler Action Get message is a
/// Scheduler Action Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.150),
/// 5.2.3.3 Scheduler Action Get.
@interface SigSchedulerActionGet : SigAcknowledgedGenericMessage
/// Index of the Schedule Register entry to get.
/// The Index field identifies a single corresponding entry of the Schedule Register.
/// The valid values for the Index field are 0x00–0x0F. Values 0x10–0xFF are Prohibited.
@property (nonatomic, assign) UInt8 index;

- (instancetype)initWithIndex:(UInt8)index;

/**
 * @brief   Initialize SigSchedulerActionGet object.
 * @param   parameters    the hex data of SigSchedulerActionGet.
 * @return  return `nil` when initialize SigSchedulerActionGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 5.2.3.6 Scheduler Action Status, opcode:0x5F


/// Scheduler Action Status is an unacknowledged message used to report
/// the entry of the Schedule Register state of an element (see Section 5.1.4.2),
/// identified by the Index field.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.151),
/// 5.2.3.6 Scheduler Action Status.
@interface SigSchedulerActionStatus : SigGenericMessage
/// 80 bits, The Schedule Register bit field shall be set to the value of the entry
/// of the Schedule Register that is indicated by the Index field.
/// The Index field identifies a single corresponding entry of the Schedule Register.
/// The valid values for the Index field are 0x0-0xF.
@property (nonatomic, strong) SchedulerModel *schedulerModel;

- (instancetype)initWithSchedulerModel:(SchedulerModel *)schedulerModel;

/**
 * @brief   Initialize SigSchedulerActionStatus object.
 * @param   parameters    the hex data of SigSchedulerActionStatus.
 * @return  return `nil` when initialize SigSchedulerActionStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 5.2.3.1 Scheduler Get, opcode:0x8249


/// Scheduler Get is an acknowledged message used to get the
/// current Schedule Register state of an element (see Section 5.1.4.2).
/// @note   The response to the Scheduler Get message is a
/// Scheduler Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.150),
/// 5.2.3.1 Scheduler Get.
@interface SigSchedulerGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigSchedulerGet object.
 * @param   parameters    the hex data of SigSchedulerGet.
 * @return  return `nil` when initialize SigSchedulerGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 5.2.3.2 Scheduler Status, opcode:0x824A


/// Scheduler Status is an unacknowledged message used to report
/// the current Schedule Register state of an element (see Section 5.1.4.2).
/// @note   The message shall be sent as a response to the
/// Scheduler Get message (see Section 5.2.3.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.151),
/// 5.2.3.2 Scheduler Status.
@interface SigSchedulerStatus : SigGenericMessage
/// 16 bits, Bit field indicating defined Actions in the Schedule Register.
/// Each bit of the Schedules field set to 1 identifies a corresponding
/// entry of the Schedule Register.
@property (nonatomic, strong) NSMutableArray <NSNumber *>*schedulers;//[schedulerID]

- (instancetype)initWithSchedulers:(NSMutableArray <NSNumber *>*)schedulers;

/**
 * @brief   Initialize SigSchedulerStatus object.
 * @param   parameters    the hex data of SigSchedulerStatus.
 * @return  return `nil` when initialize SigSchedulerStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 5.2.3.4 Scheduler Action Set, opcode:0x60


/// Scheduler Action Set is an acknowledged message used to set
/// the entry of the Schedule Register state of an element (see
/// Section 5.1.4.2), identified by the Index field.
/// @note   The response to the Scheduler Action Set message
/// is a Scheduler Action Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.150),
/// 5.2.3.4 Scheduler Action Set.
@interface SigSchedulerActionSet : SigAcknowledgedGenericMessage
/// 80 bits, The Schedule Register bit field shall be set to the value of
/// the entry of the Schedule Register that is indicated by the Index field.
/// The Index field identifies a single corresponding entry of the Schedule
/// Register. The valid values for the Index field are 0x0-0xF.
/// The Schedule Register bit field identifies the value of the entry of the
/// Schedule Register that is indicated by the Index field.
@property (nonatomic, strong) SchedulerModel *schedulerModel;

- (instancetype)initWithSchedulerModel:(SchedulerModel *)schedulerModel;

/**
 * @brief   Initialize SigSchedulerActionSet object.
 * @param   parameters    the hex data of SigSchedulerActionSet.
 * @return  return `nil` when initialize SigSchedulerActionSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 5.2.3.5 Scheduler Action Set Unacknowledged, opcode:0x61


/// Scheduler Action Set Unacknowledged is an unacknowledged message used to set
/// the entry of the Schedule Register state of an element (see Section 5.1.4.2), identified
/// by the Index field.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.151),
/// 5.2.3.5 Scheduler Action Set Unacknowledged.
@interface SigSchedulerActionSetUnacknowledged : SigGenericMessage
/// 80 bits, The Schedule Register bit field shall be set to the value of
/// the entry of the Schedule Register that is indicated by the Index field.
/// The Index field identifies a single corresponding entry of the Schedule
/// Register. The valid values for the Index field are 0x0-0xF.
/// The Schedule Register bit field identifies the value of the entry of the
/// Schedule Register that is indicated by the Index field.
@property (nonatomic, strong) SchedulerModel *schedulerModel;

- (instancetype)initWithSchedulerModel:(SchedulerModel *)schedulerModel;

/**
 * @brief   Initialize SigSchedulerActionSetUnacknowledged object.
 * @param   parameters    the hex data of SigSchedulerActionSetUnacknowledged.
 * @return  return `nil` when initialize SigSchedulerActionSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark - 6.3.1 Light Lightness messages


#pragma mark 6.3.1.1 Light Lightness Get, opcode:0x824B


/// Light Lightness Get is an acknowledged message used to get
/// the Light Lightness Actual state of an element (see Section 6.1.2.1).
/// @note   The response to the Light Lightness Get message is
/// a Light Lightness Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.199),
/// 6.3.1.1 Light Lightness Get.
@interface SigLightLightnessGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigLightLightnessGet object.
 * @param   parameters    the hex data of SigLightLightnessGet.
 * @return  return `nil` when initialize SigLightLightnessGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.1.2 Light Lightness Set, opcode:0x824C


/// The Light Lightness Set is an acknowledged message used to set
/// the Light Lightness Actual state of an element (see Section 6.1.2).
/// @note   The response to the Light Lightness Set message is a
/// Light Lightness Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.200),
/// 6.3.1.2 Light Lightness Set.
@interface SigLightLightnessSet : SigAcknowledgedGenericMessage
/// The target value of the Light Lightness Actual state.
/// The Lightness field identifies the Light Lightness Actual state of the
/// element (see Section 6.1.2.1).
@property (nonatomic, assign) UInt16 lightness;
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Transition Time field identifies the time an element will take
/// to transition to the target state from the present state (see Section 1.4.1.1).
/// The format of the Transition Time field matches the format of the Generic
/// Default Transition Time state, as defined in Section 3.1.3. Only values of
/// 0x00 to 0x3E shall be used to specify the Transition Number of Steps.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps. (C.1)
/// C.1: If the Transition Time field is present, the Delay field shall also be present;
/// otherwise these fields shall not be present.
/// The Delay field shall be present when the Transition Time field is present. It
/// identifies the message execution delay, representing a time interval between
/// receiving the message by a model and executing the associated model behaviors.
@property (nonatomic,assign) UInt8 delay;

- (instancetype)initWithLightness:(UInt16)lightness transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay;

/**
 * @brief   Initialize SigLightLightnessSet object.
 * @param   parameters    the hex data of SigLightLightnessSet.
 * @return  return `nil` when initialize SigLightLightnessSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.1.3 Light Lightness Set Unacknowledged, opcode:0x824D


/// The Light Lightness Set Unacknowledged is an unacknowledged message used to set
/// the Light Lightness Actual state of an element (see Section 6.1.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.200),
/// 6.3.1.3 Light Lightness Set Unacknowledged.
@interface SigLightLightnessSetUnacknowledged : SigGenericMessage
/// The target value of the Light Lightness Actual state.
/// The Lightness field identifies the Light Lightness Actual state of the
/// element (see Section 6.1.2.1).
@property (nonatomic, assign) UInt16 lightness;
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Transition Time field identifies the time an element will take
/// to transition to the target state from the present state (see Section 1.4.1.1).
/// The format of the Transition Time field matches the format of the Generic
/// Default Transition Time state, as defined in Section 3.1.3. Only values of
/// 0x00 to 0x3E shall be used to specify the Transition Number of Steps.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps. (C.1)
/// C.1: If the Transition Time field is present, the Delay field shall also be present;
/// otherwise these fields shall not be present.
/// The Delay field shall be present when the Transition Time field is present. It
/// identifies the message execution delay, representing a time interval between
/// receiving the message by a model and executing the associated model behaviors.
@property (nonatomic,assign) UInt8 delay;

- (instancetype)initWithLightness:(UInt16)lightness transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay;

/**
 * @brief   Initialize SigLightLightnessSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightLightnessSetUnacknowledged.
 * @return  return `nil` when initialize SigLightLightnessSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.1.4 Light Lightness Status, opcode:0x824E


/// The Light Lightness Status is an unacknowledged message used to report
/// the Light Lightness Actual state of an element (see Section 6.1.2.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.201),
/// 6.3.1.4 Light Lightness Status.
@interface SigLightLightnessStatus : SigGenericMessage
/// The present value of the Light Lightness Actual state.
/// The Present Lightness field identifies the present Light Lightness Actual
/// state of the element (see Section 6.1.2.1).
@property (nonatomic, assign) UInt16 presentLightness;
/// The target value of the Light Lightness Actual state.
/// When an element is in the process of changing the Light Lightness Actual
/// state, the Target Lightness field identifies the target Light Lightness Actual
/// state that the element is to reach (see Section 6.1.2.1).
/// When an element is not in the process of changing the Light Lightness
/// Actual state, the Target Lightness field shall be omitted.
@property (nonatomic, assign) UInt16 targetLightness;
/// Format as defined in Section 3.1.3. (C.1)
/// C.1: If the Target Lightness field is present, the Remaining Time field shall
/// also be present; otherwise these fields shall not be present.
/// If present, the Remaining Time field identifies the time it will take the element
/// to complete the transition to the target Light Lightness Actual state of the
/// element (see Section 1.4.1.1 and Section 6.1.2.1).
@property (nonatomic,strong,nullable) SigTransitionTime *remainingTime;

- (instancetype)initWithPresentLightness:(UInt16)presentLightness targetLightness:(UInt16)targetLightness remainingTime:(nullable SigTransitionTime *)remainingTime;

/**
 * @brief   Initialize SigLightLightnessStatus object.
 * @param   parameters    the hex data of SigLightLightnessStatus.
 * @return  return `nil` when initialize SigLightLightnessStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.1.5 Light Lightness Linear Get, opcode:0x824F


/// Light Lightness Linear Get is an acknowledged message used to get
/// the Light Lightness Linear state of an element (see Section 6.1.2.1).
/// @note   The response to the Light Lightness Linear Get message
/// is a Light Lightness Linear Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.201),
/// 6.3.1.5 Light Lightness Linear Get.
@interface SigLightLightnessLinearGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigLightLightnessLinearGet object.
 * @param   parameters    the hex data of SigLightLightnessLinearGet.
 * @return  return `nil` when initialize SigLightLightnessLinearGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.1.6 Light Lightness Linear Set, opcode:0x8250


/// The Light Lightness Linear Set is an acknowledged message used to set
/// the Light Lightness Linear state of an element (see Section 6.1.2.1).
/// @note   The response to the Light Lightness Linear Set message is a
/// Light Lightness Linear Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.201),
/// 6.3.1.6 Light Lightness Linear Set.
@interface SigLightLightnessLinearSet : SigAcknowledgedGenericMessage
/// The target value of the Light Lightness Actual state.
/// The Lightness field identifies the Light Lightness Linear state of the
/// element (see Section 6.1.2.1).
@property (nonatomic, assign) UInt16 lightness;
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Transition Time field identifies the time an element will take
/// to transition to the target state from the present state (see Section 1.4.1.1).
/// The format of the Transition Time field matches the format of the Generic
/// Default Transition Time state, as defined in Section 3.1.3. Only values of
/// 0x00 to 0x3E shall be used to specify the Transition Number of Steps.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
/// The Delay field shall be present when the Transition Time field is present.
/// It identifies the message execution delay, representing a time interval
/// between receiving the message by a model and executing the associated
/// model behaviors.
@property (nonatomic,assign) UInt8 delay;

- (instancetype)initWithLightness:(UInt16)lightness transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay;

/**
 * @brief   Initialize SigLightLightnessLinearSet object.
 * @param   parameters    the hex data of SigLightLightnessLinearSet.
 * @return  return `nil` when initialize SigLightLightnessLinearSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.1.7 Light Lightness Linear Set Unacknowledged, opcode:0x8251


/// The Light Lightness Linear Set Unacknowledged is an unacknowledged message
/// used to set the Light Lightness Linear state of an element (see Section 6.1.2.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.202),
/// 6.3.1.7 Light Lightness Linear Set Unacknowledged.
@interface SigLightLightnessLinearSetUnacknowledged : SigGenericMessage
/// The target value of the Light Lightness Actual state.
/// The Lightness field identifies the Light Lightness Linear state of the
/// element (see Section 6.1.2.1).
@property (nonatomic, assign) UInt16 lightness;
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Transition Time field identifies the time an element will take
/// to transition to the target state from the present state (see Section 1.4.1.1).
/// The format of the Transition Time field matches the format of the Generic
/// Default Transition Time state, as defined in Section 3.1.3. Only values of
/// 0x00 to 0x3E shall be used to specify the Transition Number of Steps.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps.
/// The Delay field shall be present when the Transition Time field is present.
/// It identifies the message execution delay, representing a time interval
/// between receiving the message by a model and executing the associated
/// model behaviors.
@property (nonatomic,assign) UInt8 delay;

- (instancetype)initWithLightness:(UInt16)lightness transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay;

/**
 * @brief   Initialize SigLightLightnessLinearSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightLightnessLinearSetUnacknowledged.
 * @return  return `nil` when initialize SigLightLightnessLinearSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.1.8 Light Lightness Linear Status, opcode:0x8252


/// The Light Lightness Linear Status is an unacknowledged message used to report
/// the Light Lightness Linear state of an element (see Section 6.1.2.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.203),
/// 6.3.1.8 Light Lightness Linear Status.
@interface SigLightLightnessLinearStatus : SigGenericMessage
/// The present value of the Light Lightness Linear state.
/// The Present Lightness field identifies the present Light Lightness Linear
/// state of the element (see Section 6.1.2.1).
@property (nonatomic, assign) UInt16 presentLightness;
/// The target value of the Light Lightness Linear state (Optional)
/// When an element is in the process of changing the Light Lightness Linear state,
/// the Target Lightness field identifies the target Light Lightness Linear state that the
/// element is to reach (see Section 6.1.2.1).
/// When an element is not in the process of changing the Light Lightness Linear state,
/// the Target Lightness field shall be omitted.
@property (nonatomic, assign) UInt16 targetLightness;
/// Format as defined in Section 3.1.3 (C.1)
/// C.1: If the Target Lightness field is present, the Remaining Time field shall also be present;
/// otherwise these fields shall not be present.
/// If present, the Remaining Time field identifies the time it will take the element to complete
/// the transition to the target Light Lightness Linear state of the element (see Section 1.4.1.1
/// and Section 6.1.2.1).
@property (nonatomic,strong,nullable) SigTransitionTime *remainingTime;

- (instancetype)initWithPresentLightness:(UInt16)presentLightness targetLightness:(UInt16)targetLightness remainingTime:(nullable SigTransitionTime *)remainingTime;

/**
 * @brief   Initialize SigLightLightnessLinearStatus object.
 * @param   parameters    the hex data of SigLightLightnessLinearStatus.
 * @return  return `nil` when initialize SigLightLightnessLinearStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.1.9 Light Lightness Last Get, opcode:0x8253


/// Light Lightness Last Get is an acknowledged message used to get
/// the Light Lightness Last state of an element (see Section 6.1.2.3).
/// @note   The response to the Light Lightness Last Get message
/// is a Light Lightness Last Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.203),
/// 6.3.1.9 Light Lightness Last Get.
@interface SigLightLightnessLastGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigLightLightnessLastGet object.
 * @param   parameters    the hex data of SigLightLightnessLastGet.
 * @return  return `nil` when initialize SigLightLightnessLastGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.1.10 Light Lightness Last Status, opcode:0x8254


/// Light Lightness Last Status is an unacknowledged message used to report
/// the Light Lightness Last state of an element (see Section 6.1.2.3).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.203),
/// 6.3.1.10 Light Lightness Last Status.
@interface SigLightLightnessLastStatus : SigGenericMessage
/// The value of the Light Lightness Last.
/// The Lightness field identifies the Light Lightness Last state of the
/// element (see Section 6.1.2.1).
@property (nonatomic, assign) UInt16 lightness;

- (instancetype)initWithLightness:(UInt16)lightness;

/**
 * @brief   Initialize SigLightLightnessLastStatus object.
 * @param   parameters    the hex data of SigLightLightnessLastStatus.
 * @return  return `nil` when initialize SigLightLightnessLastStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.1.11 Light Lightness Default Get, opcode:0x8255


/// Light Lightness Default Get is an acknowledged message used to get
/// the Light Lightness Default state of an element (see Section 6.1.2.4).
/// @note   The response to the Light Lightness Default Get message is
/// a Light Lightness Default Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.204),
/// 6.3.1.11 Light Lightness Default Get.
@interface SigLightLightnessDefaultGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigLightLightnessDefaultGet object.
 * @param   parameters    the hex data of SigLightLightnessDefaultGet.
 * @return  return `nil` when initialize SigLightLightnessDefaultGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.1.14 Light Lightness Default Status, opcode:0x8256


/// Light Lightness Default Status is an unacknowledged message used to report
/// the Light Lightness Default state of an element (see Section 6.1.2.4).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.204),
/// 6.3.1.14 Light Lightness Default Status.
@interface SigLightLightnessDefaultStatus : SigGenericMessage
/// The value of the Light Lightness Last.
/// The Lightness field identifies the Light Lightness Default state of the element (see Section 6.1.2.4).
@property (nonatomic, assign) UInt16 lightness;

- (instancetype)initWithLightness:(UInt16)lightness;

/**
 * @brief   Initialize SigLightLightnessDefaultStatus object.
 * @param   parameters    the hex data of SigLightLightnessDefaultStatus.
 * @return  return `nil` when initialize SigLightLightnessDefaultStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.1.15 Light Lightness Range Get, opcode:0x8257


/// The Light Lightness Range Get is an acknowledged message used to get
/// the Light Lightness Range state of an element (see Section 6.1.2.5).
/// @note   The response to the Light Lightness Range Get message is a
/// Light Lightness Range Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.205),
/// 6.3.1.15 Light Lightness Range Get.
@interface SigLightLightnessRangeGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigLightLightnessRangeGet object.
 * @param   parameters    the hex data of SigLightLightnessRangeGet.
 * @return  return `nil` when initialize SigLightLightnessRangeGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.1.18 Light Lightness Range Status, opcode:0x8258


/// Light Lightness Default Status is an unacknowledged message used to report
/// the Light Lightness Default state of an element (see Section 6.1.2.4).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.206),
/// 6.3.1.18 Light Lightness Range Status.
@interface SigLightLightnessRangeStatus : SigGenericMessage
/// The Status Code field identifies the status code for the last operation.
/// The Status Code field identifies the Status Code for the last operation on the
/// Light Lightness Range state. The allowed values for status codes and their meanings
/// are documented in Section 7.2.
@property (nonatomic, assign) SigGenericMessageStatus statusCode;
/// The value of the Generic Power Range Min field of the Generic Power Range state.
/// The Range Min field identifies the Lightness Range Min field of the Light Lightness
/// Range state of the element (see Section 6.1.2.5).
@property (nonatomic,assign) UInt16 rangeMin;
/// The value of the Generic Power Range Max field of the Generic Power Range state.
/// The Range Max field identifies the Lightness Range Max field of the Light Lightness
/// Range state of the element (see Section 6.1.2.5).
@property (nonatomic,assign) UInt16 rangeMax;

- (instancetype)initWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax;

/**
 * @brief   Initialize SigLightLightnessRangeStatus object.
 * @param   parameters    the hex data of SigLightLightnessRangeStatus.
 * @return  return `nil` when initialize SigLightLightnessRangeStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.1.12 Light Lightness Default Set, opcode:0x8259


/// The Light Lightness Default Set is an acknowledged message used to set
/// the Light Lightness Default state of an element (see Section 6.1.2.4).
/// @note   The response to the Light Lightness Default Set message is a
/// Light Lightness Default Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.204),
/// 6.3.1.12 Light Lightness Default Set
@interface SigLightLightnessDefaultSet : SigAcknowledgedGenericMessage
/// The value of the Light Lightness Default state.
/// The Lightness field identifies the Light Lightness Default state of the element (see Section 6.1.2.4).
@property (nonatomic, assign) UInt16 lightness;

- (instancetype)initWithLightness:(UInt16)lightness;

/**
 * @brief   Initialize SigLightLightnessDefaultSet object.
 * @param   parameters    the hex data of SigLightLightnessDefaultSet.
 * @return  return `nil` when initialize SigLightLightnessDefaultSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.1.13 Light Lightness Default Set Unacknowledged, opcode:0x825A


/// The Light Lightness Default Set Unacknowledged is an unacknowledged message
/// used to set the Light Lightness Default state of an element (see Section 6.1.2.4).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.204),
/// 6.3.1.13 Light Lightness Default Set Unacknowledged.
@interface SigLightLightnessDefaultSetUnacknowledged : SigGenericMessage
/// The value of the Light Lightness Default state.
/// The Lightness field identifies the Light Lightness Default state of the element (see Section 6.1.2.4).
@property (nonatomic, assign) UInt16 lightness;

- (instancetype)initWithLightness:(UInt16)lightness;

/**
 * @brief   Initialize SigLightLightnessDefaultSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightLightnessDefaultSetUnacknowledged.
 * @return  return `nil` when initialize SigLightLightnessDefaultSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.1.16 Light Lightness Range Set, opcode:0x825B


/// Light Lightness Range Set is an acknowledged message used to set
/// the Light Lightness Range state of an element (see Section 6.1.2.5).
/// @note   The response to the Light Lightness Range Get message
/// is a Light Lightness Range Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.205),
/// 6.3.1.16 Light Lightness Range Set.
@interface SigLightLightnessRangeSet : SigAcknowledgedGenericMessage
/// The value of the Lightness Range Min field of the Light Lightness Range state.
/// The Range Min field identifies the Lightness Range Min field of the Light Lightness
/// Range state of the element (see Section 6.1.2.5).
@property (nonatomic,assign) UInt16 rangeMin;
/// The value of the Lightness Range Max field of the Light Lightness Range state.
/// The Range Max field identifies the Lightness Range Max field of the Light Lightness
/// Range state of the element (see Section 6.1.2.5).
/// The value of the Range Max field shall be greater or equal to the value of the Range
/// Min field.
@property (nonatomic,assign) UInt16 rangeMax;

/// Creates the Light Lightness Range Set message.
/// @param rangeMin  The value of the Lightness Range Min field of the Light Lightness Range state.
/// @param rangeMax  The value of the Lightness Range Max field of the Light Lightness Range state.
- (instancetype)initWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax;

/**
 * @brief   Initialize SigLightLightnessRangeSet object.
 * @param   parameters    the hex data of SigLightLightnessRangeSet.
 * @return  return `nil` when initialize SigLightLightnessRangeSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.1.17 Light Lightness Range Set Unacknowledged, opcode:0x825C


/// Light Lightness Range Set Unacknowledged is an unacknowledged message
/// used to set the Light Lightness Range state of an element (see Section 6.1.2.5).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.205),
/// 6.3.1.17 Light Lightness Range Set Unacknowledged.
@interface SigLightLightnessRangeSetUnacknowledged : SigGenericMessage
/// The value of the Lightness Range Min field of the Light Lightness Range state.
/// The Range Min field identifies the Lightness Range Min field of the Light Lightness
/// Range state of the element (see Section 6.1.2.5).
@property (nonatomic,assign) UInt16 rangeMin;
/// The value of the Lightness Range Max field of the Light Lightness Range state.
/// The Range Max field identifies the Lightness Range Max field of the Light Lightness
/// Range state of the element (see Section 6.1.2.5).
/// The value of the Range Max field shall be greater or equal to the value of the Range
/// Min field.
@property (nonatomic,assign) UInt16 rangeMax;

/// Creates the Light Lightness Range Set message.
/// @param rangeMin  The value of the Lightness Range Min field of the Light Lightness Range state.
/// @param rangeMax  The value of the Lightness Range Max field of the Light Lightness Range state.
- (instancetype)initWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax;

/**
 * @brief   Initialize SigLightLightnessRangeSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightLightnessRangeSetUnacknowledged.
 * @return  return `nil` when initialize SigLightLightnessRangeSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark - 6.3.2 Light CTL Messages


#pragma mark 6.3.2.1 Light CTL Get, opcode:0x825D


/// Light CTL Get is an acknowledged message used to get the Light CTL state of an element (see Section 6.1.3).
/// @note   The response to the Light CTL Get message is a Light CTL Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.206),
/// 6.3.2.1 Light CTL Get.
@interface SigLightCTLGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigLightCTLGet object.
 * @param   parameters    the hex data of SigLightCTLGet.
 * @return  return `nil` when initialize SigLightCTLGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.2.2 Light CTL Set, opcode:0x825E


/// Light CTL Set is an acknowledged message used to set the Light CTL
/// Lightness state, Light CTL Temperature state, and the Light CTL Delta
/// UV state of an element (see Section 6.1.3.6, Section 6.1.3.1, and
/// Section 6.1.3.4).
/// @note   The response to the Light CTL Set message is a Light CTL
/// Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.206),
/// 6.3.2.2 Light CTL Set.
@interface SigLightCTLSet : SigAcknowledgedGenericMessage
/// The target value of the Light CTL Lightness state.
/// The CTL Lightness field identifies the Light CTL Lightness state of
/// the element.
@property (nonatomic,assign) UInt16 CTLLightness;
/// The target value of the Light CTL Temperature state.
/// The CTL Temperature field identifies the Light CTL Temperature state of
/// the element.
@property (nonatomic,assign) UInt16 CTLTemperature;
/// The target value of the Light CTL Delta UV state.
/// The CTL Delta UV field identifies the Light CTL Delta UV state of the element.
@property (nonatomic,assign) UInt16 CTLDeltaUV;
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Transition Time field identifies the time an element will take
/// to transition to the target state from the present state (see Section 1.4.1.1).
/// The format of the Transition Time field matches the format of the Generic
/// Default Transition Time state, as defined in Section 3.1.3. Only values
/// of 0x00 to 0x3E shall be used to specify the Transition Number of Steps.
@property (nonatomic,strong,nullable) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps. (C.1)
/// C.1: If the Transition Time field is present, the Delay field shall also be
/// present; otherwise these fields shall not be present.
/// The Delay field shall be present when the Transition Time field is present.
/// It identifies the message execution delay, representing a time interval
/// between receiving the message by a model and executing the associated
/// model behaviors.
@property (nonatomic,assign) UInt8 delay;

- (instancetype)initWithCTLLightness:(UInt16)CTLLightness CTLTemperature:(UInt16)CTLTemperature CTLDeltaUV:(UInt16)CTLDeltaUV transitionTime:(SigTransitionTime * _Nullable)transitionTime delay:(UInt8)delay;

/**
 * @brief   Initialize SigLightCTLSet object.
 * @param   parameters    the hex data of SigLightCTLSet.
 * @return  return `nil` when initialize SigLightCTLSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.2.3 Light CTL Set Unacknowledged, opcode:0x825F


/// Light CTL Set Unacknowledged is an unacknowledged message used to
/// set the Light CTL Lightness state, Light CTL Temperature state, and the
/// Light CTL Delta UV state of an element (see Section 6.1.3.6, Section 6.1.3.1,
/// and Section 6.1.3.4).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.206),
/// 6.3.2.3 Light CTL Set Unacknowledged
@interface SigLightCTLSetUnacknowledged : SigGenericMessage
/// The target value of the Light CTL Lightness state.
/// The CTL Lightness field identifies the Light CTL Lightness state of
/// the element.
@property (nonatomic,assign) UInt16 CTLLightness;
/// The target value of the Light CTL Temperature state.
/// The CTL Temperature field identifies the Light CTL Temperature state of
/// the element.
@property (nonatomic,assign) UInt16 CTLTemperature;
/// The target value of the Light CTL Delta UV state.
/// The CTL Delta UV field identifies the Light CTL Delta UV state of the element.
@property (nonatomic,assign) UInt16 CTLDeltaUV;
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Transition Time field identifies the time an element will take
/// to transition to the target state from the present state (see Section 1.4.1.1).
/// The format of the Transition Time field matches the format of the Generic
/// Default Transition Time state, as defined in Section 3.1.3. Only values
/// of 0x00 to 0x3E shall be used to specify the Transition Number of Steps.
@property (nonatomic,strong,nullable) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps. (C.1)
/// C.1: If the Transition Time field is present, the Delay field shall also be
/// present; otherwise these fields shall not be present.
/// The Delay field shall be present when the Transition Time field is present.
/// It identifies the message execution delay, representing a time interval
/// between receiving the message by a model and executing the associated
/// model behaviors.
@property (nonatomic,assign) UInt8 delay;

- (instancetype)initWithCTLLightness:(UInt16)CTLLightness CTLTemperature:(UInt16)CTLTemperature CTLDeltaUV:(UInt16)CTLDeltaUV transitionTime:(SigTransitionTime * _Nullable)transitionTime delay:(UInt8)delay;

/**
 * @brief   Initialize SigLightCTLSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightCTLSetUnacknowledged.
 * @return  return `nil` when initialize SigLightCTLSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.2.4 Light CTL Status, opcode:0x8260


/// The Light CTL Status is an unacknowledged message used to report
/// the Light CTL Lightness and the Light CTL Temperature state of an
/// element (see Section 6.1.3.6 and 6.1.3.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.208),
/// 6.3.2.4 Light CTL Status.
@interface SigLightCTLStatus : SigGenericMessage
/// The present value of the Light CTL Lightness state.
/// The Present CTL Lightness field identifies the present Light CTL Lightness
/// state of the element (see Section 6.1.3.4).
@property (nonatomic,assign) UInt16 presentCTLLightness;
/// The present value of the Light CTL Temperature state.
/// The Present CTL Temperature field identifies the present Light CTL
/// Temperature state of the node (see Section 6.1.3.1).
@property (nonatomic,assign) UInt16 presentCTLTemperature;
/// The target value of the Light CTL Lightness state (Optional)
/// If present, the Target CTL Lightness field identifies the target Light CTL
/// Lightness state that the node is to reach (see Section 6.1.3.4).
@property (nonatomic,assign) UInt16 targetCTLLightness;
/// The target value of the Light CTL Temperature state (C.1)
/// C.1: If the Target CTL Lightness field is present, the Target CTL Temperature
/// and the Remaining Time fields shall also be present; otherwise these fields
/// shall not be present.
/// If present, the Target CTL Temperature field identifies the target Light CTL
/// Temperature state that the element is to reach (see Section 6.1.3.1).
@property (nonatomic,assign) UInt16 targetCTLTemperature;
/// Format as defined in Section 3.1.3 (C.1)
/// C.1: If the Target CTL Lightness field is present, the Target CTL Temperature
/// and the Remaining Time fields shall also be present; otherwise these fields
/// shall not be present.
/// If present, the Remaining Time field identifies the time it will take the element
/// to complete the transition to the target Light CTL state of the element (see
/// Section 1.4.1.1 and Section 6.1.3).
@property (nonatomic,strong,nullable) SigTransitionTime *remainingTime;

- (instancetype)initWithPresentCTLLightness:(UInt16)presentCTLLightness presentCTLTemperature:(UInt16)presentCTLTemperature targetCTLLightness:(UInt16)targetCTLLightness targetCTLTemperature:(UInt16)targetCTLTemperature remainingTime:(SigTransitionTime * _Nullable )remainingTime;

/**
 * @brief   Initialize SigLightCTLStatus object.
 * @param   parameters    the hex data of SigLightCTLStatus.
 * @return  return `nil` when initialize SigLightCTLStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.2.5 Light CTL Temperature Get, opcode:0x8261


/// The Light CTL Status is an unacknowledged message used to report
/// the Light CTL Lightness and the Light CTL Temperature state of an
/// element (see Section 6.1.3.6 and 6.1.3.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.208),
/// 6.3.2.5 Light CTL Temperature Get.
@interface SigLightCTLTemperatureGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigLightCTLTemperatureGet object.
 * @param   parameters    the hex data of SigLightCTLTemperatureGet.
 * @return  return `nil` when initialize SigLightCTLTemperatureGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.2.9 Light CTL Temperature Range Get, opcode:0x8262


/// The Light CTL Temperature Range Get is an acknowledged message used to
/// get the Light CTL Temperature Range state of an element (see Section 6.1.3.3).
/// @note   The response to the Light CTL Temperature Range Get message is
/// a Light CTL Temperature Range Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.210),
/// 6.3.2.9 Light CTL Temperature Range Get.
@interface SigLightCTLTemperatureRangeGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigLightCTLTemperatureRangeGet object.
 * @param   parameters    the hex data of SigLightCTLTemperatureRangeGet.
 * @return  return `nil` when initialize SigLightCTLTemperatureRangeGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.2.12 Light CTL Temperature Range Status, opcode:0x8263


/// Light CTL Temperature Range Status is an unacknowledged message used to report
/// the Light CTL Temperature Range state of an element (see Section 6.1.3.3).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.211),
/// 6.3.2.12 Light CTL Temperature Range Status.
@interface SigLightCTLTemperatureRangeStatus : SigGenericMessage
/// Status Code for the requesting message.
/// The Status Code field identifies the Status Code for the last operation on the Light CTL
/// Temperature Range state. The allowed values for status codes and their meanings are
/// documented in Section 7.2.
@property (nonatomic, assign) SigGenericMessageStatus statusCode;
/// The value of the Temperature Range Min field of the Light CTL Temperature Range state.
/// The Range Min field identifies the Temperature Range Min field of the Light CTL
/// Temperature Range state of the element (see Section 6.1.3.3).
@property (nonatomic,assign) UInt16 rangeMin;
/// The value of the Temperature Range Max field of the Light CTL Temperature Range state.
/// The Range Max field identifies the Temperature Range Max field of the Light CTL
/// Temperature Range state of the element (see Section 6.1.3.3).
@property (nonatomic,assign) UInt16 rangeMax;

- (instancetype)initWithStatusCode:(SigGenericMessageStatus)statusCode rangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax;

/**
 * @brief   Initialize SigLightCTLTemperatureRangeStatus object.
 * @param   parameters    the hex data of SigLightCTLTemperatureRangeStatus.
 * @return  return `nil` when initialize SigLightCTLTemperatureRangeStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.2.6 Light CTL Temperature Set, opcode:0x8264


/// The Light CTL Temperature Set is an acknowledged message used to set
/// the Light CTL Temperature state and the Light CTL Delta UV state of an
/// element (see Section 6.1.3.1 and 6.1.3.4).
/// @note   The response to the Light CTL Temperature Set message is a
/// Light CTL Temperature Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.208),
/// 6.3.2.6 Light CTL Temperature Set.
@interface SigLightCTLTemperatureSet : SigAcknowledgedGenericMessage
/// The target value of the Light CTL Temperature state.
/// The CTL Temperature field identifies the Light CTL Temperature state of the element.
@property (nonatomic,assign) UInt16 CTLTemperature;
/// The target value of the Light CTL Delta UV state.
/// The CTL Delta UV field identifies the Light CTL Delta UV state of the element.
@property (nonatomic,assign) UInt16 CTLDeltaUV;
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Transition Time field identifies the time an element will take
/// to transition to the target state from the present state (see Section 1.4.1.1).
/// The format of the Transition Time field matches the format of the Generic
/// Default Transition Time state as defined in Section 3.1.3. Only values of
/// 0x00 to 0x3E shall be used to specify the Transition Number of Steps.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps. (C.1)
/// C.1: If the Transition Time field is present, the Delay field shall also be
/// present; otherwise these fields shall not be present.
/// The Delay field shall be present when the Transition Time field is present.
/// It identifies the message execution delay, representing a time interval
/// between receiving the message by a model and executing the associated
/// model behaviors.
@property (nonatomic,assign) UInt8 delay;

- (instancetype)initWithCTLTemperature:(UInt16)CTLTemperature CTLDeltaUV:(UInt16)CTLDeltaUV transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay;

/**
 * @brief   Initialize SigLightCTLTemperatureSet object.
 * @param   parameters    the hex data of SigLightCTLTemperatureSet.
 * @return  return `nil` when initialize SigLightCTLTemperatureSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.2.7 Light CTL Temperature Set Unacknowledged, opcode:0x8265


/// The Light CTL Temperature Set Unacknowledged is an unacknowledged message
/// used to set the Light CTL Temperature state and the Light CTL Delta UV state of an
/// element (see Section 6.1.3.1 and 6.1.3.4).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.209),
/// 6.3.2.7 Light CTL Temperature Set Unacknowledged.
@interface SigLightCTLTemperatureSetUnacknowledged : SigGenericMessage
/// The target value of the Light CTL Temperature state.
/// The CTL Temperature field identifies the Light CTL Temperature state of the element.
@property (nonatomic,assign) UInt16 CTLTemperature;
/// The target value of the Light CTL Delta UV state.
/// The CTL Delta UV field identifies the Light CTL Delta UV state of the element.
@property (nonatomic,assign) UInt16 CTLDeltaUV;
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Transition Time field identifies the time an element will take
/// to transition to the target state from the present state (see Section 1.4.1.1).
/// The format of the Transition Time field matches the format of the Generic
/// Default Transition Time state as defined in Section 3.1.3. Only values of
/// 0x00 to 0x3E shall be used to specify the Transition Number of Steps.
@property (nonatomic,strong) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps. (C.1)
/// C.1: If the Transition Time field is present, the Delay field shall also be
/// present; otherwise these fields shall not be present.
/// The Delay field shall be present when the Transition Time field is present.
/// It identifies the message execution delay, representing a time interval
/// between receiving the message by a model and executing the associated
/// model behaviors.
@property (nonatomic,assign) UInt8 delay;

- (instancetype)initWithCTLTemperature:(UInt16)CTLTemperature CTLDeltaUV:(UInt16)CTLDeltaUV transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay;

/**
 * @brief   Initialize SigLightCTLTemperatureSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightCTLTemperatureSetUnacknowledged.
 * @return  return `nil` when initialize SigLightCTLTemperatureSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.2.8 Light CTL Temperature Status, opcode:0x8266


/// Light CTL Temperature Status is an unacknowledged message used to report
/// the Light CTL Temperature and Light CTL Delta UV state of an element (see
/// Section 6.1.3.1 and 6.1.3.4).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.210),
/// 6.3.2.8 Light CTL Temperature Status.
@interface SigLightCTLTemperatureStatus : SigGenericMessage
/// The present value of the Light CTL Temperature state.
/// The Present CTL Temperature field identifies the present Light CTL Temperature
/// state of the element (see Section 6.1.3.1).
@property (nonatomic,assign) UInt16 presentCTLTemperature;
/// The present value of the Light CTL Delta UV state.
/// The Present CTL Delta UV field identifies the present Light CTL Delta UV state of
/// the element (see Section 6.1.3.4).
@property (nonatomic,assign) UInt16 presentCTLDeltaUV;
/// The target value of the Light CTL Temperature state (Optional)
/// If present, the Target CTL Temperature field identifies the target Light CTL
/// Temperature state that the element is to reach (see Section 6.1.3.1).
@property (nonatomic,assign) UInt16 targetCTLTemperature;
/// The target value of the Light CTL Delta UV state (C.1)
/// C.1: If the Target CTL Temperature field is present, the Target CTL Delta UV field
/// and the Remaining Time field shall also be present; otherwise these fields shall
/// not be present.
/// If present, the Target CTL Delta UV field identifies the target Light CTL Delta UV
/// state that the element is to reach (see Section 6.1.3.4).
@property (nonatomic,assign) UInt16 targetCTLDeltaUV;
/// Format as defined in Section 3.1.3 (C.1)
/// C.1: If the Target CTL Temperature field is present, the Target CTL Delta UV field
/// and the Remaining Time field shall also be present; otherwise these fields shall
/// not be present.
/// If present, the Remaining Time field identifies the time it will take the element to
/// complete the transition to the target state of the element (see Section 1.4.1.1
/// and Section 6.1.3).
@property (nonatomic,strong,nullable) SigTransitionTime *remainingTime;

- (instancetype)initWithPresentCTLTemperature:(UInt16)presentCTLTemperature presentCTLDeltaUV:(UInt16)presentCTLDeltaUV targetCTLTemperature:(UInt16)targetCTLTemperature targetCTLDeltaUV:(UInt16)targetCTLDeltaUV remainingTime:(nullable SigTransitionTime *)remainingTime;

/**
 * @brief   Initialize SigLightCTLTemperatureStatus object.
 * @param   parameters    the hex data of SigLightCTLTemperatureStatus.
 * @return  return `nil` when initialize SigLightCTLTemperatureStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.2.13 Light CTL Default Get, opcode:0x8267


/// Light CTL Default Get is an acknowledged message used to get the Light CTL Temperature Default and Light CTL Delta UV Default states of an element (see Section 6.1.3).
/// @note   The response to the Light CTL Default Get message is a Light CTL Default Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.212),
/// 6.3.2.13 Light CTL Default Get.
@interface SigLightCTLDefaultGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigLightCTLDefaultGet object.
 * @param   parameters    the hex data of SigLightCTLDefaultGet.
 * @return  return `nil` when initialize SigLightCTLDefaultGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.2.16 Light CTL Default Status, opcode:0x8268


/// The Light CTL Default Status is an unacknowledged message used to report
/// the Light CTL Temperature Default and the Light CTL Delta UV Default states
/// of an element (see Section 6.1.3.2 and 6.1.3.5).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.213),
/// 6.3.2.16 Light CTL Default Status.
@interface SigLightCTLDefaultStatus : SigGenericMessage
/// The value of the Light Lightness Default state.
/// The Lightness field identifies the Light Lightness Default state of the element.
@property (nonatomic,assign) UInt16 lightness;
/// The value of the Light CTL Temperature Default state.
/// The Temperature field identifies the Light CTL Temperature Default state of
/// the element.
@property (nonatomic,assign) UInt16 temperature;
/// The value of the Light CTL Delta UV Default state.
/// The Delta UV field identifies the Light CTL Delta UV Default state of
/// the element.
@property (nonatomic,assign) UInt16 deltaUV;

- (instancetype)initWithLightness:(UInt16)lightness temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV;

/**
 * @brief   Initialize SigLightCTLDefaultStatus object.
 * @param   parameters    the hex data of SigLightCTLDefaultStatus.
 * @return  return `nil` when initialize SigLightCTLDefaultStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.2.14 Light CTL Default Set, opcode:0x8269


/// The Light CTL Default Set is an acknowledged message used to set the
/// Light CTL Temperature Default state and the Light CTL Delta UV Default
/// state of an element (see Section 6.1.3.2 and 6.1.3.5).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.212),
/// 6.3.2.14 Light CTL Default Set.
@interface SigLightCTLDefaultSet : SigAcknowledgedGenericMessage
/// The value of the Light Lightness Default state.
/// The Lightness field identifies the Light Lightness Default state of the element.
@property (nonatomic,assign) UInt16 lightness;
/// The value of the Light CTL Temperature Default state.
/// The Temperature field identifies the Light CTL Temperature Default state of the element.
@property (nonatomic,assign) UInt16 temperature;
/// The value of the Light CTL Delta UV Default state.
/// The Delta UV field identifies the Light CTL Delta UV Default state of the element.
@property (nonatomic,assign) UInt16 deltaUV;

- (instancetype)initWithLightness:(UInt16)lightness temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV;

/**
 * @brief   Initialize SigLightCTLDefaultSet object.
 * @param   parameters    the hex data of SigLightCTLDefaultSet.
 * @return  return `nil` when initialize SigLightCTLDefaultSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.2.14 Light CTL Default Set, opcode:0x826A


/// The Light CTL Default Set Unacknowledged is an unacknowledged message
/// used to set the Light CTL Temperature Default state and the Light CTL Delta
/// UV Default state of an element (see Section 6.1.3.2 and 6.1.3.5).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.212),
/// 6.3.2.14 Light CTL Default Set.
@interface SigLightCTLDefaultSetUnacknowledged : SigGenericMessage
/// The value of the Light Lightness Default state.
/// The Lightness field identifies the Light Lightness Default state of the element.
@property (nonatomic,assign) UInt16 lightness;
/// The value of the Light CTL Temperature Default state.
/// The Temperature field identifies the Light CTL Temperature Default state of the element.
@property (nonatomic,assign) UInt16 temperature;
/// The value of the Light CTL Delta UV Default state.
/// The Delta UV field identifies the Light CTL Delta UV Default state of the element.
@property (nonatomic,assign) UInt16 deltaUV;

- (instancetype)initWithLightness:(UInt16)lightness temperature:(UInt16)temperature deltaUV:(UInt16)deltaUV;

/**
 * @brief   Initialize SigLightCTLDefaultSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightCTLDefaultSetUnacknowledged.
 * @return  return `nil` when initialize SigLightCTLDefaultSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.2.10 Light CTL Temperature Range Set, opcode:0x826B


/// Light CTL Temperature Range Set is an acknowledged message
///  used to set the Light CTL Temperature Range state of an
///  element (see Section 6.1.3.3).
/// @note   The response to the Light CTL Temperature Range Get
/// message is a Light CTL Temperature Range Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.210),
/// 6.3.2.10 Light CTL Temperature Range Set.
@interface SigLightCTLTemperatureRangeSet : SigAcknowledgedGenericMessage
/// The value of the Temperature Range Min field of the Light
/// CTL Temperature Range state.
/// The Range Min field identifies the Temperature Range Min
///  field of the Light CTL Temperature Range state of the
///   element (see Section 6.1.3.3).
@property (nonatomic,assign) UInt16 rangeMin;
/// The value of the Temperature Range Max field of the Light
/// CTL Temperature Range state.
/// The Range Max field identifies the Temperature Range Max
/// field of the Light CTL Temperature Range state of the
/// element (see Section 6.1.3.3).
@property (nonatomic,assign) UInt16 rangeMax;

- (instancetype)initWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax;

/**
 * @brief   Initialize SigLightCTLTemperatureRangeSet object.
 * @param   parameters    the hex data of SigLightCTLTemperatureRangeSet.
 * @return  return `nil` when initialize SigLightCTLTemperatureRangeSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.2.11 Light CTL Temperature Range Set Unacknowledged, opcode:0x826C


/// Light CTL Temperature Range Set Unacknowledged is an unacknowledged
/// message used to set the Light CTL Temperature Range state of an element
/// (see Section 6.1.3.3).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.211),
/// 6.3.2.11 Light CTL Temperature Range Set Unacknowledged.
@interface SigLightCTLTemperatureRangeSetUnacknowledged : SigGenericMessage
/// The value of the Temperature Range Min field of the Light
/// CTL Temperature Range state.
/// The Range Min field identifies the Temperature Range Min
///  field of the Light CTL Temperature Range state of the
///   element (see Section 6.1.3.3).
@property (nonatomic,assign) UInt16 rangeMin;
/// The value of the Temperature Range Max field of the Light
/// CTL Temperature Range state.
/// The Range Max field identifies the Temperature Range Max
/// field of the Light CTL Temperature Range state of the
/// element (see Section 6.1.3.3).
@property (nonatomic,assign) UInt16 rangeMax;

- (instancetype)initWithRangeMin:(UInt16)rangeMin rangeMax:(UInt16)rangeMax;

/**
 * @brief   Initialize SigLightCTLTemperatureRangeSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightCTLTemperatureRangeSetUnacknowledged.
 * @return  return `nil` when initialize SigLightCTLTemperatureRangeSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark - 6.3.3 Light HSL messages


#pragma mark 6.3.3.1 Light HSL Get, opcode:0x826D


/// The Light HSL Get is an acknowledged message used to get
/// the Light HSL Lightness (see Section 6.1.4.5), Light HSL Hue
/// (see Section 6.1.4.1), and Light HSL Saturation (see
/// Section 6.1.4.2) states of an element.
/// @note   The response to the Light HSL Get message is
/// a Light HSL Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.213),
/// 6.3.3.1 Light HSL Get.
@interface SigLightHSLGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigLightHSLGet object.
 * @param   parameters    the hex data of SigLightHSLGet.
 * @return  return `nil` when initialize SigLightHSLGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.3.7 Light HSL Hue Get, opcode:0x826E


/// The Light HSL Hue Get is an acknowledged message used to get
/// the Light HSL Hue state of an element (see Section 6.1.4.1).
/// @note   The response to the Light HSL Hue Get message is a
/// Light HSL Hue Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.216),
/// 6.3.3.7 Light HSL Hue Get.
@interface SigLightHSLHueGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigLightHSLHueGet object.
 * @param   parameters    the hex data of SigLightHSLHueGet.
 * @return  return `nil` when initialize SigLightHSLHueGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.3.8 Light HSL Hue Set, opcode:0x826F


/// The Light HSL Hue Set is an acknowledged message used to set the
/// target Light HSL Hue state of an element (see Section 6.1.4.1).
/// @note   The response to the Light HSL Hue Set message is a Light
/// HSL Hue Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.216),
/// 6.3.3.8 Light HSL Hue Set.
@interface SigLightHSLHueSet : SigAcknowledgedGenericMessage
/// The target value of the Light HSL Hue state.
/// The Hue field identifies the Light HSL Hue of the element (see Section 6.1.4.1).
@property (nonatomic,assign) UInt16 hue;
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Transition Time field identifies the time an element will
/// take to transition to the target state from the present state (see
/// Section 1.4.1.1). The format of the Transition Time field matches the
/// format of the Generic Default Transition Time state, as defined in
/// Section 3.1.3. Only values of 0x00 to 0x3E shall be used to specify
/// the Transition Number of Steps.
@property (nonatomic,strong,nullable) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps. (C.1)
/// C.1: If the Transition Time field is present, the Delay field shall also be
/// present; otherwise these fields shall not be present.
/// The Delay field shall be present when the Transition Time field is present.
/// It identifies the message execution delay, representing a time interval
/// between receiving the message by a model and executing the associated
/// model behaviors.
@property (nonatomic,assign) UInt8 delay;

- (instancetype)initWithHue:(UInt16)hue transitionTime:(SigTransitionTime * _Nullable)transitionTime delay:(UInt8)delay;

/**
 * @brief   Initialize SigLightHSLHueSet object.
 * @param   parameters    the hex data of SigLightHSLHueSet.
 * @return  return `nil` when initialize SigLightHSLHueSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.3.9 Light HSL Hue Set Unacknowledged, opcode:0x8270


/// The Light HSL Hue Set Unacknowledged is an unacknowledged message
/// used to set the target Light HSL Hue state of an element (see Section 6.1.4.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.216),
/// 6.3.3.9 Light HSL Hue Set Unacknowledged.
@interface SigLightHSLHueSetUnacknowledged : SigGenericMessage
/// The target value of the Light HSL Hue state.
/// The Hue field identifies the Light HSL Hue of the element (see Section 6.1.4.1).
@property (nonatomic,assign) UInt16 hue;
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Transition Time field identifies the time an element will
/// take to transition to the target state from the present state (see
/// Section 1.4.1.1). The format of the Transition Time field matches the
/// format of the Generic Default Transition Time state, as defined in
/// Section 3.1.3. Only values of 0x00 to 0x3E shall be used to specify
/// the Transition Number of Steps.
@property (nonatomic,strong,nullable) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps. (C.1)
/// C.1: If the Transition Time field is present, the Delay field shall also be
/// present; otherwise these fields shall not be present.
/// The Delay field shall be present when the Transition Time field is present.
/// It identifies the message execution delay, representing a time interval
/// between receiving the message by a model and executing the associated
/// model behaviors.
@property (nonatomic,assign) UInt8 delay;

- (instancetype)initWithHue:(UInt16)hue transitionTime:(SigTransitionTime * _Nullable)transitionTime delay:(UInt8)delay;

/**
 * @brief   Initialize SigLightHSLHueSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightHSLHueSetUnacknowledged.
 * @return  return `nil` when initialize SigLightHSLHueSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.3.10 Light HSL Hue Status, opcode:0x8271


/// The Light HSL Hue Status is an unacknowledged message used
/// to report the Light HSL Hue state of an element (see Section 6.1.4.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.217),
/// 6.3.3.10 Light HSL Hue Status.
@interface SigLightHSLHueStatus : SigGenericMessage
/// The present value of the Light HSL Hue state.
/// The Present Hue field identifies the present Light HSL Hue state of
/// the element (see Section 6.1.4.1).
@property (nonatomic,assign) UInt16 presentHue;
/// The target value of the Light HSL Hue state (Optional)
/// If present, the Target Hue field identifies the target Light HSL Hue
/// state that the element is to reach (see Section 6.1.4.1).
@property (nonatomic,assign) UInt16 targetHue;
/// Format as defined in Section 3.1.3 (C.1)
/// C.1: If the Target Hue field is present, the Remaining Time field shall
/// also be present; otherwise these fields shall not be present.
/// If present, the Remaining Time field identifies the time it will take the
/// element to complete the transition to the target Light HSL Hue state
/// of the element (see Section 1.4.1.1 and Section 6.1.2).
@property (nonatomic,strong) SigTransitionTime *transitionTime;

- (instancetype)initWithPresentHue:(UInt16)presentHue targetHue:(UInt16)targetHue transitionTime:(SigTransitionTime *)transitionTime;

/**
 * @brief   Initialize SigLightHSLHueStatus object.
 * @param   parameters    the hex data of SigLightHSLHueStatus.
 * @return  return `nil` when initialize SigLightHSLHueStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.3.11 Light HSL Saturation Get, opcode:0x8272


/// The Light HSL Saturation Get is an acknowledged message used to
/// get the Light HSL Saturation state of an element (see Section 6.1.4.2).
/// @note   The response to the Light HSL Saturation Get message is
/// a Light HSL Saturation Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.217),
/// 6.3.3.11 Light HSL Saturation Get.
@interface SigLightHSLSaturationGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigLightHSLSaturationGet object.
 * @param   parameters    the hex data of SigLightHSLSaturationGet.
 * @return  return `nil` when initialize SigLightHSLSaturationGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.3.12 Light HSL Saturation Set, opcode:0x8273


/// The Light HSL Saturation Set is an acknowledged message used to
/// set the target Light HSL Saturation state of an element (see
/// Section 6.1.4.2).
/// @note   The response to the Light HSL Saturation Set message is
/// a Light HSL Saturation Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.218),
/// 6.3.3.12 Light HSL Saturation Set.
@interface SigLightHSLSaturationSet : SigAcknowledgedGenericMessage
/// The target value of the Light HSL Saturation state.
/// The Saturation field identifies the Light HSL Saturation the element
/// (see Section 6.1.4.2).
@property (nonatomic,assign) UInt16 saturation;
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Transition Time field identifies the time an element will
/// take to transition to the target state from the present state (see
/// Section 1.4.1.1). The format of the Transition Time field matches the
/// format of the Generic Default Transition Time state, as defined in
/// Section 3.1.3. Only values of 0x00 to 0x3E shall be used to specify
/// the Transition Number of Steps.
@property (nonatomic,strong,nullable) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps. (C.1)
/// C.1: If the Transition Time field is present, the Delay field shall also be
/// present; otherwise these fields shall not be present.
/// The Delay field shall be present when the Transition Time field is present.
/// It identifies the message execution delay, representing a time interval
/// between receiving the message by a model and executing the associated
/// model behaviors.
@property (nonatomic,assign) UInt8 delay;

- (instancetype)initWithSaturation:(UInt16)saturation transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay;

/**
 * @brief   Initialize SigLightHSLSaturationSet object.
 * @param   parameters    the hex data of SigLightHSLSaturationSet.
 * @return  return `nil` when initialize SigLightHSLSaturationSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.3.13 Light HSL Saturation Set Unacknowledged, opcode:0x8274


/// The Light HSL Saturation Set Unacknowledged is an unacknowledged message
/// used to set the target Light HSL Saturation state of an element (see Section 6.1.4.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.218),
/// 6.3.3.13 Light HSL Saturation Set Unacknowledged.
@interface SigLightHSLSaturationSetUnacknowledged : SigGenericMessage
/// The target value of the Light HSL Saturation state.
/// The Saturation field identifies the Light HSL Saturation the element
/// (see Section 6.1.4.2).
@property (nonatomic,assign) UInt16 saturation;
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Transition Time field identifies the time an element will
/// take to transition to the target state from the present state (see
/// Section 1.4.1.1). The format of the Transition Time field matches the
/// format of the Generic Default Transition Time state, as defined in
/// Section 3.1.3. Only values of 0x00 to 0x3E shall be used to specify
/// the Transition Number of Steps.
@property (nonatomic,strong,nullable) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps. (C.1)
/// C.1: If the Transition Time field is present, the Delay field shall also be
/// present; otherwise these fields shall not be present.
/// The Delay field shall be present when the Transition Time field is present.
/// It identifies the message execution delay, representing a time interval
/// between receiving the message by a model and executing the associated
/// model behaviors.
@property (nonatomic,assign) UInt8 delay;

- (instancetype)initWithSaturation:(UInt16)saturation transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay;

/**
 * @brief   Initialize SigLightHSLSaturationSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightHSLSaturationSetUnacknowledged.
 * @return  return `nil` when initialize SigLightHSLSaturationSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.3.14 Light HSL Saturation Status, opcode:0x8275


/// The Light HSL Saturation Status is an unacknowledged message used to
/// report the Light HSL Saturation state of an element (see Section 6.1.4.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.219),
/// 6.3.3.14 Light HSL Saturation Status.
@interface SigLightHSLSaturationStatus : SigGenericMessage
/// The present value of the Light HSL Saturation state.
/// The Present Saturation field identifies the present Light HSL Saturation state
/// of the element (see Section 6.1.4.2).
@property (nonatomic,assign) UInt16 presentSaturation;
/// The target value of the Light HSL Saturation state. (Optional)
/// If present, the Target Saturation field identifies the target Light HSL Saturation
/// state that the element is to reach (see Section 6.1.4.2).
@property (nonatomic,assign) UInt16 targetSaturation;
/// Format as defined in Section 3.1.3. (C.1)
/// C.1: If the Target Saturation field is present, the Remaining Time field shall also
/// be present; otherwise these fields shall not be present.
/// If present, the Remaining Time field identifies the time it will take the element to
/// complete the transition to the target Light HSL Saturation state of the element
/// (see Section 1.4.1.1 and Section 6.1.2).
@property (nonatomic,strong,nullable) SigTransitionTime *remainingTime;

- (instancetype)initWithPresentSaturation:(UInt16)presentSaturation targetSaturation:(UInt16)targetSaturation remainingTime:(nullable SigTransitionTime *)remainingTime;

/**
 * @brief   Initialize SigLightHSLSaturationStatus object.
 * @param   parameters    the hex data of SigLightHSLSaturationStatus.
 * @return  return `nil` when initialize SigLightHSLSaturationStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.3.2 Light HSL Set, opcode:0x8276


/// The Light HSL Set is an acknowledged message used to set
/// the Light HSL Lightness (see Section 6.1.4.5) state, Light HSL
/// Hue state (see Section 6.1.4.1), and the Light HSL Saturation
/// state (see Section 6.1.4.2) of an element.
/// @note   The response to the Light HSL Set message is a
/// Light HSL Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.213),
/// 6.3.3.2 Light HSL Set.
@interface SigLightHSLSet : SigAcknowledgedGenericMessage
/// The target value of the Light HSL Lightness state.
/// The HSL Lightness field identifies the Light HSL Lightness state
/// of the element.
@property (nonatomic,assign) UInt16 HSLLightness;
/// The target value of the Light HSL Hue state.
/// The HSL Hue field identifies the Light HSL Hue state of the element.
@property (nonatomic,assign) UInt16 HSLHue;
/// The target value of the Light HSL Saturation state.
/// The HSL Saturation field identifies the Light HSL Saturation state
/// of the element.
@property (nonatomic,assign) UInt16 HSLSaturation;
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Transition Time field identifies the time an element will
/// take to transition to the target state from the present state (see
/// Section 1.4.1.1). The format of the Transition Time field matches the
/// format of the Generic Default Transition Time state, as defined in
/// Section 3.1.3. Only values of 0x00 to 0x3E shall be used to specify
/// the Transition Number of Steps.
@property (nonatomic,strong,nullable) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps (C.1)
/// C.1: If the Transition Time field is present, the Delay field shall also
/// be present; otherwise these fields shall not be present.
@property (nonatomic,assign) UInt8 delay;

- (instancetype)initWithHSLLightness:(UInt16)HSLLightness HSLHue:(UInt16)HSLHue HSLSaturation:(UInt16)HSLSaturation transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay;

/**
 * @brief   Initialize SigLightHSLSet object.
 * @param   parameters    the hex data of SigLightHSLSet.
 * @return  return `nil` when initialize SigLightHSLSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.3.3 Light HSL Set Unacknowledged, opcode:0x8277


/// The Light HSL Set Unacknowledged is an unacknowledged message
/// used to set the Light HSL Lightness (see Section 6.1.4.5) state, Light
/// HSL Hue state (see Section 6.1.4.1), and the Light HSL Saturation
/// state (see Section 6.1.4.2) of an element.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.214),
/// 6.3.3.3 Light HSL Set Unacknowledged.
@interface SigLightHSLSetUnacknowledged : SigGenericMessage
/// The target value of the Light HSL Lightness state.
/// The HSL Lightness field identifies the Light HSL Lightness state
/// of the element.
@property (nonatomic,assign) UInt16 HSLLightness;
/// The target value of the Light HSL Hue state.
/// The HSL Hue field identifies the Light HSL Hue state of the element.
@property (nonatomic,assign) UInt16 HSLHue;
/// The target value of the Light HSL Saturation state.
/// The HSL Saturation field identifies the Light HSL Saturation state
/// of the element.
@property (nonatomic,assign) UInt16 HSLSaturation;
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Transition Time field identifies the time an element will
/// take to transition to the target state from the present state (see
/// Section 1.4.1.1). The format of the Transition Time field matches the
/// format of the Generic Default Transition Time state, as defined in
/// Section 3.1.3. Only values of 0x00 to 0x3E shall be used to specify
/// the Transition Number of Steps.
@property (nonatomic,strong,nullable) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps (C.1)
/// C.1: If the Transition Time field is present, the Delay field shall also
/// be present; otherwise these fields shall not be present.
@property (nonatomic,assign) UInt8 delay;

- (instancetype)initWithHSLLightness:(UInt16)HSLLightness HSLHue:(UInt16)HSLHue HSLSaturation:(UInt16)HSLSaturation transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay;

/**
 * @brief   Initialize SigLightHSLSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightHSLSetUnacknowledged.
 * @return  return `nil` when initialize SigLightHSLSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.3.4 Light HSL Status, opcode:0x8278


/// Light HSL Status is an unacknowledged message used to report
/// the Light HSL Lightness, Light HSL Hue, and Light HSL Saturation
/// states of an element (see Section 6.1.4.5, 6.1.4.1, and 6.1.4.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.215),
/// 6.3.3.4 Light HSL Status.
@interface SigLightHSLStatus : SigGenericMessage
/// The present value of the Light HSL Lightness state.
/// The HSL Lightness field identifies the present Light HSL Lightness
/// state of the element (see Section 6.1.4.5).
@property (nonatomic,assign) UInt16 HSLLightness;
/// The present value of the Light HSL Hue state.
/// The HSL Hue field identifies the present Light HSL Hue state of the
/// element (see Section 6.1.4.1).
@property (nonatomic,assign) UInt16 HSLHue;
/// The present value of the Light HSL Saturation state.
/// The HSL Saturation field identifies the present Light HSL Saturation
/// state of the element (see Section 6.1.4.2).
@property (nonatomic,assign) UInt16 HSLSaturation;
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Remaining Time field identifies the time it will take the
/// element to complete the transition to the target state of the element
/// (see Section 1.4.1.1).
@property (nonatomic,strong,nullable) SigTransitionTime *remainingTime;

- (instancetype)initWithHSLLightness:(UInt16)HSLLightness HSLHue:(UInt16)HSLHue HSLSaturation:(UInt16)HSLSaturation remainingTime:(SigTransitionTime * _Nullable )remainingTime;

/**
 * @brief   Initialize SigLightHSLStatus object.
 * @param   parameters    the hex data of SigLightHSLStatus.
 * @return  return `nil` when initialize SigLightHSLStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.3.5 Light HSL Target Get, opcode:0x8279


/// Light HSL Target Get is an acknowledged message used to get the
/// target Light HSL Lightness (see Section 6.1.4.5), Light HSL Hue (see
/// Section 6.1.4.1), and Light HSL Saturation (see Section 6.1.4.2) states
/// of an element.
/// @note   For example, it may be used when an element reports it is
/// in transition to target Light HSL Lightness (see Section 6.1.4.5), Light
/// HSL Hue (see Section 6.1.4.1), or Light HSL Saturation (see
/// Section 6.1.4.2) states by including a positive Remaining Time field in
/// the Light HSL Status message (see Section 6.3.3.4), the Light Lightness
/// Status message (see Section 6.3.1.4), or the Light xyL Status message
/// (see Section 6.3.4.4).
/// @note   The response to the Light HSL Target Get message is a
/// Light HSL Target Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.215),
/// 6.3.3.5 Light HSL Target Get.
@interface SigLightHSLTargetGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigLightHSLTargetGet object.
 * @param   parameters    the hex data of SigLightHSLTargetGet.
 * @return  return `nil` when initialize SigLightHSLTargetGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.3.6 Light HSL Target Status, opcode:0x827A


/// The Light HSL Target Status is an unacknowledged message used to
/// report the target Light HSL Lightness, Light HSL Hue, and Light HSL
/// Saturation states of an element (see Section 6.1.4.5, 6.1.4.1 and 6.1.4.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.215),
/// 6.3.3.6 Light HSL Target Status.
@interface SigLightHSLTargetStatus : SigGenericMessage
/// The target value of the Light HSL Lightness state.
/// The HSL Lightness Target field identifies the target Light HSL Lightness
/// state of the element (see Section 6.1.4.5).
@property (nonatomic,assign) UInt16 HSLLightnessTarget;
/// The target value of the Light HSL Hue state.
/// The HSL Hue Target field identifies the target Light HSL Hue state of the
/// element (see Section 6.1.4.1).
@property (nonatomic,assign) UInt16 HSLHueTarget;
/// The target Light HSL Saturation state.
/// The HSL Saturation Target field identifies the target Light HSL Saturation
/// state of the element (see Section 6.1.4.2).
@property (nonatomic,assign) UInt16 HSLSaturationTarget;
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Remaining Time field identifies the time it will take the element
/// to complete the transition to the target state of the element.
@property (nonatomic,strong,nullable) SigTransitionTime *remainingTime;

- (instancetype)initWithHSLLightnessTarget:(UInt16)HSLLightnessTarget HSLHueTarget:(UInt16)HSLHueTarget HSLSaturationTarget:(UInt16)HSLSaturationTarget remainingTime:(nullable SigTransitionTime *)remainingTime;

/**
 * @brief   Initialize SigLightHSLTargetStatus object.
 * @param   parameters    the hex data of SigLightHSLTargetStatus.
 * @return  return `nil` when initialize SigLightHSLTargetStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.3.15 Light HSL Default Get, opcode:0x827B


/// Light HSL Default Get is an acknowledged message used to get
/// the Light Lightness Default (see Section 6.1.2.4), the Light HSL
/// Hue Default (see Section 6.1.4.2), and Light HSL Saturation Default
/// (see Section 6.1.4.5) states of an element.
/// @note   The response to the Light HSL Default Get message is
/// a Light HSL Default Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.219),
/// 6.3.3.15 Light HSL Default Get.
@interface SigLightHSLDefaultGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigLightHSLDefaultGet object.
 * @param   parameters    the hex data of SigLightHSLDefaultGet.
 * @return  return `nil` when initialize SigLightHSLDefaultGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.3.18 Light HSL Default Status, opcode:0x827C


/// Light HSL Default Status is an unacknowledged message used to report
/// the Light Lightness Default (see Section 6.1.2.4), the Light HSL Hue Default
/// (see Section 6.1.4.2), and Light HSL Saturation Default (see Section 6.1.4.5)
/// states of an element.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.220),
/// 6.3.3.18 Light HSL Default Status.
@interface SigLightHSLDefaultStatus : SigGenericMessage
/// The value of the Light Lightness Default state.
/// The Lightness field identifies the Light Lightness Default state of the element.
@property (nonatomic,assign) UInt16 lightness;
/// The value of the Light HSL Hue Default state.
/// The Hue field identifies the Light HSL Hue Default state of the element.
@property (nonatomic,assign) UInt16 hue;
/// The value of the Light HSL Saturation Default state.
/// The Saturation field identifies the Light HSL Saturation Default state of the element.
@property (nonatomic,assign) UInt16 saturation;

- (instancetype)initWithLightness:(UInt16)lightness hue:(UInt16)hue saturation:(UInt16)saturation;

/**
 * @brief   Initialize SigLightHSLDefaultStatus object.
 * @param   parameters    the hex data of SigLightHSLDefaultStatus.
 * @return  return `nil` when initialize SigLightHSLDefaultStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.3.19 Light HSL Range Get, opcode:0x827D


/// The Light HSL Range Get is an acknowledged message used to get
/// the Light HSL Hue Range (see Section 6.1.4.3) and Light HSL Saturation
/// Range (see Section 6.1.4.6) states of an element.
/// @note   The response to the Light HSL Range Get message is a
/// Light HSL Range Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.221),
/// 6.3.3.19 Light HSL Range Get.
@interface SigLightHSLRangeGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigLightHSLRangeGet object.
 * @param   parameters    the hex data of SigLightHSLRangeGet.
 * @return  return `nil` when initialize SigLightHSLRangeGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.3.22 Light HSL Range Status, opcode:0x827E


/// Light HSL Range Status is an unacknowledged message used to report
/// the Light HSL Hue Range (see Section 6.1.4.3) and Light HSL Saturation
/// Range (see Section 6.1.4.6) states of an element.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.222),
/// 6.3.3.22 Light HSL Range Status.
@interface SigLightHSLRangeStatus : SigGenericMessage
/// Status Code for the requesting message.
/// The Status Code field identifies the Status Code for the last operation on
/// the Light HSL Hue Range and Light HSL Saturation Range states. The allowed
/// values for status codes and their meanings are documented in Section 7.2.
@property (nonatomic, assign) SigGenericMessageStatus statusCode;
/// The value of the Hue Range Min field of the Light HSL Hue Range state.
/// The Hue Range Min field identifies the Light HSL Hue Range Min field of
/// the Light HSL Hue Range state of the element (see Section 6.1.4.3).
@property (nonatomic,assign) UInt16 hueRangeMin;
/// The value of the Hue Range Max field of the Light HSL Hue Range state.
/// The Hue Range Max field identifies the Light HSL Hue Range Max field of
/// the Light HSL Hue Range state of the element (see Section 6.1.4.3).
@property (nonatomic,assign) UInt16 hueRangeMax;
/// The value of the Saturation Range Min field of the Light HSL Saturation
/// Range state.
/// The Saturation Range Min field identifies the Light HSL Saturation Range
/// Min field of the Light HSL Saturation Range state of the element (see
/// Section 6.1.4.6).
@property (nonatomic,assign) UInt16 saturationRangeMin;
/// The value of the Saturation Range Max field of the Light HSL Saturation
/// Range state.
/// The Saturation Range Max field identifies the Light HSL Saturation Range
/// Max field of the Light HSL Saturation state of the element (see Section 6.1.4.6).
@property (nonatomic,assign) UInt16 saturationRangeMax;

- (instancetype)initWithStatusCode:(SigGenericMessageStatus)statusCode hueRangeMin:(UInt16)hueRangeMin hueRangeMax:(UInt16)hueRangeMax saturationRangeMin:(UInt16)saturationRangeMin saturationRangeMax:(UInt16)saturationRangeMax;

/**
 * @brief   Initialize SigLightHSLRangeStatus object.
 * @param   parameters    the hex data of SigLightHSLRangeStatus.
 * @return  return `nil` when initialize SigLightHSLRangeStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.3.16 Light HSL Default Set, opcode:0x827F


/// Light HSL Default Set is an acknowledged message used to set
/// the Light Lightness Default (see Section 6.1.2.4), the Light HSL
/// Hue Default (see Section 6.1.4.2), and Light HSL Saturation
/// Default (see Section 6.1.4.5) states of an element.
/// @note   The response to the Light HSL Default Set message is
/// a Light HSL Default Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.219),
/// 6.3.3.16 Light HSL Default Set.
@interface SigLightHSLDefaultSet : SigAcknowledgedGenericMessage
/// The value of the Light Lightness Default state.
/// The Lightness field identifies the Light Lightness Default
/// state of the element.
@property (nonatomic,assign) UInt16 lightness;
/// The value of the Light HSL Hue Default state.
/// The Hue field identifies the Light HSL Hue Default state
/// of the element.
@property (nonatomic,assign) UInt16 hue;
/// The value of the Light HSL Saturation Default state.
/// The Saturation field identifies the Light HSL Saturation
/// Default state of the element.
@property (nonatomic,assign) UInt16 saturation;

- (instancetype)initWithLightness:(UInt16)lightness hue:(UInt16)hue saturation:(UInt16)saturation;

/**
 * @brief   Initialize SigLightHSLDefaultSet object.
 * @param   parameters    the hex data of SigLightHSLDefaultSet.
 * @return  return `nil` when initialize SigLightHSLDefaultSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.3.17 Light HSL Default Set Unacknowledged, opcode:0x8280


/// Light HSL Default Set Unacknowledged is an unacknowledged message used
/// to set the Light Lightness Default (see Section 6.1.2.4), the Light HSL Hue
/// Default (see Section 6.1.4.2), and Light HSL Saturation Default (see Section 6.1.4.5)
/// states of an element.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.220),
/// 6.3.3.17 Light HSL Default Set Unacknowledged.
@interface SigLightHSLDefaultSetUnacknowledged : SigGenericMessage
/// The value of the Light Lightness Default state.
/// The Lightness field identifies the Light Lightness Default
/// state of the element.
@property (nonatomic,assign) UInt16 lightness;
/// The value of the Light HSL Hue Default state.
/// The Hue field identifies the Light HSL Hue Default state
/// of the element.
@property (nonatomic,assign) UInt16 hue;
/// The value of the Light HSL Saturation Default state.
/// The Saturation field identifies the Light HSL Saturation
/// Default state of the element.
@property (nonatomic,assign) UInt16 saturation;

- (instancetype)initWithLightness:(UInt16)lightness hue:(UInt16)hue saturation:(UInt16)saturation;

/**
 * @brief   Initialize SigLightHSLDefaultSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightHSLDefaultSetUnacknowledged.
 * @return  return `nil` when initialize SigLightHSLDefaultSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.3.20 Light HSL Range Set, opcode:0x8281


/// Light HSL Range Set is an acknowledged message used to set the
/// Light HSL Hue Range (see Section 6.1.4.3) and Light HSL Saturation
/// Range (see Section 6.1.4.6) states of an element.
/// @note   The response to the Light HSL Range Set message is a
/// Light HSL Range Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.221),
/// 6.3.3.20 Light HSL Range Set.
@interface SigLightHSLRangeSet : SigAcknowledgedGenericMessage
/// The value of the Hue Range Min field of the Light HSL Hue Range state.
/// The Hue Range Min field identifies the Light HSL Hue Range Min field
/// of the Light HSL Hue Range state of the element (see Section 6.1.4.3).
@property (nonatomic,assign) UInt16 hueRangeMin;
/// The value of the Hue Range Max field of the Light HSL Hue Range state.
/// The Hue Range Max field identifies the Light HSL Hue Range Max field
/// of the Light HSL Hue Range state of the element (see Section 6.1.4.3).
/// The value of the Hue Range Max field shall be greater or equal to the
/// value of the Hue Range Min field.
@property (nonatomic,assign) UInt16 hueRangeMax;
/// The value of the Saturation Range Min field of the Light HSL Saturation
/// Range state.
/// The Saturation Range Min field identifies the Light HSL Saturation Range
/// Min field of the Light HSL Saturation Range state of the element (see Section 6.1.4.6).
@property (nonatomic,assign) UInt16 saturationRangeMin;
/// The value of the Saturation Range Max field of the Light HSL Saturation
/// Range state.
/// The Saturation Range Max field identifies the Light HSL Saturation Range
/// Max field of the Light HSL Saturation state of the element (see Section 6.1.4.6).
/// The value of the Saturation Range Max field shall be greater or equal to
/// the value of the Saturation Range Min field.
@property (nonatomic,assign) UInt16 saturationRangeMax;

- (instancetype)initWithHueRangeMin:(UInt16)hueRangeMin hueRangeMax:(UInt16)hueRangeMax saturationRangeMin:(UInt16)saturationRangeMin saturationRangeMax:(UInt16)saturationRangeMax;

/**
 * @brief   Initialize SigLightHSLRangeSet object.
 * @param   parameters    the hex data of SigLightHSLRangeSet.
 * @return  return `nil` when initialize SigLightHSLRangeSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.3.21 Light HSL Range Set Unacknowledged, opcode:0x82


/// Light HSL Range Set Unacknowledged is an unacknowledged message
/// used to set the Light HSL Hue Range (see Section 6.1.4.3) and Light HSL
/// Saturation Range (see Section 6.1.4.6) states of an element.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.221),
/// 6.3.3.21 Light HSL Range Set Unacknowledged.
@interface SigLightHSLRangeSetUnacknowledged : SigGenericMessage
/// The value of the Hue Range Min field of the Light HSL Hue Range state.
/// The Hue Range Min field identifies the Light HSL Hue Range Min field
/// of the Light HSL Hue Range state of the element (see Section 6.1.4.3).
@property (nonatomic,assign) UInt16 hueRangeMin;
/// The value of the Hue Range Max field of the Light HSL Hue Range state.
/// The Hue Range Max field identifies the Light HSL Hue Range Max field
/// of the Light HSL Hue Range state of the element (see Section 6.1.4.3).
/// The value of the Hue Range Max field shall be greater or equal to the
/// value of the Hue Range Min field.
@property (nonatomic,assign) UInt16 hueRangeMax;
/// The value of the Saturation Range Min field of the Light HSL Saturation
/// Range state.
/// The Saturation Range Min field identifies the Light HSL Saturation Range
/// Min field of the Light HSL Saturation Range state of the element (see Section 6.1.4.6).
@property (nonatomic,assign) UInt16 saturationRangeMin;
/// The value of the Saturation Range Max field of the Light HSL Saturation
/// Range state.
/// The Saturation Range Max field identifies the Light HSL Saturation Range
/// Max field of the Light HSL Saturation state of the element (see Section 6.1.4.6).
/// The value of the Saturation Range Max field shall be greater or equal to
/// the value of the Saturation Range Min field.
@property (nonatomic,assign) UInt16 saturationRangeMax;

- (instancetype)initWithHueRangeMin:(UInt16)hueRangeMin hueRangeMax:(UInt16)hueRangeMax saturationRangeMin:(UInt16)saturationRangeMin saturationRangeMax:(UInt16)saturationRangeMax;

/**
 * @brief   Initialize SigLightHSLRangeSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightHSLRangeSetUnacknowledged.
 * @return  return `nil` when initialize SigLightHSLRangeSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark - 6.3.4 Light xyL messages


#pragma mark 6.3.4.1 Light xyL Get, opcode:0x8283


/// The Light xyL Get is an acknowledged message used to get
/// the Light xyL Lightness (see Section 6.1.5.5), Light xyL x (see
/// Section 6.1.5.1), and Light xyL y (see Section 6.1.5.2) states
/// of an element.
/// @note   Upon receiving a Light xyL Get message, the element
/// shall respond with a Light xyL Status message.
/// @note   The response to the Light xyL Get message is a Light
/// xyL Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.223),
/// 6.3.4.1 Light xyL Get.
@interface SigLightXyLGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigLightXyLGet object.
 * @param   parameters    the hex data of SigLightXyLGet.
 * @return  return `nil` when initialize SigLightXyLGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.4.2 Light xyL Set, opcode:0x8284


/// The Light xyL Set is an acknowledged message used to set the Light
/// xyL Lightness (see Section 6.1.5.5), Light xyL x state (see Section 6.1.5.1),
/// and the Light xyL y states (see Section 6.1.5.2) of an element.
/// @note   The response to the Light xyL Set message is a Light xyL
/// Status message. The structure of the message is defined in the following table.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.223),
/// 6.3.4.2 Light xyL Set.
@interface SigLightXyLSet : SigAcknowledgedGenericMessage
/// The target value of the Light xyL Lightness state.
/// The xyL Lightness field identifies the Light xyL Lightness state of the element.
@property (nonatomic,assign) UInt16 xyLLightness;
/// The target value of the Light xyL x state.
/// The xyL x field identifies the Light xyL x state of the element.
@property (nonatomic,assign) UInt16 xyLX;
/// The target value of the Light xyL y state.
/// The xyL y field identifies the Light xyL y state of the element.
@property (nonatomic,assign) UInt16 xyLY;
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Transition Time field identifies the time an element will take to
/// transition to the target state from the present state (see Section 1.4.1.1). The
/// format of the Transition Time field matches the format of the Generic Default
/// Transition Time state, as defined in Section 3.1.3. Only values of 0x00 to 0x3E
/// shall be used to specify the Transition Number of Steps.
@property (nonatomic,strong,nullable) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps (C.1)
/// C.1: If the Transition Time field is present, the Delay field shall also be present;
/// otherwise these fields shall not be present.
/// The Delay field shall be present when the Transition Time field is present. It identifies
/// the message execution delay, representing a time interval between receiving the
/// message by a model and executing the associated model behaviors.
@property (nonatomic,assign) UInt8 delay;

- (instancetype)initWithXyLLightness:(UInt16)xyLLightness xyLX:(UInt16)xyLX xyLY:(UInt16)xyLY transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay;

/**
 * @brief   Initialize SigLightXyLSet object.
 * @param   parameters    the hex data of SigLightXyLSet.
 * @return  return `nil` when initialize SigLightXyLSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.4.3 Light xyL Set Unacknowledged, opcode:0x8285


/// The Light xyL Set Unacknowledged is an unacknowledged message used to
/// set the Light xyL Lightness (see Section 6.1.5.5), Light xyL x (see Section 6.1.5.1),
/// and the Light xyL y states (see Section 6.1.5.2) of an element.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.224),
/// 6.3.4.3 Light xyL Set Unacknowledged.
@interface SigLightXyLSetUnacknowledged : SigGenericMessage
/// The target value of the Light xyL Lightness state.
/// The xyL Lightness field identifies the Light xyL Lightness state of the element.
@property (nonatomic,assign) UInt16 xyLLightness;
/// The target value of the Light xyL x state.
/// The xyL x field identifies the Light xyL x state of the element.
@property (nonatomic,assign) UInt16 xyLX;
/// The target value of the Light xyL y state.
/// The xyL y field identifies the Light xyL y state of the element.
@property (nonatomic,assign) UInt16 xyLY;
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Transition Time field identifies the time an element will take to
/// transition to the target state from the present state (see Section 1.4.1.1). The
/// format of the Transition Time field matches the format of the Generic Default
/// Transition Time state, as defined in Section 3.1.3. Only values of 0x00 to 0x3E
/// shall be used to specify the Transition Number of Steps.
@property (nonatomic,strong,nullable) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps (C.1)
/// C.1: If the Transition Time field is present, the Delay field shall also be present;
/// otherwise these fields shall not be present.
/// The Delay field shall be present when the Transition Time field is present. It identifies
/// the message execution delay, representing a time interval between receiving the
/// message by a model and executing the associated model behaviors.
@property (nonatomic,assign) UInt8 delay;

- (instancetype)initWithXyLLightness:(UInt16)xyLLightness xyLX:(UInt16)xyLX xyLY:(UInt16)xyLY transitionTime:(nullable SigTransitionTime *)transitionTime delay:(UInt8)delay;

/**
 * @brief   Initialize SigLightXyLSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightXyLSetUnacknowledged.
 * @return  return `nil` when initialize SigLightXyLSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.4.4 Light xyL Status, opcode:0x8286


/// The Light xyL Status is an unacknowledged message used to report
/// the Light xyL Lightness, Light xyL x, and Light xyL y states of an
/// element (see Section 6.1.5.5, 6.1.5.1 and 6.1.5.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.224),
/// 6.3.4.4 Light xyL Status.
@interface SigLightXyLStatus : SigGenericMessage
/// The present value of the Light xyL Lightness state.
/// The xyL Lightness field identifies the present Light xyL Lightness state
/// of the element (see Section 6.1.5.5).
@property (nonatomic,assign) UInt16 xyLLightness;
/// The present value of the Light xyL x state.
/// The xyL x field identifies the present Light xyL x state of the element
/// (see Section 6.1.5.1).
@property (nonatomic,assign) UInt16 xyLX;
/// The present value of the Light xyL y state.
/// The xyL y field identifies the present Light xyL y state of the element
/// (see Section 6.1.5.2).
@property (nonatomic,assign) UInt16 xyLY;
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Remaining Time field identifies the time it will take the
/// element to complete the transition to the target state of the element
/// (see Section 1.4.1.1).
@property (nonatomic,strong,nullable) SigTransitionTime *remainingTime;

- (instancetype)initWithXyLLightness:(UInt16)xyLLightness xyLX:(UInt16)xyLX xyLY:(UInt16)xyLY remainingTime:(nullable SigTransitionTime *)remainingTime;

/**
 * @brief   Initialize SigLightXyLStatus object.
 * @param   parameters    the hex data of SigLightXyLStatus.
 * @return  return `nil` when initialize SigLightXyLStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.4.5 Light xyL Target Get, opcode:0x8287


/// The Light xyL Target Get is an acknowledged message used to get the
/// target Light xyL Lightness (see Section 6.1.5.5), Light xyL x (see
/// Section 6.1.5.1), and Light xyL y (see Section 6.1.5.2) states of an element.
/// @note   For example, it may be used when an element reports it is in
/// transition to new Light xyL Lightness (see Section 6.1.5.5), Light xyL x
/// (see Section 6.1.5.1), or Light xyL y (see Section 6.1.5.2) states by including
/// a positive Remaining Time field in the Light xyL Status message (see
/// Section 6.3.4.4), Light Lightness Status message (see Section 6.3.1.4),
/// or the Light HSL Status message (see Section 6.3.3.4).
/// @note   The response to the Light xyL Target Get message is a Light
/// xyL Target Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.225),
/// 6.3.4.5 Light xyL Target Get.
@interface SigLightXyLTargetGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigLightXyLTargetGet object.
 * @param   parameters    the hex data of SigLightXyLTargetGet.
 * @return  return `nil` when initialize SigLightXyLTargetGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.4.6 Light xyL Target Status, opcode:0x8288


/// Light xyL Target Status is an unacknowledged message used to report
/// the target Light xyL Lightness, Light xyL x, and Light xyL y states of an
/// element (see Section 6.1.5.5, 6.1.5.1 and 6.1.5.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.225),
/// 6.3.4.6 Light xyL Target Status.
@interface SigLightXyLTargetStatus : SigGenericMessage
/// The target value of the Light xyL Lightness state.
/// The Target xyL Lightness field identifies the target Light xyL Lightness
/// state of the element (see Section 6.1.5.5).
@property (nonatomic,assign) UInt16 targetXyLLightness;
/// The target value of the Light xyL x state.
/// The Target xyL x field identifies the target Light xyL x state of the element
/// (see Section 6.1.5.1).
@property (nonatomic,assign) UInt16 targetXyLX;
/// The target value of the Light xyL y state.
/// The Target xyL y field identifies the target Light xyL y state of the element
/// (see Section 6.1.5.2).
@property (nonatomic,assign) UInt16 targetXyLY;
/// Format as defined in Section 3.1.3. (Optional)
/// The Remaining Time field identifies the time it will take the element to
/// complete the transition to the target state of the element.
@property (nonatomic,strong,nullable) SigTransitionTime *remainingTime;

- (instancetype)initWithTargetXyLLightness:(UInt16)targetXyLLightness targetXyLX:(UInt16)targetXyLX targetXyLY:(UInt16)targetXyLY remainingTime:(nullable SigTransitionTime *)remainingTime;

/**
 * @brief   Initialize SigLightXyLTargetStatus object.
 * @param   parameters    the hex data of SigLightXyLTargetStatus.
 * @return  return `nil` when initialize SigLightXyLTargetStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.4.7 Light xyL Default Get, opcode:0x8289


/// Light xyL Default Get is an acknowledged message used to get the
/// Light Lightness Default (see Section 6.1.2.4), the Light xyL x Default
/// (see Section 6.1.5.2), and Light xyL y Default (see Section 6.1.5.5)
/// states of an element.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.225),
/// 6.3.4.7 Light xyL Default Get.
@interface SigLightXyLDefaultGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigLightXyLDefaultGet object.
 * @param   parameters    the hex data of SigLightXyLDefaultGet.
 * @return  return `nil` when initialize SigLightXyLDefaultGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.4.10 Light xyL Default Status, opcode:0x828A


/// Light xyL Default Status is an unacknowledged message used to report
/// the Light Lightness Default (see Section 6.1.2.4), the Light xyL x Default
/// (see Section 6.1.5.2), and Light xyL y Default (see Section 6.1.5.5) states
/// of an element.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.226),
/// 6.3.4.10 Light xyL Default Status.
@interface SigLightXyLDefaultStatus : SigGenericMessage
/// The value of the Light xyL Lightness state.
/// The Lightness field identifies the Light Lightness Default state of the element.
@property (nonatomic,assign) UInt16 lightness;
/// The value of the Light xyL x state.
/// The xyL x field identifies the Light xyL x Default state of the element.
@property (nonatomic,assign) UInt16 xyLX;
/// The value of the Light xyL y state.
/// The xyL y field identifies the Light xyL y Default state of the element.
@property (nonatomic,assign) UInt16 xyLY;

- (instancetype)initWithXyLLightness:(UInt16)lightness xyLX:(UInt16)xyLX xyLY:(UInt16)xyLY;

/**
 * @brief   Initialize SigLightXyLDefaultStatus object.
 * @param   parameters    the hex data of SigLightXyLDefaultStatus.
 * @return  return `nil` when initialize SigLightXyLDefaultStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.4.11 Light xyL Range Get, opcode:0x828B


/// The Light xyL Range Get is an acknowledged message used to get
/// the Light xyL x Range (see Section 6.1.5.3) and Light xyL y Range
/// (see Section 6.1.5.6) states of an element.
/// @note   The response to the Light xyL Range Get message is a
/// Light xyL Range Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.227),
/// 6.3.4.11 Light xyL Range Get.
@interface SigLightXyLRangeGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigLightXyLRangeGet object.
 * @param   parameters    the hex data of SigLightXyLRangeGet.
 * @return  return `nil` when initialize SigLightXyLRangeGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.4.14 Light xyL Range Status, opcode:0x828C


/// Light xyL Range Status is an unacknowledged message used to report
/// the Light xyL x Range (see Section 6.1.5.3) and Light xyL y Range (see
/// Section 6.1.5.6) states of an element.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.228),
/// 6.3.4.14 Light xyL Range Status.
@interface SigLightXyLRangeStatus : SigGenericMessage
/// Status Code for the requesting message.
/// The Status Code field identifies the Status Code for the last operation
/// on the Light xyL x Range and Light xyL y Range states. The allowed
/// values for status codes and their meanings are documented in Section 7.2.
@property (nonatomic, assign) SigGenericMessageStatus statusCode;
/// The value of the xyL x Range Min field of the Light xyL x Range state.
/// The xyL x Range Min field identifies the Light xyL x Range Min field of
/// the Light xyL x Range state of the element (see Section 6.1.5.3).
@property (nonatomic,assign) UInt16 xyLXRangeMin;
/// The value of the xyL x Range Max field of the Light xyL x Range state.
/// The xyL x Range Max field identifies the Light xyL x Range Max field of
/// the Light xyL x Range state of the element (see Section 6.1.5.3).
@property (nonatomic,assign) UInt16 xyLXRangeMax;
/// The value of the xyL y Range Min field of the Light xyL y Range state.
/// The xyL y Range Min field identifies the Light xyL y Range Min field of
/// the Light xyL y Range state of the element (see Section 6.1.5.6).
@property (nonatomic,assign) UInt16 xyLYRangeMin;
/// The value of the xyL y Range Max field of the Light xyL y Range state.
/// The xyL y Range Max field identifies the Light xyL y Range Max field of
/// the Light xyL y Range state of the element (see Section 6.1.4.6).
@property (nonatomic,assign) UInt16 xyLYRangeMax;

- (instancetype)initWithStatusCode:(SigGenericMessageStatus)statusCode xyLXRangeMin:(UInt16)xyLXRangeMin xyLXRangeMax:(UInt16)xyLXRangeMax xyLYRangeMin:(UInt16)xyLYRangeMin xyLYRangeMax:(UInt16)xyLYRangeMax;

/**
 * @brief   Initialize SigLightXyLRangeStatus object.
 * @param   parameters    the hex data of SigLightXyLRangeStatus.
 * @return  return `nil` when initialize SigLightXyLRangeStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.4.8 Light xyL Default Set, opcode:0x828D


/// Light xyL Default Set is an acknowledged message used to set
/// the Light Lightness Default (see Section 6.1.2.4), the Light xyL x
/// Default (see Section 6.1.5.2), and Light xyL y Default (see
/// Section 6.1.5.5) states of an element.
/// @note   The response to the Light xyL Default Set message is
/// a Light xyL Default Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.226),
/// 6.3.4.8 Light xyL Default Set.
@interface SigLightXyLDefaultSet : SigAcknowledgedGenericMessage
/// The value of the Light Lightness Default state.
/// The Lightness field identifies the Light Lightness Default
/// state of the element.
@property (nonatomic,assign) UInt16 lightness;
/// The value of the Light xyL x Default state.
/// The xyL x field identifies the Light xyL x Default state of
/// the element.
@property (nonatomic,assign) UInt16 xyLX;
/// The value of the Light xyL y Default state.
/// The xyL y field identifies the Light xyL y Default state of
/// the element.
@property (nonatomic,assign) UInt16 xyLY;

- (instancetype)initWithLightness:(UInt16)lightness xyLX:(UInt16)xyLX xyLY:(UInt16)xyLY;

/**
 * @brief   Initialize SigLightXyLDefaultSet object.
 * @param   parameters    the hex data of SigLightXyLDefaultSet.
 * @return  return `nil` when initialize SigLightXyLDefaultSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.4.9 Light xyL Default Set Unacknowledged, opcode:0x828E


/// Light xyL Default Set Unacknowledged is an unacknowledged message
/// used to set the Light Lightness Default (see Section 6.1.2.4), the Light
/// xyL x Default (see Section 6.1.5.2), and Light xyL y Default (see
/// Section 6.1.5.5) states of an element.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.226),
/// 6.3.4.9 Light xyL Default Set Unacknowledged.
@interface SigLightXyLDefaultSetUnacknowledged : SigGenericMessage
/// The value of the Light Lightness Default state.
/// The Lightness field identifies the Light Lightness Default
/// state of the element.
@property (nonatomic,assign) UInt16 lightness;
/// The value of the Light xyL x Default state.
/// The xyL x field identifies the Light xyL x Default state of
/// the element.
@property (nonatomic,assign) UInt16 xyLX;
/// The value of the Light xyL y Default state.
/// The xyL y field identifies the Light xyL y Default state of
/// the element.
@property (nonatomic,assign) UInt16 xyLY;

- (instancetype)initWithLightness:(UInt16)lightness xyLX:(UInt16)xyLX xyLY:(UInt16)xyLY;

/**
 * @brief   Initialize SigLightXyLDefaultSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightXyLDefaultSetUnacknowledged.
 * @return  return `nil` when initialize SigLightXyLDefaultSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.4.12 Light xyL Range Set, opcode:0x828F


/// Light xyL Range Set is an acknowledged message used to set
/// the Light xyL x Range (see Section 6.1.5.3) and Light xyL y
/// Range (see Section 6.1.5.6) states of an element.
/// @note   The response to the Light xyL Range Set message is
/// a Light xyL Range Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.227),
/// 6.3.4.12 Light xyL Range Set.
@interface SigLightXyLRangeSet : SigAcknowledgedGenericMessage
/// The value of the xyL x Range Min field of the Light xyL x Range state.
/// The xyL x Range Min field identifies the Light xyL x Range Min
/// field of the Light xyL x Range state of the element (see Section 6.1.5.3).
@property (nonatomic,assign) UInt16 xyLXRangeMin;
/// The value of the xyL x Range Max field of the Light xyL x Range state.
/// The xyL x Range Max field identifies the Light xyL x Range Max
/// field of the Light xyL x Range state of the element (see Section 6.1.5.3).
/// The value of the xyL x Range Max field shall be greater or
/// equal to the value of the xyL x Range Min field.
@property (nonatomic,assign) UInt16 xyLXRangeMax;
/// The value of the xyL y Range Min field of the Light xyL y Range state.
/// The xyL y Range Min field identifies the Light xyL y Range Min field of
/// the Light xyL y Range state of the element (see Section 6.1.5.6).
@property (nonatomic,assign) UInt16 xyLYRangeMin;
/// The value of the xyL y Range Max field of the Light xyL y Range state.
/// The xyL y Range Max field identifies the Light xyL y Range Max field of
/// the Light xyL y Range state of the element (see Section 6.1.4.6).
/// The value of the xyL y Range Max field shall be greater or equal to the
/// value of the xyL y Range Min field.
@property (nonatomic,assign) UInt16 xyLYRangeMax;

- (instancetype)initWithXyLXRangeMin:(UInt16)xyLXRangeMin xyLXRangeMax:(UInt16)xyLXRangeMax xyLYRangeMin:(UInt16)xyLYRangeMin xyLYRangeMax:(UInt16)xyLYRangeMax;

/**
 * @brief   Initialize SigLightXyLRangeSet object.
 * @param   parameters    the hex data of SigLightXyLRangeSet.
 * @return  return `nil` when initialize SigLightXyLRangeSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.4.13 Light xyL Range Set Unacknowledged, opcode:0x8290


/// Light xyL Range Set Unacknowledged is an unacknowledged message
/// used to set the Light xyL x Range (see Section 6.1.5.3) and Light xyL y
/// Range (see Section 6.1.5.6) states of an element.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.227),
/// 6.3.4.13 Light xyL Range Set Unacknowledged.
@interface SigLightXyLRangeSetUnacknowledged : SigGenericMessage
/// The value of the xyL x Range Min field of the Light xyL x Range state.
/// The xyL x Range Min field identifies the Light xyL x Range Min
/// field of the Light xyL x Range state of the element (see Section 6.1.5.3).
@property (nonatomic,assign) UInt16 xyLXRangeMin;
/// The value of the xyL x Range Max field of the Light xyL x Range state.
/// The xyL x Range Max field identifies the Light xyL x Range Max
/// field of the Light xyL x Range state of the element (see Section 6.1.5.3).
/// The value of the xyL x Range Max field shall be greater or
/// equal to the value of the xyL x Range Min field.
@property (nonatomic,assign) UInt16 xyLXRangeMax;
/// The value of the xyL y Range Min field of the Light xyL y Range state.
/// The xyL y Range Min field identifies the Light xyL y Range Min field of
/// the Light xyL y Range state of the element (see Section 6.1.5.6).
@property (nonatomic,assign) UInt16 xyLYRangeMin;
/// The value of the xyL y Range Max field of the Light xyL y Range state.
/// The xyL y Range Max field identifies the Light xyL y Range Max field of
/// the Light xyL y Range state of the element (see Section 6.1.4.6).
/// The value of the xyL y Range Max field shall be greater or equal to the
/// value of the xyL y Range Min field.
@property (nonatomic,assign) UInt16 xyLYRangeMax;

- (instancetype)initWithXyLXRangeMin:(UInt16)xyLXRangeMin xyLXRangeMax:(UInt16)xyLXRangeMax xyLYRangeMin:(UInt16)xyLYRangeMin xyLYRangeMax:(UInt16)xyLYRangeMax;

/**
 * @brief   Initialize SigLightXyLRangeSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightXyLRangeSetUnacknowledged.
 * @return  return `nil` when initialize SigLightXyLRangeSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark - 6.3.5 Light LC messages - 6.3.5.1 Light LC Mode messages


#pragma mark 6.3.5.1.1 Light LC Mode Get, opcode:0x8291


/// Light LC Mode Get is an acknowledged message used to get
/// the Light LC Mode state of an element (see Section 6.2.3.1).
/// @note   The response to the Light LC Mode Get message is
/// a Light LC Mode Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.229),
/// 6.3.5.1.1 Light LC Mode Get.
@interface SigLightLCModeGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigLightLCModeGet object.
 * @param   parameters    the hex data of SigLightLCModeGet.
 * @return  return `nil` when initialize SigLightLCModeGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.5.1.2 Light LC Mode Set, opcode:0x8292


/// The Light LC Mode Set is an acknowledged message used to set
/// the Light LC Mode state of an element (see Section 6.2.3.1).
/// @note   The response to the Light LC Mode Set message is a
/// Light LC Mode Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.229),
/// 6.3.5.1.2 Light LC Mode Set.
@interface SigLightLCModeSet : SigAcknowledgedGenericMessage
/// The target value of the Light LC Mode state.
/// The Mode field identifies the Light LC Mode state of the
/// element (see Section 6.2.3.1).
@property (nonatomic,assign) UInt8 mode;

- (instancetype)initWithMode:(UInt8)mode;

/**
 * @brief   Initialize SigLightLCModeSet object.
 * @param   parameters    the hex data of SigLightLCModeSet.
 * @return  return `nil` when initialize SigLightLCModeSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.5.1.3 Light LC Mode Set Unacknowledged, opcode:0x8293


/// The Light LC Mode Set Unacknowledged is an unacknowledged message
/// used to set the Light LC Mode state of an element (see Section 6.2.3.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.229),
/// 6.3.5.1.3 Light LC Mode Set Unacknowledged.
@interface SigLightLCModeSetUnacknowledged : SigGenericMessage
/// The target value of the Light LC Mode state.
/// The Mode field identifies the Light LC Mode state of the
/// element (see Section 6.2.3.1).
@property (nonatomic,assign) UInt8 mode;

- (instancetype)initWithMode:(UInt8)mode;

/**
 * @brief   Initialize SigLightLCModeSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightLCModeSetUnacknowledged.
 * @return  return `nil` when initialize SigLightLCModeSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.5.1.4 Light LC Mode Status, opcode:0x8294


/// The Light LC Mode Status is an unacknowledged message used to report
/// the Light LC Mode state of an element (see Section 6.2.3.1).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.229),
/// 6.3.5.1.4 Light LC Mode Status.
@interface SigLightLCModeStatus : SigGenericMessage
/// The target value of the Light LC Mode state.
/// The Mode field identifies the Light LC Mode state of the
/// element (see Section 6.2.3.1).
@property (nonatomic,assign) UInt8 mode;

- (instancetype)initWithMode:(UInt8)mode;

/**
 * @brief   Initialize SigLightLCModeStatus object.
 * @param   parameters    the hex data of SigLightLCModeStatus.
 * @return  return `nil` when initialize SigLightLCModeStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark - 6.3.5 Light LC messages - 6.3.5.2 Light LC Occupancy Mode messages


#pragma mark 6.3.5.2.1 Light LC OM Get, opcode:0x8295


/// Light LC OM Get is an acknowledged message used to get
/// the Light LC Occupancy Mode state of an element (see
/// Section 6.2.3.2).
/// @note   The response to the Light LC OM Get message is
/// a Light LC OM Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.230),
/// 6.3.5.2.1 Light LC OM Get.
@interface SigLightLCOMGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigLightLCOMGet object.
 * @param   parameters    the hex data of SigLightLCOMGet.
 * @return  return `nil` when initialize SigLightLCOMGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.5.2.2 Light LC OM Set, opcode:0x8296


/// The Light LC OM Set is an acknowledged message used to set
/// the Light LC Occupancy Mode state of an element (see
/// Section 6.2.3.2).
/// @note   The response to the Light LC OM Set message is a
/// Light LC OM Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.230),
/// 6.3.5.2.2 Light LC OM Set.
@interface SigLightLCOMSet : SigAcknowledgedGenericMessage
/// The target value of the Light LC Occupancy Mode state.
/// The Mode field identifies the Light LC Occupancy Mode state of
/// the element (see Section 6.2.3.2).
@property (nonatomic,assign) UInt8 mode;

- (instancetype)initWithMode:(UInt8)mode;

/**
 * @brief   Initialize SigLightLCOMSet object.
 * @param   parameters    the hex data of SigLightLCOMSet.
 * @return  return `nil` when initialize SigLightLCOMSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.5.2.3 Light LC OM Set Unacknowledged, opcode:0x8297


/// The Light LC OM Set Unacknowledged is an unacknowledged
/// message used to set the Light LC Occupancy Mode state of an
/// element (see Section 6.2.3.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.230),
/// 6.3.5.2.3 Light LC OM Set Unacknowledged.
@interface SigLightLCOMSetUnacknowledged : SigGenericMessage
/// The target value of the Light LC Occupancy Mode state.
/// The Mode field identifies the Light LC Occupancy Mode state of
/// the element (see Section 6.2.3.2).
@property (nonatomic,assign) UInt8 mode;

- (instancetype)initWithMode:(UInt8)mode;

/**
 * @brief   Initialize SigLightLCOMSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightLCOMSetUnacknowledged.
 * @return  return `nil` when initialize SigLightLCOMSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.5.2.4 Light LC OM Status, opcode:0x8298


/// The Light LC OM Status is an unacknowledged message used to report
/// the Light LC Occupancy Mode state of an element (see Section 6.2.3.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.230),
/// 6.3.5.2.4 Light LC OM Status.
@interface SigLightLCOMStatus : SigGenericMessage
/// The present value of the Light LC Occupancy Mode state.
/// The Mode field identifies the present Light LC Occupancy Mode state of
/// the element (see Section 6.2.3.2).
@property (nonatomic,assign) UInt8 mode;

- (instancetype)initWithMode:(UInt8)mode;

/**
 * @brief   Initialize SigLightLCOMStatus object.
 * @param   parameters    the hex data of SigLightLCOMStatus.
 * @return  return `nil` when initialize SigLightLCOMStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark - 6.3.5 Light LC messages - 6.3.5.3 Light LC Light OnOff messages


#pragma mark 6.3.5.3.1 Light LC Light OnOff Get, opcode:0x8299


/// Light LC Light OnOff Get is an acknowledged message used to get
/// the Light LC Light OnOff state of an element (see Section 6.2.3.3).
/// @note   The response to the Light LC Light OnOff Get message is
/// a Light LC Light OnOff Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.231),
/// 6.3.5.3.1 Light LC Light OnOff Get.
@interface SigLightLCLightOnOffGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigLightLCLightOnOffGet object.
 * @param   parameters    the hex data of SigLightLCLightOnOffGet.
 * @return  return `nil` when initialize SigLightLCLightOnOffGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.5.3.2 Light LC Light OnOff Set, opcode:0x829A


/// The Light LC Light OnOff Set is an acknowledged message used to set
/// the Light LC Light OnOff state of an element (see Section 6.2.3.3).
/// @note   The response to the Light LC Light OnOff Set message is a
/// Light LC Light OnOff Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.231),
/// 6.3.5.3.2 Light LC Light OnOff Set.
@interface SigLightLCLightOnOffSet : SigAcknowledgedGenericMessage
/// The target value of the Light LC Light OnOff state.
/// The Light OnOff field identifies the Light LC Light OnOff state of the
/// element (see Section 6.2.3.3).
@property (nonatomic,assign) BOOL lightOnOff;
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Transition Time field identifies the time an element will
/// take to transition to the target state from the present state (see
/// Section 1.4.1.1). The format of the Transition Time field matches the
/// format of the Generic Default Transition Time state as defined in
/// Section 3.1.3. Only values of 0x00 to 0x3E shall be used to specify
/// the Transition Number of Steps.
@property (nonatomic,strong,nullable) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps (C.1)
/// C.1: If the Transition Time field is present, the Delay field shall also be
/// present; otherwise these fields shall not be present.
/// The Delay field shall be present when the Transition Time field is present.
/// It identifies the message execution delay, representing a time interval
/// between receiving the message by a model and executing the associated
/// model behaviors.
@property (nonatomic,assign) UInt8 delay;

- (instancetype)initWithLightOnOff:(BOOL)lightOnOff transitionTime:(nullable SigTransitionTime *)transitionTime dalay:(UInt8)delay;

/**
 * @brief   Initialize SigLightLCLightOnOffSet object.
 * @param   parameters    the hex data of SigLightLCLightOnOffSet.
 * @return  return `nil` when initialize SigLightLCLightOnOffSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.5.3.3 Light LC Light OnOff Set Unacknowledged, opcode:0x829B


/// The Light LC Light OnOff Set Unacknowledged is an unacknowledged
/// message used to set the Light LC Light OnOff state of an element (see
/// Section 6.2.3.3).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.231),
/// 6.3.5.3.3 Light LC Light OnOff Set Unacknowledged.
@interface SigLightLCLightOnOffSetUnacknowledged : SigGenericMessage
/// The target value of the Light LC Light OnOff state.
/// The Light OnOff field identifies the Light LC Light OnOff state of the
/// element (see Section 6.2.3.3).
@property (nonatomic,assign) BOOL lightOnOff;
/// Format as defined in Section 3.1.3. (Optional)
/// If present, the Transition Time field identifies the time an element will
/// take to transition to the target state from the present state (see
/// Section 1.4.1.1). The format of the Transition Time field matches the
/// format of the Generic Default Transition Time state as defined in
/// Section 3.1.3. Only values of 0x00 to 0x3E shall be used to specify
/// the Transition Number of Steps.
@property (nonatomic,strong,nullable) SigTransitionTime *transitionTime;
/// Message execution delay in 5 millisecond steps (C.1)
/// C.1: If the Transition Time field is present, the Delay field shall also be
/// present; otherwise these fields shall not be present.
/// The Delay field shall be present when the Transition Time field is present.
/// It identifies the message execution delay, representing a time interval
/// between receiving the message by a model and executing the associated
/// model behaviors.
@property (nonatomic,assign) UInt8 delay;

- (instancetype)initWithLightOnOff:(BOOL)lightOnOff transitionTime:(nullable SigTransitionTime *)transitionTime dalay:(UInt8)delay;

/**
 * @brief   Initialize SigLightLCLightOnOffSetUnacknowledged object.
 * @param   parameters    the hex data of SigLightLCLightOnOffSetUnacknowledged.
 * @return  return `nil` when initialize SigLightLCLightOnOffSetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.5.3.4 Light LC Light OnOff Status, opcode:0x829C


/// The Light LC Light OnOff Status is an unacknowledged message used to report
/// the Light LC Light OnOff state of an element (see Section 6.2.3.3).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.232),
/// 6.3.5.3.4 Light LC Light OnOff Status.
@interface SigLightLCLightOnOffStatus : SigGenericMessage
/// The present value of the Light LC Light OnOff state.
/// The Present Light OnOff field identifies the present Light LC Light OnOff state
/// of the element (see Section 6.2.3.3).
@property (nonatomic,assign) BOOL presentLightOnOff;
/// The target value of the Light LC Light OnOff state (Optional)
/// If present, the Target Light OnOff field identifies the target Light LC Light OnOff
/// state that the element is to reach (see Section 6.2.3.3).
@property (nonatomic,assign) BOOL targetLightOnOff;
/// Format as defined in Section 3.1.3. (C.1)
/// C.1: If the Target Light OnOff field is present, the Remaining Time field shall also
/// be present; otherwise these fields shall not be present.
/// If present, the Remaining Time field identifies the time it will take the element to
/// complete the transition to the target Light LC Light OnOff state of the node (see
/// Section 1.4.1.1 and Section 6.2.3.3).
@property (nonatomic,strong,nullable) SigTransitionTime *remainingTime;

- (instancetype)initWithPresentLightOnOff:(BOOL)presentLightOnOff targetLightOnOff:(BOOL)targetLightOnOff remainingTime:(nullable SigTransitionTime *)remainingTime;

/**
 * @brief   Initialize SigLightLCLightOnOffStatus object.
 * @param   parameters    the hex data of SigLightLCLightOnOffStatus.
 * @return  return `nil` when initialize SigLightLCLightOnOffStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark - 6.3.6 Light LC Property Messages
/*
 Light LC Property messages operate on Light LC states defined
 in Section 6.2.3 by referring to device properties defined by
 these states and providing Raw values for the device properties.
 */

#pragma mark 6.3.6.1 Light LC Property Get, opcode:0x829D


/// Light LC Property Get is an acknowledged message used to get
/// the Light LC Property state of an element (see Section 6.2).
/// @note   The response to the Light LC Property Get message
/// is a Light LC Property Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.232),
/// 6.3.6.1 Light LC Property Get.
@interface SigLightLCPropertyGet : SigAcknowledgedGenericMessage
/// Property ID identifying a Light LC Property.
/// The Light LC Property ID field identifies a Light LC Property ID
/// state of an element (see Section 6.2).
@property (nonatomic,assign) UInt16 lightLCPropertyID;

- (instancetype)initWithLightLCPropertyID:(UInt16)lightLCPropertyID;

/**
 * @brief   Initialize SigLightLCPropertyGet object.
 * @param   parameters    the hex data of SigLightLCPropertyGet.
 * @return  return `nil` when initialize SigLightLCPropertyGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.6.2 Light LC Property Set, opcode:0x62


/// The Light LC Property Set is an acknowledged message used to set
/// the Light LC Property state of an element (see Section 6.2).
/// @note   The response to the Light LC Property Set message is a
/// Light LC Property Status message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.233),
/// 6.3.6.2 Light LC Property Set.
@interface SigLightLCPropertySet : SigAcknowledgedGenericMessage
/// Property ID identifying a Light LC Property.
/// The Light LC Property ID field identifies a Light LC Property ID state
/// of an element (see Section 6.2).
@property (nonatomic,assign) UInt16 lightLCPropertyID;
/// Raw value for the Light LC Property.
/// The Light LC Property Value field identifies a Light LC Property Value
/// state of an element (see Section 6.2).
@property (nonatomic,strong) NSData *lightLCPropertyValue;

- (instancetype)initWithLightLCPropertyID:(UInt16)lightLCPropertyID lightLCPropertyValue:(NSData *)lightLCPropertyValue;

/**
 * @brief   Initialize SigLightLCPropertySet object.
 * @param   parameters    the hex data of SigLightLCPropertySet.
 * @return  return `nil` when initialize SigLightLCPropertySet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 6.3.6.3 Light LC Property Set Unacknowledged, opcode:0x63


/// The Light LC Property Set Unacknowledged is an unacknowledged
/// message used to set the Light LC Property state of an element (see
/// Section 6.2).
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.233),
/// 6.3.6.3 Light LC Property Set Unacknowledged.
@interface SigLightLCPropertySetUnacknowledged : SigGenericMessage
/// Property ID identifying a Light LC Property.
/// The Light LC Property ID field identifies a Light LC Property ID state
/// of an element (see Section 6.2).
@property (nonatomic,assign) UInt16 lightLCPropertyID;
/// Raw value for the Light LC Property.
/// The Light LC Property Value field identifies a Light LC Property Value
/// state of an element (see Section 6.2).
@property (nonatomic,strong) NSData *lightLCPropertyValue;

- (instancetype)initWithLightLCPropertyID:(UInt16)lightLCPropertyID lightLCPropertyValue:(NSData *)lightLCPropertyValue;

/**
 * @brief   Initialize SigLightLCPropertySetUnacknowledged object.
 * @param   parameters    the hex data of SigLightLCPropertySetUnacknowledged.
 * @return  return `nil` when initialize SigLightLCPropertySetUnacknowledged object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 6.3.6.4 Light LC Property Status, opcode:0x64


/// The Light LC Property Status is an unacknowledged message used to
/// report the Light LC Property state of an element (see Section 6.2).
/// @note   The message is sent as a response to the Light LC Property
/// Get and Light LC Property Set messages or may be sent as an
/// unsolicited message.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.233),
/// 6.3.6.4 Light LC Property Status.
@interface SigLightLCPropertyStatus : SigGenericMessage
/// Property ID identifying a Light LC Property.
/// The Light LC Property ID field identifies a Light LC Property ID state of
/// an element (see Section 6.2).
@property (nonatomic,assign) UInt16 lightLCPropertyID;
/// Raw value for the Light LC Property.
/// The Light LC Property Value field identifies a Light LC Property Value
/// state of an element (see Section 6.2).
@property (nonatomic,strong) NSData *lightLCPropertyValue;

- (instancetype)initWithLightLCPropertyID:(UInt16)lightLCPropertyID lightLCPropertyValue:(NSData *)lightLCPropertyValue;

/**
 * @brief   Initialize SigLightLCPropertyStatus object.
 * @param   parameters    the hex data of SigLightLCPropertyStatus.
 * @return  return `nil` when initialize SigLightLCPropertyStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark - 8.4.1 Firmware Update model messages
/*
 This section defines the format of messages exchanged
 between the Firmware Update Client and Firmware Update
 Server models.
 */


#pragma mark 8.4.1.1 Firmware Update Information Get, opcode:0xB601


/// The Firmware Update Information Get message is an acknowledged
/// message used to get information about the firmware images installed
/// on a node.
/// @note   The response to the Firmware Update Information Get
/// message is a Firmware Update Information Status message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.91),
/// 8.4.1.1 Firmware Update Information Get.
@interface SigFirmwareUpdateInformationGet : SigAcknowledgedGenericMessage
/// Index of the first requested entry from the Firmware Information List state.
/// The First Index field shall indicate the first entry on the Firmware Information
/// List state of the Firmware Update Server (see Section 8.3.1.1) to return in
/// the Firmware Update Information Status message.
@property (nonatomic,assign) UInt8 firstIndex;
/// Maximum number of entries that the server includes in a Firmware Update
/// Information Status message.
/// The Entries Limit field shall indicate the maximum number of Firmware
/// Information Entry fields to return in the Firmware Update Information
/// Status message.
@property (nonatomic,assign) UInt8 entriesLimit;

/**
 * @brief   Initialize SigFirmwareUpdateInformationGet object.
 * @param   parameters    the hex data of SigFirmwareUpdateInformationGet.
 * @return  return `nil` when initialize SigFirmwareUpdateInformationGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithFirstIndex:(UInt8)firstIndex entriesLimit:(UInt8)entriesLimit;

/// The Type of the response message.
- (Class)responseType;

/// The Op Code of the response message.
- (UInt32)responseOpCode;

@end


#pragma mark 8.4.1.2 Firmware Update Information Status, opcode:0xB602


/// The Firmware Update Information Status message is an unacknowledged
/// message used to report information about firmware images installed on a node.
/// @note   The Firmware Update Information Status message is sent in
/// response to a Firmware Update Information Get message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.92),
/// 8.4.1.2 Firmware Update Information Status.
@interface SigFirmwareUpdateInformationStatus : SigGenericMessage
/// The number of entries in the Firmware Information List state of the server.
/// The Firmware Information List Count field shall indicate the number of entries
/// in the Firmware Information List state.
@property (nonatomic,assign) UInt8 firmwareInformationListCount;
/// Index of the first requested entry from the Firmware Information List state.
/// The First Index field shall indicate the first entry on the Firmware Information List
/// state of the Firmware Update Server (see Section 8.3.1.1) that was returned.
@property (nonatomic,assign) UInt8 firstIndex;
/// List of entries
/// The Firmware Information List field shall contain the selected entries from the
/// Firmware Information List state. The structure of the Firmware Information List
/// field is defined in Table 8.27.
@property (nonatomic,strong) NSMutableArray <SigFirmwareInformationEntryModel *>*firmwareInformationList;

/**
 * @brief   Initialize SigFirmwareUpdateInformationStatus object.
 * @param   parameters    the hex data of SigFirmwareUpdateInformationStatus.
 * @return  return `nil` when initialize SigFirmwareUpdateInformationStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 8.4.1.3 Firmware Update Firmware Metadata Check, opcode:0xB603


/// The Firmware Update Firmware Metadata Check message is an acknowledged
/// message, sent to a Firmware Update Server, to check whether the node can
/// accept a firmware update.
/// @note   The response to the Firmware Update Firmware Metadata Check
/// message is a Firmware Update Firmware Metadata Status message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.93),
/// 8.4.1.3 Firmware Update Firmware Metadata Check.
@interface SigFirmwareUpdateFirmwareMetadataCheck : SigAcknowledgedGenericMessage
/// Index of the firmware image in the Firmware Information List state to check.
/// The Update Firmware Image Index field shall identify the firmware image in the
/// Firmware Information List state on the Firmware Update Server that the
/// metadata is checked against.
@property (nonatomic,assign) UInt8 updateFirmwareImageIndex;
/// Vendor-specific metadata (Optional). Size is 1 to 255.
/// If present, the Incoming Firmware Metadata field shall contain the custom data
/// from the firmware vendor. The firmware metadata that can be used to check
/// whether the installed firmware image identified by the Firmware Image Index
/// field will accept an update based on the metadata provided for the new
/// firmware image.
@property (nonatomic,strong) NSData *incomingFirmwareMetadata;

- (instancetype)initWithUpdateFirmwareImageIndex:(UInt8)updateFirmwareImageIndex incomingFirmwareMetadata:(nullable NSData *)incomingFirmwareMetadata;

/**
 * @brief   Initialize SigFirmwareUpdateFirmwareMetadataCheck object.
 * @param   parameters    the hex data of SigFirmwareUpdateFirmwareMetadataCheck.
 * @return  return `nil` when initialize SigFirmwareUpdateFirmwareMetadataCheck object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 8.4.1.4 Firmware Update Firmware Metadata Status, opcode:0xB604


/// The Firmware Update Firmware Metadata Status message is an unacknowledged
/// message sent to a Firmware Update Client that is used to report whether a
/// Firmware Update Server can accept a firmware update.
/// @note   The Firmware Update Firmware Metadata Status message is sent in
/// response to a Firmware Update Firmware Metadata Check message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.93),
/// 8.4.1.4 Firmware Update Firmware Metadata Status.
@interface SigFirmwareUpdateFirmwareMetadataStatus : SigGenericMessage
/// Status Code from the firmware metadata check, size is 3 bits.
/// The Status field shall report whether the firmware metadata check against the image
/// by the Firmware Image Index indicated that the firmware image can be updated.
/// The status codes for this field are defined in Table 8.23.
@property (nonatomic,assign) SigFirmwareUpdateServerAndClientModelStatusType status;
/// The Firmware Update Additional Information state from the Firmware Update Server
/// (see Section 8.3.1.3), size is 5 bits.
/// The Additional Information field shall indicate the Firmware Update Additional
/// Information state (see Section 8.3.1.3).
@property (nonatomic,assign) SigFirmwareUpdateAdditionalInformationStatusType additionalInformation;
/// Index of the firmware image in the Firmware Information List state that was checked.
/// The Firmware Image Index field shall identify the firmware image in the Firmware
/// Information List state on the Firmware Update Server that was checked.
@property (nonatomic,assign) UInt8 updateFirmwareImageIndex;

/**
 * @brief   Initialize SigFirmwareUpdateFirmwareMetadataStatus object.
 * @param   parameters    the hex data of SigFirmwareUpdateFirmwareMetadataStatus.
 * @return  return `nil` when initialize SigFirmwareUpdateFirmwareMetadataStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 8.4.1.5 Firmware Update Get, opcode:0xB605


/// The Firmware Update Get message is an acknowledged message
/// used to get the current status of the Firmware Update Server.
/// @note   The response to the Firmware Update Get message is a
/// Firmware Update Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.94),
/// 8.4.1.5 Firmware Update Get.
@interface SigFirmwareUpdateGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigFirmwareUpdateGet object.
 * @param   parameters    the hex data of SigFirmwareUpdateGet.
 * @return  return `nil` when initialize SigFirmwareUpdateGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 8.4.1.6 Firmware Update Start, opcode:0xB606


/// The Firmware Update Start message is an acknowledged message used to
/// start a firmware update on a Firmware Update Server.
/// @note   The response to the Firmware Update Start message is a Firmware
/// Update Status message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.94),
/// 8.4.1.6 Firmware Update Start.
@interface SigFirmwareUpdateStart : SigAcknowledgedGenericMessage
/// Time To Live value to use during firmware image transfer.
/// The Update TTL field shall indicate the time to live (TTL) value that is used during
/// firmware image transfer. The values for the Update TTL field are defined in Table 8.5.
@property (nonatomic,assign) UInt8 updateTTL;
/// Used to compute the timeout of the firmware image transfer.
/// The Update Timeout Base field shall contain the value that the Firmware Update
/// Server uses to calculate when firmware image transfer will be suspended.
@property (nonatomic,assign) UInt16 updateTimeoutBase;
/// BLOB identifier for the firmware image.
/// The Update BLOB ID field shall contain the BLOB identifier to use during firmware
/// image transfer.
@property (nonatomic,assign) UInt64 updateBLOBID;
/// Index of the firmware image in the Firmware Information List state to be updated.
/// The Firmware Image Index field shall identify the firmware image in the Firmware
/// Information List state to be updated.
@property (nonatomic,assign) UInt8 updateFirmwareImageIndex;
/// Vendor-specific firmware metadata (Optional). Size is 1 to 255.
/// If present, the Incoming Firmware Metadata field shall contain the custom data
/// from the firmware vendor that is used to check whether the firmware image can
/// be updated.
@property (nonatomic,strong) NSData *incomingFirmwareMetadata;
- (instancetype)initWithUpdateTTL:(UInt8)updateTTL updateTimeoutBase:(UInt16)updateTimeoutBase updateBLOBID:(UInt64)updateBLOBID updateFirmwareImageIndex:(UInt8)updateFirmwareImageIndex incomingFirmwareMetadata:(nullable NSData *)incomingFirmwareMetadata;

/**
 * @brief   Initialize SigFirmwareUpdateStart object.
 * @param   parameters    the hex data of SigFirmwareUpdateStart.
 * @return  return `nil` when initialize SigFirmwareUpdateStart object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 8.4.1.7 Firmware Update Cancel, opcode:0xB607


/// The Firmware Update Cancel message is an acknowledged message
/// used to cancel a firmware update and delete any stored information
/// about the update on a Firmware Update Server.
/// @note   The response to a Firmware Update Cancel message is a
/// Firmware Update Status message.
/// @note   The Firmware Update Cancel message has no parameters.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.95),
/// 8.4.1.7 Firmware Update Cancel.
@interface SigFirmwareUpdateCancel : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigFirmwareUpdateCancel object.
 * @param   parameters    the hex data of SigFirmwareUpdateCancel.
 * @return  return `nil` when initialize SigFirmwareUpdateCancel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 8.4.1.8 Firmware Update Apply, opcode:0xB608


/// The Firmware Update Apply message is an acknowledged message
/// used to apply a firmware image that has been transferred to a
/// Firmware Update Server.
/// @note   The response to a Firmware Update Apply message is a
/// Firmware Update Status message.
/// @note   The Firmware Update Apply message has no parameters.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.95),
/// 8.4.1.8 Firmware Update Apply.
@interface SigFirmwareUpdateApply : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigFirmwareUpdateApply object.
 * @param   parameters    the hex data of SigFirmwareUpdateApply.
 * @return  return `nil` when initialize SigFirmwareUpdateApply object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 8.4.1.9 Firmware Update Status, opcode:0xB609


/// The Firmware Update Status message is an unacknowledged message
/// sent by a Firmware Update Server to report the status of a firmware update.
/// @note   A Firmware Updates Status message is sent in response to
/// a Firmware Update Get message, a Firmware Update Start message,
/// a Firmware Update Cancel message, or a Firmware Update Apply message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.95),
/// 8.4.1.9 Firmware Update Status.
@interface SigFirmwareUpdateStatus : SigGenericMessage
/// Status Code for the requesting message, size is 3 bits.
/// The Status field shall identify the Status Code for the last operation
/// performed on the Firmware Update Server state. The values of the
/// field are defined in Table 8.23.
@property (nonatomic,assign) SigFirmwareUpdateServerAndClientModelStatusType status;
/// Reserved for Future Use. Size is 2 bits.
@property (nonatomic,assign) UInt8 RFU1;
/// The Update Phase state of the Firmware Update Server, size is 3 bits.
/// The Update Phase field shall indicate the Update Phase state (see
/// Section 8.3.1.2).
@property (nonatomic,assign) SigFirmwareUpdatePhaseType updatePhase;
/// Time To Live value to use during firmware image transfer (Optional),
/// size is 8 bits.
/// If present, the Update TTL field shall indicate the Update TTL state.
/// The Update TTL values are defined in Table 8.5.
@property (nonatomic,assign) UInt8 updateTTL;
/// The Firmware Update Additional Information state from the Firmware
/// Update Server (see Section 8.3.1.3) (C.1), size is 5 bits. (C.1: If the
/// Update TTL field is present, the Additional Information field, RFU2 field,
/// Update Timeout Base field, Update BLOB ID field, and Firmware Image
/// Index field shall be present; otherwise, the Additional Information field,
/// RFU2 field, Update Timeout Base field, BLOB ID field, and Firmware
/// Image Index field shall not be present.)
/// If present, the Additional Information field shall indicate the Firmware
/// Update Additional Information state (see Section 8.3.1.3).
@property (nonatomic,assign) SigFirmwareUpdateAdditionalInformationStatusType additionalInformation;
/// Reserved for Future Use (C.1), size is 3 bits.(C.1: If the Update TTL field
/// is present, the Additional Information field, RFU2 field, Update Timeout
/// Base field, Update BLOB ID field, and Firmware Image Index field shall
/// be present; otherwise, the Additional Information field, RFU2 field,
/// Update Timeout Base field, BLOB ID field, and Firmware Image Index
/// field shall not be present.)
@property (nonatomic,assign) UInt8 RFU2;
/// Used to compute the timeout of the firmware image transfer (C.1).(C.1: If
/// the Update TTL field is present, the Additional Information field, RFU2 field,
/// Update Timeout Base field, Update BLOB ID field, and Firmware Image
/// Index field shall be present; otherwise, the Additional Information field,
/// RFU2 field, Update Timeout Base field, BLOB ID field, and Firmware Image
/// Index field shall not be present.)
/// If present, the Update Timeout Base field shall indicate the Update Server
/// Timeout Base state.
@property (nonatomic,assign) UInt16 updateTimeoutBase;
/// BLOB identifier for the firmware image (C.1).(C.1: If the Update TTL field is
/// present, the Additional Information field, RFU2 field, Update Timeout Base
/// field, Update BLOB ID field, and Firmware Image Index field shall be present;
/// otherwise, the Additional Information field, RFU2 field, Update Timeout Base
/// field, BLOB ID field, and Firmware Image Index field shall not be present.)
/// If present, the Update BLOB ID field shall indicate the Update BLOB ID state.
@property (nonatomic,assign) UInt64 updateBLOBID;
/// The index of the firmware image in the Firmware Information List state being
/// updated (C.1).(C.1: If the Update TTL field is present, the Additional Information
/// field, RFU2 field, Update Timeout Base field, Update BLOB ID field, and Firmware
/// Image Index field shall be present; otherwise, the Additional Information field,
/// RFU2 field, Update Timeout Base field, BLOB ID field, and Firmware Image
/// Index field shall not be present.)
/// If present, the Update Firmware Image Index field shall identify the firmware
/// image in the Firmware Information List state being updated.
@property (nonatomic,assign) UInt8 updateFirmwareImageIndex;

/**
 * @brief   Initialize SigFirmwareUpdateStatus object.
 * @param   parameters    the hex data of SigFirmwareUpdateStatus.
 * @return  return `nil` when initialize SigFirmwareUpdateStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark - 8.4.2 Firmware Distribution model messages


#pragma mark 8.4.2.8 Firmware Distribution Get, opcode:0xB60A


/// The Firmware Distribution Get message is an acknowledged message
/// sent by a Firmware Distribution Client to get the state of the current
/// firmware image distribution on a Firmware Distribution Server.
/// @note   The response to a Firmware Distribution Get message is a
/// Firmware Distribution Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.100),
/// 8.4.2.8 Firmware Distribution Get.
@interface SigFirmwareDistributionGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigFirmwareDistributionGet object.
 * @param   parameters    the hex data of SigFirmwareDistributionGet.
 * @return  return `nil` when initialize SigFirmwareDistributionGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 8.4.2.9 Firmware Distribution Start, opcode:0xB60B


/// The Firmware Distribution Start message is an acknowledged message
/// sent by a Firmware Distribution Client to start the firmware image
/// distribution to the Updating nodes in the Distribution Receivers List.
/// @note   The response to a Firmware Distribution Start message is a
/// Firmware Distribution Status message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.100),
/// 8.4.2.9 Firmware Distribution Start.
@interface SigFirmwareDistributionStart : SigAcknowledgedGenericMessage
/// Index of the application key used in a firmware image distribution.
/// The Distribution AppKey Index field shall indicate the application key
/// used for the firmware image distribution.
@property (nonatomic,assign) UInt16 distributionAppKeyIndex;
/// Time To Live value used in a firmware image distribution.
/// The Distribution TTL field shall indicate the TTL value used for a firmware
/// image distribution. The values for the Distribution TTL field are defined
/// in Table 8.5.
@property (nonatomic,assign) UInt8 distributionTTL;
/// Used to compute the timeout of the firmware image distribution.
/// The Distribution Timeout Base field shall contain the value that is used to
/// calculate when firmware image distribution will be suspended.
@property (nonatomic,assign) UInt16 distributionTimeoutBase;
/// Mode of the transfer, size is 2 bits.
/// The Distribution Transfer Mode field shall indicate the mode used to transfer
/// the BLOB to the Updating node (see Section 7.2.1.4).
@property (nonatomic,assign) SigTransferModeState distributionTransferMode;
/// Firmware update policy, size is 1 bits.
/// The Update Policy field shall indicate the update policy that the Firmware
/// Distribution Server will use for this firmware image distribution.
@property (nonatomic,assign) SigUpdatePolicyType updatePolicy;
/// Reserved for Future Use. Size is 5 bits.
@property (nonatomic,assign) UInt8 RFU;
/// Index of the firmware image in the Firmware Images List state to use during
/// firmware image distribution.
/// The Distribution Firmware Image Index field shall identify the index of the
/// firmware image from the Firmware Images List state to transfer during
/// firmware image distribution.
@property (nonatomic,assign) UInt16 distributionFirmwareImageIndex;
/// Multicast address used in a firmware image distribution. Size is 16 bits or 128 bits.
/// The Distribution Multicast Address field shall identify the multicast address of the
/// Updating nodes that the firmware image is intended for. The value of the Distribution
/// Multicast Address field shall be a group address, the Label UUID of a virtual address,
/// or the Unassigned address.
@property (nonatomic,strong) NSData *distributionMulticastAddress;

- (instancetype)initWithDistributionAppKeyIndex:(UInt16)distributionAppKeyIndex distributionTTL:(UInt8)distributionTTL distributionTimeoutBase:(UInt16)distributionTimeoutBase distributionTransferMode:(SigTransferModeState)distributionTransferMode updatePolicy:(SigUpdatePolicyType)updatePolicy RFU:(UInt8)RFU distributionFirmwareImageIndex:(UInt16)distributionFirmwareImageIndex distributionMulticastAddress:(NSData *)distributionMulticastAddress;

/**
 * @brief   Initialize SigFirmwareDistributionStart object.
 * @param   parameters    the hex data of SigFirmwareDistributionStart.
 * @return  return `nil` when initialize SigFirmwareDistributionStart object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 8.4.2.10 Firmware Distribution Cancel, opcode:0xB60C


/// The Firmware Distribution Cancel message is an acknowledged message
/// sent by a Firmware Distribution Client to stop the firmware image distribution
/// from a Firmware Distribution Server.
/// @note   The response to a Firmware Distribution Cancel message is a
/// Firmware Distribution Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.101),
/// 8.4.2.10 Firmware Distribution Cancel.
@interface SigFirmwareDistributionCancel : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigFirmwareDistributionCancel object.
 * @param   parameters    the hex data of SigFirmwareDistributionCancel.
 * @return  return `nil` when initialize SigFirmwareDistributionCancel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 8.4.2.11 Firmware Distribution Apply, opcode:0xB60D


/// The Firmware Distribution Apply message is an acknowledged message
/// sent from a Firmware Distribution Client to a Firmware Distribution Server
/// to apply the firmware image on the Updating nodes.
/// @note   The response to a Firmware Distribution Apply message is a
/// Firmware Distribution Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.101),
/// 8.4.2.11 Firmware Distribution Apply.
@interface SigFirmwareDistributionApply : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigFirmwareDistributionApply object.
 * @param   parameters    the hex data of SigFirmwareDistributionApply.
 * @return  return `nil` when initialize SigFirmwareDistributionApply object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 8.4.2.12 Firmware Distribution Status, opcode:0xB60E


/// The Firmware Distribution Status message is an unacknowledged message
/// sent by a Firmware Distribution Server to report the status of a firmware
/// image distribution.
/// @note   A Firmware Distribution Status message is sent as a response to
/// a Firmware Distribution Get message, Firmware Distribution Start message,
/// Firmware Distribution Cancel message, or Firmware Distribution Apply message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.102),
/// 8.4.2.12 Firmware Distribution Status.
@interface SigFirmwareDistributionStatus : SigGenericMessage
/// Status Code for the requesting message.
/// The Status field shall identify the Status Code for the last operation performed
/// on the Firmware Distribution Server state. The status codes are defined in Table 8.24.
@property (nonatomic,assign) SigFirmwareDistributionServerAndClientModelStatusType status;
/// Phase of the firmware image distribution.
/// The Distribution Phase field shall indicate the Distribution Phase state.
@property (nonatomic,assign) SigDistributionPhaseState distributionPhase;
/// Multicast address used in firmware image distribution (Optional).
/// If present, the Distribution Multicast Address field shall indicate the Distribution
/// Multicast Address state. When using a Label UUID, the status message shall
/// provide this value as a virtual address, as defined in the Mesh Profile specification [2].
@property (nonatomic,assign) UInt16 distributionMulticastAddress;
/// Index of an application key used in a firmware image distribution (C.1). (C.1: If the
/// Distribution Multicast Address field is present, the Distribution AppKey Index field,
/// Distribution TTL field, Distribution Timeout Base field, Distribution Transfer Mode
/// field, Update Policy field, RFU field, and the Distribution Firmware Image Index
/// field shall also be present; otherwise, the Distribution AppKey Index field, Distribution
/// TTL field, Distribution Timeout Base field, Distribution Transfer Mode field, Update
/// Policy field, RFU field and the Distribution Firmware Image Index field shall not be
/// present.)
/// If present, the Distribution AppKey Index field shall indicate the Distribution
/// AppKey Index state.
@property (nonatomic,assign) UInt16 distributionAppKeyIndex;
/// Time To Live value used in a firmware image distribution (C.1). (C.1: If the Distribution
/// Multicast Address field is present, the Distribution AppKey Index field, Distribution TTL
/// field, Distribution Timeout Base field, Distribution Transfer Mode field, Update Policy
/// field, RFU field, and the Distribution Firmware Image Index field shall also be present;
/// otherwise, the Distribution AppKey Index field, Distribution TTL field, Distribution
/// Timeout Base field, Distribution Transfer Mode field, Update Policy field, RFU field and
/// the Distribution Firmware Image Index field shall not be present.)
@property (nonatomic,assign) UInt8 distributionTTL;
/// Used to compute the timeout of the firmware image distribution (C.1). (C.1: If the
/// Distribution Multicast Address field is present, the Distribution AppKey Index field,
/// Distribution TTL field, Distribution Timeout Base field, Distribution Transfer Mode
/// field, Update Policy field, RFU field, and the Distribution Firmware Image Index field
/// shall also be present; otherwise, the Distribution AppKey Index field, Distribution
/// TTL field, Distribution Timeout Base field, Distribution Transfer Mode field, Update
/// Policy field, RFU field and the Distribution Firmware Image Index field shall not be
/// present.)
@property (nonatomic,assign) UInt16 distributionTimeoutBase;
/// Mode of the transfer, size is 2 bits (C.1). (C.1: If the Distribution Multicast Address
/// field is present, the Distribution AppKey Index field, Distribution TTL field, Distribution
/// Timeout Base field, Distribution Transfer Mode field, Update Policy field, RFU field,
/// and the Distribution Firmware Image Index field shall also be present; otherwise, the
/// Distribution AppKey Index field, Distribution TTL field, Distribution Timeout Base field,
/// Distribution Transfer Mode field, Update Policy field, RFU field and the Distribution
/// Firmware Image Index field shall not be present.)
@property (nonatomic,assign) SigTransferModeState distributionTransferMode;
/// Firmware update policy, size is 1 bits (C.1). (C.1: If the Distribution Multicast Address
/// field is present, the Distribution AppKey Index field, Distribution TTL field, Distribution
/// Timeout Base field, Distribution Transfer Mode field, Update Policy field, RFU field, and
/// the Distribution Firmware Image Index field shall also be present; otherwise, the
/// Distribution AppKey Index field, Distribution TTL field, Distribution Timeout Base field,
/// Distribution Transfer Mode field, Update Policy field, RFU field and the Distribution
/// Firmware Image Index field shall not be present.)
@property (nonatomic,assign) SigUpdatePolicyType updatePolicy;
/// Reserved for Future Use. Size is 5 bits (C.1). (C.1: If the Distribution Multicast Address
/// field is present, the Distribution AppKey Index field, Distribution TTL field, Distribution
/// Timeout Base field, Distribution Transfer Mode field, Update Policy field, RFU field, and
/// the Distribution Firmware Image Index field shall also be present; otherwise, the Distribution
/// AppKey Index field, Distribution TTL field, Distribution Timeout Base field, Distribution
/// Transfer Mode field, Update Policy field, RFU field and the Distribution Firmware Image
/// Index field shall not be present.)
@property (nonatomic,assign) UInt8 RFU;
/// Index of the firmware image in the Firmware Images List state to use during firmware
/// image distribution (C.1). (C.1: If the Distribution Multicast Address field is present, the
/// Distribution AppKey Index field, Distribution TTL field, Distribution Timeout Base field,
/// Distribution Transfer Mode field, Update Policy field, RFU field, and the Distribution
/// Firmware Image Index field shall also be present; otherwise, the Distribution AppKey
/// Index field, Distribution TTL field, Distribution Timeout Base field, Distribution Transfer
/// Mode field, Update Policy field, RFU field and the Distribution Firmware Image Index
/// field shall not be present.)
@property (nonatomic,assign) UInt16 distributionFirmwareImageIndex;

/**
 * @brief   Initialize SigFirmwareDistributionStatus object.
 * @param   parameters    the hex data of SigFirmwareDistributionStatus.
 * @return  return `nil` when initialize SigFirmwareDistributionStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithStatus:(SigFirmwareDistributionServerAndClientModelStatusType)status distributionPhase:(SigDistributionPhaseState)distributionPhase distributionMulticastAddress:(UInt16)distributionMulticastAddress distributionAppKeyIndex:(UInt16)distributionAppKeyIndex distributionTTL:(UInt8)distributionTTL distributionTimeoutBase:(UInt16)distributionTimeoutBase distributionTransferMode:(SigTransferModeState)distributionTransferMode updatePolicy:(SigUpdatePolicyType)updatePolicy RFU:(UInt8)RFU distributionFirmwareImageIndex:(UInt16)distributionFirmwareImageIndex;

@end


#pragma mark 8.4.2.13 Firmware Distribution Upload Get, opcode:0xB616


/// The Firmware Distribution Upload Get message is an acknowledged
/// message sent by a Firmware Distribution Client to check the status of
/// a firmware image upload to a Firmware Distribution Server.
/// @note   The response to a Firmware Distribution Upload Get message
/// is a Firmware Distribution Upload Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.103),
/// 8.4.2.13 Firmware Distribution Upload Get.
@interface SigFirmwareDistributionUploadGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigFirmwareDistributionUploadGet object.
 * @param   parameters    the hex data of SigFirmwareDistributionUploadGet.
 * @return  return `nil` when initialize SigFirmwareDistributionUploadGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 8.4.2.14 Firmware Distribution Upload Start, opcode:0xB617


/// The Firmware Distribution Upload Start message is an acknowledged message sent
/// by a Firmware Distribution Client to start a firmware image upload to a Firmware
/// Distribution Server.
/// @note   The response to a Firmware Distribution Upload Start message is a
/// Firmware Distribution Upload Status message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.103),
/// 8.4.2.14 Firmware Distribution Upload Start.
@interface SigFirmwareDistributionUploadStart : SigAcknowledgedGenericMessage
/// Time To Live value used in a firmware image upload.
/// The Upload TTL field shall indicate the TTL value used for a firmware image upload.
/// The values for the Upload TTL field are defined in Table 8.5.
@property (nonatomic,assign) UInt8 uploadTTL;
/// Used to compute the timeout of the firmware image upload.
/// The Upload Timeout Base field shall contain the value that is used to calculate when
/// firmware image upload will be suspended.
@property (nonatomic,assign) UInt16 uploadTimeoutBase;
/// BLOB identifier for the firmware image.
/// The Upload BLOB ID field shall field shall contain the BLOB identifier to use during
/// firmware image upload.
@property (nonatomic,assign) UInt64 uploadBLOBID;
/// Firmware image size (in octets).
/// The Upload Firmware Size field shall indicate the size of the firmware image being
/// uploaded.
@property (nonatomic,assign) UInt32 uploadFirmwareSize;
/// Size of the Upload Firmware Metadata field.
/// The Upload Firmware Metadata Length field shall indicate the length of the Upload
/// Firmware Metadata field.
@property (nonatomic,assign) UInt8 uploadFirmwareMetadataLength;
/// Vendor-specific firmware metadata (C.1). Size is 1 to 255. (C.1: If the value of the
/// Upload Firmware Metadata Length field is greater than 0, the Upload Firmware
/// Metadata field shall be present; otherwise, the Upload Firmware Metadata field
/// shall be omitted.)
/// If present, the Upload Firmware Metadata field shall contain the custom data from
/// the firmware vendor that will be used to check whether the firmware image can be
/// updated.
@property (nonatomic,strong) NSData *uploadFirmwareMetadata;
/// The Firmware ID identifying the firmware image being uploaded. Size is Variable.
/// The Upload Firmware ID field shall identify the firmware image being uploaded.
@property (nonatomic,strong) NSData *uploadFirmwareID;

- (instancetype)initWithUploadTTL:(UInt8)uploadTTL uploadTimeoutBase:(UInt16)uploadTimeoutBase uploadBLOBID:(UInt64)uploadBLOBID uploadFirmwareSize:(UInt32)uploadFirmwareSize uploadFirmwareMetadataLength:(UInt8)uploadFirmwareMetadataLength uploadFirmwareMetadata:(NSData *)uploadFirmwareMetadata uploadFirmwareID:(NSData *)uploadFirmwareID;

/**
 * @brief   Initialize SigFirmwareDistributionUploadStart object.
 * @param   parameters    the hex data of SigFirmwareDistributionUploadStart.
 * @return  return `nil` when initialize SigFirmwareDistributionUploadStart object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 8.4.2.15 Firmware Distribution Upload OOB Start, opcode:0xB618


/// The Firmware Distribution Upload OOB Start message is an acknowledged message
/// sent by a Firmware Distribution Client to start a firmware image upload to a Firmware
/// Distribution Server using an out-of-band mechanism.
/// @note   The response to a Firmware Distribution Upload OOB Start message is a
/// Firmware Distribution Upload Status message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.104),
/// 8.4.2.15 Firmware Distribution Upload OOB Start.
@interface SigFirmwareDistributionUploadOOBStart : SigAcknowledgedGenericMessage
/// Length of the Upload URI field.
/// The Upload URI Length field shall indicate the length of the Upload URI field.
/// The value 0 is prohibited.
@property (nonatomic,assign) UInt8 uploadURILength;
/// URI for the firmware image source. Size is 1 to 255.
/// The Upload URI field shall contain the URI used to retrieve the firmware image.
@property (nonatomic,strong) NSData *uploadURI;
/// The Firmware ID value used to generate the URI query string. Size is Variable.
/// The Upload Firmware ID field shall contain the value used to generate the URI query
/// string (see Section 8.2.2.1 and Section 8.2.3.1).
@property (nonatomic,strong) NSData *uploadFirmwareID;

- (instancetype)initWithUploadURILength:(UInt8)uploadURILength uploadURI:(NSData *)uploadURI uploadFirmwareID:(NSData *)uploadFirmwareID;

/**
 * @brief   Initialize SigFirmwareDistributionUploadOOBStart object.
 * @param   parameters    the hex data of SigFirmwareDistributionUploadOOBStart.
 * @return  return `nil` when initialize SigFirmwareDistributionUploadOOBStart object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 8.4.2.16 Firmware Distribution Upload Cancel, opcode:0xB619


/// The Firmware Distribution Upload Cancel message is an acknowledged
/// message sent by a Firmware Distribution Client to stop a firmware image
/// upload to a Firmware Distribution Server.
/// @note   The response to a Firmware Distribution Upload Cancel
/// message is a Firmware Distribution Upload Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.104),
/// 8.4.2.16 Firmware Distribution Upload Cancel.
@interface SigFirmwareDistributionUploadCancel : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigFirmwareDistributionUploadCancel object.
 * @param   parameters    the hex data of SigFirmwareDistributionUploadCancel.
 * @return  return `nil` when initialize SigFirmwareDistributionUploadCancel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 8.4.2.17 Firmware Distribution Upload Status, opcode:0xB61A


/// The Firmware Distribution Upload Status message is an unacknowledged message
/// sent by a Firmware Distribution Server to report the status of a firmware image upload.
/// @note   A Firmware Distribution Upload Status message is sent as a response to a
/// Firmware Distribution Upload Get message, Firmware Distribution Upload Start message,
/// Firmware Distribution Upload OOB Start message, or Firmware Distribution
/// Upload Cancel message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.104),
/// 8.4.2.17 Firmware Distribution Upload Status.
@interface SigFirmwareDistributionUploadStatus : SigGenericMessage
/// Status Code for the requesting message.
/// The Status field shall identify the Status Code for the last operation performed on the
/// Firmware Distribution Server state. The values for the field are defined in Table 8.24.
@property (nonatomic,assign) SigFirmwareDistributionServerAndClientModelStatusType status;
/// Phase of the firmware image upload to a Firmware Distribution Server.
/// The Upload Phase field shall indicate the Upload Phase state.
@property (nonatomic,assign) SigFirmwareUploadPhaseStateType uploadPhase;
/// A percentage indicating the progress of the firmware image upload (Optional). Size is 7 bits.
/// (If present, the Upload Progress field shall indicate the progress of the firmware upload as
/// a percentage. The values 0x65 to 0xFF are prohibited.)
@property (nonatomic,assign) UInt8 uploadProgress;
/// A bit indicating whether the Upload is done in-band or out-of-band (C.1). YES mean has oob.
@property (nonatomic,assign) BOOL uploadType;
/// he Firmware ID identifying the firmware image being uploaded (C.1). Size is Variable.
/// (C.1: When the Upload Progress field is present, the Upload Firmware ID field shall
/// be present; otherwise, the Upload Firmware ID field shall be omitted.)
/// If present, the Upload Firmware ID field shall indicate the Upload Firmware ID state.
@property (nonatomic,strong) NSData *uploadFirmwareID;

/**
 * @brief   Initialize SigFirmwareDistributionUploadStatus object.
 * @param   parameters    the hex data of SigFirmwareDistributionUploadStatus.
 * @return  return `nil` when initialize SigFirmwareDistributionUploadStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithStatus:(SigFirmwareDistributionServerAndClientModelStatusType)status uploadPhase:(SigFirmwareUploadPhaseStateType)uploadPhase uploadProgress:(UInt8)uploadProgress uploadType:(BOOL)uploadType uploadFirmwareID:(NSData *)uploadFirmwareID;

@end


#pragma mark 8.4.2.18 Firmware Distribution Firmware Get, opcode:0xB61B


/// The Firmware Distribution Firmware Get message is an acknowledged message
/// sent by a Firmware Distribution Client to check whether a specific firmware
/// image is stored on a Firmware Distribution Server.
/// @note   The response to a Firmware Distribution Firmware Get message is a
/// Firmware Distribution Firmware Status message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.105),
/// 8.4.2.18 Firmware Distribution Firmware Get.
@interface SigFirmwareDistributionFirmwareGet : SigAcknowledgedGenericMessage
/// The Firmware ID identifying the firmware image to check. Size is Variable.
@property (nonatomic,strong) NSData *firmwareID;

- (instancetype)initWithFirmwareID:(NSData *)firmwareID;

/**
 * @brief   Initialize SigFirmwareDistributionFirmwareGet object.
 * @param   parameters    the hex data of SigFirmwareDistributionFirmwareGet.
 * @return  return `nil` when initialize SigFirmwareDistributionFirmwareGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 8.4.2.22 Firmware Distribution Firmware Status, opcode:0xB61C


/// The Firmware Distribution Firmware Status message is an unacknowledged message
/// sent by a Firmware Distribution Server to report the status of an operation on a stored
/// firmware image.
/// @note   A Firmware Distribution Firmware Status message is sent in response to a
/// Firmware Distribution Firmware Get message, a Firmware Distribution Firmware Get
/// By Index message, a Firmware Distribution Firmware Delete message, or a Firmware
/// Distribution Firmware Delete All message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.106),
/// 8.4.2.22 Firmware Distribution Firmware Status.
@interface SigFirmwareDistributionFirmwareStatus : SigGenericMessage
/// Status Code for the requesting message.
/// The Status field shall identify the Status Code for the last operation performed on the
/// Firmware Distribution Server state. The Status codes are defined in Table 8.24.
@property (nonatomic,assign) SigFirmwareDistributionServerAndClientModelStatusType status;
/// The number of firmware images stored on the Firmware Distribution Server.
/// The Entry Count field shall indicate the number of firmware images listed in the Firmware
/// Images List state.
@property (nonatomic,assign) UInt16 entryCount;
/// Index of the firmware image in the Firmware Images List state.
/// The Distribution Firmware Image Index field shall indicate the index of the firmware image
/// in the Firmware Images List state. The values from 0x0000 to 0xFFFE identify a firmware
/// image in the Firmware Images List state. The value 0xFFFF indicates that the firmware
/// image is not listed in the Firmware Images List state.
@property (nonatomic,assign) UInt16 distributionFirmwareImageIndex;
/// Identifies associated firmware image (C.1). Size is Variable. (C.1: The Firmware ID field shall
/// be present in either of the following conditions: 1) if the Firmware Images List state lists the
/// firmware image identified by the Distribution Firmware Image Index field; 2) if the message
/// is sent in response to a Firmware Distribution Firmware Get message; otherwise, the
/// Firmware ID field shall be omitted.)
/// If present, the Firmware ID field shall contain the Firmware ID of the firmware image at the
/// index in the Firmware Images List state that is identified by the Distribution Firmware Image
/// Index field.
@property (nonatomic,strong) NSData *firmwareID;

/**
 * @brief   Initialize SigFirmwareDistributionFirmwareStatus object.
 * @param   parameters    the hex data of SigFirmwareDistributionFirmwareStatus.
 * @return  return `nil` when initialize SigFirmwareDistributionFirmwareStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithStatus:(SigFirmwareDistributionServerAndClientModelStatusType)status entryCount:(UInt16)entryCount distributionFirmwareImageIndex:(UInt16)distributionFirmwareImageIndex firmwareID:(NSData *)firmwareID;

@end


#pragma mark 8.4.2.19 Firmware Distribution Firmware Get By Index, opcode:0xB61D


/// The Firmware Distribution Firmware Get By Index message is an acknowledged message
/// sent by a Firmware Distribution Client to check which firmware image is stored in a particular
/// entry in the Firmware Images List state on a Firmware Distribution Server.
/// @note   The response to a Firmware Distribution Firmware Get By Index message is a
/// Firmware Distribution Firmware Status message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.105),
/// 8.4.2.19 Firmware Distribution Firmware Get By Index.
@interface SigFirmwareDistributionFirmwareGetByIndex : SigAcknowledgedGenericMessage
/// Index of the entry in the Firmware Images List state.
/// The Distribution Firmware Image Index field shall indicate the index of the entry in the
/// Firmware Images List state.
@property (nonatomic,assign) UInt16 distributionFirmwareImageIndex;

- (instancetype)initWithDistributionFirmwareImageIndex:(UInt16)distributionFirmwareImageIndex;

/**
 * @brief   Initialize SigFirmwareDistributionFirmwareGetByIndex object.
 * @param   parameters    the hex data of SigFirmwareDistributionFirmwareGetByIndex.
 * @return  return `nil` when initialize SigFirmwareDistributionFirmwareGetByIndex object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 8.4.2.20 Firmware Distribution Firmware Delete, opcode:0xB61E


/// The Firmware Distribution Firmware Delete message is an acknowledged message
/// sent by a Firmware Distribution Client to delete a stored firmware image on a
/// Firmware Distribution Server.
/// @note   The response to a Firmware Distribution Firmware Delete message is a
/// Firmware Distribution Firmware Status message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.106),
/// 8.4.2.20 Firmware Distribution Firmware Delete.
@interface SigFirmwareDistributionFirmwareDelete : SigAcknowledgedGenericMessage
/// Identifies the firmware image to delete. Size is Variable.
/// The Firmware ID field shall identify the firmware image to be deleted from the
/// Firmware Images List state.
@property (nonatomic,strong) NSData *firmwareID;

- (instancetype)initWithFirmwareID:(NSData *)firmwareID;

/**
 * @brief   Initialize SigFirmwareDistributionFirmwareDelete object.
 * @param   parameters    the hex data of SigFirmwareDistributionFirmwareDelete.
 * @return  return `nil` when initialize SigFirmwareDistributionFirmwareDelete object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 8.4.2.21 Firmware Distribution Firmware Delete All, opcode:0xB61F


/// The Firmware Distribution Firmware Delete All message is an acknowledged message
/// sent by a Firmware Distribution Client to delete all firmware images stored on a
/// Firmware Distribution Server.
/// @note   The response to a Firmware Distribution Firmware Delete All message is a
/// Firmware Distribution Firmware Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.106),
/// 8.4.2.21 Firmware Distribution Firmware Delete All.
@interface SigFirmwareDistributionFirmwareDeleteAll : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigFirmwareDistributionFirmwareDeleteAll object.
 * @param   parameters    the hex data of SigFirmwareDistributionFirmwareDeleteAll.
 * @return  return `nil` when initialize SigFirmwareDistributionFirmwareDeleteAll object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 8.4.2.4 Firmware Distribution Receivers Get, opcode:0xB60F


/// The Firmware Distribution Receivers Get message is an acknowledged message
/// sent by the Firmware Distribution Client to get the firmware distribution status of
/// each Updating node.
/// @note   The response to a Firmware Distribution Receivers Get message is a
/// Firmware Distribution Receivers List message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.97),
/// 8.4.2.4 Firmware Distribution Receivers Get.
@interface SigFirmwareDistributionReceiversGet : SigAcknowledgedGenericMessage
/// Index of the first requested entry from the Distribution Receivers List state.
/// The First Index field shall indicate the first entry of the Distribution Receivers List
/// state of the Firmware Distribution Server to return in the Firmware Distribution
/// Receivers List message.
@property (nonatomic,assign) UInt16 firstIndex;
/// Maximum number of entries that the server includes in a Firmware Distribution
/// Receivers List message.
/// The Entries Limit field shall set the maximum number of Updating Node Entries
/// to return in the Firmware Distribution Receivers List message. The value of the
/// Entries Limit field shall be greater than 0.
@property (nonatomic,assign) UInt16 entriesLimit;

- (instancetype)initWithFirstIndex:(UInt16)firstIndex entriesLimit:(UInt16)entriesLimit;

/**
 * @brief   Initialize SigFirmwareDistributionReceiversGet object.
 * @param   parameters    the hex data of SigFirmwareDistributionReceiversGet.
 * @return  return `nil` when initialize SigFirmwareDistributionReceiversGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 8.4.2.5 Firmware Distribution Receivers List, opcode:0xB610


/// The Firmware Distribution Receivers List message is an unacknowledged message sent
/// by the Firmware Distribution Server to report the firmware distribution status of each receiver.
/// @note   A Firmware Distribution Receivers List message is sent as a response to a
/// Firmware Distribution Receivers Get message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.98),
/// 8.4.2.5 Firmware Distribution Receivers List.
@interface SigFirmwareDistributionReceiversList : SigGenericMessage
/// The number of entries in the Distribution Receivers List state.
/// The Receivers List Size field shall indicate the Distribution Receivers List Size state.
@property (nonatomic,assign) UInt16 receiversListCount;
/// Index of the first requested entry from the Distribution Receivers List state.
/// The First Index field shall indicate the index of the first entry in the Distribution Receivers List
/// state of the Firmware Distribution Server that was returned.
@property (nonatomic,assign) UInt16 firstIndex;
/// List of entries (Optional), size is Variable.
/// If present, the Receivers List field shall contain entries from the Distribution Receivers List state.
@property (nonatomic,strong) NSMutableArray <SigUpdatingNodeEntryModel *>*receiversList;

/**
 * @brief   Initialize SigFirmwareDistributionReceiversList object.
 * @param   parameters    the hex data of SigFirmwareDistributionReceiversList.
 * @return  return `nil` when initialize SigFirmwareDistributionReceiversList object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithReceiversListCount:(UInt16)receiversListCount firstIndex:(UInt16)firstIndex receiversList:(NSArray <SigUpdatingNodeEntryModel *>*)receiversList;

@end


#pragma mark 8.4.2.1 Firmware Distribution Receivers Add, opcode:0xB611


/// The Firmware Distribution Receivers Add message is an acknowledged message sent
/// by a Firmware Distribution Client to add new entries to the Distribution Receivers List
/// state of a Firmware Distribution Server.
/// @note   The response to a Firmware Distribution Receivers Add message is a
/// Firmware Distribution Receivers Status message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.96),
/// 8.4.2.1 Firmware Distribution Receivers Add.
@interface SigFirmwareDistributionReceiversAdd : SigAcknowledgedGenericMessage
/// The Firmware Distribution Receivers Add message shall contain at least one Receiver Entry.
/// For each Receiver Entry field in the message, the value of the Address field shall be unique.
@property (nonatomic,strong) NSMutableArray <SigReceiverEntryModel *>*receiverEntriesList;
- (instancetype)initWithReceiverEntriesList:(NSArray <SigReceiverEntryModel *>*)receiverEntriesList;

/**
 * @brief   Initialize SigFirmwareDistributionReceiversAdd object.
 * @param   parameters    the hex data of SigFirmwareDistributionReceiversAdd.
 * @return  return `nil` when initialize SigFirmwareDistributionReceiversAdd object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 8.4.2.2 Firmware Distribution Receivers Delete All, opcode:0xB612


/// The Firmware Distribution Receivers Delete All message is an acknowledged
/// message sent by a Firmware Distribution Client to remove all entries from the
/// Distribution Receivers List state of a Firmware Distribution Server.
/// @note   The response to a Firmware Distribution Receivers Delete All message
/// is a Firmware Distribution Receivers Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.97),
/// 8.4.2.2 Firmware Distribution Receivers Delete All.
@interface SigFirmwareDistributionReceiversDeleteAll : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigFirmwareDistributionReceiversDeleteAll object.
 * @param   parameters    the hex data of SigFirmwareDistributionReceiversDeleteAll.
 * @return  return `nil` when initialize SigFirmwareDistributionReceiversDeleteAll object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 8.4.2.3 Firmware Distribution Receivers Status, opcode:0xB613


/// The Firmware Distribution Receivers Status message is an unacknowledged
/// message sent by a Firmware Distribution Server to report the current size of
/// the Distribution Receivers List state.
/// @note   A Firmware Distribution Receivers Status message is sent as a
/// response to a Firmware Distribution Receivers Add message or a Firmware
/// Distribution Receivers Delete All message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.97),
/// 8.4.2.3 Firmware Distribution Receivers Status.
@interface SigFirmwareDistributionReceiversStatus : SigGenericMessage
/// Status Code for the requesting message.
/// The Status field shall identify the Status Code for the last operation performed
/// on the Firmware Distribution Server states. The values for the field are defined
/// in Table 8.24.
@property (nonatomic,assign) SigFirmwareDistributionServerAndClientModelStatusType status;
/// The number of entries in the Distribution Receivers List state.
/// The Receivers List Count field shall indicate the Distribution Receivers List
/// Count state.
@property (nonatomic,assign) UInt16 receiversListCount;

/**
 * @brief   Initialize SigFirmwareDistributionReceiversStatus object.
 * @param   parameters    the hex data of SigFirmwareDistributionReceiversStatus.
 * @return  return `nil` when initialize SigFirmwareDistributionReceiversStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithStatus:(SigFirmwareDistributionServerAndClientModelStatusType)status receiversListCount:(UInt16)receiversListCount;

@end


#pragma mark 8.4.2.6 Firmware Distribution Capabilities Get, opcode:0xB614


/// The Firmware Distribution Capabilities Get message is an acknowledged message
/// sent by a Firmware Distribution Client to get the capabilities of a Firmware
/// Distribution Server.
/// @note   The response to a Firmware Distribution Capabilities Get message is a
/// Firmware Distribution Capabilities Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.99),
/// 8.4.2.6 Firmware Distribution Capabilities Get.
@interface SigFirmwareDistributionCapabilitiesGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigFirmwareDistributionCapabilitiesGet object.
 * @param   parameters    the hex data of SigFirmwareDistributionCapabilitiesGet.
 * @return  return `nil` when initialize SigFirmwareDistributionCapabilitiesGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 8.4.2.7 Firmware Distribution Capabilities Status, opcode:0xB615


/// The Firmware Distribution Capabilities Status message is an unacknowledged
/// message sent by a Firmware Distribution Server to report Distributor capabilities.
/// @note   A Firmware Distribution Capabilities Status message is sent as a
/// response to a Firmware Distributor Capabilities Get message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.99),
/// 8.4.2.7 Firmware Distribution Capabilities Status.
@interface SigFirmwareDistributionCapabilitiesStatus : SigGenericMessage
/// Maximum number of entries in the Distribution Receivers List state.
/// The Max Distribution Receivers List Size field shall indicate the value of the Max
/// Distribution Receivers List Size state.
@property (nonatomic,assign) UInt16 maxDistributionReceiversListSize;
/// Maximum number of entries in the Firmware Images List state.
/// The Max Firmware Images List Size field shall indicate the value of the Max
/// Firmware Images List Size state.
@property (nonatomic,assign) UInt16 maxFirmwareImagesListSize;
/// Maximum size of one firmware image (in octets).
/// The Max Firmware Image Size field shall indicate the value of the Max Firmware
/// Image Size state.
@property (nonatomic,assign) UInt32 maxFirmwareImageSize;
/// Total space dedicated to storage of firmware images (in octets).
/// The Max Upload Space field shall indicate the value of the Max Upload Space state.
@property (nonatomic,assign) UInt32 maxUploadSpace;
/// Remaining available space in firmware image storage (in octets).
/// The Remaining Upload Space field shall indicate the value of the Remaining
/// Upload Space state.
@property (nonatomic,assign) UInt32 remainingUploadSpace;
/// Value of the Out-of-Band Retrieval Supported state.
/// The Out-of-Band Retrieval Supported field shall indicate the value of the Out-of-Band
/// Retrieval Supported state.
@property (nonatomic,assign) UInt8 outOfBandRetrievalSupported;
/// Value of the Supported URI Scheme Names state (C.1).(C.1: If the value of the Out-of-Band
/// Retrieval Supported field is OOB Supported, the Supported URI Scheme Names field shall
/// be present; otherwise, the Supported URI Scheme Names field shall not be present.)
/// If present, the Supported URI Scheme Names field shall indicate the value of the
/// Supported URI Scheme Names state.
@property (nonatomic,strong) NSData *supportedURISchemeNames;

/**
 * @brief   Initialize SigFirmwareDistributionCapabilitiesStatus object.
 * @param   parameters    the hex data of SigFirmwareDistributionCapabilitiesStatus.
 * @return  return `nil` when initialize SigFirmwareDistributionCapabilitiesStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithMaxDistributionReceiversListSize:(UInt16)maxDistributionReceiversListSize maxFirmwareImagesListSize:(UInt16)maxFirmwareImagesListSize maxFirmwareImageSize:(UInt32)maxFirmwareImageSize maxUploadSpace:(UInt32)maxUploadSpace remainingUploadSpace:(UInt32)remainingUploadSpace outOfBandRetrievalSupported:(UInt8)outOfBandRetrievalSupported;

@end


#pragma mark - 7.3.1 BLOB Transfer messages


#pragma mark 7.3.1.1 BLOB Transfer Get, opcode:0xB701


/// BLOB Transfer Get is an acknowledged message used to get
/// the state of a BLOB transfer, if any, on a BLOB Transfer Server.
/// @note   The response to a BLOB Transfer Get message is a
/// BLOB Transfer Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.28),
/// 7.3.1.1 BLOB Transfer Get.
@interface SigBLOBTransferGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigBLOBTransferGet object.
 * @param   parameters    the hex data of SigBLOBTransferGet.
 * @return  return `nil` when initialize SigBLOBTransferGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 7.3.1.2 BLOB Transfer Start, opcode:0xB702


/// BLOB Transfer Start is an acknowledged message used to start
/// a new BLOB transfer.
/// @note   The response to a BLOB Transfer Start message is a
/// BLOB Transfer Status message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.28),
/// 7.3.1.2 BLOB Transfer Start.
@interface SigBLOBTransferStart : SigAcknowledgedGenericMessage
/// Reserved for Future Use, size is 6 bits.
@property (nonatomic,assign) UInt8 RFU;
/// BLOB transfer mode, size is 2 bits.
/// The Transfer Mode field shall indicate the new value of the Transfer
/// Mode state (see Section 7.2.1.4). The valid values of the field are
/// Pull BLOB Transfer Mode and Push BLOB Transfer Mode.
@property (nonatomic,assign) SigTransferModeState transferMode;
/// Unique BLOB identifier.
/// The BLOB ID field shall indicate the ID of the BLOB that is to be
/// transferred.
@property (nonatomic,assign) UInt64 BLOBID;
/// BLOB size in bytes
/// The BLOB Size field shall indicate the size of the BLOB to be
/// transferred.
@property (nonatomic,assign) UInt32 BLOBSize;
/// Size of the block during this transfer.
/// The Block Size Log field shall indicate the new value of the Block
/// Size Log state to be used in the transfer (see Section 7.2.1.1.3).
@property (nonatomic,assign) UInt8 BLOBBlockSizeLog;
/// Maximum payload size supported by the client.
/// The Client MTU Size field shall indicate the new value of the Client
/// MTU Size state (see Section 7.2.2.5).
@property (nonatomic,assign) UInt16 MTUSize;

- (instancetype)initWithTransferMode:(SigTransferModeState)transferMode BLOBID:(UInt64)BLOBID BLOBSize:(UInt32)BLOBSize BLOBBlockSizeLog:(UInt8)BLOBBlockSizeLog MTUSize:(UInt16)MTUSize;

/**
 * @brief   Initialize SigBLOBTransferStart object.
 * @param   parameters    the hex data of SigBLOBTransferStart.
 * @return  return `nil` when initialize SigBLOBTransferStart object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 7.3.1.3 BLOB Transfer Cancel, opcode:0xB703


/// BLOB Transfer Cancel is an acknowledged message used to cancel
/// the ongoing BLOB transfer.
/// @note   The response to the BLOB Transfer Cancel message is a
/// BLOB Transfer Status message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.28),
/// 7.3.1.3 BLOB Transfer Cancel.
@interface SigObjectTransferCancel : SigAcknowledgedGenericMessage
/// BLOB identifier.
/// The BLOB ID field shall identify the BLOB whose transfer is to be canceled.
@property (nonatomic,assign) UInt64 BLOBID;

- (instancetype)initWithBLOBID:(UInt64)BLOBID;

/**
 * @brief   Initialize SigObjectTransferCancel object.
 * @param   parameters    the hex data of SigObjectTransferCancel.
 * @return  return `nil` when initialize SigObjectTransferCancel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 7.3.1.4 BLOB Transfer Status, opcode:0xB704


/// BLOB Transfer Status is an unacknowledged message used to report
/// the state of the BLOB Transfer Server.
/// @note   The message is sent in response to a BLOB Transfer Get message,
/// a BLOB Transfer Start message, or a BLOB Transfer Cancel message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.29),
/// 7.3.1.4 BLOB Transfer Status.
@interface SigBLOBTransferStatus : SigGenericMessage
/// Status Code for the requesting message, size is 4 bits.
/// The Status field shall identify the Status Code for the last operation
/// performed on the BLOB Transfer Server states. The values of the Status
/// field are defined in Table 7.17.
@property (nonatomic,assign) SigBLOBTransferStatusType status;
/// Reserved for Future Use, size is 2 bits.
@property (nonatomic,assign) UInt8 RFU;
/// BLOB transfer mode, size is 2 bits.
/// The Transfer Mode field shall indicate the Transfer Mode state of the BLOB
/// Transfer Server (see Section 7.2.1.4).
@property (nonatomic,assign) SigTransferModeState transferMode;
/// Transfer phase, size is 8 bits.
/// The Transfer Phase field shall indicate the Transfer Phase state (see
/// Section 7.2.2.1).
@property (nonatomic,assign) SigTransferPhaseState transferPhase;
/// BLOB identifier (Optional)
/// If present, the BLOB ID field shall indicate the Expected BLOB ID state (see
/// Section 7.2.2.2).
@property (nonatomic,assign) UInt64 BLOBID;
/// BLOB size in octets (C.1). (C.1: If the BLOB ID field is present, then the BLOB
/// Size field may be present; otherwise, the BLOB Size field shall not be present.)
/// If present, the BLOB Size field shall indicate the BLOB Size state of the BLOB
/// Transfer Server (see Section 7.2.1.1.2).
@property (nonatomic,assign) UInt32 BLOBSize;
/// Indicates the block size (C.2). (C.2: If the BLOB Size field is present, then the
/// Block Size Log, Transfer MTU Size, and Blocks Not Received fields shall be
/// present; otherwise, these fields shall not be present.)
/// If present, the Block Size Log field shall indicate the Block Size Log state of
/// the BLOB Transfer Server (see Section 7.2.1.1.3).
@property (nonatomic,assign) UInt8 blockSizeLog;
/// MTU size in octets (C.2). (C.2: If the BLOB Size field is present, then the Block
/// Size Log, Transfer MTU Size, and Blocks Not Received fields shall be present;
/// otherwise, these fields shall not be present.)
/// If present, the Transfer MTU Size shall indicate the calculated Transfer MTU
/// size value (see Section 7.4.1.4.2).
@property (nonatomic,assign) UInt16 transferMTUSize;
/// Bit field indicating blocks that were not received (C.2), size is variable.
/// If present, the Blocks Not Received field shall indicate the Blocks Not Received
/// state (see Section 7.2.2.3).
@property (nonatomic,strong) NSData *blocksNotReceived;

/**
 * @brief   Initialize SigBLOBTransferStatus object.
 * @param   parameters    the hex data of SigBLOBTransferStatus.
 * @return  return `nil` when initialize SigBLOBTransferStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithStatus:(SigBLOBTransferStatusType)status RFU:(UInt8)RFU transferMode:(SigTransferModeState)transferMode transferPhase:(SigTransferPhaseState)transferPhase BLOBID:(UInt64)BLOBID BLOBSize:(UInt32)BLOBSize blockSizeLog:(UInt8)blockSizeLog transferMTUSize:(UInt16)transferMTUSize blocksNotReceived:(NSData *)blocksNotReceived;

@end


#pragma mark 3.1.3.1.6 BLOB Block Start, opcode:0xB705


/// BLOB Block Start is an acknowledged message used to start
/// the transfer of an incoming block to the server.
/// @note   The response to the BLOB Block Start message is
/// a BLOB Block Status message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.30),
/// 3.1.3.1.6 BLOB Block Start.
@interface SigBLOBBlockStart : SigAcknowledgedGenericMessage
/// Block number
/// The Block Number field shall indicate the new value of the Block
/// Number state (see Section 7.2.1.2.1).
@property (nonatomic,assign) UInt16 blockNumber;
/// Chunk size bytes for this block
/// The Chunk Size field shall indicate the new value of the Chunk
/// Size state (see Section 7.2.1.2.2).
@property (nonatomic,assign) UInt16 chunkSize;

- (instancetype)initWithBlockNumber:(UInt16)blockNumber chunkSize:(UInt16)chunkSize;

/**
 * @brief   Initialize SigBLOBBlockStart object.
 * @param   parameters    the hex data of SigBLOBBlockStart.
 * @return  return `nil` when initialize SigBLOBBlockStart object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 3.1.3.1.8 BLOB Chunk Transfer, opcode:0x7D


/// BLOB Chunk Transfer is an unacknowledged message used to
/// deliver a chunk of the current block to a BLOB Transfer Server.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.30),
/// 3.1.3.1.8 BLOB Chunk Transfer.
@interface SigBLOBChunkTransfer : SigGenericMessage
/// Chunk number
/// The Chunk Number field shall indicate the chunk’s number in a set
/// of chunks in a block.
@property (nonatomic,assign) UInt16 chunkNumber;
/// Part of the BLOB data, size is 1 to Chunk Size.
/// The Chunk Data field shall contain the chunk of the block identified by
/// the Chunk Number. The size of the Chunk Data field shall be greater
/// than 0 octets, and shall be less than or equal to the number of octets
/// indicated by the Chunk Size state.
@property (nonatomic,strong) NSData *chunkData;
/// Whether to use outsourcing for data transmission
@property (nonatomic,assign) BOOL sendBySegmentPdu;

- (instancetype)initWithChunkNumber:(UInt16)chunkNumber chunkData:(NSData *)chunkData sendBySegmentPdu:(BOOL)sendBySegmentPdu;

/**
 * @brief   Initialize SigBLOBChunkTransfer object.
 * @param   parameters    the hex data of SigBLOBChunkTransfer.
 * @return  return `nil` when initialize SigBLOBChunkTransfer object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 3.1.3.1.5 BLOB Block Get, opcode:0xB707


/// BLOB Block Get is an acknowledged message used to retrieve the phase
/// of the transfer and to find out which block is active (i.e., being transferred),
/// if any, and get the status of that block.
/// @note   The response to the BLOB Block Get message is a BLOB Block
/// Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.29),
/// 3.1.3.1.5 BLOB Block Get.
@interface SigBLOBBlockGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigBLOBBlockGet object.
 * @param   parameters    the hex data of SigBLOBBlockGet.
 * @return  return `nil` when initialize SigBLOBBlockGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 7.3.1.8 BLOB Partial Block Report, opcode:0x7C


/// BLOB Partial Block Report is an unacknowledged message used
/// by the BLOB Transfer Server to request chunks.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.31),
/// 7.3.1.8 BLOB Partial Block Report.
@interface SigBLOBPartialBlockReport : SigGenericMessage
/// List of chunks requested by the server (Optional). (If present, the Encoded
/// Missing Chunks field shall indicate the list of the chunks requested by the
/// BLOB Transfer Server. Each chunk is identified by a uint16 Chunk Number.
/// The list of the Chunk Numbers is encoded using UTF-8 as defined in [18].)
@property (nonatomic,strong) NSMutableArray <NSNumber *>*encodedMissingChunks;//[@(MissingChunkIndex)]

/**
 * @brief   Initialize SigBLOBPartialBlockReport object.
 * @param   parameters    the hex data of SigBLOBPartialBlockReport.
 * @return  return `nil` when initialize SigBLOBPartialBlockReport object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 3.1.3.1.7 BLOB Block Status, opcode:0x7E


/// BLOB Block Status is an unacknowledged message used to report
/// the status of the active block (i.e., the block being transferred), if any.
/// @note   The message is sent as a response to the BLOB Block
/// Start message or the BLOB Block Get message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.30),
/// 3.1.3.1.7 BLOB Block Status.
@interface SigBLOBBlockStatus : SigGenericMessage
/// Status code of the block transfer, size is 4 bits.
/// The Status field shall identify the Status Code for the last operation
/// performed on the BLOB Transfer Server states. The values of the
/// Status field are defined in Table 7.17.
@property (nonatomic,assign) SigBLOBBlockStatusType status;
/// Reserved for Future Use, size is 2 bits.
@property (nonatomic,assign) UInt8 RFU;
/// Indicates the format used to report missing chunks, size is 2 bits.
/// The Format field defines how missing chunks are reported. The values
/// of the Format field are defined in Table 7.23.
@property (nonatomic,assign) SigBLOBBlockFormatType format;
/// Block’s number in a set of blocks.
/// The Block Number field shall indicate the Block Number state (see
/// Section 7.2.1.2.1).
@property (nonatomic,assign) UInt16 blockNumber;
/// Chunk Size (in octets) for the current block.
/// The Chunk Size field shall indicate the Chunk Size state (see
/// Section 7.2.1.2.2).
@property (nonatomic,assign) UInt16 chunkSize;
/// Bit field of missing chunks for this block (C.1). (C.1: If the Format
/// field is set to Some Chunks Missing, the Missing Chunks field shall
/// be present; otherwise, it shall not be present.)
/// If present, the Missing Chunks field shall indicate the Missing Chunks state.
@property (nonatomic,strong) NSArray <NSNumber *>*missingChunksList;//[@(node.address)]
/// List of chunks requested by the server (C.2). (C.2 If the Format field is
/// set to Encoded Missing Chunks, the Encoded Missing Chunks field
/// shall be present; otherwise, it shall not be present.)
/// If present, the Encoded Missing Chunks field shall indicate the list of
/// the chunks requested by the BLOB Transfer Server. Each chunk is identified
/// by a uint16 Chunk Number. The list of the Chunk Numbers is encoded using
/// UTF-8 as defined in [18].
/// For example, if the requested chunks list is “0x00, 0x10, 0x80, 0x100”, the
/// value of the Encoded Missing Chunks field is “0x00, 0x10, 0xC2, 0x80, 0xC4, 0x80”.
@property (nonatomic,strong) NSArray <NSNumber *>*encodedMissingChunksList;//[@(node.address)]

/**
 * @brief   Initialize SigBLOBBlockStatus object.
 * @param   parameters    the hex data of SigBLOBBlockStatus.
 * @return  return `nil` when initialize SigBLOBBlockStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithStatus:(SigBLOBBlockStatusType)status RFU:(UInt8)RFU format:(SigBLOBBlockFormatType)format blockNumber:(UInt16)blockNumber chunkSize:(UInt16)chunkSize missingChunksList:(NSArray <NSNumber *>*)missingChunksList encodedMissingChunksList:(NSArray <NSNumber *>*)encodedMissingChunksList;

@end


#pragma mark 7.3.1.10 BLOB Information Get, opcode:0xB70A


/// BLOB Information Get is an acknowledged message used to get
/// the Capabilities state of the BLOB Transfer Server.
/// @note   The response to the BLOB Information Get message is
/// a BLOB Information Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.32),
/// 7.3.1.10 BLOB Information Get.
@interface SigBLOBInformationGet : SigAcknowledgedGenericMessage

/**
 * @brief   Initialize SigBLOBInformationGet object.
 * @param   parameters    the hex data of SigBLOBInformationGet.
 * @return  return `nil` when initialize SigBLOBInformationGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 7.3.1.11 BLOB Information Status, opcode:0xB70B


/// BLOB Information Get is an acknowledged message used to get
/// the Capabilities state of the BLOB Transfer Server.
/// @note   The response to the BLOB Information Get message is
/// a BLOB Information Status message.
/// @note   There are no parameters for this message.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.32),
/// 7.3.1.11 BLOB Information Status.
@interface SigBLOBInformationStatus : SigGenericMessage
/// Minimum block size: 2 ^ Min Block Size Log
/// The Min Block Size Log field shall indicate the Min Block Size Log
/// state (see Section 7.2.2.7.1).
@property (nonatomic,assign) UInt8 minBlockSizeLog;
/// Maximum block size: 2 ^ Max Block Size Log
/// The Max Block Size Log field shall indicate the Max Block Size Log
/// state (see Section 7.2.2.7.2).
@property (nonatomic,assign) UInt8 maxBlockSizeLog;
/// Supported maximum number of chunks in block
/// The Max Total Chunks field shall indicate the Max Total Chunks state
/// (see Section 7.2.2.7.3).
@property (nonatomic,assign) UInt16 maxChunksNumber;
/// Maximum size of chunk supported by the server
/// The Max Chunk Size field shall indicate the Max Chunk Size state
/// (see Section 7.2.2.7.4).
@property (nonatomic,assign) UInt16 maxChunkSize;
/// Maximum BLOB size supported by the server
/// The Max BLOB Size field shall indicate the Max BLOB Size state
/// (see Section 7.2.2.7.5).
@property (nonatomic,assign) UInt32 maxBLOBSize;
/// Maximum payload size supported by the server
/// The Server MTU Size field shall indicate the Server MTU Size state
/// (see Section 7.2.2.7.6).
@property (nonatomic,assign) UInt16 MTUSize;
/// BLOB transfer modes supported by the server
/// The Supported Transfer Mode field shall indicate the Supported Transfer Mode state (see Section 7.2.2.7.7).
@property (nonatomic,assign) struct SigSupportedTransferMode supportedTransferMode;

/**
 * @brief   Initialize SigBLOBInformationStatus object.
 * @param   parameters    the hex data of SigBLOBInformationStatus.
 * @return  return `nil` when initialize SigBLOBInformationStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithMinBlockSizeLog:(UInt8)minBlockSizeLog maxBlockSizeLog:(UInt8)maxBlockSizeLog maxChunksNumber:(UInt16)maxChunksNumber maxChunkSize:(UInt16)maxChunkSize maxBLOBSize:(UInt32)maxBLOBSize MTUSize:(UInt16)MTUSize supportedTransferMode:(struct SigSupportedTransferMode)supportedTransferMode;

@end


#pragma mark - 4.3.X Bridge messages
/*
 The Bridge messages are used to configure the behavior of
 a Subnet Bridge node.
 The Bridge messages shall be encrypted and authenticated
 using the DevKey of the Subnet Bridge node.
 */


#pragma mark 4.3.X.1 SUBNET_BRIDGE_GET, opcode:0xBF70


/// The SUBNET_BRIDGE_GET message is an acknowledged message used to get
/// the current Subnet Bridge state of a node (see Section 4.2.X).
/// @note   The response to a SUBNET_BRIDGE_GET message is a
/// SUBNET_BRIDGE_STATUS message.
/// @note   This message has no parameters.
/// @note   - seeAlso: MshPRF_SBR_CR_r03.pdf (page.10),
/// 4.3.X.1 SUBNET_BRIDGE_GET.
@interface SigSubnetBridgeGet : SigConfigMessage

/**
 * @brief   Initialize SigSubnetBridgeGet object.
 * @param   parameters    the hex data of SigSubnetBridgeGet.
 * @return  return `nil` when initialize SigSubnetBridgeGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.X.2 SUBNET_BRIDGE_SET, opcode:0xBF71


/// The SUBNET_BRIDGE_SET message is an acknowledged message used to set
/// the Subnet Bridge state of a node (see Section 4.2.X).
/// @note   The response to a SUBNET_BRIDGE_SET message is a
/// SUBNET_BRIDGE_STATUS message.
/// @note   - seeAlso: MshPRF_SBR_CR_r03.pdf (page.10),
/// 4.3.X.2 SUBNET_BRIDGE_SET.
@interface SigSubnetBridgeSet : SigConfigMessage
/// New Subnet Bridge state.
/// The Subnet_Bridge field determines the new Subnet Bridge state for the node
/// as defined in Section 4.2.X.
@property (nonatomic,assign) SigSubnetBridgeStateValues subnetBridge;

/**
 * @brief   Initialize SigSubnetBridgeSet object.
 * @param   parameters    the hex data of SigSubnetBridgeSet.
 * @return  return `nil` when initialize SigSubnetBridgeSet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithSubnetBridge:(SigSubnetBridgeStateValues)subnetBridge;

@end


#pragma mark 4.3.X.3 SUBNET_BRIDGE_STATUS, opcode:0xBF72


/// The SUBNET_BRIDGE_STATUS message is an unacknowledged message
/// used to report the current Subnet Bridge state of a node (see Section 4.2.X).
/// @note   - seeAlso: MshPRF_SBR_CR_r03.pdf (page.10),
/// 4.3.X.3 SUBNET_BRIDGE_STATUS.
@interface SigSubnetBridgeStatus : SigGenericMessage
/// Current Subnet Bridge state, size is 8 bytes.
/// The Subnet_Bridge field indicates the current Subnet Bridge state of the node
/// as defined in Section 4.2.X.
@property (nonatomic,strong) SigSubnetBridgeModel *subnetBridge;

/**
 * @brief   Initialize SigSubnetBridgeStatus object.
 * @param   parameters    the hex data of SigSubnetBridgeStatus.
 * @return  return `nil` when initialize SigSubnetBridgeStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithSubnetBridge:(SigSubnetBridgeModel *)subnetBridge;

@end


#pragma mark 4.3.X.4 BRIDGING_TABLE_ADD, opcode:0xBF73


/// The BRIDGING_TABLE_ADD message is an acknowledged message used to add
/// entries to the Bridging Table state (see Section 4.2.X+1) of a Subnet Bridge node.
/// @note   The response to a BRIDGING_TABLE_ADD message is a
/// BRIDGING_TABLE_STATUS message.
/// @note   - seeAlso: MshPRF_SBR_CR_r03.pdf (page.10),
/// 4.3.X.4 BRIDGING_TABLE_ADD.
@interface SigBridgeTableAdd : SigConfigMessage
/// Current Subnet Bridge state, size is 8 bytes.
@property (nonatomic,strong) SigSubnetBridgeModel *subnetBridge;

/**
 * @brief   Initialize SigBridgeTableAdd object.
 * @param   parameters    the hex data of SigBridgeTableAdd.
 * @return  return `nil` when initialize SigBridgeTableAdd object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithSubnetBridge:(SigSubnetBridgeModel *)subnetBridge;

@end


#pragma mark 4.3.X.5 BRIDGING_TABLE_REMOVE, opcode:0xBF74


/// The BRIDGING_TABLE_REMOVE message is an acknowledged message
/// used to remove entries from the Bridging Table state (see Section 4.2.X+1)
/// of a Subnet Bridge node.
/// @note   The response to a BRIDGING_TABLE_REMOVE message is a
/// BRIDGING_TABLE_STATUS message.
/// @note   - seeAlso: MshPRF_SBR_CR_r03.pdf (page.10),
/// 4.3.X.5 BRIDGING_TABLE_REMOVE.
@interface SigBridgeTableRemove : SigConfigMessage
/// NetKey Index of the first subnet, size is 12 bits.
/// Each of the NetKeyIndex1 and NetKeyIndex2 fields is the global NetKey
/// Index of the NetKey associated with one of the subnets.
@property (nonatomic, assign) UInt16 netKeyIndex1;
/// NetKey Index of the second subnet, size is 12 bits.
/// Each of the NetKeyIndex1 and NetKeyIndex2 fields is the global NetKey
/// Index of the NetKey associated with one of the subnets.
@property (nonatomic, assign) UInt16 netKeyIndex2;
/// Address of the node in the first subnet, size is 16 bits.
/// The Address1 and Address2 fields identify the addresses of the nodes in
/// the first subnet and in the second subnet, respectively.
/// The Address1 field value shall be the unassigned address or a unicast address.
@property (nonatomic, assign) UInt16 address1;
/// Address of the node in the second subnet, size is 16 bits.
/// The Address1 and Address2 fields identify the addresses of the nodes in
/// the first subnet and in the second subnet, respectively.
/// The Address2 field value shall not be the all-nodes fixed group address.
@property (nonatomic, assign) UInt16 address2;

/**
 * @brief   Initialize SigBridgeTableRemove object.
 * @param   parameters    the hex data of SigBridgeTableRemove.
 * @return  return `nil` when initialize SigBridgeTableRemove object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithNetKeyIndex1:(UInt16)netKeyIndex1 netKeyIndex2:(UInt16)netKeyIndex2 address1:(UInt16)address1 address2:(UInt16)address2;

@end


#pragma mark 4.3.X.6 BRIDGING_TABLE_STATUS, opcode:0xBF75


/// The BRIDGING_TABLE_STATUS message is an unacknowledged message
/// used to report the status of the most recent operation on the Bridging Table
/// state (see Section 4.2.X+1) of a Subnet Bridge node.
/// @note   - seeAlso: MshPRF_SBR_CR_r03.pdf (page.12),
/// 4.3.X.6 BRIDGING_TABLE_STATUS.
@interface SigBridgeTableStatus : SigGenericMessage
/// Status Code for the requesting message.
/// The Status field reports the Status Code for the most recent operation on the
/// Bridging Table state. The Status codes are defined in Section 4.3.5.
@property (nonatomic, assign) SigConfigMessageStatus status;
/// Current Subnet Bridge state, size is 8 bytes.
@property (nonatomic,strong) SigSubnetBridgeModel *subnetBridge;

/**
 * @brief   Initialize SigBridgeTableStatus object.
 * @param   parameters    the hex data of SigBridgeTableStatus.
 * @return  return `nil` when initialize SigBridgeTableStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithStatus:(SigConfigMessageStatus)status subnetBridge:(SigSubnetBridgeModel *)subnetBridge;

@end


#pragma mark 4.3.X.7 BRIDGED_SUBNETS_GET, opcode:0xBF76


/// The BRIDGED_SUBNETS_GET message is an acknowledged message used to get
/// a filtered set of pairs of NetKey Indexes (see Table 4.Y+12) extracted from the
/// Bridging Table state entries of a Subnet Bridge node.
/// @note   The response to a BRIDGED_SUBNETS_GET message is a
/// BRIDGED_SUBNETS_LIST message.
/// @note   - seeAlso: MshPRF_SBR_CR_r03.pdf (page.13),
/// 4.3.X.7 BRIDGED_SUBNETS_GET.
@interface SigBridgeSubnetsGet : SigConfigMessage
/// Filter to be applied when reporting the set of pairs of NetKey Indexes, size is 2 bits.
/// The Filter field determines the filtering to be applied when reporting the set of
/// pairs of NetKey Indexes extracted from the Bridging Table state entries in the
/// response message.
@property (nonatomic, assign) SigFilterFieldValues filter;
/// Prohibited, size is 2 bits.
@property (nonatomic, assign) UInt8 prohibited;
/// NetKey Index of any of the subnets, size is 12 bits.
/// The NetKeyIndex field is the global NetKey Index of the NetKey to be used for
/// filtering if the Filter field value is different from 0b00.
@property (nonatomic, assign) UInt16 netKeyIndex;
/// Start offset in units of pairs of NetKey Indexes to read, size is 8 bits.
/// The Start_Index field determines the offset in units of pairs of NetKey Indexes
///  (see Table 4.Y+12) to start from when reporting the filtered set of pairs of
///  NetKey Indexes extracted from the Bridging Table state entries of a Subnet Bridge.
@property (nonatomic, assign) UInt8 startIndex;

/**
 * @brief   Initialize SigBridgeSubnetsGet object.
 * @param   parameters    the hex data of SigBridgeSubnetsGet.
 * @return  return `nil` when initialize SigBridgeSubnetsGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithFilter:(SigFilterFieldValues)filter prohibited:(UInt8)prohibited netKeyIndex:(UInt16)netKeyIndex startIndex:(UInt8)startIndex;

@end


#pragma mark 4.3.X.8 BRIDGED_SUBNETS_LIST, opcode:0xBF77


/// The BRIDGED_SUBNETS_LIST message is an unacknowledged message used
/// to report a filtered set of pairs of NetKey Indexes extracted from the Bridging
/// Table state entries of a Subnet Bridge node.
/// @note   This message is a response to the BRIDGED_SUBNETS_GET message.
/// @note   - seeAlso: MshPRF_SBR_CR_r03.pdf (page.14),
/// 4.3.X.8 BRIDGED_SUBNETS_LIST.
@interface SigBridgeSubnetsList : SigGenericMessage
/// Filter applied to the set of pairs of NetKey Indexes, size is 2 bits.
/// The Filter field indicates the filtering applied when reporting the set of pairs of
/// NetKey Indexes extracted from the Bridging Table state entries of the Subnet
/// Bridge. Table 4.Y+10 defines the Filter field values.
@property (nonatomic, assign) SigFilterFieldValues filter;
/// Prohibited, size is 2 bits.
@property (nonatomic, assign) UInt8 prohibited;
/// NetKey Index used for filtering or ignored, size is 12 bits.
/// If used for filtering, the NetKeyIndex field is the global NetKey Index of the NetKey
/// associated with one of the subnets.
@property (nonatomic, assign) UInt16 netKeyIndex;
/// Start offset in units of bridges.
/// The Start_Index field indicates the offset in units of pairs of NetKey Indexes (see
/// Table 4.Y+12) to start from when reporting the filtered set of pairs of NetKey Indexes
/// extracted from the Bridging Table state entries of the Subnet Bridge.
@property (nonatomic, assign) UInt8 startIndex;
/// Filtered set of N pairs of NetKey Indexes (optional), size is variable (24 * N).
/// If present, the Bridged_Subnets_List field contains the filtered set of pairs of NetKey
/// Indexes extracted from the Bridging Table state entries of the Subnet Bridge. Each
/// Bridged_Subnets_List field entry shall be formatted as defined in Table 4.Y+12.
@property (nonatomic, strong) NSArray <SigBridgeSubnetModel *>*bridgedSubnetsList;

/**
 * @brief   Initialize SigBridgeSubnetsList object.
 * @param   parameters    the hex data of SigBridgeSubnetsList.
 * @return  return `nil` when initialize SigBridgeSubnetsList object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithFilter:(SigFilterFieldValues)filter prohibited:(UInt8)prohibited netKeyIndex:(UInt16)netKeyIndex startIndex:(UInt8)startIndex bridgedSubnetsList:(NSArray <SigBridgeSubnetModel *>*)bridgedSubnetsList;

@end


#pragma mark 4.3.X.9 BRIDGING_TABLE_GET, opcode:0xBF78


/// The BRIDGING_TABLE_GET message is an acknowledged message used to get
/// the list of addresses and allowed traffic directions of the Bridging Table state
/// entries (see Section 4.2.X+1) of a Subnet Bridge node.
/// @note   The response to a BRIDGING_TABLE_GET message is a
/// BRIDGING_TABLE_LIST message.
/// @note   - seeAlso: MshPRF_SBR_CR_r03.pdf (page.14),
/// 4.3.X.9 BRIDGING_TABLE_GET.
@interface SigBridgeTableGet : SigConfigMessage
/// NetKey Index of the first subnet, size is 12 bits.
/// Each of the NetKeyIndex1 and NetKeyIndex2 fields is the global NetKey Index of
/// the NetKey associated with one of the subnets.
@property (nonatomic, assign) UInt16 netKeyIndex1;
/// NetKey Index of the second subnet, size is 12 bits.
/// Each of the NetKeyIndex1 and NetKeyIndex2 fields is the global NetKey Index of
/// the NetKey associated with one of the subnets.
@property (nonatomic, assign) UInt16 netKeyIndex2;
/// Start offset to read in units of Bridging Table state entries, size is 16 bits.
/// The Start_Index field determines the offset in units of Bridging Table state entries
/// (see Section 4.2.X+1) to start from when reporting the list of addresses and allowed
/// traffic directions of the Bridging Table state.
@property (nonatomic, assign) UInt16 startIndex;

/**
 * @brief   Initialize SigBridgeTableGet object.
 * @param   parameters    the hex data of SigBridgeTableGet.
 * @return  return `nil` when initialize SigBridgeTableGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithNetKeyIndex1:(UInt16)netKeyIndex1 netKeyIndex2:(UInt16)netKeyIndex2 startIndex:(UInt16)startIndex;

@end


#pragma mark 4.3.X.10 BRIDGING_TABLE_LIST, opcode:0xBF79


/// The BRIDGING_TABLE_LIST message is an unacknowledged message used to
/// report the list of addresses and allowed traffic directions of the Bridging Table
/// state entries (see Section 4.2.X+1) of a Subnet Bridge node.
/// @note   - seeAlso: MshPRF_SBR_CR_r03.pdf (page.15),
/// 4.3.X.10 BRIDGING_TABLE_LIST.
@interface SigBridgeTableList : SigGenericMessage
/// Status Code for the requesting message.
/// The Status field reports the Status Code for the most recent
/// BRIDGING_TABLE_GET message. The Status codes are defined in Section 4.3.6.
@property (nonatomic, assign) UInt8 status;
/// NetKey Index of the first subnet, size is 12 bits.
/// Each of the NetKeyIndex1 and NetKeyIndex2 fields is the global NetKey Index of
/// the NetKey associated with one of the subnets.
@property (nonatomic, assign) UInt16 netKeyIndex1;
/// NetKey Index of the second subnet, size is 12 bits.
/// Each of the NetKeyIndex1 and NetKeyIndex2 fields is the global NetKey Index of
/// the NetKey associated with one of the subnets.
@property (nonatomic, assign) UInt16 netKeyIndex2;
/// Start offset to read in units of Bridging Table state entries, size is 16 bits.
/// The Start_Index field indicates the offset in units of Bridging Table state entries (see
/// Section 4.2.X+1) used when reporting the list of addresses and allowed traffic
/// directions of the Bridging Table state.
@property (nonatomic, assign) UInt16 startIndex;
/// List of bridged addresses and allowed traffic directions (C.1). (C.1: If the value of the
/// Status field is Success, the Bridged_Addresses_List field shall be optional; otherwise,
/// the Bridged_Addresses_List field shall not be present.)
/// If present, the Bridged_Addresses_List field contains a portion of the addresses and
/// allowed traffic directions of the Bridging Table state entries. The format of the
/// Bridged_Addresses_List entry is defined in Table 4.Y+15.
@property (nonatomic, strong) NSArray <SigBridgedAddressesModel *>*bridgedAddressesList;

/**
 * @brief   Initialize SigBridgeTableList object.
 * @param   parameters    the hex data of SigBridgeTableList.
 * @return  return `nil` when initialize SigBridgeTableList object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

- (instancetype)initWithStatus:(UInt8)status netKeyIndex1:(UInt16)netKeyIndex1 netKeyIndex2:(UInt16)netKeyIndex2 startIndex:(UInt16)startIndex bridgedAddressesList:(NSArray <SigBridgedAddressesModel *>*)bridgedAddressesList;

@end


#pragma mark 4.3.11.11 BRIDGING_TABLE_SIZE_GET, opcode:0xBF7A


/// A BRIDGING_TABLE_SIZE_GET message is an acknowledged message used to get
/// the Bridging Table Size state (see Section 4.2.43) of a Subnet Bridge node.
/// @note   The response to a BRIDGING_TABLE_SIZE_GET message is a
/// BRIDGING_TABLE_SIZE_STATUS message.
/// @note   This message has no parameters.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.414),
/// 4.3.11.11 BRIDGING_TABLE_SIZE_GET.
@interface SigBridgingTableSizeGet : SigConfigMessage

/**
 * @brief   Initialize SigBridgingTableSizeGet object.
 * @param   parameters    the hex data of SigBridgingTableSizeGet.
 * @return  return `nil` when initialize SigBridgingTableSizeGet object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


#pragma mark 4.3.11.12 BRIDGING_TABLE_SIZE_STATUS, opcode:0xBF7B


/// A BRIDGING_TABLE_SIZE_STATUS message is an unacknowledged message used to
/// report the Bridging Table Size state (see Section 4.2.43) of a Subnet Bridge node.
/// @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.414),
/// 4.3.11.12 BRIDGING_TABLE_SIZE_STATUS.
@interface SigBridgingTableSizeStatus : SigGenericMessage
/// Bridging Table Size state.
/// The Bridging_Table_Size field indicates the Bridging Table Size state of the node,
/// as defined in Section 4.2.43.
/// The Bridging Table Size state is a 2-octet value indicating the maximum number of Bridging
/// Table state entries that the Subnet Bridge node can support. Multiple Bridging Table state
/// entries can be stored for a single pair of subnets.
/// The Bridging Table Size state value shall be at least 16.
@property (nonatomic, assign) UInt16 bridgingTableSize;

/**
 * @brief   Initialize SigBridgingTableSizeStatus object.
 * @param   parameters    the hex data of SigBridgingTableSizeStatus.
 * @return  return `nil` when initialize SigBridgingTableSizeStatus object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/**
 * @brief   Initialize SigBridgingTableSizeStatus object.
 * @param   bridgingTableSize    Bridging Table Size state.
 * The Bridging_Table_Size field indicates the Bridging Table Size state of the node,
 * as defined in Section 4.2.43.
 * The Bridging Table Size state is a 2-octet value indicating the maximum number of Bridging
 * Table state entries that the Subnet Bridge node can support. Multiple Bridging Table state
 * entries can be stored for a single pair of subnets.
 * The Bridging Table Size state value shall be at least 16.
 * @return  return `nil` when initialize SigBridgingTableSizeStatus object fail.
 */
- (instancetype)initWithBridgingTableSize:(UInt16)bridgingTableSize;

@end


@interface SigTelinkOnlineStatusMessage : SigMeshMessage
@property (nonatomic,assign) UInt16 address;
@property (nonatomic,assign) DeviceState state;
@property (nonatomic,assign) UInt8 brightness;
@property (nonatomic,assign) UInt8 temperature;
- (instancetype)initWithAddress:(UInt16)address state:(DeviceState)state brightness:(UInt8)brightness temperature:(UInt8)temperature;
@end


NS_ASSUME_NONNULL_END
