/********************************************************************************************************
 * @file     OTSBaseModel.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2021/11/29
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#import "OTSBaseModel.h"
#import "NSData+Compression.h"

@implementation OTSBaseModel

/// Get Hex: opCode + parameters
- (NSData *)payload {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem = self.opCode;
    NSData *temData = [NSData dataWithBytes:&tem length:1];
    [mData appendData:temData];
    if ([self parameters]) {
        [mData appendData:[self parameters]];
    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [OTSResponseCodeModel class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((OTSResponseCodeModel *)[[self.responseType alloc] init]).opCode;
}

@end


@implementation OACPBaseModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.UUIDString = [NSString stringWithFormat:@"%04X",Bluetooth16BitsUUID_objectActionControlPoint];
    }
    return self;
}

/// The Type of the response message.
- (Class)responseType {
    return [OACPResponseCodeModel class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((OACPBaseModel *)[[self.responseType alloc] init]).opCode;
}

@end


@implementation ObjectModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _objectType = 0x7FFF;//待定：Mesh Proxy=0x1828，PTS测试使用的是0x7FFF。
    }
    return self;
}

/**
 * @brief   Initialize ObjectModel object.
 * @param   dictionary  The dictionary of the share mesh network.
 * @return  return `nil` when initialize ObjectModel object fail.
 */
- (instancetype)initWithMeshDictionary:(NSDictionary *)dictionary {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _shareMeshDictionary = dictionary;
        SigDataSource *ds = [[SigDataSource alloc] init];
        [ds setDictionaryToDataSource:dictionary];
        _shareMeshDataSource = ds;
        _objectData = [LibTools getJSONDataWithDictionary:dictionary].compressionData;
        //==========test==========//
//        NSMutableData *mData = [NSMutableData dataWithData:_objectData];
//        [mData appendData:mData];
//        [mData appendData:mData];
//        [mData appendData:mData];
//        [mData appendData:mData];
//        [mData appendData:mData];
//        [mData appendData:mData];
//        [mData appendData:mData];
//        [mData appendData:mData];
//        [mData appendData:mData];
//        _objectData = mData;
        //==========test==========//
        _objectType = 0x7FFF;//待定：Mesh Proxy=0x1828，PTS测试使用的是0x7FFF。
        _objectName = ds.meshName;
        _objectSize = (UInt32)_objectData.length;
        struct ObjectProperties objectProperties = {0};
        objectProperties.value = 0;
        objectProperties.Delete = 1;
        objectProperties.Read = 1;
        objectProperties.Write = 1;
        _objectProperties = objectProperties;
    }
    return self;
}

@end


@implementation OACPCreateModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = OACPOpCode_Create;
        _size = 0;
        _type = nil;
    }
    return self;
}

/**
 * @brief   Initialize OACPCreateModel object.
 * @param   size  The size of the object.
 * @param   type  The gatt_uuid format type defines UUIDs of more than one size, including a large
 * 128-bit UUID. This service supports all UUIDs that conform to the gatt_uuid format. However, note that
 * it will only be possible to create objects with an Object Type represented by a 128-bit UUID via this
 * control point if the Server is able to negotiate an ATT_MTU size with the Client that is 24 octets or larger
 * or else by using the GATT Write Long Characteristic Values sub-procedure to write the value.
 * @return  return `nil` when initialize OACPCreateModel object fail.
 */
- (instancetype)initWithSize:(UInt32)size type:(NSData *)type {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = OACPOpCode_Create;
        _size = size;
        _type = type;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt32 tem = _size;
    NSData *temData = [NSData dataWithBytes:&tem length:4];
    [mData appendData:temData];
    [mData appendData:_type];
    return mData;
}

@end


@implementation OACPDeleteModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = OACPOpCode_Delete;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

@end


//这个OTS指令在CDTP里面是可选的指令。
@implementation OACPCalculateChecksumModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = OACPOpCode_CalculateChecksum;
        _offset = 0;
        _length = 0;
    }
    return self;
}

