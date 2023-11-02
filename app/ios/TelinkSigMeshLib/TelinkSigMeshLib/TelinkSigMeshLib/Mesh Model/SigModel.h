/********************************************************************************************************
 * @file     SigModel.h
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/8/15
 *
 * @par     Copyright (c) [2021], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/

#import <Foundation/Foundation.h>
NS_ASSUME_NONNULL_BEGIN

@class SigNetkeyDerivaties,OpenSSLHelper,SigRangeModel,SigSceneRangeModel,SigNodeFeatures,SigRelayretransmitModel,SigNetworktransmitModel,SigElementModel,SigNodeKeyModel,SigModelIDModel,SigRetransmitModel,SigPeriodModel,SigHeartbeatPubModel,SigHeartbeatSubModel,SigBaseMeshMessage,SigConfigNetworkTransmitSet,SigConfigNetworkTransmitStatus,SigPublishModel,SigNodeModel,SigMeshMessage,SigNetkeyModel,SigAppkeyModel,SigIvIndex,SigPage0,SigSubnetBridgeModel,SigMeshAddress, SigDirectControlStatus, SigForwardingTableModel,SigProvisioningCapabilitiesPdu;

//typedef void(^bleEnableCallback)(CBCentralManager *central,BOOL enable);
//typedef void(^bleChangeNotifyCallback)(CBPeripheral *peripheral,BOOL isNotifying);

/// base callback of message from mesh.
typedef void(^responseAllMessageBlock)(UInt16 source,UInt16 destination,SigMeshMessage *responseMessage);

/// callback about SigBluetooth

/// callback of ble init success
typedef void(^bleInitSuccessCallback)(CBCentralManager *central);

/// callback of ble Central Update State
typedef void(^bleCentralUpdateStateCallback)(CBCentralManagerState state);

/// callback of ble Scan Peripheral response
typedef void(^bleScanPeripheralCallback)(CBPeripheral *peripheral, NSDictionary<NSString *, id> *advertisementData, NSNumber *RSSI, BOOL unprovisioned);

/// callback of ble Scan Special Peripheral response
typedef void(^bleScanSpecialPeripheralCallback)(CBPeripheral *peripheral, NSDictionary<NSString *, id> *advertisementData, NSNumber *RSSI, BOOL successful);

/// callback of ble Connect Peripheral
typedef void(^bleConnectPeripheralCallback)(CBPeripheral *peripheral,BOOL successful);

/// callback of ble Connect Peripheral wirh error
typedef void(^bleConnectPeripheralWithErrorCallback)(CBPeripheral *peripheral, NSError * _Nullable error);

/// callback of ble Discover Services
typedef void(^bleDiscoverServicesCallback)(CBPeripheral *peripheral,BOOL successful);

/// callback With parameters: CBPeripheral、CBCharacteristic、NSError
typedef void(^bleCharacteristicResultCallback)(CBPeripheral *peripheral,CBCharacteristic *characteristic,NSError * _Nullable error);

/// callback of ble Read OTACharachteristic
typedef void(^bleReadOTACharachteristicCallback)(CBCharacteristic *characteristic,BOOL successful);

/// callback of ble Cancel Connect
typedef void(^bleCancelConnectCallback)(CBPeripheral *peripheral,BOOL successful);

/// callback of ble Cancel All Connect
typedef void(^bleCancelAllConnectCallback)(void);

/// callback of ble Disconnect
typedef void(^bleDisconnectCallback)(CBPeripheral *peripheral,NSError *error);

/// callback of ble Is Ready To Send Write Without Response
typedef void(^bleIsReadyToSendWriteWithoutResponseCallback)(CBPeripheral *peripheral);

/// callback of ble Did Update Value For Characteristic
typedef void(^bleDidUpdateValueForCharacteristicCallback)(CBPeripheral *peripheral,CBCharacteristic *characteristic, NSError * _Nullable error);

/// callback of ble Did Write Value For Characteristic
typedef void(^bleDidWriteValueForCharacteristicCallback)(CBPeripheral *peripheral,CBCharacteristic *characteristic, NSError * _Nullable error);

/// callback of addDevice about start Connect
typedef void(^addDevice_startConnectCallBack)(NSString *identify);

/// callback of addDevice about start Provision
typedef void(^addDevice_startProvisionCallBack)(NSString *identify,UInt16 address);

/// callback of addDevice about capabilitiesPdu response
typedef UInt16 (^addDevice_capabilitiesWithReturnCallBack)(SigProvisioningCapabilitiesPdu *capabilitiesPdu);

/// callback of addDevice about provision Success
typedef void(^addDevice_provisionSuccessCallBack)(NSString *identify,UInt16 address);

/// callback of addDevice about keyBind Success
typedef void(^addDevice_keyBindSuccessCallBack)(NSString *identify,UInt16 address);

/// callback of addDevice about Auto add devices finish
typedef void(^AddDeviceFinishCallBack)(void);

/// 作用：消除CBL2CAPChannel在iOS11以下编译是的警告。
/// Action: Eliminates warnings when CBL2CAPChannel is compiled under iOS11.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunguarded-availability-new"
typedef void(^openChannelResultCallback)(CBPeripheral *peripheral,CBL2CAPChannel * _Nullable channel,NSError * _Nullable error);
#pragma clang diagnostic pop

/// base model of SigLib.
@interface SigModel : NSObject
@end


@interface ModelIDModel : SigModel
/// Group name of model ID.
@property (nonatomic,strong) NSString *modelGroup;
/// Model ID name.
@property (nonatomic,strong) NSString *modelName;
/// The value of model ID.
@property (nonatomic,assign) NSInteger sigModelID;

/// Initialize ModelIDModel object.
/// @param modelGroup Group name of model ID.
/// @param modelName the Model ID name.
/// @param sigModelID The value of model ID.
/// @returns return `nil` when initialize ModelIDModel object fail.
-(instancetype)initWithModelGroup:(NSString *)modelGroup modelName:(NSString *)modelName sigModelID:(NSInteger)sigModelID;

@end


@interface ModelIDs : SigModel
@property (nonatomic,strong) NSArray <ModelIDModel *>*modelIDs;//所有的modelID
@property (nonatomic,strong) NSArray <ModelIDModel *>*defaultModelIDs;//默认keyBind的modelID
@end


/// Default group information
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


/*
 5.1.4 Scheduler
 5.1.4.1 Scheduler overview
 Scheduler provides a means of autonomous change of states of a
 device based on the notion of UTC time and the ISO 8601 calendar
 [15] and a register of defined time points with associated state-
 changing actions. For example, a lamp may automatically turn off
 every day at 2AM, or a coffee machine may make coffee at 6:30AM.
 The scheduler is based on a register (see Section 5.1.4.2) that
 is capable of storing up to sixteen scheduled entries, each
 containing a starting point in local time, that may include values
 that represent multiple values, and an associated action to perform.
 */
/// The Schedule Register state is a 16-entry, zero-based, indexed array of 76-bit values
/// formatted as Scheduled Time. Each entry represents a state-changing event. Time and
/// date fields represent local time.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.139),
/// 5.1.4.2 Schedule Register.
/// @note   The Year, Month, Day, Hour, Minute, and Second fields represent local time (i.e.,
/// after the TAI-UTC Delta and Time Zone Offset have been applied). The fields have the
/// meaning defined in ISO 8601 [15] (which replicates the "Gregorian" calendar in common
/// use). Some of these values can either represent an exact value or a range of values when
/// the scheduled action is performed.
/// @note   The minute and second field values have an enumerated value that represents ‘at
/// a random’ value. This scheduled action shall be triggered once within the corresponding
/// hour or minute.
/// @note   5.1.4.2.1 Schedule Register behavior
/// When the current time is known to the device, which is indicated by a value of the TAI
/// Seconds state (see Section 5.1.1.1) that is greater than zero, the entries in the Schedule
/// Register are tracked. If there is a match between the Time state (see Section 5.1.1) and
/// an entry in the Schedule Register, a corresponding Action indicated by the values in the
/// Action and Scene fields shall be executed.
/// @note   Note:A recommended implementation of the Scheduler should calculate the value
/// of the TAI Seconds (see Section 5.1.1.1) of the next scheduled event and put it in a queue
/// of scheduled events sorted by time. Every second, the first event in the queue is compared
/// with the value of the Time state (see Section 5.1.1). The first event is executed if it is less than
/// or equal to the Time state and then removed from the queue. After execution, the Repeat Flag
/// shall be checked, and the next occurrence of the scheduled event is calculated and put in the
/// queue.
@interface SchedulerModel : SigModel<NSCopying>
/*
 Value              Description
 0x00–0x63          2 least significant digits of the year
 0x64               Any year
 All other values   Prohibited
 */
/// Scheduled year for the action (see Table 5.5)
/// The Year field represents 2 least significant digits of the year of the occurrence of the
/// scheduled event.
@property (nonatomic,assign) UInt64 year;
/*
 Bit    Description
 0      Scheduled in January
 1      Scheduled in February
 2      Scheduled in March
 3      Scheduled in April
 4      Scheduled in May
 5      Scheduled in June
 6      Scheduled in July
 7      Scheduled in August
 8      Scheduled in September
 9      Scheduled in October
 10     Scheduled in November
 11     Scheduled in December
 */
/// Scheduled month for the action (see Table 5.6)
/// The Month field represents the months of the occurrences of the scheduled event.
@property (nonatomic,assign) UInt64 month;
/*
 Value      Description
 0x00       Any day
 0x01–0x1F  Day of the month
 */
/// Scheduled day of the month for the action (see Table 5.7)
/// The Day field represents the day the month of the occurrence of the scheduled event.
/// If the day of the month has a number that is larger than the number of days in the month,
/// then the event occurs in the last day of the month. For example, in February if the day
/// field holds the value 29, the action is triggered on February 28th in a non-leap year or
/// February 29th in a leap year.
@property (nonatomic,assign) UInt64 day;
/*
 Value              Description
 0x00–0x17          Hour of the day (00 to 23 hours)
 0x18               Any hour of the day
 0x19               Once a day (at a random hour)
 All other values   Prohibited
 */
/// Scheduled hour for the action (see Table 5.8)
/// The Hour field represents the hour of the occurrence of the scheduled event.
@property (nonatomic,assign) UInt64 hour;
/*
 Value      Description
 0x00–0x3B  Minute of the hour (00 to 59)
 0x3C       Any minute of the hour
 0x3D       Every 15 minutes (minute modulo 15 is 0) (0, 15, 30, 45)
 0x3E       Every 20 minutes (minute modulo 20 is 0) (0, 20, 40)
 0x3F       Once an hour (at a random minute)
 */
/// Scheduled minute for the action (see Table 5.9)
/// The Minute field represents the minute of the occurrence of the scheduled event.
@property (nonatomic,assign) UInt64 minute;
/*
 Value      Description
 0x00–0x3B  Second of the minute (00 to 59)
 0x3C       Any second of the minute
 0x3D       Every 15 seconds (minute modulo 15 is 0) (0, 15, 30, 45)
 0x3E       Every 20 seconds (minute modulo 20 is 0) (0, 20, 40)
 0x3F       Once an minute (at a random second)
 */
/// Scheduled second for the action (see Table 5.10)
/// The Second field represents the second of the occurrence of the scheduled event.
@property (nonatomic,assign) UInt64 second;
/*
 Bit    Description
 0      Scheduled on Mondays
 1      Scheduled on Tuesdays
 2      Scheduled on Wednesdays
 3      Scheduled on Thursdays
 4      Scheduled on Fridays
 5      Scheduled on Saturdays
 6      Scheduled on Sundays
 */
/// Schedule days of the week for the action (see Table 5.11)
/// The DayOfWeek field represents the days of the week that the scheduled event will trigger.
/// When a bit is set to 1, the scheduled event is executed on that day.
@property (nonatomic,assign) UInt64 week;
/*
 Value              Description
 0x0                Turn Off
 0x1                Turn On
 0x2                Scene Recall
 0xF                No action
 All other values   Reserved for Future Use
 */
/// Action to be performed at the scheduled time (see Table 5.12)
/// The Action field represents an action to be executed for a scheduled event as defined
/// in the table below.
/// When the Action field value is 0x0, this means an action that is an equivalent of receiving
/// by the element a Generic OnOff Set Unacknowledged message (see Section 3.2.1.3) with
/// the OnOff field set to 0x00 and the Transition Time field set to the value of the Transition
/// Time field of the Schedule Register.
/// When the Action field value is 0x1, this means an action that is an equivalent of receiving
/// by the element a Generic OnOff Set Unacknowledged message (see Section 3.2.1.3) with
/// the OnOff field set to 0x01 and the Transition Time field set to the value of the Transition
/// Time field of the Schedule Register.
/// When the Action field value is 0x2, this means an action that is an equivalent of receiving
/// by the element a Scene Recall Unacknowledged message (see Section 5.2.2.4) with the
/// Scene Number field set to the value of the Scene Number field of the Schedule Register
/// and the Transition Time field set to the value of the Transition Time field of the Schedule
/// Register.
/// When the Action field value is 0xF, this means no action is performed.
@property (nonatomic,assign) SchedulerType action;
/// Transition time for this action (see Section 3.1.3)
/// The Transition Time field represents a Transition Time to be used when an action triggered
/// by the scheduler is executed. The format is defined in Section 3.1.3.
@property (nonatomic,assign) UInt64 transitionTime;
/*
 Value              Description
 0x0000             No scene
 All other values   Scene number
 */
/// Scene number to be used for some actions (see Table 5.13)
/// The Scene Number field represents a Scene to be recalled.
@property (nonatomic,assign) UInt64 sceneId;
/// 4 bits, Enumerates (selects) a Schedule Register entry. The valid values for the Index field are 0x0-0xF.
@property (nonatomic,assign) UInt64 schedulerID;
@property (nonatomic,assign) UInt64 schedulerData;

/// get dictionary from SchedulerModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSchedulerModel;

/// Set dictionary to SchedulerModel object.
/// @param dictionary SchedulerModel dictionary object.
- (void)setDictionaryToSchedulerModel:(NSDictionary *)dictionary;

@end


