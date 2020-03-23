/********************************************************************************************************
 * @file     MeshOTAManager.m
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
//  MeshOTAManager.m
//  TelinkBlueDemo
//
//  Created by Arvin on 2018/4/24.
//  Copyright © 2018年 Green. All rights reserved.
//

#import "MeshOTAManager.h"

@interface MeshOTAManager()
@property (nonatomic, copy) ProgressBlock progressBlock;
@property (nonatomic, copy) FinishBlock finishBlock;
@property (nonatomic, copy) ErrorBlock errorBlock;

@property (nonatomic, strong) NSMutableArray <NSNumber *>*allArray;//Mesh OTA的所有短地址列表
@property (nonatomic, strong) NSMutableArray <NSNumber *>*successArray;//Mesh OTA成功的短地址列表

@end

@implementation MeshOTAManager

+ (MeshOTAManager*)share{
    static MeshOTAManager *meshOTAManager = nil;
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        meshOTAManager = [[MeshOTAManager alloc] init];
        [meshOTAManager initData];
    });
    return meshOTAManager;
}

-(void)initData {
    self.allArray = [NSMutableArray array];
    self.successArray = [NSMutableArray array];
}

- (void)resetMeshOTAData{
    self.otaData = nil;
    self.progressBlock = nil;
    self.finishBlock = nil;
    self.errorBlock = nil;
    [self nilBlock];
}

- (void)stopMeshOTA{
    [self resetMeshOTAData];
    
    __weak typeof(self) weakSelf = self;
    [Bluetooth.share stopAutoConnect];
    [Bluetooth.share cancelAllConnecttionWithComplete:^{
        [weakSelf saveIsMeshOTAing:NO];
        [Bluetooth.share setNormalState];
    }];
    access_cmd_fw_distribut_stop(SigDataSource.share.curLocationNodeModel.address);
}

///查询当前是否处在meshOTA
- (BOOL)isMeshOTAing{
    NSDictionary *dict = [[NSUserDefaults standardUserDefaults] objectForKey:kSaveMeshOTADictKey];
    if (dict != nil && [dict[@"isMeshOTAing"] boolValue]) {
        return YES;
    } else {
        return NO;
    }
}

- (void)saveIsMeshOTAing:(BOOL)isMeshOTAing{
    NSDictionary *dict = @{@"isMeshOTAing":@(isMeshOTAing)};
    [[NSUserDefaults standardUserDefaults] setObject:dict forKey:kSaveMeshOTADictKey];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

///api for LibHandle.m, needn't developer call it.
- (void)callBackProgressFromLib:(u8)progress{
    if (self.progressBlock) {
        self.progressBlock(progress);
        if (progress == 99) {
            //clear FW_UPDATE_STATUS
            [self.successArray removeAllObjects];
        }
        if (progress == 100) {
            [self showMeshOTAResult];
        }
    }
}

- (void)callBackMeshOTASuccessAddress:(u16)address{
    if (![self.successArray containsObject:@(address)] && [self.allArray containsObject:@(address)]) {
        [self.successArray addObject:@(address)];
    }
}

- (void)showMeshOTAResult{
    if (self.finishBlock) {
        NSMutableArray *failArray = [NSMutableArray array];
        if (self.allArray.count != self.successArray.count) {
            for (NSNumber *tem in self.allArray) {
                if (![self.successArray containsObject:tem]) {
                    [failArray addObject:tem];
                }
            }
        }
        self.finishBlock(self.successArray, failArray);
    }
    //OTA完成，初始化参数
    [self resetMeshOTAData];
    [self saveIsMeshOTAing:NO];
}

- (void)showMeshOTAFail{
    if (self.errorBlock) {
        NSError *error = [NSError errorWithDomain:@"Mesh OTA fail" code:-1 userInfo:nil];
        self.errorBlock(error);
    }
    [self stopMeshOTA];
}

- (void)startMeshOTAWithLocationAddress:(int)locationAddress cid:(int)cid deviceAddresses:(NSArray <NSNumber *>*)deviceAddresses otaData:(NSData *)otaData progressHandle:(ProgressBlock)progressBlock finishHandle:(FinishBlock)finishBlock errorHandle:(ErrorBlock)errorBlock{
    self.otaData = otaData;
    self.progressBlock = progressBlock;
    self.finishBlock = finishBlock;
    self.errorBlock = errorBlock;
    [self saveIsMeshOTAing:YES];
    [self blockState];
    [self.allArray removeAllObjects];
    [self.successArray removeAllObjects];
    [self.allArray addObjectsFromArray:deviceAddresses];
    
    NSString *temStr = @"a3ff0000000000000100b60a1102210000ff00c00200";
    for (int i=1; i<deviceAddresses.count; i++) {
        temStr = [temStr stringByAppendingString:@"0000"];
    }
    NSData *cmdData = [LibTools nsstringToHex:temStr];
    u8 *bytes = (u8 *)cmdData.bytes;
    memcpy(bytes+8, &locationAddress, 2);
    memcpy(bytes+12, &cid, 2);
    for (int i=0; i<deviceAddresses.count; i++) {
        int address = [deviceAddresses[i] intValue];
        memcpy(bytes+20+i*2, &address, 2);
    }
//    TeLog(@"startMeshOTA:%@",[NSData dataWithBytes:bytes length:(u32)cmdData.length]);
    SendOpByINI(bytes, (u32)cmdData.length);
}

- (void)blockState{
    TeLog(@"");
    __weak typeof(self) weakSelf = self;
    [Bluetooth.share setWorkWithPeripheralCallBack:^(NSString *uuidString) {
        if (weakSelf.isMeshOTAing) {
        //mesh ota 过程中断开，等待重连并继续mesh ota
        APP_set_mesh_ota_pause_flag(0);
        }
    }];
}

- (void)nilBlock{
    Bluetooth.share.workWithPeripheralCallBack = nil;
}

@end
