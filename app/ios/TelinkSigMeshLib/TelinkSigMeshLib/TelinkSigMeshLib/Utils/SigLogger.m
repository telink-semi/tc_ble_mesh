/********************************************************************************************************
 * @file     SigLogger.m
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
//  SigLogger.m
//  TelinkSigMeshLib
//
//  Created by Liangjiazhi on 2019/8/16.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import "SigLogger.h"
#import <sys/time.h>

#define kTelinkSDKDebugLogData @"TelinkSDKDebugLogData"
#define kTelinkSDKMeshJsonData @"TelinkSDKMeshJsonData"
#define kTelinkSDKDebugLogDataSize ((double)1024*1024*20) //默认日志最大存储大小为20M。

@implementation SigLogger

+ (SigLogger *)share{
    static SigLogger *shareLogger = nil;
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        shareLogger = [[SigLogger alloc] init];
        [shareLogger initLogFile];
    });
    return shareLogger;
}

- (void)initLogFile{
    NSFileManager *manager = [[NSFileManager alloc] init];
    if (![manager fileExistsAtPath:self.logFilePath]) {
        BOOL ret = [manager createFileAtPath:self.logFilePath contents:nil attributes:nil];
        if (ret) {
            NSLog(@"%@",@"creat success");
        } else {
            NSLog(@"%@",@"creat failure");
        }
    }
    if (![manager fileExistsAtPath:self.meshJsonFilePath]) {
        BOOL ret = [manager createFileAtPath:self.meshJsonFilePath contents:nil attributes:nil];
        if (ret) {
            NSLog(@"%@",@"creat TelinkSDKMeshJsonData success");
        } else {
            NSLog(@"%@",@"creat TelinkSDKMeshJsonData failure");
        }
    }
}

- (NSString *)logFilePath {
    return [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).lastObject stringByAppendingPathComponent:kTelinkSDKDebugLogData];
}

- (NSString *)meshJsonFilePath {
    return [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).lastObject stringByAppendingPathComponent:kTelinkSDKMeshJsonData];
}

- (void)setSDKLogLevel:(SigLogLevel)logLevel{
    _logLevel = logLevel;
    [self checkSDKLogFileSize];
    [self enableLogger];
}

- (void)enableLogger{
    TelinkLogWithFile([NSString stringWithFormat:@"\n\n\n\n\t打开APP，初始化TelinkSigMesh %@日志模块\n\n\n",kTelinkSigMeshLibVersion]);
}

/// 监测缓存本地的日志文件大小，大于阈值则砍掉多余部分，只保留阈值的80%。
- (void)checkSDKLogFileSize {
    NSFileHandle *handle = [NSFileHandle fileHandleForReadingAtPath:SigLogger.share.logFilePath];
    NSData *data = [handle readDataToEndOfFile];
    [handle closeFile];
    NSInteger length = data.length;
    if (length > kTelinkSDKDebugLogDataSize) {
        NSInteger saveLength = ceil(kTelinkSDKDebugLogDataSize * 0.8);
        NSData *saveData = [data subdataWithRange:NSMakeRange(length - saveLength, saveLength)];
        handle = [NSFileHandle fileHandleForWritingAtPath:SigLogger.share.logFilePath];
        [handle truncateFileAtOffset:0];
        [handle writeData:saveData];
        [handle closeFile];
    }
}


void saveLogData(id data){
    if (SigLogger.share.logLevel > 0) {
        @synchronized (SigLogger.share) {
            [SigLogger.share saveLogOnThread:data];
        }
    }
}

- (void)saveLogOnThread:(id)data {
    NSDate *date = [NSDate date];
    NSDateFormatter *f = [[NSDateFormatter alloc] init];
    f.dateFormat = @"yyyy-MM-dd HH:mm:ss.SSS";
    f.locale = [[NSLocale alloc] initWithLocaleIdentifier:@"zh_CN"];
    NSString *dstr = [f stringFromDate:date];
    
    NSFileHandle *handle = [NSFileHandle fileHandleForUpdatingAtPath:SigLogger.share.logFilePath];
    [handle seekToEndOfFile];
    if ([data isKindOfClass:[NSData class]]) {
        NSString *tempString = [[NSString alloc] initWithFormat:@"%@ : Response-> %@\n",dstr,data];
        NSData *tempData = [tempString dataUsingEncoding:NSUTF8StringEncoding];
        [handle writeData:tempData];
        
    }else{
        NSString *tempString = [[NSString alloc] initWithFormat:@"%@ : %@\n",dstr,data];
        NSData *tempData = [tempString dataUsingEncoding:NSUTF8StringEncoding];
        [handle writeData:tempData];
    }
    [handle closeFile];
}

static NSFileHandle *TelinkLogFileHandle()
{
    static NSFileHandle *fileHandle = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        NSFileManager *manager = [[NSFileManager alloc] init];
        if (![manager fileExistsAtPath:SigLogger.share.logFilePath]) {
            BOOL ret = [manager createFileAtPath:SigLogger.share.logFilePath contents:nil attributes:nil];
            if (ret) {
                NSLog(@"%@",@"creat success");
            } else {
                NSLog(@"%@",@"creat failure");
            }
        }
        fileHandle = [NSFileHandle fileHandleForUpdatingAtPath:SigLogger.share.logFilePath];
        [fileHandle seekToEndOfFile];
    });
    return fileHandle;
}
void TelinkLogWithFile(NSString *format, ...) {
    va_list L;
    va_start(L, format);
    NSString *message = [[NSString alloc] initWithFormat:format arguments:L];
    if (DEBUG) {
        NSLog(@"%@", message);
    }
    // 开启异步子线程，将打印写入文件
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        NSFileHandle *output = TelinkLogFileHandle();
        if (output != nil) {
            time_t rawtime;
            struct tm timeinfo;
            char buffer[64];
            time(&rawtime);
            localtime_r(&rawtime, &timeinfo);
            struct timeval curTime;
            gettimeofday(&curTime, NULL);
            int milliseconds = curTime.tv_usec / 1000;
            strftime(buffer, 64, "%Y-%m-%d %H:%M", &timeinfo);
            char fullBuffer[128] = { 0 };
            snprintf(fullBuffer, 128, "%s:%2d.%.3d ", buffer, timeinfo.tm_sec, milliseconds);
            [output writeData:[[[NSString alloc] initWithCString:fullBuffer encoding:NSASCIIStringEncoding] dataUsingEncoding:NSUTF8StringEncoding]];
            [output writeData:[message dataUsingEncoding:NSUTF8StringEncoding]];
            static NSData *returnData = nil;
            if (returnData == nil)
                returnData = [@"\n" dataUsingEncoding:NSUTF8StringEncoding];
            [output writeData:returnData];
        }
    });
    va_end(L);
}

- (void)clearAllLog {
    NSFileHandle *handle = [NSFileHandle fileHandleForWritingAtPath:SigLogger.share.logFilePath];
    [handle truncateFileAtOffset:0];
    [handle closeFile];
}

void saveMeshJsonData(id data){
    if (SigLogger.share.logLevel > 0) {
        NSFileHandle *handle = [NSFileHandle fileHandleForUpdatingAtPath:SigLogger.share.meshJsonFilePath];
        [handle truncateFileAtOffset:0];
        if ([data isKindOfClass:[NSData class]]) {
            [handle writeData:(NSData *)data];
        }else{
            NSString *tempString = [[NSString alloc] initWithFormat:@"%@",data];
            NSData *tempData = [tempString dataUsingEncoding:NSUTF8StringEncoding];
            [handle writeData:tempData];
        }
        [handle closeFile];
    }
}

@end
