//
//  TelinkHttpRequest.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2020/3/24.
//  Copyright © 2020 梁家誌. All rights reserved.
//

#import "TelinkHttpRequest.h"
#import <CommonCrypto/CommonDigest.h>

#define RequestTypeGet      @"GET"
#define RequestTypePUT      @"PUT"
#define RequestTypePOST     @"POST"
#define RequestTypeDelete   @"DELETE"

//#define BaseUrl @"http://192.168.18.59:8080/"
#define BaseUrl @"http://47.115.40.63:8080/"


@interface TelinkHttpRequest ()<NSURLConnectionDataDelegate>
@property (copy, nonatomic) MyBlock myBlock;
@end


@implementation TelinkHttpRequest{
    NSMutableData *_httpReceiveData;
}

-(id)init{
    if (self = [super init]) {
        _httpReceiveData = [[NSMutableData alloc] init];
    }
    return self;
}

-(void)requestWithRequestType:(NSString *)requestType withUrl:(NSString *)urlStr withHeader:(NSDictionary *)header withContent:(id)content{
    
    NSURL *url = [NSURL URLWithString:urlStr];
    
    NSMutableURLRequest *request = [[NSMutableURLRequest alloc] initWithURL:url cachePolicy:NSURLRequestUseProtocolCachePolicy timeoutInterval:10];
    
    [request setHTTPMethod:requestType];
    
    for (NSString *key in header.allKeys) {
        [request addValue:[header objectForKey:key] forHTTPHeaderField:key];
    }

    NSLog(@"%@", request.URL);
    NSLog(@"%@", request.HTTPMethod);
    NSLog(@"%@", header);

//    if (content) {
//        NSData *contentData = [NSJSONSerialization dataWithJSONObject:content options:0 error:nil];
//        [request setHTTPBody:contentData];
//    }
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
    
    NSURLSessionTask *task = [[NSURLSession sharedSession] dataTaskWithRequest:request completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
        
        if (error) {
            if (_myBlock) _myBlock(nil, error);
        }else{
            NSHTTPURLResponse *r = (NSHTTPURLResponse*)response;
            NSLog(@"%ld %@", (long)[r statusCode], [NSHTTPURLResponse localizedStringForStatusCode:[r statusCode]]);
            NSInteger statusCode = [r statusCode];
            if (statusCode != 200) {
                NSDictionary *result = [NSJSONSerialization JSONObjectWithData:data options:0 error:nil];
                
                if (result.count>0) {
//                    NSError *err = [NSError errorWithDomain:BaseUrl code:[[[result objectForKey:@"error"] objectForKey:@"code"] integerValue] userInfo:@{NSLocalizedDescriptionKey : [[result objectForKey:@"error"] objectForKey:@"msg"]}];
                    NSError *err = [NSError errorWithDomain:BaseUrl code:[[result objectForKey:@"status"] integerValue] userInfo:@{NSLocalizedDescriptionKey : [result objectForKey:@"message"]}];
                    _myBlock(nil, err);
                }else{
                    NSError *err = [NSError errorWithDomain:BaseUrl code:99999 userInfo:@{NSLocalizedDescriptionKey : @"服务器异常，请稍后···"}];
                    _myBlock(nil, err);
                }
              
            }else{
                NSError *err;
                NSDictionary *result = nil;
                if (data.length) {
                    result = [NSJSONSerialization JSONObjectWithData:data options:0 error:&err];
                }
                if (err) {
                    if (_myBlock) _myBlock(nil, err);
                }else{
                    if (_myBlock) _myBlock(result, nil);
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
+ (void)uploadJsonDictionary:(NSDictionary *)jsonDict timeout:(NSInteger)timeout didLoadData:(MyBlock)block {
    do {
        TelinkHttpRequest *req = [[TelinkHttpRequest alloc] init];
        req.myBlock = block;
//        NSDictionary *header = @{@"Content-Type" : @"application/json"};
        NSDictionary *header = @{@"Content-Type" : @"application/x-www-form-urlencoded"};

        //GET
//        [req requestWithRequestType:RequestTypeGet withUrl:[NSString stringWithFormat:@"%@upload?data=%@&timeout=%ld",BaseUrl,@"123456",(long)timeout] withHeader:header withContent:nil];

        //POST
NSDictionary *content = @{@"data" : [LibTools getJSONStringWithDictionary:jsonDict],@"timeout":@(timeout)};
//NSDictionary *content = @{@"data" : @"123",@"timeout":@(timeout)};
        [req requestWithRequestType:RequestTypePOST withUrl:[NSString stringWithFormat:@"%@upload",BaseUrl] withHeader:header withContent:content];
    } while (0);
}


/// 2.download json dictionary
/// @param uuid identify of json dictionary
/// @param block result callback
+ (void)downloadJsonDictionaryWithUUID:(NSString *)uuid didLoadData:(MyBlock)block {
    do {
        TelinkHttpRequest *req = [[TelinkHttpRequest alloc] init];
        req.myBlock = block;
//        NSDictionary *header = @{@"Content-Type" : @"application/json"};
        NSDictionary *header = @{@"Content-Type" : @"application/x-www-form-urlencoded"};

        //GET
//        [req requestWithRequestType:RequestTypeGET withUrl:[NSString stringWithFormat:@"%@download?uuid=%@",BaseUrl,uuid] withHeader:header withContent:nil];
        
        //POST
        NSDictionary *content = @{@"uuid" : uuid};
        [req requestWithRequestType:RequestTypePOST withUrl:[NSString stringWithFormat:@"%@download",BaseUrl] withHeader:header withContent:content];
        
    } while (0);
}

@end
