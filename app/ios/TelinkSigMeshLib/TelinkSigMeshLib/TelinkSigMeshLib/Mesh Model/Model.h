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
//  SigMeshLib
//
//  Created by Liangjiazhi on 2019/8/15.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>
//#import "LibHandle.h"

@class BLEStore,SigNodeModel,SigMeshMessage,SigNetkeyModel,SigAppkeyModel,SigIvIndex;
typedef void(^BeaconBackCallBack)(BOOL available);
typedef void(^responseAllMessageBlock)(UInt16 source,UInt16 destination,SigMeshMessage *responseMessage);

typedef enum : NSUInteger {
    MeshAddress_unassignedAddress = 0x0000,
    MeshAddress_minUnicastAddress = 0x0001,
    MeshAddress_maxUnicastAddress = 0x7FFF,
    MeshAddress_minVirtualAddress = 0x8000,
    MeshAddress_maxVirtualAddress = 0xBFFF,
    MeshAddress_minGroupAddress   = 0xC000,
    MeshAddress_maxGroupAddress   = 0xFEFF,
    MeshAddress_allProxies        = 0xFFFC,
    MeshAddress_allFriends        = 0xFFFD,
    MeshAddress_allRelays         = 0xFFFE,
    MeshAddress_allNodes          = 0xFFFF,
} MeshAddress;

#define ELE_LIGHT_MODEL_SIZE  (380-12)
typedef struct{
    UInt8 nums;
    UInt8 numv;
    UInt8 id[ELE_LIGHT_MODEL_SIZE];
}mesh_element_model_id;
typedef struct{
    UInt16 relay       :1;
    UInt16 proxy       :1;
    UInt16 frid          :1;
    UInt16 low_power   :1;
    UInt16 rfu         :12;
}mesh_page_feature_t;
typedef struct{
    UInt16 cid;
    UInt16 pid;
    UInt16 vid;
    UInt16 crpl;
    mesh_page_feature_t feature;
}mesh_page0_head_t;
typedef struct{
    UInt16 len_cps;
#if 1   // must same with page0_local_t from start to numv
    mesh_page0_head_t page0_head;
    UInt16 local;
    mesh_element_model_id model_id;
#endif
}VC_node_cps_t;
typedef struct{
    UInt16 node_adr;    // primary address
    UInt8 element_cnt;
    UInt8 rsv;
    UInt8 dev_key[16];
    VC_node_cps_t cps;
}VC_node_info_t;    // size is 404(0x194)
typedef struct{
    SInt8 rssi;
    UInt8 uuid[16];
    UInt16 oob;
}remote_prov_scan_report;
typedef struct{
    UInt16 unicast;
    remote_prov_scan_report scan_report;
}remote_prov_scan_report_win32;


/// Information that points to out-of-band (OOB) information
/// needed for provisioning.
typedef struct{
    union{
        UInt16 value;
        struct{
            UInt8 other      :1;//value的低1个bit
            UInt8 electornicURI :1;//val的低位第2个bit
            UInt8 qrCode :1;//val的低位第3个bit
            UInt8 barCode :1;//val的低位第4个bit
            UInt8 nfc :1;//val的低位第5个bit
            UInt8 number :1;//val的低位第6个bit
            UInt8 string :1;//val的低位第7个bit
            // Bits 8-11 are reserved for future use.
            UInt8 onBox :1;//val的低位第12个bit
            UInt8 insideBox :1;//val的低位第13个bit
            UInt8 onPieceOfPaper :1;//val的低位第14个bit
            UInt8 insideManual :1;//val的低位第15个bit
            UInt8 onDevice :1;//val的低位第16个bit
        };
    };
}OobInformation;

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
    AddDeviceModelStateProvisionFail,
    AddDeviceModelStateBinding,
    AddDeviceModelStateBindSuccess,
    AddDeviceModelStateBindFail,
    AddDeviceModelStateScaned,
    AddDeviceModelStateProvisioning,
} AddDeviceModelState;//添加的设备的状态

