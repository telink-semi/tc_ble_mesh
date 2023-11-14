/********************************************************************************************************
 * @file     SigMeshOCDemoTests.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2021/10/21
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

#import <XCTest/XCTest.h>
//#import "TelinkSigMeshLib.h"
//#import "ColorManager.h"

@interface SigMeshOCDemoTests : XCTestCase

@end

@implementation SigMeshOCDemoTests

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
}

- (void)testExample {
    // This is an example of a functional test case.
    // Use XCTAssert and related functions to verify your tests produce the correct results.
}

- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

///// 测试解密共享文件夹的json文件`TelinkSDKMeshJsonData`
//- (void)testGetDecryptTelinkSDKMeshJsonData {
//    //1.
//    NSLog(@"%@",SigLogger.share.getDecryptTelinkSDKMeshJsonData);
//    //2.
////    NSString *key = @"com.telink.TelinkSDKMeshJsonData";
////    NSLog(@"%@",[SigLogger.share getDecryptTelinkSDKMeshJsonDataWithPassword:key]);
//}
//
///// 测试SigProvisionerModel创建及Unicast分配
//- (void)testCreateProvisionerAndDistributionUnicast {
//    SigDataSource.share.defaultAllocatedUnicastRangeHighAddress = 0x0100;
//    UInt16 maxHighAddressUnicast = 0;
//    while (1) {
//        if (maxHighAddressUnicast >= 0x7FFF) {
//            NSLog(@"mesh地址可用区间已经消耗完了！！");
//            break;
//        }
//        SigProvisionerModel *provisioner = [[SigProvisionerModel alloc] initWithExistProvisionerMaxHighAddressUnicast:maxHighAddressUnicast andProvisionerUUID:[LibTools convertDataToHexStr:[LibTools initMeshUUID]]];
//        NSLog(@"MaxHighAddressUnicast=0x%X,lowAddress=0x%@,highAddress=0x%@",maxHighAddressUnicast,provisioner.allocatedUnicastRange.firstObject.lowAddress,provisioner.allocatedUnicastRange.firstObject.highAddress);
//        maxHighAddressUnicast = provisioner.allocatedUnicastRange.firstObject.heightIntAddress;
//    }
//}
//
//- (void)testHSLModel {
////    CGFloat value = 0.9;
////    while (value <= 1.000000) {
////        CGFloat hue = (value * 359.999999)/360.0;
////        NSLog(@"UISlider.value=%f,hue=%f,intHue=%f-%d",value,hue,floorf((hue * 360)),(int)(hue*360));
////        value += 0.000001;
////    }
////    CGFloat hue = (value * 359.999999)/360.0;
////    NSLog(@"UISlider.value=%f,hue=%f,intHue=%f-%d",value,hue,floorf((hue * 360)),(int)(hue*360));
//    for (int r = 0; r < 256; r+=5) {
//        for (int g = 0; g < 256; g+=5) {
//            for (int b = 0; b < 256; b+=5) {
//                UIColor *color = [UIColor colorWithRed:r/255.0 green:g/255.0 blue:b/255.0 alpha:1.0];
//                HSVModel *hsv = [ColorManager getHSVWithColor:color];
//                HSLModel *hsl = [ColorManager getHSLWithColor:color];
//                NSLog(@"r=%d,g=%d,b=%d,h=%f,s=%f,v=%f,h=%f,s=%f,l=%f",r,g,b,hsv.hue,hsv.saturation,hsv.value,hsl.hue,hsl.saturation,hsl.lightness);
//            }
//        }
//    }
//}
//
//- (void)testSubnetBridge {
//    SigSubnetBridgeStatus *status1 = [[SigSubnetBridgeStatus alloc] init];
//    SigSubnetBridgeModel *model = [[SigSubnetBridgeModel alloc] initWithDirections:SigDirectionsFieldValues_bidirectional netKeyIndex1:0x001 netKeyIndex2:0x000 address1:0x0100 address2:0x0004];
//    SigSubnetBridgeStatus *status2 = [[SigSubnetBridgeStatus alloc] initWithSubnetBridge:model];
//    SigSubnetBridgeStatus *status3 = [[SigSubnetBridgeStatus alloc] initWithParameters:status2.parameters];
//    NSLog(@"status1=%@",status1.parameters);
//    NSLog(@"status2=%@",status2.parameters);
//    NSLog(@"status3=%@",status3.parameters);
//
//    SigBridgeSubnetModel *model1 = [[SigBridgeSubnetModel alloc] init];
//    SigBridgeSubnetModel *model2 = [[SigBridgeSubnetModel alloc] initWithNetKeyIndex1:0x001 netKeyIndex2:0x000];
//    SigBridgeSubnetModel *model3 = [[SigBridgeSubnetModel alloc] initWithParameters:model2.parameters];
//    NSLog(@"model1=%@",model1.parameters);
//    NSLog(@"model2=%@",model2.parameters);
//    NSLog(@"model3=%@",model3.parameters);
//}
//
//- (void)testSigPeriodModel {
//    SigPeriodModel *model1 = [[SigPeriodModel alloc] init];
//    model1.numberOfSteps = 15;
//    model1.resolution = [LibTools getSigStepResolutionInMillisecondsOfJson:SigStepResolution_hundredsOfMilliseconds];
//    NSLog(@"model1=%f",[LibTools getIntervalWithSigPeriodModel:model1]);
//    SigPeriodModel *model2 = [[SigPeriodModel alloc] init];
//    model2.numberOfSteps = 25;
//    model2.resolution = [LibTools getSigStepResolutionInMillisecondsOfJson:SigStepResolution_seconds];
//    NSLog(@"model2=%f",[LibTools getIntervalWithSigPeriodModel:model2]);
//    SigPeriodModel *model3 = [[SigPeriodModel alloc] init];
//    model3.numberOfSteps = 35;
//    model3.resolution = [LibTools getSigStepResolutionInMillisecondsOfJson:SigStepResolution_tensOfSeconds];
//    NSLog(@"model3=%f",[LibTools getIntervalWithSigPeriodModel:model3]);
//    SigPeriodModel *model4 = [[SigPeriodModel alloc] init];
//    model4.numberOfSteps = 45;
//    model4.resolution = [LibTools getSigStepResolutionInMillisecondsOfJson:SigStepResolution_tensOfMinutes];
//    NSLog(@"model4=%f",[LibTools getIntervalWithSigPeriodModel:model4]);
//}

@end
