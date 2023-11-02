/********************************************************************************************************
 * @file SceneStoreMessage.java
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
package com.telink.ble.mesh.core.message.scene;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.generic.GenericMessage;

/**
 * This class represents a message for storing a scene in a scene store.
 * It extends the GenericMessage class.
 * <p>
 * The scene store message contains the following properties:
 * - sceneNumber: The ID of the scene to be stored.
 * - ack: A boolean value indicating whether an acknowledgement is requested for the message.
 * <p>
 * The SceneStoreMessage class provides methods for creating a simple scene store message, setting the scene number and acknowledgement status, and getting the response opcode and parameters.
 * <p>
 * Example usage:
 * SceneStoreMessage message = SceneStoreMessage.getSimple(address, appKeyIndex, sceneNumber, ack, rspMax);
 * message.setSceneNumber(sceneNumber);
 * message.setAck(ack);
 */
public class SceneStoreMessage extends GenericMessage {
    private int sceneNumber;
    private boolean ack;

    /**
     * Creates a simple scene store message with the specified address, application key index, scene number, acknowledgement status, and response maximum.
     *
     * @param address     The destination address of the message.
     * @param appKeyIndex The application key index of the message.
     * @param sceneNumber The ID of the scene to be stored.
     * @param ack         A boolean value indicating whether an acknowledgement is requested for the message.
     * @param rspMax      The maximum number of response messages allowed for the message.
     * @return The created SceneStoreMessage object.
     */
    public static SceneStoreMessage getSimple(int address, int appKeyIndex, int sceneNumber, boolean ack, int rspMax) {
        SceneStoreMessage message = new SceneStoreMessage(address, appKeyIndex);
        message.sceneNumber = sceneNumber;
        message.ack = ack;
        message.setResponseMax(rspMax);
        return message;
    }

    /**
     * Constructs a SceneStoreMessage object with the specified destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index of the message.
     */
    public SceneStoreMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Returns the response opcode for the message.
     * If an acknowledgement is requested, the opcode is SCENE_REG_STATUS; otherwise, it is the super class's response opcode.
     *
     * @return The response opcode for the message.
     */
    @Override
    public int getResponseOpcode() {
        return ack ? Opcode.SCENE_REG_STATUS.value : super.getResponseOpcode();
    }

    /**
     * Returns the opcode for the message.
     * If an acknowledgement is requested, the opcode is SCENE_STORE; otherwise, it is SCENE_STORE_NOACK.
     *
     * @return The opcode for the message.
     */
    @Override
    public int getOpcode() {
        return ack ? Opcode.SCENE_STORE.value : Opcode.SCENE_STORE_NOACK.value;
    }

    /**
     * Returns the parameters for the message.
     * The parameters consist of the scene number in little-endian format.
     *
     * @return The parameters for the message.
     */
    @Override
    public byte[] getParams() {
        return new byte[]{(byte) sceneNumber, (byte) (sceneNumber >> 8)};
    }

    /**
     * Sets the scene number for the message.
     *
     * @param sceneNumber The ID of the scene to be stored.
     */
    public void setSceneNumber(int sceneNumber) {
        this.sceneNumber = sceneNumber;
    }

    /**
     * Sets the acknowledgement status for the message.
     *
     * @param ack A boolean value indicating whether an acknowledgement is requested for the message.
     */
    public void setAck(boolean ack) {
        this.ack = ack;
    }
}