/**
 * @brief   Initialize OACPCalculateChecksumModel object.
 * @param   offset  An Offset parameter is required with the Calculate Checksum Op Code. This parameter is used
 * to request a checksum covering the octets beginning from a specified octet position within the object. The value
 * represents an integer number of octets and is based from zero; the first octet of the object contents has an index of zero,
 * the second octet has an index of one, etc. The octet numbered zero is the least significant octet.
 * @param   length  A Length parameter is required with the Calculate Checksum Op Code. The object contents
 * included in the calculation shall consist of Length number of octets, starting with the octet identified by the value of
 * the Offset parameter.
 * @return  return `nil` when initialize OACPCalculateChecksumModel object fail.
 */
- (instancetype)initWithOffset:(UInt32)offset length:(UInt32)length {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = OACPOpCode_CalculateChecksum;
        _offset = offset;
        _length = length;
    }
    return self;
}

/**
 * @brief   Initialize OACPCalculateChecksumModel object.
 * @param   offset  An Offset parameter is required with the Calculate Checksum Op Code. This parameter is used
 * to request a checksum covering the octets beginning from a specified octet position within the object. The value
 * represents an integer number of octets and is based from zero; the first octet of the object contents has an index of zero,
 * the second octet has an index of one, etc. The octet numbered zero is the least significant octet.
 * @param   length  A Length parameter is required with the Calculate Checksum Op Code. The object contents
 * included in the calculation shall consist of Length number of octets, starting with the octet identified by the value of
 * the Offset parameter.
 * @param   crc32  When the Calculate Checksum Op Code is written to the OACP and an error condition does not occur,
 * the Server shall calculate a 32-bit CRC value from the specified octets of the Current Object, using an ISO/IEC 3309
 * compliant 32-bit CRC algorithm [4]. After calculating the checksum, the Server shall respond with the “Success” result code.
 * The newly calculated checksum value shall be provided in the Checksum response parameter.
 * @return  return `nil` when initialize OACPCalculateChecksumModel object fail.
 */
- (instancetype)initWithOffset:(UInt32)offset length:(UInt32)length crc32:(UInt32)crc32 {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = OACPOpCode_CalculateChecksum;
        _offset = offset;
        _length = length;
        _crc32 = crc32;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt32 tem = _offset;
    NSData *temData = [NSData dataWithBytes:&tem length:4];
    [mData appendData:temData];
    tem = _length;
    temData = [NSData dataWithBytes:&tem length:4];
    [mData appendData:temData];
    if (_crc32 != 0) {
        tem = _crc32;
        temData = [NSData dataWithBytes:&tem length:4];
        [mData appendData:temData];
    }
    return mData;
}

@end


//这个OTS指令在CDTP里面是被排除的指令。
@implementation OACPExecuteModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = OACPOpCode_Execute;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

@end


@implementation OACPReadModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = OACPOpCode_Read;
        _offset = 0;
        _length = 0;
    }
    return self;
}

/**
 * @brief   Initialize OACPReadModel object.
 * @param   offset  offset of object need to read.
 * @param   length  length of object need to read.
 * @return  return `nil` when initialize OACPReadModel object fail.
 */
- (instancetype)initWithOffset:(UInt32)offset length:(UInt32)length {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = OACPOpCode_Read;
        _offset = offset;
        _length = length;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt32 tem = _offset;
    NSData *temData = [NSData dataWithBytes:&tem length:4];
    [mData appendData:temData];
    tem = _length;
    temData = [NSData dataWithBytes:&tem length:4];
    [mData appendData:temData];
    return mData;
}

@end


@implementation OACPWriteModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = OACPOpCode_Write;
        _offset = 0;
        _length = 0;
        _mode.Truncate = 0;
    }
    return self;
}

/**
 * @brief   Initialize OACPWriteModel object.
 * @param   offset  offset of object need to write.
 * @param   length  length of object need to write.
 * @param   truncate  CDTP need use false.
 * @return  return `nil` when initialize OACPWriteModel object fail.
 */
