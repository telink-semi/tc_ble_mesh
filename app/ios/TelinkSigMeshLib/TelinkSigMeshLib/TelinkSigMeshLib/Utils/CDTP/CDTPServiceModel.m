/********************************************************************************************************
 * @file     CDTPServiceModel.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2023/6/30
 *
 * @par     Copyright (c) [2023], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "CDTPServiceModel.h"
#import "OTSBaseModel.h"

@interface CDTPServiceModel ()<CBPeripheralManagerDelegate>
/// CBPeripheralManager
/// The <code>CBPeripheralManager</code> class is an abstraction of the Peripheral and Broadcaster GAP roles, and the GATT Server
/// role. Its primary function is to allow you to manage published services within the GATT database, and to advertise these services
/// to other devices.
/// Each application has sandboxed access to the shared GATT database. You can add services to the database by calling {@link addService:};
/// they can be removed via {@link removeService:} and {@link removeAllServices}, as appropriate. While a service is in the database,
/// it is visible to and can be accessed by any connected GATT Client. However, applications that have not specified the "bluetooth-peripheral"
/// background mode will have the contents of their service(s) "disabled" when in the background. Any remote device trying to access
/// characteristic values or descriptors during this time will receive an error response.
/// Once you've published services that you want to share, you can ask to advertise their availability and allow other devices to connect
/// to you by calling {@link startAdvertising:}. Like the GATT database, advertisement is managed at the system level and shared by all
/// applications. This means that even if you aren't advertising at the moment, someone else might be!
@property (nonatomic, strong) CBPeripheralManager *peripheralManager;
/// 3.1 OTSFeatureCharacteristic
/// The OTS Feature characteristic exposes which optional features are supported by the Server implementation.
@property (nonatomic, strong) CBMutableCharacteristic *otsFeatureCharacteristic;
/// 3.2.2 Object Name Characteristic
/// The Object Name characteristic exposes the name of the Current Object.
@property (nonatomic, strong) CBMutableCharacteristic *objectNameCharacteristic;
/// 3.2.3 Object Type Characteristic
/// This characteristic exposes the type of the Current Object, identifying the object type by UUID.
/// Relevant object types that are to be used for a given application may be specified by a higher-level specification.
@property (nonatomic, strong) CBMutableCharacteristic *objectTypeCharacteristic;
/// 3.2.4 Object Size Characteristic
/// The Object Size characteristic exposes the current size as well as the allocated size of the Current Object.
@property (nonatomic, strong) CBMutableCharacteristic *objectSizeCharacteristic;
/// 3.2.5 Object First-Created Characteristic
/// The Object First-Created characteristic exposes a value representing a date and time when the object contents were
/// first created. Note that this may be an earlier date than the date of the creation of the object on the present Server; it
/// can relate to the original file creation date of a firmware release, for instance, rather than the time that the object was
/// written to the Server.
/// The date and time value shall be represented in Coordinated Universal Time (UTC).
/// Note that, once the value of the object first-created metadata has been set to a valid date and time (i.e., populated with
/// non-zero values), its value is not intended to change any further, since the time of first creation of an object is a constant
/// by definition.
@property (nonatomic, strong) CBMutableCharacteristic *objectFirstCreatedCharacteristic;
/// 3.2.6 Object Last-Modified Characteristic
/// The Object Last-Modified characteristic exposes a value representing a date and time when the object content was last
/// modified.
/// The date and time value shall be represented in Coordinated Universal Time (UTC).
@property (nonatomic, strong) CBMutableCharacteristic *objectLastModifiedCharacteristic;
/// 3.2.8 Object Properties Characteristic
/// This characteristic exposes a bit field representing the properties of the Current Object. When a bit is set to 1 (True),
/// the Current Object possesses the associated property as defined in [3].
/// An object’s properties determine the operations that are permitted to be performed on that object, as described in Section 3.3.
/// For example, an attempt to use the OACP Write Op Code when the Write bit of the properties for the Current Object is 0 (False)
/// will result in an error response.
@property (nonatomic, strong) CBMutableCharacteristic *objectPropertiesCharacteristic;
/// 3.3 ObjectActionControlPoint(OACP)
@property (nonatomic, strong) CBMutableCharacteristic *objectActionControlPointCharacteristic;
/// Description
/// An L2CAP channel is a duplex byte stream interface (similar to a network socket) that can be used for much more efficient
/// binary data transfer. This is used in some streaming applications, such as the Bluetooth Object Transfer Service.
/// The PSM (protocol/service multiplexer) is specified by the peripheral when it opens the channel. Some channels have predefined
/// identifiers controlled by Bluetooth organisation. Apple has also outlined a generic design for PSM exchange. To advertise an
/// L2CAP channel on a specific service, a characteristic with the UUID "ABDD3056-28FA-441D-A470-55A75A52553A" is added to
/// that service, and updated by the peripheral when the L2CAP channel is opened.
/// - seeAlso: https://classic.yarnpkg.com/en/package/cordova-plugin-ble-central#readme,l2cap.open.
@property (nonatomic, strong) CBMutableCharacteristic *psmCharacteristic;
/// Marks whether the Bluetooth service list has been initialized.
@property (nonatomic, assign) BOOL hasInitServices;//标记是否已经初始化服务列表
/// Mark whether Bluetooth broadcast data needs to be sent.
@property (nonatomic, assign) BOOL needAdvertise;//标记是否需要发送蓝牙广播数据
/// A CBL2CAPChannel represents a live L2CAP connection to a remote device.
@property (nonatomic, assign) CBL2CAPPSM channelPSM;
/// the OACP Features
@property (nonatomic, assign) struct OACPFeatures serviceOacpFeature;
/// The CDTP object of the share mesh network.
@property (nonatomic, strong) ObjectModel *objectModel;
/// Mark whether the Bluetooth channel has been opened.
@property (nonatomic, assign) BOOL isOpenedChannel;//标记是否已经打开蓝牙通道
/// The ChannelModel object.
@property (nonatomic, strong, nullable) ChannelModel *channelModel;

@end

@implementation CDTPServiceModel

#pragma mark - Private

- (void)setShareMeshDictionary:(NSDictionary * _Nonnull)shareMeshDictionary {
    _shareMeshDictionary = shareMeshDictionary;
    _objectModel = [[ObjectModel alloc] initWithMeshDictionary:shareMeshDictionary];
}

- (void)initServices {    
    //create characteristic list: Table 3.1: Requirements for characteristics used in the Object Transfer Service

    // OTS Feature Characteristic
    self.otsFeatureCharacteristic = [[CBMutableCharacteristic alloc] initWithType:[CBUUID UUIDWithString:[NSString stringWithFormat:@"%04X",Bluetooth16BitsUUID_OTSFeature]] properties:CBCharacteristicPropertyRead value:nil permissions:CBAttributePermissionsReadEncryptionRequired];
    [self.otsFeatureCharacteristic setDescriptors:@[[[CBMutableDescriptor alloc] initWithType: [CBUUID UUIDWithString:CBUUIDCharacteristicUserDescriptionString] value:@"OTS Feature"]]];

    // Object Name Characteristic
    self.objectNameCharacteristic = [[CBMutableCharacteristic alloc] initWithType:[CBUUID UUIDWithString:[NSString stringWithFormat:@"%04X",Bluetooth16BitsUUID_objectName]] properties:CBCharacteristicPropertyRead|CBCharacteristicPropertyWrite value:nil permissions:CBAttributePermissionsReadEncryptionRequired|CBAttributePermissionsWriteEncryptionRequired];
    [self.objectNameCharacteristic setDescriptors:@[[[CBMutableDescriptor alloc] initWithType: [CBUUID UUIDWithString:CBUUIDCharacteristicUserDescriptionString] value:@"Object Name"]]];

    // Object Type Characteristic
    self.objectTypeCharacteristic = [[CBMutableCharacteristic alloc] initWithType:[CBUUID UUIDWithString:[NSString stringWithFormat:@"%04X",Bluetooth16BitsUUID_objectType]] properties:CBCharacteristicPropertyRead value:nil permissions:CBAttributePermissionsReadEncryptionRequired];
    [self.objectTypeCharacteristic setDescriptors:@[[[CBMutableDescriptor alloc] initWithType: [CBUUID UUIDWithString:CBUUIDCharacteristicUserDescriptionString] value:@"Object Type"]]];

    // Object Size Characteristic
    self.objectSizeCharacteristic = [[CBMutableCharacteristic alloc] initWithType:[CBUUID UUIDWithString:[NSString stringWithFormat:@"%04X",Bluetooth16BitsUUID_objectSize]] properties:CBCharacteristicPropertyRead value:nil permissions:CBAttributePermissionsReadEncryptionRequired];
    [self.objectSizeCharacteristic setDescriptors:@[[[CBMutableDescriptor alloc] initWithType: [CBUUID UUIDWithString:CBUUIDCharacteristicUserDescriptionString] value:@"Object Size"]]];

    // Object First Created Characteristic
    self.objectFirstCreatedCharacteristic = [[CBMutableCharacteristic alloc] initWithType:[CBUUID UUIDWithString:[NSString stringWithFormat:@"%04X",Bluetooth16BitsUUID_objectFirstCreated]] properties:CBCharacteristicPropertyRead|CBCharacteristicPropertyWrite value:nil permissions:CBAttributePermissionsReadEncryptionRequired|CBAttributePermissionsWriteEncryptionRequired];
    [self.objectFirstCreatedCharacteristic setDescriptors:@[[[CBMutableDescriptor alloc] initWithType: [CBUUID UUIDWithString:CBUUIDCharacteristicUserDescriptionString] value:@"Object First Created"]]];

    // Object Last Modified Characteristic
    self.objectLastModifiedCharacteristic = [[CBMutableCharacteristic alloc] initWithType:[CBUUID UUIDWithString:[NSString stringWithFormat:@"%04X",Bluetooth16BitsUUID_objectLastModified]] properties:CBCharacteristicPropertyRead|CBCharacteristicPropertyWrite value:nil permissions:CBAttributePermissionsReadEncryptionRequired|CBAttributePermissionsWriteEncryptionRequired];
    [self.objectLastModifiedCharacteristic setDescriptors:@[[[CBMutableDescriptor alloc] initWithType: [CBUUID UUIDWithString:CBUUIDCharacteristicUserDescriptionString] value:@"Object Last Modified"]]];

    // Object Properties Characteristic
    self.objectPropertiesCharacteristic = [[CBMutableCharacteristic alloc] initWithType:[CBUUID UUIDWithString:[NSString stringWithFormat:@"%04X",Bluetooth16BitsUUID_objectProperties]] properties:CBCharacteristicPropertyRead|CBCharacteristicPropertyWrite value:nil permissions:CBAttributePermissionsReadEncryptionRequired|CBAttributePermissionsWriteEncryptionRequired];
    [self.objectPropertiesCharacteristic setDescriptors:@[[[CBMutableDescriptor alloc] initWithType: [CBUUID UUIDWithString:CBUUIDCharacteristicUserDescriptionString] value:@"Object Properties"]]];

    // Object Action Control Point Characteristic
    self.objectActionControlPointCharacteristic = [[CBMutableCharacteristic alloc] initWithType:[CBUUID UUIDWithString:[NSString stringWithFormat:@"%04X",Bluetooth16BitsUUID_objectActionControlPoint]] properties:CBCharacteristicPropertyIndicateEncryptionRequired|CBCharacteristicPropertyWrite value:nil permissions:CBAttributePermissionsWriteEncryptionRequired];
    [self.objectActionControlPointCharacteristic setDescriptors:@[[[CBMutableDescriptor alloc] initWithType: [CBUUID UUIDWithString:CBUUIDCharacteristicUserDescriptionString] value:@"Object Action Control Point"]]];

    // PSM
    self.psmCharacteristic = [[CBMutableCharacteristic alloc] initWithType:[CBUUID UUIDWithString:CBUUIDL2CAPPSMCharacteristicString] properties:CBCharacteristicPropertyRead value:nil permissions:CBAttributePermissionsReadEncryptionRequired];
    [self.psmCharacteristic setDescriptors:@[[[CBMutableDescriptor alloc] initWithType: [CBUUID UUIDWithString:CBUUIDCharacteristicUserDescriptionString] value:@"PSM"]]];
    
    //2.service1初始化并加入两个characteristics
    CBMutableService *service1 = [[CBMutableService alloc] initWithType:[CBUUID UUIDWithString:kObjectTransferService] primary:YES];
    [service1 setCharacteristics:@[self.otsFeatureCharacteristic, self.objectNameCharacteristic, self.objectTypeCharacteristic, self.objectSizeCharacteristic, self.objectFirstCreatedCharacteristic, self.objectLastModifiedCharacteristic, self.objectPropertiesCharacteristic, self.objectActionControlPointCharacteristic, self.psmCharacteristic]];
    [self.peripheralManager removeAllServices];
    [self.peripheralManager addService:service1];
}

///初始化服务列表中特征的值
- (void)initCharacteristicValue {
    [self.otsFeatureCharacteristic setValue:[self getValueOfOTSFeature]];
    [self.objectNameCharacteristic setValue:[self getValueOfObjectName]];
    [self.objectTypeCharacteristic setValue:[self getValueOfObjectType]];
    [self.objectSizeCharacteristic setValue:[self getValueOfObjectSize]];
    TelinkLogInfo(@"设置ObjectSize=0x%@, 取self.objectSizeCharacteristic.value=%@", [self getValueOfObjectSize], self.objectSizeCharacteristic.value);
    [self.objectFirstCreatedCharacteristic setValue:[self getValueOfObjectFirstCreated]];
    [self.objectLastModifiedCharacteristic setValue:[self getValueOfObjectLastModified]];
    [self.objectPropertiesCharacteristic setValue:[self getValueOfObjectProperties]];
    [self.psmCharacteristic setValue:[self getValueOfPSM]];
}

- (NSData *)getValueOfOTSFeature {
    NSData *data = [NSData dataWithBytes:&_serviceOacpFeature.value length:4];
    return data;
}

- (NSData *)getValueOfObjectName {
    return [self.objectModel.shareMeshDataSource.meshName dataUsingEncoding:NSUTF8StringEncoding];
}

- (NSData *)getValueOfObjectType {
    return [LibTools nsstringToHex:kPROXYService];
}

- (NSData *)getValueOfObjectSize {
    NSMutableData *mData = [NSMutableData data];
    UInt32 size = (UInt32)self.objectModel.objectData.length;
    UInt32 allocatedSize = 0xFFFFFFFF;
    [mData appendBytes:&size length:4];
    [mData appendBytes:&allocatedSize length:4];
    return mData;
}

- (NSData *)getValueOfObjectFirstCreated {
    GattDateTimeModel *time = [[GattDateTimeModel alloc] initWithDate:[NSDate date]];
    return time.parameters;
}

- (NSData *)getValueOfObjectLastModified {
    GattDateTimeModel *time = [[GattDateTimeModel alloc] initWithDate:[LibTools getDateWithTimeStringOfJson:self.objectModel.shareMeshDataSource.timestamp]];
    return time.parameters;
}

- (NSData *)getValueOfObjectProperties {
    struct ObjectProperties op = {};
    op.value = 0;
    op.Delete = 1;
    op.Read = 1;
    op.Write = 1;
    NSData *data = [NSData dataWithBytes:&op.value length:4];
    return data;
}

- (NSData *)getValueOfPSM {
    NSData *data = [NSData dataWithBytes:&_channelPSM length:2];
    return data;
}

- (void)callbackServiceReadError:(NSError *)error {
    dispatch_async(dispatch_get_main_queue(), ^{
        if ([self.delegate respondsToSelector:@selector(onServiceReadError:)]) {
            [self.delegate onServiceReadError:error];
        }
    });
}

- (void)callbackServiceWriteError:(NSError *)error {
    dispatch_async(dispatch_get_main_queue(), ^{
        if ([self.delegate respondsToSelector:@selector(onServiceWriteError:)]) {
            [self.delegate onServiceWriteError:error];
        }
    });
}

- (void)responseSuccessWithRequestOacpOpCode:(OACPOpCode)requestOacpOpCode {
    [self responseOacpResultWithResultCode:OACPResultCode_Success requestOacpOpCode:requestOacpOpCode];
}

- (void)responseOpCodeNotSupportedWithRequestOacpOpCode:(OACPOpCode)requestOacpOpCode {
    [self responseOacpResultWithResultCode:OACPResultCode_OpCodeNotSupported requestOacpOpCode:requestOacpOpCode];
}

- (void)responseInvalidParameterWithRequestOacpOpCode:(OACPOpCode)requestOacpOpCode {
    [self responseOacpResultWithResultCode:OACPResultCode_InvalidParameter requestOacpOpCode:requestOacpOpCode];
}

- (void)responseInsufficientResourcesWithRequestOacpOpCode:(OACPOpCode)requestOacpOpCode {
    [self responseOacpResultWithResultCode:OACPResultCode_InsufficientResources requestOacpOpCode:requestOacpOpCode];
}

- (void)responseInvalidObjectWithRequestOacpOpCode:(OACPOpCode)requestOacpOpCode {
    [self responseOacpResultWithResultCode:OACPResultCode_InvalidObject requestOacpOpCode:requestOacpOpCode];
}

- (void)responseChannelUnavailableWithRequestOacpOpCode:(OACPOpCode)requestOacpOpCode {
    [self responseOacpResultWithResultCode:OACPResultCode_ChannelUnavailable requestOacpOpCode:requestOacpOpCode];
}

- (void)responseUnsupportedTypeWithRequestOacpOpCode:(OACPOpCode)requestOacpOpCode {
    [self responseOacpResultWithResultCode:OACPResultCode_UnsupportedType requestOacpOpCode:requestOacpOpCode];
}

- (void)responseProcedureNotPermittedWithRequestOacpOpCode:(OACPOpCode)requestOacpOpCode {
    [self responseOacpResultWithResultCode:OACPResultCode_ProcedureNotPermitted requestOacpOpCode:requestOacpOpCode];
}

- (void)responseObjectLockedWithRequestOacpOpCode:(OACPOpCode)requestOacpOpCode {
    [self responseOacpResultWithResultCode:OACPResultCode_ObjectLocked requestOacpOpCode:requestOacpOpCode];
}

- (void)responseOperationFailedWithRequestOacpOpCode:(OACPOpCode)requestOacpOpCode {
    [self responseOacpResultWithResultCode:OACPResultCode_OperationFailed requestOacpOpCode:requestOacpOpCode];
}

- (void)responseOacpResultWithResultCode:(OACPResultCode)resultCode requestOacpOpCode:(OACPOpCode)requestOacpOpCode {
    [self responseOacpResultWithResultCode:resultCode requestOacpOpCode:requestOacpOpCode responseParameter:nil];
}

- (void)responseOacpResultWithResultCode:(OACPResultCode)resultCode requestOacpOpCode:(OACPOpCode)requestOacpOpCode responseParameter:(NSData * _Nullable)responseParameter {
    OACPResponseCodeModel *response = [[OACPResponseCodeModel alloc] initWithRequestOacpOpCode:requestOacpOpCode ResultCode:resultCode responseParameter:responseParameter];
    [self.peripheralManager updateValue:response.payload forCharacteristic:self.objectActionControlPointCharacteristic onSubscribedCentrals:nil];
}

- (void)handleReceivceDataOfOACPCharacteristic:(NSData *)data {
    UInt8 tem8 = 0;
    Byte *dataByte = (Byte *)data.bytes;
    memcpy(&tem8, dataByte, 1);
    OACPOpCode op = tem8;
    if (op == OACPOpCode_Create) {
        [self handleCreateOpCodeDataOfOACPCharacteristic:data];
    } else if (op == OACPOpCode_Delete) {
        [self handleDeleteOpCodeDataOfOACPCharacteristic:data];
    } else if (op == OACPOpCode_CalculateChecksum) {
        [self handleCalculateChecksumOpCodeDataOfOACPCharacteristic:data];
    } else if (op == OACPOpCode_Read) {
        [self handleReadOpCodeDataOfOACPCharacteristic:data];
    } else if (op == OACPOpCode_Write) {
        [self handleWriteOpCodeDataOfOACPCharacteristic:data];
    } else if (op == OACPOpCode_Abort) {
        [self handleAbortOpCodeDataOfOACPCharacteristic:data];
    } else {
        // OpCode Not Supported
        [self responseOpCodeNotSupportedWithRequestOacpOpCode:op];
    }
}

- (void)handleCreateOpCodeDataOfOACPCharacteristic:(NSData *)data {
    if (self.serviceOacpFeature.OACPCreateOpCodeSupported == 0) {
        // The Create Op Code is not supported by the Server.
        // Op Code Not Supported
        [self responseOpCodeNotSupportedWithRequestOacpOpCode:OACPOpCode_Create];
        return;
    }
    if (data.length < 7) {
        // The Parameter received does not meet the requirements of the service.
        // Invalid Parameter
        [self responseInvalidParameterWithRequestOacpOpCode:OACPOpCode_Create];
        return;
    }
    Byte *dataByte = (Byte *)data.bytes;
    UInt32 tem32 = 0;
    UInt16 tem16 = 0;
    memcpy(&tem32, dataByte+1, 4);
    memcpy(&tem16, dataByte+5, 2);
    if (tem16 != self.objectModel.objectType) {
        // The Server does not accept an object of the type specified in the Type parameter.
        // Unsupported Type
        [self responseUnsupportedTypeWithRequestOacpOpCode:OACPOpCode_Create];
        return;
    }
    long freeSize = [LibTools freeDiskSpaceInBytes];
    if (freeSize - tem32 < 500 * 1024 * 1024) {
        //如果接收object后手机剩余内存小于500M，则上报资源不足，且不允许进行OTS传输。
        // The Server cannot accept an object of the size specified in the Size parameter.
        // Insufficient Resources
        [self responseInsufficientResourcesWithRequestOacpOpCode:OACPOpCode_Create];
        return;
    }
    
    /*创建 Object 成功流程*/
    self.objectModel = [[ObjectModel alloc] init];
    self.objectModel.objectSize = tem32;
    self.objectModel.objectData = [NSMutableData data];
    self.objectSizeCharacteristic.value = [LibTools nsstringToHex:@"00"];
    self.objectFirstCreatedCharacteristic.value = [LibTools nsstringToHex:@"00"];
    self.objectLastModifiedCharacteristic.value = [LibTools nsstringToHex:@"00"];
    struct ObjectProperties objectProperties = {0};
    objectProperties.value = 0;
    objectProperties.Write = 1;
    self.objectModel.objectProperties = objectProperties;
    [self responseSuccessWithRequestOacpOpCode:OACPOpCode_Create];
}

