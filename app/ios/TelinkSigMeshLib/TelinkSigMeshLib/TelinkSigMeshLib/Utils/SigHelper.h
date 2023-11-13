/********************************************************************************************************
 * @file     SigHelper.h
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/8/15
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

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface SigHelper : NSObject


+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));


/**
 *  @brief  Singleton method
 *
 *  @return the default singleton instance. You are not allowed to create your own instances of this class.
 */
+ (instancetype)share;

/**
 * @brief   Determine whether the address is a valid address.(inValid address range is 0xFF00~0xFFF8)
 * @param   address    The network layer defines four basic types of addresses: unassigned, unicast, virtual, and group.
 * @return  YES means address is valid address, NO means address is invalid.
 * @note    3.4.2 Addresses, seeAlso: MshPRT_v1.1.pdf  (page.54)
 */
- (BOOL)isValidAddress:(UInt16)address;

/**
 * @brief   Determine whether the address is a unassigned address.(unassigned address is 0)
 * @param   address    The network layer defines four basic types of addresses: unassigned, unicast, virtual, and group.
 * @return  YES means address is unassigned address, NO means address is not unassigned address.
 * @note    3.4.2 Addresses, seeAlso: MshPRT_v1.1.pdf  (page.54)
 */
- (BOOL)isUnassignedAddress:(UInt16)address;

/**
 * @brief   Determine whether the address is a unicast address.(The range of unicast address is 0x0001~0x7FFF)
 * @param   address    The network layer defines four basic types of addresses: unassigned, unicast, virtual, and group.
 * @return  YES means address is unicast address, NO means address is not unicast address.
 * @note    3.4.2 Addresses, seeAlso: MshPRT_v1.1.pdf  (page.54)
 */
- (BOOL)isUnicastAddress:(UInt16)address;

/**
 * @brief   Determine whether the address is a virtual address.(The range of virtual address is 0x8000~0xBFFF)
 * @param   address    The network layer defines four basic types of addresses: unassigned, unicast, virtual, and group.
 * @return  YES means address is virtual address, NO means address is not virtual address.
 * @note    3.4.2 Addresses, seeAlso: MshPRT_v1.1.pdf  (page.54)
 */
- (BOOL)isVirtualAddress:(UInt16)address;

/**
 * @brief   Determine whether the address is a group address.(The range of group address is 0xC000~0xEFFF)
 * @param   address    The network layer defines four basic types of addresses: unassigned, unicast, virtual, and group.
 * @return  YES means address is group address, NO means address is not group address.
 * @note    3.4.2 Addresses, seeAlso: MshPRT_v1.1.pdf  (page.54)
 */
- (BOOL)isGroupAddress:(UInt16)address;

/// Get Milliseconds Of Transition Interval.
/// @param periodSteps The Default Transition Number of Steps field is a 6-bit value representing the number of transition steps. The field values represent the states defined in the following table.
/// Value               Description
/// 0x00                The Generic Default Transition Time is immediate.
/// 0x01–0x3E      The number of steps.
/// 0x3E               The value is unknown. The state cannot be set to this value, but an element 0x3F may report an unknown value if a transition is higher than or not determined.
/// @param periodResolution The Default Transition Step Resolution field is a 2-bit bit field that determines the resolution of the Generic Default Transition Time state.
- (int)getMillisecondsOfTransitionIntervalWithPeriodSteps:(UInt8)periodSteps periodResolution:(SigStepResolution)periodResolution;

/**
 * @brief   Change UInt16 node address to hexadecimal string.
 * @param   address    The unicastAddress of node.
 * @return  The hexadecimal string of node address.
 */
- (NSString *)getNodeAddressString:(UInt16)address;

/**
 * @brief   Change UInt16 number to hexadecimal string.
 * @param   address    The UInt16 number address.
 * @return  The hexadecimal string of UInt16 number address.
 */
- (NSString *)getUint16String:(UInt16)address;

/**
 * @brief   Change UInt32 number to hexadecimal string.
 * @param   address    The UInt32 number address.
 * @return  The hexadecimal string of UInt32 number address.
 */
- (NSString *)getUint32String:(UInt32)address;

/**
 * @brief   Change UInt64 number to hexadecimal string.
 * @param   address    The UInt64 number address.
 * @return  The hexadecimal string of UInt64 number address.
 */
- (NSString *)getUint64String:(UInt64)address;

