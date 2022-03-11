/********************************************************************************************************
 * @file TimeStatusMessage.java
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

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * Created by kee on 2019/8/20.
 */

public class TimeStatusMessage extends StatusMessage implements Parcelable {

    private static final int DATA_LEN_COMPLETE = 10;

    /**
     * TAI seconds
     * 40 bits
     * The current TAI time in seconds
     * <p>
     * If the TAI Seconds field is 0x0000000000
     * the Subsecond, Uncertainty, Time Authority, TAI-UTC Delta and Time Zone Offset fields shall be omitted;
     * otherwise these fields shall be present.
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
     * tag of is complete message
     */
    private boolean isComplete = false;

    public TimeStatusMessage() {
    }

    protected TimeStatusMessage(Parcel in) {
        taiSeconds = in.readLong();
        subSecond = in.readByte();
        uncertainty = in.readByte();
        timeAuthority = in.readByte();
        delta = in.readInt();
        zoneOffset = in.readInt();
        isComplete = in.readByte() != 0;
    }

    public static final Creator<TimeStatusMessage> CREATOR = new Creator<TimeStatusMessage>() {
        @Override
        public TimeStatusMessage createFromParcel(Parcel in) {
            return new TimeStatusMessage(in);
        }

        @Override
        public TimeStatusMessage[] newArray(int size) {
            return new TimeStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.taiSeconds = MeshUtils.bytes2Long(params, index, 5, ByteOrder.LITTLE_ENDIAN);
        index += 5;
        // tai
        if (params.length == DATA_LEN_COMPLETE) {
            this.isComplete = true;
            this.subSecond = params[index++];
            this.uncertainty = params[index++];
            this.timeAuthority = (byte) (params[index] & 0b01);
            this.delta = (params[index++] & 0xEF) | ((params[index++] & 0xFF) << 8);
            this.zoneOffset = params[index] & 0xFF;
        }
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeLong(taiSeconds);
        dest.writeByte(subSecond);
        dest.writeByte(uncertainty);
        dest.writeByte(timeAuthority);
        dest.writeInt(delta);
        dest.writeInt(zoneOffset);
        dest.writeByte((byte) (isComplete ? 1 : 0));
    }

    public long getTaiSeconds() {
        return taiSeconds;
    }

    public byte getSubSecond() {
        return subSecond;
    }

    public byte getUncertainty() {
        return uncertainty;
    }

    public byte getTimeAuthority() {
        return timeAuthority;
    }

    public int getDelta() {
        return delta;
    }

    public int getZoneOffset() {
        return zoneOffset;
    }

    public boolean isComplete() {
        return isComplete;
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

    public void setComplete(boolean complete) {
        isComplete = complete;
    }
}
