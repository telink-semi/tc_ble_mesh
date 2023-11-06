/********************************************************************************************************
 * @file     SDKLibCommand+firmwareUpdate.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2022/3/9
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

#import "SDKLibCommand+firmwareUpdate.h"

@implementation SDKLibCommand (firmwareUpdate)


#pragma mark - 8.4.1 Firmware Update model messages
/*
 This section defines the format of messages exchanged
 between the Firmware Update Client and Firmware Update
 Server models.
 */


#pragma mark 8.4.1.1 Firmware Update Information Get, opcode:0xB601


/// The Firmware Update Information Get message is an acknowledged
/// message used to get information about the firmware images installed
/// on a node.
/// - note:   The response to the Firmware Update Information Get
/// message is a Firmware Update Information Status message.
/// - note:  - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.91),
/// 8.4.1.1 Firmware Update Information Get.
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - firstIndex: Index of the first requested entry from the Firmware Information List state.
/// The First Index field shall indicate the first entry on the Firmware Information
/// List state of the Firmware Update Server (see Section 8.3.1.1) to return in
/// the Firmware Update Information Status message.
///   - entriesLimit: Maximum number of entries that the server includes in a Firmware Update
/// Information Status message.
/// The Entries Limit field shall indicate the maximum number of Firmware
/// Information Entry fields to return in the Firmware Update Information
/// Status message.
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)firmwareUpdateInformationGetWithDestination:(UInt16)destination firstIndex:(UInt8)firstIndex entriesLimit:(UInt8)entriesLimit retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareInformationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareUpdateInformationGet *message = [[SigFirmwareUpdateInformationGet alloc] initWithFirstIndex:firstIndex entriesLimit:entriesLimit];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseFirmwareInformationStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 8.4.1.3 Firmware Update Firmware Metadata Check, opcode:0xB603


/// The Firmware Update Firmware Metadata Check message is an acknowledged
/// message, sent to a Firmware Update Server, to check whether the node can
/// accept a firmware update.
/// - note:   The response to the Firmware Update Firmware Metadata Check
/// message is a Firmware Update Firmware Metadata Status message.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.93),
/// 8.4.1.3 Firmware Update Firmware Metadata Check.
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - updateFirmwareImageIndex: Index of the firmware image in the Firmware Information List state to check.
/// The Update Firmware Image Index field shall identify the firmware image in the
/// Firmware Information List state on the Firmware Update Server that the
/// metadata is checked against.
///   - incomingFirmwareMetadata: Vendor-specific metadata (Optional). Size is 1 to 255.
/// If present, the Incoming Firmware Metadata field shall contain the custom data
/// from the firmware vendor. The firmware metadata that can be used to check
/// whether the installed firmware image identified by the Firmware Image Index
/// field will accept an update based on the metadata provided for the new
/// firmware image.
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)firmwareUpdateFirmwareMetadataCheckWithDestination:(UInt16)destination updateFirmwareImageIndex:(UInt8)updateFirmwareImageIndex incomingFirmwareMetadata:(nullable NSData *)incomingFirmwareMetadata retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareUpdateFirmwareMetadataStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareUpdateFirmwareMetadataCheck *message = [[SigFirmwareUpdateFirmwareMetadataCheck alloc] initWithUpdateFirmwareImageIndex:updateFirmwareImageIndex incomingFirmwareMetadata:incomingFirmwareMetadata];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseFirmwareUpdateFirmwareMetadataStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 8.4.1.5 Firmware Update Get, opcode:0xB605


