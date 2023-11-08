/********************************************************************************************************
 * @file     LibTools.h 
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/10/12
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

@interface LibTools : NSObject

/**
 把NSString转成可写入蓝牙设备的Hex Data

 @param string 原始字符串数据
 @return 返回data
 */
+ (NSData *)nsstringToHex:(NSString *)string;

/**
 NSData 转  十六进制string(大写)
 
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

/// 返回手机当前时间的时区
+ (NSInteger)currentTimeZoon;

/// 返回手机当前时间的时间戳
+ (NSString *)getNowTimeTimestamp;

/// 返回当前时间字符串格式："yyyy-MM-dd HH:mm:ss"
+ (NSString *)getNowTimeTimeString;

/// 返回当前时间字符串格式："YYYY-MM-ddThh:mm:ssX"，eg: @"2021-10-08T08:33:16Z". 如果要使用该特定格式，则必须将时区设置为UTC.当时北京时间为2021-10-08T16:33:16。
+ (NSString *)getNowTimeStringOfJson;

/// Time string to NSDate
/// - Parameter timeString: time string of json file, 时间字符串格式："YYYY-MM-ddThh:mm:ssX"，eg: @"2021-10-08T08:33:16Z".
+ (NSDate *)getDateWithTimeStringOfJson:(NSString *)timeString;

/// 返回json中的SigStepResolution对应的毫秒数，只有四种值：100,1000,10000,600000.
+ (NSInteger)getSigStepResolutionInMillisecondsOfJson:(SigStepResolution)resolution;

/// Create 16 bytes hex as network key.
+ (NSData *)createNetworkKey;

/// Create 16 bytes hex as app key.
+ (NSData *)initAppKey;

/// Create 16 bytes hex as mesh UUID.
+ (NSData *)initMeshUUID;

/// Create any length hex data.
/// - Parameter length: The length value of random data.
+ (NSData *)createRandomDataWithLength:(NSInteger)length;

/// 返回带冒号的mac
+ (NSString *)getMacStringWithMac:(NSString *)mac;

/// NSData转Uint8
+ (UInt8)uint8FromBytes:(NSData *)fData;

/// NSData转Uint16
+ (UInt16)uint16FromBytes:(NSData *)fData;

/// NSData转Uint32
+ (UInt32)uint32FromBytes:(NSData *)fData;

/// NSData转Uint64
+ (UInt64)uint64FromBytes:(NSData *)fData;

/// 16进制NSString转Uint8
+ (UInt8)uint8From16String:(NSString *)string;

/// 16进制NSString转Uint16
+ (UInt16)uint16From16String:(NSString *)string;

/// 16进制NSString转Uint32
+ (UInt32)uint32From16String:(NSString *)string;

/// 16进制NSString转Uint64
+ (UInt64)uint64From16String:(NSString *)string;

/// Get Virtual Address Of LabelUUID
/// - Parameter string: The LabelUUID string.
+ (UInt16)getVirtualAddressOfLabelUUID:(NSString *)string;

/// D7C5BD18-4282-F31A-0CE0-0468BC0B8DE8 -> D7C5BD184282F31A0CE00468BC0B8DE8
+ (NSString *)meshUUIDToUUID:(NSString *)uuid;

/// D7C5BD184282F31A0CE00468BC0B8DE8 -> D7C5BD18-4282-F31A-0CE0-0468BC0B8DE8
+ (NSString *)UUIDToMeshUUID:(NSString *)meshUUID;

/// xxxx -> 0000xxxx-0000-1000-8000-008505f9b34fb or xxxxxxxx -> xxxxxxxx-0000-1000-8000-008505f9b34fb
+ (NSString *)change16BitsUUIDTO128Bits:(NSString *)uuid;

/// Change Uint16 Lightness to UInt8 Lum.
/// @param lightness ligheness is the ligheness value that send to node by SDKCommand, range is 0x0000~0xFFFF.
+ (UInt8)lightnessToLum:(UInt16)lightness;

/// Change UInt8 Lum to Uint16 Lightness.
/// @param lum luminosity is the ligheness value that show in UI, range is 0~100.
+ (UInt16)lumToLightness:(UInt8)lum;

/// Change Uint16 Temperature to UInt8 Temperature100.
/// @param temp temperature value that send to node by SDKCommand, range is 0x0000~0xFFFF.
+ (UInt8)tempToTemp100:(UInt16)temp;

/// Change UInt8 Temperature100 to Uint16 Temperature.
/// @param temp100 temperature value that show in UI, range is 0~100.
+ (UInt16)temp100ToTemp:(UInt8)temp100;

/// Change SInt16 Level to UInt8 Lum.
/// @param level The Generic Level state is a 16-bit signed integer (2’s complement) representing the state of an element. , range is 0x0000–0xFFFF.
+ (UInt8)levelToLum:(SInt16)level;

/// Change UInt8 Lum to SInt16 Level.
/// @param lum luminosity is the ligheness value that show in UI, range is 0~100.
+ (SInt16)lumToLevel:(UInt8)lum;

/// Change Uint16 Lightness to SInt16 Level.
/// @param lightness ligheness is the ligheness value that send to node by SDKCommand, range is 0x0000~0xFFFF.
+ (SInt16)lightnessToLevel:(UInt16)lightness;

/// Change SInt16 Level to Uint16 Lightness.
/// @param level The Generic Level state is a 16-bit signed integer (2’s complement) representing the state of an element. , range is 0x0000–0xFFFF.
+ (UInt16)levelToLightness:(SInt16)level;

/// Change UInt16 value to Sint16 value.
/// @param val The UInt16 value.
+ (SInt16)uInt16ToSInt16:(UInt16)val;

/// Change Sint16 value to UInt16 value.
/// @param val The Sint16 value.
+ (SInt16)sint16ToUInt16:(SInt16)val;

///（四舍五入，保留两位小数）
+ (float)roundFloat:(float)price;

/// 通过周期对象SigPeriodModel获取周期时间，单位为秒。
+ (SigStepResolution)getSigStepResolutionWithSigPeriodModel:(SigPeriodModel *)periodModel;

#pragma mark - JSON相关

/**
 *  字典数据转换成JSON字符串（没有可读性）
 *
 *  @param dictionary 待转换的字典数据
 *  @return JSON字符串
 */
