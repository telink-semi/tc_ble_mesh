/********************************************************************************************************
 * @file     SensorVC.m
 *
 * @brief    for TLSR chips
 *
 * @author     telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *             The information contained herein is confidential and proprietary property of Telink
 *              Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *             of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *             Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 *              Licensees are granted free, non-transferable use of the information in this
 *             file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/
//
//  SensorVC.m
//  SigMeshOCDemo
//
//  Created by Liangjiazhi on 2019/3/14.
//  Copyright © 2019年 Telink. All rights reserved.
//

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
//    NSString *str = @"a3 ff 00 00 00 00 00 00 02 00 e0 11 02 e1 02 00 00";
    NSString *str = @"a3 ff 00 00 00 00 00 00 02 00 82 01";
    NSString *string = [str.uppercaseString removeAllSapceAndNewlines];
    NSData *data = [LibTools nsstringToHex:string];
    
    UInt8 *bytes = (UInt8 *)data.bytes;
    UInt16 address = self.model.address;
    memcpy(bytes+8, &address, 2);
    NSData *iniData = [NSData dataWithBytes:bytes length:data.length];
    
    __weak typeof(self) weakSelf = self;
    [SDKLibCommand sendOpINIData:iniData successCallback:^(UInt16 source, UInt16 destination, SigMeshMessage * _Nonnull responseMessage) {
        NSString *str = [NSString stringWithFormat:@"Response: opcode=0x%x, parameters=%@",(unsigned int)responseMessage.opCode,responseMessage.parameters];
        TeLogVerbose(@"%@",str);
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
        TeLogVerbose(@"");
    }];
    TeLogInfo(@"send ini:%@",[LibTools convertDataToHexStr:[NSData dataWithBytes:bytes length:data.length]]);
}

//- (IBAction)clickKictOut:(UIButton *)sender {
//    TeLogDebug(@"");
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
//    TeLogDebug(@"");
//    __weak typeof(self) weakSelf = self;
//    [DemoCommand kickoutDevice:self.model.address retryCount:0 responseMaxCount:0 successCallback:^(UInt16 source, UInt16 destination, SigConfigNodeResetStatus * _Nonnull responseMessage) {
//        TeLogDebug(@"delete device success.");
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
    TeLogDebug(@"");
}

@end
