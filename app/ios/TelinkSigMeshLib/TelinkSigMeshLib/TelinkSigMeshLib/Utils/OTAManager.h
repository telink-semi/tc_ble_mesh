/********************************************************************************************************
 * @file     OTAManager.h 
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
//  OTAManager.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2018/7/18.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef void(^singleDeviceCallBack)(SigNodeModel *device);
typedef void(^singleProgressCallBack)(float progress);
typedef void(^finishCallBack)(NSArray <SigNodeModel *>*successModels,NSArray <SigNodeModel *>*fileModels);

@interface OTAManager : NSObject


+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));


+ (OTAManager *)share;

/**
 OTA，can not call repeat when app is OTAing
 
 @param otaData data for OTA
 @param models models for OTA
 @param singleSuccessAction callback when single model OTA  success
 @param singleFailAction callback when single model OTA  fail
 @param singleProgressAction callback with single model OTA progress
 @param finishAction callback when all models OTA finish
 @return  ture when call API success;false when call API fail.
 */
- (BOOL)startOTAWithOtaData:(NSData *)otaData models:(NSArray <SigNodeModel *>*)models singleSuccessAction:(singleDeviceCallBack)singleSuccessAction singleFailAction:(singleDeviceCallBack)singleFailAction singleProgressAction:(singleProgressCallBack)singleProgressAction finishAction:(finishCallBack)finishAction;

/// stop OTA
- (void)stopOTA;

@end
