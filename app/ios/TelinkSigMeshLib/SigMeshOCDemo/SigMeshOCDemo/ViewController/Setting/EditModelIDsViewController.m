/********************************************************************************************************
 * @file     EditModelIDsViewController.m 
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
//  EditModelIDsViewController.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "EditModelIDsViewController.h"
#import "ChooseModelIDCell.h"

@interface EditModelIDsViewController()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <ModelIDModel *>*source;
@property (nonatomic, strong) NSMutableArray <NSNumber *>*allNumbers;
@property (nonatomic, strong) NSMutableArray <NSNumber *>*selectNumbers;
@end

@implementation EditModelIDsViewController

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    ChooseModelIDCell *cell = (ChooseModelIDCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_ChooseModelIDCellID forIndexPath:indexPath];
    ModelIDModel *model = self.source[indexPath.row];
    [cell updateContent:model];
    cell.selectButton.selected = [self.selectNumbers containsObject:@(model.sigModelID)];
    return cell;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.source.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 44.0;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    ModelIDModel *model = self.source[indexPath.row];
    if ([self.selectNumbers containsObject:@(model.sigModelID)]) {
        [self.selectNumbers removeObjectAtIndex:[self.selectNumbers indexOfObject:@(model.sigModelID)]];
    } else {
        [self.selectNumbers addObject:@(model.sigModelID)];
    }
    [self.tableView reloadData];
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)normalSetting{
    [super normalSetting];
    self.title = @"Edit";
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_ChooseModelIDCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_ChooseModelIDCellID];
    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithTitle:@"Save" style:UIBarButtonItemStylePlain target:self action:@selector(clickSave)];
    self.navigationItem.rightBarButtonItem = rightItem;
    
    ModelIDs *modelIDs = [[ModelIDs alloc] init];
    self.source = [[NSMutableArray alloc] initWithArray:modelIDs.modelIDs];
    self.selectNumbers = [[NSMutableArray alloc] initWithArray:SigDataSource.share.keyBindModelIDs];
    self.allNumbers = [[NSMutableArray alloc] init];
    NSArray *source = [NSArray arrayWithArray:self.source];
    for (ModelIDModel *model in source) {
        [self.allNumbers addObject:@(model.sigModelID)];
    }
}

- (void)clickSave{
    SigDataSource.share.keyBindModelIDs = self.selectNumbers;
    [SigDataSource.share saveLocationData];
    [ShowTipsHandle.share hidden];
    [self.navigationController popViewControllerAnimated:YES];
}

@end
