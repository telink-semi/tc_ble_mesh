/********************************************************************************************************
 * @file ForwardingTableAddMessage.java
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

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * This class represents a message used to delete an entry from the forwarding table.
 * It extends the ConfigMessage class.
 */
public class ForwardingTableDeleteMessage extends ConfigMessage {
    /**
     * The NetKey Index of the NetKey used in the subnet.
     * It is represented by 12 bits.
     */
    public int netKeyIndex;
    /**
     * The primary element address of the Path Origin.
     * It is represented by 16 bits.
     */
    public int pathOrigin;
    /**
     * The destination address.
     * It is represented by 16 bits.
     */
    public int destination;

    /**
     * Creates a simple ForwardingTableDeleteMessage with the specified destination address and NetKey Index.
     *
     * @param destinationAddress The destination address.
     * @param netKeyIndex        The NetKey Index.
     * @return The created ForwardingTableDeleteMessage.
     */
    public static ForwardingTableDeleteMessage getSimple(int destinationAddress, int netKeyIndex) {
        ForwardingTableDeleteMessage message = new ForwardingTableDeleteMessage(destinationAddress);
        message.setResponseMax(1);
        message.netKeyIndex = netKeyIndex;
        return message;
    }

    /**
     * Constructs a ForwardingTableDeleteMessage with the specified destination address.
     *
     * @param destinationAddress The destination address.
     */
    public ForwardingTableDeleteMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Gets the opcode value of the ForwardingTableDeleteMessage.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.FORWARDING_TABLE_DELETE.value;
    }

    /**
     * Gets the response opcode value of the ForwardingTableDeleteMessage.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.FORWARDING_TABLE_STATUS.value;
    }

    /**
     * Gets the parameters of the ForwardingTableDeleteMessage as a byte array.
     *
     * @return The parameters as a byte array.
     */
    @Override
    public byte[] getParams() {
        return ByteBuffer.allocate(6).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) netKeyIndex)
                .putShort((short) pathOrigin)
                .putShort((short) destination).array();
    }
}


