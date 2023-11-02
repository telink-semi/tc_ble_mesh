/********************************************************************************************************
 * @file OnOffGetMessage.java
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
package com.telink.ble.mesh.core.message.generic;

import com.telink.ble.mesh.core.message.Opcode;

/**
 * This class represents an OnOffGetMessage, which is a specific type of GenericMessage.
 * It is used to send a request to get the On/Off status of a device.
 * This message can be created using the getSimple() method, which sets the destination address, app key index, and response maximum.
 * The opcode for this message is G_ONOFF_GET, and the response opcode is G_ONOFF_STATUS.
 * The params for this message are null.
 */
public class OnOffGetMessage extends GenericMessage {

    /**
     * Creates and returns a simple OnOffGetMessage with the specified destination address, app key index, and response maximum.
     *
     * @param destinationAddress The destination address for the message.
     * @param appKeyIndex        The app key index for the message.
     * @param rspMax             The response maximum for the message.
     * @return The created OnOffGetMessage.
     */
    public static OnOffGetMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax) {
        OnOffGetMessage message = new OnOffGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        return message;
    }

    /**
     * Creates a new OnOffGetMessage with the specified destination address and app key index.
     *
     * @param destinationAddress The destination address for the message.
     * @param appKeyIndex        The app key index for the message.
     */
    public OnOffGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Returns the opcode for this message, which is G_ONOFF_GET.
     *
     * @return The opcode for this message.
     */
    @Override
    public int getOpcode() {
        return Opcode.G_ONOFF_GET.value;
    }

    /**
     * Returns the response opcode for this message, which is G_ONOFF_STATUS.
     *
     * @return The response opcode for this message.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.G_ONOFF_STATUS.value;
    }

    /**
     * Returns the params for this message, which is null.
     *
     * @return The params for this message.
     */
    @Override
    public byte[] getParams() {
        return null;
    }
}