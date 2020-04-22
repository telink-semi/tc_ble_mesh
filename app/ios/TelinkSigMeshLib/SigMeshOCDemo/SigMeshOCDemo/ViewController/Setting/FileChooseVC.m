/********************************************************************************************************
 * @file     FileChooseVC.m
 *
 * @brief    for TLSR chips
 *
 * @author     telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *             The information contained herein is confidential and proprietary property of Telink
 *              Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *             of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *             Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 *              Licensees are granted free, non-transferable use of the information in this
 *             file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/
//
//  FileChooseVC.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2019/7/4.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import "FileChooseVC.h"
#import "OTAFileSource.h"
#import "UIViewController+Message.h"
#import "ChooseBinCell.h"

@interface FileChooseVC ()<UITableViewDelegate,UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
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

- (void)showTips:(NSString *)message{
    __weak typeof(self) weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        [weakSelf showAlertSureWithTitle:@"Hits" message:message sure:^(UIAlertAction *action) {
            
        }];
    });
}

- (void)normalSetting{
    [super normalSetting];
    self.title = @"Choose json file";
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
    TeLogDebug(@"");
}

@end
