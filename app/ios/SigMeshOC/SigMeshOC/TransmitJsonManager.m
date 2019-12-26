/********************************************************************************************************
 * @file     TransmitJsonManager.m
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
//  TransmitJsonManager.m
//  SigMeshOC
//
//  Created by Liangjiazhi on 2018/11/22.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import "TransmitJsonManager.h"

typedef void(^writeJsonResponse)(void);

@interface TransmitJsonManager ()<CBCentralManagerDelegate,CBPeripheralDelegate,CBPeripheralManagerDelegate>
//分享端
@property (nonatomic,strong) CBCentralManager *manager;
@property (nonatomic,strong) CBPeripheral *sharePeripheral;
@property (nonatomic,strong) CBCharacteristic *jsonCharacteristic;
@property (nonatomic,copy) writeJsonResponse responseBlock;
@property (nonatomic,strong) NSData *shareJsonData;
@property (nonatomic,strong) NSString *shareMeshUUID;

//接收端
@property (nonatomic,strong) CBPeripheralManager *peripheralManager;
@property (nonatomic,strong) CBMutableCharacteristic *app2devCharacteristic;
@property (nonatomic,assign) BOOL hasInitServises;//标记是否已经初始化服务列表
@property (nonatomic,strong) NSMutableData *receiveJsonData;
@property (nonatomic,strong) NSString *receiveMeshUUID;

@property (nonatomic,assign) BOOL isSharing;
@property (nonatomic,assign) BOOL isReceiving;

@end

@implementation TransmitJsonManager

+ (TransmitJsonManager *)share{
    static TransmitJsonManager *share = nil;
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        share = [[TransmitJsonManager alloc] init];
        share.hasInitServises = NO;
        [share initManager];
    });
    return share;
}

///初始化SDK，包括初始化服务列表
- (void)initManager{
    self.manager = [[CBCentralManager alloc] initWithDelegate:self queue:dispatch_get_main_queue()];
    self.peripheralManager = [[CBPeripheralManager alloc] initWithDelegate:self queue:dispatch_get_main_queue()];
}

#pragma mark - 分享端

- (void)startScan{
    if (self.manager.state == CBCentralManagerStatePoweredOn) {
        NSLog(@"TransmitJsonManager startScan");
        [self.manager scanForPeripheralsWithServices:nil options:nil];
    } else {
        NSLog(@"try scan");
        [self performSelector:@selector(startScan) withObject:nil afterDelay:0.1];
    }

}

#pragma mark - CBCentralManagerDelegate
- (void)centralManagerDidUpdateState:(CBCentralManager *)central{
    NSLog(@"centralManagerDidUpdateState = %ld",(long)central.state);
}

- (void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary<NSString *,id> *)advertisementData RSSI:(NSNumber *)RSSI{
    if (central == self.manager && [advertisementData.allKeys containsObject:CBAdvertisementDataServiceUUIDsKey]) {
        NSArray *suuids = advertisementData[CBAdvertisementDataServiceUUIDsKey];
        if (suuids && suuids.count != 1) {
            return;
        }
        
        NSString *suuidString = ((CBUUID *)suuids.firstObject).UUIDString;
        
        if ([suuidString isEqualToString:self.shareMeshUUID]) {
            self.sharePeripheral = peripheral;
            [self.manager stopScan];
            [self.manager connectPeripheral:peripheral options:nil];
        }
    }
}

- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral{
    if (central == self.manager && peripheral == self.sharePeripheral) {
        peripheral.delegate = self;
        [peripheral discoverServices:nil];
    }
}

#pragma mark - CBPeripheralDelegate
- (void)peripheral:(CBPeripheral *)peripheral didDiscoverServices:(NSError *)error{
    TeLog(@"didDiscoverServices");
    for (CBService *s in peripheral.services) {
        if ([s.UUID.UUIDString isEqualToString:kServiceUUID]) {
            [peripheral discoverCharacteristics:nil forService:s];
        }
    }
}

- (void)peripheral:(CBPeripheral *)peripheral didDiscoverCharacteristicsForService:(CBService *)service error:(NSError *)error{
    NSLog(@"didDiscoverCharacteristicsForService");
    for (CBCharacteristic *c in service.characteristics) {
        [peripheral setNotifyValue:YES forCharacteristic:c];
        if ([c.UUID.UUIDString isEqualToString:kWriteNotiyCharacteristicUUID]) {
            self.jsonCharacteristic = c;
            NSInteger jsonCount = self.shareJsonData.length/kPacketJsonLength + (self.shareJsonData.length%kPacketJsonLength==0?0:1);
            
            NSOperationQueue *oprationQueue = [[NSOperationQueue alloc] init];
            [oprationQueue addOperationWithBlock:^{
                //这个block语句块在子线程中执行
                NSLog(@"oprationQueue");
                
                __weak typeof(self) weakSelf = self;
                dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
                [self startSendJsonWithResponse:^{
                    dispatch_semaphore_signal(semaphore);
                }];
                dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
                
                __block NSInteger curIndex = 0;
                while (curIndex < jsonCount) {
                    NSData *data = nil;
                    if (kPacketJsonLength*(curIndex+1) > self.shareJsonData.length) {
                        data = [self.shareJsonData subdataWithRange:NSMakeRange(kPacketJsonLength*curIndex, self.shareJsonData.length - kPacketJsonLength*curIndex)];
                    } else {
                        data = [self.shareJsonData subdataWithRange:NSMakeRange(kPacketJsonLength*curIndex, kPacketJsonLength)];
                    }
                    
                    [self sendJsonData:data index:(int)curIndex response:^{
                        curIndex ++;
                        dispatch_semaphore_signal(semaphore);
                    }];
                    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
                    
                }
                
                [self endSendJsonWithResponse:^{
                    [weakSelf.manager cancelPeripheralConnection:weakSelf.sharePeripheral];
                }];
            }];
            break;
        }
    }
}

- (void)peripheral:(CBPeripheral *)peripheral didWriteValueForCharacteristic:(CBCharacteristic *)characteristic error:(nullable NSError *)error{
    if (peripheral == self.sharePeripheral && characteristic == self.jsonCharacteristic && error == nil) {
        if (self.responseBlock) {
            self.responseBlock();
        }
    }
}

- (void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error{
    if (central == self.manager && peripheral == self.sharePeripheral && error == nil) {
//        //分享一次完成，重新广播，可以继续给其它设备进行分享
//        [self performSelector:@selector(startScan) withObject:nil afterDelay:1.0];
        //分享一次完成，回调分享完成代理方法
        if ([self.delegate respondsToSelector:@selector(onEndShare)]) {
            [self.delegate onEndShare];
        }
    }
}

- (void)sendJsonData:(NSData *)data index:(int)index response:(writeJsonResponse)response{
    
    self.responseBlock = response;
    
    NSInteger length = data.length+4;
    Byte *tempBytes = (Byte *)[data bytes];
    Byte resultBytes[length];
    
    memset(resultBytes, 0xff, length);      //初始化
    memcpy(resultBytes, &index, 2); //设置索引
    memcpy(resultBytes+2, tempBytes, data.length); //copy传过来的data
    uint16_t crc = crc16(resultBytes, (int)data.length + 2);
    memcpy(resultBytes+data.length+2, &crc, 2); //设置crc校验值
    NSData *writeData = [NSData dataWithBytes:resultBytes length:length];
    TeLog(@"jsonPackIndex -> %04x ,length:%lu", index,(unsigned long)writeData.length);
    [self sendData:writeData];
}

- (void)startSendJsonWithResponse:(writeJsonResponse)response{
    
    self.responseBlock = response;
    
    NSLog(@"startSendJson");
    UInt8 buf[2] = {0x01,0xff};
    NSData *writeData = [NSData dataWithBytes:buf length:2];
    [self sendData:writeData];
}

- (void)endSendJsonWithResponse:(writeJsonResponse)response{
    
    self.responseBlock = response;
    
    NSLog(@"endSendJson");
    UInt8 buf[2] = {0x02,0xff};
    NSData *writeData = [NSData dataWithBytes:buf length:2];
    [self sendData:writeData];
}

- (void)sendData:(NSData *)data{
    if (self.jsonCharacteristic) {
        [self.sharePeripheral writeValue:data forCharacteristic:self.jsonCharacteristic type:CBCharacteristicWriteWithResponse];
    }
}

#pragma mark - 接收端
- (void)initServices{
    NSLog(@"initServices");
    //1.创建特征1
    self.app2devCharacteristic = [[CBMutableCharacteristic alloc] initWithType:[CBUUID UUIDWithString:kWriteNotiyCharacteristicUUID] properties:CBCharacteristicPropertyNotify|CBCharacteristicPropertyWrite value:nil permissions:CBAttributePermissionsReadable|CBAttributePermissionsWriteable];
    //characteristics字段描述
    CBUUID *CBUUIDCharacteristicUserDescriptionStringUUID = [CBUUID UUIDWithString:CBUUIDCharacteristicUserDescriptionString];
    //设置description
    CBMutableDescriptor *readwriteCharacteristicDescription1 = [[CBMutableDescriptor alloc]initWithType: CBUUIDCharacteristicUserDescriptionStringUUID value:@"App to Device"];
    
    [self.app2devCharacteristic setDescriptors:@[readwriteCharacteristicDescription1]];
    
    //2.service1初始化并加入两个characteristics
    CBMutableService *service1 = [[CBMutableService alloc]initWithType:[CBUUID UUIDWithString:kServiceUUID] primary:YES];
    [service1 setCharacteristics:@[self.app2devCharacteristic]];
    [self.peripheralManager removeAllServices];
    [self.peripheralManager addService:service1];
    
}

///初始化服务列表中特征的值
- (void)initCharacteristicValue{
    NSData *data = [@"0" dataUsingEncoding:NSUTF8StringEncoding];
    [self.app2devCharacteristic setValue:data];
}

///开始广播
- (void)startAdvertising:(NSString *)string{
    //备注：对于外设管理器只支持两个key  `CBAdvertisementDataLocalNameKey`和 `CBAdvertisementDataServiceUUIDsKey`.
    [self.peripheralManager startAdvertising:@{CBAdvertisementDataServiceUUIDsKey : @[[CBUUID UUIDWithString:string]]}];
}

///结束广播
- (void)stopAdvertising{
    if (self.peripheralManager.isAdvertising) {
        [self.peripheralManager stopAdvertising];
    }
}

#pragma mark - CBPeripheralManagerDelegate
//peripheralManager状态改变
- (void)peripheralManagerDidUpdateState:(CBPeripheralManager *)peripheral{
    NSLog(@"callback peripheralManagerDidUpdateState");
    BOOL isOpen = NO;
    switch (peripheral.state) {
            //在这里判断蓝牙设别的状态  当开启了则可调用  setUp方法(自定义)
        case CBPeripheralManagerStatePoweredOn:
            NSLog(@"powered on");
            isOpen = YES;
            if (!self.hasInitServises) {
                [self initServices];
            }
            break;
        case CBPeripheralManagerStatePoweredOff:
            NSLog(@"powered off");
            break;
        default:
            break;
    }
}

//perihpheral添加了service
- (void)peripheralManager:(CBPeripheralManager *)peripheral didAddService:(CBService *)service error:(NSError *)error{
    NSLog(@"callback didAddService");
    if (error) {
        NSLog(@"error =%@",error.localizedDescription);
    }else{
        self.hasInitServises = YES;
        [self initCharacteristicValue];
    }
}

//peripheral开始发送advertising
- (void)peripheralManagerDidStartAdvertising:(CBPeripheralManager *)peripheral error:(NSError *)error{
    NSLog(@"callback peripheralManagerDidStartAdvertising");
    BOOL isSuccess = NO;
    if (error) {
        NSLog(@"error =%@",error.localizedDescription);
    } else {
        isSuccess = YES;
    }
}

//读characteristics请求
- (void)peripheralManager:(CBPeripheralManager *)peripheral didReceiveReadRequest:(CBATTRequest *)request{
    NSLog(@"callback didReceiveReadRequest");
    //判断是否有读数据的权限
    if (request.characteristic.properties & CBCharacteristicPropertyRead) {
        NSData *data = request.characteristic.value;
        [request setValue:data];
        NSLog(@"data = %@",data);
        //对请求作出成功响应
        [self.peripheralManager respondToRequest:request withResult:CBATTErrorSuccess];
    }else{
        [self.peripheralManager respondToRequest:request withResult:CBATTErrorWriteNotPermitted];
    }
}

/*
 jsonData传输步骤：
 start ：0x01 0xff
 data：index(2bytes)+data(16bytes)+crc(2bytes)(最后一个包data不足16bytes，所以最后包长度为5-20)
 end：0x02 0xff
 */
