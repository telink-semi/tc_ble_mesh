/********************************************************************************************************
 * @file BlobTransferStartMessage.java
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
package com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.firmwareupdate.UpdatingMessage;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;


/**
 * This class represents a message used to start a BLOB transfer.
 * The message contains information such as the transfer mode, BLOB identifier, BLOB size, block size, and client MTU size.
 * It extends the UpdatingMessage class.
 */
public class BlobTransferStartMessage extends UpdatingMessage {

    /**
     * The transfer mode for the BLOB transfer.
     * It is represented by the higher 2 bits in the first byte of the message.
     */
    private TransferMode transferMode = TransferMode.PUSH;

    /**
     * The identifier for the BLOB.
     * It is a 64-bit value.
     */
    private long blobId;

    /**
     * The size of the BLOB in octets.
     * It is a 32-bit value.
     */
    private int blobSize;

    /**
     * The block size for the transfer.
     * It is an 8-bit value.
     */
    private int blockSizeLog;

    /**
     * The maximum transmission unit (MTU) size supported by the client.
     */
    private int clientMTUSize;

    /**
     * Constructs a BlobTransferStartMessage object with the specified destination address and application key index.
     *
     * @param destinationAddress The destination address for the message.
     * @param appKeyIndex        The application key index for the message.
     */
    public BlobTransferStartMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

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

    /**
     * Returns the opcode for the message, which is Opcode.BLOB_TRANSFER_START.
     *
     * @return The opcode for the message.
     */
    @Override
    public int getOpcode() {
        return Opcode.BLOB_TRANSFER_START.value;
    }

    /**
     * Returns the response opcode for the message, which is Opcode.BLOB_TRANSFER_STATUS.
     *
     * @return The response opcode for the message.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.BLOB_TRANSFER_STATUS.value;
    }

    /**
     * Returns the parameters of the message as a byte array.
     * The parameters include the transfer mode, blob ID, blob size, block size, and client MTU size.
     *
     * @return The parameters of the message as a byte array.
     */
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

    /**
     * Sets the transfer mode for the BLOB transfer.
     *
     * @param transferMode The transfer mode to set.
     */
    public void setTransferMode(TransferMode transferMode) {
        this.transferMode = transferMode;
    }

    /**
     * Sets the BLOB identifier.
     *
     * @param blobId The BLOB identifier to set.
     */
    public void setBlobId(long blobId) {
        this.blobId = blobId;
    }

    /**
     * Sets the size of the BLOB in octets.
     *
     * @param blobSize The size of the BLOB to set.
     */
    public void setBlobSize(int blobSize) {
        this.blobSize = blobSize;
    }

    /**
     * Sets the block size for the transfer.
     *
     * @param blockSizeLog The block size to set.
     */
    public void setBlockSizeLog(int blockSizeLog) {
        this.blockSizeLog = blockSizeLog;
    }

    /**
     * Sets the maximum transmission unit (MTU) size supported by the client.
     *
     * @param clientMTUSize The client MTU size to set.
     */
    public void setClientMTUSize(int clientMTUSize) {
        this.clientMTUSize = clientMTUSize;
    }
}