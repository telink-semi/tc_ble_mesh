/********************************************************************************************************
 * @file     SDKLibCommand+remoteProvision.m
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

#import "SDKLibCommand+remoteProvision.h"

@implementation SDKLibCommand (remoteProvision)


#pragma mark - 4.3.4 Remote Provisioning messages
/*
 Remote Provisioning messages are used by a Remote Provisioning Client to communicate with a
 Remote Provisioning Server over a mesh network to find the UUID of unprovisioned devices
 within immediate radio range of the Remote Provisioning Server and to provision a remote
 unprovisioned device. Remote Provisioning messages also can be used to obtain extended
 information about an unprovisioned device or to execute a Device Key Refresh procedure or a
 Node Address Refresh procedure or a Node Composition Refresh procedure.
 */

#pragma mark 4.3.4.1 Remote Provisioning Scan Capabilities Get, opcode:0x804F


/**
 * @brief   Remote Provisioning Scan Capabilities Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.336),
 * 4.3.4.1 Remote Provisioning Scan Capabilities Get.
 */
+ (SigMessageHandle *)remoteProvisioningScanCapabilitiesGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseRemoteProvisioningScanCapabilitiesStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigRemoteProvisioningScanCapabilitiesGet *message = [[SigRemoteProvisioningScanCapabilitiesGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseRemoteProvisioningScanCapabilitiesStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = responseMaxCount;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendConfigMessage:message toDestination:destination command:command];
}

#pragma mark 4.3.4.3 Remote Provisioning Scan Get, opcode:0x8051


/**
 * @brief   Remote Provisioning Scan Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.337),
 * 4.3.4.3 Remote Provisioning Scan Get.
 */
+ (SigMessageHandle *)remoteProvisioningScanGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseRemoteProvisioningScanStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigRemoteProvisioningScanGet *message = [[SigRemoteProvisioningScanGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseRemoteProvisioningScanStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = responseMaxCount;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendConfigMessage:message toDestination:destination command:command];
}

#pragma mark 4.3.4.4 Remote Provisioning Scan Start, opcode:0x8052


/**
 * @brief   Remote Provisioning Scan Start.
 * @param   destination    the unicastAddress of destination.
 * @param   scannedItemsLimit   Maximum number of scanned items to be reported.
 * The ScannedItemsLimit field identifies the maximum number of unprovisioned
 * devices the Remote Provisioning Server can report while executing the Remote
 * Provisioning Scan procedure. Value 0 indicates that the Remote Provisioning
 * Client does not set a limit on the number of unprovisioned devices that the
 * Remote Provisioning Server can report.
 * @param   timeout    Time limit for a scan (in seconds).
 * The Timeout field identifies the new value of the Remote Provisioning Timeout
 * state (see Section 4.2.23.2). The value of the Timeout field shall not be 0.
 * @param   UUID    Device UUID (Optional)
 * If the UUID field is present, the Remote Provisioning Client is requesting a
 * Single Device Scanning procedure, i.e., a scan for a specific device identified
 * by the value of the UUID field. If the UUID field is absent, the Remote
 * Provisioning Client is requesting a scan for all unprovisioned devices within
 * immediate radio range (a Multiple Devices Scanning).
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.337),
 * 4.3.4.4 Remote Provisioning Scan Start.
 */
+ (SigMessageHandle *)remoteProvisioningScanStartWithDestination:(UInt16)destination scannedItemsLimit:(UInt8)scannedItemsLimit timeout:(UInt8)timeout UUID:(nullable NSData *)UUID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseRemoteProvisioningScanStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigRemoteProvisioningScanStart *message = [[SigRemoteProvisioningScanStart alloc] initWithScannedItemsLimit:scannedItemsLimit timeout:timeout UUID:UUID];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseRemoteProvisioningScanStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = responseMaxCount;
    command.retryCount = retryCount;
    // 特殊处理：因为该指令是存在response的，SDK会自动将异常的responseMaxCount=0修改为responseMaxCount=1，但该指令又不希望SDK对其进行retry，因此在此处修改一个很大的重试时间间隔timeout以避免预期外的超时callback。（想到更好的逻辑后再优化此代码。）
    if (responseMaxCount == 0 && retryCount == 0) {
        command.timeout = 0xFFFF;
    }
    return [SigMeshLib.share sendConfigMessage:message toDestination:destination command:command];
}


