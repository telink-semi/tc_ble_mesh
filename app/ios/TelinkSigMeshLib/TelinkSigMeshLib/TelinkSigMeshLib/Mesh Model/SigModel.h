/********************************************************************************************************
 * @file     SigModel.h
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
//  SigModel.h
//  TelinkSigMeshLib
//
//  Created by 梁家誌 on 2019/8/15.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import <Foundation/Foundation.h>
NS_ASSUME_NONNULL_BEGIN

@class SigNetkeyDerivaties,OpenSSLHelper,SigRangeModel,SigSceneRangeModel,SigNodeFeatures,SigRelayretransmitModel,SigNetworktransmitModel,SigElementModel,SigNodeKeyModel,SigModelIDModel,SigRetransmitModel,SigPeriodModel,SigHeartbeatPubModel,SigHeartbeatSubModel,SigBaseMeshMessage,SigConfigNetworkTransmitSet,SigConfigNetworkTransmitStatus,SigPublishModel   ,SigNodeModel,SigMeshMessage,SigNetkeyModel,SigAppkeyModel,SigIvIndex,SigPage0;
typedef void(^BeaconBackCallBack)(BOOL available);
typedef void(^responseAllMessageBlock)(UInt16 source,UInt16 destination,SigMeshMessage *responseMessage);

// callback about SigBluetooth
typedef void(^bleInitSuccessCallback)(CBCentralManager *central);
typedef void(^bleCentralUpdateStateCallback)(CBCentralManagerState state);
typedef void(^bleEnableCallback)(CBCentralManager *central,BOOL enable);
typedef void(^bleScanPeripheralCallback)(CBPeripheral *peripheral, NSDictionary<NSString *, id> *advertisementData, NSNumber *RSSI, BOOL unprovisioned);
typedef void(^bleScanSpecialPeripheralCallback)(CBPeripheral *peripheral, NSDictionary<NSString *, id> *advertisementData, NSNumber *RSSI, BOOL successful);
typedef void(^bleConnectPeripheralCallback)(CBPeripheral *peripheral,BOOL successful);
typedef void(^bleDiscoverServicesCallback)(CBPeripheral *peripheral,BOOL successful);
typedef void(^bleChangeNotifyCallback)(CBPeripheral *peripheral,BOOL isNotifying);
typedef void(^bleReadOTACharachteristicCallback)(CBCharacteristic *characteristic,BOOL successful);
typedef void(^bleCancelConnectCallback)(CBPeripheral *peripheral,BOOL successful);
typedef void(^bleCancelAllConnectCallback)(void);
typedef void(^bleDisconnectCallback)(CBPeripheral *peripheral,NSError *error);
typedef void(^bleIsReadyToSendWriteWithoutResponseCallback)(CBPeripheral *peripheral);
typedef void(^bleDidUpdateValueForCharacteristicCallback)(CBPeripheral *peripheral,CBCharacteristic *characteristic, NSError * _Nullable error);
typedef void(^bleDidWriteValueForCharacteristicCallback)(CBPeripheral *peripheral,CBCharacteristic *characteristic, NSError * _Nullable error);

@interface SigModel : NSObject
@end


@interface ModelIDModel : SigModel
@property (nonatomic,strong) NSString *modelGroup;
@property (nonatomic,strong) NSString *modelName;
@property (nonatomic,assign) NSInteger sigModelID;

-(instancetype)initWithModelGroup:(NSString *)modelGroup modelName:(NSString *)modelName sigModelID:(NSInteger)sigModelID;

@end


@interface ModelIDs : SigModel
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


@interface Groups : SigModel
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


@interface SchedulerModel : SigModel<NSCopying>
@property (nonatomic,assign) UInt64 schedulerID;//4 bits, Enumerates (selects) a Schedule Register entry. The valid values for the Index field are 0x0-0xF.
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


@interface AddDeviceModel : SigModel
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

@interface ActionModel : SigModel
@property (nonatomic,assign) UInt16 address;
@property (nonatomic,assign) DeviceState state;
@property (nonatomic,assign) UInt8 trueBrightness;//1-100
@property (nonatomic,assign) UInt8 trueTemperature;//0-100
- (instancetype)initWithNode:(SigNodeModel *)node;
- (BOOL)isSameActionWithAction:(ActionModel *)action;
- (NSDictionary *)getDictionaryOfActionModel;
- (void)setDictionaryToActionModel:(NSDictionary *)dictionary;
@end


static Byte CTByte[] = {(Byte) 0x11, (Byte) 0x02, (Byte) 0x01, (Byte) 0x00, (Byte) 0x32, (Byte) 0x37, (Byte) 0x69, (Byte) 0x00, (Byte) 0x07, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x19, (Byte) 0x01, (Byte) 0x00, (Byte) 0x00, (Byte) 0x02, (Byte) 0x00, (Byte) 0x03, (Byte) 0x00, (Byte) 0x04, (Byte) 0x00, (Byte) 0x05, (Byte) 0x00, (Byte) 0x00, (Byte) 0xfe, (Byte) 0x01, (Byte) 0xfe, (Byte) 0x02, (Byte) 0xfe, (Byte) 0x00, (Byte) 0xff, (Byte) 0x01, (Byte) 0xff, (Byte) 0x00, (Byte) 0x12, (Byte) 0x01, (Byte) 0x12, (Byte) 0x00, (Byte) 0x10, (Byte) 0x02, (Byte) 0x10, (Byte) 0x04, (Byte) 0x10, (Byte) 0x06, (Byte) 0x10, (Byte) 0x07, (Byte) 0x10, (Byte) 0x03, (Byte) 0x12, (Byte) 0x04, (Byte) 0x12, (Byte) 0x06, (Byte) 0x12, (Byte) 0x07, (Byte) 0x12, (Byte) 0x00, (Byte) 0x13, (Byte) 0x01, (Byte) 0x13, (Byte) 0x03, (Byte) 0x13, (Byte) 0x04, (Byte) 0x13, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x02, (Byte) 0x00, (Byte) 0x02, (Byte) 0x10, (Byte) 0x06, (Byte) 0x13};
static Byte PanelByte[] = {(Byte) 0x11, (Byte) 0x02, (Byte) 0x07, (Byte) 0x00, (Byte) 0x32, (Byte) 0x37, (Byte) 0x69, (Byte) 0x00, (Byte) 0x07, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00, (Byte) 0x02, (Byte) 0x00, (Byte) 0x03, (Byte) 0x00, (Byte) 0x04, (Byte) 0x00, (Byte) 0x05, (Byte) 0x00, (Byte) 0x00, (Byte) 0xfe, (Byte) 0x01, (Byte) 0xfe, (Byte) 0x02, (Byte) 0xfe, (Byte) 0x00, (Byte) 0xff, (Byte) 0x01, (Byte) 0xff, (Byte) 0x00, (Byte) 0x12, (Byte) 0x01, (Byte) 0x12, (Byte) 0x00, (Byte) 0x10, (Byte) 0x03, (Byte) 0x12, (Byte) 0x04, (Byte) 0x12, (Byte) 0x06, (Byte) 0x12, (Byte) 0x07, (Byte) 0x12, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00, (Byte) 0x11, (Byte) 0x02, (Byte) 0x01, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x05, (Byte) 0x01, (Byte) 0x00, (Byte) 0x10, (Byte) 0x03, (Byte) 0x12, (Byte) 0x04, (Byte) 0x12, (Byte) 0x06, (Byte) 0x12, (Byte) 0x07, (Byte) 0x12, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x05, (Byte) 0x01, (Byte) 0x00, (Byte) 0x10, (Byte) 0x03, (Byte) 0x12, (Byte) 0x04, (Byte) 0x12, (Byte) 0x06, (Byte) 0x12, (Byte) 0x07, (Byte) 0x12, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00};

@interface DeviceTypeModel : SigModel
@property (nonatomic, assign) UInt16 CID;
@property (nonatomic, assign) UInt16 PID;
@property (nonatomic, strong) SigPage0 *defaultCompositionData;

- (instancetype)initWithCID:(UInt16)cid PID:(UInt16)pid;
- (instancetype)initWithCID:(UInt16)cid PID:(UInt16)pid compositionData:(NSData *)cpsData;

@end


@interface SigAddConfigModel : SigModel
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
@interface IniCommandModel : SigModel
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
@property (nonatomic, strong, nullable) NSData *commandData;//max length is MESH_CMD_ACCESS_LEN_MAX. SigGenericOnOffSet: commandData of turn on without TransitionTime and delay is {0x01,0x00,0x00}. commandData of turn off without TransitionTime and delay is {0x00,0x00,0x00}

@property (nonatomic, copy) responseAllMessageBlock responseCallBack;
@property (nonatomic, assign) BOOL hasReceiveResponse;

@property (nonatomic,strong) SigNetkeyModel *netkeyA;
@property (nonatomic,strong) SigAppkeyModel *appkeyA;
@property (nonatomic,strong) SigIvIndex *ivIndexA;
@property (nonatomic,assign) NSTimeInterval timeout;

/// create sig model ini command
- (instancetype)initSigModelIniCommandWithNetkeyIndex:(UInt16)netkeyIndex appkeyIndex:(UInt16)appkeyIndex retryCount:(UInt8)retryCount responseMax:(UInt8)responseMax address:(UInt16)address opcode:(UInt16)opcode commandData:(NSData *)commandData;
/// create vebdor model ini command
- (instancetype)initVendorModelIniCommandWithNetkeyIndex:(UInt16)netkeyIndex appkeyIndex:(UInt16)appkeyIndex retryCount:(UInt8)retryCount responseMax:(UInt8)responseMax address:(UInt16)address opcode:(UInt8)opcode vendorId:(UInt16)vendorId responseOpcode:(UInt8)responseOpcode needTid:(BOOL)needTid tid:(UInt8)tid commandData:(nullable NSData *)commandData;
/// create model by ini data
/// @param iniCommandData ini data, eg: "a3ff000000000200ffffc21102c4020100"
- (instancetype)initWithIniCommandData:(NSData *)iniCommandData;

@end


@interface SigNodeUpdateStatusModel : SigModel
@property (nonatomic, assign) UInt16 address;
@property (nonatomic, assign) SigUpdateStatusType status;

- (instancetype)initWithAddress:(UInt16)address status:(SigUpdateStatusType)status;

@end

/// 8.4.1.2 Firmware Update Information Status
/// - seeAlso: MshMDL_DFU_MBT_CR_R04_LbL25.pdf  (page.81)
@interface SigFirmwareInformationEntryModel : SigModel
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


@interface SigTransitionTime : NSObject
/// Transition Number of Steps, 6-bit value.
///
/// Value 0 indicates an immediate transition.
///
/// Value 0x3F means that the value is unknown. The state cannot be
/// set to this value, but an element may report an unknown value if
/// a transition is higher than 0x3E or not determined.
@property (nonatomic,assign) UInt8 steps;

/// The step resolution.
@property (nonatomic,assign) SigStepResolution stepResolution;

/// The transition time in milliseconds.
@property (nonatomic,assign) int milliseconds;

/// The transition time as `TimeInterval` in seconds.
@property (nonatomic,assign) NSTimeInterval interval;

@property (nonatomic,assign) UInt8 rawValue;


/// Creates the Transition Time object for an unknown time.
- (instancetype)init;

- (instancetype)initWithRawValue:(UInt8)rawValue;

/// Creates the Transition Time object.
///
/// Only values of 0x00 through 0x3E shall be used to specify the value
/// of the Transition Number of Steps field.
///
/// - parameter steps: Transition Number of Steps, valid values are in
///                    range 0...62. Value 63 means that the value is
///                    unknown and the state cannot be set to this value.
/// - parameter stepResolution: The step resolution.
- (instancetype)initWithSetps:(UInt8)steps stepResolution:(SigStepResolution)stepResolution;

@end


@interface SigMeshAddress : NSObject
/// 16-bit address.
@property (nonatomic, assign) UInt16 address;
/// Virtual label UUID.
@property (nonatomic, strong) CBUUID *virtualLabel;

- (instancetype)initWithHex:(NSString *)hex;

/// Creates a Mesh Address. For virtual addresses use `initWithVirtualLabel:` instead.
/// @param address address
-(instancetype)initWithAddress:(UInt16)address;

/// Creates a Mesh Address based on the virtual label.
- (instancetype)initWithVirtualLabel:(CBUUID *)virtualLabel;

- (NSString *)getHex;

@end


/// The object is used to describe the number of times a message is published and the interval between retransmissions of the published message.
@interface SigRetransmit : NSObject
/// Number of retransmissions for network messages. The value is in range from 0 to 7, where 0 means no retransmissions.
@property (nonatomic,assign) UInt8 count;
/// The interval (in milliseconds) between retransmissions (50...3200 with step 50).
@property (nonatomic,assign) UInt16 interval;
/// Retransmission steps, from 0 to 31. Use `interval` to get the interval in ms.
- (UInt8)steps;
- (instancetype)initWithPublishRetransmitCount:(UInt8)publishRetransmitCount intervalSteps:(UInt8)intervalSteps;
@end


@interface SigPublish : NSObject

/// Publication address for the Model. It's 4 or 32-character long hexadecimal string.
@property (nonatomic,strong) NSString *address;
/// Publication address for the model.
@property (nonatomic,strong) SigMeshAddress *publicationAddress;//Warning: assuming hex address is valid!
/// An Application Key index, indicating which Applicaiton Key to use for the publication.
@property (nonatomic,assign) UInt16 index;
/// An integer from 0 to 127 that represents the Time To Live (TTL) value for the outgoing publish message. 255 means default TTL value.
@property (nonatomic,assign) UInt8 ttl;
/// The interval (in milliseconds) between subsequent publications.
@property (nonatomic,assign) int period;
/// The number of steps, in range 0...63.
@property (nonatomic,assign) UInt8 periodSteps;
/// The resolution of the number of steps.
@property (nonatomic,assign) SigStepResolution periodResolution;
/// An integer 0 o 1 that represents whether master security (0) materials or friendship security material (1) are used.
@property (nonatomic,assign) int credentials;
/// The object describes the number of times a message is published and the interval between retransmissions of the published message.
@property (nonatomic,strong) SigRetransmit *retransmit;

/// Creates an instance of Publish object.
/// @param stringDestination The publication address.
/// @param keyIndex The Application Key that will be used to send messages.
/// @param friendshipCredentialsFlag `True`, to use Friendship Security Material, `false` to use Master Security Material.
/// @param ttl Time to live. Use 0xFF to use Node's default TTL.
/// @param periodSteps Period steps, together with `periodResolution` are used to calculate period interval. Value can be in range 0...63. Value 0 disables periodic publishing.
/// @param periodResolution The period resolution, used to calculate interval. Use `._100_milliseconds` when periodic publishing is disabled.
/// @param retransmit The retransmission data. See `Retransmit` for details.
- (instancetype)initWithStringDestination:(NSString *)stringDestination withKeyIndex:(UInt16)keyIndex friendshipCredentialsFlag:(int)friendshipCredentialsFlag ttl:(UInt8)ttl periodSteps:(UInt8)periodSteps periodResolution:(SigStepResolution)periodResolution retransmit:(SigRetransmit *)retransmit;

/// Creates an instance of Publish object.
/// @param destination The publication address.
/// @param keyIndex The Application Key that will be used to send messages.
/// @param friendshipCredentialsFlag `True`, to use Friendship Security Material, `false` to use Master Security Material.
/// @param ttl Time to live. Use 0xFF to use Node's default TTL.
/// @param periodSteps Period steps, together with `periodResolution` are used to calculate period interval. Value can be in range 0...63. Value 0 disables periodic publishing.
/// @param periodResolution The period resolution, used to calculate interval. Use `._100_milliseconds` when periodic publishing is disabled.
/// @param retransmit The retransmission data. See `Retransmit` for details.
- (instancetype)initWithDestination:(UInt16)destination withKeyIndex:(UInt16)keyIndex friendshipCredentialsFlag:(int)friendshipCredentialsFlag ttl:(UInt8)ttl periodSteps:(UInt8)periodSteps periodResolution:(SigStepResolution)periodResolution retransmit:(SigRetransmit *)retransmit;

/// Returns the interval between subsequent publications in seconds.
- (NSTimeInterval)publicationInterval;
/// Returns whether master security materials are used.
- (BOOL)isUsingMasterSecurityMaterial;
/// Returns whether friendship security materials are used.
- (BOOL)isUsingFriendshipSecurityMaterial;

@end


/// 5.2.1.2 Time Set
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.143)

@interface SigTimeModel : NSObject
/// 40 bits, The current TAI time in seconds.
@property (nonatomic, assign) UInt64 TAISeconds;
/// The sub-second time in units of 1/256th second.
@property (nonatomic, assign) UInt8 subSeconds;
/// The estimated uncertainty in 10 millisecond steps.
@property (nonatomic, assign) UInt8 uncertainty;
/// 1 bit, 0 = NO Time Authority, 1 = Time Authority.
@property (nonatomic, assign) UInt8 timeAuthority;
/// 15 bits, Current difference between TAI and UTC in seconds.
@property (nonatomic, assign) UInt16 TAI_UTC_Delta;
/// The local time zone offset in 15-minute increments.
@property (nonatomic, assign) UInt8 timeZoneOffset;

- (instancetype)initWithTAISeconds:(UInt64)TAISeconds subSeconds:(UInt8)subSeconds uncertainty:(UInt8)uncertainty timeAuthority:(UInt8)timeAuthority TAI_UTC_Delta:(UInt16)TAI_UTC_Delta timeZoneOffset:(UInt8)timeZoneOffset;
- (instancetype)initWithParameters:(NSData *)parameters;
- (NSData *)getTimeParameters;

@end


@interface SigSensorDescriptorModel : NSObject

/// The Sensor Property ID field is a 2-octet value referencing a device property that describes the meaning and the format of data reported by a sensor.(0x0001–0xFFFF)
@property (nonatomic, assign) UInt16 propertyID;
/// The Sensor Positive Tolerance field is a 12-bit value representing the magnitude of a possible positive error associated with the measurements that the sensor is reporting.(0x001–0xFFF)
@property (nonatomic, assign) UInt16 positiveTolerance;
/// The Sensor Negative Tolerance field is a 12-bit value representing the magnitude of a possible negative error associated with the measurements that the sensor is reporting.(0x001–0xFFF)
@property (nonatomic, assign) UInt16 negativeTolerance;
/// This Sensor Sampling Function field specifies the averaging operation or type of sampling function applied to the measured value.(0x00–0x07)
@property (nonatomic, assign) SigSensorSamplingFunctionType samplingFunction;
/// This Sensor Measurement Period field specifies a uint8 value n that represents the averaging time span, accumulation time, or measurement period in seconds over which the measurement is taken.(0x00–0xFF)
@property (nonatomic, assign) UInt8 measurementPeriod;
/// The measurement reported by a sensor is internally refreshed at the frequency indicated in the Sensor Update Interval field.(0x00–0xFF)
@property (nonatomic, assign) UInt8 updateInterval;

- (NSData *)getDescriptorParameters;
- (instancetype)initWithDescriptorParameters:(NSData *)parameters;

@end


/// mesh设备广播包解密模型。唯一标识符为identityData，且只存储本地json存在的identityData不为空的SigEncryptedModel。设备断电后会改变identityData，出现相同的address的SigEncryptedModel时，需要replace旧的。
@interface SigEncryptedModel : NSObject
@property (nonatomic, strong) NSData *identityData;
@property (nonatomic, strong) NSData *hashData;
@property (nonatomic, strong) NSData *randomData;
@property (nonatomic, strong) NSString *peripheralUUID;
@property (nonatomic, strong) NSData *encryptedData;
@property (nonatomic, assign) UInt16 address;
@end


@interface SigNetkeyModel : NSObject

@property (nonatomic, copy, nullable) NSString *oldKey;

@property (nonatomic, assign) UInt16 index;

@property (nonatomic, assign) KeyRefreshPhase phase;

@property (nonatomic, copy) NSString *timestamp;

@property (nonatomic, copy) NSString *key;

@property (nonatomic, copy) NSString *name;

@property (nonatomic, copy) NSString *minSecurity;

@property (nonatomic, strong) SigNetkeyDerivaties *keys;
@property (nonatomic, strong) SigNetkeyDerivaties *oldKeys;

/// Network identifier.
@property (nonatomic, assign) UInt8 nid;
/// Network identifier derived from the old key.
@property (nonatomic, assign) UInt8 oldNid;
/// The IV Index for this subnetwork.
@property (nonatomic, strong) SigIvIndex *ivIndex;
/// The Network ID derived from this Network Key. This identifier is public information.
@property (nonatomic, strong) NSData *networkId;
/// The Network ID derived from the old Network Key. This identifier is public information. It is set when `oldKey` is set.
@property (nonatomic, strong) NSData *oldNetworkId;

- (SigNetkeyDerivaties *)transmitKeys;

/// Returns whether the Network Key is the Primary Network Key.
/// The Primary key is the one which Key Index is equal to 0.
///
/// A Primary Network Key may not be removed from the mesh network.
- (BOOL)isPrimary;

- (NSDictionary *)getDictionaryOfSigNetkeyModel;
- (void)setDictionaryToSigNetkeyModel:(NSDictionary *)dictionary;

- (NSString *)getNetKeyDetailString;

@end

@interface SigNetkeyDerivaties : NSObject

@property (nonatomic, strong) NSData *identityKey;

@property (nonatomic, strong) NSData *beaconKey;

@property (nonatomic, strong) NSData *encryptionKey;

@property (nonatomic, strong) NSData *privacyKey;

- (SigNetkeyDerivaties *)initWithNetkeyData:(NSData *)key helper:(OpenSSLHelper *)helper;

@end

@interface SigIvIndex : NSObject

@property (nonatomic,assign) UInt32 index;//init 0
@property (nonatomic,assign) BOOL updateActive;//init NO

- (instancetype)initWithIndex:(UInt32)index updateActive:(BOOL)updateActive;

@end

@interface SigProvisionerModel : NSObject

@property (nonatomic, strong) NSMutableArray <SigRangeModel *>*allocatedGroupRange;

@property (nonatomic, strong) NSMutableArray <SigRangeModel *>*allocatedUnicastRange;

@property (nonatomic, copy) NSString *UUID;

@property (nonatomic, copy) NSString *provisionerName;

@property (nonatomic, strong) NSMutableArray <SigSceneRangeModel *>*allocatedSceneRange;


/**
 create new provisioner by count of exist provisioners.

 @param count count of exist provisioners
 @param provisionerUUID new provisioner's uuid
 @return SigProvisionerModel model
 */
