/********************************************************************************************************
 * @file     SigProvisioningData.h
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
//  SigProvisioningData.h
//  SigMeshLib
//
//  Created by Liangjiazhi on 2019/8/22.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class SigDataSource,SigNetkeyModel,SigIvIndex;

UIKIT_EXTERN NSString *const sessionKeyOfCalculateKeys;
UIKIT_EXTERN NSString *const sessionNonceOfCalculateKeys;
UIKIT_EXTERN NSString *const deviceKeyOfCalculateKeys;

@interface SigProvisioningData : NSObject

@property (nonatomic, strong) SigNetkeyModel *networkKey;
@property (nonatomic, strong) SigIvIndex *ivIndex;
@property (nonatomic, assign) UInt16 unicastAddress;
@property (nonatomic, strong) NSData *deviceKey;
@property (nonatomic, strong) NSData *provisionerRandom;
@property (nonatomic, strong) NSData *provisionerPublicKey;
@property (nonatomic, strong) NSData *sharedSecret;

- (instancetype)initWithAlgorithm:(Algorithm)algorithm;

- (void)prepareWithNetwork:(SigDataSource *)network networkKey:(SigNetkeyModel *)networkKey unicastAddress:(UInt16)unicastAddress;

- (void)accumulatePduData:(NSData *)data;

- (void)provisionerDidObtainWithDevicePublicKey:(NSData *)data;

- (void)provisionerDidObtainAuthValue:(NSData *)data;

- (void)provisionerDidObtainWithDeviceConfirmation:(NSData *)data;

- (void)provisionerDidObtainWithDeviceRandom:(NSData *)data;

- (BOOL)validateConfirmation;

- (NSData *)provisionerConfirmation;

- (NSData *)encryptedProvisioningDataWithMic;

#pragma mark - Helper methods

- (void)generateProvisionerRandomAndProvisionerPublicKey;

@end

NS_ASSUME_NONNULL_END
