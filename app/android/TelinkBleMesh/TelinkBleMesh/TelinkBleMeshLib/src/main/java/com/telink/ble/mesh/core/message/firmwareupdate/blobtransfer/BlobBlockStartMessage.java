/********************************************************************************************************
 * @file BlobBlockStartMessage.java
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
 * This class represents a message used to start the transfer of a blob block.
 * It extends the UpdatingMessage class.
 * The message contains the block number and chunk size of the incoming block.
 */
public class BlobBlockStartMessage extends UpdatingMessage {
    /**
     * The block number of the incoming block.
     * It is represented by 2 bytes.
     */
    private int blockNumber;

    /**
     * The chunk size (in octets) for the incoming block.
     * It is represented by 2 bytes.
     */
    private int chunkSize;

    /**
     * Creates a new BlobBlockStartMessage with the specified destination address and application key index.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index of the message.
     */
    public BlobBlockStartMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Returns the opcode of the message.
     *
     * @return The opcode of the message.
     */
    @Override
    public int getOpcode() {
        return Opcode.BLOB_BLOCK_START.value;
    }

    /**
     * Returns the opcode of the response message.
     *
     * @return The opcode of the response message.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.BLOB_BLOCK_STATUS.value;
    }

    /**
     * Returns the parameters of the message as a byte array.
     *
     * @return The parameters of the message as a byte array.
     */
    @Override
    public byte[] getParams() {
        return ByteBuffer.allocate(4).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) blockNumber)
                .putShort((short) chunkSize).array();
    }

    /**
     * Sets the block number of the incoming block.
     *
     * @param blockNumber The block number of the incoming block.
     */
    public void setBlockNumber(int blockNumber) {
        this.blockNumber = blockNumber;
    }

    /**
     * Sets the chunk size of the incoming block.
     *
     * @param chunkSize The chunk size of the incoming block.
     */
    public void setChunkSize(int chunkSize) {
        this.chunkSize = chunkSize;
    }

    /**
     * Creates and returns a simple BlobBlockStartMessage with the specified destination address, application key index,
     * block number, and chunk size.
     *
     * @param destinationAddress The destination address of the message.
     * @param appKeyIndex        The application key index of the message.
     * @param blockNumber        The block number of the incoming block.
     * @param chunkSize          The chunk size of the incoming block.
     * @return A simple BlobBlockStartMessage with the specified parameters.
     */
    public static BlobBlockStartMessage getSimple(int destinationAddress, int appKeyIndex,
                                                  int blockNumber,
                                                  int chunkSize) {
        BlobBlockStartMessage message = new BlobBlockStartMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        message.blockNumber = blockNumber;
        message.chunkSize = chunkSize;
        return message;
    }
}