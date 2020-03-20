package com.telink.ble.mesh.core.message.scheduler;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.generic.GenericMessage;

/**
 * scene store
 * Created by kee on 2019/8/14.
 */
public class SchedulerActionGetMessage extends GenericMessage {

    // scene id
    public byte index;


    public static SchedulerActionGetMessage getSimple(int address, int appKeyIndex, byte schedulerIndex, int rspMax) {
        SchedulerActionGetMessage message = new SchedulerActionGetMessage(address, appKeyIndex);
        message.index = schedulerIndex;
        message.setResponseMax(rspMax);
        return message;
    }

    public SchedulerActionGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.SCHD_ACTION_STATUS.value;
    }

    @Override
    public int getOpcode() {
        return Opcode.SCHD_ACTION_GET.value;
    }

    @Override
    public byte[] getParams() {
        return new byte[]{index};
    }

}
