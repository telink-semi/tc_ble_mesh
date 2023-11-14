/********************************************************************************************************
 * @file     MeshOTAVC.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/4/17
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

#import "MeshOTAVC.h"
#import "MeshOTAItemCell.h"
#import "UIButton+extension.h"
#import "HomeViewController.h"
#import "OTAFileSource.h"
#import "UIViewController+Message.h"

/**
 Attention: more detail about mesh OTA can look document Mesh_Firmware_update_20180228_d05r05.pdf
 */
@interface MeshOTAVC ()<UITableViewDataSource,UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UIButton *getFWInfoButton;
@property (weak, nonatomic) IBOutlet UIButton *startButton;
@property (weak, nonatomic) IBOutlet UITableView *tableView;
@property (weak, nonatomic) IBOutlet UIButton *verifyOnlyButton;
@property (weak, nonatomic) IBOutlet UIButton *verifyAndApplyButton;
@property (weak, nonatomic) IBOutlet UILabel *policyLabel;
@property (weak, nonatomic) IBOutlet UILabel *verifyOnlyLabel;
@property (weak, nonatomic) IBOutlet UILabel *verifyAndApplyLabel;
@property (weak, nonatomic) IBOutlet UIButton *connectedDeviceButton;
@property (weak, nonatomic) IBOutlet UIButton *phoneButton;
@property (weak, nonatomic) IBOutlet UILabel *initiatorProgressLabel;
@property (weak, nonatomic) IBOutlet UIProgressView *initiatorProgressView;
@property (weak, nonatomic) IBOutlet UILabel *distributorProgressLabel;
@property (weak, nonatomic) IBOutlet UIProgressView *distributorProgressView;
@property (nonatomic, strong) NSMutableArray <SigNodeModel *>*allItemArray;
@property (nonatomic, strong) NSMutableArray <NSNumber *>*allItemAddressArray;
@property (nonatomic, strong) NSMutableArray *binStringArray;
@property (nonatomic, strong) NSMutableArray <SigNodeModel *>*selectItemArray;
@property (nonatomic, strong) NSMutableArray <NSNumber *>*selectItemAddressArray;
@property (nonatomic, assign) NSInteger binIndex;
@property (nonatomic, strong) NSMutableDictionary *allItemVIDDict;
@property (assign, nonatomic) BOOL needDelayReloadData;
@property (assign, nonatomic) BOOL isDelaying;
@property (nonatomic, strong) NSMutableArray <SigUpdatingNodeEntryModel *>*receiversList;
@property (nonatomic, strong) NSMutableArray <NSNumber *>*successAddresses;
@property (nonatomic, strong) NSMutableArray <NSNumber *>*failAddresses;
@end

@implementation MeshOTAVC

- (void)viewDidLoad {
    [super viewDidLoad];
    self.startButton.backgroundColor = UIColor.telinkButtonBlue;
    self.getFWInfoButton.backgroundColor = UIColor.telinkButtonBlue;
    self.title = @"Mesh OTA";
    self.needDelayReloadData = NO;
    self.verifyOnlyButton.selected = NO;
    self.verifyAndApplyButton.selected = YES;
    self.connectedDeviceButton.selected = YES;
    self.phoneButton.selected = NO;
    [self updateInitiatorProgress:0];
    [self updateDistributorProgress:0];

    UIBarButtonItem *leftItem = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"back"] style:UIBarButtonItemStylePlain target:self action:@selector(clickBackButton)];
    self.navigationItem.leftBarButtonItem = leftItem;

    UIView *footerView = [[UIView alloc] initWithFrame:CGRectZero];
    self.tableView.tableFooterView = footerView;
    [self.tableView registerNib:[UINib nibWithNibName:CellIdentifiers_MeshOTAItemCellID bundle:nil] forCellReuseIdentifier:CellIdentifiers_MeshOTAItemCellID];
    //iOS 15中 UITableView 新增了一个属性：sectionHeaderTopPadding。此属性会给每一个 section header 增加一个默认高度，当我们使用 UITableViewStylePlain 初始化UITableView 的时候，系统默认给 section header 增高了22像素。
    if(@available(iOS 15.0,*)) {
        self.tableView.sectionHeaderTopPadding = 0;
    }

    self.binIndex = -1;
    self.selectItemArray = [NSMutableArray array];
    self.selectItemAddressArray = [NSMutableArray array];
    self.allItemArray = [[NSMutableArray alloc] initWithArray:SigDataSource.share.curNodes];
    self.allItemAddressArray = [NSMutableArray array];
    for (SigNodeModel *node in self.allItemArray) {
        [self.allItemAddressArray addObject:@(node.address)];
    }
    self.binStringArray = [NSMutableArray arrayWithArray:OTAFileSource.share.getAllBinFile];
    self.allItemVIDDict = [NSMutableDictionary dictionary];
    self.successAddresses = [NSMutableArray array];
    self.failAddresses = [NSMutableArray array];

    [MeshOTAManager.share saveIsMeshOTAing:NO];

    if (_isContinue) {
        [self refreshContinueUI];
    } else {
        NSNumber *addressNumber = [[NSUserDefaults standardUserDefaults] valueForKey:kDistributorAddress];
        if (addressNumber == nil || addressNumber.intValue == 0 || addressNumber.intValue == SigDataSource.share.curLocationNodeModel.address) {
            //不弹框

        } else {
            //弹框
            __weak typeof(self) weakSelf = self;
            UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Warning - previous firmware-update is still running" message:@"continue previous firmware-updateprocessing?\nclick CONTINUE to continue\nclick NEW to clear cache and start new processing" preferredStyle:UIAlertControllerStyleAlert];
            [alertController addAction:[UIAlertAction actionWithTitle:@"NEW" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
                //取消旧的MeshOTA
                [MeshOTAManager.share stopFirmwareUpdateWithCompleteHandle:^(BOOL isSuccess) {
                    [[NSUserDefaults standardUserDefaults] setValue:@(0) forKey:kDistributorAddress];
                    [[NSUserDefaults standardUserDefaults] removeObjectForKey:kDistributorPolicy];
                    [[NSUserDefaults standardUserDefaults] removeObjectForKey:kUpdateNodeAddresses];
                    [[NSUserDefaults standardUserDefaults] synchronize];
                }];
            }]];
            [alertController addAction:[UIAlertAction actionWithTitle:@"CONTINUE" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
                [weakSelf refreshContinueUI];
            }]];
            [self presentViewController:alertController animated:YES completion:nil];
        }
    }
}

