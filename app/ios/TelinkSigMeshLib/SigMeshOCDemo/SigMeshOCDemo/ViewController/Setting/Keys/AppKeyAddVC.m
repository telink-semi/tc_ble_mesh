/********************************************************************************************************
 * @file     AppKeyAddVC.m
 *
 * @brief    Add or edit the AppKey of Mesh network.
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

#import "AppKeyAddVC.h"
#import "NSString+extension.h"
#import "UIViewController+Message.h"
#import "SettingDetailItemCell.h"

@interface AppKeyAddVC ()
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <NSString *>*iconArray;
@property (nonatomic, strong) NSMutableArray <NSString *>*titleArray;
@property (nonatomic, strong) NSMutableArray <NSString *>*valueArray;
@property (nonatomic, strong) SigAppkeyModel *showAppKeyModel;
@end

@implementation AppKeyAddVC

- (void)viewDidLoad {
    [super viewDidLoad];
    self.title = self.isAdd ? @"Add AppKey" : @"Edit AppKey";
    SigAppkeyModel *key = [[SigAppkeyModel alloc] init];
    [key setDictionaryToSigAppkeyModel:self.appKeyModel.getDictionaryOfSigAppkeyModel];
    self.showAppKeyModel = key;
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(SettingDetailItemCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(SettingDetailItemCell.class)];
    //init rightBarButtonItem
    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"ic_check"] style:UIBarButtonItemStylePlain target:self action:@selector(clickSave)];
    self.navigationItem.rightBarButtonItem = rightItem;
    [self updateUI];
}

- (void)updateUI {
    self.iconArray = [NSMutableArray arrayWithArray:@[@"ic_name", @"ic_id", @"ic_bound_key", @"ic_key_value", @"ic_key_value"]];
    self.titleArray = [NSMutableArray arrayWithArray:@[@"Name", @"Index", @"Bound Net Key", @"Key", @"Old Key"]];
    self.valueArray = [NSMutableArray arrayWithArray:@[self.showAppKeyModel.name, [NSString stringWithFormat:@"0x%03lX",(long)self.showAppKeyModel.index], [NSString stringWithFormat:@"0x%03lX", (long)self.showAppKeyModel.boundNetKey], self.showAppKeyModel.key.length > 0 ? self.showAppKeyModel.key.uppercaseString : @"NULL", self.showAppKeyModel.oldKey.length > 0 ? self.showAppKeyModel.oldKey.uppercaseString : @"NULL"]];
    [self.tableView reloadData];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    SettingDetailItemCell *cell = (SettingDetailItemCell *)[tableView dequeueReusableCellWithIdentifier:NSStringFromClass(SettingDetailItemCell.class) forIndexPath:indexPath];
    cell.iconImageView.image = [UIImage imageNamed:self.iconArray[indexPath.row]];
    cell.nameLabel.text = self.titleArray[indexPath.row];
    cell.detailLabel.text = self.valueArray[indexPath.row];
    if (indexPath.row == 0 || indexPath.row == 4) {
        cell.nextImageView.hidden = YES;
    } else {
        cell.nextImageView.hidden = NO;
    }
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.row == 1) {
        [self clickChangeAppKeyIndex];
    } else if (indexPath.row == 2) {
        [self clickChangeBoundNetKey];
    } else if (indexPath.row == 3) {
        [self clickChangeAppKeyValue];
   }
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.iconArray.count;
}

- (void)clickChangeAppKeyIndex {
    __weak typeof(self) weakSelf = self;
    UIAlertController *inputAlertController = [UIAlertController alertControllerWithTitle:@"Update AppKey Index" message:@"please input content" preferredStyle:UIAlertControllerStyleAlert];
    [inputAlertController addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
        textField.placeholder = @"please input new netKey index";
        textField.text = [NSString stringWithFormat:@"%03lX",(long)weakSelf.showAppKeyModel.index];
    }];
    [inputAlertController addAction:[UIAlertAction actionWithTitle:@"CANCEL" style:UIAlertActionStyleDefault handler:nil]];
    [inputAlertController addAction:[UIAlertAction actionWithTitle:@"CONFIRM" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        TelinkLogDebug(@"输入AppKey index完成");
        UITextField *indexTextField = inputAlertController.textFields.firstObject;
        NSString *index = indexTextField.text.removeAllSpaceAndNewlines;
        //check input length
        if (index.length == 0) {
            // need input any string
            [weakSelf showTips:@"Please input new appKey index."];
            return;
        }
        if (![LibTools validateHex:index]) {
            // Please enter a hexadecimal string
            [weakSelf showTips:@"Please enter a hexadecimal string."];
            return;
        }
        if (index.length > 3) {
            // Please enter a hexadecimal string
            [weakSelf showTips:@"The range of appKey index is 0~0xFFF."];
            return;
        }
        if (index.length % 2 == 1) {
            index = [@"0" stringByAppendingString:index];
        }
        UInt16 intIndex = [LibTools uint16From16String:index];
        BOOL hadExist = NO;
        NSArray *temAppkeys = [NSArray arrayWithArray:weakSelf.network.appKeys];
        for (SigAppkeyModel *tem in temAppkeys) {
            if (tem.index == intIndex) {
                if (self.isAdd) {
                    hadExist = YES;
                    break;
                } else {
                    if (tem != self.appKeyModel) {
                        hadExist = YES;
                        break;
                    }
                }
            }
        }
        if (hadExist) {
            [self showTips:[NSString stringWithFormat:@"AppKey Index:0x%03X already exists, please add other appkey index.",intIndex]];
            return;
        }
        weakSelf.showAppKeyModel.index = intIndex;
        [weakSelf updateUI];
    }]];
    [self presentViewController:inputAlertController animated:YES completion:nil];
}

- (void)clickChangeBoundNetKey {
    __weak typeof(self) weakSelf = self;
    UIAlertController *inputAlertController = [UIAlertController alertControllerWithTitle:@"Update Bound NetKey Index" message:@"please input content" preferredStyle:UIAlertControllerStyleAlert];
    [inputAlertController addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
        textField.placeholder = @"please input new Bound NetKey Index";
        textField.text = [NSString stringWithFormat:@"%03lX",(long)weakSelf.showAppKeyModel.boundNetKey];
    }];
    [inputAlertController addAction:[UIAlertAction actionWithTitle:@"CANCEL" style:UIAlertActionStyleDefault handler:nil]];
    [inputAlertController addAction:[UIAlertAction actionWithTitle:@"CONFIRM" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        TelinkLogDebug(@"输入Bound NetKey index完成");
        UITextField *indexTextField = inputAlertController.textFields.firstObject;
        NSString *index = indexTextField.text.removeAllSpaceAndNewlines;
        //check input length
        if (index.length == 0) {
            // need input any string
            [weakSelf showTips:@"Please input new bound netKey index."];
            return;
        }
        if (![LibTools validateHex:index]) {
            // Please enter a hexadecimal string
            [weakSelf showTips:@"Please enter a hexadecimal string."];
            return;
        }
        if (index.length > 3) {
            // Please enter a hexadecimal string
            [weakSelf showTips:@"The range of bound netKey index is 0~0xFFF."];
            return;
        }
        if (index.length % 2 == 1) {
            index = [@"0" stringByAppendingString:index];
        }
        UInt16 boundNetKey = [LibTools uint16From16String:index];
        BOOL hadExist = NO;
        NSArray *temNetkeys = [NSArray arrayWithArray:weakSelf.network.netKeys];
        for (SigNetkeyModel *tem in temNetkeys) {
            if (tem.index == boundNetKey) {
                hadExist = YES;
                break;
            }
        }
        if (!hadExist) {
            [self showTips:[NSString stringWithFormat:@"boundNetKey index:0x%03X is not exists, please add other boundNetKey index.",boundNetKey]];
            return;
        }
        weakSelf.showAppKeyModel.boundNetKey = boundNetKey;
        [weakSelf updateUI];
    }]];
    [self presentViewController:inputAlertController animated:YES completion:nil];
}

- (void)clickChangeAppKeyValue {
    __weak typeof(self) weakSelf = self;
    UIAlertController *inputAlertController = [UIAlertController alertControllerWithTitle:@"Update AppKey Value" message:@"please input content" preferredStyle:UIAlertControllerStyleAlert];
    [inputAlertController addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
        textField.placeholder = @"please input new appKey value";
        textField.text = weakSelf.showAppKeyModel.key;
    }];
    [inputAlertController addAction:[UIAlertAction actionWithTitle:@"CANCEL" style:UIAlertActionStyleDefault handler:nil]];
    [inputAlertController addAction:[UIAlertAction actionWithTitle:@"CONFIRM" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        TelinkLogDebug(@"输入AppKey value完成");
        UITextField *valueTF = inputAlertController.textFields.firstObject;
        NSString *value = valueTF.text.removeAllSpaceAndNewlines;
        //check input length
        if (value.length == 0) {
            // need input any string
            [weakSelf showTips:@"Please input new appKey value."];
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
        NSArray *temAppkeys = [NSArray arrayWithArray:weakSelf.network.appKeys];
        for (SigAppkeyModel *tem in temAppkeys) {
            if ([tem.key isEqualToString:value]) {
                //不允许不同的NetKey下的APPkey相同
//                if (self.isAdd) {
                //允许不同的NetKey下的APPkey相同
                if (weakSelf.isAdd && tem.boundNetKey == weakSelf.showAppKeyModel.boundNetKey) {
                    hadExist = YES;
                    break;
                } else {
                    //不允许不同的NetKey下的APPkey相同
//                    if (tem != self.appKeyModel) {
                    //允许不同的NetKey下的APPkey相同
                        if (tem != weakSelf.appKeyModel && tem.boundNetKey == weakSelf.showAppKeyModel.boundNetKey) {
                        hadExist = YES;
                        break;
                    }
                }
            }
        }
        if (hadExist) {
            [weakSelf showTips:[NSString stringWithFormat:@"AppKey:0x%@ already exists, please add other appkey.", value]];
            return;
        }
        weakSelf.showAppKeyModel.key = value;
        [weakSelf updateUI];
    }]];
    [self presentViewController:inputAlertController animated:YES completion:nil];
}

- (void)clickSave {
    BOOL hadBound = NO;
    NSArray *temNodes = [NSArray arrayWithArray:self.network.curNodes];
    for (SigNodeModel *node in temNodes) {
        if (node.appKeys && node.appKeys.count > 0) {
            for (SigNodeKeyModel *nodeKey in node.appKeys) {
                if (nodeKey.index == self.appKeyModel.index) {
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
        [self showAlertSureWithTitle:@"Hits" message:@"Some nodes have already bound this appkey, you can`t edit it!" sure:nil];
        /*客户需要保证绑定该AppKey的设备都在线，并且给所有绑定该AppKey的设备发送SigConfigAppKeyUpdate指令且成功后，才可以修改mesh里面的AppKey的值。此处发送逻辑过于复杂且限制太多，当前只是禁止客户进行修改操作。客户使用demoAPP时，可以通过下面的步骤实现修改mesh网络的某个AppKey的功能：可以先从设备里面移除该AppKey，再在setting界面修改AppKey，再给设备重新绑定AppKey。*/
        return;
    }

    if (self.backAppKeyModel) {
        self.backAppKeyModel(self.showAppKeyModel);
    }
    [self.navigationController popViewControllerAnimated:YES];
}

@end
