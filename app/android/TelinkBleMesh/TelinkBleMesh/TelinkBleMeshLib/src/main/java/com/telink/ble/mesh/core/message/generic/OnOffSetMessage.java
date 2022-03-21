/********************************************************************************************************
 * @file OnOffSetMessage.java
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
package com.telink.ble.mesh.core.message.generic;

import com.telink.ble.mesh.core.message.Opcode;

/**
 * include on off set and on off set no ack
 * by {@link #ack}
 * Created by kee on 2019/8/14.
 */
public class OnOffSetMessage extends GenericMessage {

    public static final byte ON = 1;

    public static final byte OFF = 0;

    // 1: on, 0: off
    private byte onOff;

    private byte tid = 0;

    private byte transitionTime = 0;

    private byte delay = 0;

    private boolean ack = false;

    private boolean isComplete = false;

    public static OnOffSetMessage getSimple(int address, int appKeyIndex, int onOff, boolean ack, int rspMax) {
        OnOffSetMessage message = new OnOffSetMessage(address, appKeyIndex);
        message.onOff = (byte) onOff;
        message.ack = ack;
        message.setTidPosition(1);
        message.setResponseMax(rspMax);
        // for test
        //        message.ack = false;
        //        message.isComplete = true;
        return message;
    }

    public OnOffSetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getResponseOpcode() {
        return ack ? Opcode.G_ONOFF_STATUS.value : super.getResponseOpcode();
    }

    @Override
    public int getOpcode() {
        return ack ? Opcode.G_ONOFF_SET.value : Opcode.G_ONOFF_SET_NOACK.value;
    }

    @Override
    public byte[] getParams() {
        /*byte[] realParams = isComplete ?
                new byte[]{
                        this.onOff,
                        this.tid,
                        this.transitionTime,
                        this.delay
                }
                :
                new byte[]{
                        this.onOff,
                        this.tid
                }
                ;
        byte[] params = new byte[378];
        System.arraycopy(realParams, 0, params, 0, realParams.length);
        return params;*/
        return
                isComplete ?
                        new byte[]{
                                this.onOff,
                                this.tid,
                                this.transitionTime,
                                this.delay
                        }
                        :
                        new byte[]{
                                this.onOff,
                                this.tid
                        }
                ;
    }

    public void setOnOff(byte onOff) {
        this.onOff = onOff;
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
