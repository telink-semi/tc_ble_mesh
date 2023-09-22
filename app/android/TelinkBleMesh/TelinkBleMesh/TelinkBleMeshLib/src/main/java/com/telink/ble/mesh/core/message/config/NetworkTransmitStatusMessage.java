/********************************************************************************************************
 * @file NetworkTransmitStatusMessage.java
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

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.StatusMessage;

/**
 * The Config Network Transmit Status is an unacknowledged message used to report the current Network Transmit state of a node
 * The NetworkTransmitStatusMessage class represents an unacknowledged message used to report the current Network Transmit state of a node.
 * It extends the StatusMessage class and implements the Parcelable interface.
 * <p>
 * The count variable represents the number of transmissions attempted for each Network PDU.
 * The intervalSteps variable represents the interval between each transmission in steps of 10 milliseconds.
 * <p>
 * This message is used to provide information about the current Network Transmit state of a node.
 *
 * @see com.telink.ble.mesh.core.message.Opcode#CFG_NW_TRANSMIT_STATUS
 */
public class NetworkTransmitStatusMessage extends StatusMessage implements Parcelable {
    public int count;
    public int intervalSteps;

    /**
     * Default constructor for the NetworkTransmitStatusMessage class.
     */
    public NetworkTransmitStatusMessage() {
    }

    /**
     * Constructor for the NetworkTransmitStatusMessage class that initializes the count and intervalSteps variables.
     *
     * @param in The Parcel object containing the count and intervalSteps values.
     */
    protected NetworkTransmitStatusMessage(Parcel in) {
        count = in.readInt();
        intervalSteps = in.readInt();
    }

    /**
     * Creator constant for the NetworkTransmitStatusMessage class that implements the Parcelable.Creator interface.
     */
    public static final Creator<NetworkTransmitStatusMessage> CREATOR = new Creator<NetworkTransmitStatusMessage>() {
        @Override
        public NetworkTransmitStatusMessage createFromParcel(Parcel in) {
            return new NetworkTransmitStatusMessage(in);
        }

        @Override
        public NetworkTransmitStatusMessage[] newArray(int size) {
            return new NetworkTransmitStatusMessage[size];
        }
    };

    /**
     * Parses the byte array data and sets the count and intervalSteps variables.
     *
     * @param data The byte array data to be parsed.
     */
    @Override
    public void parse(byte[] data) {
        this.count = data[0] & 0b111;
        this.intervalSteps = (data[0] & 0xFF) >> 3;
    }

    /**
     * Returns the count value.
     *
     * @return The count value.
     */
    public int getCount() {
        return count;
    }

    /**
     * Returns the intervalSteps value.
     *
     * @return The intervalSteps value.
     */
    public int getIntervalSteps() {
        return intervalSteps;
    }

    /**
     * Returns a bitmask indicating the set of special object types marshaled by this Parcelable object instance.
     *
     * @return A bitmask indicating the set of special object types marshaled by this Parcelable object instance.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Flatten this object in to a Parcel.
     *
     * @param dest  The Parcel object to write the data to.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(count);
        dest.writeInt(intervalSteps);
    }
}

