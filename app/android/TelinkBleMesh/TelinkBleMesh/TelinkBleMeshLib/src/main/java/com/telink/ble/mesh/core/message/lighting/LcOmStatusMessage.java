/********************************************************************************************************
 * @file LcOmStatusMessage.java
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

import com.telink.ble.mesh.core.message.StatusMessage;

/**
 * This class represents a status message for the light lc mode of a device.
 * It extends the StatusMessage class and implements the Parcelable interface.
 */
public class LcOmStatusMessage extends StatusMessage implements Parcelable {

    private byte mode;

    /**
     * Default constructor for the LightnessStatusMessage class.
     */
    public LcOmStatusMessage() {
    }

    /**
     * Constructor for the LightnessStatusMessage class that takes a Parcel as input.
     *
     * @param in The Parcel object to read the values from.
     */
    protected LcOmStatusMessage(Parcel in) {
        mode = in.readByte();
    }

    /**
     * Creator object for the LightnessStatusMessage class, used to create new instances from a Parcel.
     */
    public static final Creator<LcOmStatusMessage> CREATOR = new Creator<LcOmStatusMessage>() {
        @Override
        public LcOmStatusMessage createFromParcel(Parcel in) {
            return new LcOmStatusMessage(in);
        }

        @Override
        public LcOmStatusMessage[] newArray(int size) {
            return new LcOmStatusMessage[size];
        }
    };

    /**
     * Parses the byte array parameters and sets the values accordingly.
     *
     * @param params The byte array containing the parameters.
     */
    @Override
    public void parse(byte[] params) {
        this.mode = params[0];
    }

    /**
     * Returns the mode value.
     *
     * @return The mode value.
     */
    public byte getMode() {
        return mode;
    }

    /**
     * Returns the content description of the Parcelable object.
     *
     * @return The content description of the Parcelable object.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the values of the Parcelable object to a Parcel.
     *
     * @param dest  The Parcel object to write the values to.
     * @param flags Additional flags for writing the Parcel.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(mode);
    }
}