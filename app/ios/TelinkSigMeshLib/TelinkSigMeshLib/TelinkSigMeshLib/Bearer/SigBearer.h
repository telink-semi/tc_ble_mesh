/********************************************************************************************************
 * @file     SigBearer.h
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
//  SigBearer.h
//  SigMeshLib
//
//  Created by Liangjiazhi on 2019/8/23.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class SigBearer,SigPdu;

typedef void(^bearerOperationResultCallback)(BOOL successful);
typedef void(^bearerChangePeripheralCallback)(BOOL successful);
typedef void(^startMeshConnectResultBlock)(BOOL successful);
typedef void(^stopMeshConnectResultBlock)(BOOL successful);

/// The PDU Type identifies the type of the message.
/// Bearers may use this type to set the proper value in the
/// payload. For ADV beared it will be a proper AD Type (see Assigned
/// Numbers / Generic Access Profile), for GATT bearer the correct
/// Message type in the Proxy PDU.
///
/// Some message types are handled only by some bearers,
/// for example the provisioning PDU type must be sent using a
/// Provisioning Bearer (PB type of bearer).
typedef enum : UInt8 {
    /// The message is a Network PDU.
    /// See: Section 3.4.4 of Bluetooth Mesh Specification 1.0.1.
    SigPduType_networkPdu = 0,
    /// See: Section 3.9 of Bluetooth Mesh Specification 1.0.1.
    SigPduType_meshBeacon = 1,
    /// The message is a proxy configuration message.
    /// This message type may be used only for GATT Bearer.
    /// See: Section 6.5 of Bluetooth Mesh Specification 1.0.1.
    SigPduType_proxyConfiguration = 2,
    /// The message is a Provisioning PDU.
    /// This message type may be used only in Provisioning Bearers (PB).
    /// See: Section 5.4.1 of Bluetooth Mesh Specification 1.0.1.
    SigPduType_provisioningPdu = 3,
} SigPduType;

struct PduTypes {
    union{
        UInt8 value;
        struct{
            /// Static OOB Information is available.
            UInt8 networkPdu      :1;//value的低1个bit
            UInt8 meshBeacon :1;//val的低位第2个bit
            UInt8 proxyConfiguration :1;//val的低位第3个bit
            UInt8 provisioningPdu :1;//val的低位第4个bit
        };
    };
};

@interface SigPudModel : NSObject
@property (nonatomic,strong) NSData *pduData;
@property (nonatomic,assign) SigPduType pduType;
@end


@interface SigTransmitter : NSObject
/// This method sends the given data over the bearer.
/// Data longer than MTU will automatically be segmented
/// using the bearer protocol if bearer implements segmentation.
///
/// - parameter data: The data to be sent over the Bearer.
/// - parameter type: The PDU type.
/// - throws: This method throws an error if the PDU type
///           is not supported, or data could not be sent for
///           some other reason.
- (void)sendData:(NSData *)data ofType:(SigPduType)type;
@end


@protocol SigBearerDelegate <NSObject>
@optional

/// Callback called when a packet has been received using the SigBearer. Data longer than MTU will automatically be reassembled using the bearer protocol if bearer implements segmentation.
/// @param bearer The SigBearer on which the data were received.
/// @param data The data received.
/// @param type The type of the received data.
- (void)bearer:(SigBearer *)bearer didDeliverData:(NSData *)data ofType:(SigPduType)type;

@end


@protocol SigBearerDataDelegate <NSObject>
@optional

/// Callback called when the Bearer is ready for use.
/// @param bearer The Bearer.
- (void)bearerDidOpen:(SigBearer *)bearer;

/// Callback called when the Bearer is no longer open.
/// @param bearer The Bearer.
/// @param error The reason of closing the Bearer, or `nil` if closing was intended.
- (void)bearer:(SigBearer *)bearer didCloseWithError:(NSError *)error;

@end

typedef void(^SendPacketsFinishCallback)(void);

@interface SigBearer : SigTransmitter
@property (nonatomic, weak) id <SigBearerDelegate>delegate;
@property (nonatomic, weak) id <SigBearerDataDelegate>dataDelegate;
@property (nonatomic, assign) BOOL isAutoReconnect;//标记是否自动重连（ota和添加流程不需要自动重连）
@property (nonatomic, copy) SendPacketsFinishCallback sendPacketFinishBlock;

- (BOOL)isOpen;
- (BOOL)isProvisioned;

#pragma  mark - Public API

+ (SigBearer *)share;

- (void)changePeripheral:(CBPeripheral *)peripheral result:(bearerChangePeripheralCallback)block;

- (void)changePeripheralIdentifierUuid:(NSString *)uuid result:(bearerChangePeripheralCallback)block;

- (CBPeripheral *)getCurrentPeripheral;

/// This method opens the Bearer.
- (void)openWithResult:(bearerOperationResultCallback)block;

/// This method closes the Bearer.
- (void)closeWithResult:(bearerOperationResultCallback)block;

- (void)connectAndReadServicesWithPeripheral:(CBPeripheral *)peripheral result:(bearerOperationResultCallback)result;

- (void)sentPcakets:(NSArray <NSData *>*)packets toCharacteristic:(CBCharacteristic *)characteristic type:(CBCharacteristicWriteType)type complete:(SendPacketsFinishCallback)complete;
- (void)sentPcakets:(NSArray <NSData *>*)packets toCharacteristic:(CBCharacteristic *)characteristic type:(CBCharacteristicWriteType)type;

- (void)sendBlePdu:(SigPdu *)pdu ofType:(SigPduType)type;

- (void)sendOTAData:(NSData *)data;

- (void)setBearerProvisioned:(BOOL)provisioned;

/// 开始连接SigDataSource这个单列的mesh网络。
- (void)startMeshConnectWithTimeOut:(NSTimeInterval)timeout complete:(nullable startMeshConnectResultBlock)complete;

/// 断开一个mesh网络的连接，切换不同的mesh网络时使用。
- (void)stopMeshConnectWithComplete:(nullable stopMeshConnectResultBlock)complete;

@end

NS_ASSUME_NONNULL_END
