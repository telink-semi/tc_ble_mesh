/********************************************************************************************************
 * @file OnOffSetMessage.java
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


/**
 * include on off set and on off set no ack
 * This class represents an On/Off Set Message, which is a type of Generic Message.
 * It contains methods to set the parameters of the message and generate the message payload.
 * The message payload includes the on/off value, transaction ID, transition time, and delay.
 * It also has methods to determine the response opcode and the opcode of the message.
 * The message can be created with or without acknowledgement, and can be marked as complete.
 */
public class OnOffSetMessage extends GenericMessage {
    public static final byte ON = 1;
    public static final byte OFF = 0;
    // 1: on, 0: off
    private byte onOff;
    private byte tid = 0;
    private byte transitionTime = 0;
    private byte delay = 0;
    private boolean ack = false;
    private boolean isComplete = false;

    /**
     * Creates a simple On/Off Set Message with the given parameters.
     *
     * @param address     The destination address of the message.
     * @param appKeyIndex The application key index of the message.
     * @param onOff       The on/off value of the message.
     * @param ack         The acknowledgement flag of the message.
     * @param rspMax      The maximum response time of the message.
     * @return The created OnOffSetMessage object.
     */
    public static OnOffSetMessage getSimple(int address, int appKeyIndex, int onOff, boolean ack, int rspMax) {
        OnOffSetMessage message = new OnOffSetMessage(address, appKeyIndex);
        message.onOff = (byte) onOff;
        message.ack = ack;
        message.setTidPosition(1);
        message.setResponseMax(rspMax);
        return message;
    }

    /**
     * Creates a new On/Off Set Message with the given destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index of the message.
     */
    public OnOffSetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Returns the response opcode of the message.
     * If acknowledgement is enabled, it returns the opcode for On/Off Status.
     * Otherwise, it returns the super class's response opcode.
     *
     * @return The response opcode of the message.
     */
    @Override
    public int getResponseOpcode() {
        return ack ? Opcode.G_ONOFF_STATUS.value : super.getResponseOpcode();
    }

    /**
     * Returns the opcode of the message.
     * If acknowledgement is enabled, it returns the opcode for On/Off Set.
     * Otherwise, it returns the opcode for On/Off Set No Acknowledgement.
     *
     * @return The opcode of the message.
     */
    @Override
    public int getOpcode() {
        return ack ? Opcode.G_ONOFF_SET.value : Opcode.G_ONOFF_SET_NOACK.value;
    }

    /**
     * Generates the message payload parameters.
     * If the message is marked as complete, it includes the on/off value, transaction ID, transition time, and delay.
     * Otherwise, it includes only the on/off value and transaction ID.
     *
     * @return The byte array representing the message payload parameters.
     */
    @Override
    public byte[] getParams() {
        return isComplete ?
                new byte[]{
                        this.onOff,
                        this.tid,
                        this.transitionTime,
                        this.delay
                }
                :
                new byte[]{
                        this.onOff,
                        this.tid
                };
    }

    /**
     * Sets the on/off value of the message.
     *
     * @param onOff The on/off value to be set.
     */
    public void setOnOff(byte onOff) {
        this.onOff = onOff;
    }

    /**
     * Sets the transaction ID of the message.
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
     * Sets the completion flag of the message.
     *
     * @param complete The completion flag to be set.
     */
    public void setComplete(boolean complete) {
        isComplete = complete;
    }
}