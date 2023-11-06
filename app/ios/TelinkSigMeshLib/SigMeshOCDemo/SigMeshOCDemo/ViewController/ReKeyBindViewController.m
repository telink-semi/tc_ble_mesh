/********************************************************************************************************
 * @file     ReKeyBindViewController.m 
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
    __weak typeof(self) weakSelf = self;
    if (SigBearer.share.isOpen) {
        NSNumber *type = [[NSUserDefaults standardUserDefaults] valueForKey:kKeyBindType];
        UInt8 keyBindType = type.integerValue;
        UInt16 productID = [LibTools uint16From16String:self.model.pid];
        DeviceTypeModel *deviceType = [SigDataSource.share getNodeInfoWithCID:kCompanyID PID:productID];
        NSData *cpsData = deviceType.defaultCompositionData.parameters;
        if (keyBindType == KeyBindType_Fast) {
            if (cpsData == nil || cpsData.length == 0) {
                keyBindType = KeyBindType_Normal;
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
            [self performSelector:@selector(showKeyBindFail) withObject:nil afterDelay:120.0];//非直连设备进行keybind，多跳的情况下速度会很慢，修改超时时间。
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
    _messageHandle = [SDKLibCommand resetNodeWithDestination:self.model.address retryCount:SigDataSource.share.defaultRetryCount responseMaxCount:1 successCallback:^(UInt16 source, UInt16 destination, SigConfigNodeResetStatus * _Nonnull responseMessage) {
        
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        if (isResponseAll) {
            TeLogDebug(@"kickout success.");
        } else {
            TeLogDebug(@"kickout fail.");
        }
        [SigDataSource.share deleteNodeFromMeshNetworkWithDeviceAddress:weakSelf.model.address];
        dispatch_async(dispatch_get_main_queue(), ^{
            [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf];
            [weakSelf pop];
        });
    }];
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(resetNodeTimeout) object:nil];
        [self performSelector:@selector(resetNodeTimeout) withObject:nil afterDelay:5.0];
    });
}

- (void)resetNodeTimeout {
    [SigDataSource.share deleteNodeFromMeshNetworkWithDeviceAddress:self.model.address];
    [NSObject cancelPreviousPerformRequestsWithTarget:self];
    [self pop];
}

- (void)pop{
    dispatch_async(dispatch_get_main_queue(), ^{
        [ShowTipsHandle.share hidden];
        [self.navigationController popViewControllerAnimated:YES];
    });
}

- (void)normalSetting {
    [super normalSetting];
    self.title = @"Key Bind";
    self.detailLabel.text = [NSString stringWithFormat:@"meshAddress:0x%02X\nUUID:%@",self.model.address,self.model.UUID];
    self.kickOutButton.backgroundColor = UIColor.telinkButtonRed;
    self.keindyButton.backgroundColor = UIColor.telinkButtonBlue;
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
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
