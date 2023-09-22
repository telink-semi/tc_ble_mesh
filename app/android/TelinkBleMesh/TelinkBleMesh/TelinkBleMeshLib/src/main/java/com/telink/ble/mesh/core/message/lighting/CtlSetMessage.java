/********************************************************************************************************
 * @file CtlSetMessage.java
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
 * This class represents a Control Set Message, which is used to include CLT set and CTL set without acknowledgement.
 * The message can be acknowledged or not acknowledged by setting the 'ack' flag.
 * The message contains parameters such as lightness, temperature, deltaUV, transaction ID, transition time, and delay.
 * The 'isComplete' flag indicates whether the message is complete or not.
 * <p>
 * This class provides methods to set and get the values of the parameters.
 * <p>
 * Example usage:
 * CtlSetMessage message = CtlSetMessage.getSimple(address, appKeyIndex, lightness, temperature, deltaUV, ack, rspMax);
 * message.setLightness(lightness);
 * message.setTemperature(temperature);
 * message.setDeltaUV(deltaUV);
 * message.setTid(tid);
 * message.setTransitionTime(transitionTime);
 * message.setDelay(delay);
 * message.setAck(ack);
 * message.setComplete(isComplete);
 */
public class CtlSetMessage extends GenericMessage {
    private int lightness;
    private int temperature;
    private int deltaUV;
    private byte tid = 0;
    private byte transitionTime = 0;
    private byte delay = 0;
    private boolean ack = false;
    private boolean isComplete = false;

    /**
     * Creates a new CtlSetMessage with the given destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index of the message.
     */
    public CtlSetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
        setTidPosition(6);
    }

    /**
     * Returns the response opcode of the message.
     * If the 'ack' flag is set, it returns the opcode for LIGHT_CTL_STATUS.
     * Otherwise, it returns the super class' response opcode.
     *
     * @return The response opcode of the message.
     */
    @Override
    public int getResponseOpcode() {
        return ack ? Opcode.LIGHT_CTL_STATUS.value : super.getResponseOpcode();
    }

    /**
     * Returns the opcode of the message.
     * If the 'ack' flag is set, it returns the opcode for LIGHT_CTL_SET.
     * Otherwise, it returns the opcode for LIGHT_CTL_SET_NOACK.
     *
     * @return The opcode of the message.
     */
    @Override
    public int getOpcode() {
        return ack ? Opcode.LIGHT_CTL_SET.value : Opcode.LIGHT_CTL_SET_NOACK.value;
    }

    /**
     * Returns the parameters of the message as a byte array.
     * If the 'isComplete' flag is set, it returns a byte array with 9 bytes.
     * Otherwise, it returns a byte array with 7 bytes.
     *
     * @return The parameters of the message as a byte array.
     */
    @Override
    public byte[] getParams() {
        return isComplete ?
                ByteBuffer.allocate(9).order(ByteOrder.LITTLE_ENDIAN)
                        .putShort((short) lightness)
                        .putShort((short) temperature)
                        .putShort((short) deltaUV)
                        .put(tid)
                        .put(transitionTime)
                        .put(delay).array()
                :
                ByteBuffer.allocate(7).order(ByteOrder.LITTLE_ENDIAN)
                        .putShort((short) lightness)
                        .putShort((short) temperature)
                        .putShort((short) deltaUV)
                        .put(tid).array();
    }

    /**
     * Sets the lightness parameter of the message.
     *
     * @param lightness The lightness value to set.
     */
    public void setLightness(int lightness) {
        this.lightness = lightness;
    }

    /**
     * Sets the temperature parameter of the message.
     *
     * @param temperature The temperature value to set.
     */
    public void setTemperature(int temperature) {
        this.temperature = temperature;
    }

    /**
     * Sets the deltaUV parameter of the message.
     *
     * @param deltaUV The deltaUV value to set.
     */
    public void setDeltaUV(int deltaUV) {
        this.deltaUV = deltaUV;
    }

    /**
     * Sets the transaction ID parameter of the message.
     *
     * @param tid The transaction ID value to set.
     */
    public void setTid(byte tid) {
        this.tid = tid;
    }

    /**
     * Sets the transition time parameter of the message.
     *
     * @param transitionTime The transition time value to set.
     */
    public void setTransitionTime(byte transitionTime) {
        this.transitionTime = transitionTime;
    }

    /**
     * Sets the delay parameter of the message.
     *
     * @param delay The delay value to set.
     */
    public void setDelay(byte delay) {
        this.delay = delay;
    }

    /**
     * Sets the acknowledgement flag of the message.
     *
     * @param ack The acknowledgement flag value to set.
     */
    public void setAck(boolean ack) {
        this.ack = ack;
    }

    /**
     * Sets the complete flag of the message.
     *
     * @param complete The complete flag value to set.
     */
    public void setComplete(boolean complete) {
        isComplete = complete;
    }

    /**
     * Creates and returns a simple CtlSetMessage with the given parameters.
     *
     * @param address     The destination address of the message.
     * @param appKeyIndex The application key index of the message.
     * @param lightness   The lightness value of the message.
     * @param temperature The temperature value of the message.
     * @param deltaUV     The deltaUV value of the message.
     * @param ack         The acknowledgement flag value of the message.
     * @param rspMax      The maximum response value of the message.
     * @return The simple CtlSetMessage.
     */
    public static CtlSetMessage getSimple(int address, int appKeyIndex, int lightness, int temperature, int deltaUV, boolean ack, int rspMax) {
        CtlSetMessage message = new CtlSetMessage(address, appKeyIndex);
        message.lightness = lightness;
        message.temperature = temperature;
        message.deltaUV = deltaUV;
        message.ack = ack;
        message.setResponseMax(rspMax);
        return message;
    }
}