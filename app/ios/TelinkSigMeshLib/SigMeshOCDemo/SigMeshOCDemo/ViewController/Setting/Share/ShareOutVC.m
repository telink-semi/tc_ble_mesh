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

@interface ShareOutVC ()<UITableViewDelegate,UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIButton *selectJsonButton;
@property (weak, nonatomic) IBOutlet UIButton *selectQRCodeButton;
@property (nonatomic, strong) NSMutableArray <SigNetkeyModel *>*sourceArray;
@property (nonatomic, strong) NSMutableArray <SigNetkeyModel *>*selectArray;

@end

@implementation ShareOutVC

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    self.selectJsonButton.selected = YES;
    self.selectQRCodeButton.selected = NO;
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    self.sourceArray = [NSMutableArray arrayWithArray:SigDataSource.share.netKeys];
    self.selectArray = [NSMutableArray arrayWithObject:SigDataSource.share.curNetkeyModel];
    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(KeyCell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(KeyCell.class)];
}

- (IBAction)clickExportButton:(UIButton *)sender {
    if (self.selectArray.count == 0) {
        [self showTips:@"Please select at least one network key!"];
        return;
    }
    
    //3.3.2新增逻辑：只分享选中的NetKey和该NetKey下的AppKey。
    SigDataSource *exportDS = [[SigDataSource alloc] init];
    [exportDS setDictionaryToDataSource:SigDataSource.share.getDictionaryFromDataSource];
    exportDS.netKeys = [NSMutableArray arrayWithArray:self.selectArray];
    NSMutableArray *netkeyIndexs = [NSMutableArray array];
    for (SigNetkeyModel *model in exportDS.netKeys) {
        [netkeyIndexs addObject:@(model.index)];
    }
    NSMutableArray *apps = [NSMutableArray array];
    for (SigAppkeyModel *model in SigDataSource.share.appKeys) {
        if ([netkeyIndexs containsObject:@(model.boundNetKey)]) {
            [apps addObject:model];
        }
    }
    exportDS.appKeys = [NSMutableArray arrayWithArray:apps];

    NSMutableDictionary *exportDict = [NSMutableDictionary dictionaryWithDictionary:[exportDS getDictionaryFromDataSource]];
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

    if (self.selectJsonButton.selected) {
        [self exporyMeshByJsonFileWithDictionary:exportDict];
    } else if (self.selectQRCodeButton.selected) {
        [self exporyMeshByQRCodeWithDictionary:exportDict];
    }
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

- (void)exporyMeshByQRCodeWithDictionary:(NSDictionary *)dictionary {
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

- (void)pushToShowQRCodeVCWithUUID:(NSString *)uuid {
    ShowQRCodeViewController *vc = (ShowQRCodeViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_ShowQRCodeViewControllerID storybroad:@"Setting"];
    vc.uuidString = uuid;
    [self.navigationController pushViewController:vc animated:YES];
}

- (IBAction)clickSelectJsonFile:(UIButton *)sender {
    self.selectJsonButton.selected = YES;
    self.selectQRCodeButton.selected = NO;

}

- (IBAction)clickSelectQRCode:(UIButton *)sender {
    self.selectJsonButton.selected = NO;
    self.selectQRCodeButton.selected = YES;

}

- (void)showTips:(NSString *)message{
    __weak typeof(self) weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        [weakSelf showAlertSureWithTitle:@"Hits" message:message sure:^(UIAlertAction *action) {
            
        }];
    });
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
