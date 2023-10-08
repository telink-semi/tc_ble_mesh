/********************************************************************************************************
 * @file     OTSCommand.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2021/11/29
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

#import "OTSCommand.h"

@implementation OTSCommand


#pragma mark - OACP


#pragma mark 3.3.2.1 Create Procedure


/**
 * @brief   OACP Create Procedure.
 * @param   size  The size of the object.
 * @param   type  The gatt_uuid format type defines UUIDs of more than one size, including a large
 * 128-bit UUID. This service supports all UUIDs that conform to the gatt_uuid format. However, note that
 * it will only be possible to create objects with an Object Type represented by a 128-bit UUID via this
 * control point if the Server is able to negotiate an ATT_MTU size with the Client that is 24 octets or larger
 * or else by using the GATT Write Long Characteristic Values sub-procedure to write the value.
 * @param   timeout    the timeout of this command.
 * @param   responseCallback    callback when node response the status message.
 * @return  A OTSCommand object of this command.
 * @note    - seeAlso: OTS_v10.pdf (page.26),
 * 3.3.2.1 Create Procedure.
 */
- (instancetype)initOACPCreateCommandWithSize:(UInt32)size type:(NSData *)type timeout:(NSTimeInterval)timeout responseCallback:(ResponseOfOACPMessageBlock)responseCallback {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        OACPCreateModel *model = [[OACPCreateModel alloc] initWithSize:size type:type];
        _curOTSMessage = model;
        _timeout = timeout;
        _responseCallback = (ResponseAllOTSMessageBlock)responseCallback;
    }
    return self;
}


#pragma mark 3.3.2.2 Delete Procedure


/**
 * @brief   OACP Delete Procedure.
 * @param   timeout    the timeout of this command.
 * @param   responseCallback    callback when node response the status message.
 * @return  A OTSCommand object of this command.
 * @note    - seeAlso: OTS_v10.pdf (page.27),
 * 3.3.2.2 Delete Procedure.
 */
- (instancetype)initOACPDeleteCommandWithTimeout:(NSTimeInterval)timeout responseCallback:(ResponseOfOACPMessageBlock)responseCallback {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        OACPDeleteModel *model = [[OACPDeleteModel alloc] init];
        _curOTSMessage = model;
        _timeout = timeout;
        _responseCallback = (ResponseAllOTSMessageBlock)responseCallback;
    }
    return self;
}


#pragma mark 3.3.2.3 Calculate Checksum Procedure


/**
 * @brief   OACP Calculate Checksum Procedure.
 * @param   offset  An Offset parameter is required with the Calculate Checksum Op Code. This parameter is used
 * to request a checksum covering the octets beginning from a specified octet position within the object. The value
 * represents an integer number of octets and is based from zero; the first octet of the object contents has an index of zero,
 * the second octet has an index of one, etc. The octet numbered zero is the least significant octet.
 * @param   length  A Length parameter is required with the Calculate Checksum Op Code. The object contents
 * included in the calculation shall consist of Length number of octets, starting with the octet identified by the value of
 * the Offset parameter.
 * @param   timeout    the timeout of this command.
 * @param   responseCallback    callback when node response the status message.
 * @return  A OTSCommand object of this command.
 * @note    - seeAlso: OTS_v10.pdf (page.28),
 * 3.3.2.3 Calculate Checksum Procedure.
 */
- (instancetype)initOACPCalculateChecksumCommandWithOffset:(UInt32)offset length:(UInt32)length timeout:(NSTimeInterval)timeout responseCallback:(ResponseOfOACPMessageBlock)responseCallback {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        OACPCalculateChecksumModel *model = [[OACPCalculateChecksumModel alloc] initWithOffset:offset length:length];
        _curOTSMessage = model;
        _timeout = timeout;
        _responseCallback = (ResponseAllOTSMessageBlock)responseCallback;
    }
    return self;
}

