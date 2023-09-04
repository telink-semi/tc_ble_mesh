/********************************************************************************************************
 * @file FDUploadStartMessage.java
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

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * The Firmware Distribution Upload Start message is an acknowledged message sent by a Firmware Distribution Client to start a firmware image upload to a Firmware Distribution Server.
 * The response to a Firmware Distribution Upload Start message is a Firmware Distribution Upload Status message.
 */
public class FDUploadStartMessage extends UpdatingMessage {

    /**
     * Upload TTL
     * Time To Live value used in a firmware image upload
     * 1 byte
     */
    public int uploadTTL;

    /**
     * Upload Timeout Base
     * Used to compute the timeout of the firmware image upload
     * 2 bytes
     */
    public int uploadTimeoutBase;

    /**
     * Upload BLOB ID
     * BLOB identifier for the firmware image
     * 8 bytes
     */
    public long uploadBLOBID;

    /**
     * Upload Firmware Size
     * Firmware image size (in octets)
     * 4 bytes
     */
    public int uploadFirmwareSize;

    /**
     * Upload Firmware Metadata Length
     * Size of the Upload Firmware Metadata field
     * 1 byte
     */
    public int uploadFirmwareMetadataLength;

    /**
     * Upload Firmware Metadata
     * Vendor-specific firmware metadata
     * 1 to 255 bytes
     */
    public byte[] uploadFirmwareMetadata;

    /**
     * Upload Firmware ID
     * The Firmware ID identifying the firmware image being uploaded
     * Variable
     */
    public byte[] uploadFirmwareID;


    public FDUploadStartMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    public static FDUploadStartMessage getSimple(int destinationAddress, int appKeyIndex) {
        FDUploadStartMessage message = new FDUploadStartMessage(destinationAddress, appKeyIndex);

        message.setResponseMax(1);
        return message;
    }

    @Override
    public int getOpcode() {
        return Opcode.FD_UPLOAD_START.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.FD_UPLOAD_STATUS.value;
    }


    @Override
    public byte[] getParams() {
        ByteBuffer buffer = ByteBuffer.allocate(16 + uploadFirmwareMetadata.length + uploadFirmwareID.length).order(ByteOrder.LITTLE_ENDIAN);
        buffer.put((byte) uploadTTL)
                .putShort((short) uploadTimeoutBase)
                .putLong(uploadBLOBID)
                .putInt(uploadFirmwareSize)
                .put((byte) uploadFirmwareMetadataLength)
                .put(uploadFirmwareMetadata)
                .put(uploadFirmwareID);
        return buffer.array();
    }
}
