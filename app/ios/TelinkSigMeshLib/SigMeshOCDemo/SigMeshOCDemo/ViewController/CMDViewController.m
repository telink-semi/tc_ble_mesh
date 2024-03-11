/********************************************************************************************************
 * @file     CMDViewController.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/7/31
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

#import "CMDViewController.h"
#import "NSString+extension.h"
#import "UIViewController+Message.h"
#import "CMDCell.h"
#import "UIButton+extension.h"

/**
 Attention: more detail about ini packet can look document 《SIG Mesh iOS APP(OC版本)使用以及开发手册.docx》k.自定义ini指令组包结构说明
 */
@interface CMDViewController()<UITextFieldDelegate>
@property (strong, nonatomic) IBOutlet UITableView *tableView;
@property (strong, nonatomic) IBOutlet UITextView *showTextView;
@property (weak, nonatomic) IBOutlet UIImageView *showMoreImageView;
@property (weak, nonatomic) IBOutlet NSLayoutConstraint *showButtonTopConstraint;
@property (nonatomic,strong) NSMutableArray <IniCommandModel *>*commandList;
@property (nonatomic,strong) NSMutableArray <NSString *>*nameList;
@property (nonatomic,strong) IniCommandModel *currentCommand;
@property (nonatomic,strong) NSString *currentCommandName;
@property (nonatomic,strong) NSString *logString;
@end

@implementation CMDViewController

#pragma mark - Life method

