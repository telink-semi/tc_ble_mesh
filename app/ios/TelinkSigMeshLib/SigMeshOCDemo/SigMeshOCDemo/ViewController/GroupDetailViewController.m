/********************************************************************************************************
 * @file     GroupDetailViewController.m 
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/7/31
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

@property (assign, nonatomic) BOOL hadChangeBrightness;
@property (assign, nonatomic) BOOL hasNextBrightness;
@property (assign, nonatomic) UInt8 nextBrightness;
@property (assign, nonatomic) BOOL hadChangeTempareture;
@property (assign, nonatomic) BOOL hasNextTempareture;
@property (assign, nonatomic) UInt8 nextTempareture;

@end

@implementation GroupDetailViewController

#pragma mark - xib event
- (IBAction)changeBrightness:(UISlider *)sender {
    self.LumLabel.text = [NSString stringWithFormat:@"Lum(%d)(at ele adr:0x%X):",(int)sender.value,self.model.intAddress];
    self.model.groupBrightness = sender.value;
    if (!self.hadChangeBrightness) {
        self.nextBrightness = sender.value;
        [self changeBrightness];
    } else {
        self.hasNextBrightness = YES;
        self.nextBrightness = sender.value;
    }
}

- (void)changeBrightnessFinish {
    self.hadChangeBrightness = NO;
    if (self.hasNextBrightness) {
        [self changeBrightness];
    }
}

- (void)changeBrightness {
    self.hadChangeBrightness = YES;
    self.hasNextBrightness = NO;
//    __weak typeof(self) weakSelf = self;
    [DemoCommand changeBrightnessWithBrightness100:self.nextBrightness address:self.model.intAddress retryCount:0 responseMaxCount:0 ack:NO successCallback:^(UInt16 source, UInt16 destination, SigLightLightnessStatus * _Nonnull responseMessage) {
//        [weakSelf.collectionView reloadData];
    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
        
    }];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(changeBrightnessFinish) object:nil];
        [self performSelector:@selector(changeBrightnessFinish) withObject:nil afterDelay:kCommandInterval];
    });
}

- (IBAction)changeTempareture:(UISlider *)sender {
    self.TempLabel.text = [NSString stringWithFormat:@"Temp(%d)(at ele adr:0x%X):",(int)sender.value,self.model.intAddress];
    self.model.groupTempareture = sender.value;
    if (!self.hadChangeTempareture) {
        self.nextTempareture = sender.value;
        [self changeTempareture];
    } else {
        self.hasNextTempareture = YES;
        self.nextTempareture = sender.value;
    }
}

- (void)changeTemparetureFinish {
    self.hadChangeTempareture = NO;
    if (self.hasNextTempareture) {
        [self changeTempareture];
    }
}

- (void)changeTempareture {
    self.hadChangeTempareture = YES;
    self.hasNextTempareture = NO;
//    __weak typeof(self) weakSelf = self;
    [DemoCommand changeTempratureWithTemprature100:self.nextTempareture address:self.model.intAddress retryCount:0 responseMaxCount:0 ack:NO successCallback:^(UInt16 source, UInt16 destination, SigLightCTLTemperatureStatus * _Nonnull responseMessage) {
//        [weakSelf.collectionView reloadData];
    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
        
    }];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(changeTemparetureFinish) object:nil];
        [self performSelector:@selector(changeTemparetureFinish) withObject:nil afterDelay:kCommandInterval];
    });
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
    self.hadChangeBrightness = NO;
    self.hasNextBrightness = NO;
    self.nextBrightness = 0;
    self.hadChangeTempareture = NO;
    self.hasNextTempareture = NO;
    self.nextTempareture = 0;

    self.LumLabel.text = [NSString stringWithFormat:@"Lum(%d)(at ele adr:0x%X):",self.model.groupBrightness,self.model.intAddress];
    self.TempLabel.text = [NSString stringWithFormat:@"Temp(%d)(at ele adr:0x%X):",self.model.groupTempareture,self.model.intAddress];
    self.title = @"Group Setting";
    self.headerLabel.text = [NSString stringWithFormat:@"%@ Devices:",self.model.name];
    self.brightnessSlider.value = self.model.groupBrightness;
    self.tempratureSlider.value = self.model.groupTempareture;

    self.source = [[NSMutableArray alloc] init];
    NSArray *groupDevices = [NSArray arrayWithArray:self.model.groupDevices];
    for (SigNodeModel *model in groupDevices) {
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
