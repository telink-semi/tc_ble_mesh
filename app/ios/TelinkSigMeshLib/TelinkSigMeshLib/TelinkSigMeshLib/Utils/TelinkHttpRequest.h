//
//  TelinkHttpRequest.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2020/3/24.
//  Copyright © 2020 梁家誌. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

#define CustomErrorDomain @"cn.telink.httpRequest"

typedef void (^MyBlock) (id result, NSError *err);

@interface TelinkHttpRequest : NSObject

/// 1.upload json dictionary
/// @param jsonDict json dictionary
/// @param timeout Upload data effective time
/// @param block result callback
+ (void)uploadJsonDictionary:(NSDictionary *)jsonDict timeout:(NSInteger)timeout didLoadData:(MyBlock)block;


/// 2.download json dictionary
/// @param uuid identify of json dictionary
/// @param block result callback
+ (void)downloadJsonDictionaryWithUUID:(NSString *)uuid didLoadData:(MyBlock)block;

@end

NS_ASSUME_NONNULL_END
