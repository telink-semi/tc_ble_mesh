/********************************************************************************************************
 * @file     SigSegmentedMessage.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/9/16
 *
 * @par     Copyright (c) [2021], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/

#import "SigSegmentedMessage.h"

@implementation SigSegmentedMessage

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _sequenceZero = 0;
    }
    return self;
}

/// Returns whether the message is composed of only a single
/// segment. Single segment messages are used to send short,
/// acknowledged messages. The maximum size of payload of upper
/// transport control PDU is 8 bytes.
- (BOOL)isSingleSegment {
    return _lastSegmentNumber == 0;
}

/// Returns the `segmentOffset` as `Int`.
- (int)index {
    return (int)_segmentOffset;
}

/// Returns the expected number of segments for this message.
- (int)count {
    return (int)_lastSegmentNumber + 1;
}

@end
