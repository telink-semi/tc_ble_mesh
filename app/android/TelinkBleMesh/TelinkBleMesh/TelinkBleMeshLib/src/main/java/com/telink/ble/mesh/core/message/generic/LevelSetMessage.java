/********************************************************************************************************
 * @file LevelSetMessage.java
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
 * define level set and level set no ack
 * by {@link #ack}
 * Created by kee on 2019/8/14.
 */
public class LevelSetMessage extends GenericMessage {
    // 1: on, 0: off
    private int level;

    private byte tid = 0;

    private byte transitionTime = 0;

    private byte delay = 0;

    private boolean ack = false;

    /**
     * is complete message with optional params filled
     */
    private boolean isComplete = false;

    public LevelSetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
        setTidPosition(2);
    }


    @Override
    public int getOpcode() {
        return ack ? Opcode.G_LEVEL_SET.value : Opcode.G_LEVEL_SET_NOACK.value;
    }

    @Override
    public byte[] getParams() {
        return isComplete ?
                new byte[]{
                        (byte) this.level,
                        (byte) (this.level >> 8),
                        this.tid,
                        this.transitionTime,
                        this.delay
                }
                :
                new byte[]{
                        (byte) this.level,
                        (byte) (this.level >> 8),
                        this.tid
                };
    }

    public void setLevel(int level) {
        this.level = level;
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
