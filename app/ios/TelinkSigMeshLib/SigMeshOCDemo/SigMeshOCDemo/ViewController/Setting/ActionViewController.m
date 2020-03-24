/********************************************************************************************************
 * @file     ActionViewController.m 
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
//  ActionViewController.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2018/9/26.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "ActionViewController.h"
#import "HSLViewController.h"
#import "SceneDetailViewController.h"

@interface ActionViewController ()
@property (weak, nonatomic) IBOutlet UISlider *brightnessSlider;
@property (weak, nonatomic) IBOutlet UISlider *tempratureSlider;
@property (weak, nonatomic) IBOutlet UILabel *LumLabel;
@property (weak, nonatomic) IBOutlet UILabel *TempLabel;

@end

@implementation ActionViewController

#pragma mark - xib event
- (IBAction)changeBrightness:(UISlider *)sender {
    self.LumLabel.text = [NSString stringWithFormat:@"Lum(%d)",(int)sender.value];
    [DemoCommand changeBrightnessWithBrightness100:sender.value address:self.model.address responseMaxCount:1 ack:NO successCallback:nil resultCallback:nil];
//    [DemoCommand changeBrightness:YES address:self.model.address para:sender.value isGet:NO respondMax:1 complete:nil];
}

- (IBAction)changeTempareture:(UISlider *)sender {
    SigNodeModel *device = [SigDataSource.share getNodeWithAddress:self.model.address];
    self.TempLabel.text = [NSString stringWithFormat:@"Temp(%d)",(int)sender.value];
    [DemoCommand changeTempratureWithTemprature100:sender.value address:[device.temperatureAddresses.firstObject intValue] responseMaxCount:1 ack:NO successCallback:nil resultCallback:nil];
//    [DemoCommand changeTemprature:YES address:[device.temperatureAddresses.firstObject intValue] para:sender.value isGet:NO respondMax:1 complete:nil];
}

- (IBAction)clickHSL:(UIButton *)sender {
    SigNodeModel *device = [SigDataSource.share getNodeWithAddress:self.model.address];
    HSLViewController *vc = (HSLViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_HSLViewControllerID];
    vc.isGroup = NO;
    vc.model = device;
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)normalSetting{
    [super normalSetting];
    
    self.LumLabel.text = [NSString stringWithFormat:@"Lum(%d)",self.model.trueBrightness];
    self.TempLabel.text = [NSString stringWithFormat:@"Temp(%d)",self.model.trueTemperature];
    self.brightnessSlider.value = self.model.trueBrightness;
    self.tempratureSlider.value = self.model.trueTemperature;
    self.title = [NSString stringWithFormat:@"%d",self.model.address];
    
    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithTitle:@"Save" style:UIBarButtonItemStylePlain target:self action:@selector(clickSave)];
    self.navigationItem.rightBarButtonItem = rightItem;
}

- (void)clickSave{
    ActionModel *curAction = [[ActionModel alloc] initWithNode:[[SigDataSource share] getNodeWithAddress:self.model.address]];
    if (self.backAction) {
        self.backAction(curAction);
    }
    [self popVC:[SceneDetailViewController class]];
}

- (void)popVC:(Class)vcClass{
    BOOL has = NO;
    for (UIViewController *controller in self.navigationController.viewControllers) {
        if ([controller isKindOfClass:vcClass]) {
            has = YES;
            [self.navigationController popToViewController:controller animated:YES];
            break;
        }
    }
    if (!has) {
        [self.navigationController popViewControllerAnimated:YES];
    }
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

@end