//写characteristics请求
- (void)peripheralManager:(CBPeripheralManager *)peripheral didReceiveWriteRequests:(NSArray *)requests{
    NSLog(@"callback didReceiveWriteRequests");
    CBATTRequest *request = requests[0];
    
    //判断是否有写数据的权限
    if (request.characteristic.properties & CBCharacteristicPropertyWrite) {
        //需要转换成CBMutableCharacteristic对象才能进行写值
        CBMutableCharacteristic *c =(CBMutableCharacteristic *)request.characteristic;
        c.value = request.value;
        NSData *data = request.value;
        if (data.length == 2) {
            UInt8 Opcode = 0,value = 0;
            [data getBytes:&Opcode range:NSMakeRange(0, 1)];
            [data getBytes:&value range:NSMakeRange(1, 1)];
            if (Opcode == 1 && value == 0xff) {
                //收到开始包
                self.receiveJsonData = [NSMutableData data];
            }else if (Opcode == 2 && value == 0xff){
                //收到结束包
                if (self.receiveJsonData.length > 0) {
                    if ([self.delegate respondsToSelector:@selector(onReceiveJsonData:)]) {
                        [self.delegate onReceiveJsonData:self.receiveJsonData];
                    }
                    [self stopAdvertising];
                }
            }
        }else if (data.length > 4 && data.length <= kPacketAllLength){
            //json数据包拼接
            [self.receiveJsonData appendData:[data subdataWithRange:NSMakeRange(2, data.length-4)]];
        }
        [self.peripheralManager respondToRequest:request withResult:CBATTErrorSuccess];
    }else{
        [self.peripheralManager respondToRequest:request withResult:CBATTErrorWriteNotPermitted];
    }
}

