/********************************************************************************************************
 * @file     ProxyFilterVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/4/11
 *
 * @par     Copyright (c) [2023], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "ProxyFilterVC.h"
#import "ProxyFilterCell.h"
#import "SettingItemCell.h"
#import "FilterAddressCell.h"
#import "UIButton+extension.h"
#import "UIViewController+Message.h"

@interface ProxyFilterVC ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIButton *addAddressButton;
@property (nonatomic, strong) SigProxyFilterModel *filterModel;

@end

@implementation ProxyFilterVC

- (void)viewDidLoad {
    [super viewDidLoad];
    self.title = @"Proxy Filter";
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_ProxyFilterCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_ProxyFilterCellID];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_FilterAddressCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_FilterAddressCellID];
    self.addAddressButton.backgroundColor = UIColor.telinkButtonBlue;
    [self refreshSourceAndUI];
}

- (void)refreshSourceAndUI {
    self.filterModel = SigDataSource.share.filterModel;
    [self.tableView reloadData];
}

- (IBAction)clickAddAddress:(UIButton *)sender {
    __weak typeof(self) weakSelf = self;
    UIAlertController *inputAlertController = [UIAlertController alertControllerWithTitle:@"Hits" message:@"Please enter new address from 0x0001 to 0xFFFF." preferredStyle:UIAlertControllerStyleAlert];
    [inputAlertController addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
        textField.placeholder = @"0x0001 ~ 0xFFFF";
        textField.text = @"";
    }];
    [inputAlertController addAction:[UIAlertAction actionWithTitle:@"Done" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        TeLogDebug(@"输入address完成");
        UITextField *addressTF = inputAlertController.textFields.firstObject;
        //check input length
        if (addressTF.text.length == 0) {
            // need input any string
            [weakSelf showTips:@"Please input new address."];
            return;
        }
        //check input format
        if ([LibTools validateHex:addressTF.text] == NO) {
            // need input hexadecimal char.
            [weakSelf showTips:@"Please input a hexadecimal string."];
            return;
        }
        UInt16 address = [LibTools uint16From16String:addressTF.text];
        TeLogDebug(@"输入address=%d",address);
        if ([weakSelf.filterModel.addressList containsObject:@(address)]) {
            [weakSelf showTips:[NSString stringWithFormat:@"Add fail! The address 0x%04X already exists.", address]];
            return;
        }
        
        UIAlertController *pushAlertController = [UIAlertController alertControllerWithTitle:@"Hits" message:[NSString stringWithFormat:@"Are you sure to add address 0x%04X?", address] preferredStyle:UIAlertControllerStyleAlert];
        [pushAlertController addAction:[UIAlertAction actionWithTitle:@"Add" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            [weakSelf.filterModel.addressList addObject:@(address)];
            [weakSelf saveFilterLocation];
            [weakSelf refreshSourceAndUI];
        }]];
        [pushAlertController addAction:[UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
            TeLogDebug(@"点击取消");
        }]];
        [weakSelf presentViewController:pushAlertController animated:YES completion:nil];
    }]];
    [self presentViewController:inputAlertController animated:YES completion:nil];
}

- (void)saveFilterLocation {
    NSDictionary *dict = [self.filterModel getDictionaryOfSigProxyFilterModel];
    NSData *filterData = [LibTools getJSONDataWithDictionary:dict];
    [[NSUserDefaults standardUserDefaults] setValue:filterData forKey:kFilter];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    __weak typeof(self) weakSelf = self;
    if (indexPath.row == 0) {
        ProxyFilterCell *cell = (ProxyFilterCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_ProxyFilterCellID forIndexPath:indexPath];
        cell.accessoryType = UITableViewCellAccessoryNone;
        [cell setFilterType:self.filterModel.filterType];
        [cell setBlock:^(SigProxyFilerType filterType) {
            weakSelf.filterModel.filterType = filterType;
            [weakSelf saveFilterLocation];
        }];
        return cell;
    } else {
        FilterAddressCell *cell = (FilterAddressCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_FilterAddressCellID forIndexPath:indexPath];
        cell.accessoryType = UITableViewCellAccessoryNone;
        NSNumber *address = self.filterModel.addressList[indexPath.row-1];
        cell.detailLabel.text = [NSString stringWithFormat:@"Address: 0x%04X", address.intValue];
        [cell.deleteButton addAction:^(UIButton *button) {
            if (address.intValue == SigDataSource.share.curLocationNodeModel.address && weakSelf.filterModel.filterType == SigProxyFilerType_whitelist) {
                [weakSelf showTips:[NSString stringWithFormat:@"0x%04X is location address of iPhone, this address can not delete!", address.intValue]];
                return;
            }
            UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Warning" message:[NSString stringWithFormat:@"Are you sure delete address: 0x%04X?", address.intValue] preferredStyle:UIAlertControllerStyleAlert];
            [alertController addAction:[UIAlertAction actionWithTitle:@"Confirm" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
                TeLogDebug(@"点击确认");
                [weakSelf.filterModel.addressList removeObject:address];
                NSDictionary *dict = [weakSelf.filterModel getDictionaryOfSigProxyFilterModel];
                NSData *filterData = [LibTools getJSONDataWithDictionary:dict];
                [[NSUserDefaults standardUserDefaults] setValue:filterData forKey:kFilter];
                [[NSUserDefaults standardUserDefaults] synchronize];
                [weakSelf refreshSourceAndUI];
            }]];
            [alertController addAction:[UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
                TeLogDebug(@"点击取消");
                
            }]];
            [self presentViewController:alertController animated:YES completion:nil];
        }];
        return cell;

    }    
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    UITableViewCell *cell = [tableView cellForRowAtIndexPath:indexPath];
    cell.selected = NO;
    [tableView reloadData];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.filterModel.addressList.count + 1;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.row == 0) {
        return 135;
    } else {
        return 51.0;
    }
}

@end
