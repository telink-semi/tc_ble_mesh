/********************************************************************************************************
 * @file HslStatusMessage.java
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
 * This class represents a HSL status message.
 * It extends the StatusMessage class and implements the Parcelable interface.
 * The HSL status message contains information about the lightness, hue, saturation, and remaining time.
 * It also has a flag to indicate if the message is complete or not.
 */
public class HslStatusMessage extends StatusMessage implements Parcelable {
    private static final int DATA_LEN_COMPLETE = 7;
    private int lightness;
    private int hue;
    private int saturation;
    private byte remainingTime;
    private boolean isComplete = false;

    /**
     * Default constructor for the HslStatusMessage class.
     */
    public HslStatusMessage() {
    }

    /**
     * Constructor for the HslStatusMessage class that takes a Parcel as input.
     * It reads the values from the Parcel and assigns them to the corresponding variables.
     *
     * @param in The Parcel containing the values for the HslStatusMessage.
     */
    protected HslStatusMessage(Parcel in) {
        lightness = in.readInt();
        hue = in.readInt();
        saturation = in.readInt();
        remainingTime = in.readByte();
        isComplete = in.readByte() != 0;
    }

    /**
     * Method to write the values of the HslStatusMessage to a Parcel.
     *
     * @param dest  The Parcel to write the values to.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(lightness);
        dest.writeInt(hue);
        dest.writeInt(saturation);
        dest.writeByte(remainingTime);
        dest.writeByte((byte) (isComplete ? 1 : 0));
    }

    /**
     * Method to describe the contents of the HslStatusMessage.
     *
     * @return An integer value representing the contents of the HslStatusMessage.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * A Creator object that implements the Parcelable.Creator interface.
     * It is used to create new instances of the HslStatusMessage class from a Parcel.
     */
    public static final Creator<HslStatusMessage> CREATOR = new Creator<HslStatusMessage>() {
        @Override
        public HslStatusMessage createFromParcel(Parcel in) {
            return new HslStatusMessage(in);
        }

        @Override
        public HslStatusMessage[] newArray(int size) {
            return new HslStatusMessage[size];
        }
    };

    /**
     * Method to parse the byte array and set the values of the HslStatusMessage.
     *
     * @param params The byte array containing the values to be parsed.
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.lightness = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        this.hue = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        this.saturation = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        if (params.length >= DATA_LEN_COMPLETE) {
            this.isComplete = true;
            this.remainingTime = params[index];
        }
    }

    /**
     * Method to get the lightness value of the HslStatusMessage.
     *
     * @return An integer representing the lightness value.
     */
    public int getLightness() {
        return lightness;
    }

    /**
     * Method to get the hue value of the HslStatusMessage.
     *
     * @return An integer representing the hue value.
     */
    public int getHue() {
        return hue;
    }

    /**
     * Method to get the saturation value of the HslStatusMessage.
     *
     * @return An integer representing the saturation value.
     */
    public int getSaturation() {
        return saturation;
    }

    /**
     * Method to get the remaining time value of the HslStatusMessage.
     *
     * @return A byte representing the remaining time value.
     */
    public byte getRemainingTime() {
        return remainingTime;
    }

    /**
     * Method to check if the HslStatusMessage is complete or not.
     *
     * @return A boolean value indicating if the message is complete or not.
     */
    public boolean isComplete() {
        return isComplete;
    }
}