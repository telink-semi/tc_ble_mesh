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

public class BlobChunkTransferMessage extends UpdatingMessage {


    /**
     * The chunk’s number in a set of chunks in a block
     * 2 bytes
     */
    private int chunkNumber;

    /**
     * Binary data from the current block
     */
    private byte[] chunkData;


    public static BlobChunkTransferMessage getSimple(int destinationAddress, int appKeyIndex,
                                                     int chunkNumber,
                                                     byte[] chunkData) {
        BlobChunkTransferMessage message = new BlobChunkTransferMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        message.chunkNumber = chunkNumber;
        message.chunkData = chunkData;
        return message;
    }

    public BlobChunkTransferMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.BLOB_CHUNK_TRANSFER.value;
    }

    @Override
    public int getResponseOpcode() {
        return OPCODE_INVALID;
    }

    @Override
    public byte[] getParams() {
        ByteBuffer bf = ByteBuffer.allocate(2 + chunkData.length).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) chunkNumber)
                .put(chunkData);
        return bf.array();
    }

    @Override
    public String toString() {
        return "BlobChunkTransferMessage{" +
                "chunkNumber=" + chunkNumber +
                ", chunkData=" + Arrays.bytesToHexString(chunkData) +
                '}';
    }

    public void setChunkNumber(int chunkNumber) {
        this.chunkNumber = chunkNumber;
    }

    public void setChunkData(byte[] chunkData) {
        this.chunkData = chunkData;
    }
}
