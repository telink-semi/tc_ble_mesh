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
//  Created by 梁家誌 on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "ReKeyBindViewController.h"

@interface ReKeyBindViewController()
@property (nonatomic, assign) BOOL hasClickKickout;
@property (nonatomic, assign) BOOL hasClickKeyBind;
@property (weak, nonatomic) IBOutlet UILabel *detailLabel;
@property (weak, nonatomic) IBOutlet UIButton *kickOutButton;
@property (weak, nonatomic) IBOutlet UIButton *keindyButton;
@property (nonatomic, strong) SigMessageHandle *messageHandle;
@end

@implementation ReKeyBindViewController

- (IBAction)kickOut:(UIButton *)sender {
    TeLogDebug(@"");
    self.hasClickKickout = YES;
    [ShowTipsHandle.share show:Tip_KickoutDevice];
        
    if (SigBearer.share.isOpen) {
        [self kickoutAction];
    } else {
        [SigDataSource.share deleteNodeFromMeshNetworkWithDeviceAddress:self.model.address];
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
        UInt8 keyBindType = type.integerValue;
        UInt16 productID = [LibTools uint16From16String:self.model.pid];
        DeviceTypeModel *deviceType = [SigDataSource.share getNodeInfoWithCID:kCompanyID PID:productID];
        NSData *cpsData = deviceType.defaultCompositionData.parameters;
        if (keyBindType == KeyBindTpye_Fast) {
            if (cpsData == nil || cpsData.length == 0) {
                keyBindType = KeyBindTpye_Normal;
            }
        }
        if (cpsData && cpsData.length > 0) {
            cpsData = [cpsData subdataWithRange:NSMakeRange(1, cpsData.length - 1)];
        }

        [SDKLibCommand keyBind:self.model.address appkeyModel:SigDataSource.share.curAppkeyModel keyBindType:keyBindType productID:productID cpsData:cpsData keyBindSuccess:^(NSString * _Nonnull identify, UInt16 address) {
            [weakSelf performSelectorOnMainThread:@selector(showKeyBindSuccess) withObject:nil waitUntilDone:YES];
            weakSelf.hasClickKeyBind = NO;
            SigNodeModel *node = [SigDataSource.share getNodeWithAddress:address];
            if (node && node.isRemote) {
                [node addDefaultPublicAddressToRemote];
                [SigDataSource.share saveLocationData];
            }
        } fail:^(NSError * _Nonnull error) {
            [weakSelf performSelectorOnMainThread:@selector(showKeyBindFail) withObject:nil waitUntilDone:YES];
            weakSelf.hasClickKeyBind = NO;
        }];
    } else {
        [SDKLibCommand startMeshConnectWithComplete:^(BOOL successful) {
            if (successful) {
                [weakSelf performSelectorOnMainThread:@selector(keyBind:) withObject:nil waitUntilDone:YES];
            } else {
                [weakSelf performSelectorOnMainThread:@selector(showKeyBindFail) withObject:nil waitUntilDone:YES];
            }
            weakSelf.hasClickKeyBind = NO;
        }];
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(showKeyBindFail) object:nil];
            [self performSelector:@selector(showKeyBindFail) withObject:nil afterDelay:30.0];
        });
    }
    
}

- (void)showKeyBindSuccess {
    TeLogDebug(@"reKeyBind success");
    dispatch_async(dispatch_get_main_queue(), ^{
        [ShowTipsHandle.share show:Tip_ReKeyBindDeviceSuccess];
        [ShowTipsHandle.share delayHidden:3.0];
        [self performSelector:@selector(pop) withObject:nil afterDelay:3.0];
    });
}

- (void)connectPeripheralWithUUIDTimeout{
    [self showKeyBindFail];
}

- (void)kickoutConnectPeripheralWithUUIDTimeout{
    self.hasClickKickout = NO;
    [ShowTipsHandle.share hidden];
    [SigDataSource.share deleteNodeFromMeshNetworkWithDeviceAddress:self.model.address];
    [self pop];
}

- (void)showKeyBindFail{
    TeLogVerbose(@"reKeyBind fail");
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(showKeyBindFail) object:nil];
    });
    [ShowTipsHandle.share hidden];
    [SDKLibCommand stopMeshConnectWithComplete:^(BOOL successful) {
        if (successful) {
            TeLogDebug(@"stopMeshConnect success");
        } else {
            TeLogDebug(@"stopMeshConnect fail");
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
    _messageHandle = [SDKLibCommand resetNodeWithDestination:self.model.address retryCount:0 responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigNodeResetStatus * _Nonnull responseMessage) {
        
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        if (isResponseAll) {
            TeLogDebug(@"kickout success.");
        } else {
            TeLogDebug(@"kickout fail.");
        }
        [SigDataSource.share deleteNodeFromMeshNetworkWithDeviceAddress:weakSelf.model.address];
        [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf];
        [weakSelf pop];
    }];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(resetNodeTimeout) object:nil];
        [self performSelector:@selector(resetNodeTimeout) withObject:nil afterDelay:5.0];
    });

//    if (self.model && [self.model.peripheralUUID isEqualToString:SigBearer.share.getCurrentPeripheral.identifier.UUIDString]) {
//        //if node is Bluetooth.share.currentPeripheral, wait node didDisconnectPeripheral, delay 1.5s and pop.
//    } else {
//        //if node isn't Bluetooth.share.currentPeripheral, delay 5s and pop.
//        [self performSelector:@selector(pop) withObject:nil afterDelay:TimeOut_KickoutConnectedDelayResponseTime];
//    }
}

- (void)resetNodeTimeout {
    [SigDataSource.share deleteNodeFromMeshNetworkWithDeviceAddress:self.model.address];
    [NSObject cancelPreviousPerformRequestsWithTarget:self];
    [self pop];
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
//    self.detailLabel.text = [NSString stringWithFormat:@"meshAddress:0x%02X\nmac:%@",self.model.address,[LibTools getMacStringWithMac:self.model.macAddress]];
    self.detailLabel.text = [NSString stringWithFormat:@"meshAddress:0x%02X\nUUID:%@",self.model.address,self.model.UUID];
}

- (void)viewWillDisappear:(BOOL)animated{
    [super viewWillDisappear:animated];
    [NSObject cancelPreviousPerformRequestsWithTarget:self];
    if (_messageHandle) {
        [_messageHandle cancel];
    }
}

-(void)dealloc{
    TeLogDebug(@"");
}
@end
