/********************************************************************************************************
 * @file     SDKLibCommand+opcodesAggregatorSequence.m
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

#import "SDKLibCommand+opcodesAggregatorSequence.h"

@implementation SDKLibCommand (opcodesAggregatorSequence)


#pragma mark - 4.3.9 Opcodes Aggregator messages


#pragma mark 4.3.9.2 OPCODES_AGGREGATOR_SEQUENCE, opcode:0xB809


/**
 * @brief   OPCODES_AGGREGATOR_SEQUENCE.
 * @param   message    the SigOpcodesAggregatorSequence object.
 * @param   retryCount    the retryCount of this command.
 * @param   responseMaxCount    the max response status message count of this command.
 * @param   successCallback    callback when node response the status message.
 * @param   resultCallback    callback when all the response message had response or timeout.
 * @return  A SigMessageHandle object of this command, developer can cannel the command by this SigMessageHandle object.
 * @note    - seeAlso: MshPRTd1.1r20_PRr00.pdf (page.403),
 * 4.3.9.2 OPCODES_AGGREGATOR_SEQUENCE.
 */
+ (SigMessageHandle *)sendSigOpcodesAggregatorSequenceMessage:(SigOpcodesAggregatorSequence *)message retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount successCallback:(responseOpcodesAggregatorStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback {
    SDKLibCommand *command = [[SDKLibCommand alloc] initWithMessage:message retryCount:retryCount responseMaxCount:responseMaxCount responseAllMessageCallBack:(responseAllMessageBlock)successCallback resultCallback:resultCallback];
    command.responseOpcodesAggregatorStatusCallBack = successCallback;
    command.curNetkey = SigMeshLib.share.dataSource.curNetkeyModel;
    command.curAppkey = SigMeshLib.share.dataSource.curAppkeyModel;
    command.curIvIndex = SigMeshLib.share.dataSource.curNetkeyModel.ivIndex;
    command.hadRetryCount = 0;
    command.tidPosition = 0;
    command.tid = 0;
    command.source = SigMeshLib.share.dataSource.curLocationNodeModel.elements.firstObject;
    command.destination = [[SigMeshAddress alloc] initWithAddress:message.elementAddress];
    if (message.isEncryptByDeviceKey) {
        //mesh数据使用DeviceKey进行加密
        return [SigMeshLib.share sendConfigMessage:(SigConfigMessage *)message toDestination:message.elementAddress command:command];
    } else {
        //mesh数据使用AppKey进行加密
        return [SigMeshLib.share sendMeshMessage:message fromLocalElement:nil toDestination:command.destination usingApplicationKey:command.curAppkey command:command];
    }
}

@end