- (void)handleDeleteOpCodeDataOfOACPCharacteristic:(NSData *)data {
    if (self.serviceOacpFeature.OACPDeleteOpCodeSupported == 0) {
        // The Delete Op Code is not supported by the Server.
        // Op Code Not Supported
        [self responseOpCodeNotSupportedWithRequestOacpOpCode:OACPOpCode_Delete];
        return;
    }
    if (self.objectModel == nil || self.objectModel.objectData == nil || self.objectModel.objectData.length == 0) {
        // The Current Object is an Invalid Object.
        // Invalid Object
        [self responseInvalidObjectWithRequestOacpOpCode:OACPOpCode_Delete];
        return;
    }
    if (self.objectModel != nil && self.objectModel.objectProperties.Delete == 0) {
        // The object’s properties do not permit deletion of the object.
        // Procedure Not Permitted
        [self responseProcedureNotPermittedWithRequestOacpOpCode:OACPOpCode_Delete];
        return;
    }
    if (self.objectModel.objectLocked || self.objectModel.isReading || self.objectModel.isWriting) {
        // The Current Object is locked by the Server.
        // An object transfer is in progress that is using the Current Object.
        // Object Locked
        [self responseObjectLockedWithRequestOacpOpCode:OACPOpCode_Delete];
        return;
    }
    
    /*删除 Object 成功流程*/
    self.objectModel = [[ObjectModel alloc] init];
    [self responseSuccessWithRequestOacpOpCode:OACPOpCode_Delete];
}

