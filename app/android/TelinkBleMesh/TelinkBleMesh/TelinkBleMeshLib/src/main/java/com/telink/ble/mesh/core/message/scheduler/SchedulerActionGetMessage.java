/********************************************************************************************************
 * @file SchedulerActionGetMessage.java
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

/**
 * scheduler action get
 * Created by kee on 2019/8/14.
 */
public class SchedulerActionGetMessage extends GenericMessage {

    // scene id
    private byte index;


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

    public void setIndex(byte index) {
        this.index = index;
    }
}
