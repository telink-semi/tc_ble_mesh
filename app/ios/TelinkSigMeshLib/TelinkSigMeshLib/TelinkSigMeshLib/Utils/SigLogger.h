/********************************************************************************************************
 * @file     SigLogger.h
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/8/16
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

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

/*
log 写法2：
*/
#define TelinkLog(fmt, ...) TelinkLogWithFile(YES,(@"[%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);

#define TelinkLogError(fmt, ...) if (SigLogger.share.logLevel & SigLogFlagError) {\
TelinkLogWithFile(SigLogger.share.logLevel & SigLogFlagError,(@"[Error][%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);\
} else if (SigLogger.share.logLevel) {\
TelinkLogWithFile(NO,(@"[Error][%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);\
}

#define TelinkLogWarn(fmt, ...) if (SigLogger.share.logLevel & SigLogFlagWarning) {\
TelinkLogWithFile(YES,(@"[Warn][%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);\
} else if (SigLogger.share.logLevel) {\
TelinkLogWithFile(NO,(@"[Warn][%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);\
}

#define TelinkLogInfo(fmt, ...) if (SigLogger.share.logLevel & SigLogFlagInfo) {\
TelinkLogWithFile(YES,(@"[Info][%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);\
} else if (SigLogger.share.logLevel) {\
TelinkLogWithFile(NO,(@"[Info][%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);\
}

#define TelinkLogDebug(fmt, ...) if (SigLogger.share.logLevel & SigLogFlagDebug) {\
TelinkLogWithFile(YES,(@"[Debug][%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);\
} else if (SigLogger.share.logLevel) {\
TelinkLogWithFile(NO,(@"[Debug][%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);\
}

#define TelinkLogVerbose(fmt, ...) if (SigLogger.share.logLevel & SigLogFlagVerbose) {\
TelinkLogWithFile(YES,(@"[Verbose][%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);\
} else if (SigLogger.share.logLevel) {\
TelinkLogWithFile(NO,(@"[Verbose][%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);\
}


typedef enum : UInt8 {
    // 00000001 SigLogFlagError
    SigLogFlagError = (1 << 0),
    // 00000010 SigLogFlagWarning
    SigLogFlagWarning = (1 << 1),
    // 00000100 SigLogFlagInfo
    SigLogFlagInfo = (1 << 2),
    // 00001000 SigLogFlagDebug
    SigLogFlagDebug = (1 << 3),
    // 00010000 SigLogFlagVerbose
    SigLogFlagVerbose = (1 << 4),
} SigLogFlag;

typedef enum : UInt8 {
    // No logs
    SigLogLevelOff = 0,
    // Error logs only
    SigLogLevelError = (SigLogFlagError),
    // Error and warning logs
    SigLogLevelWarning = (SigLogLevelError | SigLogFlagWarning),
    // Error, warning and info logs
    SigLogLevelInfo = (SigLogLevelWarning | SigLogFlagInfo),
    // Error, warning, info and debug logs
    SigLogLevelDebug = (SigLogLevelInfo | SigLogFlagDebug),
    // Error, warning, info, debug and verbose logs
    SigLogLevelVerbose = (SigLogLevelDebug | SigLogFlagVerbose),
    // All logs (11111111)
    SigLogLevelAll = 0xFF,
} SigLogLevel;

@interface SigLogger : NSObject
@property (assign,nonatomic,readonly) SigLogLevel logLevel;


+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));


/**
 *  @brief  Singleton method
 *
 *  @return the default singleton instance. You are not allowed to create your own instances of this class.
 */
+ (instancetype)share;

/// 设置log等级，debug模式设置为SigLogLevelDebug即可，上架推荐使用SigLogLevelOff。
- (void)setSDKLogLevel:(SigLogLevel)logLevel;

/// 清除所有log
- (void)clearAllLog;

/// 获取特定长度的log字符串
- (NSString *)getLogStringWithLength:(NSInteger)length;

/// 缓存加密的json数据于iTunes中
void saveMeshJsonData(id data);

/// 解密iTunes中缓存的加密的json数据
- (NSString *)getDecryptTelinkSDKMeshJsonData;

/// 使用秘钥解密iTunes中缓存的加密的json数据
- (NSString *)getDecryptTelinkSDKMeshJsonDataWithPassword:(NSString *)password;

/**
 自定义打印，会自动写文件
 */
extern void TelinkLogWithFile(BOOL show,NSString *format, ...);

@end

NS_ASSUME_NONNULL_END
