/********************************************************************************************************
 * @file     DeviceGroupViewController.m 
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
//  DeviceGroupViewController.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2018/10/10.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "DeviceGroupViewController.h"
#import "DeviceGroupListCell.h"

@interface DeviceGroupViewController ()<UITableViewDelegate,UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic,strong) NSMutableArray <NSNumber *>*source;
@end

@implementation DeviceGroupViewController

#pragma mark - Life method
- (void)normalSetting{
    [super normalSetting];
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];

    self.source = [[NSMutableArray alloc] init];
    if (self.model.getGroupIDs) {
        for (NSNumber *groupID in self.model.getGroupIDs) {
            [self.source addObject:groupID];
        }
    }
}

#pragma mark - UITableView
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return SigDataSource.share.groups.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    DeviceGroupListCell *cell = (DeviceGroupListCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_DeviceGroupListCellID forIndexPath:indexPath];
    SigGroupModel *gModel = SigDataSource.share.groups[indexPath.row];
    [cell contentWithGroupAddress:@(gModel.intAddress) groupName:gModel.name model:self.model];
    return cell;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 51.0;
}

-(void)dealloc{
    TeLog(@"");
}

@end
