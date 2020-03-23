/********************************************************************************************************
 * @file     OTAFileSource.m 
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
//  OTAFileSource.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "OTAFileSource.h"

@interface OTAFileSource()
@property (nonatomic, strong) NSMutableArray *fileSource;
@end

@implementation OTAFileSource

+ (OTAFileSource *)share{
    static OTAFileSource *shareFS = nil;
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        shareFS = [[OTAFileSource alloc] init];
        shareFS.fileSource = [[NSMutableArray alloc] init];
    });
    return shareFS;
}

- (NSArray <NSString *>*)getAllBinFile{
    [_fileSource removeAllObjects];
    
    // 搜索bin文件的目录(工程内部添加的bin文件)
    NSArray *paths = [[NSBundle mainBundle] pathsForResourcesOfType:@"bin" inDirectory:nil];
    NSFileManager *fileManager = [NSFileManager defaultManager];
    for (NSString *binPath in paths) {
        NSString *binName = [fileManager displayNameAtPath:binPath];
        [self addBinFileWithPath:binName];
    }
    //搜索Documents(通过iTunes File 加入的文件需要在此搜索)
    NSFileManager *mang = [NSFileManager defaultManager];
    NSError *error = nil;
    NSString *fileLocalPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject];
    NSArray *fileNames = [mang contentsOfDirectoryAtPath:fileLocalPath error:&error];
    for (NSString *path in fileNames) {
        //本地512k的存储文件名为“test.bin”，不可用于OTA
        if ([path containsString:@".bin"] && ![path containsString:@"test.bin"]) {
            [self addBinFileWithPath:path];
        }
    }
    
    return _fileSource;
}

- (void)addBinFileWithPath:(NSString *)path{
    path = [path stringByReplacingOccurrencesOfString:@".bin" withString:@""];
    [_fileSource addObject:path];
}

- (NSArray <NSString *>*)getAllMeshJsonFile{
    [_fileSource removeAllObjects];
    
//    // 搜索bin文件的目录(工程内部添加的bin文件)
//    NSArray *paths = [[NSBundle mainBundle] pathsForResourcesOfType:@"json" inDirectory:nil];
//    NSFileManager *fileManager = [NSFileManager defaultManager];
//    for (NSString *binPath in paths) {
//        NSString *binName = [fileManager displayNameAtPath:binPath];
//        [_fileSource addObject:binName];
//    }
    //搜索Documents(通过iTunes File 加入的文件需要在此搜索)
    NSFileManager *mang = [NSFileManager defaultManager];
    NSError *error = nil;
    NSString *fileLocalPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject];
    NSArray *fileNames = [mang contentsOfDirectoryAtPath:fileLocalPath error:&error];
    for (NSString *path in fileNames) {
        if ([path containsString:@".json"]) {
            [_fileSource addObject:path];
        }
    }
    
    return _fileSource;
}

- (NSData *)getDataWithBinName:(NSString *)binName{
    NSData *data = [[NSFileHandle fileHandleForReadingAtPath:[[NSBundle mainBundle] pathForResource:binName ofType:@"bin"]] readDataToEndOfFile];
    if (!data) {
        //通过iTunes File 加入的文件
        NSString *fileLocalPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject];
        fileLocalPath = [NSString stringWithFormat:@"%@/%@.bin",fileLocalPath,binName];
        NSError *err = nil;
        NSFileHandle *fileHandle = [NSFileHandle fileHandleForReadingFromURL:[NSURL URLWithString:fileLocalPath] error:&err];
        data = fileHandle.readDataToEndOfFile;
    }
    return data;
}

- (NSData *)getDataWithMeshJsonName:(NSString *)jsonName{
    NSData *data = [[NSFileHandle fileHandleForReadingAtPath:[[NSBundle mainBundle] pathForResource:jsonName ofType:nil]] readDataToEndOfFile];
    if (!data) {
        //通过iTunes File 加入的文件
        NSString *fileLocalPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject];
        fileLocalPath = [NSString stringWithFormat:@"%@/%@",fileLocalPath,jsonName];
        NSError *err = nil;
        data = [NSData dataWithContentsOfFile:fileLocalPath options:0 error:&err];
    }
    return data;
}

@end