@interface Model : NSObject

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
@property (nonatomic,assign) UInt64 schedulerID;//4 bits, Enumerates (selects) a Schedule Register entry. The valid values for the Index field are 0x0-0xF.
//@property (nonatomic,assign) UInt64 valid_flag_or_idx;
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


/// 缓存蓝牙扫描回调的模型，uuid(peripheral.identifier.UUIDString)为唯一标识符。
@interface SigScanRspModel : NSObject
@property (nonatomic, strong) NSData *nodeIdentityData;//byte[0]:type=0x01,byte[1~17]:data
@property (nonatomic, strong) NSData *networkIDData;//byte[0]:type=0x00,byte[1~9]:data
@property (nonatomic, strong) NSString *macAddress;
@property (nonatomic, assign) UInt16 CID;//企业ID，默认为0x0211，十进制为529.
@property (nonatomic, assign) UInt16 PID;//产品ID，CT灯为1，面板panel为7.
@property (nonatomic, strong) NSString *uuid;
@property (nonatomic, assign) UInt16 address;
@property (nonatomic, strong) NSString *advName;//广播包中的CBAdvertisementDataLocalNameKey
@property (nonatomic, strong) NSString *advUuid;//未添加的设备广播包中的CBAdvertisementDataServiceDataKey中的UUID（bytes:0-15），cid和pid为其前四个字节
@property (nonatomic, assign) OobInformation advOobInformation;//未添加的设备广播包中的CBAdvertisementDataServiceDataKey中的oob信息（bytes:16-17）
@property (nonatomic, strong) NSDictionary<NSString *,id> *advertisementData;//扫描到的蓝牙设备广播包完整数据
@property (nonatomic, assign) BOOL provisioned;//YES表示已经入网。

- (instancetype)initWithPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary<NSString *,id> *)advertisementData;
@end


//缓存Remot add扫描回调的模型
@interface SigRemoteScanRspModel : NSObject
@property (nonatomic, assign) UInt16 reportNodeAddress;
@property (nonatomic, strong) NSData *reportNodeUUID;
@property (nonatomic, assign) int RSSI;//负值
@property (nonatomic, assign) OobInformation oob;
@property (nonatomic, strong) NSString *macAddress;
- (instancetype)initWithParameters:(NSData *)parameters;
//- (instancetype)initWithPar:(UInt8 *)par len:(UInt8)len;
@end


@interface AddDeviceModel : Model
@property (nonatomic, strong) SigScanRspModel *scanRspModel;
@property (nonatomic, assign) AddDeviceModelState state;
- (instancetype)initWithRemoteScanRspModel:(SigRemoteScanRspModel *)scanRemoteModel;
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

@interface BLEStore : Model

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


