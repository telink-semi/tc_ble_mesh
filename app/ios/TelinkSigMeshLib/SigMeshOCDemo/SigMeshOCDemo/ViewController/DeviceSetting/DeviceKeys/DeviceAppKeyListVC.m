/********************************************************************************************************
 * @file     DeviceAppKeyListVC.m
 *
 * @brief    Show all AppKey of node.
 *
 * @author   Telink, 梁家誌
 * @date     2020/9/17
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/

#import "DeviceAppKeyListVC.h"
#import "UIButton+extension.h"
#import "UIViewController+Message.h"
#import "DeviceChooseKeyVC.h"
#import "AppKeyCell.h"

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
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(AppKeyCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(AppKeyCell.class)];
    //init rightBarButtonItems
    UIBarButtonItem *rightItem1 = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemAdd target:self action:@selector(clickAdd:)];
    UIBarButtonItem *rightItem2 = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh target:self action:@selector(clickRefresh:)];
    self.navigationItem.rightBarButtonItems = @[rightItem1,rightItem2];
    //longpress to delete scene
    UILongPressGestureRecognizer *gesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(cellDidPress:)];
    [self.view addGestureRecognizer:gesture];
    [self refreshUI];
}

- (void)clickAdd:(UIButton *)button {
    if (self.model.appKeys.count >= 2) {
        [self showTips:@"more than 2 app keys is not supported"];
        return;
    }

    DeviceChooseKeyVC *vc = [[DeviceChooseKeyVC alloc] init];
    __weak typeof(self) weakSelf = self;
    [vc setModel:self.model];
    [vc setBackAppKeyModel:^(SigAppkeyModel * _Nonnull appKeyModel) {
        [weakSelf keyBindWithAppKey:appKeyModel];
    }];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)clickRefresh:(UIButton *)button {
    [ShowTipsHandle.share show:@"get node AppKey list..."];

    NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
    __weak typeof(self) weakSelf = self;
    [operationQueue addOperationWithBlock:^{
        //这个block语句块在子线程中执行
        NSArray *temList = [NSArray arrayWithArray:self.model.netKeys];
        NSMutableArray *backList = [NSMutableArray array];

        for (SigNodeKeyModel *tem in temList) {
            dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
            [SDKLibCommand configAppKeyGetWithDestination:weakSelf.model.address networkKeyIndex:tem.index retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigAppKeyList * _Nonnull responseMessage) {
                TelinkLogInfo(@"AppKeyGet responseMessage=%@,parameters=%@,source=0x%x,destination=0x%x",responseMessage,responseMessage.parameters,source,destination);
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
        TelinkLogInfo(@"AppKeyAdd responseMessage=%@,parameters=%@,source=0x%x,destination=0x%x",responseMessage,responseMessage.parameters,source,destination);
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
        TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
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
        TelinkLogInfo(@"AppKeyDelete responseMessage=%@,parameters=%@,source=0x%x,destination=0x%x",responseMessage,responseMessage.parameters,source,destination);
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
        TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
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
                [tem addObject:netKey];
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
                [self showTips:@"The node needs at least one appkey!"];
                return;
            }
            if (!SigBearer.share.isOpen) {
                [self showTips:@"The mesh network is not online!"];
                return;
            }

            SigAppkeyModel *model = self.sourceArray[indexPath.row];
            if (model.index == SigDataSource.share.curAppkeyModel.index) {
                [self showTips:@"You cannot delete a app key in use!"];
                return;
            }
            __weak typeof(self) weakSelf = self;
            [self showAlertSureAndCancelWithTitle:kDefaultAlertTitle message:[NSString stringWithFormat:@"Are you sure delete appKey, index:0x%04lX key:%@",(long)model.index,model.key] sure:^(UIAlertAction *action) {
                [weakSelf deleteAppKeyOfDevice:model];
            } cancel:nil];
        }
    }
}

- (void)keyBindWithAppKey:(SigAppkeyModel *)appKey {
    __weak typeof(self) weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        [ShowTipsHandle.share show:@"add appkey and KeyBind..."];
    });
    NSNumber *type = [[NSUserDefaults standardUserDefaults] valueForKey:kKeyBindType];
    UInt8 keyBindType = type.integerValue;
    UInt16 productID = [LibTools uint16From16String:self.model.pid];
    DeviceTypeModel *deviceType = [SigDataSource.share getNodeInfoWithCID:kCompanyID PID:productID];
    NSData *cpsData = deviceType.defaultCompositionData.parameters;
    if (keyBindType == KeyBindType_Fast) {
        if (cpsData == nil || cpsData.length == 0) {
            keyBindType = KeyBindType_Normal;
        }
    }
    if (cpsData && cpsData.length > 0) {
        cpsData = [cpsData subdataWithRange:NSMakeRange(1, cpsData.length - 1)];
    }

    [SDKLibCommand keyBind:self.model.address appkeyModel:appKey keyBindType:keyBindType productID:productID cpsData:cpsData keyBindSuccess:^(NSString * _Nonnull identify, UInt16 address) {
        dispatch_async(dispatch_get_main_queue(), ^{
            SigNodeKeyModel *nodeKeyModel = [[SigNodeKeyModel alloc] initWithIndex:appKey.index updated:NO];
            if (![weakSelf.model.appKeys containsObject:nodeKeyModel]) {
                [weakSelf.model.appKeys addObject:nodeKeyModel];
            }
            [SigDataSource.share saveLocationData];
            [weakSelf performSelectorOnMainThread:@selector(refreshUI) withObject:nil waitUntilDone:YES];
            [ShowTipsHandle.share show:@"KeyBind success!"];
            [ShowTipsHandle.share delayHidden:2.0];
        });
    } fail:^(NSError * _Nonnull error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [ShowTipsHandle.share show:[NSString stringWithFormat:@"KeyBind fail! error=%@",error]];
            [ShowTipsHandle.share delayHidden:2.0];
        });
    }];
}

#pragma mark - UITableViewDataSource
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.sourceArray.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    AppKeyCell *cell = [tableView dequeueReusableCellWithIdentifier:NSStringFromClass(AppKeyCell.class) forIndexPath:indexPath];
    [cell setModel:self.sourceArray[indexPath.row]];
    cell.editButton.hidden = YES;
    return cell;
}

@end
