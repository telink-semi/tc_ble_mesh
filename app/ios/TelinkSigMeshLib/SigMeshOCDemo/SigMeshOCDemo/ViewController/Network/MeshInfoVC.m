/********************************************************************************************************
 * @file     MeshInfoVC.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/1/24
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

#import "MeshInfoVC.h"
#import "OOBListVC.h"
#import "AppKeyListVC.h"
#import "NetKeyListVC.h"
#import "SettingDetailItemCell.h"

@interface MeshInfoVC ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <NSString *>*iconArray;
@property (nonatomic, strong) NSMutableArray <NSString *>*titleArray;
@property (nonatomic, strong) NSMutableArray <NSString *>*valueArray;
@end

@implementation MeshInfoVC

- (void)viewDidLoad {
    [super viewDidLoad];
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(SettingDetailItemCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(SettingDetailItemCell.class)];
    //init rightBarButtonItem
    UIBarButtonItem *item = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"ic_edit_bar"] style:UIBarButtonItemStylePlain target:self action:@selector(clickEditMeshName)];
    self.navigationItem.rightBarButtonItem = item;
    [self setTitle:@"Mesh Info" subTitle:self.network.meshName];
}

- (void)clickEditMeshName {
    __weak typeof(self) weakSelf = self;
    UIAlertController *inputAlertController = [UIAlertController alertControllerWithTitle:@"Update Mesh Name" message:@"please input content" preferredStyle:UIAlertControllerStyleAlert];
    [inputAlertController addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
        textField.placeholder = @"please input new mesh name";
        textField.text = weakSelf.network.meshName;
    }];
    [inputAlertController addAction:[UIAlertAction actionWithTitle:@"CANCEL" style:UIAlertActionStyleDefault handler:nil]];
    [inputAlertController addAction:[UIAlertAction actionWithTitle:@"CONFIRM" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        TelinkLogDebug(@"输入mesh name完成");
        UITextField *addressTF = inputAlertController.textFields.firstObject;
        //check input length
        if (addressTF.text.length == 0) {
            // need input any string
            [weakSelf showTips:@"Please input new mesh name."];
            return;
        }
        weakSelf.network.meshName = addressTF.text;
        [weakSelf setTitle:@"Mesh Info" subTitle:weakSelf.network.meshName];
        [weakSelf refreshSourceAndUI];
        [weakSelf showTips:@"update name success"];
        if (weakSelf.backNetwork) {
            weakSelf.backNetwork(weakSelf.network);
        }
    }]];
    [self presentViewController:inputAlertController animated:YES completion:nil];
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
    [self refreshSourceAndUI];
}

- (void)refreshSourceAndUI {
    self.iconArray = [NSMutableArray arrayWithArray:@[@"ic_name", @"ic_id", @"ic_iv_index", @"ic_sequence_number", @"ic_local_address", @"ic_keys", @"ic_keys"]];
    self.titleArray = [NSMutableArray arrayWithArray:@[@"Mesh Name", @"Mesh UUID", @"IV Index", @"Sequence Number", @"Local Address", @"Net Keys", @"App Keys"]];
    self.valueArray = [NSMutableArray arrayWithArray:@[self.network.meshName, self.network.meshUUID.uppercaseString, [NSString stringWithFormat:@"0x%08X",(unsigned int)self.network.getIvIndexUInt32], [NSString stringWithFormat:@"0x%06X",(unsigned int)self.network.getSequenceNumberUInt32], [NSString stringWithFormat:@"0x%04X",(unsigned int)self.network.curLocationNodeModel.address], [NSString stringWithFormat:@"%lu Net Keys", (unsigned long)self.network.netKeys.count], [NSString stringWithFormat:@"%lu App Keys", (unsigned long)self.network.appKeys.count]]];
    [self.tableView reloadData];
}

- (void)clickAppKeyListButton {
    AppKeyListVC *vc = [[AppKeyListVC alloc] init];
    vc.network = self.network;
    vc.backNetwork = self.backNetwork;
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)clickNetKeyListButton {
    NetKeyListVC *vc = [[NetKeyListVC alloc] init];
    vc.network = self.network;
    vc.backNetwork = self.backNetwork;
    [self.navigationController pushViewController:vc animated:YES];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    SettingDetailItemCell *cell = (SettingDetailItemCell *)[tableView dequeueReusableCellWithIdentifier:NSStringFromClass(SettingDetailItemCell.class) forIndexPath:indexPath];
    cell.iconImageView.image = [UIImage imageNamed:self.iconArray[indexPath.row]];
    cell.nameLabel.text = self.titleArray[indexPath.row];
    cell.detailLabel.text = self.valueArray[indexPath.row];
    cell.nextImageView.hidden = indexPath.row < 5;
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.row == 5) {
        [self clickNetKeyListButton];
    } else if (indexPath.row == 6) {
        [self clickAppKeyListButton];
   }
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.iconArray.count;
}

@end
