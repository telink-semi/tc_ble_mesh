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
//  Created by Liangjiazhi on 2019/8/16.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN


#ifdef DEBUG

/*
 log 写法1：
 */
//#define TeLog(fmt, ...) SigLogger.share.saveLogObject = [NSString stringWithFormat:(@"[%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__];\
//NSLog(@"%@",SigLogger.share.saveLogObject);\
//saveLogData(SigLogger.share.saveLogObject);
//
//#define TeLogError(fmt, ...) if (SigLogger.share.logLevel & SigLogFlagError) {\
//SigLogger.share.saveLogObject = [NSString stringWithFormat:(@"[Error][%@][%s Line %d] " fmt),[NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__];\
//NSLog(@"%@",SigLogger.share.saveLogObject);\
//saveLogData(SigLogger.share.saveLogObject);\
//}
//
//#define TeLogWarn(fmt, ...)     if (SigLogger.share.logLevel & SigLogFlagWarning) {\
//SigLogger.share.saveLogObject = [NSString stringWithFormat:(@"[Warn][%@][%s Line %d] " fmt),[NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__];\
//NSLog(@"%@",SigLogger.share.saveLogObject);\
//saveLogData(SigLogger.share.saveLogObject);\
//}
//
//#define TeLogInfo(fmt, ...)     if (SigLogger.share.logLevel & SigLogFlagInfo) {\
//SigLogger.share.saveLogObject = [NSString stringWithFormat:(@"[Info][%@][%s Line %d] " fmt),[NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__];\
//NSLog(@"%@",SigLogger.share.saveLogObject);\
//saveLogData(SigLogger.share.saveLogObject);\
//}
//
//#define TeLogDebug(fmt, ...)     if (SigLogger.share.logLevel & SigLogFlagDebug) {\
//SigLogger.share.saveLogObject = [NSString stringWithFormat:(@"[Debug][%@][%s Line %d] " fmt),[NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__];\
//NSLog(@"%@",SigLogger.share.saveLogObject);\
//saveLogData(SigLogger.share.saveLogObject);\
//}
//
//#define TeLogVerbose(fmt, ...)     if (SigLogger.share.logLevel & SigLogFlagVerbose) {\
//SigLogger.share.saveLogObject = [NSString stringWithFormat:(@"[Verbose][%@][%s Line %d] " fmt),[NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__];\
//NSLog(@"%@",SigLogger.share.saveLogObject);\
//saveLogData(SigLogger.share.saveLogObject);\
//}


/*
log 写法2：
*/
//#define TeLog(fmt, ...) @synchronized (SigLogger.share) {\
//SigLogger.share.saveLogObject = [NSString stringWithFormat:(@"[%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__];\
//NSLog(@"%@",SigLogger.share.saveLogObject);\
//saveLogData(SigLogger.share.saveLogObject);\
//}
//
//#define TeLogError(fmt, ...) @synchronized (SigLogger.share) {\
//if (SigLogger.share.logLevel & SigLogFlagError) {\
//SigLogger.share.saveLogObject = [NSString stringWithFormat:(@"[Error][%@][%s Line %d] " fmt),[NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__];\
//NSLog(@"%@",SigLogger.share.saveLogObject);\
//saveLogData(SigLogger.share.saveLogObject);\
//}\
//}
//
//#define TeLogWarn(fmt, ...)     @synchronized (SigLogger.share) {\
//if (SigLogger.share.logLevel & SigLogFlagWarning) {\
//SigLogger.share.saveLogObject = [NSString stringWithFormat:(@"[Warn][%@][%s Line %d] " fmt),[NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__];\
//NSLog(@"%@",SigLogger.share.saveLogObject);\
//saveLogData(SigLogger.share.saveLogObject);\
//}\
//}
//
//#define TeLogInfo(fmt, ...)     @synchronized (SigLogger.share) {\
//if (SigLogger.share.logLevel & SigLogFlagInfo) {\
//SigLogger.share.saveLogObject = [NSString stringWithFormat:(@"[Info][%@][%s Line %d] " fmt),[NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__];\
//NSLog(@"%@",SigLogger.share.saveLogObject);\
//saveLogData(SigLogger.share.saveLogObject);\
//}\
//}
//
//#define TeLogDebug(fmt, ...)     @synchronized (SigLogger.share) {\
//if (SigLogger.share.logLevel & SigLogFlagDebug) {\
//SigLogger.share.saveLogObject = [NSString stringWithFormat:(@"[Debug][%@][%s Line %d] " fmt),[NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__];\
//NSLog(@"%@",SigLogger.share.saveLogObject);\
//saveLogData(SigLogger.share.saveLogObject);\
//}\
//}
//
//#define TeLogVerbose(fmt, ...)     @synchronized (SigLogger.share) {\
//if (SigLogger.share.logLevel & SigLogFlagVerbose) {\
//SigLogger.share.saveLogObject = [NSString stringWithFormat:(@"[Verbose][%@][%s Line %d] " fmt),[NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__];\
//NSLog(@"%@",SigLogger.share.saveLogObject);\
//saveLogData(SigLogger.share.saveLogObject);\
//}\
//}


