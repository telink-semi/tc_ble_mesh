/********************************************************************************************************
 * @file NetKeyAddMessage.java
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

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * The Config NetKey Add is an acknowledged message used to add a NetKey to a NetKey List on a node.
 * The added NetKey is then used by the node to authenticate and decrypt messages it receives, as well as authenticate and encrypt messages it sends.
 * <p>
 * The response to a Config NetKey Add message is a Config NetKey Status message.
 */

/**
 * This class represents a network key add message that is used to add a new network key to a device.
 * It extends the ConfigMessage class.
 */
public class NetKeyAddMessage extends ConfigMessage {
    public int netKeyIndex; // The index of the network key
    public byte[] netKey; // The network key

    /**
     * Constructs a NetKeyAddMessage object with the specified destination address.
     *
     * @param destinationAddress The destination address of the message.
     */
    public NetKeyAddMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Constructs a NetKeyAddMessage object with the specified destination address, network key index, and network key.
     *
     * @param destinationAddress The destination address of the message.
     * @param netKeyIndex        The index of the network key.
     * @param netKey             The network key.
     */
    public NetKeyAddMessage(int destinationAddress, int netKeyIndex, byte[] netKey) {
        super(destinationAddress);
        this.netKeyIndex = netKeyIndex;
        this.netKey = netKey;
    }

    /**
     * Returns the opcode of the message, which is NETKEY_ADD.
     *
     * @return The opcode of the message.
     */
    @Override
    public int getOpcode() {
        return Opcode.NETKEY_ADD.value;
    }

    /**
     * Returns the response opcode of the message, which is NETKEY_STATUS.
     *
     * @return The response opcode of the message.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.NETKEY_STATUS.value;
    }

    /**
     * Returns the parameters of the message as a byte array.
     *
     * @return The parameters of the message.
     */
    @Override
    public byte[] getParams() {
        ByteBuffer paramsBuffer = ByteBuffer.allocate(2 + 16).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) this.netKeyIndex)
                .put(netKey);
        return paramsBuffer.array();
    }
}