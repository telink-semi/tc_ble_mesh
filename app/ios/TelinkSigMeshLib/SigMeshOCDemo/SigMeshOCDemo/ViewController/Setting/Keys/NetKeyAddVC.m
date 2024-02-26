/********************************************************************************************************
 * @file     NetKeyAddVC.m
 *
 * @brief    Add or edit the NetKey of Mesh network.
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

#import "NetKeyAddVC.h"
#import "NSString+extension.h"
#import "UIViewController+Message.h"
#import "SettingDetailItemCell.h"

@interface NetKeyAddVC ()
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <NSString *>*iconArray;
@property (nonatomic, strong) NSMutableArray <NSString *>*titleArray;
@property (nonatomic, strong) NSMutableArray <NSString *>*valueArray;
@property (nonatomic, strong) SigNetkeyModel *showNetKeyModel;
@end

@implementation NetKeyAddVC

- (void)viewDidLoad {
    [super viewDidLoad];
    self.title = self.isAdd ? @"Add NetKey" : @"Edit NetKey";
    SigNetkeyModel *key = [[SigNetkeyModel alloc] init];
    [key setDictionaryToSigNetkeyModel:self.netKeyModel.getDictionaryOfSigNetkeyModel];
    self.showNetKeyModel = key;
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(SettingDetailItemCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(SettingDetailItemCell.class)];
    //init rightBarButtonItem
    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"ic_check"] style:UIBarButtonItemStylePlain target:self action:@selector(clickSave)];
    self.navigationItem.rightBarButtonItem = rightItem;
    [self updateUI];
}

- (void)updateUI {
    self.iconArray = [NSMutableArray arrayWithArray:@[@"ic_name", @"ic_id", @"ic_key_value", @"ic_key_value", @"ic_phase", @"ic_min_security", @"ic_timestamp"]];
    self.titleArray = [NSMutableArray arrayWithArray:@[@"Name", @"Index", @"Key", @"Old Key", @"Phase", @"Min Security", @"Timestamp"]];
    self.valueArray = [NSMutableArray arrayWithArray:@[self.showNetKeyModel.name, [NSString stringWithFormat:@"0x%03lX",(long)self.showNetKeyModel.index], self.showNetKeyModel.key.length > 0 ? self.showNetKeyModel.key.uppercaseString : @"NULL", self.showNetKeyModel.oldKey.length > 0 ? self.showNetKeyModel.oldKey.uppercaseString : @"NULL", [NSString stringWithFormat:@"0x%02X",self.showNetKeyModel.phase], self.showNetKeyModel.minSecurity, self.showNetKeyModel.timestamp]];
    [self.tableView reloadData];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    SettingDetailItemCell *cell = (SettingDetailItemCell *)[tableView dequeueReusableCellWithIdentifier:NSStringFromClass(SettingDetailItemCell.class) forIndexPath:indexPath];
    cell.iconImageView.image = [UIImage imageNamed:self.iconArray[indexPath.row]];
    cell.nameLabel.text = self.titleArray[indexPath.row];
    cell.detailLabel.text = self.valueArray[indexPath.row];
    if (indexPath.row > 2 || indexPath.row == 0) {
        cell.nextImageView.hidden = YES;
    } else {
        cell.nextImageView.hidden = NO;
    }
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.row == 1) {
        [self clickChangeNetKeyIndex];
    } else if (indexPath.row == 2) {
        [self clickChangeNetKeyValue];
   }
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.iconArray.count;
}

- (void)clickChangeNetKeyIndex {
    __weak typeof(self) weakSelf = self;
    UIAlertController *inputAlertController = [UIAlertController alertControllerWithTitle:@"Update NetKey Index" message:@"please input content" preferredStyle:UIAlertControllerStyleAlert];
    [inputAlertController addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
        textField.placeholder = @"please input new netKey index";
        textField.text = [NSString stringWithFormat:@"%03lX",(long)weakSelf.showNetKeyModel.index];
    }];
    [inputAlertController addAction:[UIAlertAction actionWithTitle:@"CANCEL" style:UIAlertActionStyleDefault handler:nil]];
    [inputAlertController addAction:[UIAlertAction actionWithTitle:@"CONFIRM" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        TelinkLogDebug(@"输入NetKey index完成");
        UITextField *indexTextField = inputAlertController.textFields.firstObject;
        NSString *index = indexTextField.text.removeAllSpaceAndNewlines;
        //check input length
        if (index.length == 0) {
            // need input any string
            [weakSelf showTips:@"Please input new netKey index."];
            return;
        }
        if (![LibTools validateHex:index]) {
            // Please enter a hexadecimal string
            [weakSelf showTips:@"Please enter a hexadecimal string."];
            return;
        }
        if (index.length > 3) {
            // Please enter a hexadecimal string
            [weakSelf showTips:@"The range of netKey index is 0~0xFFF."];
            return;
        }
        if (index.length % 2 == 1) {
            index = [@"0" stringByAppendingString:index];
        }
        UInt16 intIndex = [LibTools uint16From16String:index];
        BOOL hadExist = NO;
        NSArray *temNetkeys = [NSArray arrayWithArray:weakSelf.network.netKeys];
        for (SigNetkeyModel *tem in temNetkeys) {
            if (tem.index == intIndex) {
                if (weakSelf.isAdd) {
                    hadExist = YES;
                    break;
                } else {
                    if (tem != weakSelf.netKeyModel) {
                        hadExist = YES;
                        break;
                    }
                }
            }
        }
        if (hadExist) {
            [weakSelf showTips:[NSString stringWithFormat:@"NetKey Index:0x%03X already exists, please add other netkey index.", intIndex]];
            return;
        }
        weakSelf.showNetKeyModel.index = intIndex;
        [weakSelf updateUI];
    }]];
    [self presentViewController:inputAlertController animated:YES completion:nil];
}

- (void)clickChangeNetKeyValue {
    __weak typeof(self) weakSelf = self;
    UIAlertController *inputAlertController = [UIAlertController alertControllerWithTitle:@"Update NetKey Value" message:@"please input content" preferredStyle:UIAlertControllerStyleAlert];
    [inputAlertController addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
        textField.placeholder = @"please input new netKey value";
        textField.text = weakSelf.showNetKeyModel.key;
    }];
    [inputAlertController addAction:[UIAlertAction actionWithTitle:@"CANCEL" style:UIAlertActionStyleDefault handler:nil]];
    [inputAlertController addAction:[UIAlertAction actionWithTitle:@"CONFIRM" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        TelinkLogDebug(@"输入NetKey value完成");
        UITextField *valueTF = inputAlertController.textFields.firstObject;
        NSString *value = valueTF.text.removeAllSpaceAndNewlines;
        //check input length
        if (value.length == 0) {
            // need input any string
            [weakSelf showTips:@"Please input new netKey value."];
            return;
        }
        if (![LibTools validateHex:value]) {
            // Please enter a hexadecimal string
            [weakSelf showTips:@"Please enter a hexadecimal string."];
            return;
        }
        if (value.length != 32) {
            // Please enter a hexadecimal string
            [weakSelf showTips:@"Please enter 16 bytes hex key data."];
            return;
        }
        BOOL hadExist = NO;
        NSArray *temNetkeys = [NSArray arrayWithArray:weakSelf.network.netKeys];
        for (SigNetkeyModel *tem in temNetkeys) {
            if ([tem.key isEqualToString:value]) {
                if (weakSelf.isAdd) {
                    hadExist = YES;
                    break;
                } else {
                    if (tem != weakSelf.netKeyModel) {
                        hadExist = YES;
                        break;
                    }
                }
            }
        }
        if (hadExist) {
            [weakSelf showTips:[NSString stringWithFormat:@"NetKey:0x%@ already exists, please add other netkey.", value]];
            return;
        }
        weakSelf.showNetKeyModel.key = value;
        [weakSelf updateUI];
    }]];
    [self presentViewController:inputAlertController animated:YES completion:nil];
}

- (void)clickSave {
    if (!self.isAdd) {
        BOOL hadBound = NO;
        NSArray *temNodes = [NSArray arrayWithArray:self.network.curNodes];
        for (SigNodeModel *node in temNodes) {
            if (node.netKeys && node.netKeys.count > 0) {
                for (SigNodeKeyModel *nodeKey in node.netKeys) {
                    if (nodeKey.index == self.netKeyModel.index) {
                        hadBound = YES;
                        break;
                    }
                }
                if (hadBound) {
                    break;
                }
            }
        }
        if (hadBound) {
            [self showAlertSureWithTitle:@"Hits" message:@"Some nodes have already bound this netkey, you can`t edit it!" sure:nil];
            /*客户需要保证添加了该NetKey的设备都在线，并且给所有添加了该NetKey的设备发送SigConfigNetKeyUpdate指令且成功后，才可以修改mesh里面的NetKey的值。此处发送逻辑过于复杂且限制太多，当前只是禁止客户进行修改操作。客户使用demoAPP时，可以通过下面的步骤实现修改mesh网络的某个NetKey的功能：可以先从设备里面移除该NetKey，再在setting界面修改NetKey，再给设备重新添加NetKey。*/
            return;
        }
    }

    if (self.backNetKeyModel) {
        self.backNetKeyModel(self.showNetKeyModel);
    }
    [self.navigationController popViewControllerAnimated:YES];
}

@end
