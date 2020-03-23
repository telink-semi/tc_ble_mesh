//
//  SigPublishManager.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/12/20.
//  Copyright © 2019 梁家誌. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface SigPublishManager : NSObject

+ (SigPublishManager *)share;

- (void)startCheckOfflineTimerWithAddress:(NSNumber *)address;

- (void)stopCheckOfflineTimerWithAddress:(NSNumber *)address;

@end

NS_ASSUME_NONNULL_END
