/********************************************************************************************************
 * @file CtlTemperatureSetMessage.java
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

    private int temperature;

    private int deltaUV;

    // transaction id
    private byte tid = 0;

    private byte transitionTime = 0;

    private byte delay = 0;

    private boolean ack = false;

    private boolean isComplete = false;

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
        return
                isComplete ?
                        ByteBuffer.allocate(7).order(ByteOrder.LITTLE_ENDIAN)
                                .putShort((short) temperature)
                                .putShort((short) deltaUV)
                                .put(tid)
                                .put(transitionTime)
                                .put(delay).array()
                        :
                        ByteBuffer.allocate(5).order(ByteOrder.LITTLE_ENDIAN)
                                .putShort((short) temperature)
                                .putShort((short) deltaUV)
                                .put(tid).array();
    }

    public void setTemperature(int temperature) {
        this.temperature = temperature;
    }

    public void setDeltaUV(int deltaUV) {
        this.deltaUV = deltaUV;
    }

    public void setTid(byte tid) {
        this.tid = tid;
    }

    public void setTransitionTime(byte transitionTime) {
        this.transitionTime = transitionTime;
    }

    public void setDelay(byte delay) {
        this.delay = delay;
    }

    public void setAck(boolean ack) {
        this.ack = ack;
    }

    public void setComplete(boolean complete) {
        isComplete = complete;
    }
}
