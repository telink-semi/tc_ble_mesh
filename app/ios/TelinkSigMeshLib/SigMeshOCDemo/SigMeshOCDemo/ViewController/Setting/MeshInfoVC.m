/********************************************************************************************************
 * @file     MeshInfoVC.m
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
//  MeshInfoVC.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2019/1/24.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import "MeshInfoVC.h"

@interface MeshInfoVC ()
@property (weak, nonatomic) IBOutlet NSLayoutConstraint *RPLabelTopLayout;
@property (weak, nonatomic) IBOutlet UILabel *remoteProvisionLabel;
@property (weak, nonatomic) IBOutlet UILabel *netKeyLabel;
@property (weak, nonatomic) IBOutlet UILabel *appKeyLabel;
@property (weak, nonatomic) IBOutlet UISwitch *addTypeSwitch;
@property (weak, nonatomic) IBOutlet UISwitch *remoteSwitch;
@property (weak, nonatomic) IBOutlet UISwitch *fastProvisionSwitch;
@property (weak, nonatomic) IBOutlet UISwitch *onlineStatusSwitch;

@end

@implementation MeshInfoVC

- (void)viewDidLoad{
    [super viewDidLoad];
    self.title = @"Mesh Info";
    self.netKeyLabel.text = [LibTools convertDataToHexStr:SigDataSource.share.curNetKey];
    self.appKeyLabel.text = [LibTools convertDataToHexStr:SigDataSource.share.curAppKey];
    NSNumber *type = [[NSUserDefaults standardUserDefaults] valueForKey:kKeyBindType];
    self.addTypeSwitch.on = type.integerValue == KeyBindTpye_Fast;
    NSNumber *fastProvision = [[NSUserDefaults standardUserDefaults] valueForKey:kFastAddType];
    self.fastProvisionSwitch.on = fastProvision.boolValue;
    NSNumber *online = [[NSUserDefaults standardUserDefaults] valueForKey:kGetOnlineStatusType];
    self.onlineStatusSwitch.on = online.boolValue;

    NSNumber *remote = [[NSUserDefaults standardUserDefaults] valueForKey:kRemoteAddType];
    self.remoteSwitch.on = remote.boolValue;
    self.remoteProvisionLabel.hidden = !kExistRemoteProvision;
    self.remoteSwitch.hidden = !kExistRemoteProvision;
    self.RPLabelTopLayout.constant = kExistRemoteProvision == YES ? 30 : -30;
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (IBAction)clickSwitch:(UISwitch *)sender {
    NSNumber *type = [NSNumber numberWithInteger:sender.on == YES ? KeyBindTpye_Fast : KeyBindTpye_Normal];
    [[NSUserDefaults standardUserDefaults] setValue:type forKey:kKeyBindType];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (IBAction)clickRemoteSwitch:(UISwitch *)sender {
    NSNumber *type = [NSNumber numberWithBool:sender.on];
    [[NSUserDefaults standardUserDefaults] setValue:type forKey:kRemoteAddType];
    if (sender.on && _fastProvisionSwitch.on) {
        _fastProvisionSwitch.on = NO;
        [[NSUserDefaults standardUserDefaults] setValue:[NSNumber numberWithBool:NO] forKey:kFastAddType];
    }
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (IBAction)clickFastProvisionSwitch:(UISwitch *)sender {
    NSNumber *type = [NSNumber numberWithBool:sender.on];
    [[NSUserDefaults standardUserDefaults] setValue:type forKey:kFastAddType];
    if (sender.on && _remoteSwitch.on) {
        _remoteSwitch.on = NO;
        [[NSUserDefaults standardUserDefaults] setValue:[NSNumber numberWithBool:NO] forKey:kRemoteAddType];
    }
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (IBAction)clickOnlineStatusSwitch:(UISwitch *)sender {
    NSNumber *type = [NSNumber numberWithBool:sender.on];
    [[NSUserDefaults standardUserDefaults] setValue:type forKey:kGetOnlineStatusType];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (IBAction)clickCopyNetKey:(UIButton *)sender {
    UIPasteboard*pasteboard = [UIPasteboard generalPasteboard];
    pasteboard.string=self.netKeyLabel.text;
    [self showTips:@"copy netKey success"];
}

- (IBAction)clickCopyAppKey:(UIButton *)sender {
    UIPasteboard*pasteboard = [UIPasteboard generalPasteboard];
    pasteboard.string=self.appKeyLabel.text;
    [self showTips:@"copy appKey success"];
}

- (void)showTips:(NSString *)tips{
    dispatch_async(dispatch_get_main_queue(), ^{
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Hits" message:tips preferredStyle:UIAlertControllerStyleAlert];
        [alertController addAction:[UIAlertAction actionWithTitle:@"Sure" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            TeLogDebug(@"点击确认");
        }]];
        [self presentViewController:alertController animated:YES completion:nil];
    });
}

@end
