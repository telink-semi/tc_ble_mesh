/********************************************************************************************************
 * @file FirmwareMetadataCheckMessage.java
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

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * This class represents a message for checking the firmware metadata during a firmware update.
 * It extends the UpdatingMessage class.
 */
public class FirmwareMetadataCheckMessage extends UpdatingMessage {

    /**
     * Index of the firmware image in the Firmware Information List state to check.
     * This value is represented by 1 byte.
     */
    private int updateFirmwareImageIndex;

    /**
     * If the value of the Incoming Firmware Metadata Length field is greater than 0,
     * the Incoming Firmware Metadata field shall be present.
     */
    private byte[] incomingFirmwareMetadata;

    /**
     * Creates a simple FirmwareMetadataCheckMessage object with the specified destination address, app key index, index, and incoming firmware metadata.
     *
     * @param destinationAddress       The destination address of the message.
     * @param appKeyIndex              The app key index of the message.
     * @param index                    The index of the firmware image to check.
     * @param incomingFirmwareMetadata The incoming firmware metadata.
     * @return A FirmwareMetadataCheckMessage object.
     */
    public static FirmwareMetadataCheckMessage getSimple(int destinationAddress, int appKeyIndex, int index, byte[] incomingFirmwareMetadata) {
        FirmwareMetadataCheckMessage message = new FirmwareMetadataCheckMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        message.updateFirmwareImageIndex = index;
        message.incomingFirmwareMetadata = incomingFirmwareMetadata;
        return message;
    }

    /**
     * Constructs a FirmwareMetadataCheckMessage object with the specified destination address and app key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The app key index of the message.
     */
    public FirmwareMetadataCheckMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Gets the opcode of the message.
     *
     * @return The opcode of the message.
     */
    @Override
    public int getOpcode() {
        return Opcode.FIRMWARE_UPDATE_FIRMWARE_METADATA_CHECK.value;
    }

    /**
     * Gets the response opcode of the message.
     *
     * @return The response opcode of the message.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.FIRMWARE_UPDATE_FIRMWARE_METADATA_STATUS.value;
    }

    /**
     * Gets the parameters of the message.
     *
     * @return The parameters of the message.
     */
    @Override
    public byte[] getParams() {
        int metadataLen = (incomingFirmwareMetadata == null || incomingFirmwareMetadata.length <= 0) ? 0 : incomingFirmwareMetadata.length;
        final int len = 1 + metadataLen;
        ByteBuffer buffer = ByteBuffer.allocate(len).order(ByteOrder.LITTLE_ENDIAN)
                .put((byte) updateFirmwareImageIndex);
        if (metadataLen != 0) {
            buffer.put(incomingFirmwareMetadata);
        }
        return buffer.array();
    }

    /**
     * Sets the update firmware image index.
     *
     * @param updateFirmwareImageIndex The update firmware image index to set.
     */
    public void setUpdateFirmwareImageIndex(int updateFirmwareImageIndex) {
        this.updateFirmwareImageIndex = updateFirmwareImageIndex;
    }

    /**
     * Sets the incoming firmware metadata.
     *
     * @param incomingFirmwareMetadata The incoming firmware metadata to set.
     */
    public void setIncomingFirmwareMetadata(byte[] incomingFirmwareMetadata) {
        this.incomingFirmwareMetadata = incomingFirmwareMetadata;
    }
}