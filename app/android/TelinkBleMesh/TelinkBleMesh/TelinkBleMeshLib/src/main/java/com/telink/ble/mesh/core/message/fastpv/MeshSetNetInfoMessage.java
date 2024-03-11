/********************************************************************************************************
 * @file MeshSetNetInfoMessage.java
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

/**
 * This class represents a Mesh Set Net Info Message, which is a specific type of Generic Message.
 * It contains a byte array to hold the network information data.
 */
public class MeshSetNetInfoMessage extends GenericMessage {
    private byte[] netInfoData;

    /**
     * Creates and returns a new instance of MeshSetNetInfoMessage with the given destination address,
     * application key index, and network information data.
     *
     * @param destinationAddress The destination address for the message.
     * @param appKeyIndex        The application key index for the message.
     * @param netInfoData        The network information data for the message.
     * @return A new instance of MeshSetNetInfoMessage.
     */
    public static MeshSetNetInfoMessage getSimple(int destinationAddress, int appKeyIndex, byte[] netInfoData) {
        MeshSetNetInfoMessage message = new MeshSetNetInfoMessage(destinationAddress, appKeyIndex);
        message.netInfoData = netInfoData;
        return message;
    }

    /**
     * Creates a new instance of MeshSetNetInfoMessage with the given destination address and application key index.
     *
     * @param destinationAddress The destination address for the message.
     * @param appKeyIndex        The application key index for the message.
     */
    public MeshSetNetInfoMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Gets the opcode value for the Mesh Set Net Info Message.
     *
     * @return The opcode value for the message.
     */
    @Override
    public int getOpcode() {
        return Opcode.VD_MESH_PROV_DATA_SET.value;
    }

    /**
     * Gets the response opcode value for the Mesh Set Net Info Message.
     *
     * @return The response opcode value for the message.
     */
    @Override
    public int getResponseOpcode() {
        return OPCODE_INVALID;
    }

    /**
     * Gets the parameters of the Mesh Set Net Info Message.
     *
     * @return The network information data.
     */
    @Override
    public byte[] getParams() {
        return netInfoData;
    }

    /**
     * Sets the network information data for the Mesh Set Net Info Message.
     *
     * @param netInfoData The network information data to set.
     */
    public void setNetInfoData(byte[] netInfoData) {
        this.netInfoData = netInfoData;
    }
}