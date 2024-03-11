/********************************************************************************************************
 * @file NodeIdentitySetMessage.java
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
 * The Config Node Identity Set is a message used to set the current Node Identity state for a subnet.
 * It is an acknowledged message, meaning that the sender expects a response.
 * The response to a Config Node Identity Set message is a Config Node Identity Status message.
 * {@link NodeIdentityStatusMessage}
 */
public class NodeIdentitySetMessage extends ConfigMessage {

    // Variables
    public int netKeyIndex;
    public byte identity;

    /**
     * Creates a new NodeIdentitySetMessage with the given destination address.
     *
     * @param destinationAddress The destination address for the message.
     */
    public NodeIdentitySetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Creates a simple NodeIdentitySetMessage with the given destination address, netKeyIndex, and identity.
     *
     * @param destinationAddress The destination address for the message.
     * @param netKeyIndex        The netKey index to set.
     * @param identity           The identity to set.
     * @return The created NodeIdentitySetMessage.
     */
    public static NodeIdentitySetMessage getSimple(int destinationAddress, int netKeyIndex, byte identity) {
        NodeIdentitySetMessage message = new NodeIdentitySetMessage(destinationAddress);
        message.netKeyIndex = netKeyIndex;
        message.identity = identity;
        return message;
    }

    /**
     * Gets the opcode for the NodeIdentitySetMessage.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.NODE_ID_SET.value;
    }

    /**
     * Gets the response opcode for the NodeIdentitySetMessage.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.NODE_ID_STATUS.value;
    }

    /**
     * Gets the parameters for the NodeIdentitySetMessage.
     *
     * @return The parameters as a byte array.
     */
    @Override
    public byte[] getParams() {
        // netKey index lower 12 bits
        int netAppKeyIndex = (netKeyIndex & 0x0FFF);
        ByteBuffer paramsBuffer = ByteBuffer.allocate(3).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) (netKeyIndex & 0x0FFF))
                .put((byte) identity);
        return paramsBuffer.array();
    }

    /**
     * Sets the netKeyIndex for the NodeIdentitySetMessage.
     *
     * @param netKeyIndex The netKeyIndex to set.
     */
    public void setNetKeyIndex(int netKeyIndex) {
        this.netKeyIndex = netKeyIndex;
    }

    /**
     * Sets the identity for the NodeIdentitySetMessage.
     *
     * @param identity The identity to set.
     */
    public void setIdentity(byte identity) {
        this.identity = identity;
    }
}