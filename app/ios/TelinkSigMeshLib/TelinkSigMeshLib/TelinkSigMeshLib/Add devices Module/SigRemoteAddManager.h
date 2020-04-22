//
//  SigRemoteAddManager.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2020/3/26.
//  Copyright © 2020 梁家誌. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef void(^remoteProvisioningScanReportCallBack)(SigRemoteScanRspModel *scanRemoteModel);

@interface SigRemoteAddManager : NSObject


+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));


+ (SigRemoteAddManager *)share;

- (void)startRemoteProvisionScanWithReportCallback:(remoteProvisioningScanReportCallBack)reportCallback resultCallback:(resultBlock)resultCallback;

@end

NS_ASSUME_NONNULL_END
