/********************************************************************************************************
 * @file     CDTPServiceListVC.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/7/24
 *
 * @par     Copyright (c) [2023], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "CDTPServiceListVC.h"
#import "MeshOTAItemCell.h"
#import "UIButton+extension.h"

@interface CDTPServiceListVC ()<CDTPClientDelegate, UITableViewDelegate, UITableViewDataSource>
@property (nonatomic, strong) CDTPClientModel *client;
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UILabel *cdtpTipsLabel;
@property (weak, nonatomic) IBOutlet UIButton *startButton;
@property (nonatomic, strong) NSMutableArray <ServiceModel *>*serviceList;
@property (nonatomic, assign) NSInteger selectIndex;

@end

@implementation CDTPServiceListVC

- (void)normalSetting{
    [super normalSetting];
    [self setStartButtonEnable:YES];
    if (self.isExportToGateway) {
        self.title = @"CDTP Export To Gateway";
    } else {
        self.title = @"CDTP Import From Other Phone";
    }
    self.tableView.tableFooterView = [[UIView alloc] initWithFrame:CGRectZero];
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_MeshOTAItemCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_MeshOTAItemCellID];
    
    __weak typeof(self) weakSelf = self;
    self.client = [[CDTPClientModel alloc] initWithBleInitResult:^(CBCentralManager * _Nonnull central) {
        [weakSelf.client startScanCDTPService];
    }];
    self.client.delegate = self;
    _serviceList = [NSMutableArray array];
    _selectIndex = -1;
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    [self.client endShare];
}

- (IBAction)clickStartButton:(UIButton *)sender {
    if (self.selectIndex < 0 || self.selectIndex >= self.serviceList.count) {
        [self showTips:@"Please select service."];
        return;
    }
    [self setStartButtonEnable:NO];
    ServiceModel *model = self.serviceList[self.selectIndex];
    
    if (self.isExportToGateway) {
        [self.client startWriteMeshObject:self.meshObject toServicePeripheral:model.peripheral];
    } else {
        [self.client startReadMeshObjectFromServicePeripheral:model.peripheral];
    }
}

- (void)setStartButtonEnable:(BOOL)enable {
    self.startButton.backgroundColor = enable ? UIColor.telinkButtonBlue : [UIColor colorWithRed:185.0/255.0 green:185.0/255.0 blue:185.0/255.0 alpha:1.0];
    self.tableView.userInteractionEnabled = enable;
}

/*!
 *  @method onDiscoverServicePeripheral:advertisementData:RSSI:
 *
 *  @param peripheral           A <code>CBPeripheral</code> object.
 *  @param advertisementData    A dictionary containing any advertisement and scan response data.
 *  @param RSSI                 The current RSSI of <i>peripheral</i>, in dBm. A value of <code>127</code> is reserved and indicates the RSSI
 *                                was not available.
 *
 *  @discussion                 This method is invoked while scanning, upon the discovery of <i>peripheral</i> by <i>central</i>. A discovered peripheral must
 *                              be retained in order to use it; otherwise, it is assumed to not be of interest and will be cleaned up by the central manager. For
 *                              a list of <i>advertisementData</i> keys, see {@link CBAdvertisementDataLocalNameKey} and other similar constants.
 *
 *  @seealso                    CBAdvertisementData.h
 *
 */
