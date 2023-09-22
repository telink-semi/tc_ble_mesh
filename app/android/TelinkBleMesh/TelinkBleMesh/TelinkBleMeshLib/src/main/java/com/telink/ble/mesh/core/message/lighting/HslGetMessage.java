/********************************************************************************************************
 * @file HslGetMessage.java
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
 * This class represents a HSL Get message that is used to retrieve the HSL (Hue, Saturation, Lightness) values from a lighting device.
 * It extends the LightingMessage class.
 */
public class HslGetMessage extends LightingMessage {

    /**
     * Creates a simple HSL Get message with the specified destination address, application key index, and response maximum.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index of the message.
     * @param rspMax             The maximum number of responses expected.
     * @return The created HSL Get message.
     */
    public static HslGetMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax) {
        HslGetMessage message = new HslGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        return message;
    }

    /**
     * Creates a HSL Get message with the specified destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index of the message.
     */
    public HslGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Gets the opcode value of the HSL Get message.
     *
     * @return The opcode value of the HSL Get message.
     */
    @Override
    public int getOpcode() {
        return Opcode.LIGHT_HSL_GET.value;
    }

    /**
     * Gets the response opcode value of the HSL Get message.
     *
     * @return The response opcode value of the HSL Get message.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.LIGHT_HSL_STATUS.value;
    }
}