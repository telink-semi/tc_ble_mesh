/********************************************************************************************************
 * @file     NSString+extension.h 
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
//  NSString+extension.h
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/8/2.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSString (extension)

/*
 *去掉首尾空格
 */
- (NSString *)removeHeadAndTailSpace;

/*
 *去掉首尾空格 包括后面的换行 \n
 */
- (NSString *)removeHeadAndTailSpacePro;

/*
 *去掉所有空格
 */
- (NSString *)removeAllSapce;

/*
 *去掉所有空格和最后的回车
 */
- (NSString *)removeAllSapceAndNewlines;

/// 去掉所有空格和最后的回车，并在字符串前面补充“0”使其满足长度length
/// @param length 需要返回的字符串的长度，大于该长度直接返回，不在补“0”
- (NSString *)formatToLength:(UInt8)length;

/*
 *循环在间隔n个字符添加m个空格
 */
- (NSString *)insertSpaceNum:(int)spaceNum charNum:(int)charNum;

///JSON字符串转化为字典
+ (NSDictionary *)dictionaryWithJsonString:(NSString *)jsonString;

@end
