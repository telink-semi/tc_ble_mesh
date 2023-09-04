/********************************************************************************************************
 * @file FirmwareUpdateStartMessage.java
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

public class FirmwareUpdateStartMessage extends UpdatingMessage {

    /**
     * default ttl
     */
    public static final byte DEFAULT_UPDATE_TTL = 10;

    /**
     * Time To Live value to use during firmware image transfer
     * 1 byte
     */
    public byte updateTtl = DEFAULT_UPDATE_TTL;

    /**
     * Used to compute the timeout of the firmware image transfer
     * Client Timeout = [12,000 * (Client Timeout Base + 1) + 100 * Transfer TTL] milliseconds
     * using blockSize
     * 2 bytes
     */
    public int updateTimeoutBase;

    /**
     * BLOB identifier for the firmware image
     * 8 bytes
     */
    public long updateBLOBID;

    /**
     * Index of the firmware image in the Firmware Information List state to be updated
     * 1 byte
     */
    public int updateFirmwareImageIndex;

    /**
     * Vendor-specific firmware metadata
     * If the value of the Incoming Firmware Metadata Length field is greater than 0,
     * the Incoming Firmware Metadata field shall be present.
     * 1-255 bytes
     */
    public byte[] metadata;

    public static FirmwareUpdateStartMessage getSimple(int destinationAddress, int appKeyIndex,
                                                       int updateTimeoutBase, long blobId, byte[] metadata) {
        FirmwareUpdateStartMessage message = new FirmwareUpdateStartMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        message.updateTimeoutBase = updateTimeoutBase;
        message.updateBLOBID = blobId;
        message.updateFirmwareImageIndex = 0;
        message.metadata = metadata;
        return message;
    }

    public FirmwareUpdateStartMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.FIRMWARE_UPDATE_START.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.FIRMWARE_UPDATE_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        final int len = 12 + metadata.length;
        ByteBuffer bf = ByteBuffer.allocate(len).order(ByteOrder.LITTLE_ENDIAN)
                .put(updateTtl).putShort((short) updateTimeoutBase)
                .putLong(updateBLOBID).put((byte) updateFirmwareImageIndex).put(metadata);
        return bf.array();
    }

    public byte getUpdateTtl() {
        return updateTtl;
    }

    public void setUpdateTtl(byte updateTtl) {
        this.updateTtl = updateTtl;
    }

    public int getUpdateTimeoutBase() {
        return updateTimeoutBase;
    }

    public void setUpdateTimeoutBase(int updateTimeoutBase) {
        this.updateTimeoutBase = updateTimeoutBase;
    }

    public long getUpdateBLOBID() {
        return updateBLOBID;
    }

    public void setUpdateBLOBID(long updateBLOBID) {
        this.updateBLOBID = updateBLOBID;
    }

    public int getUpdateFirmwareImageIndex() {
        return updateFirmwareImageIndex;
    }

    public void setUpdateFirmwareImageIndex(int updateFirmwareImageIndex) {
        this.updateFirmwareImageIndex = updateFirmwareImageIndex;
    }

    public byte[] getMetadata() {
        return metadata;
    }

    public void setMetadata(byte[] metadata) {
        this.metadata = metadata;
    }
}
