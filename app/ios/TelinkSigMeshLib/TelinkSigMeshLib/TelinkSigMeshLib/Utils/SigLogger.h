/********************************************************************************************************
 * @file     SigLogger.h
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
//  SigLogger.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/8/16.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN


/*
log 写法1：测试验证线程
*/
//# define TeLog(fmt, ...) TelinkLogWithFile((@"[%@][%s Line %d] " fmt), [NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__);
//
//#define TeLogError(fmt, ...) if (SigLogger.share.logLevel & SigLogFlagError) {\
//TelinkLogWithFile((@"[Error][%@][%s Line %d] " fmt), [NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__);\
//}
//
//#define TeLogWarn(fmt, ...) if (SigLogger.share.logLevel & SigLogFlagWarning) {\
//TelinkLogWithFile((@"[Warn][%@][%s Line %d] " fmt), [NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__);\
//}
//
//#define TeLogInfo(fmt, ...) if (SigLogger.share.logLevel & SigLogFlagInfo) {\
//TelinkLogWithFile((@"[Info][%@][%s Line %d] " fmt), [NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__);\
//}
//
//#define TeLogDebug(fmt, ...) if (SigLogger.share.logLevel & SigLogFlagDebug) {\
//TelinkLogWithFile((@"[Debug][%@][%s Line %d] " fmt), [NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__);\
//}
//
//#define TeLogVerbose(fmt, ...) if (SigLogger.share.logLevel & SigLogFlagVerbose) {\
//TelinkLogWithFile((@"[Verbose][%@][%s Line %d] " fmt), [NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__);\
//}


/*
log 写法2：
*/
# define TeLog(fmt, ...) TelinkLogWithFile(YES,(@"[%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);

#define TeLogError(fmt, ...) if (SigLogger.share.logLevel & SigLogFlagError) {\
TelinkLogWithFile(SigLogger.share.logLevel & SigLogFlagError,(@"[Error][%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);\
} else {\
TelinkLogWithFile(NO,(@"[Error][%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);\
}

#define TeLogWarn(fmt, ...) if (SigLogger.share.logLevel & SigLogFlagWarning) {\
TelinkLogWithFile(YES,(@"[Warn][%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);\
} else {\
TelinkLogWithFile(NO,(@"[Warn][%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);\
}

#define TeLogInfo(fmt, ...) if (SigLogger.share.logLevel & SigLogFlagInfo) {\
TelinkLogWithFile(YES,(@"[Info][%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);\
} else {\
TelinkLogWithFile(NO,(@"[Info][%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);\
}

#define TeLogDebug(fmt, ...) if (SigLogger.share.logLevel & SigLogFlagDebug) {\
TelinkLogWithFile(YES,(@"[Debug][%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);\
} else {\
TelinkLogWithFile(NO,(@"[Debug][%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);\
}

#define TeLogVerbose(fmt, ...) if (SigLogger.share.logLevel & SigLogFlagVerbose) {\
TelinkLogWithFile(YES,(@"[Verbose][%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);\
} else {\
TelinkLogWithFile(NO,(@"[Verbose][%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);\
}


typedef enum : NSUInteger {
    /**
         *  0...00001 SigLogFlagError
         */
    SigLogFlagError = (1 << 0),
    /**
         *  0...00010 SigLogFlagWarning
         */
    SigLogFlagWarning = (1 << 1),
    /**
         *  0...00100 SigLogFlagInfo
         */
    SigLogFlagInfo = (1 << 2),
    /**
         *  0...01000 SigLogFlagDebug
         */
    SigLogFlagDebug = (1 << 3),
    /**
         *  0...10000 SigLogFlagVerbose
         */
    SigLogFlagVerbose = (1 << 4),
} SigLogFlag;

typedef enum : NSUInteger {
    /**
         *  No logs
         */
    SigLogLevelOff = 0,
    /**
         *  Error logs only
         */
    SigLogLevelError = (SigLogFlagError),
    /**
         *  Error and warning logs
         */
    SigLogLevelWarning = (SigLogLevelError | SigLogFlagWarning),
    /**
         *  Error, warning and info logs
         */
    SigLogLevelInfo = (SigLogLevelWarning | SigLogFlagInfo),
    /**
         *  Error, warning, info and debug logs
         */
    SigLogLevelDebug = (SigLogLevelInfo | SigLogFlagDebug),
    /**
         *  Error, warning, info, debug and verbose logs
         */
    SigLogLevelVerbose = (SigLogLevelDebug | SigLogFlagVerbose),
    /**
        *  All logs (1...11111)
        */
    SigLogLevelAll = NSUIntegerMax,
} SigLogLevel;

@interface SigLogger : NSObject
@property (assign,nonatomic,readonly) SigLogLevel logLevel;


+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));


+ (SigLogger *)share;

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
- (NSString *)getDecryptTelinkSDKMeshJsonDataWithPassword:(NSString *)password;

/**
 自定义打印，会自动写文件
 */
void TelinkLogWithFile(BOOL show,NSString *format, ...);

@end

NS_ASSUME_NONNULL_END
