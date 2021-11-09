/********************************************************************************************************
 * @file     SigProvisioningData.h
 *
 * @brief    for TLSR chips
 *
 * @author       Telink, 梁家誌
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
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/8/22.
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

@property (nonatomic, strong) NSData *provisioningInvitePDUValue;
@property (nonatomic, strong) NSData *provisioningCapabilitiesPDUValue;
@property (nonatomic, strong) NSData *provisioningStartPDUValue;
@property (nonatomic, strong) NSData *devicePublicKey;

- (instancetype)initWithAlgorithm:(Algorithm)algorithm;

- (void)prepareWithNetwork:(SigDataSource *)network networkKey:(SigNetkeyModel *)networkKey unicastAddress:(UInt16)unicastAddress;

/// Call this method when the device Public Key has been obtained. This must be called after generating keys.
/// @param data The device Public Key.
- (void)provisionerDidObtainWithDevicePublicKey:(NSData *)data;

/// Call this method when the Auth Value has been obtained.
- (void)provisionerDidObtainAuthValue:(NSData *)data;

/// Call this method when the device Provisioning Confirmation has been obtained.
- (void)provisionerDidObtainWithDeviceConfirmation:(NSData *)data;

/// Call this method when the device Provisioning Random has been obtained.
- (void)provisionerDidObtainWithDeviceRandom:(NSData *)data;

/// This method validates the received Provisioning Confirmation and matches it with one calculated locally based on the Provisioning Random received from the device and Auth Value.
- (BOOL)validateConfirmation;

/// Returns the Provisioner Confirmation value. The Auth Value must be set prior to calling this method.
- (NSData *)provisionerConfirmation;

- (NSData *)getProvisioningData;

- (NSData *)getEncryptedProvisioningDataAndMicWithProvisioningData:(NSData *)provisioningData;

#pragma mark - Helper methods

- (void)generateProvisionerRandomAndProvisionerPublicKey;

@end

NS_ASSUME_NONNULL_END
