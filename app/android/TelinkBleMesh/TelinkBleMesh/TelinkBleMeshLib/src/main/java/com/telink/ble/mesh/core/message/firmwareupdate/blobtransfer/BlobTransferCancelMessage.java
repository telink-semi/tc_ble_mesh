/********************************************************************************************************
 * @file BlobTransferCancelMessage.java
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
 * This class represents a message used to cancel a blob transfer.
 * It extends the UpdatingMessage class.
 */
public class BlobTransferCancelMessage extends UpdatingMessage {
    /**
     * The identifier of the blob to be cancelled.
     * It is a 64-bit value.
     */
    private long blobId;

    /**
     * Creates a new BlobTransferCancelMessage with the specified destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index of the message.
     */
    public BlobTransferCancelMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Creates a new BlobTransferCancelMessage with the specified destination address, application key index, and blob ID.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index of the message.
     * @param blobId             The ID of the blob to be cancelled.
     * @return The created BlobTransferCancelMessage.
     */
    public static BlobTransferCancelMessage getSimple(int destinationAddress, int appKeyIndex, long blobId) {
        BlobTransferCancelMessage message = new BlobTransferCancelMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        message.blobId = blobId;
        return message;
    }

    /**
     * Gets the opcode of the message.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.BLOB_TRANSFER_CANCEL.value;
    }

    /**
     * Gets the response opcode of the message.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.BLOB_TRANSFER_STATUS.value;
    }

    /**
     * Gets the parameters of the message.
     *
     * @return The parameters as a byte array.
     */
    @Override
    public byte[] getParams() {
        return ByteBuffer.allocate(16).order(ByteOrder.LITTLE_ENDIAN)
                .putLong(blobId)
                .array();
    }

    /**
     * Sets the blob ID of the message.
     *
     * @param blobId The blob ID to be set.
     */
    public void setBlobId(long blobId) {
        this.blobId = blobId;
    }
}