/********************************************************************************************************
 * @file     BLEProperties.h 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
//
//  BLEProperties.h
//  SigMeshOC
//
//  Created by Liangjiazhi on 2018/7/12.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Model.h"

@class SDKLibCommand;

typedef enum : NSUInteger {
    StateNormal,
    StateAddDevice_provision,
    StateAddDevice_keyBind,
    StateOTA,
} State;

@interface BLEProperties : NSObject

@property (nonatomic,strong) CBCentralManager *manager;
@property (nonatomic,strong) BLEStore *store;
@property (nonatomic,strong) SDKLibCommand *commandHandle;

@property (nonatomic,strong) NSData *NetworkKey;
@property (nonatomic,assign) State state;

@end
