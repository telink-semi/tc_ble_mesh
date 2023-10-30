/********************************************************************************************************
 * @file PrivateNodeIdentitySetMessage.java
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
package com.telink.ble.mesh.core.message.privatebeacon;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.config.ConfigMessage;
import com.telink.ble.mesh.core.message.config.NodeIdentitySetMessage;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * A PRIVATE_NODE_IDENTITY_SET message is an acknowledged message used to set the current Private Node Identity state for a subnet (see Section 4.2.46).
 * The response to a PRIVATE_NODE_IDENTITY_SET message is a PRIVATE_NODE_IDENTITY_STATUS message.
 * The PrivateNodeIdentitySetMessage class represents a message used to set the current Private Node Identity state for a subnet.
 * This message is used to acknowledge the setting of the Private Node Identity state.
 */
public class PrivateNodeIdentitySetMessage extends ConfigMessage {
    /**
     * The index of the NetKey, represented by 2 bytes.
     */
    public int netKeyIndex;
    /**
     * The new Private Node Identity state, represented by 1 byte.
     */
    public byte privateIdentity;

    /**
     * Constructs a PrivateNodeIdentitySetMessage with the specified destination address.
     *
     * @param destinationAddress The destination address of the message.
     */
    public PrivateNodeIdentitySetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Creates a simple PrivateNodeIdentitySetMessage with the given destination address, netKeyIndex, and identity.
     *
     * @param destinationAddress The destination address for the message.
     * @param netKeyIndex        The netKey index to set.
     * @param identity           The identity to set.
     * @return The created PrivateNodeIdentitySetMessage.
     */
    public static PrivateNodeIdentitySetMessage getSimple(int destinationAddress, int netKeyIndex, byte identity) {
        PrivateNodeIdentitySetMessage message = new PrivateNodeIdentitySetMessage(destinationAddress);
        message.netKeyIndex = netKeyIndex;
        message.privateIdentity = identity;
        return message;
    }

    /**
     * Returns the opcode of the message.
     *
     * @return The opcode of the message.
     */
    @Override
    public int getOpcode() {
        return Opcode.PRIVATE_NODE_IDENTITY_SET.value;
    }

    /**
     * Returns the response opcode of the message.
     *
     * @return The response opcode of the message.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.PRIVATE_NODE_IDENTITY_STATUS.value;
    }

    /**
     * Returns the parameters of the message as a byte array.
     *
     * @return The parameters of the message as a byte array.
     */
    @Override
    public byte[] getParams() {
        return ByteBuffer.allocate(3).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) netKeyIndex).put(privateIdentity).array();
    }
}