/********************************************************************************************************
 * @file     AddForwardingTableVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2022/3/18
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "AddForwardingTableVC.h"
#import "UIViewController+Message.h"
#import "NSString+extension.h"
#import "CustomAlertView.h"
#import "ChooseEntryVC.h"
#import "AddTableHeaderCell.h"
#import "TableNodeCell.h"
#import "UIButton+extension.h"

@interface AddForwardingTableVC ()<UITableViewDelegate,UITableViewDataSource,UITextFieldDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
/// Button layer used to set add button title..
@property (weak, nonatomic) IBOutlet UIButton *addButton;
@property (nonatomic, strong) SigForwardingTableModel *forwardingTableModel;
@property (nonatomic, strong) NSArray <SigNodeModel *>*source;

@end

@implementation AddForwardingTableVC

- (IBAction)addForwardingTable:(UIButton *)sender {
    if (self.forwardingTableModel.tableSource == 0) {
        [self showTips:@"Please select origin device."];
        return;
    }
    if (self.forwardingTableModel.tableDestination == 0) {
        [self showTips:@"Please select target device."];
        return;
    }
    if (self.forwardingTableModel.entryNodeAddress.count == 0) {
        [self showTips:@"Please select nodes on route."];
        return;
    }
    //编辑的table暂时未考虑查重
    if (self.isNew && [SigDataSource.share.forwardingTableModelList containsObject:self.forwardingTableModel]) {
        //已经存在了相同的起点终点的路径
        //the same origin and target already exists
        [self showTips:@"the same origin and target already exists."];
        return;
    }
    
    
    //逻辑：一次性添加多个设备的table，第一个设备添加table成功后就添加table到SIGDataSource里面，中途出现添加失败，则跳过当前设备继续添加下一个，且不保存失败的设备到table里面，最后再提示部分成功、部分失败等情况，客户可以点击重试。
    TelinkLogDebug(@"");
    if (SigBearer.share.isOpen) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [ShowTipsHandle.share show:self.isNew ? Tip_AddFrowardingTable : Tip_EditFrowardingTable];
        });
        __weak typeof(self) weakSelf = self;
        __block BOOL hasFail = NO;
        __block BOOL hasSuccess = NO;
        NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
        [operationQueue addOperationWithBlock:^{
            NSMutableArray *addArray = [NSMutableArray arrayWithArray:weakSelf.forwardingTableModel.entryNodeAddress];
            //补起点
            if (![addArray containsObject:@(weakSelf.forwardingTableModel.tableSource)]) {
                [addArray addObject:@(weakSelf.forwardingTableModel.tableSource)];
            }
            //补终点
            if (![addArray containsObject:@(weakSelf.forwardingTableModel.tableDestination)]) {
                [addArray addObject:@(weakSelf.forwardingTableModel.tableDestination)];
            }

            NSMutableArray *deleteArray = [NSMutableArray array];
            if (weakSelf.isNew == NO && weakSelf.oldTable) {
                for (NSNumber *addressNumber in weakSelf.oldTable.entryNodeAddress) {
                    if (![addArray containsObject:addressNumber]) {
                        [deleteArray addObject:addressNumber];
                    }
                }
            }

            // delete table entry
            for (NSNumber *addressNumber in deleteArray) {
                UInt16 address = addressNumber.intValue;
                dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
                TelinkLogInfo(@"send request for forwardingTableDelete, address:%d", address);
                [SDKLibCommand forwardingTableDeleteWithNetKeyIndex:weakSelf.forwardingTableModel.netKeyIndex pathOrigin:weakSelf.forwardingTableModel.tableSource pathDestination:weakSelf.forwardingTableModel.tableDestination destination:address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigForwardingTableStatus * _Nonnull responseMessage) {
                    TelinkLogDebug(@"forwardingTableDelete=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                    if (responseMessage.status == SigConfigMessageStatus_success) {
                        hasSuccess = YES;
                    } else {
                        hasFail = YES;
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    dispatch_semaphore_signal(semaphore);
                }];
                dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 60.0));
            }

            // add table entry
            for (NSNumber *addressNumber in addArray) {
                UInt16 address = addressNumber.intValue;
                dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
                TelinkLogInfo(@"send request for forwardingTableAdd, address:%d", address);
                UInt16 bearerTowardPathOrigin = weakSelf.forwardingTableModel.bearerTowardPathOrigin;
                UInt16 bearerTowardPathTarget = weakSelf.forwardingTableModel.bearerTowardPathTarget;
                if (address == weakSelf.forwardingTableModel.tableSource) {
                    bearerTowardPathOrigin = 0;
                }
                if (address == weakSelf.forwardingTableModel.tableDestination) {
                    bearerTowardPathTarget = 0;
                }
                [SDKLibCommand forwardingTableAddWithNetKeyIndex:weakSelf.forwardingTableModel.netKeyIndex unicastDestinationFlag:weakSelf.forwardingTableModel.unicastDestinationFlag backwardPathValidatedFlag:weakSelf.forwardingTableModel.backwardPathValidatedFlag pathOriginUnicastAddrRange:weakSelf.forwardingTableModel.pathOriginUnicastAddrRange pathTargetUnicastAddrRange:weakSelf.forwardingTableModel.pathTargetUnicastAddrRange multicastDestination:weakSelf.forwardingTableModel.multicastDestination bearerTowardPathOrigin:bearerTowardPathOrigin bearerTowardPathTarget:bearerTowardPathTarget destination:address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigForwardingTableStatus * _Nonnull responseMessage) {
                    TelinkLogDebug(@"forwardingTableAdd=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                    if (responseMessage.status == SigConfigMessageStatus_success) {
                        hasSuccess = YES;
                    } else {
                        hasFail = YES;
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    if (error) {
                        hasFail = YES;
                    }
                    dispatch_semaphore_signal(semaphore);
                }];
                dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 60.0));
            }
            if (hasSuccess) {
                // 存在成功，保存本地
                if (weakSelf.isNew) {
                    [SigDataSource.share.forwardingTableModelList addObject:weakSelf.forwardingTableModel];
                } else {
                    NSInteger index = [SigDataSource.share.forwardingTableModelList indexOfObject:weakSelf.oldTable];
                    [SigDataSource.share.forwardingTableModelList replaceObjectAtIndex:index withObject:weakSelf.forwardingTableModel];
                    weakSelf.oldTable = weakSelf.forwardingTableModel;
                    weakSelf.forwardingTableModel = [[SigForwardingTableModel alloc] initWithOldSigForwardingTableModel:weakSelf.oldTable];
                }
                [SigDataSource.share saveLocationData];
            }
            dispatch_async(dispatch_get_main_queue(), ^{
                if (hasFail) {
                    NSString *message = nil;
                    if (hasSuccess) {
                        // 部分成功部分失败
                        message = weakSelf.isNew ? Tip_AddFrowardingTableSomeFail : Tip_EditFrowardingTableSomeFail;
                    } else {
                        // 全部失败
                        message = weakSelf.isNew ? Tip_AddFrowardingTableAllFail : Tip_EditFrowardingTableAllFail;
                    }
                    [weakSelf showRetryAlertWithMessage:message];
                } else if (hasSuccess) {
                    // 成功
                    [ShowTipsHandle.share show:weakSelf.isNew ? Tip_AddFrowardingTableSuccess : Tip_EditFrowardingTableSuccess];
                    [ShowTipsHandle.share delayHidden:0.5];
                    [weakSelf performSelector:@selector(pop) withObject:nil afterDelay:2.0];
                } else {
                    //不发送指令
                    [ShowTipsHandle.share show:@"not send command!"];
                    [ShowTipsHandle.share delayHidden:0.5];
                }
            });
        }];
    } else {
        [self showTips:[NSString stringWithFormat:@"The mesh is offline, app can not %@ forwarding table.", self.isNew ? @"add" : @"edit"]];
    }
}

- (void)showRetryAlertWithMessage:(NSString *)message {
    [ShowTipsHandle.share hidden];
    UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"Tips" message:message preferredStyle:UIAlertControllerStyleAlert];
    __weak typeof(self) weakSelf = self;
    UIAlertAction *action = [UIAlertAction actionWithTitle:@"Retry" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        [weakSelf addForwardingTable:weakSelf.addButton];
    }];
    UIAlertAction *revoke = [UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:nil];
    [alert addAction:action];
    [alert addAction:revoke];
    [self presentViewController:alert animated:YES completion:nil];
}

- (void)pop {
    [self.navigationController popViewControllerAnimated:YES];
}

- (BOOL)validateInput {



    return YES;
}

#pragma mark - Life method
- (void)normalSetting{
    [super normalSetting];
    self.addButton.backgroundColor = UIColor.telinkButtonBlue;
    if (self.isNew) {
        self.title = @"Add Forwarding Table";
        [self.addButton setTitle:@"Add Forwarding Table" forState:UIControlStateNormal];
//        SigNodeModel *node1 = SigDataSource.share.curNodes.firstObject;
//        SigNodeModel *node2 = SigDataSource.share.curNodes.lastObject;
//        SigUnicastAddressRangeFormatModel *format1 = [[SigUnicastAddressRangeFormatModel alloc] initWithLengthPresent:node1.elements.count > 1 rangeStart:node1.address rangeLength:node1.elements.count];
//        SigUnicastAddressRangeFormatModel *format2 = [[SigUnicastAddressRangeFormatModel alloc] initWithLengthPresent:node2.elements.count > 1 rangeStart:node2.address rangeLength:node2.elements.count];
        self.forwardingTableModel = [[SigForwardingTableModel alloc] initWithNetKeyIndex:SigDataSource.share.curNetkeyModel.index unicastDestinationFlag:YES backwardPathValidatedFlag:YES pathOriginUnicastAddrRange:nil pathTargetUnicastAddrRange:nil multicastDestination:0 bearerTowardPathOrigin:0x0001 bearerTowardPathTarget:0x0001];
    } else {
        self.title = @"Edit Forwarding Table";
        [self.addButton setTitle:@"Edit Forwarding Table" forState:UIControlStateNormal];
        self.forwardingTableModel = [[SigForwardingTableModel alloc] initWithOldSigForwardingTableModel:self.oldTable];
    }
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(AddTableHeaderCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(AddTableHeaderCell.class)];
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(TableNodeCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(TableNodeCell.class)];
    //iOS 15中 UITableView 新增了一个属性：sectionHeaderTopPadding。此属性会给每一个 section header 增加一个默认高度，当我们使用 UITableViewStylePlain 初始化UITableView 的时候，系统默认给 section header 增高了22像素。
    if(@available(iOS 15.0,*)) {
        self.tableView.sectionHeaderTopPadding = 0;
    }

    self.tableView.estimatedRowHeight = 50.0;
    [self reloadUI];
}

- (void)reloadUI {
    NSMutableArray *mArray = [NSMutableArray array];
    for (NSNumber *number in self.forwardingTableModel.entryNodeAddress) {
        SigNodeModel *node = [SigDataSource.share getNodeWithAddress:number.intValue];
        if (node) {
            [mArray addObject:node];
        }
    }
    self.source = mArray;
    [self.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
}

-(void)dealloc{
    NSLog(@"%s",__func__);
}

#pragma mark - UITableView
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.source.count+1;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.row == 0) {
        AddTableHeaderCell *cell = [tableView dequeueReusableCellWithIdentifier:NSStringFromClass(AddTableHeaderCell.class) forIndexPath:indexPath];
        __weak typeof(self) weakSelf = self;
        if (self.forwardingTableModel.tableSource == 0) {
            cell.originDeviceImageView.hidden = YES;
            cell.originDeviceNameLabel.hidden = YES;
        } else {
            cell.originDeviceImageView.hidden = NO;
            cell.originDeviceNameLabel.hidden = NO;
            cell.originDeviceImageView.image = [DemoTool getNodeStateImageWithUnicastAddress:self.forwardingTableModel.tableSource];
            SigNodeModel *node = [SigDataSource.share getNodeWithAddress:self.forwardingTableModel.tableSource];
            cell.originDeviceNameLabel.text = [NSString stringWithFormat:@"Name-%@\nAdr-0x%04X", node.name, self.forwardingTableModel.tableSource];
        }
        if (self.forwardingTableModel.tableDestination == 0) {
            cell.targetDeviceImageView.hidden = YES;
            cell.targetDeviceNameLabel.hidden = YES;
        } else {
            cell.targetDeviceImageView.hidden = NO;
            cell.targetDeviceNameLabel.hidden = NO;
            cell.targetDeviceImageView.image = [DemoTool getNodeStateImageWithUnicastAddress:self.forwardingTableModel.tableDestination];
            SigNodeModel *node = [SigDataSource.share getNodeWithAddress:self.forwardingTableModel.tableDestination];
            cell.targetDeviceNameLabel.text = [NSString stringWithFormat:@"Name-%@\nAdr-0x%04X", node.name, self.forwardingTableModel.tableDestination];
        }
        [cell.selectOriginButton addAction:^(UIButton *button) {
            [weakSelf pushToSelectOriginDeviceVC];
        }];
        [cell.selectTargetButton addAction:^(UIButton *button) {
            [weakSelf pushToSelectTargetDeviceVC];
        }];
        [cell.selectRouteButton addAction:^(UIButton *button) {
            [weakSelf pushToSelectRouteDeviceVC];
        }];
        return cell;
    }
    TableNodeCell *cell = [tableView dequeueReusableCellWithIdentifier:NSStringFromClass(TableNodeCell.class) forIndexPath:indexPath];
    SigNodeModel *model = self.source[indexPath.row-1];
    cell.model = model;
    return cell;
}

- (void)pushToSelectOriginDeviceVC {
    // Source
    ChooseEntryVC *vc = (ChooseEntryVC *)[UIStoryboard initVC:ViewControllerIdentifiers_ChooseEntryVCID storyboard:@"Setting"];
    vc.entryType = EntryType_originRange;
    if (self.forwardingTableModel.tableSource != 0) {
        vc.oldAddresses = [NSMutableArray arrayWithObject:@(self.forwardingTableModel.tableSource)];
    }
    __weak typeof(self) weakSelf = self;
    [vc setBackAddressCallback:^(NSMutableArray<NSNumber *> * _Nonnull addresses) {
        if (addresses.count > 1) {
            return;
        } else if (addresses == nil || addresses.count == 0) {
            weakSelf.forwardingTableModel.pathOriginUnicastAddrRange = nil;
        } else {
            UInt16 address = addresses.firstObject.intValue;
            SigNodeModel *model = [SigDataSource.share getNodeWithAddress:address];
            if (model) {
                SigUnicastAddressRangeFormatModel *range = [[SigUnicastAddressRangeFormatModel alloc] initWithLengthPresent:model.elements.count > 1 rangeStart:address rangeLength:model.elements.count];
                weakSelf.forwardingTableModel.pathOriginUnicastAddrRange = range;
            }
        }
        [weakSelf reloadUI];
    }];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)pushToSelectTargetDeviceVC {
    // Destination
    ChooseEntryVC *vc = (ChooseEntryVC *)[UIStoryboard initVC:ViewControllerIdentifiers_ChooseEntryVCID storyboard:@"Setting"];
    vc.entryType = EntryType_targetRange;
    if (self.forwardingTableModel.tableDestination != 0) {
        vc.oldAddresses = [NSMutableArray arrayWithObject:@(self.forwardingTableModel.tableDestination)];
    }
    __weak typeof(self) weakSelf = self;
    [vc setBackAddressCallback:^(NSMutableArray<NSNumber *> * _Nonnull addresses) {
        if (addresses.count > 1) {
            return;
        } else if (addresses == nil || addresses.count == 0) {
            weakSelf.forwardingTableModel.unicastDestinationFlag = YES;
            weakSelf.forwardingTableModel.pathTargetUnicastAddrRange = nil;
        } else {
            UInt16 address = addresses.firstObject.intValue;
            SigNodeModel *model = [SigDataSource.share getNodeWithAddress:address];
            if (model) {
                weakSelf.forwardingTableModel.unicastDestinationFlag = YES;
                SigUnicastAddressRangeFormatModel *range = [[SigUnicastAddressRangeFormatModel alloc] initWithLengthPresent:model.elements.count > 1 rangeStart:address rangeLength:model.elements.count];
                weakSelf.forwardingTableModel.pathTargetUnicastAddrRange = range;
            } else {
                weakSelf.forwardingTableModel.unicastDestinationFlag = NO;
                weakSelf.forwardingTableModel.multicastDestination = address;
            }
        }
        [weakSelf reloadUI];
    }];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)pushToSelectRouteDeviceVC  {
    // entry node address
    ChooseEntryVC *vc = (ChooseEntryVC *)[UIStoryboard initVC:ViewControllerIdentifiers_ChooseEntryVCID storyboard:@"Setting"];
    vc.entryType = EntryType_tableEntry;
    vc.oldAddresses = [NSMutableArray arrayWithArray:self.forwardingTableModel.entryNodeAddress];
    __weak typeof(self) weakSelf = self;
    [vc setBackAddressCallback:^(NSMutableArray<NSNumber *> * _Nonnull addresses) {
        weakSelf.forwardingTableModel.entryNodeAddress = [NSMutableArray arrayWithArray:addresses];
        [weakSelf reloadUI];
    }];
    [self.navigationController pushViewController:vc animated:YES];
}

@end