- (void)onDiscoverServicePeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary<NSString *,id> *)advertisementData RSSI:(NSNumber *)RSSI {
    TelinkLogInfo(@"peripheral=%@ advertisementData=%@ RSSI=%@", peripheral, advertisementData, RSSI);
    ServiceModel *model = [[ServiceModel alloc] initWithPeripheral:peripheral advertisementData:advertisementData RSSI:RSSI];
    if ([_serviceList containsObject:model]) {
        [_serviceList replaceObjectAtIndex:[_serviceList indexOfObject:model] withObject:model];
    } else {
        [_serviceList addObject:model];
    }
    [self.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
}

/// A callback called whenever the CDTP Client Write some object data to CDTP Service.
/// @param progress The progress of write object. Range of progress is 0.0~1.0.
- (void)onClientWriteProgress:(float)progress {
    if (self.isExportToGateway) {
        TelinkLogInfo(@"progress=%f", progress);
        self.cdtpTipsLabel.text = [NSString stringWithFormat:@"Write Progress: %0.f%%", progress * 100];
//        if (progress == 1.0) {
//            __weak typeof(self) weakSelf = self;
//            [self showTips:@"Share Success!" sure:^(UIAlertAction *action) {
//                [weakSelf.navigationController popViewControllerAnimated:YES];
//            }];
//        }
    }
}

/// A callback called whenever the CDTP Client read some object data from CDTP Service.
/// @param progress The progress of read object. Range of progress is 0.0~1.0.
- (void)onClientReadProgress:(float)progress {
    if (!self.isExportToGateway) {
        TelinkLogInfo(@"progress=%f", progress);
        self.cdtpTipsLabel.text = [NSString stringWithFormat:@"Read Progress: %0.f%%", progress * 100];
    }
}

/// This method is called back when an exception is triggered when the CDTP Client write Object data to the CDTP Service. error is nil means Write success.
/// @param error Error of Write progress.
- (void)onClientWriteFinishWithError:(NSError * __nullable )error {
    if (self.isExportToGateway) {
        TelinkLogInfo(@"error=%@", error)
        [self setStartButtonEnable:YES];
        if (error) {
            self.cdtpTipsLabel.text = [NSString stringWithFormat:@"Write Error: %@", error.localizedDescription];
            __weak typeof(self) weakSelf = self;
            [self showTips:[NSString stringWithFormat:@"Write Error: %@", error.localizedDescription] sure:^(UIAlertAction *action) {
                [weakSelf.navigationController popViewControllerAnimated:YES];
            }];
        } else {
            self.cdtpTipsLabel.text = @"Write finish";
            __weak typeof(self) weakSelf = self;
            [self showTips:@"Share Success!" sure:^(UIAlertAction *action) {
                [weakSelf.navigationController popViewControllerAnimated:YES];
            }];
        }
    }
}

/// This method is called back when an exception is triggered when the CDTP Client read Object data from the CDTP Service. error is nil means read success.
/// @param data The CDTP mesh network object data.
/// @param error Error of read progress.
- (void)onClientReadFinishWithData:(NSData * __nullable )data error:(NSError * __nullable )error {
    if (!self.isExportToGateway) {
        TelinkLogInfo(@"data=%@ error=%@", data, error)
        [self setStartButtonEnable:YES];
        __weak typeof(self) weakSelf = self;
        if (error) {
            self.cdtpTipsLabel.text = [NSString stringWithFormat:@"Read Error: %@", error.localizedDescription];
            [self showTips:[NSString stringWithFormat:@"Read Error: %@", error.localizedDescription] sure:^(UIAlertAction *action) {
                [weakSelf.navigationController popViewControllerAnimated:YES];
            }];
        } else {
            self.cdtpTipsLabel.text = [NSString stringWithFormat:@"Read finish: length=%ld", data.length];
            NSData *decompressionData = data.decompressionData;
            NSDictionary *dict = [LibTools getDictionaryWithJSONData:decompressionData];
            if (dict && dict.allKeys.count > 0) {
                [self handleMeshDictionaryFromShareImport:dict];
            } else {
                [self showTips:@"import fail!" sure:^(UIAlertAction *action) {
                    [weakSelf.navigationController popViewControllerAnimated:YES];
                }];
            }
        }
    }
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifiers_MeshOTAItemCellID forIndexPath:indexPath];
    [self configureCell:cell forRowAtIndexPath:indexPath];
    return cell;
}

- (void)configureCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath{
    MeshOTAItemCell *itemCell = (MeshOTAItemCell *)cell;
    __weak typeof(self) weakSelf = self;
    ServiceModel *model = self.serviceList[indexPath.row];
    itemCell.titleLabel.text = [NSString stringWithFormat:@"name:%@\nRSSI:%@\nuuid:%@\nadv:%@", model.showName, model.RSSI, model.peripheral.identifier.UUIDString, model.advertisementData];
    itemCell.selectButton.selected = indexPath.row == self.selectIndex;
    
    [itemCell.selectButton addAction:^(UIButton *button) {
        button.selected = !button.selected;
        if (button.selected) {
            weakSelf.selectIndex = indexPath.row;
        } else {
            weakSelf.selectIndex = -1;
        }
        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
    }];

}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return self.serviceList.count;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    MeshOTAItemCell *itemCell = [tableView cellForRowAtIndexPath:indexPath];
    itemCell.selectButton.selected = !itemCell.selectButton.selected;
    if (itemCell.selectButton.selected) {
        self.selectIndex = indexPath.row;
    } else {
        self.selectIndex = -1;
    }
    [self.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
}

-(void)dealloc {
    TelinkLogDebug(@"");
}

@end
