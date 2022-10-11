/********************************************************************************************************
 * @file     AppDelegate.m 
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/7/31
 *
 * @par     Copyright (c) [2021], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
        type = [NSNumber numberWithInteger:KeyBindType_Normal];
        [[NSUserDefaults standardUserDefaults] setValue:type forKey:kKeyBindType];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }

    //demo v2.8.0新增remote添加模式，demo默认使用普通添加模式。
    NSNumber *remoteType = [[NSUserDefaults standardUserDefaults] valueForKey:kRemoteAddType];
    if (remoteType == nil) {
        remoteType = [NSNumber numberWithBool:NO];
        [[NSUserDefaults standardUserDefaults] setValue:remoteType forKey:kRemoteAddType];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }

    //demo v2.8.1新增私有定制getOnlinestatus，demo默认使用私有定制获取状态。
    NSNumber *onlineType = [[NSUserDefaults standardUserDefaults] valueForKey:kGetOnlineStatusType];
    if (onlineType == nil) {
        onlineType = [NSNumber numberWithBool:YES];
        [[NSUserDefaults standardUserDefaults] setValue:onlineType forKey:kGetOnlineStatusType];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }

    //demo v3.0.0新增fast provision添加模式，demo默认使用普通添加模式。
    NSNumber *fastAddType = [[NSUserDefaults standardUserDefaults] valueForKey:kFastAddType];
    if (fastAddType == nil) {
        fastAddType = [NSNumber numberWithBool:NO];
        [[NSUserDefaults standardUserDefaults] setValue:fastAddType forKey:kFastAddType];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }

    //demo v3.2.2新增staticOOB设备添加的兼容模式，demo默认使用兼容模式。（兼容模式为staticOOB设备在无OOB数据的情况下通过noOOB provision的方式进行添加;不兼容模式为staticOOB设备必须通过staticOOB provision的方式进行添加）。
    NSNumber *addStaticOOBDeviceByNoOOBEnable = [[NSUserDefaults standardUserDefaults] valueForKey:kAddStaticOOBDeviceByNoOOBEnable];
    if (addStaticOOBDeviceByNoOOBEnable == nil) {
        addStaticOOBDeviceByNoOOBEnable = [NSNumber numberWithBool:YES];
        [[NSUserDefaults standardUserDefaults] setValue:addStaticOOBDeviceByNoOOBEnable forKey:kAddStaticOOBDeviceByNoOOBEnable];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
    SigDataSource.share.addStaticOOBDeviceByNoOOBEnable = addStaticOOBDeviceByNoOOBEnable.boolValue;
    
    //demo v3.3.3将原来的两种DLE模式修改3种Extend Bearer Mode。（客户定制功能）
    NSNumber *extendBearerMode = [[NSUserDefaults standardUserDefaults] valueForKey:kExtendBearerMode];
    if (extendBearerMode == nil) {
        extendBearerMode = [NSNumber numberWithInt:SigTelinkExtendBearerMode_noExtend];
        [[NSUserDefaults standardUserDefaults] setValue:extendBearerMode forKey:kExtendBearerMode];
        [[NSUserDefaults standardUserDefaults] synchronize];
    } else {
        UInt8 extend = [extendBearerMode intValue];
        SigDataSource.share.telinkExtendBearerMode = extend;
        if (extend == SigTelinkExtendBearerMode_noExtend) {
            //(默认)关闭DLE功能后，SDK的Access消息是长度大于15字节才进行segment分包。
            SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength = kUnsegmentedMessageLowerTransportPDUMaxLength;
        } else {
            //(可选)打开DLE功能后，SDK的Access消息是长度大于229字节才进行segment分包。
            SigDataSource.share.defaultUnsegmentedMessageLowerTransportPDUMaxLength = kDLEUnsegmentLength;
        }
    }

    //demo v3.3.6新增provision算法配置项，demo默认为SigFipsP256EllipticCurve_auto。
    SigDataSource.share.fipsP256EllipticCurve = SigFipsP256EllipticCurve_auto;

    //demo中setting界面显示的log信息，客户开发到后期，APP稳定后可以不集成该功能，且上架最好关闭log保存功能。(客户发送iTunes中的日志文件“TelinkSDKDebugLogData”给泰凌微即可)
    [SigLogger.share setSDKLogLevel:SigLogLevelDebug];
//    [SigLogger.share setSDKLogLevel:SigLogLevelAll];

    /*初始化SDK*/
    //1.一个provisioner分配的地址范围，默认为0x400。
    SigDataSource.share.defaultAllocatedUnicastRangeHighAddress = kAllocatedUnicastRangeHighAddress;
    //2.mesh网络的sequenceNumber步长，默认为128。