+ (nullable NSString *)getJSONStringWithDictionary:(NSDictionary *)dictionary;
 
/**
 *  字典数据转换成JSON字符串（有可读性）
 *
 *  @param dictionary 待转换的字典数据
 *  @return JSON字符串
 */
+ (nullable NSString *)getReadableJSONStringWithDictionary:(NSDictionary *)dictionary;
 
/**
 *  字典数据转换成JSON数据
 *
 *  @param dictionary 待转换的字典数据
 *  @return JSON数据
 */
+ (nullable NSData *)getJSONDataWithDictionary:(NSDictionary *)dictionary;

/**
*  NSData数据转换成字典数据
*
*  @param data 待转换的NSData数据
*  @return 字典数据
*/
+(NSDictionary *)getDictionaryWithJSONData:(NSData*)data;

/**
*  JSON字符串转换成字典数据
*
*  @param jsonString 待转换的JSON字符串
*  @return 字典数据
*/
+ (nullable NSDictionary *)getDictionaryWithJsonString:(NSString *)jsonString;

#pragma mark - CRC相关

unsigned short crc16 (unsigned char *pD, int len);

+ (UInt32)getCRC32OfData:(NSData *)data;

#pragma mark - AES相关

/**
 * @brief   128 bit AES ECB encryption on speicified plaintext and keys
 * @param   inData    Pointer to speicified plaintext.
 * @param   in_len    The length of speicified plaintext.
 * @param   key    keys to encrypt the plaintext.
 * @param   outData    Pointer to binary encrypted data.
 * @return  Result of aes128_ecb_encrypt, kCCSuccess=0 means encrypt success, other means fail.
 */
int aes128_ecb_encrypt(const unsigned char *inData, int in_len, const unsigned char *key, unsigned char *outData);

/**
 * @brief   128 bit AES ECB decryption on speicified encrypted data and keys
 * @param   inData    Pointer to encrypted data.
 * @param   in_len    The length of encrypted data.
 * @param   key    keys to decrypt the encrypted data.
 * @param   outData    Pointer to plain data.
 * @return  Result of aes128_ecb_encrypt, kCCSuccess=0 means decrypt success, other means fail.
 */
int aes128_ecb_decrypt(const unsigned char *inData, int in_len, const unsigned char *key, unsigned char *outData);

#pragma mark - 正则表达式相关

+ (BOOL)validateUUID:(NSString *)uuidString;

+ (BOOL)validateHex:(NSString *)uuidString;

#pragma mark - UTF-8相关

+ (NSArray <NSNumber *>*)getNumberListFromUTF8EncodeData:(NSData *)UTF8EncodeData;

+ (NSData *)getUTF8EncodeDataFromNumberList:(NSArray <NSNumber *>*)numberList;

#pragma mark - 文件相关

+ (NSArray <NSString *>*)getAllFileNameWithFileType:(NSString *)fileType;
+ (NSData *)getDataWithFileName:(NSString *)fileName fileType:(NSString * _Nullable )fileType;

// 获取手机剩余的存储空间大小
+ (long)freeDiskSpaceInBytes;

#pragma mark - Telink定义的6字节MAC转16字节的UUID算法

/**
 * calculate UUID by mac address
 *
 * @param macAddress mac address, eg: A4C138E3EF05
 * @return device UUID, eg: D7009091D6B5D93590C8DE0DF7803463
 */
+ (NSData *)calcUuidByMac:(NSData *)macAddress;

@end

NS_ASSUME_NONNULL_END
