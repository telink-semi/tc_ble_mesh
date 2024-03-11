/********************************************************************************************************
 * @file LinkStatusMessage.java
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
package com.telink.ble.mesh.core.message.rp;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.StatusMessage;

/**
 * This class represents a Link Status Message.
 * It extends the StatusMessage class and implements the Parcelable interface.
 * The Link Status Message contains information about the status and state of a link.
 */
public class LinkStatusMessage extends StatusMessage implements Parcelable {
    private byte status; // The status of the link
    private byte rpState; // The state of the link's RP (Routing Protocol)

    /**
     * Default constructor for the LinkStatusMessage class.
     */
    public LinkStatusMessage() {
    }

    /**
     * Constructor for the LinkStatusMessage class that takes a Parcel as input.
     * It reads the status and rpState from the Parcel.
     *
     * @param in The Parcel to read from.
     */
    protected LinkStatusMessage(Parcel in) {
        status = in.readByte();
        rpState = in.readByte();
    }

    /**
     * Creator constant for the LinkStatusMessage class.
     * It is used to create new instances of the LinkStatusMessage class from a Parcel.
     */
    public static final Creator<LinkStatusMessage> CREATOR = new Creator<LinkStatusMessage>() {
        @Override
        public LinkStatusMessage createFromParcel(Parcel in) {
            return new LinkStatusMessage(in);
        }

        @Override
        public LinkStatusMessage[] newArray(int size) {
            return new LinkStatusMessage[size];
        }
    };

    /**
     * Parses the byte array and sets the status and rpState of the LinkStatusMessage.
     *
     * @param params The byte array to parse.
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.status = params[index++];
        this.rpState = params[index++];
    }

    /**
     * Returns a bitmask indicating the set of special object types marshaled by this Parcelable object instance.
     *
     * @return Always returns 0.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the status and rpState to the Parcel.
     *
     * @param dest The Parcel to write to.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(status);
        dest.writeByte(rpState);
    }

    /**
     * Returns the state of the link's RP (Routing Protocol).
     *
     * @return The state of the link's RP.
     */
    public byte getRpState() {
        return rpState;
    }

    /**
     * Returns the status of the link.
     *
     * @return The status of the link.
     */
    public byte getStatus() {
        return status;
    }

    /**
     * Returns a string representation of the LinkStatusMessage object.
     *
     * @return A string representation of the LinkStatusMessage object.
     */
    @Override
    public String toString() {
        return "LinkStatusMessage{" +
                "status=" + status +
                ", rpState=" + rpState +
                '}';
    }
}
