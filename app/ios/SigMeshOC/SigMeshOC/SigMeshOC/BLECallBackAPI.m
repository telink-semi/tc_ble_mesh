/********************************************************************************************************
 * @file     BLECallBackAPI.m 
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
//  BLECallBackAPI.m
//  SigMeshOC
//
//  Created by Liangjiazhi on 2018/7/12.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "BLECallBackAPI.h"

@implementation BLECallBackAPI

- (void)bleCentralUpdateState:(bleCentralUpdateStateCallBack)bleCentralUpdateStateCallBack{
    _bleCentralUpdateStateCallBack = bleCentralUpdateStateCallBack;
}

- (void)bleScanNewDevice:(bleScanNewDeviceCallBack)bleScanNewDeviceCallBack{
    _bleScanNewDeviceCallBack = bleScanNewDeviceCallBack;
}

- (void)bleDisconnectOrConnectFail:(bleDisconnectOrConnectFailCallBack)bleDisconnectOrConnectFailCallBack{
    _bleDisconnectOrConnectFailCallBack = bleDisconnectOrConnectFailCallBack;
}

- (void)bleReadOTACharachteristic:(BOOL)isNeedRead complete:(bleReadOTACharachteristicCallBack)bleReadOTACharachteristicCallBack{
    if (isNeedRead) {
        [self.store readOTACharachteristicForContinue];
    }
    _bleReadOTACharachteristicCallBack = bleReadOTACharachteristicCallBack;
}

- (void)bleFinishScanedCharachteristic:(bleFinishScanedCharachteristicCallBack)bleFinishScanedCharachteristicCallBack{
    _bleFinishScanedCharachteristicCallBack = bleFinishScanedCharachteristicCallBack;
}

@end
