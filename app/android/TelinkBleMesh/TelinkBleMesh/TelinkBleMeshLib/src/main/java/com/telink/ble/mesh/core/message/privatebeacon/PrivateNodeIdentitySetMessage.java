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

import com.telink.ble.mesh.core.message.config.ConfigMessage;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * A PRIVATE_NODE_IDENTITY_SET message is an acknowledged message used to set the current Private Node Identity state for a subnet (see Section 4.2.46).
 * The response to a PRIVATE_NODE_IDENTITY_SET message is a PRIVATE_NODE_IDENTITY_STATUS message.
 */

public class PrivateNodeIdentitySetMessage extends ConfigMessage {

    /**
     * 2 bytes
     * Index of the NetKey
     */
    public int netKeyIndex;

    /**
     * 1 byte
     * New Private Node Identity state
     */
    public byte privateIdentity;


    public PrivateNodeIdentitySetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    @Override
    public int getOpcode() {
        return OPCODE_INVALID;
    }

    @Override
    public int getResponseOpcode() {
        return OPCODE_INVALID;
    }

    @Override
    public byte[] getParams() {
        return
                ByteBuffer.allocate(3).order(ByteOrder.LITTLE_ENDIAN)
                        .putShort((short) netKeyIndex).put(privateIdentity).array();
    }


}
