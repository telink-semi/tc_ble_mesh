/********************************************************************************************************
 * @file     OTAFileSource.h 
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/7/31
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
