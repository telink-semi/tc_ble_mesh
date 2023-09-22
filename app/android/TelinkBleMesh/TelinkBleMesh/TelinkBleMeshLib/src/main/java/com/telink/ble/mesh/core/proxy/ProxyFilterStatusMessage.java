/********************************************************************************************************
 * @file ProxyFilterStatusMessage.java
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
package com.telink.ble.mesh.core.proxy;

import com.telink.ble.mesh.core.MeshUtils;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;


/**
 * This class represents a Proxy Filter Status Message, which is a type of Proxy Configuration Message.
 */
public class ProxyFilterStatusMessage extends ProxyConfigurationMessage {

    private static final int DATA_LEN = 4;

    /**
     * The filter type (white list or black list).
     */
    private byte filterType;

    /**
     * The number of addresses in the proxy filter list.
     */
    private int listSize;

    /**
     * Constructs a new ProxyFilterStatusMessage instance.
     */
    public ProxyFilterStatusMessage() {
        // Default constructor
    }

    /**
     * Converts a byte array to a ProxyFilterStatusMessage object.
     *
     * @param data The byte array containing the message data.
     * @return A ProxyFilterStatusMessage object if the byte array is valid, null otherwise.
     */
    public static ProxyFilterStatusMessage fromBytes(byte[] data) {
        if (data.length != DATA_LEN) {
            return null;
        }
        ProxyFilterStatusMessage instance = new ProxyFilterStatusMessage();
        int index = 0;
        byte opcode = data[index++];
        if (opcode != OPCODE_FILTER_STATUS) {
            return null;
        }
        instance.filterType = data[index++];
        instance.listSize = MeshUtils.bytes2Integer(data, index, 2, ByteOrder.BIG_ENDIAN);
        return instance;
    }

    /**
     * Gets the opcode of the message.
     *
     * @return The opcode of the message.
     */
    @Override
    public byte getOpcode() {
        return OPCODE_FILTER_STATUS;
    }

    /**
     * Converts the ProxyFilterStatusMessage object to a byte array.
     *
     * @return A byte array representing the ProxyFilterStatusMessage object.
     */
    @Override
    public byte[] toByteArray() {
        return ByteBuffer.allocate(DATA_LEN)
                .order(ByteOrder.BIG_ENDIAN)
                .put(filterType)
                .putShort((short) listSize)
                .array();
    }

    /**
     * Gets the filter type.
     *
     * @return The filter type.
     */
    public byte getFilterType() {
        return filterType;
    }

    /**
     * Gets the number of addresses in the proxy filter list.
     *
     * @return The number of addresses in the proxy filter list.
     */
    public int getListSize() {
        return listSize;
    }
}