/********************************************************************************************************
 * @file BlobTransferStartMessage.java
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
package com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.firmwareupdate.UpdatingMessage;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class BlobTransferStartMessage extends UpdatingMessage {

    /**
     * BLOB transfer mode
     * higher 2 bits in first byte
     */
    private TransferMode transferMode = TransferMode.PUSH;

    /**
     * BLOB identifier
     * 64 bits
     */
    private long blobId;

    /**
     * BLOB size in octets
     * 32 bits
     */
    private int blobSize;

    /**
     * Indicates the block size
     * 8 bits
     */
    private int blockSizeLog;

    /**
     * MTU size supported by the client
     */
    private int clientMTUSize;


    public static BlobTransferStartMessage getSimple(int destinationAddress, int appKeyIndex,
                                                     long blobId,
                                                     int blobSize,
                                                     byte blockSizeLog,
                                                     int clientMTUSize) {
        BlobTransferStartMessage message = new BlobTransferStartMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        message.blobId = blobId;
        message.blobSize = blobSize;
        message.blockSizeLog = blockSizeLog;
        message.clientMTUSize = clientMTUSize;
        return message;
    }

    public BlobTransferStartMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.BLOB_TRANSFER_START.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.BLOB_TRANSFER_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        final byte modeValue = (byte) (transferMode.value << 6);

        return ByteBuffer.allocate(16).order(ByteOrder.LITTLE_ENDIAN)
                .put(modeValue)
                .putLong(blobId)
                .putInt(blobSize)
                .put((byte) blockSizeLog)
                .putShort((short) clientMTUSize).array();
    }

    public void setTransferMode(TransferMode transferMode) {
        this.transferMode = transferMode;
    }

    public void setBlobId(long blobId) {
        this.blobId = blobId;
    }

    public void setBlobSize(int blobSize) {
        this.blobSize = blobSize;
    }

    public void setBlockSizeLog(int blockSizeLog) {
        this.blockSizeLog = blockSizeLog;
    }

    public void setClientMTUSize(int clientMTUSize) {
        this.clientMTUSize = clientMTUSize;
    }
}
