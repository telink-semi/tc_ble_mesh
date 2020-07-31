/********************************************************************************************************
 * @file     DeviceSubscriptionListViewController.m 
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
//  DeviceSubscriptionListViewController.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/10/11.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "DeviceSubscriptionListViewController.h"
#import "ModelIDCell.h"

@interface DeviceSubscriptionListViewController ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <ModelIDModel *>*source;
@end

@implementation DeviceSubscriptionListViewController

- (void)normalSetting{
    [super normalSetting];
    self.title = @"Model Setting";
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    self.source = [[NSMutableArray alloc] init];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_ModelIDCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_ModelIDCellID];
    [self reloadSelectModelID];
}

- (void)reloadSelectModelID{
    NSArray *options = @[@(SIG_MD_G_ONOFF_S),@(SIG_MD_LIGHTNESS_S),@(SIG_MD_LIGHT_CTL_S),@(SIG_MD_LIGHT_CTL_TEMP_S),@(SIG_MD_LIGHT_HSL_S)];
    [self.source removeAllObjects];
    for (NSNumber *modelID in options) {
        ModelIDModel *model = [SigDataSource.share getModelIDModel:modelID];
        if (model) {
            [self.source addObject:model];
        }
    }
    [self.tableView reloadData];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    ModelIDCell *cell = (ModelIDCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_ModelIDCellID forIndexPath:indexPath];
    [cell updateContent:self.source[indexPath.row]];
    return cell;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.source.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 44.0;
}

@end
