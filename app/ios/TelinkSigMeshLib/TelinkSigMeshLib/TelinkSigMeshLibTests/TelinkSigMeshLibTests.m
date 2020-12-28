//
//  TelinkSigMeshLibTests.m
//  TelinkSigMeshLibTests
//
//  Created by 梁家誌 on 2020/10/29.
//  Copyright © 2020 梁家誌. All rights reserved.
//

#import <XCTest/XCTest.h>
#import "TelinkSigMeshLibExtensions/TelinkSigMeshLib.h"
/*需要使用模拟器运行测试单元！！！*/

@interface TelinkSigMeshLibTests : XCTestCase

@end

@implementation TelinkSigMeshLibTests

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

- (void)testCreateMeshNetWork {
    [SDKLibCommand startMeshSDK];
    //1.netKey
    XCTAssertEqual(SigDataSource.share.netKeys.count, 1);
    XCTAssertEqualObjects(SigDataSource.share.netKeys.firstObject, SigDataSource.share.curNetkeyModel);
    XCTAssertEqualObjects(SigDataSource.share.netKeys.firstObject.key, SigDataSource.share.curNetkeyModel.key);
    XCTAssertEqual(SigDataSource.share.curNetkeyModel.key.length, 2*16);
    XCTAssertEqual(SigDataSource.share.curNetkeyModel.index, 0);
    XCTAssertEqual(SigDataSource.share.curNetkeyModel.phase, 0);
    XCTAssertNotNil(SigDataSource.share.curNetkeyModel.timestamp);
    XCTAssertEqualObjects(SigDataSource.share.curNetkeyModel.oldKey, @"00000000000000000000000000000000");
    XCTAssertNotNil(SigDataSource.share.curNetkeyModel.name);
    XCTAssertEqualObjects(SigDataSource.share.curNetkeyModel.minSecurity, @"secure");
    XCTAssertEqualObjects(SigDataSource.share.curNetkeyModel.key, SigDataSource.share.meshUUID);

    //2.appKey
    XCTAssertEqual(SigDataSource.share.appKeys.count, 1);
    XCTAssertEqualObjects(SigDataSource.share.appKeys.firstObject, SigDataSource.share.curAppkeyModel);
    XCTAssertEqual(SigDataSource.share.curAppkeyModel.key.length, 2*16);
    XCTAssertEqualObjects(SigDataSource.share.curAppkeyModel.oldKey, @"00000000000000000000000000000000");
    XCTAssertNotNil(SigDataSource.share.curAppkeyModel.name);
    XCTAssertEqual(SigDataSource.share.curAppkeyModel.boundNetKey, 0);
    XCTAssertEqual(SigDataSource.share.curAppkeyModel.index, 0);
    
    //3.provisioner
    XCTAssertEqual(SigDataSource.share.provisioners.count, 1);
    XCTAssertEqualObjects(SigDataSource.share.provisioners.firstObject, SigDataSource.share.curProvisionerModel);
    XCTAssertEqual(SigDataSource.share.curProvisionerModel.UUID.length, 2*16);
    XCTAssertEqual(SigDataSource.share.curProvisionerModel.allocatedGroupRange.count, 1);
    XCTAssertEqual(SigDataSource.share.curProvisionerModel.allocatedGroupRange.firstObject.lowIntAddress, 0xc000);
    XCTAssertEqual(SigDataSource.share.curProvisionerModel.allocatedGroupRange.firstObject.hightIntAddress, 0xc0ff);
    XCTAssertEqual(SigDataSource.share.curProvisionerModel.allocatedUnicastRange.count, 1);
    XCTAssertEqual(SigDataSource.share.curProvisionerModel.allocatedUnicastRange.firstObject.lowIntAddress, 0x0001);
    XCTAssertEqual(SigDataSource.share.curProvisionerModel.allocatedUnicastRange.firstObject.hightIntAddress, 0x03ff);
    XCTAssertEqual(SigDataSource.share.curProvisionerModel.allocatedSceneRange.count, 1);
    XCTAssertEqualObjects(SigDataSource.share.curProvisionerModel.allocatedSceneRange.firstObject.firstScene, @"0001");
    XCTAssertEqualObjects(SigDataSource.share.curProvisionerModel.allocatedSceneRange.firstObject.lastScene, @"000F");
    XCTAssertEqualObjects(SigDataSource.share.curProvisionerModel.provisionerName, @"Telink iOS provisioner");
    XCTAssertEqualObjects(SigDataSource.share.curProvisionerModel.UUID, SigDataSource.share.getCurrentProvisionerUUID);
    
    //4.node
    XCTAssertEqual(SigDataSource.share.nodes.count, 1);
    XCTAssertEqualObjects(SigDataSource.share.nodes.firstObject, SigDataSource.share.curLocationNodeModel);
    XCTAssertEqualObjects(SigDataSource.share.curLocationNodeModel.UUID, SigDataSource.share.curProvisionerModel.UUID, @"provisioner的UUID与locationNode的UUID不相等！");
    XCTAssertTrue(SigDataSource.share.curLocationNodeModel.secureNetworkBeacon);
    XCTAssertEqual(SigDataSource.share.curLocationNodeModel.defaultTTL, TTL_DEFAULT);
    XCTAssertEqual(SigDataSource.share.curLocationNodeModel.features.proxyFeature, SigNodeFeaturesState_notSupported);
    XCTAssertEqual(SigDataSource.share.curLocationNodeModel.features.friendFeature, SigNodeFeaturesState_notEnabled);
    XCTAssertEqual(SigDataSource.share.curLocationNodeModel.features.relayFeature, SigNodeFeaturesState_notSupported);
    XCTAssertEqual(SigDataSource.share.curLocationNodeModel.relayRetransmit.count, 3);
    XCTAssertEqual(SigDataSource.share.curLocationNodeModel.relayRetransmit.interval, 10);
    XCTAssertEqual(SigDataSource.share.curLocationNodeModel.address, 1);
    XCTAssertEqualObjects(SigDataSource.share.curLocationNodeModel.name, @"Telink iOS provisioner node");
    XCTAssertEqual(SigDataSource.share.curLocationNodeModel.elements.count, 1);
    XCTAssertEqual(SigDataSource.share.curLocationNodeModel.elements.firstObject.parentNodeAddress, 1);
    XCTAssertEqual(SigDataSource.share.curLocationNodeModel.deviceKey.length, 2*16);

    //5.group
    XCTAssertEqual(SigDataSource.share.groups.count, 8);

    //6.other
    XCTAssertEqualObjects(SigDataSource.share.$schema, @"http://json-schema.org/draft-04/schema#");
    XCTAssertEqualObjects(SigDataSource.share.meshName, @"Telink-Sig-Mesh");
    XCTAssertEqualObjects(SigDataSource.share.version, @"1.0.0");
    XCTAssertNotNil(SigDataSource.share.timestamp);
    XCTAssertEqualObjects(SigDataSource.share.ivIndex, @"00000000");

    NSLog(@"==========finish!");
}

@end
