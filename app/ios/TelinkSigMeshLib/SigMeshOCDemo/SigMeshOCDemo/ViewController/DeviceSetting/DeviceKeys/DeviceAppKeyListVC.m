/********************************************************************************************************
* @file     DeviceAppKeyListVC.m
*
* @brief    Show all AppKey of node.
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
//  DeviceAppKeyListVC.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2020/9/17.
//  Copyright © 2020 Telink. All rights reserved.
//

#import "DeviceAppKeyListVC.h"
#import "KeyCell.h"
#import "UIButton+extension.h"
#import "UIViewController+Message.h"
#import "DeviceChooseKeyVC.h"

@interface DeviceAppKeyListVC ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <SigAppkeyModel *>*sourceArray;

@end

@implementation DeviceAppKeyListVC

- (void)viewDidLoad {
    [super viewDidLoad];

    self.title = @"AppKey List";
    UIView *footerView = [[UIView alloc] initWithFrame:CGRectZero];
    self.tableView.tableFooterView = footerView;
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(KeyCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(KeyCell.class)];
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
    [vc setBackAppKeyModel:^(SigAppkeyModel * _Nonnull appKeyModel) {
        [weakSelf addAppKeyToDevice:appKeyModel];
    }];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)clickRefresh:(UIButton *)button {
    [ShowTipsHandle.share show:@"get node AppKey list..."];

    NSOperationQueue *oprationQueue = [[NSOperationQueue alloc] init];
    __weak typeof(self) weakSelf = self;
    [oprationQueue addOperationWithBlock:^{
        //这个block语句块在子线程中执行
        NSArray *temList = [NSArray arrayWithArray:self.model.netKeys];
        NSMutableArray *backList = [NSMutableArray array];

        for (SigNodeKeyModel *tem in temList) {
            dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
            [SDKLibCommand configAppKeyGetWithDestination:weakSelf.model.address networkKeyIndex:tem.index retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigAppKeyList * _Nonnull responseMessage) {
                TeLogInfo(@"AppKeyGet responseMessage=%@,parameters=%@,source=0x%x,destination=0x%x",responseMessage,responseMessage.parameters,source,destination);
                if (responseMessage.networkKeyIndex == tem.index) {
                    if (responseMessage.status == SigConfigMessageStatus_success) {
                        for (NSNumber *number in responseMessage.applicationKeyIndexes) {
                            SigNodeKeyModel *nodeKeyModel = [[SigNodeKeyModel alloc] initWithIndex:number.intValue updated:NO];
                            if (![backList containsObject:nodeKeyModel]) {
                                [backList addObject:nodeKeyModel];
                                if (![weakSelf.model.appKeys containsObject:nodeKeyModel]) {
                                    weakSelf.model.appKeys = backList;
                                    [SigDataSource.share saveLocationData];
                                    [weakSelf performSelectorOnMainThread:@selector(refreshUI) withObject:nil waitUntilDone:YES];
                                }
                            }
                        }
                        dispatch_async(dispatch_get_main_queue(), ^{
                            [ShowTipsHandle.share show:@"get node AppKey list success!"];
                        });
                    } else {
                        dispatch_async(dispatch_get_main_queue(), ^{
                            [ShowTipsHandle.share show:[NSString stringWithFormat:@"get node AppKey list fail! error status=%d",responseMessage.status]];
                        });
                    }
                    dispatch_semaphore_signal(semaphore);
                }
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                if (error) {
                    dispatch_semaphore_signal(semaphore);
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [ShowTipsHandle.share show:[NSString stringWithFormat:@"get node AppKey list fail! error=%@",error]];
                    });
                }
            }];
            //Most provide 4 seconds to disconnect bluetooth connection
            dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 4.0));
            dispatch_async(dispatch_get_main_queue(), ^{
                [ShowTipsHandle.share delayHidden:2.0];
            });
        }
        weakSelf.model.appKeys = backList;
        [SigDataSource.share saveLocationData];
        [weakSelf performSelectorOnMainThread:@selector(refreshUI) withObject:nil waitUntilDone:YES];
    }];
}

- (void)addAppKeyToDevice:(SigAppkeyModel *)appKey {
    [ShowTipsHandle.share show:@"add AppKey to node..."];

    __weak typeof(self) weakSelf = self;
    [SDKLibCommand configAppKeyAddWithDestination:self.model.address appkeyModel:appKey retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigAppKeyStatus * _Nonnull responseMessage) {
        TeLogInfo(@"AppKeyAdd responseMessage=%@,parameters=%@,source=0x%x,destination=0x%x",responseMessage,responseMessage.parameters,source,destination);
        if (responseMessage.status == SigConfigMessageStatus_success) {
            SigNodeKeyModel *nodeKeyModel = [[SigNodeKeyModel alloc] initWithIndex:responseMessage.applicationKeyIndex updated:NO];
            if (![weakSelf.model.appKeys containsObject:nodeKeyModel]) {
                [weakSelf.model.appKeys addObject:nodeKeyModel];
            }
            [SigDataSource.share saveLocationData];
            [weakSelf performSelectorOnMainThread:@selector(refreshUI) withObject:nil waitUntilDone:YES];
            [ShowTipsHandle.share show:@"add AppKey to node success!"];
        } else {
            [ShowTipsHandle.share show:[NSString stringWithFormat:@"add AppKey to node fail! error status=%d",responseMessage.status]];
        }

    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        if (error) {
            [ShowTipsHandle.share show:[NSString stringWithFormat:@"add AppKey to node fail! error=%@",error]];
        }
        [ShowTipsHandle.share delayHidden:2.0];
    }];
}

- (void)deleteAppKeyOfDevice:(SigAppkeyModel *)appKey {
    [ShowTipsHandle.share show:@"delete AppKey from node..."];

    __weak typeof(self) weakSelf = self;
    [SDKLibCommand configAppKeyDeleteWithDestination:self.model.address appkeyModel:appKey retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigAppKeyStatus * _Nonnull responseMessage) {
        TeLogInfo(@"AppKeyDelete responseMessage=%@,parameters=%@,source=0x%x,destination=0x%x",responseMessage,responseMessage.parameters,source,destination);
        if (responseMessage.status == SigConfigMessageStatus_success) {
            SigNodeKeyModel *nodeKeyModel = [[SigNodeKeyModel alloc] initWithIndex:responseMessage.applicationKeyIndex updated:NO];
            [weakSelf.model.appKeys removeObject:nodeKeyModel];
            [SigDataSource.share saveLocationData];
            [weakSelf performSelectorOnMainThread:@selector(refreshUI) withObject:nil waitUntilDone:YES];
            [ShowTipsHandle.share show:@"delete AppKey to node success!"];
        } else {
            [ShowTipsHandle.share show:[NSString stringWithFormat:@"delete AppKey from node fail! error status=%d",responseMessage.status]];
        }

    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
        if (error) {
            [ShowTipsHandle.share show:[NSString stringWithFormat:@"delete AppKey from node fail! error=%@",error]];
        }
        [ShowTipsHandle.share delayHidden:2.0];
    }];
}

- (void)refreshUI {
    NSMutableArray *tem = [NSMutableArray array];
    for (SigNodeKeyModel *nodeKey in self.model.appKeys) {
        for (SigAppkeyModel *netKey in SigDataSource.share.appKeys) {
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
        NSIndexPath *indexPath = [self.tableView indexPathForRowAtPoint:[sender locationInView:self.tableView]];
        if (indexPath != nil) {
            if (self.model.appKeys.count == 1) {
                [self showAlertSureWithTitle:@"Hits" message:@"The node needs at least one appkey!" sure:nil];
                return;
            }
            if (!SigBearer.share.isOpen) {
                [self showAlertSureWithTitle:@"Hits" message:@"The mesh network is not online!" sure:nil];
                return;
            }

            SigAppkeyModel *model = self.sourceArray[indexPath.row];
            NSString *msg = [NSString stringWithFormat:@"Are you sure delete appKey, index:0x%04lX key:%@",(long)model.index,model.key];
            __weak typeof(self) weakSelf = self;
            [self showAlertSureAndCancelWithTitle:@"Hits" message:msg sure:^(UIAlertAction *action) {
                [weakSelf deleteAppKeyOfDevice:model];
            } cancel:^(UIAlertAction *action) {
                
            }];
        }
    }
}

- (void)keyBindWithAppKey:(SigAppkeyModel *)appKey {
    NSString *msg = [NSString stringWithFormat:@"Are you sure KeyBind appKey(compositionDataGet+appKeyAdd+modelAppBind*N) to index:0x%04lX key:%@",(long)appKey.index,appKey.key];
    [self showAlertSureAndCancelWithTitle:@"Hits" message:msg sure:^(UIAlertAction *action) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [ShowTipsHandle.share show:@"KeyBind..."];
        });
        NSNumber *type = [[NSUserDefaults standardUserDefaults] valueForKey:kKeyBindType];
        UInt8 keyBindType = type.integerValue;
        UInt16 productID = [LibTools uint16From16String:self.model.pid];
        DeviceTypeModel *deviceType = [SigDataSource.share getNodeInfoWithCID:kCompanyID PID:productID];
        NSData *cpsData = deviceType.defaultCompositionData.parameters;
        if (keyBindType == KeyBindTpye_Fast) {
            if (cpsData == nil || cpsData.length == 0) {
                keyBindType = KeyBindTpye_Normal;
            }
        }

        [SDKLibCommand keyBind:self.model.address appkeyModel:appKey keyBindType:keyBindType productID:productID cpsData:cpsData keyBindSuccess:^(NSString * _Nonnull identify, UInt16 address) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [ShowTipsHandle.share show:@"KeyBind success!"];
                [ShowTipsHandle.share delayHidden:3.0];
            });
        } fail:^(NSError * _Nonnull error) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [ShowTipsHandle.share show:[NSString stringWithFormat:@"KeyBind fail! error=%@",error]];
                [ShowTipsHandle.share delayHidden:3.0];
            });
        }];
    } cancel:^(UIAlertAction *action) {
        
    }];
}

#pragma mark - UITableViewDataSource
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.sourceArray.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    KeyCell *cell = [tableView dequeueReusableCellWithIdentifier:NSStringFromClass(KeyCell.class) forIndexPath:indexPath];
    SigAppkeyModel *model = self.sourceArray[indexPath.row];
    [cell setAppKeyModel:model];
    [cell.editButton setImage:[UIImage imageNamed:@"ic_keybind"] forState:UIControlStateNormal];
    __weak typeof(self) weakSelf = self;
    [cell.editButton addAction:^(UIButton *button) {
        [weakSelf keyBindWithAppKey:model];
    }];
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