- (void)normalSetting {
    [super normalSetting];
    //init commandList
    self.commandList = [NSMutableArray array];
    //init nameList
    self.nameList = [NSMutableArray array];
    //init @"Vendor On" IniCommandModel
    IniCommandModel *command1 = [[IniCommandModel alloc] initVendorModelIniCommandWithNetkeyIndex:(UInt16)SigDataSource.share.curNetkeyModel.index appkeyIndex:(UInt16)SigDataSource.share.curAppkeyModel.index retryCount:2 responseMax:0 address:0xFFFF opcode:0xC2 vendorId:0x0211 responseOpcode:0xc4 tidPosition:2 tid:0 commandData:[LibTools nsstringToHex:@"0100"]];
    //init @"Vendor Off" IniCommandModel
    IniCommandModel *command2 = [[IniCommandModel alloc] initVendorModelIniCommandWithNetkeyIndex:(UInt16)SigDataSource.share.curNetkeyModel.index appkeyIndex:(UInt16)SigDataSource.share.curAppkeyModel.index retryCount:2 responseMax:0 address:0xFFFF opcode:0xC2 vendorId:0x0211 responseOpcode:0xc4 tidPosition:2 tid:0 commandData:[LibTools nsstringToHex:@"0000"]];
    //init @"Vendor On/Off Get" IniCommandModel
    IniCommandModel *command3 = [[IniCommandModel alloc] initVendorModelIniCommandWithNetkeyIndex:(UInt16)SigDataSource.share.curNetkeyModel.index appkeyIndex:(UInt16)SigDataSource.share.curAppkeyModel.index retryCount:2 responseMax:0 address:0xFFFF opcode:0xC1 vendorId:0x0211 responseOpcode:0xc4 tidPosition:0 tid:0 commandData:nil];
    //init @"Vendor On NO-ACK" IniCommandModel
    IniCommandModel *command4 = [[IniCommandModel alloc] initVendorModelIniCommandWithNetkeyIndex:(UInt16)SigDataSource.share.curNetkeyModel.index appkeyIndex:(UInt16)SigDataSource.share.curAppkeyModel.index retryCount:2 responseMax:0 address:0xFFFF opcode:0xC3 vendorId:0x0211 responseOpcode:0 tidPosition:2 tid:0 commandData:[LibTools nsstringToHex:@"0100"]];
    //init @"Vendor Off NO-ACK" IniCommandModel
    IniCommandModel *command5 = [[IniCommandModel alloc] initVendorModelIniCommandWithNetkeyIndex:(UInt16)SigDataSource.share.curNetkeyModel.index appkeyIndex:(UInt16)SigDataSource.share.curAppkeyModel.index retryCount:2 responseMax:0 address:0xFFFF opcode:0xC3 vendorId:0x0211 responseOpcode:0 tidPosition:2 tid:0 commandData:[LibTools nsstringToHex:@"0000"]];
    //init @"Generic On" IniCommandModel
    IniCommandModel *command6 = [[IniCommandModel alloc] initSigModelIniCommandWithNetkeyIndex:(UInt16)SigDataSource.share.curNetkeyModel.index appkeyIndex:(UInt16)SigDataSource.share.curAppkeyModel.index retryCount:2 responseMax:0 address:0xFFFF opcode:CFSwapInt16HostToBig(SigOpCode_genericOnOffSet) commandData:[LibTools nsstringToHex:@"0100"]];
    command6.tidPosition = 2;
    command6.responseOpcode = (UInt16)CFSwapInt16HostToBig(SigOpCode_genericOnOffStatus);
    //init @"Generic Off" IniCommandModel
    IniCommandModel *command7 = [[IniCommandModel alloc] initSigModelIniCommandWithNetkeyIndex:(UInt16)SigDataSource.share.curNetkeyModel.index appkeyIndex:(UInt16)SigDataSource.share.curAppkeyModel.index retryCount:2 responseMax:0 address:0xFFFF opcode:CFSwapInt16HostToBig(SigOpCode_genericOnOffSet) commandData:[LibTools nsstringToHex:@"0000"]];
    command7.tidPosition = 2;
    command7.responseOpcode = (UInt16)CFSwapInt16HostToBig(SigOpCode_genericOnOffStatus);

    //init @"Opcode Aggregator(Lightness Default Get + Lightness Range Get)" IniCommandModel
    UInt16 address = 2;
    if (SigDataSource.share.curNodes.count > 0) {
        address = SigDataSource.share.curNodes.firstObject.address;
    }
    SigOpcodesAggregatorItemModel *model1 = [[SigOpcodesAggregatorItemModel alloc] initWithSigMeshMessage:[[SigLightLightnessDefaultGet alloc] init]];
    SigOpcodesAggregatorItemModel *model2 = [[SigOpcodesAggregatorItemModel alloc] initWithSigMeshMessage:[[SigLightLightnessRangeGet alloc] init]];
    NSArray *items1 = @[model1,model2];
    SigOpcodesAggregatorSequence *message1 = [[SigOpcodesAggregatorSequence alloc] initWithElementAddress:address items:items1];
    IniCommandModel *command8 = [[IniCommandModel alloc] initSigModelIniCommandWithNetkeyIndex:(UInt16)SigDataSource.share.curNetkeyModel.index appkeyIndex:(UInt16)SigDataSource.share.curAppkeyModel.index retryCount:2 responseMax:0 address:address opcode:CFSwapInt16HostToBig(SigOpCode_OpcodesAggregatorSequence) commandData:message1.parameters];
    command8.responseOpcode = (UInt16)CFSwapInt16HostToBig(SigOpCode_OpcodesAggregatorStatus);

    //init @"Opcode Aggregator(TTL Get + Friend Get + Relay Get)" IniCommandModel
    SigOpcodesAggregatorItemModel *model3 = [[SigOpcodesAggregatorItemModel alloc] initWithSigMeshMessage:[[SigConfigDefaultTtlGet alloc] init]];
    SigOpcodesAggregatorItemModel *model4 = [[SigOpcodesAggregatorItemModel alloc] initWithSigMeshMessage:[[SigConfigFriendGet alloc] init]];
    SigOpcodesAggregatorItemModel *model5 = [[SigOpcodesAggregatorItemModel alloc] initWithSigMeshMessage:[[SigConfigRelayGet alloc] init]];
    NSArray *items2 = @[model3,model4,model5];
    SigOpcodesAggregatorSequence *message2 = [[SigOpcodesAggregatorSequence alloc] initWithElementAddress:SigDataSource.share.curNodes.firstObject.address items:items2];
    IniCommandModel *command9 = [[IniCommandModel alloc] initSigModelIniCommandWithNetkeyIndex:(UInt16)SigDataSource.share.curNetkeyModel.index appkeyIndex:(UInt16)SigDataSource.share.curAppkeyModel.index retryCount:2 responseMax:0 address:address opcode:CFSwapInt16HostToBig(SigOpCode_OpcodesAggregatorSequence) commandData:message2.parameters];
    command9.isEncryptByDeviceKey = YES;
    command9.responseOpcode = (UInt16)CFSwapInt16HostToBig(SigOpCode_OpcodesAggregatorStatus);

    //init @"[Custom]" IniCommandModel
    IniCommandModel *command10 = [[IniCommandModel alloc] initSigModelIniCommandWithNetkeyIndex:(UInt16)SigDataSource.share.curNetkeyModel.index appkeyIndex:(UInt16)SigDataSource.share.curAppkeyModel.index retryCount:2 responseMax:0 address:0xFFFF opcode:0 commandData:[NSData data]];
    
    //add command to self.commandList
    [self.commandList addObjectsFromArray:@[command1, command2, command3, command4, command5, command6, command7, command8, command9, command10]];
    //add name to self.nameList
    [self.nameList addObjectsFromArray:@[@"Vendor On", @"Vendor Off", @"Vendor On/Off Get", @"Vendor On NO-ACK", @"Vendor Off NO-ACK", @"Generic On", @"Generic Off", @"Opcode Aggregator(Lightness Default Get + Lightness Range Get)", @"Opcode Aggregator(TTL Get + Friend Get + Relay Get)", @"[Custom]"]];
    //init UI data
    self.showButtonTopConstraint.constant = 0;
    self.currentCommand = self.commandList.firstObject;
    self.currentCommandName = self.nameList.firstObject;
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(CMDCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(CMDCell.class)];
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(CMDCell.class) bundle:nil] forCellReuseIdentifier:[NSString stringWithFormat:@"%@4", NSStringFromClass(CMDCell.class)]];
    //init rightBarButtonItem
    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"ic_send"] style:UIBarButtonItemStylePlain target:self action:@selector(clickSendCommand)];
    self.navigationItem.rightBarButtonItem = rightItem;
    //init logString
    self.logString = @"";
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)viewWillDisappear:(BOOL)animated{
    [super viewWillDisappear:animated];
    [self.view endEditing:YES];
}

