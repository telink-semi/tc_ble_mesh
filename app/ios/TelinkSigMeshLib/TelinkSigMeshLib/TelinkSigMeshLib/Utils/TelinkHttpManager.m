/********************************************************************************************************
* @file     TelinkHttpManager.m
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
//  TelinkHttpManager.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2020/4/29.
//  Copyright © 2020 Telink. All rights reserved.
//

#import "TelinkHttpManager.h"
#import <CommonCrypto/CommonDigest.h>

#define RequestTypeGet      @"GET"
#define RequestTypePUT      @"PUT"
#define RequestTypePOST     @"POST"
#define RequestTypeDelete   @"DELETE"

//#define BaseUrl @"http://192.168.18.59:8080/"
#define BaseUrl @"http://47.115.40.63:8080/"

#define CustomErrorDomain @"cn.telink.httpRequest"

typedef void (^TelinkHttpBlock) (TelinkHttpRequest * _Nonnull request,id _Nullable result, NSError * _Nullable err);

@interface TelinkHttpRequest : NSObject<NSURLConnectionDataDelegate>
@property (copy, nonatomic) TelinkHttpBlock httpBlock;
@end

@implementation TelinkHttpRequest{
    NSMutableData *_httpReceiveData;
}

- (instancetype)init {
    if (self = [super init]) {
        _httpReceiveData = [[NSMutableData alloc] init];
    }
    return self;
}

- (void)requestWithRequestType:(NSString *)requestType withUrl:(NSString *)urlStr withHeader:(NSDictionary *)header withContent:(id)content {
    NSURL *url = [NSURL URLWithString:urlStr];
    NSMutableURLRequest *request = [[NSMutableURLRequest alloc] initWithURL:url cachePolicy:NSURLRequestUseProtocolCachePolicy timeoutInterval:10];
    [request setHTTPMethod:requestType];
    for (NSString *key in header.allKeys) {
        [request addValue:[header objectForKey:key] forHTTPHeaderField:key];
    }
    NSLog(@"%@", request.URL);
    NSLog(@"%@", request.HTTPMethod);
    NSLog(@"%@", header);
    if (content) {
        NSString *bodyStr = @"";
        for (NSString *key in ((NSDictionary *)content).allKeys) {
            if (bodyStr.length > 0) {
                bodyStr = [NSString stringWithFormat:@"%@&%@=%@",bodyStr,key,[(NSDictionary *)content objectForKey:key]];
            }else{
                bodyStr = [NSString stringWithFormat:@"%@=%@",key,[(NSDictionary *)content objectForKey:key]];
            }
        }
        [request setHTTPBody:[bodyStr dataUsingEncoding:NSUTF8StringEncoding]];
    }
    __weak typeof(self) weakSelf = self;
    NSURLSessionTask *task = [[NSURLSession sharedSession] dataTaskWithRequest:request completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
        if (error) {
            if (weakSelf.httpBlock) weakSelf.httpBlock(weakSelf, nil, error);
        }else{
            NSHTTPURLResponse *r = (NSHTTPURLResponse*)response;
            NSLog(@"%ld %@", (long)[r statusCode], [NSHTTPURLResponse localizedStringForStatusCode:[r statusCode]]);
            NSInteger statusCode = [r statusCode];
            if (statusCode != 200) {
                NSDictionary *result = [NSJSONSerialization JSONObjectWithData:data options:0 error:nil];
                if (result.count>0) {
                    NSError *err = [NSError errorWithDomain:BaseUrl code:[[result objectForKey:@"status"] integerValue] userInfo:@{NSLocalizedDescriptionKey : [result objectForKey:@"message"]}];
                    if (weakSelf.httpBlock) {
                        weakSelf.httpBlock(weakSelf, nil, err);
                    }
                }else{
                    NSError *err = [NSError errorWithDomain:BaseUrl code:99999 userInfo:@{NSLocalizedDescriptionKey : @"服务器异常，请稍后···"}];
                    if (weakSelf.httpBlock) {
                        weakSelf.httpBlock(weakSelf, nil, err);
                    }
                }
            }else{
                NSError *err;
                NSDictionary *result = nil;
                if (data.length) {
                    result = [NSJSONSerialization JSONObjectWithData:data options:0 error:&err];
                }
                if (err) {
                    if (weakSelf.httpBlock) {
                        weakSelf.httpBlock(weakSelf, nil, err);
                    }
                }else{
                    if (weakSelf.httpBlock) {
                        weakSelf.httpBlock(weakSelf, result, nil);
                    }
                }
            }
        }
    }];
    [task resume];
}

/// 1.upload json dictionary
/// @param jsonDict json dictionary
/// @param timeout Upload data effective time
/// @param block result callback
+ (TelinkHttpRequest *)uploadJsonDictionary:(NSDictionary *)jsonDict timeout:(NSInteger)timeout didLoadData:(TelinkHttpBlock)block {
    TelinkHttpRequest *req = [[TelinkHttpRequest alloc] init];
    req.httpBlock = block;
    NSDictionary *header = @{@"Content-Type" : @"application/x-www-form-urlencoded"};
    NSDictionary *content = @{@"data" : [LibTools getJSONStringWithDictionary:jsonDict],@"timeout":@(timeout)};
    [req requestWithRequestType:RequestTypePOST withUrl:[NSString stringWithFormat:@"%@upload",BaseUrl] withHeader:header withContent:content];
    return req;
}

/// 2.download json dictionary
/// @param uuid identify of json dictionary
/// @param block result callback
+ (TelinkHttpRequest *)downloadJsonDictionaryWithUUID:(NSString *)uuid didLoadData:(TelinkHttpBlock)block {
    TelinkHttpRequest *req = [[TelinkHttpRequest alloc] init];
    req.httpBlock = block;
    NSDictionary *header = @{@"Content-Type" : @"application/x-www-form-urlencoded"};
    NSDictionary *content = @{@"uuid" : uuid};
    [req requestWithRequestType:RequestTypePOST withUrl:[NSString stringWithFormat:@"%@download",BaseUrl] withHeader:header withContent:content];
    return req;
}

@end

@interface TelinkHttpManager ()
@property (nonatomic, strong) NSMutableArray <TelinkHttpRequest *>*telinkHttpRequests;
@end

@implementation TelinkHttpManager

+ (TelinkHttpManager *)share{
    static TelinkHttpManager *shareManager = nil;
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        shareManager = [[TelinkHttpManager alloc] init];
        shareManager.telinkHttpRequests = [NSMutableArray array];
    });
    return shareManager;
}

/// 1.upload json dictionary
/// @param jsonDict json dictionary
/// @param timeout Upload data effective time
/// @param block result callback
- (void)uploadJsonDictionary:(NSDictionary *)jsonDict timeout:(NSInteger)timeout didLoadData:(MyBlock)block {
    __weak typeof(self) weakSelf = self;
    TelinkHttpRequest *request = [TelinkHttpRequest uploadJsonDictionary:jsonDict timeout:timeout didLoadData:^(TelinkHttpRequest * _Nonnull request, id  _Nullable result, NSError * _Nullable err) {
        [weakSelf.telinkHttpRequests removeObject:request];
        if (block) {
            block(result,err);
        }
    }];
    [_telinkHttpRequests addObject:request];
}

/// 2.download json dictionary
/// @param uuid identify of json dictionary
/// @param block result callback
- (void)downloadJsonDictionaryWithUUID:(NSString *)uuid didLoadData:(MyBlock)block {
    __weak typeof(self) weakSelf = self;
    TelinkHttpRequest *request = [TelinkHttpRequest downloadJsonDictionaryWithUUID:uuid didLoadData:^(TelinkHttpRequest * _Nonnull request, id  _Nullable result, NSError * _Nullable err) {
        [weakSelf.telinkHttpRequests removeObject:request];
        if (block) {
            block(result,err);
        }
    }];
    [_telinkHttpRequests addObject:request];
}

@end
