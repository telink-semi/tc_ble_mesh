/********************************************************************************************************
 * @file     ShowQRCodeViewController.m
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
//  ScanQRCodeViewController.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2018/11/19.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "ShowQRCodeViewController.h"
#import "UIImage+Extension.h"
#import "zipAndUnzip+zipString.h"
#import "zipAndUnzip.h"

@interface ShowQRCodeViewController ()
@property (strong, nonatomic) IBOutlet UIImageView *QRCodeImage;

@end

@implementation ShowQRCodeViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    self.title = @"Share";
    self.QRCodeImage.image = [UIImage createQRImageWithString:self.uuidString rate:3];
}

-(void)dealloc{
    TeLogDebug(@"%s",__func__);
}

@end
