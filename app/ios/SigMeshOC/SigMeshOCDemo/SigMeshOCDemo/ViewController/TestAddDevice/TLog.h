/********************************************************************************************************
 * @file     TLog.h 
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
//  TLog.h
//  SigMesh
//
//  Created by 石 on 2017/11/15.
//  Copyright © 2017年 Arvin.shi. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

UIKIT_EXTERN NSString *const NotifyUpdateTestLogContent;

@interface TLog : NSObject


+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));


+ (TLog *)share;

- (void)initLogFile;

- (NSString *)logFilePath;

- (NSString *)allLogString;

- (void)clearAllLog;

/**
 log
 
 @param data 存储本地的log数据，一般使用NSString类型
 */
void saveTestLogData(id data);

@end
