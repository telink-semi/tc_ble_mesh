/********************************************************************************************************
 * @file     OTAFileSource.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/7/31
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "OTAFileSource.h"

@interface OTAFileSource()
@property (nonatomic, strong) NSMutableArray *fileSource;
@property (nonatomic, strong) NSMutableArray *filePaths;
@end

@implementation OTAFileSource

/**
 *  @brief  Singleton method
 *
 *  @return the default singleton instance.
 */
+ (instancetype)share {
    /// Singleton instance
    static OTAFileSource *shareFS = nil;
    /// Note: The dispatch_once function can ensure that a certain piece
    /// of code is only executed once in the entire application life cycle!
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        /// Initialize the Singleton configure parameters.
        shareFS = [[OTAFileSource alloc] init];
        shareFS.fileSource = [[NSMutableArray alloc] init];
        shareFS.filePaths = [[NSMutableArray alloc] init];
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
//        //本地512k的存储文件名为“test.bin”，不可用于OTA。包含空格的bin文件会读取失败。
//        if ([path containsString:@".bin"] && ![path containsString:@"test.bin"] && ![path containsString:@" "]) {
//            [self addBinFileWithPath:path];
//        }
        //本地512k的存储文件名为“test.bin”，不可用于OTA。包含空格的bin文件可能会读取失败。
        if ([path containsString:@".bin"] && ![path containsString:@"test.bin"]) {
            [self addBinFileWithPath:path];
        }
    }

    return _fileSource;
}

- (NSArray <NSString *>*)getAllBinFilePath {
    [_filePaths removeAllObjects];

    // 搜索bin文件的目录(工程内部添加的bin文件)
    NSArray *bins = [[NSBundle mainBundle] pathsForResourcesOfType:@"bin" inDirectory:nil];
    for (int i = 0; i<bins.count; i++) {
        [_filePaths addObject:bins[i]];
    }

    //搜索Documents(通过iTunes File 加入的文件需要在此搜索)
    NSFileManager *manager = [NSFileManager defaultManager];
    NSError *error = nil;
    NSString *fileLocalPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject];
    NSArray *fileNames = [manager contentsOfDirectoryAtPath:fileLocalPath error:&error];
    for (NSString *path in fileNames) {
        if ([path containsString:@".bin"]) {
            [_filePaths addObject:[fileLocalPath stringByAppendingPathComponent:path]];
        }
    }
    return _filePaths;
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
    //可能出现读取失败的情况
//    NSData *data = [[NSFileHandle fileHandleForReadingAtPath:[[NSBundle mainBundle] pathForResource:binName ofType:@"bin"]] readDataToEndOfFile];
//    if (!data) {
//        //通过iTunes File 加入的文件
//        NSString *fileLocalPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject];
//        fileLocalPath = [NSString stringWithFormat:@"%@/%@.bin",fileLocalPath,binName];
//        NSError *err = nil;
//        NSFileHandle *fileHandle = [NSFileHandle fileHandleForReadingFromURL:[NSURL URLWithString:fileLocalPath] error:&err];
//        data = fileHandle.readDataToEndOfFile;
//    }
//    return data;

    //路径中间的UUID可能会改变。所以使用这种写法，解决上面读取失败的问题。
    NSArray *tem = [NSArray arrayWithArray:self.getAllBinFilePath];
    NSString *path = nil;
    for (NSString *filePath in tem) {
        if ([filePath.lastPathComponent isEqualToString:[NSString stringWithFormat:@"%@.bin", binName]]) {
            path = filePath;
            break;
        }
    }
    NSData *data = [NSData dataWithContentsOfFile:path];
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

- (UInt16)getPidWithOTAData:(NSData *)data {
    UInt16 pid = 0;
    Byte *tempBytes = (Byte *)[data bytes];
    if (data && data.length >= 4) {
        memcpy(&pid, tempBytes + 0x2, 2);
    }
    return pid;
}

- (UInt16)getVidWithOTAData:(NSData *)data {
    UInt16 vid = 0;
    Byte *tempBytes = (Byte *)[data bytes];
    if (data && data.length >= 6) {
        memcpy(&vid, tempBytes + 0x4, 2);
    }
    return vid;
}

@end
