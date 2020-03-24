//
//  SigSegmentedMessage.m
//  TelinkSigMeshLib
//
//  Created by Liangjiazhi on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigSegmentedMessage.h"

@implementation SigSegmentedMessage

- (instancetype)init {
    if (self = [super init]) {
        _sequenceZero = 0;
    }
    return self;
}

- (BOOL)isSingleSegment {
    return _lastSegmentNumber == 0;
}

- (int)index {
    return (int)_segmentOffset;
}

- (int)count {
    return (int)_lastSegmentNumber + 1;
}

@end
