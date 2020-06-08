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
//  Created by 梁家誌 on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

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

@interface HomeViewController()<UICollectionViewDelegate,UICollectionViewDataSource,SigBearerDataDelegate>
@property (strong, nonatomic) NSMutableArray <SigNodeModel *>*source;
@property (weak, nonatomic) IBOutlet UICollectionView *collectionView;
@property (weak, nonatomic) IBOutlet UIBarButtonItem *AddBarButton;
@property (assign, nonatomic) BOOL shouldSetAllOffline;//APP will set all nodes offline when user click refresh button.

@end

@implementation HomeViewController

- (IBAction)allON:(UIButton *)sender {
    NSInteger count = SigDataSource.share.getOnlineDevicesNumber;
    __weak typeof(self) weakSelf = self;
    [DemoCommand switchOnOffWithIsOn:sender.tag == 100 address:kMeshAddress_allNodes responseMaxCount:(int)count ack:YES successCallback:^(UInt16 source, UInt16 destination, SigGenericOnOffStatus * _Nonnull responseMessage) {
        if (responseMessage) {
            [SigDataSource.share updateNodeStatusWithBaseMeshMessage:responseMessage source:source];
        }
        [weakSelf reloadCollectionView];
    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
        
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
    TeLogDebug(@"");
    
    int tem = 0;
    NSArray *curNodes = [NSArray arrayWithArray:SigDataSource.share.curNodes];
    for (SigNodeModel *node in curNodes) {
        if (!node.isSensor && node.isKeyBindSuccess) {
            tem++;
        }
    }
    __weak typeof(self) weakSelf = self;
    BOOL result = [DemoCommand getOnlineStatusWithResponseMaxCount:tem successCallback:^(UInt16 source, UInt16 destination, SigGenericOnOffStatus * _Nonnull responseMessage) {
        if (responseMessage) {
            TeLogInfo(@"getOnlineStatus source=0x%x",source);
            NSDictionary *dict = @{@"source":@(source),@"responseMessage":responseMessage};
            [weakSelf performSelectorInBackground:@selector(reloadCollectionViewWithItemResponseModel:) withObject:dict];
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
        TeLogDebug(@"getOnlineStatus finish.");
    }];
    if (result && self.shouldSetAllOffline) {
        self.shouldSetAllOffline = NO;
        [SigDataSource.share setAllDevicesOutline];
        [self reloadCollectionView];
    }
}

- (void)reloadCollectionView{
    @synchronized(self) {
        self.source = [NSMutableArray arrayWithArray:SigDataSource.share.curNodes];
    }
    [self.collectionView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
}

- (void)reloadCollectionViewWithItemResponseModel:(NSDictionary *)dict{
    UInt16 address = [dict[@"source"] intValue];
//    TeLogInfo(@"reload address=0x%x",address);

    SigNodeModel *model = [SigDataSource.share getNodeWithAddress:address];
    [model updateNodeStatusWithBaseMeshMessage:dict[@"responseMessage"] source:address];
    if (model) {
        @synchronized(self) {
            NSInteger index = [self.source indexOfObject:model];
            if (index < self.source.count) {
                [self.source replaceObjectAtIndex:index withObject:model];
//                NSIndexPath *path = [NSIndexPath indexPathForItem:index inSection:0];
                dispatch_async(dispatch_get_main_queue(), ^{
                    [self.collectionView reloadData];
//                    [self.collectionView reloadItemsAtIndexPaths:@[path]];
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
    [DemoCommand switchOnOffWithIsOn:!(model.state == DeviceStateOn) address:model.address responseMaxCount:1 ack:YES successCallback:^(UInt16 source, UInt16 destination, SigGenericOnOffStatus * _Nonnull responseMessage) {
        NSDictionary *dict = @{@"source":@(source),@"responseMessage":responseMessage};
        [weakSelf performSelectorInBackground:@selector(reloadCollectionViewWithItemResponseModel:) withObject:dict];
    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
        
    }];
}

- (void)scrollowToBottom{
    NSInteger item = [self.collectionView numberOfItemsInSection:0] - 1;
    NSIndexPath *lastItemIndex = [NSIndexPath indexPathForItem:item inSection:0];
    [self.collectionView scrollToItemAtIndexPath:lastItemIndex atScrollPosition:UICollectionViewScrollPositionTop animated:NO];
}

- (void)workNormal{
    if ([SDKLibCommand isBLEInitFinish]) {
        __weak typeof(self) weakSelf = self;
        if (self.source.count > 0) {
            [SigBearer.share startMeshConnectWithComplete:^(BOOL successful) {
                if (successful) {
                    //Demo can show Bluetooth.share.currentPeripheral in HomeViewController when CanControl callback.
                    [weakSelf.collectionView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
                    //Attention: some block has change when demo call startWorkNormal API, so when SDK callback CanControl, reset block by call blockState.
                    [weakSelf blockState];
                    [weakSelf freshOnline:nil];
                } else {
                    if (SigBearer.share.isAutoReconnect) {
                        [weakSelf workNormal];
                    }
                }
            }];
        }else{
            [SigBearer.share stopMeshConnectWithComplete:^(BOOL successful) {
                if (successful) {
                    TeLogDebug(@"无设备，不需要连接。");
                } else {
                    [weakSelf workNormal];
                }
            }];
        }
    }
}

#pragma mark - Life method
- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = NO;
    //update number of node
    [self reloadCollectionView];
    //get status of node
    SigBearer.share.dataDelegate = self;
    if (SigBearer.share.isOpen) {
        [self blockState];
        [self getOnlineState];
    } else {
        [self workNormal];
    }
//    __weak typeof(self) weakSelf = self;
//    [SDKLibCommand setBluetoothCentralUpdateStateCallback:^(CBCentralManagerState state) {
//        TeLogVerbose(@"setBluetoothCentralUpdateStateCallback state=%ld",(long)state);
//        if (state == CBCentralManagerStatePoweredOn) {
//            [weakSelf workNormal];
//        } else {
//            weakSelf.shouldSetAllOffline = NO;
//            [SigDataSource.share setAllDevicesOutline];
//            [weakSelf reloadCollectionView];
//        }
//    }];
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

- (void)blockState{
    [super blockState];
    
    __weak typeof(self) weakSelf = self;
    //this block will callback when publish timer check offline.
    [SigPublishManager.share setDiscoverOutlineNodeCallback:^(NSNumber * _Nonnull unicastAddress) {
        [weakSelf reloadCollectionView];
    }];
    //Power off and power on electricity directly connected devices, returns response of opcode 0x4E82 packets.
    [SigPublishManager.share setDiscoverOnlineNodeCallback:^(NSNumber * _Nonnull unicastAddress) {
        [weakSelf reloadCollectionView];
    }];
    [SDKLibCommand setBluetoothCentralUpdateStateCallback:^(CBCentralManagerState state) {
        TeLogVerbose(@"setBluetoothCentralUpdateStateCallback state=%ld",(long)state);
        if (state == CBCentralManagerStatePoweredOn) {
            [weakSelf workNormal];
        } else {
            weakSelf.shouldSetAllOffline = NO;
            [SigDataSource.share setAllDevicesOutline];
            [weakSelf reloadCollectionView];
        }
    }];
    //设置SigBearer的block
    [SigBearer.share changePeripheral:SigBearer.share.getCurrentPeripheral result:nil];
}

#pragma  mark - SigBearerDataDelegate
- (void)bearerDidOpen:(SigBearer *)bearer {
    TeLogInfo(@"SigBearerDataDelegate1");
//    [self blockState];
//    dispatch_async(dispatch_get_main_queue(), ^{
//        [self freshOnline:nil];
//    });
}

- (void)bearer:(SigBearer *)bearer didCloseWithError:(NSError *)error {
    TeLogInfo(@"SigBearerDataDelegate2");
    self.shouldSetAllOffline = NO;
    [SigDataSource.share setAllDevicesOutline];
    [self reloadCollectionView];
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
