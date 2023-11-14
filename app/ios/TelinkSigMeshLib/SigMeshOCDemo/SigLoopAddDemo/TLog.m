/********************************************************************************************************
 * @file     TLog.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/3/7
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

#import "TLog.h"
#define kDocumentFilePath(name) ([[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject] stringByAppendingPathComponent:name])

NSString *const NotifyUpdateLogContent = @"UpdateLogContent";
NSString *const NotifyUpdateTestLogContent = @"UpdateTestLogContent";

@implementation TLog

/**
 *  @brief  Singleton method
 *
 *  @return the default singleton instance. You are not allowed to create your own instances of this class.
 */
+ (instancetype)share {
    /// Singleton instance
    static TLog *shareLog = nil;
    /// Note: The dispatch_once function can ensure that a certain piece
    /// of code is only executed once in the entire application life cycle!
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        /// Initialize the Singleton configure parameters.
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
