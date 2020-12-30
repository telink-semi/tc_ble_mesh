/********************************************************************************************************
 * @file     SingleOTAViewController.m 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
//
//  SingleOTAViewController.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "SingleOTAViewController.h"
#import "OTAFileSource.h"
#import "UIViewController+Message.h"
#import "ChooseBinCell.h"

@interface SingleOTAViewController()<UITableViewDelegate,UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIButton *otaButton;
@property (weak, nonatomic) IBOutlet UILabel *otaTipsLabel;
@property (nonatomic,strong) NSMutableArray <NSString *>*source;
@property (nonatomic, assign) int selectIndex;
@property (nonatomic, assign) BOOL OTAing;
@property (nonatomic, strong) NSData *localData;
@property (nonatomic, strong) UIColor *normalColor;
@property (nonatomic, strong) UIColor *unableColor;

@end

@implementation SingleOTAViewController

- (IBAction)clickStartOTA:(UIButton *)sender {
    if (self.OTAing) {
        return;
    }
    if (self.selectIndex == -1) {
        [self showTips:@"Please choose Bin file."];
        return;
    }else{
        self.localData = [OTAFileSource.share getDataWithBinName:self.source[self.selectIndex]];
        if (self.localData == nil || self.localData.length == 0) {
            [self showTips:@"APP can't load this Bin file."];
            return;
        }
    }
    UInt16 pid = [OTAFileSource.share getPidWithOTAData:self.localData];
    if (pid != [LibTools uint16From16String:self.model.pid]) {
        NSString *msg = [NSString stringWithFormat:@"The PID of node is different from the PID of Bin file, are you sure still OTA this node?"];
        __weak typeof(self) weakSelf = self;
        [self showAlertSureAndCancelWithTitle:@"Hits" message:msg sure:^(UIAlertAction *action) {
            [weakSelf otaAction];
        } cancel:^(UIAlertAction *action) {
            
        }];
    } else {
        [self otaAction];
    }
}

- (void)otaAction {
    TeLogVerbose(@"clickStartOTA");
    self.OTAing = YES;
    self.otaButton.backgroundColor = self.unableColor;
    self.tableView.userInteractionEnabled = NO;
    [self showOTATips:@"start connect..."];
    
    __weak typeof(self) weakSelf = self;
    BOOL result = [OTAManager.share startOTAWithOtaData:self.localData models:@[self.model] singleSuccessAction:^(SigNodeModel *device) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [weakSelf otaSuccessAction];
        });
    } singleFailAction:^(SigNodeModel *device) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [weakSelf otaFailAction];
        });
    } singleProgressAction:^(float progress) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [weakSelf showOTAProgress:progress];
        });
    } finishAction:^(NSArray<SigNodeModel *> *successModels, NSArray<SigNodeModel *> *fileModels) {
        TeLogDebug(@"");
    }];
    TeLogDebug(@"result = %d",result);
}

- (void)normalSetting{
    [super normalSetting];
    self.selectIndex = -1;
    self.OTAing = NO;
    self.normalColor = kDefultColor;
    self.unableColor = [UIColor colorWithRed:185.0/255.0 green:185.0/255.0 blue:185.0/255.0 alpha:1.0];
    self.title = [NSString stringWithFormat:@"OTA Pid:0x%X Vid:0x%X",[LibTools uint16From16String:self.model.pid],[LibTools uint16From16String:self.model.vid]];
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_ChooseBinCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_ChooseBinCellID];
    self.source = [[NSMutableArray alloc] initWithArray:OTAFileSource.share.getAllBinFile];
}

- (void)viewWillDisappear:(BOOL)animated{
    [super viewWillDisappear:animated];
    [OTAManager.share stopOTA];
}

- (void)showOTAProgress:(float)progress{
    NSString *tips = [NSString stringWithFormat:@"OTA:%.1f%%", progress];
    if (progress == 100) {
        tips = [tips stringByAppendingString:@",reboot..."];
    }
    [self showOTATips:tips];
}

- (void)showTips:(NSString *)message{
    [self showAlertSureWithTitle:@"Hits" message:message sure:^(UIAlertAction *action) {
        
    }];
}

- (void)showOTATips:(NSString *)message{
    self.otaTipsLabel.text = message;
}

- (void)otaSuccessAction{
    self.OTAing = NO;
    [self showTips:@"OTA success"];
    self.otaButton.backgroundColor = self.normalColor;
    self.tableView.userInteractionEnabled = YES;
    [self showOTATips:@"OTA success"];
    [SigBearer.share startMeshConnectWithComplete:nil];
    TeLogVerbose(@"otaSuccess");
}

- (void)otaFailAction{
    self.OTAing = NO;
    [self showTips:@"OTA fail"];
    self.otaButton.backgroundColor = self.normalColor;
    self.tableView.userInteractionEnabled = YES;
    [self showOTATips:@"OTA fail"];
    [SigBearer.share startMeshConnectWithComplete:nil];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self];
    });
    TeLogVerbose(@"otaFail");
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    ChooseBinCell *cell = (ChooseBinCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_ChooseBinCellID forIndexPath:indexPath];
    NSString *binString = self.source[indexPath.row];
    NSData *data = [OTAFileSource.share getDataWithBinName:binString];
    UInt16 vid = [OTAFileSource.share getVidWithOTAData:data];
    cell.nameLabel.text = [NSString stringWithFormat:@"%@   PID:0x%X VID:%c%c",binString,[OTAFileSource.share getPidWithOTAData:data],vid&0xff,(vid>>8)&0xff];//vid显示两个字节的ASCII
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
