package com.telink.ble.mesh.core.message.updating;

import com.telink.ble.mesh.core.message.Opcode;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class ObjectBlockGetMessage extends UpdatingMessage {
    /**
     * 8 bytes
     */
    private long objectId;

    /**
     * 2 bytes
     */
    private int blockNumber;

    public static ObjectBlockGetMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax,
                                                  long objectId,
                                                  int blockNumber) {
        ObjectBlockGetMessage message = new ObjectBlockGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        message.objectId = objectId;
        message.blockNumber = blockNumber;
        return message;
    }

    public ObjectBlockGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.OBJ_BLOCK_GET.value;
    }

    @Override
    public byte[] getParams() {
        ByteBuffer bf = ByteBuffer.allocate(10).order(ByteOrder.LITTLE_ENDIAN)
                .putLong(objectId)
                .putShort((short) blockNumber);
        return bf.array();
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.OBJ_BLOCK_STATUS.value;
    }

    public void setObjectId(long objectId) {
        this.objectId = objectId;
    }

    public void setBlockNumber(int blockNumber) {
        this.blockNumber = blockNumber;
    }
}