-(instancetype)initWithExistProvisionerCount:(UInt16)count andProvisionerUUID:(NSString *)provisionerUUID;

- (instancetype)initWithExistProvisionerMaxHighAddressUnicast:(UInt16)maxHighAddressUnicast andProvisionerUUID:(NSString *)provisionerUUID;

- (SigNodeModel *)node;

- (NSDictionary *)getDictionaryOfSigProvisionerModel;
- (void)setDictionaryToSigProvisionerModel:(NSDictionary *)dictionary;

@end

@interface SigRangeModel : NSObject

@property (nonatomic, copy) NSString *lowAddress;

@property (nonatomic, copy) NSString *highAddress;

- (NSInteger)lowIntAddress;
- (NSInteger)hightIntAddress;

- (NSDictionary *)getDictionaryOfSigRangeModel;
- (void)setDictionaryToSigRangeModel:(NSDictionary *)dictionary;

@end

@interface SigSceneRangeModel : NSObject

@property (nonatomic, copy) NSString *firstScene;

@property (nonatomic, copy) NSString *lastScene;

- (NSDictionary *)getDictionaryOfSigSceneRangeModel;
- (void)setDictionaryToSigSceneRangeModel:(NSDictionary *)dictionary;

@end

@interface SigAppkeyModel : NSObject