- (void)handleCalculateChecksumOpCodeDataOfOACPCharacteristic:(NSData *)data {
    if (self.serviceOacpFeature.OACPCalculateChecksumOpCodeSupported == 0) {
        // The Calculate Checksum Op Code is not supported by the Server.
        // OpCode Not Supported
        [self responseOpCodeNotSupportedWithRequestOacpOpCode:OACPOpCode_CalculateChecksum];
        return;
    }
    if (self.objectModel == nil || self.objectModel.objectData == nil || self.objectModel.objectData.length == 0) {
        // The Current Object is an Invalid Object.
        // Invalid Object
        [self responseInvalidObjectWithRequestOacpOpCode:OACPOpCode_CalculateChecksum];
        return;
    }
    if (data.length < 9) {
        // Invalid Parameter
        [self responseInvalidParameterWithRequestOacpOpCode:OACPOpCode_CalculateChecksum];
        return;
    }
    Byte *dataByte = (Byte *)data.bytes;
    UInt32 offset = 0;
    UInt32 length = 0;
    memcpy(&offset, dataByte+1, 4);
    memcpy(&length, dataByte+5, 4);
    if (offset >= self.objectModel.objectSize || length > self.objectModel.objectSize || (self.objectModel.objectSize - offset < length)) {
        // The sum of the values of the Offset and Length parameters exceeds the value of the Current Size field of the Object Size characteristic.
        // Invalid Parameter
        [self responseInvalidParameterWithRequestOacpOpCode:OACPOpCode_CalculateChecksum];
        return;
    }
    if (self.objectModel.objectLocked || self.objectModel.isReading || self.objectModel.isWriting) {
        // The Current Object is locked by the Server.
        // An object transfer is in progress that is using the Current Object.
        // Object Locked
        [self responseObjectLockedWithRequestOacpOpCode:OACPOpCode_CalculateChecksum];
        return;
    }
    
    /*验证 Object 校验码 成功流程*/
    NSData *sourceData = [self.objectModel.objectData subdataWithRange:NSMakeRange(offset, length)];
    UInt32 checkSum = [LibTools getCRC32OfData:sourceData];
    NSData *checkSumData = [NSData dataWithBytes:&checkSum length:4];
    [self responseOacpResultWithResultCode:OACPResultCode_Success requestOacpOpCode:OACPOpCode_CalculateChecksum responseParameter:checkSumData];
}

