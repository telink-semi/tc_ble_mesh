/********************************************************************************************************
 * @file PrivateGattProxyStatusMessage.java
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

import com.telink.ble.mesh.core.message.StatusMessage;

/**
 * A PRIVATE_GATT_PROXY_STATUS message is an unacknowledged message used to
 * report the current Private GATT Proxy state of a node
 * The PrivateGattProxyStatusMessage class represents an unacknowledged message used to report the current state of the Private GATT Proxy of a node.
 * It extends the StatusMessage class and implements the Parcelable interface.
 */
public class PrivateGattProxyStatusMessage extends StatusMessage implements Parcelable {

    /**
     * The new private GATT Proxy state.
     * It is represented by a single byte.
     */
    public byte privateGattProxy;

    /**
     * Default constructor for the PrivateGattProxyStatusMessage class.
     */
    public PrivateGattProxyStatusMessage() {
    }

    /**
     * Constructor for the PrivateGattProxyStatusMessage class.
     * It initializes the privateGattProxy field by reading from the given Parcel.
     *
     * @param in The Parcel object to read the privateGattProxy field from.
     */
    protected PrivateGattProxyStatusMessage(Parcel in) {
        privateGattProxy = in.readByte();
    }

    /**
     * Creator constant for the PrivateGattProxyStatusMessage class.
     * It is used to create new instances of the class from a Parcel.
     */
    public static final Creator<PrivateGattProxyStatusMessage> CREATOR = new Creator<PrivateGattProxyStatusMessage>() {
        @Override
        public PrivateGattProxyStatusMessage createFromParcel(Parcel in) {
            return new PrivateGattProxyStatusMessage(in);
        }

        @Override
        public PrivateGattProxyStatusMessage[] newArray(int size) {
            return new PrivateGattProxyStatusMessage[size];
        }
    };

    /**
     * Parses the given byte array and sets the privateGattProxy field based on the first element of the array.
     *
     * @param params The byte array to parse.
     */
    @Override
    public void parse(byte[] params) {
        privateGattProxy = params[0];
    }

    /**
     * Returns a bitmask indicating the set of special object types marshaled by this Parcelable object instance.
     * As the PrivateGattProxyStatusMessage class does not have any special object types, it returns 0.
     *
     * @return The bitmask indicating the set of special object types marshaled by this Parcelable object instance.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the privateGattProxy field to the given Parcel object.
     *
     * @param dest  The Parcel object to write the privateGattProxy field to.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(privateGattProxy);
    }
}