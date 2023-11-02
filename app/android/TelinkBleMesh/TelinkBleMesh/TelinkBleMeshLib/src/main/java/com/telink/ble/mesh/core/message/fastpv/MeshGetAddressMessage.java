/********************************************************************************************************
 * @file MeshGetAddressMessage.java
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
package com.telink.ble.mesh.core.message.fastpv;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.generic.GenericMessage;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * This class represents a MeshGetAddressMessage, which is a specific type of GenericMessage.
 * It is used to request the address of a device in a mesh network.
 */
public class MeshGetAddressMessage extends GenericMessage {
    private int pid;
    private int address = 0;

    /**
     * Creates a new MeshGetAddressMessage with the specified destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index of the message.
     */
    public MeshGetAddressMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Creates a simple MeshGetAddressMessage with the specified parameters.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index of the message.
     * @param rspMax             The maximum number of responses expected.
     * @param pid                The process identifier.
     * @return The created MeshGetAddressMessage.
     */
    public static MeshGetAddressMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax, int pid) {
        MeshGetAddressMessage message = new MeshGetAddressMessage(destinationAddress, appKeyIndex);
        message.setRetryCnt(0);
        message.setResponseMax(rspMax);
        message.pid = pid;
        return message;
    }

    /**
     * Gets the opcode of the message.
     *
     * @return The opcode.
     */
    @Override
    public int getOpcode() {
        return Opcode.VD_MESH_ADDR_GET.value;
    }

    /**
     * Gets the response opcode of the message.
     *
     * @return The response opcode.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.VD_MESH_ADDR_GET_STS.value;
    }

    /**
     * Gets the parameters of the message as a byte array.
     *
     * @return The parameters as a byte array.
     */
    @Override
    public byte[] getParams() {
        if (address == 0) {
            return MeshUtils.integer2Bytes(this.pid, 2, ByteOrder.LITTLE_ENDIAN);
        } else {
            return ByteBuffer.allocate(4).order(ByteOrder.LITTLE_ENDIAN)
                    .putShort((short) pid)
                    .putShort((short) address)
                    .array();
        }
    }

    /**
     * Sets the process identifier of the message.
     *
     * @param pid The process identifier.
     */
    public void setPid(int pid) {
        this.pid = pid;
    }

    /**
     * Sets the address of the message.
     *
     * @param address The address.
     */
    public void setAddress(int address) {
        this.address = address;
    }
}
