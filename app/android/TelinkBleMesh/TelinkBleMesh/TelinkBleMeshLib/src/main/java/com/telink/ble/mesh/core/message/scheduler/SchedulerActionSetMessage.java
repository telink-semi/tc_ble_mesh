/********************************************************************************************************
 * @file SchedulerActionSetMessage.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date     Sep. 30, 2017
 *
 * @par     Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/
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
