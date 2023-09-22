/********************************************************************************************************
 * @file HslSetMessage.java
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

import java.nio.ByteBuffer;
import java.nio.ByteOrder;


/**
 * include HSL set and HSL set no ack
 * This class represents a HSL Set message, which is used to set the lightness, hue, and saturation values of a device.
 * It extends the GenericMessage class.
 * The message can be configured to include a transaction id, transition time, and delay.
 * It can also be configured to request an acknowledgment from the receiving device.
 */
public class HslSetMessage extends GenericMessage {
    private int lightness; // The lightness value to set
    private int hue; // The hue value to set
    private int saturation; // The saturation value to set
    private byte tid = 0; // The transaction id
    private byte transitionTime = 0; // The transition time
    private byte delay = 0; // The delay
    private boolean ack = false; // Flag to request acknowledgment
    private boolean isComplete = false; // Flag to indicate if the message is complete

    /**
     * Constructs a simple HSL Set message with the given parameters.
     *
     * @param address     The destination address of the message
     * @param appKeyIndex The application key index
     * @param lightness   The lightness value to set
     * @param hue         The hue value to set
     * @param saturation  The saturation value to set
     * @param ack         Flag to request acknowledgment
     * @param rspMax      The maximum number of responses expected
     * @return The constructed HslSetMessage
     */
    public static HslSetMessage getSimple(int address, int appKeyIndex, int lightness, int hue, int saturation, boolean ack, int rspMax) {
        HslSetMessage message = new HslSetMessage(address, appKeyIndex);
        message.lightness = lightness;
        message.hue = hue;
        message.saturation = saturation;
        message.ack = ack;
        message.setResponseMax(rspMax);
        return message;
    }

    /**
     * Constructs a HSL Set message with the given destination address and application key index.
     *
     * @param destinationAddress The destination address of the message
     * @param appKeyIndex        The application key index
     */
    public HslSetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
        setTidPosition(6);
    }

    /**
     * Returns the response opcode based on the acknowledgment flag.
     *
     * @return The response opcode
     */
    @Override
    public int getResponseOpcode() {
        return ack ? Opcode.LIGHT_HSL_STATUS.value : super.getResponseOpcode();
    }

    /**
     * Returns the opcode based on the acknowledgment flag.
     *
     * @return The opcode
     */
    @Override
    public int getOpcode() {
        return ack ? Opcode.LIGHT_HSL_SET.value : Opcode.LIGHT_HSL_SET_NOACK.value;
    }

    /**
     * Returns the parameters of the message as a byte array.
     * The byte array includes the lightness, hue, saturation, tid, transition time, and delay if the message is complete.
     * Otherwise, it includes only the lightness, hue, saturation, and tid.
     *
     * @return The parameters as a byte array
     */
    @Override
    public byte[] getParams() {
        return isComplete ?
                ByteBuffer.allocate(9).order(ByteOrder.LITTLE_ENDIAN)
                        .putShort((short) lightness)
                        .putShort((short) hue)
                        .putShort((short) saturation)
                        .put(tid)
                        .put(transitionTime)
                        .put(delay).array()
                :
                ByteBuffer.allocate(7).order(ByteOrder.LITTLE_ENDIAN)
                        .putShort((short) lightness)
                        .putShort((short) hue)
                        .putShort((short) saturation)
                        .put(tid).array();
    }

    /**
     * Sets the lightness value of the message.
     *
     * @param lightness The lightness value to set
     */
    public void setLightness(int lightness) {
        this.lightness = lightness;
    }

    /**
     * Sets the hue value of the message.
     *
     * @param hue The hue value to set
     */
    public void setHue(int hue) {
        this.hue = hue;
    }

    /**
     * Sets the saturation value of the message.
     *
     * @param saturation The saturation value to set
     */
    public void setSaturation(int saturation) {
        this.saturation = saturation;
    }

    /**
     * Sets the transaction id of the message.
     *
     * @param tid The transaction id to set
     */
    public void setTid(byte tid) {
        this.tid = tid;
    }

    /**
     * Sets the transition time of the message.
     *
     * @param transitionTime The transition time to set
     */
    public void setTransitionTime(byte transitionTime) {
        this.transitionTime = transitionTime;
    }

    /**
     * Sets the delay of the message.
     *
     * @param delay The delay to set
     */
    public void setDelay(byte delay) {
        this.delay = delay;
    }

    /**
     * Sets the acknowledgment flag of the message.
     *
     * @param ack The acknowledgment flag to set
     */
    public void setAck(boolean ack) {
        this.ack = ack;
    }

    /**
     * Sets the complete flag of the message.
     *
     * @param complete The complete flag to set
     */
    public void setComplete(boolean complete) {
        isComplete = complete;
    }
}