- (void)handleReadOpCodeDataOfOACPCharacteristic:(NSData *)data {
    if (self.serviceOacpFeature.OACPReadOpCodeSupported == 0) {
        // The Read Op Code is not supported by the Server.
        // Op Code Not Supported
        [self responseOpCodeNotSupportedWithRequestOacpOpCode:OACPOpCode_Read];
        return;
    }
    if (self.objectModel == nil || self.objectModel.objectData == nil || self.objectModel.objectData.length == 0) {
        // The Current Object is an Invalid Object.
        // Invalid Object
        [self responseInvalidObjectWithRequestOacpOpCode:OACPOpCode_Read];
        return;
    }
    if (self.objectModel != nil && self.objectModel.objectProperties.Read == 0) {
        // The object’s properties do not permit reading the object.
        // Procedure Not Permitted
        [self responseProcedureNotPermittedWithRequestOacpOpCode:OACPOpCode_Read];
        return;
    }
    if (self.channelModel == nil || self.isOpenedChannel == NO) {
        // An Object Transfer Channel was not available for use.
        // Channel Unavailable
        [self responseChannelUnavailableWithRequestOacpOpCode:OACPOpCode_Read];
        return;
    }
    Byte *dataByte = (Byte *)data.bytes;
    UInt32 offset = 0;
    UInt32 length = 0;
    memcpy(&offset, dataByte+1, 4);
    memcpy(&length, dataByte+5, 4);
    if (offset >= self.objectModel.objectSize || length > self.objectModel.objectSize || (self.objectModel.objectSize - offset < length)) {
        // The value of the Offset parameter exceeds the value of the Current Size field of the Object Size characteristic.
        // The sum of the values of the Offset and Length parameters exceeds the value of the Current Size field of the Object Size characteristic.
        // Invalid Parameter
        [self responseInvalidParameterWithRequestOacpOpCode:OACPOpCode_Read];
        return;
    }
    
    /*
     // The value of the Length parameter exceeds the number of octets that the Server has the capacity to read from the object.
     // Insufficient Resources
     read流程的资源不足，未弄清楚这个长度如何获取，后面在补充该判断。
     */
#warning need to fix Insufficient Resources of READ
    
    if (self.objectModel.objectLocked || self.objectModel.isReading || self.objectModel.isWriting) {
        // An object transfer is already in progress that is using the Current Object.
        // Object Locked
        [self responseObjectLockedWithRequestOacpOpCode:OACPOpCode_Read];
        return;
    }
    
    /*读 Object 成功流程：1.返回成功。2.准备好数据，Client端会在接收到Read成功后再通过OACP Read指令来读取刚刚的数据*/
    
    //可能上一次是中断的流程，流并未打开，所以在回复成功之前先确保流是打开的状态
    [self.channelModel openStreamAction];

    NSData *sourceData = [self.objectModel.objectData subdataWithRange:NSMakeRange(offset, length)];
    self.objectModel.objectLocked = YES;
    [self responseSuccessWithRequestOacpOpCode:OACPOpCode_Read];
    
    //返回成功后延时100ms再发送流数据
    __weak typeof(self) weakSelf = self;
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        weakSelf.objectModel.isWriting = YES;
        [weakSelf.channelModel writeStreamWithData:sourceData progress:^(float progress) {
            float pro = (offset + length * progress)/weakSelf.objectModel.objectSize;
            dispatch_async(dispatch_get_main_queue(), ^{
                if ([weakSelf.delegate respondsToSelector:@selector(onServiceReadProgress:)]) {
                    [weakSelf.delegate onServiceReadProgress:pro];
                }
            });
        } complete:^(NSData * _Nullable data) {
            weakSelf.objectModel.isWriting = NO;
            weakSelf.objectModel.objectLocked = NO;
            if (data != nil) {
                //成功通过上面的progress=1.0来判断
                TelinkLogInfo(@"write object data to client=%@", sourceData);
            } else {
                dispatch_async(dispatch_get_main_queue(), ^{
                    if ([weakSelf.delegate respondsToSelector:@selector(onServiceReadError:)]) {
                        NSError *error = [NSError errorWithDomain:@"write object fail" code:-1 userInfo:nil];
                        [weakSelf.delegate onServiceReadError:error];
                    }
                });
            }
        }];
        
    });
}

