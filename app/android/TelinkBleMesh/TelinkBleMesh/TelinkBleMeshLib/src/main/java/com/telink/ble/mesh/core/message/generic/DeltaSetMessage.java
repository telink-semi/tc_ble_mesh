package com.telink.ble.mesh.core.message.generic;

import com.telink.ble.mesh.core.message.Opcode;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Created by kee on 2019/8/20.
 */

public class DeltaSetMessage extends GenericMessage {

    private int deltaLevel;

    private byte tid;

    private byte transitionTime;

    private byte delay;

    private boolean ack;

    private boolean isComplete = false;


    public static DeltaSetMessage getSimple(int destinationAddress, int appKeyIndex, int deltaLevel, boolean ack, int rspMax) {
        DeltaSetMessage deltaSetMessage = new DeltaSetMessage(destinationAddress, appKeyIndex);
        deltaSetMessage.deltaLevel = deltaLevel;
        deltaSetMessage.transitionTime = 0;
        deltaSetMessage.delay = 0;

        deltaSetMessage.ack = ack;
        deltaSetMessage.responseOpcode = Opcode.G_LEVEL_STATUS.value;
        deltaSetMessage.responseMax = rspMax;
        return deltaSetMessage;
    }


    public DeltaSetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
        setTidPosition(4);
    }


    @Override
    public int getOpcode() {
        return ack ? Opcode.G_DELTA_SET.value : Opcode.G_DELTA_SET_NOACK.value;
    }

    @Override
    public byte[] getParams() {
        return
                isComplete ?
                        ByteBuffer.allocate(7).order(ByteOrder.LITTLE_ENDIAN).putInt(deltaLevel)
                                .put(tid).put(transitionTime).put(delay).array()
                        :
                        ByteBuffer.allocate(5).order(ByteOrder.LITTLE_ENDIAN).putInt(deltaLevel)
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