//
- (void)peripheralManagerIsReadyToUpdateSubscribers:(CBPeripheralManager *)peripheral{
    NSLog(@"callback peripheralManagerIsReadyToUpdateSubscribers");
}

//订阅characteristic
- (void)peripheralManager:(CBPeripheralManager *)peripheral central:(CBCentral *)central didSubscribeToCharacteristic:(CBCharacteristic *)characteristic{
    NSLog(@"callback didSubscribeToCharacteristic");
}

//取消订阅characteristic
- (void)peripheralManager:(CBPeripheralManager *)peripheral central:(CBCentral *)central didUnsubscribeFromCharacteristic:(CBCharacteristic *)characteristic{
    NSLog(@"callback didUnsubscribeFromCharacteristic");
}


#pragma mark - 分享端
/**
 开始分享
 
 @param string MeshUUID转为serviceUUID格式的字符串，用于标记蓝牙模拟设备。
 @param jsonData json的NSData格式的数据。
 */
- (void)startShareWithString:(NSString *)string jsonData:(NSData *)jsonData{
    if (self.isSharing) {
        NSLog(@"注意：重复调用了startShareWithString，请检查代码");
        return;
    }
    self.isSharing = YES;
    
    self.shareMeshUUID = string;
    self.shareJsonData = jsonData;
    [self startScan];
}

/**
 结束分享
 */
- (void)stopShare{
    if (!self.isSharing) {
        NSLog(@"注意：重复调用了stopShare，请检查代码");
        return;
    }
    self.isSharing = NO;

    if (self.manager.isScanning) {
        [self.manager stopScan];
    }
    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(startScan) object:nil];
}

#pragma mark - 接收端

/**
 开始接收

 @param meshUUID 扫描二维码获取到的MeshUUID
 */
- (void)startReceiveWithMeshUUID:(NSString *)meshUUID{
    if (self.isReceiving) {
        NSLog(@"注意：重复调用了startReceive，请检查代码");
        return;
    }
    self.isReceiving = YES;
    self.receiveMeshUUID = meshUUID;
    [self startAdvertising:meshUUID];
}

/**
 结束接收
 */
- (void)stopReceive{
    if (!self.isReceiving) {
        NSLog(@"注意：重复调用了stopReceive，请检查代码");
        return;
    }
    self.isReceiving = NO;

    if (self.peripheralManager.isAdvertising) {
        [self.peripheralManager stopAdvertising];
    }
    self.receiveJsonData = [NSMutableData data];
}

@end
