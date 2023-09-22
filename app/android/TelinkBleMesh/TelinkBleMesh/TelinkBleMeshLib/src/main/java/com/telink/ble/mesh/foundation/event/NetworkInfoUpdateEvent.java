/********************************************************************************************************
 * @file NetworkInfoUpdateEvent.java
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
package com.telink.ble.mesh.foundation.event;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.foundation.Event;

/**
 * This class represents a network information update event.
 * It extends the Event class and implements the Parcelable interface.
 * The event is triggered when there is an update in the network information.
 * The event type is defined as "com.telink.ble.mesh.EVENT_TYPE_NETWORKD_INFO_UPDATE".
 * The sequence number and ivIndex are used to provide additional information about the update.
 */
public class NetworkInfoUpdateEvent extends Event<String> implements Parcelable {
    public static final String EVENT_TYPE_NETWORKD_INFO_UPDATE = "com.telink.ble.mesh.EVENT_TYPE_NETWORKD_INFO_UPDATE";

    private int sequenceNumber; // The sequence number of the update
    private int ivIndex; // The ivIndex of the update

    /**
     * Constructs a new NetworkInfoUpdateEvent with the specified sender, type, sequence number, and ivIndex.
     *
     * @param sender         the object that triggered the event
     * @param type           the type of the event
     * @param sequenceNumber the sequence number of the update
     * @param ivIndex        the ivIndex of the update
     */
    public NetworkInfoUpdateEvent(Object sender, String type, int sequenceNumber, int ivIndex) {
        super(sender, type);
        this.sequenceNumber = sequenceNumber;
        this.ivIndex = ivIndex;
    }

    /**
     * Constructs a new NetworkInfoUpdateEvent by reading from a Parcel.
     *
     * @param in the Parcel to read from
     */
    protected NetworkInfoUpdateEvent(Parcel in) {
        sequenceNumber = in.readInt();
        ivIndex = in.readInt();
    }

    /**
     * Creator for the NetworkInfoUpdateEvent class.
     */
    public static final Creator<NetworkInfoUpdateEvent> CREATOR = new Creator<NetworkInfoUpdateEvent>() {
        @Override
        public NetworkInfoUpdateEvent createFromParcel(Parcel in) {
            return new NetworkInfoUpdateEvent(in);
        }

        @Override
        public NetworkInfoUpdateEvent[] newArray(int size) {
            return new NetworkInfoUpdateEvent[size];
        }
    };

    /**
     * Returns the sequence number of the update.
     *
     * @return the sequence number of the update
     */
    public int getSequenceNumber() {
        return sequenceNumber;
    }

    /**
     * Returns the ivIndex of the update.
     *
     * @return the ivIndex of the update
     */
    public int getIvIndex() {
        return ivIndex;
    }

    /**
     * Describes the contents of the Parcelable object.
     *
     * @return a bitmask indicating the set of special object types marshaled by the Parcelable.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the NetworkInfoUpdateEvent object to a Parcel.
     *
     * @param dest  the Parcel to write to
     * @param flags additional flags about how the object should be written
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(sequenceNumber);
        dest.writeInt(ivIndex);
    }
}