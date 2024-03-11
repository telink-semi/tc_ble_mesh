/********************************************************************************************************
 * @file MeshSetAddressMessage.java
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

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.generic.GenericMessage;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * This class represents a MeshSetAddressMessage, which is a specific type of GenericMessage.
 * It is used to set the address of a mesh device in a mesh network.
 */
public class MeshSetAddressMessage extends GenericMessage {
    private byte[] mac; // The MAC address of the device
    private int newMeshAddress; // The new mesh address to be set

    /**
     * Creates a new MeshSetAddressMessage with the given destination address and application key index.
     *
     * @param destinationAddress The destination address of the message
     * @param appKeyIndex        The application key index of the message
     */
    public MeshSetAddressMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Creates a simple MeshSetAddressMessage with the given parameters.
     *
     * @param destinationAddress The destination address of the message
     * @param appKeyIndex        The application key index of the message
     * @param mac                The MAC address of the device
     * @param newMeshAddress     The new mesh address to be set
     * @return The created MeshSetAddressMessage
     */
    public static MeshSetAddressMessage getSimple(int destinationAddress, int appKeyIndex, byte[] mac, int newMeshAddress) {
        MeshSetAddressMessage message = new MeshSetAddressMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        message.mac = mac;
        message.newMeshAddress = newMeshAddress;
        return message;
    }

    /**
     * Gets the opcode of the message.
     *
     * @return The opcode of the message
     */
    @Override
    public int getOpcode() {
        return Opcode.VD_MESH_ADDR_SET.value;
    }

    /**
     * Gets the response opcode of the message.
     *
     * @return The response opcode of the message
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.VD_MESH_ADDR_SET_STS.value;
    }

    /**
     * Gets the parameters of the message.
     *
     * @return The parameters of the message as a byte array
     */
    @Override
    public byte[] getParams() {
        return ByteBuffer.allocate(8).order(ByteOrder.LITTLE_ENDIAN).put(mac).putShort((short) newMeshAddress).array();
    }

    /**
     * Sets the MAC address of the device.
     *
     * @param mac The MAC address to be set
     */
    public void setMac(byte[] mac) {
        this.mac = mac;
    }

    /**
     * Sets the new mesh address to be set.
     *
     * @param newMeshAddress The new mesh address to be set
     */
    public void setNewMeshAddress(int newMeshAddress) {
        this.newMeshAddress = newMeshAddress;
    }
}