/// The model for caching Bluetooth scan callbacks, with uuid (peripheral. identifier. UUIDString) as the unique identifier.
@interface SigScanRspModel : NSObject
/// advertisement data for key `CBAdvertisementDataServiceDataKey`
@property (nonatomic, strong) NSData *advertisementDataServiceData;
/// MacAddress is subData in advertisement data for key `CBAdvertisementDataManufacturerDataKey`, MacAddress is an obsolete attribute.
@property (nonatomic, strong) NSString *macAddress;
/// company ID, default is 0x0211=529.
@property (nonatomic, assign) UInt16 CID;
/// product ID
@property (nonatomic, assign) UInt16 PID;
/// the unique identifier, (peripheral. identifier. UUIDString).
@property (nonatomic, strong) NSString *uuid;
/// the unicastAddress of provisioned node, this address of unprovision node is invalid.
@property (nonatomic, assign) UInt16 address;
/// the name in advertisement data for key `CBAdvertisementDataLocalNameKey`
@property (nonatomic, strong) NSString *advName;
/// the uuid of unprovision node, is subData in advertisement data for key `CBAdvertisementDataServiceDataKey`. range is 0~15.
@property (nonatomic, strong) NSString *advUuid;
/// the OobInformation of unprovision node, is subData in advertisement data for key `CBAdvertisementDataServiceDataKey`. range is 16~17.
@property (nonatomic, assign) struct OobInformation advOobInformation;
/// Complete data of Bluetooth broadcast package.
@property (nonatomic, strong) NSDictionary<NSString *,id> *advertisementData;
/// `YES` mean provision, `NO` nean unprovision.
@property (nonatomic, assign) BOOL provisioned;
/// the CBPeripheral object of BLE device.
@property (nonatomic, strong) CBPeripheral *peripheral;

/// Initialize SigScanRspModel object.
/// @param peripheral the CBPeripheral object of BLE device.
/// @param advertisementData Complete data of Bluetooth broadcast package.
/// @returns return `nil` when initialize SigScanRspModel object fail.
- (instancetype)initWithPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary<NSString *,id> *)advertisementData;

/// return the identification type of provisioned node.
- (SigIdentificationType)getIdentificationType;

@end


/// The model for caching Remote scan callbacks, with reportNodeUUID as the unique identifier.
@interface SigRemoteScanRspModel : NSObject
/// the unicastAddress report this SigRemoteScanRspModel object.
@property (nonatomic, assign) UInt16 reportNodeAddress;
/// the nodeUuid of this SigRemoteScanRspModel object.
@property (nonatomic, strong) NSData *reportNodeUUID;
/// the RSSI of this SigRemoteScanRspModel object.
@property (nonatomic, assign) int RSSI;//负值
/// the OobInformation of this SigRemoteScanRspModel object.
@property (nonatomic, assign) struct OobInformation oob;
/// the macAddress of this SigRemoteScanRspModel object.
@property (nonatomic, strong) NSString *macAddress;

/// Initialize SigRemoteScanRspModel object.
/// @param parameters Complete data of SigRemoteScanRspModel object.
/// @returns return `nil` when initialize SigRemoteScanRspModel object fail.
- (instancetype)initWithParameters:(NSData *)parameters;

@end


/// the model use for add device VC
@interface AddDeviceModel : SigModel
@property (nonatomic, strong) SigScanRspModel *scanRspModel;//SigRemoteScanRspModel need change to SigScanRspModel.
@property (nonatomic, assign) AddDeviceModelState state;//state of add device

/// Initialize AddDeviceModel object.
/// @param scanRemoteModel SigRemoteScanRspModel object.
/// @returns return `nil` when initialize AddDeviceModel object fail.
- (instancetype)initWithRemoteScanRspModel:(SigRemoteScanRspModel *)scanRemoteModel;

@end


/// the action model of scene
@interface ActionModel : SigModel
@property (nonatomic,assign) UInt16 address;//unicastAddress
@property (nonatomic,assign) DeviceState state;//on off outofline
@property (nonatomic,assign) UInt8 trueBrightness;//1-100
@property (nonatomic,assign) UInt8 trueTemperature;//0-100

/// Initialize ActionModel object.
/// @param node SigNodeModel object.
/// @returns return `nil` when initialize ActionModel object fail.
- (instancetype)initWithNode:(SigNodeModel *)node;

/// Initialize ActionModel object.
/// @param node SigNodeModel object.
/// @param elementAddress elementAddress of node.
/// @returns return `nil` when initialize ActionModel object fail.
- (instancetype)initWithNode:(SigNodeModel *)node elementAddress:(UInt16)elementAddress;

/// Compare two actions to see if they are the same.
/// @param action ActionModel object
/// @returns YES means same, NO means different.
- (BOOL)isSameActionWithAction:(ActionModel *)action;

/// get dictionary from ActionModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfActionModel;

/// Set dictionary to ActionModel object.
/// @param dictionary ActionModel dictionary object.
- (void)setDictionaryToActionModel:(NSDictionary *)dictionary;

@end


static Byte CTByte[] = {(Byte) 0x11, (Byte) 0x02, (Byte) 0x01, (Byte) 0x00, (Byte) 0x32, (Byte) 0x37, (Byte) 0x69, (Byte) 0x00, (Byte) 0x07, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x19, (Byte) 0x01, (Byte) 0x00, (Byte) 0x00, (Byte) 0x02, (Byte) 0x00, (Byte) 0x03, (Byte) 0x00, (Byte) 0x04, (Byte) 0x00, (Byte) 0x05, (Byte) 0x00, (Byte) 0x00, (Byte) 0xfe, (Byte) 0x01, (Byte) 0xfe, (Byte) 0x02, (Byte) 0xfe, (Byte) 0x00, (Byte) 0xff, (Byte) 0x01, (Byte) 0xff, (Byte) 0x00, (Byte) 0x12, (Byte) 0x01, (Byte) 0x12, (Byte) 0x00, (Byte) 0x10, (Byte) 0x02, (Byte) 0x10, (Byte) 0x04, (Byte) 0x10, (Byte) 0x06, (Byte) 0x10, (Byte) 0x07, (Byte) 0x10, (Byte) 0x03, (Byte) 0x12, (Byte) 0x04, (Byte) 0x12, (Byte) 0x06, (Byte) 0x12, (Byte) 0x07, (Byte) 0x12, (Byte) 0x00, (Byte) 0x13, (Byte) 0x01, (Byte) 0x13, (Byte) 0x03, (Byte) 0x13, (Byte) 0x04, (Byte) 0x13, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x02, (Byte) 0x00, (Byte) 0x02, (Byte) 0x10, (Byte) 0x06, (Byte) 0x13};
static Byte HSLByte[] = {(Byte) 0x11, (Byte) 0x02, (Byte) 0x02, (Byte) 0x00, (Byte) 0x33, (Byte) 0x33, (Byte) 0x69, (Byte) 0x00, (Byte) 0x07, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x0E, (Byte) 0x01, (Byte) 0x00, (Byte) 0x00, (Byte) 0x02, (Byte) 0x00, (Byte) 0x03, (Byte) 0x00, (Byte) 0x00, (Byte) 0xFE, (Byte) 0x00, (Byte) 0xFF, (Byte) 0x00, (Byte) 0x10, (Byte) 0x02, (Byte) 0x10, (Byte) 0x04, (Byte) 0x10, (Byte) 0x06, (Byte) 0x10, (Byte) 0x07, (Byte) 0x10, (Byte) 0x00, (Byte) 0x13, (Byte) 0x01, (Byte) 0x13, (Byte) 0x07, (Byte) 0x13, (Byte) 0x08, (Byte) 0x13, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x02, (Byte) 0x00, (Byte) 0x02, (Byte) 0x10, (Byte) 0x0A, (Byte) 0x13, (Byte) 0x00, (Byte) 0x00, (Byte) 0x02, (Byte) 0x00, (Byte) 0x02, (Byte) 0x10, (Byte) 0x0B, (Byte) 0x13};
static Byte PanelByte[] = {(Byte) 0x11, (Byte) 0x02, (Byte) 0x07, (Byte) 0x00, (Byte) 0x32, (Byte) 0x37, (Byte) 0x69, (Byte) 0x00, (Byte) 0x07, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00, (Byte) 0x02, (Byte) 0x00, (Byte) 0x03, (Byte) 0x00, (Byte) 0x04, (Byte) 0x00, (Byte) 0x05, (Byte) 0x00, (Byte) 0x00, (Byte) 0xfe, (Byte) 0x01, (Byte) 0xfe, (Byte) 0x02, (Byte) 0xfe, (Byte) 0x00, (Byte) 0xff, (Byte) 0x01, (Byte) 0xff, (Byte) 0x00, (Byte) 0x12, (Byte) 0x01, (Byte) 0x12, (Byte) 0x00, (Byte) 0x10, (Byte) 0x03, (Byte) 0x12, (Byte) 0x04, (Byte) 0x12, (Byte) 0x06, (Byte) 0x12, (Byte) 0x07, (Byte) 0x12, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00, (Byte) 0x11, (Byte) 0x02, (Byte) 0x01, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x05, (Byte) 0x01, (Byte) 0x00, (Byte) 0x10, (Byte) 0x03, (Byte) 0x12, (Byte) 0x04, (Byte) 0x12, (Byte) 0x06, (Byte) 0x12, (Byte) 0x07, (Byte) 0x12, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x05, (Byte) 0x01, (Byte) 0x00, (Byte) 0x10, (Byte) 0x03, (Byte) 0x12, (Byte) 0x04, (Byte) 0x12, (Byte) 0x06, (Byte) 0x12, (Byte) 0x07, (Byte) 0x12, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00};
static Byte LPNByte[] = {(Byte) 0x11, (Byte) 0x02, (Byte) 0x01, (Byte) 0x02, (Byte) 0x33, (Byte) 0x33, (Byte) 0x69, (Byte) 0x00, (Byte) 0x0a, (Byte) 0x00, (Byte) 0x00, (Byte) 0x00, (Byte) 0x05, (Byte) 0x01, (Byte) 0x00, (Byte) 0x00, (Byte) 0x02, (Byte) 0x00, (Byte) 0x03, (Byte) 0x00, (Byte) 0x00, (Byte) 0x10, (Byte) 0x02, (Byte) 0x10, (Byte) 0x11, (Byte) 0x02, (Byte) 0x00, (Byte) 0x00};


/// The DeviceTypeModel caching CID、PID and defaultCompositionData, use for fast provision and default bound.
@interface DeviceTypeModel : SigModel
@property (nonatomic, assign) UInt16 CID;//CID
@property (nonatomic, assign) UInt16 PID;//PID
@property (nonatomic, strong) SigPage0 *defaultCompositionData;//default Composition Data

/// Initialize DeviceTypeModel object.
/// @param cid company ID
/// @param pid product ID
/// @param cpsData composition data
/// @returns return `nil` when initialize DeviceTypeModel object fail.
- (instancetype)initWithCID:(UInt16)cid PID:(UInt16)pid compositionData:(nullable NSData *)cpsData;

/// Set composition Data to DeviceTypeModel.
/// @param compositionData composition data
- (void)setCompositionData:(NSData *)compositionData;

@end


/// The config parameters of add device.
@interface SigAddConfigModel : SigModel
/// The CBPeripheral object of BLE device.
@property (nonatomic, strong) CBPeripheral *peripheral;
/// The mesh network key.
@property (nonatomic, strong) NSData *networkKey;
/// The mesh network index.
@property (nonatomic, assign) UInt16 netkeyIndex;
/// The mesh application key.
@property (nonatomic, strong) NSData *appKey;
/// The mesh application index.
@property (nonatomic, assign) UInt16 appkeyIndex;
/// The static OOB data for static OOB provision.
@property (nonatomic, strong, nullable) NSData *staticOOBData;
/// The type of keybind, KeyBindType_Normal or KeyBindType_Fast.
@property (nonatomic, assign) KeyBindType keyBindType;
/// The product ID of unprovision node.
@property (nonatomic, assign) UInt16 productID;
/// The composition data of unprovision node.
@property (nonatomic, strong, nullable) NSData *cpsData;
/// New callback for start connect.
@property (nonatomic, copy) addDevice_startConnectCallBack startConnectBlock;
/// New callback for start provision.
@property (nonatomic, copy) addDevice_startProvisionCallBack startProvisionBlock;

/// Initialize SigAddConfigModel object.
/// - Parameters:
///   - peripheral: The CBPeripheral object of BLE device.
///   - networkKey: The mesh network key.
///   - netkeyIndex: The mesh network index.
///   - appKey: The mesh application key.
///   - appkeyIndex: The mesh application index.
///   - staticOOBData: The static OOB data for static OOB provision.
///   - keyBindType: The type of keybind, KeyBindType_Normal or KeyBindType_Fast.
///   - productID: The product ID of unprovision node.
///   - cpsData: The composition data of unprovision node.
/// - returns:
/// return `nil` when initialize SigAddConfigModel object fail.
- (instancetype)initWithCBPeripheral:(CBPeripheral *)peripheral networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appKey:(NSData *)appKey appkeyIndex:(UInt16)appkeyIndex staticOOBData:(NSData * _Nullable)staticOOBData keyBindType:(KeyBindType)keyBindType productID:(UInt16)productID cpsData:(NSData * _Nullable)cpsData;

@end


/// The ini command for developer.
/// sig model struct of firmware: mesh_bulk_cmd_par_t.
/// vendor model struct of firmware: mesh_vendor_par_ini_t.
/// It is sig model command when vendorId is 0, and It is vendor model command when vendorId isn't 0.
/// sig model config: netkeyIndex, appkeyIndex, retryCount, responseMax, address, opcode, commandData.
@interface IniCommandModel : SigModel
/// The mesh network index.
@property (nonatomic, assign) UInt16 netkeyIndex;
/// The mesh application index.
@property (nonatomic, assign) UInt16 appkeyIndex;
/// The retry count of command, only for reliable command.
@property (nonatomic, assign) UInt8 retryCount;
/// The response max count of command, only for reliable command.
@property (nonatomic, assign) UInt8 responseMax;
/// The destination address.
@property (nonatomic, assign) UInt16 address;
/// may be Uint16 or 16 bytes Label UUID.
@property (nonatomic, strong) SigMeshAddress *meshAddressModel;
/// The opcode of command, eg:
/// SigGenericOnOffSet:0x8202. SigGenericOnOffSetUnacknowledged:0x8203.
/// VendorOnOffSet:0xC4, VendorOnOffSetUnacknowledged:0xC3.
@property (nonatomic, assign) UInt16 opcode;
/// The vendor ID of command.
/// 0 means sig model command, other means vendor model command.
@property (nonatomic, assign) UInt16 vendorId;
/// The response opcode of command, only for reliable command.
/// eg: The response opcode of VendorOnOffSet is 0xC4.
@property (nonatomic, assign) UInt8 responseOpcode;
/// Does the command require a tid number? YES means need tid, NO means needn`t tid.
/// when needTid is NO, parameter tidPosition and tid is invalid.
@property (nonatomic, assign) BOOL needTid;
/// The position of tid in the ini command.
@property (nonatomic, assign) UInt8 tidPosition;
/// The value of tid in the ini command.
@property (nonatomic, assign) UInt8 tid;
/// The Hex command data.
/// eg: SigGenericOnOffSet: commandData of turn on without TransitionTime and delay is {0x01,0x00,0x00}.
/// eg: SigGenericOnOffSet: commandData of turn off without TransitionTime and delay is {0x00,0x00,0x00}
@property (nonatomic, strong, nullable) NSData *commandData;
/// The response message callback by this handle.
@property (nonatomic, copy) responseAllMessageBlock responseCallBack;
/// The timeout of this command.
@property (nonatomic,assign) NSTimeInterval timeout;
/// YES means use netkey+deviceKey, NO means use netkey+appKey. Default is NO.
@property (nonatomic,assign) BOOL isEncryptByDeviceKey;

