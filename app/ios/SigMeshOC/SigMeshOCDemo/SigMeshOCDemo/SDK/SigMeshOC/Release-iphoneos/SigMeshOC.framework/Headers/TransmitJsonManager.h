/********************************************************************************************************
 * @file     TransmitJsonManager.h
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
//  TransmitJsonManager.h
//  SigMeshOC
//
//  Created by Liangjiazhi on 2018/11/22.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol TransmitJsonManagerDelegate <NSObject>
@optional

///回调分享json数据完成
- (void)onEndShare;

///回调获取到的json数据
- (void)onReceiveJsonData:(NSData *)data;

@end


/**
 TransmitJsonManager：蓝牙json数据分享使用的数据传输管理类。由于json数据比较大，不适合直接把json数据转成二维码，而是使用蓝牙的点对点数据传输的办法。
 
 代码逻辑：
 1.分享端取当前mesh的MeshUUID转化为标准的serviceUUID格式字符串，组成字典，再生成二维码，如：{"MeshUUID":"27C7B6D2-7447-AC0F-93EB-3CEA7C2B9FFB"}
 2.接收端扫描二维码，获取到MeshUUID，生成一个模拟蓝牙设备，并给模拟蓝牙设备添加固定的service和Characteristic用于接收json数据。之后模拟蓝牙设备开始广播蓝牙广播数据，将获取到的MeshUUID放入广播数据的ServiceUUID中，如：@{CBAdvertisementDataServiceUUIDsKey : @[[CBUUID UUIDWithString:string]]}
 3.分享端扫描周围的蓝牙设备，获取到广播包存在CBAdvertisementDataServiceUUIDsKey且值为MeshUUID的蓝牙设备。开始连接、扫描service、扫描Characteristic，存在传输json数据的Characteristic则开始发送json蓝牙数据包。数据包组成：1.start packet(2bytes:0x01FF)。2.json data packet 20bytes(2bytes:index,0x0000-0xFFFF + 16bytes:json data，最后一个json data packet可能不足16bytes + 2bytes:前18字节的crc检验值)。3.end packet(2bytes:0x02FF)。注意：1.json data packet长度由宏定义kPacketAllLength决定，包长度越长，传输使用时间越短。2.由于json data较为庞大，所有demo使用了zip进行了数据压缩。
 4.分享端发送json完成，主动断开连接。接收端收到end packet，停止广播数据并把完整的json数据返回给界面。
 */
@interface TransmitJsonManager : NSObject

@property (nonatomic, weak) id delegate;

+ (TransmitJsonManager *)share;

#pragma mark - 分享端
/**
 开始分享
 
 @param string MeshUUID转为serviceUUID格式的字符串，用于标记蓝牙模拟设备。
 @param jsonData json的NSData格式的数据。
 */
- (void)startShareWithString:(NSString *)string jsonData:(NSData *)jsonData;

/**
 结束分享
 */
- (void)stopShare;

#pragma mark - 接收端
/**
 开始接收
 
 @param meshUUID 扫描二维码获取到的MeshUUID
 */
- (void)startReceiveWithMeshUUID:(NSString *)meshUUID;

/**
 结束接收
 */
- (void)stopReceive;

@end

NS_ASSUME_NONNULL_END
