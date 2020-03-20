package com.telink.ble.mesh.core.message.lighting;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.generic.GenericMessage;

/**
 * include lightness set and lightness set no ack
 * by {@link #ack}
 * Created by kee on 2019/8/14.
 */
public class LightnessSetMessage extends GenericMessage {
    // 2 bytes
    public int lightness;

    // transition id
    public byte tid = 0;

    public byte transitionTime = 0;

    public byte delay = 0;

    public boolean ack = false;

    public static LightnessSetMessage getSimple(int address, int appKeyIndex, int lightness, boolean ack, int rspMax) {
        LightnessSetMessage message = new LightnessSetMessage(address, appKeyIndex);
        message.lightness =  lightness;
        message.ack = ack;
        message.setContainsTid(true);
        message.setResponseMax(rspMax);
        return message;
    }

    public LightnessSetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getResponseOpcode() {
        return ack ? Opcode.LIGHTNESS_STATUS.value : super.getResponseOpcode();
    }

    @Override
    public int getOpcode() {
        return ack ? Opcode.LIGHTNESS_SET.value : Opcode.LIGHTNESS_SET_NOACK.value;
    }

    @Override
    public byte[] getParams() {
        return new byte[]{
                (byte) this.lightness,
                (byte) (this.lightness >> 8),
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
