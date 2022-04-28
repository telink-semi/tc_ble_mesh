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


public class ForwardingTableAddMessage extends ConfigMessage {

    /**
     * NetKey Index of the NetKey used in the subnet
     * 12 bits
     */
    public int netKeyIndex;

    /**
     * 2 bits
     */
    public int Prohibited;

    /**
     * Indicates whether or not the destination of the path is a unicast address
     * if 0, multicastDestination should be present
     * if 1, pathTargetUnicastAddrRange should be present
     * 1 bit
     */
    public int unicastDestinationFlag;

    /**
     * Indicates whether or not the backward path has been validated
     * 1 bit
     */
    public int backwardPathValidatedFlag;

    /**
     * Unicast address range of the Path Origin
     * variable (16 or 24)
     */
    public byte[] pathOriginUnicastAddrRange;

    /**
     * Unicast address range of the Path Target
     * variable (16 or 24)
     */
    public byte[] pathTargetUnicastAddrRange;

    /**
     * Multicast destination address
     * 16 bits
     */
    public int multicastDestination = 0;

    /**
     * Index of the bearer toward the Path Origin
     * 16 bits
     */
    public int bearerTowardPathOrigin = 0;

    /**
     * Index of the bearer toward the Path Target
     * 16 bits
     */
    public int bearerTowardPathTarget = 0;


    public static ForwardingTableAddMessage getSimple(int destinationAddress, int netKeyIndex) {
        ForwardingTableAddMessage message = new ForwardingTableAddMessage(destinationAddress);
        message.setResponseMax(1);
        message.netKeyIndex = netKeyIndex;
        return message;
    }

    public ForwardingTableAddMessage(int destinationAddress) {
        super(destinationAddress);
    }

    @Override
    public int getOpcode() {
        return Opcode.FORWARDING_TABLE_ADD.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.FORWARDING_TABLE_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        // 6 = (NetKeyIndex + flags) + Bearer_Toward_Path_Origin + Bearer_Toward_Path_Target
        // 2 = Multicast_Destination
        boolean isUnicast = unicastDestinationFlag == 1;
        int len = 6 + pathOriginUnicastAddrRange.length +
                (isUnicast ? pathTargetUnicastAddrRange.length : 2);

        ByteBuffer bf = ByteBuffer.allocate(len).order(ByteOrder.LITTLE_ENDIAN);
        int idxAndFlags = (backwardPathValidatedFlag << 15) | (unicastDestinationFlag << 14) | netKeyIndex;
//                (netKeyIndex << 4) | (unicastDestinationFlag << 1) | backwardPathValidatedFlag;
        bf.putShort((short) idxAndFlags).put(pathOriginUnicastAddrRange);
        if (isUnicast) {
            bf.put(pathTargetUnicastAddrRange);
        } else {
            bf.putShort((short) multicastDestination);
        }
        bf.putShort((short) bearerTowardPathOrigin).putShort((short) bearerTowardPathTarget);
        return bf.array();
    }
}
