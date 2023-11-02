/********************************************************************************************************
 * @file HslTargetStatusMessage.java
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
package com.telink.ble.mesh.core.message.lighting;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;


/**
 * This class represents a status message for the HSL target information. It extends the StatusMessage class and implements the Parcelable interface.
 * The target information includes the target lightness, hue, saturation, remaining time, and a flag indicating if the message is complete.
 */
public class HslTargetStatusMessage extends StatusMessage implements Parcelable {
    private static final int DATA_LEN_COMPLETE = 7;
    private int targetLightness;
    private int targetHue;
    private int targetSaturation;
    private byte remainingTime;
    private boolean isComplete = false;

    /**
     * Default constructor for the HslTargetStatusMessage class.
     */
    public HslTargetStatusMessage() {
    }

    /**
     * Constructor for the HslTargetStatusMessage class that initializes the object from a Parcel.
     *
     * @param in The Parcel object to read the data from.
     */
    protected HslTargetStatusMessage(Parcel in) {
        targetLightness = in.readInt();
        targetHue = in.readInt();
        targetSaturation = in.readInt();
        remainingTime = in.readByte();
        isComplete = in.readByte() != 0;
    }

    /**
     * Creator constant for the Parcelable interface.
     */
    public static final Creator<HslTargetStatusMessage> CREATOR = new Creator<HslTargetStatusMessage>() {
        @Override
        public HslTargetStatusMessage createFromParcel(Parcel in) {
            return new HslTargetStatusMessage(in);
        }

        @Override
        public HslTargetStatusMessage[] newArray(int size) {
            return new HslTargetStatusMessage[size];
        }
    };

    /**
     * Parses the byte array parameters and sets the target information and completion flag.
     *
     * @param params The byte array parameters to parse.
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.targetLightness = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        this.targetHue = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        this.targetSaturation = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        if (params.length >= DATA_LEN_COMPLETE) {
            this.isComplete = true;
            this.remainingTime = params[index];
        }
    }

    /**
     * Returns a bitmask indicating the set of special object types marshaled by this Parcelable object instance.
     *
     * @return The bitmask value.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the target information and completion flag to the Parcel object.
     *
     * @param dest  The Parcel object to write the data to.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(targetLightness);
        dest.writeInt(targetHue);
        dest.writeInt(targetSaturation);
        dest.writeByte(remainingTime);
        dest.writeByte((byte) (isComplete ? 1 : 0));
    }

    /**
     * Returns the target lightness value.
     *
     * @return The target lightness.
     */
    public int getTargetLightness() {
        return targetLightness;
    }

    /**
     * Returns the target hue value.
     *
     * @return The target hue.
     */
    public int getTargetHue() {
        return targetHue;
    }

    /**
     * Returns the target saturation value.
     *
     * @return The target saturation.
     */
    public int getTargetSaturation() {
        return targetSaturation;
    }

    /**
     * Returns the remaining time value.
     *
     * @return The remaining time.
     */
    public byte getRemainingTime() {
        return remainingTime;
    }

    /**
     * Returns a flag indicating if the message is complete.
     *
     * @return True if the message is complete, false otherwise.
     */
    public boolean isComplete() {
        return isComplete;
    }
}