- (void)refreshContinueUI {
    [self userAbled:NO];
    self.navigationItem.hidesBackButton = NO;
    [self updateInitiatorProgress:1.0];
    MeshOTAManager.share.phoneIsDistributor = NO;
    NSNumber *policyNumber = [[NSUserDefaults standardUserDefaults] valueForKey:kDistributorPolicy];
    MeshOTAManager.share.updatePolicy = policyNumber.intValue;
    NSNumber *addressNumber = [[NSUserDefaults standardUserDefaults] valueForKey:kDistributorAddress];
    MeshOTAManager.share.distributorAddress = addressNumber.intValue;
    if (MeshOTAManager.share.updatePolicy == SigUpdatePolicyType_verifyOnly) {
        [self clickVerifyOnly:self.verifyOnlyButton];
    } else {
        [self clickVerifyAndApply:self.verifyAndApplyButton];
    }
    NSMutableArray <NSNumber *>*mutableList = [NSMutableArray arrayWithArray:[[NSUserDefaults standardUserDefaults] objectForKey:kUpdateNodeAddresses]];
    [self continueFirmwareUpdateWithDeviceAddresses:mutableList];
}

- (void)continueFirmwareUpdateWithDeviceAddresses:(NSArray <NSNumber *>*)addresses {
    __weak typeof(self) weakSelf = self;
    weakSelf.receiversList = [NSMutableArray array];
    weakSelf.selectItemAddressArray = [NSMutableArray arrayWithArray:addresses];
    [weakSelf.selectItemArray removeAllObjects];
    for (NSNumber *number in addresses) {
        SigNodeModel *node = [SigDataSource.share getNodeWithAddress:number.intValue];
        if (node && ![weakSelf.selectItemArray containsObject:node]) {
            [weakSelf.selectItemArray addObject:node];
        }
    }
    [weakSelf.tableView reloadData];
    MeshOTAManager.share.needCheckVersionAfterApply = YES;
    [MeshOTAManager.share continueFirmwareUpdateWithDeviceAddresses:addresses advDistributionProgressHandle:^(SigFirmwareDistributionReceiversList *responseMessage) {
        [weakSelf showAdvDistributionProgressHandle:responseMessage];
    } finishHandle:^(NSArray<NSNumber *> *successAddresses, NSArray<NSNumber *> *failAddresses) {
        [weakSelf showFinishHandleWithSuccessAddresses:successAddresses failAddresses:failAddresses];
    } errorHandle:^(NSError * _Nullable error) {
        [weakSelf showerrorHandle:error];
    }];
    [self configReconnectUI];
}

- (void)configReconnectUI {
    [MeshOTAManager.share setPeripheralStateChangeBlock:^(CBPeripheral *peripheral) {
        dispatch_async(dispatch_get_main_queue(), ^{
            if (peripheral.state == CBPeripheralStateConnected) {
                //重连成功
                [ShowTipsHandle.share show:@"Mesh reconnected successful!"];
                [ShowTipsHandle.share delayHidden:2.0];
            } else {
                //中途断开连接
                [ShowTipsHandle.share show:@"Mesh had disconnected! APP will try to reconnect."];
            }
        });
    }];
}