#pragma mark 4.3.4.5 Remote Provisioning Scan Stop, opcode:0x8053


/**
 * @brief   Remote Provisioning Scan Stop.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.338),
 * 4.3.4.5 Remote Provisioning Scan Stop.
 */
+ (SigMessageHandle *)remoteProvisioningScanStopWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseRemoteProvisioningScanStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigRemoteProvisioningScanStop *message = [[SigRemoteProvisioningScanStop alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseRemoteProvisioningScanStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = responseMaxCount;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendConfigMessage:message toDestination:destination command:command];
}


#pragma mark 4.3.4.8 Remote Provisioning Extended Scan Start, opcode:0x8056


/**
 * @brief   Remote Provisioning Extended Scan Start.
 * @param   destination    the unicastAddress of destination.
 * @param   ADTypeFilterCount    Number of AD Types in the ADTypeFilter field.
 * The ADTypeFilterCount field identifies number of AD types listed in the ADTypeFilter field.
 * @param   ADTypeFilter    List of AD Types to be reported (C.1), size:variable.
 * C.1: If ADTypeFilterCount field value is not zero, the ADTypeFilter shall be present; otherwise
 * the ADTypeFilter shall not be present
 * If present, the ADTypeFilter is a list of AD types that the client is requesting. The ADTypeFilter
 * shall not contain the Shortened Local Name AD Type, the Incomplete List of 16-bit Service
 * UUIDs AD Type, the Incomplete List of 32-bit Service UUIDs AD Type, or the Incomplete List
 * of 128-bit Service UUIDs AD Type.
 * Note: If the ADTypeFilter field contains the Complete Local Name AD Type, the client is
 * requesting either the Complete Local Name or the Shortened Local Name.
 * @param   UUID    Device UUID (Optional).
 * If present, the UUID field identifies the Device UUID of the unprovisioned device for which
 * additional information is requested (see Section 4.4.5.3). If the UUID field is absent, the
 * request retrieves information about the Remote Provisioning Server (see Section 4.4.5.5.2.1).
 * In the latter case, the Remote Provisioning Server ignores the Timeout field value.
 * @param   timeout    Time limit for a scan (in seconds) (C.2). Length of time (in seconds) to collect information
 * about the unprovisioned device: 0x01~0x05
 * C.2: If UUID field is present, the Timeout field shall also be present; otherwise Timeout
 * field shall not be present.
 * The Timeout field indicates how long the Remote Provisioning Client requests the Remote
 * Provisioning Server to collect information about the unprovisioned device identified by the
 * UUID. Table 4.11 defines the values for the Timeout field.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.340),
 * 4.3.4.8 Remote Provisioning Extended Scan Start.
 */
+ (SigMessageHandle *)remoteProvisioningExtendedScanStartWithDestination:(UInt16)destination ADTypeFilterCount:(UInt8)ADTypeFilterCount ADTypeFilter:(nullable NSData *)ADTypeFilter UUID:(nullable NSData *)UUID timeout:(UInt8)timeout retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount resultCallback:(resultBlock)resultCallback {
    SigRemoteProvisioningExtendedScanStart *message = [[SigRemoteProvisioningExtendedScanStart alloc] initWithADTypeFilterCount:ADTypeFilterCount ADTypeFilter:ADTypeFilter UUID:UUID timeout:timeout];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.resultCallback = resultCallback;
    command.responseMaxCount = responseMaxCount;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendConfigMessage:message toDestination:destination command:command];
}


#pragma mark 4.3.4.10 Remote Provisioning Link Get, opcode:0x8058


/**
 * @brief   Remote Provisioning Link Get.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.342),
 * 4.3.4.10 Remote Provisioning Link Get.
 */
