//
//  BackgroundTimer.h
//  TelinkSigMeshDemo
//
//  Created by 梁家誌 on 2019/11/27.
//  Copyright © 2019 梁家誌. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface BackgroundTimer : NSObject

@property (nonatomic,assign,readonly) NSTimeInterval interval;
/// Chedules a timer that can be started from a background DispatchQueue.
+ (BackgroundTimer * _Nonnull)scheduledTimerWithTimeInterval:(NSTimeInterval)interval repeats:(BOOL)repeats block:(void (^ _Nonnull)(BackgroundTimer * _Nonnull t))block;
/// Asynchronously cancels the dispatch source, preventing any further invocation of its event handler block.
- (void)invalidate;

@end

NS_ASSUME_NONNULL_END
