/********************************************************************************************************
 * @file     AddBridgeTableVC.m
 *
 * @brief    Add Bridge Table to node.
 *
 * @author   Telink, 梁家誌
 * @date     2021/1/25
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

#import "AddBridgeTableVC.h"
#import "UIViewController+Message.h"
#import "NSString+extension.h"
#import "BridgeTableItemCell.h"
#import "BRStringPickerView.h"

@interface AddBridgeTableVC ()<UITableViewDelegate,UITableViewDataSource,UITextFieldDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIButton *addBridgingTableButton;
@property (nonatomic, strong) SigSubnetBridgeModel *bridgeModel;
@property (nonatomic, strong) SigMessageHandle *messageHandle;
@property (nonatomic, assign) NSInteger selectRow;
@property (nonatomic, strong) NSArray *titleArr;
@property (nonatomic, strong) NSMutableArray *netkeyArr;

@end

@implementation AddBridgeTableVC

- (IBAction)addBridgingTable:(UIButton *)sender {
    TelinkLogDebug(@"");
    if (SigBearer.share.isOpen) {
        if (self.model.state != DeviceStateOutOfLine) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(addSubnetBridgeTimeOut) object:nil];
                [self performSelector:@selector(addSubnetBridgeTimeOut) withObject:nil afterDelay:10.0];
                [ShowTipsHandle.share show:Tip_AddSubnetBridge];
            });
            __weak typeof(self) weakSelf = self;
            __block BOOL hasFail = NO;
            _messageHandle = [SDKLibCommand bridgeTableAddWithDestination:self.model.address subnetBridge:self.bridgeModel retryCount:2 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigBridgeTableStatus * _Nonnull responseMessage) {
                TelinkLogDebug(@"subnetBridgeSet=%@,source=%d,destination=%d",[LibTools convertDataToHexStr:responseMessage.parameters],source,destination);
                if (responseMessage.status == SigConfigMessageStatus_success) {
                    [weakSelf.model.subnetBridgeList addObject:weakSelf.bridgeModel];
                    [SigDataSource.share saveLocationData];
                    [ShowTipsHandle.share show:Tip_AddSubnetBridgeSuccess];
                } else {
                    hasFail = YES;
                    [ShowTipsHandle.share show:[NSString stringWithFormat:@"add Subnet Bridge to node fail! error status=%d",responseMessage.status]];
                }
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                dispatch_async(dispatch_get_main_queue(), ^{
                    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(addSubnetBridgeTimeOut) object:nil];
                    if (!isResponseAll || error || hasFail) {
                        [ShowTipsHandle.share hidden];
                        [weakSelf showTips:Tip_AddSubnetBridgeFail];
                    } else {
                        [ShowTipsHandle.share delayHidden:2.0];
                        [weakSelf performSelector:@selector(pop) withObject:nil afterDelay:2.0];
                    }
                });
            }];
        } else {
            [self showTips:@"The node is offline, app can not add bridge table."];
        }
    } else {
        [self showTips:@"The mesh is offline, app can not add bridge table."];
    }
}

- (void)pop {
    [self.navigationController popViewControllerAnimated:YES];
}

- (void)addSubnetBridgeTimeOut {
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(addSubnetBridgeTimeOut) object:nil];
        [ShowTipsHandle.share hidden];
        [self showTips:Tip_AddSubnetBridgeFail];
    });
    [self.messageHandle cancel];
}

#pragma mark - Life method
- (void)normalSetting{
    [super normalSetting];
    self.title = @"Add Subnet Bridge";
    self.addBridgingTableButton.backgroundColor = UIColor.telinkButtonBlue;
    self.tableView.allowsSelection = NO;
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(BridgeTableItemCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(BridgeTableItemCell.class)];
    //iOS 15中 UITableView 新增了一个属性：sectionHeaderTopPadding。此属性会给每一个 section header 增加一个默认高度，当我们使用 UITableViewStylePlain 初始化UITableView 的时候，系统默认给 section header 增高了22像素。
    if(@available(iOS 15.0,*)) {
        self.tableView.sectionHeaderTopPadding = 0;
    }

    SigSubnetBridgeModel *m = [[SigSubnetBridgeModel alloc] initWithDirections:SigDirectionsFieldValues_bidirectional netKeyIndex1:SigDataSource.share.netKeys.lastObject.index netKeyIndex2:SigDataSource.share.netKeys.firstObject.index address1:0x0400 address2:0x0004];
    self.bridgeModel = m;
    self.titleArr = @[@"Direction",@"Net Key 1",@"Net Key 2",@"Address1 : 0x",@"Address2 : 0x"];
    self.netkeyArr = [NSMutableArray array];
    for (SigNetkeyModel *m in SigDataSource.share.netKeys) {
        [self.netkeyArr addObject:[NSString stringWithFormat:@"%@(0x%04X)",m.name,m.index]];
    }
}

-(void)dealloc{
    if (_messageHandle) {
        [_messageHandle cancel];
    }
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
    switch (indexPath.row) {
        case 0:
        {
            cell.inputTF.placeholder = @"请选择";
            cell.inputTF.text = [NSString stringWithFormat:@"%@(0x%X)",[SigHelper.share getDetailOfSigDirectionsFieldValues:self.bridgeModel.directions],self.bridgeModel.directions];
        }
            break;
        case 1:
        {
            cell.inputTF.placeholder = @"请选择";
            cell.inputTF.text = [NSString stringWithFormat:@"%@(0x%04X)",[SigDataSource.share getNetkeyModelWithNetkeyIndex:self.bridgeModel.netKeyIndex1].name,self.bridgeModel.netKeyIndex1];
        }
            break;
        case 2:
        {
            cell.inputTF.placeholder = @"请选择";
            cell.inputTF.text = [NSString stringWithFormat:@"%@(0x%04X)",[SigDataSource.share getNetkeyModelWithNetkeyIndex:self.bridgeModel.netKeyIndex2].name,self.bridgeModel.netKeyIndex2];
        }
            break;
        case 3:
        {
            cell.inputTF.placeholder = @"请输入";
            cell.inputTF.returnKeyType = UIReturnKeyDone;
            cell.inputTF.text = [NSString stringWithFormat:@"%04X",self.bridgeModel.address1];
        }
            break;
        case 4:
        {
            cell.inputTF.placeholder = @"请输入";
            cell.inputTF.returnKeyType = UIReturnKeyDone;
            cell.inputTF.text = [NSString stringWithFormat:@"%04X",self.bridgeModel.address2];
        }
            break;
        default:
            break;
    }
    return cell;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {
    return 50.0f;
}

- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section {
    return 0.0001f;
}

- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section {
    return 0.0001f;
}

#pragma mark - UITextFieldDelegate 返回键
- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    if (textField.tag == 3 || textField.tag == 4) {
        [textField resignFirstResponder];
    }
    return YES;
}

#pragma mark - UITextFieldDelegate
- (BOOL)textFieldShouldBeginEditing:(UITextField *)textField {
    if (textField.tag == 3 || textField.tag == 4) {
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
    NSLog(@"结束编辑:%@", textField.text);
    switch (textField.tag) {
        case 3:
        {
            if ([self validateString:textField.text.removeAllSapceAndNewlines]) {
                self.bridgeModel.address1 = [LibTools uint16From16String:textField.text];
            } else {
                [self showTips:@"Please enter the correct address!"];
            }
        }
            break;
        case 4:
        {
            if ([self validateString:textField.text.removeAllSapceAndNewlines]) {
                self.bridgeModel.address2 = [LibTools uint16From16String:textField.text];
            } else {
                [self showTips:@"Please enter the correct address!"];
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
            // 方向
            BRStringPickerView *stringPickerView = [[BRStringPickerView alloc] init];
            stringPickerView.pickerMode = BRStringPickerComponentSingle;
            stringPickerView.title = @"please choose directions";
            stringPickerView.dataSourceArr = @[@"Unidirectional(0x1)", @"Directional(0x2)"];
            stringPickerView.selectIndex = self.bridgeModel.directions == SigDirectionsFieldValues_unidirectional ? 0 : 1;
            stringPickerView.pickerStyle.cancelBtnTitle = @"Cancel";
            stringPickerView.pickerStyle.doneBtnTitle = @"Done";
            stringPickerView.resultModelBlock = ^(BRResultModel *resultModel) {
                weakSelf.bridgeModel.directions = resultModel.index == 0 ? SigDirectionsFieldValues_unidirectional : SigDirectionsFieldValues_bidirectional;
                textField.text = resultModel.value;
            };
            [stringPickerView show];
        }
            break;
        case 1:
        {
            // Net Key 1
            BRStringPickerView *stringPickerView = [[BRStringPickerView alloc] init];
            stringPickerView.pickerMode = BRStringPickerComponentSingle;
            stringPickerView.title = @"please choose Net Key 1";
            stringPickerView.dataSourceArr = self.netkeyArr;
            SigNetkeyModel *selectModel = [SigDataSource.share getNetkeyModelWithNetkeyIndex:self.bridgeModel.netKeyIndex1];
            stringPickerView.selectIndex = [SigDataSource.share.netKeys indexOfObject:selectModel];
            stringPickerView.pickerStyle.cancelBtnTitle = @"Cancel";
            stringPickerView.pickerStyle.doneBtnTitle = @"Done";
            stringPickerView.resultModelBlock = ^(BRResultModel *resultModel) {
                self.bridgeModel.netKeyIndex1 = SigDataSource.share.netKeys[resultModel.index].index;
                textField.text = resultModel.value;
            };
            [stringPickerView show];
        }
            break;
        case 2:
        {
            // Net Key 2
            BRStringPickerView *stringPickerView = [[BRStringPickerView alloc] init];
            stringPickerView.pickerMode = BRStringPickerComponentSingle;
            stringPickerView.title = @"please choose Net Key 2";
            stringPickerView.dataSourceArr = self.netkeyArr;
            SigNetkeyModel *selectModel = [SigDataSource.share getNetkeyModelWithNetkeyIndex:self.bridgeModel.netKeyIndex2];
            stringPickerView.selectIndex = [SigDataSource.share.netKeys indexOfObject:selectModel];
            stringPickerView.pickerStyle.cancelBtnTitle = @"Cancel";
            stringPickerView.pickerStyle.doneBtnTitle = @"Done";
            stringPickerView.resultModelBlock = ^(BRResultModel *resultModel) {
                self.bridgeModel.netKeyIndex2 = SigDataSource.share.netKeys[resultModel.index].index;
                textField.text = resultModel.value;
            };
            [stringPickerView show];
        }
            break;
        default:
            break;
    }
}

@end