@property (nonatomic, copy) NSString *oldKey;

@property (nonatomic, copy) NSString *key;

@property (nonatomic, copy) NSString *name;

@property (nonatomic, assign) NSInteger boundNetKey;

@property (nonatomic, assign) NSInteger index;

@property (nonatomic, assign) UInt8 aid;
@property (nonatomic, assign) UInt8 oldAid;

- (SigNetkeyModel *)getCurrentBoundNetKey;
- (NSData *)getDataKey;
- (NSData *)getDataOldKey;

- (NSDictionary *)getDictionaryOfSigAppkeyModel;
- (void)setDictionaryToSigAppkeyModel:(NSDictionary *)dictionary;

- (NSString *)getAppKeyDetailString;

@end

@interface SigSceneModel : NSObject<NSCopying>

@property (nonatomic, copy) NSString *name;

@property (nonatomic, assign) NSInteger number;

@property (nonatomic, strong) NSMutableArray <NSString *>*addresses;

//暂时添加并保存json
@property (nonatomic, strong) NSMutableArray <ActionModel *>*actionList;

- (NSDictionary *)getDictionaryOfSigSceneModel;
- (void)setDictionaryToSigSceneModel:(NSDictionary *)dictionary;
- (NSDictionary *)getFormatDictionaryOfSigSceneModel;

