/********************************************************************************************************
 * @file MoveSetMessage.java
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
package com.telink.ble.mesh.core.message.generic;

import com.telink.ble.mesh.core.message.Opcode;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Created by kee on 2019/8/20.
 */

/**
 * This class represents a Move Set Message, which is a type of Generic Message used in a communication protocol.
 * It extends the GenericMessage class and includes additional fields and methods specific to the Move Set Message.
 * The Move Set Message is used to set the delta level, tid, transition time, and delay for a particular operation.
 * The message can be sent with or without acknowledgement.
 * The isComplete field indicates whether the message is complete or not.
 * The class provides methods to set and get the delta level, tid, transition time, delay, acknowledgement, and completeness of the message.
 * It also overrides methods from the GenericMessage class to provide the opcode and parameters specific to the Move Set Message.
 * The class is designed to be used in a communication system where messages are sent between devices.
 */

public class MoveSetMessage extends GenericMessage {
    private int deltaLevel;
    private byte tid;
    private byte transitionTime;
    private byte delay;
    private boolean ack;
    private boolean isComplete = false;

    /**
     * Constructs a Move Set Message with the specified destination address and application key index.
     *
     * @param destinationAddress The destination address for the message.
     * @param appKeyIndex        The application key index for the message.
     */
    public MoveSetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
        setTidPosition(2);
    }

    /**
     * Returns the opcode of the Move Set Message.
     * If acknowledgement is enabled, it returns the opcode for an acknowledged message.
     * Otherwise, it returns the opcode for a non-acknowledged message.
     *
     * @return The opcode of the Move Set Message.
     */
    @Override
    public int getOpcode() {
        return ack ? Opcode.G_MOVE_SET.value : Opcode.G_MOVE_SET_NOACK.value;
    }

    /**
     * Returns the parameters of the Move Set Message as a byte array.
     * If the message is complete, it includes the delta level, tid, transition time, and delay in the byte array.
     * Otherwise, it only includes the delta level and tid.
     *
     * @return The parameters of the Move Set Message as a byte array.
     */
    @Override
    public byte[] getParams() {
        return isComplete ?
                ByteBuffer.allocate(5)
                        .order(ByteOrder.LITTLE_ENDIAN)
                        .putShort((short) deltaLevel)
                        .put(tid).put(transitionTime).put(delay).array()
                :
                ByteBuffer.allocate(3)
                        .order(ByteOrder.LITTLE_ENDIAN)
                        .putShort((short) deltaLevel)
                        .put(tid).array();
    }

    /**
     * Sets the delta level for the Move Set Message.
     *
     * @param deltaLevel The delta level to be set.
     */
    public void setDeltaLevel(int deltaLevel) {
        this.deltaLevel = deltaLevel;
    }

    /**
     * Sets the tid for the Move Set Message.
     *
     * @param tid The tid to be set.
     */
    public void setTid(byte tid) {
        this.tid = tid;
    }

    /**
     * Sets the transition time for the Move Set Message.
     *
     * @param transitionTime The transition time to be set.
     */
    public void setTransitionTime(byte transitionTime) {
        this.transitionTime = transitionTime;
    }

    /**
     * Sets the delay for the Move Set Message.
     *
     * @param delay The delay to be set.
     */
    public void setDelay(byte delay) {
        this.delay = delay;
    }

    /**
     * Sets the acknowledgement flag for the Move Set Message.
     *
     * @param ack The acknowledgement flag to be set.
     */
    public void setAck(boolean ack) {
        this.ack = ack;
    }

    /**
     * Sets the completeness flag for the Move Set Message.
     *
     * @param complete The completeness flag to be set.
     */
    public void setComplete(boolean complete) {
        isComplete = complete;
    }
}