/*
 The function of these three parameters is to configure the key and ivIndex actually
 used by the current SDKLibCommand instruction. Only the fastProvision process uses
 special keys and ivIndex, and other instructions use default values.
 */
/// The SigNetkeyModel of this command.
@property (nonatomic,strong) SigNetkeyModel *curNetkey;
/// The SigAppkeyModel of this command.
@property (nonatomic,strong) SigAppkeyModel *curAppkey;
/// The SigIvIndex of this command.
@property (nonatomic,strong) SigIvIndex *curIvIndex;

/// Initialize sig model IniCommandModel object.
/// - Parameters:
///   - netkeyIndex: The mesh network index.
///   - appkeyIndex: The mesh application index.
///   - retryCount: The retry count of command, only for reliable command.
///   - responseMax: The response max count of command, only for reliable command.
///   - address: The destination address.
///   - opcode: The opcode of command.
/// SigGenericOnOffSet:0x8202. SigGenericOnOffSetUnacknowledged:0x8203.
/// VendorOnOffSet:0xC4, VendorOnOffSetUnacknowledged:0xC3.
///   - commandData: The Hex command data.
///   eg: SigGenericOnOffSet: commandData of turn on without TransitionTime and delay is {0x01,0x00,0x00}.
///   eg: SigGenericOnOffSet: commandData of turn off without TransitionTime and delay is {0x00,0x00,0x00}
/// - returns:
/// return `nil` when initialize sig model IniCommandModel object fail.
- (instancetype)initSigModelIniCommandWithNetkeyIndex:(UInt16)netkeyIndex appkeyIndex:(UInt16)appkeyIndex retryCount:(UInt8)retryCount responseMax:(UInt8)responseMax address:(UInt16)address opcode:(UInt16)opcode commandData:(NSData *)commandData;

/// Initialize vendor model IniCommandModel object.
/// - Parameters:
///   - netkeyIndex: The mesh network index.
///   - appkeyIndex: The mesh application index.
///   - retryCount: The retry count of command, only for reliable command.
///   - responseMax: The response max count of command, only for reliable command.
///   - address: The destination address.
///   - opcode: The opcode of command.
/// SigGenericOnOffSet:0x8202. SigGenericOnOffSetUnacknowledged:0x8203.
/// VendorOnOffSet:0xC4, VendorOnOffSetUnacknowledged:0xC3.
///   - vendorId: The vendor ID of command.
/// 0 means sig model command, other means vendor model command.
///   - responseOpcode: The response opcode of command, only for reliable command.
/// eg: The response opcode of VendorOnOffSet is 0xC4.
///   - needTid: The position of tid in the ini command.
///   when needTid is NO, parameter tidPosition and tid is invalid.
///   - tidPosition: The position of tid in the ini command.
///   - tid: The value of tid in the ini command.
///   - commandData: The Hex command data.
///   eg: SigGenericOnOffSet: commandData of turn on without TransitionTime and delay is {0x01,0x00,0x00}.
///   eg: SigGenericOnOffSet: commandData of turn off without TransitionTime and delay is {0x00,0x00,0x00}
/// - returns:
/// return `nil` when initialize vendor model IniCommandModel object fail.
- (instancetype)initVendorModelIniCommandWithNetkeyIndex:(UInt16)netkeyIndex appkeyIndex:(UInt16)appkeyIndex retryCount:(UInt8)retryCount responseMax:(UInt8)responseMax address:(UInt16)address opcode:(UInt8)opcode vendorId:(UInt16)vendorId responseOpcode:(UInt8)responseOpcode needTid:(BOOL)needTid tidPosition:(UInt8)tidPosition tid:(UInt8)tid commandData:(nullable NSData *)commandData;

/// Initialize IniCommandModel object with complete Hex iniCommand data.
/// - Parameter iniCommandData: complete Hex iniCommand data.
/// eg: "a3ff000000000200ffffc21102c4020100"
/// - returns:
/// return `nil` when initialize IniCommandModel object fail.
- (instancetype)initWithIniCommandData:(NSData *)iniCommandData;

@end


/// The Receivers List field shall contain entries from the Distribution Receivers List state.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.98),
/// 8.4.2.5 Firmware Distribution Receivers List.
@interface SigUpdatingNodeEntryModel : SigModel
/// least significant bits of the unicast address of the Updating node. Size is 15 bits.
/// The Address field shall indicate the address of the Updating node.
@property (nonatomic, assign) UInt16 address;
/// Retrieved Update Phase state of the Updating node. Size is 4 bits.
/// The Retrieved Update Phase field shall indicate the Update Phase state of the Firmware
/// Update Server identified by the Address field. The values for this field are defined in Table 8.8.
@property (nonatomic, assign) SigFirmwareUpdatePhaseType retrievedUpdatePhase;
/// Status of the last operation with the Firmware Update Server. Size is 3 bits.
/// The Update Status field shall indicate the status of the last operation between the Firmware
/// Update Client and the Firmware Update Server identified by the Address field. The values
/// for this field are defined in Table 8.23.
@property (nonatomic, assign) SigFirmwareUpdateServerAndClientModelStatusType updateStatus;
/// Status of the last operation with the BLOB Transfer Server. Size is 4 bits.
/// The Transfer Status field shall indicate the status of the last operation between the BLOB
/// Transfer Client and the BLOB Transfer Server identified by the Address field. The values
/// for this field are defined in Table 7.17.
@property (nonatomic, assign) SigBLOBTransferStatusType transferStatus;
/// Progress of the BLOB transfer in 2 percent increments. Size is 6 bits.
/// The Transfer Progress field shall indicate the progress of the firmware image transfer.
/// The value is multiplied by 2 to calculate the percentage of the firmware image that has
/// been delivered and confirmed. The range of allowed values for the Transfer Progress
/// field is 0x0 to 0x32. The values 0x33 to 0x3F are prohibited.
@property (nonatomic, assign) UInt8 transferProgress;
/// Index of the firmware image on the Firmware Information List state that is being
/// updated. Size is 8 bits.
/// The Update Firmware Image Index field shall identify the index of the firmware
/// image on the Firmware Information List state of the Updating node that is being updated.
@property (nonatomic, assign) UInt8 updateFirmwareImageIndex;
/// The Hex data of SigUpdatingNodeEntryModel.
@property (nonatomic, strong) NSData *parameters;

/// Initialize SigUpdatingNodeEntryModel object.
/// - Parameters:
///   - address: least significant bits of the unicast address of the Updating node. Size is 15 bits.
/// The Address field shall indicate the address of the Updating node.
///   - retrievedUpdatePhase: Retrieved Update Phase state of the Updating node. Size is 4 bits.
/// The Retrieved Update Phase field shall indicate the Update Phase state of the Firmware
/// Update Server identified by the Address field. The values for this field are defined in Table 8.8.
///   - updateStatus: Status of the last operation with the Firmware Update Server. Size is 3 bits.
/// The Update Status field shall indicate the status of the last operation between the Firmware
/// Update Client and the Firmware Update Server identified by the Address field. The values
/// for this field are defined in Table 8.23.
///   - transferStatus: Status of the last operation with the BLOB Transfer Server. Size is 4 bits.
/// The Transfer Status field shall indicate the status of the last operation between the BLOB
/// Transfer Client and the BLOB Transfer Server identified by the Address field. The values
/// for this field are defined in Table 7.17.
///   - transferProgress: Progress of the BLOB transfer in 2 percent increments. Size is 6 bits.
/// The Transfer Progress field shall indicate the progress of the firmware image transfer.
/// The value is multiplied by 2 to calculate the percentage of the firmware image that has
/// been delivered and confirmed. The range of allowed values for the Transfer Progress
/// field is 0x0 to 0x32. The values 0x33 to 0x3F are prohibited.
///   - updateFirmwareImageIndex: Index of the firmware image on the Firmware Information List state that is being
/// updated. Size is 8 bits.
/// The Update Firmware Image Index field shall identify the index of the firmware
/// image on the Firmware Information List state of the Updating node that is being updated.
/// - returns:
/// return `nil` when initialize SigUpdatingNodeEntryModel object fail.
- (instancetype)initWithAddress:(UInt16)address retrievedUpdatePhase:(SigFirmwareUpdatePhaseType)retrievedUpdatePhase updateStatus:(SigFirmwareUpdateServerAndClientModelStatusType)updateStatus transferStatus:(SigBLOBTransferStatusType)transferStatus transferProgress:(UInt8)transferProgress updateFirmwareImageIndex:(UInt8)updateFirmwareImageIndex;

/// Initialize SigUpdatingNodeEntryModel object with Hex data.
/// - Parameter parameters: Hex data of SigUpdatingNodeEntryModel object.
/// - returns:
/// return `nil` when initialize SigUpdatingNodeEntryModel object fail.
- (instancetype)initWithParameters:(NSData *)parameters;

/// Get SigUpdatingNodeEntryModel object describe string.
- (NSString *)getDetailString;

@end


/// The Firmware Information Entry field shall identify the information for a
/// firmware subsystem on the node from the Firmware Information List state.
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.92),
/// 8.4.1.2 Firmware Update Information Status.
@interface SigFirmwareInformationEntryModel : SigModel
/// Length of the Current Firmware ID field.
/// The Current Firmware ID Length field shall indicate the length of the Current
/// Firmware ID field.
@property (nonatomic,assign) UInt8 currentFirmwareIDLength;
/// Identifies the firmware image on the node or any subsystem on the node.
/// Size is variable.
/// The Current Firmware ID field shall contain the Firmware ID of a firmware
/// image installed on the node. The format of this field is defined in Table 8.4.
@property (nonatomic,strong) NSData *currentFirmwareID;
/// Length of the Update URI field.
/// The Update URI Length field shall indicate the length of the Update URI field.
@property (nonatomic,assign) UInt8 updateURILength;
/// URI used to retrieve a new firmware image (C.1)Size is 1 ~ 255.
/// C.1: If the value of the Update URI Length field is greater than 0, the Update
/// URI field shall be present.
/// If present, the Update URI field shall indicate the URI link used to retrieve the
/// firmware image.
@property (nonatomic,strong) NSData *updateURL;
/// The Hex data of SigFirmwareInformationEntryModel.
@property (nonatomic,strong) NSData *parameters;

/// Initialize SigFirmwareInformationEntryModel object with Hex data.
/// - Parameter parameters: Hex data of SigFirmwareInformationEntryModel object.
/// - returns:
/// return `nil` when initialize SigFirmwareInformationEntryModel object fail.
- (instancetype)initWithParameters:(NSData *)parameters;

/// Get FirmwareID String
- (NSString *)getFirmwareIDString;

/// Get UpdateURI String
- (NSString *)getUpdateURIString;

@end


/// The Receiver Entry field
/// @note   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.97),
/// 8.4.2.1 Firmware Distribution Receivers Add.
/// Table 8.34: The format of the Receiver Entry field
@interface SigReceiverEntryModel : SigModel
/// The unicast address of the Updating node.
/// The Address field shall contain the unicast address of an Updating node.
@property (nonatomic,assign) UInt16 address;
/// The index of the firmware image in the Firmware Information List state to be updated.
/// The Update Firmware Image Index field shall identify the index of a firmware image in
/// the Firmware Information List state of the Updating node to be updated.
@property (nonatomic,assign) UInt8 updateFirmwareImageIndex;
/// The Hex data of SigReceiverEntryModel.
@property (nonatomic,strong) NSData *parameters;

/// Initialize SigReceiverEntryModel object.
/// - Parameters:
///     - address: The unicast address of the Updating node.
/// The Address field shall contain the unicast address of an Updating node.
///     - updateFirmwareImageIndex: The index of the firmware image in the Firmware Information List state to be updated.
/// The Update Firmware Image Index field shall identify the index of a firmware image in
/// the Firmware Information List state of the Updating node to be updated.
/// - returns:
/// return `nil` when initialize SigReceiverEntryModel object fail.
- (instancetype)initWithAddress:(UInt16)address updateFirmwareImageIndex:(UInt8)updateFirmwareImageIndex;

/// Initialize SigReceiverEntryModel object with hex data.
/// - Parameters:
///     - parameters: The Hex data of SigReceiverEntryModel.
/// - returns:
/// return `nil` when initialize SigReceiverEntryModel object fail.
- (instancetype)initWithParameters:(NSData *)parameters;

@end


/// The Generic Default Transition Time state determines how long an element shall take to transition
/// from a present state to a new state (see Section 1.4.1.1). This is a 1-octet value that consists of two
/// fields: a 2-bit bit field representing the step resolution and a 6-bit bit field representing the number
/// of transition steps.
/// @note   This mechanism covers a wide range of times that may be required by different applications:
/// • For 100 millisecond step resolution, the range is 0 through 6.2 seconds.
/// • For 1 second step resolution, the range is 0 through 62 seconds.
/// • For 10 seconds step resolution, the range is 0 through 620 seconds (10.5 minutes).
/// • For 10 minutes step resolution, the range is 0 through 620 minutes (10.5 hours). The Generic Default
/// Transition Time is calculated using the following formula:
/// Generic Default Transition Time = Default Transition Step Resolution * Default Transition Number of Steps.
/// Default values for the Generic Default Transition Step Resolution and the Default Transition Number of
/// Steps are implementation-specific and are defined by a device manufacturer.
/// @note   - seeAlso: Mesh_Model_Specification v1.0.pdf (page.29),
/// 3.1.3 Generic Default Transition Time.
/// Table 3.3: Generic Default Transition Time state format
@interface SigTransitionTime : NSObject
/// The number of Steps, 6-bit value.
/// The Default Transition Number of Steps field is a 6-bit value representing the number of transition steps.
/// The field values represent the states defined in the following table.
/// Value            Description
/// 0x00             The Generic Default Transition Time is immediate.
/// 0x01–0x3E   The number of steps.
/// 0x3F             The value is unknown. The state cannot be set to this value,
/// but an element may report an unknown value if a transition is higher than 0x3E or not determined.
@property (nonatomic,assign) UInt8 steps;

