/********************************************************************************************************
 * @file     HomeViewController.m 
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

#import "HomeViewController.h"
#import "LogViewController.h"
#import "CMDViewController.h"
#import "ReKeyBindViewController.h"
#import "HomeItemCell.h"
#import "SingleDeviceViewController.h"
#import "AddDeviceViewController.h"
#import "UIViewController+Message.h"
#import "SensorVC.h"
#import "RemoteAddVC.h"
#import "MeshOTAVC.h"

@interface HomeViewController()<UICollectionViewDelegate,UICollectionViewDataSource,SigBearerDataDelegate,SigDataSourceDelegate,SigMessageDelegate,SigBluetoothDelegate>
@property (strong, nonatomic) NSMutableArray <SigNodeModel *>*source;
@property (weak, nonatomic) IBOutlet UICollectionView *collectionView;
@property (weak, nonatomic) IBOutlet UIBarButtonItem *AddBarButton;
@property (assign, nonatomic) BOOL shouldSetAllOffline;//APP will set all nodes offline when user click refresh button.
@property (assign, nonatomic) BOOL needDelayReloadData;
@property (assign, nonatomic) BOOL isDelaying;
@property (assign, nonatomic) UInt16 cancelDistributorAddress;

@end

@implementation HomeViewController

- (IBAction)allON:(UIButton *)sender {
    int tem = 0;
    NSArray *curNodes = [NSArray arrayWithArray:SigDataSource.share.curNodes];
    for (SigNodeModel *node in curNodes) {
        if (!node.isSensor && !node.isRemote && node.isKeyBindSuccess) {
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
- (IBAction)addNewDevice:(UIBarButtonItem *)sender {
    //v3.3.3.6及之后版本，初次分享过来，没有ivIndex时，需要连接mesh成功或者用户手动输入ivIndex。
    if (!SigDataSource.share.existLocationIvIndexAndLocationSequenceNumber) {
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
    } else {
        [self pushToAddDeviceVC];
    }
}

- (void)pushToAddDeviceVC {
    BOOL isRemoteAdd = [[[NSUserDefaults standardUserDefaults] valueForKey:kRemoteAddType] boolValue];
    [SDKLibCommand setBluetoothCentralUpdateStateCallback:nil];
    if (isRemoteAdd) {
        TeLogVerbose(@"click remote add device");
        
        RemoteAddVC *vc = (RemoteAddVC *)[UIStoryboard initVC:ViewControllerIdentifiers_RemoteAddVCID];
        [SigDataSource.share setAllDevicesOutline];
        [self.navigationController pushViewController:vc animated:YES];
    } else {
        BOOL isFastAdd = [[[NSUserDefaults standardUserDefaults] valueForKey:kFastAddType] boolValue];
        if (isFastAdd) {
            UIViewController *vc = [UIStoryboard initVC:ViewControllerIdentifiers_FastProvisionAddViewControllerID];
            [SigDataSource.share setAllDevicesOutline];
            [self.navigationController pushViewController:vc animated:YES];
        } else {
            TeLogVerbose(@"click normal add device");
            
            //自动添加多个设备
            AddDeviceViewController *vc = (AddDeviceViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_AddDeviceViewControllerID];
            [SigDataSource.share setAllDevicesOutline];
            [self.navigationController pushViewController:vc animated:YES];
        }
    }
}

#pragma mark see log entrance
- (IBAction)clickToLogVC:(UIButton *)sender {
    LogViewController *vc = (LogViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_LogViewControllerID];
    [self.navigationController pushViewController:vc animated:YES];
}

- (IBAction)clickToCMDVC:(UIButton *)sender {
    CMDViewController *vc = (CMDViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_CMDViewControllerID];
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
        TeLogDebug(@"getOnlineStatus finish.");
    }];
}

- (void)getOnlineStateWithResultCallback:(resultBlock)resultCallback {
//    TeLogDebug(@"");
    
    int tem = 0;
    NSArray *curNodes = [NSArray arrayWithArray:SigDataSource.share.curNodes];
    for (SigNodeModel *node in curNodes) {
        if (!node.isSensor && !node.isRemote && node.isKeyBindSuccess) {
            tem++;
        }
    }
    BOOL result = [DemoCommand getOnlineStatusWithResponseMaxCount:tem successCallback:^(UInt16 source, UInt16 destination, SigGenericOnOffStatus * _Nonnull responseMessage) {
        //界面刷新统一在SDK回调函数didReceiveMessage:中进行
    } resultCallback:resultCallback];
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
//    TeLogDebug(@"==========Home cellForItemAtIndexPath");
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
                if (successful) {
                    //                    [weakSelf getOnOffAction];
                } else {
                    if (SigBearer.share.isAutoReconnect) {
                        [weakSelf workNormal];
                    }
                }
            }];
        }else{
            [SDKLibCommand stopMeshConnectWithComplete:^(BOOL successful) {
                if (successful) {
                    TeLogDebug(@"无设备，不需要连接。");
                } else {
                    [weakSelf workNormal];
                }
            }];
        }
    }
}

- (void)showMeshOTAHits {
    //demo v3.3.3新增meshOTA兼容，存储上一次作为distributor的直连节点的address，为0或者本地节点则默认无meshOTA记录。
    NSNumber *addressNumber = [[NSUserDefaults standardUserDefaults] valueForKey:kDistributorAddress];
    __weak typeof(self) weakSelf = self;
    if (addressNumber == nil || addressNumber.intValue == 0 || addressNumber.intValue == SigDataSource.share.curLocationNodeModel.address) {
        //使用startMeshConnectWithComplete连接附近节点即可
        [self workNormal];
    } else {
        //需要兼容meshOTA，直连distributor
        static dispatch_once_t onceToken;
        dispatch_once(&onceToken, ^{
            dispatch_async(dispatch_get_main_queue(), ^{
                UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Hits" message:@"发现APP存在上一次的MeshOTA记录，是否直连上一次的distributor设备继续MeshOTA？或者取消上一次的MeshOTA记录？" preferredStyle:UIAlertControllerStyleAlert];
                [alertController addAction:[UIAlertAction actionWithTitle:@"继续" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
                    TeLogDebug(@"点击继续");
                    SigNodeModel *node = [SigDataSource.share getNodeWithAddress:addressNumber.intValue];
                    if (node) {
                        NSString *t = [NSString stringWithFormat:@"mesh ota... connecting distributor"];
                        [ShowTipsHandle.share show:t];
                        //无限扫描连接distributor，直到连接成功。（可能会修改）
                        [ConnectTools.share startConnectToolsWithNodeList:@[node] timeout:0xFFFFFFFF Complete:^(BOOL successful) {
                            if (successful) {
                                //查询完状态再弹框，防止MeshOTA界面firmwareUpdateInformationGet按钮计算responseMax异常。
                                [weakSelf getOnlineStateWithResultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
                                    TeLogDebug(@"getOnlineStatus finish.");
                                    //meshOTA逻辑里面的查询进度，无需在此添加查询进度代码。
                                    [weakSelf continueMeshOTA];
                                }];
                            } else {
                                //连接失败
                                NSString *t = [NSString stringWithFormat:@"mesh ota... connect distributor fail!"];
                                [ShowTipsHandle.share show:t];
                                [ShowTipsHandle.share delayHidden:1.0];
                                [weakSelf workNormal];
                            }
                        }];
                    } else {
                        NSString *t = [NSString stringWithFormat:@"mesh ota... connect distributor fail!"];
                        [ShowTipsHandle.share show:t];
                        [ShowTipsHandle.share delayHidden:1.0];
                        weakSelf.cancelDistributorAddress = addressNumber.intValue;
                        #ifdef kExist
                        if (kExistMeshOTA) {
                            MeshOTAManager.share.distributorAddress = addressNumber.intValue;
                        }
                        #endif
                        [weakSelf workNormal];
                    }
                }]];
                [alertController addAction:[UIAlertAction actionWithTitle:@"取消" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
                    TeLogDebug(@"点击取消");
                    weakSelf.cancelDistributorAddress = addressNumber.intValue;
                    #ifdef kExist
                    if (kExistMeshOTA) {
                        MeshOTAManager.share.distributorAddress = addressNumber.intValue;
                    }
                    #endif
                    [weakSelf workNormal];
                }]];
                [weakSelf presentViewController:alertController animated:YES completion:nil];
            });
        });
    }
}

- (void)continueMeshOTA {
    __weak typeof(self) weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        [ShowTipsHandle.share delayHidden:0.0];
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Hits" message:@"mesh ota... connect distributor success! APP will push to MeshOTA UI." preferredStyle:UIAlertControllerStyleAlert];
        [alertController addAction:[UIAlertAction actionWithTitle:@"Sure" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            TeLogDebug(@"点击确认");
            MeshOTAVC *vc = [[MeshOTAVC alloc] init];
            vc.isContinue = YES;
            [weakSelf.navigationController pushViewController:vc animated:YES];
        }]];
        [weakSelf presentViewController:alertController animated:YES completion:nil];
    });
}

- (void)showTips:(NSString *)tips{
    dispatch_async(dispatch_get_main_queue(), ^{
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Hits" message:tips preferredStyle:UIAlertControllerStyleAlert];
        [alertController addAction:[UIAlertAction actionWithTitle:@"Sure" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            TeLogDebug(@"点击确认");
        }]];
        [self presentViewController:alertController animated:YES completion:nil];
    });
}

- (void)getOnOffAction {
    //Demo can show Bluetooth.share.currentPeripheral in HomeViewController when CanControl callback.
    [self.collectionView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
    //Attention: some block has change when demo call startWorkNormal API, so when SDK callback CanControl, reset block by call blockState.
    [self blockState];
    [self freshOnline:nil];
}

#pragma mark - Life method
- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = NO;
    //get status of node
    SigBearer.share.dataDelegate = self;
    SigMeshLib.share.delegateForDeveloper = self;
    //demo v3.3.3新增meshOTA兼容，存储上一次作为distributor的直连节点的address，为0或者本地节点则默认无meshOTA记录。
    NSNumber *addressNumber = [[NSUserDefaults standardUserDefaults] valueForKey:kDistributorAddress];
    if (addressNumber == nil || addressNumber.intValue == 0 || addressNumber.intValue == SigDataSource.share.curLocationNodeModel.address) {
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
                    TeLogDebug(@"getOnlineStatus finish.");
                }];
            }
        } else {
            [SigDataSource.share setAllDevicesOutline];
            [self delayReloadCollectionView];
            [self workNormal];
        }
    }
    TeLogVerbose(@"当前provisioner的本地地址是0x%X,uuid=%@",SigDataSource.share.curLocationNodeModel.address,SigDataSource.share.getCurrentProvisionerUUID);
}

- (void)viewWillDisappear:(BOOL)animated{
    [super viewWillDisappear:animated];
    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(workNormal) object:nil];
}

- (void)normalSetting{
    [super normalSetting];
    self.needDelayReloadData = NO;
    SigDataSource.share.delegate = self;
    self.title = @"Device";
    UILongPressGestureRecognizer *gesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(cellDidPress:)];
    [self.view addGestureRecognizer:gesture];
    self.source = [NSMutableArray arrayWithArray:SigDataSource.share.curNodes];
    SigBluetooth.share.delegate = self;
    [self showMeshOTAHits];
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
        TeLogVerbose(@"setBluetoothCentralUpdateStateCallback state=%ld",(long)state);
        if (state == CBCentralManagerStatePoweredOn) {
            //demo v3.3.3新增meshOTA兼容，存储上一次作为distributor的直连节点的address，为0或者本地节点则默认无meshOTA记录。
            NSNumber *addressNumber = [[NSUserDefaults standardUserDefaults] valueForKey:kDistributorAddress];
            if (addressNumber == nil || addressNumber.intValue == 0 || addressNumber.intValue == SigDataSource.share.curLocationNodeModel.address) {
                [weakSelf workNormal];
            }
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
//    TeLogInfo(@"bearer did Connected And Discover Services!");
//    [self blockState];
//    dispatch_async(dispatch_get_main_queue(), ^{
//        [self freshOnline:nil];
//    });
}

- (void)bearerDidOpen:(SigBearer *)bearer {
//    TeLogInfo(@"bearer did open!");
//    [self blockState];
    if (self.cancelDistributorAddress) {
        #ifdef kExist
        if (kExistMeshOTA) {
            __weak typeof(self) weakSelf = self;
            [MeshOTAManager.share stopFirmwareUpdateWithCompleteHandle:^(BOOL isSuccess) {
                weakSelf.cancelDistributorAddress = 0;
                [[NSUserDefaults standardUserDefaults] setValue:@(0) forKey:kDistributorAddress];
                [[NSUserDefaults standardUserDefaults] removeObjectForKey:kDistributorPolicy];
                [[NSUserDefaults standardUserDefaults] removeObjectForKey:kUpdateNodeAddresses];
                [[NSUserDefaults standardUserDefaults] synchronize];
                dispatch_async(dispatch_get_main_queue(), ^{
                    NSString *tip = [NSString stringWithFormat:@"cancel Mesh ota finish!"];
                    [weakSelf showTips:tip];
                    [weakSelf freshOnline:nil];
                });
            }];
        }
        #endif
    } else {
        //非主页，重连mesh成功，是否需要获取设备的状态（v3.3.3.5版本发现meshOTA界面是需要获取状态的）(v3.3.3.6版本发现弹UIAlertController框提示@"cancel Mesh ota finish!"没有点击确定的情况下不会获取设备状态，此次再次修改)
        dispatch_async(dispatch_get_main_queue(), ^{
            UIViewController *vc = self.currentViewController;
            if ([vc isMemberOfClass:[self class]] || [vc isMemberOfClass:[MeshOTAVC class]] || [vc isMemberOfClass:[UIAlertController class]]) {
                [self freshOnline:nil];
            } else {
                TeLogInfo(@"needn`t get status.%@",vc);
            }
        });
    }
}

- (void)bearer:(SigBearer *)bearer didCloseWithError:(NSError *)error {
    TeLogVerbose(@"");
    self.shouldSetAllOffline = NO;
    [SigDataSource.share setAllDevicesOutline];
    [self delayReloadCollectionView];
}

#pragma  mark - SigDataSourceDelegate
- (void)onSequenceNumberUpdate:(UInt32)sequenceNumber ivIndexUpdate:(UInt32)ivIndex {
    TeLogVerbose(@"本地存储数据需要更新sequenceNumber=0x%X,ivIndex=0x%X",sequenceNumber,ivIndex);
}

///// Callback called when the unicastRange of provisioner had been changed. APP need update the json to cloud at this time!（如果APP实现了该代理方法，SDK会在当前provisioner地址还剩余10个或者更少的时候给provisioner分配一段新的地址区间。如果APP未实现该方法，SDK在但区间耗尽时超界分配地址(即- (UInt16)provisionAddress方法会返回非本区间的地址)。）
///// @param unicastRange Randge model had beed change.
///// @param provisioner provisioner of unicastRange.
//- (void)onUpdateAllocatedUnicastRange:(SigRangeModel *)unicastRange ofProvisioner:(SigProvisionerModel *)provisioner {
//    //注意：客户如果不想使用SDK分配的区间，可以从provisioner里面remove这个SDK创建的unicastRange。
//    TeLogVerbose(@"当前provisioner的地址区间已经被分配完毕，SDK自动给provisioner.uuid=%@分配新的地址区间%@~~~>%@",provisioner.UUID,unicastRange.lowAddress,unicastRange.highAddress);
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

- (void)didReceiveSigSecureNetworkBeaconMessage:(SigSecureNetworkBeacon *)message {
    TeLogInfo(@"%@",message);
}

- (void)didReceiveSigMeshPrivateBeaconMessage:(SigMeshPrivateBeacon *)message {
    TeLogInfo(@"%@",message);
}

#pragma mark - SigBluetoothDelegate

/// 需要过滤的设备，返回YES则过滤，返回NO则不过滤。
//- (BOOL)needToBeFilteredNodeWithSigScanRspModel:(SigScanRspModel *)scanRspModel provisioned:(BOOL)provisioned peripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary<NSString *, id> *)advertisementData RSSI:(NSNumber *)RSSI {
//    NSString *mac = @"A4C138093CD7";
//    if ([scanRspModel.macAddress isEqualToString:mac]) {
//        TeLogInfo(@"已经过滤掉设备%@",mac);
//        return YES;
//    }
//    return NO;
//}

@end
