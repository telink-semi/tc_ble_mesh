/********************************************************************************************************
 * @file     HomeViewController.m
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

#import "HomeViewController.h"
#import "LogViewController.h"
#import "CMDViewController.h"
#import "ReKeyBindViewController.h"
#import "HomeItemCell.h"
#import "SingleDeviceViewController.h"
#import "UIViewController+Message.h"
#import "SensorVC.h"
#import "MeshOTAVC.h"

@interface HomeViewController()<UICollectionViewDelegate,UICollectionViewDataSource,SigBearerDataDelegate,SigDataSourceDelegate,SigMessageDelegate,SigBluetoothDelegate>
@property (strong, nonatomic) NSMutableArray <SigNodeModel *>*source;
@property (weak, nonatomic) IBOutlet UICollectionView *collectionView;
@property (weak, nonatomic) IBOutlet UIButton *allONButton;
@property (weak, nonatomic) IBOutlet UIButton *allOFFButton;
@property (weak, nonatomic) IBOutlet UIButton *cmdButton;
@property (weak, nonatomic) IBOutlet UIButton *logButton;
@property (assign, nonatomic) BOOL shouldSetAllOffline;//APP will set all nodes offline when user click refresh button.
@property (assign, nonatomic) BOOL needDelayReloadData;
@property (assign, nonatomic) BOOL isDelaying;

@end

@implementation HomeViewController

- (IBAction)allON:(UIButton *)sender {
    int tem = 0;
    NSArray *curNodes = [NSArray arrayWithArray:SigDataSource.share.curNodes];
    for (SigNodeModel *node in curNodes) {
        if (!node.isSensor && !node.isRemote && node.isKeyBindSuccess && node.state != DeviceStateOutOfLine) {
            tem++;
        }
    }
    __weak typeof(self) weakSelf = self;
    [DemoCommand switchOnOffWithIsOn:sender.tag == 100 address:kMeshAddress_allNodes responseMaxCount:tem ack:YES successCallback:^(UInt16 source, UInt16 destination, SigGenericOnOffStatus * _Nonnull responseMessage) {
        [weakSelf delayReloadCollectionView];
    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {

    }];
}

#pragma mark add node entrance
- (void)addNewDevice {
    //v3.3.3.6及之后版本，初次分享过来，没有ivIndex时，需要连接mesh成功或者用户手动输入ivIndex。
    if (!SigDataSource.share.existLocationIvIndexAndLocationSequenceNumber) {
        __weak typeof(self) weakSelf = self;
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Hits" message:@"connect to the current network to get IV index before add nodes or input IV index in input box？" preferredStyle:UIAlertControllerStyleAlert];
        [alertController addAction:[UIAlertAction actionWithTitle:@"Input IV index" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            TelinkLogDebug(@"点击输入ivIndex");
            UIAlertController *inputAlertController = [UIAlertController alertControllerWithTitle:@"Hits" message:@"Please input IV index in input box" preferredStyle:UIAlertControllerStyleAlert];
            [inputAlertController addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
                textField.placeholder = @"Please input IV index in input box";
                textField.text = @"0";
            }];
            [inputAlertController addAction:[UIAlertAction actionWithTitle:@"Done" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
                TelinkLogDebug(@"输入ivIndex完成");
                UITextField *ivIndex = inputAlertController.textFields.firstObject;
                UInt32 ivIndexUInt32 = [LibTools uint32From16String:ivIndex.text];
                [SigDataSource.share setIvIndexUInt32:ivIndexUInt32];
                [SigDataSource.share setSequenceNumberUInt32:0];
                [SigDataSource.share saveCurrentIvIndex:ivIndexUInt32 sequenceNumber:0];
                TelinkLogDebug(@"输入ivIndex=%d",ivIndexUInt32);
                UIAlertController *pushAlertController = [UIAlertController alertControllerWithTitle:@"Hits" message:[NSString stringWithFormat:@"IV index = 0x%08X, start add devices.", ivIndexUInt32] preferredStyle:UIAlertControllerStyleAlert];
                [pushAlertController addAction:[UIAlertAction actionWithTitle:@"Add Devices" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
                    [weakSelf pushToAddDeviceVC];
                }]];
                [weakSelf presentViewController:pushAlertController animated:YES completion:nil];
            }]];
            [inputAlertController addAction:[UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
                TelinkLogDebug(@"点击取消");
            }]];
            [weakSelf presentViewController:inputAlertController animated:YES completion:nil];
        }]];
        [alertController addAction:[UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
            TelinkLogDebug(@"点击取消");
        }]];
        [self presentViewController:alertController animated:YES completion:nil];
    } else {
        [self pushToAddDeviceVC];
    }
}

- (void)pushToAddDeviceVC {
    [SDKLibCommand setBluetoothCentralUpdateStateCallback:nil];
    [SigDataSource.share setAllDevicesOutline];
    ProvisionMode mode = [[[NSUserDefaults standardUserDefaults] valueForKey:kProvisionMode] intValue];
    if (mode == ProvisionMode_normalSelectable) {
        TelinkLogVerbose(@"click normal selectable add device");
        //先扫描，用户选择添加设备
        UIViewController *vc = [UIStoryboard initVC:ViewControllerIdentifiers_AddDeviceVCID];
        [self.navigationController pushViewController:vc animated:YES];
    } else if (mode == ProvisionMode_normalAuto) {
        TelinkLogVerbose(@"click normal auto add device");
        //自动添加多个设备
        UIViewController *vc = [UIStoryboard initVC:ViewControllerIdentifiers_AutoAddDeviceVCID];
        [self.navigationController pushViewController:vc animated:YES];
    } else if (mode == ProvisionMode_remoteProvision) {
        //remote Provision
        TelinkLogVerbose(@"click remote provision add device");
        UIViewController *vc = [UIStoryboard initVC:ViewControllerIdentifiers_RemoteAddVCID];
        [self.navigationController pushViewController:vc animated:YES];
    } else if (mode == ProvisionMode_fastProvision) {
        //fast provision
        TelinkLogVerbose(@"click fast provision add device");
        UIViewController *vc = [UIStoryboard initVC:ViewControllerIdentifiers_FastProvisionAddViewControllerID];
        [self.navigationController pushViewController:vc animated:YES];
    }
}

#pragma mark see log entrance

- (IBAction)clickToCMDVC:(UIButton *)sender {
    CMDViewController *vc = (CMDViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_CMDViewControllerID];
    [self.navigationController pushViewController:vc animated:YES];
}

- (IBAction)clickToLogVC:(UIButton *)sender {
    UIViewController *vc = [UIStoryboard initVC:ViewControllerIdentifiers_LogViewControllerID];
    [self.navigationController pushViewController:vc animated:YES];
}

- (IBAction)freshOnline:(UIBarButtonItem *)sender {
    //mesh isn't connected.
    if (!SigBearer.share.isOpen) {
        return;
    }

    BOOL hasKeyBindSuccess = NO;
    NSArray *curNodes = [NSArray arrayWithArray:SigDataSource.share.curNodes];
    for (SigNodeModel *model in curNodes) {
        if (model.isKeyBindSuccess && !model.isSensor && !model.isRemote) {
            hasKeyBindSuccess = YES;
            break;
        }
    }
    //there hasn't node that had keybound successful and that isn't sensor.
    if (!hasKeyBindSuccess) {
        [self delayReloadCollectionView];//需要刷新出直连节点的UI。
        return;
    }
    self.shouldSetAllOffline = YES;
    [self getOnlineStateWithResultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TelinkLogDebug(@"getOnlineStatus finish.");
    }];
}

- (void)getOnlineStateWithResultCallback:(resultBlock)resultCallback {
    __weak typeof(self) weakSelf = self;
    BOOL result = [DemoCommand getOnlineStatusWithResponseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigGenericOnOffStatus * _Nonnull responseMessage) {
        //界面刷新统一在SDK回调函数didReceiveMessage:中进行
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        [weakSelf showMeshOTAWarningAlertController];
        if (resultCallback) {
            resultCallback(isResponseAll, error);
        }
    }];
    if (result && self.shouldSetAllOffline) {
        self.shouldSetAllOffline = NO;
        [SigDataSource.share setAllDevicesOutline];
        [self delayReloadCollectionView];
    }
}

//刷新UI需要间隔0.1秒，防止100个设备时出现界面卡顿。
- (void)delayReloadCollectionView {
    if (!self.needDelayReloadData) {
        self.needDelayReloadData = YES;
        self.isDelaying = NO;
        self.source = [NSMutableArray arrayWithArray:SigDataSource.share.curNodes];
        dispatch_async(dispatch_get_main_queue(), ^{
            [self.collectionView reloadData];
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
        [self delayReloadCollectionView];
    }
}

#pragma mark - UICollectionViewDelegate
- (nonnull __kindof UICollectionViewCell *)collectionView:(nonnull UICollectionView *)collectionView cellForItemAtIndexPath:(nonnull NSIndexPath *)indexPath {
//    TelinkLogDebug(@"==========Home cellForItemAtIndexPath");
    HomeItemCell *item = [collectionView dequeueReusableCellWithReuseIdentifier:CellIdentifiers_HomeItemCellID forIndexPath:indexPath];
    SigNodeModel *model = self.source[indexPath.item];
    [item updateContent:model];
    return item;
}

- (NSInteger)collectionView:(nonnull UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    return self.source.count;
}

-(void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath{
    SigNodeModel *model = self.source[indexPath.item];

    if (model.isSensor || model.isRemote) {
        return;
    }

    if (model.state == DeviceStateOutOfLine || model.isKeyBindSuccess == NO) {
        [ShowTipsHandle.share show:Tip_DeviceOutline];
        [ShowTipsHandle.share delayHidden:2.0];
        return;
    }

    [DemoCommand switchOnOffWithIsOn:!(model.state == DeviceStateOn) address:model.address responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigGenericOnOffStatus * _Nonnull responseMessage) {
        //界面刷新统一在SDK回调函数didReceiveMessage:中进行
    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {

    }];
}

- (void)scrollToBottom {
    NSInteger item = [self.collectionView numberOfItemsInSection:0] - 1;
    NSIndexPath *lastItemIndex = [NSIndexPath indexPathForItem:item inSection:0];
    [self.collectionView scrollToItemAtIndexPath:lastItemIndex atScrollPosition:UICollectionViewScrollPositionTop animated:NO];
}

- (void)workNormal {
    if ([SDKLibCommand isBLEInitFinish]) {
        __weak typeof(self) weakSelf = self;
        if (self.source.count > 0) {
            [SDKLibCommand startMeshConnectWithComplete:^(BOOL successful) {
                if (!successful) {
                    if (SigBearer.share.isAutoReconnect) {
                        [weakSelf workNormal];
                    }
                }
            }];
        }else{
            [SDKLibCommand stopMeshConnectWithComplete:^(BOOL successful) {
                if (successful) {
                    TelinkLogDebug(@"无设备，不需要连接。");
                } else {
                    [weakSelf workNormal];
                }
            }];
        }
    }
}

- (void)showMeshOTAWarningAlertController {
    //demo v4.1.0.0 修改meshOTA弹框逻辑。
    //key=kDistributorAddress存储上一次作为distributor的直连节点的address.
    //distributor为0或者本地节点则默认无meshOTA记录。
    __weak typeof(self) weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        UIViewController *vc = weakSelf.currentViewController;
        if ([vc isMemberOfClass:[weakSelf class]]) {
            NSNumber *addressNumber = [[NSUserDefaults standardUserDefaults] valueForKey:kDistributorAddress];
            if (addressNumber == nil || addressNumber.intValue == 0 || addressNumber.intValue == SigDataSource.share.curLocationNodeModel.address) {
                //无meshOTA记录
                //mesh已经连接
                //且获取online status完成
                //无需新增其它逻辑
            } else {
                //存在meshOTA记录
                //弹框处理
                static dispatch_once_t onceToken;
                dispatch_once(&onceToken, ^{
                    dispatch_async(dispatch_get_main_queue(), ^{
                        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Warning - MeshOTA is still running" message:@"MeshOTA distribution is still running, continue?\nclick GO to enter MeshOTA processing page\nclick STOP to stop distribution" preferredStyle:UIAlertControllerStyleAlert];
                        [alertController addAction:[UIAlertAction actionWithTitle:@"STOP" style:UIAlertActionStyleDestructive handler:^(UIAlertAction * _Nonnull action) {
                            [MeshOTAManager.share stopFirmwareUpdateWithCompleteHandle:^(BOOL isSuccess) {
                                [[NSUserDefaults standardUserDefaults] setValue:@(0) forKey:kDistributorAddress];
                                [[NSUserDefaults standardUserDefaults] removeObjectForKey:kDistributorPolicy];
                                [[NSUserDefaults standardUserDefaults] removeObjectForKey:kUpdateNodeAddresses];
                                [[NSUserDefaults standardUserDefaults] synchronize];
                                dispatch_async(dispatch_get_main_queue(), ^{
                                    [weakSelf showTips:@"Stop Mesh ota finish!"];
                                });
                            }];
                        }]];
                        [alertController addAction:[UIAlertAction actionWithTitle:@"GO" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
                            MeshOTAVC *vc = [[MeshOTAVC alloc] init];
                            vc.isContinue = YES;
                            [weakSelf.navigationController pushViewController:vc animated:YES];
                        }]];
                        [weakSelf presentViewController:alertController animated:YES completion:nil];
                    });
                });
            }
        }
    });
}

#pragma mark - Life method

//将tabBar.hidden移到viewDidAppear，解决下一界面的手势返回动作取消时导致界面下方出现白条的问题。
- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    self.tabBarController.tabBar.hidden = NO;
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    [self setTitle:@"Device" subTitle:SigDataSource.share.meshName];
//    self.tabBarController.tabBar.hidden = NO;
    UIBarButtonItem *item = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemAdd target:self action:@selector(addNewDevice)];
    self.navigationItem.rightBarButtonItem = item;
    //get status of node
    SigBearer.share.dataDelegate = self;
    SigMeshLib.share.delegateForDeveloper = self;
    if (SigBearer.share.isOpen) {
        if ([LibTools uint16From16String:SigDataSource.share.getCurrentConnectedNode.pid] == SigNodePID_Switch) {
            [SigDataSource.share setAllDevicesOutline];
            [self delayReloadCollectionView];
            __weak typeof(self) weakSelf = self;
            [ConnectTools.share stopConnectToolsWithComplete:^(BOOL successful) {
                [weakSelf workNormal];
            }];
        } else {
            [self delayReloadCollectionView];
            [self blockState];
            [self getOnlineStateWithResultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                TelinkLogDebug(@"getOnlineStatus finish.");
            }];
        }
    } else {
        [SigDataSource.share setAllDevicesOutline];
        [self delayReloadCollectionView];
        [self workNormal];
    }
}

- (void)viewWillDisappear:(BOOL)animated{
    [super viewWillDisappear:animated];
    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(workNormal) object:nil];
}

- (void)normalSetting{
    [super normalSetting];
    self.allONButton.backgroundColor = UIColor.telinkButtonBlue;
    self.allOFFButton.backgroundColor = UIColor.telinkButtonBlue;
    self.cmdButton.backgroundColor = UIColor.telinkButtonBlue;
    self.logButton.backgroundColor = UIColor.telinkButtonBlue;
    self.needDelayReloadData = NO;
    SigDataSource.share.delegate = self;
    UILongPressGestureRecognizer *gesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(cellDidPress:)];
    [self.view addGestureRecognizer:gesture];
    self.source = [NSMutableArray arrayWithArray:SigDataSource.share.curNodes];
    SigBluetooth.share.delegate = self;
}

//获取手机当前显示的ViewController
- (UIViewController*)currentViewController{
    UIViewController *vc = [UIApplication sharedApplication].keyWindow.rootViewController;
    while (1) {
        if ([vc isKindOfClass:[UITabBarController class]]) {
            vc = ((UITabBarController*)vc).selectedViewController;
        }
        if ([vc isKindOfClass:[UINavigationController class]]) {
            vc = ((UINavigationController*)vc).visibleViewController;
        }
        if (vc.presentedViewController) {
            vc = vc.presentedViewController;
        }else{
            break;
        }
    }
    return vc;
}

- (void)blockState{
    [super blockState];

    __weak typeof(self) weakSelf = self;
    //this block will callback when publish timer check offline.
    [SigPublishManager.share setDiscoverOutlineNodeCallback:^(NSNumber * _Nonnull unicastAddress) {
        [weakSelf delayReloadCollectionView];
    }];
    //Power off and power on electricity directly connected devices, returns response of opcode 0x4E82 packets.
    [SigPublishManager.share setDiscoverOnlineNodeCallback:^(NSNumber * _Nonnull unicastAddress) {
        [weakSelf delayReloadCollectionView];
    }];
    [SDKLibCommand setBluetoothCentralUpdateStateCallback:^(CBCentralManagerState state) {
        TelinkLogVerbose(@"setBluetoothCentralUpdateStateCallback state=%ld",(long)state);
        if (state == CBCentralManagerStatePoweredOn) {
            [weakSelf workNormal];
        } else {
            weakSelf.shouldSetAllOffline = NO;
            [SigDataSource.share setAllDevicesOutline];
            [weakSelf delayReloadCollectionView];
        }
    }];
    //设置SigBearer的block
    [SigBearer.share changePeripheral:SigBearer.share.getCurrentPeripheral result:nil];
}

#pragma  mark - SigBearerDataDelegate
- (void)bearerDidConnectedAndDiscoverServices:(SigBearer *)bearer {
//    TelinkLogInfo(@"bearer did Connected And Discover Services!");
}

- (void)bearerDidOpen:(SigBearer *)bearer {
    //非主页，重连mesh成功，是否需要获取设备的状态（v3.3.3.5版本发现meshOTA界面是需要获取状态的）(v3.3.3.6版本发现弹UIAlertController框提示@"cancel Mesh ota finish!"没有点击确定的情况下不会获取设备状态，此次再次修改)
    dispatch_async(dispatch_get_main_queue(), ^{
        UIViewController *vc = self.currentViewController;
        if ([vc isMemberOfClass:[self class]] || [vc isMemberOfClass:[MeshOTAVC class]] || [vc isMemberOfClass:[UIAlertController class]]) {
            [self freshOnline:nil];
        } else {
            TelinkLogInfo(@"needn`t get status.%@",vc);
        }
    });
}

- (void)bearer:(SigBearer *)bearer didCloseWithError:(NSError *)error {
    TelinkLogVerbose(@"");
    self.shouldSetAllOffline = NO;
    [SigDataSource.share setAllDevicesOutline];
    [self delayReloadCollectionView];
}

#pragma  mark - SigDataSourceDelegate
- (void)onSequenceNumberUpdate:(UInt32)sequenceNumber ivIndexUpdate:(UInt32)ivIndex {
    TelinkLogVerbose(@"本地存储数据需要更新sequenceNumber=0x%X,ivIndex=0x%X",sequenceNumber,ivIndex);
}

/**
 * @brief   Callback called when the mesh network JOSN data had been changed.
 * APP need update the json to cloud at this time!
 * @param   network new mesh network JSON data.
 * @note    When the SDK calls `saveLocationData`, it will also notify the APP through this callback method.
 */
