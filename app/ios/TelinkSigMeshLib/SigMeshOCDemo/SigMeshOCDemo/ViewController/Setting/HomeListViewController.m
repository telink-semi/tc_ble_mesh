/********************************************************************************************************
 * @file     HomeListViewController.m
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
//  HomeListViewController.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2018/11/19.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "HomeListViewController.h"
#import "ShowQRCodeViewController.h"
#import "ScanCodeVC.h"
#import "NSString+extension.h"
#import "zipAndUnzip+zipString.h"
#import "zipAndUnzip.h"

@interface HomeListViewController ()<TransmitJsonManagerDelegate,UITableViewDelegate,UITableViewDataSource>
@property (strong, nonatomic) ScanCodeVC *scanCodeVC;
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <SigDataSource *>*source;
@property (assign, nonatomic) BOOL isReceiving;

@end

@implementation HomeListViewController

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    NSString *identifier= @"UITableViewCell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:identifier];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:identifier];
    }
    
    SigDataSource *model = _source[indexPath.row];
    cell.textLabel.text = @"MeshUUID:";
    cell.detailTextLabel.text = model.meshUUID;
    return cell;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.source.count;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    ShowQRCodeViewController *vc = (ShowQRCodeViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_ShowQRCodeViewControllerID storybroad:@"Setting"];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
    
    self.source = [[NSMutableArray alloc] initWithObjects:SigDataSource.share, nil];
    [self.tableView reloadData];
}

- (void)normalSetting{
    [super normalSetting];
    self.title = @"Homes";
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    
    UIBarButtonItem *rightItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemCamera target:self action:@selector(clickCamera)];
    self.navigationItem.rightBarButtonItem = rightItem;
}

- (void)clickCamera{
    [self.navigationController pushViewController:self.scanCodeVC animated:YES];
}

- (ScanCodeVC *)scanCodeVC {
    if (!_scanCodeVC) {
        _scanCodeVC = [ScanCodeVC scanCodeVC];
        TransmitJsonManager.share.delegate = self;
        __weak typeof(self) weakSelf = self;
        [_scanCodeVC scanDataViewControllerBackBlock:^(id content) {
            //AnalysisShareDataVC
            NSData *data = (NSData *)content;
            NSDictionary *dic;
            if ([data length]) {
                dic = [NSString dictionaryWithJsonString:(NSString *)content];
            }else{
                //hasn't data
                [[NSNotificationCenter defaultCenter] postNotificationName:@"BackToMain" object:nil];
                [weakSelf showTips:@"QRCode is error."];
                return;
            }
            
            //根据kShareWithBluetoothPointToPoint使用不同的分享方式
            if (kShareWithBluetoothPointToPoint) {
                //适配ShowQRCodeViewController.m生成二维码的做法2
                if (!([dic.allKeys containsObject:kJsonMeshUUID_key])) {
                    //信息不完整
                    [[NSNotificationCenter defaultCenter] postNotificationName:@"BackToMain" object:nil];
                    [weakSelf showTips:@"QRCode is error."];
                    return;
                }
                NSString *scanMeshUUID = dic[kJsonMeshUUID_key];
                [TransmitJsonManager.share startReceiveWithMeshUUID:scanMeshUUID];
                weakSelf.isReceiving = YES;
            } else {
                //适配ShowQRCodeViewController.m生成二维码的做法1
                NSString *temps = content;
                unsigned long count = temps.length / 2;
                Byte byte[count];
                memset(byte, 0, count);
                for (int i = 0; i<count; i++) {
                    NSString *bs = [temps substringWithRange:NSMakeRange(i*2, 2)];
                    byte[i] = strtoul([bs UTF8String], 0, 16);
                }
                NSData *resultData = [NSData dataWithBytes:byte length:count];
                [weakSelf onReceiveJsonData:resultData];
            }
            
        }];
    }
    return _scanCodeVC;
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

#pragma
#pragma mark - TransmitJsonManagerDelegate
///回调获取到的json数据
- (void)onReceiveJsonData:(NSData *)data{
    NSDictionary *jsonDict = [LibTools getDictionaryWithJSONData:[zipAndUnzip gzipInflate:data]];
    [SigDataSource.share setDictionaryToDataSource:jsonDict];
    [SigDataSource.share checkExistLocationProvisioner];
    [SigDataSource.share writeDataSourceToLib];
    [SigDataSource.share.scanList removeAllObjects];
//    [SigDataSource.share.matchsNodeIdentityArray removeAllObjects];
//    [SigDataSource.share.noMatchsNodeIdentityArray removeAllObjects];

    //断开原来的蓝牙连接
    [SigBearer.share closeWithResult:^(BOOL successful) {
        if (successful) {
            TeLogDebug(@"clocse success.");
            //扫描完成，退出扫描界面
            [[NSNotificationCenter defaultCenter] postNotificationName:@"BackToMain" object:nil];
        } else {
            TeLogDebug(@"clocse fail.");
        }
    }];
}

-(void)dealloc{
    if (self.isReceiving) {
        [TransmitJsonManager.share stopReceive];
    }
    TeLogDebug(@"%s",__func__);
}

@end