- (void)updateInitiatorProgress:(float)progress {
    self.initiatorProgressView.progress = progress;
    self.initiatorProgressLabel.text = progress == 0 ? @"Initiator Progress:" : [NSString stringWithFormat:@"Initiator Progress: %d%%",(int)(progress*100)];
}

- (void)updateDistributorProgress:(float)progress {
    self.distributorProgressView.progress = progress;
    self.distributorProgressLabel.text = progress == 0 ? @"Distributor Progress:" : [NSString stringWithFormat:@"Distributor Progress: %d%%",(int)(progress*100)];
}

- (void)updateNodeModelVidWithAddress:(UInt16)address vid:(UInt16)vid{
    [self.allItemVIDDict setObject:@(vid) forKey:@(address)];
    [SigDataSource.share updateNodeModelVidWithAddress:address vid:CFSwapInt16HostToBig(vid)];
    [self performSelectorOnMainThread:@selector(delayReloadTableViewView) withObject:nil waitUntilDone:YES];
}

//刷新UI需要间隔0.1秒，防止100个设备时出现界面卡顿。
- (void)delayReloadTableViewView {
    if (!self.needDelayReloadData) {
        self.needDelayReloadData = YES;
        self.isDelaying = NO;
        dispatch_async(dispatch_get_main_queue(), ^{
            [self.tableView reloadData];
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(delayFinish) object:nil];
            [self performSelector:@selector(delayFinish) withObject:nil afterDelay:0.1];
        });
    } else {
        if (!self.isDelaying) {
            self.isDelaying = YES;
        }
    }
}

- (void)delayFinish {
    self.needDelayReloadData = NO;
    if (self.isDelaying) {
        [self delayReloadTableViewView];
    }
}

- (void)clickBackButton {
    if (MeshOTAManager.share.isMeshOTAing) {
        __weak typeof(self) weakSelf = self;
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Warning" message:@"Mesh OTA is still running, stop it?\nclick STOP to stop updating;\nclick FORCE CLOSE to exit;\nclick CANCEL to dismiss dialog" preferredStyle:UIAlertControllerStyleAlert];
        [alertController addAction:[UIAlertAction actionWithTitle:@"FORCE CLOSE" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            //强制返回上一页
            [weakSelf.navigationController popViewControllerAnimated:YES];
        }]];
        [alertController addAction:[UIAlertAction actionWithTitle:@"STOP" style:UIAlertActionStyleDestructive handler:^(UIAlertAction * _Nonnull action) {
            //强制停止MeshOTA
            [MeshOTAManager.share stopFirmwareUpdateWithCompleteHandle:^(BOOL isSuccess) {
                [[NSUserDefaults standardUserDefaults] setValue:@(0) forKey:kDistributorAddress];
                [[NSUserDefaults standardUserDefaults] removeObjectForKey:kDistributorPolicy];
                [[NSUserDefaults standardUserDefaults] removeObjectForKey:kUpdateNodeAddresses];
                [[NSUserDefaults standardUserDefaults] synchronize];
            }];
        }]];
        [alertController addAction:[UIAlertAction actionWithTitle:@"CANCEL" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
            //取消
        }]];
        [self presentViewController:alertController animated:YES completion:nil];
    } else {
        [self.navigationController popViewControllerAnimated:YES];
    }
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)viewWillDisappear:(BOOL)animated{
    [super viewWillDisappear:animated];
//    //界面可手动返回的情况，需要手动调用stopMeshOTA
//    if (MeshOTAManager.share.isMeshOTAing) {
//        [MeshOTAManager.share stopFirmwareUpdateWithCompleteHandle:nil];
//    }
}

- (void)viewDidDisappear:(BOOL)animated{
    [super viewDidDisappear:animated];
}

- (void)setStartButtonEnable:(BOOL)enable{
    self.startButton.backgroundColor = enable ? UIColor.telinkButtonBlue : UIColor.telinkButtonUnableBlue;
}

- (IBAction)clickVerifyOnly:(UIButton *)sender {
    sender.selected = YES;
    self.verifyAndApplyButton.selected = !sender.selected;
}

- (IBAction)clickVerifyAndApply:(UIButton *)sender {
    sender.selected = YES;
    self.verifyOnlyButton.selected = !sender.selected;
}

- (IBAction)clickConnectedDevice:(UIButton *)sender {
    sender.selected = YES;
    self.phoneButton.selected = !sender.selected;
    self.policyLabel.hidden = NO;
    self.verifyOnlyButton.hidden = NO;
    self.verifyOnlyLabel.hidden = NO;
    self.verifyAndApplyButton.hidden = NO;
    self.verifyAndApplyLabel.hidden = NO;
}

