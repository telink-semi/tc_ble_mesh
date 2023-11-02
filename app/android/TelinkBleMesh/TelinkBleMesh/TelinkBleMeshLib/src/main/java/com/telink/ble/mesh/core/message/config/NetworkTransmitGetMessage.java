/********************************************************************************************************
 * @file NetworkTransmitGetMessage.java
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
 * The Config Network Transmit Get is an acknowledged message used to get the current Network Transmit state of a node
 * The NetworkTransmitGetMessage class represents an acknowledged message used to retrieve the current Network Transmit state of a node.
 * It extends the ConfigMessage class.
 */
public class NetworkTransmitGetMessage extends ConfigMessage {

    /**
     * Constructs a new NetworkTransmitGetMessage object with the specified destination address.
     *
     * @param destinationAddress The destination address of the message.
     */
    public NetworkTransmitGetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Returns the opcode value for the Network Transmit Get message.
     *
     * @return The opcode value for the Network Transmit Get message.
     */
    @Override
    public int getOpcode() {
        return Opcode.CFG_NW_TRANSMIT_GET.value;
    }

    /**
     * Returns the opcode value for the response message to the Network Transmit Get message.
     *
     * @return The opcode value for the response message to the Network Transmit Get message.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.CFG_NW_TRANSMIT_STATUS.value;
    }

    /**
     * Returns the parameters of the Network Transmit Get message.
     *
     * @return The parameters of the Network Transmit Get message.
     */
    @Override
    public byte[] getParams() {
        return null;
    }
}