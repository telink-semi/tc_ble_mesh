/********************************************************************************************************
 * @file     DebugViewController.m
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
//  DebugViewController.m
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/11/29.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "DebugViewController.h"

@interface DebugViewController ()
@property (weak, nonatomic) IBOutlet UIButton *button1;
@property (weak, nonatomic) IBOutlet UIButton *button2;
@property (nonatomic,strong) NSString *path;
@end

@implementation DebugViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    self.title = @"Debug";
    self.button1.enabled = self.button2.enabled = YES;
}

- (IBAction)clickButton1:(UIButton *)sender {
//    if(mesh_adv_tx_cmd_sno < 0xc10000){
//        mesh_adv_tx_cmd_sno = 0xc10000;
//        TeLogDebug(@"mesh_adv_tx_cmd_sno = 0xc10000");
//        [self showTips:@"success"];
//    }
}

- (IBAction)clickButton2:(UIButton *)sender {
//    if(iv_idx_st.keep_time_s < 0x60000){
//        iv_idx_st.keep_time_s = 0x60000;
//        TeLogDebug(@"iv_idx_st.keep_time_s = 0x60000");
//        [self showTips:@"success"];
//    }
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

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

@end