/// The resolution of the Default Transition Number of Steps field.
/// The Default Transition Step Resolution field is a 2-bit bit field that determines the resolution of the Generic
/// Default Transition Time state. The field values represent the states defined in the following table.
/// Value            Description
/// 0b00            The Default Transition Step Resolution is 100 milliseconds
/// 0b01            The Default Transition Step Resolution is 1 second
/// 0b10            The Default Transition Step Resolution is 10 seconds
/// 0b11            The Default Transition Step Resolution is 10 minutes
@property (nonatomic,assign) SigStepResolution stepResolution;

/// The transition time in milliseconds.
@property (nonatomic,assign) int milliseconds;

/// The transition time as `TimeInterval` in seconds.
@property (nonatomic,assign) NSTimeInterval interval;
/// The hex data of SigTransitionTime.
@property (nonatomic,assign) UInt8 rawValue;

/// Initialize SigTransitionTime object.
/// @param rawValue The hex data of SigTransitionTime.
- (instancetype)initWithRawValue:(UInt8)rawValue;

/// Initialize SigTransitionTime object.
/// - parameter steps: The number of Steps, 6-bit value.
/// The Default Transition Number of Steps field is a 6-bit value representing the number of transition steps.
/// The field values represent the states defined in the following table.
/// - parameter stepResolution: The resolution of the Default Transition Number of Steps field.
/// The Default Transition Step Resolution field is a 2-bit bit field that determines the resolution of the Generic
/// Default Transition Time state. The field values represent the states defined in the following table.
/// - returns:
/// return `nil` when initialize SigTransitionTime object fail.
- (instancetype)initWithSetps:(UInt8)steps stepResolution:(SigStepResolution)stepResolution;

@end


@interface SigMeshAddress : NSObject
/// 16-bit address.
@property (nonatomic, assign) UInt16 address;
/// Virtual label UUID.
@property (nonatomic, strong, nullable) CBUUID *virtualLabel;

- (instancetype)initWithHex:(NSString *)hex;

/// Creates a Mesh Address. For virtual addresses use `initWithVirtualLabel:` instead.
/// @param address address
-(instancetype)initWithAddress:(UInt16)address;

/// Creates a Mesh Address based on the virtual label.
- (instancetype)initWithVirtualLabel:(CBUUID *)virtualLabel;

- (NSString *)getHex;

@end


/// The object is used to describe the number of times a message is published and the interval between
/// retransmissions of the published message.
@interface SigRetransmit : NSObject
/// Number of retransmissions for network messages. The value is in range from 0 to 7, where 0 means no retransmissions.
@property (nonatomic,assign) UInt8 count;
/// The interval (in milliseconds) between retransmissions (50...3200 with step 50).
@property (nonatomic,assign) UInt16 interval;
/// Initialize SigRetransmit object.
/// - Parameters:
///   - publishRetransmitCount: Number of retransmissions for network messages. The value is in range from 0 to 7, where 0 means no retransmissions.
///   - intervalSteps: The interval (in milliseconds) between retransmissions (50...3200 with step 50).
/// - returns:
/// return `nil` when initialize SigRetransmit object fail.
- (instancetype)initWithPublishRetransmitCount:(UInt8)publishRetransmitCount intervalSteps:(UInt8)intervalSteps;

/// Retransmission steps, from 0 to 31. Use `interval` to get the interval in ms.
- (UInt8)steps;

@end


/// @note   - seeAlso: Mesh_v1.0.pdf (page.158), 4.3.2.16 Config Model Publication Set.
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
@property (nonatomic,strong)  SigRetransmit *retransmit;

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
/// The TAI Seconds field identifies the TAI Seconds state (see Section 5.1.1.1).
/// The TAI Seconds state is the current TAI time in seconds after the epoch
/// 2000-01-01T00:00:00 TAI (1999-12-31T23:59:28 UTC).
/// For example, the value 0x20E5369D represents the 2017-06-27T15:30:37 TAI (15:30:00 UTC).
/// When an element does not know the time, a special value of 0x0000000000 is used.
@property (nonatomic, assign) UInt64 TAISeconds;
/// The sub-second time in units of 1/256th second.
/// The Subsecond field identifies the Subsecond state (see Section 5.1.1.2).
/// The Subsecond is a fractional part of the TAI time, in units of 1/256th seconds. An implementation
/// may increment this field by more than one unit (i.e. the mechanism it uses may have a larger
/// granularity) and/or by different amounts at each increment.
@property (nonatomic, assign) UInt8 subSeconds;
/// The estimated uncertainty in 10 millisecond steps.
/// The Uncertainty field identifies the Time Uncertainty state (see Section 5.1.1.3).
/// The Uncertainty field represents the accumulated uncertainty of the Mesh Timestamp in
/// 10- millisecond steps. It includes the Uncertainty of the time source and the accumulated
/// uncertainty resulting from the local clock drift since the last update. Value 255 represents
/// uncertainty of 2.55 seconds or more.
@property (nonatomic, assign) UInt8 uncertainty;
/// 1 bit, 0 = NO Time Authority, 1 = Time Authority.
/// The Time Authority field identifies the Time Authority state (see Section 5.1.1.4).
/// The Time Authority bit represents whether the element has a reliable source of TAI, such as
/// a GPS receiver or an NTP-synchronized clock. Time Authority = 1 is when the device itself
/// has access to a reliable and trusted time source, such as NTP or GPS, or has been given the
/// TAI time by a Provisioner (using the Time Setup Server model and a DevKey – see Section 5.3.2).
/// Note: Many time sources do not provide TAI directly, but it can be derived from UTC (if the current
/// TAI-UTC Delta is known) or from GPS time (which is always 19 seconds behind TAI).
@property (nonatomic, assign) UInt8 timeAuthority;
/// 15 bits, Current difference between TAI and UTC in seconds.
/// The TAI-UTC Delta field identifies the TAI-UTC Delta Current state (see Section 5.1.1.8).
/// The TAI-UTC Delta Current state represents the value: current_TAI minus current_UTC.
/// For example, on 2017-01-19, this value equals +37. The valid range is -255 through +32512
/// (i.e., 0x00FF represents a value of 0 and 0x7FFF represents a value of 32512).
@property (nonatomic, assign) UInt16 TAI_UTC_Delta;
/// The local time zone offset in 15-minute increments.
/// The Time Zone Offset field shall be set to the Time Zone Offset Current state (see Section 5.1.1.5).
/// The Time Zone Offset Current field represents the current zone offset in 15-minute increments.
/// The value is the number of 15-minute increments from UTC. Positive numbers are eastwards.
/// The state is a uint8 value representing the valid range of -64 through +191 (i.e., 0x40 represents
/// a value of 0 and 0xFF represents a value of 191).
/// Note: The offset representation with a range -64 through +191 (-16 through +47.75 hours) is in anticipation
/// of proposals dealing with the leap seconds issue by replacing the leap seconds with local zone changes,
/// which means that by the year 5000 the UK will be in zone +8 in the winter while New Zealand will be +21
/// in the summer with some places +22 or more.
@property (nonatomic, assign) UInt8 timeZoneOffset;

/// Initialize SigTimeModel object.
/// - Parameters:
///   - TAISeconds: 40 bits, The current TAI time in seconds.
/// The TAI Seconds field identifies the TAI Seconds state (see Section 5.1.1.1).
/// The TAI Seconds state is the current TAI time in seconds after the epoch
/// 2000-01-01T00:00:00 TAI (1999-12-31T23:59:28 UTC).
/// For example, the value 0x20E5369D represents the 2017-06-27T15:30:37 TAI (15:30:00 UTC).
/// When an element does not know the time, a special value of 0x0000000000 is used.
///   - subSeconds: The sub-second time in units of 1/256th second.
/// The Subsecond field identifies the Subsecond state (see Section 5.1.1.2).
/// The Subsecond is a fractional part of the TAI time, in units of 1/256th seconds. An implementation
/// may increment this field by more than one unit (i.e. the mechanism it uses may have a larger
/// granularity) and/or by different amounts at each increment.
///   - uncertainty: The estimated uncertainty in 10 millisecond steps.
/// The Uncertainty field identifies the Time Uncertainty state (see Section 5.1.1.3).
/// The Uncertainty field represents the accumulated uncertainty of the Mesh Timestamp in 10- millisecond steps.
/// It includes the Uncertainty of the time source and the accumulated uncertainty resulting from the local clock drift
/// since the last update. Value 255 represents uncertainty of 2.55 seconds or more.
///   - timeAuthority: 1 bit, 0 = NO Time Authority, 1 = Time Authority.
/// The Time Authority field identifies the Time Authority state (see Section 5.1.1.4).
/// The Time Authority bit represents whether the element has a reliable source of TAI, such as a GPS receiver or an
/// NTP-synchronized clock. Time Authority = 1 is when the device itself has access to a reliable and trusted time
/// source, such as NTP or GPS, or has been given the TAI time by a Provisioner (using the Time Setup Server model
/// and a DevKey – see Section 5.3.2).
/// Note: Many time sources do not provide TAI directly, but it can be derived from UTC (if the current TAI-UTC Delta
/// is known) or from GPS time (which is always 19 seconds behind TAI).
///   - TAI_UTC_Delta: 15 bits, Current difference between TAI and UTC in seconds.
/// The TAI-UTC Delta field identifies the TAI-UTC Delta Current state (see Section 5.1.1.8).
/// The TAI-UTC Delta Current state represents the value: current_TAI minus current_UTC. For example, on 2017-01-19,
/// this value equals +37. The valid range is -255 through +32512 (i.e., 0x00FF represents a value of 0 and 0x7FFF
/// represents a value of 32512).
///   - timeZoneOffset: The local time zone offset in 15-minute increments.
/// The Time Zone Offset field shall be set to the Time Zone Offset Current state (see Section 5.1.1.5).
/// The Time Zone Offset Current field represents the current zone offset in 15-minute increments. The value is the number
/// of 15-minute increments from UTC. Positive numbers are eastwards. The state is a uint8 value representing the valid
/// range of -64 through +191 (i.e., 0x40 represents a value of 0 and 0xFF represents a value of 191).
/// Note: The offset representation with a range -64 through +191 (-16 through +47.75 hours) is in anticipation of proposals
/// dealing with the leap seconds issue by replacing the leap seconds with local zone changes, which means that by the
/// year 5000 the UK will be in zone +8 in the winter while New Zealand will be +21 in the summer with some places +22 or more.
/// - returns:
/// return `nil` when initialize SigTimeModel object fail.
- (instancetype)initWithTAISeconds:(UInt64)TAISeconds subSeconds:(UInt8)subSeconds uncertainty:(UInt8)uncertainty timeAuthority:(UInt8)timeAuthority TAI_UTC_Delta:(UInt16)TAI_UTC_Delta timeZoneOffset:(UInt8)timeZoneOffset;

/// Initialize SigTimeModel object.
/// - Parameter parameters: The hex of SigTimeModel object.
/// - returns:
/// return `nil` when initialize SigTimeModel object fail.
- (instancetype)initWithParameters:(NSData *)parameters;

/// Get hex data of SigTimeModel object.
- (NSData *)getTimeParameters;

@end


/// The Sensor Descriptor state represents the attributes describing the sensor data.
/// This state does not change throughout the lifetime of an element.
/// 4.1.1 Sensor Descriptor
/// - seeAlso: Mesh_Model_Specification v1.0.pdf  (page.105)
@interface SigSensorDescriptorModel : NSObject
/// The Sensor Property ID field is a 2-octet value referencing a device property that describes the meaning
/// and the format of data reported by a sensor (see Section 0). A measurement reported by a sensor may
/// be represented as a single data point (see Section 4.1.4) or as a column of a series of data points, such
/// as a histogram (see Section 4.1.5). This representation is also determined by the device property.
/// The values for the field are defined in the following table.
/// Value                       Meaning
/// 0x0000                    Prohibited
/// 0x0001–0xFFFF      Identifier of a device property (see Section 2.1)
@property (nonatomic, assign) UInt16 propertyID;
/// The Sensor Positive Tolerance field is a 12-bit value representing the magnitude of a possible positive error
/// associated with the measurements that the sensor is reporting. For cases in which the tolerance information
/// is not available, a special number has been assigned to indicate “Unspecified”.
/// The values for the state are defined in the following table.
/// Value                   Description
/// 0x000                   Unspecified
/// 0x001–0xFFF       The positive tolerance of the sensor. See Note below.
/// Note: The magnitude of a possible positive error associated with the reported data (expressed as a percentage)
/// is derived using the following formula:
/// 𝑃𝑜𝑠𝑠𝑖𝑏𝑙𝑒 𝑃𝑜𝑠𝑖𝑡𝑖𝑣𝑒 𝐸𝑟𝑟𝑜𝑟 [%] = 100 [%] ∗ 𝑃𝑜𝑠𝑖𝑡𝑖𝑣𝑒 𝑇𝑜𝑙𝑒𝑟𝑎𝑛𝑐𝑒 / 4095
@property (nonatomic, assign) UInt16 positiveTolerance;
/// The Sensor Negative Tolerance field is a 12-bit value representing the magnitude of a possible negative error
/// associated with the measurements that the sensor is reporting. When the tolerance information is not available,
/// a special number is assigned indicating the value is Unspecified.
/// The values for the state are defined in the following table.
/// Value                   Description
/// 0x000 Unspecified
/// 0x001–0xFFF The negative tolerance of the sensor. See Note below.
/// Note: The magnitude of a possible negative error associated with the reported data (expressed as a percentage)
/// is derived using the following formula:
/// 𝑃𝑜𝑠𝑠𝑖𝑏𝑙𝑒 𝑁𝑒𝑔𝑎𝑡𝑖𝑣𝑒 𝐸𝑟𝑟𝑜𝑟 [%] = 100 [%] ∗ 𝑁𝑒𝑔𝑎𝑡𝑖𝑣𝑒 𝑇𝑜𝑙𝑒𝑟𝑎𝑛𝑐𝑒 / 4095
@property (nonatomic, assign) UInt16 negativeTolerance;
/// This Sensor Sampling Function field specifies the averaging operation or type of sampling function applied to the
/// measured value. For example, this field can identify whether the measurement is an arithmetic mean value or an
/// instantaneous value. The values for this field are enumerated in Table 4.5.
/// For cases in which the sampling function is not made available, a special number has been assigned to indicate
/// the value is Unspecified. The values for the state are defined in the following table.
/// Value               Description
/// 0x00                Unspecified
/// 0x01                Instantaneous
/// 0x02                Arithmetic Mean
/// 0x03                RMS
/// 0x04                Maximum
/// 0x05                Minimum
/// 0x06                Accumulated. (See note below.)
/// 0x07                Count. (See note below.)
/// 0x08–0xFF       Reserved for Future Use
/// Note:   The Count sampling function can be used for a discrete variable such as the number of lightning discharges
/// detected by a lightning detector. The Sensor Measurement Period (see Section 4.1.1.5) in this case would state the
/// length of the period over which a counted number of lightning strikes was detected. The Accumulated sampling
/// function is intended to represent a cumulative moving average. The measurement value would be a cumulative moving
/// average value that was continually updated with a frequency indicated by the Sensor Update Interval (see Section 4.1.1.6).
@property (nonatomic, assign) SigSensorSamplingFunctionType samplingFunction;
/// This Sensor Measurement Period field specifies a uint8 value n that represents the averaging time span, accumulation
/// time, or measurement period in seconds over which the measurement is taken, using the formula:
/// 𝑟𝑒𝑝𝑟𝑒𝑠𝑒𝑛𝑡𝑒𝑑 𝑣𝑎𝑙𝑢𝑒 = 1.1^(n-64)
/// For example, it can specify the length of the period used to obtain an average reading.
/// For those cases where a value for the measurement period is not available or is not applicable, a special number has
/// been assigned to indicate Not Applicable. The values for the state are defined in the following table.
/// Value n                     Represented Value                       Description
/// 0x00                        Not Applicable                              Not Applicable
/// 0x01–0xFF               1.1^(n-64)                                      Time period in seconds
@property (nonatomic, assign) UInt8 measurementPeriod;
/// The measurement reported by a sensor is internally refreshed at the frequency indicated in the Sensor Update Interval
/// field (e.g., a temperature value that is internally updated every 15 minutes). This field specifies a uint8 value n that
/// determines the interval (in seconds) between updates, using the formula:
/// 𝑟𝑒𝑝𝑟𝑒𝑠𝑒𝑛𝑡𝑒𝑑 𝑣𝑎𝑙𝑢𝑒 = 1.1^(n-64)
/// For those cases where a value for the Sensor Update Interval is not available or is not applicable, a special number has
/// been assigned to indicate Not Applicable. The values for the state are defined in the following table.
/// Value n                     Represented Value                       Description
/// 0x00                        Not Applicable                              Not Applicable
/// 0x01–0xFF               1.1^(n-64)                                      Update interval in seconds
@property (nonatomic, assign) UInt8 updateInterval;