- (void)handleWriteOpCodeDataOfOACPCharacteristic:(NSData *)data {
    if (self.serviceOacpFeature.OACPWriteOpCodeSupported == 0) {
        // OpCode Not Supported
        [self responseOpCodeNotSupportedWithRequestOacpOpCode:OACPOpCode_Write];
        return;
    }
    if (self.objectModel == nil) {
        // Invalid Object
        [self responseInvalidObjectWithRequestOacpOpCode:OACPOpCode_Write];
        return;
    }
    if (self.objectModel != nil && self.objectModel.objectProperties.Write == 0) {
        // Procedure Not Permitted
        [self responseProcedureNotPermittedWithRequestOacpOpCode:OACPOpCode_Write];
        return;
    }
    /*
     Patching was attempted but patching is not supported by the Server.
     已尝试进行修补，但服务器不支持进行修补。
     */
    /*
     Patching was attempted but the object’s properties do not permit patching of the object contents.
     已尝试修补，但对象的属性不允许修补对象内容。
     */
    
    Byte *dataByte = (Byte *)data.bytes;
    UInt8 tem8 = 0;
    if (data.length >= 10) {
        memcpy(&tem8, dataByte+9, 1);
        if (((tem8 >> 1) & 1) == 1) {
            // Truncation was attempted but the object’s properties do not permit truncation of the object contents.
            // Procedure Not Permitted
            [self responseProcedureNotPermittedWithRequestOacpOpCode:OACPOpCode_Write];
            return;
        }
    }
    if (self.channelModel == nil || self.isOpenedChannel == NO) {
        // An Object Transfer Channel was not available for use.
        // Channel Unavailable
        [self responseChannelUnavailableWithRequestOacpOpCode:OACPOpCode_Write];
        return;
    }
    if (data.length >= 10) {
        memcpy(&tem8, dataByte+9, 1);
        if ((tem8 & 0b11111101) > 0) {
            // The Mode parameter contains an RFU value.
            // Invalid Parameter
            [self responseInvalidParameterWithRequestOacpOpCode:OACPOpCode_Write];
            return;
        }
    }
    UInt32 offset = 0;
    UInt32 length = 0;
    if (data.length >= 9) {
        memcpy(&offset, dataByte+1, 4);
        memcpy(&length, dataByte+5, 4);
        /*Mesh Object只支持读写属性，不支持“附加”、“截断”或“修补”属性。 我的理解是只能通过一次写进去。*/
        /*Object单次接收是只能接收到167k左右的数据，之后Client端是可以继续发送数据但Service端是没有数据上报了，修改做法为单次写100k的数据，总数据大于100k则分成多个100k的数据端来发送。*/
        if (offset >= self.objectModel.objectSize || length > self.objectModel.objectSize || (self.objectModel.objectSize - offset < length)) {
            //The value of the Offset parameter exceeds the value of the Current Size field of the Object Size characteristic.
            // The sum of the values of the Offset and Length parameters exceeds the value of the Allocated Size field of the Object Size characteristic AND the Server does NOT support appending additional data to an object.
            // Invalid Parameter
            [self responseInvalidParameterWithRequestOacpOpCode:OACPOpCode_Write];
            return;
        }
    }
    
    /*
     //Insufficient Resources
     The value of the Length parameter exceeds the number of octets that the Server has the capacity to write to the object.
     */
    
    if (self.objectModel.objectLocked || self.objectModel.isReading || self.objectModel.isWriting) {
        // The Current Object is locked by the Server.
        // An object transfer is already in progress that is using the Current Object.
        // Object Locked
        [self responseObjectLockedWithRequestOacpOpCode:OACPOpCode_Write];
        return;
    }
    
    /*Write Object 指令成功流程：1.返回成功。2.准备好接收数据的buff，Client端会在接收到Write成功后再通过OACP Write指令来写数据到刚刚的buff*/
    
    //可能上一次是中断的流程，流并未打开，所以在回复成功之前先确保流是打开的状态
    [self.channelModel openStreamAction];
    
    self.objectModel.objectData = [NSMutableData data];
    self.objectModel.objectLocked = YES;
    [self responseSuccessWithRequestOacpOpCode:OACPOpCode_Write];
    
    self.objectModel.isReading = YES;
    __weak typeof(self) weakSelf = self;
    [self.channelModel readStreamWithSize:length progress:^(float progress) {
        if (progress != 1.0) {
            dispatch_async(dispatch_get_main_queue(), ^{
                if ([weakSelf.delegate respondsToSelector:@selector(onServiceWriteProgress:objectModel:)]) {
                    [weakSelf.delegate onServiceWriteProgress:progress objectModel:weakSelf.objectModel];
                }
            });
        }
    } complete:^(NSData * _Nullable data) {
        weakSelf.objectModel.isReading = NO;
        weakSelf.objectModel.objectLocked = NO;
        if (data.length == weakSelf.objectModel.objectSize) {
            weakSelf.objectModel.objectData = data;
            dispatch_async(dispatch_get_main_queue(), ^{
                if ([weakSelf.delegate respondsToSelector:@selector(onServiceWriteProgress:objectModel:)]) {
                    [weakSelf.delegate onServiceWriteProgress:1.0 objectModel:weakSelf.objectModel];
                }
            });
        }
    }];
}

