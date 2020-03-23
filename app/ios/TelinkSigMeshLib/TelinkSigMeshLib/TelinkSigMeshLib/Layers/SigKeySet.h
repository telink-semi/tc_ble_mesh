/********************************************************************************************************
* @file     SigKeySet.h
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
//  SigKeySet.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/9/28.
//  Copyright © 2019 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface SigKeySet : NSObject
/// The Network Key used to encrypt the message.
@property (nonatomic,strong) SigNetkeyModel *networkKey;
/// The Access Layer key used to encrypt the message.
@property (nonatomic,strong) NSData *accessKey;
/// Application Key identifier, or `nil` for Device Key.
@property (nonatomic,assign) UInt8 aid;
@end

@interface SigAccessKeySet : SigKeySet
@property (nonatomic,strong) SigAppkeyModel *applicationKey;
- (instancetype)initWithApplicationKey:(SigAppkeyModel *)applicationKey;
- (SigNetkeyModel *)networkKey;
- (NSData *)accessKey;
- (UInt8)aid;
@end

@interface SigDeviceKeySet : SigKeySet
@property (nonatomic,assign) BOOL isInitAid;
@property (nonatomic,strong) SigNodeModel *node;
- (NSData *)accessKey;
- (instancetype)initWithNetworkKey:(SigNetkeyModel *)networkKey node:(SigNodeModel *)node;
@end

NS_ASSUME_NONNULL_END