/// Initialize SigSensorDescriptorModel object.
/// - Parameter parameters: The hex of SigSensorDescriptorModel object.
/// - returns:
/// return `nil` when initialize SigSensorDescriptorModel object fail.
- (instancetype)initWithDescriptorParameters:(NSData *)parameters;

/// Get Sensor Descriptor hex data.
- (NSData *)getDescriptorParameters;

@end


/// mesh设备广播包解密模型。唯一标识符为identityData，且只存储本地json存在的identityData不为空的SigEncryptedModel。设备断电后会改变identityData，出现相同的address的SigEncryptedModel时，需要replace旧的。
@interface SigEncryptedModel : NSObject
/// advertisementDataService data
@property (nonatomic, strong) NSData *advertisementDataServiceData;
/// hash data
@property (nonatomic, strong) NSData *hashData;
/// random data
@property (nonatomic, strong) NSData *randomData;
/// peripheral UUID
@property (nonatomic, strong) NSString *peripheralUUID;
/// encrypted Data
@property (nonatomic, strong) NSData *encryptedData;
/// unicastAddress of node.
@property (nonatomic, assign) UInt16 address;
@end


/// The network key object represents the state of the mesh network key
/// that is used for securing communication at the network layer [1].
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.10),
/// 2.1.2 Network key object.
@interface SigNetkeyModel : NSObject

/// The name property contains a UTF-8 string, which should be a human-readable
/// name for the mesh subnet associated with this network key.
@property (nonatomic, copy) NSString *name;

/// The index property contains an integer from 0 to 4095 that represents the NetKey
/// index for this network key.
@property (nonatomic, assign) UInt16 index;

/// The phase property contains an integer with a value of 0, 1, or 2 that represents the
/// Key Refresh phase[1] for the subnet associated with this network key.
@property (nonatomic, assign) KeyRefreshPhase phase;

/// The key property contains a 32-character hexadecimal string that represents the
/// 128-bit network key.
@property (nonatomic, copy) NSString *key;

/// The minSecurity property contains a string with a value of either “insecure” or “secure”,
/// which describes a minimum security level for a subnet associated with this network key.
/// If all the nodes on the subnet associated with this network key have been provisioned
/// using the Secure Provisioning procedure [1], then the value of minSecurity property for
/// the subnet is set to “secure”; otherwise, the value of the minSecurity is set to “insecure”.
@property (nonatomic, copy) NSString *minSecurity;

/// The oldKey property contains a 32-character hexadecimal string that represents the 128-bit
/// network key, and shall be present when the phase property has a non-zero value, such as
/// when the Key Refresh procedure is in progress. The value of the oldKey property contains
/// the previous network key.(C.1)
/// C.1: Mandatory if the phase property is non-zero; otherwise excluded.
@property (nonatomic, copy, nullable) NSString *oldKey;

/// The timestamp property contains a string that represents the last time the value of the phase
/// property has been updated. The timestamp is based on Coordinated Universal Time (UTC) and
/// follows the “date-time” format as defined by JSON Schema Draft 4 [3], which is based
/// on the Internet date/time format described in Section 5.6 of RFC 3339 [6]:
/// YYYY-MM-DDThh:mm:ssZ or YYYY-MM-DDThh:mm:ss+/- timeoffset
/// where “YYYY” denotes a year; “MM” denotes a two-digit month (01 to 12); “DD” denotes
/// a two-digit day of the month (01 to 31); “hh” denotes a two-digit hour (00 to 23); “mm”
/// denotes a two-digit minute (00 to 59);'ss" denotes a two-digit second (00 to 59); “Z” denotes
/// the UTC time zone; and “timeoffset” denotes the offset between local time and UTC in the
/// format of +hh:mm or -hh:mm.
@property (nonatomic, copy) NSString *timestamp;


@property (nonatomic, strong) SigNetkeyDerivaties *keys;
@property (nonatomic, strong) SigNetkeyDerivaties *oldKeys;

/// Network identifier.
@property (nonatomic, assign) UInt8 nid;
/// Network identifier derived from the old key.
@property (nonatomic, assign) UInt8 oldNid;
/// Network identifier.
@property (nonatomic, assign) UInt8 directedSecurityNid;
/// Network identifier derived from the old key.
@property (nonatomic, assign) UInt8 directedSecurityOldNid;
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

/// Get dictionary from SigNetkeyModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigNetkeyModel;

/// Set dictionary to SigNetkeyModel object.
/// @param dictionary SigNetkeyModel dictionary object.
- (void)setDictionaryToSigNetkeyModel:(NSDictionary *)dictionary;

/// Get detail string of SigNetkeyModel object.
- (NSString *)getNetKeyDetailString;

@end

/// 参考文档：MshPRFd1.1r14_clean.pdf，page200.
@interface SigNetkeyDerivaties : NSObject

@property (nonatomic, strong) NSData *identityKey;

@property (nonatomic, strong) NSData *beaconKey;

@property (nonatomic, strong) NSData *encryptionKey;

@property (nonatomic, strong) NSData *privacyKey;
/// directed forwarding派生key
@property (nonatomic, strong) NSData *directedSecurityEncryptionKey;
/// directed forwarding派生key
@property (nonatomic, strong) NSData *directedSecurityPrivacyKey;

- (SigNetkeyDerivaties *)initWithNetkeyData:(NSData *)key helper:(OpenSSLHelper *)helper;

@end

@interface SigIvIndex : NSObject

@property (nonatomic,assign) UInt32 index;//init 0
@property (nonatomic,assign) BOOL updateActive;//init NO

- (instancetype)initWithIndex:(UInt32)index updateActive:(BOOL)updateActive;

@end


/// A Provisioner is a mesh node that is capable of provisioning a device to the
/// mesh network as described in [1], and is represented by a provisioner object
/// in the Mesh Configuration Database.
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.8),
/// 2.1.1 Provisioner object.
@interface SigProvisionerModel : NSObject

/// The provisionerName property contains a UTF-8 string, which should be a
/// human-readable name of the Provisioner associated with the mesh network.
@property (nonatomic, copy) NSString *provisionerName;

/// The UUID property contains a string that represents the 128-bit Device UUID [1].
/// This string shall follow the UUID string representation format as defined by [7].
/// If the Provisioner can perform configuration procedures, then the nodes property
/// of the Mesh Object (see Section 2.1) contains a node object (described in
/// Section 2.1.4) that matches the Provisioner’s UUID.
/// @note   UUID format in JSON："standardUUID": {
/// "type": "string",
/// "name": "UUID",
/// "pattern": "^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$"}
@property (nonatomic, copy) NSString *UUID;

/// The allocatedUnicastRange property contains an array of unicast range objects
/// (see Section 2.1.1.1).
@property (nonatomic, strong) NSMutableArray <SigRangeModel *>*allocatedUnicastRange;

/// The allocatedGroupRange property contains an array of group range objects
/// (see Section 2.1.1.2).
@property (nonatomic, strong) NSMutableArray <SigRangeModel *>*allocatedGroupRange;

/// The allocatedSceneRange property contains an array of scene range objects
/// (see Section 2.1.1.3).
@property (nonatomic, strong) NSMutableArray <SigSceneRangeModel *>*allocatedSceneRange;


/**
 create new provisioner by count of exist provisioners.（已弃用，请使用'initWithExistProvisionerMaxHighAddressUnicast:andProvisionerUUID:'）

 @param count count of exist provisioners
 @param provisionerUUID new provisioner's uuid
 @return SigProvisionerModel model
 */
- (instancetype)initWithExistProvisionerCount:(UInt16)count andProvisionerUUID:(NSString *)provisionerUUID DEPRECATED_MSG_ATTRIBUTE("Use 'initWithExistProvisionerMaxHighAddressUnicast:andProvisionerUUID:' instead");

/// Initialize SigProvisionerModel object.
/// @param maxHighAddressUnicast The max hight unicastAddress of `allocatedUnicastRange`.
/// @param provisionerUUID The UUID of new provisioner.
/// @returns return `nil` when initialize SigProvisionerModel object fail.
- (instancetype)initWithExistProvisionerMaxHighAddressUnicast:(UInt16)maxHighAddressUnicast andProvisionerUUID:(NSString *)provisionerUUID;

/// Get SigNodeModel object of current provisioner.
- (nullable SigNodeModel *)node;

/// Get dictionary from SigProvisionerModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigProvisionerModel;

/// Set dictionary to SigProvisionerModel object.
/// @param dictionary SigProvisionerModel dictionary object.
- (void)setDictionaryToSigProvisionerModel:(NSDictionary *)dictionary;

@end


/// The unicast range object represents the range of unicast addresses that the
/// Provisioner can allocate to new devices when they are provisioned onto the
/// mesh network, without needing to coordinate the node additions with other
/// Provisioners. The unicast range object contains the properties defined in Table 2.3.
/// The group range object represents the range of group addresses that the
/// Provisioner can allocate to newly created groups, without needing to
/// coordinate the group additions with other Provisioners.
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.9),
/// 2.1.1.1 Unicast range object and 2.1.1.2 Group range object.
@interface SigRangeModel : NSObject

/// As Unicast range object: The lowAddress and highAddress properties contain 4-character
/// hexadecimal strings representing values from 0x0001 to 0x7FFF. The value of the
/// lowAddress property shall be less than or equal to the value of the highAddress property.
/// @note   As Unicast range object Note: The process for allocating the range of
/// unicast addresses is implementation specific.
/// As Group range object: The lowAddress and highAddress properties contain 4-character
/// hexadecimal strings, representing values from 0xC000 to 0xFEFF. The value of the
/// lowAddress property shall be less than or equal to the value of the highAddress property.
/// @note   As Group range object Note: The process for allocating the range of
/// group addresses is implementation specific.
@property (nonatomic, copy) NSString *lowAddress;

/// As Unicast range object: The lowAddress and highAddress properties contain 4-character
/// hexadecimal strings representing values from 0x0001 to 0x7FFF. The value of the
/// lowAddress property shall be less than or equal to the value of the highAddress property.
/// @note   As Unicast range object Note: The process for allocating the range of
/// unicast addresses is implementation specific.
/// As Group range object: The lowAddress and highAddress properties contain 4-character
/// hexadecimal strings, representing values from 0xC000 to 0xFEFF. The value of the
/// lowAddress property shall be less than or equal to the value of the highAddress property.
/// @note   As Group range object Note: The process for allocating the range of
/// group addresses is implementation specific.
@property (nonatomic, copy) NSString *highAddress;

/// Get int number of low address.
- (NSInteger)lowIntAddress;

/// Get int number of hight address.
- (NSInteger)hightIntAddress;

/// Get dictionary from SigRangeModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigRangeModel;

/// Set dictionary to SigRangeModel object.
/// @param dictionary SigRangeModel dictionary object.
- (void)setDictionaryToSigRangeModel:(NSDictionary *)dictionary;

/// Initialize SigRangeModel object.
/// @param maxHighAddressUnicast The max hight unicastAddress of `allocatedUnicastRange`.
/// @returns return `nil` when initialize SigRangeModel object fail.
- (instancetype)initWithMaxHighAddressUnicast:(UInt16)maxHighAddressUnicast;

@end


/// The scene range object represents the range of scene numbers that the
/// Provisioner can use to register new scenes in the mesh network, without
/// needing to coordinate the allocated scene numbers with other Provisioners.
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.10),
/// 2.1.1.3 Scene range object.
@interface SigSceneRangeModel : NSObject

/// The firstScene and lastScene properties contain 4-character hexadecimal
/// strings, representing values from 0x0001 to 0xFFFF. The value of the firstScene
/// property shall be less than or equal to the value of the lastScene property.
/// @note   The process for allocating the range of scene numbers is
/// implementation specific.
@property (nonatomic, copy) NSString *firstScene;

/// The firstScene and lastScene properties contain 4-character hexadecimal
/// strings, representing values from 0x0001 to 0xFFFF. The value of the firstScene
/// property shall be less than or equal to the value of the lastScene property.
/// @note   The process for allocating the range of scene numbers is
/// implementation specific.
@property (nonatomic, copy) NSString *lastScene;

/// Get dictionary from SigSceneRangeModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigSceneRangeModel;

/// Set dictionary to SigSceneRangeModel object.
/// @param dictionary SigSceneRangeModel dictionary object.
- (void)setDictionaryToSigSceneRangeModel:(NSDictionary *)dictionary;

@end


