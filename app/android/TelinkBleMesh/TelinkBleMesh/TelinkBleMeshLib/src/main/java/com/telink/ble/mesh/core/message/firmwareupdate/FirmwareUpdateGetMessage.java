/********************************************************************************************************
 * @file FirmwareUpdateGetMessage.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date     Sep. 30, 2017
 *
 * @par     Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
package com.telink.ble.mesh.core.message.firmwareupdate;

import com.telink.ble.mesh.core.message.Opcode;

/**
 * This class represents a firmware update get message, which is used to request information about a firmware update.
 * It extends the UpdatingMessage class.
 */
public class FirmwareUpdateGetMessage extends UpdatingMessage {

    /**
     * Creates a simple firmware update get message with the specified destination address and application key index.
     * @param destinationAddress The destination address for the message.
     * @param appKeyIndex The application key index for the message.
     * @return The created FirmwareUpdateGetMessage object.
     */
    public static FirmwareUpdateGetMessage getSimple(int destinationAddress, int appKeyIndex) {
        FirmwareUpdateGetMessage message = new FirmwareUpdateGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        return message;
    }

    /**
     * Constructs a new FirmwareUpdateGetMessage with the specified destination address and application key index.
     * @param destinationAddress The destination address for the message.
     * @param appKeyIndex The application key index for the message.
     */
    public FirmwareUpdateGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Gets the opcode for the firmware update get message.
     * @return The opcode for the firmware update get message.
     */
    @Override
    public int getOpcode() {
        return Opcode.FIRMWARE_UPDATE_GET.value;
    }

    /**
     * Gets the response opcode for the firmware update get message.
     * @return The response opcode for the firmware update get message.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.FIRMWARE_UPDATE_STATUS.value;
    }
}