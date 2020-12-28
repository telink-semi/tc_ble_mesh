/********************************************************************************************************
 * @file     SigBluetooth.h
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
//  SigBluetooth.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/8/16.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol SigBluetoothDelegate <NSObject>
@optional
/// 需要过滤的设备，返回YES则过滤，返回NO则不过滤。
- (BOOL)needToBeFilteredNodeWithSigScanRspModel:(SigScanRspModel *)scanRspModel provisioned:(BOOL)provisioned peripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary<NSString *, id> *)advertisementData RSSI:(NSNumber *)RSSI;
@end


@interface SigBluetooth : NSObject
/// Default is NO.
@property (nonatomic,assign) BOOL waitScanRseponseEnabel;
/// new delagate function block since v3.2.3: all notify reporting by this block.
@property (nonatomic,copy,nullable) bleDidUpdateValueForCharacteristicCallback bluetoothDidUpdateValueCallback;
/// new delagate function block since v3.2.3: notify writeWithResponse by this block.
@property (nonatomic,copy,nullable) bleDidWriteValueForCharacteristicCallback bluetoothDidWriteValueCallback;
@property (nonatomic, weak) id <SigBluetoothDelegate>delegate;

+ (instancetype)new __attribute__((unavailable("please initialize by use .share or .share()")));
- (instancetype)init __attribute__((unavailable("please initialize by use .share or .share()")));


+ (SigBluetooth *)share;

#pragma  mark - Public


/// init system CBCentralManager, developer can scan CBPeripheral when CBCentralManager.state is CBCentralManagerStatePoweredOn.
/// @param result callback when CBCentralManager had inited.
- (void)bleInit:(bleInitSuccessCallback)result;
- (BOOL)isBLEInitFinish;

- (void)setBluetoothCentralUpdateStateCallback:(bleCentralUpdateStateCallback)bluetoothCentralUpdateStateCallback;

- (void)setBluetoothDisconnectCallback:(_Nullable bleDisconnectCallback)bluetoothDisconnectCallback;

- (void)scanUnprovisionedDevicesWithResult:(bleScanPeripheralCallback)result;

- (void)scanProvisionedDevicesWithResult:(bleScanPeripheralCallback)result;

/// 自定义扫描接口，checkNetworkEnable表示是否对已经入网的1828设备进行NetworkID过滤，过滤则只能扫描到当前手机的本地mesh数据里面的设备。
- (void)scanWithServiceUUIDs:(NSArray <CBUUID *>* _Nonnull)UUIDs checkNetworkEnable:(BOOL)checkNetworkEnable result:(bleScanPeripheralCallback)result;

- (void)setBluetoothIsReadyToSendWriteWithoutResponseCallback:(bleIsReadyToSendWriteWithoutResponseCallback)bluetoothIsReadyToSendWriteWithoutResponseCallback;

- (void)setBluetoothDidUpdateValueForCharacteristicCallback:(bleDidUpdateValueForCharacteristicCallback)bluetoothDidUpdateValueForCharacteristicCallback;

- (void)setBluetoothDidUpdateOnlineStatusValueCallback:(bleDidUpdateValueForCharacteristicCallback)bluetoothDidUpdateOnlineStatusValueCallback;

- (void)scanMeshNodeWithPeripheralUUID:(NSString *)peripheralUUID timeout:(NSTimeInterval)timeout resultBlock:(bleScanSpecialPeripheralCallback)block;

- (void)stopScan;

- (void)connectPeripheral:(CBPeripheral *)peripheral timeout:(NSTimeInterval)timeout resultBlock:(bleConnectPeripheralCallback)block;

- (void)discoverServicesOfPeripheral:(CBPeripheral *)peripheral timeout:(NSTimeInterval)timeout resultBlock:(bleDiscoverServicesCallback)block;

- (void)changeNotifyToState:(BOOL)state Peripheral:(CBPeripheral *)peripheral characteristic:(CBCharacteristic *)characteristic timeout:(NSTimeInterval)timeout resultBlock:(bleChangeNotifyCallback)block;

- (void)cancelAllConnecttionWithComplete:(bleCancelAllConnectCallback)complete;
- (void)cancelConnectionPeripheral:(CBPeripheral *)peripheral timeout:(NSTimeInterval)timeout resultBlock:(bleCancelConnectCallback)block;

- (void)readOTACharachteristicWithTimeout:(NSTimeInterval)timeout complete:(bleReadOTACharachteristicCallback)complete;
- (void)cancelReadOTACharachteristic;

- (CBPeripheral *)getPeripheralWithUUID:(NSString *)uuidString;

- (CBCharacteristic *)getCharacteristicWithUUIDString:(NSString *)uuid OfPeripheral:(CBPeripheral *)peripheral;

- (BOOL)isWorkNormal;

#pragma mark - new gatt api since v3.2.3
- (BOOL)readCharachteristic:(CBCharacteristic *)characteristic ofPeripheral:(CBPeripheral *)peripheral;
- (BOOL)writeValue:(NSData *)value toPeripheral:(CBPeripheral *)peripheral forCharacteristic:(CBCharacteristic *)characteristic type:(CBCharacteristicWriteType)type;

@end

NS_ASSUME_NONNULL_END
