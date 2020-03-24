/********************************************************************************************************
* @file     SigPublish.h
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
//  SigPublish.h
//  TelinkSigMeshLib
//
//  Created by Liangjiazhi on 2019/9/11.
//  Copyright © 2019 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/// The object is used to describe the number of times a message is
/// published and the interval between retransmissions of the published
/// message.
@interface SigRetransmit : NSObject
/// Number of retransmissions for network messages.
/// The value is in range from 0 to 7, where 0 means no retransmissions.
@property (nonatomic,assign) UInt8 count;
/// The interval (in milliseconds) between retransmissions (50...3200 with step 50).
@property (nonatomic,assign) UInt16 interval;
/// Retransmission steps, from 0 to 31. Use `interval` to get the interval in ms.
- (UInt8)steps;
- (instancetype)initWithPublishRetransmitCount:(UInt8)publishRetransmitCount intervalSteps:(UInt8)intervalSteps;
@end


@interface SigPublish : NSObject

/// Publication address for the Model. It's 4 or 32-character long
/// hexadecimal string.
@property (nonatomic,strong) NSString *address;
/// Publication address for the model.
@property (nonatomic,strong) SigMeshAddress *publicationAddress;//Warning: assuming hex address is valid!
/// An Application Key index, indicating which Applicaiton Key to
/// use for the publication.
@property (nonatomic,assign) UInt16 index;
/// An integer from 0 to 127 that represents the Time To Live (TTL)
/// value for the outgoing publish message. 255 means default TTL value.
@property (nonatomic,assign) UInt8 ttl;
/// The interval (in milliseconds) between subsequent publications.
@property (nonatomic,assign) int period;
/// The number of steps, in range 0...63.
@property (nonatomic,assign) UInt8 periodSteps;
/// The resolution of the number of steps.
@property (nonatomic,assign) SigStepResolution periodResolution;
/// An integer 0 o 1 that represents whether master security
/// (0) materials or friendship security material (1) are used.
@property (nonatomic,assign) int credentials;
/// The object describes the number of times a message is published and the
/// interval between retransmissions of the published message.
@property (nonatomic,strong) SigRetransmit *retransmit;

- (instancetype)initWithStringDestination:(NSString *)stringDestination withKeyIndex:(UInt16)keyIndex friendshipCredentialsFlag:(int)friendshipCredentialsFlag ttl:(UInt8)ttl periodSteps:(UInt8)periodSteps periodResolution:(SigStepResolution)periodResolution retransmit:(SigRetransmit *)retransmit;

- (instancetype)initWithDestination:(UInt16)destination withKeyIndex:(UInt16)keyIndex friendshipCredentialsFlag:(int)friendshipCredentialsFlag ttl:(UInt8)ttl periodSteps:(UInt8)periodSteps periodResolution:(SigStepResolution)periodResolution retransmit:(SigRetransmit *)retransmit;

/// Creates a copy of the Publish object, but replaces the address
/// with the given one. This method should be used to fill the virtual
/// label after a ConfigModelPublicationStatus has been received.
//- (SigPublish *)withAddress:(SigMeshAddress *)address;

/// Publication address for the model.
//- (SigMeshAddress *)publicationAddress;
/// Returns the interval between subsequent publications in seconds.
- (NSTimeInterval)publicationInterval;
/// Returns whether master security materials are used.
- (BOOL)isUsingMasterSecurityMaterial;
/// Returns whether friendship security materials are used.
- (BOOL)isUsingFriendshipSecurityMaterial;

@end

NS_ASSUME_NONNULL_END
