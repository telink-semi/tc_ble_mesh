/********************************************************************************************************
 * @file MeshFirmwareParser.java
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
package com.telink.ble.mesh.core.access;

import com.telink.ble.mesh.util.MeshLogger;

import java.util.zip.CRC32;

/**
 * Created by kee on 2019/10/12.
 */

public class MeshFirmwareParser {

//    private static final int DEFAULT_OBJECT_ID = 0x8877665544332211L;

    private static final int DEFAULT_BLOCK_SIZE = 4 * 1024;

    private static final int DEFAULT_CHUNK_SIZE = 256;

    private byte[] firmwareData;

//    private long objectId = DEFAULT_OBJECT_ID;

    private int objectSize;

    private int mBlockSize = DEFAULT_BLOCK_SIZE;

    private int mChunkSize = DEFAULT_CHUNK_SIZE;

    private int curBlockIndex;

    private int curChunkIndex;

    private int totalBlockNumber;

    private int totalChunkNumber;

    private int progress = -1;

    public void reset(byte[] data) {
        this.firmwareData = data;
        this.objectSize = data.length;

        this.curBlockIndex = -1;
        this.curChunkIndex = -1;
        progress = -1;
        totalBlockNumber = (int) Math.ceil(((double) objectSize) / mBlockSize);
        totalChunkNumber = (int) Math.ceil(((double) objectSize) / mChunkSize);
    }

    public void reset(byte[] data, int blockSize, int chunkSize) {
        this.mBlockSize = blockSize;
        this.mChunkSize = chunkSize;
        this.reset(data);
    }


    /**
     * prepare for next block
     *
     * @return if has next
     */
    public int nextBlock() {
        curChunkIndex = -1;
        return ++curBlockIndex;
    }

    public void resetBlock() {
        curChunkIndex = -1;
        MeshLogger.d("reset block : " + curBlockIndex + " -- " + curChunkIndex);
        // curBlockIndex = -1;
    }

    public boolean hasNextBlock() {
        return curBlockIndex + 1 < totalBlockNumber;
    }

    public int getCurBlockSize() {
        int blockSize;

        if (hasNextBlock() || (objectSize % mBlockSize) == 0) {
            // not the last block or last block size is mBlockSize
            blockSize = mBlockSize;
        } else {
            blockSize = objectSize % mBlockSize;
        }
        return blockSize;
    }


    /**
     * refresh progress by chunk/total
     *
     * @return progress
     */
    public boolean validateProgress() {
        // Math.ceil(mBlockSize/mChunkSize)
        final int MAX_PROGRESS = 99; // 99 or 100
        float chunkNumberOffset = curBlockIndex * ((float) mBlockSize / mChunkSize) + (curChunkIndex + 1);
        // max 99
        int progress = (int) (chunkNumberOffset * MAX_PROGRESS / totalChunkNumber);
        if (progress <= this.progress) {
            return false;
        }
        this.progress = progress;
        return true;
    }

    public int getProgressAt(int blockIndex, int chunkIndex) {
        final int MAX_PROGRESS = 99; // 99 or 100
        float chunkNumberOffset = blockIndex * ((float) mBlockSize / mChunkSize) + (chunkIndex + 1);
        return (int) (chunkNumberOffset * MAX_PROGRESS / totalChunkNumber);
    }

    public int getProgress() {
        return progress;
    }

    /**
     * generate next chunk at current block
     *
     * @return chunk-message or null when all chunk sent
     */
    public byte[] nextChunk() {
        int chunkNumber;
        double blockSize = getCurBlockSize();

        chunkNumber = (int) Math.ceil(blockSize / mChunkSize);

        if (curChunkIndex + 1 < chunkNumber) {
            // has next
            curChunkIndex++;
            int chunkSize;
            if (curChunkIndex + 1 < chunkNumber || blockSize % mChunkSize == 0) {
                chunkSize = mChunkSize;
            } else {
                chunkSize = (int) (blockSize % mChunkSize);
            }

            byte[] chunkData = new byte[chunkSize];
            int offset = curBlockIndex * mBlockSize + curChunkIndex * mChunkSize;
            System.arraycopy(firmwareData, offset, chunkData, 0, chunkSize);


            return chunkData;
        } else {
            return null;
        }
    }


    public byte[] chunkAt(int chunkIndex) {
        int chunkNumber;

        double blockSize = getCurBlockSize();

        chunkNumber = (int) Math.ceil(blockSize / mChunkSize);
        if (chunkIndex >= chunkNumber) return null;

        int chunkSize;
        if (chunkIndex + 1 < chunkNumber || blockSize % mChunkSize == 0) {

            chunkSize = mChunkSize;
        } else {
            chunkSize = (int) (blockSize % mChunkSize);
        }

        byte[] chunkData = new byte[chunkSize];
        int offset = curBlockIndex * mBlockSize + chunkIndex * mChunkSize;
        System.arraycopy(firmwareData, offset, chunkData, 0, chunkSize);

        return chunkData;

    }

    public int currentBlockIndex() {
        return curBlockIndex;
    }

    public int currentChunkIndex() {
        return curChunkIndex;
    }

    public int getObjectSize() {
        return objectSize;
    }

    public int getBlockSize() {
        return mBlockSize;
    }

    public int getChunkSize() {
        return mChunkSize;
    }

    /**
     * get current block checksum
     */
    public int getBlockChecksum() {
        int blockSize = getCurBlockSize();
        byte[] blockData = new byte[blockSize];
        System.arraycopy(firmwareData, curBlockIndex * mBlockSize, blockData, 0, blockSize);
        CRC32 crc32 = new CRC32();
        crc32.update(blockData);
        return (int) crc32.getValue();
    }
}
