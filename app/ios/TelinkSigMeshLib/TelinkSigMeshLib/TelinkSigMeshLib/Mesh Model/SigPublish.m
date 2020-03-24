/********************************************************************************************************
* @file     SigPublish.m
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
//  SigPublish.m
//  TelinkSigMeshLib
//
//  Created by Liangjiazhi on 2019/9/11.
//  Copyright © 2019 Telink. All rights reserved.
//

#import "SigPublish.h"


@implementation SigRetransmit

- (instancetype)init {
    if (self = [super init]) {
        _count = 0;
        _interval = 50;
    }
    return self;
}

- (instancetype)initWithPublishRetransmitCount:(UInt8)publishRetransmitCount intervalSteps:(UInt8)intervalSteps {
    if (self = [super init]) {
        _count = publishRetransmitCount;
        // Interval is in 50 ms steps.
        _interval = (UInt16)(intervalSteps + 1) * 50;// ms
    }
    return self;
}

- (UInt8)steps {
    return (UInt8)((_interval / 50) - 1);
}

@end


@implementation SigPublish

/// Creates an instance of Publish object.
///
/// - parameters:
///   - destination: The publication address.
///   - applicationKey: The Application Key that will be used to send messages.
///   - friendshipCredentialsFlag: `True`, to use Friendship Security Material,
///                                `false` to use Master Security Material.
///   - ttl: Time to live. Use 0xFF to use Node's default TTL.
///   - periodSteps: Period steps, together with `periodResolution` are used to
///                  calculate period interval. Value can be in range 0...63.
///                  Value 0 disables periodic publishing.
///   - periodResolution: The period resolution, used to calculate interval.
///                       Use `._100_milliseconds` when periodic publishing is
///                       disabled.
///   - retransmit: The retransmission data. See `Retransmit` for details.
- (instancetype)initWithStringDestination:(NSString *)stringDestination withKeyIndex:(UInt16)keyIndex friendshipCredentialsFlag:(int)friendshipCredentialsFlag ttl:(UInt8)ttl periodSteps:(UInt8)periodSteps periodResolution:(SigStepResolution)periodResolution retransmit:(SigRetransmit *)retransmit {
    if (self = [super init]) {
        _address = stringDestination;
        _index = keyIndex;
        _credentials = friendshipCredentialsFlag ? 1 : 0;
        _ttl = ttl;
        _periodSteps = periodSteps;
        _periodResolution = periodResolution;
        _period = [SigHelper.share getPeriodFromSteps:periodResolution];
        _retransmit = retransmit;
    }
    return self;
}

/// This initializer should be used to remove the publication from a Model.
- (instancetype)init {
    self = [super init];
    if (self) {
        _address = @"0000";
        _index = 0;
        _credentials = 0;
        _ttl = 0;
        _periodSteps = 0;
        _periodResolution = SigStepResolution_hundredsOfMilliseconds;
        _period = 0;
        _retransmit = [[SigRetransmit alloc] initWithPublishRetransmitCount:0 intervalSteps:0];
    }
    return self;
}

- (instancetype)initWithDestination:(UInt16)destination withKeyIndex:(UInt16)keyIndex friendshipCredentialsFlag:(int)friendshipCredentialsFlag ttl:(UInt8)ttl periodSteps:(UInt8)periodSteps periodResolution:(SigStepResolution)periodResolution retransmit:(SigRetransmit *)retransmit {
    self = [super init];
    if (self) {
        _address = [SigHelper.share getUint16String:destination];
        _index = keyIndex;
        _credentials = friendshipCredentialsFlag;
        _ttl = ttl;
        _periodSteps = periodSteps;
        _periodResolution = periodResolution;
        _period = [SigHelper.share getPeriodFromSteps:periodResolution];
        _retransmit = retransmit;
    }
    return self;
}

//- (SigPublish *)withAddress:(SigMeshAddress *)address {
//    return Publish(to: address.hex, withKeyIndex: index,
//                   friendshipCredentialsFlag: credentials, ttl: ttl,
//                   periodSteps: periodSteps, periodResolution: periodResolution,
//                   retransmit: retransmit)
//}

//- (SigMeshAddress *)publicationAddress {
//    // Warning: assuming hex address is valid!
//    return MeshAddress(hex: address)!
//}

- (NSTimeInterval)publicationInterval {
    return (NSTimeInterval)_period / 1000.0;
}

- (BOOL)isUsingMasterSecurityMaterial {
    return _credentials == 0;
}

- (BOOL)isUsingFriendshipSecurityMaterial {
    return _credentials == 1;
}

- (SigMeshAddress *)publicationAddress {
    return [[SigMeshAddress alloc] initWithHex:_address];
}

@end
