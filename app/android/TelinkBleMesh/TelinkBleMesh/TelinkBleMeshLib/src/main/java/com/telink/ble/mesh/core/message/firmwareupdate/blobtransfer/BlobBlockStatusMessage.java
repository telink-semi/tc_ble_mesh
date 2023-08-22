/********************************************************************************************************
 * @file BlobBlockStatusMessage.java
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

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.List;

public class BlobBlockStatusMessage extends StatusMessage implements Parcelable {

    /**
     * All chunks in the block are missing.
     */
    public static final int FORMAT_ALL_CHUNKS_MISSING = 0x00;

    /**
     * All chunks in the block have been received.
     */
    public static final int FORMAT_NO_CHUNKS_MISSING = 0x01;

    /**
     * At least one chunk has been received and at least one chunk is missing.
     */
    public static final int FORMAT_SOME_CHUNKS_MISSING = 0x02;

    /**
     * List of chunks requested by the server
     */
    public static final int FORMAT_ENCODED_MISSING_CHUNKS = 0x03;

    /**
     * Status Code for the requesting message
     * lower 4 bits in first byte
     *
     * @see TransferStatus
     */
    private int status;

    /**
     * Indicates the format used to report missing chunks
     * higher 2 bits in first byte
     */
    private int format;

    /**
     * Transfer phase
     */
    // remote in R06 -- 20200618
//    private int transferPhase;

    /**
     * Block number of the current block
     * 16 bits
     */
    private int blockNumber;

    /**
     * Chunk Size (in octets) for the current block
     * 16 bits
     */
    private int chunkSize;

    /**
     * Bit field of missing chunks for this block
     */
    private List<Integer> missingChunks;

    /**
     * List of chunks requested by the server
     * using UTF-8
     */
    private List<Integer> encodedMissingChunks;


    public BlobBlockStatusMessage() {
    }


    protected BlobBlockStatusMessage(Parcel in) {
        status = in.readInt();
        format = in.readInt();
//        transferPhase = in.readInt();
        blockNumber = in.readInt();
        chunkSize = in.readInt();
        missingChunks = new ArrayList<>();
        in.readList(missingChunks, null);
        encodedMissingChunks = new ArrayList<>();
        in.readList(encodedMissingChunks, null);
    }

    public static final Creator<BlobBlockStatusMessage> CREATOR = new Creator<BlobBlockStatusMessage>() {
        @Override
        public BlobBlockStatusMessage createFromParcel(Parcel in) {
            return new BlobBlockStatusMessage(in);
        }

        @Override
        public BlobBlockStatusMessage[] newArray(int size) {
            return new BlobBlockStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.status = params[index] & 0x0F;
        this.format = (params[index++] >> 6) & 0x03;
//        transferPhase = params[index++] & 0xFF;
        this.blockNumber = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        this.chunkSize = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;

        if (format == FORMAT_SOME_CHUNKS_MISSING) {
            missingChunks = MeshUtils.parseMissingBitField(params, index);
        } else if (format == FORMAT_ENCODED_MISSING_CHUNKS) {
            encodedMissingChunks = new ArrayList<>();
            byte[] missing = new byte[params.length - index];
            System.arraycopy(params, index, missing, 0, missing.length);
            String decodeMissingChunks = new String(missing, Charset.forName("UTF-8"));
            for (char c : decodeMissingChunks.toCharArray()) {
                encodedMissingChunks.add(c & 0xFFFF);
            }
        }
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(status);
        dest.writeInt(format);
//        dest.writeInt(transferPhase);
        dest.writeInt(blockNumber);
        dest.writeInt(chunkSize);
        dest.writeList(missingChunks);
        dest.writeList(encodedMissingChunks);
    }

    public int getStatus() {
        return status;
    }

    public int getFormat() {
        return format;
    }

    public int getBlockNumber() {
        return blockNumber;
    }

    public long getChunkSize() {
        return chunkSize;
    }

    public List<Integer> getMissingChunks() {
        return missingChunks;
    }

    public List<Integer> getEncodedMissingChunks() {
        return encodedMissingChunks;
    }

    public void setStatus(int status) {
        this.status = status;
    }

    public void setFormat(int format) {
        this.format = format;
    }

    public void setBlockNumber(int blockNumber) {
        this.blockNumber = blockNumber;
    }

    public void setChunkSize(int chunkSize) {
        this.chunkSize = chunkSize;
    }

    public void setMissingChunks(List<Integer> missingChunks) {
        this.missingChunks = missingChunks;
    }

    public void setEncodedMissingChunks(List<Integer> encodedMissingChunks) {
        this.encodedMissingChunks = encodedMissingChunks;
    }

    @Override
    public String toString() {
        return "BlobBlockStatusMessage{" +
                "status=" + status +
                ", format=" + format +
                ", blockNumber=" + blockNumber +
                ", chunkSize=" + chunkSize +
                ", missingChunks=" + (missingChunks == null ? "null" : missingChunks.size()) +
                ", encodedMissingChunks=" + (encodedMissingChunks == null ? "null" : encodedMissingChunks.size()) +
                '}';
    }
}
