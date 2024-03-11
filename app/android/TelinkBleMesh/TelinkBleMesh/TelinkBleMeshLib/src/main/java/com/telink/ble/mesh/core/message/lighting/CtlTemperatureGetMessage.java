/********************************************************************************************************
 * @file CtlTemperatureGetMessage.java
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

/**
 * This class represents a message to get the current temperature in a lighting control system.
 * It extends the LightingMessage class.
 */
public class CtlTemperatureGetMessage extends LightingMessage {

    /**
     * Creates a new instance of CtlTemperatureGetMessage with the specified destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index of the message.
     */
    public CtlTemperatureGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Returns a simple instance of CtlTemperatureGetMessage with the specified destination address, application key index, and maximum response.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index of the message.
     * @param rspMax             The maximum response.
     * @return A simple instance of CtlTemperatureGetMessage.
     */
    public static CtlTemperatureGetMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax) {
        CtlTemperatureGetMessage message = new CtlTemperatureGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        return message;
    }

    /**
     * Returns the opcode of the message.
     *
     * @return The opcode of the message.
     */
    @Override
    public int getOpcode() {
        return Opcode.LIGHT_CTL_TEMP_GET.value;
    }

    /**
     * Returns the response opcode of the message.
     *
     * @return The response opcode of the message.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.LIGHT_CTL_TEMP_STATUS.value;
    }
}