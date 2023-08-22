/********************************************************************************************************
 * @file FirmwareMetadataCheckMessage.java
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

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * check whether the node can accept a firmware update.
 */
public class FirmwareMetadataCheckMessage extends UpdatingMessage {

    /**
     * Index of the firmware image in the Firmware Information List state to check
     * 1 byte
     */
    private int updateFirmwareImageIndex;

    /**
     * If the value of the Incoming Firmware Metadata Length field is greater than 0,
     * the Incoming Firmware Metadata field shall be present.
     */
    private byte[] incomingFirmwareMetadata;

    public static FirmwareMetadataCheckMessage getSimple(int destinationAddress, int appKeyIndex,
                                                         int index, byte[] incomingFirmwareMetadata) {
        FirmwareMetadataCheckMessage message = new FirmwareMetadataCheckMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        message.updateFirmwareImageIndex = index;
        message.incomingFirmwareMetadata = incomingFirmwareMetadata;

        return message;
    }

    public FirmwareMetadataCheckMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.FIRMWARE_UPDATE_FIRMWARE_METADATA_CHECK.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.FIRMWARE_UPDATE_FIRMWARE_METADATA_STATUS.value;
    }

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

    public void setUpdateFirmwareImageIndex(int updateFirmwareImageIndex) {
        this.updateFirmwareImageIndex = updateFirmwareImageIndex;
    }

    public void setIncomingFirmwareMetadata(byte[] incomingFirmwareMetadata) {
        this.incomingFirmwareMetadata = incomingFirmwareMetadata;
    }
}
