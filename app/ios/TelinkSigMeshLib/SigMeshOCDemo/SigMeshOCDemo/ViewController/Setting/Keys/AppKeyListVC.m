/********************************************************************************************************
* @file     AppKeyListVC.m
*
* @brief    Show all AppKey of Mesh network.
*
* @author       Telink, 梁家誌
* @date         2020
*
* @par      Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd.
*           All rights reserved.
*
*           The information contained herein is confidential property of Telink
*           Semiconductor (Shanghai) Co., Ltd. and is available under the terms
*           of Commercial License Agreement between Telink Semiconductor (Shanghai)
*           Co., Ltd. and the licensee or the terms described here-in. This heading
*           MUST NOT be removed from this file.
*
*           Licensee shall not delete, modify or alter (or permit any third party to delete, modify, or
*           alter) any information contained herein in whole or in part except as expressly authorized
*           by Telink semiconductor (shanghai) Co., Ltd. Otherwise, licensee shall be solely responsible
*           for any claim to the extent arising out of or relating to such deletion(s), modification(s)
*           or alteration(s).
*
*           Licensees are granted free, non-transferable use of the information in this
*           file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
*
*******************************************************************************************************/
//
//  AppKeyListVC.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2020/9/17.
//  Copyright © 2020 Telink. All rights reserved.
//

#import "AppKeyListVC.h"
#import "KeyCell.h"
#import "UIButton+extension.h"
#import "UIViewController+Message.h"
#import "AppKeyAddVC.h"

@interface AppKeyListVC ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <SigAppkeyModel *>*sourceArray;

@end

@implementation AppKeyListVC

- (void)viewDidLoad {
    [super viewDidLoad];

    self.title = @"AppKey List";
    self.sourceArray = [NSMutableArray arrayWithArray:SigDataSource.share.appKeys];
    UIView *footerView = [[UIView alloc] initWithFrame:CGRectZero];
    self.tableView.tableFooterView = footerView;
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(KeyCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(KeyCell.class)];
    UIBarButtonItem *rightItem1 = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemAdd target:self action:@selector(clickAdd:)];
    self.navigationItem.rightBarButtonItem = rightItem1;
    //longpress to delete scene
    UILongPressGestureRecognizer *gesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(cellDidPress:)];
    [self.view addGestureRecognizer:gesture];
}

- (void)clickAdd:(UIButton *)button{
    [self addAppKeyByManualInput];
}

- (void)addAppKeyByManualInput {
    AppKeyAddVC *vc = [[AppKeyAddVC alloc] init];
    vc.isAdd = YES;
    SigAppkeyModel *appkey = [[SigAppkeyModel alloc] init];
    appkey.oldKey = @"00000000000000000000000000000000";
    appkey.key = [LibTools convertDataToHexStr:[LibTools initAppKey]];
    appkey.index = SigDataSource.share.appKeys.count;
    appkey.name = [NSString stringWithFormat:@"appkey%ld",(long)appkey.index];
    appkey.boundNetKey = SigDataSource.share.curNetkeyModel.index;
    vc.appKeyModel = appkey;
    __weak typeof(self) weakSelf = self;
    [vc setBackAppKeyModel:^(SigAppkeyModel * _Nonnull appKeyModel) {
        if (![SigDataSource.share.appKeys containsObject:appKeyModel]) {
            [SigDataSource.share.appKeys addObject:appKeyModel];
            [SigDataSource.share saveLocationData];
        }
        if (![weakSelf.sourceArray containsObject:appKeyModel]) {
            [weakSelf.sourceArray addObject:appKeyModel];
            [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
        }
    }];
    [self.navigationController pushViewController:vc animated:YES];
}

#pragma  mark LongPressGesture
- (void)cellDidPress:(UILongPressGestureRecognizer *)sender{
    if (sender.state == UIGestureRecognizerStateBegan) {
        NSIndexPath *indexPath = [self.tableView indexPathForRowAtPoint:[sender locationInView:self.tableView]];
        if (indexPath != nil) {
            if (SigDataSource.share.appKeys.count == 1) {
                [self showAlertSureWithTitle:@"Hits" message:@"The mesh network needs at least one appkey!" sure:nil];
                return;
            }

            SigAppkeyModel *model = self.sourceArray[indexPath.row];
            BOOL hadBound = NO;
            NSArray *temNodes = [NSArray arrayWithArray:SigDataSource.share.curNodes];
            for (SigNodeModel *node in temNodes) {
                if (node.appKeys && node.appKeys.count > 0) {
                    for (SigNodeKeyModel *nodeKey in node.appKeys) {
                        if (nodeKey.index == model.index) {
                            hadBound = YES;
                            break;
                        }
                    }
                    if (hadBound) {
                        break;
                    }
                }
            }
            TeLogDebug(@"%@",indexPath);
            if (hadBound) {
                [self showAlertSureWithTitle:@"Hits" message:@"Some nodes have already bound this appkey, you can`t delete it!" sure:nil];
                return;
            }

            NSString *msg = [NSString stringWithFormat:@"Are you sure delete appKey, index:0x%04lX key:%@",(long)model.index,model.key];
            __weak typeof(self) weakSelf = self;
            [self showAlertSureAndCancelWithTitle:@"Hits" message:msg sure:^(UIAlertAction *action) {
                [SigDataSource.share.appKeys removeObject:model];
                [SigDataSource.share saveLocationData];
                [weakSelf.sourceArray removeObject:model];
                [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
            } cancel:^(UIAlertAction *action) {
                
            }];
        }
    }
}

#pragma mark - UITableViewDataSource
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.sourceArray.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    KeyCell *cell = [tableView dequeueReusableCellWithIdentifier:NSStringFromClass(KeyCell.class) forIndexPath:indexPath];
    SigAppkeyModel *model = self.sourceArray[indexPath.row];
    [cell setAppKeyModel:model];
    __weak typeof(self) weakSelf = self;
    [cell.editButton addAction:^(UIButton *button) {        
        AppKeyAddVC *vc = [[AppKeyAddVC alloc] init];
        vc.isAdd = NO;
        [vc setAppKeyModel:model];
        [vc setBackAppKeyModel:^(SigAppkeyModel * _Nonnull appKeyModel) {
            [SigDataSource.share.appKeys replaceObjectAtIndex:indexPath.row withObject:appKeyModel];
            [SigDataSource.share saveLocationData];
            [weakSelf.sourceArray replaceObjectAtIndex:indexPath.row withObject:appKeyModel];
            [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
        }];
        [weakSelf.navigationController pushViewController:vc animated:YES];
    }];
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [tableView cellForRowAtIndexPath:indexPath];
    cell.selected = NO;
    SigAppkeyModel *model = self.sourceArray[indexPath.row];
    if (model != SigDataSource.share.curAppkeyModel) {
        NSString *msg = [NSString stringWithFormat:@"Are you sure change current appKey to index:0x%04lX key:%@",(long)model.index,model.key];
        __weak typeof(self) weakSelf = self;
        [self showAlertSureAndCancelWithTitle:@"Hits" message:msg sure:^(UIAlertAction *action) {
            SigDataSource.share.curAppkeyModel = model;
            [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
        } cancel:^(UIAlertAction *action) {
            
        }];
    }
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 55;
}

@end
