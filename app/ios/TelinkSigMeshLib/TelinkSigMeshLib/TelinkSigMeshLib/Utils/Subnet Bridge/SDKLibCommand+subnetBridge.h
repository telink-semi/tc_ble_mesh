/********************************************************************************************************
 * @file     SDKLibCommand+subnetBridge.h
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2021/2/2
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

NS_ASSUME_NONNULL_BEGIN

@interface SDKLibCommand (subnetBridge)


#pragma mark - 4.3.11 Bridge messages
/*
 The Bridge messages are used to configure the behavior of a Subnet Bridge node.
 The Bridge messages shall be encrypted and authenticated using the DevKey of the Subnet Bridge node.
 */

#pragma mark 4.3.11.1 SUBNET_BRIDGE_GET, opcode:0xBF70


/**
 * @brief   SUBNET_BRIDGE_GET.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.406),
 * 4.3.11.1 SUBNET_BRIDGE_GET.
 */
+ (SigMessageHandle *)subnetBridgeGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSubnetBridgeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark 4.3.11.2 SUBNET_BRIDGE_SET, opcode:0xBF71


/**
 * @brief   SUBNET_BRIDGE_SET.
 * @param   destination    the unicastAddress of destination.
 * @param   subnetBridge    New Subnet Bridge state.
 * The Subnet_Bridge field determines the new Subnet Bridge state for the node as
 * defined in Section 4.2.41.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.407),
 * 4.3.11.2 SUBNET_BRIDGE_SET.
 */
+ (SigMessageHandle *)subnetBridgeSetWithDestination:(UInt16)destination subnetBridge:(SigSubnetBridgeStateValues)subnetBridge retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseSubnetBridgeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark 4.3.11.4 BRIDGING_TABLE_ADD, opcode:0xBF73


/**
 * @brief   BRIDGING_TABLE_ADD.
 * @param   destination    the unicastAddress of destination.
 * @param   subnetBridge    New Subnet Bridge state.
 * Current Subnet Bridge state, size is 8 bytes.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.408),
 * 4.3.11.4 BRIDGING_TABLE_ADD.
 */
+ (SigMessageHandle *)bridgeTableAddWithDestination:(UInt16)destination subnetBridge:(SigSubnetBridgeModel *)subnetBridge retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseBridgeTableStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark 4.3.11.5 BRIDGING_TABLE_REMOVE, opcode:0xBF74


/**
 * @brief   BRIDGING_TABLE_REMOVE.
 * @param   destination    the unicastAddress of destination.
 * @param   netKeyIndex1    NetKey Index of the first subnet, size is 12 bits.
 * Each of the NetKeyIndex1 and NetKeyIndex2 fields is the global NetKey Index of the NetKey
 * associated with the first subnet and the second subnet, respectively.
 * @param   netKeyIndex2    NetKey Index of the second subnet, size is 12 bits.
 * The Address1 and Address2 fields identify the addresses of the nodes in the first subnet and in the second subnet, respectively.
 * @param   address1    Address of the node in the first subnet, size is 16 bits.
 * The Address1 field value shall be the unassigned address or a unicast address.
 * The Address1 and Address2 fields identify the addresses of the nodes in the first subnet and in the second subnet, respectively.
 * @param   address2    Address of the node in the second subnet, size is 16 bits.
 * The Address2 field value shall not be the all-nodes fixed group address.
 * The Address1 and Address2 fields identify the addresses of the nodes in the first subnet and in the second subnet, respectively.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.408),
 * 4.3.11.5 BRIDGING_TABLE_REMOVE.
 */
+ (SigMessageHandle *)bridgeTableRemoveWithDestination:(UInt16)destination netKeyIndex1:(UInt16)netKeyIndex1 netKeyIndex2:(UInt16)netKeyIndex2 address1:(UInt16)address1 address2:(UInt16)address2 retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseBridgeTableStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark 4.3.11.7 BRIDGED_SUBNETS_GET, opcode:0xBF76


/**
 * @brief   BRIDGED_SUBNETS_GET.
 * @param   destination    the unicastAddress of destination.
 * @param   filter    Filter to be applied when reporting the set of pairs of NetKey Indexes.
 * The Filter field determines the filtering to be applied when reporting the set of pairs of NetKey
 * Indexes extracted from the Bridging Table state entries in the response message.
 * @param   prohibited    Prohibited.
 * @param   netKeyIndex    NetKey Index of any of the subnets.
 * The NetKeyIndex field is the global NetKey Index of the NetKey to be used for filtering if the Filter field value is different from 0b00.
 * @param   startIndex    Start offset in units of pairs of NetKey Indexes to read.
 * The Start_Index field determines the offset in units of pairs of NetKey Indexes (see Table 4.293) to start from when reporting
 * the filtered set of pairs of NetKey Indexes extracted from the Bridging Table state entries of a Subnet Bridge.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.410),
 * 4.3.11.7 BRIDGED_SUBNETS_GET.
 */
+ (SigMessageHandle *)bridgeSubnetsGetWithDestination:(UInt16)destination filter:(SigFilterFieldValues)filter prohibited:(UInt8)prohibited netKeyIndex:(UInt16)netKeyIndex startIndex:(UInt8)startIndex retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseBridgeSubnetsListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark 4.3.11.9 BRIDGING_TABLE_GET, opcode:0xBF78


/**
 * @brief   BRIDGING_TABLE_GET.
 * @param   destination    the unicastAddress of destination.
 * @param   netKeyIndex1    NetKey Index of first subnet.
 * The NetKeyIndex1 field is the global NetKey Index of the NetKey associated with
 * the first subnet (NetKeyIndex1) in the Bridging Table state.
 * @param   netKeyIndex2    NetKey Index of second subnet.
 * The NetKeyIndex2 field is the global NetKey Index of the NetKey associated with
 * the second subnet (NetKeyIndex2) in the Bridging Table state.
 * @param   startIndex    Start offset to read in units of Bridging Table state entries.
 * The Start_Index field determines the offset in units of Bridging Table state entries (see Section 4.2.42) to start
 * from when reporting the list of addresses and allowed traffic directions of the Bridging Table state.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.412),
 * 4.3.11.9 BRIDGING_TABLE_GET.
 */
+ (SigMessageHandle *)bridgeTableGetWithDestination:(UInt16)destination netKeyIndex1:(UInt16)netKeyIndex1 netKeyIndex2:(UInt16)netKeyIndex2 startIndex:(UInt16)startIndex retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseBridgeTableListMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;


#pragma mark 4.3.11.11 BRIDGING_TABLE_SIZE_GET, opcode:0xBF7A


/**
 * @brief   BRIDGING_TABLE_SIZE_GET.
 * @param   destination    the unicastAddress of destination.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note   - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.414),
 * 4.3.11.11 BRIDGING_TABLE_SIZE_GET.
 */
+ (SigMessageHandle *)bridgingTableSizeGetWithDestination:(UInt16)destination retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseBridgingTableSizeStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;

@end

NS_ASSUME_NONNULL_END
