/********************************************************************************************************
 * @file     SigMeshOCDemoTests.m 
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
//  SigMeshOCDemoTests.m
//  SigMeshOCDemoTests
//
//  Created by 梁家誌 on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import <XCTest/XCTest.h>
#import "TelinkSigMeshLib.h"
#import "ColorManager.h"

@interface SigMeshOCDemoTests : XCTestCase

@end

@implementation SigMeshOCDemoTests

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
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


/// 测试解密共享文件夹的json文件`TelinkSDKMeshJsonData`
- (void)testGetDecryptTelinkSDKMeshJsonData {
//    NSLog(@"%@",SigLogger.share.getDecryptTelinkSDKMeshJsonData);
    
//    NSString *key = [[NSBundle mainBundle] bundleIdentifier];
    NSString *key = @"com.Ledvance.smartapp";
    NSLog(@"%@",[SigLogger.share getDecryptTelinkSDKMeshJsonDataWithPassword:key]);
}

/// 测试SigProvisionerModel创建及Unicast分配
- (void)testCreateProvisionerAndDistributionUnicast {
    SigDataSource.share.defaultAllocatedUnicastRangeHighAddress = 0x0100;
    UInt16 maxHighAddressUnicast = 0;
    while (1) {
        if (maxHighAddressUnicast >= 0x7FFF) {
            NSLog(@"mesh地址可用区间已经消耗完了！！");
            break;
        }
        SigProvisionerModel *provisioner = [[SigProvisionerModel alloc] initWithExistProvisionerMaxHighAddressUnicast:maxHighAddressUnicast andProvisionerUUID:[LibTools convertDataToHexStr:[LibTools initMeshUUID]]];
        NSLog(@"MaxHighAddressUnicast=0x%X,lowAddress=0x%@,highAddress=0x%@",maxHighAddressUnicast,provisioner.allocatedUnicastRange.firstObject.lowAddress,provisioner.allocatedUnicastRange.firstObject.highAddress);
        maxHighAddressUnicast = provisioner.allocatedUnicastRange.firstObject.hightIntAddress;
    }
}

- (void)testHSLModel {
//    CGFloat value = 0.9;
//    while (value <= 1.000000) {
//        CGFloat hue = (value * 359.999999)/360.0;
//        NSLog(@"UISlider.value=%f,hue=%f,intHue=%f-%d",value,hue,floorf((hue * 360)),(int)(hue*360));
//        value += 0.000001;
//    }
//    CGFloat hue = (value * 359.999999)/360.0;
//    NSLog(@"UISlider.value=%f,hue=%f,intHue=%f-%d",value,hue,floorf((hue * 360)),(int)(hue*360));
    for (int r = 0; r < 256; r+=5) {
        for (int g = 0; g < 256; g+=5) {
            for (int b = 0; b < 256; b+=5) {
                UIColor *color = [UIColor colorWithRed:r/255.0 green:g/255.0 blue:b/255.0 alpha:1.0];
                HSVModel *hsv = [ColorManager getHSVWithColor:color];
                HSLModel *hsl = [ColorManager getHSLWithColor:color];
                NSLog(@"r=%d,g=%d,b=%d,h=%f,s=%f,v=%f,h=%f,s=%f,l=%f",r,g,b,hsv.hue,hsv.saturation,hsv.value,hsl.hue,hsl.saturation,hsl.lightness);
            }
        }
    }
}

@end