- (IBAction)clickPhone:(UIButton *)sender {
    sender.selected = YES;
    self.connectedDeviceButton.selected = !sender.selected;
    self.policyLabel.hidden = YES;
    self.verifyOnlyButton.hidden = YES;
    self.verifyOnlyLabel.hidden = YES;
    self.verifyAndApplyButton.hidden = YES;
    self.verifyAndApplyLabel.hidden = YES;
}

- (IBAction)clickGetFwInfo:(UIButton *)sender {
    if (SigMeshLib.share.isBusyNow) {
        TelinkLogInfo(@"send request for GetFwInfo, but busy now.");
        [self showTips:@"busy now."];
        return;
    }
    __weak typeof(self) weakSelf = self;
    [self.allItemVIDDict removeAllObjects];

    //2.firmwareUpdateInformationGet，该消息在modelID：kSigModel_FirmwareUpdateServer_ID里面。
    UInt16 modelIdentifier = kSigModel_FirmwareUpdateServer_ID;
    NSArray *curNodes = [NSArray arrayWithArray:SigDataSource.share.curNodes];
    NSInteger responseMax = 0;
    NSMutableArray *LPNArray = [NSMutableArray array];
    for (SigNodeModel *model in curNodes) {
        NSArray *addressArray = [model getAddressesWithModelID:@(modelIdentifier)];
        if (model.state != DeviceStateOutOfLine && addressArray && addressArray.count > 0 && model.features.lowPowerFeature == SigNodeFeaturesState_notSupported) {
            responseMax ++;
        }
        if (model.features.lowPowerFeature != SigNodeFeaturesState_notSupported) {
            [LPNArray addObject:model];
        }
    }

    NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
    [operationQueue addOperationWithBlock:^{
        //这个block语句块在子线程中执行
        //如果responseMax = 0，则无需发送到0xFFFF获取版本号。
        if (responseMax > 0 || (responseMax == 0 && LPNArray.count == 0)) {
            dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
            [SDKLibCommand firmwareUpdateInformationGetWithDestination:kMeshAddress_allNodes firstIndex:0 entriesLimit:1 retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:responseMax successCallback:^(UInt16 source, UInt16 destination, SigFirmwareUpdateInformationStatus * _Nonnull responseMessage) {
                if (responseMessage.firmwareInformationListCount > 0) {
                    /*
                     responseMessage.firmwareInformationList.firstObject.currentFirmwareID.length = 4: 2 bytes pid(设备类型) + 2 bytes vid(版本id).
                     */
                    if (responseMessage.firmwareInformationList.count > 0) {
                        NSData *currentFirmwareID = responseMessage.firmwareInformationList.firstObject.currentFirmwareID;
                        UInt16 pid = 0,vid = 0;
                        Byte *pu = (Byte *)[currentFirmwareID bytes];
                        if (currentFirmwareID.length >= 2) memcpy(&pid, pu, 2);
                        if (currentFirmwareID.length >= 4) memcpy(&vid, pu + 2, 2);
                        vid = CFSwapInt16HostToBig(vid);
                        TelinkLogDebug(@"firmwareUpdateInformationGet=%@,pid=%d,vid=%d",[LibTools convertDataToHexStr:currentFirmwareID],pid,vid);
                        [weakSelf updateNodeModelVidWithAddress:source vid:vid];
                    }
                }
            } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                dispatch_semaphore_signal(semaphore);
            }];
            //Most provide 4 seconds
            dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 10.0));
        }

        if (LPNArray && LPNArray.count) {
            for (SigNodeModel *model in LPNArray) {
                dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
                [SDKLibCommand firmwareUpdateInformationGetWithDestination:model.address firstIndex:0 entriesLimit:1 retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigFirmwareUpdateInformationStatus * _Nonnull responseMessage) {
                    if (responseMessage.firmwareInformationListCount > 0) {
                        /*
                         responseMessage.firmwareInformationList.firstObject.currentFirmwareID.length = 4: 2 bytes pid(设备类型) + 2 bytes vid(版本id).
                         */
                        if (responseMessage.firmwareInformationList.count > 0) {
                            NSData *currentFirmwareID = responseMessage.firmwareInformationList.firstObject.currentFirmwareID;
                            UInt16 pid = 0,vid = 0;
                            Byte *pu = (Byte *)[currentFirmwareID bytes];
                            if (currentFirmwareID.length >= 2) memcpy(&pid, pu, 2);
                            if (currentFirmwareID.length >= 4) memcpy(&vid, pu + 2, 2);
                            vid = CFSwapInt16HostToBig(vid);
                            TelinkLogDebug(@"firmwareUpdateInformationGet=%@,pid=%d,vid=%d",[LibTools convertDataToHexStr:currentFirmwareID],pid,vid);
                            [weakSelf updateNodeModelVidWithAddress:source vid:vid];
                        }
                    }
                } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                    TelinkLogInfo(@"isResponseAll=%d,error=%@",isResponseAll,error);
                    dispatch_semaphore_signal(semaphore);
                }];
                //Most provide 4 seconds
                dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 10.0));
            }
        }
    }];
}

