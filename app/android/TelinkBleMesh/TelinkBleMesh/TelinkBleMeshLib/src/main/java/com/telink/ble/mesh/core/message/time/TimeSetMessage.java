/********************************************************************************************************
 * @file TimeSetMessage.java
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

    /**
     * Creates a new TimeSetMessage object with the given destination address and application key index.
     *
     * @param address     The destination address for the message.
     * @param appKeyIndex The application key index for the message.
     */
    public static TimeSetMessage getSimple(int address, int appKeyIndex, long taiSeconds, int zoneOffset, int rspMax) {
        TimeSetMessage message = new TimeSetMessage(address, appKeyIndex);
        message.taiSeconds = taiSeconds;
        message.zoneOffset = zoneOffset;
        message.setResponseMax(rspMax);
        return message;
    }

    /**
     * Creates a new TimeSetMessage object with the given destination address and application key index.
     *
     * @param destinationAddress The destination address for the message.
     * @param appKeyIndex        The application key index for the message.
     */
    public TimeSetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Returns the response opcode for the message.
     * If the ack flag is true, it returns the opcode for the time-status message.
     * Otherwise, it returns an invalid opcode.
     *
     * @return The response opcode.
     */
    @Override
    public int getResponseOpcode() {
        return ack ? Opcode.TIME_STATUS.value : OPCODE_INVALID;
    }

    /**
     * Returns the opcode for the message.
     * If the ack flag is true, it returns the opcode for the time-set message.
     * Otherwise, it returns the opcode for the time-status message.
     *
     * @return The opcode.
     */
    @Override
    public int getOpcode() {
        return ack ? Opcode.TIME_SET.value : Opcode.TIME_STATUS.value;
    }

    /**
     * Returns the parameters of the message as a byte array.
     * The parameters include the TAI seconds, sub-second time, uncertainty, time authority, delta, and zone offset.
     *
     * @return The parameters as a byte array.
     */
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

    /**
     * Sets the TAI seconds parameter of the message.
     *
     * @param taiSeconds The TAI seconds value to set.
     */
    public void setTaiSeconds(long taiSeconds) {
        this.taiSeconds = taiSeconds;
    }

    /**
     * Sets the sub-second parameter of the message.
     *
     * @param subSecond The sub-second value to set.
     */
    public void setSubSecond(byte subSecond) {
        this.subSecond = subSecond;
    }

    /**
     * Sets the uncertainty parameter of the message.
     *
     * @param uncertainty The uncertainty value to set.
     */
    public void setUncertainty(byte uncertainty) {
        this.uncertainty = uncertainty;
    }

    /**
     * Sets the time authority parameter of the message.
     *
     * @param timeAuthority The time authority value to set.
     */
    public void setTimeAuthority(byte timeAuthority) {
        this.timeAuthority = timeAuthority;
    }

    /**
     * Sets the delta parameter of the message.
     *
     * @param delta The delta value to set.
     */
    public void setDelta(int delta) {
        this.delta = delta;
    }

    /**
     * Sets the zone offset parameter of the message.
     *
     * @param zoneOffset The zone offset value to set.
     */
    public void setZoneOffset(int zoneOffset) {
        this.zoneOffset = zoneOffset;
    }

    /**
     * Sets the ack flag of the message.
     *
     * @param ack The ack flag value to set.
     */
    public void setAck(boolean ack) {
        this.ack = ack;
    }
}

