/********************************************************************************************************
 * @file     ReKeyBindViewController.m 
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
//  ReKeyBindViewController.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "ReKeyBindViewController.h"

@interface ReKeyBindViewController()
@property (nonatomic, assign) BOOL hasClickKickout;
@property (nonatomic, assign) BOOL hasClickKeyBind;
@property (weak, nonatomic) IBOutlet UILabel *detailLabel;
@property (weak, nonatomic) IBOutlet UIButton *kickOutButton;
@property (weak, nonatomic) IBOutlet UIButton *keindyButton;
@end

@implementation ReKeyBindViewController

- (IBAction)kickOut:(UIButton *)sender {
    TeLog(@"");
    self.hasClickKickout = YES;
    [ShowTipsHandle.share show:Tip_KickoutDevice];
    
    [SigDataSource.share removeModelWithDeviceAddress:self.model.address];
    
    if (self.ble.isConnected) {
        [self kickoutAction];
    } else {
        [self pop];
    }
}

- (IBAction)keyBind:(UIButton *)sender {
    TeLog(@"");
    self.hasClickKeyBind = YES;
    [self blockState];
    [ShowTipsHandle.share show:Tip_ReKeyBindDevice];
    [self.ble setKeyBindState];
    
    [self tryConnectDevice];
    
}

- (void)tryConnectDevice{
    if (!self.model.peripheralUUID && self.model.peripheralUUID.length == 0) {
        //if node advertise networkID, app can't locations peripheral.
        TeLog(@"还未扫描到设备");
        [self startScanForKeyBind];
    } else {
        //if node advertise nodeIdentity, app can locations peripheral.
        if ([self.ble.currentPeripheral.identifier.UUIDString isEqualToString:self.model.peripheralUUID] && self.ble.isConnected) {
            [self dalayToSetFilter];
        }else{
            __weak typeof(self) weakSelf = self;
            [self.ble cancelConnection:self.ble.currentPeripheral complete:^{
                [weakSelf startConnectForKeyBind];
            }];
        }
    }
}

- (void)startScanForKeyBind{
    [self.ble startScan];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(scanPeripheralTimeout) object:nil];
        [self performSelector:@selector(scanPeripheralTimeout) withObject:nil afterDelay:10.0];
    });
}

- (void)startConnectForKeyBind{
    if (self.model.peripheralUUID) {
        [self.ble connectPeripheralWithUUID:self.model.peripheralUUID];
        [self performSelector:@selector(connectPeripheralWithUUIDTimeout) withObject:nil afterDelay:kConnectWithUUIDTimeout];
    }else{
        TeLog(@"Error:self.model.peripheralUUID=nil");
    }
}

- (void)scanPeripheralTimeout{
    [Bluetooth.share stopScan];
    [self showKeyBindFail];
}

- (void)connectPeripheralWithUUIDTimeout{
    [self showKeyBindFail];
}

- (void)kickoutConnectPeripheralWithUUIDTimeout{
    self.hasClickKickout = NO;
    [ShowTipsHandle.share hidden];
    [SigDataSource.share removeModelWithDeviceAddress:self.model.address];
    [self pop];
}

- (void)keyBindWithPeripheralUUID:(NSString *)uuid{
    //2.keyBind currentPeripheral
    saveLogData([NSString stringWithFormat:@"reKeyBind address:%02x uuid:%@",self.model.address,uuid]);
    __weak typeof(self) weakSelf = self;
    NSNumber *type = [[NSUserDefaults standardUserDefaults] valueForKey:kKeyBindType];
    [self.ble.commandHandle keyBind:self.model.address appkey:SigDataSource.share.curAppKey appkeyIndex:SigDataSource.share.curAppkeyModel.index netkeyIndex:SigDataSource.share.curNetkeyModel.index keyBindType:type.integerValue keyBindSuccess:^(NSString *identify, UInt16 address) {
        saveLogData(@"reKeyBind success");
        [ShowTipsHandle.share show:Tip_ReKeyBindDeviceSuccess];
        [ShowTipsHandle.share delayHidden:3.0];
        [weakSelf performSelector:@selector(pop) withObject:nil afterDelay:3.0];
        weakSelf.hasClickKeyBind = NO;
    } fail:^(NSString *errorString) {
        [weakSelf showKeyBindFail];
        weakSelf.hasClickKeyBind = NO;
    }];
}

- (void)showKeyBindFail{
    saveLogData(@"reKeyBind fail");
    [ShowTipsHandle.share hidden];
    [self.ble cancelAllConnecttionWithComplete:nil];
    
    UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"Warn" message:Tip_ReKeyBindDeviceFail preferredStyle:UIAlertControllerStyleAlert];
    __weak typeof(self) weakSelf = self;
    UIAlertAction *action = [UIAlertAction actionWithTitle:@"confirm" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        [weakSelf keyBind:weakSelf.keindyButton];
    }];
    UIAlertAction *revoke = [UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:nil];
    [alert addAction:action];
    [alert addAction:revoke];
    [self presentViewController:alert animated:YES completion:nil];
}

- (void)kickoutAction{
    TeLog(@"");
    __weak typeof(self) weakSelf = self;
    [DemoCommand kickoutDevice:self.model.address complete:^{
        TeLog(@"delete device success.");
        [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf];
        [weakSelf pop];
    }];
    
    if (self.model && [self.model.peripheralUUID isEqualToString:self.ble.currentPeripheral.identifier.UUIDString]) {
        //if node is Bluetooth.share.currentPeripheral, wait node didDisconnectPeripheral, delay 1.5s and pop.
    } else {
        //if node isn't Bluetooth.share.currentPeripheral, delay 5s and pop.
        [self performSelector:@selector(pop) withObject:nil afterDelay:TimeOut_KickoutConnectedDelayResponseTime];
    }
}

- (void)pop{
    [ShowTipsHandle.share hidden];
    [self.navigationController popViewControllerAnimated:YES];
}

- (void)dalayToSetFilter{
    TeLog(@"");
    if (self.ble.isConnected) {
        __block int responseCount = 0;
        __weak typeof(self) weakSelf = self;
        [Bluetooth.share setFilterWithLocationAddress:SigDataSource.share.curLocationNodeModel.address timeout:kSetFilterTimeout complete:^{
            responseCount ++;
            if (responseCount == 3) {
                set_pair_login_ok(1);
                [weakSelf keyBindWithPeripheralUUID:weakSelf.ble.currentPeripheral.identifier.UUIDString];
                Bluetooth.share.setFilterResponseCallBack = nil;
                [Bluetooth.share cancelSetFilterWithLocationAddressTimeout];
            }
        }fail:^{
            TeLog(@"setFilter fail.");
        }];
        mesh_tx_sec_nw_beacon_all_net(1);//send beacon, blt_sts can only be 0 or 1.
    }
}

- (void)blockState{
    [super blockState];
    
    __weak typeof(self) weakSelf = self;
    [self.ble bleFinishScanedCharachteristic:^(CBPeripheral *peripheral) {
        if (weakSelf.ble.state == StateAddDevice_keyBind && [peripheral.identifier.UUIDString isEqualToString:weakSelf.model.peripheralUUID]) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(connectPeripheralWithUUIDTimeout) object:nil];
            });
            [weakSelf dalayToSetFilter];
        }else if (weakSelf.ble.state == StateNormal && [peripheral.identifier.UUIDString isEqualToString:weakSelf.model.peripheralUUID] && weakSelf.hasClickKickout){
            weakSelf.hasClickKickout = NO;
            dispatch_async(dispatch_get_main_queue(), ^{
                [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(kickoutConnectPeripheralWithUUIDTimeout) object:nil];
            });
            [weakSelf kickoutAction];
        }
    }];
    [self.ble bleDisconnectOrConnectFail:^(CBPeripheral *peripheral) {
        if (weakSelf.hasClickKickout) {
            [ShowTipsHandle.share show:Tip_DisconnectOrConnectFail];
            [weakSelf performSelector:@selector(pop) withObject:nil afterDelay:TimeOut_KickoutDelayResponseTime];
        }
        if (weakSelf.hasClickKeyBind) {
            [weakSelf showKeyBindFail];
        }
    }];
    [self.ble bleScanNewDevice:^(CBPeripheral *peripheral, BOOL provisioned) {
        if (!provisioned && ![SigDataSource.share getScanRspModelWithAddress:self.model.address]) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(scanPeripheralTimeout) object:nil];
            });
            [weakSelf.ble stopScan];
            [weakSelf.ble connectPeripheral:peripheral];
            [weakSelf performSelector:@selector(connectPeripheralWithUUIDTimeout) withObject:nil afterDelay:kConnectWithUUIDTimeout];
        }
    }];
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
    self.title = @"Key Bind";
    SigScanRspModel *scanModel = [SigDataSource.share getScanRspModelWithUUID:self.model.peripheralUUID];
    self.detailLabel.text = [NSString stringWithFormat:@"meshAddress:0x%02X\nmac:%@",self.model.address,[LibTools getMacStringWithMac:scanModel.macAddress]];
}

- (void)viewWillDisappear:(BOOL)animated{
    [super viewWillDisappear:animated];
    [NSObject cancelPreviousPerformRequestsWithTarget:self];
}

-(void)dealloc{
    TeLog(@"");
}
@end
