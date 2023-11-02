/********************************************************************************************************
 * @file LevelSetMessage.java
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
 * define level set and level set no ack
 * by {@link #ack}
 * LevelSetMessage class represents a message for setting the level of a device.
 * This message can be sent with or without acknowledgement.
 * The level can be set to either "on" or "off".
 * This message can also include optional parameters such as transaction ID, transition time, and delay.
 * Created by kee on 2019/8/14.
 */
public class LevelSetMessage extends GenericMessage {
    // 1: on, 0: off
    private int level;
    private byte tid = 0;
    private byte transitionTime = 0;
    private byte delay = 0;
    private boolean ack = false;
    /**
     * Indicates whether the message is complete with all the optional parameters filled.
     */
    private boolean isComplete = false;

    /**
     * Constructs a LevelSetMessage object with the specified destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index.
     */
    public LevelSetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
        setTidPosition(2);
    }

    /**
     * Gets the opcode of the message.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return ack ? Opcode.G_LEVEL_SET.value : Opcode.G_LEVEL_SET_NOACK.value;
    }

    /**
     * Gets the parameters of the message.
     *
     * @return The parameters as a byte array.
     */
    @Override
    public byte[] getParams() {
        return isComplete ?
                new byte[]{
                        (byte) this.level,
                        (byte) (this.level >> 8),
                        this.tid,
                        this.transitionTime,
                        this.delay
                }
                :
                new byte[]{
                        (byte) this.level,
                        (byte) (this.level >> 8),
                        this.tid
                };
    }

    /**
     * Sets the level of the device.
     *
     * @param level The level value.
     */
    public void setLevel(int level) {
        this.level = level;
    }

    /**
     * Sets the transaction ID.
     *
     * @param tid The transaction ID value.
     */
    public void setTid(byte tid) {
        this.tid = tid;
    }

    /**
     * Sets the transition time.
     *
     * @param transitionTime The transition time value.
     */
    public void setTransitionTime(byte transitionTime) {
        this.transitionTime = transitionTime;
    }

    /**
     * Sets the delay.
     *
     * @param delay The delay value.
     */
    public void setDelay(byte delay) {
        this.delay = delay;
    }

    /**
     * Sets whether the message requires acknowledgement.
     *
     * @param ack True if acknowledgement is required, false otherwise.
     */
    public void setAck(boolean ack) {
        this.ack = ack;
    }

    /**
     * Sets whether the message is complete with all the optional parameters filled.
     *
     * @param complete True if the message is complete, false otherwise.
     */
    public void setComplete(boolean complete) {
        isComplete = complete;
    }
}