/********************************************************************************************************
 * @file OpcodeAggregatorSequenceMessage.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2017
 *
 * @par Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
package com.telink.ble.mesh.core.message.aggregator;

import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.networking.AccessType;


/**
 * An OPCODES_AGGREGATOR_SEQUENCE message is an acknowledged message
 * that the Opcodes Aggregator Client uses to encapsulate a sequence of access messages to be processed by the Opcodes Aggregator Server.
 * <p>
 * The response to an OPCODES_AGGREGATOR_SEQUENCE message is an OPCODES_AGGREGATOR_STATUS message.
 *
 * @see OpcodeAggregatorStatusMessage
 */
public class OpcodeAggregatorSequenceMessage extends MeshMessage {

    /**
     * @param accessType  message type, defines the encryption key
     * @param appKeyIndex used when access type is AccessType.APPLICATION
     */
    public OpcodeAggregatorSequenceMessage(int destinationAddress,
                                           AccessType accessType,
                                           int appKeyIndex,
                                           byte[] params
    ) {
        this.destinationAddress = destinationAddress;
        this.accessType = accessType;
        this.appKeyIndex = appKeyIndex;
        this.params = params;
    }


    @Override
    public int getOpcode() {
        return Opcode.OP_AGGREGATOR_SEQUENCE.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.OP_AGGREGATOR_STATUS.value;
    }

}
