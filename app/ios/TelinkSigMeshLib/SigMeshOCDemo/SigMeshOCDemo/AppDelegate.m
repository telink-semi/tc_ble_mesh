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
    [[UITabBarItem appearance] setTitleTextAttributes:@{NSForegroundColorAttributeName:UIColor.telinkButtonBlue} forState:UIControlStateSelected];

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
//    [SigLogger.share setSDKLogLevel:SigLogLevelDebug];
    [SigLogger.share setSDKLogLevel:SigLogLevelAll];
//    [SigLogger.share setSDKLogLevel:SigLogLevelOff];//上架则关闭Log保存功能

    /*初始化SDK*/
    //1.一个provisioner分配的地址范围，默认为0x400。
    SigDataSource.share.defaultAllocatedUnicastRangeHighAddress = kAllocatedUnicastRangeHighAddress;
    //2.mesh网络的sequenceNumber步长，默认为128。
//    SigDataSource.share.defaultSequenceNumberIncrement = kSequenceNumberIncrement;
    SigDataSource.share.defaultSequenceNumberIncrement = 16;
    //3.启动SDK。
    [SDKLibCommand startMeshSDK];
        
//    //(可选，旧判断)SDK的分组默认绑定5个modelID，可通过以下接口修改分组默认绑定的modelIDs
//    SigDataSource.share.defaultGroupSubscriptionModels = [NSMutableArray arrayWithArray:@[@(kSigModel_GenericOnOffServer_ID),@(kSigModel_LightLightnessServer_ID),@(kSigModel_LightCTLServer_ID),@(kSigModel_LightCTLTemperatureServer_ID),@(kSigModel_LightHSLServer_ID),@(kSigModel_GenericLevelServer_ID)]];
////    [SigDataSource.share.defaultGroupSubscriptionModels addObject:@(0x00000211)];//新增vendorModelID用于测试加组及vendor组控。
    /// (可选，新判断)demo v4.1.0.0及之后添加的标志，标志订阅组地址时是否把Service Level ModelID（0x1003）也订阅了。
    /// 客户存在Mesh遥控器产品且Mesh遥控器产品存在Level控制功能，则需要在app端使能该功能。默认值为关闭。
    /// The flag added in demo v4.1.0.0 and later indicates whether the Service Level ModelID (0x1003) is also subscribed when subscribing to the group address.
    /// If the customer has a Mesh remote control product and the Mesh remote control product has a Level control function, this function needs to be enabled on the app side. The default value is off.
    NSNumber *levelEnable = [[NSUserDefaults standardUserDefaults] valueForKey:kSubscribeLevelServiceModelID];
    if (levelEnable == nil) {
        levelEnable = [NSNumber numberWithBool:NO];
        [[NSUserDefaults standardUserDefaults] setValue:levelEnable forKey:kSubscribeLevelServiceModelID];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
    if (levelEnable.boolValue) {
        if (![SigDataSource.share.defaultGroupSubscriptionModels containsObject:@(kSigModel_GenericLevelServer_ID)]) {
            [SigDataSource.share.defaultGroupSubscriptionModels addObject:@(kSigModel_GenericLevelServer_ID)];
        }
    } else {
        if ([SigDataSource.share.defaultGroupSubscriptionModels containsObject:@(kSigModel_GenericLevelServer_ID)]) {
            [SigDataSource.share.defaultGroupSubscriptionModels removeObject:@(kSigModel_GenericLevelServer_ID)];
        }
    }
    
    //(可选)SDK默认实现了PID为1、2、7、0x0201的设备的fast bind功能，其它类型的设备可通过以下接口添加该类型设备默认的nodeInfo以实现fast bind功能
//    //示范代码：添加PID=8，composition data=TemByte的数据到SigDataSource.share.defaultNodeInfos。
//    DeviceTypeModel *model = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:8];
//    static Byte TemByte[] = {(Byte) 0x11, (Byte) 0x02, (Byte) 0x01, (Byte) 0x00, (Byte) 0x32, (Byte) 0x37, (Byte) 0x69, (Byte) 0x00, (Byte) 0x07, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x19, (Byte) 0x01, (Byte) 0x00, (Byte) 0x00, (Byte) 0x02, (Byte) 0x00, (Byte) 0x03, (Byte) 0x00, (Byte) 0x04, (Byte) 0x00, (Byte) 0x05, (Byte) 0x00, (Byte) 0x00, (Byte) 0xfe, (Byte) 0x01, (Byte) 0xfe, (Byte) 0x02, (Byte) 0xfe, (Byte) 0x00, (Byte) 0xff, (Byte) 0x01, (Byte) 0xff, (Byte) 0x00, (Byte) 0x12, (Byte) 0x01, (Byte) 0x12, (Byte) 0x00, (Byte) 0x10, (Byte) 0x02, (Byte) 0x10, (Byte) 0x04, (Byte) 0x10, (Byte) 0x06, (Byte) 0x10, (Byte) 0x07, (Byte) 0x10, (Byte) 0x03, (Byte) 0x12, (Byte) 0x04, (Byte) 0x12, (Byte) 0x06, (Byte) 0x12, (Byte) 0x07, (Byte) 0x12, (Byte) 0x00, (Byte) 0x13, (Byte) 0x01, (Byte) 0x13, (Byte) 0x03, (Byte) 0x13, (Byte) 0x04, (Byte) 0x13, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x02, (Byte) 0x00, (Byte) 0x02, (Byte) 0x10, (Byte) 0x06, (Byte) 0x13};
//    NSData *nodeInfoData = [NSData dataWithBytes:TemByte length:76];
//    [model setCompositionData:nodeInfoData];
//    [SigDataSource.share.defaultNodeInfos addObject:model];

    //v3.3.3.6版本开始，新增8278、8269、9518的cpsData.
    [self addMoreCompositionData];
    
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
    //(可选)v3.3.3.6及之后新增配置项
//    SigDataSource.share.aggregatorEnable = YES;
    
    /// (可选) demo v3.3.3.6及之后新增配置项，标记APP端发送数据是否使用The directed security material进行加密，默认值是NO。
    /// (Optional) A new configuration item is added in demo v3.3.3.6 and later to mark whether the data sent by the APP is encrypted using The directed security material. The default value is NO.
    NSNumber *directedSecurityEnable = [[NSUserDefaults standardUserDefaults] valueForKey:kDirectedSecurityEnable];
    if (directedSecurityEnable == nil) {
        directedSecurityEnable = [NSNumber numberWithBool:NO];
        [[NSUserDefaults standardUserDefaults] setValue:directedSecurityEnable forKey:kDirectedSecurityEnable];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
    SigDataSource.share.sendByDirectedSecurity = directedSecurityEnable.boolValue;
    
    //(可选)v3.3.3.6及之后新增Demo配置项，用于传递bin到APP里面。
    [self createTelinkBinFolder];

//    //(可选)v4.1.0.0及之后新增Demo配置项，用于客户自定义proxy filter。
//    NSData *filterData = [[NSUserDefaults standardUserDefaults] valueForKey:kFilter];
//    if (filterData == nil) {
//        SigProxyFilterModel *model = [[SigProxyFilterModel alloc] init];
//        NSDictionary *dict = [model getDictionaryOfSigProxyFilterModel];
//        filterData = [LibTools getJSONDataWithDictionary:dict];
//        [[NSUserDefaults standardUserDefaults] setValue:filterData forKey:kFilter];
//        [[NSUserDefaults standardUserDefaults] synchronize];
//    }
//    SigProxyFilterModel *filter = [[SigProxyFilterModel alloc] init];
//    NSDictionary *dict = [LibTools getDictionaryWithJSONData:filterData];
//    [filter setDictionaryToSigProxyFilterModel:dict];
//    SigDataSource.share.filterModel = filter;
    
    // demo v4.1.0.0新增主动添加还是手动添加的配置项，
    NSNumber *autoProvision = [[NSUserDefaults standardUserDefaults] valueForKey:kAutoProvision];
    if (autoProvision == nil) {
        // demo默认使用手动添加模式。
        autoProvision = [NSNumber numberWithBool:NO];
        [[NSUserDefaults standardUserDefaults] setValue:autoProvision forKey:kAutoProvision];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }

    return YES;
}

- (void)addMoreCompositionData {
    struct TelinkPID telinkPid_8258_CT = {};
    struct TelinkPID telinkPid_8258_HSL = {};
    struct TelinkPID telinkPid_8258_Panel = {};
    struct TelinkPID telinkPid_8258_LPN = {};
    struct TelinkPID telinkPid_8278_CT = {};
    struct TelinkPID telinkPid_8278_HSL = {};
    struct TelinkPID telinkPid_8278_Panel = {};
    struct TelinkPID telinkPid_8278_LPN = {};
    struct TelinkPID telinkPid_8269_CT = {};
    struct TelinkPID telinkPid_8269_HSL = {};
    struct TelinkPID telinkPid_8269_Panel = {};
    struct TelinkPID telinkPid_8269_LPN = {};
    struct TelinkPID telinkPid_9518_CT = {};
    struct TelinkPID telinkPid_9518_HSL = {};
    struct TelinkPID telinkPid_9518_Panel = {};
    struct TelinkPID telinkPid_9518_LPN = {};

    telinkPid_8258_CT.MCUChipType = telinkPid_8258_HSL.MCUChipType = telinkPid_8258_Panel.MCUChipType = telinkPid_8258_LPN.MCUChipType = CHIP_TYPE_8258;
    telinkPid_8278_CT.MCUChipType = telinkPid_8278_HSL.MCUChipType = telinkPid_8278_Panel.MCUChipType = telinkPid_8278_LPN.MCUChipType = CHIP_TYPE_8278;
    telinkPid_8269_CT.MCUChipType = telinkPid_8269_HSL.MCUChipType = telinkPid_8269_Panel.MCUChipType = telinkPid_8269_LPN.MCUChipType = CHIP_TYPE_8269;
    telinkPid_9518_CT.MCUChipType = telinkPid_9518_HSL.MCUChipType = telinkPid_9518_Panel.MCUChipType = telinkPid_9518_LPN.MCUChipType = CHIP_TYPE_9518;

    telinkPid_8258_CT.majorProductType = telinkPid_8258_HSL.majorProductType = telinkPid_8258_Panel.majorProductType = telinkPid_8278_CT.majorProductType = telinkPid_8278_HSL.majorProductType = telinkPid_8278_Panel.majorProductType = telinkPid_8269_CT.majorProductType = telinkPid_8269_HSL.majorProductType = telinkPid_8269_Panel.majorProductType = telinkPid_9518_CT.majorProductType = telinkPid_9518_HSL.majorProductType = telinkPid_9518_Panel.majorProductType = MajorProductType_light;
    telinkPid_8258_LPN.majorProductType = telinkPid_8278_LPN.majorProductType = telinkPid_8269_LPN.majorProductType = telinkPid_9518_LPN.majorProductType = MajorProductType_LPN;
    telinkPid_8258_CT.minorProductType = telinkPid_8278_CT.minorProductType = telinkPid_8269_CT.minorProductType = telinkPid_9518_CT.minorProductType = SigNodePID_CT;
    telinkPid_8258_HSL.minorProductType = telinkPid_8278_HSL.minorProductType = telinkPid_8269_HSL.minorProductType = telinkPid_9518_HSL.minorProductType = SigNodePID_HSL;
    telinkPid_8258_Panel.minorProductType = telinkPid_8278_Panel.minorProductType = telinkPid_8269_Panel.minorProductType = telinkPid_9518_Panel.minorProductType = SigNodePID_Panel;
    telinkPid_8258_LPN.minorProductType = telinkPid_8278_LPN.minorProductType = telinkPid_8269_LPN.minorProductType = telinkPid_9518_LPN.minorProductType = 1;

    DeviceTypeModel *deviceType_CT = [SigDataSource.share getNodeInfoWithCID:kCompanyID PID:SigNodePID_CT];
    DeviceTypeModel *model_8278_CT = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:telinkPid_8278_CT.value compositionData:nil];
    DeviceTypeModel *model_8269_CT = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:telinkPid_8269_CT.value compositionData:nil];
    DeviceTypeModel *model_9518_CT = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:telinkPid_9518_CT.value compositionData:nil];
    model_8278_CT.defaultCompositionData = model_8269_CT.defaultCompositionData = model_9518_CT.defaultCompositionData = deviceType_CT.defaultCompositionData;
    DeviceTypeModel *deviceType_HSL = [SigDataSource.share getNodeInfoWithCID:kCompanyID PID:SigNodePID_HSL];
    DeviceTypeModel *model_8278_HSL = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:telinkPid_8278_HSL.value compositionData:nil];
    DeviceTypeModel *model_8269_HSL = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:telinkPid_8269_HSL.value compositionData:nil];
    DeviceTypeModel *model_9518_HSL = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:telinkPid_9518_HSL.value compositionData:nil];
    model_8278_HSL.defaultCompositionData = model_8269_HSL.defaultCompositionData = model_9518_HSL.defaultCompositionData = deviceType_HSL.defaultCompositionData;
    DeviceTypeModel *deviceType_Panel = [SigDataSource.share getNodeInfoWithCID:kCompanyID PID:SigNodePID_Panel];
    DeviceTypeModel *model_8278_Panel = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:telinkPid_8278_Panel.value compositionData:nil];
    DeviceTypeModel *model_8269_Panel = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:telinkPid_8269_Panel.value compositionData:nil];
    DeviceTypeModel *model_9518_Panel = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:telinkPid_9518_Panel.value compositionData:nil];
    model_8278_Panel.defaultCompositionData = model_8269_Panel.defaultCompositionData = model_9518_Panel.defaultCompositionData = deviceType_Panel.defaultCompositionData;
    DeviceTypeModel *deviceType_LPN = [SigDataSource.share getNodeInfoWithCID:kCompanyID PID:SigNodePID_LPN];
    DeviceTypeModel *model_8278_LPN = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:telinkPid_8278_LPN.value compositionData:nil];
    DeviceTypeModel *model_8269_LPN = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:telinkPid_8269_LPN.value compositionData:nil];
    DeviceTypeModel *model_9518_LPN = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:telinkPid_9518_LPN.value compositionData:nil];
    model_8278_LPN.defaultCompositionData = model_8269_LPN.defaultCompositionData = model_9518_LPN.defaultCompositionData = deviceType_LPN.defaultCompositionData;
    [SigDataSource.share.defaultNodeInfos addObjectsFromArray:@[model_8278_CT, model_8269_CT, model_9518_CT, model_8278_HSL, model_8269_HSL, model_9518_HSL, model_8278_Panel, model_8269_Panel, model_9518_Panel, model_8278_LPN, model_8269_LPN, model_9518_LPN]];
}

/// 需要在Document文件夹里面创建了文件夹或者文件，在手机系统的`文件`APP里面才会显示出当前APP的文件夹。才可以通过`存储到“文件”`功能将bin文件导入当前APP。
- (void)createTelinkBinFolder {
    //获取document的路径
    NSString * documentPath =  NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).lastObject;
   //创建文件管理器
   NSFileManager *fileManager =  [NSFileManager defaultManager];
    NSString *binPath = [documentPath stringByAppendingPathComponent:@"data"];
    if (![fileManager fileExistsAtPath:binPath]) {
        [fileManager createFileAtPath:binPath contents:nil attributes:nil];
    }
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
