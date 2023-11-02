/********************************************************************************************************
 * @file BlobChunkTransferMessage.java
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
import com.telink.ble.mesh.util.Arrays;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
/**
 * This class represents a message for transferring a chunk of binary data in a block.
 * It extends the UpdatingMessage class.
 */
public class BlobChunkTransferMessage extends UpdatingMessage {

    /**
     * The chunk's number in a set of chunks in a block.
     * It is represented by 2 bytes.
     */
    private int chunkNumber;

    /**
     * Binary data from the current block.
     */
    private byte[] chunkData;

    /**
     * Constructs a new BlobChunkTransferMessage with the specified destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex The application key index.
     */
    public BlobChunkTransferMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Returns the opcode value for this message.
     *
     * @return The opcode value.
     */
    @Override
    public int getOpcode() {
        return Opcode.BLOB_CHUNK_TRANSFER.value;
    }

    /**
     * Returns the response opcode value for this message.
     *
     * @return The response opcode value.
     */
    @Override
    public int getResponseOpcode() {
        return OPCODE_INVALID;
    }

    /**
     * Returns the parameters of this message as a byte array.
     *
     * @return The parameters as a byte array.
     */
    @Override
    public byte[] getParams() {
        ByteBuffer bf = ByteBuffer.allocate(2 + chunkData.length).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) chunkNumber)
                .put(chunkData);
        return bf.array();
    }

    /**
     * Returns a string representation of this BlobChunkTransferMessage.
     *
     * @return A string representation of the message.
     */
    @Override
    public String toString() {
        return "BlobChunkTransferMessage{" +
                "chunkNumber=" + chunkNumber +
                ", chunkData=" + Arrays.bytesToHexString(chunkData) +
                '}';
    }

    /**
     * Sets the chunk number for this message.
     *
     * @param chunkNumber The chunk number to set.
     */
    public void setChunkNumber(int chunkNumber) {
        this.chunkNumber = chunkNumber;
    }

    /**
     * Sets the chunk data for this message.
     *
     * @param chunkData The chunk data to set.
     */
    public void setChunkData(byte[] chunkData) {
        this.chunkData = chunkData;
    }

    /**
     * Returns a BlobChunkTransferMessage object with the specified parameters.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex The application key index.
     * @param chunkNumber The chunk number.
     * @param chunkData The chunk data.
     * @return The BlobChunkTransferMessage object.
     */
    public static BlobChunkTransferMessage getSimple(int destinationAddress, int appKeyIndex,
                                                     int chunkNumber,
                                                     byte[] chunkData) {
        BlobChunkTransferMessage message = new BlobChunkTransferMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        message.chunkNumber = chunkNumber;
        message.chunkData = chunkData;
        return message;
    }
}