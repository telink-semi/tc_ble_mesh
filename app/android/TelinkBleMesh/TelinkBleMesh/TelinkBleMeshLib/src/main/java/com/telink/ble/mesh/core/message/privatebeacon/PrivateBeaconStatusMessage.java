/********************************************************************************************************
 * @file PrivateBeaconStatusMessage.java
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
package com.telink.ble.mesh.core.message.privatebeacon;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * A PRIVATE_BEACON_STATUS message is an unacknowledged message used to report the current Private Beacon state and Random Update Interval Steps state of a node.
 * This message is a response to PRIVATE_BEACON_GET message or a PRIVATE_BEACON_SET message.
 * The PrivateBeaconStatusMessage class represents an unacknowledged message used to report the current state of a Private Beacon and Random Update Interval Steps of a node. This message is typically a response to a PRIVATE_BEACON_GET or PRIVATE_BEACON_SET message.
 */
public class PrivateBeaconStatusMessage extends StatusMessage implements Parcelable {
    /**
     * New Private Beacon state.
     * This field is a 1-byte value.
     */
    public byte privateBeacon;
    /**
     * New Random Update Interval Steps state (optional).
     * This field is a 1-byte value.
     */
    public byte randomUpdateIntervalSteps;
    public boolean isComplete = false;

    /**
     * Default constructor for the PrivateBeaconStatusMessage class.
     */
    public PrivateBeaconStatusMessage() {
    }

    /**
     * Constructor for the PrivateBeaconStatusMessage class.
     *
     * @param in The Parcel object used for deserialization.
     */
    protected PrivateBeaconStatusMessage(Parcel in) {
        privateBeacon = in.readByte();
        randomUpdateIntervalSteps = in.readByte();
        isComplete = in.readByte() != 0;
    }

    /**
     * A Creator object that implements the Parcelable interface for the PrivateBeaconStatusMessage class.
     */
    public static final Creator<PrivateBeaconStatusMessage> CREATOR = new Creator<PrivateBeaconStatusMessage>() {
        @Override
        public PrivateBeaconStatusMessage createFromParcel(Parcel in) {
            return new PrivateBeaconStatusMessage(in);
        }

        @Override
        public PrivateBeaconStatusMessage[] newArray(int size) {
            return new PrivateBeaconStatusMessage[size];
        }
    };

    /**
     * Parses the byte array and sets the values of the PrivateBeaconStatusMessage object accordingly.
     *
     * @param params The byte array to be parsed.
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        privateBeacon = params[index++];
        if (params.length >= 2) {
            isComplete = true;
            randomUpdateIntervalSteps = params[index];
        }
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
     * Flattens the object into a Parcel.
     *
     * @param dest  The Parcel object to write the flattened object to.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(privateBeacon);
        dest.writeByte(randomUpdateIntervalSteps);
        dest.writeByte((byte) (isComplete ? 1 : 0));
    }
}