@end

@interface SigGroupModel : NSObject

@property (nonatomic, copy) NSString *name;

@property (nonatomic, copy) NSString *address;

@property (nonatomic, copy) NSString *parentAddress;

@property (nonatomic, copy) SigMeshAddress *meshAddress;

- (UInt16)intAddress;

- (NSDictionary *)getDictionaryOfSigGroupModel;
- (void)setDictionaryToSigGroupModel:(NSDictionary *)dictionary;

//临时缓存groupBrightness、groupTempareture，关闭APP后就丢失。
@property (nonatomic,assign) UInt8 groupBrightness;
@property (nonatomic,assign) UInt8 groupTempareture;

- (BOOL)isOn;
- (NSMutableArray <SigNodeModel *>*)groupDevices;
- (NSMutableArray <SigNodeModel *>*)groupOnlineDevices;

@end

///Attention: Boolean type should use bool not BOOL.
@interface SigNodeModel : NSObject<NSCopying>

@property (nonatomic, strong) SigNodeFeatures *features;

@property (nonatomic, copy) NSString *unicastAddress;

@property (nonatomic, assign) bool secureNetworkBeacon;

@property (nonatomic, strong) SigRelayretransmitModel *relayRetransmit;

@property (nonatomic, strong) SigNetworktransmitModel *networkTransmit;

