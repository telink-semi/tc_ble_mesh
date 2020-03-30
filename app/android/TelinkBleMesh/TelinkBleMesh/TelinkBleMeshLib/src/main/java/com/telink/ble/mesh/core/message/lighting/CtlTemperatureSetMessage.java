package com.telink.ble.mesh.core.message.lighting;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.generic.GenericMessage;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * include CLT Temperature set and CTL Temperature set no ack
 * by {@link #ack}
 * Created by kee on 2019/8/14.
 */
public class CtlTemperatureSetMessage extends GenericMessage {

    public int temperature;

    public int deltaUV;

    // transition id
    public byte tid = 0;

    public byte transitionTime = 0;

    public byte delay = 0;

    public boolean ack = false;

    public static CtlTemperatureSetMessage getSimple(int address, int appKeyIndex, int temperature, int deltaUV, boolean ack, int rspMax) {
        CtlTemperatureSetMessage message = new CtlTemperatureSetMessage(address, appKeyIndex);
        message.temperature = temperature;
        message.deltaUV = deltaUV;
        message.ack = ack;
        message.setResponseMax(rspMax);
        return message;
    }

    public CtlTemperatureSetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
        setTidPosition(4);
    }

    @Override
    public int getResponseOpcode() {
        return ack ? Opcode.LIGHT_CTL_TEMP_STATUS.value : super.getResponseOpcode();
    }

    @Override
    public int getOpcode() {
        return ack ? Opcode.LIGHT_CTL_TEMP_SET.value : Opcode.LIGHT_CTL_TEMP_SET_NOACK.value;
    }

    @Override
    public byte[] getParams() {
        return ByteBuffer.allocate(7).order(ByteOrder.LITTLE_ENDIAN)
                .putShort((short) temperature)
                .putShort((short) deltaUV)
                .put(tid)
                .put(transitionTime)
                .put(delay).array();
    }

}
