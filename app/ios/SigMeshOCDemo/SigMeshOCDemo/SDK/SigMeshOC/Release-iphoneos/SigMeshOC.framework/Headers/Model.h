/********************************************************************************************************
 * @file     Model.h 
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
//  Model.h
//  SigMeshOC
//
//  Created by Liangjiazhi on 2018/7/12.
//  Copyright © 2018年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "LibHandle.h"

@class SIGModel,BLEStore,SigNodeModel;
typedef void(^BeaconBackCallBack)(BOOL available);

///Agreement document for reference, “Mesh_Model_Specification v1.0.pdf”，page298.
typedef enum : NSUInteger {
    OpcodeOnOffStatusResponse = 0x0482,
    OpcodeCurrentCTLResponse = 0x6082,
    OpcodeCTLTemperatureStatusResponse = 0x6682,
    OpcodeHSLNotifyResponse = 0x7882,//publish modelID is SIG_MD_LIGHT_HSL_S，notify callback OPCode
    OpcodeSubListGetResponse = 0x2A80,
    
    OpcodeBrightnessGetResponse = 0x4E82,
    OpcodeChangeLevelResponse = 0x0882,
    
    // eg:<f00d0140 0040801f 0001401f c00010>
    OpcodeEditSubListResponse = 0x1F80,//Agreement document for reference, Mesh_v1.0.pdf, 4.3.2.26, Config Model Subscription Status
    OpcodePublishAddressResponse = 0x1980,
    
    OpcodeOnOffG = 0x0182,
    OpcodeOnOffS = 0x0282,
    
    OpcodeSubscriptorAddRequest = 0x1B80,
    OpcodeSubscriptorDelRequest = 0x1C80,
    
    OpcodeDelectDeviceResponse = 0x4A80,

    OpcodeGetSchedulerStatusResponse = 0x4A82,

    //add address:2 to sceneID:3<f00f0200 01008245 00030001 00020003 00>
    //del address:2<f00d0200 01008245 00020001 000200>
    //add scene and delete scene is same OPCode.
    OpcodeSceneRegisterStatusResponse = 0x4582,
    
    OpcodeSetTimeResponse = 0x5D,
    OpcodeRecallSceneResponse = 0x5E,
    OpcodeSetSchedulerResponse = 0x5F,
    
    OpcodeSetUUIDResponse = 0x5B80,//remote add, the success opcode of api setRemoteAddDeviceInfoWithUnicastAddress
    
    OpcodeGetFwInfoResponse = 0x02B6,
    
} Opcode;

typedef enum : NSUInteger {
    KeyBindTpye_Normal,//普通添加模式
    KeyBindTpye_Quick,//快速添加模式
    KeyBindTpye_Reserved,//预留
} KeyBindTpye;

typedef enum : NSUInteger {
    DeviceStateOn,
    DeviceStateOff,
    DeviceStateOutOfLine,
} DeviceState;//设备状态

typedef enum : NSUInteger {
    SchedulerTypeOff = 0x0,
    SchedulerTypeOn = 0x1,
    SchedulerTypeScene = 0x2,
    SchedulerTypeNoAction = 0xF,
} SchedulerType;//闹钟类型

typedef enum : NSUInteger {
    AddDeviceModelStateBinding,
    AddDeviceModelStateBindSuccess,
    AddDeviceModelStateBindFail,
} AddDeviceModelState;//添加的设备的状态

typedef enum : UInt16 {
    SigNodePID_CT = 1,
    SigNodePID_Panel = 7,
} SigNodePID;

@interface Model : NSObject

@end


@interface SIGModel : Model
@property (nonatomic,strong) ResponseModel *currentModel;
@property (nonatomic,strong) NSMutableArray <ResponseModel *>*responseModels;
@end


@interface ModelIDModel : Model
@property (nonatomic,strong) NSString *modelGroup;
@property (nonatomic,strong) NSString *modelName;
@property (nonatomic,assign) NSInteger sigModelID;

-(instancetype)initWithModelGroup:(NSString *)modelGroup modelName:(NSString *)modelName sigModelID:(NSInteger)sigModelID;

@end


@interface ModelIDs : Model
@property (nonatomic,assign) NSInteger modelIDCount;
@property (nonatomic,strong) ModelIDModel *model1;
@property (nonatomic,strong) ModelIDModel *model2;
@property (nonatomic,strong) ModelIDModel *model3;
@property (nonatomic,strong) ModelIDModel *model4;
@property (nonatomic,strong) ModelIDModel *model5;
@property (nonatomic,strong) ModelIDModel *model6;
@property (nonatomic,strong) ModelIDModel *model7;
@property (nonatomic,strong) ModelIDModel *model8;
@property (nonatomic,strong) ModelIDModel *model9;
@property (nonatomic,strong) ModelIDModel *model10;
@property (nonatomic,strong) ModelIDModel *model11;
@property (nonatomic,strong) ModelIDModel *model12;
@property (nonatomic,strong) ModelIDModel *model13;
@property (nonatomic,strong) ModelIDModel *model14;
@property (nonatomic,strong) ModelIDModel *model15;
@property (nonatomic,strong) ModelIDModel *model16;
@property (nonatomic,strong) ModelIDModel *model17;
@property (nonatomic,strong) ModelIDModel *model18;
@property (nonatomic,strong) ModelIDModel *model19;
@property (nonatomic,strong) ModelIDModel *model20;
@property (nonatomic,strong) ModelIDModel *model21;
@property (nonatomic,strong) ModelIDModel *model22;
@property (nonatomic,strong) ModelIDModel *model23;
@property (nonatomic,strong) ModelIDModel *model24;
@property (nonatomic,strong) ModelIDModel *model25;
@property (nonatomic,strong) ModelIDModel *model26;
@property (nonatomic,strong) ModelIDModel *model27;
@property (nonatomic,strong) ModelIDModel *model28;
@property (nonatomic,strong) ModelIDModel *model29;
@property (nonatomic,strong) ModelIDModel *model30;
@property (nonatomic,strong) ModelIDModel *model31;
@property (nonatomic,strong) ModelIDModel *model32;
@property (nonatomic,strong) ModelIDModel *model33;
@property (nonatomic,strong) ModelIDModel *model34;
@property (nonatomic,strong) ModelIDModel *model35;
@property (nonatomic,strong) ModelIDModel *model36;
@property (nonatomic,strong) ModelIDModel *model37;
@property (nonatomic,strong) ModelIDModel *model38;
@property (nonatomic,strong) ModelIDModel *model39;
@property (nonatomic,strong) ModelIDModel *model40;
@property (nonatomic,strong) ModelIDModel *model41;
@property (nonatomic,strong) ModelIDModel *model42;
@property (nonatomic,strong) ModelIDModel *model43;
@property (nonatomic,strong) ModelIDModel *model44;
@property (nonatomic,strong) ModelIDModel *model45;
@property (nonatomic,strong) ModelIDModel *model46;
@property (nonatomic,strong) ModelIDModel *model47;
@property (nonatomic,strong) ModelIDModel *model48;
@property (nonatomic,strong) ModelIDModel *model49;
@property (nonatomic,strong) ModelIDModel *model50;
@property (nonatomic,strong) ModelIDModel *model51;
@property (nonatomic,strong) ModelIDModel *model52;
@property (nonatomic,strong) NSArray <ModelIDModel *>*modelIDs;//所有的modelID
@property (nonatomic,strong) NSArray <ModelIDModel *>*defaultModelIDs;//默认keyBind的modelID
@end


@interface Groups : Model
@property (nonatomic,assign) NSInteger groupCount;
@property (nonatomic,strong) NSString *name1;
@property (nonatomic,strong) NSString *name2;
@property (nonatomic,strong) NSString *name3;
@property (nonatomic,strong) NSString *name4;
@property (nonatomic,strong) NSString *name5;
@property (nonatomic,strong) NSString *name6;
@property (nonatomic,strong) NSString *name7;
@property (nonatomic,strong) NSString *name8;
@property (nonatomic,strong) NSArray <NSString *>*names;
@end


@interface SchedulerModel : Model<NSCopying>
@property (nonatomic,assign) UInt8 schedulerID;//range: 0x0~0xF.
@property (nonatomic,assign) UInt64 valid_flag_or_idx;
@property (nonatomic,assign) UInt64 year;
@property (nonatomic,assign) UInt64 month;
@property (nonatomic,assign) UInt64 day;
@property (nonatomic,assign) UInt64 hour;
@property (nonatomic,assign) UInt64 minute;
@property (nonatomic,assign) UInt64 second;
@property (nonatomic,assign) UInt64 week;
@property (nonatomic,assign) SchedulerType action;
@property (nonatomic,assign) UInt64 transitionTime;
@property (nonatomic,assign) UInt64 schedulerData;
@property (nonatomic,assign) UInt64 sceneId;
- (NSDictionary *)getDictionaryOfSchedulerModel;
- (void)setDictionaryToSchedulerModel:(NSDictionary *)dictionary;
@end


//缓存蓝牙扫描回调的模型
@interface SigScanRspModel : NSObject
@property (nonatomic, strong) NSString *macAddress;
@property (nonatomic, assign) UInt16 CID;//企业ID，默认为0x0211，十进制为529.
@property (nonatomic, assign) UInt16 PID;//产品ID，CT灯为1，面板panel为7.

@property (nonatomic, strong) NSString *uuid;
@property (nonatomic, assign) UInt16 address;
@property (nonatomic, strong) NSData *nodeIdentityData;//byte[0]:type=0x01,byte[1~17]:data
@property (nonatomic, strong) NSData *networkIDData;//byte[0]:type=0x00,byte[1~9]:data

@property (nonatomic, strong) NSDictionary<NSString *,id> *advertisementData;//扫描到的蓝牙设备广播包完整数据

- (instancetype)initWithPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary<NSString *,id> *)advertisementData;
@end


//缓存Remot add扫描回调的模型
@interface SigRemoteScanRspModel : NSObject
@property (nonatomic, assign) UInt16 unicastAddress;
@property (nonatomic, strong) NSData *uuid;
@property (nonatomic, assign) int RSSI;//负值
@property (nonatomic, assign) UInt16 oob;
@property (nonatomic, strong) NSString *macAddress;
- (instancetype)initWithPar:(u8 *)par len:(u8)len;
@end


@interface AddDeviceModel : Model
@property (nonatomic, strong) SigScanRspModel *scanRspModel;
@property (nonatomic, assign) AddDeviceModelState state;
@end

@interface PublishResponseModel : NSObject
@property (nonatomic, assign) UInt8 status;
@property (nonatomic, assign) UInt16 elementAddress;
@property (nonatomic, assign) UInt16 publishAddress;
@property (nonatomic, assign) UInt16 appKeyIndex;
@property (nonatomic, assign) UInt8 credentialFlag;
@property (nonatomic, assign) UInt8 RFU;
@property (nonatomic, assign) UInt8 publishTTL;
@property (nonatomic, assign) UInt8 publishPeriod;
@property (nonatomic, assign) UInt8 publishRetransmitCount;
@property (nonatomic, assign) UInt8 publishRetransmitIntervalSteps;
@property (nonatomic, assign) BOOL isVendorModelID;
@property (nonatomic, assign) UInt32 modelIdentifier;

- (instancetype)initWithResponseData:(NSData *)rspData;

@end

@interface ActionModel : Model
@property (nonatomic,assign) UInt16 address;
@property (nonatomic,assign) DeviceState state;
@property (nonatomic,assign) UInt8 trueBrightness;//1-100
@property (nonatomic,assign) UInt8 trueTemperature;//0-100
- (instancetype)initWithNode:(SigNodeModel *)node;
- (BOOL)isSameActionWithAction:(ActionModel *)action;
- (NSDictionary *)getDictionaryOfActionModel;
- (void)setDictionaryToActionModel:(NSDictionary *)dictionary;
@end

@interface BLEStore : SIGModel

@property (nonatomic,strong) NSMutableArray <CBPeripheral *>*scanedPeripherals;
@property (nonatomic,strong) CBPeripheral *peripheral; //当前直连的设备
@property (nonatomic,strong) CBCharacteristic *OTACharacteristic;
@property (nonatomic,strong) CBCharacteristic *PBGATT_OutCharacteristic;
@property (nonatomic,strong) CBCharacteristic *PBGATT_InCharacteristic;
@property (nonatomic,strong) CBCharacteristic *PROXY_OutCharacteristic;
@property (nonatomic,strong) CBCharacteristic *PROXY_InCharacteristic;
@property (nonatomic,strong) CBCharacteristic *OnlineStatusCharacteristic;//私有定制，上报节点的状态的特征
@property (nonatomic,strong) CBCharacteristic *MeshOTACharacteristic;
@property (nonatomic,copy) BeaconBackCallBack beaconBackCallBack;

- (BOOL)isWorkNormal;

- (void)saveToLocal:(CBPeripheral *)tempPeripheral;

- (void)addRSSIWithPeripheral:(CBPeripheral *)peripheral RSSI:(int)rssi;

- (void)clearRecord;

- (CBPeripheral *)hightestRSSI;

- (void)ressetParameters;

- (void)libBeaconBack:(BOOL)available;

- (void)writeForPROXYIn:(NSData *)d;

- (void)writeForPBGATTIn:(NSData *)d;

- (void)writeForOnlineStatus:(NSData *)d;

- (void)writeOTAData:(NSData *)d;

- (void)writeMeshOTAData:(NSData *)d;

- (void)readOTACharachteristicForContinue;

@end

@interface RangeModel : Model

@property (nonatomic, strong) NSString *lowAddress;

@property (nonatomic, strong) NSString *highAddress;

@end


@interface DeviceTypeModel : Model

@property (nonatomic, assign) UInt16 CID;

@property (nonatomic, assign) SigNodePID PID;

@property (nonatomic, assign) VC_node_info_t defultNodeInfo;

- (instancetype)initWithCID:(UInt16)cid PID:(SigNodePID)pid;
- (void)setDefultNodeInfoData:(NSData *)data;

@end
