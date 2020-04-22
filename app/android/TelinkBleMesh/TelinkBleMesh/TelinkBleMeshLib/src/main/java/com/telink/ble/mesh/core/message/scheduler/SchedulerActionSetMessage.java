package com.telink.ble.mesh.core.message.scheduler;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.generic.GenericMessage;
import com.telink.ble.mesh.entity.Scheduler;

/**
 * include Scheduler Action set and Scheduler Action set no ack
 * by {@link #ack}
 * Created by kee on 2019/8/14.
 */
public class SchedulerActionSetMessage extends GenericMessage {

    private Scheduler scheduler;

    private boolean ack = false;

    public static SchedulerActionSetMessage getSimple(int address, int appKeyIndex, Scheduler scheduler, boolean ack, int rspMax) {
        SchedulerActionSetMessage message = new SchedulerActionSetMessage(address, appKeyIndex);
        message.scheduler = scheduler;
        message.ack = ack;
        message.setResponseMax(rspMax);
        return message;
    }

    public SchedulerActionSetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getResponseOpcode() {
        return ack ? Opcode.SCHD_ACTION_STATUS.value : super.getResponseOpcode();
    }

    @Override
    public int getOpcode() {
        return ack ? Opcode.SCHD_ACTION_SET.value : Opcode.SCHD_ACTION_SET_NOACK.value;
    }

    @Override
    public byte[] getParams() {
        return scheduler.toBytes();
    }

    public void setScheduler(Scheduler scheduler) {
        this.scheduler = scheduler;
    }

    public void setAck(boolean ack) {
        this.ack = ack;
    }
}