- (void)onMeshNetworkUpdated:(SigDataSource *)network {
    [self addOrUpdateMeshDictionaryToMeshList:[network getDictionaryFromDataSource]];
}

/**
 * @brief   Callback called when the unicastRange of provisioner had been changed. APP need update the json to cloud at this time!
 * @param   unicastRange Randge model had been change.
 * @param   provisioner provisioner of unicastRange.
 * @note    The address of the last node may be out of range.
 */
//- (void)onUpdateAllocatedUnicastRange:(SigRangeModel *)unicastRange ofProvisioner:(SigProvisionerModel *)provisioner {
//    //注意：客户如果不想使用SDK分配的区间，可以从provisioner里面remove这个SDK创建的unicastRange。
//    TelinkLogVerbose(@"当前provisioner的地址区间已经被分配完毕，SDK自动给provisioner.uuid=%@分配新的地址区间%@~~~>%@",provisioner.UUID,unicastRange.lowAddress,unicastRange.highAddress);
//}

#pragma  mark LongPressGesture
- (void)cellDidPress:(UILongPressGestureRecognizer *)sender{
    if (sender.state == UIGestureRecognizerStateBegan) {
        NSIndexPath *indexPath = [self.collectionView indexPathForItemAtPoint:[sender locationInView:self.collectionView]];
        if (indexPath != nil) {
            SigNodeModel *model = self.source[indexPath.item];
            if (model.isKeyBindSuccess) {
                SingleDeviceViewController *vc = (SingleDeviceViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_SingleDeviceViewControllerID storyboard:@"DeviceSetting"];
                vc.model = model;
                [self.navigationController pushViewController:vc animated:YES];
            } else {
                ReKeyBindViewController *vc = (ReKeyBindViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_ReKeyBindViewControllerID];
                vc.model = model;
                [self.navigationController pushViewController:vc animated:YES];
            }
        }
    }
}

