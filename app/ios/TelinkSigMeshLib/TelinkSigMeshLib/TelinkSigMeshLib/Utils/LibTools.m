/********************************************************************************************************
 * @file     LibTools.m 
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
//  LibTools.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2018/10/12.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "LibTools.h"
#import <CommonCrypto/CommonCryptor.h>

@implementation LibTools

+ (NSData *)nsstringToHex:(NSString *)string{
    const char *buf = [string UTF8String];
    NSMutableData *data = [NSMutableData data];
    if (buf)
    {
        unsigned long len = strlen(buf);
        char singleNumberString[3] = {'\0', '\0', '\0'};
        uint32_t singleNumber = 0;
        for(uint32_t i = 0 ; i < len; i+=2)
        {
            if ( ((i+1) < len) && isxdigit(buf[i])  )
            {
                singleNumberString[0] = buf[i];
                singleNumberString[1] = buf[i + 1];
                sscanf(singleNumberString, "%x", &singleNumber);
                uint8_t tmp = (uint8_t)(singleNumber & 0x000000FF);
                [data appendBytes:(void *)(&tmp)length:1];
            } else if (len == 1 && isxdigit(buf[i])) {
                singleNumberString[0] = buf[i];
                sscanf(singleNumberString, "%x", &singleNumber);
                uint8_t tmp = (uint8_t)(singleNumber & 0x000000FF);
                [data appendBytes:(void *)(&tmp)length:1];
            }
            else
            {
                break;
            }
        }
    }
    return data;
}

/**
 NSData 转  十六进制string(大写)
 
 @return NSString类型的十六进制string
 */
+ (NSString *)convertDataToHexStr:(NSData *)data{
    if (!data || [data length] == 0) {
        return @"";
    }
    NSMutableString *string = [[NSMutableString alloc] initWithCapacity:[data length]];
    
    [data enumerateByteRangesUsingBlock:^(const void *bytes, NSRange byteRange, BOOL *stop) {
        unsigned char *dataBytes = (unsigned char*)bytes;
        for (NSInteger i = 0; i < byteRange.length; i++) {
            NSString *hexStr = [NSString stringWithFormat:@"%X", (dataBytes[i]) & 0xff];
            if ([hexStr length] == 2) {
                [string appendString:hexStr];
            } else {
                [string appendFormat:@"0%@", hexStr];
            }
        }
    }];
    return string;
}

///NSData字节翻转
+ (NSData *)turnOverData:(NSData *)data{
    NSMutableData *backData = [NSMutableData data];
    for (int i=0; i<data.length; i++) {
        [backData appendData:[data subdataWithRange:NSMakeRange(data.length-1-i, 1)]];
    }
    return backData;
}

/**
 计算2000-01-01-00:00:00 到现在的秒数
 
 @return 返回2000-01-01-00:00:00 到现在的秒数
 */
+ (NSInteger )secondsFrome2000{
    NSInteger section = 0;
    //1970到现在的秒数-1970到2000的秒数
    section = [[NSDate date] timeIntervalSince1970] - 946684800;
//    TeLogInfo(@"new secondsFrome2000=%ld",(long)section);
    return section;
}

///返回手机当前时间的时区
+ (NSInteger)currentTimeZoon{
    [NSTimeZone resetSystemTimeZone]; // 重置手机系统的时区
    NSInteger offset = [NSTimeZone localTimeZone].secondsFromGMT;
    offset = offset/3600;
    return offset;
}

+ (NSString *)getNowTimeTimestamp{
    NSDate* dat = [NSDate dateWithTimeIntervalSinceNow:0];
    NSTimeInterval a=[dat timeIntervalSince1970];
    NSString*timeString = [NSString stringWithFormat:@"%0.f", a];//转为字符型
    return timeString;
}

+ (NSString *)getNowTimeTimestampFrome2000{
    return [NSString stringWithFormat:@"%020lX",(long)[LibTools secondsFrome2000]];
}

+ (NSString *)getNowTimeTimeString {
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    formatter.dateFormat = @"yyyy-MM-dd HH:mm:ss";
    NSString *time = [formatter stringFromDate:[NSDate date]];
    return time;
}