- (IBAction)clickStartMeshOTA:(UIButton *)sender {
    if (![MeshOTAManager share].isMeshOTAing) {
        [self startMeshOTA];
    }else{
        [self showTips:@"Device is meshOTAing, needn't call repeat."];
    }
}

#pragma mark - UITableViewDataSource
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 2;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return section == 0 ? self.allItemArray.count+1 : self.binStringArray.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:NSStringFromClass(MeshOTAItemCell.class) forIndexPath:indexPath];
    [self configureCell:cell forRowAtIndexPath:indexPath];

    return cell;
}

- (void)configureCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath{
    MeshOTAItemCell *itemCell = (MeshOTAItemCell *)cell;
    __weak typeof(self) weakSelf = self;

    if (indexPath.section == 0) {
        if (indexPath.row == 0) {
            itemCell.titleLabel.text = @"choose all";
            itemCell.selectButton.selected = self.selectItemArray.count == self.allItemArray.count;
            [itemCell.selectButton addAction:^(UIButton *button) {
                if (!button.selected) {
                    NSArray *allItemArray = [NSArray arrayWithArray:weakSelf.allItemArray];
                    NSMutableArray *selectNodes = [NSMutableArray array];
                    NSMutableArray *selectAddresses = [NSMutableArray array];
                    for (SigNodeModel *model in allItemArray) {
                        if (model.state != DeviceStateOutOfLine || model.features.lowPowerFeature != SigNodeFeaturesState_notSupported) {
                            [selectNodes addObject:model];
                            [selectAddresses addObject:@(model.address)];
                        }
                    }
                    weakSelf.selectItemArray = selectNodes;
                    weakSelf.selectItemAddressArray = selectAddresses;
                } else {
                    [weakSelf.selectItemArray removeAllObjects];
                    [weakSelf.selectItemAddressArray removeAllObjects];
                }
                [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
            }];
        }else{
            SigNodeModel *model = self.allItemArray[indexPath.row-1];
            NSNumber *address = self.allItemAddressArray[indexPath.row-1];
            int vid = 0;
            if ([self.allItemVIDDict.allKeys containsObject:address]) {
                vid = [self.allItemVIDDict[address] intValue];
            }
            UInt16 modelIdentifier = kSigModel_BLOBTransferServer_ID;
            NSArray *addressArray = [model getAddressesWithModelID:@(modelIdentifier)];
            if (addressArray && addressArray.count > 0) {
                NSString *str = [NSString stringWithFormat:@"%@adr:0x%X pid:0x%X vid:0x%X",model.features.lowPowerFeature != SigNodeFeaturesState_notSupported ? @"LPN-" : @"",model.address,[LibTools uint16From16String:model.pid], vid];
                if ([self.selectItemAddressArray containsObject:@(model.address)]) {
                    SigUpdatingNodeEntryModel *nodeEntryModel = [self getSigUpdatingNodeEntryModelWithAddress:model.address];
                    if (nodeEntryModel != nil) {
                        str = [NSString stringWithFormat:@"%@ %@",str,[self getDetailStringOfSigUpdatingNodeEntryModel:nodeEntryModel]];
                    } else if ([self.failAddresses containsObject:@(model.address)]) {
                        str = [NSString stringWithFormat:@"%@ %@",str,@"fail"];
                    } else if ([self.successAddresses containsObject:@(model.address)]) {
                        str = [NSString stringWithFormat:@"%@ %@",str,@"success"];
                    }
                }
                itemCell.titleLabel.text = str;
            } else {
                vid = [LibTools uint16From16String:model.vid];
                itemCell.titleLabel.text = [NSString stringWithFormat:@"%@adr:0x%X pid:0x%X vid:0x%X Not support",model.features.lowPowerFeature != SigNodeFeaturesState_notSupported ? @"LPN-" : @"",model.address,[LibTools uint16From16String:model.pid], vid];//显示两个字节的ASCII
            }

            if (self.selectItemAddressArray.count > 0) {
                itemCell.selectButton.selected = [self.selectItemAddressArray containsObject:address];
            } else {
                itemCell.selectButton.selected = NO;
            }
            [itemCell.selectButton addAction:^(UIButton *button) {
                if (model.state != DeviceStateOutOfLine || model.features.lowPowerFeature != SigNodeFeaturesState_notSupported) {
                    if ([weakSelf.selectItemAddressArray containsObject:address]) {
                        [weakSelf.selectItemArray removeObject:model];
                        [weakSelf.selectItemAddressArray removeObject:address];
                    }else{
                        [weakSelf.selectItemArray addObject:model];
                        [weakSelf.selectItemAddressArray addObject:address];
                    }
                    [weakSelf.tableView reloadData];
                } else {
                    [weakSelf showTips:@"This node is outline."];
                    return;
                }
            }];
        }
    } else {
        NSString *binString = self.binStringArray[indexPath.row];
        NSData *data = [OTAFileSource.share getDataWithBinName:binString];
        if (data && data.length) {
            UInt16 vid = [OTAFileSource.share getVidWithOTAData:data];
            vid = CFSwapInt16HostToBig(vid);
            itemCell.titleLabel.text = [NSString stringWithFormat:@"%@ pid:0x%X vid:0x%X",binString,[OTAFileSource.share getPidWithOTAData:data], vid];//vid显示两个字节的ASCII
        } else {
            itemCell.titleLabel.text = [NSString stringWithFormat:@"%@,read bin fail!",binString];//bin文件读取失败。
        }
        itemCell.selectButton.selected = indexPath.row == _binIndex;
        [itemCell.selectButton addAction:^(UIButton *button) {
            weakSelf.binIndex = indexPath.row;
            [weakSelf checkPID];
            [weakSelf.tableView reloadData];
        }];
    }
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section{
    if (section == 0) {
        return @"Device list";
    } else {
        return @"OTA file";
    }
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath{
    return 44;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    if (indexPath.section == 0) {
        if (indexPath.row == 0) {
            __weak typeof(self) weakSelf = self;
            MeshOTAItemCell *itemCell = [tableView cellForRowAtIndexPath:indexPath];
            if (!itemCell.selectButton.selected) {
                NSArray *allItemArray = [NSArray arrayWithArray:weakSelf.allItemArray];
                NSMutableArray *selectNodes = [NSMutableArray array];
                NSMutableArray *selectAddresses = [NSMutableArray array];
                for (SigNodeModel *model in allItemArray) {
                    if (model.state != DeviceStateOutOfLine || model.features.lowPowerFeature != SigNodeFeaturesState_notSupported) {
                        [selectNodes addObject:model];
                        [selectAddresses addObject:@(model.address)];
                    }
                }
                weakSelf.selectItemArray = selectNodes;
                weakSelf.selectItemAddressArray = selectAddresses;
            } else {
                [weakSelf.selectItemArray removeAllObjects];
                [weakSelf.selectItemAddressArray removeAllObjects];
            }
        }else{
            SigNodeModel *model = self.allItemArray[indexPath.row-1];
            NSNumber *address = self.allItemAddressArray[indexPath.row-1];
            if (model.state != DeviceStateOutOfLine || model.features.lowPowerFeature != SigNodeFeaturesState_notSupported) {
                if ([self.selectItemAddressArray containsObject:address]) {
                    [self.selectItemArray removeObject:model];
                    [self.selectItemAddressArray removeObject:address];
                }else{
                    [self.selectItemArray addObject:model];
                    [self.selectItemAddressArray addObject:address];
                }
            } else {
                [self showTips:@"This node is outline."];
                return;
            }
        }
        [self.tableView reloadData];
    } else {
        if (_binIndex != indexPath.row) {
            _binIndex = indexPath.row;
            [self checkPID];
            [self.tableView reloadData];
        }
    }
}

- (void)updateSigUpdatingNodeEntryModel:(SigUpdatingNodeEntryModel *)model {
    if ([self.receiversList containsObject:model]) {
        [self.receiversList replaceObjectAtIndex:[self.receiversList indexOfObject:model] withObject:model];
    } else {
        [self.receiversList addObject:model];
    }
}

- (SigUpdatingNodeEntryModel *)getSigUpdatingNodeEntryModelWithAddress:(UInt16)address {
    SigUpdatingNodeEntryModel *nodeEntryModel = nil;
    NSArray *array = [NSArray arrayWithArray:self.receiversList];
    for (SigUpdatingNodeEntryModel *m in array) {
        if (m.address == address) {
            nodeEntryModel = m;
            break;
        }
    }
    return nodeEntryModel;
}

- (void)checkPID {
    NSString *binString = self.binStringArray[_binIndex];
    NSData *data = [OTAFileSource.share getDataWithBinName:binString];
    UInt16 pid = [OTAFileSource.share getPidWithOTAData:data];

    //如果存在非当前PID的设备选中了要进行MeshOTA，则全部取消设备的选中，让客户重新选择。
    BOOL chooseDifferent = NO;
    for (SigNodeModel *node in self.selectItemArray) {
        if ([LibTools uint16From16String:node.pid] != pid) {
            chooseDifferent = YES;
            break;
        }
    }
    if (chooseDifferent) {
        [self.selectItemArray removeAllObjects];
        [self.selectItemAddressArray removeAllObjects];
        [self.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
        [self showAlertSureWithTitle:@"Hits" message:@"The PID of node is different from the PID of Bin file! Please choose device again." sure:nil];
    }
}

- (void)startMeshOTA{
    if (!SigBearer.share.isOpen) {
        [self showTips:@"APP is not connected mesh."];
        return;
    }
    if (self.selectItemArray.count == 0) {
        [self showTips:@"Please choose some devices for mesh OTA."];
        return;
    }
    if (self.binIndex < 0) {
        [self showTips:@"Please choose the bin file for mesh OTA."];
        return;
    }

    TelinkLogVerbose(@"选中的bin文件为%@",self.binStringArray[self.binIndex]);
    NSData *data = [OTAFileSource.share getDataWithBinName:self.binStringArray[self.binIndex]];
    if (data == nil || data.length == 0) {
        [self showTips:@"APP can't load this Bin file."];
        return;
    }
    NSMutableArray *tem = [NSMutableArray array];
    NSArray *selectItemArray = [NSArray arrayWithArray:self.selectItemArray];
    for (SigNodeModel *model in selectItemArray) {
        [tem addObject:@(model.address)];
    }

    NSData *incomingFirmwareMetadata = nil;
    if (data && data.length >= 6) {
        //incomingFirmwareMetadata默认为8个字节的0。需要bin文件里面从index为2开始取4个字节的数据，再补充4个字节的0。
        UInt32 tem32 = 0;
        NSData *temData = [NSData dataWithBytes:&tem32 length:4];
        NSMutableData *mData = [NSMutableData dataWithData:[data subdataWithRange:NSMakeRange(2, 4)]];
        [mData appendData:temData];
        incomingFirmwareMetadata = mData;
    } else {
        [self showTips:@"This Bin file is invalid."];
        return;
    }
    if (SigMeshLib.share.isBusyNow) {
        TelinkLogInfo(@"click start MeshOTA, but busy now.");
        [self showTips:@"busy now."];
        return;
    }

    [self updateInitiatorProgress:0];
    [self updateDistributorProgress:0];
    [self userAbled:NO];
    self.receiversList = [NSMutableArray array];
    self.successAddresses = [NSMutableArray array];
    self.failAddresses = [NSMutableArray array];
    [self.tableView reloadData];

    __weak typeof(self) weakSelf = self;

    //模拟的4K测试固件数据，设备端会校验失败而ota失败
    //==========test==========//
//        UInt8 tem8 = 0;
//        data = [NSData dataWithBytes:&tem8 length:4 * 1024];
    //==========test==========//

    //新版本meshOTA接口
    MeshOTAManager.share.updatePolicy = self.verifyOnlyButton.selected ? SigUpdatePolicyType_verifyOnly : SigUpdatePolicyType_verifyAndApply;
    MeshOTAManager.share.phoneIsDistributor = self.connectedDeviceButton.selected ? NO : YES;
    MeshOTAManager.share.needCheckVersionAfterApply = YES;

    [[NSUserDefaults standardUserDefaults] setValue:tem forKey:kUpdateNodeAddresses];
    [[NSUserDefaults standardUserDefaults] setValue:@(MeshOTAManager.share.updatePolicy) forKey:kDistributorPolicy];
    [[NSUserDefaults standardUserDefaults] synchronize];

    [MeshOTAManager.share startFirmwareUpdateWithDeviceAddresses:tem otaData:data incomingFirmwareMetadata:incomingFirmwareMetadata gattDistributionProgressHandle:^(NSInteger progress) {
        [weakSelf showGattDistributionProgressHandle:progress];
    } advDistributionProgressHandle:^(SigFirmwareDistributionReceiversList *responseMessage) {
        [weakSelf showAdvDistributionProgressHandle:responseMessage];
    } finishHandle:^(NSArray<NSNumber *> *successAddresses, NSArray<NSNumber *> *failAddresses) {
        [weakSelf showFinishHandleWithSuccessAddresses:successAddresses failAddresses:failAddresses];
    } errorHandle:^(NSError * _Nullable error) {
        [weakSelf showerrorHandle:error];
    }];
    [self configReconnectUI];
}

- (void)showGattDistributionProgressHandle:(NSInteger)progress {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self updateInitiatorProgress:progress / 100.0];
    });
}

- (void)showAdvDistributionProgressHandle:(SigFirmwareDistributionReceiversList *)responseMessage {
//    TelinkLogInfo(@"adv responseMessage=%@",responseMessage);
//            if (responseMessage && responseMessage.receiversList && responseMessage.receiversList.count > 0) {
    if (responseMessage && responseMessage.receiversList) {
        dispatch_async(dispatch_get_main_queue(), ^{
            if (!MeshOTAManager.share.phoneIsDistributor) {
                self.navigationItem.hidesBackButton = NO;
                NSNumber *distributorAddress = [NSNumber numberWithInteger:MeshOTAManager.share.distributorAddress];
                [[NSUserDefaults standardUserDefaults] setValue:distributorAddress forKey:kDistributorAddress];
                [[NSUserDefaults standardUserDefaults] synchronize];
            }
            [self updateReceiversList:responseMessage.receiversList];
        });
    }
}

- (void)showFinishHandleWithSuccessAddresses:(NSArray<NSNumber *> *)successAddresses  failAddresses:(NSArray<NSNumber *> *)failAddresses {
    NSString *tip = [NSString stringWithFormat:@"Mesh ota finish, success:%ld,fail:%ld", (long)successAddresses.count, (long)failAddresses.count];
    self.successAddresses = [NSMutableArray arrayWithArray:successAddresses];
    self.failAddresses = [NSMutableArray arrayWithArray:failAddresses];
    [self.receiversList removeAllObjects];
    dispatch_async(dispatch_get_main_queue(), ^{
        [[NSUserDefaults standardUserDefaults] setValue:@(0) forKey:kDistributorAddress];
        [[NSUserDefaults standardUserDefaults] removeObjectForKey:kDistributorPolicy];
        [[NSUserDefaults standardUserDefaults] removeObjectForKey:kUpdateNodeAddresses];
        [[NSUserDefaults standardUserDefaults] synchronize];
        [self userAbled:YES];
        [ShowTipsHandle.share delayHidden:0];
        [self showTips:tip];
        [self.tableView reloadData];
        if (self.successAddresses.count) {
            TelinkLogVerbose(@"");
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
                TelinkLogVerbose(@"");
                [self clickGetFwInfo:self.getFWInfoButton];
            });
        }
    });
}

