package com.telink.ble.mesh.core.message.lighting;

import com.telink.ble.mesh.core.message.Opcode;

/**
 * Created by kee on 2019/9/19.
 */

public class CtlTemperatureGetMessage extends LightingMessage {

    public static CtlTemperatureGetMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax) {
        CtlTemperatureGetMessage message = new CtlTemperatureGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        return message;
    }

    public CtlTemperatureGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.LIGHT_CTL_TEMP_GET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.LIGHT_CTL_TEMP_STATUS.value;
    }
}
