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

    self.title = @"Mesh Info";
    _ivIndex = SigDataSource.share.getIvIndexUInt32;
    [self refreshSourceAndUI];
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)refreshSourceAndUI {
    NSMutableArray *array = [NSMutableArray array];
    [array addObject:[NSString stringWithFormat:@"MeshUUID: %@",SigDataSource.share.meshUUID.uppercaseString]];
    [array addObject:[NSString stringWithFormat:@"IV Index: 0x%08X",(unsigned int)_ivIndex]];
    [array addObject:[NSString stringWithFormat:@"Sequence Number: 0x%06X",(unsigned int)SigDataSource.share.getSequenceNumberUInt32]];
    [array addObject:[NSString stringWithFormat:@"Local Address: 0x%04X",(unsigned int)SigDataSource.share.curLocationNodeModel.address]];
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
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)clickNetKeyListButton {
    NetKeyListVC *vc = [[NetKeyListVC alloc] init];
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
        [SDKLibCommand updateIvIndexWithKeyRefreshFlag:NO ivUpdateActive:YES networkId:SigDataSource.share.curNetkeyModel.networkId ivIndex:weakSelf.ivIndex + 1 usingNetworkKey:SigDataSource.share.curNetkeyModel];
        dispatch_async(dispatch_get_main_queue(), ^{
            [ShowTipsHandle.share delayHidden:0.5];
        });

    }];

}

- (IBAction)clickResetMesh:(UIButton *)sender {
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Warning" message:@"Wipe all mesh info?" preferredStyle:UIAlertControllerStyleAlert];
    [alertController addAction:[UIAlertAction actionWithTitle:@"Confirm" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        TeLogDebug(@"点击确认");
        //清除SigDataSource.share里面的所有参数（包括scanList、sequenceNumber、sequenceNumberOnDelegate），并随机生成新的默认参数。
        [SigDataSource.share resetMesh];
    }]];
    [alertController addAction:[UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
        TeLogDebug(@"点击取消");
        
    }]];
    [self presentViewController:alertController animated:YES completion:nil];
}

- (void)ivUpdateTimeout {
    dispatch_async(dispatch_get_main_queue(), ^{
        [ShowTipsHandle.share hidden];
        [self showTips:@"ivUpdate fail."];
    });
}

- (void)showTips:(NSString *)tips{
    dispatch_async(dispatch_get_main_queue(), ^{
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Hits" message:tips preferredStyle:UIAlertControllerStyleAlert];
        [alertController addAction:[UIAlertAction actionWithTitle:@"Sure" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            TeLogDebug(@"点击确认");
        }]];
        [self presentViewController:alertController animated:YES completion:nil];
    });
}


- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.row == 0 || indexPath.row == 1 || indexPath.row == 2 || indexPath.row == 3) {
        InfoButtonCell *cell = (InfoButtonCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_InfoButtonCellID forIndexPath:indexPath];
        cell.accessoryType = UITableViewCellAccessoryNone;
        cell.showLabel.text = _source[indexPath.row];
//        cell.showButton.hidden = YES;
        [cell.showButton removeFromSuperview];
        return cell;
    } else if (indexPath.row == 4 || indexPath.row == 5) {
        InfoNextCell *cell = (InfoNextCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_InfoNextCellID forIndexPath:indexPath];
        cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
        cell.showLabel.text = _source[indexPath.row];
        return cell;
    } else  if (indexPath.row == 6) {
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
    if (indexPath.row == 4) {
        [self clickNetKeyListButton];
    } else if (indexPath.row == 5) {
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
