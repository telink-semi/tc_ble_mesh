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
//  SigMeshOC
//
//  Created by Liangjiazhi on 2018/10/12.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "LibTools.h"

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
    TeLog(@"new secondsFrome2000=%ld",(long)section);
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

+ (NSData *)createNetworkKey{
    //原做法：生成的数据长度不足16，弃用
//    NSTimeInterval time = [[NSDate date] timeIntervalSince1970];
//    NSData *data = [NSData dataWithBytes: &time length: sizeof(time)];
//    TeLog(@"NetworkKey.length = %lu",(unsigned long)data.length);
//    if (data.length > 16) {
//        data = [data subdataWithRange:NSMakeRange(0, 16)];
//    }
//    TeLog(@"NetworkKey.length = %lu",(unsigned long)data.length);
//    return data;
    //新做法：生成的数据长度满足要求，与appkey生成规则一致。
    return [self createRandomDataWithLength:16];
}

+ (NSData *)initAppKey{
    return [self createRandomDataWithLength:16];
}

+ (NSData *)createRandomDataWithLength:(NSInteger)length{
    u8 key[length];
    for (int i=0; i<length; i++) {
        key[i] = arc4random() % 256;
    }
    NSData *data = [NSData dataWithBytes:key length:length];
    TeLog(@"createRandomData:%@",data);
    if (data.length == 0) {
        TeLog(@"ERROE : createRandomData fail");
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
    return [self uint8FromBytes:[self turnOverData:[self nsstringToHex:string]]];
}

///16进制NSString转Uint16
+ (UInt16)uint16From16String:(NSString *)string{
    return [self uint16FromBytes:[self turnOverData:[self nsstringToHex:string]]];
}

///16进制NSString转Uint32
+ (UInt32)uint32From16String:(NSString *)string{
    return [self uint32FromBytes:[self turnOverData:[self nsstringToHex:string]]];
}

///16进制NSString转Uint64
+ (UInt64)uint64From16String:(NSString *)string{
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

+ (NSString *)getUint32String:(UInt32)address {
    return [NSString stringWithFormat:@"%08X",(unsigned int)address];
}

+ (NSString *)getUint64String:(UInt64)address {
    return [NSString stringWithFormat:@"%016llX",address];
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
+(NSDictionary*)getDictionaryWithJSONData:(NSData*)data {
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

@end
