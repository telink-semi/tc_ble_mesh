/********************************************************************************************************
 * @file     LibTools.h 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
//
//  LibTools.h
//  SigMeshOC
//
//  Created by Liangjiazhi on 2018/10/12.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface LibTools : NSObject

/**
 把NSString转成可写入蓝牙设备的Hex Data

 @param string 原始字符串数据
 @return 返回data
 */
+ (NSData *)nsstringToHex:(NSString *)string;

/**
 NSData 转  十六进制string
 
 @return NSString类型的十六进制string
 */
+ (NSString *)convertDataToHexStr:(NSData *)data;

///NSData字节翻转
+ (NSData *)turnOverData:(NSData *)data;

/**
 计算2000.1.1 00:00:00 到现在的秒数
 
 @return 返回2000.1.1 00:00:00 到现在的秒数
 */
+ (NSInteger )secondsFrome2000;

///返回手机当前时间的时区
+ (NSInteger)currentTimeZoon;

+ (NSString *)getNowTimeTimestamp;

+ (NSString *)getNowTimeTimestampFrome2000;

+ (NSData *)createNetworkKey;

+ (NSData *)initAppKey;

+ (NSData *)createRandomDataWithLength:(NSInteger)length;

+ (NSData *)initMeshUUID;

+ (long long)NSDataToUInt:(NSData *)data;

///返回带冒号的mac
+ (NSString *)getMacStringWithMac:(NSString *)mac;

///NSData转Uint8
+ (UInt8)uint8FromBytes:(NSData *)fData;

//NSData转Uint16
+ (UInt16)uint16FromBytes:(NSData *)fData;

///NSData转Uint32
+ (UInt32)uint32FromBytes:(NSData *)fData;

///NSData转Uint64
+ (UInt64)uint64FromBytes:(NSData *)fData;

///16进制NSString转Uint8
+ (UInt8)uint8From16String:(NSString *)string;

///16进制NSString转Uint16
+ (UInt16)uint16From16String:(NSString *)string;

///16进制NSString转Uint32
+ (UInt32)uint32From16String:(NSString *)string;

///16进制NSString转Uint64
+ (UInt64)uint64From16String:(NSString *)string;

///D7C5BD18-4282-F31A-0CE0-0468BC0B8DE8 -> D7C5BD184282F31A0CE00468BC0B8DE8
+ (NSString *)meshUUIDToUUID:(NSString *)uuid;

///D7C5BD184282F31A0CE00468BC0B8DE8 -> D7C5BD18-4282-F31A-0CE0-0468BC0B8DE8
+ (NSString *)UUIDToMeshUUID:(NSString *)meshUUID;

//SDK的版本号
+ (NSString *)getSDKVersion;

+ (float)floatWithdecimalNumber:(double)num;

+ (double)doubleWithdecimalNumber:(double)num;

+ (NSString *)stringWithDecimalNumber:(double)num;

+ (NSDecimalNumber *)decimalNumber:(double)num;

+ (NSString *)getUint32String:(UInt32)address;
+ (NSString *)getUint64String:(UInt64)address;

#pragma mark - JSON相关

/**
 *  字典数据转换成JSON字符串（没有可读性）
 *
 *  @param dictionary 待转换的字典数据
 *  @return JSON字符串
 */
+ (NSString *)getJSONStringWithDictionary:(NSDictionary *)dictionary;
 
/**
 *  字典数据转换成JSON字符串（有可读性）
 *
 *  @param dictionary 待转换的字典数据
 *  @return JSON字符串
 */
+ (NSString *)getReadableJSONStringWithDictionary:(NSDictionary *)dictionary;
 
/**
 *  字典数据转换成JSON数据
 *
 *  @param dictionary 待转换的字典数据
 *  @return JSON数据
 */
+ (NSData *)getJSONDataWithDictionary:(NSDictionary *)dictionary;

/**
*  NSData数据转换成字典数据
*
*  @param data 待转换的NSData数据
*  @return 字典数据
*/
+(NSDictionary*)getDictionaryWithJSONData:(NSData*)data;

/**
*  JSON字符串转换成字典数据
*
*  @param jsonString 待转换的JSON字符串
*  @return 字典数据
*/
+ (NSDictionary *)getDictionaryWithJsonString:(NSString *)jsonString;

@end

NS_ASSUME_NONNULL_END
