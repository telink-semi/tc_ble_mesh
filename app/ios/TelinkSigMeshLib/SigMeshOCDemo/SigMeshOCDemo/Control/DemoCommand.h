/********************************************************************************************************
 * @file     DemoCommand.h
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/7/31
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/

#import <Foundation/Foundation.h>

@interface DemoCommand : NSObject

/// Is get online status from private uuid, YES main callback onoffCallback、brighrnessCallback、temperatureCallback、HSLCallback、levelCallback from OnlineStatusCharacteristic.
+ (BOOL)isPrivatelyGetOnlineStatus;

/// Get Online device, publish use SigGenericOnOffGet, private use OnlineStatusCharacteristic
+ (BOOL)getOnlineStatusWithResponseMaxCount:(int)responseMaxCount successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// Get Lum
+ (BOOL)getLumWithNodeAddress:(UInt16)nodeAddress responseMacCount:(int)responseMacCount successCallback:(responseLightLightnessStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// Get CTL, (Lum and Temperature)
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

/// Change temperature
+ (BOOL)changeTemperatureWithTemperature100:(UInt8)temperature100 address:(UInt16)address retryCount:(NSInteger)retryCount responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseLightCTLTemperatureStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
/// Get temperature
+ (BOOL)getTemperatureWithAddress:(UInt16)address retryCount:(NSInteger)retryCount responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseLightCTLTemperatureStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

/// Change HSL
+ (BOOL)changeHSLWithAddress:(UInt16)address hue:(float)hue saturation:(float)saturation brightness:(float)brightness responseMaxCount:(int)responseMaxCount ack:(BOOL)ack successCallback:(responseLightHSLStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
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
