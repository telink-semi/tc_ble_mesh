/********************************************************************************************************
 * @file BridgingTableRemoveMessage.java
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

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.Opcode;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Created by kee on 2021/1/14.
 */
public class BridgingTableRemoveMessage extends ConfigMessage {


    /**
     * NetKey Index of the first subnet
     * 12 bits
     */
    public int netKeyIndex1;

    /**
     * NetKey Index of the second subnet
     * 12 bits
     */
    public int netKeyIndex2;

    /**
     * Address of the node in the first subnet
     * 16 bits
     */
    public int address1;

    /**
     * Address of the node in the second subnet
     * 16 bits
     */
    public int address2;


    public BridgingTableRemoveMessage(int destinationAddress) {
        super(destinationAddress);
    }

    @Override
    public int getOpcode() {
        return Opcode.BRIDGING_TABLE_REMOVE.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.BRIDGING_TABLE_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        int netKeyIndexes = (netKeyIndex1 & 0x0FFF) | ((netKeyIndex2 & 0x0FFF) << 12);
        byte[] indexesBuf = MeshUtils.integer2Bytes(netKeyIndexes, 3, ByteOrder.LITTLE_ENDIAN);
        return ByteBuffer.allocate(7).order(ByteOrder.LITTLE_ENDIAN)
                .put(indexesBuf)
                .putShort((short) address1)
                .putShort((short) address2).array();
    }


}
