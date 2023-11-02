/********************************************************************************************************
 * @file LightnessGetMessage.java
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
 * This class represents a LightnessGetMessage, which is a type of LightingMessage. 
 * It is used to retrieve the lightness value from a lighting device. 
 * The message contains the destination address and application key index. 
 * It also has methods to set the response maximum value and to get the opcode and response opcode values. 
 */ 
public class LightnessGetMessage extends LightingMessage {
    /**
     * Creates a simple LightnessGetMessage object with the specified destination address, application key index, and response maximum value.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index.
     * @param rspMax             The response maximum value.
     * @return A TimeGetMessage object.
     */
    public static LightnessGetMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax) {
        LightnessGetMessage message = new LightnessGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        return message;
    }

    /**
     * Creates a new LightnessGetMessage object with the specified destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index.
     */
    public LightnessGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Gets the opcode value for the message.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.LIGHTNESS_GET.value;
    }

    /**
     * Gets the response opcode value for the message.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.LIGHTNESS_STATUS.value;
    }
}
