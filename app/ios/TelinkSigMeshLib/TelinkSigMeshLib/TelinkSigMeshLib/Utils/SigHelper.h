/********************************************************************************************************
 * @file     SigHelper.h
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
//  SigHelper.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/8/15.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface SigHelper : NSObject


+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));


+ (SigHelper *)share;

- (BOOL)isValidAddress:(UInt16)address;
- (BOOL)isUnassignedAddress:(UInt16)address;
- (BOOL)isUnicastAddress:(UInt16)address;
- (BOOL)isVirtualAddress:(UInt16)address;
- (BOOL)isGroupAddress:(UInt16)address;

- (int)getPeriodFromSteps:(SigStepResolution)steps;

- (NSString *)getNodeAddressString:(UInt16)address;
- (NSString *)getUint16String:(UInt16)address;
- (NSString *)getUint32String:(UInt32)address;
- (NSString *)getUint64String:(UInt64)address;

- (float)getRandomfromA:(float)a toB:(float)b;

/// The TTL field is a 7-bit field. The following values are defined:
/// • 0 = has not been relayed and will not be relayed
/// • 1 = may have been relayed, but will not be relayed
/// • 2 to 126 = may have been relayed and can be relayed
/// • 127 = has not been relayed and can be relayed
///
/// @param ttl TTL (Time To Live)
- (BOOL)isValidTTL:(UInt8)ttl;
- (BOOL)isRelayedTTL:(UInt8)ttl;

- (UInt16)getUint16LightnessFromUInt8Lum:(UInt8)lum;
- (UInt8)getUInt8LumFromUint16Lightness:(UInt16)lightness;
- (SInt16)getSInt16LevelFromUInt8Lum:(UInt8)lum;
- (UInt8)getUInt8LumFromSInt16Level:(SInt16)level;
- (UInt16)getUint16TemperatureFromUInt8Temperature100:(UInt8)temperature100;
/// use for driver pwm, 0--100 is absolute value, not related to temp range
- (UInt8)getUInt8Temperature100HWFromUint16Temperature:(UInt16)temperature;
- (UInt8)getUInt8Temperature100FromUint16Temperature:(UInt16)temperature;
- (UInt32)getDivisionRoundWithValue:(UInt32)value dividend:(UInt32)dividend;
- (UInt16)getUInt16FromSInt16:(SInt16)s16;
- (SInt16)getSInt16FromUInt16:(UInt16)u16;
- (UInt8)getOnOffFromeSInt16Level:(SInt16)level;
- (UInt16)getUInt16LightnessFromSInt16Level:(SInt16)level;
- (SInt16)getSInt16LevelFromUInt16Lightness:(UInt16)lightness;

/// response opcode, eg://SigOpCode_configAppKeyGet:0x8001->SigOpCode_configAppKeyList:0x8002
/// @param sendOpcode opcode of send command.
- (int)getResponseOpcodeWithSendOpcode:(int)sendOpcode;


/// Yes means message need response, No means needn't response.
/// @param message message
- (BOOL)isAcknowledgedMessage:(SigMeshMessage *)message;

@end

NS_ASSUME_NONNULL_END
