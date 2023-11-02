/********************************************************************************************************
 * @file MeshConfirmRequestMessage.java
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
 * This class represents a Mesh Confirm Request message, which is used to confirm the provisioning of a Mesh network.
 * It extends the GenericMessage class.
 */
public class MeshConfirmRequestMessage extends GenericMessage {

    /**
     * Creates a simple Mesh Confirm Request message with the given destination address and application key index.
     * The response maximum is set to 0 and the retry count is set to 1.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index of the message.
     * @return The created MeshConfirmRequestMessage object.
     */
    public static MeshConfirmRequestMessage getSimple(int destinationAddress, int appKeyIndex) {
        MeshConfirmRequestMessage message = new MeshConfirmRequestMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(0);
        message.setRetryCnt(1);
        return message;
    }

    /**
     * Constructs a MeshConfirmRequestMessage object with the given destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index of the message.
     */
    public MeshConfirmRequestMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Returns the opcode value for the Mesh Confirm Request message.
     *
     * @return The opcode value for the Mesh Confirm Request message.
     */
    @Override
    public int getOpcode() {
        return Opcode.VD_MESH_PROV_CONFIRM.value;
    }

    /**
     * Returns the response opcode value for the Mesh Confirm Request message.
     *
     * @return The response opcode value for the Mesh Confirm Request message.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.VD_MESH_PROV_CONFIRM_STS.value;
    }

    /**
     * Returns the parameters of the Mesh Confirm Request message.
     *
     * @return The parameters of the Mesh Confirm Request message.
     */
    @Override
    public byte[] getParams() {
        return null;
    }
}