/********************************************************************************************************
* @file     SigKeySet.m
*
* @brief    for TLSR chips
*
* @author     telink
* @date     Sep. 30, 2010
*
* @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
*           All rights reserved.
*
*             The information contained herein is confidential and proprietary property of Telink
*              Semiconductor (Shanghai) Co., Ltd. and is available under the terms
*             of Commercial License Agreement between Telink Semiconductor (Shanghai)
*             Co., Ltd. and the licensee in separate contract or the terms described here-in.
*           This heading MUST NOT be removed from this file.
*
*              Licensees are granted free, non-transferable use of the information in this
*             file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
*
*******************************************************************************************************/
//
//  SigKeySet.m
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/28.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigKeySet.h"

@implementation SigKeySet

@end


@implementation SigAccessKeySet
- (instancetype)initWithApplicationKey:(SigAppkeyModel *)applicationKey {
    if (self = [super init]) {
        _applicationKey = applicationKey;
    }
    return self;
}

- (SigNetkeyModel *)networkKey {
    return _applicationKey.getCurrentBoundNetKey;
}

- (NSData *)accessKey {
    if (self.networkKey.phase == distributingKeys) {
        NSData *oldkey = self.applicationKey.getDataOldKey;
        if (oldkey != nil && oldkey.length != 0) {
            return oldkey;
        }
        return _applicationKey.getDataKey;
    }
    return _applicationKey.getDataKey;
}

- (UInt8)aid {
    if (self.networkKey.phase == distributingKeys) {
        UInt8 aid = self.applicationKey.oldAid;
        if (aid != 0) {
            return aid;
        }
        return _applicationKey.aid;
    }
    return _applicationKey.aid;
}

- (NSString *)description {
    return[NSString stringWithFormat:@"<%p> - applicationKey:0x%@", self, [LibTools convertDataToHexStr:[self accessKey]]];
}

@end


@implementation SigDeviceKeySet

- (instancetype)init {
    if (self = [super init]) {
        _isInitAid = NO;
    }
    return self;
}

- (NSData *)accessKey {
    return [LibTools nsstringToHex:_node.deviceKey];
}

- (instancetype)initWithNetworkKey:(SigNetkeyModel *)networkKey node:(SigNodeModel *)node {
    if (self = [super init]) {
        self.networkKey = networkKey;
        _node = node;
        _isInitAid = NO;
    }
    return self;
}

- (NSString *)description {
    return[NSString stringWithFormat:@"<%p> - deviceKey:0x%@", self, [LibTools convertDataToHexStr:[self accessKey]]];
}

@end
