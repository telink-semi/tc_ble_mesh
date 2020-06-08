/********************************************************************************************************
* @file     ChooseOOBFileVC.m
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
//  ChooseOOBFileVC.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2020/5/12.
//  Copyright © 2020 Telink. All rights reserved.
//

#import "ChooseOOBFileVC.h"
#import "ChooseBinCell.h"
#import "UIViewController+Message.h"

@interface ChooseOOBFileVC ()<UITableViewDelegate,UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, assign) int selectIndex;
@property (nonatomic,strong) NSMutableArray <NSString *>*source;

@end

@implementation ChooseOOBFileVC

- (IBAction)clickImportFile:(UIButton *)sender {
    if (self.selectIndex == -1) {
        [self showTips:@"please choose oob file!"];
        return;
    }
    NSString *fileName = self.source[self.selectIndex];
    NSData *fileData = [self getDataWithFileName:fileName];
    if (fileData == nil || fileData.length == 0) {
        [self showTips:@"this oob file is empty, please choose other!"];
        return;
    }
        
    if (self.backOobFileInfo) {
        self.backOobFileInfo(fileData,fileName);
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
    self.title = @"File Selector";
    self.selectIndex = -1;
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_ChooseBinCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_ChooseBinCellID];
    
    self.source = [NSMutableArray array];
    //搜索Documents(通过iTunes File 加入的文件需要在此搜索)
    NSFileManager *mang = [NSFileManager defaultManager];
    NSError *error = nil;
    NSString *fileLocalPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject];
    NSArray *fileNames = [mang contentsOfDirectoryAtPath:fileLocalPath error:&error];
    for (NSString *path in fileNames) {
        //本地512k的存储文件名为“test.bin”，不可用于OTA
        if ([path containsString:@".txt"]) {
            [self.source addObject:path];
        }
    }
}

- (NSData *)getDataWithFileName:(NSString *)fileName{
    //通过iTunes File 加入的文件
    NSString *fileLocalPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject];
    fileLocalPath = [NSString stringWithFormat:@"%@/%@",fileLocalPath,fileName];
    NSError *err = nil;
    NSFileHandle *fileHandle = [NSFileHandle fileHandleForReadingFromURL:[NSURL URLWithString:fileLocalPath] error:&err];
    NSData *data = fileHandle.readDataToEndOfFile;
    return data;
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