/**
 * @brief   Private OACP Calculate Checksum Procedure.
 * @param   offset  An Offset parameter is required with the Calculate Checksum Op Code. This parameter is used
 * to request a checksum covering the octets beginning from a specified octet position within the object. The value
 * represents an integer number of octets and is based from zero; the first octet of the object contents has an index of zero,
 * the second octet has an index of one, etc. The octet numbered zero is the least significant octet.
 * @param   length  A Length parameter is required with the Calculate Checksum Op Code. The object contents
 * included in the calculation shall consist of Length number of octets, starting with the octet identified by the value of
 * the Offset parameter.
 * @param   crc32    The crc32 value of CDTP Client.
 * @param   timeout    the timeout of this command.
 * @param   responseCallback    callback when node response the status message.
 * @return  A OTSCommand object of this command.
 * @note    - seeAlso: OTS_v10.pdf (page.28),
 * 3.3.2.3 Calculate Checksum Procedure.
 */
- (instancetype)initPrivateOACPCalculateChecksumCommandWithOffset:(UInt32)offset length:(UInt32)length crc32:(UInt32)crc32 timeout:(NSTimeInterval)timeout responseCallback:(ResponseOfOACPMessageBlock)responseCallback {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        OACPCalculateChecksumModel *model = [[OACPCalculateChecksumModel alloc] initWithOffset:offset length:length crc32:crc32];
        _curOTSMessage = model;
        _timeout = timeout;
        _responseCallback = (ResponseAllOTSMessageBlock)responseCallback;
    }
    return self;
}


#pragma mark 3.3.2.4 Execute Procedure


/**
 * @brief   OACP Execute Procedure.
 * @param   timeout    the timeout of this command.
 * @param   responseCallback    callback when node response the status message.
 * @return  A OTSCommand object of this command.
 * @note    - seeAlso: OTS_v10.pdf (page.29),
 * 3.3.2.4 Execute Procedure.
 */
- (instancetype)initOACPExecuteCommandWithTimeout:(NSTimeInterval)timeout responseCallback:(ResponseOfOACPMessageBlock)responseCallback {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        OACPExecuteModel *model = [[OACPExecuteModel alloc] init];
        _curOTSMessage = model;
        _timeout = timeout;
        _responseCallback = (ResponseAllOTSMessageBlock)responseCallback;
    }
    return self;
}


#pragma mark 3.3.2.5 Read Procedure


/**
 * @brief   OACP Read Procedure.
 * @param   offset  offset of object need to read.
 * @param   length  length of object need to read.
 * @param   timeout    the timeout of this command.
 * @param   responseCallback    callback when node response the status message.
 * @return  A OTSCommand object of this command.
 * @note    - seeAlso: OTS_v10.pdf (page.30),
 * 3.3.2.5 Read Procedure.
 */
- (instancetype)initOACPReadCommandWithOffset:(UInt32)offset length:(UInt32)length timeout:(NSTimeInterval)timeout responseCallback:(ResponseOfOACPMessageBlock)responseCallback {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        OACPReadModel *model = [[OACPReadModel alloc] initWithOffset:offset length:length];
        _curOTSMessage = model;
        _timeout = timeout;
        _responseCallback = (ResponseAllOTSMessageBlock)responseCallback;
    }
    return self;
}


#pragma mark 3.3.2.6 Write Procedure


/**
 * @brief   OACP Write Procedure.
 * @param   offset  offset of object need to write.
 * @param   length  length of object need to write.
 * @param   truncate  CDTP need use false.
 * @param   timeout    the timeout of this command.
 * @param   responseCallback    callback when node response the status message.
 * @return  A OTSCommand object of this command.
 * @note    - seeAlso: OTS_v10.pdf (page.31),
 * 3.3.2.6 Write Procedure.
 */
- (instancetype)initOACPWriteCommandWithOffset:(UInt32)offset length:(UInt32)length truncate:(bool)truncate timeout:(NSTimeInterval)timeout responseCallback:(ResponseOfOACPMessageBlock)responseCallback {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        OACPWriteModel *model = [[OACPWriteModel alloc] initWithOffset:offset length:length truncate:truncate];
        _curOTSMessage = model;
        _timeout = timeout;
        _responseCallback = (ResponseAllOTSMessageBlock)responseCallback;
    }
    return self;
}


#pragma mark 3.3.2.7 Abort Procedure