- (void)handleAbortOpCodeDataOfOACPCharacteristic:(NSData *)data {
    if (self.serviceOacpFeature.OACPAbortOpCodeSupported == 0) {
        // The Abort Op Code is not supported by the Server.
        // OpCode Not Supported
        [self responseOpCodeNotSupportedWithRequestOacpOpCode:OACPOpCode_Abort];
        return;
    }
    if (!self.objectModel.isReading && !self.objectModel.isWriting) {
        // No OACP Read operation is in progress – there is nothing to abort.
        // Operation Failed
        [self responseOperationFailedWithRequestOacpOpCode:OACPOpCode_Abort];
        return;
    }
    [self.channelModel closeStreamAction];
    
    /*Abort Object 指令成功流程：1.停止OACP Read的数据上报操作。2.返回abort成功*/
    self.objectModel.objectLocked = NO;
    [self responseSuccessWithRequestOacpOpCode:OACPOpCode_Abort];
}

#pragma mark - Public

/**
 * @brief   Initialize CDTPServiceModel object.
 * @param   shareMeshDictionary  The dictionary of the share mesh network.
 * @return  return `nil` when initialize CDTPServiceModel object fail.
 */
- (instancetype)initWithShareMeshDictionary:(NSDictionary *)shareMeshDictionary {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        [self setShareMeshDictionary:shareMeshDictionary];
        _hasInitServices = NO;
        struct OACPFeatures feature = {};
        feature.value = 0;
        feature.OACPCreateOpCodeSupported = 1;
        feature.OACPDeleteOpCodeSupported = 1;
        feature.OACPCalculateChecksumOpCodeSupported = 1;
        feature.OACPReadOpCodeSupported = 1;
        feature.OACPWriteOpCodeSupported = 1;
        feature.OACPAbortOpCodeSupported = 1;
        _serviceOacpFeature = feature;
        _peripheralManager = [[CBPeripheralManager alloc] initWithDelegate:self queue:dispatch_get_main_queue()];
    }
    return self;
}

