/********************************************************************************************************
 * @file LcPropertyStatusMessage.java
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
 * This class represents a status message for the light lc mode of a device.
 * It extends the StatusMessage class and implements the Parcelable interface.
 */
public class LcPropertyStatusMessage extends StatusMessage implements Parcelable {

    private int propertyID;

    private byte[] propertyValue;

    /**
     * Default constructor for the LightnessStatusMessage class.
     */
    public LcPropertyStatusMessage() {
    }

    /**
     * Constructor for the LightnessStatusMessage class that takes a Parcel as input.
     *
     * @param in The Parcel object to read the values from.
     */
    protected LcPropertyStatusMessage(Parcel in) {
        propertyID = in.readInt();
        in.readByteArray(propertyValue);
    }

    /**
     * Creator object for the LightnessStatusMessage class, used to create new instances from a Parcel.
     */
    public static final Creator<LcPropertyStatusMessage> CREATOR = new Creator<LcPropertyStatusMessage>() {
        @Override
        public LcPropertyStatusMessage createFromParcel(Parcel in) {
            return new LcPropertyStatusMessage(in);
        }

        @Override
        public LcPropertyStatusMessage[] newArray(int size) {
            return new LcPropertyStatusMessage[size];
        }
    };

    /**
     * Parses the byte array parameters and sets the values accordingly.
     *
     * @param params The byte array containing the parameters.
     */
    @Override
    public void parse(byte[] params) {
        this.propertyID = MeshUtils.bytes2Integer(params, 0, 2, ByteOrder.LITTLE_ENDIAN);
        this.propertyValue = new byte[params.length - 2];
        System.arraycopy(params, 2, this.propertyValue, 0, params.length - 2);
    }

    public int getPropertyID() {
        return propertyID;
    }

    public byte[] getPropertyValue() {
        return propertyValue;
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
        dest.writeInt(this.propertyID);
        dest.writeByteArray(propertyValue);
    }
}