/********************************************************************************************************
 * @file     DemoCommand.m 
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
//  DemoCommand.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "DemoCommand.h"

@implementation DemoCommand

/// Is get online status from private uuid, YES main callback onoffCallback、brighrnessCallback、tempratureCallback、HSLCallback、levelCallback from OnlineStatusCharacteristic.
+ (BOOL)isPrivatelyGetOnlineStatus {
    NSNumber *online = [[NSUserDefaults standardUserDefaults] valueForKey:kGetOnlineStatusType];
    BOOL hasOTACharacteristic = Bluetooth.share.store.OnlineStatusCharacteristic != nil;
    return online.boolValue && hasOTACharacteristic;
}

/// New API on demo since V2.8.1: Get Online device, publish use access_cmd_onoff_get(), private use OnlineStatusCharacteristic
+ (BOOL)getOnlineStatus:(BOOL)executeCommand reqCount:(int)reqCount complete:(responseModelCallBack)complete{
    /*Attention: when demo open funcation getOnlinestatus on Mesh Info ViewController, and current node has OnlineStatusCharacteristic, that API will get online status privately; if not, that API will get online status publish. */
    if ([self isPrivatelyGetOnlineStatus]) {
        [Bluetooth.share.commandHandle getOnlineStatusFromUUIDWithCompletation:complete];
        NSString *logStr = @"send onlinestatus from uuid";
        TeLog(@"%@", logStr);
        return YES;
    }
    
    if (!executeCommand) {
        return NO;
    }
    BOOL result = [Bluetooth.share.commandHandle getOnlineStatusWithExecuteCommand:executeCommand reqCount:reqCount Completation:complete];
    if (result) {
        NSString *logStr = @"send request for onlinestatus";
        TeLog(@"%@", logStr);
    }else{
        NSString *logStr = @"send request for onlinestatus, but busy now";
        TeLog(@"%@", logStr);
    }
    return result;
}

/// Get Lum
+ (BOOL)getLum:(BOOL)executeCommand nodeAddress:(UInt16)nodeAddress reqCount:(int)reqCount complete:(responseModelCallBack)complete{
    if (!executeCommand) {
        return NO;
    }
    BOOL result = [Bluetooth.share.commandHandle getLumWithExecuteCommand:executeCommand nodeAddress:nodeAddress reqCount:reqCount Completation:complete];
    if (result) {
        NSString *logStr = @"send request for onlinestatus";
        TeLog(@"%@", logStr);
    }else{
        NSString *logStr = @"send request for onlinestatus, but busy now";
        TeLog(@"%@", logStr);
    }
    return result;
}

/// Get CTL, (Lum and Temprature)
+ (BOOL)getCTL:(BOOL)executeCommand nodeAddress:(UInt16)nodeAddress reqCount:(int)reqCount complete:(responseModelCallBack)complete{
    if (!executeCommand) {
        return NO;
    }
    BOOL result = [Bluetooth.share.commandHandle getCTLWithExecuteCommand:executeCommand nodeAddress:nodeAddress reqCount:reqCount Completation:complete];
    if (result) {
        NSString *logStr = @"send request for onlinestatus";
        TeLog(@"%@", logStr);
    }else{
        NSString *logStr = @"send request for onlinestatus, but busy now";
        TeLog(@"%@", logStr);
    }
    return result;
}

///change subscribe list（develop can see mesh_node.h line129 to get more detail of option）
+ (void)editSubscribeList:(BOOL)executeCommand groAdd:(UInt16)groAdd nodeAddress:(UInt16)nodeAddress eleAddress:(UInt16)eleAddress add:(BOOL)add option:(UInt32)option complete:(responseModelCallBack)complete{
    [Bluetooth.share.commandHandle editSubscribeListWithExecuteCommand:executeCommand groAdd:groAdd nodeAddress:nodeAddress eleAddress:eleAddress add:add option:option Completation:complete];
    if (!executeCommand) {
        return;
    }
    NSString *logStr = [NSString stringWithFormat:@"%@ node_adr:0x%X ele_adr:0x%X gradd:0x%X option:0x%X",(add?@"Add":@"Del"),nodeAddress,eleAddress,groAdd,(unsigned int)option];
    TeLog(@"%@", logStr);
}

