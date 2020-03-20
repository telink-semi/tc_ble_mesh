package com.telink.ble.mesh.core.message.generic;

import com.telink.ble.mesh.core.message.Opcode;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Created by kee on 2019/8/20.
 */

public class MoveSetMessage extends GenericMessage {

    private int deltaLevel;

    private byte tid;

    private byte transitionTime;

    private byte delay;

    boolean ack;

    public MoveSetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }


    @Override
    public int getOpcode() {
        return ack ? Opcode.G_MOVE_SET.value : Opcode.G_MOVE_SET_NOACK.value;
    }

    @Override
    public byte[] getParams() {
        return ByteBuffer.allocate(5)
                .order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) deltaLevel)
                .put(tid).put(transitionTime).put(delay).array();
    }

    @Override
    public void setTid(byte tid) {
        this.tid = tid;
    }
}
