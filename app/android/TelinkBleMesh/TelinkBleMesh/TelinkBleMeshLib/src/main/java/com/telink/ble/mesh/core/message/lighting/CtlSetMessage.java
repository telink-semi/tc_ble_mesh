package com.telink.ble.mesh.core.message.lighting;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.generic.GenericMessage;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * include CLT set and CTL set no ack
 * by {@link #ack}
 * Created by kee on 2019/8/14.
 */
public class CtlSetMessage extends GenericMessage {

    public int lightness;

    public int temperature;

    public int deltaUV;

    // transition id
    public byte tid = 0;

    public byte transitionTime = 0;

    public byte delay = 0;

    public boolean ack = false;

    public static CtlSetMessage getSimple(int address, int appKeyIndex, int lightness, int temperature, int deltaUV, boolean ack, int rspMax) {
        CtlSetMessage message = new CtlSetMessage(address, appKeyIndex);
        message.lightness = lightness;
        message.temperature = temperature;
        message.deltaUV = deltaUV;
        message.ack = ack;
        message.setResponseMax(rspMax);
        return message;
    }

    public CtlSetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
        setTidPosition(6);
    }

    @Override
    public int getResponseOpcode() {
        return ack ? Opcode.LIGHT_CTL_STATUS.value : super.getResponseOpcode();
    }

    @Override
    public int getOpcode() {
        return ack ? Opcode.LIGHT_CTL_SET.value : Opcode.LIGHT_CTL_SET_NOACK.value;
    }

    @Override
    public byte[] getParams() {
        return ByteBuffer.allocate(9).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) lightness)
                .putShort((short) temperature)
                .putShort((short) deltaUV)
                .put(tid)
                .put(transitionTime)
                .put(delay).array();
    }
}
