package com.telink.ble.mesh.core.message.time;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.lighting.LightingMessage;

/**
 * Created by kee on 2019/9/19.
 */

public class TimeGetMessage extends LightingMessage {

    public static TimeGetMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax) {
        TimeGetMessage message = new TimeGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        return message;
    }

    public TimeGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.TIME_STATUS.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.TIME_STATUS.value;
    }
}