/// CDTP Service start Advertising.
- (void)startAdvertising {
    NSDictionary *dict = @{CBAdvertisementDataServiceUUIDsKey : @[[CBUUID UUIDWithString:kObjectTransferService]], CBAdvertisementDataLocalNameKey : @"tlk"};
    if (@available(iOS 10.0, *)) {
        if (self.peripheralManager.state == CBManagerStatePoweredOn) {
            [self.peripheralManager startAdvertising:dict];
        } else {
            self.needAdvertise = YES;
        }
    } else {
        if (self.peripheralManager.state == CBPeripheralManagerStatePoweredOn) {
            [self.peripheralManager startAdvertising:dict];
        } else {
            self.needAdvertise = YES;
        }
    }
}

/// CDTP Service stop Advertising.
- (void)stopAdvertising {
    if (self.peripheralManager.isAdvertising) {
        [self.peripheralManager stopAdvertising];
    }
}

/// CDTP Service stop CDTP Share progress.
- (void)endShare {
    [self stopAdvertising];
    if (self.channelPSM > 0) {
        [self.peripheralManager unpublishL2CAPChannel:self.channelPSM];
    }
}

#pragma mark - CBPeripheralManagerDelegate (Service of CDTP)

/*!
 *  @method peripheralManagerDidUpdateState:
 *
 *  @param peripheral   The peripheral manager whose state has changed.
 *
 *  @discussion         Invoked whenever the peripheral manager's state has been updated. Commands should only be issued when the state is
 *                      <code>CBPeripheralManagerStatePoweredOn</code>. A state below <code>CBPeripheralManagerStatePoweredOn</code>
 *                      implies that advertisement has paused and any connected centrals have been disconnected. If the state moves below
 *                      <code>CBPeripheralManagerStatePoweredOff</code>, advertisement is stopped and must be explicitly restarted, and the
 *                      local database is cleared and all services must be re-added.
 *
 *  @see                state
 *
 */
- (void)peripheralManagerDidUpdateState:(CBPeripheralManager *)peripheral{
    NSLog(@"callback peripheralManagerDidUpdateState=%ld", (long)peripheral.state);
    BOOL isOpen = NO;
    switch (peripheral.state) {
            //在这里判断蓝牙设别的状态  当开启了则可调用  setUp方法(自定义)
        case CBPeripheralManagerStatePoweredOn:
            NSLog(@"powered on");
            [peripheral publishL2CAPChannelWithEncryption:YES];
            isOpen = YES;
            if (!self.hasInitServices) {
                [self initServices];
            }
            break;
        case CBPeripheralManagerStatePoweredOff:
            NSLog(@"powered off");
            if (self.channelPSM > 0) {
                [peripheral unpublishL2CAPChannel:self.channelPSM];
            }
            break;
        default:
            break;
    }
}

/*!
 *  @method peripheralManager:didAddService:error:
 *
 *  @param peripheral   The peripheral manager providing this information.
 *  @param service      The service that was added to the local database.
 *  @param error        If an error occurred, the cause of the failure.
 *
 *  @discussion         This method returns the result of an @link addService: @/link call. If the service could
 *                      not be published to the local database, the cause will be detailed in the <i>error</i> parameter.
 *
 */
- (void)peripheralManager:(CBPeripheralManager *)peripheral didAddService:(CBService *)service error:(NSError *)error{
    NSLog(@"callback didAddService");
    if (error) {
        NSLog(@"error =%@",error.localizedDescription);
    }else{
        self.hasInitServices = YES;
        [self initCharacteristicValue];
        if (self.needAdvertise) {
            [self startAdvertising];
        }
    }
}

/*!
 *  @method peripheralManagerDidStartAdvertising:error:
 *
 *  @param peripheral   The peripheral manager providing this information.
 *  @param error        If an error occurred, the cause of the failure.
 *
 *  @discussion         This method returns the result of a @link startAdvertising: @/link call. If advertisement could
 *                      not be started, the cause will be detailed in the <i>error</i> parameter.
 *
 */
- (void)peripheralManagerDidStartAdvertising:(CBPeripheralManager *)peripheral error:(NSError *)error{
    NSLog(@"callback peripheralManagerDidStartAdvertising");
    BOOL isSuccess = NO;
    if (error) {
        NSLog(@"error =%@",error.localizedDescription);
    } else {
        isSuccess = YES;
    }
}

/*!
 *  @method peripheralManager:didReceiveReadRequest:
 *
 *  @param peripheral   The peripheral manager requesting this information.
 *  @param request      A <code>CBATTRequest</code> object.
 *
 *  @discussion         This method is invoked when <i>peripheral</i> receives an ATT request for a characteristic with a dynamic value.
 *                      For every invocation of this method, @link respondToRequest:withResult: @/link must be called.
 *
 *  @see                CBATTRequest
 *
 */
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

/*!
 *  @method peripheralManager:didReceiveWriteRequests:
 *
 *  @param peripheral   The peripheral manager requesting this information.
 *  @param requests     A list of one or more <code>CBATTRequest</code> objects.
 *
 *  @discussion         This method is invoked when <i>peripheral</i> receives an ATT request or command for one or more characteristics with a dynamic value.
 *                      For every invocation of this method, @link respondToRequest:withResult: @/link should be called exactly once. If <i>requests</i> contains
 *                      multiple requests, they must be treated as an atomic unit. If the execution of one of the requests would cause a failure, the request
 *                      and error reason should be provided to <code>respondToRequest:withResult:</code> and none of the requests should be executed.
 *
 *  @see                CBATTRequest
 *
 */
