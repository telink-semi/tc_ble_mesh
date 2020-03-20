package com.telink.ble.mesh.core.message.updating;

import com.telink.ble.mesh.core.message.Opcode;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;


public class ObjectTransferStartMessage extends UpdatingMessage {


    /**
     * 8 bytes
     */
    private long objectId;

    /**
     * 4 bytes
     */
    private int objectSize;

    /**
     * Size of the block during this transfer
     */
    private byte blockSizeLog;


    public static ObjectTransferStartMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax,
                                                       long objectId,
                                                       int objectSize,
                                                       byte blockSizeLog) {
        ObjectTransferStartMessage message = new ObjectTransferStartMessage(destinationAddress, appKeyIndex);

        message.setResponseMax(rspMax);
        message.objectId = objectId;
        message.objectSize = objectSize;
        message.blockSizeLog = blockSizeLog;
        return message;
    }

    public ObjectTransferStartMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.OBJ_TRANSFER_START.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.OBJ_TRANSFER_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        ByteBuffer bf = ByteBuffer.allocate(13).order(ByteOrder.LITTLE_ENDIAN)
                .putLong(objectId)
                .putInt(objectSize)
                .put(blockSizeLog);
        return bf.array();
    }

    public void setObjectId(long objectId) {
        this.objectId = objectId;
    }

    public void setObjectSize(int objectSize) {
        this.objectSize = objectSize;
    }

    public void setBlockSizeLog(byte blockSizeLog) {
        this.blockSizeLog = blockSizeLog;
    }
}