-(void)dealloc{
    SigBluetooth.share.bluetoothDidUpdateValueCallback = nil;
    TelinkLogDebug(@"");
}

#pragma mark - UITableViewDelegate,UITableViewDataSource

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:NSStringFromClass(CMDCell.class) forIndexPath:indexPath];
    if (indexPath.row == 4) {
        cell = [tableView dequeueReusableCellWithIdentifier:[NSString stringWithFormat:@"%@4", NSStringFromClass(CMDCell.class)] forIndexPath:indexPath];
    }
    [self configureCell:cell forRowAtIndexPath:indexPath];
    return cell;
}

- (void)configureCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath {
    CMDCell *itemCell = (CMDCell *)cell;
    IniCommandModel *model = self.currentCommand;
    NSString *params = [LibTools convertDataToHexStr:model.commandData];
    __weak typeof(self) weakSelf = self;

    itemCell.valueTF.text = @"";
    itemCell.paramsLabel.text = @"";
    itemCell.valueButton.hidden = YES;
    itemCell.moreImageView.hidden = YES;
    itemCell.valueTF.hidden = NO;
    if (indexPath.row != 4) {
        [itemCell.paramsLabel removeFromSuperview];
    }
    itemCell.valueTF.tag = indexPath.row;
    itemCell.valueTF.delegate = self;
    NSString *spaceString = @"                                                                                                                                                                              ";
    switch (indexPath.row) {
        case 0:
        {
            itemCell.valueTF.placeholder = [@"Please select the command that the APP needs to send." stringByAppendingString:spaceString];
            itemCell.nameLabel.text = @"Action:";
            itemCell.moreImageView.hidden = NO;
            itemCell.valueButton.hidden = NO;
            itemCell.valueTF.hidden = YES;
            [itemCell.valueButton setTitle:self.currentCommandName forState:UIControlStateNormal];
            [itemCell.valueButton addAction:^(UIButton *button) {
                [weakSelf clickSelectActionsButton:button];
            }];
            break;
        }
        case 1:
        {
            itemCell.valueTF.placeholder = [@"Please select the access type of this command." stringByAppendingString:spaceString];
            itemCell.nameLabel.text = @"Access Type:";
            itemCell.moreImageView.hidden = NO;
            itemCell.valueButton.hidden = NO;
            itemCell.valueTF.hidden = YES;
            [itemCell.valueButton setTitle:model.isEncryptByDeviceKey ? @"Device(Device Key)" : @"Application(App Key)" forState:UIControlStateNormal];
            [itemCell.valueButton addAction:^(UIButton *button) {
                [weakSelf clickSelectAccessTypeButton:button];
            }];
            break;
        }
        case 2:
            itemCell.valueTF.placeholder = [@"Please input the destination address of this command." stringByAppendingString:spaceString];
            itemCell.nameLabel.text = @"*dst adr:0x";
            itemCell.valueTF.text = [NSString stringWithFormat:@"%04X", model.address];
            break;
        case 3:
            itemCell.valueTF.placeholder = [@"Please input opcode of this command." stringByAppendingString:spaceString];
            itemCell.nameLabel.text = @"*opcode:0x";
            itemCell.valueTF.text = [self getHexStringOpcodeOfCommand:model];
            break;
        case 4:
            itemCell.valueTF.placeholder = [@"Please input parameters of this command." stringByAppendingString:spaceString];
            itemCell.nameLabel.text = @"params:0x";
            itemCell.valueTF.text = [NSString stringWithFormat:@"%@", params];
            //显示:大写，去空格，加空格
            itemCell.paramsLabel.text = [[params substringWithRange:NSMakeRange(0, params.length/2*2)] insertSpaceNum:1 charNum:2];
            break;
        case 5:
            itemCell.valueTF.placeholder = [@"Null for NO-ACK message." stringByAppendingString:spaceString];
            itemCell.nameLabel.text = @"rsp opcode:0x";
            itemCell.valueTF.text = [self getHexStringResponseOpcodeOfCommand:model];
            break;
        case 6:
            itemCell.nameLabel.text = @"rsp max:";
            itemCell.valueTF.placeholder = [@"Expected response count for reliable message." stringByAppendingString:spaceString];
            itemCell.valueTF.text = [NSString stringWithFormat:@"%d", model.responseMax];
            break;
        case 7:
            itemCell.nameLabel.text = @"retry count:";
            itemCell.valueTF.placeholder = [@"Max retry count for reliable message." stringByAppendingString:spaceString];
            itemCell.valueTF.text = [NSString stringWithFormat:@"%d", model.retryCount];
            break;
        case 8:
            itemCell.valueTF.placeholder = [@"Please input ttl of this command." stringByAppendingString:spaceString];
            itemCell.nameLabel.text = @"ttl:";
            itemCell.valueTF.text = [NSString stringWithFormat:@"%d", model.ttl];
            break;
        case 9:
            itemCell.nameLabel.text = @"tid position:";
            itemCell.valueTF.placeholder = [@"Null for NO-TID message" stringByAppendingString:spaceString];
            if (model.tidPosition != 0) {
                itemCell.valueTF.text = [NSString stringWithFormat:@"%d", model.tidPosition];
            } else {
                itemCell.valueTF.text = @"";
            }
            break;

        default:
            break;
    }
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return 10;
}

