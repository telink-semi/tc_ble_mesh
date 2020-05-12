/********************************************************************************************************
 * @file     SettingViewController.m 
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
//  SettingViewController.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "SettingViewController.h"
#import "SettingItemCell.h"
#import "MeshOTAVC.h"

@interface SettingViewController()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (nonatomic, strong) NSMutableArray <NSString *>*source;
@property (nonatomic, strong) NSMutableArray <NSString *>*iconSource;
@property (nonatomic, strong) NSMutableArray <NSString *>*vcIdentifiers;
@property (weak, nonatomic) IBOutlet UILabel *versionLabel;
@end

@implementation SettingViewController

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    SettingItemCell *cell = (SettingItemCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_SettingItemCellID forIndexPath:indexPath];
    cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    cell.nameLabel.text = self.source[indexPath.row];
    cell.iconImageView.image = [UIImage imageNamed:self.iconSource[indexPath.row]];
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    NSString *titleString = self.source[indexPath.row];
    NSString *sb = @"Setting";
    UIViewController *vc = nil;
    if ([titleString isEqualToString:@"Log"] || [titleString isEqualToString:@"Choose Add Devices"]) {
        sb = @"Main";
    }
    
    if ([titleString isEqualToString:@"Mesh OTA"]) {
        vc = [[MeshOTAVC alloc] init];
    }else{
        vc = [UIStoryboard initVC:self.vcIdentifiers[indexPath.row] storybroad:sb];
    }
    if ([titleString isEqualToString:@"Choose Add Devices"]) {
        [SigDataSource.share setAllDevicesOutline];
    }
    [self.navigationController pushViewController:vc animated:YES];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.source.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 51.0;
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = NO;
}

- (void)normalSetting{
    [super normalSetting];
    self.title = @"Setting";
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    self.source = [NSMutableArray array];
    self.iconSource = [NSMutableArray array];
    self.vcIdentifiers = [NSMutableArray array];
    if (kShowScenes) {
        [self.source addObject:@"Scenes"];
        [self.iconSource addObject:@"scene"];
        [self.vcIdentifiers addObject:ViewControllerIdentifiers_SceneListViewControllerID];
    }
    if (kshowShare) {
        [self.source addObject:@"Share"];
        [self.iconSource addObject:@"ic_model"];
        [self.vcIdentifiers addObject:ViewControllerIdentifiers_ShareViewControllerID];
    }
    if (kExistMeshOTA) {
        [self.source addObject:@"Mesh OTA"];
        [self.iconSource addObject:@"ic_mesh_ota"];
        [self.vcIdentifiers addObject:@"no found"];
    }
//    if (kShowDebug) {
//        [self.source addObject:@"Debug"];
//        [self.iconSource addObject:@"ic_model"];
//        [self.vcIdentifiers addObject:ViewControllerIdentifiers_DebugViewControllerID];
//    }
    if (kshowLog) {
        [self.source addObject:@"Log"];
        [self.iconSource addObject:@"ic_model"];
        [self.vcIdentifiers addObject:ViewControllerIdentifiers_LogViewControllerID];
    }
    if (kshowMeshInfo) {
        [self.source addObject:@"Mesh Info"];
        [self.iconSource addObject:@"ic_model"];
        [self.vcIdentifiers addObject:ViewControllerIdentifiers_MeshInfoViewControllerID];
    }
    if (kshowChooseAdd) {
        [self.source addObject:@"Choose Add Devices"];
        [self.iconSource addObject:@"ic_model"];
        [self.vcIdentifiers addObject:ViewControllerIdentifiers_ChooseAndAddDeviceViewControllerID];
    }

    NSString *app_Version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
    self.versionLabel.text = [NSString stringWithFormat:@"V%@",app_Version];
    
}

@end