static Byte CTByte[] = {(Byte) 0x11, (Byte) 0x02, (Byte) 0x01, (Byte) 0x00, (Byte) 0x32, (Byte) 0x37, (Byte) 0x69, (Byte) 0x00, (Byte) 0x07, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x19, (Byte) 0x01, (Byte) 0x00, (Byte) 0x00, (Byte) 0x02, (Byte) 0x00, (Byte) 0x03, (Byte) 0x00, (Byte) 0x04, (Byte) 0x00, (Byte) 0x05, (Byte) 0x00, (Byte) 0x00, (Byte) 0xfe, (Byte) 0x01, (Byte) 0xfe, (Byte) 0x02, (Byte) 0xfe, (Byte) 0x00, (Byte) 0xff, (Byte) 0x01, (Byte) 0xff, (Byte) 0x00, (Byte) 0x12, (Byte) 0x01, (Byte) 0x12, (Byte) 0x00, (Byte) 0x10, (Byte) 0x02, (Byte) 0x10, (Byte) 0x04, (Byte) 0x10, (Byte) 0x06, (Byte) 0x10, (Byte) 0x07, (Byte) 0x10, (Byte) 0x03, (Byte) 0x12, (Byte) 0x04, (Byte) 0x12, (Byte) 0x06, (Byte) 0x12, (Byte) 0x07, (Byte) 0x12, (Byte) 0x00, (Byte) 0x13, (Byte) 0x01, (Byte) 0x13, (Byte) 0x03, (Byte) 0x13, (Byte) 0x04, (Byte) 0x13, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x02, (Byte) 0x00, (Byte) 0x02, (Byte) 0x10, (Byte) 0x06, (Byte) 0x13};
static Byte PanelByte[] = {(Byte) 0x11, (Byte) 0x02, (Byte) 0x07, (Byte) 0x00, (Byte) 0x32, (Byte) 0x37, (Byte) 0x69, (Byte) 0x00, (Byte) 0x07, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00, (Byte) 0x02, (Byte) 0x00, (Byte) 0x03, (Byte) 0x00, (Byte) 0x04, (Byte) 0x00, (Byte) 0x05, (Byte) 0x00, (Byte) 0x00, (Byte) 0xfe, (Byte) 0x01, (Byte) 0xfe, (Byte) 0x02, (Byte) 0xfe, (Byte) 0x00, (Byte) 0xff, (Byte) 0x01, (Byte) 0xff, (Byte) 0x00, (Byte) 0x12, (Byte) 0x01, (Byte) 0x12, (Byte) 0x00, (Byte) 0x10, (Byte) 0x03, (Byte) 0x12, (Byte) 0x04, (Byte) 0x12, (Byte) 0x06, (Byte) 0x12, (Byte) 0x07, (Byte) 0x12, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00, (Byte) 0x11, (Byte) 0x02, (Byte) 0x01, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x05, (Byte) 0x01, (Byte) 0x00, (Byte) 0x10, (Byte) 0x03, (Byte) 0x12, (Byte) 0x04, (Byte) 0x12, (Byte) 0x06, (Byte) 0x12, (Byte) 0x07, (Byte) 0x12, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x05, (Byte) 0x01, (Byte) 0x00, (Byte) 0x10, (Byte) 0x03, (Byte) 0x12, (Byte) 0x04, (Byte) 0x12, (Byte) 0x06, (Byte) 0x12, (Byte) 0x07, (Byte) 0x12, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00};

@interface DeviceTypeModel : Model
@property (nonatomic, assign) UInt16 CID;

@property (nonatomic, assign) UInt16 PID;

@property (nonatomic, strong) NSData *defaultCompositionData;

@property (nonatomic, assign) VC_node_info_t defultNodeInfo;

- (instancetype)initWithCID:(UInt16)cid PID:(UInt16)pid;

@end


@interface SigAddConfigModel : Model
@property (nonatomic, strong) CBPeripheral *peripheral;
@property (nonatomic, assign) UInt16 unicastAddress;
@property (nonatomic, strong) NSData *networkKey;
@property (nonatomic, assign) UInt16 netkeyIndex;
@property (nonatomic, strong) NSData *appKey;
@property (nonatomic, assign) UInt16 appkeyIndex;
@property (nonatomic, assign) ProvisionTpye provisionType;
@property (nonatomic, strong) NSData *staticOOBData;
@property (nonatomic, assign) KeyBindTpye keyBindType;
@property (nonatomic, assign) UInt16 productID;
@property (nonatomic, strong) NSData *cpsData;

- (instancetype)initWithCBPeripheral:(CBPeripheral *)peripheral unicastAddress:(UInt16)unicastAddress networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appKey:(NSData *)appkey appkeyIndex:(UInt16)appkeyIndex provisionType:(ProvisionTpye)provisionType staticOOBData:(NSData *)staticOOBData keyBindType:(KeyBindTpye)keyBindType productID:(UInt16)productID cpsData:(NSData *)cpsData;

@end


#define OP_TYPE_SIG1         1
#define OP_TYPE_SIG2         2
#define OP_TYPE_VENDOR         3
#define BIT(n)                          ( 1<<(n) )
#define SIZE_OF_OP(op)    ((op & BIT(7)) ? ((op & BIT(6)) ? OP_TYPE_VENDOR : OP_TYPE_SIG2) : OP_TYPE_SIG1)
#define GET_OP_TYPE(op)    (SIZE_OF_OP(op))
#define IS_VENDOR_OP(op)    (GET_OP_TYPE(op) == OP_TYPE_VENDOR)

