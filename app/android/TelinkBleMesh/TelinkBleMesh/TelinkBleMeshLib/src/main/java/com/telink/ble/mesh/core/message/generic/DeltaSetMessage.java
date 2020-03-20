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

    boolean ack;

    public static DeltaSetMessage getSimple(int destinationAddress, int appKeyIndex, int deltaLevel, boolean ack, int rspMax) {
        DeltaSetMessage deltaSetMessage = new DeltaSetMessage(destinationAddress, appKeyIndex);
        deltaSetMessage.deltaLevel = deltaLevel;
        deltaSetMessage.containsTid = true;
        deltaSetMessage.transitionTime = 0;
        deltaSetMessage.delay = 0;

        deltaSetMessage.ack = ack;
        deltaSetMessage.responseOpcode = Opcode.G_LEVEL_STATUS.value;
        deltaSetMessage.responseMax = rspMax;
        return deltaSetMessage;
    }


    public DeltaSetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }


    @Override
    public int getOpcode() {
        return ack ? Opcode.G_DELTA_SET.value : Opcode.G_DELTA_SET_NOACK.value;
    }

    @Override
    public byte[] getParams() {
        return ByteBuffer.allocate(7).order(ByteOrder.LITTLE_ENDIAN).putInt(deltaLevel)
                .put(tid).put(transitionTime).put(delay).array();
    }

    @Override
    public void setTid(byte tid) {
        this.tid = tid;
    }
}