/**
 * @brief   OACP Abort Procedure.
 * @param   timeout    the timeout of this command.
 * @param   responseCallback    callback when node response the status message.
 * @return  A OTSCommand object of this command.
 * @note    - seeAlso: OTS_v10.pdf (page.34),
 * 3.3.2.7 Abort Procedure.
 */
- (instancetype)initOACPAbortCommandWithTimeout:(NSTimeInterval)timeout responseCallback:(ResponseOfOACPMessageBlock)responseCallback {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        OACPAbortModel *model = [[OACPAbortModel alloc] init];
        _curOTSMessage = model;
        _timeout = timeout;
        _responseCallback = (ResponseAllOTSMessageBlock)responseCallback;
    }
    return self;
}


#pragma mark - OLCP
- (instancetype)initOLCPFirstCommandWithTimeout:(NSTimeInterval)timeout responseCallback:(ResponseOfOLCPMessageBlock)responseCallback {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        OLCPFirstModel *model = [[OLCPFirstModel alloc] init];
        _curOTSMessage = model;
        _timeout = timeout;
        _responseCallback = (ResponseAllOTSMessageBlock)responseCallback;
    }
    return self;
}

- (instancetype)initOLCPLastCommandWithTimeout:(NSTimeInterval)timeout responseCallback:(ResponseOfOLCPMessageBlock)responseCallback {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        OLCPLastModel *model = [[OLCPLastModel alloc] init];
        _curOTSMessage = model;
        _timeout = timeout;
        _responseCallback = (ResponseAllOTSMessageBlock)responseCallback;
    }
    return self;
}

- (instancetype)initOLCPPreviousCommandWithTimeout:(NSTimeInterval)timeout responseCallback:(ResponseOfOLCPMessageBlock)responseCallback {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        OLCPPreviousModel *model = [[OLCPPreviousModel alloc] init];
        _curOTSMessage = model;
        _timeout = timeout;
        _responseCallback = (ResponseAllOTSMessageBlock)responseCallback;
    }
    return self;
}

- (instancetype)initOLCPNextCommandWithTimeout:(NSTimeInterval)timeout responseCallback:(ResponseOfOLCPMessageBlock)responseCallback {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        OLCPNextModel *model = [[OLCPNextModel alloc] init];
        _curOTSMessage = model;
        _timeout = timeout;
        _responseCallback = (ResponseAllOTSMessageBlock)responseCallback;
    }
    return self;
}

- (instancetype)initOLCPGoToCommandWithObjectID:(NSData *)objectID timeout:(NSTimeInterval)timeout responseCallback:(ResponseOfOLCPMessageBlock)responseCallback {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        OLCPGoToModel *model = [[OLCPGoToModel alloc] initWithObjectID:objectID];
        _curOTSMessage = model;
        _timeout = timeout;
        _responseCallback = (ResponseAllOTSMessageBlock)responseCallback;
    }
    return self;
}

- (instancetype)initOLCPOrderCommandWithOrder:(ListSortOrder)order timeout:(NSTimeInterval)timeout responseCallback:(ResponseOfOLCPMessageBlock)responseCallback {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        OLCPOrderModel *model = [[OLCPOrderModel alloc] initWithOrder:order];
        _curOTSMessage = model;
        _timeout = timeout;
        _responseCallback = (ResponseAllOTSMessageBlock)responseCallback;
    }
    return self;
}

- (instancetype)initOLCPRequestNumberOfObjectsCommandWithTimeout:(NSTimeInterval)timeout responseCallback:(ResponseOfOLCPMessageBlock)responseCallback {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        OLCPRequestNumberOfObjectsModel *model = [[OLCPRequestNumberOfObjectsModel alloc] init];
        _curOTSMessage = model;
        _timeout = timeout;
        _responseCallback = (ResponseAllOTSMessageBlock)responseCallback;
    }
    return self;
}

- (instancetype)initOLCPClearMarkingCommandWithTimeout:(NSTimeInterval)timeout responseCallback:(ResponseOfOLCPMessageBlock)responseCallback {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        OLCPClearMarkingModel *model = [[OLCPClearMarkingModel alloc] init];
        _curOTSMessage = model;
        _timeout = timeout;
        _responseCallback = (ResponseAllOTSMessageBlock)responseCallback;
    }
    return self;
}

@end
