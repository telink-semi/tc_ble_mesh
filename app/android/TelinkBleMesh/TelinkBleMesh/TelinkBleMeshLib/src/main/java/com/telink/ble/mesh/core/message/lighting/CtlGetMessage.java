/********************************************************************************************************
 * @file CtlGetMessage.java
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
 * This class represents a Control Get Message, which is a type of Lighting Message.
 * It is used to request the current control state of a lighting device.
 * It extends the LightingMessage class.
 */
public class CtlGetMessage extends LightingMessage {

    /**
     * Creates a simple Control Get Message with the specified destination address, application key index, and response maximum.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index.
     * @param rspMax             The maximum number of responses expected.
     * @return The created Control Get Message.
     */
    public static CtlGetMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax) {
        CtlGetMessage message = new CtlGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        return message;
    }

    /**
     * Creates a Control Get Message with the specified destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index.
     */
    public CtlGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Gets the opcode of the Control Get Message.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.LIGHT_CTL_GET.value;
    }

    /**
     * Gets the response opcode of the Control Get Message.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.LIGHT_CTL_STATUS.value;
    }
}