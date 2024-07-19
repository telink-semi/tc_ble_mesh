/********************************************************************************************************
 * @file     MeshOTAVC.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/4/17
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

#import "MeshOTAVC.h"
#import "MeshOTACell.h"
#import "UIButton+extension.h"
#import "HomeViewController.h"
#import "UIViewController+Message.h"
#ifndef kIsTelinkCloudSigMeshLib
#import "OTAFileSource.h"
#import "BinFileChooseVC.h"
#import "DeviceSelectVC.h"
#else
#import "SelectUpdateDeviceVC.h"
#endif


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
@property (weak, nonatomic) IBOutlet UIButton *binFileNameButton;
@property (weak, nonatomic) IBOutlet UILabel *binFileVersionLabel;
@property (weak, nonatomic) IBOutlet NSLayoutConstraint *tableViewBottom;
@property (nonatomic, strong) NSMutableArray <SigNodeModel *>*selectItemArray;
@property (nonatomic, strong) NSMutableArray <NSNumber *>*selectItemAddressArray;
//@property (nonatomic, assign) NSInteger binIndex;
@property (nonatomic, strong) NSMutableDictionary *allItemVIDDict;
/// key: @(address), value: SigFirmwareUpdateInformationStatus
@property (nonatomic, strong) NSMutableDictionary *allNodeFirmwareUpdateInformationStatusDict;
@property (assign, nonatomic) BOOL needDelayReloadData;
@property (assign, nonatomic) BOOL isDelaying;
@property (nonatomic, strong) NSMutableArray <SigUpdatingNodeEntryModel *>*receiversList;
@property (nonatomic, strong) NSMutableArray <NSNumber *>*successAddresses;
@property (nonatomic, strong) NSMutableArray <NSNumber *>*failAddresses;
#ifdef kIsTelinkCloudSigMeshLib
@property (nonatomic, strong) CloudVersionInfoModel *currentVersionInfo;
#else
@property (nonatomic, strong) NSString *currentBinString;
#endif
@end

@implementation MeshOTAVC

- (void)viewDidLoad {
    [super viewDidLoad];
#ifdef kIsTelinkCloudSigMeshLib
//隐藏选择Bin文件的UI
    self.tableViewBottom.constant = 0;
#endif
    self.startButton.backgroundColor = UIColor.telinkButtonBlue;
    self.getFWInfoButton.backgroundColor = UIColor.telinkButtonBlue;
    self.title = @"Mesh OTA";
    self.needDelayReloadData = NO;
    [self clickPhone:self.phoneButton];
    self.verifyAndApplyButton.selected = YES;
    self.verifyOnlyButton.selected = NO;
    [self updateInitiatorProgress:0];
    [self updateDistributorProgress:0];
    //init rightBarButtonItem
    UIBarButtonItem *leftItem = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"back"] style:UIBarButtonItemStylePlain target:self action:@selector(clickBackButton)];
    self.navigationItem.leftBarButtonItem = leftItem;

    [self.tableView registerNib:[UINib nibWithNibName:NSStringFromClass(MeshOTACell.class) bundle:nil] forCellReuseIdentifier:NSStringFromClass(MeshOTACell.class)];

    self.selectItemArray = [NSMutableArray array];
    self.selectItemAddressArray = [NSMutableArray array];
#ifndef kIsTelinkCloudSigMeshLib
    self.currentBinString = nil;
#endif
    self.allItemVIDDict = [NSMutableDictionary dictionary];
    self.allNodeFirmwareUpdateInformationStatusDict = [NSMutableDictionary dictionary];
    self.successAddresses = [NSMutableArray array];
    self.failAddresses = [NSMutableArray array];

    [MeshOTAManager.share saveIsMeshOTAing:NO];

    if (_isContinue) {
        self.connectedDeviceButton.selected = YES;
        self.phoneButton.selected = NO;
        self.policyLabel.hidden = self.verifyOnlyButton.hidden = self.verifyOnlyLabel.hidden = self.verifyAndApplyButton.hidden = self.verifyAndApplyLabel.hidden = NO;
        [self refreshContinueUI];
    } else {
        NSNumber *addressNumber = [[NSUserDefaults standardUserDefaults] valueForKey:kDistributorAddress];
        if (addressNumber == nil || addressNumber.intValue == 0 || addressNumber.intValue == SigDataSource.share.curLocationNodeModel.address) {
            //不弹框
            [self refreshDistributorUI];
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
                    [[NSUserDefaults standardUserDefaults] removeObjectForKey:kDistributorBinString];
                    [[NSUserDefaults standardUserDefaults] synchronize];
                }];
                [weakSelf refreshDistributorUI];
            }]];
            [alertController addAction:[UIAlertAction actionWithTitle:@"CONTINUE" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
                [weakSelf refreshContinueUI];
            }]];
            [self presentViewController:alertController animated:YES completion:nil];
        }
    }
}

- (void)refreshDistributorUI {
    //进入MeshOTA，如果直连节点支持作为distributor，distributor默认选中直连节点；如果直连节点不支持作为distributor，distributor默认选中手机。
    SigNodeModel *node = SigDataSource.share.getCurrentConnectedNode;
    SigModelIDModel *modelId = [node getModelIDModelWithModelID:kSigModel_FirmwareDistributionServer_ID];
    if (modelId == nil) {
        [self.connectedDeviceButton setImage:[UIImage imageNamed:@"bukexuan"] forState:UIControlStateNormal];
        [self clickPhone:self.phoneButton];
    } else {
        [self clickConnectedDevice:self.connectedDeviceButton];
    }
}

- (void)refreshContinueUI {
    [self userAbled:NO];
    self.navigationItem.hidesBackButton = NO;
    [self updateInitiatorProgress:1.0];
    MeshOTAManager.share.phoneIsDistributor = NO;
    [self clickConnectedDevice:self.connectedDeviceButton];
    NSNumber *policyNumber = [[NSUserDefaults standardUserDefaults] valueForKey:kDistributorPolicy];
    MeshOTAManager.share.updatePolicy = policyNumber.intValue;
    NSNumber *addressNumber = [[NSUserDefaults standardUserDefaults] valueForKey:kDistributorAddress];
    MeshOTAManager.share.distributorAddress = addressNumber.intValue;
    if (MeshOTAManager.share.updatePolicy == SigUpdatePolicyType_verifyOnly) {
        [self clickVerifyOnly:self.verifyOnlyButton];
    } else {
        [self clickVerifyAndApply:self.verifyAndApplyButton];
    }
#ifndef kIsTelinkCloudSigMeshLib
    NSString *binString = [NSUserDefaults.standardUserDefaults valueForKey:kDistributorBinString];
    self.currentBinString = binString;
    [self refreshBinFileUI];
#endif
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
#ifdef kIsTelinkCloudSigMeshLib
    // 云端APP不可OTA到不同的PID的Bin文件
    NSArray *selectArray = [NSArray arrayWithArray:self.selectItemArray];
    NSMutableArray <NSNumber *>*pidArray = [NSMutableArray array];
    for (SigNodeModel *node in selectArray) {
        if (![pidArray containsObject:@([LibTools uint16From16String:node.pid])]) {
            [pidArray addObject:@([LibTools uint16From16String:node.pid])];
        }
    }
    if (pidArray.count != 1) {
        [self showTips:@"Please choose the device with the same productId."];
        return;
    }
    NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
    [operationQueue addOperationWithBlock:^{
        //这个block语句块在子线程中执行
        dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
        [self getCloudVersionInfoWithResultBlock:^(id  _Nullable result, NSError * _Nullable err) {
            dispatch_semaphore_signal(semaphore);
        }];
        dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 10.0));
        NSData *data = [self loadBinFileWithCloudVersionInfoModel:self.currentVersionInfo];
        TelinkLogVerbose(@"选中的bin文件为%@", self.currentVersionInfo.name);
        if (data == nil || data.length == 0) {
            [self showTips:@"APP can't load this Bin file."];
            return;
        }
        MeshOTAManager.share.otaData = data;
    }];
#else
    if (self.currentBinString == nil || self.currentBinString.length == 0) {
        [self showTips:@"Please choose the bin file for mesh OTA."];
        return;
    }
    TelinkLogVerbose(@"选中的bin文件为%@",self.currentBinString);
    NSData *data = [OTAFileSource.share getDataWithBinName:self.currentBinString];
    if (data == nil || data.length == 0) {
        [self showTips:@"APP can't load this Bin file."];
        return;
    }
    MeshOTAManager.share.otaData = data;
#endif

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
    self.initiatorProgressLabel.text = progress == 0 ? @"Initiate Progress:" : [NSString stringWithFormat:@"Initiate Progress: %d%%",(int)(progress*100)];
}

- (void)updateDistributorProgress:(float)progress {
    self.distributorProgressView.progress = progress;
    self.distributorProgressLabel.text = progress == 0 ? @"Distribute Progress:" : [NSString stringWithFormat:@"Distribute Progress: %d%%",(int)(progress*100)];
}

- (void)updateNodeModelVidWithAddress:(UInt16)address vid:(UInt16)vid{
    [self.allItemVIDDict setObject:@(vid) forKey:@(address)];
    [SigDataSource.share updateNodeModelVidWithAddress:address vid:CFSwapInt16HostToBig(vid)];
    [self performSelectorOnMainThread:@selector(delayReloadTableViewView) withObject:nil waitUntilDone:YES];
}

// Refreshing the UI requires an interval of 0.1 seconds to prevent interface stuttering when there are 100 devices.
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
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:kDefaultAlertTitle message:@"Mesh OTA is still running, stop it?\nclick STOP to stop updating;\nclick FORCE CLOSE to exit;\nclick CANCEL to dismiss dialog" preferredStyle:UIAlertControllerStyleAlert];
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
                [[NSUserDefaults standardUserDefaults] removeObjectForKey:kDistributorBinString];
                [[NSUserDefaults standardUserDefaults] synchronize];
            }];
        }]];
        [alertController addAction:[UIAlertAction actionWithTitle:kDefaultAlertCancel style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
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
    SigNodeModel *node = SigDataSource.share.getCurrentConnectedNode;
    SigModelIDModel *modelId = [node getModelIDModelWithModelID:kSigModel_FirmwareDistributionServer_ID];
    if (modelId == nil) {
        [self showTips:@"The current directly connected node does not support serving as a distributor role."];
        return;
    }
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

#ifdef kIsTelinkCloudSigMeshLib
- (void)getCloudVersionInfoWithResultBlock:(MyBlock)block {
    if (self.selectItemArray.count > 0) {
        SigNodeModel *node = self.selectItemArray.firstObject;
        __weak typeof(self) weakSelf = self;
        [TelinkHttpTool getLatestVersionInfoRequestWithProductId:[LibTools uint16From16String:node.pid] didLoadData:^(id  _Nullable result, NSError * _Nullable err) {
            if (err) {
                [weakSelf showTips:[NSString stringWithFormat:@"%@", err.localizedDescription]];
            } else {
                NSDictionary *dic = (NSDictionary *)result;
                int code = [dic[@"code"] intValue];
                if (code == 200) {
                    TelinkLogInfo(@"getLatestVersionInfo successful! dic=%@", dic);
                    NSDictionary *dict = dic[@"data"];
                    if (dict.count > 0) {
                        CloudVersionInfoModel *version = [[CloudVersionInfoModel alloc] init];
                        [version setDictionaryToCloudVersionInfoModel:dict];
                        weakSelf.currentVersionInfo = version;
                    }
                } else {
                    NSString *msg = [NSString stringWithFormat:@"getLatestVersionInfo errorCode = %d, message = %@", code, dic[@"message"]];
                    [weakSelf showTips:msg];
                }
            }
            if (block) {
                block(result, err);
            }
        }];
    }
}
#else
- (void)refreshBinFileUI {
    if (self.currentBinString == nil || self.currentBinString.length == 0) {
        [self.binFileNameButton setTitle:@"file error" forState:UIControlStateNormal];
        self.binFileVersionLabel.text = @"bin version: null";
    } else {
        [self.binFileNameButton setTitle:self.currentBinString forState:UIControlStateNormal];
        NSData *data = [OTAFileSource.share getDataWithBinName:self.currentBinString];
        if (data && data.length) {
            UInt16 vid = [OTAFileSource.share getVidWithOTAData:data];
            vid = CFSwapInt16HostToBig(vid);
            self.binFileVersionLabel.text = [NSString stringWithFormat:@"bin version: pid-0x%X vid-0x%X", [OTAFileSource.share getPidWithOTAData:data], vid];//vid显示两个字节的ASCII
        } else {
            self.binFileVersionLabel.text = @"read bin fail!";
        }
    }
}
#endif

- (IBAction)clickGetFwInfo:(UIButton *)sender {
#ifdef kIsTelinkCloudSigMeshLib
    [self getCloudVersionInfoWithResultBlock:nil];
#endif
    if (SigMeshLib.share.isBusyNow) {
        TelinkLogInfo(@"send request for GetFwInfo, but busy now.");
        [self showTips:@"busy now."];
        return;
    }
    __weak typeof(self) weakSelf = self;
    [self.allItemVIDDict removeAllObjects];
    //2.firmwareUpdateInformationGet，该消息在modelID：kSigModel_FirmwareUpdateServer_ID里面。
    UInt16 modelIdentifier = kSigModel_FirmwareUpdateServer_ID;
    NSArray *curNodes = [NSArray arrayWithArray:self.selectItemArray];
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
                        weakSelf.allNodeFirmwareUpdateInformationStatusDict[@(source)] = responseMessage;
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
                            weakSelf.allNodeFirmwareUpdateInformationStatusDict[@(source)] = responseMessage;
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

- (IBAction)clickSelectDevicesButton:(UIButton *)sender {
#ifndef kIsTelinkCloudSigMeshLib
    DeviceSelectVC *vc = [[DeviceSelectVC alloc] init];
#else
    SelectUpdateDeviceVC *vc = [[SelectUpdateDeviceVC alloc] init];
#endif    
    __weak typeof(self) weakSelf = self;
    [vc setBackSelectNodes:^(NSArray<SigNodeModel *> * _Nonnull nodes) {
        weakSelf.selectItemArray = [NSMutableArray arrayWithArray:nodes];
        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
        [weakSelf clickGetFwInfo:weakSelf.getFWInfoButton];
    }];
    [self.navigationController pushViewController:vc animated:YES];
}

- (IBAction)clickSelectBinFileButton:(UIButton *)sender {
#ifndef kIsTelinkCloudSigMeshLib
    BinFileChooseVC *vc = [[BinFileChooseVC alloc] init];
    __weak typeof(self) weakSelf = self;
    [vc setBackSelectBinString:^(NSString * _Nonnull binString) {
        weakSelf.currentBinString = binString;
        [weakSelf refreshBinFileUI];
    }];
    [self.navigationController pushViewController:vc animated:YES];
#endif
}

#pragma mark - UITableViewDataSource

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.selectItemArray.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    MeshOTACell *cell = (MeshOTACell *)[tableView dequeueReusableCellWithIdentifier:NSStringFromClass(MeshOTACell.class) forIndexPath:indexPath];
    SigNodeModel *node = self.selectItemArray[indexPath.row];
    [cell setModel:node];
    if ([self.allNodeFirmwareUpdateInformationStatusDict.allKeys containsObject:@(node.address)]) {
        SigFirmwareUpdateInformationStatus *status = self.allNodeFirmwareUpdateInformationStatusDict[@(node.address)];
        cell.firmwareLabel.text = [NSString stringWithFormat:@"FwId:%@", status.firmwareInformationList.firstObject.getFirmwareIDString];
    } else {
        cell.firmwareLabel.text = @"FwId:NULL";
    }
    cell.stateImage.image = [UIImage imageNamed:@"ic_mesh_ota"];
    if ([self.successAddresses containsObject:@(node.address)]) {
        cell.stateImage.image = [UIImage imageNamed:@"ic_mesh_ota_complete"];
    } else if ([self.failAddresses containsObject:@(node.address)]) {
        cell.stateImage.image = [UIImage imageNamed:@"ic_mesh_ota_fail"];
    }
    NSString *string = @"state: INIT\n";
    SigUpdatingNodeEntryModel *nodeEntryModel = [self getSigUpdatingNodeEntryModelWithAddress:node.address];
    if (nodeEntryModel) {
        string = [NSString stringWithFormat:@"state: %@\n", [self getDetailStringOfSigUpdatingNodeEntryModel:nodeEntryModel]];
    }
    if ([MeshOTAManager.share.additionalInformationDictionary.allKeys containsObject:@(node.address)]) {
        string = [string stringByAppendingFormat:@"additional: %@", [SigHelper.share getDetailOfSigFirmwareUpdateAdditionalInformationStatusType:[MeshOTAManager.share.additionalInformationDictionary[@(node.address)] intValue]]];
    } else {
        string = [string stringByAppendingString:@"additional: NULL"];
    }
    cell.stateLabel.text = string;
    return cell;
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
#ifdef kIsTelinkCloudSigMeshLib
    UInt16 pid = self.currentVersionInfo.productId;
#else
    NSData *data = [OTAFileSource.share getDataWithBinName:self.currentBinString];
    UInt16 pid = [OTAFileSource.share getPidWithOTAData:data];
#endif

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
        [self showTips:@"The PID of node is different from the PID of Bin file! Please choose device again."];
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
#ifdef kIsTelinkCloudSigMeshLib
    // 云端APP不可OTA到不同的PID的Bin文件
    NSArray *selectArray = [NSArray arrayWithArray:self.selectItemArray];
    NSMutableArray <NSNumber *>*pidArray = [NSMutableArray array];
    for (SigNodeModel *node in selectArray) {
        if (![pidArray containsObject:@([LibTools uint16From16String:node.pid])]) {
            [pidArray addObject:@([LibTools uint16From16String:node.pid])];
        }
    }
    if (pidArray.count != 1) {
        [self showTips:@"Please choose the device with the same productId."];
        return;
    }
    NSData *data = [self loadBinFileWithCloudVersionInfoModel:self.currentVersionInfo];
    TelinkLogVerbose(@"选中的bin文件为%@", self.currentVersionInfo.name);
#else
    if (self.currentBinString == nil || self.currentBinString.length == 0) {
        [self showTips:@"Please choose the bin file for mesh OTA."];
        return;
    }
    TelinkLogVerbose(@"选中的bin文件为%@",self.currentBinString);
    NSData *data = [OTAFileSource.share getDataWithBinName:self.currentBinString];
#endif
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
#ifndef kIsTelinkCloudSigMeshLib
    [[NSUserDefaults standardUserDefaults] setValue:self.currentBinString forKey:kDistributorBinString];
#endif
    [[NSUserDefaults standardUserDefaults] synchronize];

    [MeshOTAManager.share setFirmwareUpdateFirmwareMetadataCheckSuccessHandle:^(NSDictionary *dict) {
        [weakSelf.tableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
    }];
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

#ifdef kIsTelinkCloudSigMeshLib
/// 从沙盒里面读取bin文件，nil则本地没有该文件
- (NSData *)loadBinFileWithCloudVersionInfoModel:(CloudVersionInfoModel *)versionInfo {
    NSData *data = nil;
    NSArray *array = [versionInfo.binFilePath componentsSeparatedByString:@"\\"]; //从字符/中分隔成多个元素的数组
    NSString *fileName = [array lastObject];
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *path = [paths objectAtIndex:0];
    NSString *filePath = [path stringByAppendingPathComponent:fileName];
    NSFileManager *fileManager = [NSFileManager defaultManager];
    BOOL result = [fileManager fileExistsAtPath:filePath];
    if (result) {
        data = [[NSFileHandle fileHandleForReadingAtPath:filePath] readDataToEndOfFile];
    }
    return data;
}
#endif

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
    dispatch_async(dispatch_get_main_queue(), ^{
        [[NSUserDefaults standardUserDefaults] setValue:@(0) forKey:kDistributorAddress];
        [[NSUserDefaults standardUserDefaults] removeObjectForKey:kDistributorPolicy];
        [[NSUserDefaults standardUserDefaults] removeObjectForKey:kUpdateNodeAddresses];
        [[NSUserDefaults standardUserDefaults] removeObjectForKey:kDistributorBinString];
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

    NSString *tip = [NSString stringWithFormat:@"Mesh ota fail, error string = %@, error code = %ld", error.domain, (long)error.code];
    dispatch_async(dispatch_get_main_queue(), ^{
        [[NSUserDefaults standardUserDefaults] setValue:@(0) forKey:kDistributorAddress];
        [[NSUserDefaults standardUserDefaults] removeObjectForKey:kDistributorPolicy];
        [[NSUserDefaults standardUserDefaults] removeObjectForKey:kUpdateNodeAddresses];
        [[NSUserDefaults standardUserDefaults] removeObjectForKey:kDistributorBinString];
        [[NSUserDefaults standardUserDefaults] synchronize];
        [self userAbled:YES];
        [ShowTipsHandle.share delayHidden:0];
        [self showTips:tip];
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
