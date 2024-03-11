/********************************************************************************************************
 * @file     SensorVC.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/3/14
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

#import "SensorVC.h"
#import "NSString+extension.h"

@interface SensorVC ()
@property (weak, nonatomic) IBOutlet UITextView *logTV;
@property (weak, nonatomic) IBOutlet UIButton *GetSensorStatusButton;

@property (nonatomic,strong) NSString *logString;
@property (nonatomic, assign) BOOL hasClickKickout;

@end

@implementation SensorVC

- (void)viewDidLoad {
    [super viewDidLoad];
    self.title = @"Sensor";
    self.logString = @"";
}

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.tabBarController.tabBar.hidden = YES;
}

- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
}

- (IBAction)clickGetSensorStatus:(UIButton *)sender {
    NSString *str = @"a3 ff 00 00 00 00 00 00 02 00 e0 11 02 e1 02 00 00";
//    NSString *str = @"a3 ff 00 00 00 00 00 00 02 00 82 01";
    NSString *string = [str.uppercaseString removeAllSpaceAndNewlines];
    NSData *data = [LibTools nsstringToHex:string];

    UInt8 *bytes = (UInt8 *)data.bytes;
    UInt16 address = self.model.address;
    memcpy(bytes+8, &address, 2);
    NSData *iniData = [NSData dataWithBytes:bytes length:data.length];

    __weak typeof(self) weakSelf = self;
    [SDKLibCommand sendOpINIData:iniData successCallback:^(UInt16 source, UInt16 destination, SigMeshMessage * _Nonnull responseMessage) {
        NSString *str = [NSString stringWithFormat:@"Response: opcode=0x%x, parameters=%@",(unsigned int)responseMessage.opCode,responseMessage.parameters];
        TelinkLogVerbose(@"%@",str);
        [weakSelf showNewLogMessage:str];
        if (responseMessage.opCode == 0xE11102 && source == weakSelf.model.address) {
            UInt16 h = 0,t = 0;
            Byte *byte = (Byte *)[responseMessage.parameters bytes];
            if (responseMessage.parameters.length >= 2) {
                memcpy(&h, byte, 2);
            }
            if (responseMessage.parameters.length >= 4) {
                memcpy(&t, byte+2, 2);
            }
            [weakSelf showNewLogMessage:[NSString stringWithFormat:@"response:%@,h=%d,t=%d",responseMessage.parameters,h,t]];
        }
    } resultCallback:^(BOOL isResponseAll, NSError * _Nullable error) {
        TelinkLogVerbose(@"");
    }];
    TelinkLogInfo(@"send ini:%@",[LibTools convertDataToHexStr:[NSData dataWithBytes:bytes length:data.length]]);
}

//- (IBAction)clickKictOut:(UIButton *)sender {
//    TelinkLogDebug(@"");
//    self.hasClickKickout = YES;
//    [ShowTipsHandle.share show:Tip_KickoutDevice];
//
//    [SigDataSource.share deleteNodeFromMeshNetworkWithDeviceAddress:self.model.address];
//
//    if (SigBearer.share.isOpen) {
//        [self kickoutAction];
//    } else {
//        [self pop];
//    }
//
//}
//
//- (void)kickoutAction{
//    TelinkLogDebug(@"");
//    __weak typeof(self) weakSelf = self;
//    [DemoCommand kickoutDevice:self.model.address retryCount:0 responseMaxCount:0 successCallback:^(UInt16 source, UInt16 destination, SigConfigNodeResetStatus * _Nonnull responseMessage) {
//        TelinkLogDebug(@"delete device success.");
//        [NSObject cancelPreviousPerformRequestsWithTarget:weakSelf];
//        [weakSelf pop];
//    } resultCallback:^(BOOL isResponseAll, NSError * _Nonnull error) {
//
//    }];
//
//    if (self.model && [self.model.peripheralUUID isEqualToString:SigBearer.share.getCurrentPeripheral.identifier.UUIDString]) {
//        //if node is Bluetooth.share.currentPeripheral, wait node didDisconnectPeripheral, delay 1.5s and pop.
//    } else {
//        //if node isn't Bluetooth.share.currentPeripheral, delay 5s and pop.
//        [self performSelector:@selector(pop) withObject:nil afterDelay:TimeOut_KickoutConnectedDelayResponseTime];
//    }
//}
//
//- (void)pop{
//    dispatch_async(dispatch_get_main_queue(), ^{
//        [ShowTipsHandle.share hidden];
//        [self.navigationController popViewControllerAnimated:YES];
//    });
//}

- (void)showNewLogMessage:(NSString *)msg{
    NSDateFormatter *dformatter = [[NSDateFormatter alloc] init];
    dformatter.dateFormat =@"HH:mm:ss.SSS";
    self.logString = [self.logString stringByAppendingString:[NSString stringWithFormat:@"%@ %@\n",[dformatter stringFromDate:[NSDate date]],msg]];
    dispatch_async(dispatch_get_main_queue(), ^{
        self.logTV.text = self.logString;
        [self.logTV scrollRangeToVisible:NSMakeRange(self.logTV.text.length, 1)];
    });
}

-(void)dealloc{
    TelinkLogDebug(@"");
}

@end
