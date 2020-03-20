package com.telink.ble.mesh.core.message.lighting;

import com.telink.ble.mesh.core.message.Opcode;

/**
 * Created by kee on 2019/9/19.
 */

public class CtlGetMessage extends LightingMessage {

    public static CtlGetMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax) {
        CtlGetMessage message = new CtlGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        return message;
    }

    public CtlGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.LIGHT_CTL_GET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.LIGHT_CTL_STATUS.value;
    }
}