+ (SigMessageHandle *)remoteProvisioningLinkGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseRemoteProvisioningLinkStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigRemoteProvisioningLinkGet *message = [[SigRemoteProvisioningLinkGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseRemoteProvisioningLinkStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = responseMaxCount;
    command.retryCount = retryCount;
    return [SigMeshLib.share sendConfigMessage:message toDestination:destination command:command];
}


#pragma mark 4.3.4.11 Remote Provisioning Link Open, opcode:0x8059


/**
 * @brief   Remote Provisioning Link Open.
 * @param   destination    the unicastAddress of destination.
 * @param   UUID    Device UUID (Optional).
 * If present, the UUID field identifies the Device UUID of an unprovisioned device that the
 * link will be open to. If the UUID field is absent, the Remote Provisioning Server will open
 * the Device Key Refresh Interface.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.342),
 * 4.3.4.11 Remote Provisioning Link Open.
 */
+ (SigMessageHandle *)remoteProvisioningLinkOpenWithDestination:(UInt16)destination UUID:(nullable NSData *)UUID retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseRemoteProvisioningLinkStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigRemoteProvisioningLinkOpen *message = [[SigRemoteProvisioningLinkOpen alloc] initWithUUID:UUID];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseRemoteProvisioningLinkStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = responseMaxCount;
    command.retryCount = retryCount;
    command.timeout = SigMeshLib.share.dataSource.defaultReliableIntervalOfNotLPN * 2;//link open 超时为1.28*2秒。
    return [SigMeshLib.share sendConfigMessage:message toDestination:destination command:command];
}


#pragma mark 4.3.4.12 Remote Provisioning Link Close, opcode:0x805A


/**
 * @brief   Remote Provisioning Link Close.
 * @param   destination    the unicastAddress of destination.
 * @param   reason    Link close reason code.
 * The Reason field identifies the reason for the provisioning link close. The Reason field values for the
 * Remote Provisioning Link Close message are defined in Table 4.15.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.343),
 * 4.3.4.12 Remote Provisioning Link Close.
 */
+ (SigMessageHandle *)remoteProvisioningLinkCloseWithDestination:(UInt16)destination reason:(SigRemoteProvisioningLinkCloseStatus)reason retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseRemoteProvisioningLinkStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigRemoteProvisioningLinkClose *message = [[SigRemoteProvisioningLinkClose alloc] initWithReason:reason];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.responseAllMessageCallBack = (responseAllMessageBlock)successCallback;
    command.responseRemoteProvisioningLinkStatusCallBack = successCallback;
    command.resultCallback = resultCallback;
    command.responseMaxCount = responseMaxCount;
    command.retryCount = retryCount;
    command.timeout = SigMeshLib.share.dataSource.defaultReliableIntervalOfNotLPN * 2;//link close 超时为1.28*2秒。
    return [SigMeshLib.share sendConfigMessage:message toDestination:destination command:command];
}


#pragma mark 4.3.4.14 Remote Provisioning PDU Send, opcode:0x805D


/**
 * @brief   Remote Provisioning PDU Send.
 * @param   destination    the unicastAddress of destination.
 * @param   outboundPDUNumber    Provisioning PDU identification number.
 * The OutboundPDUNumber field identifies the identification number of the Provisioning
 * PDU set in the ProvisioningPDU field.
 * @param   provisioningPDU    Provisioning PDU.
 * The ProvisioningPDU field identifies the Provisioning PDU that either will be sent to an
 * unprovisioned device or will be processed locally if the Device Key Refresh procedure
 * is in progress.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.345),
 * 4.3.4.14 Remote Provisioning PDU Send.
 */
+ (SigMessageHandle *)remoteProvisioningPDUSendWithDestination:(UInt16)destination outboundPDUNumber:(UInt8)outboundPDUNumber provisioningPDU:(NSData *)provisioningPDU retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount resultCallback:(resultBlock)resultCallback {
    SigRemoteProvisioningPDUSend *message = [[SigRemoteProvisioningPDUSend alloc] initWithOutboundPDUNumber:outboundPDUNumber provisioningPDU:provisioningPDU];
    SDKLibCommand *command = [[SDKLibCommand alloc] init];
    command.curMeshMessage = message;
    command.resultCallback = resultCallback;
    command.responseMaxCount = responseMaxCount;
    command.retryCount = retryCount;
    command.timeout = 10.0;//PDUSend 超时为10秒。
    return [SigMeshLib.share sendConfigMessage:message toDestination:destination command:command];
}

@end
