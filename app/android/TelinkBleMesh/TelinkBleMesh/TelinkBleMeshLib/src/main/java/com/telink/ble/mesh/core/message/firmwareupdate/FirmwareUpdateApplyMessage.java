/********************************************************************************************************
 * @file FirmwareUpdateApplyMessage.java
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
package com.telink.ble.mesh.core.message.firmwareupdate;

import com.telink.ble.mesh.core.message.Opcode;

/**
 * Represents a firmware update apply message.
 * Extends the UpdatingMessage class.
 */
public class FirmwareUpdateApplyMessage extends UpdatingMessage {

    /**
     * Creates a simple firmware update apply message with the specified destination address and app key index.
     * Sets the response max to 1.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The app key index of the message.
     * @return A FirmwareUpdateApplyMessage object.
     */
    public static FirmwareUpdateApplyMessage getSimple(int destinationAddress, int appKeyIndex) {
        FirmwareUpdateApplyMessage message = new FirmwareUpdateApplyMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        return message;
    }

    /**
     * Creates a firmware update apply message with the specified destination address and app key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The app key index of the message.
     */
    public FirmwareUpdateApplyMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Gets the opcode of the firmware update apply message.
     *
     * @return The opcode value of the firmware update apply message.
     */
    @Override
    public int getOpcode() {
        return Opcode.FIRMWARE_UPDATE_APPLY.value;
    }

    /**
     * Gets the response opcode of the firmware update apply message.
     *
     * @return The response opcode value of the firmware update apply message.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.FIRMWARE_UPDATE_STATUS.value;
    }
}