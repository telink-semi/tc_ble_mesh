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
//#import "SigDataSource.h"
//#import "TransmitJsonManager.h"
#import "zipAndUnzip+zipString.h"
#import "zipAndUnzip.h"

@interface ShowQRCodeViewController ()<TransmitJsonManagerDelegate>
@property (strong, nonatomic) IBOutlet UIImageView *QRCodeImage;
@property (nonatomic, assign) CGFloat brightness;
@property (nonatomic,strong) NSString *curMeshUUID;
@property (nonatomic,strong) NSData *jsonData;

@end

@implementation ShowQRCodeViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    self.title = @"Share";
    
    //根据kShareWithBluetoothPointToPoint使用不同的分享方式
    if (kShareWithBluetoothPointToPoint) {
        //做法2：meshUUID转为serviceUUID，再拼接字典，生成二维码
        //转化为serviceUUID格式，注意：serviceUUID只有两种数据长度，否则调用iOS广播蓝牙数据的接口时会crash。
        NSString *string = SigDataSource.share.curProvisionerModel.UUID;
        self.curMeshUUID = [LibTools UUIDToMeshUUID:string];
        
        NSMutableDictionary *qrdic = [[NSMutableDictionary alloc] init];
        qrdic[kJsonMeshUUID_key] = self.curMeshUUID;
        NSError *error = nil;
        NSData *json = [NSJSONSerialization dataWithJSONObject:qrdic options:NSJSONWritingPrettyPrinted error:&error];
        
        self.QRCodeImage.image = [UIImage createQRImageWithData:json rate:3];
    } else {
        //做法1：json直接生成二维码(与telink mesh的二维码生成逻辑完全一样)
        NSDictionary *jsonDict = SigDataSource.share.getDictionaryFromDataSource;
        NSError *error = nil;
        NSData *json = [NSJSONSerialization dataWithJSONObject:jsonDict options:NSJSONWritingPrettyPrinted error:&error];
        NSData *createData = [zipAndUnzip gzipDeflate:json];
        NSString *content = [NSString stringWithFormat:@"%@", createData];
        content = [content substringWithRange:NSMakeRange(1, content.length - 2)];
        NSString *resultContent = [content stringByReplacingOccurrencesOfString:@" " withString:@"" options:0 range:NSMakeRange(0, content.length)];
        NSData *resultData = [resultContent dataUsingEncoding:NSUTF8StringEncoding];
        self.QRCodeImage.image = [UIImage createQRImageWithData:resultData rate:1];
    }
    
    NSDictionary *jsonDict = SigDataSource.share.getDictionaryFromDataSource;
    NSData *jsonData = [LibTools getJSONDataWithDictionary:jsonDict];
    [TransmitJsonManager.share startShareWithString:self.curMeshUUID jsonData:[zipAndUnzip gzipDeflate:jsonData]];
    TransmitJsonManager.share.delegate = self;
}

#pragma
#pragma mark - TransmitJsonManagerDelegate
///回调分享json数据完成
- (void)onEndShare{
    [self.navigationController popViewControllerAnimated:YES];
}

-(void)dealloc{
    [TransmitJsonManager.share stopShare];
    TeLog(@"%s",__func__);
}

@end
