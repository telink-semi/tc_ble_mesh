//
//  SigPublishManager.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/12/20.
//  Copyright © 2019 梁家誌. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef void(^DiscoverOutlineNodeBlock)(NSNumber *unicastAddress);
typedef void(^DiscoverOnlineNodeBlock)(NSNumber *unicastAddress);

@interface SigPublishManager : NSObject
@property (nonatomic,copy) DiscoverOutlineNodeBlock discoverOutlineNodeCallback;
@property (nonatomic,copy) DiscoverOnlineNodeBlock discoverOnlineNodeCallback;

+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));


+ (SigPublishManager *)share;

- (void)startCheckOfflineTimerWithAddress:(NSNumber *)address;

- (void)stopCheckOfflineTimerWithAddress:(NSNumber *)address;

@end

NS_ASSUME_NONNULL_END
