/********************************************************************************************************
 * @file NetKeyDeleteMessage.java
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
 * The NetKeyDeleteMessage class represents a Config NetKey Delete message.
 * This message is used to delete a NetKey from a NetKey List on a node.
 * <p>
 * The message is sent to the destination address specified in the constructor.
 * <p>
 * The Config NetKey Delete is an acknowledged message used to delete a NetKey on a NetKey List from a node.
 * The response to this message is a Config NetKey Status message.
 */
public class NetKeyDeleteMessage extends ConfigMessage {

    // The index of the NetKey to be deleted
    public int netKeyIndex;

    /**
     * Constructs a NetKeyDeleteMessage object with the specified destination address.
     *
     * @param destinationAddress The destination address for the message.
     */
    public NetKeyDeleteMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Constructs a NetKeyDeleteMessage object with the specified destination address and NetKey index.
     *
     * @param destinationAddress The destination address for the message.
     * @param netKeyIndex        The index of the NetKey to be deleted.
     */
    public NetKeyDeleteMessage(int destinationAddress, int netKeyIndex) {
        super(destinationAddress);
        this.netKeyIndex = netKeyIndex;
    }

    /**
     * Returns the opcode for the NetKey Delete message.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.NETKEY_DEL.value;
    }

    /**
     * Returns the opcode for the response to the NetKey Delete message.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.NETKEY_STATUS.value;
    }

    /**
     * Returns the parameters of the NetKey Delete message as a byte array.
     *
     * @return The parameters byte array.
     */
    @Override
    public byte[] getParams() {
        ByteBuffer paramsBuffer = ByteBuffer.allocate(2).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) this.netKeyIndex);
        return paramsBuffer.array();
    }
}
