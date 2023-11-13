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
#import "UIViewController+Message.h"

@interface GroupDetailViewController()
@property (weak, nonatomic) IBOutlet UILabel *headerLabel;
@property (weak, nonatomic) IBOutlet UISlider *brightnessSlider;
@property (weak, nonatomic) IBOutlet UISlider *temperatureSlider;
@property (weak, nonatomic) IBOutlet UILabel *LumLabel;
@property (weak, nonatomic) IBOutlet UILabel *TempLabel;
@property (weak, nonatomic) IBOutlet UICollectionView *collectionView;
@property (weak, nonatomic) IBOutlet UIButton *hslButton;
@property (weak, nonatomic) IBOutlet UILabel *lumLevelLabel;
@property (weak, nonatomic) IBOutlet UIButton *lumLevelDecreaseButton;
@property (weak, nonatomic) IBOutlet UIButton *lumLevelIncreaseButton;
@property (weak, nonatomic) IBOutlet UILabel *tempLevelLabel;
@property (weak, nonatomic) IBOutlet UIButton *tempLevelDecreaseButton;
@property (weak, nonatomic) IBOutlet UIButton *tempLevelIncreaseButton;
@property (weak, nonatomic) IBOutlet UILabel *hueLevelLabel;
@property (weak, nonatomic) IBOutlet UIButton *hueLevelDecreaseButton;
@property (weak, nonatomic) IBOutlet UIButton *hueLevelIncreaseButton;
@property (weak, nonatomic) IBOutlet UILabel *satLevelLabel;
@property (weak, nonatomic) IBOutlet UIButton *satLevelDecreaseButton;
@property (weak, nonatomic) IBOutlet UIButton *satLevelIncreaseButton;
//add group level line UI
@property (weak, nonatomic) IBOutlet UIView *levelBackgroundView;
@property (nonatomic,strong) NSMutableArray <SigNodeModel *>*source;

@property (assign, nonatomic) BOOL hadChangeBrightness;
@property (assign, nonatomic) BOOL hasNextBrightness;
@property (assign, nonatomic) UInt8 nextBrightness;
@property (assign, nonatomic) BOOL hadChangeTempareture;
@property (assign, nonatomic) BOOL hasNextTempareture;
@property (assign, nonatomic) UInt8 nextTempareture;
@property (assign, nonatomic) UInt16 extendGroupAddress;

@end

@implementation GroupDetailViewController

#pragma mark - xib event

- (IBAction)clickLumLevelDecreaseButton:(UIButton *)sender {
    if (self.model.hasLumLevelCapability) {
        [self changeLevelWithGroupAddress:self.extendGroupAddress isAdd:NO];
    } else {
        [self showAlertSureWithTitle:@"Tips" message:[NSString stringWithFormat:@"The group \"%@\" hasn`t device that has lum level capability!", self.model.name] sure:nil];
    }
}

- (IBAction)clickLumLevelIncreaseButton:(UIButton *)sender {
    if (self.model.hasLumLevelCapability) {
        [self changeLevelWithGroupAddress:self.extendGroupAddress isAdd:YES];
    } else {
        [self showAlertSureWithTitle:@"Tips" message:[NSString stringWithFormat:@"The group \"%@\" hasn`t device that has lum level capability!", self.model.name] sure:nil];
    }
}

- (IBAction)clickTempLevelDecreaseButton:(UIButton *)sender {
    if (self.model.hasTempLevelCapability) {
        [self changeLevelWithGroupAddress:self.extendGroupAddress+1 isAdd:NO];
    } else {
        [self showAlertSureWithTitle:@"Tips" message:[NSString stringWithFormat:@"The group \"%@\" hasn`t device that has temperature level capability!", self.model.name] sure:nil];
    }
}

- (IBAction)clickTempLevelIncreaseButton:(UIButton *)sender {
    if (self.model.hasTempLevelCapability) {
        [self changeLevelWithGroupAddress:self.extendGroupAddress+1 isAdd:YES];
    } else {
        [self showAlertSureWithTitle:@"Tips" message:[NSString stringWithFormat:@"The group \"%@\" hasn`t device that has temperature level capability!", self.model.name] sure:nil];
    }
}

- (IBAction)clickHueLevelDecreaseButton:(UIButton *)sender {
    if (self.model.hasHueLevelCapability) {
        [self changeLevelWithGroupAddress:self.extendGroupAddress+2 isAdd:NO];
    } else {
        [self showAlertSureWithTitle:@"Tips" message:[NSString stringWithFormat:@"The group \"%@\" hasn`t device that has Hue level capability!", self.model.name] sure:nil];
    }
}

- (IBAction)clickHueLevelIncreaseButton:(UIButton *)sender {
    if (self.model.hasHueLevelCapability) {
        [self changeLevelWithGroupAddress:self.extendGroupAddress+2 isAdd:YES];
    } else {
        [self showAlertSureWithTitle:@"Tips" message:[NSString stringWithFormat:@"The group \"%@\" hasn`t device that has Hue level capability!", self.model.name] sure:nil];
    }
}