- (instancetype)initWithOffset:(UInt32)offset length:(UInt32)length truncate:(bool)truncate {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = OACPOpCode_Write;
        _offset = offset;
        _length = length;
        _mode.Truncate = truncate;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt32 tem = _offset;
    NSData *temData = [NSData dataWithBytes:&tem length:4];
    [mData appendData:temData];
    tem = _length;
    temData = [NSData dataWithBytes:&tem length:4];
    [mData appendData:temData];
    UInt8 tem8 = _mode.value;
    temData = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:temData];
    return mData;
}

@end


@implementation OACPAbortModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = OACPOpCode_Abort;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

@end


@implementation OTSResponseCodeModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.requestOpCode = 0;
        self.resultCode = 0;
        self.responseParameter = nil;
    }
    return self;
}

/**
 * @brief   Initialize OTSResponseCodeModel object.
 * @param   parameters    the hex data of OTSResponseCodeModel.
 * @return  return `nil` when initialize OTSResponseCodeModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        if (parameters == nil || parameters.length >= 3) {
            UInt8 tem8 = 0;
            Byte *dataByte = (Byte *)parameters.bytes;
            memcpy(&tem8, dataByte, 1);
            if (tem8 == OACPOpCode_ResponseCode || tem8 == OLCPOpCode_ResponseCode) {
                memcpy(&tem8, dataByte+1, 1);
                _requestOpCode = tem8;
                memcpy(&tem8, dataByte+2, 1);
                _resultCode = tem8;
                if (parameters.length > 3) {
                    _responseParameter = [NSData dataWithData:[parameters subdataWithRange:NSMakeRange(3, parameters.length-3)]];
                }
                return self;
            } else {
                return nil;
            }
        }else{
            return nil;
        }
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem8 = _requestOpCode;
    NSData *temData = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:temData];
    tem8 = _resultCode;
    temData = [NSData dataWithBytes:&tem8 length:1];
    [mData appendData:temData];
    if (_responseParameter) {
        [mData appendData:_responseParameter];
    }
    return mData;
}

@end


@implementation OACPResponseCodeModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = OACPOpCode_ResponseCode;
    }
    return self;
}

/**
 * @brief   Initialize OACPResponseCodeModel object.
 * @param   parameters    the hex data of OACPResponseCodeModel.
 * @return  return `nil` when initialize OACPResponseCodeModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    if (self = [super initWithParameters:parameters]) {
        self.opCode = OACPOpCode_ResponseCode;
    }
    return self;
}

- (instancetype)initWithRequestOacpOpCode:(OACPOpCode)requestOacpOpCode ResultCode:(OACPResultCode)resultCode responseParameter:(NSData * _Nullable)responseParameter {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = OACPOpCode_ResponseCode;
        self.requestOpCode = requestOacpOpCode;
        self.resultCode = resultCode;
        self.responseParameter = responseParameter;
    }
    return self;
}

@end


@implementation OLCPBaseModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.UUIDString = [NSString stringWithFormat:@"%04X",Bluetooth16BitsUUID_objectListControlPoint];
    }
    return self;
}

- (NSData *)payload {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem = self.opCode;
    NSData *temData = [NSData dataWithBytes:&tem length:1];
    [mData appendData:temData];
    if ([self parameters]) {
        [mData appendData:[self parameters]];
    }
    return mData;
}

/// The Type of the response message.
- (Class)responseType {
    return [OACPResponseCodeModel class];
}

/// The Op Code of the response message.
- (UInt32)responseOpCode {
    return ((OACPBaseModel *)[[self.responseType alloc] init]).opCode;
}

@end


@implementation OLCPFirstModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = OLCPOpCode_First;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

@end


@implementation OLCPLastModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = OLCPOpCode_Last;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

@end


@implementation OLCPPreviousModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = OLCPOpCode_Previous;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

@end


@implementation OLCPNextModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = OLCPOpCode_Next;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

@end


@implementation OLCPGoToModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = OLCPOpCode_GoTo;
    }
    return self;
}

- (instancetype)initWithObjectID:(NSData *)objectID {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = OLCPOpCode_GoTo;
        _objectID = objectID;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    [mData appendData:_objectID];
    return mData;
}

@end


@implementation OLCPOrderModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = OLCPOpCode_Order;
    }
    return self;
}

- (instancetype)initWithOrder:(ListSortOrder)order {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = OLCPOpCode_Order;
        _order = order;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    NSMutableData *mData = [NSMutableData data];
    UInt8 tem = _order;
    NSData *temData = [NSData dataWithBytes:&tem length:1];
    [mData appendData:temData];
    return mData;
}

@end


@implementation OLCPRequestNumberOfObjectsModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = OLCPOpCode_RequestNumberOfObjects;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

@end


@implementation OLCPClearMarkingModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = OLCPOpCode_ClearMarking;
    }
    return self;
}

/// Message parameters as Data.
- (NSData *)parameters {
    return nil;
}

@end


@implementation OLCPResponseCodeModel

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.opCode = OLCPOpCode_ResponseCode;
    }
    return self;
}

/**
 * @brief   Initialize OLCPResponseCodeModel object.
 * @param   parameters    the hex data of OLCPResponseCodeModel.
 * @return  return `nil` when initialize OLCPResponseCodeModel object fail.
 */
