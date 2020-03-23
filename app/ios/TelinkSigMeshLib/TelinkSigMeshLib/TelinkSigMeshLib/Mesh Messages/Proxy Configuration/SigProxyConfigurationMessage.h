//
//  SigProxyConfigurationMessage.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/10/30.
//  Copyright © 2019 梁家誌. All rights reserved.
//

#import <TelinkSigMeshLib/TelinkSigMeshLib.h>

NS_ASSUME_NONNULL_BEGIN

///more detail:  Mesh_v1.0.pdf section 6.4~6.7
@interface SigProxyConfigurationMessage : SigBaseMeshMessage

/// The message Op Code.
@property (nonatomic,assign) UInt8 opCode;

@end


/// A type of a Proxy Configuration message which opcode is known
/// during compilation time.
@interface SigStaticProxyConfigurationMessage : SigProxyConfigurationMessage
@end


/// The base class for acknowledged messages.
///
/// An acknowledged message is transmitted and acknowledged by each
/// receiving element by responding to that message. The response is
/// typically a status message. If a response is not received within
/// an arbitrary time period, the message will be retransmitted
/// automatically until the timeout occurs.
@interface SigAcknowledgedProxyConfigurationMessage : SigProxyConfigurationMessage
/// The Op Code of the response message.
@property (nonatomic,assign) UInt8 responseOpCode;
@end


@interface SigStaticAcknowledgedProxyConfigurationMessage : SigAcknowledgedProxyConfigurationMessage
/// The Type of the response message.
@property (nonatomic,strong) SigStaticProxyConfigurationMessage *responseType;
@end


@interface SigSetFilterType : SigStaticAcknowledgedProxyConfigurationMessage
/// The new filter type.
@property (nonatomic,assign) SigProxyFilerType filterType;

/// Creates a new Set Filter Type message.
///
/// This message can be used to set the proxy filter type and
/// clear the proxy filter list.
///
/// - parameter type: The new filter type. Setting the same
///                   filter type as was set before will clear
///                   the filter.
- (instancetype)initWithType:(SigProxyFilerType)type;

- (instancetype)initWithParameters:(NSData *)parameters;

@end


@interface SigAddAddressesToFilter : SigStaticAcknowledgedProxyConfigurationMessage
/// Arrays of addresses to be added to the proxy filter.
@property (nonatomic,strong) NSMutableArray <NSNumber *>*addresses;

/// Creates the Add Addresses To Filter message.
///
/// - parameter addresses: The array of addresses to be added
///                        to the current filter.
- (instancetype)initWithAddresses:(NSArray <NSNumber *>*)addresses;

- (instancetype)initWithParameters:(NSData *)parameters;

@end


@interface SigRemoveAddressesFromFilter : SigStaticAcknowledgedProxyConfigurationMessage
/// Arrays of addresses to be added to the proxy filter.
@property (nonatomic,strong) NSMutableArray <NSNumber *>*addresses;

/// Creates the Add Addresses To Filter message.
///
/// - parameter addresses: The array of addresses to be added
///                        to the current filter.
- (instancetype)initWithAddresses:(NSArray <NSNumber *>*)addresses;

- (instancetype)initWithParameters:(NSData *)parameters;

@end


@interface SigFilterStatus : SigStaticProxyConfigurationMessage
/// The new filter type.
@property (nonatomic,assign) SigProxyFilerType filterType;
/// Number of addresses in the proxy filter list.
@property (nonatomic,assign) UInt16 listSize;

/// Creates a new Filter Status message.
///
/// - parameter type: The current filter type.
/// - parameter listSize: Number of addresses in the proxy
///                       filter list.
- (instancetype)initWithType:(SigProxyFilerType)type listSize:(UInt16)listSize;

- (instancetype)initWithParameters:(NSData *)parameters;

@end


NS_ASSUME_NONNULL_END
