/********************************************************************************************************
 * @file RelayGetMessage.java
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
package com.telink.ble.mesh.core.message.config;

import com.telink.ble.mesh.core.message.Opcode;


/**
 * The Config Relay Get is an acknowledged message used to get the current Relay and Relay Retransmit states of a node.
 * The RelayGetMessage class represents an acknowledged message used to retrieve the current Relay and Relay Retransmit states of a node.
 */
public class RelayGetMessage extends ConfigMessage {

    /**
     * Constructs an empty RelayGetMessage object.
     */
    public RelayGetMessage() {
    }

    /**
     * Constructs a RelayGetMessage object with the specified destination address.
     *
     * @param destinationAddress The address of the destination node.
     */
    public RelayGetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Returns the opcode value for the RelayGetMessage.
     *
     * @return The opcode value for the RelayGetMessage.
     */
    @Override
    public int getOpcode() {
        return Opcode.CFG_RELAY_GET.value;
    }

    /**
     * Returns the response opcode value for the RelayGetMessage.
     *
     * @return The response opcode value for the RelayGetMessage.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.CFG_RELAY_STATUS.value;
    }

    /**
     * Returns the parameters of the RelayGetMessage.
     *
     * @return The parameters of the RelayGetMessage.
     */
    @Override
    public byte[] getParams() {
        return null;
    }
}