/// sig model struct: mesh_bulk_cmd_par_t, vendor model struct: mesh_vendor_par_ini_t. It is sig model command when vendorId is 0, and It is vendor model command when vendorId isn't 0. sig model config: netkeyIndex, appkeyIndex, retryCount, responseMax, address, opcode, commandData.
@interface IniCommandModel : Model
@property (nonatomic, assign) UInt16 netkeyIndex;
@property (nonatomic, assign) UInt16 appkeyIndex;
@property (nonatomic, assign) UInt8 retryCount;// only for reliable command
@property (nonatomic, assign) UInt8 responseMax;// only for reliable command
@property (nonatomic, assign) UInt16 address;
@property (nonatomic, assign) UInt16 opcode;// SigGenericOnOffSet:0x8202. SigGenericOnOffSetUnacknowledged:0x8203. VendorOnOffSet:0xC4, VendorOnOffSetUnacknowledged:0xC3.
@property (nonatomic, assign) UInt16 vendorId;// 0 means sig model command, other means vendor model command.
@property (nonatomic, assign) UInt8 responseOpcode;// response of VendorOnOffSet:0xC4.
@property (nonatomic, assign) BOOL needTid;
@property (nonatomic, assign) UInt8 tid;
@property (nonatomic, strong) NSData *commandData;//max length is MESH_CMD_ACCESS_LEN_MAX. SigGenericOnOffSet: commandData of turn on without TransitionTime and delay is {0x01,0x00,0x00}. commandData of turn off without TransitionTime and delay is {0x00,0x00,0x00}

@property (nonatomic, copy) responseAllMessageBlock responseCallBack;
@property (nonatomic, assign) BOOL hasReceiveResponse;

@property (nonatomic,strong) SigNetkeyModel *netkeyA;
@property (nonatomic,strong) SigAppkeyModel *appkeyA;
@property (nonatomic,strong) SigIvIndex *ivIndexA;
@property (nonatomic,assign) NSTimeInterval timeout;

/// create sig model ini command
- (instancetype)initSigModelIniCommandWithNetkeyIndex:(UInt16)netkeyIndex appkeyIndex:(UInt16)appkeyIndex retryCount:(UInt8)retryCount responseMax:(UInt8)responseMax address:(UInt16)address opcode:(UInt16)opcode commandData:(NSData *)commandData;
/// create vebdor model ini command
- (instancetype)initVendorModelIniCommandWithNetkeyIndex:(UInt16)netkeyIndex appkeyIndex:(UInt16)appkeyIndex retryCount:(UInt8)retryCount responseMax:(UInt8)responseMax address:(UInt16)address opcode:(UInt8)opcode vendorId:(UInt16)vendorId responseOpcode:(UInt8)responseOpcode needTid:(BOOL)needTid tid:(UInt8)tid commandData:(NSData *)commandData;

/// create model by ini data
/// @param iniCommandData ini data, eg: "a3ff000000000200ffffc21102c4020100"
- (instancetype)initWithIniCommandData:(NSData *)iniCommandData;

@end


@interface SigNodeUpdateStatusModel : Model
@property (nonatomic, assign) UInt16 address;
@property (nonatomic, assign) SigUpdateStatusType status;
- (instancetype)initWithAddress:(UInt16)address status:(SigUpdateStatusType)status;
@end

/// 8.4.1.2 Firmware Update Information Status
/// - seeAlso: MshMDL_DFU_MBT_CR_R04_LbL25.pdf  (page.81)
@interface SigFirmwareInformationEntryModel : Model
/// Length of the Current Firmware ID field.
@property (nonatomic,assign) UInt8 currentFirmwareIDLength;
/// Identifies the firmware image on the node or any subsystem on the node. Size is variable.
@property (nonatomic,strong) NSData *currentFirmwareID;
/// Length of the Update URI field.
@property (nonatomic,assign) UInt8 updateURILength;
/// URI used to retrieve a new firmware image. Size is 1 ~ 255. (optional)
@property (nonatomic,strong) NSData *updateURL;
@property (nonatomic,strong) NSData *parameters;
- (instancetype)initWithParameters:(NSData *)parameters;
@end
