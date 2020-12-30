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
//  Created by 梁家誌 on 2018/8/27.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "HSLViewController.h"
#import "ColorManager.h"
#import "ColorModelCell.h"
#import "BaseTableView.h"

@interface HSLViewController()<UITableViewDataSource,BaseTableViewDelegate,ColorModelCellDelegate>
@property (weak, nonatomic) IBOutlet BaseTableView *tableView;
@property (strong, nonatomic) UIColor *colorModel;
@property (strong, nonatomic) RGBModel *rgbModel;
@property (strong, nonatomic) HSVModel *hsvModel;
@property (strong, nonatomic) HSLModel *hslModel;
@property (assign, nonatomic) BOOL hasNextCMD;
@property (strong, nonatomic) ColorModelCell *colorModelCell;
@end

@implementation HSLViewController

- (void)normalSetting{
    [super normalSetting];
    self.title = @"HSL";
    self.tableView.separatorStyle = UITableViewCellSeparatorStyleNone;//去掉下划线
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_ColorModelCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_ColorModelCellID];
    self.tableView.baseTableViewDelegate = self;
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
    // 禁用返回手势
    if ([self.navigationController respondsToSelector:@selector(interactivePopGestureRecognizer)]) {
        self.navigationController.interactivePopGestureRecognizer.enabled = NO;
    }
}

- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    // 开启返回手势
    if ([self.navigationController respondsToSelector:@selector(interactivePopGestureRecognizer)]) {
        self.navigationController.interactivePopGestureRecognizer.enabled = YES;
    }
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
        [DemoCommand changeHSLWithAddress:address hue:self.hslModel.hue saturation:self.hslModel.saturation brightness:self.hslModel.lightness responseMaxCount:0 ack:NO successCallback:nil resultCallback:nil];
        self.hasNextCMD = NO;
    } else {
        if (!self.hasNextCMD) {
            self.hasNextCMD = YES;
            dispatch_async(dispatch_get_main_queue(), ^{
                [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(sendHSLData) object:nil];
                [self performSelector:@selector(sendHSLData) withObject:nil afterDelay:kCMDInterval];
            });
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

#pragma mark - ColorModelCellDelegate

- (void)colorModelCell:(ColorModelCell *)cell didChangedColorWithColor:(UIColor *)color rgbModel:(RGBModel *)rgbModel hsvModel:(HSVModel *)hsvModel hslModel:(HSLModel *)hslModel {
    self.colorModel = color;
    self.rgbModel = rgbModel;
    self.hsvModel = hsvModel;
    self.hslModel = hslModel;
    [self sendHSLData];
}

#pragma mark - UITableViewDataSource

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return 1;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    ColorModelCell *cell = (ColorModelCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_ColorModelCellID forIndexPath:indexPath];
    cell.delegate = self;
    [cell setColorModel:[UIColor yellowColor]];
    self.colorModelCell = cell;
    return cell;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 692.5;
}

#pragma mark - BaseTableViewDelegate

// 目的：触摸到色盘的layer内部时则相应色盘手势，不再响应BaseTableView的滑动手势了。
-(BOOL)baseTableView:(BaseTableView *)baseTableView gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch {
    CGPoint point = [touch locationInView:self.view];
    point = [self.colorModelCell.layer convertPoint:point fromLayer:self.view.layer];
    if ([self.colorModelCell.layer containsPoint:point]) {
        point = [self.colorModelCell.colorPicker.layer convertPoint:point fromLayer:self.colorModelCell.layer];
        if ([self.colorModelCell.colorPicker.layer containsPoint:point]) {
            return NO;
        }
    }
    return YES;
}

@end
