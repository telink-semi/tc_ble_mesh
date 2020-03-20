package com.telink.ble.mesh.core.message.lighting;

import com.telink.ble.mesh.core.message.Opcode;

/**
 * Created by kee on 2019/9/19.
 */

public class HslGetMessage extends LightingMessage {

    public static HslGetMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax) {
        HslGetMessage message = new HslGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        return message;
    }

    public HslGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.LIGHT_HSL_GET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.LIGHT_HSL_STATUS.value;
    }
}
