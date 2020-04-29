/********************************************************************************************************
* @file     SigRemoteAddManager.h
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
//  SigRemoteAddManager.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2020/3/26.
//  Copyright © 2020 梁家誌. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef void(^remoteProvisioningScanReportCallBack)(SigRemoteScanRspModel *scanRemoteModel);

@interface SigRemoteAddManager : NSObject


+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));


+ (SigRemoteAddManager *)share;

- (void)startRemoteProvisionScanWithReportCallback:(remoteProvisioningScanReportCallBack)reportCallback resultCallback:(resultBlock)resultCallback;

@end

NS_ASSUME_NONNULL_END