- (void)peripheralManager:(CBPeripheralManager *)peripheral didReceiveWriteRequests:(NSArray *)requests{
    NSLog(@"callback didReceiveWriteRequests");
    CBATTRequest *request = requests[0];
    
    //判断是否有写数据的权限
    if (request.characteristic.properties & CBCharacteristicPropertyWrite) {
        [self.peripheralManager respondToRequest:request withResult:CBATTErrorSuccess];
        
        //需要转换成CBMutableCharacteristic对象才能进行写值
        CBMutableCharacteristic *c =(CBMutableCharacteristic *)request.characteristic;
        c.value = request.value;
        NSData *data = request.value;
        NSLog(@"request.value=%@", [LibTools convertDataToHexStr:request.value]);
        if (c == self.objectActionControlPointCharacteristic) {
            [self handleReceivceDataOfOACPCharacteristic:data];
        } else if (c == self.objectNameCharacteristic) {
            self.objectModel.objectName = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        } else if (c == self.objectFirstCreatedCharacteristic) {
            self.objectModel.firstCreatedData = data;
        } else if (c == self.objectLastModifiedCharacteristic) {
            self.objectModel.lastModifiedData = data;
        } else if (c == self.objectPropertiesCharacteristic) {
            Byte *dataByte = (Byte *)data.bytes;
            UInt32 tem32 = 0;
            memcpy(&tem32, dataByte+1, 4);
            struct ObjectProperties objectProperties = {0};
            objectProperties.value = tem32;
            self.objectModel.objectProperties = objectProperties;
        }
    } else {
        [self.peripheralManager respondToRequest:request withResult:CBATTErrorWriteNotPermitted];
    }
}

/*!
 *  @method peripheralManagerIsReadyToUpdateSubscribers:
 *
 *  @param peripheral   The peripheral manager providing this update.
 *
 *  @discussion         This method is invoked after a failed call to @link updateValue:forCharacteristic:onSubscribedCentrals: @/link, when <i>peripheral</i> is again
 *                      ready to send characteristic value updates.
 *
 */
- (void)peripheralManagerIsReadyToUpdateSubscribers:(CBPeripheralManager *)peripheral{
    NSLog(@"callback peripheralManagerIsReadyToUpdateSubscribers");
}

/*!
 *  @method peripheralManager:central:didSubscribeToCharacteristic:
 *
 *  @param peripheral       The peripheral manager providing this update.
 *  @param central          The central that issued the command.
 *  @param characteristic   The characteristic on which notifications or indications were enabled.
 *
 *  @discussion             This method is invoked when a central configures <i>characteristic</i> to notify or indicate.
 *                          It should be used as a cue to start sending updates as the characteristic value changes.
 *
 */
- (void)peripheralManager:(CBPeripheralManager *)peripheral central:(CBCentral *)central didSubscribeToCharacteristic:(CBCharacteristic *)characteristic{
    NSLog(@"callback didSubscribeToCharacteristic");
    [self stopAdvertising];
}

/*!
 *  @method peripheralManager:central:didUnsubscribeFromCharacteristic:
 *
 *  @param peripheral       The peripheral manager providing this update.
 *  @param central          The central that issued the command.
 *  @param characteristic   The characteristic on which notifications or indications were disabled.
 *
 *  @discussion             This method is invoked when a central removes notifications/indications from <i>characteristic</i>.
 *
 */
- (void)peripheralManager:(CBPeripheralManager *)peripheral central:(CBCentral *)central didUnsubscribeFromCharacteristic:(CBCharacteristic *)characteristic{
    NSLog(@"callback didUnsubscribeFromCharacteristic");
    if (peripheral.state != CBPeripheralStateConnected) {
        NSError *error = [NSError errorWithDomain:@"The peripheral had disconnected!" code:-1 userInfo:nil];
        if (self.objectModel.isReading) {
            [self callbackServiceReadError:error];
        } else if (self.objectModel.isWriting) {
            [self callbackServiceWriteError:error];
        }
        [self.channelModel closeChannelAction];
    }
}

/*!
 *  @method peripheralManager:didPublishL2CAPChannel:error:
 *
 *  @param peripheral   The peripheral manager requesting this information.
 *  @param PSM            The PSM of the channel that was published.
 *  @param error        If an error occurred, the cause of the failure.
 *
 *  @discussion         This method is the response to a  @link publishL2CAPChannel: @/link call.  The PSM will contain the PSM that was assigned for the published
 *                        channel
 *
 */
- (void)peripheralManager:(CBPeripheralManager *)peripheral didPublishL2CAPChannel:(CBL2CAPPSM)PSM error:(nullable NSError *)error {
    TelinkLogInfo(@"peripheral=%@ PSM=%d error=%@", peripheral, PSM, error);
    self.channelPSM = PSM;
}

/*!
 *  @method peripheralManager:didUnublishL2CAPChannel:error:
 *
 *  @param peripheral   The peripheral manager requesting this information.
 *  @param PSM            The PSM of the channel that was published.
 *  @param error        If an error occurred, the cause of the failure.
 *
 *  @discussion         This method is the response to a  @link unpublishL2CAPChannel: @/link call.
 *
 */
- (void)peripheralManager:(CBPeripheralManager *)peripheral didUnpublishL2CAPChannel:(CBL2CAPPSM)PSM error:(nullable NSError *)error {
    TelinkLogInfo(@"peripheral=%@ PSM=%d error=%@", peripheral, PSM, error);

}

/*!
 *  @method peripheralManager:didOpenL2CAPChannel:error:
 *
 *  @param peripheral       The peripheral manager requesting this information.
 *  @param channel              A <code>CBL2CAPChannel</code> object.
 *    @param error        If an error occurred, the cause of the failure.
 *
 *  @discussion            This method returns the result of establishing an incoming L2CAP channel , following publishing a channel using @link publishL2CAPChannel: @link call.
 *
 */
- (void)peripheralManager:(CBPeripheralManager *)peripheral didOpenL2CAPChannel:(nullable CBL2CAPChannel *)channel error:(nullable NSError *)error {
    TelinkLogInfo(@"peripheral=%@ channel=%d error=%@", peripheral, channel, error);
    if (error) {
        self.isOpenedChannel = NO;
        self.channelModel = nil;
    } else {
        self.isOpenedChannel = YES;
        self.channelModel = [[ChannelModel alloc] initWithChannel:channel];
    }
}

@end
