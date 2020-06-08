/********************************************************************************************************
* @file     TelinkHttpManager.h
*
* @brief    for TLSR chips
*
* @author     telink
* @date     Sep. 30, 2010
*
* @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
*           All rights reserved.
*
*             The information contained herein is confidential and proprietary property of Telink
*              Semiconductor (Shanghai) Co., Ltd. and is available under the terms
*             of Commercial License Agreement between Telink Semiconductor (Shanghai)
*             Co., Ltd. and the licensee in separate contract or the terms described here-in.
*           This heading MUST NOT be removed from this file.
*
*              Licensees are granted free, non-transferable use of the information in this
*             file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
*
*******************************************************************************************************/
//
//  TelinkHttpManager.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2020/4/29.
//  Copyright © 2020 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class TelinkHttpRequest;
typedef void (^MyBlock) (id _Nullable result, NSError * _Nullable err);

@interface TelinkHttpManager : NSObject

+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));


+ (TelinkHttpManager *)share;

/// 1.upload json dictionary
/// @param jsonDict json dictionary
/// @param timeout Upload data effective time
/// @param block result callback
- (void)uploadJsonDictionary:(NSDictionary *)jsonDict timeout:(NSInteger)timeout didLoadData:(MyBlock)block;

/// 2.download json dictionary
/// @param uuid identify of json dictionary
/// @param block result callback
- (void)downloadJsonDictionaryWithUUID:(NSString *)uuid didLoadData:(MyBlock)block;

@end

NS_ASSUME_NONNULL_END