/// The application key object represents the state of a mesh application key
/// that is used for securing communication at the access layer [1].
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.11),
/// 2.1.3 Application key object.
@interface SigAppkeyModel : NSObject

/// The name property contains a UTF-8 string, which should be a human-readable
/// name for the application functionality associated with this application key
/// (e.g., “Home Automation”).
@property (nonatomic, copy) NSString *name;

/// The index property contains an integer from 0 to 4095 that represents the
/// AppKey index for this application key.
@property (nonatomic, assign) NSInteger index;

/// The boundNetKey property contains a corresponding NetKey index from the netKeys
/// property of the Mesh Object. All application keys are bound to network keys that are
/// documented in the Mesh Object. That is, the bound network key index corresponds
/// to the index value of one of the network key entries in the Mesh Object’s netKeys array
/// (see Sections 2.1 and 2.1.2).
@property (nonatomic, assign) NSInteger boundNetKey;

/// The key property contains a 32-character hexadecimal string that represents the
/// 128-bit application key.
@property (nonatomic, copy) NSString *key;

/// The oldKey property contains a 32-character hexadecimal string that represents the
/// 128-bit application key, and is present if the application key is updated during the Key
/// Refresh procedure. The value of the oldKey property contains the previous application
/// key.(C.1)
/// C.1: Mandatory if the key refresh phase of the network key to which this application key
/// is bound is non-zero, and if this application key is being updated by the Mesh Manager;
/// otherwise excluded.
@property (nonatomic, copy) NSString *oldKey;


@property (nonatomic, assign) UInt8 aid;
@property (nonatomic, assign) UInt8 oldAid;

/// Get curent bound network key.
- (nullable SigNetkeyModel *)getCurrentBoundNetKey;

/// Get hex data appkey of SigAppkeyModel object.
- (nullable NSData *)getDataKey;

/// Get hex data old appkey of SigAppkeyModel object.
- (nullable NSData *)getDataOldKey;

/// Get dictionary from SigAppkeyModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigAppkeyModel;

/// Set dictionary to SigAppkeyModel object.
/// @param dictionary SigAppkeyModel dictionary object.
- (void)setDictionaryToSigAppkeyModel:(NSDictionary *)dictionary;

/// Get detail string of SigAppkeyModel object.
- (NSString *)getAppKeyDetailString;

@end


/// 2.1.7 exclusionList object
@interface SigExclusionListObjectModel : NSObject<NSCopying>

@property (nonatomic, assign) NSInteger ivIndex;

@property (nonatomic, strong) NSMutableArray <NSString *>*addresses;

/// Get dictionary from SigExclusionListObjectModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigExclusionListObjectModel;

/// Set dictionary to SigExclusionListObjectModel object.
/// @param dictionary SigExclusionListObjectModel dictionary object.
- (void)setDictionaryToSigExclusionListObjectModel:(NSDictionary *)dictionary;

@end


/// The scene object represents a configured state of a mesh scene.
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.22),
/// 2.1.6 Scene object
@interface SigSceneModel : NSObject<NSCopying>

/// The name property contains a string that represents the name of this scene.
@property (nonatomic, copy) NSString *name;

/// The number property contains a 4-character hexadecimal string that represents
/// a value from 0x0001 to 0xFFFF that represents the scene number.
@property (nonatomic, copy) NSString *number;

/// The addresses property contains an array of 4-character hexadecimal strings that
/// represent the addresses of elements whose Scene Register state contains this
/// scene as described in [5].
@property (nonatomic, strong) NSMutableArray <NSString *>*addresses;

//暂时添加并保存json
@property (nonatomic, strong) NSMutableArray <ActionModel *>*actionList;

/// Get dictionary from SigSceneModel object.(save local)
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigSceneModel;

/// Set dictionary to SigSceneModel object.
/// @param dictionary SigSceneModel dictionary object.
- (void)setDictionaryToSigSceneModel:(NSDictionary *)dictionary;

/// Get format dictionary from SigSceneModel object.(Mesh Spec format)
/// @returns return dictionary object.
- (NSDictionary *)getFormatDictionaryOfSigSceneModel;

@end


/// If a node is part of a Group, at least one model of the node is subscribed
/// to the Group’s group address.
/// @note   A Group may have a Parent Group. In this case, all the models
/// of a node that are subscribed to the Group’s address are also subscribed
/// to the Parent Group’s address.
/// @note   For example, the Second-Floor Group is a parent of the Bedroom
/// Group and the Guest Bedroom Group. In this case, at least one model of all
/// the nodes of the Bedroom Group is subscribed to a group address or virtual
/// label of the Bedroom Group and Second-Floor Group; and at least one model
/// of all the nodes of the Guest Bedroom Group is subscribed to the group
/// address or virtual label of the Guest Bedroom Group and the Second-Floor Group.
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.21),
/// 2.1.5 Group object.
@interface SigGroupModel : NSObject

/// The name property contains a UTF-8 string, which should be a human-readable
/// name for this group within the mesh network.
@property (nonatomic, copy) NSString *name;

/// The address property contains a 4-character hexadecimal string from 0xC000
/// to 0xFEFF or a 32- character hexadecimal string of virtual label UUID, and is
/// the address of the group.
@property (nonatomic, copy) NSString *address;

/// The parentAddress property contains a 4-character hexadecimal string or a
/// 32-character hexadecimal string and represents an address of a parent group
/// in which this group is included. The parentAddress property contains a
/// hexadecimal string that represents 0x0000, a number from 0xC000 to 0xFEFF,
/// or a 32-character hexadecimal string of virtual label UUID. The value of the
/// parentAddress property shall not be equal to the value of the address property.
/// The value of 0x0000 indicates that the group is not included in another group
/// (i.e., the group has no parent).
@property (nonatomic, copy) NSString *parentAddress;


@property (nonatomic, copy) SigMeshAddress *meshAddress;

- (UInt16)intAddress;

/// Get dictionary from SigGroupModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigGroupModel;

/// Set dictionary to SigGroupModel object.
/// @param dictionary SigGroupModel dictionary object.
- (void)setDictionaryToSigGroupModel:(NSDictionary *)dictionary;

//临时缓存groupBrightness、groupTempareture，关闭APP后就丢失。
@property (nonatomic,assign) UInt8 groupBrightness;
@property (nonatomic,assign) UInt8 groupTempareture;

- (BOOL)isOn;
- (NSMutableArray <SigNodeModel *>*)groupDevices;
- (NSMutableArray <SigNodeModel *>*)groupOnlineDevices;

- (BOOL)hasLevelCapability;
- (BOOL)hasLumLevelCapability;
- (BOOL)hasTempLevelCapability;
- (BOOL)hasHueLevelCapability;
- (BOOL)hasSatLevelCapability;

@end


/// The node object represents a configured state of a mesh node.
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.12),
/// 2.1.4 Node object
@interface SigNodeModel : NSObject<NSCopying>

/// The UUID property contains a string that represents the 128-bit Device UUID [1].
/// This string shall follow the UUID string representation format as defined by [7].
/// @note   UUID format in JSON："standardUUID": {
/// "type": "string",
/// "name": "UUID",
/// "pattern": "^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$"}
@property (nonatomic, copy) NSString *UUID;

/// The unicastAddress property contains a 4-character hexadecimal string representing
/// a value from 0x0001 to 0x7FFF that is the primary unicast address of this node.
@property (nonatomic, copy) NSString *unicastAddress;

/// The deviceKey property contains a 32-character hexadecimal string that represents
/// the 128-bit device key for this node.(C.4)
/// C.4: Present if the partial property is set to “false”; otherwise optional.
@property (nonatomic, copy) NSString *deviceKey;

/// The security property contains a string with two allowed values, “insecure” and
/// “secure”, representing the level of security for the subnet on which the node has
/// been originally provisioned.
@property (nonatomic, copy) NSString *security;

/// The netKeys property contains an array of node network key objects (see Section 2.1.4.1)
/// that includes information about the network keys known to this node.
@property (nonatomic, strong) NSMutableArray<SigNodeKeyModel *> *netKeys;

/// The configComplete property contains a Boolean value that represents whether the Mesh
/// Manager finished configuring this node. The property’s value is set to “true” once the
/// Mesh Manager determines that this node’s configuration process is completed; otherwise,
/// the property’s value is set to “false”.
@property (nonatomic, assign) bool configComplete;

/// The name property contains a UTF-8 string, which should be a human-readable name that
/// can identify this node within the mesh network.(Optional)
@property (nonatomic, copy) NSString *name;

/// The cid property contains a 4-character hexadecimal string that represents a 16-bit Company
/// Identifier (CID) assigned by the Bluetooth SIG [4]. The value of this property is obtained from
/// node composition data [1].(C.1)
/// C.1: Present if node composition data page 0 is available; otherwise omitted.
@property (nonatomic, copy) NSString *cid;

/// The pid property contains a 4-character hexadecimal string that represents a 16-bit,
/// vendor-assigned Product Identifier (PID). The value of this property is obtained from node
/// composition data [1].(C.1)
/// C.1: Present if node composition data page 0 is available; otherwise omitted.
@property (nonatomic, copy) NSString *pid;

/// The vid property contains a 4-character hexadecimal string that represents a 16-bit,
/// vendor-assigned product Version Identifier (VID). The value of this property is obtained
/// from node composition data [1].(C.1)
/// C.1: Present if node composition data page 0 is available; otherwise omitted.
@property (nonatomic, copy) NSString *vid;

/// The crpl property contains a 4-character hexadecimal string that represents a 16-bit
/// value indicating the minimum number of Replay Protection List (RPL) entries for this
/// node. The value of this property is obtained from node composition data [1].(C.1)
/// C.1: Present if node composition data page 0 is available; otherwise omitted.
@property (nonatomic, copy) NSString *crpl;

/// The features property contains a features object (see Section 2.1.4.2).(C.1)
/// C.1: Present if node composition data page 0 is available; otherwise omitted.
@property (nonatomic, strong) SigNodeFeatures *features;

/// The secureNetworkBeacon property contains a Boolean value that represents
/// whether the node is configured to send Secure Network beacons.(C.5)
/// C.5: If the value is unknown, this property is excluded.
@property (nonatomic, assign) bool secureNetworkBeacon;

/// The defaultTTL property contains an integer from 0 to 127 that represents the
/// default Time to Live (TTL) value used when sending messages [1].(C.5)
/// C.5: If the value is unknown, this property is excluded.
@property (nonatomic, assign) UInt8 defaultTTL;

/// The networkTransmit property contains a network transmit object (see Section 2.1.4.3).(C.5)
/// C.5: If the value is unknown, this property is excluded.
@property (nonatomic, strong) SigNetworktransmitModel *networkTransmit;

/// The relayRetransmit property contains a relay retransmit object (see Section 2.1.4.4).(C.5)
/// C.5: If the value is unknown, this property is excluded.
@property (nonatomic, strong) SigRelayretransmitModel *relayRetransmit;

/// The appKeys property contains an array of node application key objects (see Section 2.1.4.5)
/// that includes information about the application keys known to this node.
@property (nonatomic, strong) NSMutableArray<SigNodeKeyModel *> *appKeys;//node isn't unbound when appkeys is empty.

/// The elements property contains an array of element objects (see Section 2.1.4.6).
@property (nonatomic, strong) NSMutableArray<SigElementModel *> *elements;

/// The excluded property contains a Boolean value that is set to “true” when the node is in the
/// process of being deleted and is excluded from the new network key distribution during the
/// Key Refresh procedure; otherwise, it is set to “false”.
@property (nonatomic, assign) bool excluded;

/// The heartbeatPub property contains a heartbeat publish object (see Section 2.1.4.7).(C.2)
/// C.2: Present if heartbeat publication is configured; otherwise omitted.
@property (nonatomic, strong, nullable) SigHeartbeatPubModel *heartbeatPub;

/// The heartbeatSub property contains a heartbeat subscribe object (see Section 2.1.4.8).(C.3)
/// C.3: Present if heartbeat subscription is configured; otherwise omitted.
@property (nonatomic, strong, nullable) SigHeartbeatSubModel *heartbeatSub;

@property (nonatomic, copy, nullable) NSString *macAddress;//new add the mac to json, get mac from scanResponse's Manufacturer Data.

//暂时添加到json数据中
@property (nonatomic,strong) NSMutableArray <SchedulerModel *>*schedulerList;
@property (nonatomic,assign) BOOL subnetBridgeEnable;
@property (nonatomic,strong) NSMutableArray <SigSubnetBridgeModel *>*subnetBridgeList;
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
@property (nonatomic,strong) NSMutableArray <NSNumber *>*subnetBridgeServerAddress;//element addresses of subnetBridgeServer
@property (nonatomic,strong) NSMutableArray <NSNumber *>*sceneAddress;//element addresses of scene
@property (nonatomic,strong) NSMutableArray <NSNumber *>*publishAddress;//element addresses of publish
@property (nonatomic,assign) UInt16 publishModelID;//modelID of set publish
@property (nonatomic,strong,nullable) NSString *peripheralUUID;

/// 缓存单灯的direct forwarding使能状态信息，存储本地，不存储JSON。
@property (nonatomic,strong,nullable) SigDirectControlStatus *directControlStatus;

///return node true brightness, range is 0~100
- (UInt8)trueBrightness;

///return node true color temperture, range is 0~100
- (UInt8)trueTemperature;

///update node status from api getOnlineStatusFromUUIDWithCompletation
- (void)updateOnlineStatusWithDeviceState:(DeviceState)state bright100:(UInt8)bright100 temperature100:(UInt8)temperature100;

- (UInt16)getNewSchedulerID;

- (void)saveSchedulerModelWithModel:(SchedulerModel *)scheduler;

- (UInt8)getElementCount;

- (NSMutableArray *)getAddressesWithModelID:(NSNumber *)sigModelID;

- (instancetype)initWithNode:(SigNodeModel *)node;

- (UInt16)address;
- (void)setAddress:(UInt16)address;

///get all groupIDs of node(获取该设备的所有组号)
- (NSMutableArray <NSNumber *>*)getGroupIDs;

///add new groupID to node(新增设备的组号)
- (void)addGroupID:(NSNumber *)groupID;

///delete old groupID from node(删除设备的组号)
- (void)deleteGroupID:(NSNumber *)groupID;

- (void)openPublish;

- (void)closePublish;

///返回是否支持publish功能
- (BOOL)hasPublishFunction;

///返回是否支持Direct Forwarding功能
- (BOOL)hasDirectForwardingFunction;

- (BOOL)hasOpenPublish;

///publish是否存在周期上报功能。
- (BOOL)hasPublishPeriod;

- (BOOL)isSensor;

