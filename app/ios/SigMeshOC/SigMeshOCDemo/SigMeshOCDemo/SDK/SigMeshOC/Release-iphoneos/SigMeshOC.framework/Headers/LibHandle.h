/********************************************************************************************************
 * @file     LibHandle.h 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
//
//  LibHandle.h
//  SigMesh
//
//  Created by 石 on 2017/11/10.
//  Copyright © 2017年 Arvin.shi. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <CoreFoundation/CoreFoundation.h>
#import <CoreBluetooth/CoreBluetooth.h>

// base controller api
#import "sig_mesh_lib/SIG_MESH_header_file/ble_lt_mesh/proj_lib/sig_mesh/interface.h"
// scene api
#import "sig_mesh_lib/SIG_MESH_header_file/ble_lt_mesh/vendor/common/scene.h"
// time api
#import "sig_mesh_lib/SIG_MESH_header_file/ble_lt_mesh/vendor/common/time_model.h"
// HSL api
#import "sig_mesh_lib/SIG_MESH_header_file/ble_lt_mesh/vendor/common/lighting_model_HSL.h"
// scheduler api
#import "sig_mesh_lib/SIG_MESH_header_file/ble_lt_mesh/vendor/common/scheduler.h"
// remote add devices api
#import "sig_mesh_lib/SIG_MESH_header_file/ble_lt_mesh/vendor/common/remote_prov.h"
// fast provision api
#import "sig_mesh_lib/SIG_MESH_header_file/ble_lt_mesh/vendor/common/fast_provision_model.h"
// mesh ota api
#import "sig_mesh_lib/SIG_MESH_header_file/ble_lt_mesh/vendor/common/mesh_ota.h"
// SendOpParaDebug
#import "sig_mesh_lib/SIG_MESH_header_file/ble_lt_mesh/vendor/common/mesh_common.h"
#import "sig_mesh_lib/SIG_MESH_header_file/ble_lt_mesh/vendor/common/app_heartbeat.h"
// static oob
#import "sig_mesh_lib/SIG_MESH_header_file/ble_lt_mesh/proj_lib/mesh_crypto/le_crypto.h"

typedef void(^provisionResultCallBack)(BOOL isSuccess);
typedef void(^keybindResultCallBack)(BOOL isSuccess);


#pragma mark- Const string
/**
 *   when cmd need response ,and call back here
 */
UIKIT_EXTERN NSString *const NotifyReliableReportBack;

/**
 *   when cmd need response ,the other cmd will can't response
 */
UIKIT_EXTERN NSString *const NotifyCommandIsBusyOrNot;

/**
 *   notify useinfo key
 */
UIKIT_EXTERN NSString *const ReliableReportBackInfoOpcodeKey;
UIKIT_EXTERN NSString *const ReliableReportBackInfoRspCountKey;
UIKIT_EXTERN NSString *const ReliableReportBackInfoReqCountKey;

UIKIT_EXTERN NSString *const CommandIsBusyKey;


/**
 @param 16 groups count max, it's 8 currently
 */
#define SubGroupsCountMax (16)

/**
 @param 384 response data max lenght
 */
#define RspDataMaxLenght (384)

struct ProvisionInfo {
    Byte prov_newkey[16];
    Byte prov_key_index[2];
    Byte flag;
    Byte prov_iv_index[4];
    Byte prov_unicast_address[2];
};

#pragma mark- Model
/// the model of sig modelID controller response packet
@interface ResponseModel : NSObject
/// all packet data
@property (nonatomic, strong) NSData *rspData;
/// packet type
@property (nonatomic, assign) int type;
/// Effective length of response, the length of data in addition to type and length.
@property (nonatomic, assign) int length;
///The corresponding node address of response
@property (nonatomic, assign) int address;
/// location unicastAddress
@property (nonatomic, assign) int rspAddress;

/// Opcode, same opcode like Model.h line.70 Opcode.
@property (nonatomic, assign) int opcode;
///// OPcode
//@property (nonatomic, assign) int vendorOPCode;
/// Custom data of current response packet
@property (nonatomic, strong) NSData *customData;