@property (nonatomic, assign) bool configComplete;

@property (nonatomic, copy) NSString *vid;

@property (nonatomic, copy) NSString *cid;

@property (nonatomic, assign) bool blacklisted;

@property (nonatomic, copy) NSString *UUID;

@property (nonatomic, copy) NSString *security;

@property (nonatomic, copy) NSString *crpl;

@property (nonatomic, assign) NSInteger defaultTTL;

@property (nonatomic, copy) NSString *pid;

@property (nonatomic, copy) NSString *name;

@property (nonatomic, copy) NSString *deviceKey;

@property (nonatomic, copy) NSString *macAddress;//new add the mac to json, get mac from scanResponse's Manufacturer Data.

@property (nonatomic, strong) SigHeartbeatPubModel *heartbeatPub;

@property (nonatomic, strong) NSMutableArray<SigElementModel *> *elements;
@property (nonatomic, strong) NSMutableArray<SigNodeKeyModel *> *netKeys;
@property (nonatomic, strong) NSMutableArray<SigNodeKeyModel *> *appKeys;//node isn't unbound when appkeys is empty.
@property (nonatomic, strong) NSMutableArray<SigHeartbeatSubModel *> *heartbeatSub;

//暂时添加到json数据中
@property (nonatomic,strong) NSMutableArray <SchedulerModel *>*schedulerList;
//@property (nonatomic, copy) NSString *sno;
//The following properties are not stored JSON
@property (nonatomic,assign) DeviceState state;
@property (nonatomic,assign) BOOL isKeyBindSuccess;
@property (nonatomic,assign) UInt16 brightness;
@property (nonatomic,assign) UInt16 temperature;
@property (nonatomic,assign) UInt16 HSL_Lightness;
@property (nonatomic,assign) UInt16 HSL_Hue;
@property (nonatomic,assign) UInt16 HSL_Saturation;
@property (nonatomic,assign) UInt8 HSL_Lightness100;
@property (nonatomic,assign) UInt8 HSL_Hue100;
@property (nonatomic,assign) UInt8 HSL_Saturation100;
@property (nonatomic,strong) NSMutableArray <NSNumber *>*keyBindModelIDs;//There are modelID that current had key bind.
@property (nonatomic,strong) SigPage0 *compositionData;//That is composition data get from add device process.APP can get ele_cut in provision_end_callback, app can get detail composition data in App_key_bind_end_callback.
@property (nonatomic,strong) NSMutableArray <NSNumber *>*onoffAddresses;//element addresses of onoff
@property (nonatomic,strong) NSMutableArray <NSNumber *>*levelAddresses;//element addresses of level
@property (nonatomic,strong) NSMutableArray <NSNumber *>*temperatureAddresses;//element addresses of color temperature
@property (nonatomic,strong) NSMutableArray <NSNumber *>*HSLAddresses;//element addresses of HSL
@property (nonatomic,strong) NSMutableArray <NSNumber *>*lightnessAddresses;//element addresses of lightness
@property (nonatomic,strong) NSMutableArray <NSNumber *>*schedulerAddress;//element addresses of scheduler
@property (nonatomic,strong) NSMutableArray <NSNumber *>*sceneAddress;//element addresses of scene
@property (nonatomic,strong) NSMutableArray <NSNumber *>*publishAddress;//element addresses of publish
@property (nonatomic,assign) UInt16 publishModelID;//modelID of set publish
@property (nonatomic,strong,nullable) NSString *peripheralUUID;

