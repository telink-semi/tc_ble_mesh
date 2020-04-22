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
        return ByteBuffer.allocate(16).order(ByteOrder.LITTLE_ENDIAN)
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
