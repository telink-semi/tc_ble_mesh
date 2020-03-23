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
@property (weak, nonatomic) IBOutlet UIButton *kickOutButton;
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

- (IBAction)clickGetSensorStatus:(UIButton *)sender {
    NSString *str = @"a3 ff 00 00 00 00 00 00 02 00 e0 11 02 e1 02 00 00";
    NSString *string = [str.uppercaseString removeAllSapceAndNewlines];
    NSData *data = [LibTools nsstringToHex:string];
    
    u8 *bytes = (u8 *)data.bytes;
    u16 address = self.model.address;
    memcpy(bytes+8, &address, 2);
    data = [NSData dataWithBytes:bytes length:data.length];
//    SendOpByINI(bytes, (u32)data.length);
    
    IniCommandModel *model = [[IniCommandModel alloc] initWithIniCommandData:data];
    __weak typeof(self) weakSelf = self;
    [self.ble.commandHandle sendIniCommand:model responseCallback:^(ResponseModel *m) {
        u16 h = 0,t = 0,address = 0;
        Byte *byte = (Byte *)[m.rspData bytes];
        if (m.rspData.length >= 11) {
            memcpy(&h, byte + 9, 2);
        }
        if (m.rspData.length >= 13) {
            memcpy(&t, byte + 11, 2);
        }
        if (m.rspData.length >= 4) {
            memcpy(&address, byte + 2, 2);
        }
        if (address == weakSelf.model.address) {
            [weakSelf showNewLogMessage:[NSString stringWithFormat:@"response:%@,h=%d,t=%d",m.rspData,h,t]];
        }
    }];
    
    [self showNewLogMessage:[NSString stringWithFormat:@"send:%@",data]];
}

- (IBAction)clickKictOut:(UIButton *)sender {
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

- (void)showNewLogMessage:(NSString *)msg{
    NSDateFormatter *dformatter = [[NSDateFormatter alloc] init];
    dformatter.dateFormat =@"HH:mm:ss.SSS";
    self.logString = [self.logString stringByAppendingString:[NSString stringWithFormat:@"%@ %@\n",[dformatter stringFromDate:[NSDate date]],msg]];
    dispatch_async(dispatch_get_main_queue(), ^{
        self.logTV.text = self.logString;
        [self.logTV scrollRangeToVisible:NSMakeRange(self.logTV.text.length, 1)];
    });
}

- (void)blockState{
    [super blockState];
    __weak typeof(self) weakSelf = self;
//#warning 2019年12月27日11:27:07，sensor ini指令，待完善。
//    [self.ble.commandHandle setResponseVendorIDCallBack:^(VendorResponseModel *m) {
//        u16 h = 0,t = 0,address = 0;
//        Byte *byte = (Byte *)[m.rspData bytes];
//        if (m.rspData.length >= 11) {
//            memcpy(&h, byte + 9, 2);
//        }
//        if (m.rspData.length >= 13) {
//            memcpy(&t, byte + 11, 2);
//        }
//        if (m.rspData.length >= 4) {
//            memcpy(&address, byte + 2, 2);
//        }
//        if (address == self.model.address) {
//            [weakSelf showNewLogMessage:[NSString stringWithFormat:@"response:%@,h=%d,t=%d",m.rspData,h,t]];
//        }
//    }];
    
    [self.ble bleDisconnectOrConnectFail:^(CBPeripheral *peripheral) {
        if (weakSelf.hasClickKickout) {
            [ShowTipsHandle.share show:Tip_DisconnectOrConnectFail];
            [weakSelf performSelector:@selector(pop) withObject:nil afterDelay:TimeOut_KickoutDelayResponseTime];
        }
    }];
}

- (void)nilBlock{
    [super nilBlock];
//    self.ble.commandHandle.responseVendorIDCallBack = nil;
}

-(void)dealloc{
    TeLog(@"");
}

@end