///return node true brightness, range is 0~100
- (UInt8)trueBrightness;

///return node true color temperture, range is 0~100
- (UInt8)trueTemperature;

///update node status, YES means status had changed, NO means status hadn't changed.
//- (BOOL)update:(ResponseModel *)m;

///update node status from api getOnlineStatusFromUUIDWithCompletation
- (void)updateOnlineStatusWithDeviceState:(DeviceState)state bright100:(UInt8)bright100 temperature100:(UInt8)temperature100;

- (UInt16)getNewSchedulerID;

- (void)saveSchedulerModelWithModel:(SchedulerModel *)scheduler;

- (UInt8)getElementCount;

- (NSMutableArray *)getAddressesWithModelID:(NSNumber *)sigModelID;

- (instancetype)initWithNode:(SigNodeModel *)node;

- (UInt16)address;
- (void)setAddress:(UInt16)address;
//- (int)getIntSNO;
//- (void)setIntSno:(UInt32)intSno;

///get all groupIDs of node(获取该设备的所有组号)
- (NSMutableArray <NSNumber *>*)getGroupIDs;

///add new groupID to node(新增设备的组号)
- (void)addGroupID:(NSNumber *)groupID;

///delete old groupID from node(删除设备的组号)
- (void)deleteGroupID:(NSNumber *)groupID;

- (void)openPublish;

- (void)closePublish;

- (BOOL)hasPublishFunction;

- (BOOL)hasOpenPublish;

///publish是否存在周期上报功能。
- (BOOL)hasPublishPeriod;

- (BOOL)isSensor;

/// Returns list of Network Keys known to this Node.
- (NSArray <SigNetkeyModel *>*)getNetworkKeys;

/// The last unicast address allocated to this node. Each node's element
/// uses its own subsequent unicast address. The first (0th) element is identified
/// by the node's unicast address. If there are no elements, the last unicast address
/// is equal to the node's unicast address.
- (UInt16)lastUnicastAddress;

/// Returns whether the address uses the given unicast address for one
/// of its elements.
///
/// - parameter addr: Address to check.
/// - returns: `True` if any of node's elements (or the node itself) was assigned
///            the given address, `false` otherwise.
- (BOOL)hasAllocatedAddr:(UInt16)addr;

- (SigModelIDModel *)getModelIDModelWithModelID:(UInt16)modelID;
- (SigModelIDModel *)getModelIDModelWithModelID:(UInt16)modelID andElementAddress:(UInt16)elementAddress;

- (NSDictionary *)getDictionaryOfSigNodeModel;
- (void)setDictionaryToSigNodeModel:(NSDictionary *)dictionary;
- (NSDictionary *)getFormatDictionaryOfSigNodeModel;

- (void)setAddSigAppkeyModelSuccess:(SigAppkeyModel *)appkey;
- (void)setCompositionData:(SigPage0 *)compositionData;
- (void)setBindSigNodeKeyModel:(SigNodeKeyModel *)appkey toSigModelIDModel:(SigModelIDModel *)modelID;

- (void)updateNodeStatusWithBaseMeshMessage:(SigBaseMeshMessage *)responseMessage source:(UInt16)source;

@end

@interface SigRelayretransmitModel : NSObject
/// Number of retransmissions for relay messages.
/// The value is in range from 1 to 8.
@property (nonatomic, assign) NSInteger count;
/// The interval (in milliseconds) between retransmissions
/// (from 10 to 320 ms in 10 ms steps).
@property (nonatomic, assign) NSInteger interval;
/// Number of 10-millisecond steps between transmissions.
@property (nonatomic, assign) UInt8 steps;

- (NSDictionary *)getDictionaryOfSigRelayretransmitModel;
- (void)setDictionaryToSigRelayretransmitModel:(NSDictionary *)dictionary;

