/********************************************************************************************************
 * @file     MeshInfoVC.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/1/24
 *
 * @par     Copyright (c) [2021], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#import "InfoNextCell.h"
#import "InfoButtonCell.h"

@interface MeshInfoVC ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <NSString *>*source;
@property (assign, nonatomic) UInt32 ivIndex;
@end

@implementation MeshInfoVC

- (void)viewDidLoad {
    [super viewDidLoad];
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_InfoNextCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_InfoNextCellID];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_InfoButtonCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_InfoButtonCellID];
    //add rightBarButtonItem for edit mesh name
    UIBarButtonItem *item = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"ic_edit_bar"] style:UIBarButtonItemStylePlain target:self action:@selector(clickEditMeshName)];
    self.navigationItem.rightBarButtonItem = item;
    [self setTitle:@"Mesh Info" subTitle:self.network.meshName];
    _ivIndex = self.network.getIvIndexUInt32;
    [self refreshSourceAndUI];
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
}

- (void)refreshSourceAndUI {
    NSMutableArray *array = [NSMutableArray array];
    [array addObject:[NSString stringWithFormat:@"Mesh Name: %@",self.network.meshName]];
    [array addObject:[NSString stringWithFormat:@"Mesh UUID: %@",self.network.meshUUID.uppercaseString]];
    [array addObject:[NSString stringWithFormat:@"IV Index: 0x%08X",(unsigned int)_ivIndex]];
    [array addObject:[NSString stringWithFormat:@"Sequence Number: 0x%06X",(unsigned int)self.network.getSequenceNumberUInt32]];
    [array addObject:[NSString stringWithFormat:@"Local Address: 0x%04X",(unsigned int)self.network.curLocationNodeModel.address]];
    [array addObject:@"NetKey List"];
    [array addObject:@"AppKey List"];
    //==========test==========//
//    [array addObject:@"IvUpdate"];
    //==========test==========//
    _source = array;
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

- (void)clickIvUpdate:(UIButton *)sender {
    if (!SigBearer.share.isOpen) {
        [self showTips:@"Mesh is disconnected."];
        return;
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        NSString *t = @"sending ivUpdate...";
        [ShowTipsHandle.share show:t];
    });

    __weak typeof(self) weakSelf = self;
    NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
    [operationQueue addOperationWithBlock:^{
        //强行使用weakSelf.ivIndex + 1和sequenceNumber=0x01进行发包。
        [SDKLibCommand updateIvIndexWithKeyRefreshFlag:NO ivUpdateActive:YES networkId:weakSelf.network.curNetkeyModel.networkId ivIndex:weakSelf.ivIndex + 1 usingNetworkKey:weakSelf.network.curNetkeyModel];
        dispatch_async(dispatch_get_main_queue(), ^{
            [ShowTipsHandle.share delayHidden:0.5];
        });

    }];

}

- (void)ivUpdateTimeout {
    dispatch_async(dispatch_get_main_queue(), ^{
        [ShowTipsHandle.share hidden];
        [self showTips:@"ivUpdate fail."];
    });
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.row <= 4) {
        InfoButtonCell *cell = (InfoButtonCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_InfoButtonCellID forIndexPath:indexPath];
        cell.accessoryType = UITableViewCellAccessoryNone;
        cell.showLabel.text = _source[indexPath.row];
//        cell.showButton.hidden = YES;
        [cell.showButton removeFromSuperview];
        return cell;
    } else if (indexPath.row == 5 || indexPath.row == 6) {
        InfoNextCell *cell = (InfoNextCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_InfoNextCellID forIndexPath:indexPath];
        cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
        cell.showLabel.text = _source[indexPath.row];
        return cell;
    } else  if (indexPath.row == 7) {
        InfoButtonCell *cell = (InfoButtonCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_InfoButtonCellID forIndexPath:indexPath];
        cell.accessoryType = UITableViewCellAccessoryNone;
        cell.showLabel.text = _source[indexPath.row];
        cell.showButton.hidden = NO;
        [cell.showButton setTitle:@"+1" forState:UIControlStateNormal];
        [cell.showButton addTarget:self action:@selector(clickIvUpdate:) forControlEvents:UIControlEventTouchUpInside];
        return cell;
    }
    return [[UITableViewCell alloc] init];
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.row == 5) {
        [self clickNetKeyListButton];
    } else if (indexPath.row == 6) {
        [self clickAppKeyListButton];
   }
    UITableViewCell *cell = [tableView cellForRowAtIndexPath:indexPath];
    cell.selected = NO;
    [tableView reloadData];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.source.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 51.0;
}

@end
