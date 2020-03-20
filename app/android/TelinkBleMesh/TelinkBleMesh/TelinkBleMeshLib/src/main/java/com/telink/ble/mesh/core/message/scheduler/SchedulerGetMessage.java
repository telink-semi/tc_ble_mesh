package com.telink.ble.mesh.core.message.scheduler;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.lighting.LightingMessage;

/**
 * Created by kee on 2019/9/19.
 */

public class SchedulerGetMessage extends LightingMessage {

    public static SchedulerGetMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax) {
        SchedulerGetMessage message = new SchedulerGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        return message;
    }

    public SchedulerGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.SCHD_GET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.SCHD_STATUS.value;
    }
}
