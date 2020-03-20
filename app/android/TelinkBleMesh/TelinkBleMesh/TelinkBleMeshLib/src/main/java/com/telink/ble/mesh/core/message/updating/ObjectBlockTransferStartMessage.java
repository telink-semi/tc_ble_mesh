package com.telink.ble.mesh.core.message.updating;

import com.telink.ble.mesh.core.message.Opcode;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;


public class ObjectBlockTransferStartMessage extends UpdatingMessage {


    private static final int CHECKSUM_ALGORITHM_CRC32 = 0x00;

    /**
     * 8 bytes
     */
    private long objectId;

    private int blockNumber;

    private int chunkSize;

    private byte blockChecksumAlgorithm = CHECKSUM_ALGORITHM_CRC32;

    /**
     * Checksum for image block
     * 4 bytes using crc32
     */
    private int blockChecksumValue;

    /**
     * Block size in bytes (optional)
     * optional when equal to Block Size [Object Transfer Start],
     * mandatory when currently transferred block size is different than Block Size (could only happen in last block).
     * 2 bytes
     */
    private int currentBlockSize;


    public static ObjectBlockTransferStartMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax,
                                                            long objectId,
                                                            int blockNumber,
                                                            int chunkSize,
                                                            int blockChecksumValue,
                                                            int currentBlockSize) {
        ObjectBlockTransferStartMessage message = new ObjectBlockTransferStartMessage(destinationAddress, appKeyIndex);

        message.setResponseMax(rspMax);
        message.objectId = objectId;
        message.blockNumber = blockNumber;
        message.chunkSize = chunkSize;
        message.blockChecksumValue = blockChecksumValue;
        message.currentBlockSize = currentBlockSize;
        return message;
    }

    public ObjectBlockTransferStartMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.OBJ_BLOCK_TRANSFER_START.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.OBJ_BLOCK_TRANSFER_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        ByteBuffer bf = ByteBuffer.allocate(19).order(ByteOrder.LITTLE_ENDIAN)
                .putLong(objectId)
                .putShort((short) blockNumber)
                .putShort((short) chunkSize)
                .put(blockChecksumAlgorithm)
                .putInt(blockChecksumValue)
                .putShort((short) currentBlockSize);
        return bf.array();
    }

    public void setObjectId(long objectId) {
        this.objectId = objectId;
    }

    public void setBlockNumber(int blockNumber) {
        this.blockNumber = blockNumber;
    }

    public void setChunkSize(int chunkSize) {
        this.chunkSize = chunkSize;
    }

    public void setBlockChecksumAlgorithm(byte blockChecksumAlgorithm) {
        this.blockChecksumAlgorithm = blockChecksumAlgorithm;
    }

    public void setBlockChecksumValue(int blockChecksumValue) {
        this.blockChecksumValue = blockChecksumValue;
    }

    public void setCurrentBlockSize(int currentBlockSize) {
        this.currentBlockSize = currentBlockSize;
    }
}
