/********************************************************************************************************
 * @file     GroupModel.h 
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
//  GroupModel.h
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

@interface GroupModel : NSObject

@property (nonatomic,assign) UInt16 groupAddress;
@property (nonatomic,strong) NSString *groupName;

- (BOOL)isOn;
- (NSMutableArray <SigNodeModel *>*)groupDevices;
- (NSMutableArray <SigNodeModel *>*)groupOnlineDevices;
@end
