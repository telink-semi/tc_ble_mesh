/********************************************************************************************************
 * @file FDCapabilitiesGetMessage.java
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
package com.telink.ble.mesh.core.message.firmwaredistribution;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.firmwareupdate.UpdatingMessage;

/**
 * This class represents a message used to retrieve the capabilities of a device in a network.
 * It extends the UpdatingMessage class.
 */
public class FDCapabilitiesGetMessage extends UpdatingMessage {

    /**
     * Constructs a new FDCapabilitiesGetMessage with the specified destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index.
     */
    public FDCapabilitiesGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Creates a simple FDCapabilitiesGetMessage with the specified destination address and application key index.
     * This method sets the maximum number of responses to 1.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index.
     * @return A simple FDCapabilitiesGetMessage.
     */
    public static FDCapabilitiesGetMessage getSimple(int destinationAddress, int appKeyIndex) {
        FDCapabilitiesGetMessage message = new FDCapabilitiesGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        return message;
    }

    /**
     * Gets the opcode of the FDCapabilitiesGetMessage.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.FD_CAPABILITIES_GET.value;
    }

    /**
     * Gets the response opcode of the FDCapabilitiesGetMessage.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.FD_CAPABILITIES_STATUS.value;
    }
}