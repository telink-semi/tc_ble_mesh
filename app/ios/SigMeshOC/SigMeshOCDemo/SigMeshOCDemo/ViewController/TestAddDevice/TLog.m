/********************************************************************************************************
 * @file     TLog.m
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
//  TLog.m
//  SigMesh
//
//  Created by 石 on 2017/11/15.
//  Copyright © 2017年 Arvin.shi. All rights reserved.
//

#import "TLog.h"
#define kDocumentFilePath(name) ([[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject] stringByAppendingPathComponent:name])

NSString *const NotifyUpdateTestLogContent = @"UpdateTestLogContent";

@implementation TLog

+ (TLog *)share{
    static TLog *shareLog = nil;
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        shareLog = [[TLog alloc] init];
    });
    return shareLog;
}

- (void)initLogFile{
    NSFileManager *manager = [[NSFileManager alloc] init];
    if (![manager fileExistsAtPath:self.logFilePath]) {
        BOOL ret = [manager createFileAtPath:self.logFilePath contents:nil attributes:nil];
        if (ret) {
            NSLog(@"creat logFile success");
        } else {
            NSLog(@"creat logFile failure");
        }
    }
}

- (NSString *)logFilePath{
    return [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).lastObject stringByAppendingPathComponent:@"logData"];
}

- (NSString *)allLogString{
    NSFileHandle *handle = [NSFileHandle fileHandleForUpdatingAtPath:[TLog share].logFilePath];
    NSData *data = [handle readDataToEndOfFile];
    NSString *str = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    return str;
}

- (void)clearAllLog{
    NSFileHandle *handle = [NSFileHandle fileHandleForUpdatingAtPath:[TLog share].logFilePath];
    [handle truncateFileAtOffset:0];
    [handle closeFile];
    
    NSString *contentS = @"";
    NSData *tempda = [contentS dataUsingEncoding:NSUTF8StringEncoding];
    [tempda writeToFile:kDocumentFilePath(@"content") atomically:YES];
}

void saveTestLogData(id data){
    NSDate *date = [NSDate date];
    NSDateFormatter *f = [[NSDateFormatter alloc] init];
    f.dateFormat = @"yyyy-MM-dd HH:mm:ss.SSS";
    f.locale = [[NSLocale alloc] initWithLocaleIdentifier:@"zh_CN"];
    NSString *dstr = [f stringFromDate:date];

    NSFileHandle *handle = [NSFileHandle fileHandleForUpdatingAtPath:[TLog share].logFilePath];
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
    [NSNotificationCenter.defaultCenter postNotificationName:(NSString *)NotifyUpdateLogContent object:nil];
}

@end
