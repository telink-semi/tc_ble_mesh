/********************************************************************************************************
 * @file     NSString+extension.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/8/2
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

#import "NSString+extension.h"
#import <CommonCrypto/CommonCrypto.h>

@implementation NSString (extension)

/*
 *去掉首尾空格
 */
- (NSString *)removeHeadAndTailSpace {
    return [self stringByTrimmingCharactersInSet:NSCharacterSet.whitespaceCharacterSet];
}

/*
 *去掉首尾空格 包括后面的换行 \n
 */
- (NSString *)removeHeadAndTailSpacePro {
    return [self stringByTrimmingCharactersInSet:NSCharacterSet.whitespaceAndNewlineCharacterSet];
}

/*
 *去掉所有空格
 */
- (NSString *)removeAllSpace {
    return [self stringByReplacingOccurrencesOfString:@" " withString:@""];
}

/*
 *去掉所有空格和最后的回车
 */
- (NSString *)removeAllSpaceAndNewlines {
    NSString *tem = [self stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
    return [tem stringByReplacingOccurrencesOfString:@" " withString:@""];
}

/// 去掉所有空格和最后的回车，并在字符串前面补充“0”使其满足长度length
/// @param length 需要返回的字符串的长度，大于该长度直接返回，不在补“0”
- (NSString *)formatToLength:(UInt8)length {
    NSString *tem = [self stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
    tem = [tem stringByReplacingOccurrencesOfString:@" " withString:@""];
    while (tem.length < length) {
        tem = [@"0" stringByAppendingString:tem];
    }
    return tem;
}

/*
 *循环在间隔n个字符添加m个空格
 */
- (NSString *)insertSpaceNum:(int)spaceNum charNum:(int)charNum {
    if (charNum <= 0) {
        return @"";
    }
    NSString *returnString = @"";
    for (int i=0; i<self.length; i++) {
        returnString = [returnString stringByAppendingString:[self substringWithRange:NSMakeRange(i, 1)]];
        if ((i + 1) % charNum == 0) {
            for (int j=0; j<spaceNum; j++) {
                returnString = [returnString stringByAppendingString:@" "];
            }
        }
    }
    return returnString;
}

///JSON字符串转化为字典
+ (NSDictionary *)dictionaryWithJsonString:(NSString *)jsonString {
    if (jsonString == nil) {
        return nil;
    }
    NSData *jsonData = [jsonString dataUsingEncoding:NSUTF8StringEncoding];
    NSError *err;
    NSDictionary *dic = [NSJSONSerialization JSONObjectWithData:jsonData options:NSJSONReadingMutableContainers error:&err];
    if(err) {
        TelinkLogDebug(@"json解析失败：%@",err);
        return nil;
    }
    return dic;
}

#pragma mark - 正则表达式相关

+ (BOOL)validateEmail:(NSString *)emailString {
//    NSString *regex = @"[A-Z0-9a-z._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,4}";
    NSString *regex = @"^([a-z0-9A-Z]+[-|\\.]?)+[a-z0-9A-Z]@([a-z0-9A-Z]+(-[a-z0-9A-Z]+)?\\.)+[a-zA-Z]{2,}$";
    NSPredicate *pred = [NSPredicate predicateWithFormat:@"SELF MATCHES %@", regex];
    return [pred evaluateWithObject:emailString];
}

#pragma mark - 时间戳相关

+ (NSString *)getTimeStringWithTimeStamp:(NSInteger)timeStap {//1684229590952
    NSDate *confromTimesp = [NSDate dateWithTimeIntervalSince1970:timeStap/1000];
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
    [dateFormatter setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
    NSString *dateString = [dateFormatter stringFromDate:confromTimesp];
    return dateString;
}

#pragma mark - sha256相关

- (NSString *)sha256String {
    const char *cString = [self cStringUsingEncoding:NSUTF8StringEncoding];
    NSData *data = [NSData dataWithBytes:cString length:self.length];
    uint8_t digest[CC_SHA256_DIGEST_LENGTH];
    CC_SHA256(data.bytes,(CC_LONG)data.length, digest);
    NSMutableString* output = [NSMutableString stringWithCapacity:CC_SHA256_DIGEST_LENGTH * 2];
    for(int i = 0; i < CC_SHA256_DIGEST_LENGTH; i++){
        [output appendFormat:@"%02x", digest[i]];
    }
    return output;
}

@end
