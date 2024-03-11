/********************************************************************************************************
 * @file     SDKLibCommand+privateBeacon.m
 *
 * @brief    A concise description.
 *
 * @author   Telink, 梁家誌
 * @date     2022/3/9
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

#import "SDKLibCommand+privateBeacon.h"

@implementation SDKLibCommand (privateBeacon)


#pragma mark - 4.3.12 Mesh Private Beacon messages


#pragma mark 4.3.12.1 PRIVATE_BEACON_GET, opcode:0xB711


/**
 * @brief   PRIVATE_BEACON_GET.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.415),
 * 4.3.12.1 PRIVATE_BEACON_GET.
 */
+ (SigMessageHandle *)privateBeaconGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responsePrivateBeaconStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigPrivateBeaconGet *message = [[SigPrivateBeaconGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responsePrivateBeaconStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:message toDestination:destination command:command];
}


#pragma mark 4.3.12.2 PRIVATE_BEACON_SET, opcode:0xB712


/**
 * @brief   PRIVATE_BEACON_SET.
 * @param   privateBeacon    New Private Beacon state.
 * The Private_Beacon field shall identify the value of the new Private Beacon
 * state (see Section 4.2.44.1) of a node.
 * @param   randomUpdateIntervalSteps    New Random Update Interval Steps state (optional).
 * 0x00, Random field is updated for every Mesh Private beacon.
 * 0x01–0xFF, Random field is updated at an interval (in seconds) of (10 * Random Update Interval Steps).
 * If present, the Random_Update_Interval_Steps field shall identify the value of the
 * new Random Update Interval Steps state (see Section 4.2.44.2) of a node.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.415),
 * 4.3.12.2 PRIVATE_BEACON_SET.
 */
+ (SigMessageHandle *)privateBeaconSetWithPrivateBeacon:(SigPrivateBeaconState)privateBeacon randomUpdateIntervalSteps:(UInt8)randomUpdateIntervalSteps destination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responsePrivateBeaconStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigPrivateBeaconSet *message = [[SigPrivateBeaconSet alloc] initWithPrivateBeacon:privateBeacon randomUpdateIntervalSteps:randomUpdateIntervalSteps];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responsePrivateBeaconStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:message toDestination:destination command:command];
}

/**
 * @brief   PRIVATE_BEACON_SET.
 * @param   privateBeacon    New Private Beacon state.
 * The Private_Beacon field shall identify the value of the new Private Beacon
 * state (see Section 4.2.44.1) of a node.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.415),
 * 4.3.12.2 PRIVATE_BEACON_SET.
 */
+ (SigMessageHandle *)privateBeaconSetWithPrivateBeacon:(SigPrivateBeaconState)privateBeacon destination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responsePrivateBeaconStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigPrivateBeaconSet *message = [[SigPrivateBeaconSet alloc] initWithPrivateBeacon:privateBeacon];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responsePrivateBeaconStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:message toDestination:destination command:command];
}


#pragma mark 4.3.12.4 PRIVATE_GATT_PROXY_GET, opcode:0xB714


/**
 * @brief   PRIVATE_GATT_PROXY_GET.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.416),
 * 4.3.12.4 PRIVATE_GATT_PROXY_GET.
 */
+ (SigMessageHandle *)privateGattProxyGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responsePrivateGattProxyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigPrivateGattProxyGet *message = [[SigPrivateGattProxyGet alloc] init];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responsePrivateGattProxyStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:message toDestination:destination command:command];
}


#pragma mark 4.3.12.5 PRIVATE_GATT_PROXY_SET, opcode:0xB715


/**
 * @brief   PRIVATE_GATT_PROXY_SET.
 * @param   privateGattProxy    New Private GATT Proxy state.
 * The Private_GATT_Proxy field shall provide the new Private GATT Proxy
 * state of the node (see Section 4.2.45). The Private_GATT_Proxy field shall
 * be either Disable (0x00) or Enable (0x01), and all other values prohibited.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.417),
 * 4.3.12.5 PRIVATE_GATT_PROXY_SET.
 */
+ (SigMessageHandle *)privateGattProxySetWithPrivateGattProxy:(SigPrivateGattProxyState)privateGattProxy destination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responsePrivateGattProxyStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigPrivateGattProxySet *message = [[SigPrivateGattProxySet alloc] initWithPrivateGattProxy:privateGattProxy];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responsePrivateGattProxyStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:message toDestination:destination command:command];
}


#pragma mark 4.3.12.7 PRIVATE_NODE_IDENTITY_GET, opcode:0xB718


/**
 * @brief   PRIVATE_NODE_IDENTITY_GET.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.417),
 * 4.3.12.7 PRIVATE_NODE_IDENTITY_GET.
 */
+ (SigMessageHandle *)privateNodeIdentityGetWithDestination:(UInt16)destination netKeyIndex:(UInt16)netKeyIndex retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responsePrivateNodeIdentityStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigPrivateNodeIdentityGet *message = [[SigPrivateNodeIdentityGet alloc] initWithNetKeyIndex:netKeyIndex];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responsePrivateNodeIdentityStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:message toDestination:destination command:command];
}


#pragma mark 4.3.12.8 PRIVATE_NODE_IDENTITY_SET, opcode:0xB719


/**
 * @brief   PRIVATE_NODE_IDENTITY_SET.
 * @param   netKeyIndex    Index of the NetKey.
 * The NetKeyIndex field is an index that shall identify the global NetKey Index of the NetKey
 * of the Node Identity state. The NetKeyIndex field shall be encoded as defined in Section 4.3.1.1.
 * @param   privateIdentity    New Private Node Identity state.
 * The Private_Identity field shall provide the new Private Node Identity state of the
 * NetKey (see Section 4.2.46). The Private_Identity field shall be either Disable (0x00) or
 * Enable (0x01), and all other values prohibited.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.418),
 * 4.3.12.8 PRIVATE_NODE_IDENTITY_SET.
 */
+ (SigMessageHandle *)privateNodeIdentitySetWithNetKeyIndex:(UInt16)netKeyIndex privateIdentity:(SigPrivateNodeIdentityState)privateIdentity destination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responsePrivateNodeIdentityStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SigPrivateNodeIdentitySet *message = [[SigPrivateNodeIdentitySet alloc] initWithNetKeyIndex:netKeyIndex privateIdentity:privateIdentity];
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responsePrivateNodeIdentityStatusCallBack = successCallback;
    return [SigMeshLib.share sendConfigMessage:message toDestination:destination command:command];
}

@end
