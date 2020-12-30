/********************************************************************************************************
 * @file     MeshOTAManager.h
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
//  MeshOTAManager.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2018/4/24.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef void(^ProgressBlock)(NSInteger progress);
typedef void(^FinishBlock)(NSArray <NSNumber *>*successAddresses,NSArray <NSNumber *>*failAddresses);

typedef enum : UInt8 {
    SigMeshOTAProgressIdle                                = 0,
    SigMeshOTAProgressFirmwareDistributionStart           = 1,
    SigMeshOTAProgressSubscriptionAdd                     = 2,
    SigMeshOTAProgressFirmwareUpdateInformationGet        = 3,
    SigMeshOTAProgressFirmwareUpdateFirmwareMetadataCheck = 4,
    SigMeshOTAProgressFirmwareUpdateStart                 = 5,
    SigMeshOTAProgressBLOBTransferGet                     = 6,
    SigMeshOTAProgressBLOBInformationGet                  = 7,
    SigMeshOTAProgressBLOBTransferStart                   = 8,
    SigMeshOTAProgressBLOBBlockStart                      = 9,
    SigMeshOTAProgressBLOBChunkTransfer                   = 10,
    SigMeshOTAProgressBLOBBlockGet                        = 11,
    SigMeshOTAProgressFirmwareUpdateGet                   = 12,
    SigMeshOTAProgressFirmwareUpdateApply                 = 13,
    SigMeshOTAProgressFirmwareDistributionCancel          = 14,
} SigMeshOTAProgress;

@interface MeshOTAManager : NSObject
//==========config parameters for meshOTA R04 R04 ==========//
@property (nonatomic, assign) UInt16 distributionAppKeyIndex;//parameters for step1:firmwareDistributionStart
@property (nonatomic, assign) SigTransferModeState distributionTransferMode;//parameters for step1:firmwareDistributionStart and step8:BLOBTransferStart
@property (nonatomic, assign) BOOL updatePolicy;//parameters for step1:firmwareDistributionStart
/// Multicast address used in a firmware image distribution. Size is 16 bits or 128 bits.
@property (nonatomic,strong) NSData *distributionMulticastAddress;//parameters for step1:firmwareDistributionStart
@property (nonatomic, assign) UInt16 distributionFirmwareImageIndex;//parameters for step1:firmwareDistributionStart
@property (nonatomic, strong) NSData *incomingFirmwareMetadata;//parameters for step4:firmwareUpdateFirmwareMetadataCheck and step5:firmwareUpdateStart
@property (nonatomic, assign) UInt16 updateFirmwareImageIndex;//parameters for step4:firmwareUpdateFirmwareMetadataCheck and step5:firmwareUpdateStart
@property (nonatomic, assign) UInt8 updateTTL;//parameters for step1:firmwareDistributionStart and step5:firmwareUpdateStart
@property (nonatomic, assign) UInt16 updateTimeoutBase;//parameters for step1:firmwareDistributionStart and step5:firmwareUpdateStart
@property (nonatomic, assign) UInt64 updateBLOBID;//parameters for step5:firmwareUpdateStart and step8:BLOBTransferStart
@property (nonatomic, assign) UInt16 MTUSize;//parameters for step8:BLOBTransferStart
//==========config parameters for meshOTA R04 R04 ==========//



+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));


+ (MeshOTAManager *)share;

/// developer call this api to start mesh ota.
- (void)startMeshOTAWithLocationAddress:(int)locationAddress deviceAddresses:(NSArray <NSNumber *>*)deviceAddresses otaData:(NSData *)otaData incomingFirmwareMetadata:(NSData *)incomingFirmwareMetadata progressHandle:(ProgressBlock)progressBlock finishHandle:(FinishBlock)finishBlock errorHandle:(ErrorBlock)errorBlock;

/// stop meshOTA, developer needn't call this api but midway stop mesh ota procress.
- (void)stopMeshOTA;

/// 查询当前是否处在meshOTA
- (BOOL)isMeshOTAing;

- (void)saveIsMeshOTAing:(BOOL)isMeshOTAing;

@end