- (IBAction)clickSatLevelDecreaseButton:(UIButton *)sender {
    if (self.model.hasSatLevelCapability) {
        [self changeLevelWithGroupAddress:self.extendGroupAddress+3 isAdd:NO];
    } else {
        [self showAlertSureWithTitle:@"Tips" message:[NSString stringWithFormat:@"The group \"%@\" hasn`t device that has Saturation level capability!", self.model.name] sure:nil];
    }
}

- (IBAction)clickSatLevelIncreaseButton:(UIButton *)sender {
    if (self.model.hasSatLevelCapability) {
        [self changeLevelWithGroupAddress:self.extendGroupAddress+3 isAdd:YES];
    } else {
        [self showAlertSureWithTitle:@"Tips" message:[NSString stringWithFormat:@"The group \"%@\" hasn`t device that has Saturation level capability!", self.model.name] sure:nil];
    }
}

- (IBAction)changeBrightness:(UISlider *)sender {
    self.LumLabel.text = [NSString stringWithFormat:@"Lum(%d)(at group adr:0x%X):",(int)sender.value,self.model.intAddress];
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
    self.TempLabel.text = [NSString stringWithFormat:@"Temp(%d)(at group adr:0x%X):",(int)sender.value,self.model.intAddress];
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
    [DemoCommand changeTemperatureWithTemperature100:self.nextTempareture address:self.model.intAddress retryCount:0 responseMaxCount:0 ack:NO successCallback:^(UInt16 source, UInt16 destination, SigLightCTLTemperatureStatus * _Nonnull responseMessage) {
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

- (void)normalSetting{
    [super normalSetting];
    //add group level line UI
    //cornerRadius
    self.levelBackgroundView.layer.cornerRadius = 10;
    //borderWidth
    self.levelBackgroundView.layer.borderWidth = 2;
    //borderColor
    self.levelBackgroundView.layer.borderColor = [UIColor telinkButtonBlue].CGColor;
    //masksToBounds
    self.levelBackgroundView.layer.masksToBounds = YES;
    //backgroundColor
    self.levelBackgroundView.backgroundColor = [UIColor clearColor];

    self.hslButton.backgroundColor = UIColor.telinkButtonBlue;
    self.hadChangeBrightness = NO;
    self.hasNextBrightness = NO;
    self.nextBrightness = 0;
    self.hadChangeTempareture = NO;
    self.hasNextTempareture = NO;
    self.nextTempareture = 0;

    self.extendGroupAddress = [SigDataSource.share getExtendGroupAddressWithBaseGroupAddress:self.model.intAddress];
    self.lumLevelLabel.text = [NSString stringWithFormat:@"Lum Level(at group adr:0x%X):",self.extendGroupAddress];
    self.tempLevelLabel.text = [NSString stringWithFormat:@"Temp Level(at group adr:0x%X):",self.extendGroupAddress+1];
    self.hueLevelLabel.text = [NSString stringWithFormat:@"Hue Level(at group adr:0x%X):",self.extendGroupAddress+2];
    self.satLevelLabel.text = [NSString stringWithFormat:@"Sat Level(at group adr:0x%X):",self.extendGroupAddress+3];

    self.LumLabel.text = [NSString stringWithFormat:@"Lum(%d)(at group adr:0x%X):",self.model.groupBrightness,self.model.intAddress];
    self.TempLabel.text = [NSString stringWithFormat:@"Temp(%d)(at group adr:0x%X):",self.model.groupTempareture,self.model.intAddress];
    self.title = @"Group Setting";
    self.headerLabel.text = [NSString stringWithFormat:@"%@ Devices:",self.model.name];
    self.brightnessSlider.value = self.model.groupBrightness;
    self.temperatureSlider.value = self.model.groupTempareture;

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
    TelinkLogDebug(@"");
}

- (void)changeLevelWithGroupAddress:(UInt16)groupAddress isAdd:(BOOL)isAdd {
    SInt16 level = ceil(0xFFFF/10.0);//向上取整
    if (!isAdd) {
        level = -level;
    }
//    __weak typeof(self) weakSelf = self;
    [DemoCommand changeLevelWithAddress:groupAddress level:level responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigGenericLevelStatus * _Nonnull responseMessage) {
        TelinkLogDebug(@"control level success.");
//        dispatch_async(dispatch_get_main_queue(), ^{
//            //根据回包进行HSL的滑竿进行联动。
//            if (weakSelf.model.HSLAddresses.count > 0) {
//                weakSelf.colorModel = [weakSelf getColorWithHue:weakSelf.model.HSL_Hue100 saturation:weakSelf.model.HSL_Saturation100 ligntness:weakSelf.model.HSL_Lightness100];
//            }
//            [weakSelf.tableView reloadData];
//        });
    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
        
    }];
}


@end
