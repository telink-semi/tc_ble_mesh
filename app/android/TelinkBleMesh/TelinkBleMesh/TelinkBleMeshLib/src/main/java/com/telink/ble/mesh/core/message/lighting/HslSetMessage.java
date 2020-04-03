package com.telink.ble.mesh.core.message.lighting;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.generic.GenericMessage;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * include HSL set and HSL set no ack
 * by {@link #ack}
 * Created by kee on 2019/8/14.
 */
public class HslSetMessage extends GenericMessage {

    public int lightness;

    public int hue;

    public int saturation;

    // transaction id
    public byte tid = 0;

    public byte transitionTime = 0;

    public byte delay = 0;

    public boolean ack = false;

    public boolean isComplete = false;

    public static HslSetMessage getSimple(int address, int appKeyIndex, int lightness, int hue, int saturation, boolean ack, int rspMax) {
        HslSetMessage message = new HslSetMessage(address, appKeyIndex);
        message.lightness = lightness;
        message.hue = hue;
        message.saturation = saturation;
        message.ack = ack;
        message.setResponseMax(rspMax);
        return message;
    }

    public HslSetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
        setTidPosition(3);
    }

    @Override
    public int getResponseOpcode() {
        return ack ? Opcode.LIGHT_HSL_STATUS.value : super.getResponseOpcode();
    }

    @Override
    public int getOpcode() {
        return ack ? Opcode.LIGHT_HSL_SET.value : Opcode.LIGHT_HSL_SET_NOACK.value;
    }

    @Override
    public byte[] getParams() {
        return
                isComplete ?
                        ByteBuffer.allocate(9).order(ByteOrder.LITTLE_ENDIAN)
                                .putShort((short) lightness)
                                .putShort((short) hue)
                                .putShort((short) saturation)
                                .put(tid)
                                .put(transitionTime)
                                .put(delay).array()
                        :
                        ByteBuffer.allocate(7).order(ByteOrder.LITTLE_ENDIAN)
                                .putShort((short) lightness)
                                .putShort((short) hue)
                                .putShort((short) saturation)
                                .put(tid).array();
    }
}
