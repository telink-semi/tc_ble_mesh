/********************************************************************************************************
 * @file SceneDeleteMessage.java
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
 * This class represents a scene delete message.
 * It extends the GenericMessage class.
 * The scene delete message is used to delete a specific scene from the scene store.
 * It contains the scene id and a flag indicating whether an acknowledgement is required.
 */
public class SceneDeleteMessage extends GenericMessage {
    // scene id
    private int sceneNumber;
    private boolean ack;

    /**
     * Creates a simple scene delete message with the given parameters.
     *
     * @param address     The destination address of the message.
     * @param appKeyIndex The application key index of the message.
     * @param sceneNumber The scene id to be deleted.
     * @param ack         Flag indicating whether an acknowledgement is required.
     * @param rspMax      The maximum number of response messages allowed.
     * @return The created scene delete message.
     */
    public static SceneDeleteMessage getSimple(int address, int appKeyIndex, int sceneNumber, boolean ack, int rspMax) {
        SceneDeleteMessage message = new SceneDeleteMessage(address, appKeyIndex);
        message.sceneNumber = sceneNumber;
        message.ack = ack;
        message.setResponseMax(rspMax);
        return message;
    }

    /**
     * Constructs a scene delete message with the given destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index of the message.
     */
    public SceneDeleteMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Gets the response opcode of the message.
     * If an acknowledgement is required, it returns the opcode for scene registration status.
     * Otherwise, it returns the super class' response opcode.
     *
     * @return The response opcode.
     */
    @Override
    public int getResponseOpcode() {
        return ack ? Opcode.SCENE_REG_STATUS.value : super.getResponseOpcode();
    }

    /**
     * Gets the opcode of the message.
     * If an acknowledgement is required, it returns the opcode for scene delete.
     * Otherwise, it returns the opcode for scene delete with no acknowledgement.
     *
     * @return The opcode.
     */
    @Override
    public int getOpcode() {
        return ack ? Opcode.SCENE_DEL.value : Opcode.SCENE_DEL_NOACK.value;
    }

    /**
     * Gets the parameters of the message.
     * The parameters consist of the scene id.
     *
     * @return The parameters.
     */
    @Override
    public byte[] getParams() {
        return new byte[]{(byte) sceneNumber, (byte) (sceneNumber >> 8)};
    }

    /**
     * Sets the scene id of the message.
     *
     * @param sceneNumber The scene id to be set.
     */
    public void setSceneNumber(int sceneNumber) {
        this.sceneNumber = sceneNumber;
    }

    /**
     * Sets the acknowledgement flag of the message.
     *
     * @param ack The acknowledgement flag to be set.
     */
    public void setAck(boolean ack) {
        this.ack = ack;
    }
}