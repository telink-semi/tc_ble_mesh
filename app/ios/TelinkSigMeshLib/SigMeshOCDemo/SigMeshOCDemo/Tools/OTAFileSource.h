/********************************************************************************************************
 * @file     OTAFileSource.h 
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
//  OTAFileSource.h
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface OTAFileSource : NSObject

+ (OTAFileSource *)share;

- (NSArray <NSString *>*)getAllBinFile;

- (NSArray <NSString *>*)getAllMeshJsonFile;

- (NSData *)getDataWithBinName:(NSString *)binName;

- (NSData *)getDataWithMeshJsonName:(NSString *)jsonName;


/// 获取Bin文件的PID的值。
/// @param data Bin文件的二进制数据。
- (UInt16)getPidWithOTAData:(NSData *)data;

/// 获取Bin文件的VID的值。
/// @param data Bin文件的二进制数据。
- (UInt16)getVidWithOTAData:(NSData *)data;

@end
