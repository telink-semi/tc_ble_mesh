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


public class ForwardingTableDeleteMessage extends ConfigMessage {

    /**
     * NetKey Index of the NetKey used in the subnet
     * 12 bits
     */
    public int netKeyIndex;

    /**
     * Primary element address of the Path Origin
     * 16 bits
     */
    public int pathOrigin;

    /**
     * Destination address
     * 16 bits
     */
    public int destination;


    public static ForwardingTableDeleteMessage getSimple(int destinationAddress, int netKeyIndex) {
        ForwardingTableDeleteMessage message = new ForwardingTableDeleteMessage(destinationAddress);
        message.setResponseMax(1);
        message.netKeyIndex = netKeyIndex;
        return message;
    }

    public ForwardingTableDeleteMessage(int destinationAddress) {
        super(destinationAddress);
    }

    @Override
    public int getOpcode() {
        return Opcode.FORWARDING_TABLE_DELETE.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.FORWARDING_TABLE_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        return ByteBuffer.allocate(6).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) netKeyIndex)
                .putShort((short) pathOrigin)
                .putShort((short) destination).array();
    }
}
