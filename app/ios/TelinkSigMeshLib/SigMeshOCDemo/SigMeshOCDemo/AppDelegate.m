/********************************************************************************************************
 * @file     AppDelegate.m 
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
//  AppDelegate.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "AppDelegate.h"
#import "SDK/IQKeyboardManager-master/IQKeyboardManager/IQKeyboardManager.h"
#import "ColorManager.h"
#import "UIButton+extension.h"

@interface AppDelegate ()

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [IQKeyboardManager sharedManager].shouldResignOnTouchOutside = YES;
    [[UINavigationBar appearance] setTitleTextAttributes:@{NSForegroundColorAttributeName:[UIColor whiteColor]}];
    [[UINavigationBar appearance] setTintColor:[UIColor whiteColor]];
    [[UITabBarItem appearance] setTitleTextAttributes:@{NSForegroundColorAttributeName:kDefultColor} forState:UIControlStateSelected];
    
    //demo v2.8.0新增快速添加模式，demo默认使用普通添加模式。
    NSNumber *type = [[NSUserDefaults standardUserDefaults] valueForKey:kKeyBindType];
    if (type == nil) {
        type = [NSNumber numberWithInteger:KeyBindTpye_Normal];
        [[NSUserDefaults standardUserDefaults] setValue:type forKey:kKeyBindType];
    }

    //demo v2.8.0新增remote添加模式，demo默认使用普通添加模式。
    NSNumber *remoteType = [[NSUserDefaults standardUserDefaults] valueForKey:kRemoteAddType];
    if (remoteType == nil) {
        remoteType = [NSNumber numberWithBool:NO];
        [[NSUserDefaults standardUserDefaults] setValue:remoteType forKey:kRemoteAddType];
    }

    //demo v2.8.1新增私有定制getOnlinestatus，demo默认使用私有定制获取状态。
    NSNumber *onlineType = [[NSUserDefaults standardUserDefaults] valueForKey:kGetOnlineStatusType];
    if (onlineType == nil) {
        onlineType = [NSNumber numberWithBool:YES];
        [[NSUserDefaults standardUserDefaults] setValue:onlineType forKey:kGetOnlineStatusType];
    }

    //demo v3.0.0新增fast provision添加模式，demo默认使用普通添加模式。
    NSNumber *fastAddType = [[NSUserDefaults standardUserDefaults] valueForKey:kFastAddType];
    if (fastAddType == nil) {
        fastAddType = [NSNumber numberWithBool:NO];
        [[NSUserDefaults standardUserDefaults] setValue:fastAddType forKey:kFastAddType];
    }

    //demo v3.2.2新增staticOOB设备添加的兼容模式，demo默认使用不兼容模式（即staticOOB设备必须通过staticOOB provision的方式进行添加）。
    NSNumber *addStaticOOBDevcieByNoOOBEnable = [[NSUserDefaults standardUserDefaults] valueForKey:kAddStaticOOBDevcieByNoOOBEnable];
    if (addStaticOOBDevcieByNoOOBEnable == nil) {
        addStaticOOBDevcieByNoOOBEnable = [NSNumber numberWithBool:NO];
        [[NSUserDefaults standardUserDefaults] setValue:fastAddType forKey:kAddStaticOOBDevcieByNoOOBEnable];
    }
    SigDataSource.share.addStaticOOBDevcieByNoOOBEnable = addStaticOOBDevcieByNoOOBEnable.boolValue;

    //demo中setting界面显示的log信息，客户开发到后期，APP稳定后可以不集成该功能，且上架最好关闭log保存功能。(客户发送iTunes中的日志文件“TelinkSDKDebugLogData”给泰凌微即可)
    [SigLogger.share setSDKLogLevel:SigLogLevelDebug];
//    [SigLogger.share setSDKLogLevel:SigLogLevelAll];

    //初始化SDK
    [SDKLibCommand startMeshSDK];
        
    //(可选)SDK的分组默认绑定5个modelID，可通过以下接口修改分组默认绑定的modelIDs
    SigDataSource.share.defaultGroupSubscriptionModels = [NSMutableArray arrayWithArray:@[@(SIG_MD_G_ONOFF_S),@(SIG_MD_LIGHTNESS_S),@(SIG_MD_LIGHT_CTL_S),@(SIG_MD_LIGHT_CTL_TEMP_S),@(SIG_MD_LIGHT_HSL_S)]];
    
    //(可选)SDK默认实现了PID为1和7的设备的fast bind功能，其它类型的设备可通过以下接口添加该类型设备默认的nodeInfo以实现fast bind功能
//    DeviceTypeModel *model = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:8];
//    NSData *nodeInfoData = [NSData dataWithBytes:TemByte length:76];
//    [model setDefultNodeInfoData:nodeInfoData];
//    [SigDataSource.share.defaultNodeInfos addObject:model];

    SigMeshLib.share.transmissionTimerInteral = 0.600;
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and invalidate graphics rendering callbacks. Games should use this method to pause the game.
}


- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}


- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the active state; here you can undo many of the changes made on entering the background.
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}


- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}


@end
