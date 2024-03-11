/********************************************************************************************************
 * @file NodeIdentityGetMessage.java
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
 * The Config Node Identity Get is an acknowledged message used to get the current Node Identity state for a subnet
 * The NodeIdentityGetMessage class represents an acknowledged message used to retrieve the current Node Identity state for a specific subnet.
 */
public class NodeIdentityGetMessage extends ConfigMessage {
    public int netKeyIndex;

    /**
     * Constructs a new NodeIdentityGetMessage with the specified destination address and network key index.
     *
     * @param destinationAddress The destination address for the message.
     * @param netKeyIndex        The network key index.
     */
    public NodeIdentityGetMessage(int destinationAddress, int netKeyIndex) {
        super(destinationAddress);
        this.netKeyIndex = netKeyIndex;
    }

    /**
     * Returns the opcode value for the NodeIdentityGetMessage.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.NODE_ID_GET.value;
    }

    /**
     * Returns the response opcode value for the NodeIdentityGetMessage.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.NODE_ID_STATUS.value;
    }

    /**
     * Returns the parameters of the NodeIdentityGetMessage as a byte array.
     *
     * @return The parameters as a byte array.
     */
    @Override
    public byte[] getParams() {
        ByteBuffer paramsBuffer = ByteBuffer.allocate(2).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) (netKeyIndex & 0x0FFF));
        return paramsBuffer.array();
    }
}