/********************************************************************************************************
 * @file BlobBlockStartMessage.java
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

public class BlobBlockStartMessage extends UpdatingMessage {


    /**
     * Block number of the incoming block
     * 2 bytes
     */
    private int blockNumber;

    /**
     * Chunk size (in octets) for the incoming block
     * 2 bytes
     */
    private int chunkSize;


    public static BlobBlockStartMessage getSimple(int destinationAddress, int appKeyIndex,
                                                  int blockNumber,
                                                  int chunkSize) {
        BlobBlockStartMessage message = new BlobBlockStartMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        message.blockNumber = blockNumber;
        message.chunkSize = chunkSize;
        return message;
    }

    public BlobBlockStartMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.BLOB_BLOCK_START.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.BLOB_BLOCK_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        return ByteBuffer.allocate(4).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) blockNumber)
                .putShort((short) chunkSize).array();
    }

    public void setBlockNumber(int blockNumber) {
        this.blockNumber = blockNumber;
    }

    public void setChunkSize(int chunkSize) {
        this.chunkSize = chunkSize;
    }
}
