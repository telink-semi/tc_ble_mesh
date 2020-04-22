//
//  SigRemoteAddManager.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2020/3/26.
//  Copyright © 2020 梁家誌. All rights reserved.
//

#import "SigRemoteAddManager.h"

@interface SigRemoteAddManager ()<SigMessageDelegate>
@property (nonatomic, assign) BOOL isRemoteScaning;
@property (nonatomic, assign) UInt16 currentReportNodeAddress;
@property (nonatomic, assign) UInt8 currentMaxScannedItems;
@property (nonatomic, weak) id oldDelegateForDeveloper;
@property (nonatomic, strong) NSMutableArray <SigRemoteScanRspModel *>*remoteScanRspModels;
@property (nonatomic,copy) remoteProvisioningScanReportCallBack reportBlock;
@property (nonatomic,copy) resultBlock scanResultBlock;
@end

@implementation SigRemoteAddManager

+ (SigRemoteAddManager *)share {
    static SigRemoteAddManager *shareManager = nil;
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        shareManager = [[SigRemoteAddManager alloc] init];
    });
    return shareManager;
}

- (void)startRemoteProvisionScanWithReportCallback:(remoteProvisioningScanReportCallBack)reportCallback resultCallback:(resultBlock)resultCallback {
    if (self.isRemoteScaning) {
        NSString *errstr = @"SigRemoteAddManager is remoteScaning, please call stopRemoteProvisionScan";
        TeLogError(@"%@",errstr);
        NSError *err = [NSError errorWithDomain:errstr code:-1 userInfo:nil];
        if (resultCallback) {
            resultCallback(NO, err);
        }
        return;
    }
    self.reportBlock = reportCallback;
    self.scanResultBlock = resultCallback;
    self.oldDelegateForDeveloper = SigMeshLib.share.delegateForDeveloper;
    SigMeshLib.share.delegateForDeveloper = self;
    self.remoteScanRspModels = [NSMutableArray array];
    self.isRemoteScaning = YES;
    [self startSingleRemoteProvisionScan];
}

- (void)stopRemoteProvisionScan {
    self.isRemoteScaning = NO;
}

- (void)endRemoteProvisionScan {
    self.isRemoteScaning = NO;
    if (self.currentMaxScannedItems >= 0x04 && self.currentMaxScannedItems <= 0xFF) {
        if (self.scanResultBlock) {
            self.scanResultBlock(YES, nil);
        }
    } else {
        NSString *errstr = @"Reomte scan fail: current connected node is no support remote provision scan.";
        TeLogError(@"%@",errstr);
        NSError *err = [NSError errorWithDomain:errstr code:-1 userInfo:nil];
        if (self.scanResultBlock) {
            self.scanResultBlock(NO, err);
        }
    }
}

- (void)startSingleRemoteProvisionScan {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(endSingleRemoteProvisionScan) object:nil];
        [self performSelector:@selector(endSingleRemoteProvisionScan) withObject:nil afterDelay:4.0+2.0];
    });
    self.currentReportNodeAddress = SigDataSource.share.getCurrentConnectedNode.address;
    [self getRemoteProvisioningScanCapabilities];
}

- (void)endSingleRemoteProvisionScan {
    [self endRemoteProvisionScan];
}

// 1.remoteProvisioningScanCapabilitiesGet
- (void)getRemoteProvisioningScanCapabilities {
    TeLogInfo(@"getRemoteProvisioningScanCapabilities address=0x%x",self.currentReportNodeAddress);
    __weak typeof(self) weakSelf = self;
    [SDKLibCommand remoteProvisioningScanCapabilitiesGetWithDestination:self.currentReportNodeAddress resMax:1 retryCount:2 successCallback:^(UInt16 source, UInt16 destination, SigRemoteProvisioningScanCapabilitiesStatus * _Nonnull responseMessage) {
//        if (responseMessage.activeScan) {
            weakSelf.currentMaxScannedItems = responseMessage.maxScannedItems;
            [weakSelf performSelector:@selector(startRemoteProvisioningScan) withObject:nil afterDelay:0.5];
//        } else {
//            TeLogInfo(@"nodeAddress 0x%x no support remote provision scan, try next address.");
//            [weakSelf endSingleRemoteProvisionScan];
//        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        if (error) {
            TeLogInfo(@"nodeAddress 0x%x get remote provision scan Capabilities timeout, try next address.");
            [weakSelf endSingleRemoteProvisionScan];
        }
    }];
}

// 2.remoteProvisioningScanStart
- (void)startRemoteProvisioningScan {
    [SDKLibCommand remoteProvisioningScanStartWithScannedItemsLimit:self.currentMaxScannedItems timeout:4.0 UUID:nil destination:kMeshAddress_allNodes resMax:1 retryCount:2 successCallback:^(UInt16 source, UInt16 destination, SigRemoteProvisioningScanStatus * _Nonnull responseMessage) {
        TeLogInfo(@"source=0x%x,destination=0x%x,opCode=0x%x,parameters=%@",responseMessage.opCode,[LibTools convertDataToHexStr:responseMessage.parameters]);
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
    }];

}

#pragma mark - SigMessageDelegate

- (void)didReceiveMessage:(SigMeshMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination {
    TeLogVerbose(@"didReceiveMessage=%@,message.parameters=%@,source=0x%x,destination=0x%x",message,message.parameters,source,destination);
    if ([message isKindOfClass:[SigRemoteProvisioningScanReport class]] && self.isRemoteScaning) {
        // Try parsing the response.
        SigRemoteScanRspModel *model = [[SigRemoteScanRspModel alloc] initWithParameters:message.parameters];
        if (!model) {
            TeLogInfo(@"parsing SigRemoteProvisioningScanReport fail.");
            return;
        }
        model.reportNodeAddress = self.currentReportNodeAddress;
        if (![self.remoteScanRspModels containsObject:model]) {
            [self.remoteScanRspModels addObject:model];
        }
        if (self.reportBlock) {
            self.reportBlock(model);
        }
    }
}

- (void)didSendMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toDestination:(UInt16)destination {
//    TeLogVerbose(@"didSendMessage=%@,class=%@,source=0x%x,destination=0x%x",message,message.class,localElement.unicastAddress,destination);
}

- (void)failedToSendMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toDestination:(UInt16)destination error:(NSError *)error {
//    TeLogVerbose(@"failedToSendMessage=%@,class=%@,source=0x%x,destination=0x%x",message,message.class,localElement.unicastAddress,destination);
}

- (void)didReceiveSigProxyConfigurationMessage:(SigProxyConfigurationMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination {
//    TeLogVerbose(@"didReceiveSigProxyConfigurationMessage=%@,source=0x%x,destination=0x%x",message,source,destination);
}

@end
