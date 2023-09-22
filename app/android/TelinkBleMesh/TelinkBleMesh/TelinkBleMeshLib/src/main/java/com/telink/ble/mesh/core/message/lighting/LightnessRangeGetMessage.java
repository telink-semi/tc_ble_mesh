/********************************************************************************************************
 * @file LightnessRangeGetMessage.java
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
 * This class represents a Lightness Range Get Message in a lighting system.
 * It extends the LightingMessage class.
 * The Lightness Range Get Message is used to request the lightness range of a lighting device.
 * It contains the destination address and application key index.
 * <p>
 * The class provides two constructors:
 * - The first constructor creates a simple LightnessRangeGetMessage object with the specified destination address, application key index, and response maximum value.
 * - The second constructor creates a new LightnessRangeGetMessage object with the specified destination address and application key index.
 * <p>
 * The class also provides a static method to create a simple LightnessRangeGetMessage object.
 * <p>
 * Example usage:
 * LightnessRangeGetMessage message = LightnessRangeGetMessage.getSimple(destinationAddress, appKeyIndex, rspMax);
 */
public class LightnessRangeGetMessage extends LightingMessage {

    /**
     * Creates a simple LightnessRangeGetMessage object with the specified destination address, application key index, and response maximum value.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index.
     * @param rspMax             The response maximum value.
     * @return A TimeGetMessage object.
     */
    public static LightnessRangeGetMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax) {
        LightnessRangeGetMessage message = new LightnessRangeGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        return message;
    }

    /**
     * Creates a new LightnessRangeGetMessage object with the specified destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index.
     */
    public LightnessRangeGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * default constructor
     */
    public LightnessRangeGetMessage() {
    }

    /**
     * Gets the opcode value for the LightnessRangeGetMessage.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.LIGHTNESS_RANGE_GET.value;
    }

    /**
     * Gets the response opcode value for the LightnessRangeGetMessage.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.LIGHTNESS_RANGE_STATUS.value;
    }
}
