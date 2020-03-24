/********************************************************************************************************
 * @file     GroupDetailViewController.m 
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
//  GroupDetailViewController.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "GroupDetailViewController.h"
#import "HomeItemCell.h"
#import "HSLViewController.h"

@interface GroupDetailViewController()
@property (weak, nonatomic) IBOutlet UILabel *headerLabel;
@property (weak, nonatomic) IBOutlet UISlider *brightnessSlider;
@property (weak, nonatomic) IBOutlet UISlider *tempratureSlider;
@property (weak, nonatomic) IBOutlet UILabel *LumLabel;
@property (weak, nonatomic) IBOutlet UILabel *TempLabel;
@property (weak, nonatomic) IBOutlet UICollectionView *collectionView;
@property (nonatomic,strong) NSMutableArray <SigNodeModel *>*source;
@end

@implementation GroupDetailViewController

#pragma mark - xib event
- (IBAction)changeBrightness:(UISlider *)sender {
    self.LumLabel.text = [NSString stringWithFormat:@"Lum(%d)(at ele adr:0x%X):",(int)sender.value,self.model.groupAddress];
    __weak typeof(self) weakSelf = self;
    [DemoCommand changeBrightnessWithBrightness100:sender.value address:self.model.groupAddress responseMaxCount:(int)self.model.groupOnlineDevices.count ack:YES successCallback:^(UInt16 source, UInt16 destination, SigLightLightnessStatus * _Nonnull responseMessage) {
        [weakSelf.collectionView reloadData];
    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
        
    }];
}

- (IBAction)changeTempareture:(UISlider *)sender {
    self.TempLabel.text = [NSString stringWithFormat:@"Temp(%d)(at ele adr:0x%X):",(int)sender.value,self.model.groupAddress];
    __weak typeof(self) weakSelf = self;
    [DemoCommand changeTempratureWithTemprature100:sender.value address:self.model.groupAddress responseMaxCount:(int)self.model.groupOnlineDevices.count ack:YES successCallback:^(UInt16 source, UInt16 destination, SigLightCTLTemperatureStatus * _Nonnull responseMessage) {
        [weakSelf.collectionView reloadData];
    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
        
    }];
}

- (IBAction)clickHSL:(UIButton *)sender {
    HSLViewController *vc = (HSLViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_HSLViewControllerID];
    vc.isGroup = YES;
    vc.groupModel = self.model;
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)blockState{
    [super blockState];
//    __weak typeof(self) weakSelf = self;
//
//    //Power off and power on electricity directly connected devices, returns response of opcode 0x4E82 packets.
//    [self.ble.commandHandle setNotifyOnlineStatusCallBack:^(ResponseModel *m) {
//        [weakSelf.collectionView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
//    }];
//
//    //node notify publish status data when publish sig modelid is 0x1303.(if developer use other modelid to set publish, developer need fix the opcode.)
//    [self.ble.commandHandle setNotifyPublishStatusCallBack:^(ResponseModel *m) {
//        [weakSelf.collectionView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
//    }];
//
//    //this block will callback when publish timer check offline.
//    [self.ble.commandHandle setCheckOfflineCallBack:^(NSNumber *address) {
//        [weakSelf.collectionView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
//    }];
}

- (void)normalSetting{
    [super normalSetting];
    
    self.LumLabel.text = [NSString stringWithFormat:@"Lum(%d)(at ele adr:0x%X):",100,self.model.groupAddress];
    self.TempLabel.text = [NSString stringWithFormat:@"Temp(%d)(at ele adr:0x%X):",100,self.model.groupAddress];
    self.title = @"Group Setting";
    self.headerLabel.text = [NSString stringWithFormat:@"%@ Devices:",self.model.groupName];
    
    self.source = [[NSMutableArray alloc] init];
    for (SigNodeModel *model in self.model.groupDevices) {
        [self.source addObject:model];
    }
    [self.source sortUsingComparator:^NSComparisonResult(id  _Nonnull obj1, id  _Nonnull obj2) {
        return [(SigNodeModel *)obj1 address] > [(SigNodeModel *)obj2 address];
    }];
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (nonnull __kindof UICollectionViewCell *)collectionView:(nonnull UICollectionView *)collectionView cellForItemAtIndexPath:(nonnull NSIndexPath *)indexPath {
    HomeItemCell *item = [collectionView dequeueReusableCellWithReuseIdentifier:CellIdentifiers_HomeItemCellID forIndexPath:indexPath];
    SigNodeModel *model = self.source[indexPath.item];
    [item updateContent:model];
    return item;
}

- (NSInteger)collectionView:(nonnull UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    return self.source.count;
}

-(void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath{
    SigNodeModel *model = self.source[indexPath.item];
    if (model.state == DeviceStateOutOfLine || model.isKeyBindSuccess == NO) {
        [ShowTipsHandle.share show:Tip_DeviceOutline];
        [ShowTipsHandle.share delayHidden:2.0];
        return;
    }
    
    __weak typeof(self) weakSelf = self;
    [DemoCommand switchOnOffWithIsOn:!(model.state == DeviceStateOn) address:model.address responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigGenericOnOffStatus * _Nonnull responseMessage) {
        if (responseMessage) {
            [SigDataSource.share updateNodeStatusWithBaseMeshMessage:responseMessage source:source];
        }
        [weakSelf.collectionView reloadData];
    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
        
    }];
}

- (void)dealloc{
    TeLogDebug(@"");
}

@end