/// Randomly generate floating point numbers directly from float number A and float number B, retaining 3 valid decimal places.
/// @param a float number A
/// @param b float number B
- (float)getRandomfromA:(float)a toB:(float)b;

/// The TTL field is a 7-bit field. The following values are defined:
/// • 0 = has not been relayed and will not be relayed
/// • 1 = may have been relayed, but will not be relayed
/// • 2 to 126 = may have been relayed and can be relayed
/// • 127 = has not been relayed and can be relayed
///
/// @param ttl TTL (Time To Live)
- (BOOL)isValidTTL:(UInt8)ttl;

/// The TTL field is a 7-bit field. The following values are defined:
/// • 0 = has not been relayed and will not be relayed
/// • 1 = may have been relayed, but will not be relayed
/// • 2 to 126 = may have been relayed and can be relayed
/// • 127 = has not been relayed and can be relayed
///
/// @param ttl TTL (Time To Live)
- (BOOL)isRelayedTTL:(UInt8)ttl;

/// Get response opcode with send opcode, eg://SigOpCode_configAppKeyGet:0x8001->SigOpCode_configAppKeyList:0x8002
/// @param sendOpcode opcode of send command.
- (int)getResponseOpcodeWithSendOpcode:(int)sendOpcode;

/// Get mesh message class with opcode
/// @param opCode opcode of send command.
- (_Nullable Class)getMeshMessageWithOpCode:(SigOpCode)opCode;

/// Get SigOpCodeType with height byte opcode
/// @param opCode opcode of mesh message.
- (SigOpCodeType)getOpCodeTypeWithOpcode:(UInt8)opCode;

/// Get UInt8 OpCode Type
/// @param opCode 1-octet Opcodes,eg:0x00; 2-octet Opcodes,eg:0x8201; 3-octet Opcodes,eg:0xC11102
- (SigOpCodeType)getOpCodeTypeWithUInt32Opcode:(UInt32)opCode;

/// Get OpCode Data
/// @param opCode 1-octet Opcodes,eg:0x00; 2-octet Opcodes,eg:0x8201; 3-octet Opcodes,eg:0xC11102
- (NSData *)getOpCodeDataWithUInt32Opcode:(UInt32)opCode;

/// opcode is encryption with deviceKey.
/// @param opCode 1-octet Opcodes,eg:0x00; 2-octet Opcodes,eg:0x8201; 3-octet Opcodes,eg:0xC11102
- (BOOL)isDeviceKeyOpCode:(UInt32)opCode;

/// Yes means message need response, No means needn't response.
/// @param message message
- (BOOL)isAcknowledgedMessage:(SigMeshMessage *)message;

/// Get Detail Of SigFirmwareUpdatePhaseType
/// @param phaseType The SigFirmwareUpdatePhaseType value.
- (NSString *)getDetailOfSigFirmwareUpdatePhaseType:(SigFirmwareUpdatePhaseType)phaseType;

/// Get Detail Of SigFirmwareUpdateServerAndClientModelStatusType
/// @param statusType The SigFirmwareUpdateServerAndClientModelStatusType value.
- (NSString *)getDetailOfSigFirmwareUpdateServerAndClientModelStatusType:(SigFirmwareUpdateServerAndClientModelStatusType)statusType;

/// Get Detail Of SigBLOBTransferStatusType
/// @param statusType The SigBLOBTransferStatusType value.
- (NSString *)getDetailOfSigBLOBTransferStatusType:(SigBLOBTransferStatusType)statusType;

/// Get Detail Of SigDirectionsFieldValues
/// @param directions The SigDirectionsFieldValues value.
- (NSString *)getDetailOfSigDirectionsFieldValues:(SigDirectionsFieldValues)directions;

/// Get Detail Of SigNodeFeaturesState
/// @param state The SigNodeFeaturesState value.
- (NSString *)getDetailOfSigNodeFeaturesState:(SigNodeFeaturesState)state;

/// Get Detail Of KeyRefreshPhase
/// @param phase The KeyRefreshPhase value.
- (NSString *)getDetailOfKeyRefreshPhase:(KeyRefreshPhase)phase;

/// Get Detail Of SigNodeIdentityState
/// @param state The SigNodeIdentityState value.
- (NSString *)getDetailOfSigNodeIdentityState:(SigNodeIdentityState)state;

@end

NS_ASSUME_NONNULL_END
