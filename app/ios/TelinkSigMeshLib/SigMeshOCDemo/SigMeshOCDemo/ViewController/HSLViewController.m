/********************************************************************************************************
 * @file     HSLViewController.m 
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
//  HSLViewController.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2018/8/27.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "HSLViewController.h"
#import "DTColorPickerImageView.h"
#import "ColorManager.h"

@interface HSLViewController()<DTColorPickerImageViewDelegate>
@property (weak, nonatomic) IBOutlet DTColorPickerImageView *colorPicker;
@property (weak, nonatomic) IBOutlet UILabel *showRGBLabel;
@property (weak, nonatomic) IBOutlet UILabel *showHSLLabel;
@property (weak, nonatomic) IBOutlet UIImageView *currentColorView;
@property (weak, nonatomic) IBOutlet UISlider *brightnessSlider;
@property (weak, nonatomic) IBOutlet UILabel *lightLabel;
@property (weak, nonatomic) IBOutlet UISlider *RSlider;
@property (weak, nonatomic) IBOutlet UISlider *GSlider;
@property (weak, nonatomic) IBOutlet UISlider *BSlider;

@property (strong, nonatomic) RGBModel *rgbModel;
@property (strong, nonatomic) HSVModel *hsvModel;
@property (strong, nonatomic) HSLModel *hslModel;
@property (assign, nonatomic) BOOL hasNextCMD;

//保存当前色盘取到的HSV中的HS的值，V值则取L滑竿的值。初始值RGB都是255，HSL值0、0、1，L滑竿值最大，颜色为白色。
@property (strong, nonatomic) HSVModel *colorWheelHSVModel;

@end

@implementation HSLViewController

- (void)normalSetting{
    [super normalSetting];
    
    self.title = @"HSL";
//    if (!kControllerInHSL) {
//        self.lightLabel.text = @"V:";
//    }

    self.currentColorView.layer.cornerRadius = 8;
    self.currentColorView.layer.borderWidth = 1;
    self.currentColorView.layer.borderColor = [UIColor lightGrayColor].CGColor;
    self.brightnessSlider.value = 1.0;
    [self handleColor:[UIColor whiteColor]];
}

- (IBAction)changeBrightness:(UISlider *)sender {
    UIColor *color = nil;
    if (kControllerInHSL) {
        self.colorWheelHSVModel.value = sender.value;
        color = [ColorManager getUIColorWithHSVColor:self.colorWheelHSVModel];
    }
//    else{
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

- (void)imageView:(DTColorPickerImageView *)imageView didPickColorWithColor:(UIColor *)color{
    HSVModel *hsv = [ColorManager getHSVWithColor:color];
    hsv.value = self.brightnessSlider.value;
    UIColor *temColor = [ColorManager getUIColorWithHSVColor:hsv];
    [self handleColor:temColor];
    [self sendHSLData];
}

- (void)handleColor:(UIColor *)color{
    if (kControllerInHSL) {
        self.hslModel = [ColorManager getHSLWithColor:color];
        self.showHSLLabel.text = [NSString stringWithFormat:@"HSL:\n H--%d\n S--%d\n L--%d",(int)(self.hslModel.hue*100),(int)(self.hslModel.saturation*100),(int)(self.hslModel.lightness*100)];
//        self.brightnessSlider.value = self.hslModel.lightness;
    }
//    else {
//        self.hsvModel = [ColorManager getHSVWithColor:color];
//        self.showHSLLabel.text = [NSString stringWithFormat:@"HSV:\n H--%d\n S--%d\n V--%d",(int)(self.hsvModel.hue*100),(int)(self.hsvModel.saturation*100),(int)(self.hsvModel.value*100)];
////        self.brightnessSlider.value = self.hsvModel.value;
//    }
    
    self.rgbModel = [ColorManager getRGBWithColor:color];
    self.colorWheelHSVModel = [ColorManager getHSVWithColor:color];
    self.currentColorView.backgroundColor = color;
    self.showRGBLabel.text = [NSString stringWithFormat:@"RGB:\n R--%d\n G--%d\n B--%d",(int)(self.rgbModel.red*100),(int)(self.rgbModel.green*100),(int)(self.rgbModel.blud*100)];
    self.RSlider.value = self.rgbModel.red;
    self.GSlider.value = self.rgbModel.green;
    self.BSlider.value = self.rgbModel.blud;
}

- (void)refreshRGBSlider {
    self.RSlider.value = self.rgbModel.red;
    self.GSlider.value = self.rgbModel.green;
    self.BSlider.value = self.rgbModel.blud;
}

- (void)sendHSLData{
    if ([self canSend]) {
        UInt16 address;
        int rsp_max=1;
        if (self.isGroup) {
            //组控
            address = self.groupModel.intAddress;
            rsp_max = (int)self.groupModel.groupOnlineDevices.count;
        } else {
            //单灯
            address = self.model.address;
        }
        if (kControllerInHSL) {
            [DemoCommand changeHSLWithAddress:address hue100:self.hslModel.hue*100 saturation100:self.hslModel.saturation*100 brightness100:self.hslModel.lightness*100 responseMaxCount:0 ack:NO successCallback:nil resultCallback:nil];
        }
//        else {
//            [DemoCommand changeHSLWithAddress:address hue100:self.hsvModel.hue*100 saturation100:self.hsvModel.saturation*100 brightness100:self.hsvModel.value*100 responseMaxCount:0 ack:NO successCallback:nil resultCallback:nil];
//        }
        self.hasNextCMD = NO;
    } else {
        if (!self.hasNextCMD) {
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sendHSLData) object:nil];
            [self performSelector:@selector(sendHSLData) withObject:nil afterDelay:kCMDInterval];
        }
    }
}

//判断是否发包
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

-(void)dealloc{
    TeLogDebug(@"");
}

@end