@end

/// The object represents parameters of the transmissions of network layer messages originating from a mesh node.
@interface SigNetworktransmitModel : NSObject
/// Number of retransmissions for relay messages.
/// The value is in range from 1 to 8.
@property (nonatomic, assign) NSInteger count;
/// The interval (in milliseconds) between retransmissions
/// (from 10 to 320 ms in 10 ms steps).
@property (nonatomic, assign) NSInteger interval;
/// Number of 10-millisecond steps between transmissions.
- (UInt8)septs;

/// The interval in as `TimeInterval` in seconds.
- (NSTimeInterval)timeInterval;

- (instancetype)initWithSigConfigNetworkTransmitSet:(SigConfigNetworkTransmitSet *)request;

- (instancetype)initWithSigConfigNetworkTransmitStatus:(SigConfigNetworkTransmitStatus *)status;

- (NSDictionary *)getDictionaryOfSigNetworktransmitModel;
- (void)setDictionaryToSigNetworktransmitModel:(NSDictionary *)dictionary;

@end

/// The features object represents the functionality of a mesh node that is determined by the set features that the node supports.
@interface SigNodeFeatures : NSObject

/// The state of Relay feature. Default is 2.
@property (nonatomic,assign) SigNodeFeaturesState relayFeature;
/// The state of Proxy feature. Default is 2.
@property (nonatomic,assign) SigNodeFeaturesState proxyFeature;
/// The state of Friend feature. Default is 2.
@property (nonatomic,assign) SigNodeFeaturesState friendFeature;
/// The state of Low Power feature. Default is 2.
@property (nonatomic,assign) SigNodeFeaturesState lowPowerFeature;

- (UInt16)rawValue;
- (instancetype)initWithRawValue:(UInt16)rawValue;
- (instancetype)initWithRelay:(SigNodeFeaturesState)relayFeature proxy:(SigNodeFeaturesState)proxyFeature friend:(SigNodeFeaturesState)friendFeature lowPower:(SigNodeFeaturesState)lowPowerFeature;

- (NSDictionary *)getDictionaryOfSigFeatureModel;
- (void)setDictionaryToSigFeatureModel:(NSDictionary *)dictionary;

@end

@interface SigNodeKeyModel : NSObject
/// The Key index for this network key.
@property (nonatomic, assign) UInt16 index;
/// This flag contains value set to `false`, unless a Key Refresh
/// procedure is in progress and the network has been successfully
/// updated.
@property (nonatomic, assign) bool updated;
- (instancetype)initWithIndex:(UInt16)index updated:(bool)updated;

- (NSDictionary *)getDictionaryOfSigNodeKeyModel;
- (void)setDictionaryToSigNodeKeyModel:(NSDictionary *)dictionary;

@end

@interface SigElementModel : NSObject

@property (nonatomic, strong) NSMutableArray<SigModelIDModel *> *models;

@property (nonatomic, copy) NSString *name;

@property (nonatomic, copy) NSString *location;

@property (nonatomic, assign) UInt8 index;

@property (nonatomic, assign) UInt16 parentNodeAddress;

- (instancetype)initWithLocation:(SigLocation)location;

- (instancetype)initWithCompositionData:(NSData *)compositionData offset:(int *)offset;

/// Returns the Unicast Address of the Element. For Elements not added to Node this returns the Element index value as `Address`.
- (UInt16)unicastAddress;

- (SigNodeModel * _Nullable)getParentNode;

- (SigLocation)getSigLocation;
- (void)setSigLocation:(SigLocation)sigLocation;

- (NSDictionary *)getDictionaryOfSigElementModel;
- (void)setDictionaryToSigElementModel:(NSDictionary *)dictionary;

- (NSData *)getElementData;

@end

@interface SigModelIDModel : NSObject
/// An array of Appliaction Key indexes to which this model is bound.
@property (nonatomic, strong) NSMutableArray <NSNumber *>*bind;//[KeyIndex]
//Attention: length=4，为SIG modelID，类型为UInt16；length=8，为vendor modelID，类型为UInt32
@property (nonatomic, copy) NSString *modelId;
/// The array of Unicast or Group Addresses (4-character hexadecimal value), or Virtual Label UUIDs (32-character hexadecimal string).
@property (nonatomic, strong) NSMutableArray <NSString *>*subscribe;
/// The configuration of this Model's publication.
@property (nonatomic, strong, nullable) SigPublishModel *publish;
/// The model message handler. This is non-`nil` for supported local Models and `nil` for Models of remote Nodes.
@property (nonatomic,weak) id delegate;

///返回整形的modelID
- (int)getIntModelID;
- (UInt16)getIntModelIdentifier;
- (UInt16)getIntCompanyIdentifier;

- (instancetype)initWithSigModelId:(UInt16)sigModelId;

- (instancetype)initWithVendorModelId:(UInt32)vendorModelId;

///// Bluetooth SIG or vendor-assigned model identifier.
//- (UInt16)modelIdentifier;
///// The Company Identifier or `nil`, if the model is Bluetooth SIG-assigned.
//- (UInt16)companyIdentifier;
/// Returns `true` for Models with identifiers assigned by Bluetooth SIG,
/// `false` otherwise.
- (BOOL)isBluetoothSIGAssigned;
/// Returns the list of known Groups that this Model is subscribed to.
/// It may be that the Model is subscribed to some other Groups, which are
/// not known to the local database, and those are not returned.
/// Use `isSubscribed(to:)` to check other Groups.
- (NSArray <SigGroupModel *>*)subscriptions;

- (BOOL)isConfigurationServer;
- (BOOL)isConfigurationClient;
- (BOOL)isHealthServer;
- (BOOL)isHealthClient;

