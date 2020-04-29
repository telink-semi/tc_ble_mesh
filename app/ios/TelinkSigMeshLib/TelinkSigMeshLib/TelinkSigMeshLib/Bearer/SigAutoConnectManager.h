/********************************************************************************************************
* @file     SigAutoConnectManager.h
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
//  SigAutoConnectManager.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/12/4.
//  Copyright © 2019 梁家誌. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef void(^connectSuccessCallBack)(NSString *uuidString);
typedef void(^connectFailCallBack)(NSString *errorString);

@interface SigAutoConnectManager : NSObject
@property (nonatomic,strong) CBPeripheral *peripheral;
@property (nonatomic,assign) BOOL autoConnect;


/// connect for normal: scan -> connect -> readGATT -> open notify -> callback(get status)
/// @param complete call back when success
- (void)startAutoConnectWithComplete:(connectSuccessCallBack)complete;


/// connect for normal: scan -> connect -> readGATT -> open notify -> callback(get status)
/// @param timeout time from scan to callback
/// @param successBlock call back when success
/// @param failBlock call back when fail
- (void)startConnectWithTimeout:(UInt8)timeout successBlock:(connectSuccessCallBack)successBlock failBlock:(connectFailCallBack)failBlock;


/// api use to stop auto connect. (eg: stop connection to change meshNerwork, stop connection to add node)
- (void)stopCurrentMeshConnectAndStopAutoConnect;


/// api use after SigBearer had disconnected, SigAutoConnectManager will auto connect at there.
- (void)meshNetworkDisconnected;

@end

NS_ASSUME_NONNULL_END
