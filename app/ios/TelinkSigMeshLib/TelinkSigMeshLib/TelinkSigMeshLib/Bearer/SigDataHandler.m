/********************************************************************************************************
 * @file     SigDataHandler.m
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
//  SigDataHandler.m
//  SigMeshLib
//
//  Created by Liangjiazhi on 2019/8/19.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import "SigDataHandler.h"
#import "ProxyProtocolHandler.h"
#import "SigEncryptionHelper.h"

@interface SigDataHandler ()
@property (nonatomic, strong) NSThread *receiveThread;
@property (nonatomic, strong) NSThread *sendThread;
/// The protocol used for segmentation and reassembly.
@property (nonatomic, strong) ProxyProtocolHandler *protocolHandler;

@end

@implementation SigDataHandler

+ (SigDataHandler *)share {
    static SigDataHandler *shareHandler = nil;
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        shareHandler = [[SigDataHandler alloc] init];
        shareHandler.protocolHandler = [[ProxyProtocolHandler alloc] init];
        [shareHandler initThread];
    });
    return shareHandler;
}

- (void)initThread{
    _receiveThread = [[NSThread alloc] initWithTarget:self selector:@selector(startThread) object:nil];
    [_receiveThread start];
    _sendThread = [[NSThread alloc] initWithTarget:self selector:@selector(startThread) object:nil];
    [_sendThread start];
}

#pragma mark - Private
- (void)startThread{
    [NSTimer scheduledTimerWithTimeInterval:[[NSDate distantFuture] timeIntervalSinceNow] target:self selector:@selector(nullFunc) userInfo:nil repeats:NO];
    while (1) {
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
    }
}

- (void)nullFunc{}

- (void)receiveOnlineStatueData:(NSData *)data {
    [self performSelector:@selector(anasislyOnlineStatueDataFromUUID:) onThread:self.receiveThread withObject:data waitUntilDone:NO];
}

- (void)anasislyOnlineStatueDataFromUUID:(NSData *)data{
//    TeLogInfo(@"onlineStatus解密前=%@",[LibTools convertDataToHexStr:data]);
//    UInt8 *byte = (UInt8 *)data.bytes;
//    online_st_gatt_dec(byte, data.length);
//    TeLogInfo(@"onlineStatus解密后=%@",[LibTools convertDataToHexStr:[NSData dataWithBytes:byte length:data.length]]);
    
    //使用OC提供的方法进行解密
//    TeLogInfo(@"onlineStatus解密前=%@,key=%@",[LibTools convertDataToHexStr:data],[LibTools convertDataToHexStr:beaconKey]);
    NSData *beaconKey = SigDataSource.share.curNetkeyModel.keys.beaconKey;
    UInt8 *beaconKeyByte = (UInt8 *)beaconKey.bytes;
    UInt8 *byte = (UInt8 *)data.bytes;
    UInt8 allLen = data.length;
    UInt8 ivLen = 4;
    UInt8 micLen = 2;
    UInt8 dataLen = allLen - ivLen - micLen;
    [SigEncryptionHelper.share aesAttDecryptionPacketOnlineStatusWithNetworkBeaconKey:beaconKeyByte iv:byte ivLen:ivLen mic:byte + ivLen + dataLen micLen:micLen ps:byte + ivLen psLen:dataLen];
    TeLogInfo(@"onlineStatus解密后=%@",[LibTools convertDataToHexStr:[NSData dataWithBytes:byte length:data.length]]);

    UInt8 opcodeInt=0,statusDataLength=6,statusCount=0;
    memcpy(&opcodeInt, byte, 1);
    memcpy(&statusDataLength, byte + 1, 1);
    statusCount = (UInt8)(data.length-4-2)/statusDataLength;//减去OPCode+length+snumber+CRC
    
    for (int i=0; i<statusCount; i++) {
        UInt16 address = 0;
        memcpy(&address, byte + 4 + statusDataLength*i, 2);
        if (address == 0) {
            continue;
        }
        SigNodeModel *device = [SigDataSource.share getNodeWithAddress:address];
        if (device) {
            UInt8 stateInt=0,bright100=0,temperature100=0;
            if (statusDataLength > 2) {
                memcpy(&stateInt, byte + 4 + statusDataLength*i + 2, 1);
            }
            if (statusDataLength > 3) {
                memcpy(&bright100, byte + 4 + statusDataLength*i + 3, 1);
            }
            if (statusDataLength > 4) {
                memcpy(&temperature100, byte + 4 + statusDataLength*i + 4, 1);
            }

            DeviceState state = stateInt == 0 ? DeviceStateOutOfLine : (bright100 == 0 ? DeviceStateOff : DeviceStateOn);
            [device updateOnlineStatusWithDeviceState:state  bright100:bright100 temperature100:temperature100];
            [SigMeshLib.share updateOnlineStatusWithDeviceAddress:address deviceState:state bright100:bright100 temperature100:temperature100];            
        }
    }
}

@end