/// Change publish list
+ (void)editPublishList:(BOOL)executeCommand publishAddress:(UInt16)pub_adr nodeAddress:(UInt16)nodeAddress eleAddress:(UInt16)eleAddress option:(UInt32)option period:(mesh_pub_period_t)period complete:(responseModelCallBack)complete{
    [Bluetooth.share.commandHandle editPublishListWithExecuteCommand:executeCommand publishAddress:pub_adr nodeAddress:nodeAddress eleAddress:eleAddress option:option period:period Completation:complete];
    if (!executeCommand) {
        return;
    }
    NSString *logStr = [NSString stringWithFormat:@"%@ node_adr:0x%X ele_adr:0x%X pub_adr:0x%X option:0x%X",(@"edit"),nodeAddress,eleAddress,pub_adr,(unsigned int)option];
    TeLog(@"%@", logStr);
}

/// Get publish address
+ (void)getPublishAddress:(BOOL)executeCommand nodeAddress:(UInt16)nodeAddress eleAddress:(UInt16)eleAddress option:(UInt32)option complete:(responseModelCallBack)complete{
    [Bluetooth.share.commandHandle getPublishAddressWithExecuteCommand:executeCommand nodeAddress:nodeAddress eleAddress:eleAddress option:option Completation:complete];
    if (!executeCommand) {
        return;
    }
    NSString *logStr = [NSString stringWithFormat:@"%@ node_adr:0x%X ele_adr:0x%X option:0x%X",(@"get publish"),nodeAddress,eleAddress,(unsigned int)option];
    TeLog(@"%@", logStr);
}

/// Turn on or turn off the lights part
+ (void)switchOnOff:(BOOL)executeCommand on:(BOOL)on address:(UInt16)address resMax:(int)resMax complete:(responseModelCallBack)complete{
    BOOL ack = ![self isPrivatelyGetOnlineStatus];
    if (ack) {
        ack = complete != nil;
    }

    [Bluetooth.share.commandHandle switchOnOffWithExecuteCommand:executeCommand on:on address:address resMax:resMax ack:ack Completation:complete];
    if (!executeCommand) {
        return;
    }
    NSString *logStr = [NSString stringWithFormat:@"%@0x%02X device",(on?@"Turn on ":@"Turn off "),address];
    TeLog(@"%@", logStr);
}

/// Change brightness
+ (void)changeBrightness:(BOOL)executeCommand address:(UInt16)address para:(float)para isGet:(BOOL)isGet respondMax:(int)respondMax complete:(responseModelCallBack)complete{
    BOOL ack = ![self isPrivatelyGetOnlineStatus];
    if (ack) {
        ack = complete != nil;
    }
    
    [Bluetooth.share.commandHandle changeBrightnessWithExecuteCommand:executeCommand address:address para:para isGet:isGet respondMax:respondMax ack:ack Completation:complete];
    if (!executeCommand) {
        return;
    }
    NSString *logStr = [NSString stringWithFormat:@"change device :%02x Brightness %f",address,para];
    TeLog(@"%@", logStr);
}

/// Change temprature
+ (void)changeTemprature:(BOOL)executeCommand address:(UInt16)address para:(float)para isGet:(BOOL)isGet respondMax:(int)respondMax complete:(responseModelCallBack)complete{
    BOOL ack = ![self isPrivatelyGetOnlineStatus];
    if (ack) {
        ack = complete != nil;
    }

    [Bluetooth.share.commandHandle changeTempratureWithExecuteCommand:executeCommand address:address para:para isGet:isGet respondMax:respondMax ack:ack Completation:complete];
    if (!executeCommand) {
        return;
    }
    NSString *logStr = [NSString stringWithFormat:@"change device :%02x temprature %f",address,para];
    TeLog(@"%@", logStr);
}

/// Change HSL
+ (void)changeHSL:(BOOL)executeCommand address:(UInt16)address hue:(UInt8)hue saturation:(UInt8)saturation brightness:(UInt8)brightness isGet:(BOOL)isGet respondMax:(int)respondMax complete:(responseModelCallBack)complete {
    BOOL ack = ![self isPrivatelyGetOnlineStatus];
    if (ack) {
        ack = complete != nil;
    }

    [Bluetooth.share.commandHandle changeHSLWithExecuteCommand:executeCommand address:address hue:hue saturation:saturation brightness:brightness isGet:isGet respondMax:respondMax ack:ack Completation:complete];
    if (!executeCommand) {
        return;
    }
    NSString *logStr = [NSString stringWithFormat:@"change device :%02x HSL h:%d s:%d l:%d",address,hue,saturation,brightness];
    TeLog(@"%@", logStr);
}

/// Change level
+ (void)changeLevel:(BOOL)executeCommand address:(UInt16)address level:(s16)level isGet:(BOOL)isGet respondMax:(int)respondMax complete:(responseModelCallBack)complete{
    BOOL ack = ![self isPrivatelyGetOnlineStatus];
    if (ack) {
        ack = complete != nil;
    }

    [Bluetooth.share.commandHandle changeLevelWithExecuteCommand:executeCommand address:address level:level isGet:isGet respondMax:respondMax ack:ack Completation:complete];
    if (!executeCommand) {
        return;
    }
    NSString *logStr = [NSString stringWithFormat:@"change device :%02x level level:%d ",address,level];
    TeLog(@"%@", logStr);
}

