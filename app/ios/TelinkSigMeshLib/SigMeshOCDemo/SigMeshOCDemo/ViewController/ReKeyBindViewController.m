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
    TeLogDebug(@"");
    self.hasClickKickout = YES;
    [ShowTipsHandle.share show:Tip_KickoutDevice];
        
    if (SigBearer.share.isOpen) {
        [self kickoutAction];
    } else {
        [SigDataSource.share removeModelWithDeviceAddress:self.model.address];
        [self pop];
    }
}

- (IBAction)keyBind:(UIButton *)sender {
    TeLogDebug(@"");
    self.hasClickKeyBind = YES;
    [self blockState];
    [ShowTipsHandle.share show:Tip_ReKeyBindDevice];
//    [self.ble setKeyBindState];
    
//    [self tryConnectDevice];
    
    __weak typeof(self) weakSelf = self;
    if (SigBearer.share.isOpen) {
        NSNumber *type = [[NSUserDefaults standardUserDefaults] valueForKey:kKeyBindType];
        [SigKeyBindManager.share keyBind:self.model.address appkeyModel:SigDataSource.share.curAppkeyModel keyBindType:type.integerValue productID:0 cpsData:nil keyBindSuccess:^(NSString * _Nonnull identify, UInt16 address) {
            [weakSelf performSelectorOnMainThread:@selector(showKeyBindSuccess) withObject:nil waitUntilDone:YES];
            weakSelf.hasClickKeyBind = NO;
        } fail:^(NSError * _Nonnull error) {
            [weakSelf performSelectorOnMainThread:@selector(showKeyBindFail) withObject:nil waitUntilDone:YES];
            weakSelf.hasClickKeyBind = NO;
        }];
    } else {
        [SigBearer.share startMeshConnectWithTimeOut:kStartMeshConnectTimeout complete:^(BOOL successful) {
            if (successful) {
                [weakSelf performSelectorOnMainThread:@selector(keyBind:) withObject:nil waitUntilDone:YES];
            } else {
                [weakSelf performSelectorOnMainThread:@selector(showKeyBindFail) withObject:nil waitUntilDone:YES];
            }
            weakSelf.hasClickKeyBind = NO;
        }];
    }
    
}

- (void)showKeyBindSuccess {
    TeLogDebug(@"reKeyBind success");
    dispatch_async(dispatch_get_main_queue(), ^{
        [ShowTipsHandle.share show:Tip_ReKeyBindDeviceSuccess];
        [ShowTipsHandle.share delayHidden:3.0];
    });
    [self performSelector:@selector(pop) withObject:nil afterDelay:3.0];
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

- (void)showKeyBindFail{
    saveLogData(@"reKeyBind fail");
    [ShowTipsHandle.share hidden];
    [SigBearer.share closeWithResult:^(BOOL successful) {
        if (successful) {
            TeLogDebug(@"close success");
        } else {
            TeLogDebug(@"close fail");
        }
    }];
    
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
    TeLogDebug(@"send kickout.");
    __weak typeof(self) weakSelf = self;
    [DemoCommand kickoutDevice:self.model.address successCallback:^(UInt16 source, UInt16 destination, SigConfigNodeResetStatus * _Nonnull responseMessage) {

    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
        if (isResponseAll) {
            TeLogDebug(@"kickout success.");
        } else {
            TeLogDebug(@"kickout fail.");
        }
        [SigDataSource.share removeModelWithDeviceAddress:weakSelf.model.address];
        [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf];
        [weakSelf pop];
    }];
    
//    if (self.model && [self.model.peripheralUUID isEqualToString:SigBearer.share.getCurrentPeripheral.identifier.UUIDString]) {
//        //if node is Bluetooth.share.currentPeripheral, wait node didDisconnectPeripheral, delay 1.5s and pop.
//    } else {
//        //if node isn't Bluetooth.share.currentPeripheral, delay 5s and pop.
//        [self performSelector:@selector(pop) withObject:nil afterDelay:TimeOut_KickoutConnectedDelayResponseTime];
//    }
}

- (void)pop{
//    self.ble.commandHandle.delectDeviceCallBack = nil;
    dispatch_async(dispatch_get_main_queue(), ^{
        [ShowTipsHandle.share hidden];
        [self.navigationController popViewControllerAnimated:YES];
    });
}

- (void)blockState{
    [super blockState];
    
//    __weak typeof(self) weakSelf = self;
//    [self.ble bleFinishScanedCharachteristic:^(CBPeripheral *peripheral) {
//        if (weakSelf.ble.state == StateAddDevice_keyBind && [peripheral.identifier.UUIDString isEqualToString:weakSelf.model.peripheralUUID]) {
//            dispatch_async(dispatch_get_main_queue(), ^{
//                [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(connectPeripheralWithUUIDTimeout) object:nil];
//            });
//            [weakSelf dalayToSetFilter];
//        }else if (weakSelf.ble.state == StateNormal && [peripheral.identifier.UUIDString isEqualToString:weakSelf.model.peripheralUUID] && weakSelf.hasClickKickout){
//            weakSelf.hasClickKickout = NO;
//            dispatch_async(dispatch_get_main_queue(), ^{
//                [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(kickoutConnectPeripheralWithUUIDTimeout) object:nil];
//            });
//            [weakSelf kickoutAction];
//        }
//    }];
//    [self.ble bleDisconnectOrConnectFail:^(CBPeripheral *peripheral) {
//        if (weakSelf.hasClickKickout) {
//            [ShowTipsHandle.share show:Tip_DisconnectOrConnectFail];
//            [weakSelf performSelector:@selector(pop) withObject:nil afterDelay:TimeOut_KickoutDelayResponseTime];
//        }
//        if (weakSelf.hasClickKeyBind) {
//            [weakSelf showKeyBindFail];
//        }
//    }];
//    [self.ble bleScanNewDevice:^(CBPeripheral *peripheral, BOOL provisioned) {
//        if (!provisioned && ![SigDataSource.share getScanRspModelWithAddress:self.model.address]) {
//            dispatch_async(dispatch_get_main_queue(), ^{
//                [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf selector:@selector(scanPeripheralTimeout) object:nil];
//            });
//            [weakSelf.ble stopScan];
//            [weakSelf.ble connectPeripheral:peripheral];
//            [weakSelf performSelector:@selector(connectPeripheralWithUUIDTimeout) withObject:nil afterDelay:kConnectWithUUIDTimeout];
//        }
//    }];
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
    TeLogDebug(@"");
}
@end