#pragma mark - SigMessageDelegate

/// A callback called whenever a Mesh Message has been received from the mesh network.
/// @param message The received message.
/// @param source The Unicast Address of the Element from which the message was sent.
/// @param destination The address to which the message was sent.
- (void)didReceiveMessage:(SigMeshMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination {
    if ([message isKindOfClass:[SigGenericOnOffStatus class]]
        || [message isKindOfClass:[SigTelinkOnlineStatusMessage class]]
        || [message isKindOfClass:[SigLightLightnessStatus class]]
        || [message isKindOfClass:[SigLightLightnessLastStatus class]]
        || [message isKindOfClass:[SigLightCTLStatus class]]
        || [message isKindOfClass:[SigLightHSLStatus class]]
        || [message isKindOfClass:[SigLightXyLStatus class]]
        || [message isKindOfClass:[SigLightLCLightOnOffStatus class]]) {
        [self delayReloadCollectionView];
    }
}

/// A callback called whenever a SigSecureNetworkBeacon Message has been received from the mesh network.
/// @param message The received message.
- (void)didReceiveSigSecureNetworkBeaconMessage:(SigSecureNetworkBeacon *)message {
    TelinkLogInfo(@"%@",message);
}

/// A callback called whenever a SigMeshPrivateBeacon Message has been received from the mesh network.
/// @param message The received message.
- (void)didReceiveSigMeshPrivateBeaconMessage:(SigMeshPrivateBeacon *)message {
    TelinkLogInfo(@"%@",message);
}

#pragma mark - SigBluetoothDelegate

/// 需要过滤的设备，返回YES则过滤，返回NO则不过滤。
//- (BOOL)needToBeFilteredNodeWithSigScanRspModel:(SigScanRspModel *)scanRspModel provisioned:(BOOL)provisioned peripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary<NSString *, id> *)advertisementData RSSI:(NSNumber *)RSSI {
//    NSString *mac = @"A4C138093CD7";
//    if ([scanRspModel.macAddress isEqualToString:mac]) {
//        TelinkLogInfo(@"已经过滤掉设备%@",mac);
//        return YES;
//    }
//    return NO;
//}

@end
