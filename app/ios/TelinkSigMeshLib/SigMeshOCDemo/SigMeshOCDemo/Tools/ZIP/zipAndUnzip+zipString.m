/********************************************************************************************************
 * @file     zipAndUnzip+zipString.m 
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/12/03
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

#import "zipAndUnzip+zipString.h"

@implementation zipAndUnzip (zipString)
+ (NSData *)zipString:(NSString *)string {
    NSStringEncoding enc = CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingISOLatin1);
    NSData *data  = [string dataUsingEncoding:enc];
    NSData *zipData = [self gzipDeflate:data];
    return zipData;
}
+ (NSString *)changeData:(NSData *)data {
    NSStringEncoding enc = CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingISOLatin1);
    Byte *byte = (Byte *)[data bytes];
    NSData *odata = [NSData dataWithBytes:byte length:data.length];
    NSString *str = [[NSString alloc] initWithData:odata encoding:enc];
    return str;
}
@end
