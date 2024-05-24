/********************************************************************************************************
 * @file     AppDelegate.m
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

#import "AppDelegate.h"
#import <IQKeyboardManager/IQKeyboardManager.h>
#import "ColorManager.h"

@interface AppDelegate ()

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    //1.Config App tint
    [self configAppTint];
    //2.Config App key bind type
    [self configKeyBindType];
    //3.Config App add static OOB device by No-OOB enable
    [self configAddStaticOOBDeviceByNoOOBEnable];
    //4.Config App extend bearer mode
    [self configAppExtendBearerMode];
    //5.Config App provision elliptic curve algorithm
    [self configAppProvisionEllipticCurveAlgorithm];
    //6.Config SDK log level
    [self configSDKLogLevel];
    //7.Config SigDataSource default parameter of SDK
    [self configSigDataSourceDefaultParameter];
    //8.(必须实现)Start Mesh SDK
    [SDKLibCommand startMeshSDK];
    //9.Import one mesh dictionary from app to SDK
    [self importOneMeshDictionaryFromAppToSDK];
    //10.Create Telink Bin Folder in iPhone
    [self createTelinkBinFolder];
    //11.Config Filter
    [self configFilter];
    //12.Config Provision Mode
    [self configProvisionMode];
    //13.Config Import Complete Action
    [self configImportCompleteAction];
    //14.Config Default Root Certificate Data
    [self configDefaultRootCertificateData];
    //15.Config sortType of nodeList
    [self configSortTypeOfNodeList];

    return YES;
}

/// 1.Config App tint
- (void)configAppTint {
    [IQKeyboardManager sharedManager].shouldResignOnTouchOutside = YES;
    [[UINavigationBar appearance] setTitleTextAttributes:@{NSForegroundColorAttributeName:[UIColor whiteColor]}];
    [[UINavigationBar appearance] setTintColor:[UIColor whiteColor]];
    [[UITabBarItem appearance] setTitleTextAttributes:@{NSForegroundColorAttributeName:UIColor.telinkButtonBlue} forState:UIControlStateSelected];
}

/// 2.Config App key bind type
- (void)configKeyBindType {
    //demo v2.8.0新增快速添加模式，demo默认使用普通添加模式。
    NSNumber *type = [[NSUserDefaults standardUserDefaults] valueForKey:kKeyBindType];
    if (type == nil) {
        type = [NSNumber numberWithInteger:KeyBindType_Normal];
        [[NSUserDefaults standardUserDefaults] setValue:type forKey:kKeyBindType];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
}

/// 3.Config App add static OOB device by No-OOB enable
- (void)configAddStaticOOBDeviceByNoOOBEnable {
    //demo v3.2.2新增staticOOB设备添加的兼容模式，demo默认使用兼容模式。（兼容模式为staticOOB设备在无OOB数据的情况下通过noOOB provision的方式进行添加;不兼容模式为staticOOB设备必须通过staticOOB provision的方式进行添加）。
    NSNumber *addStaticOOBDeviceByNoOOBEnable = [[NSUserDefaults standardUserDefaults] valueForKey:kAddStaticOOBDeviceByNoOOBEnable];
    if (addStaticOOBDeviceByNoOOBEnable == nil) {
        addStaticOOBDeviceByNoOOBEnable = [NSNumber numberWithBool:YES];
        [[NSUserDefaults standardUserDefaults] setValue:addStaticOOBDeviceByNoOOBEnable forKey:kAddStaticOOBDeviceByNoOOBEnable];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
    SigDataSource.share.addStaticOOBDeviceByNoOOBEnable = addStaticOOBDeviceByNoOOBEnable.boolValue;
}

/// 4.Config App extend bearer mode
- (void)configAppExtendBearerMode {
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
}

/// 5.Config App provision elliptic curve algorithm
- (void)configAppProvisionEllipticCurveAlgorithm {
    //demo v3.3.6新增provision算法配置项，demo默认为SigFipsP256EllipticCurve_auto。
    SigDataSource.share.fipsP256EllipticCurve = SigFipsP256EllipticCurve_auto;
}

/// 6.Config SDK log level
- (void)configSDKLogLevel {
    //demo中setting界面显示的log信息，客户开发到后期，APP稳定后可以不集成该功能，且上架最好关闭log保存功能。(客户发送iTunes中的日志文件“TelinkSDKDebugLogData”给泰凌微即可)
//    [SigLogger.share setSDKLogLevel:SigLogLevelDebug];
    [SigLogger.share setSDKLogLevel:SigLogLevelAll];
//    [SigLogger.share setSDKLogLevel:SigLogLevelOff];//上架则关闭Log保存功能
}

/// 7.Config SigDataSource default parameter of SDK
- (void)configSigDataSourceDefaultParameter {
    //1.一个provisioner分配的地址范围，默认为0x400。
    SigDataSource.share.defaultAllocatedUnicastRangeHighAddress = kAllocatedUnicastRangeHighAddress;
    //2.mesh网络的sequenceNumber步长，默认为128。
    SigDataSource.share.defaultSequenceNumberIncrement = kSequenceNumberIncrement;
    
    //3.(可选，旧判断)SDK的分组默认绑定5个modelID，可通过以下接口修改分组默认绑定的modelIDs
//    SigDataSource.share.defaultGroupSubscriptionModels = [NSMutableArray arrayWithArray:@[@(kSigModel_GenericOnOffServer_ID),@(kSigModel_LightLightnessServer_ID),@(kSigModel_LightCTLServer_ID),@(kSigModel_LightCTLTemperatureServer_ID),@(kSigModel_LightHSLServer_ID),@(kSigModel_LightLCServer_ID)]];
//    [SigDataSource.share.defaultGroupSubscriptionModels addObject:@(0x00000211)];//新增vendorModelID用于测试加组及vendor组控。

    //4.(可选，新判断)demo v4.1.0.0及之后添加的标志，标志订阅组地址时是否把Service Level ModelID（0x1003）也订阅了。
    // 客户存在Mesh遥控器产品且Mesh遥控器产品存在Level控制功能，则需要在app端使能该功能。默认值为关闭。
    // The flag added in demo v4.1.0.0 and later indicates whether the Service Level ModelID (0x1003) is also subscribed when subscribing to the group address.
    // If the customer has a Mesh remote control product and the Mesh remote control product has a Level control function, this function needs to be enabled on the app side. The default value is off.
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

    //5.(可选)SDK默认实现了PID为1、2、7、0x0201的设备的fast bind功能，其它类型的设备可通过以下接口添加该类型设备默认的nodeInfo以实现fast bind功能
//    //示范代码：添加PID=8，composition data=TemByte的数据到SigDataSource.share.defaultNodeInfos。
//    DeviceTypeModel *model = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:8];
//    static Byte TemByte[] = {(Byte) 0x11, (Byte) 0x02, (Byte) 0x01, (Byte) 0x00, (Byte) 0x32, (Byte) 0x37, (Byte) 0x69, (Byte) 0x00, (Byte) 0x07, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x19, (Byte) 0x01, (Byte) 0x00, (Byte) 0x00, (Byte) 0x02, (Byte) 0x00, (Byte) 0x03, (Byte) 0x00, (Byte) 0x04, (Byte) 0x00, (Byte) 0x05, (Byte) 0x00, (Byte) 0x00, (Byte) 0xfe, (Byte) 0x01, (Byte) 0xfe, (Byte) 0x02, (Byte) 0xfe, (Byte) 0x00, (Byte) 0xff, (Byte) 0x01, (Byte) 0xff, (Byte) 0x00, (Byte) 0x12, (Byte) 0x01, (Byte) 0x12, (Byte) 0x00, (Byte) 0x10, (Byte) 0x02, (Byte) 0x10, (Byte) 0x04, (Byte) 0x10, (Byte) 0x06, (Byte) 0x10, (Byte) 0x07, (Byte) 0x10, (Byte) 0x03, (Byte) 0x12, (Byte) 0x04, (Byte) 0x12, (Byte) 0x06, (Byte) 0x12, (Byte) 0x07, (Byte) 0x12, (Byte) 0x00, (Byte) 0x13, (Byte) 0x01, (Byte) 0x13, (Byte) 0x03, (Byte) 0x13, (Byte) 0x04, (Byte) 0x13, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x02, (Byte) 0x00, (Byte) 0x02, (Byte) 0x10, (Byte) 0x06, (Byte) 0x13};
//    NSData *nodeInfoData = [NSData dataWithBytes:TemByte length:76];
//    [model setCompositionData:nodeInfoData];
//    [SigDataSource.share.defaultNodeInfos addObject:model];

    //6.添加更多的默认Composition Data数据，如果客户没有使用下面这3款型号的芯片，可以不掉下面的方法。
    //v3.3.3.6版本开始，新增8278、8269、9518的cpsData.
    [self addMoreCompositionData];

    //7.(可选)SDK默认publish周期为20秒，通过修改可以修改SDK的默认publish参数，或者客户自行实现publish检测机制。
//    SigPeriodModel *periodModel = [[SigPeriodModel alloc] init];
//    periodModel.numberOfSteps = kPublishIntervalOfDemo;
////    periodModel.numberOfSteps = 5;//整形，范围0x01~0x3F.
//    periodModel.resolution = [LibTools getSigStepResolutionInMillisecondsOfJson:SigStepResolution_seconds];
//    SigDataSource.share.defaultPublishPeriodModel = periodModel;

    //8.(可选)发送完一组segment数据包后transmissionTimerInterval+0.5*TTL的时间会再次重发segment数据。其中transmissionTimerInterval会取当前的值与0.2之间的最大值。
    SigMeshLib.share.transmissionTimerInterval = 0.600;
    //9.(可选)配置是否在KeyBind完成后进行Public Time Model。SDK默认为YES。
//    SigDataSource.share.needPublishTimeModel = NO;
    //10.(可选)v3.3.3新增配置项
    //非LPN节点的默认可靠发包间隔，默认值为1.28。
//    SigDataSource.share.defaultReliableIntervalOfNotLPN = kSDKLibCommandTimeout;
    //LPN节点的默认可靠发包间隔，默认值为2.56。
//    SigDataSource.share.defaultReliableIntervalOfLPN = kSDKLibCommandTimeout * 2;
    //(可选)v3.3.3.6及之后新增配置项
    //Normal KeyBind是否使用组合指令来加快KeyBind的速度。
//    SigDataSource.share.aggregatorEnable = YES;

}

/// 9.Import one mesh dictionary from app to SDK
- (void)importOneMeshDictionaryFromAppToSDK {
    //(可选)4.v4.1.0.0 APP新增Mesh网络管理功能，SDK内部只存储一个Mesh的数据，Demo可以存储多个Mesh的数据，并通过Import Mesh的方式进行Mesh的切换。
    //Demo存储一个Mesh的列表，并记录当前使用的Mesh的MeshUUID，如果登录的时候发现列表里面没有这个MeshUUID则默认选中index=0的Mesh。
    //第一次打开APP时，需要将SDK新建的网络数据存储到Demo端，并记录当前使用的MeshUUID。
    NSArray *meshList = [[NSUserDefaults standardUserDefaults] valueForKey:kCacheMeshListKey];
    if (meshList == nil) {
        NSDictionary *meshDict = [SigDataSource.share getDictionaryFromDataSource];
        NSData *tempData = [LibTools getJSONDataWithDictionary:meshDict];
        //Migrate data from a single network to multiple networks.
        NSData *oldData = [NSUserDefaults.standardUserDefaults objectForKey:kSaveLocationDataKey];
        if (oldData.length > 0) {
            //exist old single network
            meshList = [NSArray arrayWithObject:oldData];
        } else {
            //not exist
            meshList = [NSArray arrayWithObject:tempData];
        }
        [[NSUserDefaults standardUserDefaults] setValue:meshList forKey:kCacheMeshListKey];
        [[NSUserDefaults standardUserDefaults] setValue:SigDataSource.share.meshUUID forKey:kCacheCurrentMeshUUIDKey];
        [[NSUserDefaults standardUserDefaults] synchronize];
    } else {
        NSString *meshUUID = [[NSUserDefaults standardUserDefaults] valueForKey:kCacheCurrentMeshUUIDKey];
        for (NSData *data in meshList) {
            NSDictionary *meshDict = [LibTools getDictionaryWithJSONData:data];
            if ([[meshDict[@"meshUUID"] uppercaseString] isEqualToString:[meshUUID uppercaseString]]) {
                [SigDataSource.share switchToNewMeshDictionary:meshDict];
                break;
            }
        }
    }
}

/// 10.Create Telink Bin Folder in iPhone
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

/// 11.(可选)Config Filter
- (void)configFilter {
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
}

/// 12.Config Provision Mode
- (void)configProvisionMode {
    //demo v4.1.0.0新增Provision模式选择参数，demo默认使用可选设备的普通添加模式。
    NSNumber *provisionMode = [[NSUserDefaults standardUserDefaults] valueForKey:kProvisionMode];
    if (provisionMode == nil) {
        // demo默认使用可选设备的普通添加模式。
        provisionMode = [NSNumber numberWithInt:ProvisionMode_normalSelectable];
        [[NSUserDefaults standardUserDefaults] setValue:provisionMode forKey:kProvisionMode];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
}

/// 13.Config Import Complete Action
- (void)configImportCompleteAction {
    // demo v4.1.0.0新增配置项：导入Mesh后自动切换Mesh还是弹框提示用户选择是否切换Mesh。
    NSNumber *importCompleteAction = [[NSUserDefaults standardUserDefaults] valueForKey:kImportCompleteAction];
    if (importCompleteAction == nil) {
        // demo默认使用弹框提示用户选择是否切换Mesh
        importCompleteAction = [NSNumber numberWithInt:ImportSwitchMode_manual];
        [[NSUserDefaults standardUserDefaults] setValue:importCompleteAction forKey:kImportCompleteAction];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
}

/// 14.Config Default Root Certificate Data
- (void)configDefaultRootCertificateData {
    //demo v3.3.4新增certificate-base Provision使用的默认根证书文件名，demo不重新赋值则默认使用PTS的root.der。
    NSString *rootCertificateName = [[NSUserDefaults standardUserDefaults] valueForKey:kRootCertificateName];
    if (rootCertificateName != nil) {
        NSData *selectData = [LibTools getDataWithFileName:rootCertificateName fileType:nil];
        if (selectData && selectData.length > 0) {
            SigDataSource.share.defaultRootCertificateData = selectData;
        }
    }
}

/// 15.Config sortType of nodeList
- (void)configSortTypeOfNodeList {
    // demo v4.1.0.1新增配置项：node节点的排序方式，默认是短地址升序。
    NSNumber *sortTypeOfNodeList = [[NSUserDefaults standardUserDefaults] valueForKey:kSortTypeOfNodeList];
    if (sortTypeOfNodeList == nil) {
        // demo默认使用短地址升序
        sortTypeOfNodeList = [NSNumber numberWithInt:SigDataSource.share.sortTypeOfNodeList];
        [[NSUserDefaults standardUserDefaults] setValue:sortTypeOfNodeList forKey:kSortTypeOfNodeList];
        [[NSUserDefaults standardUserDefaults] synchronize];
    } else {
        SigDataSource.share.sortTypeOfNodeList = [sortTypeOfNodeList intValue];
        // 重新排序一次并存储本地
        [SigDataSource.share saveLocationData];
    }
}

/// v3.3.3.6版本开始，新增8278、8269、9518的cpsData.
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
    telinkPid_8258_CT.minorProductType = telinkPid_8278_CT.minorProductType = telinkPid_8269_CT.minorProductType = telinkPid_9518_CT.minorProductType = telinkPid_8258_LPN.minorProductType = telinkPid_8278_LPN.minorProductType = telinkPid_8269_LPN.minorProductType = telinkPid_9518_LPN.minorProductType = SigNodePID_CT;
    telinkPid_8258_HSL.minorProductType = telinkPid_8278_HSL.minorProductType = telinkPid_8269_HSL.minorProductType = telinkPid_9518_HSL.minorProductType = SigNodePID_HSL;
    telinkPid_8258_Panel.minorProductType = telinkPid_8278_Panel.minorProductType = telinkPid_8269_Panel.minorProductType = telinkPid_9518_Panel.minorProductType = SigNodePID_PANEL;

    DeviceTypeModel *model_8278_CT = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:telinkPid_8278_CT.value compositionData:nil];
    DeviceTypeModel *model_8269_CT = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:telinkPid_8269_CT.value compositionData:nil];
    DeviceTypeModel *model_9518_CT = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:telinkPid_9518_CT.value compositionData:nil];
    DeviceTypeModel *model_8278_HSL = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:telinkPid_8278_HSL.value compositionData:nil];
    DeviceTypeModel *model_8269_HSL = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:telinkPid_8269_HSL.value compositionData:nil];
    DeviceTypeModel *model_9518_HSL = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:telinkPid_9518_HSL.value compositionData:nil];
    DeviceTypeModel *model_8278_Panel = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:telinkPid_8278_Panel.value compositionData:nil];
    DeviceTypeModel *model_8269_Panel = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:telinkPid_8269_Panel.value compositionData:nil];
    DeviceTypeModel *model_9518_Panel = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:telinkPid_9518_Panel.value compositionData:nil];
    DeviceTypeModel *model_8278_LPN = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:telinkPid_8278_LPN.value compositionData:nil];
    DeviceTypeModel *model_8269_LPN = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:telinkPid_8269_LPN.value compositionData:nil];
    DeviceTypeModel *model_9518_LPN = [[DeviceTypeModel alloc] initWithCID:kCompanyID PID:telinkPid_9518_LPN.value compositionData:nil];
    [SigDataSource.share.defaultNodeInfos addObjectsFromArray:@[model_8278_CT, model_8269_CT, model_9518_CT, model_8278_HSL, model_8269_HSL, model_9518_HSL, model_8278_Panel, model_8269_Panel, model_9518_Panel, model_8278_LPN, model_8269_LPN, model_9518_LPN]];
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
