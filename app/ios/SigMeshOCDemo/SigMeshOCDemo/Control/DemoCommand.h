/********************************************************************************************************
 * @file     DemoCommand.h 
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
//  DemoCommand.h
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface DemoCommand : NSObject

/// Is get online status from private uuid, YES main callback onoffCallback、brighrnessCallback、tempratureCallback、HSLCallback、levelCallback from OnlineStatusCharacteristic.
+ (BOOL)isPrivatelyGetOnlineStatus;

/// Get Online device, publish use access_cmd_onoff_get(), private use OnlineStatusCharacteristic
+ (BOOL)getOnlineStatus:(BOOL)executeCommand reqCount:(int)reqCount complete:(responseModelCallBack)complete;

/// Get Lum
+ (BOOL)getLum:(BOOL)executeCommand nodeAddress:(UInt16)nodeAddress reqCount:(int)reqCount complete:(responseModelCallBack)complete;

/// Get CTL, (Lum and Temprature)
+ (BOOL)getCTL:(BOOL)executeCommand nodeAddress:(UInt16)nodeAddress reqCount:(int)reqCount complete:(responseModelCallBack)complete;

///change subscribe list（develop can see mesh_node.h line129 to get more detail of option）
+ (void)editSubscribeList:(BOOL)executeCommand groAdd:(UInt16)groAdd nodeAddress:(UInt16)nodeAddress eleAddress:(UInt16)eleAddress add:(BOOL)add option:(UInt32)option complete:(responseModelCallBack)complete;

/// Change publish list
+ (void)editPublishList:(BOOL)executeCommand publishAddress:(UInt16)pub_adr nodeAddress:(UInt16)nodeAddress eleAddress:(UInt16)eleAddress option:(UInt32)option period:(mesh_pub_period_t)period complete:(responseModelCallBack)complete;

/// Get publish address
+ (void)getPublishAddress:(BOOL)executeCommand nodeAddress:(UInt16)nodeAddress eleAddress:(UInt16)eleAddress option:(UInt32)option complete:(responseModelCallBack)complete;

/// Turn on or turn off the lights part
+ (void)switchOnOff:(BOOL)executeCommand on:(BOOL)on address:(UInt16)address resMax:(int)resMax complete:(responseModelCallBack)complete;

/// Change brightness
+ (void)changeBrightness:(BOOL)executeCommand address:(UInt16)address para:(float)para isGet:(BOOL)isGet respondMax:(int)respondMax complete:(responseModelCallBack)complete;

/// Change temprature
+ (void)changeTemprature:(BOOL)executeCommand address:(UInt16)address para:(float)para isGet:(BOOL)isGet respondMax:(int)respondMax complete:(responseModelCallBack)complete;

/// Change HSL
+ (void)changeHSL:(BOOL)executeCommand address:(UInt16)address hue:(UInt8)hue saturation:(UInt8)saturation brightness:(UInt8)brightness isGet:(BOOL)isGet respondMax:(int)respondMax complete:(responseModelCallBack)complete;

/// Change level
+ (void)changeLevel:(BOOL)executeCommand address:(UInt16)address level:(s16)level isGet:(BOOL)isGet respondMax:(int)respondMax complete:(responseModelCallBack)complete;

/// Kick out
+ (void)kickoutDevice:(UInt16)address complete:(resultCallBack)complete;

/// Set all nodes's time to app's time
+ (void)setNowTimeWithComplete:(resultCallBack)complete;

/// Save scene
+ (void)saveSceneWithAddress:(u16)address resMax:(u32)resMax sceneId:(u16)sceneId complete:(responseModelCallBack)complete;

/// Recall scene
+ (void)recallSceneWithAddress:(u16)address resMax:(u32)resMax sceneId:(u16)sceneId complete:(responseModelCallBack)complete;

/// Delete scene
+ (void)delSceneWithAddress:(u16)address resMax:(u32)resMax sceneId:(u16)sceneId complete:(responseModelCallBack)complete;

/// Get scene register status
+ (void)getSceneRegisterStatusWithAddress:(u16)address resMax:(u32)resMax Completation:(responseModelCallBack)complete;

/// Set scheduler action
+ (void)setSchedulerActionWithAddress:(u16)address resMax:(u32)resMax schedulerModel:(SchedulerModel *)schedulerModel complete:(responseModelCallBack)complete;

/// Get scheduler status
+ (void)getSchedulerStatusWithAddress:(u16)address resMax:(u32)resMax Completation:(responseModelCallBack)complete;

/// get scheduler action
+ (void)getSchedulerActionWithAddress:(u16)address resMax:(u32)resMax schedulerModelID:(u8)schedulerModelID Completation:(responseModelCallBack)complete;

/// get fw info
+ (void)getFwInfoWithAddress:(u16)address Completation:(responseModelCallBack)complete;

@end
