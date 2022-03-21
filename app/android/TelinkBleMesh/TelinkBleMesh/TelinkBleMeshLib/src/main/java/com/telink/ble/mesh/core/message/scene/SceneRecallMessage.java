/********************************************************************************************************
 * @file SceneRecallMessage.java
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
package com.telink.ble.mesh.core.message.scene;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.generic.GenericMessage;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * SCENE recall and XX no ack
 * by {@link #ack}
 * Created by kee on 2019/8/14.
 */
public class SceneRecallMessage extends GenericMessage {

    private int sceneNumber;

    // transition id
    private byte tid = 0;

    private byte transitionTime = 0;

    private byte delay = 0;

    private boolean ack = false;

    private boolean isComplete = false;

    public static SceneRecallMessage getSimple(int address, int appKeyIndex, int sceneNumber, boolean ack, int rspMax) {
        SceneRecallMessage message = new SceneRecallMessage(address, appKeyIndex);
        message.sceneNumber = sceneNumber;
        message.ack = ack;
        message.setResponseMax(rspMax);
        return message;
    }

    public SceneRecallMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
        setTidPosition(2);
    }

    @Override
    public int getResponseOpcode() {
        return ack ? Opcode.SCENE_STATUS.value : super.getResponseOpcode();
    }

    @Override
    public int getOpcode() {
        return ack ? Opcode.SCENE_RECALL.value : Opcode.SCENE_RECALL_NOACK.value;
    }

    @Override
    public byte[] getParams() {
        return
                isComplete ?
                        ByteBuffer.allocate(5).order(ByteOrder.LITTLE_ENDIAN)
                                .putShort((short) sceneNumber)
                                .put(tid)
                                .put(transitionTime)
                                .put(delay).array()
                        :
                        ByteBuffer.allocate(3).order(ByteOrder.LITTLE_ENDIAN)
                                .putShort((short) sceneNumber)
                                .put(tid).array();
    }

    public void setSceneNumber(int sceneNumber) {
        this.sceneNumber = sceneNumber;
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
