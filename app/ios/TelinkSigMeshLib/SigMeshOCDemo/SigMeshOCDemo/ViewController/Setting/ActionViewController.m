/********************************************************************************************************
 * @file     ActionViewController.m 
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/9/26
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
    [DemoCommand changeBrightnessWithBrightness100:sender.value address:self.model.address retryCount:0 responseMaxCount:0 ack:NO successCallback:nil resultCallback:nil];
}

- (IBAction)changeTempareture:(UISlider *)sender {
    SigNodeModel *device = [SigDataSource.share getNodeWithAddress:self.model.address];
    self.TempLabel.text = [NSString stringWithFormat:@"Temp(%d)",(int)sender.value];
    [DemoCommand changeTempratureWithTemprature100:sender.value address:[device.temperatureAddresses.firstObject intValue] retryCount:0 responseMaxCount:0 ack:NO successCallback:nil resultCallback:nil];
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