#pragma mark - UITextFieldDelegate

/// may be called if forced even if shouldEndEditing returns NO (e.g. view removed from window) or endEditing:YES called
- (void)textFieldDidEndEditing:(UITextField *)textField {
    if (textField.tag == 2 || textField.tag == 3 || textField.tag == 5) {
        //优化16进制的字符串显示：
        //1.大写显示
        //2.长度为偶数，基数则在前面补零
        NSString *tem = textField.text.uppercaseString;
        if (tem.length % 2 == 1) {
            tem = [@"0" stringByAppendingString:tem];
        }
        textField.text = tem;
    } else if (textField.tag == 4) {
        textField.text = textField.text.uppercaseString;
    }
}

/// return NO to not change text
- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string {
    if (string.length > 1) {
        //处理通过多选或者粘贴的模式来一次性修改多个字符的情况
        string = [string removeAllSpaceAndNewlines];
        if ([LibTools validateHex:string]) {
            textField.text = [textField.text stringByReplacingCharactersInRange:range withString:string];
            if (textField.tag == 4) {
                NSIndexPath *path = [NSIndexPath indexPathForRow:textField.tag inSection:0];
                CMDCell *itemCell = (CMDCell *)[self.tableView cellForRowAtIndexPath:path];
                itemCell.paramsLabel.text = [[textField.text substringWithRange:NSMakeRange(0, textField.text.length/2*2)] insertSpaceNum:1 charNum:2];
            }
            return NO;
        }
    }
    //0-9的数字
    NSCharacterSet *charSet = [[NSCharacterSet characterSetWithCharactersInString:@"0123456789aA"] invertedSet];
    if (textField.tag >= 2 && textField.tag <= 5) {
        //0-9的数字+A~F+a~f
        charSet = [[NSCharacterSet characterSetWithCharactersInString:@"0123456789aAbBcCdDeEfF"] invertedSet];
    }
    NSString *filteredStr = [[string componentsSeparatedByCharactersInSet:charSet] componentsJoinedByString:@""];
    if ([string isEqualToString:filteredStr]) {
        //处理键盘单次输入一个字符的情况
        //更新UI上蓝色的params显示
        if (textField.tag == 4) {
            NSString *params = [textField.text stringByReplacingCharactersInRange:range withString:string];
            NSIndexPath *path = [NSIndexPath indexPathForRow:textField.tag inSection:0];
            CMDCell *itemCell = (CMDCell *)[self.tableView cellForRowAtIndexPath:path];
            itemCell.paramsLabel.text = [[params substringWithRange:NSMakeRange(0, params.length/2*2)] insertSpaceNum:1 charNum:2];
        }
        return YES;
    }
    return NO;
}

