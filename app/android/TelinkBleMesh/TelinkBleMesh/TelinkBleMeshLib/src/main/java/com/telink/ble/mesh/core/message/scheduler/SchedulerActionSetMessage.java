/********************************************************************************************************
 * @file SchedulerActionSetMessage.java
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
package com.telink.ble.mesh.core.message.scheduler;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.generic.GenericMessage;

/**
 * This class represents a Scheduler Action Set Message, which is used to send scheduler action set commands to a device.
 * It extends the GenericMessage class.
 * The Scheduler Action Set Message can be sent with or without acknowledgement.
 * The message includes the scheduler parameters.
 * <p>
 * Example usage:
 * SchedulerActionSetMessage message = SchedulerActionSetMessage.getSimple(address, appKeyIndex, schedulerParams, ack, rspMax);
 *
 * @author kee
 * @since 2019/8/14
 */
public class SchedulerActionSetMessage extends GenericMessage {
    private byte[] schedulerParams;
    private boolean ack = false;

    /**
     * Creates a simple SchedulerActionSetMessage object with the given parameters.
     *
     * @param address         The destination address of the message.
     * @param appKeyIndex     The index of the application key.
     * @param schedulerParams The scheduler parameters to be set.
     * @param ack             Specifies whether the message requires acknowledgement.
     * @param rspMax          The maximum number of response messages expected.
     * @return The created SchedulerActionSetMessage object.
     */
    public static SchedulerActionSetMessage getSimple(int address, int appKeyIndex, byte[] schedulerParams, boolean ack, int rspMax) {
        SchedulerActionSetMessage message = new SchedulerActionSetMessage(address, appKeyIndex);
        message.schedulerParams = schedulerParams;
        message.ack = ack;
        message.setResponseMax(rspMax);
        return message;
    }

    /**
     * Constructs a SchedulerActionSetMessage object with the given destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The index of the application key.
     */
    public SchedulerActionSetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Returns the opcode of the response message.
     * If the message requires acknowledgement, the opcode is Opcode.SCHD_ACTION_STATUS.value.
     * Otherwise, it calls the super class method to get the opcode.
     *
     * @return The opcode of the response message.
     */
    @Override
    public int getResponseOpcode() {
        return ack ? Opcode.SCHD_ACTION_STATUS.value : super.getResponseOpcode();
    }

    /**
     * Returns the opcode of the message.
     * If the message requires acknowledgement, the opcode is Opcode.SCHD_ACTION_SET.value.
     * Otherwise, the opcode is Opcode.SCHD_ACTION_SET_NOACK.value.
     *
     * @return The opcode of the message.
     */
    @Override
    public int getOpcode() {
        return ack ? Opcode.SCHD_ACTION_SET.value : Opcode.SCHD_ACTION_SET_NOACK.value;
    }

    /**
     * Returns the scheduler parameters of the message.
     *
     * @return The scheduler parameters of the message.
     */
    @Override
    public byte[] getParams() {
        return schedulerParams;
    }

    /**
     * Sets the scheduler parameters of the message.
     *
     * @param schedulerParams The scheduler parameters to be set.
     */
    public void setSchedulerParams(byte[] schedulerParams) {
        this.schedulerParams = schedulerParams;
    }

    /**
     * Sets whether the message requires acknowledgement.
     *
     * @param ack Specifies whether the message requires acknowledgement.
     */
    public void setAck(boolean ack) {
        this.ack = ack;
    }
}