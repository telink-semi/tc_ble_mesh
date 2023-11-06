/********************************************************************************************************
 * @file     AddForwardingTableVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2022/3/18
 *
 * @par     Copyright (c) [2022], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#import "BridgeTableItemCell.h"
#import "BRStringPickerView.h"
#import "CustomAlertView.h"
#import "ChooseEntryVC.h"

@interface AddForwardingTableVC ()<UITableViewDelegate,UITableViewDataSource,UITextFieldDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIButton *addButton;
@property (nonatomic, strong) SigForwardingTableModel *forwardingTableModel;
@property (nonatomic, assign) NSInteger selectRow;
@property (nonatomic, strong) NSArray *titleArr;
@property (nonatomic, strong) NSMutableArray *netkeyArr;
@end

@implementation AddForwardingTableVC

- (IBAction)addForwardingTable:(UIButton *)sender {
    //逻辑：一次性添加多个设备的table，第一个设备添加table成功后就添加table到SIGDataSource里面，中途出现添加失败，则跳过当前设备继续添加下一个，且不保存失败的设备到table里面，最后再提示部分成功、部分失败等情况，客户可以点击重试。
    TeLogDebug(@"");
    if (SigBearer.share.isOpen) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [ShowTipsHandle.share show:self.isNew ? Tip_AddFrowardingTable : Tip_EditFrowardingTable];
        });
        __weak typeof(self) weakSelf = self;
        __block BOOL hasFail = NO;
        __block BOOL hasSuccess = NO;
        NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
        [operationQueue addOperationWithBlock:^{
            NSArray *addArray = [NSArray arrayWithArray:weakSelf.forwardingTableModel.entryNodeAddress];
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
                TeLogInfo(@"send request for forwardingTableDelete, address:%d", address);
                [SDKLibCommand forwardingTableDeleteWithNetKeyIndex:weakSelf.forwardingTableModel.netKeyIndex pathOrigin:weakSelf.forwardingTableModel.tableSource pathDestination:weakSelf.forwardingTableModel.tableDestination destination:address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigForwardingTableStatus * _Nonnull responseMessage) {
                    TeLogDebug(@"forwardingTableDelete=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                    if (responseMessage.status == SigConfigMessageStatus_success) {
                        hasSuccess = YES;
                    } else {
                        hasFail = YES;
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    dispatch_semaphore_signal(semaphore);
                }];
                dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 60.0));
            }
            
            // add table entry
            for (NSNumber *addressNumber in addArray) {
                UInt16 address = addressNumber.intValue;
                dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
                TeLogInfo(@"send request for forwardingTableAdd, address:%d", address);
                UInt16 bearerTowardPathOrigin = weakSelf.forwardingTableModel.bearerTowardPathOrigin;
                UInt16 bearerTowardPathTarget = weakSelf.forwardingTableModel.bearerTowardPathTarget;
                if (address == weakSelf.forwardingTableModel.tableSource) {
                    bearerTowardPathOrigin = 0;
                }
                if (address == weakSelf.forwardingTableModel.tableDestination) {
                    bearerTowardPathTarget = 0;
                }
                [SDKLibCommand forwardingTableAddWithNetKeyIndex:weakSelf.forwardingTableModel.netKeyIndex unicastDestinationFlag:weakSelf.forwardingTableModel.unicastDestinationFlag backwardPathValidatedFlag:weakSelf.forwardingTableModel.backwardPathValidatedFlag pathOriginUnicastAddrRange:weakSelf.forwardingTableModel.pathOriginUnicastAddrRange pathTargetUnicastAddrRange:weakSelf.forwardingTableModel.pathTargetUnicastAddrRange multicastDestination:weakSelf.forwardingTableModel.multicastDestination bearerTowardPathOrigin:bearerTowardPathOrigin bearerTowardPathTarget:bearerTowardPathTarget destination:address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigForwardingTableStatus * _Nonnull responseMessage) {
                    TeLogDebug(@"forwardingTableAdd=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                    if (responseMessage.status == SigConfigMessageStatus_success) {
                        hasSuccess = YES;
                    } else {
                        hasFail = YES;
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TeLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
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
                    [ShowTipsHandle.share delayHidden:2.0];
                    [weakSelf performSelector:@selector(pop) withObject:nil afterDelay:2.0];
                } else {
                    //不发送指令
                    [ShowTipsHandle.share show:@"not send command!"];
                    [ShowTipsHandle.share delayHidden:2.0];
                }
            });
        }];
    } else {
        [self showTips:[NSString stringWithFormat:@"The mesh is offline, app cann`t %@ forwarding table.", self.isNew ? @"add" : @"edit"]];
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
//        SigNodeModel *node1 = SigDataSource.share.curLocationNodeModel;
        SigNodeModel *node1 = SigDataSource.share.curNodes.firstObject;
        SigNodeModel *node2 = SigDataSource.share.curNodes.lastObject;
        SigUnicastAddressRangeFormatModel *formt1 = [[SigUnicastAddressRangeFormatModel alloc] initWithLengthPresent:node1.elements.count > 1 rangeStart:node1.address rangeLength:node1.elements.count];
        SigUnicastAddressRangeFormatModel *formt2 = [[SigUnicastAddressRangeFormatModel alloc] initWithLengthPresent:node2.elements.count > 1 rangeStart:node2.address rangeLength:node2.elements.count];
        self.forwardingTableModel = [[SigForwardingTableModel alloc] initWithNetKeyIndex:SigDataSource.share.curNetkeyModel.index unicastDestinationFlag:YES backwardPathValidatedFlag:NO pathOriginUnicastAddrRange:formt1 pathTargetUnicastAddrRange:formt2 multicastDestination:0x8000 bearerTowardPathOrigin:0x0001 bearerTowardPathTarget:0x0001];
    } else {
        self.title = @"Edit Forwarding Table";
        [self.addButton setTitle:@"Edit Forwarding Table" forState:UIControlStateNormal];
        self.forwardingTableModel = [[SigForwardingTableModel alloc] initWithOldSigForwardingTableModel:self.oldTable];
    }
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(BridgeTableItemCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(BridgeTableItemCell.class)];
    //iOS 15中 UITableView 新增了一个属性：sectionHeaderTopPadding。此属性会给每一个 section header 增加一个默认高度，当我们使用 UITableViewStylePlain 初始化UITableView 的时候，系统默认给 section header 增高了22像素。
    if(@available(iOS 15.0,*)) {
        self.tableView.sectionHeaderTopPadding = 0;
    }

    self.tableView.estimatedRowHeight = 50.0;
    self.netkeyArr = [NSMutableArray array];
    for (SigNetkeyModel *m in SigDataSource.share.netKeys) {
        [self.netkeyArr addObject:[NSString stringWithFormat:@"%@(0x%04X)",m.name,m.index]];
    }
    [self reloadUI];
}

- (void)reloadUI {
//    if (self.forwardingTableModel.unicastDestinationFlag) {
//        self.titleArr = @[@"NetKeyIndex",@"Unicast_Destination_Flag",@"Backward_Path_Validated_Flag",@"Origin_Range", @"Target_Range", @"Bearer_Toward_Path_Origin",@"Bearer_Toward_Path_Target", @"EntryNodeAddress"];
//    } else {
//        self.titleArr = @[@"NetKeyIndex",@"Unicast_Destination_Flag",@"Backward_Path_Validated_Flag",@"Origin_Range", @"Multicast_Destination",@"Bearer_Toward_Path_Origin",@"Bearer_Toward_Path_Target", @"EntryNodeAddress"];
//    }
    self.titleArr = @[@"NetKeyIndex",@"Backward_Path_Validated_Flag",@"Source",@"Destination",@"Bearer_Toward_Path_Origin",@"Bearer_Toward_Path_Target", @"EntryNodeAddress"];
    [self.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
}

-(void)dealloc{
    NSLog(@"%s",__func__);
}

#pragma mark - UITableView
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.titleArr.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    BridgeTableItemCell *cell = [tableView dequeueReusableCellWithIdentifier:NSStringFromClass(BridgeTableItemCell.class) forIndexPath:indexPath];
    cell.showLabel.text = self.titleArr[indexPath.row];
    cell.inputTF.delegate = self;
    cell.inputTF.tag = indexPath.row;
    cell.inputTF.hidden = NO;
    cell.accessoryType = UITableViewCellAccessoryNone;
    cell.selectionStyle = UITableViewCellSelectionStyleNone;
    switch (indexPath.row) {
        case 0:
        {
            // netKeyIndex
            cell.inputTF.text = [NSString stringWithFormat:@"%@(0x%04X)",[SigDataSource.share getNetkeyModelWithNetkeyIndex:self.forwardingTableModel.netKeyIndex].name, self.forwardingTableModel.netKeyIndex];
        }
            break;
        case 1:
        {
            // backward
            if (self.forwardingTableModel.backwardPathValidatedFlag) {
                cell.inputTF.text = @"True(1)";
            } else {
                cell.inputTF.text = @"False(0)";
            }
        }
            break;
        case 2:
        {
            // Source
//            cell.inputTF.text = [NSString stringWithFormat:@"0x%X", self.forwardingTableModel.tableSource];
            if (self.forwardingTableModel.tableSource == 0) {
                cell.showLabel.text = @"Source: NULL";
            } else {
                cell.showLabel.text = [NSString stringWithFormat:@"Source: 0x%X", self.forwardingTableModel.tableSource];
            }
            cell.inputTF.hidden = YES;
            cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
            cell.selectionStyle = UITableViewCellSelectionStyleDefault;
        }
            break;
        case 3:
        {
            // Destination
//            cell.inputTF.text = [NSString stringWithFormat:@"0x%X", self.forwardingTableModel.tableDestination];
            if (self.forwardingTableModel.tableDestination == 0) {
                cell.showLabel.text = @"Destination: NULL";
            } else {
                cell.showLabel.text = [NSString stringWithFormat:@"Destination: 0x%X", self.forwardingTableModel.tableDestination];
            }
            cell.inputTF.hidden = YES;
            cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
            cell.selectionStyle = UITableViewCellSelectionStyleDefault;
        }
            break;
        case 4:
        {
            // Bearer_Toward_Path_Origin
            cell.inputTF.placeholder = @"请输入";
            cell.inputTF.returnKeyType = UIReturnKeyDone;
            cell.inputTF.text = [NSString stringWithFormat:@"%04X",self.forwardingTableModel.bearerTowardPathOrigin];
        }
            break;
        case 5:
        {
            // Bearer_Toward_Path_Target
            cell.inputTF.placeholder = @"请输入";
            cell.inputTF.returnKeyType = UIReturnKeyDone;
            cell.inputTF.text = [NSString stringWithFormat:@"%04X",self.forwardingTableModel.bearerTowardPathTarget];
        }
            break;
        case 6:
        {
            // entry node address
            cell.showLabel.text = [NSString stringWithFormat:@"EntryNodeAddress: %@", self.forwardingTableModel.getEntryNodeAddressString];
            cell.inputTF.hidden = YES;
            cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
            cell.selectionStyle = UITableViewCellSelectionStyleDefault;
        }
            break;
        default:
            break;
    }
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [tableView cellForRowAtIndexPath:indexPath];
    cell.selected = NO;
    ChooseEntryVC *vc = (ChooseEntryVC *)[UIStoryboard initVC:ViewControllerIdentifiers_ChooseEntryVCID storyboard:@"Setting"];
    __weak typeof(self) weakSelf = self;
    if (indexPath.row == 2) {
        // Source
        vc.entryType = EntryType_originRange;
        if (self.forwardingTableModel.tableSource != 0) {
            vc.oldAddresses = [NSMutableArray arrayWithObject:@(self.forwardingTableModel.tableSource)];
        }
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
    } else if (indexPath.row == 3) {
        // Destination
        vc.entryType = EntryType_targetRange;
        if (self.forwardingTableModel.tableDestination != 0) {
            vc.oldAddresses = [NSMutableArray arrayWithObject:@(self.forwardingTableModel.tableDestination)];
        }
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
    } else if (indexPath.row == 6) {
        // entry node address
        vc.entryType = EntryType_tableEntry;
        vc.oldAddresses = [NSMutableArray arrayWithArray:self.forwardingTableModel.entryNodeAddress];
        [vc setBackAddressCallback:^(NSMutableArray<NSNumber *> * _Nonnull addresses) {
            weakSelf.forwardingTableModel.entryNodeAddress = [NSMutableArray arrayWithArray:addresses];
            [weakSelf reloadUI];
        }];
    }
    [self.navigationController pushViewController:vc animated:YES];
}

//- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {
//    return 50.0f;
//}

- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section {
    return 0.0001f;
}

- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section {
    return 0.0001f;
}

#pragma mark - UITextFieldDelegate 返回键
- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    if (textField.tag == 4 || textField.tag == 5) {
        [textField resignFirstResponder];
    }
    return YES;
}

#pragma mark - UITextFieldDelegate
- (BOOL)textFieldShouldBeginEditing:(UITextField *)textField {
    if (textField.tag == 4 || textField.tag == 5) {
        [textField addTarget:self action:@selector(handlerTextFieldEndEdit:) forControlEvents:UIControlEventEditingDidEnd];
        return YES; // 当前 textField 可以编辑
    } else {
        [self.view endEditing:YES];
        [self handlerTextFieldSelect:textField];
        return NO; // 当前 textField 不可编辑，可以响应点击事件
    }
}

#pragma mark - 处理编辑事件
- (void)handlerTextFieldEndEdit:(UITextField *)textField {
    switch (textField.tag) {
        case 4:
        {
            if ([self validateString:textField.text.removeAllSapceAndNewlines]) {
                self.forwardingTableModel.bearerTowardPathOrigin = [LibTools uint16From16String:textField.text];
            } else {
                [self showTips:@"Please enter the correct bearerTowardPathOrigin!"];
            }
        }
            break;
        case 5:
        {
            if ([self validateString:textField.text.removeAllSapceAndNewlines]) {
                self.forwardingTableModel.bearerTowardPathTarget = [LibTools uint16From16String:textField.text];
            } else {
                [self showTips:@"Please enter the correct bearerTowardPathTarget!"];
            }
        }
            break;
        default:
            break;
    }
}

- (BOOL)validateString:(NSString *)str{
    NSString *strRegex = @"^[0-9a-fA-F]{0,}$";
    NSPredicate *strPredicate = [NSPredicate predicateWithFormat:@"SELF MATCHES %@",strRegex];
    return [strPredicate evaluateWithObject:str];
}

#pragma mark - 处理点击事件
- (void)handlerTextFieldSelect:(UITextField *)textField {
    __weak typeof(self) weakSelf = self;
    switch (textField.tag) {
        case 0:
        {
            // NetKeyIndex
            BRStringPickerView *stringPickerView = [[BRStringPickerView alloc] init];
            stringPickerView.pickerMode = BRStringPickerComponentSingle;
            stringPickerView.title = @"please choose NetKeyIndex";
            stringPickerView.dataSourceArr = self.netkeyArr;
            SigNetkeyModel *selectModel = [SigDataSource.share getNetkeyModelWithNetkeyIndex:self.forwardingTableModel.netKeyIndex];
            stringPickerView.selectIndex = [SigDataSource.share.netKeys indexOfObject:selectModel];
            stringPickerView.pickerStyle.cancelBtnTitle = @"Cancel";
            stringPickerView.pickerStyle.doneBtnTitle = @"Done";
            stringPickerView.resultModelBlock = ^(BRResultModel *resultModel) {
                self.forwardingTableModel.netKeyIndex = SigDataSource.share.netKeys[resultModel.index].index;
                textField.text = resultModel.value;
            };
            [stringPickerView show];
        }
            break;
        case 1:
        {
            // Backward_Path_Validated_Flag
            BRStringPickerView *stringPickerView = [[BRStringPickerView alloc] init];
            stringPickerView.pickerMode = BRStringPickerComponentSingle;
            stringPickerView.title = @"please choose Backward_Path_Validated_Flag";
            stringPickerView.dataSourceArr = @[@"False(0x0)", @"True(0x1)"];
            stringPickerView.selectIndex = self.forwardingTableModel.backwardPathValidatedFlag == NO ? 0 : 1;
            stringPickerView.pickerStyle.cancelBtnTitle = @"Cancel";
            stringPickerView.pickerStyle.doneBtnTitle = @"Done";
            stringPickerView.resultModelBlock = ^(BRResultModel *resultModel) {
                weakSelf.forwardingTableModel.backwardPathValidatedFlag = resultModel.index == 0 ? NO : YES;
                textField.text = resultModel.value;
            };
            [stringPickerView show];
        }
            break;
        case 3:
        {
            // Path_Origin_Unicast_Addr_Range
            AlertItemModel *item1 = [[AlertItemModel alloc] init];
            item1.itemType = ItemType_Choose;
            item1.headerString = @"LengthPresent:";
            item1.defaultString = weakSelf.forwardingTableModel.pathOriginUnicastAddrRange.lengthPresent ? @"True" : @"False";
            NSMutableArray *mArray = [NSMutableArray array];
            [mArray addObject:@"True"];
            [mArray addObject:@"False"];
            item1.chooseItemsArray = mArray;
            AlertItemModel *item2 = [[AlertItemModel alloc] init];
            item2.itemType = ItemType_Input;
            item2.headerString = @"RangeStart(15 bits):0x";
            item2.defaultString = @"";
            AlertItemModel *item3 = [[AlertItemModel alloc] init];
            item3.itemType = ItemType_Input;
            item3.headerString = @"RangeLength(8 bits):0x";
            item3.defaultString = @"";
            CustomAlertView *customAlertView = [[CustomAlertView alloc] initWithTitle:@"Set Path_Origin_Unicast_Addr_Range" detail:@"input new value" itemArray:@[item1, item2, item3] leftBtnTitle:@"CANCEL" rightBtnTitle:@"CONFIRM" alertResult:^(CustomAlert * _Nonnull alertView, BOOL isConfirm) {
                if (isConfirm) {
                    //CONFIRM
                    NSString *rangeStartString = [alertView getTextFieldOfRow:1].text.removeAllSapce;
                    NSString *rangeLengthString = [alertView getTextFieldOfRow:2].text.removeAllSapce;
                    BOOL result = [LibTools validateHex:rangeStartString];
                    if (result == NO || rangeStartString.length == 0) {
                        [weakSelf showTips:@"Please enter hexadecimal string to RangeStart!"];
                        return;
                    }
                    result = [LibTools validateHex:rangeLengthString];
                    if (result == NO || rangeLengthString.length == 0) {
                        [weakSelf showTips:@"Please enter hexadecimal string to RangeLength steps!"];
                        return;
                    }
                    if (rangeStartString.length > 4) {
                        [weakSelf showTips:@"The length of RangeStart is 15 bits!"];
                        return;
                    }
                    if (rangeLengthString.length > 2) {
                        [weakSelf showTips:@"The length of RangeLength is 8 bits!"];
                        return;
                    }
                    int rangeStart = [LibTools uint16From16String:rangeStartString];
                    if (rangeStart <= 0 || rangeStart > 0x7FFF) {
                        [weakSelf showTips:@"The range of RangeStart is 0x0001~0x7FFF!"];
                        return;
                    }
                    int rangeLength = [LibTools uint8From16String:rangeLengthString];
                    if (rangeLength < 0x02 || rangeLength > 0xFF) {
                        [weakSelf showTips:@"The range of RangeLength is 0x02~0xFF!"];
                        return;
                    }
                    SigUnicastAddressRangeFormatModel *format = [[SigUnicastAddressRangeFormatModel alloc] init];
                    format.lengthPresent = [alertView getSelectLeftOfRow:0] ? YES : NO;
                    format.rangeStart = rangeStart;
                    if (format.lengthPresent) {
                        format.rangeLength = rangeLength;
                    } else {
                        format.rangeLength = 2;
                    }
                    weakSelf.forwardingTableModel.pathOriginUnicastAddrRange = format;
                    [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                } else {
                    //cancel
                }
            }];
            [customAlertView showCustomAlertView];
        }
            break;
        case 4:
        {
            // Path_Target_Unicast_Addr_Range
            AlertItemModel *item1 = [[AlertItemModel alloc] init];
            item1.itemType = ItemType_Choose;
            item1.headerString = @"LengthPresent:";
            item1.defaultString = weakSelf.forwardingTableModel.pathOriginUnicastAddrRange.lengthPresent ? @"True" : @"False";
            NSMutableArray *mArray = [NSMutableArray array];
            [mArray addObject:@"True"];
            [mArray addObject:@"False"];
            item1.chooseItemsArray = mArray;
            AlertItemModel *item2 = [[AlertItemModel alloc] init];
            item2.itemType = ItemType_Input;
            item2.headerString = @"RangeStart(15 bits):0x";
            item2.defaultString = @"";
            AlertItemModel *item3 = [[AlertItemModel alloc] init];
            item3.itemType = ItemType_Input;
            item3.headerString = @"RangeLength(8 bits):0x";
            item3.defaultString = @"";
            CustomAlertView *customAlertView = [[CustomAlertView alloc] initWithTitle:@"Set Path_Target_Unicast_Addr_Range" detail:@"input new value" itemArray:@[item1, item2, item3] leftBtnTitle:@"CANCEL" rightBtnTitle:@"CONFIRM" alertResult:^(CustomAlert * _Nonnull alertView, BOOL isConfirm) {
                if (isConfirm) {
                    //CONFIRM
                    NSString *rangeStartString = [alertView getTextFieldOfRow:1].text.removeAllSapce;
                    NSString *rangeLengthString = [alertView getTextFieldOfRow:2].text.removeAllSapce;
                    BOOL result = [LibTools validateHex:rangeStartString];
                    if (result == NO || rangeStartString.length == 0) {
                        [weakSelf showTips:@"Please enter hexadecimal string to RangeStart!"];
                        return;
                    }
                    result = [LibTools validateHex:rangeLengthString];
                    if (result == NO || rangeLengthString.length == 0) {
                        [weakSelf showTips:@"Please enter hexadecimal string to RangeLength steps!"];
                        return;
                    }
                    if (rangeStartString.length > 4) {
                        [weakSelf showTips:@"The length of RangeStart is 15 bits!"];
                        return;
                    }
                    if (rangeLengthString.length > 2) {
                        [weakSelf showTips:@"The length of RangeLength is 8 bits!"];
                        return;
                    }
                    int rangeStart = [LibTools uint16From16String:rangeStartString];
                    if (rangeStart <= 0 || rangeStart > 0x7FFF) {
                        [weakSelf showTips:@"The range of RangeStart is 0x0001~0x7FFF!"];
                        return;
                    }
                    int rangeLength = [LibTools uint8From16String:rangeLengthString];
                    if (rangeLength < 0x02 || rangeLength > 0xFF) {
                        [weakSelf showTips:@"The range of RangeLength is 0x02~0xFF!"];
                        return;
                    }
                    SigUnicastAddressRangeFormatModel *format = [[SigUnicastAddressRangeFormatModel alloc] init];
                    format.lengthPresent = [alertView getSelectLeftOfRow:0] ? YES : NO;
                    format.rangeStart = rangeStart;
                    if (format.lengthPresent) {
                        format.rangeLength = rangeLength;
                    } else {
                        format.rangeLength = 2;
                    }
                    weakSelf.forwardingTableModel.pathTargetUnicastAddrRange = format;
                    [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                } else {
                    //cancel
                }
            }];
            [customAlertView showCustomAlertView];
        }
            break;
        default:
            break;
    }
}

@end