/// 返回当前节点是否是遥控器。
- (BOOL)isRemote;

/// v3.3.3.6新增的telink定义的PID结构体。
- (struct TelinkPID)getTelinkPID;

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

- (nullable SigModelIDModel *)getModelIDModelWithModelID:(UInt32)modelID;
- (nullable SigModelIDModel *)getModelIDModelWithModelID:(UInt32)modelID andElementAddress:(UInt16)elementAddress;

- (NSDictionary *)getDictionaryOfSigNodeModel;
- (void)setDictionaryToSigNodeModel:(NSDictionary *)dictionary;
- (NSDictionary *)getFormatDictionaryOfSigNodeModel;

- (void)setAddSigAppkeyModelSuccess:(SigAppkeyModel *)appkey;
- (void)setCompositionData:(SigPage0 *)compositionData;
- (void)setBindSigNodeKeyModel:(SigNodeKeyModel *)appkey toSigModelIDModel:(SigModelIDModel *)modelID;

- (void)updateNodeStatusWithBaseMeshMessage:(SigBaseMeshMessage *)responseMessage source:(UInt16)source;

- (void)addDefaultPublicAddressToRemote;

- (BOOL)hasLevelCapability;

- (BOOL)hasLumLevelCapability;

- (BOOL)hasTempLevelCapability;

- (BOOL)hasHueLevelCapability;

- (BOOL)hasSatLevelCapability;

- (SigElementModel *)getElementModelWithModelIds:(NSArray <NSNumber *>*)modelIds;

- (void)deleteNetKeyDataFromNode:(SigNodeKeyModel *)netKey;

@end


/// The relay retransmit object represents the parameters of the retransmissions of network layer
/// messages relayed by a mesh node [1].
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.16),
/// 2.1.4.4 Relay retransmit object.
@interface SigRelayretransmitModel : NSObject
/// Number of retransmissions on advertising bearer for each Network PDU relayed by the node.
/// For example, a value of 0b000 represents a single transmission with no retransmissions,
/// and a value of 0b111 represents a single transmission and 7 retransmissions for a total of 8
/// transmissions.
@property (nonatomic, assign) NSInteger relayRetransmitCount;
/// The Relay Retransmit Interval Steps field is a 5-bit value representing the number of 10
/// millisecond steps that controls the interval between message retransmissions of the
/// Network PDU relayed by the node.
/// The retransmission interval is calculated using the formula:
/// retransmission interval = (Relay Retransmit Interval Steps + 1) * 10
@property (nonatomic, assign) NSInteger relayRetransmitIntervalSteps;

- (UInt8)getIntervalOfJsonFile;
- (void)setIntervalOfJsonFile:(UInt8)intervalOfJsonFile;

/// get dictionary from SigRelayretransmitModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigRelayretransmitModel;

/// Set dictionary to SigRelayretransmitModel object.
/// @param dictionary SigRelayretransmitModel dictionary object.
- (void)setDictionaryToSigRelayretransmitModel:(NSDictionary *)dictionary;

@end


/// The network transmit object represents the parameters of the transmissions of network
/// layer messages originating from a mesh node [1].
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.16),
/// 2.1.4.3 Network transmit object.
@interface SigNetworktransmitModel : NSObject
/// The Network Transmit Count field is a 3-bit value that controls the number of message
/// transmissions of the Network PDU originating from the node. The number of transmissions
/// is the Transmit Count + 1.
/// For example a value of 0b000 represents a single transmission and a value of 0b111
/// represents 8 transmissions.
@property (nonatomic, assign) NSInteger networkTransmitCount;
/// The Network Transmit Interval Steps field is a 5-bit value representing the number of 10
/// millisecond steps that controls the interval between message transmissions of Network
/// PDUs originating from the node.
/// The transmission interval is calculated using the formula:
/// transmission interval = (Network Retransmit Interval Steps + 1) * 10
/// Each transmission should be perturbed by a random value between 0 to 10 milliseconds
/// between each transmission.
/// For example, a value of 0b10000 represents a transmission interval between 170 and 180
/// milliseconds between each transmission.
@property (nonatomic, assign) NSInteger networkTransmitIntervalSteps;

- (UInt8)getIntervalOfJsonFile;
- (void)setIntervalOfJsonFile:(UInt8)intervalOfJsonFile;

/// get dictionary from SigNetworktransmitModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigNetworktransmitModel;

/// Set dictionary to SigNetworktransmitModel object.
/// @param dictionary SigNetworktransmitModel dictionary object.
- (void)setDictionaryToSigNetworktransmitModel:(NSDictionary *)dictionary;

@end


/// The features object represents the functionality of a mesh node that is determined by the set
/// features that the node supports [1].
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.15),
/// 2.1.4.2 Features object.
@interface SigNodeFeatures : NSObject
/// The state of Relay feature. Default is 2.(C.1)
/// C.1: The value of the relay property is set to 2 if the node does not support the Relay feature.
/// The value is set to 0 if the Relay feature is not enabled or to 1 if the Relay feature is enabled.
/// If the state of the Relay feature is unknown, this property is excluded.
@property (nonatomic,assign) SigNodeFeaturesState relayFeature;
/// The state of Proxy feature. Default is 2.(C.2)
/// C.2: The value of the proxy property is set to 2 if the node does not support the Proxy feature.
/// The value is set to 0 if the Proxy feature is not enabled or to 1 if the Proxy feature is enabled.
/// If the state of the Proxy feature is unknown, this property is excluded.
@property (nonatomic,assign) SigNodeFeaturesState proxyFeature;
/// The state of Low Power feature. Default is 2.(C.3)
/// C.3: The value of the lowPower property is set to 2 if the node does not support the Low Power
/// feature or to 1 if the node supports the Low Power feature. If the support of the Low Power
/// feature is unknown, this property is excluded.
@property (nonatomic,assign) SigNodeFeaturesState lowPowerFeature;
/// The state of Friend feature. Default is 2.(C.4)
/// C.4: The value of the friend property is set to 2 if the node does not support the Friend feature.
/// The value is set to 0 if the Friend feature is not enabled or to 1 if the Friend feature is enabled.
/// If the state of the Friend feature is unknown, this property is excluded.
@property (nonatomic,assign) SigNodeFeaturesState friendFeature;

- (UInt16)rawValue;
- (instancetype)initWithRawValue:(UInt16)rawValue;
- (instancetype)initWithRelay:(SigNodeFeaturesState)relayFeature proxy:(SigNodeFeaturesState)proxyFeature friend:(SigNodeFeaturesState)friendFeature lowPower:(SigNodeFeaturesState)lowPowerFeature;

/// get dictionary from SigFeatureModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigFeatureModel;

/// Set dictionary to SigFeatureModel object.
/// @param dictionary SigFeatureModel dictionary object.
- (void)setDictionaryToSigFeatureModel:(NSDictionary *)dictionary;

@end


/// The node network key object represents the state of a network key distributed to a mesh
/// node by a Mesh Manager.
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.15),
/// 2.1.4.1 Node network key object.
@interface SigNodeKeyModel : NSObject
/// The Key index for this network key.
/// The index property contains an integer from 0 to 4095 that represents the NetKey index for
/// this network key. The network key index corresponds to the index value of one of the network
/// key entries in the Mesh Object’s netKeys array (see Sections 2.1 and 2.1.2).
@property (nonatomic, assign) UInt16 index;
/// The updated property contains a Boolean value that is set to “false”, unless the Key Refresh
/// procedure is in progress and the network key has been successfully updated.
@property (nonatomic, assign) bool updated;


- (instancetype)initWithIndex:(UInt16)index updated:(bool)updated;

/// get dictionary from SigNodeKeyModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigNodeKeyModel;

/// Set dictionary to SigNodeKeyModel object.
/// @param dictionary SigNodeKeyModel dictionary object.
- (void)setDictionaryToSigNodeKeyModel:(NSDictionary *)dictionary;

@end


/// The element object represents a mesh element that is defined as an addressable entity within
/// a mesh node [1].
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.17),
/// 2.1.4.6 Element object.
@interface SigElementModel : NSObject

/// The name property contains a UTF-8 string, which should be a human-readable name that can
/// identify an element within the node.(O)
@property (nonatomic, copy) NSString *name;

/// The index property contains an integer from 0 to 255 that represents the numeric order of the
/// element within this node. Because the node has at least one element, the primary element, as
/// required by the Mesh Profile specification [1], Section 2.3.4, the elements array of the node
/// object (see Section 2.1.4) must contain at least one element object with the value of the index
/// property set to zero, which corresponds to the primary element within the node.
@property (nonatomic, assign) UInt8 index;

/// The location property contains a 4-character hexadecimal string that represents a description
/// of the element’s location (defined in the GATT Bluetooth Namespace Descriptors section of the
/// Bluetooth SIG Assigned Numbers webpage [4]).
@property (nonatomic, copy) NSString *location;

/// The models property contains an array of model objects (see Section 2.1.4.6.1).
@property (nonatomic, strong) NSMutableArray<SigModelIDModel *> *models;

@property (nonatomic, assign) UInt16 parentNodeAddress;

- (instancetype)initWithLocation:(SigLocation)location;

- (instancetype)initWithCompositionData:(NSData *)compositionData offset:(int *)offset;

/// Returns the Unicast Address of the Element. For Elements not added to Node this returns the Element index value as `Address`.
- (UInt16)unicastAddress;

- (SigNodeModel * _Nullable)getParentNode;

- (SigLocation)getSigLocation;
- (void)setSigLocation:(SigLocation)sigLocation;

/// get dictionary from SigElementModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigElementModel;

/// Set dictionary to SigElementModel object.
/// @param dictionary SigElementModel dictionary object.
- (void)setDictionaryToSigElementModel:(NSDictionary *)dictionary;

- (NSData *)getElementData;

- (BOOL)hasModelIdString:(NSString *)modelIdString;

@end


/// The model object represents a configured state of a mesh model.
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.17),
/// 2.1.4.6.1 Model object.
@interface SigModelIDModel : NSObject
/// Attention: length=4，为SIG modelID，类型为UInt16；
/// length=8，为vendor modelID，类型为UInt32
/// The modelId property contains either a 4-character hexadecimal string that represents a
/// Bluetooth SIG- defined model identifier [1] and [5], or an 8-character hexadecimal
/// string that represents a vendor-defined model identifier. In the latter case, the first 4
/// characters correspond to a Bluetooth-assigned Company Identifier [4], and the last 4
/// characters contain a vendor-assigned model identifier.
@property (nonatomic, copy) NSString *modelId;
/// The array of Unicast or Group Addresses (4-character hexadecimal value), or Virtual
/// Label UUIDs (32-character hexadecimal string).
/// The subscribe property contains an array of 4-character hexadecimal strings, with values
/// from 0xC000 to 0xFFFE, that represents group addresses and/or an array of 32-character
/// hexadecimal strings that represents virtual label UUIDs.
@property (nonatomic, strong) NSMutableArray <NSString *>*subscribe;
/// The configuration of this Model's publication.
/// The publish property contains a publish object (see Section 2.1.4.6.1.1) that describes the
/// configuration of this model’s publication.
@property (nonatomic, strong, nullable) SigPublishModel *publish;
/// An array of Appliaction Key indexes to which this model is bound.
/// The bind property contains an array of integers that represents indexes of the application
/// keys (see Section 2.1.3) to which this model is bound. Each application key index corresponds
/// to the index values of one of the application key entries in the node’s appKeys array (see
/// Sections 2.1.4 and 2.1.4.5).
@property (nonatomic, strong) NSMutableArray <NSNumber *>*bind;//[KeyIndex]

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
- (BOOL)isVendorModelID;

/// Returns the list of known Groups that this Model is subscribed to.
/// It may be that the Model is subscribed to some other Groups, which are
/// not known to the local database, and those are not returned.
/// Use `isSubscribed(to:)` to check other Groups.
- (NSArray <SigGroupModel *>*)subscriptions;

- (BOOL)isConfigurationServer;
- (BOOL)isConfigurationClient;
- (BOOL)isHealthServer;
- (BOOL)isHealthClient;
/// 返回是否是强制使用deviceKey加解密的modelID，是则无需进行keyBind操作。
- (BOOL)isDeviceKeyModelID;

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

/// get dictionary from SigModelIDModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigModelIDModel;

/// Set dictionary to SigModelIDModel object.
/// @param dictionary SigModelIDModel dictionary object.
- (void)setDictionaryToSigModelIDModel:(NSDictionary *)dictionary;

@end


/// The publish object represents parameters that define how the messages are published
/// by a mesh model [1].
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.18),
/// 2.1.4.6.1.1 Publish object.
@interface SigPublishModel : NSObject
/// The address property contains the publication address for the model. It can be either a
/// 4-character hexadecimal string that represents a group or unicast address, or a 32-character
/// hexadecimal string that represents virtual label UUIDs.
@property (nonatomic, copy) NSString *address;

/// The index property contains an integer that represents an application key index (see
/// Section 2.1.3), indicating which application key to use for the publication. The application
/// key index corresponds to the index value of one of the application key entries in the node’s
/// appKeys array (see Sections 2.1.4 and 2.1.4.5).
@property (nonatomic, assign) NSInteger index;

/// The ttl property contains an integer from 0 to 127 that represents the Time to Live (TTL) value
/// for published messages or an integer with a value of 255 that indicates that the node’s default
/// TTL is used for publication.
@property (nonatomic, assign) NSInteger ttl;

/// The period property contains a publish period object (see Section 2.1.4.6.1.2) that describes
/// the interval between subsequent publications. If the value of this property is 0, the periodic
/// publication is disabled (as defined in [1]).
@property (nonatomic, strong) SigPeriodModel *period;

/// The credentials property contains an integer of 0 or 1 that represents whether managed flooding
/// security material [8] (0) or friendship security material (1) is used.
@property (nonatomic, assign) NSInteger credentials;

/// The retransmit property contains a retransmit object (see Section 2.1.4.6.1.3) that describes the
/// number of times a message is published and the interval between retransmissions of the
/// published messages.
@property (nonatomic, strong) SigRetransmitModel *retransmit;


/// get dictionary from SigPublishModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigPublishModel;

/// Set dictionary to SigPublishModel object.
/// @param dictionary SigPublishModel dictionary object.
- (void)setDictionaryToSigPublishModel:(NSDictionary *)dictionary;

@end


/// The retransmit object is used to describe the number of times a message is published and the
/// interval between retransmissions of the published messages.
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.19)x  xXx,
/// 2.1.4.6.1.3 Retransmit object.
@interface SigRetransmitModel : NSObject

