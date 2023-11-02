/********************************************************************************************************
 * @file NetworkTransmitSetMessage.java
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
package com.telink.ble.mesh.core.message.config;

import com.telink.ble.mesh.core.message.Opcode;

/**
 * set network transmit params
 * Network transmit params are used when node sending network pdu from self -- source address is self --
 * Meanwhile relay params are used when relay network pdu, generally smaller than network transmit
 */
public class NetworkTransmitSetMessage extends ConfigMessage {


    // networkTransmitCount, default is 5
    public int count;


    // networkTransmitIntervalSteps, default is 2
    // transmission interval = (Network Transmit Interval Steps + 1) * 10
    public int intervalSteps;

    /**
     * Creates a new NetworkTransmitSetMessage with the given destination address.
     *
     * @param destinationAddress The destination address for the message.
     */
    public NetworkTransmitSetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    /**
     * Creates a new NetworkTransmitSetMessage with the given destination address, count, and interval steps.
     *
     * @param destinationAddress The destination address for the message.
     * @param count              The number of times the network PDU should be transmitted.
     * @param intervalSteps      The interval steps for transmitting the network PDU.
     * @return The created NetworkTransmitSetMessage.
     */
    public static NetworkTransmitSetMessage getSimple(int destinationAddress, int count, int intervalSteps) {
        NetworkTransmitSetMessage message = new NetworkTransmitSetMessage(destinationAddress);
        message.count = count;
        message.intervalSteps = intervalSteps;
        return message;
    }

    /**
     * Sets the number of times the network PDU should be transmitted.
     *
     * @param count The number of times the network PDU should be transmitted.
     */
    public void setCount(int count) {
        this.count = count;
    }

    /**
     * Sets the interval steps for transmitting the network PDU.
     *
     * @param intervalSteps The interval steps for transmitting the network PDU.
     */
    public void setIntervalSteps(int intervalSteps) {
        this.intervalSteps = intervalSteps;
    }

    /**
     * Returns the opcode value for the NetworkTransmitSetMessage.
     *
     * @return The opcode value for the NetworkTransmitSetMessage.
     */
    @Override
    public int getOpcode() {
        return Opcode.CFG_NW_TRANSMIT_SET.value;
    }

    /**
     * Returns the response opcode value for the NetworkTransmitSetMessage.
     *
     * @return The response opcode value for the NetworkTransmitSetMessage.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.CFG_NW_TRANSMIT_STATUS.value;
    }

    /**
     * Returns the parameters of the NetworkTransmitSetMessage.
     *
     * @return The parameters of the NetworkTransmitSetMessage.
     */
    @Override
    public byte[] getParams() {
        return new byte[]{
                (byte) ((count & 0b111) | (intervalSteps << 3))
        };
    }
}