/// 返回当前时间字符串格式："YYYY-MM-ddThh:mm:ssXXX"，eg: @"2018-12-23T11:45:22-08:00"
/// 返回当前时间字符串格式："YYYY-MM-ddThh:mm:ssZ"，eg: @"2018-12-23T11:45:22-0800"
+ (NSString *)getNowTimeStringOfJson {
    NSDate *date = [NSDate date];
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    formatter.dateFormat = @"YYYY-MM-dd";
    NSString *time1 = [formatter stringFromDate:date];
//    formatter.dateFormat = @"hh:mm:ssZ";
    formatter.dateFormat = @"hh:mm:ssXXX";
    NSString *time2 = [formatter stringFromDate:date];
    return [NSString stringWithFormat:@"%@T%@",time1,time2];
}

/// 返回json中的SigStepResolution对应的毫秒数，只有四种值：100,1000,10000,600000.
+ (NSInteger)getSigStepResolutionInMillisecondsOfJson:(SigStepResolution)resolution {
    NSInteger tem = 100;
    switch (resolution) {
        case SigStepResolution_hundredsOfMilliseconds:
            tem = 100;
            break;
        case SigStepResolution_seconds:
            tem = 1000;
            break;
        case SigStepResolution_tensOfSeconds:
            tem = 10000;
            break;
        case SigStepResolution_tensOfMinutes:
            tem = 600000;
            break;
        default:
            break;
    }
    return tem;
}

+ (NSData *)createNetworkKey{
    //原做法：生成的数据长度不足16，弃用
//    NSTimeInterval time = [[NSDate date] timeIntervalSince1970];
//    NSData *data = [NSData dataWithBytes: &time length: sizeof(time)];
//    TeLogInfo(@"NetworkKey.length = %lu",(unsigned long)data.length);
//    if (data.length > 16) {
//        data = [data subdataWithRange:NSMakeRange(0, 16)];
//    }
//    TeLogInfo(@"NetworkKey.length = %lu",(unsigned long)data.length);
//    return data;
    //新做法：与appkey生成规则一致。
    return [self createRandomDataWithLength:16];
}

+ (NSData *)initAppKey{
    return [self createRandomDataWithLength:16];
}

+ (NSData *)createRandomDataWithLength:(NSInteger)length{
    UInt8 key[length];
    for (int i=0; i<length; i++) {
        key[i] = arc4random() % 256;
    }
    NSData *data = [NSData dataWithBytes:key length:length];
//    TeLogInfo(@"createRandomData:%@",data);
    if (data.length == 0) {
        TeLogInfo(@"ERROE : createRandomData fail");
    }
    return data;
}

+ (NSData *)initMeshUUID{
    return [self createRandomDataWithLength:16];
}

///NSData转long long
+ (long long)NSDataToUInt:(NSData *)data {
    long long datatemplength;
    [data getBytes:&datatemplength length:sizeof(datatemplength)];
    long long result = CFSwapInt64BigToHost(datatemplength);//大小端不一样，需要转化
    return result;
}

///返回带冒号的mac
+ (NSString *)getMacStringWithMac:(NSString *)mac{
    if (mac.length == 12) {
        NSString *tem = @"";
        for (int i=0; i<6; i++) {
            tem = [tem stringByAppendingString:[mac substringWithRange:NSMakeRange(i*2, 2)]];
            if (i<5) {
                tem = [tem stringByAppendingString:@":"];
            }
        }
        return tem;
    }else{
        return mac;
    }
}

///NSData转Uint8
+ (UInt8)uint8FromBytes:(NSData *)fData
{
    NSAssert(fData.length == 1, @"uint8FromBytes: (data length != 1)");
    NSData *data = fData;
    UInt8 val = 0;
    [data getBytes:&val length:1];
    return val;
}

///NSData转Uint16
+ (UInt16)uint16FromBytes:(NSData *)fData
{
    NSAssert(fData.length <= 2, @"uint16FromBytes: (data length > 2)");
    //    NSData *data = [self turnOverData:fData];
    NSData *data = fData;
    
    UInt16 val0 = 0;
    UInt16 val1 = 0;
    [data getBytes:&val0 range:NSMakeRange(0, 1)];
    if (data.length > 1) [data getBytes:&val1 range:NSMakeRange(1, 1)];
    
    UInt16 dstVal = (val0 & 0xff) + ((val1 << 8) & 0xff00);
    return dstVal;
}

