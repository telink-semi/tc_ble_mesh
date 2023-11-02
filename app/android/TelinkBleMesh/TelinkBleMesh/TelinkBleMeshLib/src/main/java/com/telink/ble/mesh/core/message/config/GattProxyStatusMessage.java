/********************************************************************************************************
 * @file GattProxyStatusMessage.java
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
 * The Config GATT Proxy Status is an unacknowledged message used to report the current GATT Proxy state of a node
 * <p>
 * The GattProxyStatusMessage class represents an unacknowledged message used to report the current GATT Proxy state of a node.
 * It is a response to the GattProxyGetMessage and GattProxySetMessage.
 * <p>
 * This message is identified by the Opcode CFG_GATT_PROXY_STATUS.
 * <p>
 * The GattProxyStatusMessage class implements the Parcelable interface to allow for easy serialization and deserialization of the message.
 * <p>
 * The gattProxy field represents the current GATT Proxy state of the node.
 *
 * @see com.telink.ble.mesh.core.message.Opcode#CFG_GATT_PROXY_STATUS
 */
public class GattProxyStatusMessage extends StatusMessage implements Parcelable {

    public byte gattProxy;

    /**
     * Default constructor for the GattProxyStatusMessage class.
     */
    public GattProxyStatusMessage() {
    }

    /**
     * Constructor for the GattProxyStatusMessage class that takes a Parcel as input.
     * Used for deserialization of the message.
     *
     * @param in The Parcel containing the serialized GattProxyStatusMessage object.
     */
    protected GattProxyStatusMessage(Parcel in) {
        gattProxy = in.readByte();
    }

    /**
     * Creator constant for the GattProxyStatusMessage class that implements the Parcelable.Creator interface.
     */
    public static final Creator<GattProxyStatusMessage> CREATOR = new Creator<GattProxyStatusMessage>() {
        @Override
        public GattProxyStatusMessage createFromParcel(Parcel in) {
            return new GattProxyStatusMessage(in);
        }

        @Override
        public GattProxyStatusMessage[] newArray(int size) {
            return new GattProxyStatusMessage[size];
        }
    };

    /**
     * Method to parse the received byte array and populate the fields of the GattProxyStatusMessage object.
     *
     * @param params The byte array containing the parameters of the message.
     */
    @Override
    public void parse(byte[] params) {
        gattProxy = params[0];
    }

    /**
     * Method to describe the contents of the GattProxyStatusMessage object.
     *
     * @return An integer value representing the contents description.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Method to serialize the GattProxyStatusMessage object into a Parcel.
     *
     * @param dest  The Parcel in which the object should be written.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(gattProxy);
    }
}