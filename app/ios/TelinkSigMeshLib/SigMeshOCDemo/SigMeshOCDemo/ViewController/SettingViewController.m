/********************************************************************************************************
 * @file     SettingViewController.m 
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/7/31
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

#import "SettingViewController.h"
#import "SettingItemCell.h"
#import "MeshOTAVC.h"
#import "ResponseTestVC.h"

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
    if ([titleString isEqualToString:@"Choose Add Devices"]) {
        if (SigDataSource.share.existLocationIvIndexAndLocationSequenceNumber) {
            [self pushToAddDeviceVC];
        } else {
            __weak typeof(self) weakSelf = self;
            UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Hits" message:@"connect to the current network to get IV index before add nodes or input IV index in input box？" preferredStyle:UIAlertControllerStyleAlert];
            [alertController addAction:[UIAlertAction actionWithTitle:@"Input IV index" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
                TeLogDebug(@"点击输入ivIndex");
                UIAlertController *inputAlertController = [UIAlertController alertControllerWithTitle:@"Hits" message:@"Please input IV index in input box" preferredStyle:UIAlertControllerStyleAlert];
                [inputAlertController addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
                    textField.placeholder = @"Please input IV index in input box";
                    textField.text = @"0";
                }];
                [inputAlertController addAction:[UIAlertAction actionWithTitle:@"Done" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
                    TeLogDebug(@"输入ivIndex完成");
                    UITextField *ivIndex = inputAlertController.textFields.firstObject;
                    UInt32 ivIndexUInt32 = [LibTools uint32From16String:ivIndex.text];
                    [SigDataSource.share setIvIndexUInt32:ivIndexUInt32];
                    [SigDataSource.share setSequenceNumberUInt32:0];
                    [SigDataSource.share saveCurrentIvIndex:ivIndexUInt32 sequenceNumber:0];
                    TeLogDebug(@"输入ivIndex=%d",ivIndexUInt32);
                    UIAlertController *pushAlertController = [UIAlertController alertControllerWithTitle:@"Hits" message:[NSString stringWithFormat:@"IV index = 0x%08X, start add devices.", ivIndexUInt32] preferredStyle:UIAlertControllerStyleAlert];
                    [pushAlertController addAction:[UIAlertAction actionWithTitle:@"Add Devices" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
                        [weakSelf pushToAddDeviceVC];
                    }]];
                    [weakSelf presentViewController:pushAlertController animated:YES completion:nil];
                }]];
                [inputAlertController addAction:[UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
                    TeLogDebug(@"点击取消");
                }]];
                [weakSelf presentViewController:inputAlertController animated:YES completion:nil];
            }]];
            [alertController addAction:[UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
                TeLogDebug(@"点击取消");
            }]];
            [self presentViewController:alertController animated:YES completion:nil];
        }
        return;
    }
    NSString *sb = @"Setting";
    UIViewController *vc = nil;
    if ([titleString isEqualToString:@"Log"] || [titleString isEqualToString:@"Choose Add Devices"]) {
        sb = @"Main";
    }

    if ([titleString isEqualToString:@"Mesh OTA"]) {
        vc = [[MeshOTAVC alloc] init];
    } else if ([titleString isEqualToString:@"Test"]) {
//        vc = [[ResponseTestVC alloc] init];
//        ((ResponseTestVC *)vc).isResponseTest = YES;
        [self clickTest];
        return;
    } else {
        vc = [UIStoryboard initVC:self.vcIdentifiers[indexPath.row] storyboard:sb];
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

- (void)clickTest {
    __weak typeof(self) weakSelf = self;
    UIAlertController *actionSheet = [UIAlertController alertControllerWithTitle:@"Select Test Actions" message:nil preferredStyle:UIAlertControllerStyleAlert];
    UIAlertAction *alertT1 = [UIAlertAction actionWithTitle:@"Response Test" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        ResponseTestVC *vc = [[ResponseTestVC alloc] init];
        vc.isResponseTest = YES;
        [weakSelf.navigationController pushViewController:vc animated:YES];
    }];
    [actionSheet addAction:alertT1];
    UIAlertAction *alertT2 = [UIAlertAction actionWithTitle:@"Interval Test" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        ResponseTestVC *vc = [[ResponseTestVC alloc] init];
        vc.isResponseTest = NO;
        [weakSelf.navigationController pushViewController:vc animated:YES];
    }];
    [actionSheet addAction:alertT2];
    UIAlertAction *alertF = [UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
        NSLog(@"Cancel");
    }];
    [actionSheet addAction:alertF];
    [self presentViewController:actionSheet animated:YES completion:nil];
}

- (void)pushToAddDeviceVC {
    UIViewController *vc = [UIStoryboard initVC:ViewControllerIdentifiers_ChooseAndAddDeviceViewControllerID storyboard:@"Main"];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
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
    #ifdef kExist
    if (kExistMeshOTA) {
        [self.source addObject:@"Mesh OTA"];
        [self.iconSource addObject:@"ic_mesh_ota"];
        [self.vcIdentifiers addObject:@"no found"];
    }
    #endif
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
    if (kshowMeshSettings) {
        [self.source addObject:@"Settings"];
        [self.iconSource addObject:@"ic_model"];
        [self.vcIdentifiers addObject:ViewControllerIdentifiers_SettingsVCID];
    }
    if (kshowChooseAdd) {
        [self.source addObject:@"Choose Add Devices"];
        [self.iconSource addObject:@"ic_model"];
        [self.vcIdentifiers addObject:ViewControllerIdentifiers_ChooseAndAddDeviceViewControllerID];
    }
#ifdef kExist
    if (kshowTest) {
        [self.source addObject:@"Test"];
        [self.iconSource addObject:@"ic_model"];
        [self.vcIdentifiers addObject:ViewControllerIdentifiers_TestVCID];
    }
#endif
#ifdef kExist
    if (kExistForwardingTableEntry) {
        [self.source addObject:@"Direct Control List"];
        [self.iconSource addObject:@"ic_model"];
        [self.vcIdentifiers addObject:ViewControllerIdentifiers_DirectControlListVCID];
        [self.source addObject:@"Forwarding Table List"];
        [self.iconSource addObject:@"ic_model"];
        [self.vcIdentifiers addObject:ViewControllerIdentifiers_ForwardingTableVCID];
    }
#endif

//    NSString *app_Version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
    NSString *app_Version = kTelinkSigMeshLibVersion;
    
#ifdef DEBUG
    NSString *appBundleVersion = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleVersion"];
    self.versionLabel.text = [NSString stringWithFormat:@"%@ Bulid:%@",app_Version,appBundleVersion];
#else
    self.versionLabel.text = [NSString stringWithFormat:@"%@",app_Version];
#endif

}

@end