///NSData转Uint32
+ (UInt32)uint32FromBytes:(NSData *)fData
{
    NSAssert(fData.length <= 4, @"uint32FromBytes: (data length > 4)");
    //    NSData *data = [self turnOverData:fData];
    NSData *data = fData;
    
    UInt32 val0 = 0;
    UInt32 val1 = 0;
    UInt32 val2 = 0;
    UInt32 val3 = 0;
    [data getBytes:&val0 range:NSMakeRange(0, 1)];
    if (data.length > 1) [data getBytes:&val1 range:NSMakeRange(1, 1)];
    if (data.length > 2) [data getBytes:&val2 range:NSMakeRange(2, 1)];
    if (data.length > 3) [data getBytes:&val3 range:NSMakeRange(3, 1)];
    
    UInt32 dstVal = (val0 & 0xff) + ((val1 << 8) & 0xff00) + ((val2 << 16) & 0xff0000) + ((val3 << 24) & 0xff000000);
    return dstVal;
}

///NSData转Uint64
+ (UInt64)uint64FromBytes:(NSData *)fData
{
    NSAssert(fData.length <= 8, @"uint64FromBytes: (data length > 8)");
    //    NSData *data = [self turnOverData:fData];
    NSData *data = fData;
    
    UInt64 val0 = 0;
    UInt64 val1 = 0;
    UInt64 val2 = 0;
    UInt64 val3 = 0;
    UInt64 val4 = 0;
    UInt64 val5 = 0;
    UInt64 val6 = 0;
    UInt64 val7 = 0;
    [data getBytes:&val0 range:NSMakeRange(0, 1)];
    if (data.length > 1) [data getBytes:&val1 range:NSMakeRange(1, 1)];
    if (data.length > 2) [data getBytes:&val2 range:NSMakeRange(2, 1)];
    if (data.length > 3) [data getBytes:&val3 range:NSMakeRange(3, 1)];
    if (data.length > 4) [data getBytes:&val4 range:NSMakeRange(4, 1)];
    if (data.length > 5) [data getBytes:&val5 range:NSMakeRange(5, 1)];
    if (data.length > 6) [data getBytes:&val6 range:NSMakeRange(6, 1)];
    if (data.length > 7) [data getBytes:&val7 range:NSMakeRange(7, 1)];

    UInt64 dstVal = (val0 & 0xff) + ((val1 << 8) & 0xff00) + ((val2 << 16) & 0xff0000) + ((val3 << 24) & 0xff000000) + ((val4 << 32) & 0xff00000000) + ((val5 << 40) & 0xff0000000000) + ((val6 << 48) & 0xff000000000000) + ((val7 << 56) & 0xff00000000000000);
    return dstVal;
}

///16进制NSString转Uint8
+ (UInt8)uint8From16String:(NSString *)string{
    if (string == nil || string.length == 0) {
        return 0;
    }
//    return [self uint8FromBytes:[self turnOverData:[self nsstringToHex:[self formatIntString:string]]]];
    return [self uint8FromBytes:[self turnOverData:[self nsstringToHex:string]]];
}

///16进制NSString转Uint16
+ (UInt16)uint16From16String:(NSString *)string{
    if (string == nil || string.length == 0) {
        return 0;
    }
//    return [self uint16FromBytes:[self turnOverData:[self nsstringToHex:[self formatIntString:string]]]];
    return [self uint16FromBytes:[self turnOverData:[self nsstringToHex:string]]];
}

///16进制NSString转Uint32
+ (UInt32)uint32From16String:(NSString *)string{
    if (string == nil || string.length == 0) {
        return 0;
    }
//    return [self uint32FromBytes:[self turnOverData:[self nsstringToHex:[self formatIntString:string]]]];
    return [self uint32FromBytes:[self turnOverData:[self nsstringToHex:string]]];
}

