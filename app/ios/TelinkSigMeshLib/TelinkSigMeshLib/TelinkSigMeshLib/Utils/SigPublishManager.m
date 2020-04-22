//
//  SigPublishManager.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/12/20.
//  Copyright © 2019 梁家誌. All rights reserved.
//

#import "SigPublishManager.h"

@interface SigPublishManager ()
//Dictionary of timer that check node off line.
@property (nonatomic,strong) NSMutableDictionary <NSNumber *,BackgroundTimer *>*checkOfflineTimerDict;
@end

@implementation SigPublishManager

+ (SigPublishManager *)share{
    static SigPublishManager *sharePublish = nil;
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        sharePublish = [[SigPublishManager alloc] init];
        [sharePublish initData];
    });
    return sharePublish;
}

- (void)initData{
    _checkOfflineTimerDict = [NSMutableDictionary dictionary];
}

#pragma mark check outline timer
- (void)setDeviceOffline:(NSNumber *)address{
    UInt16 adr = [address intValue];
    
    [self stopCheckOfflineTimerWithAddress:@(adr)];
    
    SigNodeModel *device = [SigDataSource.share getNodeWithAddress:adr];
    if (device) {
        if (device.hasPublishFunction && device.hasOpenPublish) {
            device.state = DeviceStateOutOfLine;
            NSString *str = [NSString stringWithFormat:@"======================device offline:0x%02X======================",adr];
            TeLogInfo(@"setDeviceOffline:0x%02X,%@",adr,str);
            if (self.discoverOutlineNodeCallback) {
                self.discoverOutlineNodeCallback(@(device.address));
            }
        }
    }
}

- (void)startCheckOfflineTimerWithAddress:(NSNumber *)address{
    SigNodeModel *device = [SigDataSource.share getNodeWithAddress:address.intValue];
    if (device && device.hasPublishFunction && device.hasOpenPublish && device.hasPublishPeriod) {
        [self stopCheckOfflineTimerWithAddress:address];
        __weak typeof(self) weakSelf = self;
        BackgroundTimer *timer = [BackgroundTimer scheduledTimerWithTimeInterval:kOfflineInterval repeats:NO block:^(BackgroundTimer * _Nonnull t) {
            [weakSelf setDeviceOffline:address];
        }];
        _checkOfflineTimerDict[address] = timer;
    }
}

- (void)stopCheckOfflineTimerWithAddress:(NSNumber *)address{
    BackgroundTimer *timer = _checkOfflineTimerDict[address];
    if (timer) {
        [_checkOfflineTimerDict removeObjectForKey:address];
    }
    [timer invalidate];
}


@end
