/********************************************************************************************************
 * @file     FileChooseVC.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/7/4
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

#import "FileChooseVC.h"
#import "OTAFileSource.h"
#import "UIViewController+Message.h"
#import "ChooseBinCell.h"

@interface FileChooseVC ()<UITableViewDelegate,UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIButton *importButton;
@property (nonatomic, assign) int selectIndex;
@property (nonatomic,strong) NSMutableArray <NSString *>*source;

@end

@implementation FileChooseVC

- (IBAction)clickImportFile:(UIButton *)sender {
    if (self.selectIndex == -1) {
        [self showTips:@"please choose json file!"];
        return;
    }
    NSString *jsonName = self.source[self.selectIndex];
    NSData *jsonData = [OTAFileSource.share getDataWithMeshJsonName:jsonName];
    if (jsonData == nil || jsonData.length == 0) {
        [self showTips:@"this json file is empty, please choose other!"];
        return;
    }

    if (self.backJsonData) {
        self.backJsonData(jsonData,jsonName);
    }

    [self.navigationController popViewControllerAnimated:YES];
}

- (void)normalSetting{
    [super normalSetting];
    self.title = @"Choose json file";
    self.importButton.backgroundColor = UIColor.telinkButtonBlue;
    self.selectIndex = -1;
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_ChooseBinCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_ChooseBinCellID];
    self.source = [[NSMutableArray alloc] initWithArray:OTAFileSource.share.getAllMeshJsonFile];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    ChooseBinCell *cell = (ChooseBinCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_ChooseBinCellID forIndexPath:indexPath];
    NSString *binString = self.source[indexPath.row];
    cell.nameLabel.text = binString;
    cell.selectButton.selected = indexPath.row == self.selectIndex;
    return cell;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.source.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 44.0;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.row == self.selectIndex) {
        self.selectIndex = -1;
    } else {
        self.selectIndex = (int)indexPath.row;
    }
    [self.tableView reloadData];
}

-(void)dealloc{
    TelinkLogDebug(@"");
}

@end
