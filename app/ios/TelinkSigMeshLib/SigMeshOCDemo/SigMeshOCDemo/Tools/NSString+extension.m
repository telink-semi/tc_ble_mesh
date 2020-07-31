/********************************************************************************************************
 * @file     NSString+extension.m 
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
//  NSString+extension.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/8/2.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "NSString+extension.h"

@implementation NSString (extension)

/*
 *去掉首尾空格
 */
- (NSString *)removeHeadAndTailSpace{
    return [self stringByTrimmingCharactersInSet:NSCharacterSet.whitespaceCharacterSet];
}

/*
 *去掉首尾空格 包括后面的换行 \n
 */
- (NSString *)removeHeadAndTailSpacePro{
    return [self stringByTrimmingCharactersInSet:NSCharacterSet.whitespaceAndNewlineCharacterSet];
}

/*
 *去掉所有空格
 */
- (NSString *)removeAllSapce{
    return [self stringByReplacingOccurrencesOfString:@" " withString:@""];
}

/*
 *去掉所有空格和最后的回车
 */
- (NSString *)removeAllSapceAndNewlines{
    NSString *tem = [self stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
    return [tem stringByReplacingOccurrencesOfString:@" " withString:@""];
}

/*
 *循环在间隔n个字符添加m个空格
 */
- (NSString *)insertSpaceNum:(int)spaceNum charNum:(int)charNum{
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
+ (NSDictionary *)dictionaryWithJsonString:(NSString *)jsonString
{
    if (jsonString == nil) {
        return nil;
    }
    
    NSData *jsonData = [jsonString dataUsingEncoding:NSUTF8StringEncoding];
    NSError *err;
    NSDictionary *dic = [NSJSONSerialization JSONObjectWithData:jsonData
                                                        options:NSJSONReadingMutableContainers
                                                          error:&err];
    if(err)
    {
        TeLogDebug(@"json解析失败：%@",err);
        return nil;
    }
    return dic;
}

@end
