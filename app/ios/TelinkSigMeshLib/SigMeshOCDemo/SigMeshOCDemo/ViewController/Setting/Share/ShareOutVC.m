/********************************************************************************************************
 * @file     ShareOutVC.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/1/24
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

#import "ShareOutVC.h"
#import "UIButton+extension.h"
#import "UIViewController+Message.h"
#import "KeyCell.h"
#import "ShowQRCodeViewController.h"
#import "Reachability.h"
#import "ShareTipsVC.h"
#import "CDTPExportVC.h"
#import "CDTPServiceListVC.h"

@interface ShareOutVC ()<UITableViewDelegate,UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIButton *selectCDTPToOtherPhoneButton;
@property (weak, nonatomic) IBOutlet UIButton *selectCDTPToGatewayButton;
@property (weak, nonatomic) IBOutlet UIButton *selectQRCodeAndCloudButton;
@property (weak, nonatomic) IBOutlet UIButton *selectJsonFileButton;
@property (weak, nonatomic) IBOutlet UIButton *exportButton;
@property (nonatomic, strong) NSMutableArray <SigNetkeyModel *>*sourceArray;
@property (nonatomic, strong) NSMutableArray <SigNetkeyModel *>*selectArray;

@end

@implementation ShareOutVC

- (void)viewDidLoad {
    [super viewDidLoad];
    [self setTitle:@"Share Export" subTitle:self.network.meshName];
    // Do any additional setup after loading the view.
    // 由于mesh v1.1并未release CDTP功能，暂时屏蔽CDTP功能。
    self.selectCDTPToOtherPhoneButton.selected = NO;
    self.selectCDTPToGatewayButton.selected = NO;
    self.selectQRCodeAndCloudButton.selected = YES;
    self.selectJsonFileButton.selected = NO;
    self.exportButton.backgroundColor = UIColor.telinkButtonBlue;
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    self.sourceArray = [NSMutableArray arrayWithArray:self.network.netKeys];
    self.selectArray = [NSMutableArray arrayWithObject:self.network.curNetkeyModel];
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(KeyCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(KeyCell.class)];
}

- (IBAction)clickSelectCDTPExportToOtherPhoneButton:(UIButton *)sender {
    self.selectCDTPToOtherPhoneButton.selected = YES;
    self.selectCDTPToGatewayButton.selected = NO;
    self.selectQRCodeAndCloudButton.selected = NO;
    self.selectJsonFileButton.selected = NO;
}

- (IBAction)clickSelectCDTPExportToGatewayButton:(UIButton *)sender {
    self.selectCDTPToOtherPhoneButton.selected = NO;
    self.selectCDTPToGatewayButton.selected = YES;
    self.selectQRCodeAndCloudButton.selected = NO;
    self.selectJsonFileButton.selected = NO;
}

- (IBAction)clickSelectQRCodeAndCloudButton:(UIButton *)sender {
    self.selectCDTPToOtherPhoneButton.selected = NO;
    self.selectCDTPToGatewayButton.selected = NO;
    self.selectQRCodeAndCloudButton.selected = YES;
    self.selectJsonFileButton.selected = NO;
}

- (IBAction)clickSelectJsonFileButton:(UIButton *)sender {
    self.selectCDTPToOtherPhoneButton.selected = NO;
    self.selectCDTPToGatewayButton.selected = NO;
    self.selectQRCodeAndCloudButton.selected = NO;
    self.selectJsonFileButton.selected = YES;
}

- (IBAction)clickCDTPExportToOtherPhoneTipsButton:(UIButton *)sender {
    ShareTipsVC *vc = (ShareTipsVC *)[UIStoryboard initVC:ViewControllerIdentifiers_ShareTipsVCID storyboard:@"Setting"];
    vc.title = TipsTitle_CDTPExportToOtherPhone;
    vc.tipsMessage = TipsMessage_QRCodeAndCDTPTransferJSONToOtherPhone;
    [self.navigationController pushViewController:vc animated:YES];
}

- (IBAction)clickCDTPExportToGatewayTipsButton:(UIButton *)sender {
    ShareTipsVC *vc = (ShareTipsVC *)[UIStoryboard initVC:ViewControllerIdentifiers_ShareTipsVCID storyboard:@"Setting"];
    vc.title = TipsTitle_CDTPExportToGateway;
    vc.tipsMessage = TipsMessage_QRCodeAndCDTPTransferJSONToGateway;
    [self.navigationController pushViewController:vc animated:YES];
}

- (IBAction)clickCloudTransferJsonButton:(UIButton *)sender {
    ShareTipsVC *vc = (ShareTipsVC *)[UIStoryboard initVC:ViewControllerIdentifiers_ShareTipsVCID storyboard:@"Setting"];
    vc.title = TipsTitle_QRCodeAndCloudTransferJSON;
    vc.tipsMessage = TipsMessage_QRCodeAndCloudTransferJSON;
    [self.navigationController pushViewController:vc animated:YES];
}

- (IBAction)clickJsonFileButton:(UIButton *)sender {
    ShareTipsVC *vc = (ShareTipsVC *)[UIStoryboard initVC:ViewControllerIdentifiers_ShareTipsVCID storyboard:@"Setting"];
    vc.title = TipsTitle_JSONFile;
    vc.tipsMessage = TipsMessage_JSONFile;
    [self.navigationController pushViewController:vc animated:YES];
}

- (IBAction)clickExportButton:(UIButton *)sender {
    if (self.network.curNodes.count == 0) {
        [self showTips:@"not allow to share empty network!"];
        return;
    }
    if (self.selectArray.count == 0) {
        [self showTips:@"Please select at least one network key!"];
        return;
    }
    
    //3.3.2新增逻辑：只分享选中的NetKey和该NetKey下的AppKey。
    SigDataSource *exportDS = [[SigDataSource alloc] init];
    [exportDS setDictionaryToDataSource:self.network.getFormatDictionaryFromDataSource];
    exportDS.netKeys = [NSMutableArray arrayWithArray:self.selectArray];
    NSMutableArray *netkeyIndexs = [NSMutableArray array];
    for (SigNetkeyModel *model in exportDS.netKeys) {
        [netkeyIndexs addObject:@(model.index)];
    }
    NSMutableArray *apps = [NSMutableArray array];
    for (SigAppkeyModel *model in self.network.appKeys) {
        if ([netkeyIndexs containsObject:@(model.boundNetKey)]) {
            [apps addObject:model];
        }
    }
    exportDS.appKeys = [NSMutableArray arrayWithArray:apps];

    NSMutableDictionary *exportDict = [NSMutableDictionary dictionaryWithDictionary:[exportDS getFormatDictionaryFromDataSource]];
    //3.3.2新增逻辑：未定义subnet bridge的key，暂时不分享subnet bridge相关内容。
    if ([exportDict.allKeys containsObject:@"nodes"]) {
        NSArray *nodeList = exportDict[@"nodes"];
        NSMutableArray *newNodes = [NSMutableArray array];
        if (nodeList && nodeList.count) {
            for (NSDictionary *dic in nodeList) {
                NSMutableDictionary *mDict = [NSMutableDictionary dictionaryWithDictionary:dic];
                if ([mDict.allKeys containsObject:@"subnetBridgeList"]) {
                    [mDict removeObjectForKey:@"subnetBridgeList"];
                }
                if ([mDict.allKeys containsObject:@"subnetBridgeEnable"]) {
                    [mDict removeObjectForKey:@"subnetBridgeEnable"];
                }
                [newNodes addObject:mDict];
            }
            exportDict[@"nodes"] = newNodes;
        }
    }

    if (self.selectCDTPToOtherPhoneButton.selected) {
        [self exporyMeshByCDTPWithDictionary:exportDict isExportToGateway:NO];
    } else if (self.selectCDTPToGatewayButton.selected) {
        [self exporyMeshByCDTPWithDictionary:exportDict isExportToGateway:YES];
    } else if (self.selectQRCodeAndCloudButton.selected) {
        [self exporyMeshByQRCodeAndCloudWithDictionary:exportDict];
    } else if (self.selectJsonFileButton.selected) {
        [self exporyMeshByJsonFileWithDictionary:exportDict];
    }
}

- (void)exporyMeshByCDTPWithDictionary:(NSDictionary *)dictionary isExportToGateway:(BOOL)isExportToGateway {
    ObjectModel *object = [[ObjectModel alloc] initWithMeshDictionary:dictionary];
    if (isExportToGateway) {
        CDTPServiceListVC *vc = (CDTPServiceListVC *)[UIStoryboard initVC:ViewControllerIdentifiers_CDTPServiceListVCID storyboard:@"Setting"];
        vc.meshObject = object;
        vc.isExportToGateway = YES;
        [self.navigationController pushViewController:vc animated:YES];
    } else {
        CDTPExportVC *vc = (CDTPExportVC *)[UIStoryboard initVC:ViewControllerIdentifiers_CDTPExportVCID storyboard:@"Setting"];
        vc.meshDictionary = dictionary;
        [self.navigationController pushViewController:vc animated:YES];
    }
}

- (void)exporyMeshByQRCodeAndCloudWithDictionary:(NSDictionary *)dictionary {
    NSString *remoteHostName = @"www.apple.com";
    Reachability *hostReachability = [Reachability reachabilityWithHostName:remoteHostName];
    if (hostReachability.currentReachabilityStatus == NotReachable) {
        [self showTips:@"The Internet connection appears to be offline."];
        return;
    }
    
    __weak typeof(self) weakSelf = self;
    //设置有效时间5分钟
    [TelinkHttpManager.share uploadJsonDictionary:dictionary timeout:60 * 5 didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
        dispatch_async(dispatch_get_main_queue(), ^{
            if (err) {
                NSString *errstr = [NSString stringWithFormat:@"%@",err];
                TeLogInfo(@"%@",errstr);
                [weakSelf showTips:errstr];
            } else {
                TeLogInfo(@"result=%@",result);
                NSDictionary *dic = (NSDictionary *)result;
                BOOL isSuccess = [dic[@"isSuccess"] boolValue];
                if (isSuccess) {
                    [weakSelf pushToShowQRCodeVCWithUUID:dic[@"data"]];
                }else{
                    [weakSelf showTips:dic[@"msg"]];
                }
            }
        });
    }];
}

- (void)exporyMeshByJsonFileWithDictionary:(NSDictionary *)dictionary {
    //导出json文件名为“mesh-时间.json”
    NSDate *date = [NSDate date];
    NSDateFormatter *f = [[NSDateFormatter alloc] init];
    f.dateFormat = @"yyyy-MM-dd-HH:mm:ss.SSS";
    f.locale = [[NSLocale alloc] initWithLocaleIdentifier:@"zh_CN"];
    NSString *dstr = [f stringFromDate:date];
    NSString *jsonName = [NSString stringWithFormat:@"mesh-%@.json",dstr];
    
    NSString *path = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).lastObject stringByAppendingPathComponent:jsonName];

    NSFileManager *manager = [[NSFileManager alloc] init];
    BOOL exist = [manager fileExistsAtPath:path];
    if (!exist) {
        BOOL ret = [manager createFileAtPath:path contents:nil attributes:nil];
        if (ret) {
            NSData *tempData = [LibTools getJSONDataWithDictionary:dictionary];
            NSFileHandle *handle = [NSFileHandle fileHandleForWritingAtPath:path];
            [handle truncateFileAtOffset:0];
            [handle writeData:tempData];
            [handle closeFile];
            
            NSString *tipString = [NSString stringWithFormat:@"export %@ success!",jsonName];
            [self showTips:tipString];
            TeLogDebug(@"%@",tipString);
        } else {
            NSString *tipString = [NSString stringWithFormat:@"export %@ fail!",jsonName];
            [self showTips:tipString];
            TeLogDebug(@"%@",tipString);
        }
    }
}

- (void)pushToShowQRCodeVCWithUUID:(NSString *)uuid {
    ShowQRCodeViewController *vc = (ShowQRCodeViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_ShowQRCodeViewControllerID storyboard:@"Setting"];
    vc.uuidString = uuid;
    [self.navigationController pushViewController:vc animated:YES];
}

#pragma mark - UITableViewDataSource
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.sourceArray.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    KeyCell *cell = [tableView dequeueReusableCellWithIdentifier:NSStringFromClass(KeyCell.class) forIndexPath:indexPath];
    SigNetkeyModel *model = self.sourceArray[indexPath.row];
    [cell setNetKeyModel:model];
    [cell.editButton setImage:[UIImage imageNamed:@"unxuan"] forState:UIControlStateNormal];
    [cell.editButton setImage:[UIImage imageNamed:@"xuan"] forState:UIControlStateSelected];
    cell.editButton.selected = [self.selectArray containsObject:model];
    __weak typeof(self) weakSelf = self;
    [cell.editButton addAction:^(UIButton *button) {
        if ([weakSelf.selectArray containsObject:model]) {
            [weakSelf.selectArray removeObject:model];
        } else {
            [weakSelf.selectArray addObject:model];
        }
        [weakSelf.tableView reloadData];
    }];
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [tableView cellForRowAtIndexPath:indexPath];
    cell.selected = NO;
    SigNetkeyModel *model = self.sourceArray[indexPath.row];
    if ([self.selectArray containsObject:model]) {
        [self.selectArray removeObject:model];
    } else {
        [self.selectArray addObject:model];
    }
    [self.tableView reloadData];
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 55;
}

-(void)dealloc{
    TeLogDebug(@"");
}

@end
