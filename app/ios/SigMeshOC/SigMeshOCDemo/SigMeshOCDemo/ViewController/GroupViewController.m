/********************************************************************************************************
 * @file     GroupViewController.m 
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
//  GroupViewController.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "GroupViewController.h"
#import "GroupModel.h"
#import "GroupDetailViewController.h"
#import "GroupCell.h"

@interface GroupViewController()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <GroupModel *>*source;
@end

@implementation GroupViewController

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    GroupCell *cell = (GroupCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_GroupCellID forIndexPath:indexPath];
    [cell updateContent:self.source[indexPath.row]];
    return cell;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.source.count;
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = NO;
    [self updateData];
}

- (void)normalSetting{
    [super normalSetting];
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    self.source = [[NSMutableArray alloc] init];
    self.title = @"Group";
    UILongPressGestureRecognizer *gesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(cellDidPress:)];
    [self.view addGestureRecognizer:gesture];
}

- (void)updateData{
    [self.source removeAllObjects];
    for (int i=0; i<SigDataSource.share.groups.count; i++) {
        SigGroupModel *g = SigDataSource.share.groups[i];
        GroupModel *model = [[GroupModel alloc] init];
        model.groupAddress = [LibTools uint16From16String:g.address];
        model.groupName = g.name;
        [self.source addObject:model];
    }
    [self.tableView reloadData];
}

#pragma  mark LongPressGesture
- (void)cellDidPress:(UILongPressGestureRecognizer *)sender{
    if (sender.state == UIGestureRecognizerStateBegan) {
        NSIndexPath *indexPath = [self.tableView indexPathForRowAtPoint:[sender locationInView:self.tableView]];
        if (indexPath != nil) {
            GroupModel *model = self.source[indexPath.item];
            GroupDetailViewController *vc = (GroupDetailViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_GroupDetailViewControllerID];
            vc.model = model;
            [self.navigationController pushViewController:vc animated:YES];
        }
    }
}

@end