/// The count property contains an integer from 0 to 7 that represents the number of retransmissions
/// for published messages. A value of 0 represents no retransmissions (as described in [1]).
@property (nonatomic, assign) NSInteger count;
/// The interval property contains an integer from 50 to 1600, with a resolution of 50, that represents
/// the interval in milliseconds between the transmissions.
@property (nonatomic, assign) NSInteger interval;

/// get dictionary from SigRetransmitModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigRetransmitModel;

/// Set dictionary to SigRetransmitModel object.
/// @param dictionary SigRetransmitModel dictionary object.
- (void)setDictionaryToSigRetransmitModel:(NSDictionary *)dictionary;

@end


/// The publish period object determines the interval at which messages are published by a
/// model and is defined by two values: the number of steps and step resolution.
/// by a mesh model [1].
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.19)x  xXx,
/// 2.1.4.6.1.2 Publish period object.
@interface SigPeriodModel : NSObject

/// The numberOfSteps property contains an integer from 0 to 63 that represents the number
/// of steps used to calculate the publish period (as described in [1]).
@property (nonatomic, assign) NSInteger numberOfSteps;

/// The resolution property contains an integer that represents the publish step resolution in
/// milliseconds. The allowed values are: 100, 1000, 10000, and 600000.
@property (nonatomic, assign) NSInteger resolution;

/// get dictionary from SigPeriodModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigPeriodModel;

/// Set dictionary to SigPeriodModel object.
/// @param dictionary SigPeriodModel dictionary object.
- (void)setDictionaryToSigPeriodModel:(NSDictionary *)dictionary;

@end


/// The heartbeat publication object represents parameters that define the sending of periodic
/// Heartbeat transport control messages [1].
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.20)x  xXx,
/// 2.1.4.6.1.3 Retransmit object.
@interface SigHeartbeatPubModel : NSObject

/// The address property represents the destination address for the heartbeat messages. It
/// contains a 4- character hexadecimal string that represents a group or unicast address.
@property (nonatomic, copy) NSString *address;

/// The period property contains an integer from 0 to 65536 that represents the cadence of
/// periodical heartbeat messages in seconds.
@property (nonatomic, assign) NSInteger period;

/// The ttl property contains an integer from 0 to 127 that represents the Time to Live (TTL)
/// value for the heartbeat messages.
@property (nonatomic, assign) NSInteger ttl;

/// The index property contains an integer that represents a network key index (see Section 2.1.2),
/// indicating which network key to use for the heartbeat publication. The network key index
/// corresponds to the index value of one of the network key entries in the node’s netKeys array
/// (see Sections 2.1.4 and 2.1.4.1).
@property (nonatomic, assign) NSInteger index;

/// The features property contains an array of strings that describe the features that trigger the
/// sending of heartbeat messages when changed. The allowed values are “relay”, “proxy”,
/// “friend”, and “lowPower”.
@property (nonatomic, strong) NSMutableArray <NSString *>*features;

/// get dictionary from SigHeartbeatPubModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigHeartbeatPubModel;

/// Set dictionary to SigHeartbeatPubModel object.
/// @param dictionary SigHeartbeatPubModel dictionary object.
- (void)setDictionaryToSigHeartbeatPubModel:(NSDictionary *)dictionary;

@end


/// The heartbeat subscription object represents parameters that define the receiving of periodical
/// Heartbeat transport control messages [1].
/// @note   - seeAlso: MshCDB_1.0.1.pdf (page.21)x  xXx,
/// 2.1.4.8 Heartbeat subscription object.
@interface SigHeartbeatSubModel : NSObject

/// The source property contains the source address for Heartbeat messages that a node processes.
/// It is a 4-character hexadecimal string that represents a unicast address.
@property (nonatomic, copy) NSString *source;

/// The destination property represents the destination address for the Heartbeat messages. It contains
/// a 4- character hexadecimal string that represents a group or unicast address.
@property (nonatomic, copy) NSString *destination;

/// get dictionary from SigHeartbeatSubModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigHeartbeatSubModel;

/// Set dictionary to SigHeartbeatSubModel object.
/// @param dictionary SigHeartbeatSubModel dictionary object.
- (void)setDictionaryToSigHeartbeatSubModel:(NSDictionary *)dictionary;

@end


@interface SigOOBModel : SigModel
@property (nonatomic, assign) OOBSourceType sourceType;
@property (nonatomic, strong) NSString *UUIDString;
@property (nonatomic, strong) NSString *OOBString;
@property (nonatomic, strong) NSString *lastEditTimeString;
- (instancetype)initWithSourceType:(OOBSourceType)sourceType UUIDString:(NSString *)UUIDString OOBString:(NSString *)OOBString;
- (void)updateWithUUIDString:(NSString *)UUIDString OOBString:(NSString *)OOBString;
@end


/// 4.2.X+1 Bridging Table
/// - seeAlso: MshPRF_SBR_CR_r03.pdf  (page.9)
@interface SigSubnetBridgeModel : SigModel
/// Allowed directions for the bridged traffic, size is 8 bits.
@property (nonatomic, assign) SigDirectionsFieldValues directions;
/// NetKey Index of the first subnet, size is 12 bits.
@property (nonatomic, assign) UInt16 netKeyIndex1;
/// NetKey Index of the second subnet, size is 12 bits.
@property (nonatomic, assign) UInt16 netKeyIndex2;
/// Address of the node in the first subnet, size is 16 bits.
@property (nonatomic, assign) UInt16 address1;
/// Address of the node in the second subnet, size is 16 bits.
@property (nonatomic, assign) UInt16 address2;
/// SigSubnetBridgeModel parameters as Data.
@property (nonatomic,strong) NSData *parameters;
/// get dictionary from SubnetBridgeModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSubnetBridgeModel;

/// Set dictionary to SubnetBridgeModel object.
/// @param dictionary SubnetBridgeModel dictionary object.
- (void)setDictionaryToSubnetBridgeModel:(NSDictionary *)dictionary;
- (instancetype)initWithDirections:(SigDirectionsFieldValues)directions netKeyIndex1:(UInt16)netKeyIndex1 netKeyIndex2:(UInt16)netKeyIndex2 address1:(UInt16)address1 address2:(UInt16)address2;

/**
 * @brief   Initialize SigSubnetBridgeModel object.
 * @param   parameters    the hex data of SigSubnetBridgeModel.
 * @return  return `nil` when initialize SigSubnetBridgeModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

/// Get description string.
- (NSString *)getDescription;
@end


/// Table 4.Y+12: Bridged_Subnets_List entry format
/// - seeAlso: MshPRF_SBR_CR_r03.pdf  (page.14)
@interface SigBridgeSubnetModel : SigModel
/// NetKey Index of the first subnet, size is 12 bits.
@property (nonatomic, assign) UInt16 netKeyIndex1;
/// NetKey Index of the second subnet, size is 12 bits.
@property (nonatomic, assign) UInt16 netKeyIndex2;
/// SigBridgeSubnetModel parameters as Data.
@property (nonatomic,strong) NSData *parameters;
- (instancetype)initWithNetKeyIndex1:(UInt16)netKeyIndex1 netKeyIndex2:(UInt16)netKeyIndex2;

/**
 * @brief   Initialize SigBridgeSubnetModel object.
 * @param   parameters    the hex data of SigBridgeSubnetModel.
 * @return  return `nil` when initialize SigBridgeSubnetModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


/// Table 4.Y+15: Bridged_Addresses_List entry format
/// - seeAlso: MshPRF_SBR_CR_r03.pdf  (page.16)
@interface SigBridgedAddressesModel : SigModel
/// Address of the node in the first subnet, size is 16 bits.
@property (nonatomic, assign) UInt16 address1;
/// Address of the node in the second subnet, size is 16 bits.
@property (nonatomic, assign) UInt16 address2;
/// Allowed directions for the bridged traffic, size is 8 bits.
@property (nonatomic, assign) SigDirectionsFieldValues directions;
/// SigBridgedAddressesModel parameters as Data.
@property (nonatomic,strong) NSData *parameters;
- (instancetype)initWithAddress1:(UInt16)address1 address2:(UInt16)address2 directions:(SigDirectionsFieldValues)directions;

/**
 * @brief   Initialize SigBridgedAddressesModel object.
 * @param   parameters    the hex data of SigBridgedAddressesModel.
 * @return  return `nil` when initialize SigBridgedAddressesModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;

@end


/// Table 4.259: Format of the Aggregator Item
/// - seeAlso: MshPRFd1.1r13_clean.pdf  (page.388)
/// An empty item shall be represented by setting the value of the Length_Format field to 0 and the value of the Length_Short field to 0.
@interface SigOpcodesAggregatorItemModel : SigModel
/// The size is 1 bit. 0: Length_Short field is present, 1: Length_Long field is present.
@property (nonatomic, assign) BOOL lengthFormat;
/// The size is 7 bits. Size of Opcode_And_Parameters field (C.1) (C.1: Included if Length_Format is 0, otherwise excluded.)
@property (nonatomic, assign) UInt8 lengthShort;
/// The size is 15 bits. Size of Opcode_And_Parameters field (C.2) (C.2: Included if Length_Format is 1, otherwise excluded.)
@property (nonatomic, assign) UInt16 lengthLong;
/// The Opcode_And_Parameters field shall contain a valid opcode and parameters (contained in an unencrypted access layer message) for the given model.
@property (nonatomic, strong) NSData *opcodeAndParameters;
@property (nonatomic, strong) NSData *parameters;

- (instancetype)initWithLengthFormat:(BOOL)lengthFormat lengthShort:(UInt8)lengthShort lengthLong:(UInt8)lengthLong opcodeAndParameters:(NSData *)opcodeAndParameters;
- (instancetype)initWithSigMeshMessage:(SigMeshMessage *)meshMessage;
- (instancetype)initWithOpcodeAndParameters:(NSData *)opcodeAndParameters;
- (SigMeshMessage *)getSigMeshMessage;
@end


@interface SigOpCodeAndParametersModel : SigModel
@property (nonatomic, assign) UInt8 opCodeSize;
@property (nonatomic, assign) UInt32 opCode;
@property (nonatomic, strong) NSData *parameters;
@property (nonatomic, strong) NSData *opCodeAndParameters;

- (instancetype)initWithOpCodeAndParameters:(NSData *)opCodeAndParameters;
- (nullable SigMeshMessage *)getSigMeshMessage;

@end


/// Table 3.106: Structure of the Date Time characteristic, eg: 2017-11-12 01:00:30 -> @"E1070B0C01001E"
/// - seeAlso: GATT_Specification_Supplement_v5.pdf  (page.103)
@interface GattDateTimeModel : SigModel
/// Year as defined by the Gregorian calendar. Valid range 1582 to 9999. A value of 0 means that the year is not known. All other values are reserved for future use (RFU).
@property (nonatomic, assign) UInt16 year;
/// Month of the year as defined by the Gregorian calendar. Valid range 1 (January) to 12 (December). A value of 0 means that the month is not known. All other values are reserved for future use (RFU).
@property (nonatomic, assign) UInt8 month;
/// Day of the month as defined by the Gregorian calendar. Valid range 1 to 31. A value of 0 means that the day of month is not known. All other values are reserved for future use (RFU).
@property (nonatomic, assign) UInt8 day;
/// Number of hours past midnight. Valid range 0 to 23. All other values are reserved for future use (RFU).
@property (nonatomic, assign) UInt8 hours;
/// Number of minutes since the start of the hour. Valid range 0 to 59. All other values are reserved for future use (RFU).
@property (nonatomic, assign) UInt8 minutes;
/// Number of seconds since the start of the minute. Valid range 0 to 59. All other values are reserved for future use (RFU).
@property (nonatomic, assign) UInt8 seconds;

@property (nonatomic, strong) NSData *parameters;

/**
 * @brief   Initialize GattDateTimeModel object.
 * @param   parameters    the hex data of GattDateTimeModel.
 * @return  return `nil` when initialize GattDateTimeModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;
- (instancetype)initWithDate:(NSDate *)date;
- (instancetype)initWithYear:(UInt16)year month:(UInt8)month day:(UInt8)day hours:(UInt8)hours minutes:(UInt8)minutes seconds:(UInt8)seconds;

@end


/// Table 3.107: Structure of the Day Date Time characteristic
/// - seeAlso: GATT_Specification_Supplement_v5.pdf  (page.104)
@interface GattDayDateTimeModel : SigModel
/// It contains year, month, day, hours, minutes, seconds. The size is 7 bytes.
@property (nonatomic, strong) GattDateTimeModel *dateTime;
/// Refer to the Day of Week characteristic in Section 3.66.
@property (nonatomic, assign) GattDayOfWeek dayOfWeek;

@property (nonatomic, strong) NSData *parameters;

/**
 * @brief   Initialize GattDayDateTimeModel object.
 * @param   parameters    the hex data of GattDayDateTimeModel.
 * @return  return `nil` when initialize GattDayDateTimeModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters;
- (instancetype)initWithDate:(NSDate *)date;
- (instancetype)initWithYear:(UInt16)year month:(UInt8)month day:(UInt8)day hours:(UInt8)hours minutes:(UInt8)minutes seconds:(UInt8)seconds dayOfWeek:(GattDayOfWeek)dayOfWeek;

@end


@interface SigProxyFilterModel : SigModel
@property (nonatomic, assign) SigProxyFilerType filterType;
@property (nonatomic, strong) NSMutableArray <NSNumber *>*addressList;

/// get dictionary from SigProxyFilterModel object.
/// @returns return dictionary object.
- (NSDictionary *)getDictionaryOfSigProxyFilterModel;

/// Set dictionary to SigProxyFilterModel object.
/// @param dictionary SigProxyFilterModel dictionary object.
- (void)setDictionaryToSigProxyFilterModel:(NSDictionary *)dictionary;

@end


//缓存结构：{@"SeqAuth":@(7Bytes), @"SeqZero":@{@"value":@(13Bits), @"SeqAuths":@[@(7Bytes),···,@(7Bytes)]}}
//busy的segment为{@"SeqAuth":@(7Bytes), @"SeqZero":@{@"value":@(13Bits), @"SeqAuths":@[]}}，SeqAuths为空数组
@interface SigNodeSeqZeroModel : SigModel
@property (nonatomic, strong) NSNumber *value;
@property (nonatomic, strong) NSMutableArray <NSNumber *>*seqAuths;

- (instancetype)initWithValue:(NSNumber *)value seqAuth:(NSNumber *)seqAuth;

@end


@interface SigNodeSequenceNumberCacheModel : SigModel
@property (nonatomic, assign) UInt16 unicastAddress;
@property (nonatomic, strong) NSNumber *seqAuth;
@property (nonatomic, strong, nullable) SigNodeSeqZeroModel *seqZero;
@end

NS_ASSUME_NONNULL_END