/// Adds the given Application Key Index to the bound keys.
///
/// - paramter applicationKeyIndex: The Application Key index to bind.
- (void)bindApplicationKeyWithIndex:(UInt16)applicationKeyIndex;

/// Removes the Application Key binding to with the given Key Index
/// and clears the publication, if it was set to use the same key.
///
/// - parameter applicationKeyIndex: The Application Key index to unbind.
- (void)unbindApplicationKeyWithIndex:(UInt16)applicationKeyIndex;

/// Adds the given Group to the list of subscriptions.
///
/// - parameter group: The new Group to be added.
- (void)subscribeToGroup:(SigGroupModel *)group;

/// Removes the given Group from list of subscriptions.
///
/// - parameter group: The Group to be removed.
- (void)unsubscribeFromGroup:(SigGroupModel *)group;

/// Removes the given Address from list of subscriptions.
///
/// - parameter address: The Address to be removed.
- (void)unsubscribeFromAddress:(UInt16)address;

/// Removes all subscribtions from this Model.
- (void)unsubscribeFromAll;

/// Whether the given Application Key is bound to this Model.
///
/// - parameter applicationKey: The key to check.
/// - returns: `True` if the key is bound to this Model,
///            otherwise `false`.
- (BOOL)isBoundToApplicationKey:(SigAppkeyModel *)applicationKey;

/// Returns whether the given Model is compatible with the one.
///
/// A compatible Models create a Client-Server pair. I.e., the
/// Generic On/Off Client is compatible to Generic On/Off Server,
/// and vice versa. The rule is that the Server Model has an even
/// Model ID and the Client Model has Model ID greater by 1.
///
/// - parameter model: The Model to compare to.
/// - returns: `True`, if the Models are compatible, `false` otherwise.
- (BOOL)isCompatibleToModel:(SigModelIDModel *)model;

/// Returns whether the Model is subscribed to the given Group.
///
/// This method may also return `true` if the Group is not known
/// to the local Provisioner and is not returned using `subscriptions`
/// property.
///
/// - parameter group: The Group to check subscription to.
/// - returns: `True` if the Model is subscribed to the Group,
///            `false` otherwise.
- (BOOL)isSubscribedToGroup:(SigGroupModel *)group;

/// Returns whether the Model is subscribed to the given address.
///
/// This method may also return `true` if the address is not known
/// to the local Provisioner and is a Group with this address is
/// not returned using `subscriptions` property.
/// Moreover, if a Virtual Label of a Group is not known, but the
/// 16-bit address is known, and the given address contains the Virtual
/// Label, with the same 16-bit address, this method will return `false`,
/// as it may not guarantee that the labels are the same.
///
/// - parameter address: The address to check subscription to.
/// - returns: `True` if the Model is subscribed to a Group with given,
///            address, `false` otherwise.
- (BOOL)isSubscribedToAddress:(SigMeshAddress *)address;

- (NSDictionary *)getDictionaryOfSigModelIDModel;
- (void)setDictionaryToSigModelIDModel:(NSDictionary *)dictionary;

@end

@interface SigPublishModel : NSObject

@property (nonatomic, assign) NSInteger index;

@property (nonatomic, assign) NSInteger credentials;

@property (nonatomic, assign) NSInteger ttl;

@property (nonatomic, strong) SigRetransmitModel *retransmit;

@property (nonatomic, strong) SigPeriodModel *period;

@property (nonatomic, copy) NSString *address;

- (NSDictionary *)getDictionaryOfSigPublishModel;
- (void)setDictionaryToSigPublishModel:(NSDictionary *)dictionary;

@end

@interface SigRetransmitModel : NSObject

@property (nonatomic, assign) NSInteger count;

@property (nonatomic, assign) NSInteger interval;

- (NSDictionary *)getDictionaryOfSigRetransmitModel;
- (void)setDictionaryToSigRetransmitModel:(NSDictionary *)dictionary;

@end

@interface SigPeriodModel : NSObject

@property (nonatomic, assign) NSInteger numberOfSteps;

@property (nonatomic, assign) NSInteger resolution;

- (NSDictionary *)getDictionaryOfSigPeriodModel;
- (void)setDictionaryToSigPeriodModel:(NSDictionary *)dictionary;

@end

@interface SigHeartbeatPubModel : NSObject

@property (nonatomic, copy) NSString *address;

@property (nonatomic, assign) NSInteger period;

@property (nonatomic, assign) NSInteger ttl;

@property (nonatomic, assign) NSInteger index;

@property (nonatomic, strong) NSMutableArray <NSString *>*features;

- (NSDictionary *)getDictionaryOfSigHeartbeatPubModel;
- (void)setDictionaryToSigHeartbeatPubModel:(NSDictionary *)dictionary;

@end

@interface SigHeartbeatSubModel : NSObject

@property (nonatomic, copy) NSString *source;

@property (nonatomic, copy) NSString *destination;

@property (nonatomic, assign) NSInteger period;

- (NSDictionary *)getDictionaryOfSigHeartbeatSubModel;
- (void)setDictionaryToSigHeartbeatSubModel:(NSDictionary *)dictionary;

@end


@interface SigOOBModel : SigModel
@property (nonatomic, assign) OOBSourceTpye sourceType;
@property (nonatomic, strong) NSString *UUIDString;
@property (nonatomic, strong) NSString *OOBString;
@property (nonatomic, strong) NSString *lastEditTimeString;
- (instancetype)initWithSourceType:(OOBSourceTpye)sourceType UUIDString:(NSString *)UUIDString OOBString:(NSString *)OOBString;
- (void)updateWithUUIDString:(NSString *)UUIDString OOBString:(NSString *)OOBString;
@end

NS_ASSUME_NONNULL_END
