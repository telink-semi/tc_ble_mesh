/********************************************************************************************************
 * @file     SigEncryptionHelper.h
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
//  SigEncryptionHelper.h
//  TelinkSigMeshLib
//
//  Created by Liangjiazhi on 2019/8/22.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef void (^keyPair)(NSData *publicKey ,NSData *privateKey);

@interface SigEncryptionHelper : NSObject


+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));


+ (SigEncryptionHelper *)share;

- (void)eccInit;

///返回手机端64字节的ECC公钥
- (NSData *)getPublicKeyData;

- (NSData *)getSharedSecretWithDevicePublicKey:(NSData *)devicePublicKey;

- (void)getECCKeyPair:(keyPair)pair;

- (UInt8)aesAttDecryptionPacketOnlineStatusWithNetworkBeaconKey:(UInt8 *)key iv:(UInt8 *)iv ivLen:(UInt8)ivLen mic:(UInt8 *)mic micLen:(UInt8)micLen ps:(UInt8 *)ps psLen:(int)psLen;

@end

NS_ASSUME_NONNULL_END
