/********************************************************************************************************
 * @file SceneRecallMessage.java
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

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * SceneRecallMessage is a class that represents a scene recall message in a smart home system.
 * It extends the GenericMessage class and is used to recall a specific scene based on the scene number.
 * This message can be sent with or without acknowledgment.
 * Acknowledgment is used to ensure that the message has been received and processed successfully.
 * The scene number, transition ID, transition time, and delay can be set for the message.
 * The isComplete flag indicates whether the message is complete or not.
 * This class provides methods to get and set the scene number, transition ID, transition time, delay, acknowledgment, and completeness of the message.
 * It also provides methods to get the response opcode and the opcode of the message.
 * The getParams method returns the parameters of the message as a byte array.
 * The SceneRecallMessage class also provides a static method to create a simple scene recall message with the given parameters.
 */
public class SceneRecallMessage extends GenericMessage {
    private int sceneNumber;
    private byte tid = 0;
    private byte transitionTime = 0;
    private byte delay = 0;
    private boolean ack = false;
    private boolean isComplete = false;

    /**
     * Creates a simple scene recall message with the given address, application key index, scene number, acknowledgment flag, and maximum response time.
     *
     * @param address     The destination address of the message.
     * @param appKeyIndex The application key index of the message.
     * @param sceneNumber The scene number to be recalled.
     * @param ack         The acknowledgment flag indicating whether the message requires acknowledgment or not.
     * @param rspMax      The maximum response time for the message.
     * @return The created SceneRecallMessage object.
     */
    public static SceneRecallMessage getSimple(int address, int appKeyIndex, int sceneNumber, boolean ack, int rspMax) {
        SceneRecallMessage message = new SceneRecallMessage(address, appKeyIndex);
        message.sceneNumber = sceneNumber;
        message.ack = ack;
        message.setResponseMax(rspMax);
        return message;
    }

    /**
     * Constructs a SceneRecallMessage object with the given destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index of the message.
     */
    public SceneRecallMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
        setTidPosition(2);
    }

    /**
     * Returns the response opcode of the message.
     * If the acknowledgment flag is true, the response opcode is SCENE_STATUS.
     * Otherwise, it is the super class's response opcode.
     *
     * @return The response opcode of the message.
     */
    @Override
    public int getResponseOpcode() {
        return ack ? Opcode.SCENE_STATUS.value : super.getResponseOpcode();
    }

    /**
     * Returns the opcode of the message.
     * If the acknowledgment flag is true, the opcode is SCENE_RECALL.
     * Otherwise, it is SCENE_RECALL_NOACK.
     *
     * @return The opcode of the message.
     */
    @Override
    public int getOpcode() {
        return ack ? Opcode.SCENE_RECALL.value : Opcode.SCENE_RECALL_NOACK.value;
    }

    /**
     * Returns the parameters of the message as a byte array.
     * If the message is complete, the byte array includes the scene number, tid, transition time, and delay.
     * Otherwise, it only includes the scene number and tid.
     *
     * @return The parameters of the message as a byte array.
     */
    @Override
    public byte[] getParams() {
        return isComplete ?
                ByteBuffer.allocate(5).order(ByteOrder.LITTLE_ENDIAN)
                        .putShort((short) sceneNumber)
                        .put(tid)
                        .put(transitionTime)
                        .put(delay).array()
                :
                ByteBuffer.allocate(3).order(ByteOrder.LITTLE_ENDIAN)
                        .putShort((short) sceneNumber)
                        .put(tid).array();
    }

    /**
     * Sets the scene number of the message.
     *
     * @param sceneNumber The scene number to be set.
     */
    public void setSceneNumber(int sceneNumber) {
        this.sceneNumber = sceneNumber;
    }

    /**
     * Sets the transition ID of the message.
     *
     * @param tid The transition ID to be set.
     */
    public void setTid(byte tid) {
        this.tid = tid;
    }

    /**
     * Sets the transition time of the message.
     *
     * @param transitionTime The transition time to be set.
     */
    public void setTransitionTime(byte transitionTime) {
        this.transitionTime = transitionTime;
    }

    /**
     * Sets the delay of the message.
     *
     * @param delay The delay to be set.
     */
    public void setDelay(byte delay) {
        this.delay = delay;
    }

    /**
     * Sets the acknowledgment flag of the message.
     *
     * @param ack The acknowledgment flag to be set.
     */
    public void setAck(boolean ack) {
        this.ack = ack;
    }

    /**
     * Sets the completeness flag of the message.
     *
     * @param complete The completeness flag to be set.
     */
    public void setComplete(boolean complete) {
        isComplete = complete;
    }
}