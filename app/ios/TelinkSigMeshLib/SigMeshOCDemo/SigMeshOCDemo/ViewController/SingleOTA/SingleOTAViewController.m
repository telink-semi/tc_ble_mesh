/********************************************************************************************************
 * @file     SingleOTAViewController.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/7/31
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "SingleOTAViewController.h"
#import "OTAFileSource.h"
#import "UIViewController+Message.h"
#import "ChooseBinCell.h"

@interface SingleOTAViewController()<UITableViewDelegate,UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIButton *otaButton;
@property (weak, nonatomic) IBOutlet UILabel *otaTipsLabel;
@property (nonatomic,strong) NSMutableArray <NSString *>*source;
@property (nonatomic, assign) int selectIndex;
@property (nonatomic, assign) BOOL OTAing;
@property (nonatomic, strong) NSData *localData;
@property (nonatomic, strong) UIColor *normalColor;
@property (nonatomic, strong) UIColor *unableColor;

@end

@implementation SingleOTAViewController

- (IBAction)clickStartOTA:(UIButton *)sender {
    if (self.OTAing) {
        return;
    }
    if (self.selectIndex == -1) {
        [self showTips:@"Please choose Bin file."];
        return;
    }else{
        self.localData = [OTAFileSource.share getDataWithBinName:self.source[self.selectIndex]];
        if (self.localData == nil || self.localData.length == 0) {
            [self showTips:@"APP can't load this Bin file."];
            return;
        }
    }
    UInt16 pid = [OTAFileSource.share getPidWithOTAData:self.localData];
    if (pid != [LibTools uint16From16String:self.model.pid]) {
        __weak typeof(self) weakSelf = self;
        [self showAlertTitle:kDefaultAlertTitle message:@"The PID of node is different from the PID of Bin file, still OTA this node?" sure:^(UIAlertAction *action) {
            [weakSelf otaAction];
        }];
    } else {
        [self otaAction];
    }
}

- (void)otaAction {
    TelinkLogVerbose(@"clickStartOTA");
    self.OTAing = YES;
    self.otaButton.backgroundColor = self.unableColor;
    self.tableView.userInteractionEnabled = NO;
    [self showOTATips:@"start connect..."];

    __weak typeof(self) weakSelf = self;
    BOOL result = [OTAManager.share startOTAWithOtaData:self.localData models:@[self.model] singleSuccessAction:^(SigNodeModel *device) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [weakSelf otaSuccessAction];
        });
    } singleFailAction:^(SigNodeModel *device) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [weakSelf otaFailAction];
        });
    } singleProgressAction:^(float progress) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [weakSelf showOTAProgress:progress];
        });
    } finishAction:^(NSArray<SigNodeModel *> *successModels, NSArray<SigNodeModel *> *fileModels) {
        TelinkLogDebug(@"");
    }];
    TelinkLogDebug(@"result = %d",result);
}

- (void)normalSetting{
    [super normalSetting];
    //init rightBarButtonItem
    UIBarButtonItem *item = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"tishi"] style:UIBarButtonItemStylePlain target:self action:@selector(clickPushToTipsVC)];
    self.navigationItem.rightBarButtonItem = item;
    self.selectIndex = -1;
    self.OTAing = NO;
    self.otaButton.backgroundColor = UIColor.telinkButtonBlue;
    self.normalColor = UIColor.telinkButtonBlue;
    self.unableColor = [UIColor colorWithRed:185.0/255.0 green:185.0/255.0 blue:185.0/255.0 alpha:1.0];
    self.title = [NSString stringWithFormat:@"OTA Pid:0x%X Vid:0x%X",[LibTools uint16From16String:self.model.pid], CFSwapInt16HostToBig([LibTools uint16From16String:self.model.vid])];
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_ChooseBinCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_ChooseBinCellID];
    self.source = [[NSMutableArray alloc] initWithArray:OTAFileSource.share.getAllBinFile];
}

- (void)viewWillDisappear:(BOOL)animated{
    [super viewWillDisappear:animated];
    [OTAManager.share stopOTA];
}

- (void)clickPushToTipsVC {
    UIViewController *vc = [UIStoryboard initVC:@"TipsVC" storyboard:@"Setting"];
    [self.navigationController pushViewController:vc animated:YES];
}

- (void)showOTAProgress:(float)progress{
    NSString *tips = [NSString stringWithFormat:@"OTA:%.1f%%", progress];
    if (progress == 100) {
        tips = [tips stringByAppendingString:@",reboot..."];
    }
    [self showOTATips:tips];
}

- (void)showOTATips:(NSString *)message{
    self.otaTipsLabel.text = message;
}

- (void)otaSuccessAction{
    self.OTAing = NO;
    [self showTips:@"OTA success"];
    self.otaButton.backgroundColor = self.normalColor;
    self.tableView.userInteractionEnabled = YES;
    [self showOTATips:@"OTA success"];
    [SDKLibCommand startMeshConnectWithComplete:nil];
    TelinkLogVerbose(@"otaSuccess");
}

- (void)otaFailAction{
    self.OTAing = NO;
    [self showTips:@"OTA fail"];
    self.otaButton.backgroundColor = self.normalColor;
    self.tableView.userInteractionEnabled = YES;
    [self showOTATips:@"OTA fail"];
    [SDKLibCommand startMeshConnectWithComplete:nil];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self];
    });
    TelinkLogVerbose(@"otaFail");
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    ChooseBinCell *cell = (ChooseBinCell *)[tableView dequeueReusableCellWithIdentifier:CellIdentifiers_ChooseBinCellID forIndexPath:indexPath];
    NSString *binString = self.source[indexPath.row];
    NSData *data = [OTAFileSource.share getDataWithBinName:binString];
    cell.nameLabel.numberOfLines = 0;
    if (data && data.length) {
        UInt16 vid = [OTAFileSource.share getVidWithOTAData:data];
        vid = CFSwapInt16HostToBig(vid);
        cell.nameLabel.text = [NSString stringWithFormat:@"%@   PID:0x%X VID:0x%X",binString,[OTAFileSource.share getPidWithOTAData:data], vid];
    } else {
        cell.nameLabel.text = [NSString stringWithFormat:@"%@,read bin fail!",binString];//bin文件读取失败。
    }
    cell.selectButton.selected = indexPath.row == self.selectIndex;
    return cell;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.source.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 44.0;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.row == self.selectIndex) {
        self.selectIndex = -1;
    } else {
        self.selectIndex = (int)indexPath.row;
    }
    [self.tableView reloadData];
}

-(void)dealloc{
    TelinkLogDebug(@"");
}

@end