#pragma mark - Event

- (IBAction)clickShowOrHideenButton:(UIButton *)sender {
    sender.selected = !sender.selected;
    if (sender.selected) {
        self.showButtonTopConstraint.constant = -(68-22)*8-68;
        self.showMoreImageView.image = [UIImage imageNamed:@"fb_bottom"];
    } else {
        self.showButtonTopConstraint.constant = 0;
        self.showMoreImageView.image = [UIImage imageNamed:@"fb_top"];
    }
}

- (void)clickSendCommand {
    [self.view endEditing:YES];
    if ([self validateCommandParameters]) {
        [self showNewLogMessage:[NSString stringWithFormat:@"send message: opcode -- %@ params -- %@",[self getHexStringOpcodeOfCommand:self.currentCommand], [LibTools convertDataToHexStr:self.currentCommand.commandData]]];
        __weak typeof(self) weakSelf = self;
        [SDKLibCommand sendIniCommandModel:self.currentCommand successCallback:^(UInt16 source, UInt16 destination, SigMeshMessage * _Nonnull responseMessage) {
            NSString *str = [NSString stringWithFormat:@"notify: opcode -- %@ params -- %@", [LibTools convertDataToHexStr:[LibTools turnOverData:[LibTools nsstringToHex:[NSString stringWithFormat:@"%X", responseMessage.opCode]]]], [LibTools convertDataToHexStr:responseMessage.parameters]];
            TelinkLogVerbose(@"%@",str);
            [weakSelf showNewLogMessage:str];
        } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
            TelinkLogVerbose(@"finish");
        }];
    }
}

- (void)clickSelectActionsButton:(UIButton *)button {
    __weak typeof(self) weakSelf = self;
    UIAlertController *actionSheet = [UIAlertController alertControllerWithTitle:@"Actions" message:nil preferredStyle:UIAlertControllerStyleActionSheet];
    for (int i=0; i<weakSelf.nameList.count; i++) {
        UIAlertAction *alertT = [UIAlertAction actionWithTitle:weakSelf.nameList[i] style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            weakSelf.currentCommand = weakSelf.commandList[i];
            weakSelf.currentCommandName = weakSelf.nameList[i];
            [weakSelf.tableView reloadData];
        }];
        [actionSheet addAction:alertT];
    }
    UIAlertAction *alertF = [UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
    }];
    [actionSheet addAction:alertF];
    actionSheet.popoverPresentationController.sourceView = button;
    actionSheet.popoverPresentationController.sourceRect =  button.frame;
    [self presentViewController:actionSheet animated:YES completion:nil];
}

