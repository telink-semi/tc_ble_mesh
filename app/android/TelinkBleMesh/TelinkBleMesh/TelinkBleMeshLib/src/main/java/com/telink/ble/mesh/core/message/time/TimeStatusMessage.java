/********************************************************************************************************
 * @file TimeStatusMessage.java
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

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * This class represents a Time Status Message, which is a type of Status Message that contains information about the current time.
 * It extends the StatusMessage class and implements the Parcelable interface for easy serialization and deserialization.
 * The Time Status Message includes fields for the current TAI time in seconds, sub-second time, uncertainty, time authority,
 * TAI-UTC delta, and time zone offset. These fields are optional and may be omitted if the TAI Seconds field is set to 0.
 * The class provides methods for parsing the message from a byte array, as well as getters and setters for accessing and modifying
 * the message fields.
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


    /**
     * Default constructor.
     */
    public TimeStatusMessage() {
    }

    /**
     * Constructor for creating a TimeStatusMessage from a Parcel.
     *
     * @param in The Parcel containing the TimeStatusMessage.
     */
    protected TimeStatusMessage(Parcel in) {
        taiSeconds = in.readLong();
        subSecond = in.readByte();
        uncertainty = in.readByte();
        timeAuthority = in.readByte();
        delta = in.readInt();
        zoneOffset = in.readInt();
        isComplete = in.readByte() != 0;
    }

    /**
     * Creator for generating instances of TimeStatusMessage from a Parcel.
     */
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

    /**
     * Parses the TimeStatusMessage fields from a byte array.
     *
     * @param params The byte array containing the TimeStatusMessage fields.
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.taiSeconds = MeshUtils.bytes2Long(params, index, 5, ByteOrder.LITTLE_ENDIAN);
        index += 5;
        // tai
        if (params.length >= DATA_LEN_COMPLETE) {
            this.isComplete = true;
            this.subSecond = params[index++];
            this.uncertainty = params[index++];
            this.timeAuthority = (byte) (params[index] & 0b01);
            this.delta = (params[index++] & 0xEF) | ((params[index++] & 0xFF) << 8);
            this.zoneOffset = params[index] & 0xFF;
        }
    }

    /**
     * Returns the TAI seconds field of the TimeStatusMessage.
     *
     * @return The TAI seconds.
     */
    public long getTaiSeconds() {
        return taiSeconds;
    }

    /**
     * Returns the sub-second field of the TimeStatusMessage.
     *
     * @return The sub-second time.
     */
    public byte getSubSecond() {
        return subSecond;
    }

    /**
     * Returns the uncertainty field of the TimeStatusMessage.
     *
     * @return The uncertainty.
     */
    public byte getUncertainty() {
        return uncertainty;
    }

    /**
     * Returns the time authority field of the TimeStatusMessage.
     *
     * @return The time authority.
     */
    public byte getTimeAuthority() {
        return timeAuthority;
    }

    /**
     * Returns the TAI-UTC delta field of the TimeStatusMessage.
     *
     * @return The TAI-UTC delta.
     */
    public int getDelta() {
        return delta;
    }

    /**
     * Returns the time zone offset field of the TimeStatusMessage.
     *
     * @return The time zone offset.
     */
    public int getZoneOffset() {
        return zoneOffset;
    }

    /**
     * Returns whether the TimeStatusMessage is complete or not.
     *
     * @return True if the message is complete, false otherwise.
     */
    public boolean isComplete() {
        return isComplete;
    }

    /**
     * Sets the TAI seconds field of the TimeStatusMessage.
     *
     * @param taiSeconds The TAI seconds to set.
     */
    public void setTaiSeconds(long taiSeconds) {
        this.taiSeconds = taiSeconds;
    }

    /**
     * Sets the sub-second field of the TimeStatusMessage.
     *
     * @param subSecond The sub-second time to set.
     */
    public void setSubSecond(byte subSecond) {
        this.subSecond = subSecond;
    }

    /**
     * Sets the uncertainty field of the TimeStatusMessage.
     *
     * @param uncertainty The uncertainty to set.
     */
    public void setUncertainty(byte uncertainty) {
        this.uncertainty = uncertainty;
    }

    /**
     * Sets the time authority field of the TimeStatusMessage.
     *
     * @param timeAuthority The time authority to set.
     */
    public void setTimeAuthority(byte timeAuthority) {
        this.timeAuthority = timeAuthority;
    }

    /**
     * Sets the TAI-UTC delta field of the TimeStatusMessage.
     *
     * @param delta The TAI-UTC delta to set.
     */
    public void setDelta(int delta) {
        this.delta = delta;
    }

    /**
     * Sets the time zone offset field of the TimeStatusMessage.
     *
     * @param zoneOffset The time zone offset to set.
     */
    public void setZoneOffset(int zoneOffset) {
        this.zoneOffset = zoneOffset;
    }

    /**
     * Sets whether the TimeStatusMessage is complete or not.
     *
     * @param complete True if the message is complete, false otherwise.
     */
    public void setComplete(boolean complete) {
        isComplete = complete;
    }

    /**
     * Describes the contents of the TimeStatusMessage for serialization.
     *
     * @return The contents description.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Serializes the TimeStatusMessage to a Parcel.
     *
     * @param dest  The destination Parcel.
     * @param flags Additional flags for the serialization.
     */
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
}