package com.telink.ble.mesh.core.message.generic;

import com.telink.ble.mesh.core.message.Opcode;

/**
 * define level set and level set no ack
 * by {@link #ack}
 * Created by kee on 2019/8/14.
 */
public class LevelSetMessage extends GenericMessage {
    // 1: on, 0: off
    public int level;

    // transition id
    public byte tid = 0;

    public byte transitionTime = 0;

    public byte delay = 0;

    public boolean ack = false;

    public LevelSetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }


    @Override
    public int getOpcode() {
        return ack ? Opcode.G_LEVEL_SET.value : Opcode.G_LEVEL_SET_NOACK.value;
    }

    @Override
    public byte[] getParams() {
        return new byte[]{
                (byte) this.level,
                (byte) (this.level >> 8),
                this.tid,
                this.transitionTime,
                this.delay
        };
    }

    @Override
    public void setTid(byte tid) {
        this.tid = tid;
    }
}