/// Kick out
+ (void)kickoutDevice:(UInt16)address complete:(resultCallBack)complete{
    [Bluetooth.share.commandHandle kickoutDevice:address complete:complete];
    NSString *logStr = [NSString stringWithFormat:@"kickout Device :%02x",address];
    TeLog(@"%@", logStr);
}

/// Set all nodes's time to app's time
+ (void)setNowTimeWithComplete:(resultCallBack)complete{
    [Bluetooth.share.commandHandle setNowTimeWithComplete:complete];
    NSString *logStr = @"set now time";
    TeLog(@"%@", logStr);
}

/// Save scene
+ (void)saveSceneWithAddress:(u16)address resMax:(u32)resMax sceneId:(u16)sceneId complete:(responseModelCallBack)complete{
    [Bluetooth.share.commandHandle saveSceneWithAddress:address resMax:resMax sceneId:sceneId Completation:complete];
    NSString *logStr = [NSString stringWithFormat:@"save scene:%d address:%02x",sceneId,address];
    TeLog(@"%@", logStr);
}

/// Recall scene
+ (void)recallSceneWithAddress:(u16)address resMax:(u32)resMax sceneId:(u16)sceneId complete:(responseModelCallBack)complete{
    [Bluetooth.share.commandHandle recallSceneWithAddress:address resMax:resMax sceneId:sceneId Completation:complete];
    NSString *logStr = [NSString stringWithFormat:@"recall scene:%d address:%02x",sceneId,address];
    TeLog(@"%@", logStr);
}

/// Delete scene
+ (void)delSceneWithAddress:(u16)address resMax:(u32)resMax sceneId:(u16)sceneId complete:(responseModelCallBack)complete{
    [Bluetooth.share.commandHandle delSceneWithAddress:address resMax:resMax sceneId:sceneId Completation:complete];
    NSString *logStr = [NSString stringWithFormat:@"del scene:%d address:%02x",sceneId,address];
    TeLog(@"%@", logStr);
}

/// Get scene register status
+ (void)getSceneRegisterStatusWithAddress:(u16)address resMax:(u32)resMax Completation:(responseModelCallBack)complete{
    [Bluetooth.share.commandHandle getSceneRegisterStatusWithAddress:address resMax:resMax Completation:complete];
    NSString *logStr = [NSString stringWithFormat:@"get scene register status address:%02x",address];
    TeLog(@"%@", logStr);
}

/// Set scheduler action
+ (void)setSchedulerActionWithAddress:(u16)address resMax:(u32)resMax schedulerModel:(SchedulerModel *)schedulerModel complete:(responseModelCallBack)complete{
    [Bluetooth.share.commandHandle setSchedulerActionWithAddress:address resMax:resMax schedulerModel:schedulerModel Completation:complete];
    NSString *logStr = [NSString stringWithFormat:@"set scheduler:%d action:%lu sceneId:%llu address:%02x",schedulerModel.schedulerID,(unsigned long)schedulerModel.action,schedulerModel.sceneId,address];
    TeLog(@"%@", logStr);
}

/// Get scheduler status
+ (void)getSchedulerStatusWithAddress:(u16)address resMax:(u32)resMax Completation:(responseModelCallBack)complete{
    [Bluetooth.share.commandHandle getSchedulerStatusWithAddress:address resMax:resMax Completation:complete];
    NSString *logStr = [NSString stringWithFormat:@"get scheduler status address:%02x",address];
    TeLog(@"%@", logStr);
}

/// get scheduler action
+ (void)getSchedulerActionWithAddress:(u16)address resMax:(u32)resMax schedulerModelID:(u8)schedulerModelID Completation:(responseModelCallBack)complete{
    [Bluetooth.share.commandHandle getSchedulerActionWithAddress:address resMax:resMax schedulerModelID:schedulerModelID Completation:complete];
    NSString *logStr = [NSString stringWithFormat:@"get schedulerID:%hhu address:%02x",schedulerModelID,address];
    TeLog(@"%@", logStr);
}

/// get fw info
+ (void)getFwInfoWithAddress:(u16)address Completation:(responseModelCallBack)complete{
    [Bluetooth.share.commandHandle getFwInfoWithAddress:address Completation:complete];
    NSString *logStr = [NSString stringWithFormat:@"get fw info address:%02x",address];
    TeLog(@"%@", logStr);
}
@end