///16进制NSString转Uint64
+ (UInt64)uint64From16String:(NSString *)string{
    if (string == nil || string.length == 0) {
        return 0;
    }
//    return [self uint32FromBytes:[self turnOverData:[self nsstringToHex:[self formatIntString:string]]]];
    return [self uint64FromBytes:[self turnOverData:[self nsstringToHex:string]]];
}

///格式化整形字符串(去除前面的"0")
+ (NSString *)formatIntString:(NSString *)string{
    NSString *tem = string;
    while (tem.length > 1 && [[tem substringWithRange:NSMakeRange(0, 1)] isEqualToString:@"0"]) {
        tem = [tem substringWithRange:NSMakeRange(1, tem.length-1)];
    }
    return tem;
}

///D7C5BD18-4282-F31A-0CE0-0468BC0B8DE8 -> D7C5BD184282F31A0CE00468BC0B8DE8
+ (NSString *)meshUUIDToUUID:(NSString *)uuid{
    return [uuid stringByReplacingOccurrencesOfString:@"-" withString:@""];
}

///D7C5BD184282F31A0CE00468BC0B8DE8 -> D7C5BD18-4282-F31A-0CE0-0468BC0B8DE8
+ (NSString *)UUIDToMeshUUID:(NSString *)meshUUID{
    return [NSString stringWithFormat:@"%@-%@-%@-%@-%@",[meshUUID substringWithRange:NSMakeRange(0, 8)],[meshUUID substringWithRange:NSMakeRange(8, 4)],[meshUUID substringWithRange:NSMakeRange(12, 4)],[meshUUID substringWithRange:NSMakeRange(16, 4)],[meshUUID substringWithRange:NSMakeRange(20, 12)]];
}

