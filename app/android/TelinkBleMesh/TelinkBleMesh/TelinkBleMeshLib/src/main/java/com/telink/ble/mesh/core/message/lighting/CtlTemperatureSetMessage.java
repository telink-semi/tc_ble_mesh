/********************************************************************************************************
 * @file CtlTemperatureSetMessage.java
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
 * This class represents a message for setting the temperature in a CLT (Color Light Temperature) device.
 * It can be used for both CLT Temperature Set and CLT Temperature Set No Ack commands.
 * The message can be sent with or without acknowledgement.
 * The message includes the temperature value and the delta UV value.
 * It also includes a transaction ID, transition time, and delay.
 * The message can be marked as complete to include all parameters, or incomplete to exclude the transition time and delay.
 * The message extends the GenericMessage class.
 *
 * @author kee
 * @see GenericMessage
 * @since 2019/8/14
 */
public class CtlTemperatureSetMessage extends GenericMessage {
    private int temperature;
    private int deltaUV;
    private byte tid = 0;
    private byte transitionTime = 0;
    private byte delay = 0;
    private boolean ack = false;
    private boolean isComplete = false;

    /**
     * Creates a new CtlTemperatureSetMessage with the given destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index.
     */
    public CtlTemperatureSetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
        setTidPosition(4);
    }

    /**
     * Creates a simple CtlTemperatureSetMessage with the given parameters.
     *
     * @param address     The destination address of the message.
     * @param appKeyIndex The application key index.
     * @param temperature The temperature value to set.
     * @param deltaUV     The delta UV value to set.
     * @param ack         Whether to include acknowledgement or not.
     * @param rspMax      The maximum number of responses expected.
     * @return The created CtlTemperatureSetMessage.
     */
    public static CtlTemperatureSetMessage getSimple(int address, int appKeyIndex, int temperature, int deltaUV, boolean ack, int rspMax) {
        CtlTemperatureSetMessage message = new CtlTemperatureSetMessage(address, appKeyIndex);
        message.temperature = temperature;
        message.deltaUV = deltaUV;
        message.ack = ack;
        message.setResponseMax(rspMax);
        return message;
    }

    /**
     * Gets the opcode for the response message.
     * If acknowledgement is enabled, returns the opcode for LIGHT_CTL_TEMP_STATUS.
     * Otherwise, returns the opcode from the super class.
     *
     * @return The opcode for the response message.
     */
    @Override
    public int getResponseOpcode() {
        return ack ? Opcode.LIGHT_CTL_TEMP_STATUS.value : super.getResponseOpcode();
    }

    /**
     * Gets the opcode for the message.
     * If acknowledgement is enabled, returns the opcode for LIGHT_CTL_TEMP_SET.
     * Otherwise, returns the opcode for LIGHT_CTL_TEMP_SET_NOACK.
     *
     * @return The opcode for the message.
     */
    @Override
    public int getOpcode() {
        return ack ? Opcode.LIGHT_CTL_TEMP_SET.value : Opcode.LIGHT_CTL_TEMP_SET_NOACK.value;
    }

    /**
     * Gets the parameters of the message as a byte array.
     * If the message is marked as complete, returns a byte array with all parameters.
     * Otherwise, returns a byte array without the transition time and delay.
     *
     * @return The parameters of the message as a byte array.
     */
    @Override
    public byte[] getParams() {
        return isComplete ?
                ByteBuffer.allocate(7).order(ByteOrder.LITTLE_ENDIAN)
                        .putShort((short) temperature)
                        .putShort((short) deltaUV)
                        .put(tid)
                        .put(transitionTime)
                        .put(delay).array()
                :
                ByteBuffer.allocate(5).order(ByteOrder.LITTLE_ENDIAN)
                        .putShort((short) temperature)
                        .putShort((short) deltaUV)
                        .put(tid).array();
    }

    /**
     * Sets the temperature value of the message.
     *
     * @param temperature The temperature value to set.
     */
    public void setTemperature(int temperature) {
        this.temperature = temperature;
    }

    /**
     * Sets the delta UV value of the message.
     *
     * @param deltaUV The delta UV value to set.
     */
    public void setDeltaUV(int deltaUV) {
        this.deltaUV = deltaUV;
    }

    /**
     * Sets the transaction ID of the message.
     *
     * @param tid The transaction ID to set.
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
     * Sets whether the message should include acknowledgement or not.
     *
     * @param ack True to include acknowledgement, false otherwise.
     */
    public void setAck(boolean ack) {
        this.ack = ack;
    }

    /**
     * Sets whether the message is complete or not.
     * If true, the message includes all parameters.
     * If false, the message excludes the transition time and delay.
     *
     * @param complete True to mark the message as complete, false otherwise.
     */
    public void setComplete(boolean complete) {
        isComplete = complete;
    }
}