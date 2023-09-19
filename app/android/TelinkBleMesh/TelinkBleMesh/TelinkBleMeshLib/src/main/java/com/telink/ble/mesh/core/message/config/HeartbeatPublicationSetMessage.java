/********************************************************************************************************
 * @file HeartbeatPublicationSetMessage.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2017
 *
 * @par Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
package com.telink.ble.mesh.core.message.config;

import com.telink.ble.mesh.core.message.Opcode;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * The Config Heartbeat Publication Set is an acknowledged message used to set the current Heartbeat Publication state of an element
 * <p>
 * The response to a Config Heartbeat Publication Set message is a Config Heartbeat Publication Status message.
 */
public class HeartbeatPublicationSetMessage extends ConfigMessage {

    private int destination;

    private byte countLog;

    private byte periodLog;

    private byte hbTtl;

    // 2 bytes
    private int features;

    // 2 bytes
    private int netKeyIndex;

    public HeartbeatPublicationSetMessage(int destinationAddress) {
        super(destinationAddress);
    }

    @Override
    public int getOpcode() {
        return Opcode.HEARTBEAT_PUB_SET.value;
    }

    @Override
    public byte[] getParams() {
        ByteBuffer byteBuffer = ByteBuffer.allocate(9).order(ByteOrder.LITTLE_ENDIAN);
        byteBuffer.putShort((short) destination)
                .put(countLog)
                .put(periodLog)
                .put(hbTtl)
                .putShort((short) features)
                .putShort((short) netKeyIndex);
        return byteBuffer.array();
    }

    public void setDestination(int destination) {
        this.destination = destination;
    }

    public void setCountLog(byte countLog) {
        this.countLog = countLog;
    }

    public void setPeriodLog(byte periodLog) {
        this.periodLog = periodLog;
    }

    public void setHbTtl(byte hbTtl) {
        this.hbTtl = hbTtl;
    }

    public void setFeatures(int features) {
        this.features = features;
    }

    public void setNetKeyIndex(int netKeyIndex) {
        this.netKeyIndex = netKeyIndex;
    }
}
