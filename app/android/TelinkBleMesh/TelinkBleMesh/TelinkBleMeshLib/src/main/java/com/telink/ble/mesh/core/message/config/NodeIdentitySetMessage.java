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
 * The Config Node Identity Set is an acknowledged message used to set the current Node Identity state for a subnet
 * <p>
 * The response to a Config Node Identity Set message is a Config Node Identity Status message.
 * {@link NodeIdentityStatusMessage}
 */

public class NodeIdentitySetMessage extends ConfigMessage {

    public int netKeyIndex;

    public byte identity;


    public static NodeIdentitySetMessage getSimple(int destinationAddress, int netKeyIndex, byte identity) {
        NodeIdentitySetMessage message = new NodeIdentitySetMessage(destinationAddress);
        message.netKeyIndex = netKeyIndex;
        message.identity = identity;
        return message;
    }

    public NodeIdentitySetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    @Override
    public int getOpcode() {
        return Opcode.NODE_ID_SET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.NODE_ID_STATUS.value;
    }

    @Override
    public byte[] getParams() {

        // netKey index lower 12 bits
        int netAppKeyIndex = (netKeyIndex & 0x0FFF);
        ByteBuffer paramsBuffer = ByteBuffer.allocate(3).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) (netKeyIndex & 0x0FFF))
                .put((byte) identity);
        return paramsBuffer.array();
    }

    public void setNetKeyIndex(int netKeyIndex) {
        this.netKeyIndex = netKeyIndex;
    }

    public void setIdentity(byte identity) {
        this.identity = identity;
    }
}
