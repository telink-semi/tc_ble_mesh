/********************************************************************************************************
 * @file     GroupModel.m 
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
//  GroupModel.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "GroupModel.h"

@implementation GroupModel

- (instancetype)init
{
    self = [super init];
    if (self) {
        _groupAddress = 0;
        _groupName = @"";
    }
    return self;
}

- (BOOL)isOn{
    BOOL tem = NO;
    for (SigNodeModel *model in self.groupDevices) {
        if (model.state == DeviceStateOn) {
            tem = YES;
            break;
        }
    }
    return tem;
}

- (NSMutableArray <SigNodeModel *>*)groupDevices{
    NSMutableArray *tem = [[NSMutableArray alloc] init];
    for (SigNodeModel *model in SigDataSource.share.curNodes) {
        if ([model.getGroupIDs containsObject:@(_groupAddress)]) {
            [tem addObject:model];
        }
    }
    return tem;
}

- (NSMutableArray <SigNodeModel *>*)groupOnlineDevices{
    NSMutableArray *tem = [[NSMutableArray alloc] init];
    for (SigNodeModel *model in SigDataSource.share.curNodes) {
        if ([model.getGroupIDs containsObject:@(_groupAddress)] && model.state != DeviceStateOutOfLine) {
            [tem addObject:model];
        }
    }
    return tem;
}
@end
