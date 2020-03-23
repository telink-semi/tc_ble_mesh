/********************************************************************************************************
 * @file     MeshOTAVC.m
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
//  MeshOTAVC.m
//  TelinkBlueDemo
//
//  Created by Arvin on 2018/4/17.
//  Copyright © 2018年 Green. All rights reserved.
//

#import "MeshOTAVC.h"
#import "MeshOTAItemCell.h"
#import "UIButton+extension.h"
//#import "MeshOTAManager.h"
#import "HomeViewController.h"
#import "OTAFileSource.h"

/**
 Attention: more detail about mesh OTA can look document Mesh_Firmware_update_20180228_d05r05.pdf
 */
@interface MeshOTAVC ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UIButton *startButton;
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <SigNodeModel *>*allItemArray;
@property (nonatomic, strong) NSMutableArray *binStringArray;
@property (nonatomic, strong) NSMutableArray <SigNodeModel *>*selectItemArray;
@property (nonatomic, assign) NSInteger binIndex;
@property (nonatomic, strong) NSMutableDictionary *allItemVIDDict;

@end

@implementation MeshOTAVC

- (void)viewDidLoad {
    [super viewDidLoad];
     
    self.title = @"Mesh OTA";
    
    UIView *footerView = [[UIView alloc] initWithFrame:CGRectZero];
    self.tableView.tableFooterView = footerView;
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_MeshOTAItemCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_MeshOTAItemCellID];

    self.binIndex = -1;
    self.selectItemArray = [NSMutableArray array];
    self.allItemArray = [[NSMutableArray alloc] initWithArray:SigDataSource.share.curNodes];
    self.binStringArray = [NSMutableArray arrayWithArray:OTAFileSource.share.getAllBinFile];
    self.allItemVIDDict = [NSMutableDictionary dictionary];
    
//    if ([MeshOTAManager share].isMeshOTAing) {
//        [self userAbled:NO];
//    }
    [MeshOTAManager.share saveIsMeshOTAing:NO];
}

- (void)updateNodeModelVidWithAddress:(UInt16)address vid:(UInt16)vid{
    [self.allItemVIDDict setObject:@(vid) forKey:@(address)];
    
    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
    if (node) {
        if ([LibTools uint16From16String:node.vid] != vid) {
            node.vid = [NSString stringWithFormat:@"%04X",vid];
            [SigDataSource.share saveLocationData];
        }
    }
    [self.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)viewWillDisappear:(BOOL)animated{
    [super viewWillDisappear:animated];
    
    //界面可手动返回的情况，需要手动调用stopMeshOTA
    if (MeshOTAManager.share.isMeshOTAing) {
        [MeshOTAManager.share stopMeshOTA];
    }
}

- (void)viewDidDisappear:(BOOL)animated{
    [super viewDidDisappear:animated];
}

- (void)setStartButtonEnable:(BOOL)enable{
    self.startButton.backgroundColor = enable ? kDefultColor : kDefultUnableColor;
}

- (IBAction)clickGetFwInfo:(UIButton *)sender {
    [self.allItemVIDDict removeAllObjects];
    
    /*返回的status是：FW_INFO_STATUS (0x02B6)
     数据结构是：
     fw_info_status_t，  即
     u16 cid，  (vendor id)
     u16 pid,   (设备类型)
     u16 vid    (版本id)*/
    __weak typeof(self) weakSelf = self;
    [Bluetooth.share.commandHandle getFwInfoWithAddress:ADR_ALL_NODES Completation:^(ResponseModel *m) {
        //f00c0004 000100b6 02110201 003238
        UInt16 cid = 0,pid = 0,vid = 0;
        Byte *pu = (Byte *)[m.rspData bytes];
        if (m.rspData.length >= 11) memcpy(&cid, pu + 9, 2);
        if (m.rspData.length >= 13) memcpy(&pid, pu + 11, 2);
        if (m.rspData.length >= 15) memcpy(&vid, pu + 13, 2);
        
        TeLog(@"getFwInfoWithAddress=%@,cid=%d,pid=%d,vid=%d",m.rspData,cid,pid,vid);
        [weakSelf updateNodeModelVidWithAddress:m.address vid:vid];
    }];
}

- (IBAction)clickStartMeshOTA:(UIButton *)sender {
    if (![MeshOTAManager share].isMeshOTAing) {
        [self startMeshOTA];
    }else{
        [self showTips:@"Device is meshOTAing, needn't call repeat."];
    }
}

- (void)showTips:(NSString *)tips{
    dispatch_async(dispatch_get_main_queue(), ^{
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Hits" message:tips preferredStyle:UIAlertControllerStyleAlert];
        [alertController addAction:[UIAlertAction actionWithTitle:@"Sure" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            TeLog(@"点击确认");
        }]];
        [self presentViewController:alertController animated:YES completion:nil];
    });
}

#pragma mark - UITableViewDataSource
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 2;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return section == 0 ? self.allItemArray.count+1 : self.binStringArray.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:NSStringFromClass(MeshOTAItemCell.class) forIndexPath:indexPath];
    [self configureCell:cell forRowAtIndexPath:indexPath];
    
    return cell;
}

