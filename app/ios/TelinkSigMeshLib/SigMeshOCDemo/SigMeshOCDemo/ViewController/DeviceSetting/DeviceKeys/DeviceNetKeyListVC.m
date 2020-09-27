/********************************************************************************************************
* @file     DeviceNetKeyListVC.m
*
* @brief    Show all NetKey of node.
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
//  DeviceNetKeyListVC.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2020/9/17.
//  Copyright © 2020 Telink. All rights reserved.
//

#import "DeviceNetKeyListVC.h"
#import "DeviceKeyCell.h"
#import "UIViewController+Message.h"
#import "DeviceChooseKeyVC.h"

@interface DeviceNetKeyListVC ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <SigNetkeyModel *>*sourceArray;

@end

@implementation DeviceNetKeyListVC

- (void)viewDidLoad {
    [super viewDidLoad];

    self.title = @"NetKey List";
    UIView *footerView = [[UIView alloc] initWithFrame:CGRectZero];
    self.tableView.tableFooterView = footerView;
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(DeviceKeyCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(DeviceKeyCell.class)];
    UIBarButtonItem *rightItem1 = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemAdd target:self action:@selector(clickAdd:)];
    UIBarButtonItem *rightItem2 = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh target:self action:@selector(clickRefresh:)];
    self.navigationItem.rightBarButtonItems = @[rightItem1,rightItem2];
    //longpress to delete scene
    UILongPressGestureRecognizer *gesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(cellDidPress:)];
    [self.view addGestureRecognizer:gesture];
    [self refreshUI];
}

- (void)clickAdd:(UIButton *)button {
    DeviceChooseKeyVC *vc = [[DeviceChooseKeyVC alloc] init];
    __weak typeof(self) weakSelf = self;
    [vc setModel:self.model];
    [vc setBackNetKeyModel:^(SigNetkeyModel * _Nonnull netKeyModel) {
        [weakSelf addNetKeyToDevice:netKeyModel];
    }];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)clickRefresh:(UIButton *)button {
    [ShowTipsHandle.share show:@"get node NetKey list..."];

    __weak typeof(self) weakSelf = self;
    [SDKLibCommand configNetKeyGetWithDestination:self.model.address retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigNetKeyList * _Nonnull responseMessage) {
        TeLogInfo(@"NetKeyGet responseMessage=%@,parameters=%@,source=0x%x,destination=0x%x",responseMessage,responseMessage.parameters,source,destination);
        NSMutableArray *backList = [NSMutableArray array];
        for (NSNumber *number in responseMessage.networkKeyIndexs) {
            SigNodeKeyModel *nodeKeyModel = [[SigNodeKeyModel alloc] initWithIndex:number.intValue updated:NO];
            [backList addObject:nodeKeyModel];
        }
        weakSelf.model.netKeys = backList;
        [SigDataSource.share saveLocationData];
        [weakSelf performSelectorOnMainThread:@selector(refreshUI) withObject:nil waitUntilDone:YES];
        [ShowTipsHandle.share show:@"get node NetKey list success!"];
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        if (error) {
            [ShowTipsHandle.share show:[NSString stringWithFormat:@"get node NetKey list fail! error=%@",error]];
        }
        [ShowTipsHandle.share delayHidden:2.0];
    }];
}

- (void)addNetKeyToDevice:(SigNetkeyModel *)netKey {
    [ShowTipsHandle.share show:@"add NetKey to node..."];

    __weak typeof(self) weakSelf = self;
    [SDKLibCommand configNetKeyAddWithDestination:self.model.address NetworkKeyIndex:netKey.index networkKeyData:[LibTools nsstringToHex:netKey.key] retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigNetKeyStatus * _Nonnull responseMessage) {
        TeLogInfo(@"NetKeyAdd responseMessage=%@,parameters=%@,source=0x%x,destination=0x%x",responseMessage,responseMessage.parameters,source,destination);
        if (responseMessage.status == SigConfigMessageStatus_success) {
            SigNodeKeyModel *nodeKeyModel = [[SigNodeKeyModel alloc] initWithIndex:netKey.index updated:NO];
            if (![weakSelf.model.netKeys containsObject:nodeKeyModel]) {
                [weakSelf.model.netKeys addObject:nodeKeyModel];
            }
            [SigDataSource.share saveLocationData];
            [weakSelf performSelectorOnMainThread:@selector(refreshUI) withObject:nil waitUntilDone:YES];
            [ShowTipsHandle.share show:@"add NetKey to node success!"];
        } else {
            [ShowTipsHandle.share show:[NSString stringWithFormat:@"add NetKey to node fail! error status=%@",responseMessage.message]];
        }

    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        if (error) {
            [ShowTipsHandle.share show:[NSString stringWithFormat:@"add NetKey to node fail! error=%@",error]];
        }
        [ShowTipsHandle.share delayHidden:2.0];
    }];
}

- (void)deleteNetKeyOfDevice:(SigNetkeyModel *)netKey {
    [ShowTipsHandle.share show:@"delete NetKey from node..."];

    __weak typeof(self) weakSelf = self;
    [SDKLibCommand configNetKeyDeleteWithDestination:self.model.address NetworkKeyIndex:netKey.index retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigNetKeyStatus * _Nonnull responseMessage) {
        TeLogInfo(@"NetKeyDelete responseMessage=%@,parameters=%@,source=0x%x,destination=0x%x",responseMessage,responseMessage.parameters,source,destination);
        if (responseMessage.status == SigConfigMessageStatus_success) {
            SigNodeKeyModel *nodeKeyModel = [[SigNodeKeyModel alloc] initWithIndex:netKey.index updated:NO];
            [weakSelf.model.netKeys removeObject:nodeKeyModel];
            [SigDataSource.share saveLocationData];
            [weakSelf performSelectorOnMainThread:@selector(refreshUI) withObject:nil waitUntilDone:YES];
            [ShowTipsHandle.share show:@"delete NetKey to node success!"];
        } else {
            [ShowTipsHandle.share show:[NSString stringWithFormat:@"delete NetKey from node fail! error status=%@",responseMessage.message]];
        }

    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        if (error) {
            [ShowTipsHandle.share show:[NSString stringWithFormat:@"delete NetKey from node fail! error=%@",error]];
        }
        [ShowTipsHandle.share delayHidden:2.0];
    }];
}

- (void)refreshUI {
    NSMutableArray *tem = [NSMutableArray array];
    for (SigNodeKeyModel *nodeKey in self.model.netKeys) {
        for (SigNetkeyModel *netKey in SigDataSource.share.netKeys) {
            if (nodeKey.index == netKey.index) {
                if (![tem containsObject:netKey]) {
                    [tem addObject:netKey];
                }
                break;
            }
        }
    }
    self.sourceArray = tem;
    [self.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
}

#pragma  mark LongPressGesture
- (void)cellDidPress:(UILongPressGestureRecognizer *)sender{
    if (sender.state == UIGestureRecognizerStateBegan) {
        if (self.model.netKeys.count == 1) {
            [self showAlertSureWithTitle:@"Hits" message:@"The node needs at least one netkey!" sure:nil];
            return;
        }
        if (!SigBearer.share.isOpen) {
            [self showAlertSureWithTitle:@"Hits" message:@"The mesh network is not online!" sure:nil];
            return;
        }
        
        NSIndexPath *indexPath = [self.tableView indexPathForRowAtPoint:[sender locationInView:self.tableView]];
        if (indexPath != nil) {
            SigNetkeyModel *model = self.sourceArray[indexPath.row];
            NSString *msg = [NSString stringWithFormat:@"Are you sure delete netKey, index:0x%04lX key:%@",(long)model.index,model.key];
            __weak typeof(self) weakSelf = self;
            [self showAlertSureAndCancelWithTitle:@"Hits" message:msg sure:^(UIAlertAction *action) {
                [weakSelf deleteNetKeyOfDevice:model];
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
    DeviceKeyCell *cell = [tableView dequeueReusableCellWithIdentifier:NSStringFromClass(DeviceKeyCell.class) forIndexPath:indexPath];
    [cell setNetKeyModel:self.sourceArray[indexPath.row]];
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [tableView cellForRowAtIndexPath:indexPath];
    cell.selected = NO;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 55;
}

@end
