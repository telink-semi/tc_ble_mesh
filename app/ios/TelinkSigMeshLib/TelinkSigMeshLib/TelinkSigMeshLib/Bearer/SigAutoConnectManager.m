//
//  SigAutoConnectManager.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/12/4.
//  Copyright © 2019 梁家誌. All rights reserved.
//

#import "SigAutoConnectManager.h"

@implementation SigAutoConnectManager

/// connect for normal: scan -> connect -> readGATT -> open notify -> callback(get status)
/// @param complete call back when success
- (void)startAutoConnectWithComplete:(connectSuccessCallBack)complete {
    
}

/// connect for normal: scan -> connect -> readGATT -> open notify -> callback(get status)
/// @param timeout time from scan to callback
/// @param successBlock call back when success
/// @param failBlock call back when fail
- (void)startConnectWithTimeout:(UInt8)timeout successBlock:(connectSuccessCallBack)successBlock failBlock:(connectFailCallBack)failBlock {
    
}

/// api use befor change meshNerwork stop nonnected and stop auto connect.
- (void)stopCurrentMeshConnectAndStopAutoConnect {
    
}

/// api use after SigBearer had disconnected, SigAutoConnectManager will auto connect at there.
- (void)meshNetworkDisconnected {
    
}

@end
