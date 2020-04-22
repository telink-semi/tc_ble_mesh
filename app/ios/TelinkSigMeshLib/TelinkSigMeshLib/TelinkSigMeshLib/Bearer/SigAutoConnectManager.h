//
//  SigAutoConnectManager.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/12/4.
//  Copyright © 2019 梁家誌. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef void(^connectSuccessCallBack)(NSString *uuidString);
typedef void(^connectFailCallBack)(NSString *errorString);

@interface SigAutoConnectManager : NSObject
@property (nonatomic,strong) CBPeripheral *peripheral;
@property (nonatomic,assign) BOOL autoConnect;


/// connect for normal: scan -> connect -> readGATT -> open notify -> callback(get status)
/// @param complete call back when success
- (void)startAutoConnectWithComplete:(connectSuccessCallBack)complete;


/// connect for normal: scan -> connect -> readGATT -> open notify -> callback(get status)
/// @param timeout time from scan to callback
/// @param successBlock call back when success
/// @param failBlock call back when fail
- (void)startConnectWithTimeout:(UInt8)timeout successBlock:(connectSuccessCallBack)successBlock failBlock:(connectFailCallBack)failBlock;


/// api use to stop auto connect. (eg: stop connection to change meshNerwork, stop connection to add node)
- (void)stopCurrentMeshConnectAndStopAutoConnect;


/// api use after SigBearer had disconnected, SigAutoConnectManager will auto connect at there.
- (void)meshNetworkDisconnected;

@end

NS_ASSUME_NONNULL_END
