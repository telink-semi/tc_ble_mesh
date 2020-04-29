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

/// Get Online device, publish use SigGenericOnOffGet, private use OnlineStatusCharacteristic
+ (BOOL)getOnlineStatusWithResponseMaxCount:(int)responseMaxCount successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// Get Lum
+ (BOOL)getLumWithNodeAddress:(UInt16)nodeAddress responseMacCount:(int)responseMacCount successCallback:(responseLightLightnessStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// Get CTL, (Lum and Temprature)
+ (BOOL)getCTLWithNodeAddress:(UInt16)nodeAddress responseMacCount:(int)responseMacCount successCallback:(responseLightCTLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

///change subscribe list（develop can see mesh_node.h line129 to get more detail of option）
+ (BOOL)editSubscribeListWithWithDestination:(UInt16)destination isAdd:(BOOL)isAdd groupAddress:(UInt16)groupAddress elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelSubscriptionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// Change publish list
+ (BOOL)editPublishListWithPublishAddress:(UInt16)publishAddress nodeAddress:(UInt16)nodeAddress elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier periodSteps:(UInt8)periodSteps periodResolution:(SigStepResolution)periodResolution retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// Get publish address
+ (BOOL)getPublishAddressWithNodeAddress:(UInt16)nodeAddress elementAddress:(UInt16)elementAddress modelIdentifier:(UInt16)modelIdentifier companyIdentifier:(UInt16)companyIdentifier retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigModelPublicationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// Turn on or turn off the lights part
+ (BOOL)switchOnOffWithIsOn:(BOOL)isOn address:(UInt16)address responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// Change brightness
+ (BOOL)changeBrightnessWithBrightness100:(UInt8)brightness100 address:(UInt16)address retryCount:(NSInteger)retryCount responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseLightLightnessStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// Change temprature
+ (BOOL)changeTempratureWithTemprature100:(UInt8)temprature100 address:(UInt16)address retryCount:(NSInteger)retryCount responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseLightCTLTemperatureStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// Get temprature
+ (BOOL)getTempratureWithAddress:(UInt16)address retryCount:(NSInteger)retryCount responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseLightCTLTemperatureStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// Change HSL
+ (BOOL)changeHSLWithAddress:(UInt16)address hue100:(UInt8)hue100 saturation100:(UInt8)saturation100 brightness100:(UInt8)brightness100 responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
+ (BOOL)getHSLWithAddress:(UInt16)address responseMaxCount:(int)responseMaxCount successCallback:(responseLightHSLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// Change level
+ (BOOL)changeLevelWithAddress:(UInt16)address level:(SInt16)level responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// Get level
+ (BOOL)getLevelWithAddress:(UInt16)address responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericLevelStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// Kick out
+ (BOOL)kickoutDevice:(UInt16)address retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseConfigNodeResetStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// Set nodes's time to app's time
+ (BOOL)setNowTimeWithAddress:(UInt16)address responseMaxCount:(int)responseMaxCount successCallback:(responseTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// status NowTime to node, without response
+ (BOOL)statusNowTimeWithAddress:(UInt16)address successCallback:(responseTimeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// Save scene
+ (BOOL)saveSceneWithAddress:(UInt16)address sceneId:(UInt16)sceneId responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// Recall scene
+ (BOOL)recallSceneWithAddress:(UInt16)address sceneId:(UInt16)sceneId responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseSceneStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// Delete scene
+ (BOOL)delSceneWithAddress:(UInt16)address sceneId:(UInt16)sceneId responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// Get scene register status
+ (BOOL)getSceneRegisterStatusWithAddress:(UInt16)address responseMaxCount:(int)responseMaxCount successCallback:(responseSceneRegisterStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// Get scheduler status
+ (BOOL)getSchedulerStatusWithAddress:(UInt16)address responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseSchedulerStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// Set scheduler action
+ (BOOL)setSchedulerActionWithAddress:(UInt16)address schedulerModel:(SchedulerModel *)schedulerModel responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseSchedulerActionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// get scheduler action
+ (BOOL)getSchedulerActionWithAddress:(UInt16)address schedulerModelID:(UInt8)schedulerModelID responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseSchedulerActionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

@end
