/********************************************************************************************************
 * @file     DeviceControlViewController.m 
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
//  DeviceControlViewController.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2018/10/10.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "DeviceControlViewController.h"
#import "DTColorPickerImageView.h"
#import "ColorManager.h"
#import "OnOffItemCell.h"
#import "NSString+calculate.h"

@interface DeviceControlViewController ()<DTColorPickerImageViewDelegate,UICollectionViewDataSource>
@property (weak, nonatomic) IBOutlet UIScrollView *detailScrollView;
@property (weak, nonatomic) IBOutlet UIView *HSLView;
@property (weak, nonatomic) IBOutlet UIImageView *currentColorView;
@property (weak, nonatomic) IBOutlet DTColorPickerImageView *colorPicker;
@property (weak, nonatomic) IBOutlet UISlider *lightSlider;
@property (weak, nonatomic) IBOutlet UILabel *lightLabel;
@property (weak, nonatomic) IBOutlet UISlider *RSlider;
@property (weak, nonatomic) IBOutlet UISlider *GSlider;
@property (weak, nonatomic) IBOutlet UISlider *BSlider;
@property (weak, nonatomic) IBOutlet UILabel *showRGBLabel;
@property (weak, nonatomic) IBOutlet UILabel *showHSLLabel;

@property (weak, nonatomic) IBOutlet UISlider *brightnessSlider;
@property (weak, nonatomic) IBOutlet UISlider *tempratureSlider;
@property (weak, nonatomic) IBOutlet UILabel *LumLabel;
@property (weak, nonatomic) IBOutlet UILabel *TempLabel;
@property (weak, nonatomic) IBOutlet UICollectionView *collectionView;

@property (weak, nonatomic) IBOutlet UILabel *lumLevelLabel;
@property (weak, nonatomic) IBOutlet UILabel *tempLevelLabel;
@property (weak, nonatomic) IBOutlet UIView *lumLevelView;
@property (weak, nonatomic) IBOutlet UIView *lumView;
@property (weak, nonatomic) IBOutlet UIView *tempLevelView;
@property (weak, nonatomic) IBOutlet UIView *tempView;

//under tow layout will change constraint when node hasn't HSL modelID.
@property (weak, nonatomic) IBOutlet NSLayoutConstraint *HSLViewHeightLayout;
@property (weak, nonatomic) IBOutlet NSLayoutConstraint *scrollHeightLayout;
//under layout will change conttaint when current node is a panel.
@property (weak, nonatomic) IBOutlet NSLayoutConstraint *onoffCollectionViewHeightLayout;

@property (strong, nonatomic) RGBModel *rgbModel;
@property (strong, nonatomic) HSVModel *hsvModel;
@property (strong, nonatomic) HSLModel *hslModel;
@property (assign, nonatomic) BOOL hasNextCMD;
@property (strong, nonatomic) NSMutableArray *onoffSource;

//保存当前色盘取到的HSV中的HS的值，V值则取L滑竿的值。初始值RGB都是255，HSL值0、0、1，L滑竿值最大，颜色为白色。
@property (strong, nonatomic) HSVModel *colorWheelHSVModel;

@property (assign, nonatomic) BOOL hadChangeBrightness;
@property (assign, nonatomic) BOOL hasNextBrightness;
@property (assign, nonatomic) UInt8 nextBrightness;
@property (assign, nonatomic) BOOL hadChangeTempareture;
@property (assign, nonatomic) BOOL hasNextTempareture;
@property (assign, nonatomic) UInt8 nextTempareture;


@end

@implementation DeviceControlViewController

#pragma mark - xib event
- (IBAction)changeBrightness:(UISlider *)sender {
    self.LumLabel.text = [NSString stringWithFormat:@"Lum(%d)(at ele adr:0x%X):",(int)sender.value,self.model.address];
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
    TeLogInfo(@"self.nextBrightness=%d",self.nextBrightness);
    [DemoCommand changeBrightnessWithBrightness100:self.nextBrightness address:self.model.address retryCount:0 responseMaxCount:0 ack:NO successCallback:^(UInt16 source, UInt16 destination, SigLightLightnessStatus * _Nonnull responseMessage) {
        
    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
        
    }];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(changeBrightnessFinish) object:nil];
        [self performSelector:@selector(changeBrightnessFinish) withObject:nil afterDelay:kCommandInterval];
    });
}

- (IBAction)changeTempareture:(UISlider *)sender {
    self.TempLabel.text = [NSString stringWithFormat:@"Temp(%d)(at ele adr:0x%X):",(int)sender.value,self.model.temperatureAddresses.firstObject.intValue];
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
    [DemoCommand changeTempratureWithTemprature100:self.nextTempareture address:[self.model.temperatureAddresses.firstObject intValue] retryCount:0 responseMaxCount:0 ack:NO successCallback:^(UInt16 source, UInt16 destination, SigLightCTLTemperatureStatus * _Nonnull responseMessage) {
        
    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
        
    }];

    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(changeTemparetureFinish) object:nil];
        [self performSelector:@selector(changeTemparetureFinish) withObject:nil afterDelay:kCommandInterval];
    });
}

- (IBAction)changeLight:(UISlider *)sender {
    UIColor *color = nil;
    if (kControllerInHSL) {
        self.colorWheelHSVModel.value = sender.value;
        color = [ColorManager getUIColorWithHSVColor:self.colorWheelHSVModel];
    }
//    else{
//        self.hsvModel.value = sender.value;
//        color = [UIColor colorWithHue:self.hsvModel.hue saturation:self.hsvModel.saturation brightness:sender.value alpha:self.hsvModel.alpha];
//    }

    [self handleColor:color];
    [self refreshRGBSlider];
    [self sendHSLData];
}

- (IBAction)changeR:(UISlider *)sender {
    UIColor *color = [UIColor colorWithRed:sender.value green:self.rgbModel.green blue:self.rgbModel.blud alpha:1.0];
    [self handleColor:color];
    [self sendHSLData];
}

- (IBAction)changeG:(UISlider *)sender {
    UIColor *color = [UIColor colorWithRed:self.rgbModel.red green:sender.value blue:self.rgbModel.blud alpha:1.0];
    [self handleColor:color];
    [self sendHSLData];
}

- (IBAction)changeB:(UISlider *)sender {
    UIColor *color = [UIColor colorWithRed:self.rgbModel.red green:self.rgbModel.green blue:sender.value alpha:1.0];
    [self handleColor:color];
    [self sendHSLData];
}

- (IBAction)changeLevel:(UIButton *)sender {
    NSInteger tag = sender.tag;
    UInt16 address = self.model.address;
    SInt16 level = ceil(0xFFFF/10.0);//向上取整
    switch (tag) {
        case 100:
            //lum lelel del
            level = -level;
            break;
        case 101:
            //lum lelel add
            break;
        case 102:
            //temp lelel del
            address = self.model.temperatureAddresses.firstObject.intValue;
            level = -level;
            break;
        case 103:
            //temp lelel add
            address = self.model.temperatureAddresses.firstObject.intValue;
            break;
            
        default:
            break;
    }
    __weak typeof(self) weakSelf = self;
    [DemoCommand changeLevelWithAddress:address level:level responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigGenericLevelStatus * _Nonnull responseMessage) {
        TeLogDebug(@"control level success.");
        [weakSelf refreshLumAndTemp];
    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
        
    }];
}


#pragma mark - DTColorPickerImageViewDelegate
//注意：调节RGB：R=83、G=83~87、B=251，转hsl为66、95、65，返回的HSL数据都是“f00c0a00 01008278 66a666a6 32f3”
- (void)imageView:(DTColorPickerImageView *)imageView didPickColorWithColor:(UIColor *)color{
    HSVModel *hsv = [ColorManager getHSVWithColor:color];
    hsv.value = self.lightSlider.value;
    UIColor *temColor = [ColorManager getUIColorWithHSVColor:hsv];
    [self handleColor:temColor];
    [self sendHSLData];
    [self refreshLumAndTemp];
}

- (void)beganTouchImageView:(DTColorPickerImageView *)imageView{
    self.detailScrollView.scrollEnabled = NO;
}

- (void)endTouchImageView:(DTColorPickerImageView *)imageView{
    self.detailScrollView.scrollEnabled = YES;
}

- (void)handleColor:(UIColor *)color{
    if (kControllerInHSL) {
        self.hslModel = [ColorManager getHSLWithColor:color];
        self.showHSLLabel.text = [NSString stringWithFormat:@"HSL:\n H--%.2f\n S--%.2f\n L--%.2f",self.hslModel.hue*100,self.hslModel.saturation*100,self.hslModel.lightness*100];
    }
//    else {
//        self.hsvModel = [ColorManager getHSVWithColor:color];
//        self.showHSLLabel.text = [NSString stringWithFormat:@"HSV:\n H--%.2f\n S--%.2f\n V--%.2f",self.hsvModel.hue*100,self.hsvModel.saturation*100,self.hsvModel.value*100];
//    }
    
    self.rgbModel = [ColorManager getRGBWithColor:color];
    self.colorWheelHSVModel = [ColorManager getHSVWithColor:color];
    self.currentColorView.backgroundColor = color;
    self.showRGBLabel.text = [NSString stringWithFormat:@"RGB:\n R--%d\n G--%d\n B--%d",(int)round(self.rgbModel.red*255),(int)round(self.rgbModel.green*255),(int)round(self.rgbModel.blud*255)];
}

- (void)refreshRGBSlider {
    self.RSlider.value = self.rgbModel.red;
    self.GSlider.value = self.rgbModel.green;
    self.BSlider.value = self.rgbModel.blud;
}

- (void)sendHSLData{
    if ([self canSend]) {
        UInt16 address = self.model.address;
//        __weak typeof(self) weakSelf = self;
        if (kControllerInHSL) {
            [DemoCommand changeHSLWithAddress:address hue100:self.hslModel.hue*100 saturation100:self.hslModel.saturation*100 brightness100:self.hslModel.lightness*100 responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigLightHSLStatus * _Nonnull responseMessage) {
                
            } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
                
            }];
        }
//        else {
//            [DemoCommand changeHSLWithAddress:address hue100:self.hsvModel.hue*100 saturation100:self.hsvModel.saturation*100 brightness100:self.hsvModel.value*100 responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigLightHSLStatus * _Nonnull responseMessage) {
//
//            } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
//
//            }];
//        }
        self.hasNextCMD = NO;
    } else {
        if (!self.hasNextCMD) {
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sendHSLData) object:nil];
            [self performSelector:@selector(sendHSLData) withObject:nil afterDelay:kCMDInterval];
        }
    }
}

// Can send data when collect with kCMDInterval
- (BOOL)canSend{
//    return YES;
    static NSTimeInterval time = 0;
    NSTimeInterval curTime = [[NSDate date] timeIntervalSince1970];
    BOOL tem = NO;
    if (curTime - time >= kCMDInterval) {
        time = curTime;
        tem = YES;
    }
    return tem;
}

#pragma mark - UICollectionViewDelegate
- (nonnull __kindof UICollectionViewCell *)collectionView:(nonnull UICollectionView *)collectionView cellForItemAtIndexPath:(nonnull NSIndexPath *)indexPath {
    OnOffItemCell *item = [collectionView dequeueReusableCellWithReuseIdentifier:CellIdentifiers_OnOffItemCellID forIndexPath:indexPath];
    NSNumber *ele_adr = self.onoffSource[indexPath.item];
    item.onoffLabel.text = [NSString stringWithFormat:@"ele adr:0x%X",ele_adr.intValue];
    
    [item setClickSwiftBlock:^(UISwitch * _Nonnull swift) {
        //attention: resMax change to 0, because node detail vc needn't response.
//        [DemoCommand switchOnOffWithIsOn:swift.isOn address:ele_adr.intValue responseMaxCount:0 ack:NO successCallback:nil resultCallback:nil];
        [DemoCommand switchOnOffWithIsOn:swift.isOn address:ele_adr.intValue responseMaxCount:1 ack:YES successCallback:nil resultCallback:nil];
    }];
    //Attention: panel has 8 onoff button, but node just has one onoff data, developer should fix that when your app has panel.
    if (indexPath.item == 0) {
        item.onoffSwitch.on = self.model.state == DeviceStateOn;
    }
    return item;
}

- (NSInteger)collectionView:(nonnull UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    return self.onoffSource.count;
}

#pragma mark - Life method
- (void)normalSetting{
    [super normalSetting];
    
    self.hadChangeBrightness = NO;
    self.hasNextBrightness = NO;
    self.nextBrightness = 0;
    self.hadChangeTempareture = NO;
    self.hasNextTempareture = NO;
    self.nextTempareture = 0;

    self.currentColorView.layer.cornerRadius = 8;
    self.currentColorView.layer.borderWidth = 1;
    self.currentColorView.layer.borderColor = [UIColor lightGrayColor].CGColor;
    self.lightSlider.value = 1.0;
    
    if (self.model.HSLAddresses.count > 0) {
        [self handleColor:[UIColor whiteColor]];
    }
    
//    if (!kControllerInHSL) {
//        self.lightLabel.text = @"V:";
//    }
    self.lumLevelLabel.text = [NSString stringWithFormat:@"Lum Level(at ele adr:0x%X):",self.model.address];
    self.tempLevelLabel.text = [NSString stringWithFormat:@"Temp Level(at ele adr:0x%X):",self.model.temperatureAddresses.firstObject.intValue];
    
    self.HSLView.hidden = self.model.HSLAddresses.count == 0;
    self.lumView.hidden = self.model.lightnessAddresses.count == 0;
    self.tempView.hidden = self.model.temperatureAddresses.count == 0;
    if (self.model.levelAddresses.count > 0 && self.model.lightnessAddresses.count > 0) {
        self.lumLevelView.hidden = NO;
    }else{
        self.lumLevelView.hidden = YES;
    }
    if (self.model.levelAddresses.count > 0 && self.model.temperatureAddresses.count > 0) {
        self.tempLevelView.hidden = NO;
    }else{
        self.tempLevelView.hidden = YES;
    }
    
    if (self.model.HSLAddresses.count == 0) {
        self.scrollHeightLayout.constant -= self.HSLViewHeightLayout.constant;
        self.HSLViewHeightLayout.constant = 0;
    }else{
        //注意：2.8.2发现RGB为255、0、0时，亮度调到100会设备颜色显示异常，暂时屏蔽
        self.lumLevelView.hidden = YES;
        self.tempLevelView.hidden = YES;
        self.lumView.hidden = YES;
        self.tempView.hidden = YES;
    }
    
    self.onoffSource = [NSMutableArray arrayWithArray:self.model.onoffAddresses];
    if (self.onoffSource.count == 0) {
        self.onoffCollectionViewHeightLayout.constant = 0;
    }else if (self.onoffSource.count <= 4) {
        self.onoffCollectionViewHeightLayout.constant = 70;
    }else{
        self.onoffCollectionViewHeightLayout.constant = 70*2+8;
    }

    [self refreshLumAndTemp];
    [self.collectionView registerNib:[UINib nibWithNibName:CellIdentifiers_OnOffItemCellID bundle:nil] forCellWithReuseIdentifier:CellIdentifiers_OnOffItemCellID];

    //Attention: app get online status use access_cmd_onoff_get() in publish since v2.7.0, so demo should get light and temprature at node detail vc.
    //modelID 0x1303:Light CTL Get
    BOOL hasCTLGet = [self.model getAddressesWithModelID:@(0x1303)].count > 0;
    //modelID 0x1300:Light Lightness Get
    BOOL hasLightnessGet = [self.model getAddressesWithModelID:@(0x1300)].count > 0;

    if (self.model.lightnessAddresses.count > 0 && self.model.temperatureAddresses.count > 0 && hasCTLGet) {
        //get light and temprature
        __weak typeof(self) weakSelf = self;
        [DemoCommand getCTLWithNodeAddress:self.model.address responseMacCount:1 successCallback:^(UInt16 source, UInt16 destination, SigLightCTLStatus * _Nonnull responseMessage) {
            [weakSelf performSelectorOnMainThread:@selector(refreshLumAndTemp) withObject:nil waitUntilDone:YES];
            [weakSelf performSelectorOnMainThread:@selector(getHSL) withObject:nil waitUntilDone:YES];
        } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
            
        }];
    }else if (self.model.lightnessAddresses.count > 0 && hasLightnessGet) {
        //get light
        __weak typeof(self) weakSelf = self;
        [DemoCommand getLumWithNodeAddress:self.model.address responseMacCount:1 successCallback:^(UInt16 source, UInt16 destination, SigLightLightnessStatus * _Nonnull responseMessage) {
            [weakSelf performSelectorOnMainThread:@selector(refreshLumAndTemp) withObject:nil waitUntilDone:YES];
            [weakSelf performSelectorOnMainThread:@selector(getHSL) withObject:nil waitUntilDone:YES];
        } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
            
        }];
    }
}

- (void)getHSL{
    //modelID 0x1307:Light HSL Get
    BOOL hasHSLGet = [self.model getAddressesWithModelID:@(0x1307)].count > 0;
    if (self.model.HSLAddresses.count > 0 && hasHSLGet) {
        __weak typeof(self) weakSelf = self;
        [DemoCommand getHSLWithAddress:self.model.address responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigLightHSLStatus * _Nonnull responseMessage) {
            [weakSelf performSelectorOnMainThread:@selector(HSLCallBack) withObject:nil waitUntilDone:YES];
            [weakSelf performSelectorOnMainThread:@selector(refreshLumAndTemp) withObject:nil waitUntilDone:YES];
        } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
            
        }];
    }
}

- (void)HSLCallBack{
    if (kControllerInHSL) {
        [self handleColor:[self getColorWithH:self.model.HSL_Hue100 S:self.model.HSL_Saturation100 L:self.model.HSL_Lightness100]];
    }
//    else {
//        [self handleColor:[UIColor colorWithHue:self.model.HSL_Hue100/100.0 saturation:self.model.HSL_Saturation100/100.0 brightness:self.model.HSL_Lightness100/100.0 alpha:1.0]];
//    }
}

- (UIColor *)getColorWithH:(UInt8)h S:(UInt8)s L:(UInt8)l{
    //注意：hsl四舍五入取两位小数，理论上hsl中h为[0~100)
    if (h == 100) {
        h = 99;
    }
    HSLModel *hsl = [[HSLModel alloc] init];
    hsl.hue = [NSString stringWithFormat:@"%d",h].div(@"100").floatValue;
    hsl.saturation = [NSString stringWithFormat:@"%d",s].div(@"100").floatValue;
    hsl.lightness = [NSString stringWithFormat:@"%d",l].div(@"100").floatValue;
    return [ColorManager getRGBWithHSLColor:hsl];
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    [self refreshLumAndTemp];
}

- (void)refreshLumAndTemp{
    if (self.model.HSLAddresses.count > 0) {
//        self.lightSlider.value = self.model.HSL_Lightness100/100.0;
        self.RSlider.value = self.rgbModel.red;
        self.GSlider.value = self.rgbModel.green;
        self.BSlider.value = self.rgbModel.blud;
        if (kControllerInHSL) {
            self.currentColorView.backgroundColor = [self getColorWithH:self.hslModel.hue*100.0 S:self.hslModel.saturation*100.0 L:self.hslModel.lightness*100.0];
        }
//        else {
//            self.currentColorView.backgroundColor = [UIColor colorWithHue:self.hsvModel.hue saturation:self.hsvModel.saturation brightness:self.hsvModel.value alpha:1.0];
//        }
    }
    
    if (self.model.onoffAddresses.count == 1) {
        [self.collectionView reloadData];
    }
    
    UInt8 lum=0,temp=0;
    if (self.model.state == DeviceStateOn) {
        lum = self.model.trueBrightness;
        temp = self.model.trueTemperature;
    }
    self.LumLabel.text = [NSString stringWithFormat:@"Lum(%d)(at ele adr:0x%X):",lum,self.model.address];
    self.TempLabel.text = [NSString stringWithFormat:@"Temp(%d)(at ele adr:0x%X):",temp,self.model.temperatureAddresses.firstObject.intValue];
    self.brightnessSlider.value = lum;
    self.tempratureSlider.value = temp;
}

-(void)dealloc{
    TeLogDebug(@"");
}

@end