- (void)configureCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath{
    MeshOTAItemCell *itemCell = (MeshOTAItemCell *)cell;
    __weak typeof(self) weakSelf = self;
    
    if (indexPath.section == 0) {
        if (indexPath.row == 0) {
            itemCell.titleLabel.text = @"choose all";
            itemCell.selectButton.selected = self.selectItemArray.count == self.allItemArray.count;
            [itemCell.selectButton addAction:^(UIButton *button) {
                if (!button.selected) {
                    for (SigNodeModel *model in weakSelf.allItemArray) {
                        if (model.state != DeviceStateOutOfLine) {
                            if (![weakSelf.selectItemArray containsObject:model]) {
                                [weakSelf.selectItemArray addObject:model];
                            }
                        }
                    }
                } else {
                    [weakSelf.selectItemArray removeAllObjects];
                }
                [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
            }];
        }else{
            SigNodeModel *model = self.allItemArray[indexPath.row-1];
            int vid = 0;
            if ([self.allItemVIDDict.allKeys containsObject:@(model.address)]) {
                vid = [self.allItemVIDDict[@(model.address)] intValue];
            }
            itemCell.titleLabel.text = [NSString stringWithFormat:@"adr:0x%X PID:0x%@    VID:%c%c",model.address,model.pid,vid&0xff,(vid>>8)&0xff];//显示两个字节的ASCII
            if (self.selectItemArray.count > 0) {
                itemCell.selectButton.selected = [self.selectItemArray containsObject:model];
            } else {
                itemCell.selectButton.selected = NO;
            }
            [itemCell.selectButton addAction:^(UIButton *button) {
                if (model.state != DeviceStateOutOfLine) {
                    if ([weakSelf.selectItemArray containsObject:model]) {
                        [weakSelf.selectItemArray removeObject:model];
                    }else{
                        [weakSelf.selectItemArray addObject:model];
                    }
                    [weakSelf.tableView reloadData];
                }
            }];
        }
    } else {
        NSString *binString = self.binStringArray[indexPath.row];
        itemCell.titleLabel.text = binString;
        itemCell.selectButton.selected = indexPath.row == _binIndex;
        [itemCell.selectButton addAction:^(UIButton *button) {
            weakSelf.binIndex = indexPath.row;
            [weakSelf.tableView reloadData];
        }];
    }
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section{
    if (section == 0) {
        return @"Device list";
    } else {
        return @"OTA file";
    }
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 44;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.section == 0) {
        if (indexPath.row == 0) {
            __weak typeof(self) weakSelf = self;
            MeshOTAItemCell *itemCell = [tableView cellForRowAtIndexPath:indexPath];
            if (!itemCell.selectButton.selected) {
                for (SigNodeModel *model in self.allItemArray) {
                    if (model.state != DeviceStateOutOfLine) {
                        if (![weakSelf.selectItemArray containsObject:model]) {
                            [weakSelf.selectItemArray addObject:model];
                        }
                    }
                }
            } else {
                [weakSelf.selectItemArray removeAllObjects];
            }
        }else{
            SigNodeModel *model = self.allItemArray[indexPath.row-1];
            if (model.state != DeviceStateOutOfLine) {
                if ([self.selectItemArray containsObject:model]) {
                    [self.selectItemArray removeObject:model];
                }else{
                    [self.selectItemArray addObject:model];
                }
            }
        }
        [self.tableView reloadData];
    } else {
        if (_binIndex != indexPath.row) {
            _binIndex = indexPath.row;
            [self.tableView reloadData];
        }
    }
}

- (void)startMeshOTA{
    if (self.selectItemArray.count == 0) {
        [self showTips:@"Please choose some devices for mesh OTA."];
        return;
    }
    if (self.binIndex < 0) {
        [self showTips:@"Please choose the bin file for mesh OTA."];
        return;
    }
    
    [self userAbled:NO];
    NSData *data = [OTAFileSource.share getDataWithBinName:self.binStringArray[self.binIndex]];
    if (data == nil || data.length == 0) {
        [self showTips:@"APP can't load this Bin file."];
        return;
    }
    NSMutableArray *tem = [NSMutableArray array];
    for (SigNodeModel *model in self.selectItemArray) {
        [tem addObject:@(model.address)];
    }
    
    __weak typeof(self) weakSelf = self;
    [MeshOTAManager.share startMeshOTAWithLocationAddress:SigDataSource.share.curLocationNodeModel.address cid:0x0211 deviceAddresses:tem otaData:data progressHandle:^(NSInteger progress) {
//        TeLog(@"progress=%d",progress);
        dispatch_async(dispatch_get_main_queue(), ^{
            NSString *t = [NSString stringWithFormat:@"mesh ota... progress:%ld%%", (long)progress];
            [ShowTipsHandle.share show:t];
            if (progress == 100) {
                [weakSelf userAbled:YES];
                [ShowTipsHandle.share delayHidden:2.0];
                [Bluetooth.share setNormalState];
            }
        });
    } finishHandle:^(NSArray<NSNumber *> *successAddresses, NSArray<NSNumber *> *failAddresses) {
        NSString *tip = [NSString stringWithFormat:@"Mesh ota finish, success:%ld,fail:%ld", (long)successAddresses.count, (long)failAddresses.count];
        dispatch_async(dispatch_get_main_queue(), ^{
            [weakSelf userAbled:YES];
            [ShowTipsHandle.share delayHidden:0];
            [weakSelf showTips:tip];
        });
    } errorHandle:^(NSError *error) {
        TeLog(@"error = %@",error);
    }];
}

- (void)userAbled:(BOOL)able{
    self.startButton.enabled = able;
    self.tableView.userInteractionEnabled = able;
    [self setStartButtonEnable:able];
}

-(void)dealloc{
    TeLog(@"%s",__func__);
}

@end