/*
log 写法3.1：测试验证线程
*/
# define TeLog(fmt, ...) TelinkLogWithFile((@"[%@][%s Line %d] " fmt), [NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__);

#define TeLogError(fmt, ...) if (SigLogger.share.logLevel & SigLogFlagError) {\
TelinkLogWithFile((@"[Error][%@][%s Line %d] " fmt), [NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__);\
}

#define TeLogWarn(fmt, ...) if (SigLogger.share.logLevel & SigLogFlagWarning) {\
TelinkLogWithFile((@"[Warn][%@][%s Line %d] " fmt), [NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__);\
}

#define TeLogInfo(fmt, ...) if (SigLogger.share.logLevel & SigLogFlagInfo) {\
TelinkLogWithFile((@"[Info][%@][%s Line %d] " fmt), [NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__);\
}

#define TeLogDebug(fmt, ...) if (SigLogger.share.logLevel & SigLogFlagDebug) {\
TelinkLogWithFile((@"[Debug][%@][%s Line %d] " fmt), [NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__);\
}

#define TeLogVerbose(fmt, ...) if (SigLogger.share.logLevel & SigLogFlagVerbose) {\
TelinkLogWithFile((@"[Verbose][%@][%s Line %d] " fmt), [NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__);\
}


/*
log 写法3.2：
*/
//# define TeLog(fmt, ...) TelinkLogWithFile((@"[%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);
//
//#define TeLogError(fmt, ...) if (SigLogger.share.logLevel & SigLogFlagError) {\
//TelinkLogWithFile((@"[Error][%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);\
//}
//
//#define TeLogWarn(fmt, ...) if (SigLogger.share.logLevel & SigLogFlagWarning) {\
//TelinkLogWithFile((@"[Warn][%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);\
//}
//
//#define TeLogInfo(fmt, ...) if (SigLogger.share.logLevel & SigLogFlagInfo) {\
//TelinkLogWithFile((@"[Info][%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);\
//}
//
//#define TeLogDebug(fmt, ...) if (SigLogger.share.logLevel & SigLogFlagDebug) {\
//TelinkLogWithFile((@"[Debug][%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);\
//}
//
//#define TeLogVerbose(fmt, ...) if (SigLogger.share.logLevel & SigLogFlagVerbose) {\
//TelinkLogWithFile((@"[Verbose][%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__);\
//}

#else

#define TeLog(fmt, ...) SigLogger.share.saveLogObject = [NSString stringWithFormat:(@"[%s Line %d] " fmt), __func__, __LINE__, ##__VA_ARGS__];\
saveLogData(SigLogger.share.saveLogObject);

#define TeLogError(fmt, ...)     if (SigLogger.share.logLevel & SigLogFlagError) {\
SigLogger.share.saveLogObject = [NSString stringWithFormat:(@"[Error][%@][%s Line %d] " fmt),[NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__];\
saveLogData(SigLogger.share.saveLogObject);\
}

#define TeLogWarn(fmt, ...)     if (SigLogger.share.logLevel & SigLogFlagWarning) {\
SigLogger.share.saveLogObject = [NSString stringWithFormat:(@"[Warn][%@][%s Line %d] " fmt),[NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__];\
saveLogData(SigLogger.share.saveLogObject);\
}

#define TeLogInfo(fmt, ...)     if (SigLogger.share.logLevel & SigLogFlagInfo) {\
SigLogger.share.saveLogObject = [NSString stringWithFormat:(@"[Info][%@][%s Line %d] " fmt),[NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__];\
saveLogData(SigLogger.share.saveLogObject);\
}

#define TeLogDebug(fmt, ...)     if (SigLogger.share.logLevel & SigLogFlagDebug) {\
SigLogger.share.saveLogObject = [NSString stringWithFormat:(@"[Debug][%@][%s Line %d] " fmt),[NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__];\
saveLogData(SigLogger.share.saveLogObject);\
}

#define TeLogVerbose(fmt, ...)     if (SigLogger.share.logLevel & SigLogFlagVerbose) {\
SigLogger.share.saveLogObject = [NSString stringWithFormat:(@"[Verbose][%@][%s Line %d] " fmt),[NSThread currentThread], __func__, __LINE__, ##__VA_ARGS__];\
saveLogData(SigLogger.share.saveLogObject);\
}

#endif



UIKIT_EXTERN NSString *const NotifyUpdateLogContent;

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
@property (strong,nonatomic) NSObject *saveLogObject;
@property (assign,nonatomic,readonly) SigLogLevel logLevel;
@property (strong,nonatomic,readonly) NSString *logFilePath;

+ (SigLogger *)share;

- (void)setSDKLogLevel:(SigLogLevel)logLevel;

/**
 log
 
 @param data The log that save location, data is always NSString.
 */
void saveLogData(id data);

- (void)clearAllLog;

void saveMeshJsonData(id data);

/**
 自定义打印，会自动写文件
 */
void TelinkLogWithFile(NSString *format, ...);

@end

NS_ASSUME_NONNULL_END
