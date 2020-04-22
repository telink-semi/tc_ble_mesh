//
//  SigLowerTransportPdu.h
//  TelinkSigMeshLib
//
//  Created by Liangjiazhi on 2019/9/16.
//  Copyright © 2019 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface SigLowerTransportPdu : NSObject
/// Source Address.
@property (nonatomic,assign) UInt16 source;
/// Destination Address.
@property (nonatomic,assign) UInt16 destination;
/// The Network Key used to decode/encode the PDU.
@property (nonatomic,strong) SigNetkeyModel *networkKey;
@property (nonatomic,strong) SigIvIndex *ivIndex;
/// Message type.
@property (nonatomic,assign) SigLowerTransportPduType type;
/// The raw data of Lower Transport Layer PDU.
@property (nonatomic,strong) NSData *transportPdu;
/// The raw data of Upper Transport Layer PDU.
@property (nonatomic,strong) NSData *upperTransportPdu;

@end

NS_ASSUME_NONNULL_END
