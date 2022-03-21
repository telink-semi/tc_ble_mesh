/********************************************************************************************************
 * @file TimeSetMessage.java
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
package com.telink.ble.mesh.core.message.time;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.generic.GenericMessage;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * time set
 * Created by kee on 2019/8/14.
 */
public class TimeSetMessage extends GenericMessage {

    /**
     * TAI seconds
     * 40 bits
     * The current TAI time in seconds
     */
    private long taiSeconds;

    /**
     * 8 bits
     * The sub-second time in units of 1/256th second
     */
    private byte subSecond;

    /**
     * 8 bits
     * The estimated uncertainty in 10 millisecond steps
     */
    private byte uncertainty;

    /**
     * 1 bit
     * 0 = No Time Authority, 1 = Time Authority
     */
    private byte timeAuthority;

    /**
     * TAI-UTC Delta
     * 15 bits
     * Current difference between TAI and UTC in seconds
     */
    private int delta;

    /**
     * Time Zone Offset
     * 8 bits
     * The local time zone offset in 15-minute increments
     */
    private int zoneOffset;

    /**
     * no-ack for time-status message
     */
    private boolean ack = false;

    public static TimeSetMessage getSimple(int address, int appKeyIndex, long taiSeconds, int zoneOffset, int rspMax) {
        TimeSetMessage message = new TimeSetMessage(address, appKeyIndex);
        message.taiSeconds = taiSeconds;
        message.zoneOffset = zoneOffset;
        message.setResponseMax(rspMax);
        return message;
    }

    public TimeSetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getResponseOpcode() {
        return ack ? Opcode.TIME_STATUS.value : OPCODE_INVALID;
    }

    @Override
    public int getOpcode() {
        return ack ? Opcode.TIME_SET.value : Opcode.TIME_STATUS.value;
    }

    @Override
    public byte[] getParams() {
        ByteBuffer byteBuffer = ByteBuffer.allocate(10).order(ByteOrder.LITTLE_ENDIAN);
        byteBuffer.put((byte) (taiSeconds))
                .put((byte) (taiSeconds >> 8))
                .put((byte) (taiSeconds >> 16))
                .put((byte) (taiSeconds >> 24))
                .put((byte) (taiSeconds >> 32))
                .put(subSecond)
                .put(uncertainty)
                .putShort((short) ((delta << 1) | timeAuthority))
                .put((byte) (zoneOffset));
        return byteBuffer.array();
    }

    public void setTaiSeconds(long taiSeconds) {
        this.taiSeconds = taiSeconds;
    }

    public void setSubSecond(byte subSecond) {
        this.subSecond = subSecond;
    }

    public void setUncertainty(byte uncertainty) {
        this.uncertainty = uncertainty;
    }

    public void setTimeAuthority(byte timeAuthority) {
        this.timeAuthority = timeAuthority;
    }

    public void setDelta(int delta) {
        this.delta = delta;
    }

    public void setZoneOffset(int zoneOffset) {
        this.zoneOffset = zoneOffset;
    }

    public void setAck(boolean ack) {
        this.ack = ack;
    }
}