+ (NSString *)getSDKVersion{
    return [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
}

+ (float)floatWithdecimalNumber:(double)num {
    return [[self decimalNumber:num] floatValue];
}

+ (double)doubleWithdecimalNumber:(double)num {
    return [[self decimalNumber:num] doubleValue];
}

+ (NSString *)stringWithDecimalNumber:(double)num {
    return [[self decimalNumber:num] stringValue];
}

+ (NSDecimalNumber *)decimalNumber:(double)num {
    NSString *numString = [NSString stringWithFormat:@"%lf", num];
    return [NSDecimalNumber decimalNumberWithString:numString];
}

+ (UInt8)lightnessToLum:(UInt16)lightness{
    return [LibTools levelToLum:[LibTools getLevelFromLightness:lightness]];
}

+ (UInt16)lumToLightness:(UInt8)lum{
    return [LibTools getLightnessFromLevel:[LibTools lumToLevel:lum]];
}

+ (UInt8)tempToTemp100:(UInt16)temp{
    return [LibTools tempToTemp100Hw:temp];// comfirm later, related with temp range
}

+ (UInt16)temp100ToTemp:(UInt8)temp100{
    if(temp100 > 100){
        temp100  = 100;
    }
    return (CTL_TEMP_MIN + ((CTL_TEMP_MAX - CTL_TEMP_MIN)*temp100)/100);
}

+ (UInt8)levelToLum:(SInt16)level{
    
    UInt16 lightness = level + 32768;
    UInt32 fix_1p2 = 0;
    if(lightness){    // fix decimals
#define LEVEL_UNIT_1P2    (65535/100/2)
        if(lightness < LEVEL_UNIT_1P2 + 2){     // +2 to fix accuracy missing
            lightness = LEVEL_UNIT_1P2 * 2;        // make sure lum is not zero when light on.
        }
        fix_1p2 = LEVEL_UNIT_1P2;
    }
    return (((lightness + fix_1p2)*100)/65535);
}

+ (SInt16)lumToLevel:(UInt8)lum{
    if(lum > 100){
        lum  = 100;
    }
    return (-32768 + [LibTools divisionRound:65535*lum dividend:100]);
}

+ (UInt32)divisionRound:(UInt32)val dividend:(UInt32)dividend{
    return (val + dividend/2)/dividend;
}

+ (UInt8)tempToTemp100Hw:(UInt16)temp{// use for driver pwm, 0--100 is absolute value, not related to temp range
    if(temp < CTL_TEMP_MIN){
        temp = CTL_TEMP_MIN;
    }
    
    if(temp > CTL_TEMP_MAX){
        temp = CTL_TEMP_MAX;
    }
    UInt32 fix_1p2 = (UInt32)(CTL_TEMP_MAX - CTL_TEMP_MIN)/100/2;    // fix decimals
    return (((temp - CTL_TEMP_MIN + fix_1p2)*100)/(CTL_TEMP_MAX - CTL_TEMP_MIN));   // temp100 can be zero.
}

+ (SInt16)getLevelFromLightness:(UInt16)lightness{
    return [LibTools uInt16ToSInt16:lightness];
}

+ (UInt16)getLightnessFromLevel:(SInt16)level{
    return [LibTools sint16ToUInt16:level];
}

+ (SInt16)uInt16ToSInt16:(UInt16)val{
    return (val - 32768);
}

+ (SInt16)sint16ToUInt16:(SInt16)val{
    return (val + 32768);
}

#pragma mark - JSON相关

/**
 *  字典数据转换成JSON字符串（没有可读性）
 *
 *  @param dictionary 待转换的字典数据
 *  @return JSON字符串
 */
+ (NSString *)getJSONStringWithDictionary:(NSDictionary *)dictionary {
    NSError *err;
    NSData *data = [NSJSONSerialization dataWithJSONObject:dictionary
                                                   options:NSJSONReadingMutableLeaves | NSJSONReadingAllowFragments
                                                     error:&err];
    if (data == nil) {
        NSLog(@"字典转换json失败：%@",err);
        return nil;
    }
    NSString *string = [[NSString alloc] initWithData:data
                                             encoding:NSUTF8StringEncoding];
    return string;
}
 
/**
 *  字典数据转换成JSON字符串（有可读性）
 *
 *  @param dictionary 待转换的字典数据
 *  @return JSON字符串
 */
+ (NSString *)getReadableJSONStringWithDictionary:(NSDictionary *)dictionary {
    NSError *err;
    NSData *data = [NSJSONSerialization dataWithJSONObject:dictionary
                                                   options:NSJSONWritingPrettyPrinted
                                                     error:&err];
    if (data == nil) {
        NSLog(@"字典转换json失败：%@",err);
        return nil;
    }
    NSString *string = [[NSString alloc] initWithData:data
                                             encoding:NSUTF8StringEncoding];
    return string;
}
 
/**
 *  字典数据转换成JSON数据
 *
 *  @param dictionary 待转换的字典数据
 *  @return JSON数据
 */
+ (NSData *)getJSONDataWithDictionary:(NSDictionary *)dictionary {
    NSError *err;
    NSData *data = [NSJSONSerialization dataWithJSONObject:dictionary
                                                   options:NSJSONWritingPrettyPrinted
                                                     error:&err];
    if (data == nil) {
        NSLog(@"字典转换json失败：%@",err);
    }
    return data;
}

/**
*  NSData数据转换成字典数据
*
*  @param data 待转换的NSData数据
*  @return 字典数据
*/
+(NSDictionary *)getDictionaryWithJSONData:(NSData*)data {
    NSString *receiveStr = [[NSString alloc]initWithData:data encoding:NSUTF8StringEncoding];
    NSData * datas = [receiveStr dataUsingEncoding:NSUTF8StringEncoding];
    NSDictionary *jsonDict = [NSJSONSerialization JSONObjectWithData:datas options:NSJSONReadingMutableLeaves error:nil];
    return jsonDict;
}

/**
*  JSON字符串转换成字典数据
*
*  @param jsonString 待转换的JSON字符串
*  @return 字典数据
*/
+ (NSDictionary *)getDictionaryWithJsonString:(NSString *)jsonString {
    if (jsonString == nil) {
        NSLog(@"json转换字典失败：json为空");
        return nil;
    }
    NSData *jsonData = [jsonString dataUsingEncoding:NSUTF8StringEncoding];
    NSError *err;
    NSDictionary *dic = [NSJSONSerialization JSONObjectWithData:jsonData
                                                        options:NSJSONReadingMutableContainers
                                                          error:&err];
    if(err)
    {
        NSLog(@"json转换字典失败：%@",err);
        return nil;
    }
    return dic;
}

#pragma mark - CRC相关

extern unsigned short crc16 (unsigned char *pD, int len) {
    static unsigned short poly[2]={0, 0xa001};
    unsigned short crc = 0xffff;
    int i,j;
    for(j=len; j>0; j--) {
        unsigned char ds = *pD++;
        for(i=0; i<8; i++) {
            crc = (crc >> 1) ^ poly[(crc ^ ds ) & 1];
            ds = ds >> 1;
        }
    }
    return crc;
}

+ (UInt32)getCRC32OfData:(NSData *)data {
    uint32_t *table = malloc(sizeof(uint32_t) * 256);
    uint32_t crc = 0xffffffff;
    uint8_t *bytes = (uint8_t *)[data bytes];
    
    for (uint32_t i=0; i<256; i++) {
        table[i] = i;
        for (int j=0; j<8; j++) {
            if (table[i] & 1) {
                table[i] = (table[i] >>= 1) ^ 0xedb88320;
            } else {
                table[i] >>= 1;
            }
        }
    }
    
    for (int i=0; i<data.length; i++) {
        crc = (crc >> 8) ^ table[(crc & 0xff) ^ bytes[i]];
    }
    crc ^= 0xffffffff;
    
    free(table);
    return crc;
}

#pragma mark - AES相关

//加密
int aes128_ecb_encrypt(const unsigned char *inData, int in_len, const unsigned char *key, unsigned char *outData) {
    size_t numBytesCrypted = 0;
    CCCryptorStatus cryptStatus = CCCrypt(kCCEncrypt, kCCAlgorithmAES128, kCCOptionECBMode, key, kCCKeySizeAES128, NULL, inData, in_len, outData, in_len, &numBytesCrypted);
    if (cryptStatus != kCCSuccess) {
        printf("aes128_ecb_encrypt fail!\n");
    }
    return (int)numBytesCrypted;
}

//解密
int aes128_ecb_decrypt(const unsigned char *inData, int in_len, const unsigned char *key, unsigned char *outData) {
    size_t numBytesCrypted = 0;
    CCCryptorStatus cryptStatus = CCCrypt(kCCDecrypt, kCCAlgorithmAES128, kCCOptionECBMode, key, kCCKeySizeAES128, NULL, inData, in_len, outData, in_len, &numBytesCrypted);
    if (cryptStatus != kCCSuccess) {
        printf("aes128_ecb_decrypt fail!\n");
    }
    return (int)numBytesCrypted;
}

#pragma mark - base64加解密相关

#define     LocalStr_None           @""
static const char encodingTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

+ (NSString *)base64StringFromText:(NSString *)text
{
    if (text && ![text isEqualToString:LocalStr_None]) {
        //取项目的bundleIdentifier作为KEY  改动了此处
        NSString *key = [[NSBundle mainBundle] bundleIdentifier];
//        NSString *key = @"com.Ledvance.smartapp";
        NSData *data = [text dataUsingEncoding:NSUTF8StringEncoding];
        //IOS 自带DES加密 Begin  改动了此处
        data = [self DESEncrypt:data WithKey:key];
        //IOS 自带DES加密 End
        return [self base64EncodedStringFrom:data];
    }
    else {
        return LocalStr_None;
    }
}

+ (NSString *)textFromBase64String:(NSString *)base64
{
    if (base64 && ![base64 isEqualToString:LocalStr_None]) {
        //取项目的bundleIdentifier作为KEY   改动了此处
        NSString *key = [[NSBundle mainBundle] bundleIdentifier];
//        NSString *key = @"com.Ledvance.smartapp";
        NSData *data = [self dataWithBase64EncodedString:base64];
        //IOS 自带DES解密 Begin    改动了此处
        data = [self DESDecrypt:data WithKey:key];
        //IOS 自带DES加密 End
        return [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    }
    else {
        return LocalStr_None;
    }
}

+ (NSString *)textFromBase64String:(NSString *)base64 password:(NSString *)password
{
    if (base64 && ![base64 isEqualToString:LocalStr_None]) {
        //取项目的bundleIdentifier作为KEY   改动了此处
//        NSString *key = [[NSBundle mainBundle] bundleIdentifier];
//        NSString *key = @"com.Ledvance.smartapp";
        NSData *data = [self dataWithBase64EncodedString:base64];
        //IOS 自带DES解密 Begin    改动了此处
        data = [self DESDecrypt:data WithKey:password];
        //IOS 自带DES加密 End
        return [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    }
    else {
        return LocalStr_None;
    }
}

/******************************************************************************
 函数名称 : + (NSData *)DESEncrypt:(NSData *)data WithKey:(NSString *)key
 函数描述 : 文本数据进行DES加密
 输入参数 : (NSData *)data
 (NSString *)key
 输出参数 : N/A
 返回参数 : (NSData *)
 备注信息 : 此函数不可用于过长文本
 ******************************************************************************/
+ (NSData *)DESEncrypt:(NSData *)data WithKey:(NSString *)key
{
    char keyPtr[kCCKeySizeAES256+1];
    bzero(keyPtr, sizeof(keyPtr));
    
    [key getCString:keyPtr maxLength:sizeof(keyPtr) encoding:NSUTF8StringEncoding];
    
    NSUInteger dataLength = [data length];
    
    size_t bufferSize = dataLength + kCCBlockSizeAES128;
    void *buffer = malloc(bufferSize);
    
    size_t numBytesEncrypted = 0;
    CCCryptorStatus cryptStatus = CCCrypt(kCCEncrypt, kCCAlgorithmDES,
                                          kCCOptionPKCS7Padding | kCCOptionECBMode,
                                          keyPtr, kCCBlockSizeDES,
                                          NULL,
                                          [data bytes], dataLength,
                                          buffer, bufferSize,
                                          &numBytesEncrypted);
    if (cryptStatus == kCCSuccess) {
        return [NSData dataWithBytesNoCopy:buffer length:numBytesEncrypted];
    }
    
    free(buffer);
    return nil;
}

/******************************************************************************
 函数名称 : + (NSData *)DESEncrypt:(NSData *)data WithKey:(NSString *)key
 函数描述 : 文本数据进行DES解密
 输入参数 : (NSData *)data
 (NSString *)key
 输出参数 : N/A
 返回参数 : (NSData *)
 备注信息 : 此函数不可用于过长文本
 ******************************************************************************/
+ (NSData *)DESDecrypt:(NSData *)data WithKey:(NSString *)key
{
    char keyPtr[kCCKeySizeAES256+1];
    bzero(keyPtr, sizeof(keyPtr));
    
    [key getCString:keyPtr maxLength:sizeof(keyPtr) encoding:NSUTF8StringEncoding];
    
    NSUInteger dataLength = [data length];
    
    size_t bufferSize = dataLength + kCCBlockSizeAES128;
    void *buffer = malloc(bufferSize);
    
    size_t numBytesDecrypted = 0;
    CCCryptorStatus cryptStatus = CCCrypt(kCCDecrypt, kCCAlgorithmDES,
                                          kCCOptionPKCS7Padding | kCCOptionECBMode,
                                          keyPtr, kCCBlockSizeDES,
                                          NULL,
                                          [data bytes], dataLength,
                                          buffer, bufferSize,
                                          &numBytesDecrypted);
    
    if (cryptStatus == kCCSuccess) {
        return [NSData dataWithBytesNoCopy:buffer length:numBytesDecrypted];
    }
    
    free(buffer);
    return nil;
}

/******************************************************************************
 函数名称 : + (NSData *)dataWithBase64EncodedString:(NSString *)string
 函数描述 : base64格式字符串转换为文本数据
 输入参数 : (NSString *)string
 输出参数 : N/A
 返回参数 : (NSData *)
 备注信息 :
 ******************************************************************************/
+ (NSData *)dataWithBase64EncodedString:(NSString *)string
{
    if (string == nil)
        [NSException raise:NSInvalidArgumentException format:@""];
    if ([string length] == 0)
        return [NSData data];
    
    static char *decodingTable = NULL;
    if (decodingTable == NULL)
    {
        decodingTable = malloc(256);
        if (decodingTable == NULL)
            return nil;
        memset(decodingTable, CHAR_MAX, 256);
        NSUInteger i;
        for (i = 0; i < 64; i++)
            decodingTable[(short)encodingTable[i]] = i;
    }
    
    const char *characters = [string cStringUsingEncoding:NSASCIIStringEncoding];
    if (characters == NULL)     //  Not an ASCII string!
        return nil;
    char *bytes = malloc((([string length] + 3) / 4) * 3);
    if (bytes == NULL)
        return nil;
    NSUInteger length = 0;
    
    NSUInteger i = 0;
    while (YES)
    {
        char buffer[4];
        short bufferLength;
        for (bufferLength = 0; bufferLength < 4; i++)
        {
            if (characters[i] == '\0')
                break;
            if (isspace(characters[i]) || characters[i] == '=')
                continue;
            buffer[bufferLength] = decodingTable[(short)characters[i]];
            if (buffer[bufferLength++] == CHAR_MAX)      //  Illegal character!
            {
                free(bytes);
                return nil;
            }
        }
        
        if (bufferLength == 0)
            break;
        if (bufferLength == 1)      //  At least two characters are needed to produce one byte!
        {
            free(bytes);
            return nil;
        }
        
        //  Decode the characters in the buffer to bytes.
        bytes[length++] = (buffer[0] << 2) | (buffer[1] >> 4);
        if (bufferLength > 2)
            bytes[length++] = (buffer[1] << 4) | (buffer[2] >> 2);
        if (bufferLength > 3)
            bytes[length++] = (buffer[2] << 6) | buffer[3];
    }
    
    bytes = realloc(bytes, length);
    return [NSData dataWithBytesNoCopy:bytes length:length];
}

/******************************************************************************
 函数名称 : + (NSString *)base64EncodedStringFrom:(NSData *)data
 函数描述 : 文本数据转换为base64格式字符串
 输入参数 : (NSData *)data
 输出参数 : N/A
 返回参数 : (NSString *)
 备注信息 :
 ******************************************************************************/
+ (NSString *)base64EncodedStringFrom:(NSData *)data
{
    if ([data length] == 0)
        return @"";
    
    char *characters = malloc((([data length] + 2) / 3) * 4);
    if (characters == NULL)
        return nil;
    NSUInteger length = 0;
    
    NSUInteger i = 0;
    while (i < [data length])
    {
        char buffer[3] = {0,0,0};
        short bufferLength = 0;
        while (bufferLength < 3 && i < [data length])
            buffer[bufferLength++] = ((char *)[data bytes])[i++];
        
        //  Encode the bytes in the buffer to four characters, including padding "=" characters if necessary.
        characters[length++] = encodingTable[(buffer[0] & 0xFC) >> 2];
        characters[length++] = encodingTable[((buffer[0] & 0x03) << 4) | ((buffer[1] & 0xF0) >> 4)];
        if (bufferLength > 1)
            characters[length++] = encodingTable[((buffer[1] & 0x0F) << 2) | ((buffer[2] & 0xC0) >> 6)];
        else characters[length++] = '=';
        if (bufferLength > 2)
            characters[length++] = encodingTable[buffer[2] & 0x3F];
        else characters[length++] = '=';
    }
    
    return [[NSString alloc] initWithBytesNoCopy:characters length:length encoding:NSASCIIStringEncoding freeWhenDone:YES];
}

#pragma mark - 正则表达式相关

+ (BOOL)validateUUID:(NSString *)uuidString {
    NSString *regex = @"\\w{8}(-\\w{4}){3}-\\w{12}";
    NSPredicate *pred = [NSPredicate predicateWithFormat:@"SELF MATCHES %@", regex];
    return [pred evaluateWithObject:uuidString];
}

+ (BOOL)validateHex:(NSString *)uuidString {
    NSString *regex = @"^[0-9a-fA-F]{0,}$";
    NSPredicate *pred = [NSPredicate predicateWithFormat:@"SELF MATCHES %@", regex];
    return [pred evaluateWithObject:uuidString];
}

@end