/// The Firmware Update Get message is an acknowledged message
/// used to get the current status of the Firmware Update Server.
/// - note:   The response to the Firmware Update Get message is a
/// Firmware Update Status message.
/// - note:   There are no parameters for this message.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.94),
/// 8.4.1.5 Firmware Update Get.
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)firmwareUpdateGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareUpdateStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareUpdateGet *message = [[SigFirmwareUpdateGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseFirmwareUpdateStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 8.4.1.6 Firmware Update Start, opcode:0xB606


/// The Firmware Update Start message is an acknowledged message used to
/// start a firmware update on a Firmware Update Server.
/// - note:   The response to the Firmware Update Start message is a Firmware
/// Update Status message.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.94),
/// 8.4.1.6 Firmware Update Start.
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - updateTTL: Time To Live value to use during firmware image transfer.
/// The Update TTL field shall indicate the time to live (TTL) value that is used during
/// firmware image transfer. The values for the Update TTL field are defined in Table 8.5.
///   - updateTimeoutBase: Used to compute the timeout of the firmware image transfer.
/// The Update Timeout Base field shall contain the value that the Firmware Update
/// Server uses to calculate when firmware image transfer will be suspended.
///   - updateBLOBID: BLOB identifier for the firmware image.
/// The Update BLOB ID field shall contain the BLOB identifier to use during firmware
/// image transfer.
///   - updateFirmwareImageIndex: Index of the firmware image in the Firmware Information List state to be updated.
/// The Firmware Image Index field shall identify the firmware image in the Firmware
/// Information List state to be updated.
///   - incomingFirmwareMetadata: Vendor-specific firmware metadata (Optional). Size is 1 to 255.
/// If present, the Incoming Firmware Metadata field shall contain the custom data
/// from the firmware vendor that is used to check whether the firmware image can
/// be updated.
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)firmwareUpdateStartWithDestination:(UInt16)destination updateTTL:(UInt8)updateTTL updateTimeoutBase:(UInt16)updateTimeoutBase updateBLOBID:(UInt64)updateBLOBID updateFirmwareImageIndex:(UInt8)updateFirmwareImageIndex incomingFirmwareMetadata:(nullable NSData *)incomingFirmwareMetadata retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareUpdateStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareUpdateStart *message = [[SigFirmwareUpdateStart alloc] initWithUpdateTTL:updateTTL updateTimeoutBase:updateTimeoutBase updateBLOBID:updateBLOBID updateFirmwareImageIndex:updateFirmwareImageIndex incomingFirmwareMetadata:incomingFirmwareMetadata];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseFirmwareUpdateStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 8.4.1.7 Firmware Update Cancel, opcode:0xB607


/// The Firmware Update Cancel message is an acknowledged message
/// used to cancel a firmware update and delete any stored information
/// about the update on a Firmware Update Server.
/// - note:   The response to a Firmware Update Cancel message is a
/// Firmware Update Status message.
/// - note:   The Firmware Update Cancel message has no parameters.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.95),
/// 8.4.1.7 Firmware Update Cancel.
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)firmwareUpdateCancelWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareUpdateStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareUpdateCancel *message = [[SigFirmwareUpdateCancel alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseFirmwareUpdateStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 8.4.1.8 Firmware Update Apply, opcode:0xB608


/// The Firmware Update Apply message is an acknowledged message
/// used to apply a firmware image that has been transferred to a
/// Firmware Update Server.
/// - note:   The response to a Firmware Update Apply message is a
/// Firmware Update Status message.
/// - note:   The Firmware Update Apply message has no parameters.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.95),
/// 8.4.1.8 Firmware Update Apply.
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)firmwareUpdateApplyWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareUpdateStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareUpdateApply *message = [[SigFirmwareUpdateApply alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseFirmwareUpdateStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 8.4.2.1 Firmware Distribution Receivers Add, opcode:0xB611


/// The Firmware Distribution Receivers Add message is an acknowledged message sent
/// by a Firmware Distribution Client to add new entries to the Distribution Receivers List
/// state of a Firmware Distribution Server.
/// - note:   The response to a Firmware Distribution Receivers Add message is a
/// Firmware Distribution Receivers Status message.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.96),
/// 8.4.2.1 Firmware Distribution Receivers Add.
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - receiverEntrysList: The Firmware Distribution Receivers Add message shall contain at least one Receiver Entry.
/// For each Receiver Entry field in the message, the value of the Address field shall be unique.
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)firmwareDistributionReceiversAddWithDestination:(UInt16)destination receiverEntrysList:(NSArray <SigReceiverEntryModel *>*)receiverEntrysList retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareDistributionReceiversStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareDistributionReceiversAdd *message = [[SigFirmwareDistributionReceiversAdd alloc] initWithReceiverEntrysList:receiverEntrysList];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseFirmwareDistributionReceiversStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 8.4.2.2 Firmware Distribution Receivers Delete All, opcode:0xB612


/// The Firmware Distribution Receivers Delete All message is an acknowledged
/// message sent by a Firmware Distribution Client to remove all entries from the
/// Distribution Receivers List state of a Firmware Distribution Server.
/// - note:   The response to a Firmware Distribution Receivers Delete All message
/// is a Firmware Distribution Receivers Status message.
/// - note:   There are no parameters for this message.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.97),
/// 8.4.2.2 Firmware Distribution Receivers Delete All.
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)firmwareDistributionReceiversDeleteAllWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareDistributionReceiversStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareDistributionReceiversDeleteAll *message = [[SigFirmwareDistributionReceiversDeleteAll alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseFirmwareDistributionReceiversStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 8.4.2.4 Firmware Distribution Receivers Get, opcode:0xB60F


/// The Firmware Distribution Receivers Get message is an acknowledged message
/// sent by the Firmware Distribution Client to get the firmware distribution status of
/// each Updating node.
/// - note:   The response to a Firmware Distribution Receivers Get message is a
/// Firmware Distribution Receivers List message.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.97),
/// 8.4.2.4 Firmware Distribution Receivers Get.
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - firstIndex: Index of the first requested entry from the Distribution Receivers List state.
/// The First Index field shall indicate the first entry of the Distribution Receivers List
/// state of the Firmware Distribution Server to return in the Firmware Distribution
/// Receivers List message.
///   - entriesLimit: Maximum number of entries that the server includes in a Firmware Distribution
/// Receivers List message.
/// The Entries Limit field shall set the maximum number of Updating Node Entries
/// to return in the Firmware Distribution Receivers List message. The value of the
/// Entries Limit field shall be greater than 0.
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)firmwareDistributionReceiversGetWithDestination:(UInt16)destination firstIndex:(UInt16)firstIndex entriesLimit:(UInt16)entriesLimit retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareDistributionReceiversListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareDistributionReceiversGet *message = [[SigFirmwareDistributionReceiversGet alloc] initWithFirstIndex:firstIndex entriesLimit:entriesLimit];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseFirmwareDistributionReceiversListCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 8.4.2.6 Firmware Distribution Capabilities Get, opcode:0xB614


/// The Firmware Distribution Capabilities Get message is an acknowledged message
/// sent by a Firmware Distribution Client to get the capabilities of a Firmware
/// Distribution Server.
/// - note:   The response to a Firmware Distribution Capabilities Get message is a
/// Firmware Distribution Capabilities Status message.
/// - note:   There are no parameters for this message.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.99),
/// 8.4.2.6 Firmware Distribution Capabilities Get.
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)firmwareDistributionCapabilitiesGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareDistributionCapabilitiesStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareDistributionCapabilitiesGet *message = [[SigFirmwareDistributionCapabilitiesGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseFirmwareDistributionCapabilitiesStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark - 8.4.2 Firmware Distribution model messages


#pragma mark 8.4.2.8 Firmware Distribution Get, opcode:0xB60A


/// The Firmware Distribution Get message is an acknowledged message
/// sent by a Firmware Distribution Client to get the state of the current
/// firmware image distribution on a Firmware Distribution Server.
/// - note:   The response to a Firmware Distribution Get message is a
/// Firmware Distribution Status message.
/// - note:   There are no parameters for this message.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.100),
/// 8.4.2.8 Firmware Distribution Get.
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)firmwareDistributionGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareDistributionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareDistributionGet *message = [[SigFirmwareDistributionGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseFirmwareDistributionStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 8.4.2.9 Firmware Distribution Start, opcode:0xB60B


/// The Firmware Distribution Start message is an acknowledged message
/// sent by a Firmware Distribution Client to start the firmware image
/// distribution to the Updating nodes in the Distribution Receivers List.
/// - note:   The response to a Firmware Distribution Start message is a
/// Firmware Distribution Status message.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.100),
/// 8.4.2.9 Firmware Distribution Start.
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - distributionAppKeyIndex: Index of the application key used in a firmware image distribution.
/// The Distribution AppKey Index field shall indicate the application key
/// used for the firmware image distribution.
///   - distributionTTL: Time To Live value used in a firmware image distribution.
/// The Distribution TTL field shall indicate the TTL value used for a firmware
/// image distribution. The values for the Distribution TTL field are defined
/// in Table 8.5.
///   - distributionTimeoutBase: Used to compute the timeout of the firmware image distribution.
/// The Distribution Timeout Base field shall contain the value that is used to
/// calculate when firmware image distribution will be suspended.
///   - distributionTransferMode: Mode of the transfer, szie is 2 bits.
/// The Distribution Transfer Mode field shall indicate the mode used to transfer
/// the BLOB to the Updating node (see Section 7.2.1.4).
///   - updatePolicy: Firmware update policy, szie is 1 bits.
/// The Update Policy field shall indicate the update policy that the Firmware
/// Distribution Server will use for this firmware image distribution.
///   - RFU: Reserved for Future Use. Size is 5 bits.
///   - distributionFirmwareImageIndex: Index of the firmware image in the Firmware Images List state to use during
/// firmware image distribution.
/// The Distribution Firmware Image Index field shall identify the index of the
/// firmware image from the Firmware Images List state to transfer during
/// firmware image distribution.
///   - distributionMulticastAddress: Multicast address used in a firmware image distribution. Size is 16 bits or 128 bits.
/// The Distribution Multicast Address field shall identify the multicast address of the
/// Updating nodes that the firmware image is intended for. The value of the Distribution
/// Multicast Address field shall be a group address, the Label UUID of a virtual address,
/// or the Unassigned address.
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)firmwareDistributionStartWithDestination:(UInt16)destination distributionAppKeyIndex:(UInt16)distributionAppKeyIndex distributionTTL:(UInt8)distributionTTL distributionTimeoutBase:(UInt16)distributionTimeoutBase distributionTransferMode:(SigTransferModeState)distributionTransferMode updatePolicy:(SigUpdatePolicyType)updatePolicy RFU:(UInt8)RFU distributionFirmwareImageIndex:(UInt16)distributionFirmwareImageIndex distributionMulticastAddress:(NSData *)distributionMulticastAddress retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareDistributionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareDistributionStart *message = [[SigFirmwareDistributionStart alloc] initWithDistributionAppKeyIndex:distributionAppKeyIndex distributionTTL:distributionTTL distributionTimeoutBase:distributionTimeoutBase distributionTransferMode:distributionTransferMode updatePolicy:updatePolicy RFU:RFU distributionFirmwareImageIndex:distributionFirmwareImageIndex distributionMulticastAddress:distributionMulticastAddress];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseFirmwareDistributionStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 8.4.2.10 Firmware Distribution Cancel, opcode:0xB60C


/// The Firmware Distribution Cancel message is an acknowledged message
/// sent by a Firmware Distribution Client to stop the firmware image distribution
/// from a Firmware Distribution Server.
/// - note:   The response to a Firmware Distribution Cancel message is a
/// Firmware Distribution Status message.
/// - note:   There are no parameters for this message.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.101),
/// 8.4.2.10 Firmware Distribution Cancel.
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)firmwareDistributionCancelWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareDistributionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareDistributionCancel *message = [[SigFirmwareDistributionCancel alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseFirmwareDistributionStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 8.4.2.11 Firmware Distribution Apply, opcode:0xB60D


/// The Firmware Distribution Apply message is an acknowledged message
/// sent from a Firmware Distribution Client to a Firmware Distribution Server
/// to apply the firmware image on the Updating nodes.
/// - note:   The response to a Firmware Distribution Apply message is a
/// Firmware Distribution Status message.
/// - note:   There are no parameters for this message.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.101),
/// 8.4.2.11 Firmware Distribution Apply.
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)firmwareDistributionApplyWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareDistributionStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareDistributionApply *message = [[SigFirmwareDistributionApply alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseFirmwareDistributionStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 8.4.2.13 Firmware Distribution Upload Get, opcode:0xB616


/// The Firmware Distribution Upload Get message is an acknowledged
/// message sent by a Firmware Distribution Client to check the status of
/// a firmware image upload to a Firmware Distribution Server.
/// - note:   The response to a Firmware Distribution Upload Get message
/// is a Firmware Distribution Upload Status message.
/// - note:   There are no parameters for this message.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.103),
/// 8.4.2.13 Firmware Distribution Upload Get.
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)firmwareDistributionUploadGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareDistributionUploadStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareDistributionUploadGet *message = [[SigFirmwareDistributionUploadGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseFirmwareDistributionUploadStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 8.4.2.14 Firmware Distribution Upload Start, opcode:0xB617


/// The Firmware Distribution Upload Start message is an acknowledged message sent
/// by a Firmware Distribution Client to start a firmware image upload to a Firmware
/// Distribution Server.
/// - note:   The response to a Firmware Distribution Upload Start message is a
/// Firmware Distribution Upload Status message.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.103),
/// 8.4.2.14 Firmware Distribution Upload Start.
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - uploadTTL: Time To Live value used in a firmware image upload.
/// The Upload TTL field shall indicate the TTL value used for a firmware image upload.
/// The values for the Upload TTL field are defined in Table 8.5.
///   - uploadTimeoutBase: Used to compute the timeout of the firmware image upload.
/// The Upload Timeout Base field shall contain the value that is used to calculate when
/// firmware image upload will be suspended.
///   - uploadBLOBID: BLOB identifier for the firmware image.
/// The Upload BLOB ID field shall field shall contain the BLOB identifier to use during
/// firmware image upload.
///   - uploadFirmwareSize: Firmware image size (in octets).
/// The Upload Firmware Size field shall indicate the size of the firmware image being
/// uploaded.
///   - uploadFirmwareMetadataLength: Size of the Upload Firmware Metadata field.
/// The Upload Firmware Metadata Length field shall indicate the length of the Upload
/// Firmware Metadata field.
///   - uploadFirmwareMetadata: Vendor-specific firmware metadata (C.1). Size is 1 to 255. (C.1: If the value of the
/// Upload Firmware Metadata Length field is greater than 0, the Upload Firmware
/// Metadata field shall be present; otherwise, the Upload Firmware Metadata field
/// shall be omitted.)
/// If present, the Upload Firmware Metadata field shall contain the custom data from
/// the firmware vendor that will be used to check whether the firmware image can be
/// updated.
///   - uploadFirmwareID: The Firmware ID identifying the firmware image being uploaded. Size is Variable.
/// The Upload Firmware ID field shall identify the firmware image being uploaded.
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)firmwareDistributionUploadStartWithDestination:(UInt16)destination uploadTTL:(UInt8)uploadTTL uploadTimeoutBase:(UInt16)uploadTimeoutBase uploadBLOBID:(UInt64)uploadBLOBID uploadFirmwareSize:(UInt32)uploadFirmwareSize uploadFirmwareMetadataLength:(UInt8)uploadFirmwareMetadataLength uploadFirmwareMetadata:(NSData *)uploadFirmwareMetadata uploadFirmwareID:(NSData *)uploadFirmwareID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareDistributionUploadStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareDistributionUploadStart *message = [[SigFirmwareDistributionUploadStart alloc] initWithUploadTTL:uploadTTL uploadTimeoutBase:uploadTimeoutBase uploadBLOBID:uploadBLOBID uploadFirmwareSize:uploadFirmwareSize uploadFirmwareMetadataLength:uploadFirmwareMetadataLength uploadFirmwareMetadata:uploadFirmwareMetadata uploadFirmwareID:uploadFirmwareID];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseFirmwareDistributionUploadStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 8.4.2.15 Firmware Distribution Upload OOB Start, opcode:0xB618


/// The Firmware Distribution Upload OOB Start message is an acknowledged message
/// sent by a Firmware Distribution Client to start a firmware image upload to a Firmware
/// Distribution Server using an out-of-band mechanism.
/// - note:   The response to a Firmware Distribution Upload OOB Start message is a
/// Firmware Distribution Upload Status message.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.104),
/// 8.4.2.15 Firmware Distribution Upload OOB Start.
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - uploadURILength: Length of the Upload URI field.
/// The Upload URI Length field shall indicate the length of the Upload URI field.
/// The value 0 is prohibited.
///   - uploadURI: URI for the firmware image source. Size is 1 to 255.
/// The Upload URI field shall contain the URI used to retrieve the firmware image.
///   - uploadFirmwareID: The Firmware ID value used to generate the URI query string. Size is Variable.
/// The Upload Firmware ID field shall contain the value used to generate the URI query
/// string (see Section 8.2.2.1 and Section 8.2.3.1).
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)firmwareDistributionUploadOOBStartWithDestination:(UInt16)destination uploadURILength:(UInt8)uploadURILength uploadURI:(NSData *)uploadURI uploadFirmwareID:(NSData *)uploadFirmwareID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareDistributionUploadStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareDistributionUploadOOBStart *message = [[SigFirmwareDistributionUploadOOBStart alloc] initWithUploadURILength:uploadURILength uploadURI:uploadURI uploadFirmwareID:uploadFirmwareID];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseFirmwareDistributionUploadStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 8.4.2.16 Firmware Distribution Upload Cancel, opcode:0xB619


/// The Firmware Distribution Upload Cancel message is an acknowledged
/// message sent by a Firmware Distribution Client to stop a firmware image
/// upload to a Firmware Distribution Server.
/// - note:   The response to a Firmware Distribution Upload Cancel
/// message is a Firmware Distribution Upload Status message.
/// - note:   There are no parameters for this message.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.104),
/// 8.4.2.16 Firmware Distribution Upload Cancel.
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)firmwareDistributionUploadCancelWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareDistributionUploadStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareDistributionUploadCancel *message = [[SigFirmwareDistributionUploadCancel alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseFirmwareDistributionUploadStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 8.4.2.18 Firmware Distribution Firmware Get, opcode:0xB61B


/// The Firmware Distribution Firmware Get message is an acknowledged message
/// sent by a Firmware Distribution Client to check whether a specific firmware
/// image is stored on a Firmware Distribution Server.
/// - note:   The response to a Firmware Distribution Firmware Get message is a
/// Firmware Distribution Firmware Status message.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.105),
/// 8.4.2.18 Firmware Distribution Firmware Get.
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - firmwareID: The Firmware ID identifying the firmware image to check. Size is Variable.
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)firmwareDistributionFirmwareGetWithDestination:(UInt16)destination firmwareID:(NSData *)firmwareID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareDistributionFirmwareStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareDistributionFirmwareGet *message = [[SigFirmwareDistributionFirmwareGet alloc] initWithFirmwareID:firmwareID];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseFirmwareDistributionFirmwareStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 8.4.2.19 Firmware Distribution Firmware Get By Index, opcode:0xB61D


/// The Firmware Distribution Firmware Get By Index message is an acknowledged message
/// sent by a Firmware Distribution Client to check which firmware image is stored in a particular
/// entry in the Firmware Images List state on a Firmware Distribution Server.
/// - note:   The response to a Firmware Distribution Firmware Get By Index message is a
/// Firmware Distribution Firmware Status message.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.105),
/// 8.4.2.19 Firmware Distribution Firmware Get By Index.
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - distributionFirmwareImageIndex: Index of the entry in the Firmware Images List state.
/// The Distribution Firmware Image Index field shall indicate the index of the entry in the
/// Firmware Images List state.
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)firmwareDistributionFirmwareGetByIndexWithDestination:(UInt16)destination distributionFirmwareImageIndex:(UInt16)distributionFirmwareImageIndex retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareDistributionFirmwareStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareDistributionFirmwareGetByIndex *message = [[SigFirmwareDistributionFirmwareGetByIndex alloc] initWithDistributionFirmwareImageIndex:distributionFirmwareImageIndex];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseFirmwareDistributionFirmwareStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 8.4.2.20 Firmware Distribution Firmware Delete, opcode:0xB61E


/// The Firmware Distribution Firmware Delete message is an acknowledged message
/// sent by a Firmware Distribution Client to delete a stored firmware image on a
/// Firmware Distribution Server.
/// - note:   The response to a Firmware Distribution Firmware Delete message is a
/// Firmware Distribution Firmware Status message.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.106),
/// 8.4.2.20 Firmware Distribution Firmware Delete.
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - firmwareID: Identifies the firmware image to delete. Size is Variable.
/// The Firmware ID field shall identify the firmware image to be deleted from the
/// Firmware Images List state.
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)firmwareDistributionFirmwareDeleteWithDestination:(UInt16)destination firmwareID:(NSData *)firmwareID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareDistributionFirmwareStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareDistributionFirmwareDelete *message = [[SigFirmwareDistributionFirmwareDelete alloc] initWithFirmwareID:firmwareID];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseFirmwareDistributionFirmwareStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 8.4.2.21 Firmware Distribution Firmware Delete All, opcode:0xB61F


/// The Firmware Distribution Firmware Delete All message is an acknowledged message
/// sent by a Firmware Distribution Client to delete all firmware images stored on a
/// Firmware Distribution Server.
/// - note:   The response to a Firmware Distribution Firmware Delete All message is a
/// Firmware Distribution Firmware Status message.
/// - note:   There are no parameters for this message.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.106),
/// 8.4.2.21 Firmware Distribution Firmware Delete All.
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)firmwareDistributionFirmwareDeleteAllWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseFirmwareDistributionFirmwareStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigFirmwareDistributionFirmwareDeleteAll *message = [[SigFirmwareDistributionFirmwareDeleteAll alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseFirmwareDistributionFirmwareStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark - 7.3.1 BLOB Transfer messages


#pragma mark 7.3.1.1 BLOB Transfer Get, opcode:0xB701


/// BLOB Transfer Get is an acknowledged message used to get
/// the state of a BLOB transfer, if any, on a BLOB Transfer Server.
/// - note:   The response to a BLOB Transfer Get message is a
/// BLOB Transfer Status message.
/// - note:   There are no parameters for this message.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.28),
/// 7.3.1.1 BLOB Transfer Get.
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)BLOBTransferGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseBLOBTransferStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigBLOBTransferGet *message = [[SigBLOBTransferGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseBLOBTransferStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 7.3.1.2 BLOB Transfer Start, opcode:0xB702


/// BLOB Transfer Start is an acknowledged message used to start
/// a new BLOB transfer.
/// - note:   The response to a BLOB Transfer Start message is a
/// BLOB Transfer Status message.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.28),
/// 7.3.1.2 BLOB Transfer Start.
/// - note: eg:
/// ---> Sending - Access PDU, source:(0001)->destination: (0012) Op Code: (0xB707), accessPdu=B707
/// <--- Response - Access PDU, source:(0012)->destination: (0001) Op Code: (0000007E), accessPdu=7E4000000001
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - transferMode: BLOB transfer mode, szie is 2 bits.
/// The Transfer Mode field shall indicate the new value of the Transfer
/// Mode state (see Section 7.2.1.4). The valid values of the field are
/// Pull BLOB Transfer Mode and Push BLOB Transfer Mode.
///   - BLOBID: Unique BLOB identifier.
/// The BLOB ID field shall indicate the ID of the BLOB that is to be
/// transferred.
///   - BLOBSize: BLOB size in bytes
/// The BLOB Size field shall indicate the size of the BLOB to be
/// transferred.
///   - BLOBBlockSizeLog: Size of the block during this transfer.
/// The Block Size Log field shall indicate the new value of the Block
/// Size Log state to be used in the transfer (see Section 7.2.1.1.3).
///   - MTUSize: Maximum payload size supported by the client.
/// The Client MTU Size field shall indicate the new value of the Client
/// MTU Size state (see Section 7.2.2.5).
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)BLOBTransferStartWithDestination:(UInt16)destination transferMode:(SigTransferModeState)transferMode BLOBID:(UInt64)BLOBID BLOBSize:(UInt32)BLOBSize BLOBBlockSizeLog:(UInt8)BLOBBlockSizeLog MTUSize:(UInt16)MTUSize retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseBLOBTransferStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigBLOBTransferStart *message = [[SigBLOBTransferStart alloc] initWithTransferMode:transferMode BLOBID:BLOBID BLOBSize:BLOBSize BLOBBlockSizeLog:BLOBBlockSizeLog MTUSize:MTUSize];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseBLOBTransferStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 7.3.1.3 BLOB Transfer Cancel, opcode:0xB703


/// BLOB Transfer Cancel is an acknowledged message used to cancel
/// the ongoing BLOB transfer.
/// - note:   The response to the BLOB Transfer Cancel message is a
/// BLOB Transfer Status message.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.28),
/// 7.3.1.3 BLOB Transfer Cancel.
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - BLOBID: BLOB identifier.
/// The BLOB ID field shall identify the BLOB whose transfer is to be canceled.
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)BLOBTransferCancelWithDestination:(UInt16)destination BLOBID:(UInt64)BLOBID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseBLOBTransferStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigObjectTransferCancel *message = [[SigObjectTransferCancel alloc] initWithBLOBID:BLOBID];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseBLOBTransferStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 3.1.3.1.5 BLOB Block Get, opcode:0xB707


/// BLOB Block Get is an acknowledged message used to retrieve the phase
/// of the transfer and to find out which block is active (i.e., being transferred),
/// if any, and get the status of that block.
/// - note:   The response to the BLOB Block Get message is a BLOB Block
/// Status message.
/// - note:   There are no parameters for this message.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.29),
/// 3.1.3.1.5 BLOB Block Get.
/// - note: eg:
/// ---> Sending - Access PDU, source:(0001)->destination: (0012) Op Code: (0xB707), accessPdu=B707
/// <--- Response - Access PDU, source:(0012)->destination: (0001) Op Code: (0000007E), accessPdu=7E4000000001
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)BLOBBlockGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseBLOBBlockStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigBLOBBlockGet *message = [[SigBLOBBlockGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseBLOBBlockStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 3.1.3.1.6 BLOB Block Start, opcode:0xB705


/// BLOB Block Start is an acknowledged message used to start
/// the transfer of an incoming block to the server.
/// - note:   The response to the BLOB Block Start message is
/// a BLOB Block Status message.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.30),
/// 3.1.3.1.6 BLOB Block Start.
/// - note: eg:
/// ---> Sending - Access PDU, source:(0001)->destination: (0012) Op Code: (0xB705), accessPdu=B70500000001
/// <--- Response - Access PDU, source:(0012)->destination: (0001) Op Code: (0000007E), accessPdu=7E0000000001
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - blockNumber: Block number
/// The Block Number field shall indicate the new value of the Block
/// Number state (see Section 7.2.1.2.1).
///   - chunkSize: Chunk size bytes for this block
/// The Chunk Size field shall indicate the new value of the Chunk
/// Size state (see Section 7.2.1.2.2).
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)BLOBBlockStartWithDestination:(UInt16)destination blockNumber:(UInt16)blockNumber chunkSize:(UInt16)chunkSize retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseBLOBBlockStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigBLOBBlockStart *message = [[SigBLOBBlockStart alloc] initWithBlockNumber:blockNumber chunkSize:chunkSize];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseBLOBBlockStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 3.1.3.1.8 BLOB Chunk Transfer, opcode:0x7D


/// BLOB Chunk Transfer is an unacknowledged message used to
/// deliver a chunk of the current block to a BLOB Transfer Server.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.30),
/// 3.1.3.1.8 BLOB Chunk Transfer.
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - chunkNumber: Chunk number
/// The Chunk Number field shall indicate the chunk’s number in a set
/// of chunks in a block.
///   - chunkData: Part of the BLOB data, szie is 1 to Chunk Size.
/// The Chunk Data field shall contain the chunk of the block identified by
/// the Chunk Number. The size of the Chunk Data field shall be greater
/// than 0 octets, and shall be less than or equal to the number of octets
/// indicated by the Chunk Size state.
///   - sendBySegmentPdu: Whether to use outsourcing for data transmission
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)BLOBChunkTransferWithDestination:(UInt16)destination chunkNumber:(UInt16)chunkNumber chunkData:(NSData *)chunkData sendBySegmentPdu:(BOOL)sendBySegmentPdu retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount resultCallback:(resultBlock)resultCallback {
    SigBLOBChunkTransfer *message = [[SigBLOBChunkTransfer alloc] initWithChunkNumber:chunkNumber chunkData:chunkData sendBySegmentPdu:sendBySegmentPdu];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.resultCallback = resultCallback;
    command.responseMaxCount = responseMaxCount;
    command.retryCount = retryCount;
    command.timeout = 10.0;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}


#pragma mark 7.3.1.10 BLOB Information Get, opcode:0xB70A


/// BLOB Information Get is an acknowledged message used to get
/// the Capabilities state of the BLOB Transfer Server.
/// - note:   The response to the BLOB Information Get message is
/// a BLOB Information Status message.
/// - note:   There are no parameters for this message.
/// - note:   - seeAlso: MshMDL_DFU_MBT_CR_R06.pdf (page.32),
/// 7.3.1.10 BLOB Information Get.
/// - note: eg:
/// ---> Sending - Access PDU, source:(0001)->destination: (0002) Op Code: (0xB70A), accessPdu=B70A
/// <--- Response - Access PDU, Access PDU, source:(0002)->destination: (0001) Op Code: (0xB70B), accessPdu=B70B0C0C10000001000003007C0101
/// - Parameters:
///   - destination: the unicastAddress of destination.
///   - retryCount: the retryCount of this command.
///   - responseMaxCount: the max response status message count of this command.
///   - successCallback: callback when node response the status message.
///   - resultCallback: callback when all the response message had response or timeout.
///  - Returns:
///  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
+ (SigMessageHandle *)BLOBInformationGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseBLOBInformationStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigBLOBInformationGet *message = [[SigBLOBInformationGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseBLOBInformationStatusCallBack = successCallback;
    return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:[[SigMeshAddress alloc] initWithAddress:destination] usingApplicationKey:SigMeshLib.share.dataSource.curAppkeyModel command:command];
}

@end
