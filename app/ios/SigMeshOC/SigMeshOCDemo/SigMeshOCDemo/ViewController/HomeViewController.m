/********************************************************************************************************
 * @file     HomeViewController.m 
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
//  HomeViewController.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "HomeViewController.h"
#import "LogViewController.h"
#import "CMDViewController.h"
#import "ReKeyBindViewController.h"
#import "HomeItemCell.h"
#import "SingleDeviceViewController.h"
#import "AddDeviceViewController.h"
//#import "SigDataSource.h"
#import "UIViewController+Message.h"
#import "SensorVC.h"
#import "RemoteAddVC.h"

@interface HomeViewController()<UICollectionViewDelegate,UICollectionViewDataSource>
@property (strong, nonatomic) NSMutableArray <SigNodeModel *>*source;
//@property (assign, nonatomic) BOOL shouldSetTime;//一个APP运行周期内只执行一次（客户APP可根据需求进行修改。如新增设备、所有设备都离线后重新连接上线等特殊情况再执行一次设置时间的操作）
@property (weak, nonatomic) IBOutlet UICollectionView *collectionView;
@property (weak, nonatomic) IBOutlet UIBarButtonItem *AddBarButton;
@property (assign, nonatomic) BOOL shouldSetAllOffline;//APP will set all nodes offline when user click refresh button.

@end

@implementation HomeViewController

- (IBAction)allON:(UIButton *)sender {
    NSInteger count = SigDataSource.share.getOnlineDevicesNumber;
    __weak typeof(self) weakSelf = self;
    [DemoCommand switchOnOff:YES on:sender.tag == 100 address:kAllDo_SIGParameters resMax:(int)count complete:^(ResponseModel *m) {
        if (m) {
            [SigDataSource.share updateResponseModelWithResponse:m];
        }
        [weakSelf reloadCollectionView];
    }];
}

#pragma mark add node entrance
- (IBAction)addNewDevice:(UIBarButtonItem *)sender {
    BOOL isRemoteAdd = [[[NSUserDefaults standardUserDefaults] valueForKey:kRemoteAddType] boolValue];
    if (isRemoteAdd) {
        TeLog(@"click remote add device");
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
            TeLog(@"click normal add device");
            
            //自动添加多个设备
            AddDeviceViewController *vc = (AddDeviceViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_AddDeviceViewControllerID];
            [SigDataSource.share setAllDevicesOutline];
            [self.navigationController pushViewController:vc animated:YES];
            
            //选择添加设备
        //        UIViewController *vc = [UIStoryboard initVC:@"ChooseAndAddDeviceVC"];
        //        [SigDataSource.share setAllDevicesOutline];
        //        [self.navigationController pushViewController:vc animated:YES];
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
    if (!Bluetooth.share.isConnected) {
        return;
    }
    
    BOOL hasKeyBindSuccess = NO;
    for (SigNodeModel *model in SigDataSource.share.curNodes) {
        if (model.isKeyBindSuccess && !model.isSensor) {
            hasKeyBindSuccess = YES;
            break;
        }
    }
    //there hasn't node that had keybound successful and that isn't sensor.
    if (!hasKeyBindSuccess) {
        return;
    }
    self.shouldSetAllOffline = YES;

    [self getOnlineState];
}

- (void)getOnlineState{
    TeLog(@"");
    
    int tem = 0;
    for (SigNodeModel *node in SigDataSource.share.curNodes) {
        if (!node.isSensor && node.isKeyBindSuccess) {
            tem++;
        }
    }
    __weak typeof(self) weakSelf = self;
    BOOL result = [DemoCommand getOnlineStatus:YES reqCount:tem complete:^(ResponseModel *m) {
//        TeLog(@"get onlinestatus response");
        
        //change since v3.1.0，SDK will call statusNowTime automatic, demo needn't call setNowTimeWithComplete.

//        if (m.opcode == 0x62) {
//            //Attention: this get online status privately when app get response opcode 0x62, call isBusy to trigger setTime in HomeViewController.
//            if (weakSelf.shouldSetTime) {
//                [Bluetooth.share.commandHandle isBusy];
//            }
//        }
        [weakSelf performSelectorInBackground:@selector(reloadCollectionViewWithItemResponseModel:) withObject:m];
    }];
    if (result && self.shouldSetAllOffline) {
        self.shouldSetAllOffline = NO;
        [SigDataSource.share setAllDevicesOutline];
        [self reloadCollectionView];
    }
    
    //change since v3.1.0，SDK will call statusNowTime automatic, demo needn't call setNowTimeWithComplete.
//    //Attention: this get online status publish, SDK will callback isBusy when all nodes status back. then demo will setTime on isBusy.
//    static dispatch_once_t onceToken;
//    dispatch_once(&onceToken, ^{
//        self.shouldSetTime = YES;
//    });
}

- (void)reloadCollectionView{
    @synchronized(self) {
        self.source = [NSMutableArray arrayWithArray:SigDataSource.share.curNodes];
    }
    [self.collectionView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
}

- (void)reloadCollectionViewWithItemResponseModel:(ResponseModel *)m{
    UInt16 address = m.address;
    TeLog(@"refresh UI in HomeVC, address=0x%x",address);
    SigNodeModel *model = [SigDataSource.share getNodeWithAddress:address];
    if (model) {
        @synchronized(self) {
            NSInteger index = [self.source indexOfObject:model];
            if (index < self.source.count) {
                [self.source replaceObjectAtIndex:index withObject:model];
                NSIndexPath *path = [NSIndexPath indexPathForItem:index inSection:0];
                dispatch_async(dispatch_get_main_queue(), ^{
                    [self.collectionView reloadItemsAtIndexPaths:@[path]];
                });
            }
        }
    }
}

#pragma mark - UICollectionViewDelegate
- (nonnull __kindof UICollectionViewCell *)collectionView:(nonnull UICollectionView *)collectionView cellForItemAtIndexPath:(nonnull NSIndexPath *)indexPath {
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

    if (model.isSensor) {
        return;
    }

    if (model.state == DeviceStateOutOfLine || model.isKeyBindSuccess == NO) {
        [ShowTipsHandle.share show:Tip_DeviceOutline];
        [ShowTipsHandle.share delayHidden:2.0];
        return;
    }
    
    __weak typeof(self) weakSelf = self;
    [DemoCommand switchOnOff:YES on:!(model.state == DeviceStateOn) address:model.address resMax:1 complete:^(ResponseModel *m) {
        if (m) {
            [SigDataSource.share updateResponseModelWithResponse:m];
        }
        [weakSelf reloadCollectionView];
    }];
}

- (void)scrollowToBottom{
    NSInteger item = [self.collectionView numberOfItemsInSection:0] - 1;
    NSIndexPath *lastItemIndex = [NSIndexPath indexPathForItem:item inSection:0];
    [self.collectionView scrollToItemAtIndexPath:lastItemIndex atScrollPosition:UICollectionViewScrollPositionTop animated:NO];
}

- (void)workNormal{
    if (self.source.count > 0) {
        
        //old code:use in v2.9.0 and before
//        NSMutableArray *availablelist = [NSMutableArray array];
//        for (SigNodeModel *model in SigDataSource.share.curNodes) {
//            //Node will hasn't peripheralUUID when demo get json data from share first, and sensor node can't connect by app.
//            if (model.peripheralUUID && !model.isSensor) {
//                [availablelist addObject:model.peripheralUUID];
//            }
//        }
//        if (availablelist.count > 0) {
//            __weak typeof(self) weakSelf = self;
//            [self.ble.commandHandle startWorkNormal:availablelist CanControl:^(NSString *uuidString) {
//                //Demo can show Bluetooth.share.currentPeripheral in HomeViewController when CanControl callback.
//                [weakSelf.collectionView reloadData];
//                //Attention: some block has change when demo call startWorkNormal API, so when SDK callback CanControl, reset block by call blockState.
//                [weakSelf blockState];
//                [weakSelf freshOnline:nil];
//            }];
//        } else {
//            //Attention: there are not peripheralUUID first when get share mesh.json, so call startScan for 2 second for scan peripheralUUID;
//            [self.ble startScan];
//            dispatch_async(dispatch_get_main_queue(), ^{
//                [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(workNormal) object:nil];
//                [self performSelector:@selector(workNormal) withObject:nil afterDelay:2.0];
//            });
//        }
        
        //new code:use in v3.0.0 and later
        __weak typeof(self) weakSelf = self;
        [self.ble.commandHandle startWorkNormalWithComplete:^(NSString *uuidString) {
            TeLog(@"connnect mesh success.");
            [weakSelf workNormalWithUuidString:uuidString];
        }];
    }else{
        [Bluetooth.share stopAutoConnect];
        [Bluetooth.share cancelAllConnecttionWithComplete:nil];
    }
}

- (void)workNormalWithUuidString:(NSString *)uuidString {
    //Demo can show Bluetooth.share.currentPeripheral in HomeViewController when CanControl callback.
    [self.collectionView reloadData];
    //Attention: some block has change when demo call startWorkNormal API, so when SDK callback CanControl, reset block by call blockState.
    [self blockState];
    [self freshOnline:nil];
}

#pragma mark - Life method
- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = NO;
    //update number of node
    [self reloadCollectionView];
    //get status of node
    [self.ble setNormalState];
    if (self.ble.isConnected) {
        [self getOnlineState];
    } else {
        [self workNormal];
    }
}

- (void)viewWillDisappear:(BOOL)animated{
    [super viewWillDisappear:animated];
    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(workNormal) object:nil];
}

- (void)normalSetting{
    [super normalSetting];
//    self.shouldSetTime = NO;
    self.title = @"Device";
    UILongPressGestureRecognizer *gesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(cellDidPress:)];
    [self.view addGestureRecognizer:gesture];
    [self reloadCollectionView];
}

- (void)isBusy:(NSNotification *)notify{
    [super isBusy:notify];
    NSDictionary *dict = notify.userInfo;
    BOOL isBusy = [dict[CommandIsBusyKey] boolValue];
    if (isBusy) {
        TeLog(@"is Busy");
    } else {
        TeLog(@"is not Busy");
        
        //change since v3.1.0，SDK will call statusNowTime automatic, demo needn't call setNowTimeWithComplete.
        
//        //set time once when demo first get online status complete
//        if (self.shouldSetTime) {
//            __weak typeof(self) weakSelf = self;
//            TeLog(@"call set time api.");
//            [DemoCommand setNowTimeWithComplete:^{
//                TeLog(@"set time response.");
//                weakSelf.shouldSetTime = NO;
//            }];
//            //Default bluetooth hardware device which has the function of the time, alarm clock, the scene is not opened, so set a time function is not waiting for the response back.
//            self.shouldSetTime = NO;
//        }
    }
}

- (void)blockState{
    [super blockState];
    
    __weak typeof(self) weakSelf = self;
    self.ble.bleFinishScanedCharachteristicCallBack = ^(CBPeripheral *peripheral) {
        if (weakSelf.ble.state == StateNormal) {
            [weakSelf freshOnline:nil];
        }
    };
    self.ble.bleCentralUpdateStateCallBack = ^(CBCentralManagerState state) {
        if (weakSelf.ble.state == StateNormal) {
            switch (state) {
                case CBCentralManagerStatePoweredOn:
                    [weakSelf workNormal];
                    break;
                case CBCentralManagerStatePoweredOff:
                    [SigDataSource.share setAllDevicesOutline];
                    [weakSelf reloadCollectionView];
                    break;
                default:
                    break;
            }
        }
    };
    
    //Power off and power on electricity directly connected devices, returns response of opcode 0x4E82 packets.
    [self.ble.commandHandle setNotifyOnlineStatusCallBack:^(ResponseModel *m) {
        [weakSelf reloadCollectionView];
    }];
    //node notify publish status data when publish sig modelid is 0x1303.(if developer use other modelid to set publish, developer need fix the opcode.)
    [self.ble.commandHandle setNotifyPublishStatusCallBack:^(ResponseModel *m) {
        [weakSelf reloadCollectionView];
    }];
    //this block will callback when publish timer check offline.
    [self.ble.commandHandle setCheckOfflineCallBack:^(NSNumber *address) {
        [weakSelf reloadCollectionView];
    }];
    [self.ble setBleDisconnectOrConnectFailCallBack:^(CBPeripheral *peripheral) {
        [SigDataSource.share setAllDevicesOutline];
        [weakSelf reloadCollectionView];
    }];
    //fast provision or remote provision, this blcok maybe nil.
    if (self.ble.workWithPeripheralCallBack == nil) {
        [self.ble setWorkWithPeripheralCallBack:^(NSString *uuidString) {
            [weakSelf workNormalWithUuidString:uuidString];
        }];
    }
}

- (void)nilBlock{
    [super nilBlock];
    self.ble.bleDisconnectOrConnectFailCallBack = nil;
    self.ble.bleFinishScanedCharachteristicCallBack = nil;
}

#pragma  mark LongPressGesture
- (void)cellDidPress:(UILongPressGestureRecognizer *)sender{
    if (sender.state == UIGestureRecognizerStateBegan) {
        NSIndexPath *indexPath = [self.collectionView indexPathForItemAtPoint:[sender locationInView:self.collectionView]];
        if (indexPath != nil) {
            SigNodeModel *model = self.source[indexPath.item];
            if (model.isKeyBindSuccess) {
//                if (model.isSensor) {
//                    SensorVC *vc = (SensorVC *)[UIStoryboard initVC:ViewControllerIdentifiers_SensorVCID storybroad:@"Main"];
//                    vc.model = model;
//                    [self.navigationController pushViewController:vc animated:YES];
//                } else {
                    SingleDeviceViewController *vc = (SingleDeviceViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_SingleDeviceViewControllerID storybroad:@"DeviceSetting"];
                    vc.model = model;
                    [self.navigationController pushViewController:vc animated:YES];
//                }
            } else {
                ReKeyBindViewController *vc = (ReKeyBindViewController *)[UIStoryboard initVC:ViewControllerIdentifiers_ReKeyBindViewControllerID];
                vc.model = model;
                [self.navigationController pushViewController:vc animated:YES];
            }
        }
    }
}

@end
