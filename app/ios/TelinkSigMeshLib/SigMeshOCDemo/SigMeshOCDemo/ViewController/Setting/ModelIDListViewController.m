/********************************************************************************************************
 * @file     ModelIDListViewController.m 
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
//  ModelIDListViewController.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2018/9/25.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "ModelIDListViewController.h"
#import "ModelIDCell.h"
#import "EditModelIDsViewController.h"

@interface ModelIDListViewController ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <ModelIDModel *>*source;
@end

@implementation ModelIDListViewController

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

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
    [self reloadSelectModelID];
}

- (void)normalSetting{
    [super normalSetting];
    self.title = @"ModelIDs";
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    self.source = [[NSMutableArray alloc] init];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_ModelIDCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_ModelIDCellID];
    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"tabbar_setting"] style:UIBarButtonItemStylePlain target:self action:@selector(clickEdit)];

    self.navigationItem.rightBarButtonItem = rightItem;
}

- (void)clickEdit{
    EditModelIDsViewController *vc = (EditModelIDsViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_EditModelIDsViewControllerID storybroad:@"Setting"];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)reloadSelectModelID{
    [self.source removeAllObjects];
    for (NSNumber *modelID in SigDataSource.share.keyBindModelIDs) {
        ModelIDModel *model = [SigDataSource.share getModelIDModel:modelID];
        if (model) {
            [self.source addObject:model];
        }
    }
    [self.tableView reloadData];
}

@end
