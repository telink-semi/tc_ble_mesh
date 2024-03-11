/********************************************************************************************************
 * @file TimeGetMessage.java
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
package com.telink.ble.mesh.core.message.time;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.lighting.LightingMessage;

/**
 * This class represents a message used to request the current time from a lighting device.
 * It extends the LightingMessage class.
 */
public class TimeGetMessage extends LightingMessage {

    /**
     * Creates a new TimeGetMessage object with the specified destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index.
     */
    public TimeGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Creates a simple TimeGetMessage object with the specified destination address, application key index, and response maximum value.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index.
     * @param rspMax             The response maximum value.
     * @return A TimeGetMessage object.
     */
    public static TimeGetMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax) {
        TimeGetMessage message = new TimeGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        return message;
    }

    /**
     * Gets the opcode value for the TimeGetMessage.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.TIME_GET.value;
    }

    /**
     * Gets the response opcode value for the TimeGetMessage.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.TIME_STATUS.value;
    }
}