- (void)showerrorHandle:(NSError * _Nullable)error {
    NSMutableArray *tem = [NSMutableArray array];
    NSArray *selectItemArray = [NSArray arrayWithArray:self.selectItemArray];
    for (SigNodeModel *model in selectItemArray) {
        [tem addObject:@(model.address)];
    }

    NSString *tip = [NSString stringWithFormat:@"Mesh ota fail, error = %@", error];
    dispatch_async(dispatch_get_main_queue(), ^{
        [[NSUserDefaults standardUserDefaults] setValue:@(0) forKey:kDistributorAddress];
        [[NSUserDefaults standardUserDefaults] removeObjectForKey:kDistributorPolicy];
        [[NSUserDefaults standardUserDefaults] removeObjectForKey:kUpdateNodeAddresses];
        [[NSUserDefaults standardUserDefaults] synchronize];
        [self userAbled:YES];
        [ShowTipsHandle.share delayHidden:0];
        [self showTips:tip];
        [self.receiversList removeAllObjects];
        self.failAddresses = [NSMutableArray arrayWithArray:tem];
        [self.tableView reloadData];
    });
    [SDKLibCommand startMeshConnectWithComplete:nil];
}

- (void)updateReceiversList:(NSArray <SigUpdatingNodeEntryModel *>*)receiversList {
    if (receiversList ==nil || (receiversList && receiversList.count == 0)) {
//        [self updateDistributorProgress:100 / 100.0];
    } else {
        for (SigUpdatingNodeEntryModel *model in receiversList) {
            [self updateSigUpdatingNodeEntryModel:model];
        }

        //进度显示所有SigUpdatingNodeEntryModel中的最大进度
        NSInteger progress = 0;
        for (SigUpdatingNodeEntryModel *model in self.receiversList) {
            if (model.transferProgress * 2 > progress) {
                progress = model.transferProgress * 2;
            }
        }
        [self updateDistributorProgress:progress / 100.0];
        [self.tableView reloadData];
    }
}

- (NSString *)getDetailStringOfSigUpdatingNodeEntryModel:(SigUpdatingNodeEntryModel *)model {
    NSString *phaseString = [SigHelper.share getDetailOfSigFirmwareUpdatePhaseType:model.retrievedUpdatePhase];
    if (model.retrievedUpdatePhase == SigFirmwareUpdatePhaseType_applySuccess) {
        phaseString = @"success";
    }
    NSString *tem = [NSString stringWithFormat:@"%@",phaseString];
    return tem;
}

- (void)userAbled:(BOOL)able{
    self.navigationItem.hidesBackButton = !able;
    self.startButton.enabled = able;
    self.tableView.userInteractionEnabled = able;
    [self setStartButtonEnable:able];
}

-(void)dealloc{
    TelinkLogInfo(@"");
}
@end