- (void)clickSelectAccessTypeButton:(UIButton *)button {
    __weak typeof(self) weakSelf = self;
    UIAlertController *actionSheet = [UIAlertController alertControllerWithTitle:@"Tpyes" message:nil preferredStyle:UIAlertControllerStyleActionSheet];
    for (int i=0; i<2; i++) {
        UIAlertAction *alertT = [UIAlertAction actionWithTitle:i == 0 ? @"Application(APP Key)" : @"Device(Device Key)" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            weakSelf.currentCommand.isEncryptByDeviceKey = i == 1;
            [button setTitle:i == 0 ? @"Application(APP Key)" : @"Device(Device Key)" forState:UIControlStateNormal];
        }];
        [actionSheet addAction:alertT];
    }
    UIAlertAction *alertF = [UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
    }];
    [actionSheet addAction:alertF];
    actionSheet.popoverPresentationController.sourceView = button;
    actionSheet.popoverPresentationController.sourceRect =  button.frame;
    [self presentViewController:actionSheet animated:YES completion:nil];
}

#pragma mark - Other

- (NSString *)getHexStringOpcodeOfCommand:(IniCommandModel *)command {
    NSString *tem = @"";
    SigOpCodeType op_type = [SigHelper.share getOpCodeTypeWithOpcode:command.opcode&0xFF];
    if (command.opcode > 0xFF) {
        op_type = [SigHelper.share getOpCodeTypeWithOpcode:CFSwapInt16BigToHost(command.opcode)&0xFF];
    }
    if (op_type == SigOpCodeType_sig1) {
        tem = [NSString stringWithFormat:@"%02X", command.opcode];
    } else if (op_type == SigOpCodeType_sig2) {
        tem = [NSString stringWithFormat:@"%04X", CFSwapInt16BigToHost(command.opcode)];
    } else if (op_type == SigOpCodeType_vendor3) {
        tem = [NSString stringWithFormat:@"%04X%02X", command.vendorId, command.opcode];
    }
    return tem;
}

- (NSString *)getHexStringResponseOpcodeOfCommand:(IniCommandModel *)command {
    NSString *tem = @"";
    if (command.responseOpcode != 0) {
        SigOpCodeType responseOpcodeType = [SigHelper.share getOpCodeTypeWithOpcode:command.responseOpcode&0xFF];
        if (command.responseOpcode > 0xFF) {
            responseOpcodeType = [SigHelper.share getOpCodeTypeWithOpcode:CFSwapInt16BigToHost(command.responseOpcode)&0xFF];
        }
        if (responseOpcodeType == SigOpCodeType_sig1) {
            tem = [NSString stringWithFormat:@"%02X", command.responseOpcode];
        } else if (responseOpcodeType == SigOpCodeType_sig2) {
            tem = [NSString stringWithFormat:@"%04X", CFSwapInt16BigToHost(command.responseOpcode)];
        } else if (responseOpcodeType == SigOpCodeType_vendor3) {
            tem = [NSString stringWithFormat:@"%04X%02X", command.vendorId, command.responseOpcode];
        }
    }
    return tem;
}

