/********************************************************************************************************
 * @file DeltaSetMessage.java
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
 * DeltaSetMessage is a class that represents a message for setting the delta level of a device.
 * It extends the GenericMessage class.
 * The delta level is the difference in the current state of the device from the desired state.
 * The message contains the delta level, transaction ID (tid), transition time, and delay.
 * It also has a flag to indicate whether an acknowledgement is required.
 * The isComplete flag indicates whether the message is complete or not.
 * This class provides methods to create a simple DeltaSetMessage object, set the delta level and other parameters,
 * and get the response opcode and opcode for the message.
 * The getParams method returns the parameters of the message as a byte array.
 */
public class DeltaSetMessage extends GenericMessage {
    private int deltaLevel;
    private byte tid;
    private byte transitionTime;
    private byte delay;
    private boolean ack;
    private boolean isComplete = false;

    /**
     * Creates a simple DeltaSetMessage object with the given destination address, application key index,
     * delta level, acknowledgement flag, and maximum response time.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index of the message.
     * @param deltaLevel         The delta level to be set.
     * @param ack                The acknowledgement flag.
     * @param rspMax             The maximum response time.
     * @return The created DeltaSetMessage object.
     */
    public static DeltaSetMessage getSimple(int destinationAddress, int appKeyIndex, int deltaLevel, boolean ack, int rspMax) {
        DeltaSetMessage deltaSetMessage = new DeltaSetMessage(destinationAddress, appKeyIndex);
        deltaSetMessage.deltaLevel = deltaLevel;
        deltaSetMessage.transitionTime = 0;
        deltaSetMessage.delay = 0;
        deltaSetMessage.ack = ack;
        deltaSetMessage.responseMax = rspMax;
        return deltaSetMessage;
    }

    /**
     * Constructs a DeltaSetMessage object with the given destination address and application key index.
     * Sets the transaction ID position to 4.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index of the message.
     */
    public DeltaSetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
        setTidPosition(4);
    }

    /**
     * Returns the response opcode for the message.
     * If an acknowledgement is required, it returns the opcode for G_LEVEL_STATUS.
     * Otherwise, it returns an invalid opcode.
     *
     * @return The response opcode for the message.
     */
    @Override
    public int getResponseOpcode() {
        return ack ? Opcode.G_LEVEL_STATUS.value : OPCODE_INVALID;
    }

    /**
     * Returns the opcode for the message.
     * If an acknowledgement is required, it returns the opcode for G_DELTA_SET.
     * Otherwise, it returns the opcode for G_DELTA_SET_NOACK.
     *
     * @return The opcode for the message.
     */
    @Override
    public int getOpcode() {
        return ack ? Opcode.G_DELTA_SET.value : Opcode.G_DELTA_SET_NOACK.value;
    }

    /**
     * Returns the parameters of the message as a byte array.
     * If the message is complete, it returns a byte array with the delta level, tid, transition time, and delay.
     * Otherwise, it returns a byte array with the delta level and tid.
     *
     * @return The parameters of the message as a byte array.
     */
    @Override
    public byte[] getParams() {
        return isComplete ?
                ByteBuffer.allocate(7).order(ByteOrder.LITTLE_ENDIAN).putInt(deltaLevel)
                        .put(tid).put(transitionTime).put(delay).array()
                :
                ByteBuffer.allocate(5).order(ByteOrder.LITTLE_ENDIAN).putInt(deltaLevel)
                        .put(tid).array();
    }

    /**
     * Sets the delta level of the message.
     *
     * @param deltaLevel The delta level to be set.
     */
    public void setDeltaLevel(int deltaLevel) {
        this.deltaLevel = deltaLevel;
    }

    /**
     * Sets the transaction ID (tid) of the message.
     *
     * @param tid The transaction ID to be set.
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
     * Sets the acknowledgement flag of the message.
     *
     * @param ack The acknowledgement flag to be set.
     */
    public void setAck(boolean ack) {
        this.ack = ack;
    }

    /**
     * Sets the complete flag of the message.
     *
     * @param complete The complete flag to be set.
     */
    public void setComplete(boolean complete) {
        isComplete = complete;
    }
}