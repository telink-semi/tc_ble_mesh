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

    private boolean ack;

    private boolean isComplete = false;

    public MoveSetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
        setTidPosition(2);
    }


    @Override
    public int getOpcode() {
        return ack ? Opcode.G_MOVE_SET.value : Opcode.G_MOVE_SET_NOACK.value;
    }

    @Override
    public byte[] getParams() {
        return
                isComplete ?
                        ByteBuffer.allocate(5)
                                .order(ByteOrder.LITTLE_ENDIAN)
                                .putShort((short) deltaLevel)
                                .put(tid).put(transitionTime).put(delay).array()
                        :
                        ByteBuffer.allocate(3)
                                .order(ByteOrder.LITTLE_ENDIAN)
                                .putShort((short) deltaLevel)
                                .put(tid).array();
    }

    public void setDeltaLevel(int deltaLevel) {
        this.deltaLevel = deltaLevel;
    }

    public void setTid(byte tid) {
        this.tid = tid;
    }

    public void setTransitionTime(byte transitionTime) {
        this.transitionTime = transitionTime;
    }

    public void setDelay(byte delay) {
        this.delay = delay;
    }

    public void setAck(boolean ack) {
        this.ack = ack;
    }

    public void setComplete(boolean complete) {
        isComplete = complete;
    }
}
