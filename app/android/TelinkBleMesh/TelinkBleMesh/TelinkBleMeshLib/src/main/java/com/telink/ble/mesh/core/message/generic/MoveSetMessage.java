package com.telink.ble.mesh.core.message.generic;

import com.telink.ble.mesh.core.message.Opcode;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Created by kee on 2019/8/20.
 */

public class MoveSetMessage extends GenericMessage {

    public int deltaLevel;

    public byte tid;

    public byte transitionTime;

    public byte delay;

    public boolean ack;

    public boolean isComplete = false;

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
}
