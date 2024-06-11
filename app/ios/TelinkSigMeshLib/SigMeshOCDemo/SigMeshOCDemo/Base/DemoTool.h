/********************************************************************************************************
 * @file     DemoTool.h
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/11/22
 *
 * @par     Copyright (c) 2023, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

@interface DemoTool : NSObject

/// Get Node State Image
/// - Parameter unicastAddress: the unicast address of node
+ (UIImage *)getNodeStateImageWithUnicastAddress:(UInt16)unicastAddress;

/// Get Node State string
/// - Parameter unicastAddress: the unicast address of node
+ (NSString *)getNodeStateStringWithUnicastAddress:(UInt16)unicastAddress;

#ifdef kIsTelinkCloudSigMeshLib
+ (NSString *)getStateStringWithState:(ResponseShareState)state;
+ (UIImage *)getStateImageWithState:(ResponseShareState)state;
+ (UIColor *)getStateColorWithState:(ResponseShareState)state;
#endif
@end

NS_ASSUME_NONNULL_END
