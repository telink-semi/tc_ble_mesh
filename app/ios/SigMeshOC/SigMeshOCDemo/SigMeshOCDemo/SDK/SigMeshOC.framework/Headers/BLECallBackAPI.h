/********************************************************************************************************
 * @file     BLECallBackAPI.h 
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
//  BLECallBackAPI.h
//  SigMeshOC
//
//  Created by Liangjiazhi on 2018/7/12.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "BLEProperties.h"

typedef void(^bleCentralUpdateStateCallBack)(CBCentralManagerState state);
typedef void(^bleScanNewDeviceCallBack)(CBPeripheral *peripheral,BOOL provisioned);
typedef void(^bleDisconnectOrConnectFailCallBack)(CBPeripheral *peripheral);
typedef void(^bleReadOTACharachteristicCallBack)(CBCharacteristic *characteristic);
typedef void(^bleFinishScanedCharachteristicCallBack)(CBPeripheral *peripheral);
typedef void(^addDevice_prvisionSuccessCallBack)(NSString *identify,UInt16 address);
typedef void(^addDevice_keyBindSuccessCallBack)(NSString *identify,UInt16 address);
typedef void(^prvisionFailCallBack)(NSString *errorString);
typedef void(^prvisionFinishCallBack)(void);
typedef void(^workWithPeripheralCallBack)(NSString *uuidString);
typedef void(^bleCancelConnectCallBack)(void);
typedef void(^bleSetFilterResponseCallBack)(void);
typedef void(^bleSetFilterFailCallBack)(void);
typedef void(^bleSetFilterResultCallBack)(BOOL isSuccess);
typedef void(^bleCompleteCallBack)(void);
typedef void(^ErrorBlock)(NSError *error);


@interface BLECallBackAPI : BLEProperties

@property (nonatomic,copy) bleCentralUpdateStateCallBack bleCentralUpdateStateCallBack;
@property (nonatomic,copy) bleScanNewDeviceCallBack bleScanNewDeviceCallBack;
@property (nonatomic,copy) bleDisconnectOrConnectFailCallBack bleDisconnectOrConnectFailCallBack;
@property (nonatomic,copy) bleReadOTACharachteristicCallBack bleReadOTACharachteristicCallBack;
@property (nonatomic,copy) bleFinishScanedCharachteristicCallBack bleFinishScanedCharachteristicCallBack;
@property (nonatomic,copy) addDevice_prvisionSuccessCallBack addDevice_prvisionSuccessCallBack;
@property (nonatomic,copy) addDevice_keyBindSuccessCallBack addDevice_keyBindSuccessCallBack;
@property (nonatomic,copy) prvisionFailCallBack prvisionFailCallBack;
@property (nonatomic,copy) prvisionFinishCallBack prvisionFinishCallBack;
@property (nonatomic,copy) workWithPeripheralCallBack workWithPeripheralCallBack;
@property (nonatomic,copy) bleCancelConnectCallBack cancelConnectCallBack;
@property (nonatomic,copy) bleSetFilterResponseCallBack setFilterResponseCallBack;
@property (nonatomic,copy) bleSetFilterFailCallBack setFilterFailCallBack;
@property (nonatomic,copy) bleCompleteCallBack noBusyCallBack;

- (void)bleCentralUpdateState:(bleCentralUpdateStateCallBack)bleCentralUpdateStateCallBack;
- (void)bleScanNewDevice:(bleScanNewDeviceCallBack)bleScanNewDeviceCallBack;
- (void)bleDisconnectOrConnectFail:(bleDisconnectOrConnectFailCallBack)bleDisconnectOrConnectFailCallBack;
- (void)bleReadOTACharachteristic:(BOOL)isNeedRead complete:(bleReadOTACharachteristicCallBack)bleReadOTACharachteristicCallBack;
- (void)bleFinishScanedCharachteristic:(bleFinishScanedCharachteristicCallBack)bleFinishScanedCharachteristicCallBack;

@end
