/********************************************************************************************************
 * @file     zipAndUnzip+zipString.m 
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
//  zipAndUnzip+zipString.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/12/03.
//  Copyright © 2018年 Telink. All rights reserved.
//

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