- (BOOL)validateCommandParameters {
    //check destination address
    CMDCell *itemCell = (CMDCell *)[self.tableView cellForRowAtIndexPath:[NSIndexPath indexPathForRow:2 inSection:0]];
    NSString *destination = itemCell.valueTF.text;
    if (destination.length > 4) {
        [self showTips:@"The range of the destination address is 0~0xFFFF."];
        return NO;
    } else if (destination == nil || destination.length == 0) {
        [self showTips:@"Please input destination."];
        return NO;
    }
    UInt16 address = [LibTools uint16From16String:destination];
    self.currentCommand.address = address;

    //check opcode
    itemCell = (CMDCell *)[self.tableView cellForRowAtIndexPath:[NSIndexPath indexPathForRow:3 inSection:0]];
    NSString *opcodeString = itemCell.valueTF.text;
    if (opcodeString.length > 6) {
        [self showTips:@"The range of the opcode is 0~0xFFFFFF."];
        return NO;
    } else if (opcodeString == nil || opcodeString.length == 0) {
        [self showTips:@"Please input opcode."];
        return NO;
    }
    if (opcodeString.length == 6) {
        //vendor opcode
        self.currentCommand.opcode = [LibTools uint8From16String:[opcodeString substringFromIndex:4]];
        self.currentCommand.vendorId = [LibTools uint16From16String:[opcodeString substringToIndex:4]];
    } else {
        //sig opcode
        self.currentCommand.opcode = [LibTools uint16From16String:opcodeString];
    }
    
    //check params
    itemCell = (CMDCell *)[self.tableView cellForRowAtIndexPath:[NSIndexPath indexPathForRow:4 inSection:0]];
    NSString *paramsString = itemCell.valueTF.text;
    self.currentCommand.commandData = [LibTools nsstringToHex:[paramsString substringWithRange:NSMakeRange(0, paramsString.length/2*2)]];

    //check response opcode
    itemCell = (CMDCell *)[self.tableView cellForRowAtIndexPath:[NSIndexPath indexPathForRow:5 inSection:0]];
    NSString *responseOpcodeString = itemCell.valueTF.text;
    if (responseOpcodeString.length > 6) {
        [self showTips:@"The range of the response opcode is 0~0xFFFFFF."];
        return NO;
    } else if (responseOpcodeString != nil && responseOpcodeString.length != 0) {
        if (responseOpcodeString.length == 6) {
            //vendor opcode
            self.currentCommand.responseOpcode = [LibTools uint8From16String:[responseOpcodeString substringFromIndex:4]];
            self.currentCommand.vendorId = [LibTools uint16From16String:[responseOpcodeString substringToIndex:4]];
        } else {
            //sig opcode
            self.currentCommand.responseOpcode = [LibTools uint16From16String:responseOpcodeString];
        }
    }
    
    //check response max
    itemCell = (CMDCell *)[self.tableView cellForRowAtIndexPath:[NSIndexPath indexPathForRow:6 inSection:0]];
    NSString *responseMaxString = itemCell.valueTF.text;
    if (responseMaxString == nil || responseMaxString.length == 0) {
        [self showTips:@"Please input response max."];
        return NO;
    }
    self.currentCommand.responseMax = [responseMaxString intValue];

    //check retry count
    itemCell = (CMDCell *)[self.tableView cellForRowAtIndexPath:[NSIndexPath indexPathForRow:7 inSection:0]];
    NSString *retryCountString = itemCell.valueTF.text;
    if (retryCountString == nil || retryCountString.length == 0) {
        [self showTips:@"Please input retry count."];
        return NO;
    }
    self.currentCommand.retryCount = [retryCountString intValue];

    //check ttl
    itemCell = (CMDCell *)[self.tableView cellForRowAtIndexPath:[NSIndexPath indexPathForRow:8 inSection:0]];
    NSString *ttlString = itemCell.valueTF.text;
    if (ttlString == nil || ttlString.length == 0) {
        [self showTips:@"Please input ttl."];
        return NO;
    }
    self.currentCommand.ttl = [ttlString intValue];

    //check tid position
    itemCell = (CMDCell *)[self.tableView cellForRowAtIndexPath:[NSIndexPath indexPathForRow:9 inSection:0]];
    NSString *tidPositionString = itemCell.valueTF.text;
    if (tidPositionString != nil && tidPositionString.length > 0) {
        UInt8 position = [tidPositionString intValue];
        if (self.currentCommand.commandData.length < position) {
            [self showTips:@"Unable to get the value of tid from the params, the length of the params needs to be greater than or equal to the value of tid position."];
            return NO;
        }
        self.currentCommand.tidPosition = position;
        if (position > 0) {
            self.currentCommand.tid = [LibTools uint8From16String:[paramsString substringWithRange:NSMakeRange(position-1, 1)]];
        }
    }
    
    return YES;
}

- (void)showNewLogMessage:(NSString *)msg {
    NSDateFormatter *dformatter = [[NSDateFormatter alloc] init];
    dformatter.dateFormat =@"HH:mm:ss.SSS";
    self.logString = [self.logString stringByAppendingString:[NSString stringWithFormat:@"%@ %@\n",[dformatter stringFromDate:[NSDate date]],msg]];
    dispatch_async(dispatch_get_main_queue(), ^{
        self.showTextView.text = self.logString;
        [self.showTextView scrollRangeToVisible:NSMakeRange(self.showTextView.text.length, 1)];
    });
}

@end
