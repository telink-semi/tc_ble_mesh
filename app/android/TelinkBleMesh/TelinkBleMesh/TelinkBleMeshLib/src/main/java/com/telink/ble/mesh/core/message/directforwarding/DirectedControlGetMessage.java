/********************************************************************************************************
 * @file FirmwareUpdateGetMessage.java
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
package com.telink.ble.mesh.core.message.directforwarding;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.config.ConfigMessage;

/**
 * Represents a directed control get message.
 * This message is used to get the control information for a specific destination address.
 * Inherits from the ConfigMessage class.
 */
public class DirectedControlGetMessage extends ConfigMessage {
    public int netKeyIndex;

    /**
     * Creates a simple directed control get message with the specified destination address and network key index.
     *
     * @param destinationAddress The destination address for the message.
     * @param netKeyIndex        The network key index to be used.
     * @return The created DirectedControlGetMessage object.
     */
    public static DirectedControlGetMessage getSimple(int destinationAddress, int netKeyIndex) {
        DirectedControlGetMessage message = new DirectedControlGetMessage(destinationAddress);
        message.setResponseMax(1);
        message.netKeyIndex = netKeyIndex;
        return message;
    }

    /**
     * Creates a new DirectedControlGetMessage object with the specified destination address.
     *
     * @param destinationAddress The destination address for the message.
     */
    public DirectedControlGetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Gets the opcode value for the directed control get message.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.DIRECTED_CONTROL_GET.value;
    }

    /**
     * Gets the response opcode value for the directed control get message.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.DIRECTED_CONTROL_STATUS.value;
    }
}