/********************************************************************************************************
 * @file LightnessDefaultGetMessage.java
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
  * This class represents a Lightness Default Get Message in a lighting system. 
  * It extends the LightingMessage class and provides methods to create and retrieve 
  * information about the message. 
  */ 
public class LightnessDefaultGetMessage extends LightingMessage {
    /**
     * Creates a simple LightnessDefaultGetMessage object with the specified destination address, application key index, and response maximum value.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index.
     * @param rspMax             The response maximum value.
     * @return A TimeGetMessage object.
     */
    public static LightnessDefaultGetMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax) {
        LightnessDefaultGetMessage message = new LightnessDefaultGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        return message;
    }

    /**
     * Creates a new LightnessDefaultGetMessage object with the specified destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index.
     */
    public LightnessDefaultGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * default constructor
     */
    public LightnessDefaultGetMessage() {
    }

    /**
     * Gets the opcode value for the message.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.LIGHTNESS_DEFULT_GET.value;
    }

    /**
     * Gets the response opcode value for the message.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.LIGHTNESS_DEFULT_STATUS.value;
    }
}
