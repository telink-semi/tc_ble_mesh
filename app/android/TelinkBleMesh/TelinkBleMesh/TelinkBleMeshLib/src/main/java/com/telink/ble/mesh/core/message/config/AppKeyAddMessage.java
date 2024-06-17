/********************************************************************************************************
 * @file AppKeyAddMessage.java
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
 * The Config AppKey Add is an acknowledged message used to add an AppKey to the AppKey List on a node
 * and bind it to the NetKey identified by NetKeyIndex.
 * <p>
 * The added AppKey can be used by the node only as a pair with the specified NetKey.
 * <p>
 * The AppKey is used to authenticate and decrypt messages it receives, as well as authenticate and encrypt messages it sends.
 * <p>
 * The response to a Config AppKey Add message is a Config AppKey Status message {@link AppKeyStatusMessage}.
 */

public class AppKeyAddMessage extends ConfigMessage {


    private int netKeyIndex;

    private int appKeyIndex;

    private byte[] appKey;

    /**
     * Creates a new AppKeyAddMessage with the specified destination address.
     *
     * @param destinationAddress The destination address of the message.
     */
    public AppKeyAddMessage(int destinationAddress) {
        super(destinationAddress);
    }

    public void setNetKeyIndex(int netKeyIndex) {
        this.netKeyIndex = netKeyIndex;
    }

    public void setAppKeyIndex(int appKeyIndex) {
        this.appKeyIndex = appKeyIndex;
    }


    public void setAppKey(byte[] appKey) {
        this.appKey = appKey;
    }

    /**
     * Gets the opcode of the message.
     *
     * @return The opcode of the message.
     */
    @Override
    public int getOpcode() {
        return Opcode.APPKEY_ADD.value;
    }

    /**
     * Gets the opcode of the response message.
     *
     * @return The opcode of the response message.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.APPKEY_STATUS.value;
    }

    /**
     * Gets the parameters of the message.
     *
     * @return The parameters of the message.
     */
    @Override
    public byte[] getParams() {

        // netKey index lower 12 bits
        // appKey index higher 12 bits

        int netAppKeyIndex = (netKeyIndex & 0x0FFF) | ((appKeyIndex & 0x0FFF) << 12);
//        int netAppKeyIndex = ((netKeyIndex & 0x0FFF) << 12) | ((appKeyIndex & 0x0FFF));
        byte[] indexBuf = MeshUtils.integer2Bytes(netAppKeyIndex, 3, ByteOrder.LITTLE_ENDIAN);

        ByteBuffer paramsBuffer = ByteBuffer.allocate(3 + 16).order(ByteOrder.LITTLE_ENDIAN)
                .put(indexBuf)
                .put(appKey);
        return paramsBuffer.array();
    }


}