- (instancetype)initWithParameters:(NSData *)parameters {
    if (self = [super initWithParameters:parameters]) {
        self.opCode = OACPOpCode_ResponseCode;
    }
    return self;
}

@end


@interface ChannelModel ()
@property (nonatomic, assign) BOOL isOpenStream;
@property (nonatomic, strong) NSData *allData;
@property (nonatomic, strong) NSMutableData *remainingData;
@property (nonatomic, assign) UInt32 readSize;
@property (nonatomic, strong) NSMutableData *readData;
@property (nonatomic, copy) ProgressCallback writeProgressCallback;
@property (nonatomic, copy) ProgressCallback readProgressCallback;
@property (nonatomic, copy) CompleteCallback writeCompleteCallback;
@property (nonatomic, copy) CompleteCallback readCompleteCallback;
@end

@implementation ChannelModel

- (instancetype)initWithChannel:(CBL2CAPChannel *)channel {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        [self openChannelSuccessAction:channel];
    }
    return self;
}

- (void)openChannelSuccessAction:(CBL2CAPChannel * _Nullable)channel {
    _channel = channel;
    [self openStreamAction];
}
#pragma clang diagnostic pop

- (void)closeChannelAction {
    [self closeStreamAction];
    _channel = nil;
}

- (void)openStreamAction {
    if (_channel && !_isOpenStream) {
        _isOpenStream = YES;
        _channel.inputStream.delegate = self;
        [_channel.inputStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        [_channel.inputStream open];
        _channel.outputStream.delegate = self;
        [_channel.outputStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        [_channel.outputStream open];
    }
}

- (void)closeStreamAction {
    if (_channel && _isOpenStream) {
        _isOpenStream = NO;
        [_channel.inputStream close];
        [_channel.inputStream removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        _channel.inputStream.delegate = nil;
        [_channel.outputStream close];
        [_channel.outputStream removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        _channel.inputStream.delegate = nil;
    }
}

- (void)readStreamWithSize:(UInt32)size progress:(ProgressCallback)progress complete:(_Nullable CompleteCallback)block {
    self.readData = [NSMutableData data];
    self.readSize = size;
    self.readProgressCallback = progress;
    self.readCompleteCallback = block;
}

- (void)writeStreamWithData:(NSData *)data progress:(ProgressCallback)progress complete:(_Nullable CompleteCallback)block {
    self.allData = [NSData dataWithData:data];
    self.remainingData = [NSMutableData dataWithData:data];
    self.writeProgressCallback = progress;
    self.writeCompleteCallback = block;
    [self sendStreamData];
}

- (void)sendStreamData {
    if (self.allData && self.allData.length > 0) {
        if (self.remainingData && self.remainingData.length > 0) {
            //Write未完成
            // 当前Object Write逻辑是每包长度100字节，每包之间的间隔是30ms。测试上面第2点的object，iOS端分享iOS端和iOS端分享gateway端都是5m29s，速率是3033字节/秒。
            NSUInteger len = self.remainingData.length > 100 ? 100 : self.remainingData.length;
            NSInteger res = [self.channel.outputStream write:[self.remainingData bytes] maxLength:len];
            NSLog(@"write data length=%ld", (long)res);
            if (res != -1) {
                [self.remainingData replaceBytesInRange:NSMakeRange(0, res) withBytes:nil length:0];
                if (self.writeProgressCallback) {
                    self.writeProgressCallback(1.0 - (self.remainingData.length/(float)self.allData.length));
                }
            } else {
                //write fail
                if (self.writeCompleteCallback) {
                    self.writeCompleteCallback(nil);
                }
            }
        } else {
            //all Write finish
            if (self.writeCompleteCallback) {
                self.writeCompleteCallback(self.allData);
            }
        }
    }
}

#pragma mark - NSStreamDelegate
- (void)stream:(NSStream *)aStream handleEvent:(NSStreamEvent)eventCode {
    TelinkLogInfo(@"%@", aStream);
    uint8_t buf[1024];
    NSData * data;
    NSInteger len;
    NSError *theError;
    switch (eventCode) {
        case NSStreamEventOpenCompleted:
            TelinkLogInfo(@"NSStreamEventOpenCompleted");
            break;
        case NSStreamEventHasSpaceAvailable:
            TelinkLogInfo(@"NSStreamEventHasSpaceAvailable");
//            [self sendStreamData];
            [self performSelector:@selector(sendStreamData) withObject:self afterDelay:0.03];
            break;
        case NSStreamEventHasBytesAvailable:
            len = [(NSInputStream *)aStream read:buf maxLength:1024];
            TelinkLogInfo(@"NSStreamEventHasBytesAvailable (this %@), read %u bytes",aStream, (int) len);
            if(len) {
                data = [NSData dataWithBytes:buf length:len];
                TelinkLogInfo(@"%@", data);
                if (self.readData.length < self.readSize) {
                    [self.readData appendData:data];
                    TelinkLogInfo(@"pro=%f", self.readData.length/(float)self.readSize);
                    if (self.readProgressCallback) {
                        self.readProgressCallback(self.readData.length/(float)self.readSize);
                    }
                    if (self.readData.length == self.readSize) {
                        if (self.readCompleteCallback) {
                            self.readCompleteCallback(self.readData);
                        }
                    }
                }
            }
            break;
        case NSStreamEventErrorOccurred:
            theError = [aStream streamError];
            TelinkLogInfo(@"NSStreamEventErrorOccurred: %@", theError);
            break;
        case NSStreamEventEndEncountered:
            TelinkLogInfo(@"NSStreamEventEndEncountered");
            //service端关闭蓝牙，Client端回调这个枚举值
            [self closeStreamAction];
            if (self.readCompleteCallback) {
                self.readCompleteCallback(self.readData);
            }
            break;
        case NSStreamEventNone:
            TelinkLogInfo(@"NSStreamEventNone");
            break;
        default:
            TelinkLogInfo(@"Other %02x", (int) eventCode);
            break;
    }
}

@end


@implementation ServiceModel

- (instancetype)initWithPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary<NSString *,id> *)advertisementData RSSI:(NSNumber *)RSSI {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        _peripheral = peripheral;
        _advertisementData = advertisementData;
        _RSSI = RSSI;
    }
    return self;
}

- (NSString *)showName {
    NSString *tem = nil;
    if (_advertisementData && [_advertisementData.allKeys containsObject:CBAdvertisementDataLocalNameKey]) {
        tem = _advertisementData[CBAdvertisementDataLocalNameKey];
    } else if (_peripheral && _peripheral.name) {
        tem = _peripheral.name;
    }
    return tem;
}

/// Determine if the data of two ServiceModel is the same
- (BOOL)isEqual:(id)object {
    if ([object isKindOfClass:[ServiceModel class]]) {
        ServiceModel *tem = (ServiceModel *)object;
        //peripheral is the unique identifier of ServiceModel.
        return _peripheral == tem.peripheral;
    } else {
        //Two ServiceModel object is different.
        return NO;
    }
}

@end
