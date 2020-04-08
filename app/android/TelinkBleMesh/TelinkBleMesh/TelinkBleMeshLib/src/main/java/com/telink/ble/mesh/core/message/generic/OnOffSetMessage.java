package com.telink.ble.mesh.core.message.generic;

import com.telink.ble.mesh.core.message.Opcode;

/**
 * include on off set and on off set no ack
 * by {@link #ack}
 * Created by kee on 2019/8/14.
 */
public class OnOffSetMessage extends GenericMessage {

    public static final byte ON = 1;

    public static final byte OFF = 0;

    // 1: on, 0: off
    public byte onOff;

    public byte tid = 0;

    public byte transitionTime = 0;

    public byte delay = 0;

    public boolean ack = false;

    public boolean isComplete = false;

    public static OnOffSetMessage getSimple(int address, int appKeyIndex, int onOff, boolean ack, int rspMax) {
        OnOffSetMessage message = new OnOffSetMessage(address, appKeyIndex);
        message.onOff = (byte) onOff;
        message.ack = ack;
        message.setTidPosition(1);
        message.setResponseMax(rspMax);
        return message;
    }

    public OnOffSetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getResponseOpcode() {
        return ack ? Opcode.G_ONOFF_STATUS.value : super.getResponseOpcode();
    }

    @Override
    public int getOpcode() {
        return ack ? Opcode.G_ONOFF_SET.value : Opcode.G_ONOFF_SET_NOACK.value;
    }

    @Override
    public byte[] getParams() {
        return
                isComplete ?
                        new byte[]{
                                this.onOff,
                                this.tid,
                                this.transitionTime,
                                this.delay
                        }
                        :
                        new byte[]{
                                this.onOff,
                                this.tid
                        }
                ;
    }
}
