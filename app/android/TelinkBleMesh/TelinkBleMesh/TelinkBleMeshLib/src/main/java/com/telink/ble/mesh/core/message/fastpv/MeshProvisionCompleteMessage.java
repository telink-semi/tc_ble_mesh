/********************************************************************************************************
 * @file MeshProvisionCompleteMessage.java
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

import java.nio.ByteOrder;

/**
 * This class represents a message indicating the completion of mesh provisioning.
 * It extends the GenericMessage class.
 * The delay field represents the delay in milliseconds and is stored as 2 bytes.
 */
public class MeshProvisionCompleteMessage extends GenericMessage {
    private int delay;

    /**
     * Constructs a new MeshProvisionCompleteMessage object with the given destination address and app key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The app key index of the message.
     */
    public MeshProvisionCompleteMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Returns the opcode value for this message.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.VD_MESH_PROV_COMPLETE.value;
    }

    /**
     * Returns the response opcode value for this message.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return OPCODE_INVALID;
    }

    /**
     * Returns the byte array representation of the delay field in little-endian format.
     *
     * @return The byte array representation of the delay field.
     */
    @Override
    public byte[] getParams() {
        return MeshUtils.integer2Bytes(delay, 2, ByteOrder.LITTLE_ENDIAN);
    }

    /**
     * Sets the delay value for this message.
     *
     * @param delay The delay value in milliseconds.
     */
    public void setDelay(int delay) {
        this.delay = delay;
    }

    /**
     * Returns a new instance of MeshProvisionCompleteMessage with the given destination address, app key index, and delay.
     *
     * @param destinationAddress The destination address of the new message.
     * @param appKeyIndex        The app key index of the new message.
     * @param delay              The delay value in milliseconds.
     * @return The new MeshProvisionCompleteMessage instance.
     */
    public static MeshProvisionCompleteMessage getSimple(int destinationAddress, int appKeyIndex, int delay) {
        MeshProvisionCompleteMessage message = new MeshProvisionCompleteMessage(destinationAddress, appKeyIndex);
        message.delay = delay;
        return message;
    }
}