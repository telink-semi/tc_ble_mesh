/********************************************************************************************************
 * @file FirmwareUpdateInfoGetMessage.java
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
 * This class represents a message for requesting firmware update information from a device.
 * It extends the UpdatingMessage class.
 */
public class FirmwareUpdateInfoGetMessage extends UpdatingMessage {
    /**
     * Index of the first requested entry from the Firmware Information List state.
     * This is a 1-byte value.
     */
    private byte firstIndex;

    /**
     * Maximum number of entries that the server includes in a Firmware Update Information Status message.
     * This is a 1-byte value.
     */
    private byte entriesLimit;

    /**
     * Creates a new instance of FirmwareUpdateInfoGetMessage with the specified destination address and app key index.
     *
     * @param destinationAddress The destination address for the message.
     * @param appKeyIndex        The app key index for the message.
     */
    public FirmwareUpdateInfoGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Returns the opcode value for the Firmware Update Information Get message.
     *
     * @return The opcode value for the message.
     */
    @Override
    public int getOpcode() {
        return Opcode.FIRMWARE_UPDATE_INFORMATION_GET.value;
    }

    /**
     * Returns the opcode value for the Firmware Update Information Status message, which is the response to the Get message.
     *
     * @return The opcode value for the response message.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.FIRMWARE_UPDATE_INFORMATION_STATUS.value;
    }

    /**
     * Returns the parameters of the message as a byte array.
     *
     * @return The parameters of the message.
     */
    @Override
    public byte[] getParams() {
        return new byte[]{firstIndex, entriesLimit};
    }

    /**
     * Sets the value of the firstIndex parameter.
     *
     * @param firstIndex The value to set.
     */
    public void setFirstIndex(byte firstIndex) {
        this.firstIndex = firstIndex;
    }

    /**
     * Sets the value of the entriesLimit parameter.
     *
     * @param entriesLimit The value to set.
     */
    public void setEntriesLimit(byte entriesLimit) {
        this.entriesLimit = entriesLimit;
    }

    /**
     * Creates a simple FirmwareUpdateInfoGetMessage with the specified destination address and app key index.
     * The response max is set to 1, the firstIndex is set to 0, and the entriesLimit is set to 1.
     *
     * @param destinationAddress The destination address for the message.
     * @param appKeyIndex        The app key index for the message.
     * @return The created FirmwareUpdateInfoGetMessage.
     */
    public static FirmwareUpdateInfoGetMessage getSimple(int destinationAddress, int appKeyIndex) {
        FirmwareUpdateInfoGetMessage message = new FirmwareUpdateInfoGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        message.firstIndex = 0;
        message.entriesLimit = 1;
        return message;
    }
}