//    SigDataSource.share.defaultSequenceNumberIncrement = kSequenceNumberIncrement;
    SigDataSource.share.defaultSequenceNumberIncrement = 16;
    //3.启动SDK。
    [SDKLibCommand startMeshSDK];
        
    //(可选)SDK的分组默认绑定5个modelID，可通过以下接口修改分组默认绑定的modelIDs
    SigDataSource.share.defaultGroupSubscriptionModels = [NSMutableArray arrayWithArray:@[@(kSigModel_GenericOnOffServer_ID),@(kSigModel_LightLightnessServer_ID),@(kSigModel_LightCTLServer_ID),@(kSigModel_LightCTLTemperatureServer_ID),@(kSigModel_LightHSLServer_ID)]];
//    [SigDataSource.share.defaultGroupSubscriptionModels addObject:@(0x00000211)];//新增vendorModelID用于测试加组及vendor组控。
    
    //(可选)SDK默认实现了PID为1和7的设备的fast bind功能，其它类型的设备可通过以下接口添加该类型设备默认的nodeInfo以实现fast bind功能
//    //示范代码：添加PID=8，composition data=TemByte的数据到SigDataSource.share.defaultNodeInfos。
//    DeviceTypeModel *model = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:8];
//    static Byte TemByte[] = {(Byte) 0x11, (Byte) 0x02, (Byte) 0x01, (Byte) 0x00, (Byte) 0x32, (Byte) 0x37, (Byte) 0x69, (Byte) 0x00, (Byte) 0x07, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x19, (Byte) 0x01, (Byte) 0x00, (Byte) 0x00, (Byte) 0x02, (Byte) 0x00, (Byte) 0x03, (Byte) 0x00, (Byte) 0x04, (Byte) 0x00, (Byte) 0x05, (Byte) 0x00, (Byte) 0x00, (Byte) 0xfe, (Byte) 0x01, (Byte) 0xfe, (Byte) 0x02, (Byte) 0xfe, (Byte) 0x00, (Byte) 0xff, (Byte) 0x01, (Byte) 0xff, (Byte) 0x00, (Byte) 0x12, (Byte) 0x01, (Byte) 0x12, (Byte) 0x00, (Byte) 0x10, (Byte) 0x02, (Byte) 0x10, (Byte) 0x04, (Byte) 0x10, (Byte) 0x06, (Byte) 0x10, (Byte) 0x07, (Byte) 0x10, (Byte) 0x03, (Byte) 0x12, (Byte) 0x04, (Byte) 0x12, (Byte) 0x06, (Byte) 0x12, (Byte) 0x07, (Byte) 0x12, (Byte) 0x00, (Byte) 0x13, (Byte) 0x01, (Byte) 0x13, (Byte) 0x03, (Byte) 0x13, (Byte) 0x04, (Byte) 0x13, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x02, (Byte) 0x00, (Byte) 0x02, (Byte) 0x10, (Byte) 0x06, (Byte) 0x13};
//    NSData *nodeInfoData = [NSData dataWithBytes:TemByte length:76];
//    [model setCompositionData:nodeInfoData];
//    [SigDataSource.share.defaultNodeInfos addObject:model];

    //(可选)SDK默认publish周期为20秒，通过修改可以修改SDK的默认publish参数，或者客户自行实现publish检测机制。
//    SigPeriodModel *periodModel = [[SigPeriodModel alloc] init];
//    periodModel.numberOfSteps = kPublishIntervalOfDemo;
////    periodModel.numberOfSteps = 5;//整形，范围0x01~0x3F.
//    periodModel.resolution = [LibTools getSigStepResolutionInMillisecondsOfJson:SigStepResolution_seconds];
//    SigDataSource.share.defaultPublishPeriodModel = periodModel;

    
    SigMeshLib.share.transmissionTimerInterval = 0.600;
//    SigDataSource.share.needPublishTimeModel = NO;
    
    //(可选)v3.3.3新增配置项
//    SigDataSource.share.defaultReliableIntervalOfLPN = kSDKLibCommandTimeout;
//    SigDataSource.share.defaultReliableIntervalOfNotLPN = kSDKLibCommandTimeout * 2;
    //(可选)v3.3.3.6及之后新增配置项，基础库TelinkSigMeshLib.framework不支持，扩展库TelinkSigMeshLibExtensions.framework支持。
//    SigDataSource.share.aggregatorEnable = YES;
    
    //(可选)v3.3.3.6及之后新增配置项，APP端发送数据是否使用The directed security material进行加密，默认是NO，The managed flooding security material。
    NSNumber *directedSecurityEnable = [[NSUserDefaults standardUserDefaults] valueForKey:kDirectedSecurityEnable];
    if (directedSecurityEnable == nil) {
        directedSecurityEnable = [NSNumber numberWithBool:NO];
        [[NSUserDefaults standardUserDefaults] setValue:directedSecurityEnable forKey:kDirectedSecurityEnable];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
    SigDataSource.share.sendByDirectedSecurity = directedSecurityEnable.boolValue;
    
    return YES;
}

- (UIInterfaceOrientationMask)application:(UIApplication *)application supportedInterfaceOrientationsForWindow:(nullable UIWindow *)window {
    return UIInterfaceOrientationMaskPortrait;
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
