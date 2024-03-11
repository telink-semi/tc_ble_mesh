/********************************************************************************************************
 * @file LcLightOnOffSetMessage.java
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
package com.telink.ble.mesh.core.message.lighting;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.generic.GenericMessage;

/**
 * This class represents a Lighting Control On Off Set message, which is used to set the lightness of a device.
 * The message can be sent with or without acknowledgement.
 * The lightness value, transition time, and delay can be set in the message.
 * The message can also be marked as complete or incomplete.
 * The message extends the GenericMessage class.
 *
 * @author kee
 * @see GenericMessage
 * @since 2019/8/14
 */
public class LcLightOnOffSetMessage extends GenericMessage {
    // 1 byte
    private byte lightOnOff;
    // transition id
    private byte tid = 0;
    private byte transitionTime = 0;
    private byte delay = 0;
    private boolean ack = false;
    private boolean isComplete = false;

    /**
     * Creates a simple LightnessSetMessage with the given parameters.
     *
     * @param address     The destination address of the message.
     * @param appKeyIndex The application key index.
     * @param onOff       The onOff value to set.
     * @param ack         Whether to send the message with acknowledgement.
     * @param rspMax      The maximum number of responses expected.
     * @return The created LightnessSetMessage.
     */
    public static LcLightOnOffSetMessage getSimple(int address, int appKeyIndex, byte onOff, boolean ack, int rspMax) {
        LcLightOnOffSetMessage message = new LcLightOnOffSetMessage(address, appKeyIndex);
        message.lightOnOff = onOff;
        message.ack = ack;
        message.setResponseMax(rspMax);
        return message;
    }

    /**
     * Constructs a LightnessSetMessage with the given destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index.
     */
    public LcLightOnOffSetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
        setTidPosition(2);
    }

    /**
     * Returns the opcode of the response message.
     * If the message is sent with acknowledgement, the opcode will be LIGHTNESS_STATUS.
     * Otherwise, it will call the superclass method to get the opcode.
     *
     * @return The opcode of the response message.
     */
    @Override
    public int getResponseOpcode() {
        return ack ? Opcode.LIGHT_LC_ONOFF_STATUS.value : super.getResponseOpcode();
    }

    /**
     * Returns the opcode of the message.
     * If the message is sent with acknowledgement, the opcode will be LIGHTNESS_SET.
     * Otherwise, it will be LIGHTNESS_SET_NOACK.
     *
     * @return The opcode of the message.
     */
    @Override
    public int getOpcode() {
        return ack ? Opcode.LIGHT_LC_ONOFF_SET.value : Opcode.LIGHT_LC_ONOFF_SET_NOACK.value;
    }

    /**
     * Returns the parameters of the message as a byte array.
     * If the message is marked as complete, it will include the lightness value, transition time, and delay in the byte array.
     * Otherwise, it will only include the lightness value and transition id.
     *
     * @return The parameters of the message as a byte array.
     */
    @Override
    public byte[] getParams() {
        return isComplete ?
                new byte[]{
                        this.lightOnOff,
                        this.tid,
                        this.transitionTime,
                        this.delay
                }
                :
                new byte[]{
                        this.lightOnOff,
                        this.tid
                };
    }

    /**
     * Sets the lightness value of the message.
     *
     * @param lightOnOff The onOff value to set.
     */
    public void setLightOnOff(byte lightOnOff) {
        this.lightOnOff = lightOnOff;
    }

    /**
     * Sets the transition id of the message.
     *
     * @param tid The transition id to set.
     */
    public void setTid(byte tid) {
        this.tid = tid;
    }

    /**
     * Sets the transition time of the message.
     *
     * @param transitionTime The transition time to set.
     */
    public void setTransitionTime(byte transitionTime) {
        this.transitionTime = transitionTime;
    }

    /**
     * Sets the delay of the message.
     *
     * @param delay The delay to set.
     */
    public void setDelay(byte delay) {
        this.delay = delay;
    }

    /**
     * Sets whether the message should be sent with acknowledgement.
     *
     * @param ack Whether to send the message with acknowledgement.
     */
    public void setAck(boolean ack) {
        this.ack = ack;
    }

    /**
     * Sets whether the message is complete or incomplete.
     *
     * @param complete Whether the message is complete.
     */
    public void setComplete(boolean complete) {
        isComplete = complete;
    }
}

