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
//  TelinkBlueDemo
//
//  Created by Arvin on 2018/4/24.
//  Copyright © 2018年 Green. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef void(^ProgressBlock)(NSInteger progress);
typedef void(^FinishBlock)(NSArray <NSNumber *>*successAddresses,NSArray <NSNumber *>*failAddresses);

@interface MeshOTAManager : NSObject

@property(nonatomic, strong) NSData *otaData;


+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));


+ (MeshOTAManager*)share;

///developer call this api to start mesh ota.
- (void)startMeshOTAWithLocationAddress:(int)locationAddress cid:(int)cid deviceAddresses:(NSArray <NSNumber *>*)deviceAddresses otaData:(NSData *)otaData progressHandle:(ProgressBlock)progressBlock finishHandle:(FinishBlock)finishBlock errorHandle:(ErrorBlock)errorBlock;

///stop meshOTA, developer needn't call this api but midway stop mesh ota procress.
- (void)stopMeshOTA;

- (BOOL)isMeshOTAing;

- (void)saveIsMeshOTAing:(BOOL)isMeshOTAing;

///api for LibHandle.m, needn't developer call it.
- (void)callBackProgressFromLib:(u8)progress;
- (void)callBackMeshOTASuccessAddress:(u16)address;

@end
