/********************************************************************************************************
 * @file CompositionDataStatusMessage.java
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
package com.telink.ble.mesh.core.message.solicitation;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.StatusMessage;

/**
 * This class represents a message containing the status of solicitation items.
 * It extends the StatusMessage class and implements the Parcelable interface for easy serialization.
 */
public class SolicitationItemsStatusMessage extends StatusMessage implements Parcelable {

    /**
     * The page number of the composition data.
     */
    public byte privateProxy;

    /**
     * Default constructor for the SolicitationItemsStatusMessage class.
     */
    public SolicitationItemsStatusMessage() {
    }

    /**
     * Constructor for the SolicitationItemsStatusMessage class that takes a Parcel as input.
     * Used for deserialization.
     *
     * @param in The Parcel containing the data to be read.
     */
    protected SolicitationItemsStatusMessage(Parcel in) {
        privateProxy = in.readByte();
    }

    /**
     * Creator constant for the Parcelable interface.
     */
    public static final Creator<SolicitationItemsStatusMessage> CREATOR = new Creator<SolicitationItemsStatusMessage>() {
        @Override
        public SolicitationItemsStatusMessage createFromParcel(Parcel in) {
            return new SolicitationItemsStatusMessage(in);
        }

        @Override
        public SolicitationItemsStatusMessage[] newArray(int size) {
            return new SolicitationItemsStatusMessage[size];
        }
    };

    /**
     * Method to parse the byte array and set the privateProxy field.
     *
     * @param params The byte array to be parsed.
     */
    @Override
    public void parse(byte[] params) {
        privateProxy = params[0];
    }

    /**
     * Method to describe the contents of the Parcelable object.
     *
     * @return Always returns 0.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Method to write the object's data to a Parcel.
     *
     * @param dest  The Parcel to write the data to.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(privateProxy);
    }
}