- (instancetype)initWithResponseData:(NSData *)data;



@property (nonatomic, strong) NSMutableArray *clientSubGroups;
@property (nonatomic, strong) NSMutableArray *serverSubGroups;
@property (nonatomic, assign) int clientPubAddress;
@property (nonatomic, assign) int serverPubAddress;

@property (nonatomic, assign) BOOL isClient;

/// Current Status，on status or off status.
@property (nonatomic, assign) BOOL currentState;
/// Point status, on status or off status.
@property (nonatomic, assign) BOOL pointState;
/// Current value, brightness value or temprature value.
@property (nonatomic, assign) int currentValue;
/// Point value, eg: the node's brightness value is 50, app need set brightness to 100, The time of gradient is 200ms. The node will response one packet every 100ms, so app will get the packet's currentValue is 75 after 100ms and and will get the packet's currentValue is 100 after 200ms, but pointValue is 100 in 2 packet.
@property (nonatomic, assign) int pointValue;


/**
 update model sub list
 
 @param otherModel new data
 */

- (void)updateResponseSubscrbeList:(ResponseModel *)otherModel isClient:(bool)isClient;


/**
 update model publish address
 
 @param otherModel new data
 @param isClient client/server
 */

- (void)updateResponsePublishAddress:(ResponseModel *)otherModel isClient:(bool)isClient;

@end

/// the model of vendor modelID controller response packet
@interface VendorResponseModel : NSObject
/// all packet data
@property (nonatomic, strong) NSData *rspData;
/// packet type
@property (nonatomic, assign) int type;
/// Effective length of response, the length of data in addition to type and length.
@property (nonatomic, assign) int length;
/// address of node
@property (nonatomic, assign) int address;
/// location unicastAddress
@property (nonatomic, assign) int rspAddress;
/// OPcode
@property (nonatomic, assign) int vendorOPCode;
/// Custom data of current response packet
@property (nonatomic, strong) NSData *customData;

- (instancetype)initWithResponseData:(NSData *)data;

@end

#pragma mark- API of lib need

typedef void(^responseModelCallBack)(ResponseModel *m);
typedef void(^resultCallBack)(void);
typedef void(^addressCallBack)(NSNumber *address);

@interface LibHandle : NSObject

#pragma mark- tool method
/**
 SDK Lib need do like this

 @param data characteristic value
 */
void dealNotifyData(NSData *data);


/**
 provisionLocal
 
 @param netkeydata key value
 @param locationAddress defult 1,range1~0x7fff, All UInt16 that highest bit is 0.
 @param netkeyIndex netkey index defult 0
 @return index
 */
int provisionLocal(NSData *netkeydata,uint16_t locationAddress,u16 netkeyIndex);


/**
 provision
 
 @param netkeydata key value
 @param address address
 @param netkeyIndex netKey index
 */
void provision(NSData *netkeydata, uint16_t address,u16 netkeyIndex);

void saveDeviceInfoAfterProvisionSuccess(UInt16 address);

/**
 keyBind,setp after provision success
 
 @param address address
 @param appKey appKey
 @param appkeyIndex appKey index
 @param netkeyIndex netKey index
 @param isFastBind YES mians fast keyBind
 */
void doKeyBind(u16 address, NSData *appKey, u16 appkeyIndex ,u16 netkeyIndex ,u8 isFastBind);

/**
 refresh modelID for keyBind
 */
extern void refreshKeyBindModelID(void);

/**
 ota step need this method
 
 @param data data need written
 @param index index of data
 */
void sendData(NSData *data, int index);

void sendOTAEndData(NSData *data, int index);

void sendReadFirmwareVersion(void);

void sendStartOTA(void);

extern unsigned short libHandleCRC16 (unsigned char *pD, int len);

/// get remote uuids by send serarch, limit is response max of one node, and timeout is always 3s.
int send_rp_scan_start(u16 address,u8 limit,u8 timeout);

int init_json(void);

@end
